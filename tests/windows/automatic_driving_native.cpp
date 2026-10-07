// Exercise the actual bridge wrappers with controlled native callbacks. This
// verifies permission scoping; it does not validate game addresses or ABI.
#include <windows.h>
BOOL WINAPI gatedReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
BOOLEAN WINAPI gatedTryAcquireSRWLockExclusive(PSRWLOCK);
ULONGLONG WINAPI shiftedGetTickCount64();
#define ReadProcessMemory gatedReadProcessMemory
#define TryAcquireSRWLockExclusive gatedTryAcquireSRWLockExclusive
#define GetTickCount64 shiftedGetTickCount64
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#undef ReadProcessMemory
#undef TryAcquireSRWLockExclusive
#undef GetTickCount64
#include <iostream>
#include <stdexcept>
#include <future>
#include <thread>
#include <cstdlib>
#include <new>
namespace {
thread_local unsigned pauseAllocationAfter{};
thread_local LPCVOID pauseReadAt{};
thread_local unsigned transientLockFailures{};
thread_local unsigned expireOnLockAttempt{};
thread_local ULONGLONG tickOffset{};
HANDLE allocationEntered{},allocationRelease{};
void pauseAllocation(){if(pauseAllocationAfter&&!--pauseAllocationAfter){SetEvent(allocationEntered);WaitForSingleObject(allocationRelease,10000);}}
}
BOOL WINAPI gatedReadProcessMemory(HANDLE process,LPCVOID address,LPVOID output,SIZE_T size,SIZE_T* copied){
 const auto result=ReadProcessMemory(process,address,output,size,copied);
 if(address==pauseReadAt){pauseReadAt=nullptr;SetEvent(allocationEntered);WaitForSingleObject(allocationRelease,10000);}
 return result;
}
BOOLEAN WINAPI gatedTryAcquireSRWLockExclusive(PSRWLOCK lock){
 if(expireOnLockAttempt&&!--expireOnLockAttempt){tickOffset=200;return FALSE;}
 if(transientLockFailures){--transientLockFailures;return FALSE;}
 return TryAcquireSRWLockExclusive(lock);
}
ULONGLONG WINAPI shiftedGetTickCount64(){return GetTickCount64()+tickOffset;}
#if defined(__GNUC__)
#define TEST_NOINLINE __attribute__((noinline))
#else
#define TEST_NOINLINE __declspec(noinline)
#endif
TEST_NOINLINE void* operator new(std::size_t size){pauseAllocation();if(auto* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
TEST_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
TEST_NOINLINE void operator delete(void* p)noexcept{std::free(p);}
TEST_NOINLINE void operator delete[](void* p)noexcept{std::free(p);}
TEST_NOINLINE void operator delete(void* p,std::size_t)noexcept{std::free(p);}
TEST_NOINLINE void operator delete[](void* p,std::size_t)noexcept{std::free(p);}
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
struct GatedResult {uint32_t delayed{},healthy{};bool independent{};};
template<class Slow,class Healthy> GatedResult gatedPublication(Slow slow,Healthy healthy,uint32_t delay=0){
 allocationEntered=CreateEventW(nullptr,TRUE,FALSE,nullptr);allocationRelease=CreateEventW(nullptr,TRUE,FALSE,nullptr);
 CHECK(allocationEntered&&allocationRelease);
 auto blocked=std::async(std::launch::async,[&]{pauseAllocationAfter=2;return slow();});
 const bool entered=WaitForSingleObject(allocationEntered,2000)==WAIT_OBJECT_0;
 auto peer=std::async(std::launch::async,healthy);
 const bool independent=peer.wait_for(std::chrono::milliseconds(200))==std::future_status::ready;
 if(delay)Sleep(delay);
 SetEvent(allocationRelease);const auto delayed=blocked.get(),status=peer.get();
 CloseHandle(allocationEntered);CloseHandle(allocationRelease);allocationEntered=allocationRelease=nullptr;
 CHECK(entered);return {delayed,status,independent};
}
// Fixture setup mutates the current image only while no callback/publication is
// running. Production never exposes a mutable publication to its readers.
Publication& testImage(){Publications::View view(publications);return const_cast<Publication&>(*view);}
auto& rulePublishersForTest(){return testImage().rules;}
auto& constraintPublishersForTest(){return testImage().constraints;}
uint64_t expiry=0;bool rawRules=true;
void refreshRawRules(){
 if(!rawRules)return;
 auto& image=testImage();const auto rows=rules(image);auto prepared=image.rules;prepared.clear();
 CHECK(prepared.publish(1,rows,expiry)==NIMBY_OK);
 image.signalLifetimes=nextLifetimes(image.rules,prepared,image.signalLifetimes,++image.revision,true);image.rules.swap(prepared);
}
RuntimeTrain& testTrain(uint64_t id){auto& image=testImage();if(!image.world)image.world=std::make_shared<TrainWorld>();image.world->markManaged(id);
 auto* slot=image.world->find(id,true);CHECK(slot);slot->id=id;return slot->state;}
void resetTrains(){testImage().world=std::make_shared<TrainWorld>();}
const auto& rules(){return rules(testImage());}
const auto& constraints(){return testImage().constraints.rows();}
const NimbyTrainConstraint* constraint(uint64_t id){return constraint(testImage(),id);}
bool freshRule(uint64_t id,uint64_t now){return freshRule(testImage(),id,now);}
bool freshConstraint(uint64_t id,uint64_t now){return freshConstraint(testImage(),id,now);}
bool freshTrain(const Train& train,double head,uint64_t now){return freshTrain(testImage(),train,head,now);}
#define rulePublishers rulePublishersForTest()
#define constraintPublishers constraintPublishersForTest()
#define session testImage().session
constexpr uint64_t trainId=0x5000000000001,signalId=0x8000000000001;
constexpr uint64_t trackId=0x1000000000001;
std::array<unsigned char,0x500> fakeMotion{};
std::array<unsigned char,0x100> fakeTrack{},crossTrack{};
std::array<uint64_t,1> pathIds{trackId};
double obstruction=.6;
bool reservationConflict=false,crossConflict=false,crossReservationConflict=false,controllerConflict=false,invalidGeometry=false;
bool repeatFollowedRange=false,repeatOccupation=false,skipOccupation=false,wrongReservationMotion=false;
int reservationChecks=0;
thread_local unsigned nativePermissionCalls=0;
thread_local double appliedDistance=-1,appliedTarget=-1,appliedCeiling=-1;
thread_local bool pauseNative=false,reenterNative=false;
thread_local std::function<void(uintptr_t)> duringNative;
HANDLE nativeEntered{},nativeRelease{};
uint64_t releaseDuringIntegrate=0;
template<class T> void put(void* at,size_t offset,T value){std::memcpy(static_cast<unsigned char*>(at)+offset,&value,sizeof value);}
uint32_t __fastcall fakeStep(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget){
 CHECK(currentStep.context==context&&currentStep.motion==motion);
 CHECK(train==11&&service==22&&mass==3.75&&budget==33);
 {StepScope nested(44,55);CHECK(currentStep.context==44&&currentStep.motion==55);}
 CHECK(currentStep.context==context&&currentStep.motion==motion);
 return 17;
}
uintptr_t __fastcall fakeScan(uintptr_t,uintptr_t){return 1;}
uintptr_t __fastcall fakeIntegrate(uintptr_t result,uintptr_t,uintptr_t,uintptr_t,uintptr_t dynamics,double,
 double ceiling,double,double,double distance,double target,int64_t ticks,uintptr_t) {
 appliedDistance=distance;appliedTarget=target;appliedCeiling=ceiling;
 if(pauseNative){SetEvent(nativeEntered);WaitForSingleObject(nativeRelease,2000);}
 if(reenterNative){reenterNative=false;CHECK(check(0,0,0,0,0,dynamics-8,signalId+1,0)==1);}
 if(duringNative)duringNative(dynamics-8);
 if(releaseDuringIntegrate){
  const auto owner=releaseDuringIntegrate;releaseDuringIntegrate=0;
  CHECK(NimbyDriving_PublishV3(nullptr,0,1000,0,owner)==NIMBY_OK);
  put(fakeMotion.data(),0x3c0,1001.0);
 }
 put(reinterpret_cast<void*>(result),0x30,ticks);return result;
}
struct NativeRange {uintptr_t track;double from,to,length;int kind,padding;uintptr_t signal;};
uint8_t __fastcall fakeOccupancy(uintptr_t context,uintptr_t track,double from,double to) {
 const bool occupied=track==reinterpret_cast<uintptr_t>(crossTrack.data())?crossConflict:
  std::max(from,to)>=obstruction&&std::min(from,to)<=.9;
 if(occupied)*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t*>(context)[2])=0;
 return !occupied;
}
uintptr_t __fastcall fakeRange(uintptr_t context,uintptr_t range) {
 auto& r=*reinterpret_cast<NativeRange*>(range);auto* ctx=reinterpret_cast<uintptr_t*>(context);
 if(r.kind==6)return 1;
 if(!skipOccupation&&!occupancy(ctx[3],r.track,r.from,r.to))return 0;
 if(!reservation(ctx[4],r.track,r.from,r.to))return 0;
 if(repeatFollowedRange&&!reservation(ctx[4],r.track,r.from,r.to))return 0;
 if(repeatOccupation&&!occupancy(ctx[3],r.track,r.from,r.to))return 0;
 if(!occupancy(ctx[3],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1))return 0;
 if(!reservation(ctx[4],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1))return 0;
 if(controllerConflict){*reinterpret_cast<uint8_t*>(ctx[8])=0;return 0;}
 return 1;
}
uint8_t __fastcall fakeReservation(uintptr_t context,uintptr_t track,double,double) {
 ++reservationChecks;
 const bool conflict=track==reinterpret_cast<uintptr_t>(crossTrack.data())?crossReservationConflict:reservationConflict;
 if(conflict)*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t*>(context)[2])=0;
 return !conflict;
}
uint8_t __fastcall fakeCheck(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t motion,uint64_t signal,uint8_t) {
 ++nativePermissionCalls;
 uint8_t allowed=1;std::array<uintptr_t,3> occ{1,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&allowed)};
 uintptr_t reservationMotion=wrongReservationMotion?motion+1:motion;
 std::array<uintptr_t,3> reserved{2,reinterpret_cast<uintptr_t>(&reservationMotion),reinterpret_cast<uintptr_t>(&allowed)};
 std::array<uintptr_t,10> ctx{};ctx[3]=reinterpret_cast<uintptr_t>(occ.data());ctx[8]=reinterpret_cast<uintptr_t>(&allowed);
 ctx[4]=reinterpret_cast<uintptr_t>(reserved.data());
 NativeRange source{reinterpret_cast<uintptr_t>(fakeTrack.data()),0,0,0,6,0,reinterpret_cast<uintptr_t>(&signal)};
 permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&source));
 NativeRange section{source.track,0,1,invalidGeometry?50.0:1000.0,0,0,0};
 permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&section));
 return allowed;
}
}
int main(){try{
 std::vector<unsigned char> module(0xb81998+8),root(0x688);
 base=reinterpret_cast<uintptr_t>(module.data());session=0x12340000;
 put(module.data(),0xb81998,reinterpret_cast<uintptr_t>(root.data()));put(root.data(),0x680,session);
 put(fakeMotion.data(),0,trainId);put(fakeMotion.data(),0x3c0,1000.0);put(fakeTrack.data(),0x88,1000.0);
 put(fakeTrack.data(),0,trackId);put(fakeMotion.data(),0x3a8,trackId);put(fakeMotion.data(),0x3b8,int8_t{1});
 put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x320,uint8_t{1});
 put(fakeMotion.data(),0x338,reinterpret_cast<uintptr_t>(pathIds.data()));
 put(fakeMotion.data(),0x340,reinterpret_cast<uintptr_t>(pathIds.data()+pathIds.size()));
 put(fakeMotion.data(),0x348,reinterpret_cast<uintptr_t>(pathIds.data()+pathIds.size()));
 const auto motion=reinterpret_cast<uintptr_t>(fakeMotion.data());
 nativeOccupancy=fakeOccupancy;nativeReservation=fakeReservation;nativePermissionRange=fakeRange;nativeCheck=fakeCheck;
 nativeStep=fakeStep;CHECK(stepMotion(66,11,motion,22,3.75,33)==17);
 CHECK(!currentStep.context&&!currentStep.motion);
 expiry=GetTickCount64()+100000;
 testTrain(trainId).motion=motion;testTrain(trainId).ahead={{signalId,1000}};
 rulePublishers.rows()={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP}};
 auto request=[&]{refreshRawRules();return check(0,0,0,0,0,motion,signalId,0);};
 // An absolute stop cannot inherit permission, even with an old stop proof.
 testTrain(trainId).memory.stopped={signalId,1000};CHECK(!request());
 rulePublishers.rows()[0].flags|=NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED;
 testTrain(trainId).memory.stopped={};CHECK(!request());
 testTrain(trainId).memory.stopped={signalId,1000};CHECK(request());
 CHECK(permissionQuery==nullptr);CHECK(testTrain(trainId).entry.at(1000,GetTickCount64()).distanceM==200); // Simulated visibility bound.
 obstruction=.12;CHECK(request());CHECK(testTrain(trainId).entry.at(1000,GetTickCount64()).distanceM>119.99&&testTrain(trainId).entry.at(1000,GetTickCount64()).distanceM<=120);
 obstruction=.6;
 // Regression: a stopped train may enter behind a leader even though the
 // game's exclusive reservation still covers this block. Permission is not a
 // measured passage and cannot remove the stop proof or start retained mode.
 reservationConflict=true;const auto beforeChecks=reservationChecks;CHECK(request());
 CHECK(reservationChecks>beforeChecks&&testTrain(trainId).memory.stopped.signal==signalId&&!testTrain(trainId).memory.sight);
 crossConflict=true;CHECK(!request());crossConflict=false;
 crossReservationConflict=true;CHECK(!request());crossReservationConflict=false;
 controllerConflict=true;CHECK(!request());controllerConflict=false;
 // Only the first longitudinal query is replaced, not an identical later
 // crossing query, another motion, or a call without physical observation.
 repeatFollowedRange=true;CHECK(!request());repeatFollowedRange=false;
 repeatOccupation=true;CHECK(!request());repeatOccupation=false;
 wrongReservationMotion=true;CHECK(!request());wrongReservationMotion=false;
 skipOccupation=true;CHECK(!request());skipOccupation=false;
 obstruction=.004;CHECK(!request());obstruction=.6;
 invalidGeometry=true;CHECK(!request());invalidGeometry=false;
 expiry=GetTickCount64();CHECK(!request());expiry=GetTickCount64()+100000;
 rulePublishers.rows()[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 testTrain(trainId).memory.stopped={};CHECK(request());
 rulePublishers.rows()[0]={signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP};CHECK(!request());
 // No rule is an ordinary native check, never an implicit permissive mode.
 rulePublishers.rows().clear();CHECK(!request());CHECK(permissionQuery==nullptr);
 // Outside the permission scope both native reservations and their output
 // byte are unchanged, even when the physical route is empty.
 uint8_t allowed=1;uintptr_t ownMotion=motion;
 std::array<uintptr_t,3> unscoped{2,reinterpret_cast<uintptr_t>(&ownMotion),reinterpret_cast<uintptr_t>(&allowed)};
 CHECK(!reservation(reinterpret_cast<uintptr_t>(unscoped.data()),reinterpret_cast<uintptr_t>(fakeTrack.data()),0,1)&&!allowed);
 reservationConflict=false;
 skipOccupation=true;obstruction=2;rulePublishers.rows()={{signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT}};
 CHECK(!request()); // Native approval with no physical query cannot grant entry.
 skipOccupation=false;obstruction=.6;
 // Exercise the actual integration wrapper: stop evidence comes from elapsed
 // simulated time and measured displacement, including the game's idle floor.
 std::array<unsigned char,0x100> network{},config{},result{},worker{};
 put(network.data(),0x10,reinterpret_cast<uintptr_t>(config.data()));put(config.data(),0xd0,1.25);
 put(worker.data(),8,reinterpret_cast<uintptr_t>(network.data()));put(worker.data(),0x68,uintptr_t{1});
 float material[8]{200.0f/3.6f,1,.5f,0,0,0,40000,100};
 std::memcpy(fakeMotion.data()+0x24,material,sizeof material);put(fakeMotion.data(),0x3c8,.01);
 nativeIntegrate=fakeIntegrate;
 rulePublishers.rows()={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
 auto& state=testTrain(trainId);state.managed=true;state.memory={};state.entry={};
 auto step=[&]{refreshRawRules();put(worker.data(),0x18,session);StepScope scope(reinterpret_cast<uintptr_t>(worker.data()),motion);
  integrate(reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(network.data()),motion+0x290,
  motion+0x3a0,motion+8,0,200.0/3.6,1,.5,1000,0,1,0);};
 step();CHECK(state.memory.stopped.signal==signalId);CHECK(request());
 state.memory.sight=RestrictedMode{signalId,signalId+1,1000,2000,30.0/3.6};state.ahead.clear();
 // No new scan: the native occupation query sees an obstacle appear, move,
 // then disappear. An old view is refreshed before every movement calculation.
 state.view={1000,200,200,GetTickCount64()-1000,true};obstruction=.02;step();
 CHECK(std::abs(appliedDistance-15)<1e-5&&appliedTarget==0&&appliedCeiling<=30.0/3.6);
 obstruction=.004;step();CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0);
 obstruction=.6;state.view.observed=0;step();CHECK(appliedCeiling==30.0/3.6&&appliedDistance==195);
 // No current worker scope cannot borrow another worker's old occupancy map.
 integrate(reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(network.data()),0,
  motion+0x3a0,motion+8,0,200.0/3.6,1,.5,0,15.0/3.6,1,0);
 CHECK(appliedDistance==0&&appliedTarget==0); // Equal-distance target must also tighten to zero.
 put(worker.data(),8,uintptr_t{0});step();CHECK(appliedCeiling==0);
 put(worker.data(),8,reinterpret_cast<uintptr_t>(network.data()));
 expiry=GetTickCount64();step();
 CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0);
 // A loaded train can already be waiting: no scan/integration has populated
 // its cache. Repeated callbacks during pause must not prove an actual stop.
 std::array<unsigned char,0x30> waitingSimulation{};
 session=reinterpret_cast<uintptr_t>(waitingSimulation.data());put(root.data(),0x680,session);
 expiry=GetTickCount64()+100000;resetTrains();
 put(fakeMotion.data(),0x458,signalId);put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x3c8,0.0);
 put(waitingSimulation.data(),0x28,int64_t{100});
 CHECK(!request());CHECK(!request());CHECK(!testTrain(trainId).memory.stopped.signal);
 reservationConflict=true;
 put(waitingSimulation.data(),0x28,int64_t{101});CHECK(request());
 CHECK(testTrain(trainId).memory.stopped.signal==signalId);
 CHECK(!testTrain(trainId).boundary.empty());
 reservationConflict=false;
 resetTrains();rulePublishers.rows()[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 CHECK(request()); // Red flashing requires no stop proof.
 resetTrains();put(fakeMotion.data(),0x458,signalId+1);CHECK(!request());
 CHECK(!testTrain(trainId).memory.stopped.signal); // A different waiting signal cannot seed permission.
 // Commands reach the real adapter, not just the standalone planner.
 installed=true;rawRules=false;rulePublishers.clear();resetTrains();
 NimbyTrainConstraint command{trainId,signalId,1,6.75,0,0};
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);
 auto& controlled=testTrain(trainId);controlled.motion=motion;controlled.ahead={{signalId,1200}};
 step();CHECK(appliedCeiling==6.75);
 NimbyTrainConstraintStatus status{};status.size=sizeof status;
 CHECK(NimbyDriving_ReadTrainConstraint(trainId,&status)==NIMBY_OK&&status.state==2&&status.exit_signal==signalId);
 auto bad=command;bad.speed_mps=-1;CHECK(NimbyDriving_TrainConstraints(&bad,1,1000,91)==NIMBY_INVALID_ARGUMENT);
 step();CHECK(appliedCeiling==6.75); // bad publication leaves command intact
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,92)==NIMBY_RESOURCE_LIMIT);
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,92)==NIMBY_OK);
 step();CHECK(appliedCeiling==6.75); // another publisher cannot erase this command
 put(fakeMotion.data(),0x3c0,1201.0);put(fakeMotion.data(),0x3b0,.201);step();
 CHECK(NimbyDriving_ReadTrainConstraint(trainId,&status)==NIMBY_OK&&status.state==3);
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);step();CHECK(appliedCeiling>6.75);
 command.revision=2;command.exit_signal=signalId+1;command.mode=1;
 controlled.ahead={{signalId+1,1400}};
 controlled.physicalRoute.begin(motion,trainId,1201,readBytes);
 controlled.physicalRoute.append(reinterpret_cast<uintptr_t>(fakeTrack.data()),.201,1,799,0,readBytes);
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);step();CHECK(appliedCeiling==6.75);
 controlled.physicalRoute.clear();step();CHECK(appliedCeiling==0&&appliedDistance==0);
 // An ordinary native scan rebuilds geometry for an explicit on-sight command.
 // Its context is distinct from the integrator's Network argument.
 nativeScan=fakeScan;put(worker.data(),0x18,session);
 double offset=0;std::array<uintptr_t,4> scanContext{reinterpret_cast<uintptr_t>(&offset),0,
  reinterpret_cast<uintptr_t>(worker.data()),reinterpret_cast<uintptr_t>(&motion)};
 NativeRange scanSection{reinterpret_cast<uintptr_t>(fakeTrack.data()),.201,1,799,0,0,0};
 CHECK(scan(reinterpret_cast<uintptr_t>(scanContext.data()),reinterpret_cast<uintptr_t>(&scanSection))==1);
 step();CHECK(appliedCeiling==6.75);
 // The current worker can change its map between frames; never reuse a former
 // context or require the old map address to stay alive to refresh geometry.
 put(worker.data(),0x68,uintptr_t{2});step();CHECK(appliedCeiling==6.75);
 put(worker.data(),0x68,uintptr_t{0});step();CHECK(appliedCeiling==0);
 put(worker.data(),0x68,uintptr_t{1});
 CHECK(constraintPublishers.publish(91,{command},GetTickCount64())==NIMBY_OK);
 step();CHECK(appliedCeiling>6.75);
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,91)==NIMBY_OK);
 CHECK(NimbyDriving_ReadTrainConstraint(trainId,&status)==NIMBY_OK&&status.state==0);
 // Two disjoint mod publications must coexist; the old global vector loses A.
 const NimbySignalDrivingRule ownerA{signalId,0,8,0,NIMBY_DRIVING_STOP};
 const NimbySignalDrivingRule ownerB{signalId+1,-1,8,0,NIMBY_DRIVING_CLEAR};
 CHECK(NimbyDriving_PublishV3(&ownerA,1,1000,0,101)==NIMBY_OK);
 CHECK(NimbyDriving_PublishV3(&ownerB,1,1000,0,102)==NIMBY_OK);
 CHECK(rule(rulePublishers.rows(),signalId)&&rule(rulePublishers.rows(),signalId+1));
 // A short collision with a native simulation hook is not a failed sample.
 // Retry within this call, keeping its original deadline and ownership checks.
 auto permissiveA=ownerA;permissiveA.flags=NIMBY_DRIVING_CLEAR;permissiveA.speed_mps=-1;
 CHECK(NimbyDriving_PublishV3(&permissiveA,1,1000,0,101)==NIMBY_OK);
 transientLockFailures=3;
 CHECK(NimbyDriving_PublishV3(&ownerA,1,1000,0,101)==NIMBY_OK&&transientLockFailures==0);
 CHECK(rule(rules(),signalId)->flags==NIMBY_DRIVING_STOP);
 transientLockFailures=3;
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK&&transientLockFailures==0);
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,91)==NIMBY_OK);
 // The final commit lock may now outlive the lease even when the preceding
 // world/time check passed. Neither rules nor train constraints may commit it.
 const auto beforeExpiry=rulePublishers;
 const auto beforeConstraintExpiry=constraintPublishers;
 expireOnLockAttempt=2;
 const auto expiredCommit=NimbyDriving_PublishV3(&permissiveA,1,100,0,101);tickOffset=0;
 CHECK(expiredCommit==NIMBY_DATA_UNAVAILABLE&&rulePublishers.sameVersion(beforeExpiry));
 expireOnLockAttempt=2;
 const auto expiredConstraintCommit=NimbyDriving_TrainConstraints(&command,1,100,91);tickOffset=0;
 CHECK(expiredConstraintCommit==NIMBY_DATA_UNAVAILABLE&&constraintPublishers.sameVersion(beforeConstraintExpiry));
 CHECK(NimbyDriving_PublishV3(nullptr,0,1000,0,103)==NIMBY_OK);
 CHECK(rulePublishers.rows().size()==2); // A foreign stop/release is a no-op.
 CHECK(NimbyDriving_PublishV3(&ownerA,1,1000,0,102)==NIMBY_RESOURCE_LIMIT);
 CHECK(rulePublishers.rows().size()==2&&rule(rulePublishers.rows(),signalId+1)); // Invalid batch is transactional.
 CHECK(NimbyDriving_PublishV2(&ownerA,1,1000,0)==NIMBY_RESOURCE_LIMIT);
 CHECK(NimbyDriving_PublishV2(nullptr,0,1000,0)==NIMBY_OK&&rulePublishers.rows().size()==2);
 // Expiry of B cannot poison A, nor be renewed by A's heartbeat.
 CHECK(rulePublishers.publish(102,{ownerB},GetTickCount64())==NIMBY_OK);
 Train first,second;first.ahead={{signalId,1300}};second.ahead={{signalId+1,1300}};
 CHECK(freshRule(signalId,GetTickCount64())&&!freshRule(signalId+1,GetTickCount64()));
 CHECK(freshTrain(first,1200,GetTickCount64())&&!freshTrain(second,1200,GetTickCount64()));
 CHECK(NimbyDriving_PublishV3(&ownerA,1,1000,0,101)==NIMBY_OK);
 CHECK(!freshRule(signalId+1,GetTickCount64()));
 auto& retained=testTrain(trainId);retained.ahead={{signalId,1300},{signalId+1,1400}};
 retained.memory.held={{1000,8,0,false,signalId},{1100,9,0,false,signalId+1}};
 retained.memory.sight=RestrictedMode{signalId+1,0,1100,0,9};
 CHECK(!freshTrain(retained,1200,GetTickCount64()));
 CHECK(NimbyDriving_PublishV3(nullptr,0,1000,0,102)==NIMBY_OK);
 CHECK(rulePublishers.rows().size()==1&&rule(rulePublishers.rows(),signalId));
 // Retirement is applied on this train's next access, never while a publisher
 // owns a global lock. Its source must still disappear before the next plan.
 synchronizeTrain(retained,testImage(),trainId,1200);
 CHECK(retained.memory.held.size()==1&&retained.memory.held.front().source==signalId&&!retained.memory.sight);
 CHECK(freshTrain(retained,1200,GetTickCount64()));
 CHECK(NimbyDriving_PublishV3(nullptr,0,1000,0,101)==NIMBY_OK&&rulePublishers.rows().empty());
 // Releasing A inside the native step cannot recreate A's old approach
 // when the step returns, while B keeps the shared train runtime alive.
 const NimbySignalDrivingRule announcement{signalId,0,8,1,0};
 CHECK(NimbyDriving_PublishV3(&announcement,1,1000,0,101)==NIMBY_OK);
 CHECK(NimbyDriving_PublishV3(&ownerB,1,1000,0,102)==NIMBY_OK);
 put(fakeMotion.data(),0x3c0,1000.0);
 auto& racing=testTrain(trainId);racing={};racing.motion=motion;racing.managed=true;
 racing.ahead={{signalId,1000},{signalId+1,1300}};
 releaseDuringIntegrate=101;step();CHECK(racing.memory.stops.empty()&&rule(rulePublishers.rows(),signalId+1));
 CHECK(NimbyDriving_PublishV3(nullptr,0,1000,0,102)==NIMBY_OK);
 // Disjoint train publishers coexist and have independent releases/expiry.
 auto other=command;other.train=trainId+1;
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);
 CHECK(NimbyDriving_TrainConstraints(&other,1,1000,92)==NIMBY_OK);
 CHECK(constraintPublishers.rows().size()==2);
 CHECK(constraintPublishers.publish(92,{other},GetTickCount64())==NIMBY_OK);
 CHECK(freshConstraint(trainId,GetTickCount64())&&!freshConstraint(trainId+1,GetTickCount64()));
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,92)==NIMBY_OK);
 CHECK(constraintPublishers.rows().size()==1&&constraint(trainId));
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,91)==NIMBY_OK&&constraintPublishers.rows().empty());
 // A large foreign publication can stall during a real allocation, after its
 // initial snapshot. B and native permission checks still complete meanwhile.
 CHECK(NimbyDriving_PublishV3(&ownerA,1,1000,0,101)==NIMBY_OK);
 CHECK(rulePublishers.publish(101,{ownerA},GetTickCount64())==NIMBY_OK);
 CHECK(NimbyDriving_PublishV3(&ownerB,1,5000,0,102)==NIMBY_OK);
 std::vector<NimbySignalDrivingRule> many(4096,ownerA);
 for(size_t i=0;i<many.size();++i)many[i].signal=signalId+2+i;
 const auto independent=gatedPublication([&]{return NimbyDriving_PublishV3(many.data(),static_cast<uint32_t>(many.size()),5000,0,101);},[&]{
  const auto status=NimbyDriving_PublishV3(&ownerB,1,5000,0,102);
  if(status!=NIMBY_OK||freshRule(signalId,GetTickCount64())||request()!=0)return uint32_t(NIMBY_INTERNAL_ERROR);
  return uint32_t(NIMBY_OK);
 });
 CHECK(independent.independent&&independent.healthy==NIMBY_OK&&independent.delayed==NIMBY_OK);
 CHECK(rule(rules(),signalId+1)&&rules().size()==many.size()+1);
 // A stalled request cannot obtain a fresh lease when it eventually returns.
 const auto expired=gatedPublication([&]{return NimbyDriving_PublishV3(&ownerA,1,100,0,101);},[&]{return NimbyDriving_PublishV3(&ownerB,1,5000,0,102);},150);
 CHECK(expired.independent&&expired.healthy==NIMBY_OK&&expired.delayed==NIMBY_DATA_UNAVAILABLE);
 CHECK(!rule(rules(),signalId));
 // A newer release from the same owner wins over an older prepared update.
 const auto removed=gatedPublication([&]{return NimbyDriving_PublishV3(&ownerA,1,5000,0,101);},[&]{return NimbyDriving_PublishV3(nullptr,0,1000,0,101);});
 CHECK(removed.independent&&removed.healthy==NIMBY_OK&&removed.delayed==NIMBY_RESOURCE_LIMIT);
 CHECK(rulePublishers.ownedRows(101).empty()&&rule(rules(),signalId+1));
 // Hooks and readbacks never wait on publication/native-state contention.
 testTrain(trainId)=RuntimeTrain{};testTrain(trainId).motion=motion;
 obstruction=2;
 std::promise<void> locked,unlock;auto unlockFuture=unlock.get_future().share();
 std::thread holder([&]{AcquireSRWLockExclusive(&publications.writers);locked.set_value();unlockFuture.wait();ReleaseSRWLockExclusive(&publications.writers);});
 locked.get_future().wait();
 const auto previousPublication=rulePublishers;
 auto publisher=std::async(std::launch::async,[&]{
  const auto began=std::chrono::steady_clock::now();
  const auto signalStatus=NimbyDriving_PublishV3(&ownerB,1,5000,0,102);
  const auto trainStatus=NimbyDriving_TrainConstraints(&command,1,5000,91);
  const auto elapsed=std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-began).count();
  std::cout<<"{\"scenario\":\"permanently_contended_publication\",\"us\":"<<elapsed<<"}\n";
  return signalStatus==NIMBY_RESOURCE_LIMIT&&trainStatus==NIMBY_RESOURCE_LIMIT;
 });
 const bool publicationBounded=publisher.wait_for(std::chrono::milliseconds(200))==std::future_status::ready;
 auto native=std::async(std::launch::async,[&]{const auto calls=nativePermissionCalls;const auto allowed=request();step();NimbyTrainConstraintStatus value{};value.size=sizeof value;
  return allowed==1&&nativePermissionCalls==calls+1&&appliedCeiling==200.0/3.6&&appliedDistance==1000&&NimbyDriving_ReadTrainConstraint(trainId,&value)==NIMBY_OK;});
 const bool nativeIndependent=native.wait_for(std::chrono::milliseconds(200))==std::future_status::ready;
 unlock.set_value();holder.join();CHECK(nativeIndependent&&native.get());
 CHECK(publicationBounded&&publisher.get()&&rulePublishers.sameVersion(previousPublication));
 // A different world arriving during preparation invalidates the old batch.
 const auto switched=gatedPublication([&]{return NimbyDriving_PublishV3(&ownerA,1,5000,0,101);},[&]{
  put(root.data(),0x680,uintptr_t{0x87650000});return NimbyDriving_PublishV3(&ownerB,1,5000,0,102);
 });
 CHECK(switched.independent&&switched.healthy==NIMBY_OK&&switched.delayed==NIMBY_DATA_UNAVAILABLE);
 CHECK(rulePublishers.ownedRows(101).empty()&&rules().size()==1);
 // A stalled world read predates B's reset. It cannot roll the shared session
 // back or erase B's new-world rules/constraints when the old read returns.
 const auto lateWorld=gatedPublication([&]{pauseAllocationAfter=0;pauseReadAt=root.data()+0x680;
  return NimbyDriving_PublishV3(&ownerA,1,5000,0,101);},[&]{
  put(root.data(),0x680,uintptr_t{0x87660000});return NimbyDriving_PublishV3(&ownerB,1,5000,0,102);
 });
 CHECK(lateWorld.independent&&lateWorld.healthy==NIMBY_OK&&lateWorld.delayed==NIMBY_DATA_UNAVAILABLE);
 CHECK(session==0x87660000&&rules().size()==1&&rule(rules(),signalId+1));
 const auto lateConstraints=gatedPublication([&]{pauseAllocationAfter=0;pauseReadAt=root.data()+0x680;
  return NimbyDriving_TrainConstraints(&command,1,5000,91);},[&]{
  put(root.data(),0x680,uintptr_t{0x87670000});return NimbyDriving_TrainConstraints(&other,1,5000,92);
 });
 CHECK(lateConstraints.independent&&lateConstraints.healthy==NIMBY_OK&&lateConstraints.delayed==NIMBY_DATA_UNAVAILABLE);
 CHECK(session==0x87670000&&constraints().size()==1&&constraint(other.train));
 // A is suspended INSIDE native integration with its state held. B and C's
 // publication must complete before A resumes; only a concurrent A is denied.
 put(root.data(),0x680,reinterpret_cast<uintptr_t>(waitingSimulation.data()));
 CHECK(NimbyDriving_PublishV3(&announcement,1,5000,0,101)==NIMBY_OK);
 CHECK(NimbyDriving_PublishV3(&ownerB,1,5000,0,102)==NIMBY_OK);
 constexpr auto peerId=trainId+0x10000,unmanagedId=trainId+0x20000;
 auto peerMotion=fakeMotion,unmanagedMotion=fakeMotion;
 put(peerMotion.data(),0,peerId);put(unmanagedMotion.data(),0,unmanagedId);
 put(fakeMotion.data(),0x3c0,1000.0);put(peerMotion.data(),0x3c0,1000.0);
 auto& crossing=testTrain(trainId);crossing={};crossing.motion=motion;crossing.ahead={{signalId,1000},{signalId+1,1400}};
 auto& peerState=testTrain(peerId);peerState.motion=reinterpret_cast<uintptr_t>(peerMotion.data());peerState.ahead={{signalId+1,1400}};
 const auto runMotion=[&](uintptr_t object){std::array<unsigned char,0x100> ownResult{};
  integrate(reinterpret_cast<uintptr_t>(ownResult.data()),reinterpret_cast<uintptr_t>(network.data()),object+0x290,
   object+0x3a0,object+8,0,50,1,.5,700,7,1,0);
  return std::array<double,3>{appliedCeiling,appliedDistance,appliedTarget};};
 obstruction=2;reservationConflict=false;crossConflict=false;crossReservationConflict=false;controllerConflict=false;
 nativeEntered=CreateEventW(nullptr,TRUE,FALSE,nullptr);nativeRelease=CreateEventW(nullptr,TRUE,FALSE,nullptr);CHECK(nativeEntered&&nativeRelease);
 auto busyA=std::async(std::launch::async,[&]{pauseNative=true;reenterNative=true;
  duringNative=[](uintptr_t object){put(reinterpret_cast<void*>(object),0x3c0,1001.0);};
  return runMotion(motion);});
 CHECK(WaitForSingleObject(nativeEntered,2000)==WAIT_OBJECT_0);
 auto peerB=std::async(std::launch::async,[&]{return runMotion(reinterpret_cast<uintptr_t>(peerMotion.data()));});
 const NimbySignalDrivingRule ownerC{signalId+2,-1,8,0,NIMBY_DRIVING_CLEAR};
 auto publisherC=std::async(std::launch::async,[&]{return NimbyDriving_PublishV3(&ownerC,1,5000,0,103);});
 const bool peerReady=peerB.wait_for(std::chrono::milliseconds(200))==std::future_status::ready;
 const bool publisherReady=publisherC.wait_for(std::chrono::milliseconds(200))==std::future_status::ready;
 const auto sameTrain=runMotion(motion),outside=runMotion(reinterpret_cast<uintptr_t>(unmanagedMotion.data()));
 const auto nativeCalls=nativePermissionCalls;
 const auto outsidePermission=check(0,0,0,0,0,reinterpret_cast<uintptr_t>(unmanagedMotion.data()),signalId+1234,0);
 const auto samePermission=request();
 SetEvent(nativeRelease);busyA.get();const auto peerResult=peerB.get();const auto publisherResult=publisherC.get();
 CloseHandle(nativeEntered);CloseHandle(nativeRelease);nativeEntered=nativeRelease=nullptr;
 CHECK(peerReady&&publisherReady&&publisherResult==NIMBY_OK);
 CHECK((peerResult==std::array<double,3>{50,700,7})&&(outside==std::array<double,3>{50,700,7}));
 CHECK((sameTrain==std::array<double,3>{0,0,0})&&samePermission==0&&outsidePermission==1&&nativePermissionCalls==nativeCalls+1);
 CHECK(crossing.memory.stops.size()==1&&crossing.memory.stops[0].signal==signalId+1); // NEW crossing was committed.
 // A same-owner release/re-add while native motion executes is a NEW lifetime.
 crossing.memory={};crossing.boundary.clear();put(fakeMotion.data(),0x3c0,1000.0);
 duringNative=[&](uintptr_t object){
  CHECK(NimbyDriving_PublishV3(nullptr,0,5000,0,101)==NIMBY_OK);
  CHECK(NimbyDriving_PublishV3(&announcement,1,5000,0,101)==NIMBY_OK);
  put(reinterpret_cast<void*>(object),0x3c0,1001.0);
 };
 runMotion(motion);duringNative={};CHECK(crossing.memory.stops.empty());
 // World replacement during a native call cannot commit into its new state.
 const auto oldWorld=testImage().world;crossing.memory={};put(fakeMotion.data(),0x3c0,1000.0);
 duringNative=[&](uintptr_t object){put(root.data(),0x680,uintptr_t{0x98760000});
  CHECK(NimbyDriving_PublishV3(&announcement,1,5000,0,101)==NIMBY_OK);put(reinterpret_cast<void*>(object),0x3c0,1001.0);};
 runMotion(motion);duringNative={};CHECK(crossing.memory.stops.empty()&&testImage().world!=oldWorld);
 // The native pass-through decision is not a cached negative result. A newly
 // published rule is found by the next scan before the interest filter runs.
 CHECK((runMotion(reinterpret_cast<uintptr_t>(unmanagedMotion.data()))==std::array<double,3>{50,700,7}));
 std::array<unsigned char,0x38> sourceSignal{};put(sourceSignal.data(),0,signalId);put(sourceSignal.data(),0x30,4);
 uintptr_t newMotion=reinterpret_cast<uintptr_t>(unmanagedMotion.data());double newOffset=0;
 put(worker.data(),0x18,session);
 std::array<uintptr_t,4> newScanContext{reinterpret_cast<uintptr_t>(&newOffset),0,reinterpret_cast<uintptr_t>(worker.data()),reinterpret_cast<uintptr_t>(&newMotion)};
 NativeRange newSource{reinterpret_cast<uintptr_t>(fakeTrack.data()),0,0,50,6,0,reinterpret_cast<uintptr_t>(sourceSignal.data())};
 CHECK(scan(reinterpret_cast<uintptr_t>(newScanContext.data()),reinterpret_cast<uintptr_t>(&newSource))==1);
 CHECK(testImage().world->isManaged(unmanagedId));
 CHECK(runMotion(newMotion)[0]<50);
 // Exhaust the real registry without marking the unrelated train's identity.
 // Interest, rather than a failed allocation, decides native pass-through.
 resetTrains();auto& fullWorld=*testImage().world;
 for(size_t i=0;i<TrainWorld::capacity;++i)fullWorld.slots[i].key.store(100000+i);
 CHECK(!fullWorld.isManaged(unmanagedId));
 CHECK((runMotion(reinterpret_cast<uintptr_t>(unmanagedMotion.data()))==std::array<double,3>{50,700,7}));
 fullWorld.markManaged(trainId);CHECK((runMotion(motion)==std::array<double,3>{0,0,0}));
 std::cout<<"{\"worldBytes\":"<<sizeof(TrainWorld)<<",\"trainBytes\":"<<sizeof(RuntimeTrain)<<"}\n";
 std::cout<<"PASS: permission, measured stop, reservations, geometry, lease, scope and native train commands\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
