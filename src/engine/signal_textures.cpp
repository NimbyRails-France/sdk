#include "engine/signal_textures.h"
#include <array>
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
bool capture(ReadMemory r,void* c,const LiveState& s,SignalTextureCatalog& out){
    // Rules live within Database at +0xa80; SignalTextures map at Rules+0x138.
    const auto rules=s.database+0xa80,table=rules+0x138;
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
        if(set.hash%count!=bucket||!string(r,c,data.data()+8,127,set.name))return false;
        const auto begin=field<uint64_t>(data.data(),0x80),end=field<uint64_t>(data.data(),0x88),cap=field<uint64_t>(data.data(),0x90);
        if(end<begin||cap<end||(end-begin)%0x50||(cap-begin)%0x50||cap-begin>4096*0x50||
           (end-begin)/0x50>file_budget||(end!=begin&&!ptr(begin)))return false;
        std::vector<unsigned char> files(end-begin);file_budget-=files.size()/0x50;
        if(!files.empty()&&!r(c,begin,files.data(),files.size()))return false;
        for(size_t i=0;i<files.size();i+=0x50){const auto* f=files.data()+i;SignalTextureFile file;
            file.source=field<int32_t>(f,0);file.hash=field<uint64_t>(f,0x48);
            if(file.source<0||file.source>2||!string(r,c,f+8,255,file.mod)||!string(r,c,f+0x28,511,file.relative_path))return false;
            set.files.push_back(std::move(file));
        }
        if(!stable(r,c,begin,files.data(),files.size())||!stable(r,c,node,data.data(),data.size())||
           !out.sets.emplace(set.hash,std::move(set)).second)return false;
        node=field<uint64_t>(data.data(),0x98);
    }
    if(seen.size()!=size||!stable(r,c,table,h.data(),sizeof h)||!stable(r,c,buckets,heads.data(),heads.size()*8)||
       !stable(r,c,rules+0x540,defaults.data(),sizeof defaults)||!stable(r,c,defaults[0],out.defaults.data(),out.defaults.size()*8))return false;
    // Native local-mod directory, UTF-16 std::wstring. Optional independently.
    std::array<unsigned char,32> root{};const auto address=s.module_base+0xb77d50;
    if(!r(c,address,root.data(),root.size())||!string(r,c,root.data(),1023,out.local_mod_root)||
       !stable(r,c,address,root.data(),root.size()))out.local_mod_root.clear();
    return true;
}
}
bool read_signal_texture_catalog(ReadMemory r,void* c,const LiveState& s,bool recognized,SignalTextureCatalog& out) noexcept {
    out={};if(!r||!recognized)return false;
    try {for(int attempt=0;attempt<3;++attempt){LiveState before{},after{};SignalTextureCatalog value;
        if(!resolve_live_state(r,c,s.module_base,true,before)||before!=s)return false;
        if(capture(r,c,s,value)&&resolve_live_state(r,c,s.module_base,true,after)&&after==s){out=std::move(value);return true;}
    }}catch(...){}return false;
}
const SignalTextureSet* select_signal_textures(const SignalTextureCatalog& catalog,int kind,uint64_t hash) noexcept {
    auto found=catalog.sets.find(hash);if(found!=catalog.sets.end())return &found->second;
    if(kind<0||static_cast<size_t>(kind)>=catalog.defaults.size())return nullptr;
    found=catalog.sets.find(catalog.defaults[kind]);return found==catalog.sets.end()?nullptr:&found->second;
}
}
