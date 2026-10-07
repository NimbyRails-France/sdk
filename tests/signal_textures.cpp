#include "engine/signal_textures.h"
#include "engine/detail/texture_name_hash.h"
#include <array>
#include <cstdio>
#include <cstring>
using namespace nimby::engine;
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;unsigned reads{},changing_reads{};uint64_t fail{},changing{};
    std::map<uint64_t,size_t> read_counts;
    template<class T>void put(uint64_t base,size_t off,T value){auto& data=regions[base];if(data.size()<off+sizeof value)data.resize(off+sizeof value);std::memcpy(data.data()+off,&value,sizeof value);}
    void string(uint64_t base,size_t off,const char* value){
        const auto n=std::strlen(value);put(base,off+16,uint64_t(n));put(base,off+24,uint64_t(15));
        std::memcpy(regions[base].data()+off,value,n+1);
    }
};
bool read(void* ctx,uint64_t address,void* out,size_t n){auto& m=*static_cast<Memory*>(ctx);++m.reads;
    ++m.read_counts[address];
    if(address==m.fail)return false;
    auto it=m.regions.upper_bound(address);if(it==m.regions.begin())return false;--it;
    if(address-it->first>it->second.size()||n>it->second.size()-(address-it->first))return false;
    std::memcpy(out,it->second.data()+address-it->first,n);
    if(address==m.changing&&++m.changing_reads%2==0)static_cast<unsigned char*>(out)[0]^=1;
    return true;
}
int main(){
    LiveState s{0x140000000,0x200000000,0x200010000,0x200020000,0x200030000};Memory m;
    m.put(s.module_base+0xb81998,0,s.root);m.put(s.root,0x540,s.database);m.put(s.root,0x5c0,s.copy);m.put(s.root,0x680,s.simulation);
    const auto rules=s.database+0xa80;const uint64_t table=0x300000000,node=0x300010000,files=0x300020000,defaults=0x300030000;
    m.put(rules,0x140,table);m.put(rules,0x148,uint64_t(1));m.put(rules,0x150,uint64_t(1));
    m.put(table,0,node);m.put(table,8,uint64_t(0));m.regions[node].resize(0xa0);
    m.put(node,0,uint64_t(42));m.string(node,8,"test_atlas");m.put(node,0x80,files);m.put(node,0x88,files+0xa0);m.put(node,0x90,files+0xa0);
    m.regions[files].resize(0xa0);
    for(size_t off:{size_t(0),size_t(0x50)}){m.put(files,off,int32_t(1));m.string(files,off+8,"test_mod");m.string(files,off+0x28,off?"stop.svg":"pass.svg");m.put(files,off+0x48,uint64_t(100+off));}
    m.put(rules,0x540,defaults);m.put(rules,0x548,defaults+40);m.put(rules,0x550,defaults+40);m.regions[defaults].resize(40);m.put(defaults,32,uint64_t(42));
    SignalTextureCatalog catalog;
    CHECK(read_signal_texture_catalog(read,&m,s,true,catalog)&&catalog.sets.size()==1);
    const auto* selected=select_signal_textures(catalog,4,99);CHECK(selected&&selected->hash==42&&selected->files[1].relative_path=="stop.svg");
    CHECK(select_signal_textures(catalog,-1,42)==selected&&select_signal_textures(catalog,-1,99)==nullptr);
    CHECK(select_signal_textures(catalog,99,99)==nullptr);
    {
        // Vectors are independently verified against the qualified executable's
        // pure hash function, including both block/tail boundaries.
        CHECK(detail::textureNameHash("sfr_bal_a_v1")==0x9a2356c6eddee8f5ULL);
        const std::pair<size_t,uint64_t> vectors[]={{0,0xcb9061bba5736cfcULL},{1,0x7939ad3abe0ec791ULL},
            {3,0x58e7cbf43a202877ULL},{8,0xadc7c33c34a6a944ULL},{9,0xbe4844626c1fe321ULL},
            {16,0x1fc6a1c280e1d7f0ULL},{17,0x100ebdd1450a33daULL},{64,0xec35b8d048817b37ULL},{65,0x8406b6391eb6490eULL}};
        std::string bytes(65,'\0');for(size_t i=0;i<bytes.size();++i)bytes[i]=static_cast<char>(i);
        for(const auto& [length,hash]:vectors)CHECK(detail::textureNameHash(std::string_view(bytes).substr(0,length))==hash);
        auto isolated=m;constexpr uint64_t other=0x300040000;constexpr size_t buckets=257;
        const auto hash=detail::textureNameHash("test_atlas"),bucket=hash%buckets;
        const auto foreign=(bucket+1)%buckets; // Deliberately invalid unrelated name/file data.
        isolated.put(node,0,hash);isolated.put(rules,0x148,uint64_t(buckets));isolated.put(rules,0x150,uint64_t(2));
        isolated.regions[table].assign((buckets+1)*8,0);isolated.put(table,bucket*8,node);isolated.put(table,foreign*8,other);
        isolated.regions[other].resize(0xa0);isolated.put(other,0,uint64_t(foreign));isolated.put(other,0x98,other);
        isolated.put(other,0x80,uint64_t(0x400000000));isolated.put(other,0x88,uint64_t(0x400000000+4096*0x50));
        isolated.put(other,0x90,uint64_t(0x400000000+4096*0x50));isolated.put(defaults,32,hash);
        const std::string_view name="test_atlas";SignalTextureCatalog selected;
        isolated.reads=0;isolated.read_counts.clear();
        CHECK(read_signal_texture_sets(read,&isolated,s,true,{&name,1},selected));
        CHECK(selected.sets.size()==1&&selected.sets.at(hash).files[1].relative_path=="stop.svg");
        CHECK(isolated.read_counts[other]==0&&isolated.read_counts[0x400000000]==0&&isolated.read_counts[rules+0x540]==0);
        CHECK(isolated.reads<40);std::printf("Targeted catalogue: %u reads, zero foreign nodes/assets/defaults\n",isolated.reads);
        CHECK(!read_signal_texture_catalog(read,&isolated,s,true,catalog)); // Explicit full remains strict.
        auto changing=isolated;changing.changing=files;changing.changing_reads=0;
        CHECK(!read_signal_texture_sets(read,&changing,s,true,{&name,1},selected)&&selected.sets.empty());
        changing=isolated;changing.changing=node;changing.changing_reads=0;
        CHECK(!read_signal_texture_sets(read,&changing,s,true,{&name,1},selected));
        changing=isolated;changing.changing=table+bucket*8;changing.changing_reads=0;
        CHECK(!read_signal_texture_sets(read,&changing,s,true,{&name,1},selected));
        auto collision=isolated;collision.put(table,foreign*8,uint64_t(0));collision.put(table,bucket*8,other);
        collision.put(other,0,hash+buckets);collision.put(other,0x98,node);
        CHECK(read_signal_texture_sets(read,&collision,s,true,{&name,1},selected)); // Neighbour's bad name/files ignored.
        collision.put(other,0,hash); // A second matching key is ambiguous, never guessed.
        CHECK(!read_signal_texture_sets(read,&collision,s,true,{&name,1},selected));
        auto optional=isolated;optional.put(other,0x98,uint64_t(0));
        uint64_t absent=0;while(absent%buckets==bucket||absent%buckets==foreign)++absent;
        const SignalTextureReference references[]={{4,hash},{4,foreign},{4,absent}};
        CHECK(read_signal_texture_references(read,&optional,s,true,references,selected));
        CHECK(select_signal_textures(selected,4,hash)&&select_signal_textures(selected,4,absent));
        CHECK(!select_signal_textures(selected,4,foreign)); // Present-but-broken is not native absence.
        optional.put(rules,0x548,defaults+3); // Bad defaults cannot hide an explicit, valid set.
        CHECK(read_signal_texture_references(read,&optional,s,true,references,selected)&&select_signal_textures(selected,4,hash));
        CHECK(!select_signal_textures(selected,4,absent));
    }
    m.put(node,0x98,node);CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog)&&catalog.sets.empty());m.put(node,0x98,uint64_t(0));
    m.put(node,0x88,files+0x9f);CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog));m.put(node,0x88,files+0xa0);
    m.put(files,0,int32_t(99));CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog));m.put(files,0,int32_t(1));
    m.put(files,0x38,uint64_t(512));CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog));m.put(files,0x38,uint64_t(8));
    m.changing=files;CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog));m.changing=0;
    m.fail=table;CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog));m.fail=0;
    m.reads=0;CHECK(!read_signal_texture_catalog(read,&m,s,false,catalog)&&m.reads==0);
    m.put(rules,0x148,uint64_t(65537));CHECK(!read_signal_texture_catalog(read,&m,s,true,catalog));m.put(rules,0x148,uint64_t(1));
    m.put(rules,0x150,uint64_t(0));m.put(table,0,uint64_t(0));CHECK(read_signal_texture_catalog(read,&m,s,true,catalog)&&catalog.sets.empty());
    std::puts("Texture catalog: native/default lookup, strings, limits, cycles, read failures and instability passed.");
}
