#include <nimby/detail/diagnostics.hpp>
// Native mechanics only: national indications are translated to numeric rules by mods.
#include <engine/automatic_driving.h>
#include <engine/automatic_controller.h>
#include <engine/driving_publishers.h>
#include <engine/binary_identity.h>
#include <platform/windows/runtime/automatic_driving_status.h>
#include <platform/windows/runtime/physical_route.h>
#include <platform/windows/runtime/driving_state.h>
#include <platform/windows/bridge_installation.h>
#include <MinHook.h>
#include <windows.h>
#include <array>
#include <unordered_map>
#include <cstring>
#include <utility>
namespace {
using namespace nimby::engine::automatic;
using Integrate=uintptr_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,double,double,double,double,double,int64_t,uintptr_t);
using Scan=uintptr_t(__fastcall*)(uintptr_t,uintptr_t);
using Check=uint8_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uint64_t,uint8_t);
using Occupancy=uint8_t(__fastcall*)(uintptr_t,uintptr_t,double,double);
using Step=uint32_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,uintptr_t);
Integrate nativeIntegrate{};Scan nativeScan{},nativePermissionRange{};Check nativeCheck{};Occupancy nativeOccupancy{},nativeReservation{};
Step nativeStep{};
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
bool readBytes(uintptr_t address,void* value,size_t size){SIZE_T got{};return address>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),value,size,&got)&&got==size;}
template<class T> bool read(uintptr_t address,T& value){return readBytes(address,&value,sizeof value);}
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
void synchronizeTrain(RuntimeTrain& train,const Publication& value,uint64_t id,double head){
 pruneReleased(train,value);
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
struct StepContext {uintptr_t context=0,motion=0;};
thread_local StepContext currentStep;
struct StepScope {
 StepContext previous;
 StepScope(uintptr_t context,uintptr_t motion):previous(currentStep){currentStep={context,motion};}
 ~StepScope(){currentStep=previous;}
};
uint32_t __fastcall stepMotion(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget){
 StepScope scope(context,motion);
 Publications::View view(publications);uint64_t id{};
 if(view->active()&&view->session==currentSession()&&read(motion,id)&&id>>48==5){
  if(view->world&&(view->world->isManaged(id)||constraint(*view,id))){
   TrainAccess access(*view,id,motion);return nativeStep(context,train,motion,service,mass,budget);
  }
 }
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
 Publications::View snapshot(publications);const auto& publication=*snapshot;
 if(publication.active()){
  uintptr_t motionRef{},motion{},nativeContext{},simulation{},offsetRef{},signalPtr{};
  uint64_t id{},signal{};int kind{};double offset{},part{},head{};
  if(read(context+0x18,motionRef)&&read(motionRef,motion)&&read(motion,id)&&id>>48==5&&
     read(context+0x10,nativeContext)&&read(nativeContext+0x18,simulation)&&
     read(context,offsetRef)&&read(offsetRef,offset)&&read(range+0x18,part)&&read(range+0x20,kind)&&
     read(motion+0x3c0,head)&&std::isfinite(offset)&&offset>=0&&std::isfinite(part)&&part>=0&&std::isfinite(head)){
   const bool managedSource=kind==6&&read(range+0x28,signalPtr)&&read(signalPtr,signal)&&publication.rules.contains(signal);
   if(publication.world&&(managedSource||constraint(publication,id)))publication.world->markManaged(id);
   if(!publication.world||!publication.world->isManaged(id))return nativeScan(context,range);
   TrainAccess access(publication,id,motion);
   if(access.state&&simulation==publication.session&&publication.session==currentSession()){
    auto& train=*access.state;
    if(train.motion!=motion||head+0.01<train.memory.lastHead){
     auto previous=train.controlled;const bool replaced=train.motion!=0;const auto revision=train.routeRevision+1;
     train={};train.motion=motion;train.routeRevision=revision;
     if(replaced&&previous.instruction.revision){train.controlled=previous;train.controlled.completed=train.controlled.cancelled=true;}
    }
    synchronizeTrain(train,publication,id,head);
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
      if(rule(rules(publication),signal))train.managed=true;
      if(telemetry)InterlockedIncrement64(&telemetry->scans);
     }
    }
    rememberSources(train,publication);
   }else if(!access.state&&telemetry)InterlockedIncrement64(access.missing?&telemetry->missingTrainSlots:&telemetry->busyTrainScans);
  }
 }
 return nativeScan(context,range);
}
uint8_t __fastcall check(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d,uintptr_t train,uintptr_t motion,uint64_t signal,uint8_t lookahead){
 if(telemetry)InterlockedIncrement64(&telemetry->permissionCalls);
 Publications::View snapshot(publications);const auto& publication=*snapshot;
 const auto* current=publication.active()&&publication.session==currentSession()?rule(rules(publication),signal):nullptr;
 if(!current){PermissionQuery query;QueryScope scope(query);return nativeCheck(a,b,c,d,train,motion,signal,lookahead);}
 // Ownership is readable without touching another train's state. Only this
 // managed signal can fail closed when its OWN train is currently unavailable.
 uint64_t id{};double head{};
 if(!read(motion,id)||id>>48!=5||!read(motion+0x3c0,head)||!std::isfinite(head))return 0;
 if(publication.world)publication.world->markManaged(id);
 TrainAccess access(publication,id,motion);
 if(!access.state){if(telemetry)InterlockedIncrement64(access.missing?&telemetry->missingTrainSlots:&telemetry->busyTrainPermissions);return 0;}
 auto& state=*access.state;synchronizeTrain(state,publication,id,head);
 const auto instruction=*current;const auto observedLifetime=lifetime(publication.signalLifetimes,signal);
 const bool fresh=freshRule(publication,signal,GetTickCount64());
 uint64_t waitingSignal{};uint8_t driving{};double speed{};int64_t ticks{};
 const bool waiting=!lookahead&&read(motion+0x458,waitingSignal)&&waitingSignal==signal&&
  read(motion+0x4b0,driving)&&driving==1&&read(motion+0x3c8,speed)&&
  read(publication.session+0x28,ticks)&&ticks>=0&&ticks<10000000000000LL;
 if(waiting&&fresh&&(instruction.flags&NIMBY_DRIVING_ON_SIGHT)) {
  state.managed=true;
  if(std::none_of(state.ahead.begin(),state.ahead.end(),[&](const auto& row){return row.signal==signal;}))state.ahead.insert(state.ahead.begin(),{signal,head});
  if(observeWaitingStop(state.waiting,signal,head,speed,ticks)&&(instruction.flags&NIMBY_DRIVING_STOP_THEN_PROCEED))state.memory.stopped={signal,head};
 }
 double source=head;
 const auto at=std::find_if(state.ahead.begin(),state.ahead.end(),[&](const auto& row){return row.signal==signal;});
 if(at!=state.ahead.end())source=at->position;
 const bool eligible=(state.managed||!state.ahead.empty())&&(!lookahead||at!=state.ahead.end())&&
  restrictedEntry(state.memory,instruction,head,{true,200,0},fresh);
 PermissionQuery query;query.signal=signal;query.motion=motion;query.started=lookahead==0;query.allowOccupation=eligible;
 nimby::windows::automatic::PhysicalRoute entryRoute;
 if(eligible){entryRoute.begin(motion,id,source,readBytes);query.route=&entryRoute;}
 const auto routeRevision=state.routeRevision;
 uint8_t native;
 {QueryScope scope(query);native=nativeCheck(a,b,c,d,train,motion,signal,lookahead);}
 // The publication may change during a native predicate. A removed/re-added
 // source is a new lifetime even when the same owner reuses identical bytes.
 Publications::View latest(publications);current=rule(rules(*latest),signal);
 uint64_t confirmedId{};double confirmedHead{};
 if(latest->world!=publication.world||latest->session!=currentSession()||state.routeRevision!=routeRevision||
    !read(motion,confirmedId)||confirmedId!=id||!read(motion+0x3c0,confirmedHead)||confirmedHead!=head||!current||
    lifetime(latest->signalLifetimes,signal)!=observedLifetime||
    std::memcmp(current,&instruction,sizeof instruction)||!freshRule(*latest,signal,GetTickCount64()))return 0;
 pruneReleased(state,*latest);
 if(instruction.flags&NIMBY_DRIVING_ON_SIGHT) {
  const bool granted=eligible&&native&&query.valid&&query.started&&std::isfinite(query.covered)&&std::min(query.free,query.covered)>5;
  state.entrySignal=signal;
  state.entry={head,std::max(0.0,source-head)+query.covered,std::max(0.0,source-head)+query.free,GetTickCount64(),granted};
  if(granted&&!lookahead){state.boundary={Passage{{signal,source},{signal,source},instruction,{}}};state.physicalRoute=std::move(entryRoute);}
  if(telemetry){
   InterlockedIncrement64(&telemetry->restrictedChecks);
   const auto n=reserveSample(telemetry->permissionCount,512);
   if(n>=0){auto& sample=telemetry->permissions[n];sample.observedAtMs=GetTickCount64();
    sample.train=id;sample.signal=signal;sample.proof=state.memory.stopped.signal;
    sample.flags=instruction.flags;sample.state=(eligible?1u:0u)|(native?2u:0u)|(query.valid?4u:0u)|(query.started?8u:0u)|(granted?16u:0u)|32u|(lookahead?64u:0u)|(query.reservationReplaced?128u:0u);
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
uintptr_t __fastcall integrate(uintptr_t result,uintptr_t network,uintptr_t path,uintptr_t presence,uintptr_t dynamics,
 double extraMass,double ceiling,double acceleration,double braking,double distance,double target,int64_t budget,uintptr_t checkState){
 if(telemetry)InterlockedIncrement64(&telemetry->integrationCalls);
 double chosenCeiling=ceiling,chosenBraking=braking,chosenDistance=distance,chosenTarget=target;
 const auto invokeNative=[&]{return nativeIntegrate(result,network,path,presence,dynamics,extraMass,chosenCeiling,acceleration,chosenBraking,chosenDistance,chosenTarget,budget,checkState);};
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
 synchronizeTrain(state,publication,id,head);
 const auto now=GetTickCount64();const bool fresh=freshTrain(publication,state,head,now);
 applyCruise(&state);
 if(state.memory.sight||(state.controlled.instruction.revision&&!state.controlled.completed&&state.controlled.instruction.mode==1)){
  uintptr_t map{},stepNetwork{},stepSession{};
  if(currentStep.motion==motion&&path==motion+0x290&&read(currentStep.context+8,stepNetwork)&&stepNetwork==network&&
     read(currentStep.context+0x18,stepSession)&&stepSession==publication.session&&read(currentStep.context+0x68,map)&&map){
   uintptr_t queryMotion=motion;uint8_t clear=1;
   std::array<uintptr_t,3> query{map,reinterpret_cast<uintptr_t>(&queryMotion),reinterpret_cast<uintptr_t>(&clear)};
   state.view=state.physicalRoute.refresh(motion,head,now,readBytes,
    [&](uintptr_t track,double from,double to,double length){bool occupied=false;
     return physicalPrefix(reinterpret_cast<uintptr_t>(query.data()),track,from,to,length,occupied);});
  }else state.view={};
 }
 auto proposed=prepareIntegration(state,rules(publication),head,material[7],material[0],material[2],material[6],extraMass,brakeFactor,fresh,now,chosenCeiling,braking,distance,target);
 chosenCeiling=proposed.ceiling;chosenBraking=proposed.braking;chosenDistance=proposed.distance;chosenTarget=proposed.target;
 if(proposed.restricted&&telemetry)InterlockedIncrement64(&telemetry->restrictedSteps);
 // Reuse the publisher field as a PRIVATE lifetime token. It never crosses the
 // public ABI. A release/re-add of the same owner must invalidate old passages.
 for(auto& passage:proposed.encountered)passage.publisher=lifetime(publication.signalLifetimes,passage.source.signal);
 rememberSources(state,publication);
 const auto routeRevision=state.routeRevision;
 const bool changed=chosenCeiling!=ceiling||chosenBraking!=braking||chosenDistance!=distance||chosenTarget!=target;
 if(changed&&telemetry)InterlockedIncrement64(&telemetry->applied);
 const auto returned=invokeNative();
 // access remains held across native integration. Nested callbacks on this
 // thread borrowed it, so there is no second lock that can lose a NEW crossing.
 if(proposed.observed){
  Publications::View latest(publications);double afterHead{};uint64_t afterId{};
  if(latest->world==publication.world&&latest->session==currentSession()&&state.routeRevision==routeRevision&&read(motion,afterId)&&afterId==id&&read(presence+0x20,afterHead)){
   pruneReleased(state,*latest);
   std::erase_if(proposed.encountered,[&](const auto& passage){return !passage.publisher||lifetime(latest->signalLifetimes,passage.source.signal)!=passage.publisher;});
   double speed{};int64_t elapsed{};
   const bool readable=read(presence+0x28,speed)&&read(result+0x30,elapsed);
   commitIntegration(state,proposed.encountered,rules(*latest),head,afterHead,speed,elapsed,budget,readable,freshTrain(*latest,state,afterHead,GetTickCount64()));
   rememberSources(state,*latest);
   if(telemetry)InterlockedIncrement64(&telemetry->committedIntegrations);
  }else if(telemetry)InterlockedIncrement64(&telemetry->discardedCommits);
 }
 if(changed){state.sampleBudget-=budget;if(state.sampleBudget<=0){
  recordIntegration(id,head,beforeSpeed,presence,result,ceiling,braking,distance,target,chosenCeiling,chosenBraking,chosenDistance,chosenTarget,fresh?0u:3u,&state);
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
