#include <platform/windows/runtime/native_consist_rear_provider.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <vector>

// Synthetic, sealed owned-memory fixture. No native function, game process,
// authority or production ownership/continuity factory is exercised here.
namespace fixture {bool watching=false;size_t allocations=0;}
#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE __declspec(noinline)
#endif
NOINLINE void* operator new(size_t size){
    if(fixture::watching)++fixture::allocations;
    if(auto* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
NOINLINE void* operator new[](size_t size){return ::operator new(size);}
NOINLINE void operator delete(void* value)noexcept{std::free(value);}
NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
NOINLINE void operator delete(void* value,size_t)noexcept{std::free(value);}
NOINLINE void operator delete[](void* value,size_t)noexcept{std::free(value);}
#undef NOINLINE
namespace fixture {
namespace provider=nimby::windows::consist_rear;
namespace rear=nimby::engine::native_rear;
namespace inventory=nimby::windows::consist_inventory;
using provider::CarSetup;using provider::Reason;using provider::Result;
constexpr uintptr_t db=0x10000000,sim=0x11000000,motionBlock=0x20000000,
    trainBlock=0x21000000,trackBlock=0x22000000,motionPointers=0x30000000,
    trainPointers=0x30000100,trackPointers=0x30000200,planned=0x40000000,
    retained=0x41000000,history=0x42000000,children=0x43000000,
    rules=0x50000000,table=0x50000100,models=0x51000000;
constexpr uint64_t train(size_t ordinal,uint16_t generation=1){return (uint64_t(5)<<48)|(ordinal<<16)|generation;}
constexpr uint64_t track(size_t ordinal,uint16_t generation=1){return (uint64_t(1)<<48)|(ordinal<<16)|generation;}
constexpr size_t carCapacity=4096;
size_t checks=0;
void require(bool yes,const char* expression,int line){
    ++checks;if(!yes){std::fprintf(stderr,"line %d: %s\n",line,expression);std::abort();}
}
#define CHECK(...) require(bool(__VA_ARGS__),#__VA_ARGS__,__LINE__)
template<class T>void put(auto& bytes,size_t at,const T& value){std::memcpy(bytes.data()+at,&value,sizeof value);}
struct Region {uintptr_t address{};std::vector<unsigned char> bytes;};
struct Memory {
    std::array<Region,15> regions{{{db,std::vector<unsigned char>(0x410)},
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
    bool gateRefused=false,readAfterRefusal=false,readRoster=false,readHistoryDescriptor=false,readInactiveChildOptional=false;
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
        mutationAtOccurrence=2;gateRefused=readAfterRefusal=readRoster=readHistoryDescriptor=readInactiveChildOptional=false;
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
        if(at>=motionBlock&&at<motionBlock+0x100000&&(at-motionBlock)%0x638==0xe0)readHistoryDescriptor=true;
        // The child in our sealed fixture has no qualified independent body.
        // Poisoned optional descriptors must not be consulted by current-only
        // capture. Only destruction of its history is established natively.
        constexpr auto child=motionBlock+0x638;
        if(value<uint8_t>(child+0x1d0)==0&&value<uint8_t>(child+0x1f0)==1&&
           (at==child+8||at==child+0x40||at==child+0xe0||at==child+0xb8||at==child+0x3a0))
            readInactiveChildOptional=true;
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
struct Storage {
    std::array<inventory::Row,16> inventoryRows{},inventoryScratch{};
    std::array<uint64_t,512> ids{};
    std::array<provider::detail::Member,8> members{};
    std::array<CarSetup,carCapacity> expected{},actual{},temporary{};
    std::array<provider::PhysicalCar,carCapacity> cars{},carsOut{};
    std::array<provider::NativeHistory,128> history{},historyAgain{};
    std::array<provider::detail::Model,16> models{};
    std::array<provider::detail::Track,128> tracks{};
    std::array<uint64_t,128> children{},childrenAgain{};
    std::array<rear::HistorySection,128> sections{},sectionsOut{};
    std::array<rear::Portion,128> portions{},portionsOut{};
    std::array<unsigned char,sizeof carsOut+sizeof sectionsOut+sizeof portionsOut> before{};
    provider::Workspace scratch(){return {inventoryRows,inventoryScratch,ids,members,expected,actual,temporary,cars,
        history,historyAgain,models,tracks,children,childrenAgain,sections,portions};}
    provider::Output output(){return {carsOut,sectionsOut,portionsOut};}
    void sentinel(){
        before.fill(0xa5);std::memcpy(carsOut.data(),before.data(),sizeof carsOut);
        std::memcpy(sectionsOut.data(),before.data()+sizeof carsOut,sizeof sectionsOut);
        std::memcpy(portionsOut.data(),before.data()+sizeof carsOut+sizeof sectionsOut,sizeof portionsOut);
    }
    bool untouched()const{return std::memcmp(carsOut.data(),before.data(),sizeof carsOut)==0&&
        std::memcmp(sectionsOut.data(),before.data()+sizeof carsOut,sizeof sectionsOut)==0&&
        std::memcmp(portionsOut.data(),before.data()+sizeof carsOut+sizeof sectionsOut,sizeof portionsOut)==0;}
};
Result capture(Memory& memory,Storage& storage,Gate& gate,provider::Limits limits={},const provider::InventoryView* view=nullptr,
    provider::Kind kind=provider::Kind::ProgressionQualified,provider::PublicationProof* publication=nullptr){
    storage.sentinel();inventory::Budget budget{Gate::preflight,&gate};auto scope=memory.scope();
    scope.kind=kind;auto output=storage.output();output.publication=publication;
    const auto before=allocations;watching=true;
    auto result=view?provider::captureWithInventory(db,sim,train(0),true,scope,*view,
        [&](uintptr_t at,void* out,size_t bytes){return memory.read(at,out,bytes);},budget,storage.scratch(),output,limits):
        provider::capture(db,sim,train(0),true,scope,[&](uintptr_t at,void* out,size_t bytes){return memory.read(at,out,bytes);},
            budget,storage.scratch(),output,limits);
    watching=false;CHECK(allocations==before);CHECK(!memory.readRoster);CHECK(!memory.readAfterRefusal);CHECK(gate.afterRefusal==0);
    if(!result.ready()){CHECK(result.cars==0&&result.history==0&&result.portions==0);CHECK(storage.untouched());
        CHECK(result.observation.history.sections.empty());}
    if(budget.exhausted){const auto calls=gate.calls;CHECK(!budget.available(1));CHECK(calls==gate.calls);}
    return result;
}
Result run(Memory& memory,Storage& storage,provider::Limits limits={}){Gate gate{memory};return capture(memory,storage,gate,limits);}

void basicAndConsist(Memory& memory,Storage& storage){
    memory.reset();auto result=run(memory,storage);
    CHECK(result.ready());CHECK(result.cars==1&&result.members==1&&result.history==1&&result.portions==1);
    CHECK(storage.carsOut[0].meters==20&&storage.carsOut[0].setup.owner==train(0));
    CHECK(result.observation.consist.lengthMeters==20);CHECK(result.observation.history.continuityQualified);
    CHECK(storage.portionsOut[0].from==.75&&storage.portionsOut[0].to<.55);
    memory.reset();memory.aggregate(2,2);memory.trackData(0,0,0,1000);CHECK(run(memory,storage).ready());
    CHECK(storage.carsOut[3].setup.owner==train(1)&&storage.carsOut[3].setup.ordinal()==1);
    // Stale attached child Presence/history is intentionally NOT substituted
    // for the root aggregate. It is not a independently qualified body.
    memory.reset();memory.aggregate(1,2,true,-1);memory.trackData(0,0,0,1000);
    CHECK(run(memory,storage).ready());CHECK(storage.carsOut[0].setup.owner==train(1));
    CHECK(storage.carsOut[0].setup.ordinal()==1&&storage.carsOut[0].setup.settings[12]==1);
    CHECK(storage.carsOut[2].setup.owner==train(0));
    memory.reset();memory.member(0,64);memory.trackData(0,0,0,10000);
    CHECK(run(memory,storage).ready());CHECK(storage.carsOut[63].setup.ordinal()==63);
    memory.reset();memory.aggregate(29,2);memory.trackData(0,0,0,1000);
    memory.vector(motionBlock+8,retained,29,32);memory.write(motionBlock+0x40,float(580));
    result=run(memory,storage);CHECK(result.reason==Reason::MissingAggregateMember);CHECK(result.expectedCars==31);
    memory.reset();memory.aggregate(1,1);memory.write(motionBlock+0x1e0,int64_t(0));
    memory.write(motionBlock+0x638+0x1e0,int64_t(0));CHECK(run(memory,storage).reason==Reason::AmbiguousOrder);
    memory.reset();memory.write(motionBlock+0x80,int32_t(9));CHECK(run(memory,storage).reason==Reason::InvalidConfiguration);
    memory.reset();memory.aggregate(1,1);memory.write(motionBlock+0x638+0x1e0,int64_t(1001));
    CHECK(run(memory,storage).reason==Reason::InvalidOrder);
    memory.reset();memory.member(0,2);memory.write(motionBlock+0x80,int32_t(1));memory.car(0,0,100,1);memory.car(0,1,100,2);
    memory.vector(motionBlock+8,retained,1,32);memory.write(motionBlock+0x40,float(20));CHECK(run(memory,storage).ready());
    memory.reset();memory.member(0,2);memory.vector(motionBlock+8,retained,1,32);CHECK(run(memory,storage).reason==Reason::MissingAggregateMember);
    memory.reset();memory.vector(motionBlock+8,retained,2,32);CHECK(run(memory,storage).reason==Reason::UnexpectedAggregateCar);
    memory.reset();memory.write(retained+8,train(0,2));CHECK(run(memory,storage).reason==Reason::CurrentCompositionLag);
    memory.reset();memory.aggregate(1,1);memory.write(planned+carCapacity*32,uint64_t(101));
    CHECK(run(memory,storage).reason==Reason::CurrentCompositionLag);
    memory.reset();memory.write(trainBlock,train(0,2));CHECK(run(memory,storage).reason==Reason::IdentityChanged);
    memory.reset();memory.aggregate(1,1);memory.write(motionBlock+0x638+0x1d8,train(0,2));
    CHECK(run(memory,storage).reason==Reason::InventoryUnknown);
    memory.reset();memory.member(0,0);CHECK(run(memory,storage).reason==Reason::EmptyConsist);
}
void modelsAndScope(Memory& memory,Storage& storage){
    memory.reset();memory.write(table,uintptr_t(0));CHECK(run(memory,storage).reason==Reason::MissingModel);
    for(const float meters:{0.f,-1.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::denorm_min()}){
        memory.reset();memory.write(models+0xfc,meters);CHECK(run(memory,storage).reason==Reason::InvalidModelLength);
    }
    memory.reset();memory.write(motionBlock+0x40,float(19));CHECK(run(memory,storage).reason==Reason::NativeLengthMismatch);
    memory.reset();memory.write(rules+0x40,uint32_t(0));CHECK(run(memory,storage).reason==Reason::InvalidModel);
    memory.reset();memory.write(db+0x408,uintptr_t(7));CHECK(run(memory,storage).reason==Reason::InvalidAddress);
    memory.reset();memory.write(models,uint64_t(102));memory.write(models+0x2a0,models);
    CHECK(run(memory,storage).reason==Reason::ModelChain);
    memory.reset();memory.denyScope=1;CHECK(run(memory,storage).reason==Reason::UnqualifiedScope);CHECK(memory.reads==0);
    memory.reset();memory.denyScope=2;CHECK(run(memory,storage).reason==Reason::ScopeChanged);
    memory.reset();memory.witness.continuityRevision=0;CHECK(run(memory,storage).reason==Reason::UnqualifiedScope);
    memory.reset();memory.witness.at.phase=rear::Phase::BeforeMovement;CHECK(run(memory,storage).reason==Reason::UnqualifiedScope);
    // Double upper sum preserves exact complete model length despite native
    // float ADDSS rounding down. No epsilon is a physical length certificate.
    memory.reset();memory.member(0,64);memory.model(0,100,.1f);
    float native=0;for(size_t i=0;i<64;++i)native=float(native+.1f);
    memory.write(motionBlock+0x40,native);auto result=run(memory,storage);CHECK(result.ready());
    CHECK(result.observation.consist.lengthMeters>=64*double(.1f));CHECK(result.observation.consist.nativeLengthMeters==native);
}
void historyAndTopology(Memory& memory,Storage& storage){
    memory.reset();memory.model(0,100,185);memory.write(motionBlock+0x40,float(185));
    memory.trackData(0,track(1));memory.trackData(1,track(2),track(0));memory.trackData(2,0,track(1));
    memory.write(history,provider::NativeHistory{track(1),1,0});memory.write(history+16,provider::NativeHistory{track(2),1,0});
    memory.vector(motionBlock+0xe0,history,2,16);auto result=run(memory,storage);CHECK(result.ready());CHECK(result.portions==3);
    CHECK(storage.portionsOut[0].track==track(0)&&storage.portionsOut[1].track==track(1)&&storage.portionsOut[2].track==track(2));
    CHECK(storage.portionsOut[2].to<.9&&storage.portionsOut[2].to>.89);
    memory.reset();memory.model(0,100,842.5f);memory.write(motionBlock+0x40,float(842.5));
    for(size_t i=0;i<85;++i){memory.trackData(i,i==84?0:track(i+1),i?track(i-1):0,10);
        if(i)memory.write(history+(i-1)*16,provider::NativeHistory{track(i),1,0});}
    memory.vector(motionBlock+0xe0,history,84,16);result=run(memory,storage);
    CHECK(result.ready()&&result.portions==85&&result.history==85);CHECK(storage.portionsOut[84].track==track(84));
    CHECK(storage.portionsOut[84].to<.5&&storage.portionsOut[84].to>.49);
    memory.reset();memory.model(0,100,75);memory.write(motionBlock+0x40,float(75));CHECK(run(memory,storage).ready());
    memory.reset();memory.model(0,100,75);memory.write(motionBlock+0x40,float(75));memory.trackData(0,track(1));memory.trackData(1,0,track(0));
    CHECK(run(memory,storage).reason==Reason::RearCoverageUnknown);
    memory.write(history,provider::NativeHistory{track(1),1,0});memory.vector(motionBlock+0xe0,history,1,16);
    result=run(memory,storage);CHECK(result.ready()&&result.portions==2);CHECK(storage.portionsOut[1].from==1&&storage.portionsOut[1].to==1);
    memory.reset();memory.model(0,100,85);memory.write(motionBlock+0x40,float(85));CHECK(run(memory,storage).reason==Reason::RearCoverageUnknown);
    memory.reset();memory.trackData(0,track(1));memory.trackData(1);memory.write(history,provider::NativeHistory{track(1),1,0});
    memory.vector(motionBlock+0xe0,history,1,16);CHECK(run(memory,storage).reason==Reason::MissingTransition);
    memory.reset();memory.write(history,provider::NativeHistory{track(0),0,0});memory.vector(motionBlock+0xe0,history,1,16);
    CHECK(run(memory,storage).reason==Reason::HistoryDirection);
    memory.reset();memory.trackData(0,0,0,100,1);memory.head(.25,-1,2);result=run(memory,storage);
    CHECK(result.ready());CHECK(storage.portionsOut[0].from==.75&&storage.portionsOut[0].orientation==1);
    memory.reset();memory.trackData(0,0,0,100,1);memory.head(.2,-1,2);
    CHECK(run(memory,storage).reason==Reason::ArithmeticUnqualified);
    memory.reset();memory.write(motionBlock+0xc0,double(.5));CHECK(run(memory,storage).reason==Reason::MirrorMismatch);
    memory.reset();memory.head(.75,0);CHECK(run(memory,storage).reason==Reason::InvalidPresence);
    memory.reset();memory.trackData(0,0,0,std::numeric_limits<double>::quiet_NaN());CHECK(run(memory,storage).reason==Reason::InvalidTopology);
    // Branch parent->child uses the retained branch at an interior metric
    // fraction, with reciprocal explicit sorted parent/child metadata.
    memory.reset();memory.model(0,100,60);memory.write(motionBlock+0x40,float(60));
    memory.trackData(1,track(2),0);memory.trackData(2,0,track(1));
    memory.write(trackBlock+0x4e8+0x3f0,track(0));memory.write(trackBlock+0x4e8+0x3f8,double(.25));
    memory.write(trackBlock+0x4e8+0x400,int32_t(-1));memory.write(children,track(1));memory.vector(trackBlock+0x408,children,1,8);
    memory.write(history,provider::NativeHistory{track(1),1,0});memory.vector(motionBlock+0xe0,history,1,16);
    result=run(memory,storage);CHECK(result.ready()&&result.portions==2);CHECK(storage.portionsOut[0].to==.25&&storage.portionsOut[1].from==1);
    memory.mutate(children,children,track(2));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.mutationRead=0;memory.write(children,track(1));
    memory.mutate(history,history,provider::NativeHistory{track(1),-1,0});CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.mutationRead=0;memory.write(history,provider::NativeHistory{track(1),1,0});
    memory.write(children,track(2));CHECK(run(memory,storage).reason==Reason::MissingTransition);
}
void mutationsAndBudgets(Memory& memory,Storage& storage){
    memory.reset();memory.mutate(planned,planned,uint64_t(101));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();memory.mutate(retained,retained+8,train(0,2));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();memory.mutate(models+0xfc,models+0xfc,float(21));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();memory.mutate(trackBlock+0x2c,trackBlock+0x2c,int8_t(1));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();memory.mutate(trackBlock,trackBlock+8,track(1),3);CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();memory.mutate(motionBlock+0x3a0,motionBlock+0x3a0+16,double(.7));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();memory.mutate(db+0x408,db+0x408,uintptr_t(0x60000000));CHECK(run(memory,storage).reason==Reason::ValuesChanged);
    memory.reset();Gate baselineGate{memory};const auto baseline=capture(memory,storage,baselineGate);CHECK(baseline.ready());
    const auto reads=memory.reads,bytes=memory.bytesRead,preflights=baselineGate.calls;
    for(size_t deny=1;deny<=reads;++deny){
        memory.reset();memory.denyRead=deny;auto result=run(memory,storage);CHECK(!result.ready());CHECK(memory.reads==deny);
    }
    for(size_t deny=1;deny<=preflights;++deny){
        memory.reset();Gate gate{memory};gate.deny=deny;auto result=capture(memory,storage,gate);
        CHECK(!result.ready());CHECK(gate.calls==deny);CHECK(gate.refused);
    }
    memory.reset();memory.throwRead=reads-2;CHECK(run(memory,storage).reason==Reason::ReaderException);
    memory.reset();provider::Limits limits;limits.reads=reads;CHECK(run(memory,storage,limits).ready());
    memory.reset();limits.reads=reads-1;CHECK(run(memory,storage,limits).reason==Reason::ReadLimit);
    memory.reset();limits={};limits.bytes=bytes;CHECK(run(memory,storage,limits).ready());
    memory.reset();limits.bytes=bytes-1;CHECK(run(memory,storage,limits).reason==Reason::ByteLimit);
    memory.reset();limits={};limits.work=baseline.work;CHECK(run(memory,storage,limits).ready());
    memory.reset();limits.work=baseline.work-1;CHECK(run(memory,storage,limits).reason==Reason::WorkLimit);
    memory.reset();limits.work=1;auto refusedWork=run(memory,storage,limits);CHECK(refusedWork.reason==Reason::WorkLimit);
    CHECK(memory.reads<20);
    memory.reset();limits={};limits.cars=0;CHECK(run(memory,storage,limits).reason==Reason::InvalidStorage);
    memory.reset();memory.aggregate(1,1);limits={};limits.members=1;CHECK(run(memory,storage,limits).reason==Reason::MemberCapacity);
    memory.reset();memory.member(0,2);limits={};limits.cars=1;CHECK(run(memory,storage,limits).reason==Reason::InvalidVector);
    std::printf("baseline: reads=%zu bytes=%zu preflights=%zu work=%zu\n",reads,bytes,preflights,baseline.work);
}
void reusedInventory(Memory& memory,Storage& storage){
    memory.reset();std::array<inventory::Row,16> rows{},temporary{};std::array<uint64_t,512> ids{};
    Gate gate{memory};inventory::Budget budget{Gate::preflight,&gate};
    auto roster=inventory::captureMotionInventory(sim,true,[&](uintptr_t at,void* out,size_t bytes){return memory.read(at,out,bytes);},
        budget,rows,{temporary,ids});CHECK(roster.observedAllSlots());
    provider::InventoryView view{{rows.data(),roster.rows},roster,memory.witness};
    const auto inventoryReads=memory.reads;auto result=capture(memory,storage,gate,{},&view);CHECK(result.ready());
    CHECK(memory.reads-inventoryReads<100);CHECK(result.reads==memory.reads-inventoryReads);
    view.witness.at.tick++;CHECK(capture(memory,storage,gate,{},&view).reason==Reason::InventoryUnknown);
    view.witness=memory.witness;view.result.rows++;CHECK(capture(memory,storage,gate,{},&view).reason==Reason::InventoryUnknown);
    view.result.rows=roster.rows;view.rows={storage.inventoryRows.data(),roster.rows};
    CHECK(capture(memory,storage,gate,{},&view).reason==Reason::OverlappingStorage);
    // Indexed relationships are produced once, not reconstructed by N-sized
    // scans for every root. Still reject stale identities and truncated/cyclic
    // chains before any potentially optimistic body is returned.
    memory.reset();memory.aggregate(1,2);memory.trackData(0,0,0,1000);
    std::array<inventory::Row,16> coupledRows{},coupledTemporary{};
    Gate coupledGate{memory};inventory::Budget coupledBudget{Gate::preflight,&coupledGate};
    const auto coupled=inventory::captureMotionInventory(sim,true,
        [&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);},coupledBudget,
        coupledRows,{coupledTemporary,ids});
    CHECK(coupled.observedAllSlots()&&coupled.relationshipsIndexed);
    provider::InventoryView linked{{coupledRows.data(),coupled.rows},coupled,memory.witness};
    CHECK(capture(memory,storage,coupledGate,{},&linked).ready());
    const auto original=coupledRows;
    for(unsigned variation=0;variation<7;++variation){
        coupledRows=original;linked.result=coupled;
        if(variation==0)linked.result.relationshipsIndexed=false;
        if(variation==1)coupledRows[0].firstChild=inventory::noRow;
        if(variation==2)coupledRows[0].firstChild=16;
        if(variation==3)coupledRows[1].nextSibling=1;
        if(variation==4)coupledRows[1].parent=train(0,2);
        if(variation==5)coupledRows[1].parentRow=1;
        if(variation==6)coupledRows[0].childCount=2;
        CHECK(capture(memory,storage,coupledGate,{},&linked).reason==Reason::InventoryUnknown);
    }
    coupledRows=original;linked.result=coupled;coupledRows[0].train=train(0,2);
    CHECK(capture(memory,storage,coupledGate,{},&linked).reason==Reason::IdentityChanged);
    coupledRows=original;linked.result=coupled;coupledRows[1].train=train(1,2);
    CHECK(capture(memory,storage,coupledGate,{},&linked).reason==Reason::IdentityChanged);
    coupledRows=original;linked.result=coupled;
    provider::Limits onlyRoot;onlyRoot.members=1;
    CHECK(capture(memory,storage,coupledGate,onlyRoot,&linked).reason==Reason::MemberCapacity);
    // The existing owned scope is synthetic. It does not make an unordered
    // roster or a missing root positive: strict ordinal order is prerequisite.
    std::swap(coupledRows[0],coupledRows[1]);
    CHECK(!capture(memory,storage,coupledGate,{},&linked).ready());
    // 511 unrelated vanilla slots do not add a linear per-root scan. The
    // exhaustive producer still observes them; this particular body lookup
    // performs O(log N + members) work against its immutable owned index.
    memory.reset();for(size_t n=1;n<512;++n)memory.write(motionBlock+n*0x638,train(n));
    std::array<inventory::Row,512> denseRows{},denseTemporary{};
    Gate denseGate{memory};inventory::Budget denseBudget{Gate::preflight,&denseGate};
    const auto dense=inventory::captureMotionInventory(sim,true,
        [&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);},
        denseBudget,denseRows,{denseTemporary,ids});CHECK(dense.relationshipsIndexed&&dense.rows==512);
    provider::InventoryView denseView{{denseRows.data(),dense.rows},dense,memory.witness};
    const auto indexed=capture(memory,storage,denseGate,{},&denseView);
    CHECK(indexed.ready());CHECK(indexed.work<128);CHECK(indexed.reads<100);
}
void currentAbsence(Memory& memory,Storage& storage){
    const auto inactive=[&]{
        memory.reset();memory.write(motionBlock+0x1d0,uint8_t(0));
        // Teardown does not clear these descriptors. Even copying history's
        // descriptor would violate the inactive optional's native lifetime.
        memory.write(motionBlock+0xe0,provider::detail::Vector{1,3,5});
        memory.write(motionBlock+8,provider::detail::Vector{1,3,5});
        memory.write(trainBlock+0xc0,provider::detail::Vector{1,3,5});
        memory.write(motionBlock+0xb8,uint64_t(7));
        memory.write(motionBlock+0x3a0,nimby::windows::automatic::CommitPresence{});
        memory.write(db+0x408,uintptr_t(7));
    };
    const auto captured=[&](Gate& gate,provider::Limits limits={},const provider::InventoryView* view=nullptr,
        provider::PublicationProof* publication=nullptr){
        return capture(memory,storage,gate,limits,view,provider::Kind::CurrentPhysicalSnapshot,publication);
    };
    // Running/scheduled/settings can request respawn in a later native step;
    // they never turn absent Dynamics into a present body at this publication.
    for(uint8_t running:{uint8_t(0),uint8_t(1)})for(uint8_t scheduled:{uint8_t(0),uint8_t(1)})
        for(uint8_t reversed:{uint8_t(0),uint8_t(1)})for(int32_t mode:{0,1,8,9}){
            inactive();memory.write(motionBlock+0x5d0,running);memory.write(motionBlock+0x5f0,scheduled);
            memory.write(motionBlock+0x1e8,reversed);memory.write(motionBlock+0x80,mode);
            // Native unhitch can leave parent/order values in a nonattached row.
            memory.write(motionBlock+0x1d8,train(7,2));memory.write(motionBlock+0x1e0,int64_t(1001));
            Gate gate{memory};provider::PublicationProof publication;auto result=captured(gate,{},nullptr,&publication);
            CHECK(result.ready()&&result.reason==Reason::CurrentBodyAbsent);
            CHECK(result.cars==0&&result.history==0&&result.portions==0&&storage.untouched());
            CHECK(!memory.readHistoryDescriptor);
            CHECK(publication.physicallyAbsent&&publication.currentPhysicalQualified&&!publication.progressionQualified);
            CHECK(publication.kind==provider::Kind::CurrentPhysicalSnapshot&&publication.at==memory.witness.at);
            CHECK(result.observation.history.sections.empty()&&!result.observation.stablePublicationQualified);
        }
    memory.reset();memory.write(motionBlock+0x5d0,uint8_t(0));memory.write(motionBlock+0x5f0,uint8_t(0));
    Gate stopped{memory};auto present=captured(stopped);CHECK(present.ready());
    CHECK(!present.publication.physicallyAbsent&&present.cars==1&&present.portions==1);
    // Attached inactive children remain represented by the active root's full
    // aggregate; they must not disappear when their own Dynamics is torn down.
    memory.reset();memory.aggregate(1,2);memory.trackData(0,0,0,1000);
    Gate coupled{memory};present=captured(coupled);CHECK(present.ready());
    CHECK(!present.publication.physicallyAbsent&&present.cars==3&&present.members==2);
    CHECK(storage.carsOut[2].setup.owner==train(1));
    // An inactive aggregate root does not prove that every attached member's
    // Dynamics is absent. Otherwise an all-root occupancy collector could
    // omit a live child when it skips attached rows as independent roots.
    memory.reset();memory.aggregate(1,2);memory.member(2,1,train(0),2);
    memory.write(motionBlock+0x1d0,uint8_t(0));
    Gate absentAggregate{memory};present=captured(absentAggregate);
    CHECK(present.ready()&&present.publication.physicallyAbsent);
    CHECK(!memory.readHistoryDescriptor&&storage.untouched());
    memory.reset();memory.aggregate(1,2);memory.member(2,1,train(0),2);
    memory.write(motionBlock+0x1d0,uint8_t(0));
    memory.write(motionBlock+0x638+0x1d0,uint8_t(1));
    Gate activeChild{memory};present=captured(activeChild);
    CHECK(!present.ready()&&!present.publication.currentPhysicalQualified);
    CHECK(present.reason==Reason::InactiveRootWithActiveMember&&present.member==train(1));
    CHECK(!memory.readHistoryDescriptor&&storage.untouched());
    // The first child (row2) was absent; the later row1 still forbids clear.
    // A stopped root's live Dynamics may carry a long physical aggregate even
    // though its attached child's own body is unqualified and history destroyed.
    // Those optional descriptors are not fallback physical bodies. This
    // is a sealed capture fixture; native aggregation/hash gateways are tested
    // independently in train_length_detours.cpp.
    const auto tornDownChild=[&]{
        const auto child=motionBlock+0x638;
        memory.write(child+8,provider::detail::Vector{1,3,5});
        memory.write(child+0xe0,provider::detail::Vector{1,3,5});
        memory.write(child+0x40,std::numeric_limits<float>::quiet_NaN());
        memory.write(child+0xb8,uint64_t(7));
        memory.write(child+0x3a0,nimby::windows::automatic::CommitPresence{});
    };
    const auto longAggregate=[&](size_t count,bool reversed=false,int64_t order=1){
        memory.reset();memory.aggregate(1,count-1,reversed,order);memory.trackData(0,0,0,1000000);
        tornDownChild();
    };
    for(const size_t count:{size_t(31),size_t(80),size_t(512),carCapacity})
        for(const bool reversed:{false,true})for(const int64_t order:{int64_t(-1),int64_t(1)}){
            longAggregate(count,reversed,order);
            const auto child=motionBlock+0x638;
            CHECK(memory.value<uint8_t>(motionBlock+0x1d0)==1);
            CHECK(memory.value<uint8_t>(child+0x1d0)==0);
            Gate longCoupled{memory};present=captured(longCoupled);
            if(!present.ready())std::fprintf(stderr,"long stopped aggregate: count=%zu reversed=%d order=%lld reason=%s member=%llx\n",
                count,int(reversed),static_cast<long long>(order),provider::reasonName(present.reason),static_cast<unsigned long long>(present.member));
            CHECK(present.ready()&&!present.publication.physicallyAbsent);
            CHECK(present.publication.currentPhysicalQualified&&!present.publication.progressionQualified);
            CHECK(present.cars==count&&present.members==2&&present.portions==1);
            CHECK(!memory.readInactiveChildOptional);
            CHECK(present.observation.history.sections.empty()&&!present.observation.stablePublicationQualified);
            float nativeLength=0;double meters=0;
            for(size_t i=0;i<count;++i){
                nativeLength=float(nativeLength+storage.carsOut[i].nativeMeters);meters+=storage.carsOut[i].meters;
                const auto rootCar=storage.carsOut[i].setup.owner==train(0);
                CHECK(storage.carsOut[i].setup.owner==(rootCar?train(0):train(1)));
                CHECK(storage.carsOut[i].nativeMeters==20&&storage.carsOut[i].meters==20);
                const size_t childOrdinal=i-(order>0?1:0);
                CHECK(storage.carsOut[i].setup.ordinal()==(rootCar?0:(reversed?count-2-childOrdinal:childOrdinal)));
                CHECK(storage.carsOut[i].setup.settings[12]==uint8_t(!rootCar&&reversed));
            }
            CHECK(nativeLength==memory.value<float>(motionBlock+0x40)&&meters==double(count)*20);
            const auto covered=(storage.portionsOut[0].from-storage.portionsOut[0].to)*1000000;
            CHECK(covered>=meters&&covered-meters<1e-6);
            const size_t rootIndex=order<0?count-1:0,firstChild=order<0?0:1;
            CHECK(storage.carsOut[rootIndex].setup.owner==train(0));
            CHECK(storage.carsOut[firstChild].setup.owner==train(1));
            CHECK(storage.carsOut[firstChild].setup.ordinal()==(reversed?count-2:0));
            CHECK(storage.carsOut[firstChild].setup.settings[12]==uint8_t(reversed));
        }
    // Neither planned mutations, stale relationships, partial root tuples nor
    // a mismatching live child can be promoted by the teardown exception.
    longAggregate(80);memory.write(planned+carCapacity*32,uint64_t(101));Gate plannedLag{memory};
    CHECK(captured(plannedLag).reason==Reason::CurrentCompositionLag);CHECK(!memory.readInactiveChildOptional);
    longAggregate(80);memory.mutate(planned+carCapacity*32,planned+carCapacity*32,uint64_t(101));Gate plannedChanged{memory};
    CHECK(captured(plannedChanged).reason==Reason::ValuesChanged);CHECK(!memory.readInactiveChildOptional);
    for(const auto [offset,value]:std::array<std::pair<size_t,uint64_t>,2>{{{0x1d8,train(0,2)},{0,train(1,2)}}}){
        const auto trigger=offset?0x1d0:0;
        longAggregate(80);memory.mutate(motionBlock+0x638+trigger,motionBlock+0x638+offset,value,4);Gate relationshipChanged{memory};
        CHECK(!captured(relationshipChanged).ready());CHECK(!memory.readInactiveChildOptional);
    }
    longAggregate(80);memory.mutate(motionBlock+0x638+0x1d0,motionBlock+0x638+0x1d0,uint8_t(1),4);Gate liveTransition{memory};
    CHECK(captured(liveTransition).reason==Reason::ValuesChanged);CHECK(!memory.readInactiveChildOptional);
    longAggregate(80);memory.vector(motionBlock+8,retained,79,32);Gate partialRoot{memory};
    CHECK(captured(partialRoot).reason==Reason::MissingAggregateMember);
    longAggregate(80);memory.write(motionBlock+0x40,float(1599));Gate wrongLength{memory};
    CHECK(captured(wrongLength).reason==Reason::NativeLengthMismatch);
    memory.reset();memory.aggregate(1,79);memory.trackData(0,0,0,1000000);
    memory.write(motionBlock+0x638+0x1d0,uint8_t(1));memory.write(retained+carCapacity*32,uint64_t(101));Gate liveMismatch{memory};
    CHECK(captured(liveMismatch).reason==Reason::CurrentCompositionLag);
    longAggregate(80);Gate progression{memory};
    CHECK(capture(memory,storage,progression).reason==Reason::InvalidVector);CHECK(memory.readInactiveChildOptional);
    // Every failed read remains terminal and leaves all output POD untouched,
    // including failures during final planned/root/model revalidation.
    longAggregate(80);Gate base{memory};CHECK(captured(base).ready());const auto readCount=memory.reads;
    for(size_t failure=1;failure<=readCount;++failure){
        longAggregate(80);memory.denyRead=failure;Gate failedRead{memory};
        CHECK(!captured(failedRead).ready());CHECK(memory.reads==failure);CHECK(!memory.readInactiveChildOptional);
    }
    // Current-only absence does not extend the legacy progression contract.
    memory.reset();memory.write(motionBlock+0x1d0,uint8_t(0));CHECK(run(memory,storage).reason==Reason::InvalidPresence);
    inactive();memory.write(motionBlock+0x1f0,uint8_t(1));memory.write(motionBlock+0x1d8,train(1));memory.member(1,1);
    Gate attached{memory};CHECK(captured(attached).reason==Reason::AttachedRoot);CHECK(!memory.readHistoryDescriptor);
    inactive();memory.denyScope=1;Gate unowned{memory};CHECK(captured(unowned).reason==Reason::UnqualifiedScope);CHECK(!memory.reads);
    inactive();memory.denyScope=2;Gate lost{memory};CHECK(captured(lost).reason==Reason::ScopeChanged);CHECK(storage.untouched());
    inactive();memory.mutate(motionBlock+0x1d0,motionBlock+0x1d0,uint8_t(1),4);
    Gate changed{memory};CHECK(captured(changed).reason==Reason::ValuesChanged);CHECK(!memory.readHistoryDescriptor);
    inactive();memory.mutate(motionBlock,motionBlock,train(0,2),4);
    Gate recycled{memory};CHECK(captured(recycled).reason==Reason::IdentityChanged);CHECK(!memory.readHistoryDescriptor);
    // Reused immutable inventory takes only bounded root checks; stale data or
    // lost ownership still cannot publish absence, nor stale output metadata.
    inactive();std::array<inventory::Row,16> rows{},temporary{};std::array<uint64_t,512> ids{};
    Gate inventoryGate{memory};inventory::Budget budget{Gate::preflight,&inventoryGate};
    const auto roster=inventory::captureMotionInventory(sim,true,
        [&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);},budget,rows,{temporary,ids});
    CHECK(roster.observedAllSlots());provider::InventoryView view{{rows.data(),roster.rows},roster,memory.witness};
    const auto beforeReads=memory.reads,beforeBytes=memory.bytesRead,beforePreflights=inventoryGate.calls;
    provider::PublicationProof publication;
    auto result=captured(inventoryGate,{},&view,&publication);CHECK(result.ready()&&publication.physicallyAbsent);
    const auto reads=memory.reads-beforeReads,bytes=memory.bytesRead-beforeBytes,preflights=inventoryGate.calls-beforePreflights;
    CHECK(reads<=13&&bytes<=400);CHECK(!memory.readHistoryDescriptor&&storage.untouched());
    const auto sentinel=publication;
    for(size_t failure=1;failure<=reads;++failure){
        inactive();Gate gate{memory};memory.denyRead=failure;publication=sentinel;
        CHECK(!captured(gate,{},&view,&publication).ready());CHECK(memory.reads==failure);
        CHECK(publication.physicallyAbsent==sentinel.physicallyAbsent&&publication.at==sentinel.at);
    }
    for(size_t failure=1;failure<=reads;++failure){
        inactive();Gate gate{memory};memory.throwRead=failure;
        CHECK(captured(gate,{},&view).reason==Reason::ReaderException);CHECK(memory.reads==failure);
    }
    for(size_t failure=1;failure<=preflights;++failure){
        inactive();Gate gate{memory};gate.deny=failure;
        CHECK(!captured(gate,{},&view).ready());CHECK(gate.calls==failure&&gate.refused);
        CHECK(!memory.readHistoryDescriptor);
    }
    inactive();Gate exactReads{memory};provider::Limits limits;limits.reads=reads;
    CHECK(captured(exactReads,limits,&view).ready());
    inactive();Gate shortReads{memory};limits.reads=reads-1;CHECK(captured(shortReads,limits,&view).reason==Reason::ReadLimit);
    inactive();Gate exactBytes{memory};limits={};limits.bytes=bytes;CHECK(captured(exactBytes,limits,&view).ready());
    inactive();Gate shortBytes{memory};limits.bytes=bytes-1;CHECK(captured(shortBytes,limits,&view).reason==Reason::ByteLimit);
    inactive();memory.write(motionBlock+0x1d0,uint8_t(1));Gate stale{memory};
    CHECK(captured(stale,{},&view).reason==Reason::ValuesChanged);CHECK(!memory.readHistoryDescriptor);
    inactive();view.witness.at.tick++;Gate wrongPublication{memory};CHECK(captured(wrongPublication,{},&view).reason==Reason::InventoryUnknown);
}
}
int main(){
    fixture::Memory memory;const auto ownedStorage=std::make_unique<fixture::Storage>();auto& storage=*ownedStorage;
    fixture::basicAndConsist(memory,storage);fixture::modelsAndScope(memory,storage);
    fixture::historyAndTopology(memory,storage);fixture::mutationsAndBudgets(memory,storage);fixture::reusedInventory(memory,storage);
    fixture::currentAbsence(memory,storage);
    std::printf("native_consist_rear_provider: %zu checks passed; row=%zu car=%zu memberScratch=%zu storage=%zu\n",
        fixture::checks,sizeof(fixture::inventory::Row),sizeof(fixture::provider::PhysicalCar),sizeof(fixture::provider::detail::Member),sizeof(fixture::Storage));
}
