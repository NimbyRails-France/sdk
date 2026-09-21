#include "engine/named_signal_extensions.h"
#include <map>
#include <cstdio>
using namespace nimby::engine;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;
    uint64_t mutate=0;unsigned hit=0,reads=0;
    template<class T> void put(uint64_t address,size_t offset,T value) {
        auto& bytes=regions[address];if(bytes.size()<offset+sizeof value)bytes.resize(offset+sizeof value);
        std::memcpy(bytes.data()+offset,&value,sizeof value);
    }
};
static bool read(void* context,uint64_t address,void* out,size_t size) {
    auto& m=*static_cast<Memory*>(context);++m.reads;
    auto i=m.regions.upper_bound(address);if(i==m.regions.begin())return false;--i;
    if(address-i->first>i->second.size() || size>i->second.size()-(address-i->first))return false;
    std::memcpy(out,i->second.data()+address-i->first,size);
    if(address==m.mutate && ++m.hit==2 && size)static_cast<unsigned char*>(out)[0]^=1;
    return true;
}
int main() {
    constexpr uint64_t signal=0x8000000000001,script=0x7000000000001;
    constexpr uint64_t object=0x100000,instances=0x200000,values=0x300000,meta=0x400000;
    Memory m;m.regions[object].resize(0xa8);m.put(object,0,signal);
    std::vector<SignalExtension> out(1);
    if(read_signal_extensions(read,&m,object,signal,false,out)||!out.empty()||m.reads)return 1;
    if(!read_signal_extensions(read,&m,object,signal,true,out)||!out.empty())return 2;
    m.put(object,0x90,instances);m.put(object,0x98,instances+0x50);m.put(object,0xa0,instances+0x50);
    m.regions[instances].resize(0x50);m.put(instances,0,script);m.put(instances,8,uint64_t(123));m.put(instances,0x40,uint64_t(456));
    m.put(instances,0x10,values);m.put(instances,0x18,values+32);m.put(instances,0x20,values+32);
    m.regions[values].resize(32);m.put(values,8,uint8_t(2));m.put(values,16,uint8_t(1));m.put(values,24,uint8_t(2));
    m.put(instances,0x28,meta);m.put(instances,0x30,meta+32);m.put(instances,0x38,meta+32);
    m.regions[meta].resize(32);
    if(!read_signal_extensions(read,&m,object,signal,true,out)||out.size()!=1||out[0].values.size()!=2||
       out[0].values[0].boolean()!=false||out[0].values[1].boolean()!=true)return 3;
    const auto baseline=m;
    // A recycled Signal slot must not inherit settings from its former ID.
    if(read_signal_extensions(read,&m,object,signal+1,true,out)||!out.empty())return 4;
    // Reject replacement of payload, metadata, instance vector or owner ID.
    for(auto address:{values,meta,instances,object}) {
        m.mutate=address;m.hit=0;out.resize(1);
        if(read_signal_extensions(read,&m,object,signal,true,out)||!out.empty())return 5;
    }
    m.mutate=0;m.put(instances,0x18,values+17);
    if(read_signal_extensions(read,&m,object,signal,true,out))return 6;
    m.put(instances,0x18,values+32);m.put(instances,0x20,values+16*513);
    if(read_signal_extensions(read,&m,object,signal,true,out))return 7;
    m.put(instances,0x20,values+32);m.regions.erase(values);
    if(read_signal_extensions(read,&m,object,signal,true,out))return 8;
    ExtensionValue v;v.bytes[0]=1;if(v.boolean())return 9;
    v.bytes[8]=2;v.bytes[0]=3;if(v.boolean())return 10;
    m=baseline;
    LiveState state{};state.database=0x500000;
    constexpr uint64_t libraries=0x600000,libraryHead=0x610000,libraryNode=0x620000;
    constexpr uint64_t library=libraryNode+24,typeHead=0x700000,typeNode=0x710000,type=typeNode+24;
    constexpr uint64_t scriptTable=0x800000,scriptBlock=0x900000,fields=0xb00000;
    auto list=[&](uint64_t map,uint64_t head,uint64_t node,uint64_t key){
        m.put(map,8,head);m.put(map,16,uint64_t(1));m.put(head,0,node);
        m.put(node,0,head);m.put(node,8,head);m.put(node,16,key);
    };
    m.put(state.database+0x1518,0,libraries);list(libraries,libraryHead,libraryNode,script);
    m.regions[library].resize(0xd0);list(library+0xb8,typeHead,typeNode,456);
    m.regions[state.database+0x300].resize(48);
    m.put(state.database+0x300,4,uint32_t(1));m.put(state.database+0x300,8,uint32_t(2));m.put(state.database+0x300,16,uint32_t(1));
    m.put(state.database+0x300,24,scriptTable);m.put(state.database+0x300,32,scriptTable+8);m.put(state.database+0x300,40,scriptTable+8);
    m.put(scriptTable,0,scriptBlock);m.regions[scriptBlock].resize(0x150);m.put(scriptBlock,0,script);m.put(scriptBlock,0xd8,uint8_t(1));
    auto name=[&](uint64_t address,size_t offset,const char* text){
        const size_t length=std::strlen(text);m.put(address,offset+16,uint64_t(length));m.put(address,offset+24,uint64_t(15));
        std::memcpy(m.regions[address].data()+offset,text,length+1);
    };
    m.regions[type].resize(0x90);name(type,0,"BalConfig");m.put(type,0x20,uint64_t(456));m.put(type,0x88,uint64_t(123));
    m.put(type,0x70,fields);m.put(type,0x78,fields+0xc0);m.put(type,0x80,fields+0xc0);
    m.regions[fields].resize(0xc0);name(fields,0,"active");name(fields,0x60,"greenFlash");
    m.put(fields,0x20,uint64_t(21));m.put(fields,0x80,uint64_t(22));m.put(meta,0,uint64_t(21));m.put(meta,16,uint64_t(22));
    std::vector<NamedSignalExtension> named;
    if(!read_named_signal_extensions(read,&m,state,object,signal,true,named)||named.size()!=1||named[0].typeName!="BalConfig"||
       named[0].fields.at("active").boolean()!=false||named[0].fields.at("greenFlash").boolean()!=true)return 11;
    // Reloaded schema and old instance must never be zipped by position.
    m.put(type,0x88,uint64_t(124));if(read_named_signal_extensions(read,&m,state,object,signal,true,named)||!named.empty())return 12;
    m.put(type,0x88,uint64_t(123));m.put(meta,0,uint64_t(22));
    if(read_named_signal_extensions(read,&m,state,object,signal,true,named))return 13;
    m.put(meta,0,uint64_t(21));m.put(scriptBlock,0xd8,uint8_t(0));
    if(read_named_signal_extensions(read,&m,state,object,signal,true,named))return 14;
    m.put(scriptBlock,0xd8,uint8_t(1));m.put(scriptBlock,0,script+1);
    if(read_named_signal_extensions(read,&m,state,object,signal,true,named))return 15;
    m.put(scriptBlock,0,script);m.mutate=type;m.hit=0;
    if(read_named_signal_extensions(read,&m,state,object,signal,true,named))return 16;
    std::puts("signal extension read guards passed");
}
