#include <nimby/detail/diagnostics.hpp>
// Native mechanics only: national indications are translated to numeric rules by mods.
#include <engine/automatic_driving.h>
#include <engine/automatic_controller.h>
#include <engine/binary_identity.h>
#include <platform/windows/runtime/automatic_driving_status.h>
#include <platform/windows/runtime/physical_route.h>
#include <MinHook.h>
#include <windows.h>
#include <array>
#include <unordered_map>
#include <cstring>
namespace {
using namespace nimby::engine::automatic;
using Integrate=uintptr_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,double,double,double,double,double,int64_t,uintptr_t);
using Scan=uintptr_t(__fastcall*)(uintptr_t,uintptr_t);
using Check=uint8_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uint64_t,uint8_t);
using Occupancy=uint8_t(__fastcall*)(uintptr_t,uintptr_t,double,double);
using Step=uint32_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,uintptr_t);
Integrate nativeIntegrate{};Scan nativeScan{},nativePermissionRange{};Check nativeCheck{};Occupancy nativeOccupancy{},nativeReservation{};
Step nativeStep{};
SRWLOCK initialization=SRWLOCK_INIT,stateLock=SRWLOCK_INIT;
uintptr_t base{},session{};uint64_t expiry{};
volatile LONG active=0;
bool installed=false;
uint32_t drivingOptions=0;
nimby::automatic_status::Shared* telemetry{};
std::vector<NimbySignalDrivingRule> rules;
struct RuntimeTrain : Train {nimby::windows::automatic::PhysicalRoute physicalRoute;};
std::unordered_map<uint64_t,RuntimeTrain> trains;
std::unordered_map<uint64_t,NimbyTrainConstraint> constraints;
uint64_t constraintExpiry=0,constraintOwner=0;
bool readBytes(uintptr_t address,void* value,size_t size){SIZE_T got{};return address>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),value,size,&got)&&got==size;}
template<class T> bool read(uintptr_t address,T& value){return readBytes(address,&value,sizeof value);}
uintptr_t currentSession(){uintptr_t root{},sim{};return read(base+0xb81998,root)&&read(root+0x680,sim)?sim:0;}
struct Lock {Lock(){AcquireSRWLockExclusive(&stateLock);}~Lock(){ReleaseSRWLockExclusive(&stateLock);}};
// The worker's occupancy map is scoped to this native step, not the integrator's
// Network object. Never keep a worker context pointer between callbacks/threads.
// Qualified caller RVA 0x44b367 passes four pointers, extra mass and tick budget
// to 0x448710; that routine calls both scan and integrate. Its context +8 is
// the integration Network, +0x18 the simulation and +0x68 the occupation map.
struct StepContext {uintptr_t context=0,motion=0;};
thread_local StepContext currentStep;
struct StepScope {
 StepContext previous;
 StepScope(uintptr_t context,uintptr_t motion):previous(currentStep){currentStep={context,motion};}
 ~StepScope(){currentStep=previous;}
};
uint32_t __fastcall stepMotion(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget){
 StepScope scope(context,motion);
 return nativeStep(context,train,motion,service,mass,budget);
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
 unsigned ranges=0;
 nimby::windows::automatic::PhysicalRoute* route=nullptr;
};
thread_local PermissionQuery* permissionQuery=nullptr;
struct QueryScope {PermissionQuery* previous;QueryScope(PermissionQuery& q):previous(permissionQuery){permissionQuery=&q;}
 ~QueryScope(){permissionQuery=previous;}};
// Query the game's physical occupancy predicate with a private output byte.
// It excludes the current train using its motion ID, exactly as the game does.
// Neither reservation tables nor the game's permission byte are edited here.
double physicalPrefix(uintptr_t context,uintptr_t track,double from,double to,double length,bool& occupied) {
 std::array<uintptr_t,3> nativeContext{};
 if(!read(context,nativeContext)||!nativeContext[0]||!nativeContext[1])return -1;
 uint8_t clear=1;nativeContext[2]=reinterpret_cast<uintptr_t>(&clear);
 auto isClear=[&](double fraction){clear=1;
  return nativeOccupancy(reinterpret_cast<uintptr_t>(nativeContext.data()),track,from,from+(to-from)*fraction)!=0&&clear;
 };
 occupied=!isClear(1);
 return occupied?length*clearPrefix(isClear):length;
}
uint8_t __fastcall occupancy(uintptr_t context,uintptr_t track,double from,double to) {
 auto* q=permissionQuery;
 // Cross-track checks made by the native callback retain their exact refusal.
 if(q&&q->valid&&q->started&&q->allowOccupation&&q->occupationPending&&q->occupationContext==context&&
    q->track==track&&q->from==from&&q->to==to) {
  q->occupationPending=false;
  bool occupied=false;const double free=physicalPrefix(context,track,from,to,q->length,occupied);
  if(free>=0){if(occupied)q->free=std::min(q->free,q->covered+free);q->reservationPending=true;return 1;}
  q->valid=false;
 }
 return nativeOccupancy(context,track,from,to);
}
uint8_t __fastcall reservation(uintptr_t context,uintptr_t track,double from,double to) {
 auto* q=permissionQuery;
 // On-sight authority replaces exclusive block admission on the followed range.
 // Otherwise a leading train's reservation prevents departure even after the
 // required stop and a successful physical-clearance query. The native callback
 // checks this range first, then crossing tracks: consume the exception ONCE,
 // so even a repeated identical crossing query keeps its native refusal.
 if(q&&q->valid&&q->started&&q->allowOccupation&&q->reservationPending&&
    q->reservationContext==context&&q->track==track&&q->from==from&&q->to==to) {
  q->reservationPending=false;
  std::array<uintptr_t,3> copy{};uintptr_t motion{};
  if(read(context,copy)&&copy[0]&&read(copy[1],motion)&&motion==q->motion&&copy[2]) {
   // Preserve native lookup/locking and retain its decision for diagnostics.
   // Only the private result byte changes; no reservation, route, signal state,
   // train speed or pre-existing native refusal is erased.
   uint8_t clear=1;copy[2]=reinterpret_cast<uintptr_t>(&clear);
   const auto result=nativeReservation(reinterpret_cast<uintptr_t>(copy.data()),track,from,to);
   q->reservationReplaced|=!result||!clear;
   return 1;
  }
  q->valid=false;
 }
 return nativeReservation(context,track,from,to);
}
uintptr_t __fastcall permissionRange(uintptr_t context,uintptr_t range) {
 auto* q=permissionQuery;
 if(!q||!q->allowOccupation)return nativePermissionRange(context,range);
 uintptr_t signalPtr{},track{},occupationContext{},reservationContext{};uint64_t signal{};
 int kind{};double from{},to{},length{},metric{};
 if(++q->ranges>4096||!read(range+0x20,kind)||!read(range+0x28,signalPtr))q->valid=false;
 if(q->valid&&kind==6&&signalPtr&&read(signalPtr,signal)&&signal==q->signal){
  q->started=true;q->covered=0;q->free=200;
  return nativePermissionRange(context,range);
 }
 if(q->valid&&q->started) {
  if(!read(range,track)||!read(range+8,from)||!read(range+16,to)||!read(range+24,length)||
     !read(track+0x88,metric)||!read(context+24,occupationContext)||!read(context+32,reservationContext)||
     !std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1||
     !std::isfinite(length)||length<0||!std::isfinite(metric)||metric<=0||
     std::abs(length-std::abs(to-from)*metric)>.01)q->valid=false;
  else {
   if(q->route)q->route->append(track,from,to,length,q->covered,readBytes);
   q->track=track;q->from=from;q->to=to;q->length=length;q->occupationContext=occupationContext;
   q->reservationContext=reservationContext;q->occupationPending=true;q->reservationPending=false;
   const auto result=nativePermissionRange(context,range);
   // A range only contributes observed clearance if the expected physical and
   // reservation predicates actually ran. An early native exit proves nothing.
   if(q->occupationPending||q->reservationPending)q->valid=false;
   q->covered+=length;q->occupationContext=q->reservationContext=0;
   q->occupationPending=q->reservationPending=false;
   return result;
  }
 }
 return nativePermissionRange(context,range);
}
uintptr_t __fastcall scan(uintptr_t context,uintptr_t range){
 if(InterlockedCompareExchange(&active,0,0)){
  uintptr_t motionRef{},motion{},nativeContext{},simulation{},offsetRef{},signalPtr{};
  uint64_t id{},signal{};int kind{};double offset{},part{},head{};
  if(read(context+0x18,motionRef)&&read(motionRef,motion)&&read(motion,id)&&id>>48==5&&
     read(context+0x10,nativeContext)&&read(nativeContext+0x18,simulation)&&
     read(context,offsetRef)&&read(offsetRef,offset)&&read(range+0x18,part)&&read(range+0x20,kind)&&
     read(motion+0x3c0,head)&&std::isfinite(offset)&&offset>=0&&std::isfinite(part)&&part>=0&&std::isfinite(head)){
   Lock lock;
   if(simulation==session&&session!=0){
    if(trains.size()>=8192&&!trains.contains(id))trains.clear();
    auto& train=trains[id];
    if(train.motion!=motion||head+0.01<train.memory.lastHead){
     auto previous=train.controlled;const bool replaced=train.motion!=0;
     train={};train.motion=motion;
     if(replaced&&previous.instruction.revision){train.controlled=previous;train.controlled.completed=train.controlled.cancelled=true;}
    }
    if(GetTickCount64()<constraintExpiry){
     if(const auto command=constraints.find(id);command!=constraints.end())train.controlled.accept(command->second,head);
     else train.controlled={};
    }else train.controlled={};
    const bool restricted=train.memory.sight||(train.controlled.instruction.revision&&!train.controlled.completed&&train.controlled.instruction.mode==1);
    if(offset==0){
     train.ahead.clear();train.managed=false;train.view={head,0,200,GetTickCount64(),true};
     if(restricted)train.physicalRoute.begin(motion,id,head,readBytes);
     else train.physicalRoute.clear();
    }
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
    if(kind==6&&read(range+0x28,signalPtr)&&read(signalPtr,signal)&&signal>>48==8){
     int signalKind{};
     if(read(signalPtr+0x30,signalKind)&&signalKind==4&&train.ahead.size()<128){
      const double position=head+offset+part;
      if(train.ahead.empty()||train.ahead.back().signal!=signal)train.ahead.push_back({signal,position});
      if(rule(rules,signal))train.managed=true;
      if(telemetry)InterlockedIncrement64(&telemetry->scans);
     }
    }
   }
  }
 }
 return nativeScan(context,range);
}
uint8_t __fastcall check(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,uintptr_t train,uintptr_t motion,uint64_t signal,uint8_t lookahead){
 NimbySignalDrivingRule instruction{};bool managed=false,fresh=false,eligible=false;
 uint64_t id=0;uintptr_t observedSession=0;double head=0,source=0;
 if(InterlockedCompareExchange(&active,0,0)&&read(motion,id)&&read(motion+0x3c0,head)&&std::isfinite(head)) {
  Lock lock;
  if(session&&session==currentSession())if(const auto* current=rule(rules,signal)) {
   instruction=*current;managed=true;fresh=GetTickCount64()<expiry;observedSession=session;
   // Loaded waiting trains have no ahead-scan cache yet. Bind only to the
   // signal the native train is actually waiting at, never a lookahead request.
   uint64_t waitingSignal{};uint8_t driving{};double speed{};int64_t ticks{};
   const bool waiting=!lookahead&&id>>48==5&&read(motion+0x458,waitingSignal)&&waitingSignal==signal&&
    read(motion+0x4b0,driving)&&driving==1&&read(motion+0x3c8,speed)&&
    read(session+0x28,ticks)&&ticks>=0&&ticks<10000000000000LL;
   if(waiting&&fresh&&(instruction.flags&NIMBY_DRIVING_ON_SIGHT)) {
    if(trains.size()>=8192&&!trains.contains(id))trains.clear();
    auto& state=trains[id];
    if(state.motion!=motion){state={};state.motion=motion;}
    state.managed=true;
    if(std::none_of(state.ahead.begin(),state.ahead.end(),[&](const auto& row){return row.signal==signal;}))
     state.ahead.insert(state.ahead.begin(),{signal,head});
    if(observeWaitingStop(state.waiting,signal,head,speed,ticks)&&(instruction.flags&NIMBY_DRIVING_STOP_THEN_PROCEED))
     state.memory.stopped={signal,head};
   }
   const auto found=trains.find(id);
   if(found!=trains.end()&&found->second.motion==motion) {
    source=head;
    const auto at=std::find_if(found->second.ahead.begin(),found->second.ahead.end(),[&](const auto& row){return row.signal==signal;});
    if(at!=found->second.ahead.end())source=at->position;
    eligible=(!lookahead||at!=found->second.ahead.end())&&
     restrictedEntry(found->second.memory,instruction,head,{true,200,0},fresh);
   }
  }
 }
 PermissionQuery query;query.signal=signal;query.motion=motion;query.started=lookahead==0;query.allowOccupation=eligible;
 nimby::windows::automatic::PhysicalRoute entryRoute;
 if(eligible){entryRoute.begin(motion,id,source,readBytes);query.route=&entryRoute;}
 uint8_t native;
 // The mod owns restricted admission on the exact followed ranges. The game
 // still evaluates crossing occupation/reservations and controller ownership;
 // their refusal cannot be turned into approval by the final decision below.
 {QueryScope scope(query);native=nativeCheck(a,b,c,d,train,motion,signal,lookahead);}
 if(!managed)return native;
 Lock lock;
 const auto* current=rule(rules,signal);
 if(session!=observedSession||session!=currentSession()||!current||
    std::memcmp(current,&instruction,sizeof instruction)||GetTickCount64()>=expiry)return 0;
 if(instruction.flags&NIMBY_DRIVING_ON_SIGHT) {
  const bool granted=eligible&&native&&query.valid&&query.started&&std::isfinite(query.covered)&&std::min(query.free,query.covered)>5;
  const auto found=trains.find(id);
  if(found!=trains.end()&&found->second.motion==motion) {
   found->second.entrySignal=signal;
   found->second.entry={head,std::max(0.0,source-head)+query.covered,
    std::max(0.0,source-head)+query.free,GetTickCount64(),granted};
   if(granted&&!lookahead){
    found->second.boundary={Passage{{signal,source},{signal,source},instruction,{}}};
    found->second.physicalRoute=std::move(entryRoute);
   }
  }
  if(telemetry){InterlockedIncrement64(&telemetry->restrictedChecks);
   // Capture changes in the actual native decision, not every simulation tick.
   // Bit 128 records replacement of a longitudinal reservation refusal. It can
   // coexist with a final denial (obstacle, crossing or another native reason).
   const uint32_t state=(eligible?1u:0u)|(native?2u:0u)|(query.valid?4u:0u)|(query.started?8u:0u)|(granted?16u:0u)|(found!=trains.end()?32u:0u)|(lookahead?64u:0u)|(query.reservationReplaced?128u:0u);
   const auto n=telemetry->permissionCount;
   if(n<512&&(!n||telemetry->permissions[n-1].state!=state||telemetry->permissions[n-1].flags!=instruction.flags||telemetry->permissions[n-1].train!=id)) {
    auto& sample=telemetry->permissions[telemetry->permissionCount];
    sample.train=id;sample.signal=signal;sample.proof=found!=trains.end()?found->second.memory.stopped.signal:0;
    sample.flags=instruction.flags;sample.state=state;sample.ranges=query.ranges;
    sample.head=head;sample.source=source;sample.covered=query.covered;sample.free=query.free;
    InterlockedExchange(&sample.ready,1);InterlockedIncrement(&telemetry->permissionCount);
   }
   if(granted)InterlockedIncrement64(&telemetry->restrictedGranted);
   else InterlockedIncrement64(&telemetry->restrictedDenied);
  }
  return granted?1:0;
 }
 if(!fresh||(instruction.flags&NIMBY_DRIVING_STOP)) {
  if(telemetry){InterlockedIncrement64(&telemetry->stops);telemetry->lastSignal=signal;}return 0;
 }
 return native;
}
uintptr_t __fastcall integrate(uintptr_t result,uintptr_t network,uintptr_t path,uintptr_t presence,uintptr_t dynamics,
 double extraMass,double ceiling,double acceleration,double braking,double distance,double target,int64_t budget,uintptr_t checkState){
 double chosenCeiling=ceiling,chosenBraking=braking,chosenDistance=distance,chosenTarget=target;
 nimby::automatic_status::Sample* sample=nullptr;
 std::vector<Passage> encountered;
 uint64_t observedId=0;uintptr_t observedSession=0;double beforeHead=0;
 if(InterlockedCompareExchange(&active,0,0)&&dynamics>=8&&presence==dynamics+0x398&&budget>0){
  uint64_t id{};double head{};float material[8]{};uintptr_t config{};double brakeFactor{};
  if(read(dynamics-8,id)&&id>>48==5&&read(presence+0x20,head)&&read(dynamics+0x1c,material)&&
     read(network+0x10,config)&&read(config+0xd0,brakeFactor)&&std::isfinite(brakeFactor)&&brakeFactor>0&&
     std::isfinite(extraMass)&&extraMass>=0&&std::isfinite(material[6])&&material[6]>0&&
     std::isfinite(material[2])&&material[2]>0&&std::isfinite(head)&&
     std::isfinite(material[0])&&material[0]>=0&&std::isfinite(material[7])&&material[7]>=0){
   Lock lock;
   if(session&&session==currentSession()) {
    const bool fresh=GetTickCount64()<expiry;
    // This argument combines native timetable cruising and a possible script
    // ceiling. Remove only the former; the integrator still bounds speed by
    // track/material and brakes for its distance/target arguments unchanged.
    uint8_t targetActive{};float nativeTargetCeiling{};
    if(fresh&&(drivingOptions&NIMBY_DRIVING_MAXIMUM_LINE_SPEED)&&
       read(dynamics-8+0x4a0,targetActive)&&targetActive<=1&&
       read(dynamics-8+0x49c,nativeTargetCeiling))
     chosenCeiling=cruiseCeiling(ceiling,material[0],true,targetActive!=0,nativeTargetCeiling);
    if(GetTickCount64()<constraintExpiry)if(const auto command=constraints.find(id);command!=constraints.end()) {
     auto& state=trains[id];
     if(!state.motion)state.motion=dynamics-8;
     if(state.motion==dynamics-8)state.controlled.accept(command->second,head);
    }
    const auto found=trains.find(id);
    if(found!=trains.end()&&found->second.motion==dynamics-8) {
     auto& state=found->second;
     if(GetTickCount64()>=constraintExpiry||!constraints.contains(id))state.controlled={};
     if(state.memory.sight||(state.controlled.instruction.revision&&!state.controlled.completed&&state.controlled.instruction.mode==1)){
      // Geometry comes from the native scanner, but occupation is read NOW,
      // before each movement step. A long canton, a pause or accelerated time
      // must not turn the scanner's cadence into repeated artificial stops.
      // Reusing the geometry requires the same path, motion, full track IDs and
      // matching current head. No new route or free continuation is invented.
      uintptr_t map{},stepNetwork{},stepSession{};
      if(currentStep.motion==dynamics-8&&path==dynamics-8+0x290&&
         read(currentStep.context+8,stepNetwork)&&stepNetwork==network&&
         read(currentStep.context+0x18,stepSession)&&stepSession==session&&
         read(currentStep.context+0x68,map)&&map){
       uintptr_t motion=dynamics-8;uint8_t clear=1;
       std::array<uintptr_t,3> query{map,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&clear)};
       state.view=state.physicalRoute.refresh(motion,head,GetTickCount64(),readBytes,
        [&](uintptr_t track,double from,double to,double length){bool occupied=false;
         return physicalPrefix(reinterpret_cast<uintptr_t>(query.data()),track,from,to,length,occupied);});
      }else state.view={};
     }
     auto proposed=prepareIntegration(state,rules,head,material[7],material[0],material[2],
         material[6],extraMass,brakeFactor,fresh,GetTickCount64(),chosenCeiling,braking,distance,target);
     chosenCeiling=proposed.ceiling;chosenBraking=proposed.braking;
     chosenDistance=proposed.distance;chosenTarget=proposed.target;
     if(proposed.restricted&&telemetry)InterlockedIncrement64(&telemetry->restrictedSteps);
     if(proposed.observed){encountered=std::move(proposed.encountered);observedId=id;observedSession=session;beforeHead=head;}
     if(telemetry&&(chosenCeiling!=ceiling||chosenBraking!=braking)) {
      InterlockedIncrement64(&telemetry->applied);state.sampleBudget-=budget;
      if(state.sampleBudget<=0&&telemetry->count<4096){
       sample=&telemetry->samples[telemetry->count];InterlockedIncrement(&telemetry->count);
       sample->train=id;sample->headBefore=head;sample->ceiling=chosenCeiling;sample->nativeCeiling=ceiling;
       sample->braking=chosenBraking*brakeFactor;
       if(state.memory.sight){const auto view=state.view.at(head,GetTickCount64());sample->restrictedSource=state.memory.sight->source;
        sample->freeDistance=view.distanceM;sample->visibilityVerified=view.verified?1u:0u;}
       read(presence+0x28,sample->before);state.sampleBudget=1000;
      }
     }
    }
   }
  }
 }
 // Never hold stateLock across native integration: native permission callbacks
 // acquire it too. Commit memory only from the movement that actually happened.
 const auto returned=nativeIntegrate(result,network,path,presence,dynamics,extraMass,chosenCeiling,acceleration,chosenBraking,chosenDistance,chosenTarget,budget,checkState);
 if(observedId) {
  double afterHead{};
  if(read(presence+0x20,afterHead)) {
   Lock lock;const auto found=trains.find(observedId);
   if(session==observedSession&&session==currentSession()&&found!=trains.end()&&found->second.motion==dynamics-8)
   {
    double speed{};int64_t elapsed{};
    const bool readable=read(presence+0x28,speed)&&read(result+0x30,elapsed);
    commitIntegration(found->second,encountered,rules,beforeHead,afterHead,speed,elapsed,budget,readable,GetTickCount64()<expiry);
   }
  }
 }
 if(sample){read(presence+0x28,sample->after);read(presence+0x20,sample->headAfter);read(result+0x30,sample->ticks);InterlockedExchange(&sample->ready,1);}

 return returned;
}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_PublishV2(const NimbySignalDrivingRule* input,uint32_t count,uint32_t lease,uint32_t options) noexcept {
 try {
  std::vector<NimbySignalDrivingRule> next;
  if(!installed||!prepareRules(input,count,lease,options,next))return NIMBY_INVALID_ARGUMENT;
  Lock lock;const auto sim=currentSession();
  if(sim!=session){trains.clear();constraints.clear();constraintExpiry=0;}
  else if(!options&&next.empty()&&constraints.empty())trains.clear();
  session=sim;drivingOptions=options;rules=std::move(next);expiry=GetTickCount64()+lease;
  if(telemetry){telemetry->session=session;InterlockedExchange(&telemetry->ruleCount,static_cast<LONG>(rules.size()));}
  InterlockedExchange(&active,session&&(!rules.empty()||options||!constraints.empty()));return NIMBY_OK;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
 if(argument)return NIMBY_INVALID_ARGUMENT;
 AcquireSRWLockExclusive(&initialization);struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
 if(installed)return NIMBY_ALREADY_INITIALIZED;
 std::array<wchar_t,32768> path{};NimbyBinaryInfo identity{};
 if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
    nimby::engine::identify(path.data(),identity)!=NIMBY_OK||!identity.recognized_research_build)return NIMBY_INVALID_BINARY;
 base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
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
  {0x449700,{0x48,0x8b,0xc4,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x53,0x55,0x56,0x57,0x41},reinterpret_cast<void*>(&scan),reinterpret_cast<void**>(&nativeScan)},
  {0x4582d0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x50,0x4c},reinterpret_cast<void*>(&occupancy),reinterpret_cast<void**>(&nativeOccupancy)},
  {0x458460,{0x4c,0x8b,0xdc,0x53,0x48,0x81,0xec,0x90,0x00,0x00,0x00,0x41,0x0f,0x29,0x73,0xe8},reinterpret_cast<void*>(&reservation),reinterpret_cast<void**>(&nativeReservation)},
  {0x458560,{0x48,0x89,0x74,0x24,0x20,0x57,0x48,0x83,0xec,0x40,0x48,0x8b,0x01,0x48,0x8b,0xf9},reinterpret_cast<void*>(&permissionRange),reinterpret_cast<void**>(&nativePermissionRange)},
  {0x451b60,{0x40,0x55,0x53,0x56,0x57,0x48,0x8d,0x6c,0x24,0xe1,0x48,0x81,0xec,0xd8,0x00,0x00},reinterpret_cast<void*>(&check),reinterpret_cast<void**>(&nativeCheck)}
 };
 for(const auto& e:entries)if(std::memcmp(reinterpret_cast<void*>(base+e.rva),e.bytes.data(),16))return NIMBY_INVALID_BINARY;
 if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
 for(const auto& e:entries)if(MH_CreateHook(reinterpret_cast<void*>(base+e.rva),e.hook,e.original)!=MH_OK){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 HMODULE self{};
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&integrate),&self)){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 // No active rules until a successful explicit publication by the mod.
 if(MH_EnableHook(MH_ALL_HOOKS)!=MH_OK){MH_Uninitialize();return NIMBY_INTERNAL_ERROR;}
 installed=true;return NIMBY_OK;
}

extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_Publish(const NimbySignalDrivingRule* input,uint32_t count,uint32_t lease) noexcept {
 return NimbyDriving_PublishV2(input,count,lease,0);
}

// Replace a complete set atomically. Invalid batches leave the active set intact.
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_TrainConstraints(
 const NimbyTrainConstraint* input,uint32_t count,uint32_t lease,uint64_t publisher) noexcept {
 try {
  if(!publisher||!installed||count>8192||(!input&&count)||lease<100||lease>5000)return NIMBY_INVALID_ARGUMENT;
  std::unordered_map<uint64_t,NimbyTrainConstraint> next;
  for(uint32_t i=0;i<count;++i)if(!validConstraint(input[i])||!next.emplace(input[i].train,input[i]).second)return NIMBY_INVALID_ARGUMENT;
  Lock lock;const auto sim=currentSession();
  if(sim!=session){trains.clear();rules.clear();constraints.clear();constraintOwner=constraintExpiry=0;drivingOptions=0;session=sim;}
  if(!sim&&count)return NIMBY_DATA_UNAVAILABLE;
  if(constraintOwner!=publisher){
   if(!count)return NIMBY_OK;
   if(!constraints.empty()&&GetTickCount64()<constraintExpiry)return NIMBY_RESOURCE_LIMIT;
   for(auto& [id,state]:trains)state.controlled={};
  }
  constraintOwner=publisher;constraints=std::move(next);constraintExpiry=GetTickCount64()+lease;
  for(auto& [id,state]:trains)if(!constraints.contains(id))state.controlled={};
  InterlockedExchange(&active,session&&(!rules.empty()||drivingOptions||!constraints.empty()));return NIMBY_OK;
 }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_ReadTrainConstraint(uint64_t train,NimbyTrainConstraintStatus* out) noexcept {
 if(!out||out->size!=sizeof *out||train>>48!=5)return NIMBY_INVALID_ARGUMENT;
 *out={};out->size=sizeof *out;out->train=train;
 Lock lock;
 if(!session||session!=currentSession())return NIMBY_DATA_UNAVAILABLE;
 if(GetTickCount64()>=constraintExpiry||!constraints.contains(train))return NIMBY_OK;
 const auto& command=constraints.at(train);out->revision=command.revision;out->speed_mps=command.speed_mps;out->exit_signal=command.exit_signal;out->state=1;
 const auto state=trains.find(train);
 if(state!=trains.end()&&state->second.controlled.instruction.revision==command.revision){
  out->state=state->second.controlled.state();out->exit_signal=state->second.controlled.exitSignal;
 }
 return NIMBY_OK;
}
