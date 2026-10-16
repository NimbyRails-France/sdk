// Bounded secondary presence reads over synthetic owned process memory.
// No game process, OS timing or native authority is involved.
#include <engine/network.h>
#include <engine/detail/memory_reader.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <new>
#include <stdexcept>
#include <vector>

#ifndef BATCHED
#define BATCHED 1
#endif
namespace allocation {
inline bool watch=false,failScratch=false;
inline size_t calls=0,scratch=0;
constexpr size_t scratchSize=511*0x638+0x4b1;
}
void* operator new(size_t n){
    if(allocation::watch){++allocation::calls;if(n==allocation::scratchSize){++allocation::scratch;if(allocation::failScratch)throw std::bad_alloc();}}
    if(auto* p=std::malloc(n?n:1))return p;
    throw std::bad_alloc();
}
void* operator new[](size_t n){return ::operator new(n);}
void operator delete(void* p)noexcept{std::free(p);}
void operator delete[](void* p)noexcept{std::free(p);}
void operator delete(void* p,size_t)noexcept{std::free(p);}
void operator delete[](void* p,size_t)noexcept{std::free(p);}

using namespace nimby::engine;
namespace {
size_t checks=0;
#define CHECK(x) do{++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
constexpr uint64_t models=0x300000000,motions=0x310000000,track=0x1000000000001;
constexpr size_t stride=0x638,prefix=0x4b1;
constexpr unsigned minimumBatchRows=128,denseRows=4096;
uint64_t id(unsigned ordinal){return 0x5000000000001ULL+(uint64_t(ordinal)<<16);}
struct Memory {
    LiveState state{0x140000000,0x200000000,0x200010000,0x200020000,0x200030000};
    std::map<uint64_t,std::vector<unsigned char>> regions;
    unsigned slots=512,blocks=0;
    size_t requests=0,bytes=0,secondary=0,batches=0,prefixes=0,full=0,afterTerminal=0,copied=0;
    bool failBatch=false,terminalBatch=false,terminal=false,changeHeader=false;
    uint64_t failBatchAddress=0;
    uint64_t failPrefix=0,mutate=0;
    size_t mutateOffset=0;uint8_t mutateMask=1;
    bool moveHead=false;double latest=.4;
    template<class T>void put(uint64_t address,size_t off,T value){auto& p=regions[address];if(p.size()<off+sizeof value)p.resize(off+sizeof value);std::memcpy(p.data()+off,&value,sizeof value);}
    Memory(unsigned count,unsigned blockSlots=512,bool sparse=false):slots(blockSlots),blocks((count+slots-1)/slots){
        put(state.module_base+gameLayout(state.profile).root_rva,0,state.root);
        put(state.root,0x540,state.database);put(state.root,0x5c0,state.copy);put(state.root,0x680,state.simulation);
        const auto pool=[&](uint64_t descriptor,uint64_t block,size_t record){
            unsigned shift=0;while((1u<<shift)<slots)++shift;
            const auto table=block-0x10000;regions[descriptor].resize(48);
            put(descriptor,4,uint32_t(shift));put(descriptor,8,uint32_t(slots));put(descriptor,16,uint32_t(slots-1));
            if(!blocks)return;
            put(descriptor,24,table);put(descriptor,32,table+blocks*8);put(descriptor,40,table+blocks*8);
            regions[block].resize(size_t(blocks)*slots*record);
            for(unsigned b=0;b<blocks;++b)put(table,b*8,block+size_t(b)*slots*record);
        };
        pool(state.database+0x200,models,0x178);pool(state.simulation+0xa0,motions,stride);
        for(unsigned i=0;i<count;++i){
            if(sparse&&i%64)continue;
            put(models,size_t(i)*0x178,id(i));
            // Inline empty name, valid for complete observations as well.
            put(models,size_t(i)*0x178+0x28,uint64_t(15));
            put(motions,size_t(i)*stride,id(i));put(motions,size_t(i)*stride+0x1d0,uint8_t(1));
            put(motions,size_t(i)*stride+0x4b0,uint8_t(1));put(motions,size_t(i)*stride+0x3a8,track);
            put(motions,size_t(i)*stride+0x3b0,.25);put(motions,size_t(i)*stride+0x3b8,int8_t(1));
            put(motions,size_t(i)*stride+0x3c8,12.);
        }
    }
    void reset(){requests=bytes=secondary=batches=prefixes=full=afterTerminal=copied=0;terminal=false;}
};
bool read(void* context,uint64_t address,void* output,size_t size){
    auto& m=*static_cast<Memory*>(context);++m.requests;m.bytes+=size;
    // A terminal budget denies actual reads, even though the bool-only
    // ReadMemory interface cannot tell read_trains to stop callback attempts.
    if(m.terminal){++m.afterTerminal;return false;}
    const auto motionEnd=motions+size_t(m.blocks)*m.slots*stride;
    const bool inMotion=address>=motions&&address<motionEnd;
    const bool secondary=inMotion&&(size==prefix||size==stride||size%stride==prefix);
    const bool batch=secondary&&size>stride;
    if(secondary){++m.secondary;m.prefixes+=size==prefix;m.full+=size==stride;m.batches+=batch;}
    if(batch&&(m.failBatch||m.failBatchAddress==address)){
        std::memset(output,0xa5,size);if(m.terminalBatch)m.terminal=true;return false;
    }
    if(size==prefix&&address==m.failPrefix)return false;
    auto at=m.regions.upper_bound(address);if(at==m.regions.begin())return false;--at;
    const auto offset=address-at->first;
    if(offset>at->second.size()||size>at->second.size()-offset)return false;
    std::memcpy(output,at->second.data()+offset,size);
    ++m.copied;
    if(secondary&&m.changeHeader){m.regions.at(m.state.simulation+0xa0)[0]^=1;m.changeHeader=false;}
    if(secondary&&m.mutate>=address&&m.mutate-address+prefix<=size){
        auto* row=static_cast<unsigned char*>(output)+m.mutate-address;
        if(m.moveHead)std::memcpy(row+0x3b0,&m.latest,sizeof m.latest);
        else row[m.mutateOffset]^=m.mutateMask;
    }
    return true;
}
std::vector<Train> capture(Memory& m,bool presence=true,bool expected=true,bool allow=true){
    std::vector<Train> out;TrainMetadataCatalog metadata;
    CHECK(read_trains(read,&m,m.state,true,out,presence,presence?nullptr:&metadata,false,0,allow)==expected);
    if(!expected)CHECK(out.empty());
    return out;
}
void valid(const std::vector<Train>& out,size_t count){
    CHECK(out.size()==count);
    for(const auto& train:out){CHECK(train.service.flags&NIMBY_SERVICE_PRESENCE_VALID);CHECK(train.positioned);CHECK(train.position.track_id==track);CHECK(train.position.fraction==.25);}
}
void contracts(){
    {
        Memory m(50);allocation::watch=true;allocation::scratch=0;auto out=capture(m);allocation::watch=false;
        valid(out,50);CHECK(m.batches==0&&m.prefixes==50&&allocation::scratch==0);
    }
    {
        Memory m(5000);allocation::watch=true;allocation::scratch=0;auto out=capture(m);allocation::watch=false;
        valid(out,5000);CHECK(m.batches==(BATCHED?10:0));CHECK(m.prefixes==(BATCHED?0:5000));CHECK(allocation::scratch==(BATCHED?1:0));
    }
    for(const auto count:{0u,1u,7u,8u,minimumBatchRows-1,minimumBatchRows,4095u}){Memory m(count);valid(capture(m),count);CHECK(m.batches==0&&m.prefixes==count);}
    {Memory m(4096);valid(capture(m),4096);CHECK(m.batches==(BATCHED?8:0));}
    {Memory m(512,512,true);valid(capture(m),8);CHECK(m.batches==0&&m.prefixes==8);}
    {Memory m(4096,1024);valid(capture(m),4096);CHECK(m.batches==0&&m.prefixes==4096);}
    for(const auto span:{minimumBatchRows*2,minimumBatchRows*2+1}){
        Memory m(4096);
        for(unsigned i=minimumBatchRows-1;i<512;++i)if(i!=span-1)m.put(motions,size_t(i)*stride,uint64_t(0));
        auto out=capture(m);CHECK(out.size()==4096&&out[0].positioned&&out[span-1].positioned);CHECK(m.batches==(BATCHED?(span==minimumBatchRows*2?8:7):0));
    }
    {
        Memory m(denseRows);m.failBatch=true;valid(capture(m),denseRows);
        CHECK(m.batches==(BATCHED?8:0)&&m.prefixes==denseRows); // Read failure only permits fallback.
    }
    {
        Memory m(denseRows);m.failBatch=true;m.failPrefix=motions+13*stride;auto out=capture(m);
        CHECK(out.size()==denseRows&&!(out[13].service.flags&NIMBY_SERVICE_PRESENCE_VALID)&&!out[13].positioned);
        CHECK(out[12].positioned&&out[14].positioned);
    }
    {
        Memory m(4096);m.failBatchAddress=motions+512*stride;valid(capture(m),4096);
        CHECK(m.batches==(BATCHED?8:0)&&m.prefixes==(BATCHED?512:4096));
    }
    {Memory m(denseRows);m.changeHeader=true;capture(m,true,false);}
    {Memory m(denseRows);m.put(motions,13*stride,uint64_t(0x80000000d0001));capture(m,true,false);}
    // No successful observed corruption can be healed by an individual read.
    for(const auto off:{size_t(0),size_t(0x1d0),size_t(0x218),size_t(0x4b0),size_t(0x3a8),size_t(0x3b8)}){
        Memory m(denseRows);m.mutate=motions+13*stride;m.mutateOffset=off;
        if(off==0x1d0||off==0x4b0)m.mutateMask=2;
        auto out=capture(m);
        CHECK(out.size()==denseRows&&!out[13].positioned);CHECK(out[12].positioned&&out[14].positioned);
        CHECK(m.prefixes==(BATCHED?0:denseRows));
        if(off<0x3a8||off==0x4b0)CHECK(!(out[13].service.flags&NIMBY_SERVICE_PRESENCE_VALID));
    }
    {
        Memory m(denseRows);m.mutate=motions+13*stride;m.moveHead=true;auto out=capture(m);
        CHECK(out[13].positioned&&out[13].position.fraction==.4&&out[12].position.fraction==.25);
    }
    {
        Memory m(denseRows);m.mutate=motions+13*stride;m.moveHead=true;m.latest=.1;auto out=capture(m);
        CHECK(!out[13].positioned&&out[12].positioned&&out[14].positioned);
    }
    {
        Memory m(50);auto out=capture(m,false);CHECK(out.size()==50&&m.batches==0&&m.prefixes==0&&m.full==50);
        CHECK(out[13].speed_available&&out[13].speed_mps==12.); // Full/service route keeps whole record.
    }
    {
        Memory m(4097);m.put(models,4096*0x178,id(4096)|0xffff000000000000ULL);
        for(const auto off:{0x1d0,0x218,0x4b0})m.put(motions,4096*stride+off,uint8_t(0));
        auto out=capture(m);valid(out,4096);CHECK(m.full==1); // Retired Motion recheck remains 0x638.
    }
    // The internal strategy defaults false for all generic callers.
    {
        Memory m(5000);std::vector<Train> out;
        CHECK(read_trains(read,&m,m.state,true,out,true));valid(out,5000);
        CHECK(m.batches==0&&m.prefixes==5000);
    }
    {
        Memory m(5000);valid(capture(m,true,true,false),5000);
        CHECK(m.batches==0&&m.prefixes==5000);
    }
    {
        Memory m(5000);m.state.profile=LiveStateProfile::Linux119;
        m.put(m.state.module_base+gameLayout(m.state.profile).root_rva,0,m.state.root);
        valid(capture(m),5000);CHECK(m.batches==0&&m.prefixes==5000);
    }
    // Minimum128 is per block, even when total DB roster already reaches4096.
    for(const auto live:{127u,128u}){
        Memory m(4096);
        for(unsigned i=live;i<512;++i)m.put(motions,size_t(i)*stride,uint64_t(0));
        auto out=capture(m);CHECK(out.size()==4096&&out[0].positioned&&!out[live].positioned);
        CHECK(m.batches==(BATCHED?(live==128?8:7):0));
    }
    if constexpr(BATCHED){
        Memory m(denseRows);m.failBatch=m.terminalBatch=true;capture(m,true,false);
        CHECK(m.batches==1&&m.prefixes==0&&m.afterTerminal>0&&m.afterTerminal<=denseRows+1);
        // No extra target copy, but attempts remain bounded rather than an
        // invented zero-byte preflight. New explicit reader API would be needed.
        CHECK(m.copied+1==m.requests-m.afterTerminal);
        Memory oom(5000);allocation::watch=allocation::failScratch=true;allocation::scratch=0;
        auto out=capture(oom);allocation::watch=allocation::failScratch=false;valid(out,5000);
        CHECK(oom.batches==0&&oom.prefixes==5000&&allocation::scratch==1);
    }
}
}
int main(){try{
    contracts();std::cout<<"PASS presence batch contracts "<<checks<<" checks\n";
}catch(const std::exception& e){allocation::watch=allocation::failScratch=false;std::cerr<<e.what()<<'\n';return 1;}}
