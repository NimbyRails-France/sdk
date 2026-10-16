// Exercise the actual bridge wrappers with controlled native callbacks. This
// verifies permission scoping; it does not validate game addresses or ABI.
#include <windows.h>
BOOL WINAPI gatedReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
BOOLEAN WINAPI gatedTryAcquireSRWLockExclusive(PSRWLOCK);
ULONGLONG WINAPI shiftedGetTickCount64();
BOOL WINAPI fixturePerformanceCounter(LARGE_INTEGER*);
BOOL WINAPI fixturePerformanceFrequency(LARGE_INTEGER*);
uintptr_t fixtureNativeCaller();
#define NRF_PHASE_CALLER() fixtureNativeCaller()
#define ReadProcessMemory gatedReadProcessMemory
#define TryAcquireSRWLockExclusive gatedTryAcquireSRWLockExclusive
#define GetTickCount64 shiftedGetTickCount64
#define QueryPerformanceCounter fixturePerformanceCounter
#define QueryPerformanceFrequency fixturePerformanceFrequency
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#undef ReadProcessMemory
#undef TryAcquireSRWLockExclusive
#undef GetTickCount64
#undef QueryPerformanceCounter
#undef QueryPerformanceFrequency
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
thread_local bool routeClockFixture=false;
thread_local int64_t routeClockTick=1000000,routeReadCost=0,nativeCheckCost=0,nativeRangeCost=0,nativeReservationCost=0;
thread_local unsigned routeClockCalls=0;
thread_local uintptr_t fixtureCaller{};
thread_local bool countDiscoveryWork=false;
thread_local size_t discoveryReads{},discoveryBytes{},discoveryAllocations{};
thread_local uintptr_t discoveryPathAddress{};
thread_local size_t discoveryPathHeaders{};
HANDLE allocationEntered{},allocationRelease{};
void pauseAllocation(){if(pauseAllocationAfter&&!--pauseAllocationAfter){SetEvent(allocationEntered);WaitForSingleObject(allocationRelease,10000);}}
}
uintptr_t fixtureNativeCaller(){return fixtureCaller;}
BOOL WINAPI gatedReadProcessMemory(HANDLE process,LPCVOID address,LPVOID output,SIZE_T size,SIZE_T* copied){
 if(countDiscoveryWork){++discoveryReads;discoveryBytes+=size;
  if(reinterpret_cast<uintptr_t>(address)==discoveryPathAddress&&size==0xc0)++discoveryPathHeaders;}
 const auto result=ReadProcessMemory(process,address,output,size,copied);
 if(routeClockFixture&&routeWorkBudget&&routeWorkBudget->active)routeClockTick+=routeReadCost;
 if(address==pauseReadAt){pauseReadAt=nullptr;SetEvent(allocationEntered);WaitForSingleObject(allocationRelease,10000);}
 return result;
}
BOOLEAN WINAPI gatedTryAcquireSRWLockExclusive(PSRWLOCK lock){
 if(expireOnLockAttempt&&!--expireOnLockAttempt){tickOffset=200;return FALSE;}
 if(transientLockFailures){--transientLockFailures;return FALSE;}
 return TryAcquireSRWLockExclusive(lock);
}
ULONGLONG WINAPI shiftedGetTickCount64(){return GetTickCount64()+tickOffset;}
BOOL WINAPI fixturePerformanceCounter(LARGE_INTEGER* value){++routeClockCalls;
 if(!routeClockFixture)return QueryPerformanceCounter(value);
 value->QuadPart=routeClockTick;return TRUE;
}
BOOL WINAPI fixturePerformanceFrequency(LARGE_INTEGER* value){++routeClockCalls;
 if(!routeClockFixture)return QueryPerformanceFrequency(value);
 value->QuadPart=1000000;return TRUE;
}
#if defined(__GNUC__)
#define TEST_NOINLINE __attribute__((noinline))
#else
#define TEST_NOINLINE __declspec(noinline)
#endif
TEST_NOINLINE void* operator new(std::size_t size){if(countDiscoveryWork)++discoveryAllocations;pauseAllocation();if(auto* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
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
std::array<unsigned char,0x500> fakeTrack{},crossTrack{};
std::array<unsigned char,0x80> fakeSourceSignal{};
std::array<uint64_t,1> pathIds{trackId};
double obstruction=.6;
bool reservationConflict=false,crossConflict=false,crossReservationConflict=false,controllerConflict=false,invalidGeometry=false;
bool repeatFollowedRange=false,repeatOccupation=false,skipOccupation=false,wrongReservationMotion=false;
int reservationChecks=0;
thread_local unsigned nativePermissionCalls=0;
thread_local double appliedDistance=-1,appliedTarget=-1,appliedCeiling=-1;
thread_local bool pauseNative=false,reenterNative=false;
thread_local bool simulateProjector=false;
thread_local bool permissionDuringScan=false;
thread_local uint8_t scanPermissionResult=0;
thread_local unsigned integratedCallbacks=0,projectedCallbacks=0;
thread_local unsigned extraOccupationQueries=0;
thread_local double simulatedMovement=0,lastProjectionDistance=0;
thread_local double nativeEventHead=std::numeric_limits<double>::infinity();
thread_local double permissionSourceFraction=0;
thread_local bool permissionHasApproach=false;
thread_local int nativeEventKind=6;
thread_local std::function<void(uintptr_t)> duringNative;
thread_local std::function<void()> duringOccupation;
thread_local std::function<void(uintptr_t)> duringStep;
thread_local uintptr_t fakeStepTrainArgument=11;
thread_local bool publishFinalStepMirror=false;
HANDLE nativeEntered{},nativeRelease{};
uint64_t releaseDuringIntegrate=0;
template<class T> void put(void* at,size_t offset,T value){std::memcpy(static_cast<unsigned char*>(at)+offset,&value,sizeof value);}
uint32_t __fastcall fakeStep(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget){
 CHECK(currentStep.context==context&&currentStep.motion==motion);
 const auto callback=duringStep;if(callback)callback(motion);
 CHECK(train==fakeStepTrainArgument&&service==22&&mass==3.75&&budget==33);
 {StepScope nested(44,55);CHECK(currentStep.context==44&&currentStep.motion==55);}
 CHECK(currentStep.context==context&&currentStep.motion==motion);
 if(publishFinalStepMirror)
  std::memcpy(reinterpret_cast<void*>(motion+0xb8),reinterpret_cast<const void*>(motion+0x3a8),0x18);
 return 17;
}
uintptr_t __fastcall fakeScan(uintptr_t context,uintptr_t range){
 if(permissionDuringScan){
  int kind{};uintptr_t signalPtr{},motionRef{},motion{};uint64_t signal{};
  if(read(range+0x20,kind)&&kind==6&&read(range+0x28,signalPtr)&&read(signalPtr,signal)&&
     read(context+0x18,motionRef)&&read(motionRef,motion))
   scanPermissionResult=check(0,0,0,0,0,motion,signal,1);
 }
 return 1;
}
uintptr_t __fastcall fakeProject(uintptr_t result,uintptr_t,uintptr_t,uintptr_t presence,double distance,uintptr_t){
 ++projectedCallbacks;lastProjectionDistance=distance;
 auto& cursor=*reinterpret_cast<ProjectionPresence*>(presence);
 const auto length=*reinterpret_cast<double*>(fakeTrack.data()+0x88);
 const auto stop=std::max(0.0,nativeEventHead-cursor.head);
 const auto travelled=std::min(distance,stop);
 const int kind=distance>=stop?nativeEventKind:8;
 const double from=cursor.fraction,to=from+travelled/length*cursor.direction;
 const ProjectionRange range{reinterpret_cast<uintptr_t>(fakeTrack.data()),from,to,travelled,kind,0,kind==6?signalId:0};
 std::memcpy(reinterpret_cast<void*>(result),&range,sizeof range);
 cursor.head+=travelled;cursor.fraction=std::nextafter(to,cursor.direction==1?1.0:0.0);
 if(kind==2)++cursor.index;
 return result;
}
uintptr_t __fastcall fakeIntegrate(uintptr_t result,uintptr_t network,uintptr_t path,uintptr_t presence,uintptr_t dynamics,double,
 double ceiling,double,double,double distance,double target,int64_t ticks,uintptr_t) {
 ++integratedCallbacks;
 appliedDistance=distance;appliedTarget=target;appliedCeiling=ceiling;
 if(pauseNative){SetEvent(nativeEntered);WaitForSingleObject(nativeRelease,2000);}
 if(reenterNative){reenterNative=false;CHECK(check(0,0,0,0,0,dynamics-8,signalId+1,0)==1);}
 if(duringNative)duringNative(dynamics-8);
 if(releaseDuringIntegrate){
  const auto owner=releaseDuringIntegrate;releaseDuringIntegrate=0;
  CHECK(NimbyDriving_PublishV3(nullptr,0,1000,0,owner)==NIMBY_OK);
  put(fakeMotion.data(),0x3c0,1001.0);
 }
 if(simulateProjector){
  // Mirror the two qualified native projector sites: first a scratch copy,
  // then real Presence only when this movement ends before the projected event.
  ProjectionPresence projected{};std::memcpy(&projected,reinterpret_cast<void*>(presence),sizeof projected);
  ProjectionRange first{};
  project(reinterpret_cast<uintptr_t>(&first),network+0x28,path,reinterpret_cast<uintptr_t>(&projected),99999999999.0,0);
  put(reinterpret_cast<void*>(presence),0x28,ceiling==0?.01:std::max(.01,ceiling));
  const auto movement=std::max(.0001,simulatedMovement);
  if(movement<first.length)project(result,network+0x28,path,presence,movement,0);
  else {
   std::memcpy(reinterpret_cast<void*>(presence),&projected,sizeof projected);
   std::memcpy(reinterpret_cast<void*>(result),&first,sizeof first);ticks=1;
  }
 }
 put(reinterpret_cast<void*>(result),0x30,ticks);return result;
}
struct NativeRange {uintptr_t track;double from,to,length;int kind,padding;uintptr_t signal;};
uint8_t __fastcall fakeOccupancy(uintptr_t context,uintptr_t track,double from,double to) {
 if(routeClockFixture&&routeWorkBudget&&routeWorkBudget->active)++extraOccupationQueries;
 if(duringOccupation){auto callback=std::move(duringOccupation);callback();}
 const bool occupied=track==reinterpret_cast<uintptr_t>(crossTrack.data())?crossConflict:
  std::max(from,to)>=obstruction&&std::min(from,to)<=.9;
 if(occupied)*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t*>(context)[2])=0;
 return !occupied;
}
uintptr_t __fastcall fakeRange(uintptr_t context,uintptr_t range) {
 if(routeClockFixture)routeClockTick+=nativeRangeCost;
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
 if(routeClockFixture)routeClockTick+=nativeReservationCost;
 ++reservationChecks;
 const bool conflict=track==reinterpret_cast<uintptr_t>(crossTrack.data())?crossReservationConflict:reservationConflict;
 if(conflict)*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t*>(context)[2])=0;
 return !conflict;
}
uint8_t __fastcall fakeCheck(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t motion,uint64_t signal,uint8_t) {
 if(routeClockFixture)routeClockTick+=nativeCheckCost;
 ++nativePermissionCalls;
 uint8_t allowed=1;std::array<uintptr_t,3> occ{1,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&allowed)};
 uintptr_t reservationMotion=wrongReservationMotion?motion+1:motion;
 std::array<uintptr_t,3> reserved{2,reinterpret_cast<uintptr_t>(&reservationMotion),reinterpret_cast<uintptr_t>(&allowed)};
 std::array<uintptr_t,10> ctx{};ctx[3]=reinterpret_cast<uintptr_t>(occ.data());ctx[8]=reinterpret_cast<uintptr_t>(&allowed);
 ctx[4]=reinterpret_cast<uintptr_t>(reserved.data());
 if(permissionHasApproach){
  double fraction{};CHECK(read(motion+0x3b0,fraction));
  NativeRange approach{reinterpret_cast<uintptr_t>(fakeTrack.data()),fraction,permissionSourceFraction,
   std::abs(permissionSourceFraction-fraction)*1000,0,0,0};
  permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&approach));
 }
 NativeRange source{reinterpret_cast<uintptr_t>(fakeTrack.data()),permissionSourceFraction,permissionSourceFraction,0,6,0,reinterpret_cast<uintptr_t>(&signal)};
 permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&source));
 NativeRange section{source.track,permissionSourceFraction,1,invalidGeometry?50.0:(1-permissionSourceFraction)*1000,0,0,0};
 permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&section));
 return allowed;
}
void captureSourceApproach(RuntimeTrain& state,uintptr_t motion,double source){
 double head{},fraction{};CHECK(read(motion+0x3c0,head)&&read(motion+0x3b0,fraction));
 permissionSourceFraction=fraction+(source-head)/1000;
 put(fakeSourceSignal.data(),0,signalId);put(fakeSourceSignal.data(),0x30,int32_t{NIMBY_SIGNAL_PATH});
 put(fakeSourceSignal.data(),0x40,trackId);put(fakeSourceSignal.data(),0x48,permissionSourceFraction);
 put(fakeSourceSignal.data(),0x50,int8_t{-1});
 state.signalRoute.begin(motion,trainId,head,GetTickCount64(),readBytes);
 state.signalRoute.append(reinterpret_cast<uintptr_t>(fakeTrack.data()),fraction,permissionSourceFraction,source-head,0,readBytes);
 state.signalRoute.boundary(reinterpret_cast<uintptr_t>(fakeSourceSignal.data()),signalId,source-head,readBytes);
}
void rescanSourceApproach(uintptr_t motion,uintptr_t worker,double source){
 double head{},fraction{};CHECK(read(motion+0x3c0,head)&&read(motion+0x3b0,fraction));
 double offset=0;std::array<uintptr_t,4> context{reinterpret_cast<uintptr_t>(&offset),0,worker,reinterpret_cast<uintptr_t>(&motion)};
 NativeRange marker{reinterpret_cast<uintptr_t>(fakeTrack.data()),fraction,permissionSourceFraction,source-head,6,0,reinterpret_cast<uintptr_t>(fakeSourceSignal.data())};
 CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&marker))==1);
 offset=source-head;
 NativeRange section{marker.track,permissionSourceFraction,1,(1-permissionSourceFraction)*1000,8,0,0};
 CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&section))==1);
}
// The transactional prototype qualifies after the real step wrapper returns,
// including its conserved Pos mirror. A bare StepScope deliberately does not
// stand in for that completed native caller in the tests below.
template<class Call> void runCommittedStep(uintptr_t worker,uintptr_t motion,Call&& call){
 alignas(8) std::array<unsigned char,0x178> dbTrain{};uint64_t id{};CHECK(read(motion,id));put(dbTrain.data(),0,id);
 auto previousCallback=std::move(duringStep);
 const auto previousTrain=fakeStepTrainArgument;const auto previousMirror=publishFinalStepMirror;
 struct Restore {
  std::function<void(uintptr_t)> callback;uintptr_t train;bool mirror;
  ~Restore(){duringStep=std::move(callback);fakeStepTrainArgument=train;publishFinalStepMirror=mirror;}
 }restore{std::move(previousCallback),previousTrain,previousMirror};
 fakeStepTrainArgument=reinterpret_cast<uintptr_t>(dbTrain.data());publishFinalStepMirror=true;
 duringStep=[&](uintptr_t object){call(object);};
 CHECK(stepMotion(worker,fakeStepTrainArgument,motion,22,3.75,33)==17);
}
template<size_t Count> void scanCommitSources(uintptr_t motion,uintptr_t worker,
 std::span<const Ahead> ahead,std::array<std::array<unsigned char,0x80>,Count>& sources){
 CHECK(ahead.size()<=Count);double head{},fraction{};
 CHECK(read(motion+0x3c0,head)&&read(motion+0x3b0,fraction));
 double offset=0,from=fraction;
 std::array<uintptr_t,4> context{reinterpret_cast<uintptr_t>(&offset),0,worker,reinterpret_cast<uintptr_t>(&motion)};
 for(size_t i=0;i<ahead.size();++i){
  const auto sourceFraction=fraction+(ahead[i].position-head)/1000;
  CHECK(sourceFraction>=from&&sourceFraction<=1);
  auto& source=sources[i];source.fill(0);put(source.data(),0,ahead[i].signal);
  put(source.data(),0x30,int32_t{NIMBY_SIGNAL_PATH});put(source.data(),0x40,trackId);
  put(source.data(),0x48,sourceFraction);put(source.data(),0x50,int8_t{-1});put(source.data(),0x51,int8_t{0});
  const double length=(sourceFraction-from)*1000;
  NativeRange marker{reinterpret_cast<uintptr_t>(fakeTrack.data()),from,sourceFraction,length,6,0,reinterpret_cast<uintptr_t>(source.data())};
  CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&marker))==1);
  offset+=length;from=sourceFraction;
 }
 NativeRange remainder{reinterpret_cast<uintptr_t>(fakeTrack.data()),from,1,(1-from)*1000,8,0,0};
 CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&remainder))==1);
}
void checkSignalProjectionFence(uintptr_t motion,uintptr_t network,uintptr_t result){
 const auto savedMotion=fakeMotion;const auto savedRules=rulePublishers.rows();
 nativeProject=fakeProject;nativeScan=fakeScan;CHECK(drivingPreferences.open());
 // The hook bounds only the projector for this exact pool/path and captured
 // Presence. Stack-copy and real-Presence sites both use the same start.
 ProjectionPresence cursor{0,0,trackId,.4,1,0,{},1000};
 ProjectionFence fence{network+0x28,motion+0x290,cursor,1007,0};
 ProjectionRange projected{};
 const auto projectAt=[&](uintptr_t pool,uintptr_t path){cursor=fence.start;
  project(reinterpret_cast<uintptr_t>(&projected),pool,path,reinterpret_cast<uintptr_t>(&cursor),20,0);};
 projectAt(fence.context,fence.path);CHECK(lastProjectionDistance==20);
 {ProjectionScope scope(&fence);
  projectAt(fence.context,fence.path);CHECK(lastProjectionDistance==7&&cursor.head==1007&&projected.kind==8);
  projectAt(fence.context+8,fence.path);CHECK(lastProjectionDistance==20);
  projectAt(fence.context,fence.path+8);CHECK(lastProjectionDistance==20);
  cursor=fence.start;cursor.head+=1;
  project(reinterpret_cast<uintptr_t>(&projected),fence.context,fence.path,reinterpret_cast<uintptr_t>(&cursor),20,0);
  CHECK(lastProjectionDistance==20); // Another cursor cannot borrow this fence.
  {ProjectionScope nested(nullptr);projectAt(fence.context,fence.path);CHECK(lastProjectionDistance==20);}
  projectAt(fence.context,fence.path);CHECK(lastProjectionDistance==7);
  auto peer=std::async(std::launch::async,[&]{ProjectionPresence other=fence.start;ProjectionRange output{};
   project(reinterpret_cast<uintptr_t>(&output),fence.context,fence.path,reinterpret_cast<uintptr_t>(&other),20,0);
   return lastProjectionDistance;});CHECK(peer.get()==20);
 }
 CHECK(projectionFence==nullptr);
 // Route metres still increase when the track fraction decreases.
 fence.start.direction=-1;fence.start.fraction=.6;cursor=fence.start;
 {ProjectionScope scope(&fence);projectAt(fence.context,fence.path);CHECK(projected.from>projected.to&&cursor.head==1007);}
 fence.start.direction=1;fence.start.fraction=.4;fence.stopHead=fence.start.head;
 for(const int kind:{0,1,2,6,7}){
  nativeEventHead=fence.start.head;nativeEventKind=kind;
  {ProjectionScope scope(&fence);projectAt(fence.context,fence.path);CHECK(projected.kind==kind&&projected.length==0);}
 }
 nativeEventHead=std::numeric_limits<double>::infinity();nativeEventKind=6;
 cursor=fence.start;
 {ProjectionScope scope(&fence);projectAt(fence.context,fence.path);CHECK(projected.kind==8&&projected.length==0&&cursor.head==1000);}
 // Even an unmanaged/invalid nested integration hides an outer fence.
 simulateProjector=true;simulatedMovement=2;cursor=fence.start;
 {ProjectionScope scope(&fence);integrate(result,network,fence.path,reinterpret_cast<uintptr_t>(&cursor),0,
   0,10,1,.5,1000,0,10,0);CHECK(lastProjectionDistance==2&&projectionFence==&fence);}
 // End-to-end bridge: a large native movement cannot cross the SDK stop
 // point. One native callback then consumes the whole stationary idle budget.
 for(const int margin:{3,0,12,100}){
  CHECK(drivingPreferences.set(margin));resetTrains();fakeMotion=savedMotion;
  put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,.1);
  put(fakeMotion.data(),0x3c0,900.0);put(fakeMotion.data(),0x3c8,0.0);put(fakeMotion.data(),0x3d0,0.0);
  auto& state=testTrain(trainId);state.motion=motion;state.managed=true;state.ahead={{signalId,1000}};
  rulePublishers.rows()={{signalId,0,8,0,NIMBY_DRIVING_STOP}};refreshRawRules();
  simulatedMovement=500;integratedCallbacks=0;
  const auto step=[&](int64_t budget=1000){integrate(result,network,motion+0x290,motion+0x3a0,motion+8,
   0,200.0/3.6,1,.5,1000,0,budget,0);};
  step();double head{};CHECK(read(motion+0x3c0,head)&&head==1000-margin);
  // The target is the true signal; no synthetic passage at its 3 m offset.
  CHECK(state.memory.held.empty()&&!state.memory.sight);
  int64_t remaining=1000,accounted=0;unsigned callerIterations=0;
  do {step(remaining);int64_t consumed{};CHECK(read(result+0x30,consumed)&&consumed>0&&consumed<=remaining);
   accounted+=consumed;remaining-=consumed;++callerIterations;CHECK(callerIterations<3);
  }while(remaining);
  int64_t elapsed{};ProjectionRange output{};
  CHECK(read(result+0x30,elapsed)&&elapsed==1000&&read(result,output)&&output.kind==8&&output.length==0);
  CHECK(callerIterations==1&&accounted==1000); // Simulated time is consumed once.
  CHECK(integratedCallbacks==2&&read(motion+0x3c0,head)&&head==1000-margin);
  CHECK(projectionFence==nullptr);
  // Pending reversal/overlap and real native events keep their original timing.
  put(fakeMotion.data(),0x3d0,.5);step();CHECK(read(result+0x30,elapsed)&&elapsed==1);
  put(fakeMotion.data(),0x3d0,0.0);nativeEventHead=head;nativeEventKind=6;
  step();CHECK(read(result,output)&&output.kind==6&&read(result+0x30,elapsed)&&elapsed==1);
  nativeEventHead=std::numeric_limits<double>::infinity();
  // Reopening removes the fence, including its artificial stationary ceiling.
  rulePublishers.rows()[0]={signalId,-1,8,0,NIMBY_DRIVING_CLEAR};refreshRawRules();simulatedMovement=1;
  step();double advanced{};CHECK(read(motion+0x3c0,advanced)&&advanced>head);
 }
 // STOP_THEN_PROCEED proves a stop 3 m before the true signal, then permits
 // restricted entry without relocating its boundary or crossing it early.
 CHECK(drivingPreferences.set(3));resetTrains();fakeMotion=savedMotion;
 put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,.197);
 put(fakeMotion.data(),0x3c0,997.0);put(fakeMotion.data(),0x3d0,0.0);
 auto& stopped=testTrain(trainId);stopped.motion=motion;stopped.managed=true;stopped.ahead={{signalId,1000}};
 rulePublishers.rows()={{signalId,0,8,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
 refreshRawRules();integrate(result,network,motion+0x290,motion+0x3a0,motion+8,0,10,1,.5,1000,0,1000,0);
 CHECK(stopped.memory.stopped.signal==signalId&&stopped.memory.stopped.position==1000);
 CHECK(!stopped.memory.sight);
 captureSourceApproach(stopped,motion,1000);
 CHECK(check(0,0,0,0,0,motion,signalId,0)==1);
 CHECK(!stopped.memory.sight&&!stopped.boundary.empty()&&stopped.boundary.front().source.position==1000);
 // An event or non-stationary result must never receive synthetic idle time.
 ProjectionFence idle{network+0x28,motion+0x290,{},997,1,motion,trainId};CHECK(read(motion+0x3a0,idle.start));
 ProjectionRange zero{reinterpret_cast<uintptr_t>(fakeTrack.data()),idle.start.fraction,idle.start.fraction,0,8,0,0};
 const auto idleTest=[&](ProjectionRange candidate,int64_t ticks,double overlap,double speed){
  put(reinterpret_cast<void*>(result),0,candidate);put(reinterpret_cast<void*>(result),0x30,ticks);
  put(fakeMotion.data(),0x3d0,overlap);put(fakeMotion.data(),0x3c8,speed);
  consumeFencedIdle(result,result,motion+0x3a0,1000,idle,0);int64_t actual{};CHECK(read(result+0x30,actual));return actual;};
 CHECK(idleTest(zero,1,0,.01)==1000);
 for(int kind:{0,1,2,3,4,5,6,7}){auto event=zero;event.kind=kind;CHECK(idleTest(event,1,0,.01)==1);}
 auto signal=zero;signal.signal=signalId;CHECK(idleTest(signal,1,0,.01)==1);
 auto moved=zero;moved.length=.0001;CHECK(idleTest(moved,1,0,.01)==1);
 CHECK(idleTest(zero,0,0,.01)==0&&idleTest(zero,1001,0,.01)==1001);
 CHECK(idleTest(zero,1,.5,.01)==1&&idleTest(zero,1,0,.2)==1);
 put(fakeMotion.data(),0,trainId+1);CHECK(idleTest(zero,1,0,.01)==1);put(fakeMotion.data(),0,trainId);
 // Admission after a measured stop must survive several real movement
 // integrations until the TRUE signal boundary (not merely the stop offset).
 // Previously the first moved sample erased the proof and reapplied fence 0.
 for(const int margin:{3,100}){
  permissionHasApproach=true;
  CHECK(drivingPreferences.set(margin));resetTrains();fakeMotion=savedMotion;
  const double signalPosition=100,start=signalPosition-margin;
  put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,start/1000);
  put(fakeMotion.data(),0x3c0,start);put(fakeMotion.data(),0x3c8,0.0);put(fakeMotion.data(),0x3d0,0.0);
  auto& departing=testTrain(trainId);departing.motion=motion;departing.managed=true;departing.ahead={{signalId,signalPosition}};
  rulePublishers.rows()={{signalId,0,8,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};refreshRawRules();
  std::array<unsigned char,0x70> worker{};put(worker.data(),8,network);put(worker.data(),0x18,session);put(worker.data(),0x68,uintptr_t{1});
  const auto move=[&]{StepScope scope(reinterpret_cast<uintptr_t>(worker.data()),motion);
   integrate(result,network,motion+0x290,motion+0x3a0,motion+8,0,10,1,.5,1000,0,1000,0);};
  simulatedMovement=0;move();CHECK(departing.memory.stopped.signal==signalId);
  captureSourceApproach(departing,motion,signalPosition);
  CHECK(check(0,0,0,0,0,motion,signalId,1)==1); // First admission can be lookahead.
  CHECK(departing.entryPhysicalRoute.valid()&&departing.boundary.empty()&&!departing.memory.sight);
  const double increment=margin==3?1:10;simulatedMovement=increment;
  double previous=start;
  for(unsigned pass=0;pass<static_cast<unsigned>(margin/increment);++pass){
   // Alternate a scan with no new permission and the real native marker chain
   // scan->lookahead admission. Both must preserve the unfinished scan route.
   permissionDuringScan=pass%2!=0;scanPermissionResult=0;
   rescanSourceApproach(motion,reinterpret_cast<uintptr_t>(worker.data()),signalPosition);
   if(permissionDuringScan)CHECK(scanPermissionResult==1);
   CHECK(departing.entryPhysicalRoute.valid()&&!departing.physicalRoute.valid()&&departing.entry.valid&&departing.entry.free==200);
   move();double advanced{};
   CHECK(read(motion+0x3c0,advanced)&&advanced==previous+increment&&advanced<=signalPosition);
   previous=advanced;CHECK(!departing.memory.sight); // No passage before boundary.
  }
  permissionDuringScan=false;
  CHECK(previous==signalPosition);
  // A physical/native admission refusal reapplies a stop for THIS train even
  // if it had begun its approach; an old grant cannot bypass a new conflict.
  resetTrains();fakeMotion=savedMotion;
  put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,start/1000);
  put(fakeMotion.data(),0x3c0,start);put(fakeMotion.data(),0x3c8,0.0);put(fakeMotion.data(),0x3d0,0.0);
  auto& rejected=testTrain(trainId);rejected.motion=motion;rejected.managed=true;rejected.ahead={{signalId,signalPosition}};
  simulatedMovement=0;move();captureSourceApproach(rejected,motion,signalPosition);CHECK(check(0,0,0,0,0,motion,signalId,0)==1);
  simulatedMovement=increment;move();double partiallyMoved{};CHECK(read(motion+0x3c0,partiallyMoved)&&partiallyMoved==start+increment);
  crossConflict=true;CHECK(!check(0,0,0,0,0,motion,signalId,0)&&!rejected.memory.stopped.signal);move();double halted{};
  CHECK(read(motion+0x3c0,halted)&&halted==partiallyMoved);crossConflict=false;
  // Revoking STOP_THEN_PROCEED permission also stops the approach in place.
  rulePublishers.rows()[0]={signalId,0,8,0,NIMBY_DRIVING_STOP};refreshRawRules();move();
  CHECK(read(motion+0x3c0,halted)&&halted==partiallyMoved);
 }
 // An ordinary native pre-source range must keep its native checks, and must
 // not be appended twice after copyPrefix already captured that geometry.
 // An obstacle appearing after admission still constrains the pre-approach.
 CHECK(drivingPreferences.set(100));resetTrains();fakeMotion=savedMotion;
 put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,0.0);
 put(fakeMotion.data(),0x3c0,0.0);put(fakeMotion.data(),0x3c8,0.0);put(fakeMotion.data(),0x3d0,0.0);
 auto& prefixTrain=testTrain(trainId);prefixTrain.motion=motion;prefixTrain.managed=true;prefixTrain.ahead={{signalId,100}};
 rulePublishers.rows()={{signalId,0,8,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};refreshRawRules();
 simulatedMovement=0;integrate(result,network,motion+0x290,motion+0x3a0,motion+8,0,10,1,.5,1000,0,1000,0);
 captureSourceApproach(prefixTrain,motion,100);permissionHasApproach=true;
 CHECK(check(0,0,0,0,0,motion,signalId,0)==1&&prefixTrain.physicalRoute.valid());
 CHECK(prefixTrain.entry.valid&&prefixTrain.entry.covered==1000&&prefixTrain.entry.free==200);
 std::array<unsigned char,0x70> prefixWorker{};put(prefixWorker.data(),8,network);put(prefixWorker.data(),0x18,session);put(prefixWorker.data(),0x68,uintptr_t{1});
 const auto preparePrefix=[&]{StepScope scope(reinterpret_cast<uintptr_t>(prefixWorker.data()),motion);
  integrate(result,network,motion+0x290,motion+0x3a0,motion+8,0,10,1,.5,1000,0,1000,0);};
 obstruction=.04;preparePrefix();
 CHECK(appliedDistance>34.99&&appliedDistance<=35&&appliedTarget==0&&appliedCeiling<=8);
 CHECK(prefixTrain.memory.stopped.admitted&&!prefixTrain.memory.sight);
 // Even with enough measured space for sight, the native pre-source refusal
 // is retained. It cannot borrow the downstream reservation exception.
 CHECK(!check(0,0,0,0,0,motion,signalId,0)&&!prefixTrain.memory.stopped.signal);
 obstruction=.6;
 // A source modified in place invalidates geometry before any new movement,
 // including a still-live Path and the same numerical train/signal IDs.
 prefixTrain.memory.stopped=captureStopProof(rulePublishers.rows()[0],100,0,100);
 CHECK(check(0,0,0,0,0,motion,signalId,0)==1);
 put(fakeSourceSignal.data(),0x48,.11);preparePrefix();
 CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0&&!prefixTrain.memory.stopped.admitted);
 // A source changed during the physical query must discard that whole view,
 // not merely validate the geometry before calling the native predicate.
 double mutationHead{};CHECK(read(motion+0x3c0,mutationHead));
 captureSourceApproach(prefixTrain,motion,100);
 prefixTrain.memory.stopped=captureStopProof(rulePublishers.rows()[0],100,mutationHead,100);
 CHECK(check(0,0,0,0,0,motion,signalId,0)==1);
 duringOccupation=[]{put(fakeSourceSignal.data(),0x48,.11);};preparePrefix();
 CHECK(!duringOccupation&&appliedDistance==0&&appliedTarget==0&&appliedCeiling==0&&!prefixTrain.memory.stopped.admitted);
 // Zero gap is a qualified source point, never a source=head fallback.
 resetTrains();fakeMotion=savedMotion;put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,.2);
 put(fakeMotion.data(),0x3c0,1000.0);put(fakeMotion.data(),0x3c8,0.0);put(fakeMotion.data(),0x3d0,0.0);
 auto& contact=testTrain(trainId);contact.motion=motion;contact.managed=true;contact.ahead={{signalId,1000}};
 contact.memory.stopped=captureStopProof(rulePublishers.rows()[0],1000,1000,100);
 captureSourceApproach(contact,motion,1000);permissionHasApproach=false;
 CHECK(check(0,0,0,0,0,motion,signalId,0)==1&&contact.physicalRoute.valid());
 put(fakeSourceSignal.data(),0x50,int8_t{1});
 CHECK(!check(0,0,0,0,0,motion,signalId,0)&&!contact.memory.stopped.signal);
 captureSourceApproach(contact,motion,1000);contact.memory.stopped=captureStopProof(rulePublishers.rows()[0],1000,1000,100);
 put(fakeTrack.data(),8,trackId+1);CHECK(!check(0,0,0,0,0,motion,signalId,0));put(fakeTrack.data(),8,uint64_t{0});
 contact.ahead.clear();CHECK(!check(0,0,0,0,0,motion,signalId,0));
 simulateProjector=false;simulatedMovement=0;nativeEventHead=std::numeric_limits<double>::infinity();permissionSourceFraction=0;permissionHasApproach=false;permissionDuringScan=false;
 fakeMotion=savedMotion;rulePublishers.rows()=savedRules;refreshRawRules();resetTrains();CHECK(drivingPreferences.set(3));
}
}
namespace {
void checkRouteWorkBudget(uintptr_t motion,uintptr_t network,uintptr_t result,uintptr_t worker){
 using Budget=nimby::windows::automatic::RouteReadBudget;
 const auto savedMotion=fakeMotion,savedTrack=fakeTrack;
 const auto savedRules=rulePublishers.rows();
 CHECK(drivingPreferences.set(100));
 routeClockFixture=true;routeClockTick=1000000;routeReadCost=0;
 nativeCheckCost=nativeRangeCost=nativeReservationCost=0;extraOccupationQueries=0;
 obstruction=2;permissionHasApproach=false;permissionDuringScan=false;
 const auto setup=[&]() -> RuntimeTrain& {
  resetTrains();fakeMotion=savedMotion;fakeTrack=savedTrack;
  put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3b0,0.0);
  put(fakeMotion.data(),0x3c0,0.0);put(fakeMotion.data(),0x3c8,0.0);put(fakeMotion.data(),0x3d0,0.0);
  rulePublishers.rows()={{signalId,0,8,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
  refreshRawRules();auto& state=testTrain(trainId);state.motion=motion;state.managed=true;state.ahead={{signalId,100}};
  captureSourceApproach(state,motion,100);
  state.memory.stopped=captureStopProof(rulePublishers.rows()[0],100,0,100);
  return state;
 };
 const auto request=[&]{return check(0,0,0,0,0,motion,signalId,1);};
 const auto integrateOnce=[&]{integrate(result,network,motion+0x290,motion+0x3a0,motion+8,0,10,1,.5,1000,0,1000,0);};
 const auto runStep=[&]{CHECK(stepMotion(worker,11,motion,22,3.75,33)==17);CHECK(!routeWorkBudget);duringStep={};};
 put(reinterpret_cast<void*>(worker),0x18,session);
 // A vanilla step owns a cheap scope but never samples a qualification clock.
 resetTrains();duringStep={};const auto beforeClock=routeClockCalls;runStep();CHECK(routeClockCalls==beforeClock);

 auto& nativeSlow=setup();nativeCheckCost=nativeRangeCost=nativeReservationCost=5000000;
 duringStep=[&](uintptr_t){
  CHECK(routeWorkBudget&&!routeWorkBudget->limit);
  CHECK(request()&&nativeSlow.entry.valid&&nativeSlow.entryPhysicalRoute.valid());
  CHECK(routeWorkBudget->limit&&routeWorkBudget->limit->reason()==Budget::Reason::None&&routeWorkBudget->virtualTick==1);
  const auto reads=routeWorkBudget->limit->reads();
  // The original integrator can take much longer than 2 ms. Its elapsed time
  // is excluded, while the SDK callbacks nested inside native checks were read.
  duringNative=[](uintptr_t){routeClockTick+=5000000;};integrateOnce();duringNative={};
  CHECK(routeWorkBudget->limit->reads()>reads&&routeWorkBudget->virtualTick==1&&routeWorkBudget->limit->reason()==Budget::Reason::None);
 };
 runStep();nativeCheckCost=nativeRangeCost=nativeReservationCost=0;

 auto& timed=setup();const auto nativeBefore=nativePermissionCalls;routeReadCost=2001;
 duringStep=[&](uintptr_t){
  CHECK(!request());CHECK(routeWorkBudget->limit->reason()==Budget::Reason::Time);
  const auto reads=routeWorkBudget->limit->reads();CHECK(reads==1);
  CHECK(!request()&&routeWorkBudget->limit->reads()==reads);
  CHECK(!timed.entry.valid&&!timed.entryPhysicalRoute.valid());
 };
 runStep();routeReadCost=0;CHECK(nativePermissionCalls==nativeBefore+2);

 // Extra native occupation probes are SDK work. A first slow probe discards
 // the entire pre-entry view; no later binary-search probe may run afterwards.
 auto& physicalSlow=setup();CHECK(request());const auto extraBefore=extraOccupationQueries;
 const auto integrationsBefore=integratedCallbacks;
 duringStep=[&](uintptr_t){
  duringOccupation=[](){routeClockTick+=2001;};integrateOnce();
  CHECK(routeWorkBudget->limit->reason()==Budget::Reason::Time&&extraOccupationQueries==extraBefore+1);
  CHECK(appliedCeiling==0&&appliedDistance==0&&appliedTarget==0);
  CHECK(!physicalSlow.view.valid&&!physicalSlow.entry.valid&&!physicalSlow.memory.stopped.admitted);
  CHECK(!request()&&extraOccupationQueries==extraBefore+1);
 };
 runStep();CHECK(integratedCallbacks==integrationsBefore+1);

 // Real successive reads consume the one step's allowance. An exhausted scan
 // and its later permission callback share that same sticky refusal.
 auto& exhausted=setup();double offset=0;
 std::array<uintptr_t,4> scanContext{reinterpret_cast<uintptr_t>(&offset),0,worker,reinterpret_cast<uintptr_t>(&motion)};
 NativeRange source{reinterpret_cast<uintptr_t>(fakeTrack.data()),0,.1,100,6,0,reinterpret_cast<uintptr_t>(fakeSourceSignal.data())};
 duringStep=[&](uintptr_t){
  {RouteWorkPhase phase;uint64_t id{};
   for(unsigned i=1;i<Budget::maximumReads;++i)CHECK(routeRead(reinterpret_cast<uintptr_t>(fakeTrack.data()),id));}
  CHECK(scan(reinterpret_cast<uintptr_t>(scanContext.data()),reinterpret_cast<uintptr_t>(&source))==1);
  CHECK(routeWorkBudget->limit->reason()==Budget::Reason::Calls&&routeWorkBudget->limit->reads()==Budget::maximumReads);
  CHECK(!request()&&!exhausted.entry.valid);
 };
 runStep();

 auto& byteLimited=setup();std::vector<unsigned char> bulk(Budget::maximumBytes,0x5a),copy(bulk.size());
 duringStep=[&](uintptr_t){
  {RouteWorkPhase phase;CHECK(routeReadBytes(reinterpret_cast<uintptr_t>(bulk.data()),copy.data(),copy.size()));}
  CHECK(!request()&&routeWorkBudget->limit->reason()==Budget::Reason::Bytes);
  CHECK(routeWorkBudget->limit->bytes()==Budget::maximumBytes&&!byteLimited.entry.valid);
 };
 runStep();CHECK(copy.front()==0x5a&&copy.back()==0x5a);

 auto& large=setup();std::vector<uint64_t> largePath(16384,trackId);
 const auto first=reinterpret_cast<uintptr_t>(largePath.data());
 put(fakeMotion.data(),0x338,first);put(fakeMotion.data(),0x340,first+largePath.size()*8);put(fakeMotion.data(),0x348,first+largePath.size()*8);
 captureSourceApproach(large,motion,100);
 duringStep=[&](uintptr_t){
  unsigned accepted=0;
  while(accepted<16&&request())++accepted;
  CHECK(accepted>0&&accepted<16&&routeWorkBudget->limit->reason()==Budget::Reason::Bytes);
  CHECK(routeWorkBudget->limit->bytes()<=Budget::maximumBytes&&!large.entry.valid);
 };
 runStep();

 // Another native step (including the same motion), another train callback,
 // and another thread all own independent counters. The outer step remains
 // refused after each inner callback returns.
 setup();auto peer=fakeMotion;put(peer.data(),0,trainId+0x10000);const auto peerMotion=reinterpret_cast<uintptr_t>(peer.data());
 duringStep=[&](uintptr_t){
  {RouteWorkPhase phase;CHECK(!routeWorkBudget->allow(Budget::maximumBytes+1));}
  auto* outer=routeWorkBudget;CHECK(outer->limit->reason()==Budget::Reason::Bytes);
  auto saved=std::move(duringStep);
  duringStep=[&](uintptr_t){CHECK(routeWorkBudget!=outer&&!routeWorkBudget->limit);CHECK(request());};
  CHECK(stepMotion(worker,11,motion,22,3.75,33)==17);duringStep=std::move(saved);
  CHECK(routeWorkBudget==outer&&!request());
  {RouteBudgetScope peerScope(peerMotion);RouteWorkPhase phase;CHECK(routeWorkBudget!=outer&&routeWorkBudget->allow(8));}
  auto independent=std::async(std::launch::async,[]{RouteBudgetScope own(0x12340000,true);RouteWorkPhase phase;return routeWorkBudget->allow(8);});
  CHECK(independent.get()&&routeWorkBudget==outer&&!routeWorkAvailable());
 };
 runStep();

 // A nested scope may start while the parent's SDK phase is active, rather
 // than only while a native original has already paused it. Neither slow
 // native work nor a child's exhausted SDK time belongs to that parent.
 setup();duringStep=[&](uintptr_t){
  RouteWorkPhase parentPhase;uint64_t id{};
  CHECK(routeRead(reinterpret_cast<uintptr_t>(fakeTrack.data()),id));
  auto* outer=routeWorkBudget;const auto parentTick=outer->virtualTick;
  auto saved=std::move(duringStep);
  duringStep=[&](uintptr_t){
   CHECK(routeWorkBudget!=outer&&!outer->active);
   routeReadCost=2001;CHECK(!request());routeReadCost=0;
   CHECK(routeWorkBudget->limit->reason()==Budget::Reason::Time);
   routeClockTick+=5000000;
  };
  CHECK(stepMotion(worker,11,motion,22,3.75,33)==17);duringStep=std::move(saved);
  CHECK(routeWorkBudget==outer&&outer->active&&routeWorkAvailable()&&outer->virtualTick==parentTick);
  captureSourceApproach(testTrain(trainId),motion,100); // Child denial invalidated its geometry, not the parent budget.
  CHECK(outer->limit->reads()==1&&request());
  {RouteBudgetScope peerScope(peerMotion);RouteWorkPhase phase;
   CHECK(!outer->active&&routeWorkBudget!=outer);
   routeClockTick+=2001;CHECK(!routeWorkAvailable());}
  CHECK(outer->active&&routeWorkAvailable()&&outer->limit->reason()==Budget::Reason::None);
 };
 runStep();

 // Clock regression during a paused native call is still an invalid clock,
 // even though the duration of that original native call is not charged.
 setup();duringStep=[&](uintptr_t){
  nativeCheckCost=-1;CHECK(!request());nativeCheckCost=0;
  CHECK(routeWorkBudget->limit->reason()==Budget::Reason::ClockRegression);
 };
 runStep();
 duringNative={};duringOccupation={};routeClockFixture=false;routeReadCost=0;
 fakeMotion=savedMotion;fakeTrack=savedTrack;rulePublishers.rows()=savedRules;refreshRawRules();resetTrains();
 obstruction=.6;permissionSourceFraction=0;CHECK(drivingPreferences.set(3));
}
void checkTransactionalNativeCommits(uintptr_t motion,uintptr_t network,uintptr_t result,uintptr_t worker){
 // These cases verify commit ordering, not elapsed wall time. A scheduler
 // pause must not exhaust qualification between the first and second commit;
 // checkRouteWorkBudget separately exercises the shared time/byte/read limits.
 struct ControlledClock {
  bool enabled=routeClockFixture;
  int64_t tick=routeClockTick,readCost=routeReadCost,checkCost=nativeCheckCost;
  int64_t rangeCost=nativeRangeCost,reservationCost=nativeReservationCost;
  ControlledClock(){
   routeClockFixture=true;routeClockTick=1000000;
   routeReadCost=nativeCheckCost=nativeRangeCost=nativeReservationCost=0;
  }
  ~ControlledClock(){
   routeClockFixture=enabled;routeClockTick=tick;routeReadCost=readCost;
   nativeCheckCost=checkCost;nativeRangeCost=rangeCost;nativeReservationCost=reservationCost;
  }
 } controlledClock;
 // Existing fixtures above intentionally exercise the default adapter. Only
 // these cases enable the not-yet-live-qualified transactional prototype.
 const auto savedMotion=fakeMotion;
 const auto savedRules=rulePublishers;
 const auto savedConstraints=constraintPublishers;
 const auto savedWorld=testImage().world;
 const auto savedSignalLifetimes=testImage().signalLifetimes,savedConstraintLifetimes=testImage().constraintLifetimes;
 const auto savedRaw=rawRules,savedTransactional=transactionalCommitEnabled,savedProjector=simulateProjector;
 const auto savedObstruction=obstruction;
 transactionalCommitEnabled=true;rawRules=false;simulateProjector=false;obstruction=2;nativeScan=fakeScan;
 put(reinterpret_cast<void*>(worker),8,network);put(reinterpret_cast<void*>(worker),0x18,session);
 put(reinterpret_cast<void*>(worker),0x68,uintptr_t{1});
 alignas(8) std::array<std::array<unsigned char,0x80>,3> sources{};
 const auto position=[&](uintptr_t object,double head,double fraction){
  put(reinterpret_cast<void*>(object),0x3a0,int32_t{0});put(reinterpret_cast<void*>(object),0x3a8,trackId);
  put(reinterpret_cast<void*>(object),0x3b0,fraction);put(reinterpret_cast<void*>(object),0x3b8,int8_t{1});
  put(reinterpret_cast<void*>(object),0x3b9,int8_t{0});put(reinterpret_cast<void*>(object),0x3c0,head);
 };
 const auto integrateMotion=[&](uintptr_t object){
  integrate(result,network,object+0x290,object+0x3a0,object+8,0,50,1,.5,700,7,1,0);
 };
 const auto setup=[&](double head,double fraction,std::span<const NimbySignalDrivingRule> rows,std::span<const Ahead> ahead)->RuntimeTrain&{
  duringNative={};duringStep={};fakeMotion=savedMotion;position(motion,head,fraction);
  rulePublishers.clear();constraintPublishers.clear();resetTrains();
  CHECK(NimbyDriving_PublishV3(rows.data(),static_cast<uint32_t>(rows.size()),5000,0,701)==NIMBY_OK);
  auto& state=testTrain(trainId);state.motion=motion;state.managed=true;
  scanCommitSources(motion,worker,ahead,sources);
  return state;
 };
 const NimbySignalDrivingRule sightRules[]{
  {signalId,4,8,0,NIMBY_DRIVING_ON_SIGHT},{signalId+1,-1,8,0,NIMBY_DRIVING_CLEAR}};
 const Ahead exitAhead[]{ {signalId+1,1000} };
 auto& rolledBack=setup(999,.499,sightRules,exitAhead);
 rolledBack.memory.sight=RestrictedMode{signalId,signalId+1,900,1000,8};
 rolledBack.memory.stops={{signalId+1,1000,0,8,false,{},signalId}};
 rolledBack.memory.lastHead=999;rolledBack.memory.observedHead=true;
 duringNative=[&](uintptr_t object){position(object,1001,.501);};
 runCommittedStep(worker,motion,[&](uintptr_t object){
  integrateMotion(object);
  // Temporary Presence has passed the exit, but native collision handling
  // will conserve the old position after integrate has already returned.
  CHECK(rolledBack.memory.sight&&rolledBack.memory.stops.size()==1);
  position(object,999,.499);
 });
 duringNative={};
 CHECK(rolledBack.memory.sight&&rolledBack.memory.sight->endSignal==signalId+1);
 CHECK(rolledBack.memory.stops.size()==1&&rolledBack.memory.stops[0].signal==signalId+1);

 // A qualified earlier integration in the SAME native step is conserved when
 // a later candidate rolls back. Rejecting N2 must not erase N1's warning.
 const NimbySignalDrivingRule announcementRules[]{
  {signalId,0,8,1,0},{signalId+1,-1,8,0,NIMBY_DRIVING_CLEAR}};
 const Ahead announcementAhead[]{ {signalId,1000},{signalId+1,1030} };
 auto& prefix=setup(999,.499,announcementRules,announcementAhead);
 unsigned integrations=0;bool firstCommittedBeforeSecond=false;
 duringNative=[&](uintptr_t object){
  if(++integrations==1)position(object,1001,.501);
  else {firstCommittedBeforeSecond=prefix.memory.stops.size()==1&&prefix.memory.stops[0].signal==signalId+1;
   position(object,1031,.531);}
 };
 runCommittedStep(worker,motion,[&](uintptr_t object){
  integrateMotion(object);integrateMotion(object);position(object,1001,.501);
 });
 duringNative={};
 CHECK(integrations==2&&firstCommittedBeforeSecond);
 CHECK(prefix.memory.stops.size()==1&&prefix.memory.stops[0].signal==signalId+1&&prefix.memory.stops[0].position==1030);

 // Entry interest can be established by scan INSIDE a previously vanilla
 // train's step. The pending frame must own the actual guard after integrate's
 // local access has returned, not merely borrow that now-destroyed guard.
 duringNative={};duringStep={};fakeMotion=savedMotion;position(motion,999,.499);
 rulePublishers.clear();constraintPublishers.clear();resetTrains();
 CHECK(NimbyDriving_PublishV3(announcementRules,2,5000,0,701)==NIMBY_OK);
 CHECK(testImage().world&&!testImage().world->isManaged(trainId));
 bool lateInterestGuardHeld=false;
 duringNative=[&](uintptr_t object){position(object,1001,.501);};
 runCommittedStep(worker,motion,[&](uintptr_t object){
  scanCommitSources(object,worker,announcementAhead,sources);
  CHECK(testImage().world->isManaged(trainId));
  integrateMotion(object);
  auto competing=std::async(std::launch::async,[&]{
   Publications::View view(publications);TrainAccess access(*view,trainId,object);
   return !access.state&&!access.missing;
  });
  CHECK(competing.wait_for(std::chrono::milliseconds(200))==std::future_status::ready);
  lateInterestGuardHeld=competing.get();
 });
 duringNative={};
 CHECK(lateInterestGuardHeld);
 const auto& lateInterest=testTrain(trainId);
 CHECK(lateInterest.memory.stops.size()==1&&lateInterest.memory.stops[0].signal==signalId+1);

 // Separate trains have separate pending frames. The child's successful step
 // cannot publish the outer train's temporary crossing or lose its warning.
 auto& outer=setup(999,.499,announcementRules,announcementAhead);
 auto peerMotion=fakeMotion;const auto peerId=trainId+0x10000,peer=reinterpret_cast<uintptr_t>(peerMotion.data());
 put(peerMotion.data(),0,peerId);
 auto& peerState=testTrain(peerId);peerState.motion=peer;peerState.managed=true;
 alignas(8) std::array<std::array<unsigned char,0x80>,3> peerSources{};
 scanCommitSources(peer,worker,announcementAhead,peerSources);
 duringNative=[&](uintptr_t object){position(object,1001,.501);};
 runCommittedStep(worker,motion,[&](uintptr_t object){
  integrateMotion(object);
  runCommittedStep(worker,peer,[&](uintptr_t child){integrateMotion(child);});
  CHECK(peerState.memory.stops.size()==1&&peerState.memory.stops[0].signal==signalId+1);
  CHECK(outer.memory.stops.empty());position(object,999,.499);
 });
 duringNative={};
 CHECK(outer.memory.stops.empty()&&peerState.memory.stops.size()==1);

 // A same-train nested step cannot certify an uncommitted outer integration.
 // Its conservative failure must leave a previously received stop in place.
 auto& reentrant=setup(999,.499,sightRules,exitAhead);
 reentrant.memory.sight=RestrictedMode{signalId,signalId+1,900,1000,8};
 reentrant.memory.stops={{signalId+1,1000,0,8,false,{},signalId}};
 duringNative=[&](uintptr_t object){position(object,1001,.501);};
 runCommittedStep(worker,motion,[&](uintptr_t object){
  integrateMotion(object);
  runCommittedStep(worker,motion,[&](uintptr_t child){integrateMotion(child);});
  position(object,999,.499);
 });
 duringNative={};
 CHECK(reentrant.memory.sight&&reentrant.memory.stops.size()==1);

 // Outside a completed native step there is no final-caller certificate. The
 // prototype keeps restrictions even when the integrator's scratch state moves.
 auto& outside=setup(999,.499,sightRules,exitAhead);
 outside.memory.sight=RestrictedMode{signalId,signalId+1,900,1000,8};
 outside.memory.stops={{signalId+1,1000,0,8,false,{},signalId}};
 duringNative=[&](uintptr_t object){position(object,1001,.501);};integrateMotion(motion);duringNative={};
 CHECK(outside.memory.sight&&outside.memory.stops.size()==1);
 CHECK(appliedCeiling==0&&appliedDistance==0&&appliedTarget==0);

 // Regression reported by AB: check() can run at projected head 100, accept a
 // NEW STOP constraint, then native collision handling restores 97. Acceptance
 // must use the last conserved head, not cancel the stop on the next step.
 const NimbySignalDrivingRule stopRule{signalId,0,8,0,NIMBY_DRIVING_STOP};
 const Ahead stopAhead[]{ {signalId,100} };
 const NimbyTrainConstraint stopCommand{trainId,0,501,0,2,0};
 // The correction is active in the default adapter as well as the prototype.
 for(const bool experimental:{false,true}){
  transactionalCommitEnabled=experimental;
  auto& commandState=setup(97,.497,std::span(&stopRule,1),stopAhead);
  runCommittedStep(worker,motion,[&](uintptr_t object){
   position(object,100,.5);
   CHECK(NimbyDriving_TrainConstraints(&stopCommand,1,5000,702)==NIMBY_OK);
   check(0,0,0,0,0,object,signalId,1);
   CHECK(commandState.controlled.instruction.revision!=stopCommand.revision);
   position(object,97,.497);
  });
  runCommittedStep(worker,motion,[&](uintptr_t object){integrateMotion(object);});
  CHECK(commandState.controlled.instruction.revision==stopCommand.revision);
  CHECK(!commandState.controlled.cancelled&&!commandState.controlled.completed&&appliedCeiling==0);
  CHECK(commandState.controlled.state()!=4);
 }
 duringNative={};duringStep={};transactionalCommitEnabled=savedTransactional;rawRules=savedRaw;simulateProjector=savedProjector;
 fakeMotion=savedMotion;rulePublishers=savedRules;constraintPublishers=savedConstraints;
 testImage().signalLifetimes=savedSignalLifetimes;testImage().constraintLifetimes=savedConstraintLifetimes;testImage().world=savedWorld;
 obstruction=savedObstruction;
}
unsigned lifecycleStopCalls{},lifecycleStartCalls{},lifecycleMergeCalls{};
bool lifecycleThrow{};
uintptr_t lifecycleDelta{};
unsigned lifecycleSharedReleaseCalls{};
void WINAPI fakeSharedRelease(PSRWLOCK lock){++lifecycleSharedReleaseCalls;ReleaseSRWLockShared(lock);SetLastError(968);}
uintptr_t __fastcall fakeRunnerStop(uintptr_t) {
 ++lifecycleStopCalls;
 CHECK(nativeWorld.apply([](auto&,auto&){})); // No SDK gate across native code.
 SetLastError(935);
 if(lifecycleThrow)throw 44;
 return 0xfedcba;
}
void __fastcall fakeRunnerStart(uintptr_t context) {
 ++lifecycleStartCalls;
 CHECK(nativeWorld.apply([&](auto& lineage,auto&){lineage.runnerEnter(context-0x5d0,context,GetCurrentThreadId());}));
 SetLastError(946);
}
void __fastcall fakeMergeWorld(uintptr_t,uintptr_t delta) {
 ++lifecycleMergeCalls;lifecycleDelta=delta;
 CHECK(nativeWorld.apply([](auto&,auto&){}));SetLastError(957);
}
void lifecycleWrappers() {
 lifecycleStopCalls=lifecycleStartCalls=lifecycleMergeCalls=0;
 nativeWorld.apply([](auto& lineage,auto&){std::destroy_at(&lineage);std::construct_at(&lineage);});
 const auto savedObservation=phaseObservationEnabled;phaseObservationEnabled=true;
 nativeRunnerStop=fakeRunnerStop;nativeRunnerStart=fakeRunnerStart;nativeWorldMerge=fakeMergeWorld;
 alignas(8) std::array<unsigned char,0x610> controller{};
 const auto controllerAddress=reinterpret_cast<uintptr_t>(controller.data()),context=controllerAddress+0x5d0;
 put(controller.data(),0,uintptr_t{0x200000});put(controller.data(),0x140,uintptr_t{0x300000});
 put(controller.data(),0x5d0,controllerAddress);
 nativeWorld.apply([](auto& lineage,auto&){lineage.lose();});
 CHECK(stopRunner(context)==0xfedcba&&lifecycleStopCalls==1&&GetLastError()==935);
 startRunner(context);CHECK(lifecycleStartCalls==1&&GetLastError()==946);
 world_lineage::Result captured;
 const world_lineage::Scope scope{controllerAddress,context,controllerAddress+0x4c0,0x200000,0x300000,
  GetCurrentThreadId(),0x3495fe,0x4784f9,0x477f05};
 nativeWorld.apply([&](auto& lineage,auto& clock){captured=lineage.capturePhase(clock,scope);});
 CHECK(captured.reason==world_lineage::Reason::Ready&&nativeWorld.current(captured.capture));
 nativeWorld.apply([&](auto& lineage,auto&){CHECK(lineage.endCapture(captured.capture));});
 phaseAncestry.emplace(phase_owner::Profile{base,true});
 {
  phase_owner::Scope controllerScope(phaseContext(),phase_owner::ControllerFrame{context,controllerAddress,true});
  fixtureCaller=base+0x349871;mergeWorld(scope.simulation,0xabcdef);fixtureCaller=0;
 }
 phaseAncestry.reset();
 CHECK(lifecycleMergeCalls==1&&lifecycleDelta==0xabcdef&&GetLastError()==957);
 world_lineage::Result next;
 nativeWorld.apply([&](auto& lineage,auto& clock){next=lineage.capturePhase(clock,scope);});
 CHECK(next.reason==world_lineage::Reason::Ready&&next.capture.continuity!=captured.capture.continuity);
 // A stop immediately revokes an owned capture before the native join returns.
 CHECK(stopRunner(context)==0xfedcba&&lifecycleStopCalls==2&&!nativeWorld.current(next.capture));
 CHECK(!nativeWorld.lineage.endCapture(next.capture));
 startRunner(context);CHECK(lifecycleStartCalls==2);
 nativeWorld.apply([&](auto& lineage,auto& clock){next=lineage.capturePhase(clock,scope);});
 CHECK(next.reason==world_lineage::Reason::Ready&&next.capture.world!=captured.capture.world);
 // An original exception is never swallowed, and it invalidates the lignée.
 lifecycleThrow=true;bool threw=false;
 try {stopRunner(context);}catch(int value){threw=value==44;}
 CHECK(threw&&lifecycleStopCalls==3&&!nativeWorld.current(next.capture));
 lifecycleThrow=false;CHECK(stopRunner(context)==0xfedcba&&lifecycleStopCalls==4);
 startRunner(context);CHECK(lifecycleStartCalls==3);
 nativeWorld.apply([&](auto& lineage,auto& clock){next=lineage.capturePhase(clock,scope);});
 CHECK(next.reason==world_lineage::Reason::Ready);
 nativeWorld.apply([&](auto& lineage,auto&){lineage.endCapture(next.capture);});
 nativeWorld.lose();phaseObservationEnabled=savedObservation;
}
void universalInventoryWithoutMod(void* root) {
 using namespace nimby::windows::phase_ownership;
 namespace roster=nimby::windows::consist_inventory;
 CHECK(!testImage().active());
 const auto savedSession=session;const auto savedClock=routeClockFixture;
 alignas(8) std::array<unsigned char,0x2200> sim{};
 alignas(8) std::array<unsigned char,0x610> controller{};
 alignas(8) std::array<unsigned char,24> descriptor{};
 alignas(8) std::array<unsigned char,0x100> network{};
 std::vector<unsigned char> block(roster::detail::allocationBytes);
 const auto simAddress=reinterpret_cast<uintptr_t>(sim.data());
 const auto networkAddress=reinterpret_cast<uintptr_t>(network.data());
 const auto controllerAddress=reinterpret_cast<uintptr_t>(controller.data());
 const auto controllerReference=controllerAddress+0x5d0;
 put(controller.data(),0x5d0,controllerAddress);
 uintptr_t blockAddress=reinterpret_cast<uintptr_t>(block.data());
 ResourceRef advanceResource{},simulationResource{};
 session=simAddress;put(root,0x680,session);routeClockFixture=true;
 put(sim.data(),0xa0,uint32_t{roster::detail::allocationBytes});
 put(sim.data(),0xa4,uint32_t{9});put(sim.data(),0xa8,uint32_t{512});
 put(sim.data(),0xac,uint32_t{512*roster::detail::stride});put(sim.data(),0xb0,uint32_t{511});
 put(sim.data(),0xb8,reinterpret_cast<uintptr_t>(&blockAddress));
 put(sim.data(),0xc0,reinterpret_cast<uintptr_t>(&blockAddress+1));
 put(sim.data(),0xc8,reinterpret_cast<uintptr_t>(&blockAddress+1));
 // Stopped vanilla train plus a child which the native active list omits.
 const uint64_t parent=0x5000000000007ULL,child=0x5000000010009ULL;
 put(block.data(),0,parent);put(block.data(),roster::detail::stride,child);
 put(block.data(),roster::detail::stride+0x1f0,uint8_t{1});
 put(block.data(),roster::detail::stride+0x1d8,parent);
 put(controller.data(),0,networkAddress);put(controller.data(),0x140,simAddress);
 put(descriptor.data(),0,uint64_t{19});put(descriptor.data(),0x10,int64_t{50});
 phaseAncestry.reset();phaseAncestry.emplace(Profile{base,true});
 ControllerFrame controllerFrame{controllerReference,controllerAddress,true};
 AdvanceFrame advance;
 advance.controllerReference=controllerFrame.controllerReference;advance.controller=controllerAddress;
 advance.controllerSimulation=advance.simulation=simAddress;advance.controllerNetwork=advance.network=networkAddress;
 advance.resourceReference=reinterpret_cast<uintptr_t>(&advanceResource);
 advance.descriptor=reinterpret_cast<uintptr_t>(descriptor.data());advance.returnAddress=base+0x3495fe;
 advance.ordinal=19;advance.elapsed=50;advance.readComplete=true;
 SimulationFrame simulation{simAddress,networkAddress,reinterpret_cast<uintptr_t>(&simulationResource),base+0x4784f9,{},50,true};
 const auto beforeAttempts=phaseObservation.inventoryAttempts.load();
 const auto beforeComplete=phaseObservation.inventoryComplete.load();
 {
  Scope controllerScope(phaseContext(),controllerFrame),advanceScope(phaseContext(),advance),simulationScope(phaseContext(),simulation);
  const auto frame=phaseFrame(simAddress+0xa0,networkAddress,simAddress,50,base+0x477f05);
  Scope ownership(phaseContext(),frame);CHECK(ownership.afterReturn(frame).qualified());
  observeTrainPhase(simAddress+0xa0,networkAddress,simAddress,phaseObservation.epoch.load(),ownership,50,base+0x477f05);
 }
 CHECK(phaseObservation.inventoryAttempts.load()==beforeAttempts+1);
 CHECK(phaseObservation.inventoryComplete.load()==beforeComplete+1);
 CHECK(phaseObservation.inventorySample.result.rows==2);
 CHECK(phaseObservation.roster[0].train==parent&&!phaseObservation.roster[0].active);
 CHECK(phaseObservation.roster[1].train==child&&phaseObservation.roster[1].parentRow==0);
 CHECK(!testImage().active()&&!testImage().world);
 // A missing child-parent link refuses the entire roster; no partial result
 // can be used to treat the unobserved body as released.
 put(block.data(),roster::detail::stride+0x1d8,uint64_t{0x5000000020007ULL});
 {
  Scope controllerScope(phaseContext(),controllerFrame),advanceScope(phaseContext(),advance),simulationScope(phaseContext(),simulation);
  const auto frame=phaseFrame(simAddress+0xa0,networkAddress,simAddress,50,base+0x477f05);Scope ownership(phaseContext(),frame);
  observeTrainPhase(simAddress+0xa0,networkAddress,simAddress,phaseObservation.epoch.load(),ownership,50,base+0x477f05);
 }
 CHECK(phaseObservation.inventoryAttempts.load()==beforeAttempts+2);
 CHECK(phaseObservation.inventoryComplete.load()==beforeComplete+1);
 CHECK(phaseObservation.inventorySample.result.reason==roster::Reason::MissingParent);
 CHECK(phaseObservation.inventorySample.result.rows==0);
 CHECK(phaseObservation.roster[1].parentRow==0); // old owned output preserved
 // The actual release wrapper observes vanilla trains only AFTER it releases
 // the native Shared lock and successfully takes Exclusive without waiting.
 put(block.data(),roster::detail::stride+0x1d8,parent);
 nativeRunnerStop=fakeRunnerStop;nativeRunnerStart=fakeRunnerStart;nativeSharedRelease=fakeSharedRelease;
 const auto savedObservation=phaseObservationEnabled;phaseObservationEnabled=true;
 stopRunner(controllerReference);startRunner(controllerReference);
 const auto lock=reinterpret_cast<PSRWLOCK>(controllerAddress+0x4c0);
 const auto beforeExclusive=exclusiveCaptureAttempts.load();
 {
  Scope controllerScope(phaseContext(),controllerFrame);
  AcquireSRWLockShared(lock);fixtureCaller=base+0x349fa3;releaseShared(lock);fixtureCaller=0;
 }
 CHECK(GetLastError()==968&&exclusiveCaptureAttempts.load()==beforeExclusive+1);
 CHECK(phaseObservation.inventoryComplete.load()==beforeComplete+2);
 CHECK(phaseObservation.inventorySample.result.rows==2);
 CHECK(TryAcquireSRWLockExclusive(lock));ReleaseSRWLockExclusive(lock);
 // A foreign return site performs its original release without any capture.
 {
  Scope controllerScope(phaseContext(),controllerFrame);
  AcquireSRWLockShared(lock);fixtureCaller=base+0x349fa4;releaseShared(lock);fixtureCaller=0;
 }
 CHECK(exclusiveCaptureAttempts.load()==beforeExclusive+1&&GetLastError()==968);
 // Simulate a UI reader holding Shared. The observer must return while this
 // reader still holds its lock; otherwise accelerated simulation stalls.
 AcquireSRWLockShared(lock);const auto beforeBusy=exclusiveCaptureBusy.load();
 auto contended=std::async(std::launch::async,[&]{
  phaseAncestry.emplace(Profile{base,true});
  Scope controllerScope(phaseContext(),controllerFrame);
  AcquireSRWLockShared(lock);fixtureCaller=base+0x349fa3;releaseShared(lock);fixtureCaller=0;
  return GetLastError();
 });
 const bool independent=contended.wait_for(std::chrono::milliseconds(200))==std::future_status::ready;
 ReleaseSRWLockShared(lock);CHECK(contended.get()==968&&independent);
 CHECK(exclusiveCaptureBusy.load()==beforeBusy+1);
 // Installing after entry into the long-lived runner has no TLS ancestry.
 // Only the current native Root/thread and completed real lock boundary can
 // create a new lineage; a wrong runner identity cannot adopt that world.
 phaseAncestry.reset();const auto savedInstalled=installed;installed=true;
 CHECK(nativeWorld.apply([](auto& lineage,auto&){std::destroy_at(&lineage);std::construct_at(&lineage);}));
 const auto rootAddress=reinterpret_cast<uintptr_t>(root);
 put(root,0x540,networkAddress);put(root,0x680,simAddress);put(root,0xb10,rootAddress+0x540);
 const auto rootLock=reinterpret_cast<PSRWLOCK>(rootAddress+0xa00);
 const auto adoptionCount=phaseObservation.inventoryComplete.load();
 put(root,0xb28,uint32_t{GetCurrentThreadId()+1});tickOffset=1000;
 AcquireSRWLockShared(rootLock);fixtureCaller=base+0x349fa3;releaseShared(rootLock);fixtureCaller=0;
 CHECK(nativeWorld.lineage.state()==world_lineage::State::Unarmed&&phaseObservation.inventoryComplete.load()==adoptionCount);
 put(root,0xb28,uint32_t{GetCurrentThreadId()});tickOffset=1200;
 AcquireSRWLockShared(rootLock);fixtureCaller=base+0x349fa3;releaseShared(rootLock);fixtureCaller=0;
 CHECK(phaseObservation.inventoryComplete.load()==adoptionCount+1&&GetLastError()==968);
 CHECK(nativeWorld.lineage.state()==world_lineage::State::Running);
 CHECK(TryAcquireSRWLockExclusive(rootLock));ReleaseSRWLockExclusive(rootLock);
 installed=savedInstalled;tickOffset=0;
 nativeWorld.lose();phaseObservationEnabled=savedObservation;
 phaseAncestry.reset();session=savedSession;put(root,0x680,session);routeClockFixture=savedClock;
}
thread_local unsigned finalOriginalCalls{},finalStageCalls{},finalVetoCalls{},membershipOriginalCalls{};
thread_local bool finalInvokeStage=true,finalNativeAllowed=true,finalVetoAllowed=true,finalThrows=false;
thread_local std::function<void()> finalDuringVeto;
uint8_t __fastcall finalStageFixture(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uint64_t,uint8_t){
 ++finalStageCalls;SetLastError(970);return 1;
}
uint8_t __fastcall finalOriginalFixture(uintptr_t context,uintptr_t reservations,uintptr_t train,uintptr_t motion,uintptr_t source){
 ++finalOriginalCalls;CHECK(finalAdmissionQuery&&finalAdmissionQuery->motion==motion);
 uint64_t id{};CHECK(read(source,id));
 if(finalThrows)throw std::runtime_error("original final admission");
 if(!finalNativeAllowed){SetLastError(971);return 0;}
 if(finalInvokeStage&&!check(context,reservations,0,0,train,motion,id,0)){SetLastError(971);return 0;}
 ++finalVetoCalls;const auto callback=finalDuringVeto;if(callback)callback();
 SetLastError(971);return finalVetoAllowed?1:0;
}
uint8_t __fastcall membershipOriginalFixture(uintptr_t,uintptr_t,uintptr_t,uintptr_t){
 ++membershipOriginalCalls;SetLastError(972);return finalNativeAllowed?1:0;
}
// Actual wrappers, not helpers: the native callback and SDK stage are each
// executed once, while retirement/mutation during the final veto fails closed.
void finalAdmissionWrappers(uintptr_t motion){
 const auto savedCheck=nativeCheck;const auto savedFinal=nativeFinalAdmission;const auto savedMembership=nativeMembership;
 const auto savedImage=testImage();const auto savedRaw=rawRules;
 rawRules=false;nativeCheck=finalStageFixture;nativeFinalAdmission=finalOriginalFixture;nativeMembership=membershipOriginalFixture;
 constexpr uint64_t owner=901;
 const NimbySignalDrivingRule green{signalId,-1,8,0,NIMBY_DRIVING_CLEAR};
 const NimbySignalDrivingRule red{signalId,0,8,0,NIMBY_DRIVING_STOP};
 alignas(8) std::array<unsigned char,0x80> source{};put(source.data(),0,signalId);
 const auto sourceAddress=reinterpret_cast<uintptr_t>(source.data());
 const auto publish=[&]{CHECK(NimbyDriving_PublishV3(&green,1,5000,0,owner)==NIMBY_OK);};
 const auto run=[&](unsigned calls=1){const auto before=finalOriginalCalls;const auto answer=finalAdmission(66,77,88,motion,sourceAddress);
  CHECK(finalOriginalCalls==before+calls);CHECK(!finalAdmissionQuery);CHECK(GetLastError()==971);return answer;};
 rulePublishers.clear();resetTrains();put(reinterpret_cast<void*>(motion),0,trainId);put(reinterpret_cast<void*>(motion),0x3c0,1000.0);
 publish();CHECK(run()==1&&finalStageCalls==1&&finalVetoCalls==1);
 finalVetoAllowed=false;CHECK(run()==0&&finalStageCalls==2&&finalVetoCalls==2);finalVetoAllowed=true;
 finalNativeAllowed=false;CHECK(run()==0&&finalStageCalls==2&&finalVetoCalls==2);finalNativeAllowed=true;
 finalInvokeStage=false;CHECK(run()==0);finalInvokeStage=true;
 finalDuringVeto=[&]{CHECK(NimbyDriving_PublishV3(&red,1,5000,0,owner)==NIMBY_OK);};CHECK(run()==0);finalDuringVeto={};
 publish();finalDuringVeto=[&]{CHECK(NimbyDriving_PublishV3(nullptr,0,5000,0,owner)==NIMBY_OK);publish();};CHECK(run()==0);finalDuringVeto={};
 publish();finalDuringVeto=[&]{tickOffset=6000;};CHECK(run()==0);tickOffset=0;finalDuringVeto={};
 publish();finalDuringVeto=[&]{put(source.data(),0,signalId+0x10000);};CHECK(run()==0);put(source.data(),0,signalId);finalDuringVeto={};
 finalDuringVeto=[&]{put(reinterpret_cast<void*>(motion),0,trainId+0x10000);};CHECK(run()==0);put(reinterpret_cast<void*>(motion),0,trainId);finalDuringVeto={};
 finalThrows=true;const auto beforeThrow=finalOriginalCalls;bool threw=false;
 try{finalAdmission(66,77,88,motion,sourceAddress);}catch(const std::runtime_error&){threw=true;}
 CHECK(threw&&finalOriginalCalls==beforeThrow+1&&!finalAdmissionQuery);finalThrows=false;
 // Nested final callbacks restore the outer query and retain its own result.
 auto otherMotion=fakeMotion;put(otherMotion.data(),0,trainId+0x20000);
 finalDuringVeto=[&]{auto outer=finalAdmissionQuery;finalDuringVeto={};
  CHECK(finalAdmission(99,77,88,reinterpret_cast<uintptr_t>(otherMotion.data()),sourceAddress)==1);
  CHECK(finalAdmissionQuery==outer);};CHECK(run(2)==1);finalDuringVeto={};
 for(const auto caller:{base+0x448b29,base+0x4493e9}){
  StepScope scope(66,motion);fixtureCaller=caller;auto before=membershipOriginalCalls;
  CHECK(membership(66,123,motion,sourceAddress)==0&&membershipOriginalCalls==before+1&&GetLastError()==972);
  CHECK(membership(67,123,motion,sourceAddress)==1);
  CHECK(membership(66,123,reinterpret_cast<uintptr_t>(otherMotion.data()),sourceAddress)==1);
  finalNativeAllowed=false;CHECK(membership(66,123,motion,sourceAddress)==0);finalNativeAllowed=true;
 }
 {StepScope scope(66,motion);fixtureCaller=base+0x449c9b;CHECK(membership(66,123,motion,sourceAddress)==1);}
 fixtureCaller=base+0x448b29;CHECK(membership(66,123,motion,sourceAddress)==1);fixtureCaller=0;
 CHECK(NimbyDriving_PublishV3(nullptr,0,5000,0,owner)==NIMBY_OK);
 {StepScope scope(66,motion);fixtureCaller=base+0x448b29;put(source.data(),0,signalId+100);
  CHECK(membership(66,123,motion,sourceAddress)==1);}
 fixtureCaller=0;nativeCheck=savedCheck;nativeFinalAdmission=savedFinal;nativeMembership=savedMembership;
 auto& image=testImage();image.worldRevision=savedImage.worldRevision;image.revision=savedImage.revision;
 image.world=savedImage.world;image.rules=savedImage.rules;image.constraints=savedImage.constraints;
 image.signalLifetimes=savedImage.signalLifetimes;image.constraintLifetimes=savedImage.constraintLifetimes;rawRules=savedRaw;
}
}
thread_local size_t stoppedNativeCalls{};
uint32_t __fastcall fakeStoppedStep(uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,uintptr_t budget){
 ++stoppedNativeCalls;
 int64_t remaining{};CHECK(read(budget,remaining)&&remaining==0);
 SetLastError(973);return 0;
}
void checkWholeStepMovementLatch(uintptr_t motion){
 const auto savedImageWorld=testImage().world;const auto savedStep=nativeStep;
 const auto savedCaller=fixtureCaller;const auto savedExperimental=transactionalCommitEnabled;
 const auto savedRaw=rawRules;const auto savedRules=rulePublishers;const auto savedMotion=fakeMotion;
 const auto savedSignalLifetimes=testImage().signalLifetimes;
 alignas(8) std::array<uintptr_t,4> context{};context[3]=session;
 uint64_t dbTrain=trainId;int64_t budget=100;
 const auto worker=reinterpret_cast<uintptr_t>(context.data()),train=reinterpret_cast<uintptr_t>(&dbTrain);
 const auto allowance=reinterpret_cast<uintptr_t>(&budget);
 put(fakeMotion.data(),0,trainId);
 auto& publication=testImage();
 // Positive consumption uses the caller's exact budget, preserves LastError,
 // and cannot accidentally set a foreign thread's/private copied allowance.
 SetLastError(969);CHECK(consumeStoppedStepBudget(publication,base+0x44b36c,worker,train,motion,trainId,allowance));
 CHECK(budget==0&&GetLastError()==969);
 const auto rejected=[&](uintptr_t caller,uintptr_t nativeContext,uintptr_t db,uint64_t id,uintptr_t quota){
  budget=100;SetLastError(969);CHECK(!consumeStoppedStepBudget(publication,caller,nativeContext,db,motion,id,quota));
  CHECK(budget==100&&GetLastError()==969);
 };
 rejected(base+0x44b367,worker,train,trainId,allowance);
 rejected(base+0x44b36c,worker,train,trainId+1,allowance);
 dbTrain=trainId+1;rejected(base+0x44b36c,worker,train,trainId,allowance);dbTrain=trainId;
 context[3]=session+1;rejected(base+0x44b36c,worker,train,trainId,allowance);context[3]=session;
 rejected(base+0x44b36c,worker,train,trainId,33);
 rejected(base+0x44b36c,1,train,trainId,allowance);
 // Actual step wrapper: the original sees zero and runs exactly once even
 // without a subsequent integrator/signal callback (Kind3/source-less path).
 rawRules=false;rulePublishers.clear();
 NimbySignalDrivingRule instruction{signalId,NIMBY_DRIVING_CLEAR,0,0,0};
 CHECK(rulePublishers.publish(77,{instruction},GetTickCount64()+1000)==NIMBY_OK);
 resetTrains();testTrain(trainId).motion=motion;testTrain(trainId).integrationUnqualified=true;
 fixtureCaller=base+0x44b36c;transactionalCommitEnabled=true;nativeStep=fakeStoppedStep;
 budget=100;stoppedNativeCalls=0;
 CHECK(stepMotion(worker,train,motion,22,3.75,allowance)==0&&budget==0&&stoppedNativeCalls==1&&GetLastError()==973);
 CHECK(!currentCommitFrame&&!routeWorkBudget&&!currentStep.context);
 nativeStep=savedStep;fixtureCaller=savedCaller;transactionalCommitEnabled=savedExperimental;
 rawRules=savedRaw;rulePublishers=savedRules;testImage().world=savedImageWorld;
 testImage().signalLifetimes=savedSignalLifetimes;fakeMotion=savedMotion;
}
void firstManagedDiscoveryPrefix(uintptr_t motion){
 const auto savedMotion=fakeMotion,savedTrack=fakeTrack,savedCross=crossTrack;
 const auto savedWorld=testImage().world;const auto savedRules=rulePublishers;
 const auto savedRaw=rawRules,savedClock=routeClockFixture;
 const auto savedCost=routeReadCost;const auto savedCaller=fixtureCaller;
 rawRules=false;routeClockFixture=true;routeReadCost=0;fixtureCaller=base+0x44b36c;
 nativeScan=fakeScan;
 const uint64_t secondTrack=trackId+0x10000,vanillaId=signalId+0x20000;
 std::array<uint64_t,2> tracks{trackId,secondTrack};
 std::array<uint64_t,1> children{secondTrack};
 std::array<unsigned char,0x80> vanilla{},managed{};
 std::array<unsigned char,0x70> worker{};
 put(worker.data(),0x18,session);
 const auto w=reinterpret_cast<uintptr_t>(worker.data());
 const auto configure=[&]{
  resetTrains();rulePublishers.clear();
  CHECK(rulePublishers.publish(77,{{signalId,0,8,0,NIMBY_DRIVING_STOP}},GetTickCount64()+100000)==NIMBY_OK);
  fakeMotion={};fakeTrack={};crossTrack={};vanilla={};managed={};tracks={trackId,secondTrack};children={secondTrack};
  put(fakeMotion.data(),0,trainId);put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x320,uint8_t{1});
  put(fakeMotion.data(),0x338,reinterpret_cast<uintptr_t>(tracks.data()));
  put(fakeMotion.data(),0x340,reinterpret_cast<uintptr_t>(tracks.data()+tracks.size()));
  put(fakeMotion.data(),0x348,reinterpret_cast<uintptr_t>(tracks.data()+tracks.size()));
  put(fakeMotion.data(),0x3a0,int32_t{0});put(fakeMotion.data(),0x3a8,trackId);
  put(fakeMotion.data(),0x3b0,.1);put(fakeMotion.data(),0x3b8,int8_t{1});put(fakeMotion.data(),0x3c0,100.0);
  put(fakeTrack.data(),0,trackId);put(fakeTrack.data(),0x88,1000.0);
  put(crossTrack.data(),0,secondTrack);put(crossTrack.data(),0x88,1000.0);
  put(fakeTrack.data(),0x408,reinterpret_cast<uintptr_t>(children.data()));
  put(fakeTrack.data(),0x410,reinterpret_cast<uintptr_t>(children.data()+children.size()));
  put(fakeTrack.data(),0x418,reinterpret_cast<uintptr_t>(children.data()+children.size()));
  put(vanilla.data(),0,vanillaId);put(vanilla.data(),0x30,int32_t{NIMBY_SIGNAL_PATH});
  put(vanilla.data(),0x40,trackId);put(vanilla.data(),0x48,.2);put(vanilla.data(),0x50,int8_t{-1});
  put(managed.data(),0,signalId);put(managed.data(),0x30,int32_t{NIMBY_SIGNAL_PATH});
  put(managed.data(),0x40,secondTrack);put(managed.data(),0x48,.3);put(managed.data(),0x50,int8_t{-1});
 };
 size_t cases=0;
 const auto run=[&](auto&& mutate,bool expected,size_t fragments=1,bool scoped=true){
  configure();double offset=0;
  std::array<uintptr_t,4> context{reinterpret_cast<uintptr_t>(&offset),0,w,reinterpret_cast<uintptr_t>(&motion)};
  NativeRange first{reinterpret_cast<uintptr_t>(fakeTrack.data()),.1,.2,100,6,0,reinterpret_cast<uintptr_t>(vanilla.data())};
  NativeRange last{reinterpret_cast<uintptr_t>(crossTrack.data()),0,.3,300,6,0,reinterpret_cast<uintptr_t>(managed.data())};
  const auto exercise=[&]{
   discoveryReads=discoveryBytes=discoveryAllocations=discoveryPathHeaders=0;
   discoveryPathAddress=motion+0x290;countDiscoveryWork=true;
   CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&first))==1);
   // A vanilla train pays no controller allocation, and is still unmanaged
   // after the first marker. Its bounded POD prefix is local to this step.
   CHECK(!testImage().world->isManaged(trainId));CHECK(discoveryAllocations==0);
   const auto readsAtFirst=discoveryReads,bytesAtFirst=discoveryBytes;
   offset=100;
   for(size_t i=0;i<fragments;++i){const double from=.2+.8*double(i)/double(fragments),to=.2+.8*double(i+1)/double(fragments);
    NativeRange part{first.track,from,to,(to-from)*1000,2,0,0};
    CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&part))==1);
    offset+=(to-from)*1000;
   }
   const auto prefixReads=discoveryReads,prefixBytes=discoveryBytes;
   CHECK(discoveryAllocations==0);CHECK(discoveryPathHeaders==size_t(scoped?2:0));countDiscoveryWork=false;
   if(cases==0){CHECK(readsAtFirst<50&&bytesAtFirst<8192);CHECK(prefixReads<80&&prefixBytes<16384);
    std::cout<<"first-discovery POD prefix: first="<<readsAtFirst<<'/'<<bytesAtFirst
     <<" full="<<prefixReads<<'/'<<prefixBytes<<" allocations=0 tlsBytes="<<sizeof(scanDiscoveryStorage)<<'\n';}
   mutate(context,offset);
   CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&last))==1);
   uint64_t id{};CHECK(read(motion,id));
   auto& state=testTrain(id);size_t emitted=0;
   const std::array<uint64_t,1> selected{signalId};
   const auto reason=state.signalRoute.copyDiagnosed(selected,2,GetTickCount64(),readBytes,
    [&](const NimbySignalRouteObservation& row){++emitted;CHECK(row.train_id==id&&row.signal_id==signalId);
     CHECK(row.blocks==2&&std::abs(row.distance_m-1200)<.01);
     CHECK(row.flags&NIMBY_SIGNAL_ROUTE_ORDERED);CHECK(!(row.flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR));},[](auto){});
   CHECK((reason==nimby::windows::automatic::RouteCopyReason::Ready)==expected);
   CHECK(emitted==size_t(expected));
   if(expected){CHECK(state.ahead.size()==2&&state.ahead[0].signal==vanillaId&&state.ahead[1].signal==signalId);
    CHECK(std::abs(state.ahead[1].position-1300)<.01);}
   ++cases;
  };
  if(scoped){StepScope step(w,motion);RouteBudgetScope quota(motion,true);exercise();}
  else exercise();
 };
 const auto unchanged=[](auto&,double&){};
 run(unchanged,true);
 // Original test only discovered a managed source at offset zero. These
 // failures exercise the actual wrapper after a vanilla marker + transition.
 run([&](auto&,double&){tracks[1]=secondTrack+1;},false); // Same Path buffer, changed full ID.
 run([&](auto&,double&){put(vanilla.data(),0x48,.21);},false);
 run([&](auto&,double&){put(vanilla.data(),0,vanillaId+1);},false);
 run([&](auto&,double&){children[0]=secondTrack+1;},false); // Same child descriptor.
 run([&](auto&,double&){put(fakeTrack.data(),0x88,1001.0);},false);
 run([&](auto&,double&){put(fakeMotion.data(),0,trainId+1);},false);
 run([&](auto&,double&){put(fakeMotion.data(),0x3b0,.11);},false);
 run([&](auto& context,double&){context[2]=1;},false); // Invalid foreign worker never borrows the prefix.
 double foreignOffset=900;
 run([&](auto& context,double&){context[0]=reinterpret_cast<uintptr_t>(&foreignOffset);},false);
 run([&](auto&,double& offset){offset+=10;},false); // Missing native section.
 run(unchanged,false,1,false); // A bare lookahead cannot manufacture a completed step scope.
 run(unchanged,true,ScanDiscovery::maximumRanges-1);
 run(unchanged,false,ScanDiscovery::maximumRanges);
 run([&](auto&,double&){routeReadCost=3000;},false);routeReadCost=0; // Shared quota is terminal.
 run([&](auto&,double&){StepScope inner(w+8,motion+8);CHECK(currentStep.discovery!=nullptr);
  StepScope deep(w+16,motion+16);CHECK(currentStep.discovery==nullptr);},true);
 run([&](auto& context,double& offset){
  // A second scan in the same step can replace its POD ranges, but does not
  // perform another full Path capture or allocate vanilla controller state.
  offset=0;NativeRange again{reinterpret_cast<uintptr_t>(fakeTrack.data()),.1,.2,100,6,0,reinterpret_cast<uintptr_t>(vanilla.data())};
  countDiscoveryWork=true;CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&again))==1);
  offset=100;NativeRange rest{again.track,.2,1,800,2,0,0};
  CHECK(scan(reinterpret_cast<uintptr_t>(context.data()),reinterpret_cast<uintptr_t>(&rest))==1);
  countDiscoveryWork=false;CHECK(discoveryPathHeaders==2&&discoveryAllocations==0);offset=900;
 },true);
 // No cache survives a native step boundary, even with the same address/ID.
 run([&](auto&,double&){currentStep.discovery->reset();},false);
 CHECK(!currentStep.discovery&&scanDiscoveryDepth==0);
 fakeMotion=savedMotion;fakeTrack=savedTrack;crossTrack=savedCross;
 testImage().world=savedWorld;rulePublishers=savedRules;rawRules=savedRaw;
 routeClockFixture=savedClock;routeReadCost=savedCost;fixtureCaller=savedCaller;
 std::cout<<"first managed native discovery: "<<cases<<" scenarios PASS\n";
}
namespace {
void selectedRouteCopyExports(){
 // Exercise the ACTUAL scan wrapper and both exported copy versions with
 // synthetic storage. No native game image/address or reservation is used.
 const auto oldWorld=testImage().world;const auto previousScan=nativeScan;
 const bool previousPermission=permissionDuringScan;
 const auto oldInterest=selectedRouteInterestUntil.load();
 struct Restore {std::shared_ptr<TrainWorld> world;decltype(nativeScan) scan;bool permission;uint64_t interest;
  ~Restore(){testImage().world=std::move(world);nativeScan=scan;permissionDuringScan=permission;selectedRouteInterestUntil=interest;}
 }restore{oldWorld,previousScan,previousPermission,oldInterest};
 selectedRouteInterestUntil=0;
 resetTrains();nativeScan=fakeScan;permissionDuringScan=false;
 alignas(8) std::array<unsigned char,0x500> motionBytes{};
 alignas(8) std::array<unsigned char,0x420> trackBytes{};
 alignas(8) std::array<unsigned char,0x100> source{},marker{},next{},worker{};
 constexpr uint64_t markerId=signalId+(1ULL<<16),nextId=signalId+(2ULL<<16);
 std::array<uint64_t,1> ids{trackId};const auto idsAt=reinterpret_cast<uintptr_t>(ids.data());
 const auto motion=reinterpret_cast<uintptr_t>(motionBytes.data()),track=reinterpret_cast<uintptr_t>(trackBytes.data());
 put(motionBytes.data(),0,trainId);put(motionBytes.data(),0x3a8,trackId);put(motionBytes.data(),0x3b0,.1);
 put(motionBytes.data(),0x3b8,int8_t{1});put(motionBytes.data(),0x3c0,100.0);put(motionBytes.data(),0x4b0,uint8_t{1});
 put(motionBytes.data(),0x320,uint8_t{1});put(motionBytes.data(),0x338,idsAt);
 put(motionBytes.data(),0x340,idsAt+8);put(motionBytes.data(),0x348,idsAt+8);
 put(trackBytes.data(),0,trackId);put(trackBytes.data(),0x88,1000.0);
 const auto object=[&](auto& bytes,uint64_t id,double fraction,int kind,int8_t direction){
  put(bytes.data(),0,id);put(bytes.data(),0x30,kind);put(bytes.data(),0x40,trackId);
  put(bytes.data(),0x48,fraction);put(bytes.data(),0x50,direction);
 };
 object(source,signalId,.2,NIMBY_SIGNAL_PATH,-1);object(marker,markerId,.5,NIMBY_SIGNAL_BALISE,1);
 object(next,nextId,.8,NIMBY_SIGNAL_PATH,-1);put(worker.data(),0x18,session);
 auto& state=testTrain(trainId);state.motion=motion;
 double offset=0;uintptr_t motionRef=motion;
 std::array<uintptr_t,4> scanContext{reinterpret_cast<uintptr_t>(&offset),0,reinterpret_cast<uintptr_t>(worker.data()),reinterpret_cast<uintptr_t>(&motionRef)};
 const auto emit=[&](double from,double to,double metres,auto& target){
  NativeRange range{track,from,to,metres,6,0,reinterpret_cast<uintptr_t>(target.data())};
  CHECK(scan(reinterpret_cast<uintptr_t>(scanContext.data()),reinterpret_cast<uintptr_t>(&range))==1);
  offset+=metres;
 };
 emit(.1,.2,100,source);emit(.2,.5,300,marker);emit(.5,.8,300,next);
 const std::array<uint64_t,1> selected{signalId};
 std::array<NimbySignalRouteObservation,4> rows{};std::array<NimbySignalRouteSection,8> sections{};
 std::array<NimbySignalRouteAnnotation,4> annotations{};uint32_t count=0,sectionCount=0,annotationCount=0;
 const auto copy=[&](uint32_t rowCapacity,uint32_t sectionCapacity,uint32_t annotationCapacity){
  return NimbyDriving_CopySignalRoutesV2(selected.data(),1,2,rows.data(),rowCapacity,&count,
   sections.data(),sectionCapacity,&sectionCount,annotations.data(),annotationCapacity,&annotationCount);
 };
 const auto permissionCalls=nativePermissionCalls;const auto reservationCalls=reservationChecks;
 CHECK(!state.signalRoute.observesObjects());
 CHECK(copy(4,8,4)==NIMBY_OK&&count==0&&sectionCount==0&&annotationCount==0);
 CHECK(!state.signalRoute.observesObjects()); // Reading V2 cannot invent cached facts.
 offset=0;emit(.1,.2,100,source);emit(.2,.5,300,marker);emit(.5,.8,300,next);
 CHECK(state.signalRoute.observesObjects());
 CHECK(copy(4,8,4)==NIMBY_OK&&count==1&&sectionCount==2&&annotationCount==2);
 CHECK(rows[0].signal_id==signalId&&rows[0].next_signal_id==nextId);
 CHECK(annotations[0].object_id==markerId&&annotations[0].native_kind==NIMBY_SIGNAL_BALISE&&annotations[0].flags==13);
 CHECK(annotations[1].object_id==nextId&&annotations[1].flags==2&&annotations[1].ordinal==1);
 CHECK(nativePermissionCalls==permissionCalls&&reservationChecks==reservationCalls); // Copies never replay predicates.
 for(const unsigned limited:{0,1,2}){
  CHECK(copy(limited==0?0:4,limited==1?0:8,limited==2?0:4)==NIMBY_BUFFER_TOO_SMALL);
  CHECK(count==0&&sectionCount==0&&annotationCount==0); // No partial route/annotations escape.
 }
 CHECK(NimbyDriving_CopySignalRoutesV2(selected.data(),1,2,rows.data(),4,&count,sections.data(),8,&sectionCount,
  annotations.data(),4,nullptr)==NIMBY_INVALID_ARGUMENT);
 CHECK(NimbyDriving_CopySignalRoutes(selected.data(),1,2,rows.data(),4,&count,sections.data(),8,&sectionCount)==NIMBY_OK&&count==1);
 // Changing a non-PATH object invalidates V2 without changing legacy V1.
 put(marker.data(),0x48,.51);CHECK(copy(4,8,4)==NIMBY_OK&&count==0&&sectionCount==0&&annotationCount==0);
 CHECK(NimbyDriving_CopySignalRoutes(selected.data(),1,2,rows.data(),4,&count,sections.data(),8,&sectionCount)==NIMBY_OK&&count==1);
 put(marker.data(),0x48,.5);selectedRouteInterestUntil=0;
 offset=0;emit(.1,.2,100,source);emit(.2,.5,300,marker);emit(.5,.8,300,next);
 CHECK(!state.signalRoute.observesObjects());
 CHECK(copy(4,8,4)==NIMBY_OK&&count==0&&annotationCount==0); // Lease expiry clears V2 until another scan.
 offset=0;emit(.1,.2,100,source);emit(.2,.5,300,marker);emit(.5,.8,300,next);
 CHECK(copy(4,8,4)==NIMBY_OK&&count==1&&annotationCount==2);
 state.ahead.resize(128);emit(.8,.9,100,next);
 CHECK(copy(4,8,4)==NIMBY_OK&&count==0&&annotationCount==0); // Omitted PATH cannot be skipped by V2.
}
}
int main(){try{
 std::vector<unsigned char> module(0xb81998+8),root(0xb40);
 base=reinterpret_cast<uintptr_t>(module.data());session=0x12340000;
 put(module.data(),0xb81998,reinterpret_cast<uintptr_t>(root.data()));put(root.data(),0x680,session);
 universalInventoryWithoutMod(root.data());
 lifecycleWrappers();
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
 auto request=[&]{refreshRawRules();auto& requesting=testTrain(trainId);requesting.motion=motion;
  if(!requesting.ahead.empty())captureSourceApproach(requesting,motion,requesting.ahead.front().position);
  return check(0,0,0,0,0,motion,signalId,0);};
 // An absolute stop cannot inherit permission, even with an old stop proof.
 testTrain(trainId).memory.stopped=captureStopProof(rulePublishers.rows().front(),1000,1000);CHECK(!request());
 rulePublishers.rows()[0].flags|=NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED;
 testTrain(trainId).memory.stopped={};CHECK(!request());
 testTrain(trainId).memory.stopped=captureStopProof(rulePublishers.rows().front(),1000,1000);CHECK(request());
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
 checkSignalProjectionFence(motion,reinterpret_cast<uintptr_t>(network.data()),reinterpret_cast<uintptr_t>(result.data()));
 checkRouteWorkBudget(motion,reinterpret_cast<uintptr_t>(network.data()),reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(worker.data()));
 rulePublishers.rows()={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
 auto& state=testTrain(trainId);state.motion=motion;state.managed=true;state.ahead={{signalId,1000}};state.memory={};state.entry={};
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
 put(waitingSimulation.data(),0x28,int64_t{101});CHECK(!request()); // Standstill alone has no source geometry.
 testTrain(trainId).ahead={{signalId,1000}};put(waitingSimulation.data(),0x28,int64_t{102});CHECK(request());
 CHECK(testTrain(trainId).memory.stopped.signal==signalId);
 CHECK(!testTrain(trainId).boundary.empty());
 reservationConflict=false;
 resetTrains();rulePublishers.rows()[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 testTrain(trainId).ahead={{signalId,1000}};CHECK(request()); // Red flashing requires no stop proof, but still qualified geometry.
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
 // A heartbeat keeps the received instruction, while a changed instruction
 // from the same publisher must not commit an obsolete passage after motion.
 const auto changedPassage=[&](NimbySignalDrivingRule before,NimbySignalDrivingRule after,bool retained){
  CHECK(NimbyDriving_PublishV3(&before,1,5000,0,101)==NIMBY_OK);
  crossing={};crossing.motion=motion;crossing.managed=true;
  crossing.ahead={{signalId,1000},{signalId+1,1400}};put(fakeMotion.data(),0x3c0,1000.0);
  const auto serial=lifetime(testImage().signalLifetimes,signalId);
  duringNative=[&](uintptr_t object){
   CHECK(NimbyDriving_PublishV3(&after,1,5000,0,101)==NIMBY_OK);
   CHECK(lifetime(testImage().signalLifetimes,signalId)==serial);
   put(reinterpret_cast<void*>(object),0x3c0,1001.0);
  };
  runMotion(motion);duringNative={};
  CHECK(crossing.memory.sight.has_value()==retained);
  CHECK(crossing.memory.stops.empty()&&crossing.memory.held.empty()&&crossing.boundary.empty());
  if(retained)CHECK(crossing.memory.sight->speed==before.reopened_speed_mps);
 };
 const NimbySignalDrivingRule normalSight{signalId,4,8,0,NIMBY_DRIVING_ON_SIGHT};
 const NimbySignalDrivingRule absoluteStop{signalId,0,8,0,NIMBY_DRIVING_STOP};
 const NimbySignalDrivingRule afterStop{signalId,0,8,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED};
 const NimbySignalDrivingRule changedSight{signalId,4,12,0,NIMBY_DRIVING_ON_SIGHT};
 changedPassage(normalSight,absoluteStop,false);
 changedPassage(afterStop,changedSight,false);
 changedPassage(absoluteStop,normalSight,false);
 changedPassage(normalSight,normalSight,true);
 CHECK(NimbyDriving_PublishV3(&announcement,1,5000,0,101)==NIMBY_OK);
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
 checkTransactionalNativeCommits(motion,reinterpret_cast<uintptr_t>(network.data()),reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(worker.data()));
 finalAdmissionWrappers(motion);
 checkWholeStepMovementLatch(motion);
 firstManagedDiscoveryPrefix(motion);
 selectedRouteCopyExports();
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
