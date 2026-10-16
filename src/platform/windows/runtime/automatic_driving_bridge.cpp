#include <nimby/detail/diagnostics.hpp>
// Native mechanics only: national indications are translated to numeric rules by mods.
#include <engine/automatic_driving.h>
#include <engine/automatic_controller.h>
#include <engine/driving_publishers.h>
#include <engine/binary_identity.h>
#include <platform/windows/runtime/automatic_driving_status.h>
#include <platform/windows/runtime/physical_route.h>
#include <platform/windows/runtime/route_read_budget.h>
#include <platform/windows/runtime/signal_route_diagnostics.h>
#include <platform/windows/runtime/driving_state.h>
#include <platform/windows/runtime/driving_preferences.h>
#include <platform/windows/runtime/native_passage_observer.h>
#include <platform/windows/runtime/native_train_phase_observer.h>
#include <platform/windows/runtime/native_consist_inventory.h>
#include <platform/windows/runtime/native_phase_ownership.h>
#include <platform/windows/runtime/native_world_lineage.h>
#include <platform/windows/runtime/native_owned_rear_snapshot.h>
#include <platform/windows/runtime/native_mutation_continuity.h>
#include <platform/windows/runtime/native_crossing_observation.h>
#include <platform/windows/runtime/integration_commit_witness.h>
#include <engine/native_passage_mailbox.h>
#include <platform/windows/bridge_installation.h>
#include <MinHook.h>
#include <windows.h>
#include <array>
#include <unordered_map>
#include <cstring>
#include <utility>
#include <sstream>
#if defined(_MSC_VER)
#include <intrin.h>
#endif
// Capture at the wrapper entry, never in a helper where the caller would be
// the SDK itself. Synthetic fixtures override only this macro, not the native
// wrappers or their validation rules.
#ifndef NRF_PHASE_CALLER
#if defined(_MSC_VER)
#define NRF_PHASE_CALLER() reinterpret_cast<uintptr_t>(_ReturnAddress())
#else
#define NRF_PHASE_CALLER() reinterpret_cast<uintptr_t>(__builtin_return_address(0))
#endif
#endif
namespace {
using namespace nimby::engine::automatic;
using Integrate=uintptr_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,double,double,double,double,double,int64_t,uintptr_t);
using Project=uintptr_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,uintptr_t);
using Scan=uintptr_t(__fastcall*)(uintptr_t,uintptr_t);
using Check=uint8_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uint64_t,uint8_t);
using Occupancy=uint8_t(__fastcall*)(uintptr_t,uintptr_t,double,double);
using Step=uint32_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,uintptr_t);
using PassBy=void(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t);
using TrainPhase=void(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,int64_t);
using ControllerPhase=void(__fastcall*)(uintptr_t);
using AdvancePhase=void(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t);
using SimulationPhase=void(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,int64_t);
using WorldReset=void(__fastcall*)(uintptr_t);
using WorldRebuild=void(__fastcall*)(uintptr_t,uintptr_t);
using RunnerStop=uintptr_t(__fastcall*)(uintptr_t);
using RunnerStart=void(__fastcall*)(uintptr_t);
using WorldMerge=void(__fastcall*)(uintptr_t,uintptr_t);
using SharedRelease=void(WINAPI*)(PSRWLOCK);
using FinalAdmission=uint8_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t);
using Membership=uint8_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t);
using ModelCommandBatch=void(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t);
Integrate nativeIntegrate{};Scan nativeScan{},nativePermissionRange{};Check nativeCheck{};Occupancy nativeOccupancy{},nativeReservation{};
Project nativeProject{};
Step nativeStep{};
PassBy nativePassBy{};
TrainPhase nativeTrainPhase{};
ControllerPhase nativeControllerPhase{};
AdvancePhase nativeAdvancePhase{};
SimulationPhase nativeSimulationPhase{};
WorldReset nativeWorldReset{};
WorldRebuild nativeWorldRebuild{};
RunnerStop nativeRunnerStop{};
RunnerStart nativeRunnerStart{};
WorldMerge nativeWorldMerge{};
SharedRelease nativeSharedRelease{};
FinalAdmission nativeFinalAdmission{};
Membership nativeMembership{};
ModelCommandBatch nativeModelCommandBatch{};
// Diagnostic only. The extra hook is installed exclusively on explicit opt-in
// after the executable hash and prologue checks. It owns no movement authority.
bool passageObservationEnabled=false;
bool phaseObservationEnabled=false;
// Experimental local post-collision commit only. The normal adapter remains
// unchanged until this profile's phase coverage is validated in the game.
bool transactionalCommitEnabled=false;
using nimby::windows::automatic::CommitReason;
using nimby::windows::automatic::CommitPresence;
using nimby::windows::automatic::CommitPathStamp;
using nimby::windows::automatic::sameCommitPresence;
using nimby::windows::automatic::validCommitPresence;
std::array<std::atomic<uint64_t>,static_cast<size_t>(CommitReason::Count)> commitOutcomes{};
std::array<std::atomic<uint64_t>,static_cast<size_t>(CommitReason::Count)> commitSampleTrain{};
std::atomic<uint64_t> commitLatched{};
std::atomic<uint64_t> stoppedStepBudgets{},unqualifiedStepBudgets{};
nimby::engine::interlocking::NativePassageMailbox passageMailbox;
std::atomic<uint64_t> passageSteps{},passageNoCandidates{},passageDroppedIdentity{},passageSequence{};
SRWLOCK initialization=SRWLOCK_INIT;
uintptr_t base{};
bool installed=false;
nimby::automatic_status::Shared* telemetry{};
using nimby::windows::automatic::Publication;
using nimby::windows::automatic::Publications;
using nimby::windows::automatic::RuntimeTrain;
using nimby::windows::automatic::TrainAccess;
using nimby::windows::automatic::TrainWorld;
using nimby::windows::automatic::SourceLifetime;
using nimby::windows::automatic::lifetime;
using nimby::windows::automatic::pruneReleased;
using nimby::windows::automatic::rememberSources;
Publications publications;
nimby::windows::automatic::DrivingPreferences drivingPreferences;
// Interest is read-only SDK bookkeeping, not a permission or native setting.
// Only V2 readers renew it; ordinary signalling pays no object/filter capture
// after this short lease expires. A new scan, never an old cache, enables V2.
std::atomic<uint64_t> selectedRouteInterestUntil{};
bool selectedRouteInterested(uint64_t now)noexcept{
 return now<selectedRouteInterestUntil.load(std::memory_order_relaxed);
}
bool readBytes(uintptr_t address,void* value,size_t size){SIZE_T got{};return address>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),value,size,&got)&&got==size;}
template<class T> bool read(uintptr_t address,T& value){return readBytes(address,&value,sizeof value);}
// Cooperative SDK-work budget, shared by one native train step. Native calls
// are paused below; their SDK callbacks enter another active phase. Thus a
// slow original integrator cannot spend its caller's qualification allowance.
// Clock work starts only at the first witness, including for initially vanilla
// trains that encounter a managed signal later in the same step. Scheduling and
// a single nonpreemptible read/query can exceed the allowance before returning.
struct RouteWorkBudget {
 uintptr_t motion{};
 std::optional<nimby::windows::automatic::RouteReadBudget> limit;
 int64_t virtualTick=1,lastClock=0;
 bool active=false;
 bool clock(int64_t& value){LARGE_INTEGER sampled{};if(!QueryPerformanceCounter(&sampled)||sampled.QuadPart<0)return false;value=sampled.QuadPart;return true;}
 void invalidClock(){if(limit)limit->available(-1);active=false;}
 void resume(){
  if(active)return;
  if(!limit){LARGE_INTEGER frequency{};const bool known=QueryPerformanceFrequency(&frequency)!=FALSE;
   limit.emplace(virtualTick,known?frequency.QuadPart:0);}
  if(limit->reason()!=nimby::windows::automatic::RouteReadBudget::Reason::None)return;
  int64_t now{};if(!clock(now)){invalidClock();return;}
  if(now<lastClock){limit->available(virtualTick-1);return;}
  lastClock=now;active=true;
 }
 bool available(){
  if(!limit||limit->reason()!=nimby::windows::automatic::RouteReadBudget::Reason::None)return false;
  if(active){int64_t now{};if(!clock(now)){invalidClock();return false;}
   if(now<lastClock){limit->available(virtualTick-1);active=false;return false;}
   const auto delta=now-lastClock;lastClock=now;
   if(delta>INT64_MAX-virtualTick){limit->available(INT64_MAX);active=false;return false;}
   virtualTick+=delta;
  }
  return limit->available(virtualTick);
 }
 void pause(){if(active){available();active=false;}}
 bool allow(size_t bytes){return available()&&limit->allow(bytes,virtualTick);}
};
thread_local RouteWorkBudget* routeWorkBudget=nullptr;
struct RouteBudgetScope {
 RouteWorkBudget local;RouteWorkBudget* previous=routeWorkBudget;bool owns=false,previousActive=false;
 explicit RouteBudgetScope(uintptr_t motion,bool newStep=false){
  if(newStep||!previous||previous->motion!=motion){
   // An inner train/step has its own work allowance. Exclude its whole scope
   // from an active parent's clock, just as from its byte/read counters.
   previousActive=previous&&previous->active;if(previousActive)previous->pause();
   local.motion=motion;routeWorkBudget=&local;owns=true;
  }
 }
 ~RouteBudgetScope(){if(owns){routeWorkBudget=previous;if(previousActive)previous->resume();}}
};
struct RouteWorkPhase {
 RouteWorkBudget* value=routeWorkBudget;bool wasActive=value&&value->active;
 RouteWorkPhase(){if(value)value->resume();}
 ~RouteWorkPhase(){if(value&&!wasActive)value->pause();}
};
struct OriginalRouteCall {
 RouteWorkBudget* value=routeWorkBudget;bool wasActive=value&&value->active;
 OriginalRouteCall(){if(value)value->pause();}
 ~OriginalRouteCall(){if(value&&wasActive)value->resume();}
};
template<class Call> decltype(auto) originalRouteCall(Call&& call){OriginalRouteCall pause;return call();}
bool routeWorkAvailable(){return routeWorkBudget&&routeWorkBudget->available();}
bool routeReadBytes(uintptr_t address,void* value,size_t size){
 return routeWorkBudget&&routeWorkBudget->allow(size)&&readBytes(address,value,size)&&routeWorkAvailable();
}
template<class T> bool routeRead(uintptr_t address,T& value){return routeReadBytes(address,&value,sizeof value);}
// Private layouts of the hash-qualified projector (RVA 0x377dd0) and its
// integrator caller (0x378600). Range kind 8 is the native partial section,
// not an invalid path (kind 0), signal (6), or track transition (2..5).
struct ProjectionPresence {
 int32_t index{},padding{};uint64_t track{};double fraction{};
 int8_t direction{},orientation{};std::array<unsigned char,6> reserved{};double head{};
};
struct ProjectionRange {
 uintptr_t track{};double from{},to{},length{};int32_t kind{},padding{};uintptr_t signal{};
};
static_assert(sizeof(ProjectionPresence)==0x28&&offsetof(ProjectionPresence,head)==0x20);
static_assert(sizeof(ProjectionRange)==0x30&&offsetof(ProjectionRange,kind)==0x20);
bool sameProjectionStart(const ProjectionPresence& a,const ProjectionPresence& b) noexcept {
 return a.index==b.index&&a.track==b.track&&a.fraction==b.fraction&&
  a.direction==b.direction&&a.orientation==b.orientation&&a.head==b.head;
}
struct ProjectionFence {
 uintptr_t context{},path{};ProjectionPresence start{};double stopHead{};unsigned clipped{};
 uintptr_t motion{};uint64_t train{};
};
thread_local ProjectionFence* projectionFence=nullptr;
struct ProjectionScope {
 ProjectionFence* previous;
 explicit ProjectionScope(ProjectionFence* fence) noexcept:previous(projectionFence){projectionFence=fence;}
 ~ProjectionScope(){projectionFence=previous;}
};
uintptr_t __fastcall project(uintptr_t result,uintptr_t context,uintptr_t path,uintptr_t presence,double distance,uintptr_t checkState){
 auto* fence=projectionFence;
 // The first native projection uses a stack copy of Presence; the second
 // uses real Presence. Both start at the exact captured path cursor. Other
 // projector users, worker threads, and nested integrations remain independent.
 ProjectionPresence cursor{};
 if(fence&&context==fence->context&&path==fence->path&&std::isfinite(distance)&&distance>=0&&
    read(presence,cursor)&&sameProjectionStart(cursor,fence->start)){
  const double remaining=std::max(0.0,fence->stopHead-cursor.head);
  if(distance>remaining){distance=remaining;++fence->clipped;}
 }
 return originalRouteCall([&]{return nativeProject(result,context,path,presence,distance,checkState);});
}
bool captureProjectionFence(ProjectionFence& fence,uintptr_t network,uintptr_t path,uintptr_t motion,
 uint64_t train,uintptr_t presence,double head,const Integration& proposed){
 if(!proposed.signalStopDistanceM||proposed.stopSignal>>48!=8||path!=motion+0x290||
    !std::isfinite(*proposed.signalStopDistanceM)||*proposed.signalStopDistanceM<0||
    network>UINTPTR_MAX-0x28||!read(presence,fence.start))return false;
 const auto& start=fence.start;
 if(start.index<0||start.track>>48!=1||!std::isfinite(start.fraction)||start.fraction<0||start.fraction>1||
    (start.direction!=1&&start.direction!=-1)||!std::isfinite(start.head)||start.head!=head)return false;
 fence.stopHead=head+*proposed.signalStopDistanceM;
 if(!std::isfinite(fence.stopHead)||fence.stopHead<head)return false;
 fence.context=network+0x28;fence.path=path;fence.motion=motion;fence.train=train;return true;
}
void consumeFencedIdle(uintptr_t result,uintptr_t returned,uintptr_t presence,int64_t budget,
 const ProjectionFence& fence,double beforeOverlap){
 // Native integration still runs once. At a proven zero-length fence its
 // 0.01 m/s idle floor would otherwise return after one tick, making the
 // caller repeat up to budget times. Kind 8 has no event; its normal caller
 // records zero travelled metres and elapsed*0.01 s. Consume that stationary
 // time in this caller-owned result only, never by changing native Motion.
 if(returned!=result||!fence.clipped||fence.stopHead!=fence.start.head||beforeOverlap!=0||budget<=1)return;
 ProjectionRange range{};ProjectionPresence after{};double overlap{},speed{};int64_t elapsed{};uint64_t track{},train{};
 if(!read(fence.motion,train)||train!=fence.train||!read(result,range)||range.kind!=8||range.signal||range.length!=0||
    !std::isfinite(range.from)||range.from!=range.to||range.from!=fence.start.fraction||
    !read(range.track,track)||track!=fence.start.track||!read(presence,after)||
    after.index!=fence.start.index||after.track!=fence.start.track||after.head!=fence.start.head||
    after.direction!=fence.start.direction||after.orientation!=fence.start.orientation||
    !std::isfinite(after.fraction)||
    (after.fraction!=fence.start.fraction&&after.fraction!=std::nextafter(range.to,after.direction==1?1.0:0.0))||
    !read(presence+0x30,overlap)||overlap!=0||!read(presence+0x28,speed)||!std::isfinite(speed)||speed<0||speed>0.010001||
    !read(result+0x30,elapsed)||elapsed<=0||elapsed>budget)return;
 std::memcpy(reinterpret_cast<void*>(result+0x30),&budget,sizeof budget);
}
LONG reserveSample(volatile LONG& count,LONG maximum) noexcept {
 auto observed=InterlockedCompareExchange(&count,0,0);
 while(observed<maximum){const auto previous=InterlockedCompareExchange(&count,observed+1,observed);
  if(previous==observed)return observed;
  observed=previous;}
 return -1;
}
void recordIntegration(uint64_t id,double head,double beforeSpeed,uintptr_t presence,uintptr_t result,
 double ceiling,double braking,double distance,double target,double chosenCeiling,double chosenBraking,
 double chosenDistance,double chosenTarget,uint32_t reason,const RuntimeTrain* state=nullptr){
 if(!telemetry)return;
 const auto n=reserveSample(telemetry->count,4096);if(n<0)return;
 auto& sample=telemetry->samples[n];sample.train=id;sample.before=beforeSpeed;sample.headBefore=head;
 sample.nativeCeiling=ceiling;sample.ceiling=chosenCeiling;sample.nativeBraking=braking;sample.braking=chosenBraking;
 sample.nativeDistance=distance;sample.nativeTarget=target;sample.chosenDistance=chosenDistance;sample.chosenTarget=chosenTarget;
 sample.observedAtMs=GetTickCount64();sample.reason=reason;
 if(state&&state->memory.sight){const auto view=state->view.at(head,sample.observedAtMs);
  sample.restrictedSource=state->memory.sight->source;sample.freeDistance=view.distanceM;sample.visibilityVerified=view.verified?1u:0u;}
 read(presence+0x28,sample.after);read(presence+0x20,sample.headAfter);read(result+0x30,sample.ticks);
 InterlockedExchange(&sample.ready,1);
}
uintptr_t currentSession(){uintptr_t root{},sim{};return read(base+0xb81998,root)&&read(root+0x680,sim)?sim:0;}
namespace phase_owner=nimby::windows::phase_ownership;
thread_local std::optional<phase_owner::Context> phaseAncestry;
phase_owner::Context& phaseContext() noexcept;
namespace world_lineage=nimby::windows::native_world;
struct NativeWorldState {
 std::atomic_flag gate=ATOMIC_FLAG_INIT;
 std::atomic<bool> pendingLost{};
 world_lineage::Clock clock;
 world_lineage::Lineage lineage;
 std::atomic<uint64_t> busy{},stops{},starts{},runnerEntries{},captures{},invalidated{};
 std::array<std::atomic<uint64_t>,static_cast<size_t>(world_lineage::Reason::Lost)+1> reasons{};
 // Mutation callbacks never wait for an observer. Missing one operation makes
 // every current token invalid until a newly observed stop/join and start/run.
 template<class Operation> bool apply(Operation&& operation) noexcept {
  if(gate.test_and_set(std::memory_order_acquire)){pendingLost.store(true,std::memory_order_release);++busy;return false;}
  struct Unlock {NativeWorldState* value;~Unlock(){value->gate.clear(std::memory_order_release);}} unlock{this};
  if(pendingLost.exchange(false,std::memory_order_acq_rel))lineage.lose();
  operation(lineage,clock);return true;
 }
 bool current(const world_lineage::Capture& capture) noexcept {
  bool result=false;
  apply([&](auto& lineage,auto&){result=lineage.current(capture);});
  return result&&!pendingLost.load(std::memory_order_acquire);
 }
 void lose() noexcept {pendingLost.store(true,std::memory_order_release);++invalidated;}
};
NativeWorldState nativeWorld;
namespace mutation_continuity=nimby::windows::mutation_continuity;
mutation_continuity::Storage<4096,4096,8192> mutationContinuityStorage;
mutation_continuity::Journal<4096,4096,8192> mutationContinuity{mutationContinuityStorage};
struct PreserveNativeError {DWORD error=GetLastError();~PreserveNativeError(){SetLastError(error);}};
struct InvalidateOnException {
 int exceptions=std::uncaught_exceptions();
 ~InvalidateOnException(){if(std::uncaught_exceptions()>exceptions)nativeWorld.lose();}
};
uintptr_t __fastcall stopRunner(uintptr_t context){
 if(!phaseObservationEnabled)return nativeRunnerStop(context);
 const auto incoming=GetLastError();InvalidateOnException exception;
 mutationContinuity.invalidateWorld(true);
 world_lineage::Attempt attempt;
 const auto controller=context>=0x5d0?context-0x5d0:0;
 nativeWorld.apply([&](auto& lineage,auto& clock){attempt=lineage.beginStop(clock,controller,context);});
 ++nativeWorld.stops;SetLastError(incoming);
 const auto result=nativeRunnerStop(context);PreserveNativeError preserve;
 if(attempt)nativeWorld.apply([&](auto& lineage,auto&){lineage.finishStop(attempt);});
 return result;
}
void __fastcall startRunner(uintptr_t context){
 if(!phaseObservationEnabled){nativeRunnerStart(context);return;}
 const auto incoming=GetLastError();InvalidateOnException exception;
 world_lineage::Attempt attempt;
 const auto controller=context>=0x5d0?context-0x5d0:0;
 nativeWorld.apply([&](auto& lineage,auto& clock){attempt=lineage.beginStart(clock,controller,context);});
 ++nativeWorld.starts;SetLastError(incoming);
 nativeRunnerStart(context);PreserveNativeError preserve;
 if(attempt)nativeWorld.apply([&](auto& lineage,auto&){lineage.finishStart(attempt);});
}
void __fastcall mergeWorld(uintptr_t sim,uintptr_t delta){
 if(!phaseObservationEnabled){nativeWorldMerge(sim,delta);return;}
 const auto incoming=GetLastError();InvalidateOnException exception;
 mutationContinuity.invalidateWorld();
 // Native delta owns allocations and can be consumed by the merge. It is
 // forwarded unchanged exactly once; no native vector is manufactured here.
 const auto caller=NRF_PHASE_CALLER();auto token=phaseContext().expected();
 if(caller==base+0x349871&&!token.controllerReference){
  uintptr_t root{};uint32_t thread{};
  if(read(base+0xb81998,root)&&root>=0x10000&&root<0x7fffffff0000ULL-0xb38&&
     read(root+0xb28,thread)&&thread==GetCurrentThreadId()){
   token.controller=root+0x540;token.controllerReference=root+0xb10;
  }
 }
 uintptr_t controller{},database{},liveSim{};
 const bool known=caller==base+0x349871&&token.controllerReference&&
  read(token.controllerReference,controller)&&controller==token.controller&&
  read(controller,database)&&read(controller+0x140,liveSim)&&liveSim==sim;
 nativeWorld.apply([&](auto& lineage,auto& clock){
  // A newly installed observer has no historical publication to invalidate.
  // Its first world is born only at the real, exclusive finished boundary.
  if(lineage.state()==world_lineage::State::Unarmed)return;
  if(known)lineage.invalidateMerge(clock,world_lineage::MergeScope{controller,token.controllerReference,
   controller+0x4c0,database,sim,GetCurrentThreadId(),caller-base});
  else lineage.lose();
 });
 ++nativeWorld.invalidated;SetLastError(incoming);
 nativeWorldMerge(sim,delta);
}
void __fastcall modelCommandBatch(uintptr_t database,uintptr_t sim,uintptr_t geography,uintptr_t commands,uintptr_t journal){
 const auto caller=NRF_PHASE_CALLER();
 if(!phaseObservationEnabled){nativeModelCommandBatch(database,sim,geography,commands,journal);return;}
 const auto incoming=GetLastError();
 const auto rva=caller>=base?caller-base:0;
 const bool exclusiveSite=rva==0x348f07||rva==0x34928b||rva==0x3493d5;
 uintptr_t root{},controller{},controllerReference{},liveDatabase{},liveSim{};uint32_t runnerThread{};
 const auto thread=GetCurrentThreadId();
 const bool known=exclusiveSite&&read(base+0xb81998,root)&&root>=0x10000&&root<0x7fffffff0000ULL-0xb38&&
  read(root+0xb10,controllerReference)&&controllerReference==root+0x540&&
  read(root+0xb28,runnerThread)&&runnerThread==thread&&
  read(controllerReference,liveDatabase)&&liveDatabase==database&&
  read(controllerReference+0x140,liveSim)&&liveSim==sim;
 controller=known?controllerReference:0;
 auto owner=mutationContinuity.owner();
 owner.controller=controller;owner.database=database;owner.simulation=sim;owner.thread=thread;
 const mutation_continuity::BatchScope scope{owner,rva,true,exclusiveSite,known,
  [](void*,const mutation_continuity::WorldKey& value)noexcept{
   bool current=false;
   nativeWorld.apply([&](auto& lineage,auto&){current=lineage.state()==world_lineage::State::Running&&lineage.world()==value.world;});
   return current&&!nativeWorld.pendingLost.load(std::memory_order_acquire);
  },nullptr};
 const auto mutation=mutationContinuity.beginBatch(scope);
 struct Complete {
  mutation_continuity::Mutation mutation;bool returned=false;
  ~Complete()noexcept{const auto error=GetLastError();mutationContinuity.finishBatch(mutation,returned);SetLastError(error);}
 } complete{mutation};
 // 30E020 owns shared_ptr/vector/journal side effects. Forward every argument
 // unchanged exactly once; do not inspect/replay command virtuals or outputs.
 SetLastError(incoming);nativeModelCommandBatch(database,sim,geography,commands,journal);
 complete.returned=true;
}
using NativeReplan440840=uintptr_t (__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,
                                              uintptr_t,uintptr_t,uintptr_t,uintptr_t);
NativeReplan440840 nativeReplan440840=nullptr;
enum class ReplanReason:uint8_t { Disabled,Unarmed,ForeignDestinationsUnknown,Invalidated,Ambiguous,Nested,Count };
constexpr const char* replanReasonName(ReplanReason value) noexcept {
 switch(value) {
 case ReplanReason::Disabled:return "disabled";
 case ReplanReason::Unarmed:return "owner_unavailable";
 case ReplanReason::ForeignDestinationsUnknown:return "foreign_destinations_unqualified";
 case ReplanReason::Invalidated:return "known_writer_invalidated";
 case ReplanReason::Ambiguous:return "context_ambiguous";
 case ReplanReason::Nested:return "nested_writer";
 default:return "unknown";
 }
}
struct ReplanCounters {
 std::array<std::atomic<uint64_t>,static_cast<size_t>(ReplanReason::Count)> reasons{};
 std::atomic<uint64_t> reads{},completed{},exceptions{};
};
ReplanCounters replanCounters;
thread_local unsigned replanDepth=0;

// This is a BEFORE-write invalidation, not an Exclusive/current-body scope.
// Even complete matching IDs do not certify immutable input or progression.
ReplanReason invalidateBeforeReplan440840(uintptr_t caller,uintptr_t database,uintptr_t sim,
                                        uintptr_t dbTrain,uintptr_t motion)noexcept {
 const auto owner=mutationContinuity.owner();
 if(!owner.world) {
  // owner() is try-only: empty may mean Unarmed, Lost OR a busy journal.
  // invalidateWorld(true) is a no-op before bootstrap, keeps Lost latched,
  // and posts pendingLost on contention. Never silently miss a live writer.
  mutationContinuity.invalidateWorld(true);return ReplanReason::Unarmed;
 }
 const auto rva=caller>=base?caller-base:0;
 uintptr_t root{},controller{},liveDb{},liveSim{};uint32_t runner{};
 const auto copy=[&](uintptr_t address,auto& output)noexcept{
  ++replanCounters.reads;return read(address,output);
 };
 const bool currentRoot=base<=UINTPTR_MAX-0xb81998&&copy(base+0xb81998,root)&&
  root>=0x10000&&root%8==0&&root<0x7fffffff0000ULL-0xb38&&
  copy(root+0xb10,controller)&&controller==root+0x540&&controller==owner.controller&&
  copy(root+0xb28,runner)&&runner==owner.thread&&
  copy(controller,liveDb)&&liveDb==owner.database&&
  copy(controller+0x140,liveSim)&&liveSim==owner.simulation;
 bool liveLineage=false;
 const bool lineageRead=nativeWorld.apply([&](auto& lineage,auto&){
  liveLineage=lineage.state()==world_lineage::State::Running&&lineage.world()==owner.world;
 });
 const bool current=currentRoot&&lineageRead&&liveLineage&&
  !nativeWorld.pendingLost.load(std::memory_order_acquire);
 // DB/Sim both foreign DO NOT prove the write destinations foreign: arg6 is
 // directly mutated and arg4 can update reservations. A borrowed live Motion
 // or DBTrain can be mixed into this pair. Without complete native address
 // provenance, keep the original unchanged but latch the tracked journal Lost.
 // Full-ID equality/difference across worlds is not an exclusion witness.
 if(current&&database&&sim&&database!=owner.database&&sim!=owner.simulation) {
  mutationContinuity.invalidateWorld(true);return ReplanReason::ForeignDestinationsUnknown;
 }
 if(replanDepth)return mutationContinuity.invalidateWorld(true),ReplanReason::Nested;
 uint64_t trainId{},motionId{};
 const bool supported=rva==0x447c26||rva==0x448174;
 const bool ids=current&&supported&&database==owner.database&&sim==owner.simulation&&
  dbTrain>=0x10000&&motion>=0x10000&&dbTrain%8==0&&motion%8==0&&
  dbTrain<0x7fffffff0000ULL&&motion<0x7fffffff0000ULL&&
  copy(dbTrain,trainId)&&copy(motion,motionId)&&
  nimby::windows::consist_inventory::detail::fullTrain(trainId)&&trainId==motionId;
 // Neither runner==worker nor currentStep ancestry is required: 447A20 can
 // replan during preparation before 448710. These facts identify only the
 // target WORLD of the known writer, not a safe reader/callback scope.
 // Global invalidation covers old/new attached roots without a guessed list.
 mutationContinuity.invalidateWorld(!ids);
 return ids?ReplanReason::Invalidated:ReplanReason::Ambiguous;
}

uintptr_t __fastcall replanRoute440840(uintptr_t output,uintptr_t database,uintptr_t sim,
 uintptr_t reservationBase,uintptr_t dbTrain,uintptr_t motion,
 uintptr_t nativePresence,uintptr_t nativeService) {
 if(!phaseObservationEnabled) {
  ++replanCounters.reasons[static_cast<size_t>(ReplanReason::Disabled)];
  return nativeReplan440840(output,database,sim,reservationBase,dbTrain,motion,nativePresence,nativeService);
 }
 const auto incoming=GetLastError();
 const auto caller=NRF_PHASE_CALLER();
 const auto reason=invalidateBeforeReplan440840(caller,database,sim,dbTrain,motion);
 ++replanCounters.reasons[static_cast<size_t>(reason)];
 // No gate is held over the native call; completion NEVER restores a stamp.
 struct Completion {
  bool tracked,returned=false;
  Completion(bool value)noexcept:tracked(value){++replanDepth;}
  ~Completion()noexcept {
   const auto error=GetLastError();--replanDepth;
   if(returned)++replanCounters.completed;
   else {++replanCounters.exceptions;if(tracked)mutationContinuity.invalidateWorld(true);}
   SetLastError(error);
  }
 } completion{reason!=ReplanReason::Unarmed};
 // Preserve all eight opaque arguments, native output bytes and full result.
 // No replay, callback, output read, AL modification, native lock or wait.
 SetLastError(incoming);
 const auto result=nativeReplan440840(output,database,sim,reservationBase,dbTrain,motion,nativePresence,nativeService);
 completion.returned=true;return result;
}
struct NativeWorldCapture {
 world_lineage::Result result;
 explicit NativeWorldCapture(const nimby::windows::phase_ownership::Window& window) noexcept {
  if(!window.qualified())return;
  const auto& token=window.token;
  const world_lineage::Scope scope{token.controller,token.controllerReference,token.controller+0x4c0,
   token.network,token.simulation,GetCurrentThreadId(),token.advanceReturn,token.simulationReturn,token.trainPhaseReturn};
  nativeWorld.apply([&](auto& lineage,auto& clock){result=lineage.capturePhase(clock,scope);});
  ++nativeWorld.reasons[static_cast<size_t>(result.reason)];
  if(result.capture)++nativeWorld.captures;
 }
 explicit NativeWorldCapture(const world_lineage::Scope& scope) noexcept {
  nativeWorld.apply([&](auto& lineage,auto& clock){
   result=lineage.state()==world_lineage::State::Unarmed&&scope.currentRootVerified&&scope.lifecycleHooksActive?
    lineage.adoptOwnedWorld(clock,scope):lineage.capturePhase(clock,scope);
  });
  ++nativeWorld.reasons[static_cast<size_t>(result.reason)];
  if(result.capture)++nativeWorld.captures;
 }
 bool current() noexcept {return result.capture&&nativeWorld.current(result.capture);}
 ~NativeWorldCapture(){
  if(result.capture)nativeWorld.apply([&](auto& lineage,auto&){lineage.endCapture(result.capture);});
 }
};
std::atomic<uint64_t> exclusiveCaptureAttempts{},exclusiveCaptureBusy{},exclusiveCaptureInvalid{};
void observeExclusiveWorld(const world_lineage::Capture& capture) noexcept;
void WINAPI releaseShared(PSRWLOCK lock){
 const auto incoming=GetLastError();
 const auto caller=NRF_PHASE_CALLER();
 phase_owner::Token token;
 const bool ownedRunner=phaseObservationEnabled&&phaseAncestry&&!phaseAncestry->empty();
 if(ownedRunner)token=phaseAncestry->expected();
 // Installation can happen after entry into long-lived 3489D0. The exact
 // release site still owns the current Root's native lock. Adoption creates
 // a NEW empty lineage; it never resumes older movement or leases.
 uintptr_t currentRoot{};bool rootVerified=false;
 if(phaseObservationEnabled&&caller==base+0x349fa3&&!ownedRunner&&
    read(base+0xb81998,currentRoot)&&currentRoot>=0x10000&&currentRoot<0x7fffffff0000ULL-0xb38&&
    reinterpret_cast<uintptr_t>(lock)==currentRoot+0xa00){
  token.controller=currentRoot+0x540;token.controllerReference=token.controller+0x5d0;rootVerified=true;
 }
 const bool eligible=phaseObservationEnabled&&(ownedRunner||rootVerified)&&caller==base+0x349fa3&&token.controller>=0x10000&&
  reinterpret_cast<uintptr_t>(lock)==token.controller+0x4c0;
 // Never upgrade a held SRW lock. The native release happens first, exactly
 // once, including foreign callers and native exception cleanup paths.
 SetLastError(incoming);nativeSharedRelease(lock);PreserveNativeError preserve;
 if(!eligible)return;
 static thread_local uint64_t nextDiagnosticCapture=0;
 const auto now=GetTickCount64();
 if(now<nextDiagnosticCapture)return;
 nextDiagnosticCapture=now+100;
 ++exclusiveCaptureAttempts;
 if(!TryAcquireSRWLockExclusive(lock)){++exclusiveCaptureBusy;return;}
 struct Unlock {PSRWLOCK value;~Unlock(){ReleaseSRWLockExclusive(value);}} unlock{lock};
 // Load/unload joins this runner before freeing Controller. The gap between
 // release and TryExclusive still requires a fresh current identity readback.
 uintptr_t controller{},database{},sim{};
 if(!read(token.controllerReference,controller)||controller!=token.controller||
    !read(controller,database)||!read(controller+0x140,sim)||!database||!sim||sim!=currentSession()){
  ++exclusiveCaptureInvalid;nativeWorld.lose();return;
 }
 uint32_t nativeThread{};
 if(rootVerified&&(!read(token.controllerReference+0x18,nativeThread)||nativeThread!=GetCurrentThreadId())){
  ++exclusiveCaptureInvalid;return;
 }
 const world_lineage::Scope scope{controller,token.controllerReference,reinterpret_cast<uintptr_t>(lock),
  database,sim,GetCurrentThreadId(),0,0,0,world_lineage::Boundary::PostSharedExclusive,caller-base,true,true,
  installed&&phaseObservationEnabled,rootVerified,nativeThread};
 NativeWorldCapture world(scope);
 if(!world.current()){++exclusiveCaptureInvalid;return;}
 observeExclusiveWorld(world.result.capture);
 if(!world.current())++exclusiveCaptureInvalid;
}
// A local invalidation epoch detects observed resets even if Sim's address and
// its native +2120 counter are reused. It is NOT a world identity or a proof
// that loading has completed. Native imports may continue after reset returns.
struct PhaseObservationState {
 using Reason=nimby::windows::automatic::PhaseCaptureReason;
 struct Sample {nimby::windows::automatic::TrainPhaseSample value;uint64_t epoch{},phase{};};
 std::atomic<uint64_t> epoch{1},resets{},phases{},captured{},busy{},contextUnavailable{},budgetExceeded{},samplesDropped{};
 std::atomic<uint32_t> resetting{};
 std::array<std::atomic<uint64_t>,static_cast<size_t>(Reason::Count)> reasons{};
 std::atomic_flag owner=ATOMIC_FLAG_INIT,sampleOwner=ATOMIC_FLAG_INIT;
 std::array<Sample,8> samples{};size_t sampleCount=0,cursor=0;
 // Universal roster is independent of SDK rule publications. Storage belongs
 // to the bridge, never the native pool; only a qualified complete attempt
 // becomes a diagnostic sample. No foreign address escapes this workspace.
 using Inventory=nimby::windows::consist_inventory::Result;
 using Row=nimby::windows::consist_inventory::Row;
 std::array<Row,4096> roster{},rosterScratch{};
 std::array<uint64_t,8192> rosterIds{};
 struct InventorySample {Inventory result;uint64_t epoch{},phase{};};
 InventorySample inventorySample{};
 bool hasInventorySample=false;
 std::array<std::atomic<uint64_t>,static_cast<size_t>(nimby::windows::consist_inventory::Reason::UnsupportedCouplingChain)+1> inventoryReasons{};
 std::atomic<uint64_t> inventoryAttempts{},inventoryComplete{};
 void inventory(const Inventory& result,uint64_t epoch,uint64_t phase) noexcept {
  ++inventoryReasons[static_cast<size_t>(result.reason)];++inventoryAttempts;
  if(result.observedAllSlots())++inventoryComplete;
  if(sampleOwner.test_and_set(std::memory_order_acquire)){++samplesDropped;return;}
  inventorySample={result,epoch,phase};hasInventorySample=true;
  sampleOwner.clear(std::memory_order_release);
 }
 void sample(const Sample& value) noexcept {
  if(sampleOwner.test_and_set(std::memory_order_acquire)){++samplesDropped;return;}
  if(sampleCount<samples.size())samples[sampleCount++]=value;else ++samplesDropped;
  sampleOwner.clear(std::memory_order_release);
 }
};
PhaseObservationState phaseObservation;
// Bootstrap-owned scratch, not allocated per train or put on the game's
// worker stack. Current geometry is not a historical rear-release witness.
nimby::windows::owned_rear::Storage ownedBodyStorage;
struct OwnedBodyDiagnostics {
 std::array<std::atomic<uint64_t>,static_cast<size_t>(nimby::windows::owned_rear::Reason::InvalidLimits)+1> reasons{};
 std::array<std::atomic<uint64_t>,static_cast<size_t>(nimby::windows::consist_rear::Reason::Count)> providerReasons{};
 std::atomic<uint64_t> attempts{},ready{},cars{},portions{},lastTrain{},lastPhase{};
 size_t cursor=0;
} ownedBodyDiagnostics;
void observeExclusiveWorld(const world_lineage::Capture& capture) noexcept {
 if(capture.boundary!=world_lineage::Boundary::PostSharedExclusive)return;
 if(phaseObservation.owner.test_and_set(std::memory_order_acquire)){++phaseObservation.busy;return;}
 struct Release{~Release(){phaseObservation.owner.clear(std::memory_order_release);}} release;
 LARGE_INTEGER started{},frequency{};
 if(!QueryPerformanceCounter(&started)||!QueryPerformanceFrequency(&frequency)){
  ++phaseObservation.contextUnavailable;return;
 }
 nimby::windows::automatic::RouteReadBudget budget(started.QuadPart,frequency.QuadPart);
 struct BudgetContext{nimby::windows::automatic::RouteReadBudget* budget;};
 BudgetContext context{&budget};
 const auto preflight=[](void* opaque,size_t bytes) noexcept {
  auto* value=static_cast<BudgetContext*>(opaque);LARGE_INTEGER tick{};
  if(!QueryPerformanceCounter(&tick))return false;
  return bytes?value->budget->allow(bytes,tick.QuadPart):value->budget->available(tick.QuadPart);
 };
 nimby::windows::consist_inventory::Budget inventoryBudget{preflight,&context};
 nimby::windows::consist_inventory::Limits inventoryLimits;inventoryLimits.rows=phaseObservation.roster.size();
 const nimby::windows::consist_inventory::OwnedScope ownedInventoryScope{
  [](void* opaque)noexcept{const auto& value=*static_cast<const world_lineage::Capture*>(opaque);
   return value.thread==GetCurrentThreadId()&&nativeWorld.current(value);},const_cast<world_lineage::Capture*>(&capture)};
 const auto inventory=nimby::windows::consist_inventory::captureMotionInventoryOwned(capture.simulation,true,
  ownedInventoryScope,readBytes,inventoryBudget,phaseObservation.roster,
  {phaseObservation.rosterScratch,phaseObservation.rosterIds},inventoryLimits);
 if(!inventory.observedAllSlots()||nativeWorld.current(capture))
  phaseObservation.inventory(inventory,capture.world,capture.phase);
 else ++phaseObservation.contextUnavailable;
 if(!inventory.observedAllSlots()||!nativeWorld.current(capture))return;
 namespace owned=nimby::windows::owned_rear;
 const owned::Ownership ownership{capture,
  [](void*,const world_lineage::Capture& value)noexcept{return nativeWorld.current(value);},
  [](void*)noexcept->uint64_t{return GetCurrentThreadId();},nullptr};
 const owned::Roster roster{capture,inventory,{phaseObservation.roster.data(),inventory.rows}};
 mutationContinuity.bind(ownership);
 if(!inventory.rows)return;
 struct ContinuityVisitorContext {
  const owned::Ownership* ownership;const owned::Roster* roster;
  nimby::windows::consist_inventory::Budget* budget;
 } continuityContext{&ownership,&roster,&inventoryBudget};
 // This rotating diagnostic copy is deliberately never published as a
 // complete world occupancy atlas. Unknown/old portions cannot clear leases.
 for(size_t visited=0;visited<inventory.rows;++visited){
  const auto index=ownedBodyDiagnostics.cursor++%inventory.rows;
  if(phaseObservation.roster[index].attached)continue;
  const auto result=ownedBodyStorage.capture(ownership,roster,phaseObservation.roster[index].train,true,readBytes,
   inventoryBudget,[](void* opaque,const owned::Body& body)noexcept{
    ownedBodyDiagnostics.cars.fetch_add(body.cars.size(),std::memory_order_relaxed);
    ownedBodyDiagnostics.portions.fetch_add(body.portions.size(),std::memory_order_relaxed);
    if(!body.publication.physicallyAbsent){
     const auto& context=*static_cast<ContinuityVisitorContext*>(opaque);
     mutation_continuity::Budget budget{65536,0,context.budget};
     mutationContinuity.observe(*context.ownership,*context.roster,body,budget);
    }
   },&continuityContext);
  ++ownedBodyDiagnostics.attempts;++ownedBodyDiagnostics.reasons[static_cast<size_t>(result.reason)];
  const auto providerReason=static_cast<size_t>(result.providerReason);
  if(providerReason<ownedBodyDiagnostics.providerReasons.size())++ownedBodyDiagnostics.providerReasons[providerReason];
  if(result.ready()){++ownedBodyDiagnostics.ready;ownedBodyDiagnostics.lastTrain.store(result.train);
   ownedBodyDiagnostics.lastPhase.store(capture.phase);}
  break;
 }
}
std::array<std::atomic<uint64_t>,static_cast<size_t>(phase_owner::Reason::Count)> phaseOwnershipReasons{};
phase_owner::Context& phaseContext() noexcept {
 if(!phaseAncestry)phaseAncestry.emplace(phase_owner::Profile{base,true});
 return *phaseAncestry;
}
template<class Read> bool readPhaseController(uintptr_t reference,uintptr_t& controller,uintptr_t& sim,
 uintptr_t& network,phase_owner::ResourceRef& resource,Read&& reader) noexcept {
 return phase_owner::detail::address(reference,8)&&reader(reference,&controller,sizeof controller)&&
  phase_owner::detail::address(controller,0x4c8)&&reader(controller+0x140,&sim,sizeof sim)&&
  reader(controller,&network,sizeof network)&&reader(controller+0x220,&resource,sizeof resource);
}
template<class Read> phase_owner::TrainPhaseFrame phaseFrame(uintptr_t trainState,uintptr_t network,uintptr_t sim,
 int64_t elapsed,uintptr_t caller,Read&& reader) noexcept {
 phase_owner::TrainPhaseFrame frame;
 frame.trainState=trainState;frame.network=network;frame.simulation=sim;frame.elapsed=elapsed;frame.returnAddress=caller;
 const auto expected=phaseContext().expected();frame.controllerReference=expected.controllerReference;
 frame.readComplete=readPhaseController(frame.controllerReference,frame.controller,frame.controllerSimulation,
  frame.controllerNetwork,frame.controllerResource,reader)&&
  phase_owner::detail::address(expected.advanceResourceReference,16)&&reader(expected.advanceResourceReference,&frame.advanceResource,sizeof frame.advanceResource)&&
  phase_owner::detail::address(expected.simulationResourceReference,16)&&reader(expected.simulationResourceReference,&frame.simulationResource,sizeof frame.simulationResource)&&
  phase_owner::detail::address(expected.descriptor,24)&&reader(expected.descriptor,&frame.ordinal,sizeof frame.ordinal)&&
  reader(expected.descriptor+0x10,&frame.requestedElapsed,sizeof frame.requestedElapsed);
 return frame;
}
phase_owner::TrainPhaseFrame phaseFrame(uintptr_t trainState,uintptr_t network,uintptr_t sim,
 int64_t elapsed,uintptr_t caller) noexcept {return phaseFrame(trainState,network,sim,elapsed,caller,readBytes);}
void __fastcall controllerPhase(uintptr_t reference){
 if(!phaseObservationEnabled){nativeControllerPhase(reference);return;}
 const auto incomingError=GetLastError();
 // A long-lived controller entry is not itself a locked world. The exact
 // descendant callsite identifies the studied Shared interval later.
 if(!phaseAncestry)phaseAncestry.emplace(phase_owner::Profile{base,true});
 else if(phaseAncestry->empty())phaseAncestry->profileWhenEmpty({base,true});
 phase_owner::ControllerFrame frame{reference};
 frame.readComplete=phase_owner::detail::address(reference,8)&&read(reference,frame.controller);
 phase_owner::Scope scope(phaseContext(),frame);
 const auto runnerThread=GetCurrentThreadId();InvalidateOnException exception;
 nativeWorld.apply([&](auto& lineage,auto&){lineage.runnerEnter(frame.controller,reference,runnerThread);});
 ++nativeWorld.runnerEntries;
 SetLastError(incomingError);
 nativeControllerPhase(reference);
 PreserveNativeError preserve;
 nativeWorld.apply([&](auto& lineage,auto&){lineage.runnerExit(runnerThread);});
}
void __fastcall advancePhase(uintptr_t sim,uintptr_t reference,uintptr_t network,uintptr_t descriptor){
 if(!phaseObservationEnabled){nativeAdvancePhase(sim,reference,network,descriptor);return;}
 const auto incomingError=GetLastError();
 const auto caller=NRF_PHASE_CALLER();
 phase_owner::AdvanceFrame frame;
 frame.simulation=sim;frame.network=network;frame.resourceReference=reference;frame.descriptor=descriptor;frame.returnAddress=caller;
 frame.controllerReference=phaseContext().expected().controllerReference;
 frame.readComplete=readPhaseController(frame.controllerReference,frame.controller,frame.controllerSimulation,
  frame.controllerNetwork,frame.controllerResource,readBytes)&&
  phase_owner::detail::address(reference,16)&&read(reference,frame.resource)&&
  phase_owner::detail::address(descriptor,24)&&read(descriptor,frame.ordinal)&&read(descriptor+0x10,frame.elapsed);
 phase_owner::Scope scope(phaseContext(),frame);
 SetLastError(incomingError);
 nativeAdvancePhase(sim,reference,network,descriptor);
}
void __fastcall simulationPhase(uintptr_t sim,uintptr_t reference,uintptr_t network,int64_t elapsed){
 if(!phaseObservationEnabled){nativeSimulationPhase(sim,reference,network,elapsed);return;}
 const auto incomingError=GetLastError();
 const auto caller=NRF_PHASE_CALLER();
 phase_owner::SimulationFrame frame;
 frame.simulation=sim;frame.network=network;frame.resourceReference=reference;frame.returnAddress=caller;frame.elapsed=elapsed;
 frame.readComplete=phase_owner::detail::address(reference,16)&&read(reference,frame.resource);
 phase_owner::Scope scope(phaseContext(),frame);
 SetLastError(incomingError);
 nativeSimulationPhase(sim,reference,network,elapsed);
}
struct PhaseResetScope {
 bool enabled;
 explicit PhaseResetScope(bool value) noexcept:enabled(value){if(enabled){
   mutationContinuity.invalidateWorld(true);
  phaseObservation.resetting.fetch_add(1,std::memory_order_acq_rel);
  phaseObservation.epoch.fetch_add(1,std::memory_order_acq_rel);++phaseObservation.resets;
 }}
 ~PhaseResetScope(){if(enabled){
  phaseObservation.epoch.fetch_add(1,std::memory_order_acq_rel);
  phaseObservation.resetting.fetch_sub(1,std::memory_order_acq_rel);
 }}
};
void __fastcall resetWorld(uintptr_t sim){PhaseResetScope reset(phaseObservationEnabled);nativeWorldReset(sim);}
void __fastcall rebuildWorld(uintptr_t sim,uintptr_t network){PhaseResetScope reset(phaseObservationEnabled);nativeWorldRebuild(sim,network);}
void observeTrainPhase(uintptr_t trainState,uintptr_t network,uintptr_t sim,uint64_t epoch,
 phase_owner::Scope& ownership,int64_t elapsed,uintptr_t caller) noexcept {
 using namespace nimby::windows::automatic;
 const auto phase=phaseObservation.phases.fetch_add(1,std::memory_order_relaxed)+1;
 if(sim<0x10000||sim>UINTPTR_MAX-0xa0||trainState!=sim+0xa0||network<0x10000||
    sim!=currentSession()||phaseObservation.resetting.load(std::memory_order_acquire)||
    phaseObservation.epoch.load(std::memory_order_acquire)!=epoch){++phaseObservation.contextUnavailable;return;}
 if(phaseObservation.owner.test_and_set(std::memory_order_acquire)){++phaseObservation.busy;return;}
 struct Release{~Release(){phaseObservation.owner.clear(std::memory_order_release);}}release;
 try {
  Publications::View view(publications);
  LARGE_INTEGER started{},frequency{},now{};
  QueryPerformanceCounter(&started);QueryPerformanceFrequency(&frequency);
  RouteReadBudget budget(started.QuadPart,frequency.QuadPart);
  std::array<PhaseObservationState::Sample,32> pending{};size_t pendingCount=0;
  const auto available=[&]{QueryPerformanceCounter(&now);return budget.available(now.QuadPart);};
  const auto boundedRead=[&](uintptr_t address,void* output,size_t size){
   QueryPerformanceCounter(&now);return budget.allow(size,now.QuadPart)&&readBytes(address,output,size);
  };
  // Observe all native Motion slots, including vanilla, stopped and attached
  // trains. A published mod rule is not a prerequisite for world protection.
  // The inventory and managed diagnostics share ONE terminal read/time budget.
  struct InventoryBudget {RouteReadBudget* budget;};
  InventoryBudget inventoryContext{&budget};
  const auto preflight=[](void* context,size_t bytes) noexcept {
   auto* value=static_cast<InventoryBudget*>(context);LARGE_INTEGER tick{};
   if(!QueryPerformanceCounter(&tick))return false;
   return bytes?value->budget->allow(bytes,tick.QuadPart):value->budget->available(tick.QuadPart);
  };
  nimby::windows::consist_inventory::Budget inventoryBudget{preflight,&inventoryContext};
  const auto inventory=nimby::windows::consist_inventory::captureMotionInventory(sim,true,readBytes,
   inventoryBudget,phaseObservation.roster,{phaseObservation.rosterScratch,phaseObservation.rosterIds});
  if(!inventory.observedAllSlots()){
   // Refusals carry no roster or authority. Keep the exact failed preflight
   // diagnostic even when the terminal budget forbids a further readback.
   phaseObservation.inventory(inventory,epoch,phase);
   if(!available()){++phaseObservation.budgetExceeded;return;}
  }
  // Rotate over the fixed train table. This is a bounded SAMPLE, never a claim
  // of complete world/consist coverage. No game calls or waiting for peers.
  size_t attempted=0,visited=0;
  for(;view->active()&&view->session==sim&&view->world&&visited<TrainWorld::capacity&&attempted<32;++visited){
   // Empty slots need only a periodic clock check. Every real memory read
   // still consumes the budget individually; avoid 8192 QPC calls per sample.
   if(visited%32==0&&!available())break;
   auto& slot=view->world->slots[phaseObservation.cursor];
   phaseObservation.cursor=(phaseObservation.cursor+1)%TrainWorld::capacity;
   if(!slot.key.load(std::memory_order_acquire))continue;
   if(!TryAcquireSRWLockExclusive(&slot.lock)){++phaseObservation.busy;continue;}
   struct Unlock{SRWLOCK* lock;~Unlock(){ReleaseSRWLockExclusive(lock);}}unlock{&slot.lock};
   if(slot.id>>48!=5||!slot.state.motion)continue;
   ++attempted;const auto result=captureTrainPhase(slot.state.motion,slot.id,boundedRead);
   ++phaseObservation.reasons[static_cast<size_t>(result.reason)];
   if(result.reason==PhaseCaptureReason::Ready)pending[pendingCount++]={result.sample,epoch,phase};
  }
  if(!available()){++phaseObservation.budgetExceeded;return;}
  if(sim!=currentSession()||phaseObservation.resetting.load(std::memory_order_acquire)||
     phaseObservation.epoch.load(std::memory_order_acquire)!=epoch){++phaseObservation.contextUnavailable;return;}
  const auto window=ownership.afterReturn(phaseFrame(trainState,network,sim,elapsed,caller,boundedRead));
  if(!available()){++phaseObservation.budgetExceeded;return;}
  if(!window.qualified()){
   ++phaseOwnershipReasons[static_cast<size_t>(window.reason)];++phaseObservation.contextUnavailable;return;
  }
  if(!available()){++phaseObservation.budgetExceeded;return;}
  if(inventory.observedAllSlots())phaseObservation.inventory(inventory,epoch,phase);
  phaseObservation.captured.fetch_add(pendingCount,std::memory_order_relaxed);
  for(size_t i=0;i<pendingCount;++i)phaseObservation.sample(pending[i]);
 }catch(...){++phaseObservation.contextUnavailable;}
}
void __fastcall trainPhase(uintptr_t trainState,uintptr_t network,uintptr_t sim,int64_t elapsed){
 if(!phaseObservationEnabled){nativeTrainPhase(trainState,network,sim,elapsed);return;}
 const auto incomingError=GetLastError();
 const auto caller=NRF_PHASE_CALLER();
 phase_owner::Scope ownership(phaseContext(),phaseFrame(trainState,network,sim,elapsed,caller));
 const auto epoch=phaseObservation.epoch.load(std::memory_order_acquire);
 // Invoke the native train phase once, before observing. Preserve its errors;
 // only the SDK diagnostic work is noexcept and may be discarded.
 SetLastError(incomingError);
 nativeTrainPhase(trainState,network,sim,elapsed);
 struct RestoreNativeError{DWORD error=GetLastError();~RestoreNativeError(){SetLastError(error);}}restoreNativeError;
 const auto window=ownership.afterReturn(phaseFrame(trainState,network,sim,elapsed,caller));
 ++phaseOwnershipReasons[static_cast<size_t>(window.reason)];
 if(!window.qualified()){++phaseObservation.phases;++phaseObservation.contextUnavailable;return;}
 NativeWorldCapture world(window);
 // Diagnostics are sampled in wall time, never once per accelerated game
 // substep. This cap belongs to logging only and grants no safety permission.
 static thread_local uint64_t nextDiagnostic=0;
 const auto now=GetTickCount64();
 if(now>=nextDiagnostic){nextDiagnostic=now+100;observeTrainPhase(trainState,network,sim,epoch,ownership,elapsed,caller);}
 // Lignée alone is a separate diagnostic: a valid lifecycle does not promote
 // the inventory/readbacks into stable history or movement permission.
 if(world.result.capture&&!world.current())++phaseObservation.contextUnavailable;
}
void appendFinalAdmissionDiagnostics(std::ostream& message);
void drainTrainPhases() noexcept {
 if(!phaseObservationEnabled)return;
 static std::atomic_flag reporting=ATOMIC_FLAG_INIT;
 if(reporting.test_and_set(std::memory_order_acquire))return;
 struct Release{~Release(){reporting.clear(std::memory_order_release);}}release;
 try {
  static uint64_t nextReport=0,lastPhases=0;
  const auto now=GetTickCount64(),phases=phaseObservation.phases.load(std::memory_order_relaxed);
  if(now<nextReport||phases==lastPhases)return;
  nextReport=now+30000;lastPhases=phases;
  std::array<PhaseObservationState::Sample,8> samples{};size_t count=0;
  PhaseObservationState::InventorySample inventory{};bool hasInventory=false;
  if(!phaseObservation.sampleOwner.test_and_set(std::memory_order_acquire)){
   count=phaseObservation.sampleCount;std::copy_n(phaseObservation.samples.begin(),count,samples.begin());phaseObservation.sampleCount=0;
   hasInventory=phaseObservation.hasInventorySample;inventory=phaseObservation.inventorySample;
   phaseObservation.hasInventorySample=false;
   phaseObservation.sampleOwner.clear(std::memory_order_release);
  }
  std::ostringstream message;
  message<<"Native train phase observation: scope=owned_post_train_phase_44d6b0 diagnostic_only=1 stable_tick_certified=0 complete_rear_coverage=0 snapshot_permissions_changed=0"
    <<" phases="<<phases<<" locally_revalidated="<<phaseObservation.captured.load()
    <<" reset_epoch="<<phaseObservation.epoch.load()<<" observed_resets="<<phaseObservation.resets.load()
    <<" busy="<<phaseObservation.busy.load()<<" context_unavailable="<<phaseObservation.contextUnavailable.load()
    <<" budget_exceeded="<<phaseObservation.budgetExceeded.load()<<" samples_dropped="<<phaseObservation.samplesDropped.load()<<" readback_attempt_reasons={";
  for(size_t i=0;i<phaseObservation.reasons.size();++i)if(auto count=phaseObservation.reasons[i].load())
   message<<phaseCaptureReasonName(static_cast<PhaseObservationState::Reason>(i))<<':'<<count<<' ';
  message<<"} ancestry_reasons={";
  for(size_t i=0;i<phaseOwnershipReasons.size();++i)if(auto count=phaseOwnershipReasons[i].load())
   message<<phase_owner::reasonName(static_cast<phase_owner::Reason>(i))<<':'<<count<<' ';
  message<<'}';
  const auto continuity=mutationContinuity.counters();
  message<<" continuity_journal={producers=30e020_exclusive_batch,440840_before_replan native_writers_complete=0 progression=0 lease_release=0"
    <<" world="<<continuity.world<<" global_mutation="<<continuity.globalMutation
    <<" begun="<<continuity.begun<<" completed="<<continuity.completed<<" exceptions="<<continuity.exceptions
    <<" unqualified="<<continuity.unqualified<<" foreign="<<continuity.foreign
    <<" prebootstrap="<<continuity.prebootstrap<<" nested="<<continuity.nested<<" busy="<<continuity.busy
    <<" tuple_changes="<<continuity.tupleChanges<<" observations="<<continuity.compositionObservations
    <<" capacity="<<continuity.capacity<<" budget="<<continuity.budget<<" lost="<<continuity.lost
    <<" roots="<<continuity.roots<<" members="<<continuity.members<<" cars="<<continuity.cars<<'}';
  message<<" replan_440840={diagnostic_only=1 reads="<<replanCounters.reads.load()
    <<" completed="<<replanCounters.completed.load()<<" exceptions="<<replanCounters.exceptions.load()<<" reasons=";
  for(size_t i=0;i<replanCounters.reasons.size();++i)if(auto count=replanCounters.reasons[i].load())
   message<<replanReasonName(static_cast<ReplanReason>(i))<<':'<<count<<' ';
  message<<'}';
  message<<" lifecycle={stops="<<nativeWorld.stops.load()<<" starts="<<nativeWorld.starts.load()
    <<" runners="<<nativeWorld.runnerEntries.load()<<" captures="<<nativeWorld.captures.load()
    <<" busy="<<nativeWorld.busy.load()<<" invalidated="<<nativeWorld.invalidated.load()<<" reasons=";
  for(size_t i=0;i<nativeWorld.reasons.size();++i)if(auto value=nativeWorld.reasons[i].load())
   message<<i<<':'<<value<<' ';
  message<<'}';
  message<<" exclusive_capture={attempts="<<exclusiveCaptureAttempts.load()<<" busy="<<exclusiveCaptureBusy.load()
    <<" invalid="<<exclusiveCaptureInvalid.load()<<" waits=0}";
  appendFinalAdmissionDiagnostics(message);
  message<<" owned_body={attempts="<<ownedBodyDiagnostics.attempts.load()<<" ready="<<ownedBodyDiagnostics.ready.load()
    <<" cars="<<ownedBodyDiagnostics.cars.load()<<" portions="<<ownedBodyDiagnostics.portions.load()
    <<" last_train="<<ownedBodyDiagnostics.lastTrain.load()<<" last_phase="<<ownedBodyDiagnostics.lastPhase.load()
    <<" current_only=1 historical_release=0 reasons=";
  for(size_t i=0;i<ownedBodyDiagnostics.reasons.size();++i)if(auto value=ownedBodyDiagnostics.reasons[i].load())message<<i<<':'<<value<<' ';
  message<<" provider_reasons=";
  for(size_t i=0;i<ownedBodyDiagnostics.providerReasons.size();++i)if(auto value=ownedBodyDiagnostics.providerReasons[i].load())
   message<<nimby::windows::consist_rear::reasonName(static_cast<nimby::windows::consist_rear::Reason>(i))<<':'<<value<<' ';
  message<<'}';
  message<<" inventory_attempts="<<phaseObservation.inventoryAttempts.load()
    <<" complete_inventory_attempts="<<phaseObservation.inventoryComplete.load()<<" inventory_reasons={";
  for(size_t i=0;i<phaseObservation.inventoryReasons.size();++i)if(auto value=phaseObservation.inventoryReasons[i].load())
   message<<nimby::windows::consist_inventory::reasonName(static_cast<nimby::windows::consist_inventory::Reason>(i))<<':'<<value<<' ';
  message<<'}';
  if(hasInventory){const auto& value=inventory.result;
   message<<" inventory={phase="<<inventory.phase<<" epoch="<<inventory.epoch
    <<" reason="<<nimby::windows::consist_inventory::reasonName(value.reason)<<" rows="<<value.rows
    <<" blocks="<<value.blocks<<" visited_slots="<<value.visitedSlots<<" reads="<<value.reads
    <<" bytes="<<value.bytes<<" train="<<value.train<<" parent="<<value.parent
    <<" observed_train="<<value.observedTrain<<" ordinal="<<value.ordinal
    <<" complete_consist_certified=0}";
  }
  for(size_t i=0;i<count;++i){const auto& sample=samples[i].value;
   message<<" sample={capture_epoch="<<samples[i].epoch<<" observer_phase="<<samples[i].phase
    <<" train="<<sample.train<<" track="<<sample.track<<" fraction="<<sample.fraction
    <<" direction="<<int(sample.direction)<<" orientation="<<int(sample.orientation)
     <<" native_length="<<sample.nativeLength<<" history_rows="<<sample.historyCount
     <<" history_fingerprint="<<sample.historyFingerprint<<" active="<<sample.active<<" coupled="<<sample.coupled
     <<" parent="<<sample.parent<<" coupling_order="<<sample.couplingOrder<<" coupling_reversed="<<sample.couplingReversed
     <<" retained_cars="<<sample.retainedCars<<" own_retained_cars="<<sample.ownRetainedCars
     <<" foreign_retained_cars="<<sample.foreignRetainedCars<<" unowned_retained_cars="<<sample.unownedRetainedCars
     <<" attachment_rows="<<sample.attachmentCount<<" composition_fingerprint="<<sample.compositionFingerprint
     <<" attachment_fingerprint="<<sample.attachmentFingerprint<<" complete_consist_certified=0}";
  }
  nimby::detail::diagnostics::write("sdk","INFO",message.str().c_str());
 }catch(...){/* Diagnostic formatting and sinks never hold a native/train lock. */}
}
// Hooks never acquire the publication lock. Only an SDK publication may
// yield briefly to another publisher; native steps never take this lock.
// One retry budget (4096 attempts or 2 ms, whichever comes first) covers all
// locks/optimistic commits in this publication; it never encloses native calls
// or allocations. Each train independently guards only its own mutable state.
// Neither the original lease deadline nor its owner/version checks are reset.
struct PublicationBudget {
 LARGE_INTEGER frequency{},started{};unsigned attempts=0;
 PublicationBudget(){QueryPerformanceFrequency(&frequency);}
 bool retry() noexcept {
  if(++attempts>4096||!frequency.QuadPart)return false;
  LARGE_INTEGER now{};QueryPerformanceCounter(&now);
  if(!started.QuadPart)started=now;
  else if(now.QuadPart-started.QuadPart>=frequency.QuadPart/500)return false; // 2 ms observed wall time; OS descheduling is not bounded here.
  if(attempts%16==0)SwitchToThread();else YieldProcessor();
  return true;
 }
};
struct PublicationLock {
 bool acquired=false;
 explicit PublicationLock(PublicationBudget& budget) noexcept {
  do {acquired=TryAcquireSRWLockExclusive(&publications.writers)!=FALSE;}
  while(!acquired&&budget.retry());
 }
 explicit operator bool()const noexcept{return acquired;}
 ~PublicationLock(){if(acquired)ReleaseSRWLockExclusive(&publications.writers);}
};
const auto& rules(const Publication& value){return value.rules.rows();}
bool freshRule(const Publication& value,uint64_t signal,uint64_t now){return value.rules.fresh(signal,now);}
bool freshConstraint(const Publication& value,uint64_t train,uint64_t now){return value.constraints.fresh(train,now);}
const NimbyTrainConstraint* constraint(const Publication& value,uint64_t id){
 const auto& rows=value.constraints.rows();
 const auto at=std::lower_bound(rows.begin(),rows.end(),id,[](const auto& row,uint64_t key){return row.train<key;});
 return at!=rows.end()&&at->train==id?&*at:nullptr;
}
bool freshTrain(const Publication& value,const Train& train,double head,uint64_t now){
 const auto expired=[&](uint64_t signal){return value.rules.contains(signal)&&!value.rules.fresh(signal,now);};
 for(const auto& row:train.ahead)if(indicationVisible(row.position,head)&&expired(row.signal))return false;
 for(const auto& row:train.memory.stops)if(expired(row.source))return false;
 for(const auto& row:train.memory.held)if(expired(row.source))return false;
 if(train.memory.sight&&expired(train.memory.sight->source))return false;
 for(const auto& row:train.boundary)if(expired(row.source.signal))return false;
 return true;
}
uint32_t trainOptions(const Publication& value,const Train* train,uint64_t now){
 uint32_t options=value.rules.soleOptions(now);
 if(train)for(const auto& row:train->ahead)options|=value.rules.options(row.signal,now);
 return options;
}
void synchronizeTrain(RuntimeTrain& train,const Publication& value,uint64_t id,double head,bool acceptConstraint=true){
 pruneReleased(train,value);
 // Permission callbacks can observe a temporary post-integrator head that the
 // caller later restores. They never use ControlledMotion; accept a new command
 // only at the next integrator's pre-movement head, not at this projected one.
 if(!acceptConstraint)return;
 const auto serial=lifetime(value.constraintLifetimes,id);
 if(train.constraintLifetime!=serial){train.controlled={};train.constraintLifetime=serial;}
 if(freshConstraint(value,id,GetTickCount64())){
  if(const auto command=constraint(value,id))train.controlled.accept(*command,head);
  else train.controlled={};
 }else train.controlled={};
}
// The worker's occupancy map is scoped to this native step, not the integrator's
// Network object. Never keep a worker context pointer between callbacks/threads.
// Qualified caller RVA 0x44b367 passes four pointers, extra mass and tick budget
// to 0x448710; that routine calls both scan and integrate. Its context +8 is
// the integration Network, +0x18 the simulation and +0x68 the occupation map.
// Discovery retains only facts already emitted by THIS synchronous scan. It
// never allocates a controller for a vanilla train or reconstructs a branch
// from Path membership. All storage/readback is fixed and shares its step's
// existing quota. Overflow leaves discovery unknown until a later full scan.
struct ScanDiscovery {
 static constexpr size_t maximumRanges=64,maximumTracks=32,maximumChildren=1024;
 struct Topology {uintptr_t address{};double metric{};std::array<unsigned char,0x18> identity{};
  std::array<unsigned char,0x30> attachment{};size_t firstChild{},children{};};
 struct Boundary {uintptr_t address{};uint64_t id{},track{};double fraction{};int32_t kind{};int8_t direction{};};
 struct Range {ProjectionRange native;double offset{};size_t topology{};Boundary source;};
 std::array<Range,maximumRanges> ranges;
 std::array<Topology,maximumTracks> topology;
 std::array<uint64_t,maximumChildren> children;
 CommitPathStamp path;
 CommitPresence start;
 uintptr_t context{},worker{},offsetReference{},motion{},session{};
 const TrainWorld* world{};uint64_t worldRevision{},train{},captured{};
 size_t count{},tracks{},childCount{};double covered{};bool valid=false,pathCaptured=false;
 void reset()noexcept{valid=false;pathCaptured=false;count=tracks=childCount=0;}
 static bool address(uintptr_t value,size_t bytes)noexcept{
  return value>=0x10000&&value%8==0&&value<0x7fffffff0000ULL&&bytes<=0x7fffffff0000ULL-value;
 }
 bool boundary(uintptr_t at,Boundary& out){
  out.address=at;
  return address(at,0x58)&&routeRead(at,out.id)&&out.id>>48==8&&routeRead(at+0x30,out.kind)&&
   routeRead(at+0x40,out.track)&&routeRead(at+0x48,out.fraction)&&routeRead(at+0x50,out.direction);
 }
 void begin(const Publication& publication,uintptr_t c,uintptr_t w,uintptr_t off,uintptr_t m,uint64_t id,double head){
  valid=false;count=tracks=childCount=0;covered=0;
  context=c;worker=w;offsetReference=off;motion=m;session=publication.session;
  world=publication.world.get();worldRevision=publication.worldRevision;train=id;captured=GetTickCount64();
  uint8_t driving{};
  if(!routeRead(motion+0x3a0,start)||!validCommitPresence(start)||start.head!=head||
     !routeRead(motion+0x4b0,driving)||driving!=1)return;
  // Capture the bounded complete Path once per native step, not per range or
  // repeated scan. A later in-step replan fails matches rather than refreshing
  // that stamp and laundering previously observed ranges into a new route.
  if(!pathCaptured){if(!path.capture(motion,routeReadBytes))return;pathCaptured=true;}
  valid=routeWorkAvailable();
 }
 void append(uintptr_t at,double offset){
  if(!valid)return;
  if(count==ranges.size()||!std::isfinite(offset)||std::abs(offset-covered)>.01){valid=false;return;}
  auto& row=ranges[count];row={};row.offset=offset;
  if(!routeRead(at,row.native)||!address(row.native.track,0x420)||
     !std::isfinite(row.native.from)||!std::isfinite(row.native.to)||row.native.from<0||row.native.from>1||
     row.native.to<0||row.native.to>1||!std::isfinite(row.native.length)||row.native.length<0){valid=false;return;}
  size_t index=0;for(;index<tracks&&topology[index].address!=row.native.track;++index){}
  if(index==tracks){
   if(tracks==topology.size()){valid=false;return;}
   auto& stamp=topology[index];stamp={};stamp.address=row.native.track;stamp.firstChild=childCount;
   if(!routeReadBytes(stamp.address,stamp.identity.data(),stamp.identity.size())||
      CommitPathStamp::field<uint64_t>(stamp.identity,0)>>48!=1||!routeRead(stamp.address+0x88,stamp.metric)||
      !std::isfinite(stamp.metric)||stamp.metric<=0||
      !routeReadBytes(stamp.address+0x3f0,stamp.attachment.data(),stamp.attachment.size())){valid=false;return;}
   const auto first=CommitPathStamp::field<uintptr_t>(stamp.attachment,0x18),
    last=CommitPathStamp::field<uintptr_t>(stamp.attachment,0x20),cap=CommitPathStamp::field<uintptr_t>(stamp.attachment,0x28);
   if(last<first||cap<last||(last-first)%8||(cap-first)%8||cap-first>4096*8||
      (cap&&!address(first,cap-first))||(last-first)/8>children.size()-childCount){valid=false;return;}
   stamp.children=(last-first)/8;
   if(stamp.children&&!routeReadBytes(first,children.data()+childCount,stamp.children*8)){valid=false;return;}
   childCount+=stamp.children;++tracks;
  }
  row.topology=index;
  if(std::abs(row.native.length-std::abs(row.native.to-row.native.from)*topology[index].metric)>.01){valid=false;return;}
  if(row.native.kind==6&&(!row.native.signal||!boundary(row.native.signal,row.source))){valid=false;return;}
  covered=offset+row.native.length;++count;
  if(!std::isfinite(covered)||!routeWorkAvailable())valid=false;
 }
 bool matches(const Publication& publication,uintptr_t c,uintptr_t w,uintptr_t off,uintptr_t m,uint64_t id,double head){
  CommitPresence current{};uint64_t identity{};uint8_t driving{};
  if(!valid||!count||c!=context||w!=worker||off!=offsetReference||m!=motion||id!=train||head!=start.head||
     publication.session!=session||publication.world.get()!=world||publication.worldRevision!=worldRevision||
     !routeRead(motion,identity)||identity!=train||!routeRead(motion+0x3a0,current)||!sameCommitPresence(current,start)||
     !routeRead(motion+0x4b0,driving)||driving!=1||!path.matches(motion,routeReadBytes))return false;
  std::array<uint64_t,maximumChildren> confirmedChildren;
  for(size_t i=0;i<tracks;++i){const auto& stamp=topology[i];Topology now;double metric{};
   if(!routeReadBytes(stamp.address,now.identity.data(),now.identity.size())||now.identity!=stamp.identity||
      !routeReadBytes(stamp.address+0x3f0,now.attachment.data(),now.attachment.size())||now.attachment!=stamp.attachment||
      !routeRead(stamp.address+0x88,metric)||metric!=stamp.metric)return false;
   const auto first=CommitPathStamp::field<uintptr_t>(stamp.attachment,0x18);
   if(stamp.children&&(!routeReadBytes(first,confirmedChildren.data(),stamp.children*8)||
      !std::equal(confirmedChildren.begin(),confirmedChildren.begin()+stamp.children,children.begin()+stamp.firstChild)))return false;
  }
  for(size_t i=0;i<count;++i){const auto& old=ranges[i].source;if(!old.address)continue;Boundary now;
   if(!boundary(old.address,now)||now.id!=old.id||now.kind!=old.kind||now.track!=old.track||
      now.fraction!=old.fraction||now.direction!=old.direction)return false;
  }
  return routeWorkAvailable();
 }
 bool replay(RuntimeTrain& target,const Publication& publication,uintptr_t c,uintptr_t w,uintptr_t off,
  uintptr_t m,uint64_t id,double head,double offset){
  RouteWorkPhase phase;
  const bool ready=std::isfinite(offset)&&std::abs(offset-covered)<=.01&&matches(publication,c,w,off,m,id,head);
  if(ready){
   target.signalRoute.begin(m,id,head,captured,routeReadBytes,selectedRouteInterested(GetTickCount64()));
   for(size_t i=0;i<count;++i){const auto& row=ranges[i];
    target.signalRoute.append(row.native.track,row.native.from,row.native.to,row.native.length,row.offset,routeReadBytes);
    if(row.source.address&&row.source.kind==NIMBY_SIGNAL_PATH){
     target.signalRoute.boundary(row.source.address,row.source.id,row.offset+row.native.length,routeReadBytes);
     target.ahead.push_back({row.source.id,head+row.offset+row.native.length});
    }else if(row.source.address&&target.signalRoute.observesObjects()){
     target.signalRoute.encounteredObject(row.source.address,row.source.id,row.offset+row.native.length,routeReadBytes);
    }
   }
  }
  const bool qualified=ready&&matches(publication,c,w,off,m,id,head);
  valid=false;return qualified;
 }
};
static_assert(std::is_trivially_copyable_v<ScanDiscovery>);
thread_local std::array<ScanDiscovery,2> scanDiscoveryStorage;
thread_local size_t scanDiscoveryDepth=0;
struct StepContext {uintptr_t context=0,motion=0;ScanDiscovery* discovery=nullptr;};
thread_local StepContext currentStep;
struct StepScope {
 StepContext previous;
 StepScope(uintptr_t context,uintptr_t motion):previous(currentStep){
  auto* discovery=scanDiscoveryDepth<scanDiscoveryStorage.size()?&scanDiscoveryStorage[scanDiscoveryDepth]:nullptr;
  ++scanDiscoveryDepth;if(discovery)discovery->reset();currentStep={context,motion,discovery};
 }
 ~StepScope(){--scanDiscoveryDepth;currentStep=previous;}
};
struct PendingIntegration {
 CommitPresence before{},after{};CommitPathStamp path;
 std::array<nimby::windows::automatic::SignalRoute::CommitSourceStamp,128> sources{};
 size_t sourceCount=0;
 std::vector<Passage> encountered;
 uint64_t routeRevision=0,signalRouteRevision=0;
 double speed=0,stopBeforeSignalMeters=0;int64_t elapsed=0,budget=0;
 bool active=false,preEntry=false;
};
struct PendingIntegrationFrame;
thread_local PendingIntegrationFrame* currentCommitFrame=nullptr;
struct PendingIntegrationFrame {
 PendingIntegrationFrame* previous=currentCommitFrame;
 uintptr_t context{},trainAddress{},motion{},networkAddress{};uint64_t train=0;
 // Pin the immutable SDK image and its train guard through the native cleanup.
 // No result pointer or game-owned transient output enters this payload.
 std::optional<Publications::View> pin;
 std::optional<TrainAccess> owner;
 PendingIntegration pending;
 unsigned integrationDepth=0;
 bool ambiguous=false;
 PendingIntegrationFrame(uintptr_t c,uintptr_t t,uintptr_t m):context(c),trainAddress(t),motion(m){
  for(auto* ancestor=previous;ancestor;ancestor=ancestor->previous)
   if(ancestor->motion==motion){ancestor->ambiguous=true;ambiguous=true;}
  currentCommitFrame=this;
 }
 ~PendingIntegrationFrame(){currentCommitFrame=previous;}
 RuntimeTrain* state()const noexcept{return owner?owner->state:nullptr;}
 void reject(CommitReason reason,bool uncertain=true)noexcept {
  commitOutcomes[static_cast<size_t>(reason)].fetch_add(1,std::memory_order_relaxed);
  if(train)commitSampleTrain[static_cast<size_t>(reason)].store(train,std::memory_order_relaxed);
  if(uncertain&&state()&&!state()->integrationUnqualified){state()->integrationUnqualified=true;commitLatched.fetch_add(1,std::memory_order_relaxed);}
  pending.active=false;pending.encountered.clear();pending.sourceCount=0;
 }
 bool bind(const Publication& publication,uint64_t id,uintptr_t network){
  constexpr uintptr_t limit=0x7fffffff0000ULL;
  if(motion<0x10000||motion>limit-0x638||context<0x10000||context>limit-0x70||
     trainAddress<0x10000||trainAddress>limit-sizeof(uint64_t)){reject(CommitReason::IdentityUnqualified);return false;}
  if(pin&&(!state()||train!=id||networkAddress!=network||pin->operator->()->world!=publication.world||
     pin->operator->()->session!=publication.session)){reject(CommitReason::PublicationChanged);return false;}
  train=id;uint64_t dbTrain{};uintptr_t nativeNetwork{},nativeSession{};
  RouteWorkPhase phase;
  if(!routeRead(trainAddress,dbTrain)||dbTrain!=id||id>>48!=5){reject(CommitReason::IdentityUnqualified);return false;}
  if(context>UINTPTR_MAX-0x68||!routeRead(context+8,nativeNetwork)||nativeNetwork!=network||
     !routeRead(context+0x18,nativeSession)||nativeSession!=publication.session){reject(CommitReason::ContextUnqualified);return false;}
  if(pin)return true;
  pin.emplace(publications);
  if(pin->operator->()->world!=publication.world||pin->operator->()->session!=publication.session){reject(CommitReason::PublicationChanged);return false;}
  train=id;networkAddress=network;owner.emplace(**pin,id,motion);
  if(!state()){reject(CommitReason::OwnerUnavailable);return false;}
  return true;
 }
};
void finishPendingIntegration(PendingIntegrationFrame& frame,bool finalCleanup){
 auto& pending=frame.pending;if(!pending.active)return;
 auto* state=frame.state();
 if(!state){frame.reject(CommitReason::OwnerUnavailable);return;}
 if(frame.ambiguous||frame.integrationDepth){frame.reject(CommitReason::Reentrant);return;}
 Publications::View latest(publications);RouteWorkPhase phase;
 if(!frame.pin||latest->world!=frame.pin->operator->()->world||latest->session!=frame.pin->operator->()->session||
    latest->session!=currentSession()){frame.reject(CommitReason::PublicationChanged);return;}
 uint64_t id{},dbTrain{};CommitPresence conserved{};
 if(!routeRead(frame.motion,id)||id!=frame.train||!routeRead(frame.trainAddress,dbTrain)||dbTrain!=id){frame.reject(CommitReason::IdentityUnqualified);return;}
 if(!routeRead(frame.motion+0x3a0,conserved)||!validCommitPresence(conserved)){frame.reject(CommitReason::PresenceUnqualified);return;}
 if(finalCleanup){
  struct Mirror {uint64_t track;double fraction;int8_t direction,orientation;std::array<unsigned char,6> reserved;};
  static_assert(sizeof(Mirror)==0x18);Mirror mirror{};
  if(!routeRead(frame.motion+0xb8,mirror)||mirror.track!=conserved.track||mirror.fraction!=conserved.fraction||
     mirror.direction!=conserved.direction||mirror.orientation!=conserved.orientation){frame.reject(CommitReason::MirrorUnqualified);return;}
 }
 // Known rollback is a discarded attempt, never a backward-route reset or an
 // instruction-memory clear. Earlier commits and newly published commands stay.
 if(!sameCommitPresence(pending.before,pending.after)&&sameCommitPresence(conserved,pending.before)){
  frame.reject(CommitReason::RolledBack,false);return;
 }
 if(!sameCommitPresence(conserved,pending.after)){frame.reject(CommitReason::PresenceUnqualified);return;}
 if(state->routeRevision!=pending.routeRevision||state->signalRoute.revision()!=pending.signalRouteRevision){frame.reject(CommitReason::RouteChanged);return;}
 if(!pending.path.matches(frame.motion,routeReadBytes)){frame.reject(CommitReason::PathUnqualified);return;}
 if(pending.sourceCount&&!state->signalRoute.matchesCommitSources(
    std::span(pending.sources.data(),pending.sourceCount),routeReadBytes)){frame.reject(CommitReason::SourcesUnqualified);return;}
 for(const auto& passage:pending.encountered){
  const auto* instruction=rule(rules(*latest),passage.source.signal);
  if(!passage.publisher||lifetime(latest->signalLifetimes,passage.source.signal)!=passage.publisher||!instruction||
     std::memcmp(instruction,&passage.instruction,sizeof *instruction)||!freshRule(*latest,passage.source.signal,GetTickCount64())){
   frame.reject(CommitReason::SourcesUnqualified);return;
  }
 }
 if(!routeWorkAvailable()){frame.reject(CommitReason::ResultUnqualified);return;}
 if(state->integrationUnqualified){frame.reject(CommitReason::AlreadyLatched,false);return;}
 pruneReleased(*state,*latest);
 commitIntegration(*state,pending.encountered,rules(*latest),pending.before.head,conserved.head,
     pending.speed,pending.elapsed,pending.budget,true,freshTrain(*latest,*state,conserved.head,GetTickCount64()),pending.stopBeforeSignalMeters);
 if(pending.preEntry&&state->memory.sight&&state->memory.sight->source==state->entrySignal)
  state->physicalRoute=std::move(state->entryPhysicalRoute);
 rememberSources(*state,*latest);
 if(telemetry)InterlockedIncrement64(&telemetry->committedIntegrations);
 frame.reject(CommitReason::Conserved,false);
}
template<class Call> uint32_t transactionalNativeStep(uintptr_t context,uintptr_t train,uintptr_t motion,Call&& call){
 PendingIntegrationFrame frame(context,train,motion);
 const auto returned=call();
 finishPendingIntegration(frame,true);
 return returned;
}
void drainCommitOutcomes()noexcept {
 if(!transactionalCommitEnabled)return;
 static std::atomic_flag consumer=ATOMIC_FLAG_INIT;
 if(consumer.test_and_set(std::memory_order_acquire))return;
 struct Release {~Release(){consumer.clear(std::memory_order_release);}} release;
 try {
  constexpr auto count=static_cast<size_t>(CommitReason::Count);
  static std::array<uint64_t,count> previous{};static uint64_t report=0,heartbeat=0;
  std::array<uint64_t,count> observed{};bool changed=false,any=false;
  for(size_t i=0;i<count;++i){observed[i]=commitOutcomes[i].load(std::memory_order_relaxed);
   changed=changed||observed[i]!=previous[i];any=any||observed[i]!=0;}
  const auto now=GetTickCount64();if(!any||now<report||(!changed&&now<heartbeat))return;
  std::ostringstream message;
  message<<"Driving transaction prototype: scope=local_native_step_448710 opt_in=1 central_entry_proof=0 rear_clear_proof=0"
         <<" path_max_tracks="<<CommitPathStamp::maximumTracks<<" source_max=128 shared_read_max=8192 shared_byte_max=4194304 shared_time_ms=2"
         <<" latched="<<commitLatched.load(std::memory_order_relaxed)
         <<" whole_step_stopped="<<stoppedStepBudgets.load(std::memory_order_relaxed)
         <<" stop_scope_unknown="<<unqualifiedStepBudgets.load(std::memory_order_relaxed)<<" outcomes={";
  for(size_t i=0;i<count;++i)if(observed[i])message<<nimby::windows::automatic::commitReasonName(static_cast<CommitReason>(i))<<':'<<observed[i]<<' ';
  message<<'}';size_t samples=0;
  for(size_t i=static_cast<size_t>(CommitReason::OutsideNativeStep);i<count&&samples<4;++i){
   const auto train=commitSampleTrain[i].load(std::memory_order_relaxed);
   if(observed[i]&&train){message<<" sample={train="<<train<<" reason="<<nimby::windows::automatic::commitReasonName(static_cast<CommitReason>(i))<<'}';++samples;}
  }
  nimby::detail::diagnostics::write("sdk",commitLatched.load(std::memory_order_relaxed)?"WARN":"INFO",message.str().c_str());
  previous=observed;report=now+1000;heartbeat=now+30000;
 }catch(...){/* Diagnostics never affect native movement or the latch. */}
}
struct PassageReadAllowance {
 size_t reads=64,bytes=32768;int64_t spent=0,last=0,frequency=0;bool valid=true;
 PassageReadAllowance()noexcept {LARGE_INTEGER value{};if(!QueryPerformanceFrequency(&value)||value.QuadPart<=0)valid=false;else frequency=value.QuadPart;}
 bool copy(uintptr_t address,void* output,size_t size)noexcept {
  if(!valid||!reads||size>bytes){valid=false;return false;}
  LARGE_INTEGER before{},after{};
  if(!QueryPerformanceCounter(&before)||before.QuadPart<last){valid=false;return false;}
  --reads;bytes-=size;const bool result=readBytes(address,output,size);
  if(!QueryPerformanceCounter(&after)||after.QuadPart<before.QuadPart){valid=false;return false;}
  last=after.QuadPart;const auto cost=after.QuadPart-before.QuadPart;
  if(cost>INT64_MAX-spent){valid=false;return false;}
  spent+=cost;if(spent>frequency/500)valid=false;
  // Native execution between callbacks is excluded. The 2 ms read allowance
  // is cooperative: one read or OS descheduling can exceed it before return.
  return result&&valid;
 }
};
struct PassageCapture {
 uintptr_t context{},train{},motion{};
 PassageReadAllowance reader;
 nimby::windows::automatic::NativePassageTransaction transaction;
 PassageCapture(uintptr_t c,uintptr_t t,uintptr_t m)noexcept:context(c),train(t),motion(m),
  transaction(t,m,[&](uintptr_t a,void* o,size_t n){return reader.copy(a,o,n);}){}
};
thread_local PassageCapture* activePassageCapture=nullptr;
void __fastcall passBy(uintptr_t scripts,uintptr_t scriptContext,uintptr_t train,uintptr_t motion,uintptr_t signal,uintptr_t service){
 auto* capture=activePassageCapture;
 // The two qualified callsites pass Extrapolator+0xA8 as scriptContext.
 // Nested steps on this thread install their own scope; unrelated workers
 // and dispatcher calls outside a step must not inherit a candidate.
 if(capture&&capture->context<=UINTPTR_MAX-0xa8&&scriptContext==capture->context+0xa8&&
    train==capture->train&&motion==capture->motion)
  capture->transaction.candidate(signal,[&](uintptr_t a,void* o,size_t n){return capture->reader.copy(a,o,n);});
 nativePassBy(scripts,scriptContext,train,motion,signal,service); // Original exactly once, unmodified arguments.
}
template<class Call> uint32_t observeNativeStep(uintptr_t context,uintptr_t train,uintptr_t motion,Call&& call){
 PassageCapture capture(context,train,motion);
 struct CaptureScope {PassageCapture* previous;CaptureScope(PassageCapture& value):previous(activePassageCapture){activePassageCapture=&value;}
  ~CaptureScope(){activePassageCapture=previous;}};
 uint32_t returned;
 {CaptureScope scope(capture);returned=call();}
 // Capture completed state before leaving the original worker stack. The
 // consumer only receives copied numeric records, never native addresses.
 std::array<nimby::engine::interlocking::NativePassageRow,8> rows{};size_t count=0;
 const auto sequence=passageSequence.fetch_add(1,std::memory_order_relaxed)+1;
 capture.transaction.finish([&](uintptr_t a,void* o,size_t n){return capture.reader.copy(a,o,n);},
  [&](const auto& observation,const auto& assessment,double beforeFraction){
   const auto& source=observation.source;
   if(observation.trainBefore>>48!=5||source.signal>>48!=8||source.track>>48!=1||!sequence){
    passageDroppedIdentity.fetch_add(1,std::memory_order_relaxed);return;
   }
   auto& row=rows[count++];row.train=observation.trainBefore;row.signal=source.signal;row.track=source.track;
   row.threadStepSequence=sequence;row.outcome=static_cast<uint32_t>(assessment.status);row.reason=static_cast<uint32_t>(assessment.reason);
   row.beforeHead=observation.beforeHead;row.boundaryHead=observation.candidate.head;row.afterHead=observation.finalPresence.head;
   row.beforeFraction=beforeFraction;row.finalFraction=observation.finalPresence.fraction;
   if(observation.readComplete&&std::isfinite(row.beforeHead)&&std::isfinite(row.boundaryHead)&&std::isfinite(row.afterHead)&&
      std::isfinite(row.beforeFraction)&&row.beforeFraction>=0&&row.beforeFraction<=1&&
      std::isfinite(row.finalFraction)&&row.finalFraction>=0&&row.finalFraction<=1)
    row.flags=nimby::engine::interlocking::NativePassageRow::NumericComplete;
  });
 passageSteps.fetch_add(1,std::memory_order_relaxed);
 if(!capture.transaction.count())passageNoCandidates.fetch_add(1,std::memory_order_relaxed);
 if(count)passageMailbox.tryPublish(std::span(rows.data(),count));
 return returned;
}
// Run only on an SDK route-copy caller, after native locks/pins are released.
// No observer formatting, disk write, or callback ever occurs on train workers.
void drainNativePassages()noexcept {
 if(!passageObservationEnabled)return;
 static std::atomic_flag consumer=ATOMIC_FLAG_INIT;
 if(consumer.test_and_set(std::memory_order_acquire))return;
 struct Release {~Release(){consumer.clear(std::memory_order_release);}} release;
 try {
  using namespace nimby::engine::native_passage;
  using namespace nimby::engine::interlocking;
  static uint64_t nextReport=0,nextHeartbeat=0,processed=0,lastProcessed=0,lastLost=0,lastDropped=0;
  static std::array<uint64_t,4> outcomes{};
  static std::array<uint64_t,static_cast<size_t>(Reason::Count)> reasons{};
  static std::array<NativePassageRow,4> samples{};static size_t sampleCount=0;
  NativePassageBatch batch;
  for(size_t i=0;i<NativePassageMailbox::slotCapacity;++i){
   if(passageMailbox.tryConsume(batch)!=NativePassageConsume::Ready)break;
   for(size_t n=0;n<batch.count;++n){const auto& row=batch.rows[n];++processed;
    if(row.outcome<outcomes.size())++outcomes[row.outcome];
    if(row.reason<reasons.size())++reasons[row.reason];
    if(sampleCount<samples.size())samples[sampleCount++]=row;
   }
  }
  const auto stats=passageMailbox.stats();const auto now=GetTickCount64();
  const auto dropped=passageDroppedIdentity.load(std::memory_order_relaxed);
  const auto steps=passageSteps.load(std::memory_order_relaxed);
  const bool changed=processed!=lastProcessed||stats.lost!=lastLost||dropped!=lastDropped;
  // A running observer with no candidates still needs a heartbeat: silence
  // must not be mistaken for verified coverage or an inactive adapter.
  if(now<nextReport||(!changed&&now<nextHeartbeat)||(!processed&&!stats.lost&&!dropped&&!steps))return;
  nextReport=now+1000;nextHeartbeat=now+30000;
  std::ostringstream message;
  message<<"Native signal passage observation: scope=native_step_448710 diagnostic_only=1 runtime_qualified=0 central_entry_proof=0 rear_clear_proof=0 native_permissions_changed=0"
         <<" steps="<<steps<<" without_candidates="<<passageNoCandidates.load(std::memory_order_relaxed)
         <<" processed="<<processed<<" accepted_batches="<<stats.accepted<<" accepted_rows="<<stats.acceptedRows
         <<" busy="<<stats.busy<<" full="<<stats.full<<" invalid="<<stats.invalid<<" lost="<<stats.lost
         <<" dropped_identity="<<passageDroppedIdentity.load(std::memory_order_relaxed)<<" loss_sticky="<<passageMailbox.lossObserved()<<" outcomes={";
  for(size_t i=0;i<outcomes.size();++i)if(outcomes[i])message<<statusName(static_cast<Status>(i))<<':'<<outcomes[i]<<' ';
  message<<"} reasons={";
  for(size_t i=0;i<reasons.size();++i)if(reasons[i])message<<reasonName(static_cast<Reason>(i))<<':'<<reasons[i]<<' ';
  message<<'}';
  for(size_t i=0;i<sampleCount;++i){const auto& row=samples[i];
   message<<" sample={train="<<row.train<<" signal="<<row.signal<<" track="<<row.track<<" observer_sequence="<<row.threadStepSequence
          <<" outcome="<<statusName(static_cast<Status>(row.outcome))<<" reason="<<reasonName(static_cast<Reason>(row.reason))
          <<" numeric_complete="<<bool(row.flags&NativePassageRow::NumericComplete);
   if(row.flags&NativePassageRow::NumericComplete)
    message<<" head_before="<<row.beforeHead<<" source_head="<<row.boundaryHead<<" head_after="<<row.afterHead
           <<" fraction_before="<<row.beforeFraction<<" fraction_after="<<row.finalFraction;
   message<<'}';
  }
  nimby::detail::diagnostics::write("sdk",stats.lost!=lastLost?"WARN":"INFO",message.str().c_str());
  lastLost=stats.lost;lastProcessed=processed;lastDropped=dropped;sampleCount=0;
 }catch(...){/* No diagnostic exception may alter a route result. */}
}
bool consumeStoppedStepBudget(const Publication& publication,uintptr_t caller,uintptr_t context,
 uintptr_t train,uintptr_t motion,uint64_t id,uintptr_t budget)noexcept {
 // A zero projection can still return a Kind3/Kind6 event at the head. Stop
 // the entire native movement loop instead, including source-less events.
 // 44B367 is its unique qualified direct caller. Arg6 is that caller's signed
 // local budget; the caller exits when the ORIGINAL budget becomes <=0.
 // Passing a private zero while leaving that local positive would retry forever.
 uint64_t motionId{},trainId{};uintptr_t simulation{};int64_t remaining{};
 const auto error=GetLastError();
 const bool qualified=caller==base+0x44b36c&&id>>48==5&&publication.session==currentSession()&&
   read(motion,motionId)&&motionId==id&&read(train,trainId)&&trainId==id&&
   context<=UINTPTR_MAX-0x18&&read(context+0x18,simulation)&&simulation==publication.session&&
   budget>=0x10000&&budget<=0x7fffffff0000ULL-sizeof remaining&&read(budget,remaining);
 bool consumed=false;
 if(qualified){
  const int64_t zero=0;SIZE_T copied{};
  consumed=WriteProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(budget),&zero,sizeof zero,&copied)&&copied==sizeof zero;
 }
 SetLastError(error);
 if(consumed)++stoppedStepBudgets;else ++unqualifiedStepBudgets;
 return consumed;
}
uint32_t __fastcall stepMotion(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget){
 const auto caller=NRF_PHASE_CALLER();
 StepScope scope(context,motion);
 RouteBudgetScope routeScope(motion,true);
 Publications::View view(publications);uint64_t id{};
 const auto invoke=[&]{const auto call=[&]{return originalRouteCall([&]{return nativeStep(context,train,motion,service,mass,budget);});};
  const auto observed=[&]{if(!passageObservationEnabled)return call();return observeNativeStep(context,train,motion,call);};
  if(!transactionalCommitEnabled)return observed();
  return transactionalNativeStep(context,train,motion,observed);
 };
 if(view->active()&&view->session==currentSession()&&read(motion,id)&&id>>48==5){
  if(view->world&&(view->world->isManaged(id)||constraint(*view,id))){
   TrainAccess access(*view,id,motion);
   if(transactionalCommitEnabled&&(!access.state||access.state->integrationUnqualified))
    consumeStoppedStepBudget(*view,caller,context,train,motion,id,budget);
   // The original still runs once: its waiting, stats and pre-budget replan
   // semantics are preserved. This is a movement latch, not replacement of
   // native reservations or a physical-entry/rear-clearance certificate.
   return invoke();
  }
 }
 return invoke();
}
// Permission traversal is synchronous on the calling simulation thread. This
// scope cannot leak to another train, a UI query, or a subsequent callback.
struct PermissionQuery {
 uint64_t signal{};
 bool started=false,valid=true,allowOccupation=false;
 double covered=0,free=200;
 uintptr_t occupationContext{},reservationContext{},track{},motion{};
 bool occupationPending=false,reservationPending=false,reservationReplaced=false;
 double from=0,to=0,length=0;
 double head=0,prefixLength=0,prefixFree=200;
 bool prefixObserved=true;
 unsigned ranges=0;
 nimby::windows::automatic::PhysicalRoute* route=nullptr;
 nimby::windows::automatic::CrossingIdentity crossingIdentity;
 nimby::windows::automatic::CrossingObservation crossing;
 unsigned crossingRanges=0;
};
thread_local PermissionQuery* permissionQuery=nullptr;
struct QueryScope {PermissionQuery* previous;QueryScope(PermissionQuery& q):previous(permissionQuery){permissionQuery=&q;}
 ~QueryScope(){permissionQuery=previous;}};
// Query the game's physical occupancy predicate with a private output byte.
// It excludes the current train using its motion ID, exactly as the game does.
// Neither reservation tables nor the game's permission byte are edited here.
double physicalPrefix(uintptr_t context,uintptr_t track,double from,double to,double length,bool& occupied,bool budgeted=false) {
 std::array<uintptr_t,3> nativeContext{};
 if(!(budgeted?routeRead(context,nativeContext):read(context,nativeContext))||!nativeContext[0]||!nativeContext[1])return -1;
 uint8_t clear=1;nativeContext[2]=reinterpret_cast<uintptr_t>(&clear);
 bool valid=true;
 auto isClear=[&](double fraction){clear=1;
  if(budgeted&&!routeWorkAvailable()){valid=false;return false;}
  const auto result=nativeOccupancy(reinterpret_cast<uintptr_t>(nativeContext.data()),track,from,from+(to-from)*fraction);
  if(budgeted&&!routeWorkAvailable()){valid=false;return false;}
  return result!=0&&clear;
 };
 occupied=!isClear(1);
 if(!valid)return -1;
 const auto free=occupied?length*clearPrefix(isClear):length;
 return valid?free:-1;
}
bool observeEntryPrefix(uintptr_t context,PermissionQuery& query){
 if(query.prefixObserved)return true;
 uintptr_t occupationContext{},motion{};std::array<uintptr_t,3> occupation{};
 if(!query.route||!routeRead(context+24,occupationContext)||!routeRead(occupationContext,occupation)||
    !occupation[0]||!routeRead(occupation[1],motion)||motion!=query.motion)return false;
 const auto view=query.route->refresh(query.motion,query.head,GetTickCount64(),routeReadBytes,
  [&](uintptr_t track,double from,double to,double length){bool occupied=false;
   return physicalPrefix(occupationContext,track,from,to,length,occupied,true);});
 if(!routeWorkAvailable()||!view.valid||!std::isfinite(view.covered)||view.covered+1e-6<query.prefixLength||
    !std::isfinite(view.free)||view.free<0)return false;
 query.prefixFree=std::min(200.0,view.free);query.free=query.prefixFree;
 query.covered=query.prefixLength;query.prefixObserved=true;return true;
}
// These copied facts observe only the pinned native crossed predicates. No
// reservation lookup, predicate replay, owner table or global snapshot is added.
// In particular an SDK-private longitudinal query cannot donate native clearance.
bool crossingContext(PermissionQuery& q,uintptr_t context,
 uintptr_t& occupationContext,uintptr_t& reservationContext) {
 uintptr_t sourceRef{};uint64_t source{},identity{};uintptr_t occupationMotion{},reservationMotion{};
 std::array<uintptr_t,3> occupation{},reservation{};
 return routeRead(context+8,sourceRef)&&routeRead(sourceRef,source)&&source==q.signal&&
  routeRead(context+24,occupationContext)&&routeRead(context+32,reservationContext)&&
  routeRead(occupationContext,occupation)&&occupation[0]&&occupation[2]&&
  routeRead(reservationContext,reservation)&&reservation[0]&&reservation[2]&&
  routeRead(occupation[1],occupationMotion)&&occupationMotion==q.motion&&
  routeRead(reservation[1],reservationMotion)&&reservationMotion==q.motion&&
  routeRead(q.motion,identity)&&identity==q.crossingIdentity.train&&routeWorkAvailable();
}
void observeCrossingPredicate(PermissionQuery* q,nimby::windows::automatic::CrossingPredicate predicate,
 uintptr_t caller,uintptr_t context,uintptr_t track,double from,double to,uint8_t result) {
 using namespace nimby::windows::automatic;
 const auto rva=caller>=base?caller-base:0;
 if(!q||!q->crossing.active()||!CrossingObservation::caller(predicate,rva))return;
 PreserveNativeError preserve;RouteWorkPhase phase;
 std::array<uintptr_t,3> copied{};uintptr_t motion{};uint64_t id{},trackId{};
 auto identity=q->crossingIdentity;
 identity.qualified=routeRead(context,copied)&&copied[0]&&copied[2]&&
  routeRead(copied[1],motion)&&motion==q->motion&&routeRead(motion,id)&&id==identity.train&&
  routeRead(track,trackId)&&routeWorkAvailable();
 q->crossing.predicate(predicate,rva,context,identity,trackId,from,to,result);
}
uint8_t __fastcall occupancy(uintptr_t context,uintptr_t track,double from,double to) {
 const auto caller=NRF_PHASE_CALLER();
 auto* q=permissionQuery;
 // Cross-track checks made by the native callback retain their exact refusal.
 if(q&&q->valid&&q->started&&q->allowOccupation&&q->occupationPending&&q->occupationContext==context&&
    q->track==track&&q->from==from&&q->to==to) {
  RouteWorkPhase phase;
  q->occupationPending=false;
  bool occupied=false;const double free=physicalPrefix(context,track,from,to,q->length,occupied,true);
  if(free>=0){if(occupied)q->free=std::min(q->free,q->covered+free);q->reservationPending=true;observeCrossingPredicate(q,nimby::windows::automatic::CrossingPredicate::Occupation,caller,context,track,from,to,1);return 1;}
  q->valid=false;
 }
 const auto result=originalRouteCall([&]{return nativeOccupancy(context,track,from,to);});
 observeCrossingPredicate(q,nimby::windows::automatic::CrossingPredicate::Occupation,caller,context,track,from,to,result);
 return result;
}
uint8_t __fastcall reservation(uintptr_t context,uintptr_t track,double from,double to) {
 const auto caller=NRF_PHASE_CALLER();
 auto* q=permissionQuery;
 // On-sight authority replaces exclusive block admission on the followed range.
 // Otherwise a leading train's reservation prevents departure even after the
 // required stop and a successful physical-clearance query. The native callback
 // checks this range first, then crossing tracks: consume the exception ONCE,
 // so even a repeated identical crossing query keeps its native refusal.
 if(q&&q->valid&&q->started&&q->allowOccupation&&q->reservationPending&&
    q->reservationContext==context&&q->track==track&&q->from==from&&q->to==to) {
  RouteWorkPhase phase;
  q->reservationPending=false;
  std::array<uintptr_t,3> copy{};uintptr_t motion{};
  if(routeRead(context,copy)&&copy[0]&&routeRead(copy[1],motion)&&motion==q->motion&&copy[2]) {
   // Preserve native lookup/locking and retain its decision for diagnostics.
   // Only the private result byte changes; no reservation, route, signal state,
   // train speed or pre-existing native refusal is erased.
   uint8_t clear=1;copy[2]=reinterpret_cast<uintptr_t>(&clear);
   const auto result=originalRouteCall([&]{return nativeReservation(reinterpret_cast<uintptr_t>(copy.data()),track,from,to);});
   if(!routeWorkAvailable()){q->valid=false;q->crossing.invalidate();return result;}
   q->reservationReplaced|=!result||!clear;
   observeCrossingPredicate(q,nimby::windows::automatic::CrossingPredicate::Reservation,caller,context,track,from,to,1);
   return 1;
  }
  q->valid=false;
 }
 const auto result=originalRouteCall([&]{return nativeReservation(context,track,from,to);});
 observeCrossingPredicate(q,nimby::windows::automatic::CrossingPredicate::Reservation,caller,context,track,from,to,result);
 return result;
}
uintptr_t permissionRangeImpl(uintptr_t context,uintptr_t range) {
 auto* q=permissionQuery;
 if(!q||!q->allowOccupation)return originalRouteCall([&]{return nativePermissionRange(context,range);});
 RouteWorkPhase phase;
 uintptr_t signalPtr{},track{},occupationContext{},reservationContext{};uint64_t signal{};
 int kind{};double from{},to{},length{},metric{};
 if(++q->ranges>4096||!routeRead(range+0x20,kind)||!routeRead(range+0x28,signalPtr))q->valid=false;
 // The source marker itself performs no longitudinal occupation query. Before
 // it can reset downstream offsets, observe the qualified head->source prefix
 // with this callback's still-live context. Never retain that context pointer.
 if(q->valid&&!q->prefixObserved&&!observeEntryPrefix(context,*q))q->valid=false;
 if(q->valid&&kind==6&&signalPtr&&routeRead(signalPtr,signal)&&signal==q->signal){
  q->started=true;q->covered=q->prefixLength;q->free=q->prefixFree;
  return originalRouteCall([&]{return nativePermissionRange(context,range);});
 }
 if(q->valid&&q->started) {
  if(!routeRead(range,track)||!routeRead(range+8,from)||!routeRead(range+16,to)||!routeRead(range+24,length)||
     !routeRead(track+0x88,metric)||!routeRead(context+24,occupationContext)||!routeRead(context+32,reservationContext)||
     !std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1||
     !std::isfinite(length)||length<0||!std::isfinite(metric)||metric<=0||
     std::abs(length-std::abs(to-from)*metric)>.01)q->valid=false;
  else {
   if(q->route)q->route->append(track,from,to,length,q->covered,routeReadBytes);
   q->track=track;q->from=from;q->to=to;q->length=length;q->occupationContext=occupationContext;
   q->reservationContext=reservationContext;q->occupationPending=true;q->reservationPending=false;
   const auto result=originalRouteCall([&]{return nativePermissionRange(context,range);});
   // A range only contributes observed clearance if the expected physical and
   // reservation predicates actually ran. An early native exit proves nothing.
   if(q->occupationPending||q->reservationPending||!routeWorkAvailable())q->valid=false;
   q->covered+=length;q->occupationContext=q->reservationContext=0;
   q->occupationPending=q->reservationPending=false;
   return result;
  }
 }
 return originalRouteCall([&]{return nativePermissionRange(context,range);});
}
uintptr_t __fastcall permissionRange(uintptr_t context,uintptr_t range) {
 using namespace nimby::windows::automatic;
 const auto incoming=GetLastError();auto* q=permissionQuery;
 bool active=false;
 if(q&&q->crossingIdentity.qualified){
  RouteWorkPhase phase;int kind{};uintptr_t signalPointer{},occupationContext{},reservationContext{};
  uint64_t signal{};
  if(++q->crossingRanges>4096||!routeRead(range+0x20,kind)||!routeRead(range+0x28,signalPointer))q->crossing.invalidate();
  else if(kind==6&&signalPointer&&routeRead(signalPointer,signal)&&signal==q->signal){
   auto identity=q->crossingIdentity;
   identity.qualified=crossingContext(*q,context,occupationContext,reservationContext);
   q->crossing.source(identity);
  }else if(q->crossing.sourceSeen()){
   CrossingRange copied;uintptr_t track{};
   copied.context=context;
   copied.qualified=crossingContext(*q,context,copied.occupationContext,copied.reservationContext)&&
    routeRead(range,track)&&routeRead(track,copied.ownTrack)&&
    routeRead(range+8,copied.from)&&routeRead(range+16,copied.to)&&routeWorkAvailable();
   active=q->crossing.beginRange(copied);
  }
 }
 SetLastError(incoming);
 const auto result=permissionRangeImpl(context,range);
 {PreserveNativeError preserve;if(active)q->crossing.endRange(static_cast<uint8_t>(result));}
 return result;
}
uintptr_t __fastcall scan(uintptr_t context,uintptr_t range){
 std::optional<RouteBudgetScope> routeScope;
 Publications::View snapshot(publications);const auto& publication=*snapshot;
 if(publication.active()){
  uintptr_t motionRef{},motion{},nativeContext{},simulation{},offsetRef{},signalPtr{};
  uint64_t id{},signal{};int kind{};double offset{},part{},head{};
  if(read(context+0x18,motionRef)&&read(motionRef,motion)&&read(motion,id)&&id>>48==5&&
     read(context+0x10,nativeContext)&&read(nativeContext+0x18,simulation)&&
     read(context,offsetRef)&&read(offsetRef,offset)&&read(range+0x18,part)&&read(range+0x20,kind)&&
     read(motion+0x3c0,head)&&std::isfinite(offset)&&offset>=0&&std::isfinite(part)&&part>=0&&std::isfinite(head)){
   const bool managedSource=kind==6&&read(range+0x28,signalPtr)&&read(signalPtr,signal)&&publication.rules.contains(signal);
   const bool previouslyManaged=publication.world&&publication.world->isManaged(id);
   if(publication.world&&(managedSource||constraint(publication,id)))publication.world->markManaged(id);
   if(!publication.world||!publication.world->isManaged(id)){
    // Only a synchronous native step can retain a prefix. Foreign lookahead,
    // another context/train and TLS overflow never create a synthetic route.
    auto* discovery=currentStep.discovery;
    if(discovery&&currentStep.context==nativeContext&&currentStep.motion==motion&&
       simulation==publication.session&&simulation==currentSession()){
     routeScope.emplace(motion);RouteWorkPhase phase;
     if(offset==0)discovery->begin(publication,context,nativeContext,offsetRef,motion,id,head);
     discovery->append(range,offset);
    }
    return originalRouteCall([&]{return nativeScan(context,range);});
   }
   routeScope.emplace(motion);
   TrainAccess access(publication,id,motion);
   if(access.state&&simulation==publication.session&&publication.session==currentSession()){
    auto& train=*access.state;
    if(train.motion!=motion||head+0.01<train.memory.lastHead){
     auto previous=train.controlled;const bool replaced=train.motion!=0;const auto revision=train.routeRevision+1;
     const bool unqualified=train.motion==motion&&train.integrationUnqualified;
     train={};train.motion=motion;train.routeRevision=revision;
     train.integrationUnqualified=unqualified;
     if(replaced&&previous.instruction.revision){train.controlled=previous;train.controlled.completed=train.controlled.cancelled=true;}
    }
    synchronizeTrain(train,publication,id,head);
    // Pre-entry geometry has its own qualified entryPhysicalRoute, refreshed
    // by integrate. This scan capture is only the established sight/controller
    // path; rebuilding a second approach view would add unbudgeted probes.
    const bool restricted=train.memory.sight||
     (train.controlled.instruction.revision&&!train.controlled.completed&&train.controlled.instruction.mode==1);
    if(offset==0||!previouslyManaged){
     train.ahead.clear();train.managed=false;train.view={head,0,200,GetTickCount64(),true};
     const auto previousRoute=train.signalRoute.revision();
     if(offset==0){RouteWorkPhase phase;const auto now=GetTickCount64();
      train.signalRoute.begin(motion,id,head,now,routeReadBytes,selectedRouteInterested(now));}
     else if(!currentStep.discovery||currentStep.context!=nativeContext||currentStep.motion!=motion||
       !currentStep.discovery->replay(train,publication,context,nativeContext,offsetRef,motion,id,head,offset)){
      train.ahead.clear();train.signalRoute.invalidate();
     }
     if(!routeWorkAvailable())train.signalRoute.invalidate();
     if(train.signalRoute.revision()!=previousRoute){
      train.memory.stopped={};train.waiting={};train.entry={};train.entrySignal=0;train.entryPhysicalRoute.clear();++train.routeRevision;
     }
     if(restricted)train.physicalRoute.begin(motion,id,head,readBytes);
     else train.physicalRoute.clear();
    }
    // Ordered route facts are observed in the existing scan. No game callback
    // is added, and no branch is reconstructed from the raw Path ID vector.
    {
     uintptr_t track{};double from{},to{};
     RouteWorkPhase phase;
     if(routeRead(range,track)&&routeRead(range+8,from)&&routeRead(range+16,to))
      train.signalRoute.append(track,from,to,part,offset,routeReadBytes);
     else train.signalRoute.invalidate();
    }
    if(!routeWorkAvailable())train.signalRoute.invalidate();
    // Only restricted trains need a second physical query. Ordinary BAL
    // continues using the normal native scan, without a full-world capture.
    if(restricted&&train.view.valid&&offset<200) {
     uintptr_t track{},map{};double from{},to{},metric{};
     if(std::abs(train.view.head-head)>1e-6||std::abs(train.view.covered-offset)>.01||
        !read(range,track)||!read(range+8,from)||!read(range+16,to)||!read(track+0x88,metric)||
        !read(nativeContext+0x68,map)||!std::isfinite(from)||!std::isfinite(to)||
        from<0||from>1||to<0||to>1||!std::isfinite(metric)||metric<=0||
        std::abs(part-std::abs(to-from)*metric)>.01){train.view.valid=false;train.physicalRoute.clear();}
     else {
      train.physicalRoute.append(track,from,to,part,offset,readBytes);
      uint8_t clear=1;std::array<uintptr_t,3> query{map,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&clear)};
      bool occupied=false;
      const double free=physicalPrefix(reinterpret_cast<uintptr_t>(query.data()),track,from,to,part,occupied);
      if(free<0){train.view.valid=false;train.physicalRoute.clear();}
      else {if(occupied)train.view.free=std::min(train.view.free,offset+free);train.view.covered=offset+part;}
     }
    }
    if(kind==6){
     int signalKind{};
     if(!read(range+0x28,signalPtr)||!read(signalPtr,signal)||signal>>48!=8||
        !read(signalPtr+0x30,signalKind))train.signalRoute.invalidateObjects();
     else if(signalKind==4&&train.ahead.size()<128){
      const double position=head+offset+part;
      {RouteWorkPhase phase;train.signalRoute.boundary(signalPtr,signal,offset+part,routeReadBytes);}
      if(!routeWorkAvailable())train.signalRoute.invalidate();
      if(train.ahead.empty()||train.ahead.back().signal!=signal)train.ahead.push_back({signal,position});
      if(rule(rules(publication),signal))train.managed=true;
      if(telemetry)InterlockedIncrement64(&telemetry->scans);
     }else if(signalKind!=4&&train.signalRoute.observesObjects()){
      // Read-only annotations of objects ALREADY emitted by this scan. They
      // never enter the driving list or turn a passive marker into a stop.
      {RouteWorkPhase phase;train.signalRoute.encounteredObject(signalPtr,signal,offset+part,routeReadBytes);}
      if(!routeWorkAvailable())train.signalRoute.invalidate();
     }else if(signalKind==4){
      // An omitted PATH at the existing driving-list ceiling must not let a
      // later balise masquerade as the first downstream boundary in V2.
      train.signalRoute.invalidateObjects();
     }
    }
    rememberSources(train,publication);
   }else if(!access.state&&telemetry)InterlockedIncrement64(access.missing?&telemetry->missingTrainSlots:&telemetry->busyTrainScans);
  }
 }
 return originalRouteCall([&]{return nativeScan(context,range);});
}
// 448210 executes 451B60, THEN the final script veto 4273D0. A permission
// recorded by the former is not itself a final admission. This TLS bracket
// observes the already executed SDK stage; it never calls a script twice.
struct FinalAdmissionQuery {
 uintptr_t context{},train{},motion{},sourcePointer{};
 uint64_t signal{};bool observed=false,stageAllowed=false;
};
thread_local FinalAdmissionQuery* finalAdmissionQuery=nullptr;
struct FinalQueryScope {
 FinalAdmissionQuery* previous=finalAdmissionQuery;
 explicit FinalQueryScope(FinalAdmissionQuery& query)noexcept{finalAdmissionQuery=&query;}
 ~FinalQueryScope(){finalAdmissionQuery=previous;}
};
enum class FinalAdmissionReason:size_t {NativeRefused,NativeOnly,ManagedAllowed,RuleChanged,IdentityChanged,StageMissing,Count};
std::array<std::atomic<uint64_t>,static_cast<size_t>(FinalAdmissionReason::Count)> finalAdmissionReasons{};
std::atomic<uint64_t> membershipRechecks{};
void appendFinalAdmissionDiagnostics(std::ostream& message){
 static constexpr std::array<const char*,static_cast<size_t>(FinalAdmissionReason::Count)> names{
  "native_refused","native_only","managed_allowed","rule_changed","identity_changed","stage_missing"};
 message<<" final_admission={membership_rechecks="<<membershipRechecks.load();
 for(size_t i=0;i<names.size();++i)message<<' '<<names[i]<<'='<<finalAdmissionReasons[i].load();
 message<<" native_veto_preserved=1 callbacks_replayed=0}";
}
uint8_t __fastcall checkImpl(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,uintptr_t train,uintptr_t motion,uint64_t signal,uint8_t lookahead){
 if(telemetry)InterlockedIncrement64(&telemetry->permissionCalls);
 Publications::View snapshot(publications);const auto& publication=*snapshot;
 const auto* current=publication.active()&&publication.session==currentSession()?rule(rules(publication),signal):nullptr;
 if(!current){PermissionQuery query;QueryScope scope(query);return originalRouteCall([&]{return nativeCheck(a,b,c,d,train,motion,signal,lookahead);});}
 // Ownership is readable without touching another train's state. Only this
 // managed signal can fail closed when its OWN train is currently unavailable.
 uint64_t id{};double head{};
 if(!read(motion,id)||id>>48!=5||!read(motion+0x3c0,head)||!std::isfinite(head))return 0;
 if(publication.world)publication.world->markManaged(id);
 TrainAccess access(publication,id,motion);
 if(!access.state){if(telemetry)InterlockedIncrement64(access.missing?&telemetry->missingTrainSlots:&telemetry->busyTrainPermissions);return 0;}
 auto& state=*access.state;synchronizeTrain(state,publication,id,head,false);
 if(transactionalCommitEnabled&&state.integrationUnqualified)return 0;
 RouteBudgetScope routeScope(motion);
 std::optional<RouteWorkPhase> qualification;
 const auto denyEntry=[&]() -> uint8_t {
  if(state.memory.stopped.signal==signal&&state.memory.stopped.admitted)state.memory.stopped={};
  if(state.entrySignal==signal){state.entry={};state.entryPhysicalRoute.clear();}
  return 0;
 };
 const auto instruction=*current;const auto observedLifetime=lifetime(publication.signalLifetimes,signal);
 if(instruction.flags&NIMBY_DRIVING_ON_SIGHT)qualification.emplace();
 const bool fresh=freshRule(publication,signal,GetTickCount64());
 double source=head;
 const auto at=std::find_if(state.ahead.begin(),state.ahead.end(),[&](const auto& row){return row.signal==signal;});
 if(at!=state.ahead.end())source=at->position;
 const bool hasAhead=at!=state.ahead.end();
 uint64_t waitingSignal{};uint8_t driving{};double speed{};int64_t ticks{};
 const bool waiting=!lookahead&&read(motion+0x458,waitingSignal)&&waitingSignal==signal&&
  read(motion+0x4b0,driving)&&driving==1&&read(motion+0x3c8,speed)&&
  read(publication.session+0x28,ticks)&&ticks>=0&&ticks<10000000000000LL;
 if(waiting&&fresh&&(instruction.flags&NIMBY_DRIVING_ON_SIGHT)) {
  state.managed=true;
  // Waiting measures standstill only. Without a captured source it cannot
  // invent the signal's route position or a head->source geometry proof.
  if(observeWaitingStop(state.waiting,signal,head,speed,ticks)&&hasAhead&&(instruction.flags&NIMBY_DRIVING_STOP_THEN_PROCEED)&&
     !(state.memory.stopped.admitted&&state.memory.stopped.position==source&&
       stopProofAt(state.memory.stopped,instruction,head,drivingPreferences.get())))
   state.memory.stopped=captureStopProof(instruction,source,head,drivingPreferences.get());
 }
 const bool eligible=(state.managed||!state.ahead.empty())&&hasAhead&&
  restrictedEntry(state.memory,instruction,head,{true,200,0},fresh,drivingPreferences.get());
 PermissionQuery query;query.signal=signal;query.motion=motion;query.head=head;query.started=lookahead==0;query.allowOccupation=eligible;
 query.crossingIdentity={motion,id,signal,state.signalRoute.revision(),hasAhead};
 query.crossing=nimby::windows::automatic::CrossingObservation(query.crossingIdentity);
 nimby::windows::automatic::PhysicalRoute entryRoute;
 if(eligible){
  entryRoute.begin(motion,id,head,routeReadBytes);query.route=&entryRoute;
  const auto gap=source-head;
  if(!std::isfinite(gap)||gap<0||gap>200)query.valid=false;
  else {
   const auto prefix=state.signalRoute.copyPrefix(signal,source,head,GetTickCount64(),routeReadBytes,
    [&](const auto& section){entryRoute.append(section.address,section.from,section.to,section.length,section.offset,routeReadBytes);});
   if(!prefix||std::abs(*prefix-gap)>.01||!entryRoute.valid())query.valid=false;
   else {query.prefixLength=*prefix;query.covered=*prefix;query.prefixObserved=*prefix==0;query.started=*prefix==0&&lookahead==0;}
  }
 }
 const auto routeRevision=state.routeRevision;
 uint8_t native;
 {QueryScope scope(query);native=originalRouteCall([&]{return nativeCheck(a,b,c,d,train,motion,signal,lookahead);});}
 // The publication may change during a native predicate. A removed/re-added
 // source is a new lifetime even when the same owner reuses identical bytes.
 Publications::View latest(publications);current=rule(rules(*latest),signal);
 uint64_t confirmedId{};double confirmedHead{};
 if(latest->world!=publication.world||latest->session!=currentSession()||state.routeRevision!=routeRevision||
    !read(motion,confirmedId)||confirmedId!=id||!read(motion+0x3c0,confirmedHead)||confirmedHead!=head||!current||
    lifetime(latest->signalLifetimes,signal)!=observedLifetime||
    std::memcmp(current,&instruction,sizeof instruction)||!freshRule(*latest,signal,GetTickCount64()))return denyEntry();
 // Preserve only the specific original crossed refusal; generic BLOCKED
 // includes longitudinal physical/reservation denials and is not a specific crossing cause.
 // Requalify the publication, incarnation and route after the native callback.
 auto crossingIdentity=query.crossingIdentity;crossingIdentity.routeRevision=state.signalRoute.revision();
 crossingIdentity.qualified=hasAhead&&routeWorkAvailable();
 const auto crossing=query.crossing.finish(crossingIdentity,native);
 const uint32_t crossingFlags=crossing?( *crossing==nimby::windows::automatic::CrossingRefusal::CrossingOccupied?
  NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED:NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED):0;
 {
  PreserveNativeError preserve;RouteWorkPhase phase;
  // A later observation with no qualified cause overwrites an earlier cause.
  // Absence remains Unknown, and cannot grant movement or release a lease.
  state.signalRoute.admission(signal,!eligible&&native!=0,GetTickCount64(),routeReadBytes,crossingFlags,!eligible);
 }
 if(!routeWorkAvailable())state.signalRoute.invalidate();
 pruneReleased(state,*latest);
 if(instruction.flags&NIMBY_DRIVING_ON_SIGHT) {
  const bool prefixKnown=hasAhead&&state.signalRoute.copyPrefix(signal,source,head,GetTickCount64(),routeReadBytes,[](const auto&){}).has_value();
  qualification.reset();
  const bool budgetReady=routeWorkAvailable();
  if(!budgetReady)state.signalRoute.invalidate();
  const bool granted=eligible&&native&&query.valid&&query.prefixObserved&&prefixKnown&&query.started&&entryRoute.valid()&&std::isfinite(query.covered)&&std::min(query.free,query.covered)>5&&budgetReady;
  if(!granted)denyEntry();
  else if(state.memory.stopped.signal==signal)state.memory.stopped.admitted=true;
  state.entrySignal=signal;
  state.entry={head,query.covered,query.free,GetTickCount64(),granted};
  if(granted){
   state.entryPhysicalRoute=std::move(entryRoute);
   if(!lookahead){state.boundary={Passage{{signal,source},{signal,source},instruction,{}}};state.physicalRoute=state.entryPhysicalRoute;}
  }
  if(telemetry){
   InterlockedIncrement64(&telemetry->restrictedChecks);
   const auto n=reserveSample(telemetry->permissionCount,512);
   if(n>=0){auto& sample=telemetry->permissions[n];sample.observedAtMs=GetTickCount64();
    sample.train=id;sample.signal=signal;sample.proof=state.memory.stopped.signal;
    sample.flags=instruction.flags;sample.state=(eligible?1u:0u)|(native?2u:0u)|(query.valid?4u:0u)|(query.started?8u:0u)|(granted?16u:0u)|32u|(lookahead?64u:0u)|(query.reservationReplaced?128u:0u);
    // Bounded existing diagnostic ring: refusal and its sticky budget reason,
    // without printing one log for every read or simulation tick.
    if(!budgetReady&&routeWorkBudget&&routeWorkBudget->limit)
     sample.state|=256u|(static_cast<uint32_t>(routeWorkBudget->limit->reason())<<16);
    sample.ranges=query.ranges;sample.head=head;sample.source=source;sample.covered=query.covered;sample.free=query.free;
    InterlockedExchange(&sample.ready,1);
   }
   InterlockedIncrement64(granted?&telemetry->restrictedGranted:&telemetry->restrictedDenied);
  }
  rememberSources(state,*latest);return granted?1:0;
 }
 rememberSources(state,*latest);
 if(!fresh||(instruction.flags&NIMBY_DRIVING_STOP)){
  if(telemetry){InterlockedIncrement64(&telemetry->stops);InterlockedExchange64(reinterpret_cast<volatile LONG64*>(&telemetry->lastSignal),signal);}return 0;
 }
 return native;
}
uint8_t __fastcall check(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,uintptr_t train,uintptr_t motion,uint64_t signal,uint8_t lookahead){
 const auto result=checkImpl(a,b,c,d,train,motion,signal,lookahead);
 auto* query=finalAdmissionQuery;
 if(query&&query->train==train&&query->motion==motion&&query->signal==signal&&!lookahead){
  query->observed=true;query->stageAllowed=result!=0;
 }
 return result;
}
uint8_t __fastcall finalAdmission(uintptr_t context,uintptr_t reservations,uintptr_t train,uintptr_t motion,uintptr_t source){
 const auto incoming=GetLastError();
 Publications::View before(publications);
 uint64_t signal{},identity{};
 const bool identified=read(source,signal)&&signal>>48==8&&read(motion,identity)&&identity>>48==5;
 const auto* selected=identified&&before->active()&&before->session==currentSession()?rule(rules(*before),signal):nullptr;
 const bool managed=selected!=nullptr;
 const auto instruction=selected?*selected:NimbySignalDrivingRule{};
 const auto sourceLifetime=managed?lifetime(before->signalLifetimes,signal):0;
 FinalAdmissionQuery query{context,train,motion,source,signal};
 uint8_t result{};
 {FinalQueryScope scope(query);SetLastError(incoming);
  result=originalRouteCall([&]{return nativeFinalAdmission(context,reservations,train,motion,source);});}
 PreserveNativeError preserve;
 const auto outcome=[&](FinalAdmissionReason reason,uint8_t value){++finalAdmissionReasons[static_cast<size_t>(reason)];return value;};
 if(!result)return outcome(FinalAdmissionReason::NativeRefused,0);
 Publications::View after(publications);
 const auto* current=identified&&after->active()&&after->session==currentSession()?rule(rules(*after),signal):nullptr;
 if(!managed&&!current)return outcome(FinalAdmissionReason::NativeOnly,result);
 // The callback can retire/re-add a source, publish a new aspect, or replace
 // the world. That demands a new complete decision, including its callback.
 // Keep the game's final veto intact; no native refusal is ever lifted here.
 if(!managed||!current||before->world!=after->world||before->session!=after->session||
    lifetime(after->signalLifetimes,signal)!=sourceLifetime||
    std::memcmp(current,&instruction,sizeof instruction)||!freshRule(*after,signal,GetTickCount64()))
  return outcome(FinalAdmissionReason::RuleChanged,0);
 uint64_t currentSignal{},currentTrain{};
 if(!read(source,currentSignal)||currentSignal!=signal||!read(motion,currentTrain)||currentTrain!=identity)
  return outcome(FinalAdmissionReason::IdentityChanged,0);
 if(!query.observed||!query.stageAllowed)return outcome(FinalAdmissionReason::StageMissing,0);
 return outcome(FinalAdmissionReason::ManagedAllowed,result);
}
uint8_t __fastcall membership(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t source){
 const auto caller=NRF_PHASE_CALLER();
 const auto result=originalRouteCall([&]{return nativeMembership(context,train,motion,source);});
 PreserveNativeError preserve;
 // Only these two positive shortcuts skip 448210 in the current movement.
 // False directs the original caller into its normal physical + SDK stage
 // and final script veto. Other membership/lookahead callers keep their ABI.
 if(!result||(caller!=base+0x448b29&&caller!=base+0x4493e9)||
    currentStep.context!=context||currentStep.motion!=motion)return result;
 uint64_t signal{},id{};
 Publications::View publication(publications);
 if(publication->active()&&publication->session==currentSession()&&
    read(source,signal)&&signal>>48==8&&read(motion,id)&&id>>48==5&&
    rule(rules(*publication),signal)){
  ++membershipRechecks;return 0;
 }
 return result;
}
uintptr_t __fastcall integrate(uintptr_t result,uintptr_t network,uintptr_t path,uintptr_t presence,uintptr_t dynamics,
 double extraMass,double ceiling,double acceleration,double braking,double distance,double target,int64_t budget,uintptr_t checkState){
 // A recursively invoked native integrator must not inherit its caller's
 // stop fence, including an unmanaged train or an invalid early-return path.
 ProjectionScope noInheritedFence(nullptr);
 if(telemetry)InterlockedIncrement64(&telemetry->integrationCalls);
 double chosenCeiling=ceiling,chosenBraking=braking,chosenDistance=distance,chosenTarget=target;
 const auto invokeNative=[&]{return originalRouteCall([&]{return nativeIntegrate(result,network,path,presence,dynamics,extraMass,chosenCeiling,acceleration,chosenBraking,chosenDistance,chosenTarget,budget,checkState);});};
 Publications::View snapshot(publications);const auto& publication=*snapshot;
 if(!publication.active()||publication.session!=currentSession()||dynamics<8||presence!=dynamics+0x398||budget<=0)return invokeNative();
 const auto motion=dynamics-8;
 uint64_t id{};double head{};float material[8]{};uintptr_t config{};double brakeFactor{};
 if(!read(motion,id)||id>>48!=5||!read(presence+0x20,head)||!read(dynamics+0x1c,material)||
    !read(network+0x10,config)||!read(config+0xd0,brakeFactor)||!std::isfinite(brakeFactor)||brakeFactor<=0||
    !std::isfinite(extraMass)||extraMass<0||!std::isfinite(material[6])||material[6]<=0||
    !std::isfinite(material[2])||material[2]<=0||!std::isfinite(head)||
    !std::isfinite(material[0])||material[0]<0||!std::isfinite(material[7])||material[7]<0)return invokeNative();
 double beforeSpeed{};read(presence+0x28,beforeSpeed);
 uint8_t targetActive{};float nativeTargetCeiling{};
 const auto applyCruise=[&](const Train* state){
  if((trainOptions(publication,state,GetTickCount64())&NIMBY_DRIVING_MAXIMUM_LINE_SPEED)&&
     read(motion+0x4a0,targetActive)&&targetActive<=1&&read(motion+0x49c,nativeTargetCeiling))
   chosenCeiling=cruiseCeiling(ceiling,material[0],true,targetActive!=0,nativeTargetCeiling);
 };
 if(publication.world&&constraint(publication,id))publication.world->markManaged(id);
 if(publication.world&&!publication.world->isManaged(id)){
  applyCruise(nullptr);
  if(telemetry)InterlockedIncrement64(chosenCeiling==ceiling?&telemetry->unmanagedNativeIntegrations:&telemetry->unmanagedCruiseIntegrations);
  return invokeNative();
 }
 PendingIntegrationFrame* commitFrame=transactionalCommitEnabled&&currentCommitFrame&&
     currentCommitFrame->motion==motion&&path==motion+0x290?currentCommitFrame:nullptr;
 // The previous iteration has passed native permission/collision processing
 // before this integrator is reached. Qualify it BEFORE planning this motion.
 // bind takes the persistent guard before the local guard below can lend one.
 bool frameBound=false;
 if(commitFrame){
  if(commitFrame->integrationDepth)commitFrame->ambiguous=true;
  else finishPendingIntegration(*commitFrame,false);
  frameBound=commitFrame->bind(publication,id,network);
 }
 TrainAccess access(publication,id,motion);
 if(!access.state){
  // The independent interest index established a managed/ambiguous identity.
  // Its missing or busy slot may carry restrictions, so only this train is
  // conservative. Never infer absence of instructions from a failed lookup.
  if(telemetry)InterlockedIncrement64(access.missing?&telemetry->missingTrainSlots:&telemetry->busyTrainIntegrations);
  chosenCeiling=0;chosenDistance=0;chosenTarget=0;
  const auto returned=invokeNative();
  recordIntegration(id,head,beforeSpeed,presence,result,ceiling,braking,distance,target,chosenCeiling,chosenBraking,chosenDistance,chosenTarget,access.missing?2u:1u);
  return returned;
 }
 auto& state=*access.state;
 // A recycled native object cannot use another object's retained instructions.
 uint64_t confirmed{};if(!read(motion,confirmed)||confirmed!=id)return invokeNative();
 if(transactionalCommitEnabled){
  // These heads can belong to an in-flight or otherwise unqualified attempt.
  // Even planning is mutating: plan/ControlledMotion may release instructions
  // when they observe that head. Reject BEFORE synchronize/prepare, preserving
  // the last qualified instruction memory rather than trying to undo it later.
  std::optional<CommitReason> refusal;
  if(commitFrame&&(commitFrame->ambiguous||commitFrame->integrationDepth))refusal=CommitReason::Reentrant;
  else if(state.integrationUnqualified)refusal=CommitReason::AlreadyLatched;
  else if(!commitFrame)refusal=CommitReason::OutsideNativeStep;
  else if(!frameBound||commitFrame->state()!=&state)refusal=CommitReason::OwnerUnavailable;
  if(refusal){
   if(commitFrame)commitFrame->reject(*refusal);
   else {commitOutcomes[static_cast<size_t>(*refusal)].fetch_add(1,std::memory_order_relaxed);
    commitSampleTrain[static_cast<size_t>(*refusal)].store(id,std::memory_order_relaxed);}
   if(!state.integrationUnqualified){state.integrationUnqualified=true;commitLatched.fetch_add(1,std::memory_order_relaxed);}
   chosenCeiling=0;chosenDistance=0;chosenTarget=0;
   if(telemetry)InterlockedIncrement64(&telemetry->restrictedSteps);
   const auto returned=invokeNative();
   // This is a restrictive command, not a claim that the native 0.01 m/s
   // floor has been eliminated or that a physical stop was certified.
   recordIntegration(id,head,beforeSpeed,presence,result,ceiling,braking,distance,target,
       chosenCeiling,chosenBraking,chosenDistance,chosenTarget,32u+static_cast<uint32_t>(*refusal),&state);
   return returned;
  }
 }
 synchronizeTrain(state,publication,id,head);
 const auto now=GetTickCount64();const bool fresh=freshTrain(publication,state,head,now);
 applyCruise(&state);
 const bool preEntry=!state.memory.sight&&state.memory.stopped.admitted&&state.entry.valid&&state.entrySignal==state.memory.stopped.signal;
 std::optional<RouteBudgetScope> routeScope;
 if(preEntry)routeScope.emplace(motion);
 if(state.memory.sight||preEntry||(state.controlled.instruction.revision&&!state.controlled.completed&&state.controlled.instruction.mode==1)){
  uintptr_t map{},stepNetwork{},stepSession{};
  if(currentStep.motion==motion&&path==motion+0x290&&read(currentStep.context+8,stepNetwork)&&stepNetwork==network&&
     read(currentStep.context+0x18,stepSession)&&stepSession==publication.session&&read(currentStep.context+0x68,map)&&map){
   uintptr_t queryMotion=motion;uint8_t clear=1;
   std::array<uintptr_t,3> query{map,reinterpret_cast<uintptr_t>(&queryMotion),reinterpret_cast<uintptr_t>(&clear)};
   const auto& physicalRoute=preEntry?state.entryPhysicalRoute:state.physicalRoute;
   const auto refresh=[&]{
    state.view=physicalRoute.refresh(motion,head,now,preEntry?routeReadBytes:readBytes,
     [&](uintptr_t track,double from,double to,double length){bool occupied=false;
      return physicalPrefix(reinterpret_cast<uintptr_t>(query.data()),track,from,to,length,occupied,preEntry);});
    return state.view.valid;
   };
   // Qualify once around the live occupation queries. A change after any query
   // discards the whole view, including zero-gap source contact.
   if(preEntry){
    {RouteWorkPhase phase;
     if(!state.signalRoute.withPrefix(state.entrySignal,state.memory.stopped.position,head,now,routeReadBytes,
        [](const auto&){},[&](double){return refresh();}))state.view={};
    }
    if(!routeWorkAvailable())state.view={};
   }else refresh();
  }else state.view={};
  if(preEntry)state.entry=state.view;
 }
 const double stopBeforeSignalMeters=drivingPreferences.get();
 auto proposed=transactionalCommitEnabled&&state.integrationUnqualified?
     Integration{0,braking,0,0,true,true,{},{},0}:
     prepareIntegration(state,rules(publication),head,material[7],material[0],material[2],material[6],extraMass,brakeFactor,fresh,now,chosenCeiling,braking,distance,target,stopBeforeSignalMeters);
 chosenCeiling=proposed.ceiling;chosenBraking=proposed.braking;chosenDistance=proposed.distance;chosenTarget=proposed.target;
 if(proposed.restricted&&telemetry)InterlockedIncrement64(&telemetry->restrictedSteps);
 // Reuse the publisher field as a PRIVATE lifetime token. It never crosses the
 // public ABI. A release/re-add of the same owner must invalidate old passages.
 for(auto& passage:proposed.encountered)passage.publisher=lifetime(publication.signalLifetimes,passage.source.signal);
 rememberSources(state,publication);
 const auto routeRevision=state.routeRevision;
 bool deferred=false;
 if(transactionalCommitEnabled&&proposed.observed){
  const auto reject=[&](CommitReason reason){
   if(commitFrame)commitFrame->reject(reason);
   else {commitOutcomes[static_cast<size_t>(reason)].fetch_add(1,std::memory_order_relaxed);
    commitSampleTrain[static_cast<size_t>(reason)].store(id,std::memory_order_relaxed);}
   if(!state.integrationUnqualified){state.integrationUnqualified=true;commitLatched.fetch_add(1,std::memory_order_relaxed);}
   chosenCeiling=0;chosenDistance=0;chosenTarget=0;
  };
  if(state.integrationUnqualified)reject(CommitReason::AlreadyLatched);
  else if(!commitFrame)reject(CommitReason::OutsideNativeStep);
  else if(!frameBound||commitFrame->state()!=&state)reject(CommitReason::OwnerUnavailable);
  else if(commitFrame->ambiguous||commitFrame->integrationDepth)reject(CommitReason::Reentrant);
  else {
   auto& pending=commitFrame->pending;RouteWorkPhase phase;
   uint64_t beforeId{},dbId{};
   if(!routeRead(motion,beforeId)||beforeId!=id||!routeRead(commitFrame->trainAddress,dbId)||dbId!=id)
    reject(CommitReason::IdentityUnqualified);
   else if(!routeRead(presence,pending.before)||!validCommitPresence(pending.before)||pending.before.head!=head)
    reject(CommitReason::PresenceUnqualified);
   else if(!pending.path.capture(motion,routeReadBytes))reject(CommitReason::PathUnqualified);
   else if(proposed.encountered.size()>pending.sources.size())reject(CommitReason::SourcesUnqualified);
   else {
    std::array<uint64_t,128> selected{};std::array<double,128> positions{};
    pending.sourceCount=0;bool selectedComplete=true;
    const auto select=[&](Ahead value){
     if(!value.signal)return;
     for(size_t i=0;i<pending.sourceCount;++i)if(selected[i]==value.signal){selectedComplete=selectedComplete&&positions[i]==value.position;return;}
     if(pending.sourceCount==selected.size()){selectedComplete=false;return;}
     selected[pending.sourceCount]=value.signal;positions[pending.sourceCount++]=value.position;
    };
    // Announcement targets and cancellation/exit points are native anchors too.
    // A stable source alone must not retain a warning to a moved/recycled target.
    for(const auto& passage:proposed.encountered){select(passage.source);select(passage.target);select(passage.next);}
    if(!selectedComplete||(pending.sourceCount&&!state.signalRoute.captureCommitSources(std::span(selected.data(),pending.sourceCount),
        std::span(positions.data(),pending.sourceCount),std::span(pending.sources.data(),pending.sourceCount),routeReadBytes))
       )reject(CommitReason::SourcesUnqualified);
    else if(!routeWorkAvailable())reject(CommitReason::ResultUnqualified);
    else {pending.routeRevision=state.routeRevision;pending.signalRouteRevision=state.signalRoute.revision();
     pending.preEntry=preEntry;pending.stopBeforeSignalMeters=stopBeforeSignalMeters;pending.budget=budget;deferred=true;}
   }
  }
 }
 const bool changed=chosenCeiling!=ceiling||chosenBraking!=braking||chosenDistance!=distance||chosenTarget!=target;
 if(changed&&telemetry)InterlockedIncrement64(&telemetry->applied);
 ProjectionFence fence{};
 const bool fenced=captureProjectionFence(fence,network,path,motion,id,presence,head,proposed);
 double beforeOverlap=std::numeric_limits<double>::quiet_NaN();
 if(fenced)read(presence+0x30,beforeOverlap);
 const auto returned=[&]{
  struct AttemptScope {PendingIntegrationFrame* frame;explicit AttemptScope(PendingIntegrationFrame* value):frame(value){if(frame)++frame->integrationDepth;}
   ~AttemptScope(){if(frame)--frame->integrationDepth;}} attempt(transactionalCommitEnabled?commitFrame:nullptr);
  ProjectionScope scoped(fenced?&fence:nullptr);return invokeNative();
 }();
 if(fenced)consumeFencedIdle(result,returned,presence,budget,fence,beforeOverlap);
 // access remains held across native integration. Nested callbacks on this
 // thread borrowed it, so there is no second lock that can lose a NEW crossing.
 if(transactionalCommitEnabled&&deferred){
  auto& pending=commitFrame->pending;RouteWorkPhase phase;uint64_t afterId{};
  if(commitFrame->ambiguous)commitFrame->reject(CommitReason::Reentrant);
  else if(!routeRead(motion,afterId)||afterId!=id)commitFrame->reject(CommitReason::IdentityUnqualified);
  else if(!routeRead(presence,pending.after)||!validCommitPresence(pending.after)||pending.after.head<pending.before.head)
   commitFrame->reject(CommitReason::PresenceUnqualified);
  else if(returned!=result||!routeRead(presence+0x28,pending.speed)||!std::isfinite(pending.speed)||pending.speed<0||
     !routeRead(result+0x30,pending.elapsed)||pending.elapsed<=0||pending.elapsed>budget)
   commitFrame->reject(CommitReason::ResultUnqualified);
  else if(!pending.path.matches(motion,routeReadBytes))commitFrame->reject(CommitReason::PathUnqualified);
  else if(!routeWorkAvailable())commitFrame->reject(CommitReason::ResultUnqualified);
  else {pending.encountered=std::move(proposed.encountered);pending.active=true;}
 }else if(!transactionalCommitEnabled&&proposed.observed){
  Publications::View latest(publications);double afterHead{};uint64_t afterId{};
  if(latest->world==publication.world&&latest->session==currentSession()&&state.routeRevision==routeRevision&&read(motion,afterId)&&afterId==id&&read(presence+0x20,afterHead)){
   pruneReleased(state,*latest);
   const bool entryBudgetReady=!preEntry||routeWorkAvailable();
   if(!entryBudgetReady){state.entry={};state.entryPhysicalRoute.clear();state.view={};state.memory.stopped={};}
   std::erase_if(proposed.encountered,[&](const auto& passage){
    const auto* current=rule(rules(*latest),passage.source.signal);
    return (!entryBudgetReady&&passage.source.signal==state.entrySignal)||!passage.publisher||lifetime(latest->signalLifetimes,passage.source.signal)!=passage.publisher||
     !current||std::memcmp(current,&passage.instruction,sizeof passage.instruction)!=0;
   });
   double speed{};int64_t elapsed{};
   const bool readable=read(presence+0x28,speed)&&read(result+0x30,elapsed);
   commitIntegration(state,proposed.encountered,rules(*latest),head,afterHead,speed,elapsed,budget,readable,freshTrain(*latest,state,afterHead,GetTickCount64()),stopBeforeSignalMeters);
   // Lookahead alone creates no passage. Once measured motion crosses the
   // real source, transfer its qualified admission geometry to retained sight.
   if(preEntry&&state.memory.sight&&state.memory.sight->source==state.entrySignal)
    state.physicalRoute=std::move(state.entryPhysicalRoute);
   rememberSources(state,*latest);
   if(telemetry)InterlockedIncrement64(&telemetry->committedIntegrations);
  }else if(telemetry)InterlockedIncrement64(&telemetry->discardedCommits);
 }
 if(changed){state.sampleBudget-=budget;if(state.sampleBudget<=0){
  const auto budgetReason=preEntry&&routeWorkBudget&&routeWorkBudget->limit?routeWorkBudget->limit->reason():nimby::windows::automatic::RouteReadBudget::Reason::None;
  const auto reason=budgetReason==nimby::windows::automatic::RouteReadBudget::Reason::None?(fresh?0u:3u):16u+static_cast<uint32_t>(budgetReason);
  recordIntegration(id,head,beforeSpeed,presence,result,ceiling,braking,distance,target,chosenCeiling,chosenBraking,chosenDistance,chosenTarget,reason,&state);
  state.sampleBudget=1000;
 }}
 return returned;
}
}
namespace {
template<class Rows> std::vector<SourceLifetime> nextLifetimes(const Rows& before,const Rows& next,
 std::span<const SourceLifetime> previous,uint64_t revision,bool sameWorld){
 std::vector<SourceLifetime> result;result.reserve(next.rows().size());
 for(const auto& row:next.rows()){
  const auto id=Rows::id(row);const auto retained=sameWorld&&before.owner(id)==next.owner(id)?lifetime(previous,id):0;
  result.push_back({id,retained?retained:revision});
 }
 return result;
}
template<bool Signals,class Row> uint32_t publishRows(std::vector<Row> rows,uint64_t deadline,uint32_t options,uint64_t owner){
 PublicationBudget budget;
 std::shared_ptr<const Publication> origin;
 for(unsigned attempt=0;attempt<3;++attempt){
  std::shared_ptr<const Publication> before;
  {PublicationLock lock(budget);if(!lock){if(telemetry)InterlockedIncrement64(&telemetry->publicationBusy);return NIMBY_RESOURCE_LIMIT;}before=publications.snapshot();}
  if(!origin)origin=before;
  else if(before->worldRevision!=origin->worldRevision)return NIMBY_DATA_UNAVAILABLE;
  const auto& prior=[&]() -> const auto& {if constexpr(Signals)return before->rules;else return before->constraints;}();
  const auto& first=[&]() -> const auto& {if constexpr(Signals)return origin->rules;else return origin->constraints;}();
  if(attempt&&!prior.sameOwnerVersion(first,owner))return NIMBY_RESOURCE_LIMIT;
  const auto sim=currentSession();const bool nonempty=!rows.empty()||options;
  if(!sim&&nonempty)return NIMBY_DATA_UNAVAILABLE;
  auto next=std::make_shared<Publication>(*before);next->revision=before->revision+1;
  const bool sameWorld=sim==before->session;
  if(!sameWorld){
   next->session=sim;++next->worldRevision;next->world=sim?std::make_shared<TrainWorld>():nullptr;
   next->rules.clear();next->constraints.clear();next->signalLifetimes.clear();next->constraintLifetimes.clear();
  }else if(sim&&!next->world)next->world=std::make_shared<TrainWorld>();
  uint32_t result;
  if constexpr(Signals){
   result=next->rules.publish(owner,rows,deadline,options);
   if(result==NIMBY_OK)next->signalLifetimes=nextLifetimes(before->rules,next->rules,before->signalLifetimes,next->revision,sameWorld);
  }else{
   result=next->constraints.publish(owner,rows,deadline);
   if(result==NIMBY_OK)next->constraintLifetimes=nextLifetimes(before->constraints,next->constraints,before->constraintLifetimes,next->revision,sameWorld);
  }
  if(result!=NIMBY_OK)return result;
  if(currentSession()!=sim||(nonempty&&GetTickCount64()>=deadline))return NIMBY_DATA_UNAVAILABLE;
  std::array<std::shared_ptr<Publication>,32> garbage;
  {PublicationLock lock(budget);if(!lock){if(telemetry)InterlockedIncrement64(&telemetry->publicationBusy);return NIMBY_RESOURCE_LIMIT;}
   if(nonempty&&GetTickCount64()>=deadline)return NIMBY_DATA_UNAVAILABLE;
   if(publications.snapshot()!=before)continue;
   if(!publications.commit(before,next,garbage)){if(telemetry)InterlockedIncrement64(&telemetry->publicationRetirementFull);return NIMBY_RESOURCE_LIMIT;}
   if(telemetry){const auto current=publications.snapshot();
    InterlockedExchange64(reinterpret_cast<volatile LONG64*>(&telemetry->session),current->session);
    InterlockedExchange(&telemetry->ruleCount,static_cast<LONG>(current->rules.rows().size()));
   }
  }
  return NIMBY_OK;
 }
 if(telemetry)InterlockedIncrement64(&telemetry->publicationConflicts);
 return NIMBY_RESOURCE_LIMIT;
}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_PublishV3(const NimbySignalDrivingRule* input,uint32_t count,uint32_t lease,uint32_t options,uint64_t publisher) noexcept {
 try {
  const auto deadline=GetTickCount64()+lease;std::vector<NimbySignalDrivingRule> next;
  if(!publisher||!installed||!prepareRules(input,count,lease,options,next))return NIMBY_INVALID_ARGUMENT;
  return publishRows<true>(std::move(next),deadline,options,publisher);
 }catch(...){nimby::detail::diagnostics::exception("sdk",__func__);return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_PublishV2(const NimbySignalDrivingRule* input,uint32_t count,uint32_t lease,uint32_t options) noexcept {
 // Legacy bridge callers share only this reserved namespace; they cannot
 // replace or release owner-scoped batches. SDK clients use V3, even for V1/V2.
 return NimbyDriving_PublishV3(input,count,lease,options,1);
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
 if(argument)return NIMBY_INVALID_ARGUMENT;
 nimby::platform::windows::BridgeInstallation installation;
 if(!installation)return installation.status();
 AcquireSRWLockExclusive(&initialization);struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
 if(installed)return NIMBY_ALREADY_INITIALIZED;
 std::array<wchar_t,32768> path{};NimbyBinaryInfo identity{};
 if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
    nimby::engine::identify(path.data(),identity)!=NIMBY_OK||!identity.recognized_research_build)return NIMBY_INVALID_BINARY;
 base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
 if(!drivingPreferences.open())return NIMBY_IO_ERROR;
 const auto mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(nimby::automatic_status::Shared),nimby::automatic_status::name(GetCurrentProcessId()).c_str());
 if(!mapping)return NIMBY_IO_ERROR;
 if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);return NIMBY_IO_ERROR;}
 telemetry=static_cast<nimby::automatic_status::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,0));
 if(!telemetry){CloseHandle(mapping);return NIMBY_IO_ERROR;}
 *telemetry=nimby::automatic_status::Shared{};
 struct MappingCleanup{HANDLE mapping;~MappingCleanup(){if(!installed){UnmapViewOfFile(telemetry);telemetry=nullptr;CloseHandle(mapping);}}} mappingCleanup{mapping};
 struct Entry{uintptr_t rva;std::array<unsigned char,16> bytes;void* hook;void** original;};
 const Entry entries[]{
  {0x448710,{0x48,0x8b,0xc4,0x4c,0x89,0x48,0x20,0x4c,0x89,0x40,0x18,0x48,0x89,0x50,0x10,0x55},reinterpret_cast<void*>(&stepMotion),reinterpret_cast<void**>(&nativeStep)},
  {0x378600,{0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x4c,0x24,0x08,0x55,0x56,0x57,0x41,0x55,0x41},reinterpret_cast<void*>(&integrate),reinterpret_cast<void**>(&nativeIntegrate)},
  {0x377dd0,{0x4c,0x89,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x48},reinterpret_cast<void*>(&project),reinterpret_cast<void**>(&nativeProject)},
  {0x449700,{0x48,0x8b,0xc4,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x53,0x55,0x56,0x57,0x41},reinterpret_cast<void*>(&scan),reinterpret_cast<void**>(&nativeScan)},
  {0x4582d0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x50,0x4c},reinterpret_cast<void*>(&occupancy),reinterpret_cast<void**>(&nativeOccupancy)},
  {0x458460,{0x4c,0x8b,0xdc,0x53,0x48,0x81,0xec,0x90,0x00,0x00,0x00,0x41,0x0f,0x29,0x73,0xe8},reinterpret_cast<void*>(&reservation),reinterpret_cast<void**>(&nativeReservation)},
  {0x458560,{0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xec,0x40,0x48,0x8b,0x01,0x48,0x8b,0xf9},reinterpret_cast<void*>(&permissionRange),reinterpret_cast<void**>(&nativePermissionRange)},
  {0x451b60,{0x40,0x55,0x53,0x56,0x57,0x48,0x8d,0x6c,0x24,0xe1,0x48,0x81,0xec,0xd8,0x00,0x00},reinterpret_cast<void*>(&check),reinterpret_cast<void**>(&nativeCheck)},
  {0x448210,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x41},reinterpret_cast<void*>(&finalAdmission),reinterpret_cast<void**>(&nativeFinalAdmission)},
  {0x447310,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x30,0x49},reinterpret_cast<void*>(&membership),reinterpret_cast<void**>(&nativeMembership)}
 };
 const Entry passageEntry{0x4275b0,{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x56,0x57,0x41,0x54,0x41,0x55},
  reinterpret_cast<void*>(&passBy),reinterpret_cast<void**>(&nativePassBy)};
 const Entry phaseEntries[]{
  {0x440840,{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x4c,0x89,0x48,0x20,0x48,0x89,0x50,0x10,0x55},reinterpret_cast<void*>(&replanRoute440840),reinterpret_cast<void**>(&nativeReplan440840)},
  {0x30e020,{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x4c,0x89,0x40,0x18,0x48,0x89,0x50,0x10,0x55},reinterpret_cast<void*>(&modelCommandBatch),reinterpret_cast<void**>(&nativeModelCommandBatch)},
  // MinHook patches five bytes here; the native RIP JMP is seven bytes and
  // the next thunk starts at +8. The 16-byte profile check is read-only.
  {0x983c2c,{0x48,0xff,0x25,0x15,0x77,0x02,0x00,0xcc,0x48,0xff,0x25,0x4d,0x77,0x02,0x00,0xff},reinterpret_cast<void*>(&releaseShared),reinterpret_cast<void**>(&nativeSharedRelease)},
  {0x348960,{0x40,0x53,0x48,0x83,0xec,0x30,0xb8,0x01,0x00,0x00,0x00,0x48,0x8b,0xd9,0x86,0x41},reinterpret_cast<void*>(&stopRunner),reinterpret_cast<void**>(&nativeRunnerStop)},
  {0x3488c0,{0x40,0x53,0x48,0x83,0xec,0x40,0x48,0x8b,0xd9,0x83,0x79,0x18,0x00,0x75,0x68,0x33},reinterpret_cast<void*>(&startRunner),reinterpret_cast<void**>(&nativeRunnerStart)},
  {0x478b20,{0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x55,0x56,0x57,0x41,0x54,0x41},reinterpret_cast<void*>(&mergeWorld),reinterpret_cast<void**>(&nativeWorldMerge)},
  {0x3489d0,{0x48,0x8b,0xc4,0x48,0x89,0x48,0x08,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41},reinterpret_cast<void*>(&controllerPhase),reinterpret_cast<void**>(&nativeControllerPhase)},
  {0x478000,{0x4c,0x89,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x55},reinterpret_cast<void*>(&advancePhase),reinterpret_cast<void**>(&nativeAdvancePhase)},
  {0x477ce0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x54,0x24,0x10,0x55,0x56,0x57,0x41,0x54,0x41},reinterpret_cast<void*>(&simulationPhase),reinterpret_cast<void**>(&nativeSimulationPhase)},
  {0x44d6b0,{0x4c,0x89,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x48},reinterpret_cast<void*>(&trainPhase),reinterpret_cast<void**>(&nativeTrainPhase)},
  {0x47d260,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x40,0x48},reinterpret_cast<void*>(&resetWorld),reinterpret_cast<void**>(&nativeWorldReset)},
  {0x47d410,{0x48,0x89,0x54,0x24,0x10,0x48,0x89,0x4c,0x24,0x08,0x53,0x55,0x56,0x57,0x41,0x54},reinterpret_cast<void*>(&rebuildWorld),reinterpret_cast<void**>(&nativeWorldRebuild)}
 };
 wchar_t shadow[4]{};const bool observePassages=GetEnvironmentVariableW(L"NRF_INTERLOCKING_SHADOW",shadow,4)==1&&shadow[0]==L'1';
 wchar_t transactional[4]{};const bool transactionalCommits=GetEnvironmentVariableW(L"NRF_DRIVING_TRANSACTIONAL_COMMIT",transactional,4)==1&&transactional[0]==L'1';
 for(const auto& e:entries)if(std::memcmp(reinterpret_cast<void*>(base+e.rva),e.bytes.data(),16))return NIMBY_INVALID_BINARY;
 if(observePassages&&std::memcmp(reinterpret_cast<void*>(base+passageEntry.rva),passageEntry.bytes.data(),16))return NIMBY_INVALID_BINARY;
 if(observePassages)for(const auto& e:phaseEntries)if(std::memcmp(reinterpret_cast<void*>(base+e.rva),e.bytes.data(),16))return NIMBY_INVALID_BINARY;
 if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
 for(const auto& e:entries)if(MH_CreateHook(reinterpret_cast<void*>(base+e.rva),e.hook,e.original)!=MH_OK){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 if(observePassages&&MH_CreateHook(reinterpret_cast<void*>(base+passageEntry.rva),passageEntry.hook,passageEntry.original)!=MH_OK){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 if(observePassages)for(const auto& e:phaseEntries)if(MH_CreateHook(reinterpret_cast<void*>(base+e.rva),e.hook,e.original)!=MH_OK){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 HMODULE self{};
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&integrate),&self)){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 // No active rules until a successful explicit publication by the mod.
 passageObservationEnabled=observePassages;
 phaseObservationEnabled=observePassages;
 transactionalCommitEnabled=transactionalCommits;
 if(MH_EnableHook(MH_ALL_HOOKS)!=MH_OK){passageObservationEnabled=false;phaseObservationEnabled=false;transactionalCommitEnabled=false;MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 installed=true;return NIMBY_OK;
}

extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_Publish(const NimbySignalDrivingRule* input,uint32_t count,uint32_t lease) noexcept {
 return NimbyDriving_PublishV2(input,count,lease,0);
}

// Replace a complete set atomically. Invalid batches leave the active set intact.
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_TrainConstraints(
 const NimbyTrainConstraint* input,uint32_t count,uint32_t lease,uint64_t publisher) noexcept {
 try {
  const auto deadline=GetTickCount64()+lease;
  if(!publisher||!installed||count>8192||(!input&&count)||lease<100||lease>5000)return NIMBY_INVALID_ARGUMENT;
  std::vector<NimbyTrainConstraint> next;if(count)next.assign(input,input+count);
  for(const auto& row:next)if(!validConstraint(row))return NIMBY_INVALID_ARGUMENT;
  std::sort(next.begin(),next.end(),[](const auto& a,const auto& b){return a.train<b.train;});
  for(size_t i=1;i<next.size();++i)if(next[i-1].train==next[i].train)return NIMBY_INVALID_ARGUMENT;
  return publishRows<false>(std::move(next),deadline,0,publisher);
 }catch(...){nimby::detail::diagnostics::exception("sdk",__func__);return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_ReadTrainConstraint(uint64_t train,NimbyTrainConstraintStatus* out) noexcept {
 if(!out||out->size!=sizeof *out||train>>48!=5)return NIMBY_INVALID_ARGUMENT;
 *out={};out->size=sizeof *out;out->train=train;
 Publications::View view(publications);
 if(!view->session||view->session!=currentSession())return NIMBY_DATA_UNAVAILABLE;
 const auto current=constraint(*view,train);
 if(!freshConstraint(*view,train,GetTickCount64())||!current)return NIMBY_OK;
 out->revision=current->revision;out->speed_mps=current->speed_mps;out->exit_signal=current->exit_signal;out->state=1;
 const auto slot=view->world?view->world->find(train,false):nullptr;
 if(!slot)return NIMBY_OK;
 // Readback does not know a native motion address. It never creates/rebinds a
 // slot or enters a native callback, and a busy train reports a bounded retry.
 if(!TryAcquireSRWLockExclusive(&slot->lock))return NIMBY_RESOURCE_LIMIT;
 struct Unlock{SRWLOCK* lock;~Unlock(){ReleaseSRWLockExclusive(lock);}}unlock{&slot->lock};
 if(slot->id==train&&slot->state.constraintLifetime==lifetime(view->constraintLifetimes,train)&&slot->state.controlled.instruction.revision==current->revision){
  out->state=slot->state.controlled.state();out->exit_signal=slot->state.controlled.exitSignal;
 }
 return NIMBY_OK;
}
namespace {
uint32_t copySignalRoutes(const uint64_t* signals,uint32_t signalCount,
 uint32_t blocks,NimbySignalRouteObservation* out,uint32_t capacity,uint32_t* count,
 NimbySignalRouteSection* sections,uint32_t sectionCapacity,uint32_t* sectionCount,
 NimbySignalRouteAnnotation* annotations,uint32_t annotationCapacity,uint32_t* annotationCount,bool selectedRoute) noexcept {
 using namespace nimby::windows::automatic;
 drainNativePassages();drainCommitOutcomes();drainTrainPhases();
 RouteBatchDiagnostic diagnostic;diagnostic.requestedSignals=signalCount;diagnostic.blocks=blocks;
 bool outputsInitialized=false;
 const auto finish=[&](uint32_t status) noexcept {
  diagnostic.status=status;
  if(outputsInitialized){diagnostic.rows=*count;diagnostic.sections=*sectionCount;}
  // One aggregate per observation worker: no formatting, allocation or sink
  // access in native scans, while holding a train slot, or for every train.
  thread_local RouteDiagnosticWindow window;
  if(window.observe(diagnostic,GetTickCount64())){
   try {
    std::ostringstream message;
    message<<"Signal route batch: status="<<status<<" requested_signals="<<signalCount<<" blocks="<<blocks
           <<" rows="<<diagnostic.rows<<" sections="<<diagnostic.sections<<" batches="<<window.batches
           <<" failed_batches="<<window.failedBatches<<" busy_slots="<<window.busySlots<<" qualification={";
    for(size_t i=0;i<window.reasons.size();++i)if(window.reasons[i])
     message<<routeCopyReasonName(static_cast<RouteCopyReason>(i))<<':'<<window.reasons[i]<<' ';
    message<<"} whole_route_samples=[";
    for(size_t i=0;i<window.sampleCount;++i)
     message<<"train="<<window.samples[i].train<<" reason="<<routeCopyReasonName(window.samples[i].reason)<<' ';
    message<<"] native_stage_only=1";
    nimby::detail::diagnostics::write("sdk",window.failedBatches?"WARN":"INFO",message.str().c_str());
   }catch(...){/* Diagnostics never change a capture's result or retry a sink. */}
   window.clearWindow();
  }
  return status;
 };
 try {
  if(!count||!sectionCount||!signals||!signalCount||signalCount>4096||!blocks||blocks>16||capacity>65536||(!out&&capacity)||
     sectionCapacity>131072||(!sections&&sectionCapacity)||
     (selectedRoute&&(!annotationCount||annotationCapacity>131072||(!annotations&&annotationCapacity))))return finish(NIMBY_INVALID_ARGUMENT);
  *count=*sectionCount=0;outputsInitialized=true;
  if(selectedRoute)*annotationCount=0;
  const std::span<const uint64_t> selected(signals,signalCount);
  for(size_t i=0;i<selected.size();++i)if(selected[i]>>48!=8||(i&&selected[i-1]>=selected[i]))return finish(NIMBY_INVALID_ARGUMENT);
  if(selectedRoute){
   // No callback/sink/predicate is run here. The next ordinary native scan
   // may collect objects; a pre-interest route remains explicitly unknown.
   selectedRouteInterestUntil.store(GetTickCount64()+2000,std::memory_order_relaxed);
  }
  // Release the immutable publication/world reader pin before formatting or
  // sending diagnostics. A slow sink must not retain retired controller state.
  const auto status=[&]() -> uint32_t {
  Publications::View view(publications);
  if(!view->active()||view->session!=currentSession()||!view->world)return NIMBY_DATA_UNAVAILABLE;
  const auto now=GetTickCount64();bool full=false,exhausted=false;
  size_t remainingBytes=8*1024*1024,remainingReads=32768;
  LARGE_INTEGER started{},frequency{};QueryPerformanceCounter(&started);QueryPerformanceFrequency(&frequency);
  const auto boundedRead=[&](uintptr_t address,void* value,size_t size){
   if(exhausted)return false;
   if(!remainingReads||size>remainingBytes){exhausted=true;return false;}
   --remainingReads;remainingBytes-=size;
   if(remainingReads%64==0&&frequency.QuadPart){LARGE_INTEGER current{};QueryPerformanceCounter(&current);
    if(current.QuadPart-started.QuadPart>frequency.QuadPart/125){exhausted=true;return false;}}
   return readBytes(address,value,size);
  };
  for(auto& slot:view->world->slots){
   if(!slot.key.load(std::memory_order_acquire))continue;
   if(!TryAcquireSRWLockExclusive(&slot.lock)){++diagnostic.busySlots;continue;}
   struct Unlock{SRWLOCK* value;~Unlock(){ReleaseSRWLockExclusive(value);}}unlock{&slot.lock};
   if(!slot.id||slot.id>>48!=5)continue;
   const auto beginRows=*count,beginSections=*sectionCount,beginAnnotations=selectedRoute?*annotationCount:0;bool localFull=false;
   const auto emitRow=[&](const auto& row){
    if(*count<capacity)out[(*count)++]=row;else localFull=true;
   };
   const auto emitSection=[&](const auto& row){
    if(*sectionCount<sectionCapacity)sections[(*sectionCount)++]=row;else localFull=true;
   };
   const auto reason=selectedRoute?
    slot.state.signalRoute.copyDiagnosed(selected,blocks,now,boundedRead,emitRow,emitSection,[&](const auto& row){
     if(*annotationCount<annotationCapacity)annotations[(*annotationCount)++]=row;else localFull=true;
    }):slot.state.signalRoute.copyDiagnosed(selected,blocks,now,boundedRead,emitRow,emitSection);
   diagnostic.note(reason,slot.id);
   if(reason!=RouteCopyReason::Ready){*count=beginRows;*sectionCount=beginSections;if(selectedRoute)*annotationCount=beginAnnotations;}
   else full|=localFull;
   if(full||exhausted)break;
  }
  // An incomplete batch cannot safely choose a winning train for a signal.
  if(full){*count=*sectionCount=0;if(selectedRoute)*annotationCount=0;return NIMBY_BUFFER_TOO_SMALL;}
  if(exhausted){*count=*sectionCount=0;if(selectedRoute)*annotationCount=0;return NIMBY_RESOURCE_LIMIT;}
  if(view->session!=currentSession()){*count=*sectionCount=0;if(selectedRoute)*annotationCount=0;return NIMBY_DATA_UNAVAILABLE;}
  return NIMBY_OK;
  }();
  return finish(status);
 }catch(...){nimby::detail::diagnostics::exception("sdk",__func__);if(count)*count=0;if(sectionCount)*sectionCount=0;if(selectedRoute&&annotationCount)*annotationCount=0;return finish(NIMBY_INTERNAL_ERROR);}
}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_CopySignalRoutes(const uint64_t* signals,uint32_t signalCount,
 uint32_t blocks,NimbySignalRouteObservation* out,uint32_t capacity,uint32_t* count,
 NimbySignalRouteSection* sections,uint32_t sectionCapacity,uint32_t* sectionCount) noexcept {
 return copySignalRoutes(signals,signalCount,blocks,out,capacity,count,sections,sectionCapacity,sectionCount,nullptr,0,nullptr,false);
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_CopySignalRoutesV2(const uint64_t* signals,uint32_t signalCount,
 uint32_t blocks,NimbySignalRouteObservation* out,uint32_t capacity,uint32_t* count,
 NimbySignalRouteSection* sections,uint32_t sectionCapacity,uint32_t* sectionCount,
 NimbySignalRouteAnnotation* annotations,uint32_t annotationCapacity,uint32_t* annotationCount) noexcept {
 return copySignalRoutes(signals,signalCount,blocks,out,capacity,count,sections,sectionCapacity,sectionCount,
                         annotations,annotationCapacity,annotationCount,true);
}
