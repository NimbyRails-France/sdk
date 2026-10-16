// Actual bridge wrappers with synthetic records. No game hooks/ABI/safety
// certificate is validated by this fixture.
#include <windows.h>
BOOL WINAPI phaseRead(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
BOOL WINAPI phaseClock(LARGE_INTEGER*);
BOOL WINAPI phaseFrequency(LARGE_INTEGER*);
ULONGLONG WINAPI phaseWallClock();
uintptr_t phaseFixtureCaller();
#define NRF_PHASE_CALLER() phaseFixtureCaller()
#define ReadProcessMemory phaseRead
#define QueryPerformanceCounter phaseClock
#define QueryPerformanceFrequency phaseFrequency
#define GetTickCount64 phaseWallClock
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#undef ReadProcessMemory
#undef QueryPerformanceCounter
#undef QueryPerformanceFrequency
#undef GetTickCount64
#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <cstdlib>
#include <new>
namespace {
size_t reads{},clocks{},checks{};
int64_t clockValue=1000000,clockCost=1;
ULONGLONG wallClock=1000000;
LPCVOID mutateAt{},unreadableAt{};
unsigned mutateVisit=1;
std::function<void()> mutation;
bool watchAllocations=false;
size_t allocations=0;
thread_local uintptr_t fixtureCaller{};
bool perturbLastError=false;
DWORD expectedNativeError{};
}
uintptr_t phaseFixtureCaller(){return fixtureCaller;}
#if defined(__GNUC__)
#define PHASE_TEST_NOINLINE __attribute__((noinline))
#else
#define PHASE_TEST_NOINLINE __declspec(noinline)
#endif
PHASE_TEST_NOINLINE void* operator new(std::size_t size){if(watchAllocations)++allocations;if(void* value=std::malloc(size?size:1))return value;throw std::bad_alloc();}
PHASE_TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
PHASE_TEST_NOINLINE void operator delete(void* value)noexcept{std::free(value);}
PHASE_TEST_NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
PHASE_TEST_NOINLINE void operator delete(void* value,std::size_t)noexcept{std::free(value);}
PHASE_TEST_NOINLINE void operator delete[](void* value,std::size_t)noexcept{std::free(value);}
BOOL WINAPI phaseRead(HANDLE process,LPCVOID address,LPVOID output,SIZE_T size,SIZE_T* copied){
 ++reads;if(address==unreadableAt)return FALSE;
 const auto result=ReadProcessMemory(process,address,output,size,copied);
 if(perturbLastError)SetLastError(0x1337);
 if(address==mutateAt&&!--mutateVisit){mutateAt=nullptr;if(mutation)mutation();}
 return result;
}
BOOL WINAPI phaseClock(LARGE_INTEGER* out){++clocks;out->QuadPart=clockValue;clockValue+=clockCost;return TRUE;}
BOOL WINAPI phaseFrequency(LARGE_INTEGER* out){++clocks;out->QuadPart=1000000;return TRUE;}
ULONGLONG WINAPI phaseWallClock(){return wallClock;}
#define CHECK(x) do {++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t trainId=0x5000000000001,trackId=0x1000000000001,signalId=0x8000000000001;
template<class T>void put(auto& bytes,size_t offset,T value){std::memcpy(bytes.data()+offset,&value,sizeof value);}
struct Fixture {
 std::vector<unsigned char> image=std::vector<unsigned char>(0xb819a0);
 alignas(8)std::array<unsigned char,0x688> root{};
 alignas(8)std::array<unsigned char,0x2300> sim{};
 alignas(8)std::array<unsigned char,0x900> network{};
 alignas(8)std::array<unsigned char,0x500> controller{};
 alignas(8)std::array<uintptr_t,2> controllerReference{},advanceReference{},simulationReference{};
 alignas(8)std::array<unsigned char,24> descriptor{};
 alignas(8)std::array<unsigned char,0x638> motion{};
 alignas(8)std::array<unsigned char,0x20> history{};
 alignas(8)std::array<unsigned char,0x80> cars{};
 alignas(8)std::array<unsigned char,0x40> attachments{};
 unsigned phases{},resets{},rebuilds{};
 unsigned controllerCalls{},advanceCalls{},simulationCalls{};
 bool arguments=true,throwNative=false,nestReset=false;
 bool foreignAdvance=false,foreignTrain=false;
 std::function<void()> nativeAction;
 uintptr_t simAddress()const{return reinterpret_cast<uintptr_t>(sim.data());}
 uintptr_t networkAddress()const{return reinterpret_cast<uintptr_t>(network.data());}
 uintptr_t motionAddress()const{return reinterpret_cast<uintptr_t>(motion.data());}
 void prepare(){
  motion.fill(0);cars.fill(0);attachments.fill(0);
  base=reinterpret_cast<uintptr_t>(image.data());
  put(image,0xb81998,reinterpret_cast<uintptr_t>(root.data()));put(root,0x680,simAddress());
  controllerReference={reinterpret_cast<uintptr_t>(controller.data()),0};
  advanceReference={networkAddress(),reinterpret_cast<uintptr_t>(root.data())};simulationReference=advanceReference;
  put(controller,0,networkAddress());put(controller,0x140,simAddress());
  std::memcpy(controller.data()+0x220,advanceReference.data(),sizeof advanceReference);
  put(descriptor,0,uint64_t{42});put(descriptor,0x10,int64_t{50});
  put(motion,0,trainId);put(motion,0x40,850.0f);put(motion,0xb8,trackId);
  put(motion,0xc0,.4);put(motion,0xc8,int8_t{1});put(motion,0xc9,int8_t{0});
  put(motion,0x1d0,uint8_t{1});put(motion,0x1f0,uint8_t{0});
  put(cars,0,uint64_t{123});put(cars,8,trainId);
  put(cars,0x20,uint64_t{456});put(cars,0x28,trainId+1);
  put(cars,0x40,uint64_t{789});
  auto carBegin=reinterpret_cast<uintptr_t>(cars.data());
  put(motion,8,carBegin);put(motion,0x10,carBegin+0x60);put(motion,0x18,carBegin+cars.size());
  put(attachments,0,trainId+1);put(attachments,0x10,int64_t{-1000});
  auto attachmentBegin=reinterpret_cast<uintptr_t>(attachments.data());
  put(motion,0x88,attachmentBegin);put(motion,0x90,attachmentBegin+0x20);put(motion,0x98,attachmentBegin+attachments.size());
  put(history,0,trackId+1);put(history,8,int32_t{-1});
  const auto begin=reinterpret_cast<uintptr_t>(history.data());
  put(motion,0xe0,begin);put(motion,0xe8,begin+0x10);put(motion,0xf0,begin+history.size());
  Publications::View view(publications);auto& publication=const_cast<Publication&>(*view);
  publication.session=simAddress();publication.world=std::make_shared<TrainWorld>();
  const NimbySignalDrivingRule rule{signalId,0,0,0,NIMBY_DRIVING_STOP};
  publication.rules.clear();CHECK(publication.rules.publish(1,{rule},GetTickCount64()+100000)==NIMBY_OK);
  auto* slot=publication.world->find(trainId,true);CHECK(slot);slot->id=trainId;slot->state.motion=motionAddress();
  phaseObservation.cursor=0;phaseObservation.sampleCount=0;
  reads=clocks=0;clockValue=1000000;clockCost=1;mutateAt=unreadableAt=nullptr;mutateVisit=1;mutation={};
  perturbLastError=false;
 }
};
Fixture* fixture{};
struct Caller {uintptr_t saved=fixtureCaller;explicit Caller(uintptr_t rva){fixtureCaller=base+rva;}~Caller(){fixtureCaller=saved;}};
void __fastcall fakeController(uintptr_t reference){
 if(perturbLastError)CHECK(GetLastError()==expectedNativeError);
 ++fixture->controllerCalls;CHECK(reference==reinterpret_cast<uintptr_t>(fixture->controllerReference.data()));
 Caller caller(fixture->foreignAdvance?0x4f3eae:0x3495fe);
 advancePhase(fixture->simAddress(),reinterpret_cast<uintptr_t>(fixture->advanceReference.data()),fixture->networkAddress(),reinterpret_cast<uintptr_t>(fixture->descriptor.data()));
}
void __fastcall fakeAdvance(uintptr_t sim,uintptr_t reference,uintptr_t network,uintptr_t descriptor){
 if(perturbLastError)CHECK(GetLastError()==expectedNativeError);
 ++fixture->advanceCalls;CHECK(sim==fixture->simAddress()&&network==fixture->networkAddress());
 CHECK(reference==reinterpret_cast<uintptr_t>(fixture->advanceReference.data())&&descriptor==reinterpret_cast<uintptr_t>(fixture->descriptor.data()));
 Caller caller(0x4784f9);simulationPhase(sim,reinterpret_cast<uintptr_t>(fixture->simulationReference.data()),network,50);
}
void __fastcall fakeSimulation(uintptr_t sim,uintptr_t reference,uintptr_t network,int64_t elapsed){
 if(perturbLastError)CHECK(GetLastError()==expectedNativeError);
 ++fixture->simulationCalls;CHECK(sim==fixture->simAddress()&&network==fixture->networkAddress()&&elapsed==50);
 CHECK(reference==reinterpret_cast<uintptr_t>(fixture->simulationReference.data()));
 Caller caller(fixture->foreignTrain?0x477f06:0x477f05);trainPhase(sim+0xa0,network,sim,elapsed);
}
void __fastcall fakePhase(uintptr_t state,uintptr_t network,uintptr_t sim,int64_t elapsed){
 if(perturbLastError)CHECK(GetLastError()==expectedNativeError);
 ++fixture->phases;fixture->arguments&=state==fixture->simAddress()+0xa0&&network==fixture->networkAddress()&&sim==fixture->simAddress()&&elapsed==50;
 if(fixture->throwNative)throw std::runtime_error("native phase error");
 put(fixture->motion,0xc0,.75);if(fixture->nativeAction)fixture->nativeAction();
 if(perturbLastError)SetLastError(0x52);
}
void __fastcall fakeReset(uintptr_t sim){++fixture->resets;CHECK(sim==fixture->simAddress());CHECK(!phaseObservationEnabled||phaseObservation.resetting.load()>0);}
void __fastcall fakeRebuild(uintptr_t sim,uintptr_t network){
 ++fixture->rebuilds;CHECK(sim==fixture->simAddress()&&network==fixture->networkAddress());
 CHECK(!phaseObservationEnabled||phaseObservation.resetting.load()>0);
 if(fixture->nestReset)resetWorld(sim);
}
void invoke(Fixture& value,bool enabled=true){
 // Each fixture invocation owns a new diagnostic sampling window. Advancing
 // this explicit clock avoids depending on CTest CPU load or wall-clock sleeps.
 wallClock+=100;
 fixture=&value;nativeTrainPhase=fakePhase;nativeWorldReset=fakeReset;nativeWorldRebuild=fakeRebuild;
 nativeControllerPhase=fakeController;nativeAdvancePhase=fakeAdvance;nativeSimulationPhase=fakeSimulation;
 phaseObservationEnabled=enabled;controllerPhase(reinterpret_cast<uintptr_t>(value.controllerReference.data()));
}
void capturesAndDisabled(){
 Fixture value;value.prepare();const auto before=phaseObservation.phases.load();
 invoke(value,false);CHECK(value.phases==1&&value.arguments);CHECK(reads==0&&clocks==0);CHECK(phaseObservation.phases.load()==before);
 value.prepare();const auto captured=phaseObservation.captured.load();invoke(value);
 CHECK(value.phases==2&&value.arguments);CHECK(phaseObservation.phases.load()==before+1);
 CHECK(phaseObservation.captured.load()==captured+1&&phaseObservation.sampleCount==1);
 CHECK(phaseObservation.samples[0].value.fraction==.75&&phaseObservation.samples[0].value.nativeLength==850);
 CHECK(phaseObservation.samples[0].value.historyCount==1&&phaseObservation.samples[0].value.active);
 CHECK(phaseObservation.samples[0].value.retainedCars==3&&phaseObservation.samples[0].value.ownRetainedCars==1);
 CHECK(phaseObservation.samples[0].value.foreignRetainedCars==1&&phaseObservation.samples[0].value.unownedRetainedCars==1);
 CHECK(phaseObservation.samples[0].value.attachmentCount==1);
 CHECK(phaseObservation.samples[0].epoch==phaseObservation.epoch.load()&&phaseObservation.samples[0].phase==before+1);
 CHECK(reads>0&&reads<150&&clocks>0);
 CHECK(value.controllerCalls==2&&value.advanceCalls==2&&value.simulationCalls==2&&phaseContext().empty());
 allocations=0;watchAllocations=true;invoke(value);watchAllocations=false;
 CHECK(allocations==0);
}
void invalidReads(){
 using namespace nimby::windows::automatic;
 Fixture value;value.prepare();
 auto count=[&](PhaseCaptureReason reason){return phaseObservation.reasons[static_cast<size_t>(reason)].load();};
 const auto changed=count(PhaseCaptureReason::ChangedReadback);
 mutateAt=value.motion.data()+0xe0;mutation=[&]{put(value.motion,0xc0,.5);};
 invoke(value);CHECK(count(PhaseCaptureReason::ChangedReadback)==changed+1&&phaseObservation.sampleCount==0);
 value.prepare();const auto badHistory=count(PhaseCaptureReason::InvalidHistory);put(value.history,0,signalId);
 invoke(value);CHECK(count(PhaseCaptureReason::InvalidHistory)==badHistory+1);
 value.prepare();const auto id=count(PhaseCaptureReason::TrainChanged);put(value.motion,0,trainId+1);
 invoke(value);CHECK(count(PhaseCaptureReason::TrainChanged)==id+1);
 value.prepare();const auto badLength=count(PhaseCaptureReason::InvalidLength);put(value.motion,0x40,std::numeric_limits<float>::infinity());
 invoke(value);CHECK(count(PhaseCaptureReason::InvalidLength)==badLength+1);
 value.prepare();const auto missing=count(PhaseCaptureReason::Unreadable);unreadableAt=value.motion.data()+0xb8;
 invoke(value);CHECK(count(PhaseCaptureReason::Unreadable)==missing+1);unreadableAt=nullptr;
 value.prepare();const auto shortBudget=phaseObservation.budgetExceeded.load();clockCost=1001;
 invoke(value);CHECK(phaseObservation.budgetExceeded.load()==shortBudget+1&&phaseObservation.sampleCount==0);
 value.prepare();const auto captured=phaseObservation.captured.load(),resetDuringRead=phaseObservation.contextUnavailable.load();
 mutateAt=value.motion.data()+0x1f0;mutation=[&]{resetWorld(value.simAddress());};
 invoke(value);CHECK(phaseObservation.captured.load()==captured&&phaseObservation.contextUnavailable.load()==resetDuringRead+1);
 CHECK(phaseObservation.sampleCount==0&&phaseObservation.resetting.load()==0);
 value.prepare();const auto readBudget=phaseObservation.budgetExceeded.load(),ready=count(PhaseCaptureReason::Ready);
 mutateVisit=3;
 mutateAt=value.motion.data()+0x1e8;mutation=[&]{clockValue+=3000;};
 invoke(value);CHECK(phaseObservation.captured.load()==captured&&phaseObservation.budgetExceeded.load()==readBudget+1);
 CHECK(phaseObservation.sampleCount==0&&count(PhaseCaptureReason::Ready)==ready+1);
}
void resetsAndNativeErrors(){
 Fixture value;value.prepare();fixture=&value;nativeWorldReset=fakeReset;nativeWorldRebuild=fakeRebuild;
 phaseObservationEnabled=false;auto epoch=phaseObservation.epoch.load();resetWorld(value.simAddress());
 CHECK(value.resets==1&&phaseObservation.epoch.load()==epoch&&phaseObservation.resetting.load()==0);
 phaseObservationEnabled=true;value.nestReset=true;rebuildWorld(value.simAddress(),value.networkAddress());
 CHECK(value.resets==2&&value.rebuilds==1&&phaseObservation.epoch.load()==epoch+4&&phaseObservation.resetting.load()==0);
 value.prepare();const auto captured=phaseObservation.captured.load(),unavailable=phaseObservation.contextUnavailable.load();
 value.nativeAction=[&]{resetWorld(value.simAddress());};invoke(value);
 CHECK(phaseObservation.captured.load()==captured&&phaseObservation.contextUnavailable.load()==unavailable+1);
 value.nativeAction={};value.throwNative=true;const auto phases=phaseObservation.phases.load();bool threw=false;
 try{invoke(value);}catch(const std::runtime_error& error){threw=std::string(error.what())=="native phase error";}
 CHECK(threw&&phaseObservation.phases.load()==phases&&phaseObservation.resetting.load()==0);
}
void pureCaptureLimits(){
 using namespace nimby::windows::automatic;
 Fixture value;value.prepare();const auto reader=[](uintptr_t address,void* output,size_t size){return readBytes(address,output,size);};
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::Ready);
 CHECK(captureTrainPhase(8,trainId,reader).reason==PhaseCaptureReason::InvalidAddress);
 CHECK(captureTrainPhase(value.motionAddress(),signalId,reader).reason==PhaseCaptureReason::TrainChanged);
 put(value.motion,0xe8,reinterpret_cast<uintptr_t>(value.history.data())+0x11);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidHistory);
 value.prepare();const auto begin=reinterpret_cast<uintptr_t>(value.history.data());put(value.motion,0xe8,begin+129*0x10);put(value.motion,0xf0,begin+129*0x10);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::HistoryCapacity);
 value.prepare();put(value.motion,0xe0,uintptr_t{});put(value.motion,0xe8,uintptr_t{});put(value.motion,0xf0,uintptr_t{});
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::Ready);
}
void compositionDiagnostics(){
 using namespace nimby::windows::automatic;
 Fixture value;value.prepare();
 const auto reader=[](uintptr_t address,void* output,size_t size){return readBytes(address,output,size);};
 auto result=captureTrainPhase(value.motionAddress(),trainId,reader);
 CHECK(result.reason==PhaseCaptureReason::Ready&&result.sample.retainedCars==3&&result.sample.attachmentCount==1);
 CHECK(result.sample.ownRetainedCars==1&&result.sample.foreignRetainedCars==1&&result.sample.unownedRetainedCars==1);
 const auto carHash=result.sample.compositionFingerprint,attachmentHash=result.sample.attachmentFingerprint;
 put(value.cars,0,uint64_t{124});result=captureTrainPhase(value.motionAddress(),trainId,reader);
 CHECK(result.reason==PhaseCaptureReason::Ready&&result.sample.compositionFingerprint!=carHash);
 CHECK(result.sample.attachmentFingerprint==attachmentHash);
 // Commands can change a child without inserting it in any parent list. A
 // readable empty list is diagnostic Ready, never an exhaustive consist proof.
 value.prepare();put(value.motion,0x1f0,uint8_t{1});put(value.motion,0x1d8,trainId+1);
 put(value.motion,0x1e0,int64_t{-123});put(value.motion,0x1e8,uint8_t{1});
 put(value.motion,0x88,uintptr_t{});put(value.motion,0x90,uintptr_t{});put(value.motion,0x98,uintptr_t{});
 result=captureTrainPhase(value.motionAddress(),trainId,reader);
 CHECK(result.reason==PhaseCaptureReason::Ready&&result.sample.coupled&&result.sample.parent==trainId+1);
 CHECK(result.sample.couplingOrder==-123&&result.sample.couplingReversed&&result.sample.attachmentCount==0);
 put(value.motion,0x1d8,trainId);CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidCoupling);
 put(value.motion,0x1d8,uint64_t{});CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidCoupling);
 put(value.motion,0x1f0,uint8_t{}); // Unhitch keeps a stale parent/order, valid diagnostic bytes.
 put(value.motion,0x1d8,signalId);CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::Ready);
 value.prepare();put(value.cars,0x28,signalId);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidComposition);
 value.prepare();put(value.attachments,0,signalId);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidAttachments);
 value.prepare();put(value.attachments,0,trainId);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidAttachments);
 value.prepare();const auto carBegin=reinterpret_cast<uintptr_t>(value.cars.data());
 put(value.motion,0x10,carBegin+129*0x20);put(value.motion,0x18,carBegin+129*0x20);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::CompositionCapacity);
 value.prepare();put(value.motion,0x10,carBegin+0x21);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidComposition);
 value.prepare();const auto attachmentBegin=reinterpret_cast<uintptr_t>(value.attachments.data());
 put(value.motion,0x90,attachmentBegin+65*0x20);put(value.motion,0x98,attachmentBegin+65*0x20);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::AttachmentsCapacity);
 value.prepare();put(value.motion,0x90,attachmentBegin+0x21);
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::InvalidAttachments);
 value.prepare();mutateAt=value.cars.data();mutation=[&]{put(value.cars,0,uint64_t{999});};
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::ChangedReadback);
 value.prepare();mutateAt=value.attachments.data();mutation=[&]{put(value.attachments,0x10,int64_t{1});};
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::ChangedReadback);
 value.prepare();mutateAt=value.motion.data()+0x1e8;mutation=[&]{put(value.motion,0x1e0,int64_t{1});};
 CHECK(captureTrainPhase(value.motionAddress(),trainId,reader).reason==PhaseCaptureReason::ChangedReadback);
 // All refused reads terminate the bounded copy immediately, without retry,
 // heap allocation or a partial sample. Exercise every read boundary.
 value.prepare();size_t readCount=0;
 const auto counted=[&](uintptr_t address,void* output,size_t size){++readCount;return reader(address,output,size);};
 CHECK(captureTrainPhase(value.motionAddress(),trainId,counted).reason==PhaseCaptureReason::Ready);
 const auto totalReads=readCount;
 for(size_t stop=1;stop<=totalReads;++stop){
  size_t calls=0;allocations=0;watchAllocations=true;
  const auto refusal=[&](uintptr_t address,void* output,size_t size){return ++calls!=stop&&reader(address,output,size);};
  const auto refused=captureTrainPhase(value.motionAddress(),trainId,refusal);watchAllocations=false;
  CHECK(refused.reason!=PhaseCaptureReason::Ready&&calls==stop&&allocations==0);
  CHECK(refused.sample.train==trainId&&refused.sample.retainedCars==0&&refused.sample.attachmentCount==0);
 }
}
void ancestryRefusals(){
 Fixture value;value.prepare();auto captured=phaseObservation.captured.load();
 value.foreignAdvance=true;invoke(value);
 CHECK(value.phases==1&&value.arguments&&phaseObservation.captured.load()==captured&&phaseObservation.sampleCount==0);
 CHECK(phaseContext().empty());
 value.foreignAdvance=false;value.foreignTrain=true;value.prepare();invoke(value);
 CHECK(value.phases==2&&phaseObservation.captured.load()==captured&&phaseObservation.sampleCount==0&&phaseContext().empty());
 value.foreignTrain=false;value.prepare();
 value.nativeAction=[&]{put(value.controller,0x140,uintptr_t{0x10000});};invoke(value);
 CHECK(phaseObservation.captured.load()==captured&&phaseObservation.sampleCount==0&&phaseContext().empty());
 value.nativeAction={};value.prepare();
 // Context may change after otherwise coherent Motion readback. The final
 // ancestor check must discard the entire staged sample set.
 mutateAt=value.motion.data()+0x1f0;mutateVisit=3;
 mutation=[&]{put(value.controller,0x220,uintptr_t{0x10000});};invoke(value);
 CHECK(phaseObservation.captured.load()==captured&&phaseObservation.sampleCount==0&&phaseContext().empty());
 value.prepare();value.throwNative=true;bool threw=false;
 try{invoke(value);}catch(const std::runtime_error&){threw=true;}
 CHECK(threw&&phaseContext().empty());
 value.throwNative=false;value.prepare();invoke(value);
 CHECK(phaseObservation.captured.load()==captured+1&&phaseContext().empty());
 value.prepare();captured=phaseObservation.captured.load();fixture=&value;phaseObservationEnabled=true;
 // A direct/worker/server invocation never inherits another thread's scopes.
 {Caller caller(0x477f05);trainPhase(value.simAddress()+0xa0,value.networkAddress(),value.simAddress(),50);}
 CHECK(phaseObservation.captured.load()==captured&&phaseObservation.sampleCount==0&&phaseContext().empty());
 value.prepare();perturbLastError=true;expectedNativeError=0x41;SetLastError(expectedNativeError);invoke(value);
 CHECK(GetLastError()==0x52&&phaseContext().empty());perturbLastError=false;
}
}
int main(){try{
 capturesAndDisabled();invalidReads();resetsAndNativeErrors();pureCaptureLimits();compositionDiagnostics();ancestryRefusals();
 std::cout<<"PASS: "<<checks<<" train-phase diagnostic checks, synthetic ABI; no stable-tick or rear-clearance certificate\n";
 return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
