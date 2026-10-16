#pragma once
#include <platform/windows/runtime/native_physical_atlas.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <vector>
// Exact synthetic memory/owner setup reused from the existing SDK fixture.
// Atlas tests call the ACTUAL owned inventory and current-body producers.
// This sealed memory is not a game ownership/writer exclusion factory.
namespace fixture {
namespace provider=nimby::windows::consist_rear;
namespace rear=nimby::engine::native_rear;
namespace inventory=nimby::windows::consist_inventory;
using provider::CarSetup;using provider::Reason;using provider::Result;
constexpr uintptr_t db=0x10000000,sim=0x11000000,motionBlock=0x20000000,
    trainBlock=0x21000000,trackBlock=0x22000000,motionPointers=0x30000000,
    trainPointers=0x30000100,trackPointers=0x30000200,planned=0x40000000,
    retained=0x41000000,history=0x42000000,children=0x43000000,
    rules=db+0xa80,table=0x50000100,models=0x51000000;
constexpr uint64_t train(size_t ordinal,uint16_t generation=1){return (uint64_t(5)<<48)|(ordinal<<16)|generation;}
constexpr uint64_t track(size_t ordinal,uint16_t generation=1){return (uint64_t(1)<<48)|(ordinal<<16)|generation;}
constexpr size_t carCapacity=128;
size_t checks=0;
void require(bool yes,const char* expression,int line){
    ++checks;if(!yes){std::fprintf(stderr,"line %d: %s\n",line,expression);std::abort();}
}
#define CHECK(...) require(bool(__VA_ARGS__),#__VA_ARGS__,__LINE__)
template<class T>void put(auto& bytes,size_t at,const T& value){std::memcpy(bytes.data()+at,&value,sizeof value);}
struct Region {uintptr_t address{};std::vector<unsigned char> bytes;};
struct Memory {
    std::array<Region,15> regions{{{db,std::vector<unsigned char>(0xb00)},
        {sim,std::vector<unsigned char>(0x200)},
        {motionBlock,std::vector<unsigned char>(0x100000)},
        {trainBlock,std::vector<unsigned char>(0x100000)},
        {trackBlock,std::vector<unsigned char>(0x100000)},
        {motionPointers,std::vector<unsigned char>(0x300)},
        {planned,std::vector<unsigned char>(8*carCapacity*32)},
        {retained,std::vector<unsigned char>(8*carCapacity*32)},
        {history,std::vector<unsigned char>(128*16)},
        {children,std::vector<unsigned char>(128*8)},
        {rules,std::vector<unsigned char>(0x80)},
        {table,std::vector<unsigned char>(3*8)},
        {models,std::vector<unsigned char>(16*0x300)},
        {0x60000000,std::vector<unsigned char>(0x100)},
        {0x61000000,std::vector<unsigned char>(0x100)}}};
    size_t reads=0,bytesRead=0,denyRead=0,throwRead=0,scopeCalls=0,denyScope=0;
    uintptr_t mutationRead=0,mutationAddress=0;
    size_t mutationAtOccurrence=2,mutationSeen=0;
    std::array<unsigned char,32> mutationValue{};size_t mutationBytes=0;
    bool gateRefused=false,readAfterRefusal=false,readRoster=false;
    provider::Witness witness{db,sim,{1,2,3,rear::Phase::RetainedMovement},train(0),4,5};
    template<class T>void write(uintptr_t at,const T& value){
        for(auto& region:regions)if(at>=region.address&&at-region.address<=region.bytes.size()&&sizeof value<=region.bytes.size()-(at-region.address)){
            std::memcpy(region.bytes.data()+at-region.address,&value,sizeof value);return;
        }
        CHECK(false);
    }
    void raw(uintptr_t at,const void* value,size_t bytes){
        for(auto& region:regions)if(at>=region.address&&at-region.address<=region.bytes.size()&&bytes<=region.bytes.size()-(at-region.address)){
            std::memcpy(region.bytes.data()+at-region.address,value,bytes);return;
        }
        CHECK(false);
    }
    template<class T>T value(uintptr_t at){T out{};for(const auto& region:regions){
        if(at>=region.address&&at-region.address<=region.bytes.size()&&sizeof out<=region.bytes.size()-(at-region.address)){
            std::memcpy(&out,region.bytes.data()+at-region.address,sizeof out);return out;
        }
        }
        CHECK(false);return out;
    }
    void vector(uintptr_t at,uintptr_t address,size_t count,size_t stride){
        write(at,provider::detail::Vector{address,address+count*stride,address+count*stride});
    }
    void pool(uintptr_t at,uintptr_t pointers,uintptr_t block,size_t stride){
        write(at,uint32_t(0x100000));write(at+4,uint32_t(9));write(at+8,uint32_t(512));
        write(at+12,uint32_t(512*stride));write(at+16,uint32_t(511));
        vector(at+24,pointers,1,8);write(pointers,block);
    }
    void car(size_t member,size_t ordinal,uint64_t model=100,uint8_t mask=255){
        CarSetup setup{model,0,{}};setup.settings[15]=mask;setup.settings[2]=17;
        write(planned+(member*carCapacity+ordinal)*32,setup);
        setup.owner=train(member);const auto index=uint16_t(ordinal);
        std::memcpy(setup.settings.data(),&index,2);
        write(retained+(member*carCapacity+ordinal)*32,setup);
    }
    void member(size_t ordinal,size_t count,uint64_t parent=0,int64_t order=1,bool reverse=false){
        const auto motion=motionBlock+ordinal*0x638,dbTrain=trainBlock+ordinal*0x178;
        write(motion,train(ordinal));write(dbTrain,train(ordinal));write(motion+0x80,int32_t(0));
        write(motion+0x1d0,uint8_t(parent?0:1));write(motion+0x1d8,parent);write(motion+0x1e0,order);
        write(motion+0x1e8,uint8_t(reverse));write(motion+0x1f0,uint8_t(parent!=0));
        vector(dbTrain+0xc0,planned+ordinal*carCapacity*32,count,32);
        vector(motion+8,retained+ordinal*carCapacity*32,count,32);vector(motion+0xe0,history,0,16);
        for(size_t i=0;i<count;++i)car(ordinal,i);
        write(motion+0x40,float(count*20));
    }
    void model(size_t ordinal,uint64_t id,float meters){
        const auto at=models+ordinal*0x300;write(at,id);write(at+0xfc,meters);write(at+0x2a0,uintptr_t(0));
        write(table+(id%2)*8,at);
    }
    void trackData(size_t ordinal,uint64_t a=0,uint64_t b=0,double metric=100,int8_t orientation=0){
        const auto at=trackBlock+ordinal*0x4e8;
        write(at,track(ordinal));write(at+8,a);write(at+16,b);write(at+0x88,metric);write(at+0x2c,orientation);
    }
    void head(double fraction=.75,int8_t direction=1,int8_t orientation=0){
        nimby::windows::automatic::CommitPresence presence{0,0,track(0),fraction,direction,orientation,{},fraction*100};
        write(motionBlock+0x3a0,presence);write(motionBlock+0xb8,track(0));write(motionBlock+0xc0,fraction);
        write(motionBlock+0xc8,direction);write(motionBlock+0xc9,orientation);
    }
    void reset(){
        for(auto& region:regions)std::fill(region.bytes.begin(),region.bytes.end(),0);
        reads=bytesRead=denyRead=throwRead=scopeCalls=denyScope=mutationRead=mutationAddress=mutationSeen=mutationBytes=0;
        mutationAtOccurrence=2;gateRefused=readAfterRefusal=readRoster=false;
        witness={db,sim,{1,2,3,rear::Phase::RetainedMovement},train(0),4,5};
        pool(sim+0xa0,motionPointers,motionBlock,0x638);pool(db+0x200,trainPointers,trainBlock,0x178);
        pool(db,trackPointers,trackBlock,0x4e8);write(db+0x408,rules);write(rules+0x38,table);write(rules+0x40,uint32_t(2));
        write(table+16,uintptr_t(0x60000000));model(0,100,20);model(1,101,10);
        member(0,1);trackData(0);head();
    }
    Memory(){reset();}
    template<class T>void mutate(uintptr_t readAt,uintptr_t changedAt,const T& value,size_t occurrence=2){
        CHECK(sizeof value<=mutationValue.size());mutationRead=readAt;mutationAddress=changedAt;
        mutationBytes=sizeof value;mutationSeen=0;std::memcpy(mutationValue.data(),&value,sizeof value);mutationAtOccurrence=occurrence;
    }
    bool read(uintptr_t at,void* out,size_t bytes){
        if(gateRefused)readAfterRefusal=true;
        ++reads;bytesRead+=bytes;
        if(reads==throwRead)throw 7;
        if(reads==denyRead)return false;
        if(at==mutationRead&&++mutationSeen==mutationAtOccurrence)raw(mutationAddress,mutationValue.data(),mutationBytes);
        if(at>=motionBlock&&at<motionBlock+0x100000&&(at-motionBlock)%0x638==0x88)readRoster=true;
        for(const auto& region:regions)if(at>=region.address&&at-region.address<=region.bytes.size()&&bytes<=region.bytes.size()-(at-region.address)){
            std::memcpy(out,region.bytes.data()+at-region.address,bytes);return true;
        }
        return false;
    }
    static bool seal(void* context,const provider::Witness& requested)noexcept{
        auto& memory=*static_cast<Memory*>(context);++memory.scopeCalls;
        return memory.scopeCalls!=memory.denyScope&&requested==memory.witness;
    }
    provider::Scope scope(){return {witness,seal,this};}
    void aggregate(size_t rootCars,size_t childCars,bool reverse=false,int64_t order=1){
        member(0,rootCars);member(1,childCars,train(0),order,reverse);
        std::array<CarSetup,carCapacity> full{};size_t at=0;
        const auto append=[&](size_t member,size_t count,bool reversed){
            for(size_t i=0;i<count;++i){auto setup=value<CarSetup>(retained+(member*carCapacity+(reversed?count-1-i:i))*32);
                if(reversed)setup.settings[12]=uint8_t(!setup.settings[12]);
                full[at++]=setup;}
        };
        if(order<0)append(1,childCars,reverse);
        append(0,rootCars,false);
        if(order>=0)append(1,childCars,reverse);
        raw(retained,full.data(),at*32);vector(motionBlock+8,retained,at,32);write(motionBlock+0x40,float(at*20));
    }
};
struct Gate {
    Memory& memory;size_t calls=0,deny=0,afterRefusal=0;
    bool refused=false;
    static bool preflight(void* context,size_t)noexcept{
        auto& gate=*static_cast<Gate*>(context);if(gate.refused)++gate.afterRefusal;
        if(++gate.calls==gate.deny){gate.refused=true;gate.memory.gateRefused=true;return false;}return true;
    }
};
namespace owned=nimby::windows::owned_rear;
namespace world=nimby::windows::native_world;
struct Owner {
    world::Clock clock;
    world::Lineage line;
    world::Capture token;
    uint64_t thread=7;
    bool allowed=true;
    size_t calls=0,deny=0;
    static bool current(void* opaque,const world::Capture& requested)noexcept {
        auto& owner=*static_cast<Owner*>(opaque);
        return ++owner.calls!=owner.deny&&owner.allowed&&owner.line.current(requested);
    }
    static uint64_t currentThread(void* opaque)noexcept{return static_cast<Owner*>(opaque)->thread;}
    void begin(){
        constexpr uintptr_t controller=0x100000,context=controller+0x5d0;
        if(line.state()==world::State::Capturing)CHECK(line.endCapture(token));
        if(line.state()!=world::State::Running){
            const auto stop=line.beginStop(clock,controller,context);CHECK(stop);CHECK(line.finishStop(stop));
            const auto start=line.beginStart(clock,controller,context);CHECK(start);CHECK(line.finishStart(start));
            CHECK(line.runnerEnter(controller,context,7));
        }
        world::Scope scope{controller,context,controller+0x4c0,db,sim,7};
        scope.boundary=world::Boundary::PostSharedExclusive;scope.releaseReturn=0x349fa3;
        scope.originalReleaseCompleted=true;scope.exclusiveTrySucceeded=true;
        const auto captured=line.capturePhase(clock,scope);CHECK(captured.reason==world::Reason::Ready);
        token=captured.capture;thread=7;allowed=true;calls=deny=0;
    }
    owned::Ownership scope(){return {token,current,currentThread,this};}
};
} // namespace fixture
