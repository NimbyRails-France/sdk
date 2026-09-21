#include "engine/versioning.h"
#include <map>
#include <iostream>
#include <stdexcept>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
struct Memory {
    static constexpr uint64_t base=0x140000000,root=0x20000000,db=0x30000000,data=0x40000000;
    std::map<uint64_t,std::vector<unsigned char>> blocks;
    uint64_t changing=0;unsigned reads=0;
    template<class T> void put(uint64_t at,const T& value){
        blocks[at].resize(sizeof value);std::memcpy(blocks[at].data(),&value,sizeof value);
    }
    static bool read(void* context,uint64_t at,void* out,size_t bytes){
        auto& self=*static_cast<Memory*>(context);
        auto it=self.blocks.find(at);
        if(it==self.blocks.end()||it->second.size()!=bytes)return false;
        std::memcpy(out,it->second.data(),bytes);
        if(at==self.changing && ++self.reads>1)static_cast<unsigned char*>(out)[0]^=8;
        return true;
    }
    Memory(){
        put(base+0xb81998,root);put(root+0x540,db);
        put(root+0x5c0,uint64_t{0x50000000});put(root+0x680,uint64_t{0x60000000});
    }
    void header(uint64_t begin=0,uint64_t end=0,uint64_t cap=0,bool zero=false){
        std::array<unsigned char,0x38> bytes{};if(!zero)bytes[0]=42;
        std::memcpy(bytes.data()+32,&begin,8);std::memcpy(bytes.data()+40,&end,8);
        std::memcpy(bytes.data()+48,&cap,8);put(db+0xa48,bytes);
    }
};
int main(){try{
    using nimby::engine::VersioningObservation;
    for(bool linuxProfile:{false,true}){
    Memory memory;memory.header();
    const auto profile=linuxProfile?nimby::engine::LiveStateProfile::Linux119:nimby::engine::LiveStateProfile::Windows119;
    if(linuxProfile){memory.blocks[Memory::base+0x10ee020]=memory.blocks[Memory::base+0xb81998];memory.blocks.erase(Memory::base+0xb81998);}VersioningObservation out;
    auto capture=[&](bool recognized=true){if(linuxProfile)memory.blocks[Memory::db+0xa40]=memory.blocks[Memory::db+0xa48];return nimby::engine::read_versioning_observation(Memory::read,&memory,Memory::base,recognized,out,profile);};
    CHECK(capture()&&out.value[0]==42&&out.history.empty());
    CHECK(!capture(false)&&out.value==VersioningObservation::Value{}&&out.state.database==0);
    memory.header(0,0,0,true);CHECK(!capture());
    memory.header(Memory::data,Memory::data+32,Memory::data+64);
    VersioningObservation::Value history{};history[0]=7;memory.put(Memory::data,history);
    CHECK(capture()&&out.history.size()==1&&out.history[0]==history);
    memory.changing=Memory::data;memory.reads=0;CHECK(!capture()&&out.history.empty());
    memory.changing=Memory::db+(linuxProfile?0xa40:0xa48);memory.reads=0;CHECK(!capture());
    memory.changing=Memory::root+0x540;memory.reads=0;CHECK(!capture());
    memory.changing=0;
    memory.header(Memory::data,Memory::data+33,Memory::data+64);CHECK(!capture());
    memory.header(Memory::data,Memory::data-32,Memory::data);CHECK(!capture());
    memory.header(Memory::data,Memory::data,Memory::data+32*4097);CHECK(!capture());
    memory.header(0,32,32);CHECK(!capture());
    memory.header(Memory::data,Memory::data+32,Memory::data+32);
    memory.blocks.erase(Memory::data);CHECK(!capture());
    }
    std::cout<<"PASS versioning read guards and changing captures\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
