#include "engine/signal_textures.h"
#include "engine/native_string.h"
#include "engine/detail/texture_name_hash.h"
#include <array>
#include <algorithm>
#include <cstring>
#include <unordered_set>
namespace nimby::engine {
namespace {
template<class T>T field(const void* p,size_t off){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof v);return v;}
bool ptr(uint64_t p){return p>=0x10000&&p<0x7fffffff0000ULL&&p%8==0;}
bool stable(ReadMemory r,void* c,uint64_t p,const void* bytes,size_t n){
    std::vector<unsigned char> after(n);return !n||(r(c,p,after.data(),n)&&std::memcmp(bytes,after.data(),n)==0);
}
template<class Char>bool string(ReadMemory r,void* c,const void* data,size_t limit,std::basic_string<Char>& out){
    const auto length=field<uint64_t>(data,16),capacity=field<uint64_t>(data,24);
    constexpr size_t inline_capacity=16/sizeof(Char)-1;
    if(length>limit||capacity<inline_capacity||capacity<length||capacity>1048576)return false;
    std::vector<Char> chars(length+1);
    if(capacity==inline_capacity)std::memcpy(chars.data(),data,(length+1)*sizeof(Char));
    else {const auto p=field<uint64_t>(data,0);if(!ptr(p)||!r(c,p,chars.data(),chars.size()*sizeof(Char))||!stable(r,c,p,chars.data(),chars.size()*sizeof(Char)))return false;}
    if(chars.back()!=0)return false;
    for(size_t i=0;i<length;++i)if(chars[i]==0)return false;
    out.assign(chars.data(),length);return true;
}
bool text(ReadMemory r,void* c,const LiveState& s,uint64_t address,const void* data,size_t limit,std::string& value){
    return gameLayout(s.profile).local_path_utf8?read_native_string(r,c,address,data,s.profile,value,limit):string(r,c,data,limit,value);
}
bool selected_set(ReadMemory r,void* c,const LiveState& s,uint64_t node,
                  const std::array<unsigned char,0xa0>& data,std::string_view expected,
                  size_t& budget,SignalTextureSet& set){
    set.hash=field<uint64_t>(data.data(),0);
    if(!text(r,c,s,node+8,data.data()+8,127,set.name)||(!expected.empty()&&set.name!=expected))return false;
    const auto begin=field<uint64_t>(data.data(),0x80),end=field<uint64_t>(data.data(),0x88),cap=field<uint64_t>(data.data(),0x90);
    if(end<begin||cap<end||(end-begin)%0x50||(cap-begin)%0x50||cap-begin>4096*0x50||
       (end-begin)/0x50>budget||(end!=begin&&!ptr(begin)))return false;
    std::vector<unsigned char> files(end-begin);budget-=files.size()/0x50;
    if(!files.empty()&&!r(c,begin,files.data(),files.size()))return false;
    set.files.reserve(files.size()/0x50);
    for(size_t i=0;i<files.size();i+=0x50){const auto* f=files.data()+i;SignalTextureFile file;
        file.source=field<int32_t>(f,0);file.hash=field<uint64_t>(f,0x48);
        if(file.source<0||file.source>2||!text(r,c,s,begin+i+8,f+8,255,file.mod)||
           !text(r,c,s,begin+i+0x28,f+0x28,511,file.relative_path))return false;
        set.files.push_back(std::move(file));
    }
    return stable(r,c,begin,files.data(),files.size())&&stable(r,c,node,data.data(),data.size());
}
bool targeted(ReadMemory r,void* c,const LiveState& s,const std::map<uint64_t,std::string_view>& requested,
              bool required,SignalTextureCatalog& out,std::unordered_set<uint64_t>* absent=nullptr){
    const auto table=s.database+gameLayout(s.profile).rules+0x138;
    std::array<uint64_t,4> header{};
    if(!r(c,table,header.data(),sizeof header))return false;
    const auto buckets=header[1],count=header[2],size=header[3];
    if(!ptr(buckets)||!count||count>65536||size>4096)return false;
    uint64_t sentinel{};
    if(!r(c,buckets+count*8,&sentinel,8))return false;
    std::map<uint64_t,std::map<uint64_t,std::string_view>> groups;
    for(const auto& [hash,name]:requested)groups[hash%count].emplace(hash,name);
    size_t fileBudget=65536,totalNodes=0;
    for(const auto& [bucket,targets]:groups){
        uint64_t head{};bool valid=r(c,buckets+bucket*8,&head,8);
        struct Link{uint64_t address,key,next;};std::vector<Link> links;
        std::unordered_set<uint64_t> seen,matched;
        std::map<uint64_t,SignalTextureSet> decoded;
        for(auto node=head;valid&&node;){
            if(!ptr(node)||node==sentinel||!seen.insert(node).second||++totalNodes>size){valid=false;break;}
            std::array<unsigned char,0xa0> data{};
            if(!r(c,node,data.data(),data.size())){valid=false;break;}
            const auto key=field<uint64_t>(data.data(),0),next=field<uint64_t>(data.data(),0x98);
            if(key%count!=bucket){valid=false;break;}
            links.push_back({node,key,next});
            if(const auto wanted=targets.find(key);wanted!=targets.end()){
                if(!matched.insert(key).second){valid=false;break;}
                SignalTextureSet set;
                if(selected_set(r,c,s,node,data,wanted->second,fileBudget,set))decoded.emplace(key,std::move(set));
                else if(required)return false;
            }
            node=next;
        }
        // Other buckets are not dependencies. A corrupt chain in a requested
        // bucket is unavoidable: it can hide the requested key or a duplicate.
        // For traversed neighbours compare only the key/link, never their
        // unrelated names, file vectors, paths or changing resource contents.
        for(const auto& link:links)if(valid){
            std::array<unsigned char,0xa0> after{};
            valid=r(c,link.address,after.data(),after.size())&&field<uint64_t>(after.data(),0)==link.key&&
                field<uint64_t>(after.data(),0x98)==link.next;
        }
        uint64_t afterHead{};
        valid=valid&&r(c,buckets+bucket*8,&afterHead,8)&&afterHead==head;
        if(!valid){if(required)return false;continue;}
        if(required&&decoded.size()!=targets.size())return false;
        if(absent)for(const auto& [hash,name]:targets)if(!matched.contains(hash))absent->insert(hash);
        out.sets.merge(decoded);
    }
    uint64_t afterSentinel{};
    return stable(r,c,table,header.data(),sizeof header)&&r(c,buckets+count*8,&afterSentinel,8)&&afterSentinel==sentinel&&
        stable(r,c,table,header.data(),sizeof header);
}
bool selected_defaults(ReadMemory r,void* c,const LiveState& s,std::vector<uint64_t>& out){
    const auto address=s.database+gameLayout(s.profile).rules+0x540;
    std::array<uint64_t,3> header{};
    if(!r(c,address,header.data(),sizeof header))return false;
    const auto begin=header[0],end=header[1],cap=header[2];
    if(end<begin||cap<end||(end-begin)%8||cap-begin>256*8||(end!=begin&&!ptr(begin)))return false;
    std::vector<uint64_t> value((end-begin)/8);
    if((!value.empty()&&!r(c,begin,value.data(),value.size()*8))||
       !stable(r,c,begin,value.data(),value.size()*8)||!stable(r,c,address,header.data(),sizeof header))return false;
    out=std::move(value);return true;
}
bool capture(ReadMemory r,void* c,const LiveState& s,SignalTextureCatalog& out){
    const auto text = [&](uint64_t address,const void* data,size_t limit,std::string& value) {
        return gameLayout(s.profile).local_path_utf8
            ? read_native_string(r,c,address,data,s.profile,value,limit)
            : string(r,c,data,limit,value);
    };
    // Rules live within Database at +0xa80; SignalTextures map at Rules+0x138.
    const auto rules=s.database+gameLayout(s.profile).rules,table=rules+0x138;
    std::array<uint64_t,4> h{};std::array<uint64_t,3> defaults{};
    if(!r(c,table,h.data(),sizeof h)||!r(c,rules+0x540,defaults.data(),sizeof defaults))return false;
    const auto buckets=h[1],count=h[2],size=h[3];
    if(!ptr(buckets)||!count||count>65536||size>4096)return false;
    std::vector<uint64_t> heads(count+1);
    if(!r(c,buckets,heads.data(),heads.size()*8))return false;
    if(defaults[1]<defaults[0]||defaults[2]<defaults[1]||(defaults[1]-defaults[0])%8||
       defaults[2]-defaults[0]>256*8||((defaults[1]!=defaults[0])&&!ptr(defaults[0])))return false;
    out.defaults.resize((defaults[1]-defaults[0])/8);
    if(!out.defaults.empty()&&!r(c,defaults[0],out.defaults.data(),out.defaults.size()*8))return false;
    std::unordered_set<uint64_t> seen;size_t file_budget=65536;
    for(size_t bucket=0;bucket<count;++bucket)for(auto node=heads[bucket];node;){
        if(!ptr(node)||node==heads.back()||!seen.insert(node).second||seen.size()>size)return false;
        std::array<unsigned char,0xa0> data{};if(!r(c,node,data.data(),data.size()))return false;
        SignalTextureSet set;set.hash=field<uint64_t>(data.data(),0);
        if(set.hash%count!=bucket||!text(node+8,data.data()+8,127,set.name))return false;
        const auto begin=field<uint64_t>(data.data(),0x80),end=field<uint64_t>(data.data(),0x88),cap=field<uint64_t>(data.data(),0x90);
        if(end<begin||cap<end||(end-begin)%0x50||(cap-begin)%0x50||cap-begin>4096*0x50||
           (end-begin)/0x50>file_budget||(end!=begin&&!ptr(begin)))return false;
        std::vector<unsigned char> files(end-begin);file_budget-=files.size()/0x50;
        if(!files.empty()&&!r(c,begin,files.data(),files.size()))return false;
        for(size_t i=0;i<files.size();i+=0x50){const auto* f=files.data()+i;SignalTextureFile file;
            file.source=field<int32_t>(f,0);file.hash=field<uint64_t>(f,0x48);
            if(file.source<0||file.source>2||!text(begin+i+8,f+8,255,file.mod)||!text(begin+i+0x28,f+0x28,511,file.relative_path))return false;
            set.files.push_back(std::move(file));
        }
        if(!stable(r,c,begin,files.data(),files.size())||!stable(r,c,node,data.data(),data.size())||
           !out.sets.emplace(set.hash,std::move(set)).second)return false;
        node=field<uint64_t>(data.data(),0x98);
    }
    if(seen.size()!=size||!stable(r,c,table,h.data(),sizeof h)||!stable(r,c,buckets,heads.data(),heads.size()*8)||
       !stable(r,c,rules+0x540,defaults.data(),sizeof defaults)||!stable(r,c,defaults[0],out.defaults.data(),out.defaults.size()*8))return false;
    // Native local-mod directory, UTF-16 std::wstring. Optional independently.
    std::array<unsigned char,32> root{};
    const auto address=s.module_base+gameLayout(s.profile).local_mod_root;
    if(r(c,address,root.data(),root.size())) {
        if(gameLayout(s.profile).local_path_utf8) {
            std::string path;
            if(text(address,root.data(),1023,path))out.local_mod_root=std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(path.data()),path.size()));
        } else {
            std::wstring path;
            if(string(r,c,root.data(),1023,path)&&stable(r,c,address,root.data(),root.size()))out.local_mod_root=path;
        }
    }
    return true;
}
}
bool read_signal_texture_catalog(ReadMemory r,void* c,const LiveState& s,bool recognized,SignalTextureCatalog& out) noexcept {
    out={};if(!r||!recognized)return false;
    try {for(int attempt=0;attempt<3;++attempt){LiveState before{},after{};SignalTextureCatalog value;
        if(!resolve_live_state(r,c,s.module_base,true,s.profile,before)||before!=s)return false;
        if(capture(r,c,s,value)&&resolve_live_state(r,c,s.module_base,true,s.profile,after)&&after==s){out=std::move(value);return true;}
    }}catch(...){}return false;
}
bool read_signal_texture_sets(ReadMemory r,void* c,const LiveState& s,bool recognized,
                              std::span<const std::string_view> names,SignalTextureCatalog& out) noexcept {
    out={};if(!r||!recognized||names.empty()||names.size()>4096)return false;
    try{
        std::map<uint64_t,std::string_view> requested;
        for(const auto name:names){
            if(name.empty()||name.size()>127||name.find('\0')!=std::string_view::npos)return false;
            const auto [entry,inserted]=requested.emplace(detail::textureNameHash(name),name);
            if(!inserted&&entry->second!=name)return false;
        }
        if(s.profile!=LiveStateProfile::Windows119){
            SignalTextureCatalog full;if(!read_signal_texture_catalog(r,c,s,true,full))return false;
            SignalTextureCatalog value;
            for(const auto& [hash,name]:requested){
                const SignalTextureSet* match=nullptr;
                for(const auto& [key,set]:full.sets)if(set.name==name){if(match)return false;match=&set;}
                if(!match)return false;
                value.sets.emplace(match->hash,*match);
            }
            out=std::move(value);
            return true;
        }
        for(unsigned attempt=0;attempt<3;++attempt){
            LiveState before{},after{};SignalTextureCatalog value;
            if(!resolve_live_state(r,c,s.module_base,true,s.profile,before)||before!=s)return false;
            if(targeted(r,c,s,requested,true,value)&&resolve_live_state(r,c,s.module_base,true,s.profile,after)&&after==s){out=std::move(value);return true;}
        }
    }catch(...){}out={};return false;
}
bool read_signal_texture_references(ReadMemory r,void* c,const LiveState& s,bool recognized,
                                   std::span<const SignalTextureReference> references,SignalTextureCatalog& out) noexcept {
    out={};if(!r||!recognized||references.size()>65536)return false;
    try{
        if(s.profile!=LiveStateProfile::Windows119)return read_signal_texture_catalog(r,c,s,true,out);
        LiveState before{},after{};SignalTextureCatalog value;
        if(!resolve_live_state(r,c,s.module_base,true,s.profile,before)||before!=s)return false;
        std::map<uint64_t,std::string_view> requested;
        for(const auto& reference:references)requested.emplace(reference.hash,std::string_view{});
        if(requested.size()>4096)return false;
        std::unordered_set<uint64_t> absent;
        if(!requested.empty()&&!targeted(r,c,s,requested,false,value,&absent))return false;
        for(const auto& [hash,name]:requested)if(!value.sets.contains(hash)&&!absent.contains(hash))value.unavailable_sets.insert(hash);
        std::vector<uint64_t> defaults;
        const bool fallback=std::any_of(references.begin(),references.end(),[&](const auto& row){return row.kind>=0&&absent.contains(row.hash);});
        if(fallback&&selected_defaults(r,c,s,defaults)){
            std::map<uint64_t,std::string_view> missing;
            for(const auto& reference:references)if(absent.contains(reference.hash)&&reference.kind>=0&&static_cast<size_t>(reference.kind)<defaults.size())
                missing.emplace(defaults[reference.kind],std::string_view{});
            SignalTextureCatalog selected;
            if(!missing.empty()&&targeted(r,c,s,missing,false,selected))value.sets.merge(selected.sets);
            value.defaults=std::move(defaults);
        }
        if(!resolve_live_state(r,c,s.module_base,true,s.profile,after)||after!=s)return false;
        out=std::move(value);return true;
    }catch(...){}return false;
}
const SignalTextureSet* select_signal_textures(const SignalTextureCatalog& catalog,int kind,uint64_t hash) noexcept {
    auto found=catalog.sets.find(hash);if(found!=catalog.sets.end())return &found->second;
    if(catalog.unavailable_sets.contains(hash))return nullptr;
    if(kind<0||static_cast<size_t>(kind)>=catalog.defaults.size())return nullptr;
    found=catalog.sets.find(catalog.defaults[kind]);return found==catalog.sets.end()?nullptr:&found->second;
}
}
