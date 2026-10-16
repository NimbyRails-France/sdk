// Exercise the actual opt-in step/dispatcher wrappers with synthetic native
// records. This verifies observation scoping, never game addresses, ABI or
// world/tick qualification. No SDK signal rules are installed in this fixture.
#include <windows.h>
BOOL WINAPI gatedReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
BOOL WINAPI passagePerformanceCounter(LARGE_INTEGER*);
BOOL WINAPI passagePerformanceFrequency(LARGE_INTEGER*);
ULONGLONG WINAPI passageGetTickCount64();
#define ReadProcessMemory gatedReadProcessMemory
#define QueryPerformanceCounter passagePerformanceCounter
#define QueryPerformanceFrequency passagePerformanceFrequency
#define GetTickCount64 passageGetTickCount64
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#undef ReadProcessMemory
#undef QueryPerformanceCounter
#undef QueryPerformanceFrequency
#undef GetTickCount64
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>

namespace {
thread_local bool watchCallbacks=false;
thread_local uint64_t readCalls=0,clockCalls=0,allocations=0;
thread_local int64_t clockTick=1000000;
thread_local LPCVOID unreadableAddress=nullptr;
}
BOOL WINAPI gatedReadProcessMemory(HANDLE process,LPCVOID address,LPVOID output,SIZE_T size,SIZE_T* copied) {
    if(watchCallbacks)++readCalls;
    if(address==unreadableAddress){if(copied)*copied=0;return FALSE;}
    return ReadProcessMemory(process,address,output,size,copied);
}
BOOL WINAPI passagePerformanceCounter(LARGE_INTEGER* value) {
    if(watchCallbacks)++clockCalls;
    value->QuadPart=++clockTick;return TRUE;
}
BOOL WINAPI passagePerformanceFrequency(LARGE_INTEGER* value) {
    if(watchCallbacks)++clockCalls;
    value->QuadPart=1000000;return TRUE;
}
ULONGLONG WINAPI passageGetTickCount64(){if(watchCallbacks)++clockCalls;return 1000;}
#if defined(__GNUC__)
#define TEST_NOINLINE __attribute__((noinline))
#else
#define TEST_NOINLINE __declspec(noinline)
#endif
TEST_NOINLINE void* operator new(std::size_t size) {
    if(watchCallbacks)++allocations;
    if(auto* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
TEST_NOINLINE void operator delete(void* value) noexcept {std::free(value);}
TEST_NOINLINE void operator delete[](void* value) noexcept {std::free(value);}
TEST_NOINLINE void operator delete(void* value,std::size_t) noexcept {std::free(value);}
TEST_NOINLINE void operator delete[](void* value,std::size_t) noexcept {std::free(value);}

namespace {
using nimby::engine::interlocking::NativePassageBatch;
using nimby::engine::interlocking::NativePassageConsume;
using nimby::engine::interlocking::NativePassageRow;
using nimby::engine::native_passage::Status;
using nimby::engine::native_passage::Reason;
using nimby::windows::automatic::PassageNativePresence;
size_t checks{};
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);}while(false)
constexpr uint64_t entity(unsigned kind,unsigned ordinal,unsigned generation=1) {
    return (uint64_t(kind)<<48)|(uint64_t(ordinal)<<16)|generation;
}
template<class T> void put(void* destination,size_t offset,T value) noexcept {
    std::memcpy(static_cast<unsigned char*>(destination)+offset,&value,sizeof value);
}
enum class Mutation {None,PathIds,SourceAnchor,SourceOrientation,TrainIdentity,Direction,Orientation,Track,Mirror,UnreadableLastSource};
struct Fixture {
    alignas(8) std::array<unsigned char,0x178> train{};
    alignas(8) std::array<unsigned char,0x638> motion{};
    alignas(8) std::array<unsigned char,0x200> context{};
    alignas(8) std::array<std::array<unsigned char,0xc8>,9> signals{};
    std::array<uint64_t,2> tracks{};
    uint64_t trainId{},trackId{},otherTrack{};
    uintptr_t scripts=11,service=22,budget=33;
    double mass=3.75,finalHead=101,finalFraction=.51;
    size_t candidates=1,scriptContextOffset=0xa8;
    Mutation mutation=Mutation::None;
    Fixture* nested=nullptr;
    unsigned stepCalls=0,passCalls=0;
    bool argumentsUnchanged=true,scopeRestored=true;
    std::array<uintptr_t,6> lastPassArguments{};
    explicit Fixture(unsigned ordinal=1) noexcept {reset(ordinal);}
    uintptr_t trainAddress()const noexcept{return reinterpret_cast<uintptr_t>(train.data());}
    uintptr_t motionAddress()const noexcept{return reinterpret_cast<uintptr_t>(motion.data());}
    uintptr_t contextAddress()const noexcept{return reinterpret_cast<uintptr_t>(context.data());}
    uintptr_t signalAddress(size_t index=0)const noexcept{return reinterpret_cast<uintptr_t>(signals[index].data());}
    void presence(double head,double fraction,int8_t direction=1,uint64_t track=0,bool mirror=true,int8_t orientation=0) noexcept {
        PassageNativePresence value{};value.track=track?track:trackId;value.fraction=fraction;
        value.direction=direction;value.orientation=orientation;value.head=head;
        std::memcpy(motion.data()+0x3a0,&value,sizeof value);
        if(mirror){
            put(motion.data(),0xb8,value.track);put(motion.data(),0xc0,value.fraction);
            put(motion.data(),0xc8,value.direction);put(motion.data(),0xc9,value.orientation);
        }
    }
    void reset(unsigned ordinal=1) noexcept {
        train.fill(0);motion.fill(0);context.fill(0);for(auto& signal:signals)signal.fill(0);
        trainId=entity(5,ordinal);trackId=entity(1,ordinal);otherTrack=entity(1,ordinal+100);
        tracks={trackId,otherTrack};put(train.data(),0,trainId);put(motion.data(),0,trainId);
        motion[0x290+0x90]=1;
        const auto begin=reinterpret_cast<uintptr_t>(tracks.data());
        put(motion.data(),0x290+0xa8,begin);put(motion.data(),0x290+0xb0,begin+sizeof tracks);
        put(motion.data(),0x290+0xb8,begin+sizeof tracks);
        for(size_t i=0;i<signals.size();++i){
            put(signals[i].data(),0,entity(8,ordinal*16+static_cast<unsigned>(i)));
            put(signals[i].data(),0x30,uint32_t{NIMBY_SIGNAL_PATH});
            put(signals[i].data(),0x40,trackId);put(signals[i].data(),0x48,.5);
            put(signals[i].data(),0x50,int8_t{-1}); // Panel-facing direction opposes travel.
        }
        presence(90,.4);
        finalHead=101;finalFraction=.51;candidates=1;scriptContextOffset=0xa8;mutation=Mutation::None;nested=nullptr;
        stepCalls=passCalls=0;argumentsUnchanged=scopeRestored=true;lastPassArguments={};
    }
};
Fixture* primaryFixture=nullptr;
Fixture* nestedFixture=nullptr;
Fixture* fixtureFor(uintptr_t motion) noexcept {
    if(primaryFixture&&primaryFixture->motionAddress()==motion)return primaryFixture;
    if(nestedFixture&&nestedFixture->motionAddress()==motion)return nestedFixture;
    return nullptr;
}
void __fastcall fakePassBy(uintptr_t scripts,uintptr_t scriptContext,uintptr_t train,uintptr_t motion,uintptr_t signal,uintptr_t service) {
    auto* fixture=fixtureFor(motion);
    if(!fixture)return;
    ++fixture->passCalls;
    fixture->argumentsUnchanged&=scripts==fixture->scripts&&scriptContext==fixture->contextAddress()+fixture->scriptContextOffset&&
        train==fixture->trainAddress()&&signal==fixture->signalAddress(fixture->passCalls-1)&&service==fixture->service;
    fixture->lastPassArguments={scripts,scriptContext,train,motion,signal,service};
}
uint32_t __fastcall fakeStep(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget) {
    auto* fixture=fixtureFor(motion);
    if(!fixture)return 0;
    ++fixture->stepCalls;
    fixture->argumentsUnchanged&=context==fixture->contextAddress()&&train==fixture->trainAddress()&&
        service==fixture->service&&mass==fixture->mass&&budget==fixture->budget;
    fixture->scopeRestored&=currentStep.context==context&&currentStep.motion==motion;
    if(fixture->nested){
        const auto* capture=activePassageCapture;
        const auto previous=currentStep;
        auto& child=*fixture->nested;
        const auto returned=stepMotion(child.contextAddress(),child.trainAddress(),child.motionAddress(),child.service,child.mass,child.budget);
        fixture->scopeRestored&=returned==17&&activePassageCapture==capture&&
            currentStep.context==previous.context&&currentStep.motion==previous.motion;
    }
    for(size_t i=0;i<fixture->candidates;++i){
        fixture->presence(100,.5);
        passBy(fixture->scripts,context+fixture->scriptContextOffset,train,motion,fixture->signalAddress(i),service);
    }
    fixture->presence(fixture->finalHead,fixture->finalFraction);
    switch(fixture->mutation){
    case Mutation::PathIds:fixture->tracks[1]=entity(1,999);break;
    case Mutation::SourceAnchor:put(fixture->signals[0].data(),0x48,.55);break;
    case Mutation::SourceOrientation:put(fixture->signals[0].data(),0x51,int8_t{1});break;
    case Mutation::TrainIdentity:
        put(fixture->train.data(),0,fixture->trainId+1);put(fixture->motion.data(),0,fixture->trainId+1);break;
    case Mutation::Direction:fixture->presence(fixture->finalHead,fixture->finalFraction,-1);break;
    case Mutation::Orientation:fixture->presence(fixture->finalHead,fixture->finalFraction,1,0,true,1);break;
    case Mutation::Track:fixture->presence(fixture->finalHead,fixture->finalFraction,1,fixture->otherTrack);break;
    case Mutation::Mirror:put(fixture->motion.data(),0xc0,.49);break;
    case Mutation::UnreadableLastSource:unreadableAddress=reinterpret_cast<LPCVOID>(fixture->signalAddress(fixture->candidates-1));break;
    case Mutation::None:break;
    }
    return 17;
}
struct CallbackCost {uint64_t reads{},clocks{},allocations{};uint32_t result{};};
CallbackCost invoke(Fixture& fixture,bool enabled=true) {
    primaryFixture=&fixture;nestedFixture=fixture.nested;nativeStep=fakeStep;nativePassBy=fakePassBy;
    passageObservationEnabled=enabled;unreadableAddress=nullptr;
    readCalls=clockCalls=allocations=0;clockTick=1000000;
    watchCallbacks=true;
    const auto result=stepMotion(fixture.contextAddress(),fixture.trainAddress(),fixture.motionAddress(),fixture.service,fixture.mass,fixture.budget);
    watchCallbacks=false;unreadableAddress=nullptr;
    return {readCalls,clockCalls,allocations,result};
}
NativePassageBatch consumeOne() {
    NativePassageBatch output{};
    CHECK(passageMailbox.tryConsume(output)==NativePassageConsume::Ready);
    return output;
}
void emptyMailbox() {
    NativePassageBatch output{};
    CHECK(passageMailbox.tryConsume(output)==NativePassageConsume::Empty);
}
void checkCallback(Fixture& fixture,const CallbackCost& cost,bool observed=true) {
    CHECK(cost.result==17&&fixture.stepCalls==1&&fixture.passCalls==fixture.candidates&&fixture.argumentsUnchanged&&fixture.scopeRestored);
    CHECK(cost.allocations==0);
    if(observed)CHECK(cost.reads>0&&cost.reads<=64&&cost.clocks>0);
    else CHECK(cost.reads==0&&cost.clocks==0);
    CHECK(currentStep.context==0&&currentStep.motion==0&&activePassageCapture==nullptr);
    CHECK((fixture.lastPassArguments==std::array<uintptr_t,6>{fixture.scripts,fixture.contextAddress()+fixture.scriptContextOffset,
        fixture.trainAddress(),fixture.motionAddress(),fixture.signalAddress(fixture.candidates-1),fixture.service}));
}
void expect(Status status,Reason reason,Fixture& fixture) {
    const auto cost=invoke(fixture);checkCallback(fixture,cost);
    const auto batch=consumeOne();CHECK(batch.count==fixture.candidates);
    for(size_t i=0;i<batch.count;++i){
        const auto& row=batch.rows[i];
        CHECK(row.outcome==static_cast<uint32_t>(status)&&row.reason==static_cast<uint32_t>(reason));
        CHECK(row.train==fixture.trainId&&row.signal==entity(8,16+static_cast<unsigned>(i))&&row.track==fixture.trackId);
        CHECK(row.threadStepSequence!=0);
    }
    emptyMailbox();
}
void optOutAndUnmanagedCrossing() {
    {Publications::View view(publications);CHECK(!view->active()&&!view->world);}
    Fixture fixture;
    const auto before=passageSteps.load();
    auto cost=invoke(fixture,false);checkCallback(fixture,cost,false);
    CHECK(passageSteps.load()==before);emptyMailbox();
    fixture.reset();cost=invoke(fixture);checkCallback(fixture,cost);
    const auto batch=consumeOne();CHECK(batch.count==1);
    const auto& row=batch.rows[0];
    CHECK(row.outcome==static_cast<uint32_t>(Status::Crossed)&&row.reason==static_cast<uint32_t>(Reason::CrossedSource));
    CHECK(row.flags==NativePassageRow::NumericComplete&&row.train==fixture.trainId&&row.track==fixture.trackId&&
        row.signal==entity(8,16)&&row.beforeHead==90&&row.boundaryHead==100&&row.afterHead==101&&
        row.beforeFraction==.4&&row.finalFraction==.51);
    // No native pointer is exported in a row: only typed IDs and copied facts.
    CHECK(row.train!=fixture.trainAddress()&&row.signal!=fixture.signalAddress()&&row.track!=fixture.motionAddress());
    {Publications::View view(publications);CHECK(!view->active()&&!view->world);}
    emptyMailbox();
}
void boundaryRollbackAndMutations() {
    Fixture fixture;
    put(fixture.motion.data(),0x3b0,std::numeric_limits<double>::quiet_NaN());
    expect(Status::Unavailable,Reason::ReadIncomplete,fixture);
    fixture.reset();
    fixture.finalHead=100+1e-7;fixture.finalFraction=std::nextafter(.5,1.0);
    expect(Status::AtBoundary,Reason::OnSourceBoundary,fixture);
    fixture.reset();fixture.finalHead=99;fixture.finalFraction=.49;
    expect(Status::BeforeSource,Reason::StillBeforeSource,fixture);
    fixture.reset();fixture.finalHead=89;fixture.finalFraction=.39;
    expect(Status::Unavailable,Reason::FinalHeadBeforeStep,fixture);
    fixture.reset();fixture.mutation=Mutation::Mirror;
    expect(Status::Unavailable,Reason::FinalPresenceMismatch,fixture);
    fixture.reset();fixture.mutation=Mutation::PathIds;
    expect(Status::Unavailable,Reason::PathChanged,fixture);
    fixture.reset();fixture.mutation=Mutation::SourceAnchor;
    expect(Status::Unavailable,Reason::SourceChanged,fixture);
    fixture.reset();fixture.mutation=Mutation::SourceOrientation;
    expect(Status::Unavailable,Reason::SourceChanged,fixture);
    fixture.reset();fixture.mutation=Mutation::TrainIdentity;
    expect(Status::Unavailable,Reason::TrainIdentityChanged,fixture);
    fixture.reset();fixture.mutation=Mutation::Direction;
    expect(Status::Unavailable,Reason::DirectionMismatch,fixture);
    fixture.reset();fixture.mutation=Mutation::Orientation;
    expect(Status::Unavailable,Reason::OrientationMismatch,fixture);
    fixture.reset();fixture.mutation=Mutation::Track;
    expect(Status::Unavailable,Reason::TrackTransition,fixture);
}
void incompleteCandidatesAndDispatcherScoping() {
    Fixture fixture;fixture.candidates=9;
    const auto cost=invoke(fixture);checkCallback(fixture,cost);
    const auto batch=consumeOne();CHECK(batch.count==8);
    for(size_t i=0;i<batch.count;++i)CHECK(batch.rows[i].outcome==static_cast<uint32_t>(Status::Unavailable)&&
        batch.rows[i].reason==static_cast<uint32_t>(Reason::CandidatesIncomplete));
    emptyMailbox();
    fixture.reset();fixture.candidates=2;fixture.mutation=Mutation::UnreadableLastSource;
    const auto failedCost=invoke(fixture);checkCallback(fixture,failedCost);
    const auto incomplete=consumeOne();CHECK(incomplete.count==2);
    for(size_t i=0;i<incomplete.count;++i)CHECK(incomplete.rows[i].outcome==static_cast<uint32_t>(Status::Unavailable)&&
        incomplete.rows[i].reason==static_cast<uint32_t>(Reason::ReadIncomplete)&&incomplete.rows[i].flags==0);
    emptyMailbox();
    // The original dispatcher still runs, but a different script context is
    // not a candidate belonging to this native step.
    fixture.reset();fixture.scriptContextOffset=0xb0;
    checkCallback(fixture,invoke(fixture));emptyMailbox();
    const auto beforeSteps=passageSteps.load();fixture.reset();primaryFixture=&fixture;nestedFixture=nullptr;
    watchCallbacks=true;readCalls=clockCalls=allocations=0;
    passBy(fixture.scripts,fixture.contextAddress()+0xa8,fixture.trainAddress(),fixture.motionAddress(),fixture.signalAddress(),fixture.service);
    watchCallbacks=false;
    CHECK(fixture.passCalls==1&&fixture.argumentsUnchanged&&readCalls==0&&clockCalls==0&&allocations==0&&passageSteps.load()==beforeSteps);
    emptyMailbox();
}
void nestedScopesAndSaturation() {
    Fixture outer(1),inner(2);outer.nested=&inner;
    const auto cost=invoke(outer);
    CHECK(cost.result==17&&cost.allocations==0&&outer.stepCalls==1&&inner.stepCalls==1&&
        outer.passCalls==1&&inner.passCalls==1&&outer.argumentsUnchanged&&inner.argumentsUnchanged&&
        outer.scopeRestored&&inner.scopeRestored&&activePassageCapture==nullptr&&currentStep.motion==0&&currentStep.context==0);
    const auto child=consumeOne(),parent=consumeOne();
    CHECK(child.count==1&&parent.count==1&&child.rows[0].train==inner.trainId&&parent.rows[0].train==outer.trainId);
    CHECK(child.rows[0].outcome==static_cast<uint32_t>(Status::Crossed)&&parent.rows[0].outcome==static_cast<uint32_t>(Status::Crossed));
    CHECK(child.rows[0].threadStepSequence<parent.rows[0].threadStepSequence&&child.sequence<parent.sequence);emptyMailbox();
    const auto baseline=passageMailbox.stats();CHECK(!passageMailbox.lossObserved());
    Fixture fixture;
    for(size_t i=0;i<16;++i){fixture.reset();checkCallback(fixture,invoke(fixture));}
    fixture.reset();checkCallback(fixture,invoke(fixture));
    const auto full=passageMailbox.stats();
    CHECK(full.accepted==baseline.accepted+16&&full.full==baseline.full+1&&full.lost==baseline.lost+1&&passageMailbox.lossObserved());
    for(size_t i=0;i<16;++i){const auto batch=consumeOne();CHECK(batch.count==1&&batch.rows[0].outcome==static_cast<uint32_t>(Status::Crossed));}
    emptyMailbox();CHECK(passageMailbox.lossObserved());
    fixture.reset();checkCallback(fixture,invoke(fixture));CHECK(consumeOne().count==1);emptyMailbox();
    CHECK(passageMailbox.stats().lost==full.lost&&passageMailbox.lossObserved());
}
}
int main() {
    try {
        optOutAndUnmanagedCrossing();boundaryRollbackAndMutations();incompleteCandidatesAndDispatcherScoping();nestedScopesAndSaturation();
        std::cout<<"PASS: "<<checks<<" native passage observer checks; synthetic ABI fixture, diagnostic only\n";
    } catch(const std::exception& error){watchCallbacks=false;std::cerr<<error.what()<<'\n';return 1;}
}
