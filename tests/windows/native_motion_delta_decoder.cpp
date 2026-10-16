// Synthetic tests of the bounded decoder. They install no native hooks and
// cannot establish packet ownership, finished movement or rear release in game.
#include <platform/windows/runtime/native_motion_delta_decoder.h>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::atomic<bool> watchAllocations{};
std::atomic<size_t> allocations{};
size_t checks{};
}
#if defined(__GNUC__)
#define DELTA_NOINLINE __attribute__((noinline))
#else
#define DELTA_NOINLINE __declspec(noinline)
#endif
DELTA_NOINLINE void* operator new(size_t bytes){
    if(watchAllocations.load())++allocations;
    if(auto* p=std::malloc(bytes?bytes:1))return p;
    throw std::bad_alloc();
}
DELTA_NOINLINE void* operator new[](size_t bytes){return ::operator new(bytes);}
DELTA_NOINLINE void operator delete(void* p)noexcept{std::free(p);}
DELTA_NOINLINE void operator delete[](void* p)noexcept{std::free(p);}
DELTA_NOINLINE void operator delete(void* p,size_t)noexcept{std::free(p);}
DELTA_NOINLINE void operator delete[](void* p,size_t)noexcept{std::free(p);}
#define CHECK(x) do {++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);} while(false)

namespace {
using namespace nimby::windows::motion_delta;
constexpr uint64_t train(uint32_t slot,uint16_t generation=1){
    return (uint64_t{5}<<48)|(uint64_t{slot}<<16)|generation;
}
struct Control {
    Witness witness;
    bool alive=true,allow=true;
    size_t preflights{},readCalls{},failedRead=SIZE_MAX,throwRead=SIZE_MAX;
    size_t changeScopeRead=SIZE_MAX,changeHeaderRead=SIZE_MAX,refusePreflight=SIZE_MAX;
    static bool current(void* opaque,const Witness& w)noexcept {
        const auto& c=*static_cast<Control*>(opaque);return c.alive&&w==c.witness;
    }
    static bool preflight(void* opaque,size_t)noexcept {
        auto& c=*static_cast<Control*>(opaque);
        return c.allow&&++c.preflights!=c.refusePreflight;
    }
};
struct Fixture {
    Header header;
    std::vector<uint64_t> motions,statistics,records178,records48;
    std::array<AffectedTrain,128> output{},scratch{};
    Control control;
    explicit Fixture(size_t motionCount=0,size_t statsCount=0,size_t n178=0,size_t n48=0):
        motions(motionCount*motionStride/8),statistics(statsCount*statisticsStride/8),
        records178(n178*record178Stride/8),records48(n48*record48Stride/8){
        header.unknown=91;header.sequence=234;
        const auto vector=[](const std::vector<uint64_t>& v){
            if(v.empty())return Triplet{};
            const auto begin=reinterpret_cast<uintptr_t>(v.data());
            return Triplet{begin,begin+v.size()*8,begin+v.size()*8};
        };
        header.motions=vector(motions);header.statistics=vector(statistics);
        header.records178=vector(records178);header.records48=vector(records48);
        for(size_t n=0;n<motionCount;++n)motions[n*motionStride/8]=train(static_cast<uint32_t>(n));
        for(size_t n=0;n<statsCount;++n)statistics[n*statisticsStride/8]=train(static_cast<uint32_t>(n));
        control.witness={0x100000,0x200000,reinterpret_cast<uintptr_t>(&header),7,11,13,
            callerReturn,Boundary::BeforeMerge,true,true};
        output.fill({train(999),999,FullMotion});
    }
    Scope scope(){return {control.witness,Control::current,&control};}
    Budget budget(){return {Control::preflight,&control};}
    bool read(uintptr_t address,void* destination,size_t bytes){
        const auto call=++control.readCalls;
        if(call==control.failedRead)return false;
        if(call==control.throwRead)throw 7;
        if(call==control.changeScopeRead)control.alive=false;
        if(call==control.changeHeaderRead)++header.sequence;
        const auto included=[&](const void* base,size_t size){
            const auto first=reinterpret_cast<uintptr_t>(base);
            return address>=first&&address-first<=size&&bytes<=size-(address-first);
        };
        bool valid=included(&header,sizeof header);
        for(const auto* v:{&motions,&statistics,&records178,&records48})
            if(!v->empty()&&included(v->data(),v->size()*8))valid=true;
        if(!valid)return false;
        std::memcpy(destination,reinterpret_cast<const void*>(address),bytes);return true;
    }
    Result run(Limits limits={}){
        auto b=budget();return capture(scope(),true,[&](auto a,auto p,auto n){return read(a,p,n);},
            b,output,scratch,limits);
    }
};

void identityAndEffects(){
    Fixture f(4,3,2,1);
    const std::array<uint64_t,4> ids{{train(6,9),train(0,0),train(6,2),train(6,9)}};
    for(size_t n=0;n<ids.size();++n)f.motions[n*motionStride/8]=ids[n];
    f.statistics[0]=train(6,9);f.statistics[statisticsStride/8]=train(4);
    f.statistics[2*statisticsStride/8]=train(6,2);
    const auto r=f.run();
    CHECK(r.ready()&&r.trains==4&&r.motionRows==4&&r.statisticsRows==3);
    CHECK(r.record178Rows==2&&r.record48Rows==1&&r.unresolvedIndirectEffects);
    CHECK(r.witness==f.control.witness&&r.sequence==234&&r.reads==9);
    CHECK(f.output[0]==(AffectedTrain{train(0,0),0,FullMotion}));
    CHECK(f.output[1]==(AffectedTrain{train(4),4,Statistics}));
    CHECK(f.output[2]==(AffectedTrain{train(6,2),6,FullMotion|Statistics}));
    CHECK(f.output[3]==(AffectedTrain{train(6,9),6,FullMotion|Statistics}));
    CHECK(f.output[2].slotGenerationMayBeReplaced()&&!f.output[1].slotGenerationMayBeReplaced());
    CHECK(!r.localStepProvenance&&!r.exclusiveWorldOwnership&&!r.nativeMergeCompleted&&
        !r.progressionQualified&&!r.rearClearQualified);
    Fixture empty;const auto marker=empty.output;const auto e=empty.run();
    CHECK(e.ready()&&!e.trains&&e.reads==2&&!e.unresolvedIndirectEffects&&empty.output==marker);
    Fixture ancillary(0,0,1,1);const auto a=ancillary.run();
    CHECK(a.ready()&&!a.trains&&a.unresolvedIndirectEffects&&a.reads==2);
}

void malformedInputs(){
    for(unsigned mode=0;mode<11;++mode){
        Fixture f(2,1);const auto marker=f.output;Reason expected=Reason::InvalidTriplet;
        switch(mode){
        case 0:f.header.motions.begin=0;break;
        case 1:f.header.motions.end=f.header.motions.begin-8;break;
        case 2:f.header.motions.capacity=f.header.motions.end-8;break;
        case 3:++f.header.motions.end;break;
        case 4:f.header.motions.begin+=1;break;
        case 5:f.header.motions.capacity+=8;break;
        case 6:f.header.motions={0xfffffffffffffff8ULL,0xfffffffffffffff8ULL,0xfffffffffffffff8ULL};break;
        case 7:f.motions[0]=(uint64_t{8}<<48)|1;expected=Reason::InvalidTrainId;break;
        case 8:f.header.statistics={f.header.motions.begin,f.header.motions.begin+statisticsStride,
            f.header.motions.begin+statisticsStride};expected=Reason::InputAllocationsOverlap;break;
        case 9:{const auto h=reinterpret_cast<uintptr_t>(&f.header);
            f.header.motions={h,h+motionStride,h+motionStride};expected=Reason::InputAllocationsOverlap;break;}
        case 10:f.header.motions={0x200000,0x200000+motionStride,0x200000+motionStride};expected=Reason::Unreadable;break;
        }
        const auto r=f.run();
        if(r.reason!=expected)std::cerr<<"malformed mode="<<mode<<" actual="<<unsigned(r.reason)<<" expected="<<unsigned(expected)<<'\n';
        CHECK(r.reason==expected&&!r.trains&&f.output==marker);
    }
    Fixture f(3,2);const auto marker=f.output;auto b=f.budget();
    auto reader=[&](auto a,auto p,auto n){return f.read(a,p,n);};
    const auto bad=capture(f.scope(),true,reader,b,f.output,f.output);
    CHECK(bad.reason==Reason::InvalidStorage&&!f.control.readCalls&&f.output==marker);
    b=f.budget();const auto small=capture(f.scope(),true,reader,b,
        std::span<AffectedTrain>(f.output).first(1),f.scratch);
    CHECK(small.reason==Reason::OutputCapacity&&f.output==marker);
    b=f.budget();const auto scratch=capture(f.scope(),true,reader,b,f.output,
        std::span<AffectedTrain>(f.scratch).first(1));
    CHECK(scratch.reason==Reason::InputRowCapacity&&f.output==marker);
    b=f.budget();f.header.motions={reinterpret_cast<uintptr_t>(f.output.data()),
        reinterpret_cast<uintptr_t>(f.output.data())+motionStride,
        reinterpret_cast<uintptr_t>(f.output.data())+motionStride};
    const auto overlap=capture(f.scope(),true,reader,b,f.output,f.scratch);
    CHECK(overlap.reason==Reason::StorageOverlapsInput&&f.output==marker);
}

void scopeAndFailures(){
    for(unsigned mode=0;mode<14;++mode){
        Fixture f(2,1);const auto marker=f.output;Reason expected=Reason::UnqualifiedScope;
        Scope scope=f.scope();bool profile=true;
        switch(mode){
        case 0:profile=false;expected=Reason::UnqualifiedProfile;break;
        case 1:scope.current=nullptr;break;
        case 2:scope.witness.originalNotStarted=false;break;
        case 3:scope.witness.detachedInputOwned=false;break;
        case 4:scope.witness.returnOffset=0x34986c;break; // CALL is not return.
        case 5:scope.witness.thread=0;break;
        case 6:scope.witness.world=0;break;
        case 7:scope.witness.serial=0;break;
        case 8:f.control.alive=false;break;
        case 9:f.control.failedRead=2;expected=Reason::Unreadable;break;
        case 10:f.control.throwRead=2;expected=Reason::ReaderException;break;
        case 11:f.control.changeScopeRead=2;expected=Reason::ScopeChanged;break;
        case 12:f.control.changeHeaderRead=2;expected=Reason::HeaderChanged;break;
        case 13:f.control.refusePreflight=2;expected=Reason::PreflightRefused;break;
        }
        auto b=f.budget();const auto r=capture(scope,profile,
            [&](auto a,auto p,auto n){return f.read(a,p,n);},b,f.output,f.scratch);
        CHECK(r.reason==expected&&!r.trains&&f.output==marker);
        if(mode<9)CHECK(f.control.readCalls==0);
    }
    Fixture f(1);const auto marker=f.output;auto b=f.budget();b.preflight=nullptr;
    const auto missing=capture(f.scope(),true,[&](auto a,auto p,auto n){return f.read(a,p,n);},
        b,f.output,f.scratch);
    CHECK(missing.reason==Reason::InvalidBudget&&!f.control.readCalls&&f.output==marker);
}

void quotas(){
    for(unsigned mode=0;mode<7;++mode){
        Fixture f(mode==3?20:3,mode==3?20:2);Limits l;Reason expected{};
        switch(mode){
        case 0:l.vectorRows=2;expected=Reason::VectorCapacity;break;
        case 1:l.inputRows=4;expected=Reason::InputRowCapacity;break;
        case 2:l.reads=2;expected=Reason::ReadLimit;break;
        case 3:l.bytes=2*sizeof(Header);expected=Reason::ByteLimit;break;
        case 4:l.work=2;expected=Reason::WorkLimit;break;
        case 5:l.vectorAllocationBytes=32;expected=Reason::VectorCapacity;break;
        case 6:l.work=0;expected=Reason::InvalidLimits;break;
        }
        const auto ownMarker=f.output;auto b=f.budget();
        const auto r=capture(f.scope(),true,[&](auto a,auto p,auto n){return f.read(a,p,n);},
            b,f.output,f.scratch,l);
        CHECK(r.reason==expected&&!r.trains&&f.output==ownMarker);
        CHECK(r.reads<=l.reads&&r.bytes<=l.bytes&&r.work<=l.work);
        if(mode==2||mode==3||mode==4)CHECK(b.exhausted&&!b.available());
    }
}

void randomizedUnionAndCeiling(){
    uint32_t random=0x42c0ffee;
    const auto next=[&]{random^=random<<13;random^=random>>17;random^=random<<5;return random;};
    for(size_t repeat=0;repeat<160;++repeat){
        const size_t motions=next()%63,stats=next()%63;
        Fixture f(motions,stats);std::map<uint64_t,uint8_t> expected;
        for(size_t n=0;n<motions;++n){const auto id=train(next()%31,static_cast<uint16_t>(next()%7));
            f.motions[n*motionStride/8]=id;expected[id]|=FullMotion;}
        for(size_t n=0;n<stats;++n){const auto id=train(next()%31,static_cast<uint16_t>(next()%7));
            f.statistics[n*statisticsStride/8]=id;expected[id]|=Statistics;}
        const auto before=allocations.load();watchAllocations=true;const auto r=f.run();watchAllocations=false;
        CHECK(r.ready()&&r.trains==expected.size()&&allocations.load()==before);
        size_t n=0;for(const auto& [id,effects]:expected){
            CHECK(f.output[n].fullId==id&&f.output[n].slot==uint32_t(id>>16)&&f.output[n].effects==effects);++n;}
    }
    Fixture f(4096,4096);std::vector<AffectedTrain> output(8192),scratch(8192);
    for(size_t n=0;n<4096;++n){
        f.motions[n*motionStride/8]=train(static_cast<uint32_t>(4095-n),1);
        f.statistics[n*statisticsStride/8]=train(static_cast<uint32_t>(4095-n),2);
    }
    auto b=f.budget();const auto before=allocations.load();watchAllocations=true;
    const auto r=capture(f.scope(),true,[&](auto a,auto p,auto n){return f.read(a,p,n);},b,output,scratch);
    watchAllocations=false;
    CHECK(r.ready()&&r.trains==8192&&r.reads==8194&&r.work<=262144&&r.bytes<=256u*1024u);
    CHECK(allocations.load()==before);
    for(size_t n=0;n<4096;++n){
        CHECK(output[n*2].fullId==train(static_cast<uint32_t>(n),1)&&output[n*2].effects==FullMotion);
        CHECK(output[n*2+1].fullId==train(static_cast<uint32_t>(n),2)&&output[n*2+1].effects==Statistics);
    }
}
}
int main(){
    try {identityAndEffects();malformedInputs();scopeAndFailures();quotas();randomizedUnionAndCeiling();
        std::cout<<checks<<" native Motion delta decoder checks passed\n";return 0;
    }catch(const std::exception& e){watchAllocations=false;std::cerr<<e.what()<<'\n';return 1;}
}
