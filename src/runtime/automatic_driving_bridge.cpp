// Native mechanics only: national indications are translated to numeric rules by mods.
#include <engine/automatic_driving.h>
#include <engine/binary_identity.h>
#include <runtime/automatic_driving_status.h>
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
Integrate nativeIntegrate{};Scan nativeScan{},nativePermissionRange{};Check nativeCheck{};Occupancy nativeOccupancy{};
SRWLOCK initialization=SRWLOCK_INIT,stateLock=SRWLOCK_INIT;
uintptr_t base{},session{};uint64_t expiry{};
volatile LONG active=0;
bool installed=false;
uint32_t drivingOptions=0;
nimby::automatic_status::Shared* telemetry{};
std::vector<NimbySignalDrivingRule> rules;
struct PhysicalView {
 double head=0,covered=0,free=200;
 uint64_t observed=0;
 bool valid=false;
 SightClearance at(double now)const {
  if(!valid||GetTickCount64()-observed>250||!std::isfinite(now)||now<head-1e-6)return {};
  return {true,std::max(0.0,std::min(free,covered)-(now-head)),0};
 }
};
struct Train {uintptr_t motion{};std::vector<Ahead> ahead;Memory memory;std::vector<Passage> boundary;bool managed=false;int64_t sampleBudget=0;PhysicalView view,entry;uint64_t entrySignal=0;uint32_t diagnosticState=~0u,diagnosticFlags=~0u;WaitingSample waiting;};
std::unordered_map<uint64_t,Train> trains;
template<class T> bool read(uintptr_t address,T& value){SIZE_T got{};return address>=0x10000&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),&value,sizeof value,&got)&&got==sizeof value;}
uintptr_t currentSession(){uintptr_t root{},sim{};return read(base+0xb81998,root)&&read(root+0x680,sim)?sim:0;}
struct Lock {Lock(){AcquireSRWLockExclusive(&stateLock);}~Lock(){ReleaseSRWLockExclusive(&stateLock);}};
// Permission traversal is synchronous on the calling simulation thread. This
// scope cannot leak to another train, a UI query, or a subsequent callback.
struct PermissionQuery {
 uint64_t signal{};
 bool started=false,valid=true,allowOccupation=false;
 double covered=0,free=200;
 uintptr_t occupationContext{},track{};
 double from=0,to=0,length=0;
 unsigned ranges=0;
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
 if(q&&q->valid&&q->started&&q->allowOccupation&&q->occupationContext==context&&
    q->track==track&&q->from==from&&q->to==to) {
  bool occupied=false;const double free=physicalPrefix(context,track,from,to,q->length,occupied);
  if(free>=0){if(occupied)q->free=std::min(q->free,q->covered+free);return 1;}
  q->valid=false;
 }
 return nativeOccupancy(context,track,from,to);
}
uintptr_t __fastcall permissionRange(uintptr_t context,uintptr_t range) {
 auto* q=permissionQuery;
 if(!q||!q->allowOccupation)return nativePermissionRange(context,range);
 uintptr_t signalPtr{},track{},occupationContext{};uint64_t signal{};
 int kind{};double from{},to{},length{},metric{};
 if(++q->ranges>4096||!read(range+0x20,kind)||!read(range+0x28,signalPtr))q->valid=false;
 if(q->valid&&kind==6&&signalPtr&&read(signalPtr,signal)&&signal==q->signal){
  q->started=true;q->covered=0;q->free=200;
  return nativePermissionRange(context,range);
 }
 if(q->valid&&q->started) {
  if(!read(range,track)||!read(range+8,from)||!read(range+16,to)||!read(range+24,length)||
     !read(track+0x88,metric)||!read(context+24,occupationContext)||
     !std::isfinite(from)||!std::isfinite(to)||from<0||from>1||to<0||to>1||
     !std::isfinite(length)||length<0||!std::isfinite(metric)||metric<=0||
     std::abs(length-std::abs(to-from)*metric)>.01)q->valid=false;
  else {
   q->track=track;q->from=from;q->to=to;q->length=length;q->occupationContext=occupationContext;
   const auto result=nativePermissionRange(context,range);
   q->covered+=length;q->occupationContext=0;
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
    if(train.motion!=motion||head+0.01<train.memory.lastHead){train={};train.motion=motion;}
    if(offset==0){train.ahead.clear();train.managed=false;train.view={head,0,200,GetTickCount64(),true};}
    // Only restricted trains need a second physical query. Ordinary BAL
    // continues using the normal native scan, without a full-world capture.
    if(train.memory.sight&&train.view.valid&&offset<200) {
     uintptr_t track{},map{};double from{},to{},metric{};
     if(std::abs(train.view.head-head)>1e-6||std::abs(train.view.covered-offset)>.01||
        !read(range,track)||!read(range+8,from)||!read(range+16,to)||!read(track+0x88,metric)||
        !read(nativeContext+0x68,map)||!std::isfinite(from)||!std::isfinite(to)||
        from<0||from>1||to<0||to>1||!std::isfinite(metric)||metric<=0||
        std::abs(part-std::abs(to-from)*metric)>.01)train.view.valid=false;
     else {
      uint8_t clear=1;std::array<uintptr_t,3> query{map,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&clear)};
      bool occupied=false;
      const double free=physicalPrefix(reinterpret_cast<uintptr_t>(query.data()),track,from,to,part,occupied);
      if(free<0)train.view.valid=false;
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
 PermissionQuery query;query.signal=signal;query.started=lookahead==0;query.allowOccupation=eligible;
 uint8_t native;
 // The game still evaluates ALL reservation and cross-track checks. Only the
 // physical occupation predicate on the exact followed range can be replaced,
 // and only for an explicit mod instruction plus the required measured stop.
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
   if(granted&&!lookahead)found->second.boundary={Passage{{signal,source},{signal,source},instruction,{}}};
  }
  if(telemetry){InterlockedIncrement64(&telemetry->restrictedChecks);
   // Capture changes in the actual native decision, not every simulation tick.
   // Bits: eligible, native approval, valid geometry, traversal started, granted.
   const uint32_t state=(eligible?1u:0u)|(native?2u:0u)|(query.valid?4u:0u)|(query.started?8u:0u)|(granted?16u:0u)|(found!=trains.end()?32u:0u)|(lookahead?64u:0u);
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
     std::isfinite(material[2])&&material[2]>0){
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
    const auto found=trains.find(id);
    if(found!=trains.end()&&found->second.motion==dynamics-8) {
     auto& state=found->second;
     if(state.managed||!state.memory.stops.empty()||!state.memory.held.empty()||!state.boundary.empty()||state.memory.sight) {
      const double physicalBraking=std::min(braking*brakeFactor,
          material[2]*0.8*material[6]/(material[6]+extraMass));
      const auto clearance=state.memory.sight?state.view.at(head):
       state.entrySignal==state.memory.stopped.signal?state.entry.at(head):SightClearance{};
      const auto proposed=plan(state.memory,state.ahead,rules,head,material[7],material[0],physicalBraking,fresh,clearance);
      if(state.memory.sight) {
       // Enforce a real stopping target as well as a speed envelope. A ceiling
       // alone would leave the native integrator's 0.01 m/s creeping floor.
       const double stopDistance=fresh&&clearance.verified?std::max(0.0,clearance.distanceM-5):0;
       if(!std::isfinite(chosenDistance)||chosenDistance<0||stopDistance<=chosenDistance){chosenDistance=stopDistance;chosenTarget=0;}
       if(telemetry)InterlockedIncrement64(&telemetry->restrictedSteps);
      }
      if(proposed.active){chosenCeiling=std::min(chosenCeiling,proposed.ceiling);chosenBraking=physicalBraking/brakeFactor;}
      if(fresh){encountered=passages(state.ahead,rules);appendBoundary(encountered,state.boundary,rules,head);}
      observedId=id;observedSession=session;beforeHead=head;
     }
     if(telemetry&&(chosenCeiling!=ceiling||chosenBraking!=braking)) {
      InterlockedIncrement64(&telemetry->applied);state.sampleBudget-=budget;
      if(state.sampleBudget<=0&&telemetry->count<4096){
       sample=&telemetry->samples[telemetry->count];InterlockedIncrement(&telemetry->count);
       sample->train=id;sample->headBefore=head;sample->ceiling=chosenCeiling;sample->nativeCeiling=ceiling;
       sample->braking=chosenBraking*brakeFactor;
       if(state.memory.sight){const auto view=state.view.at(head);sample->restrictedSource=state.memory.sight->source;
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
    crossed(found->second.memory,encountered,beforeHead,afterHead);
    found->second.boundary=atBoundary(encountered,afterHead);
    std::vector<Ahead> stoppedAhead=found->second.ahead;
    for(const auto& p:encountered)if(std::none_of(stoppedAhead.begin(),stoppedAhead.end(),[&](const auto& row){return row.signal==p.source.signal;}))stoppedAhead.push_back(p.source);
    double speed{};int64_t elapsed{};
    if(read(presence+0x28,speed)&&read(result+0x30,elapsed)&&elapsed>0&&elapsed<=budget) {
     // At a native waiting boundary the reported speed can retain its 0.01
     // floor. Zero displacement over simulated time is the stop evidence.
     const double measured=(afterHead==beforeHead&&speed>=0&&speed<=0.010001)?0:speed;
     observeStop(found->second.memory,stoppedAhead,rules,beforeHead,afterHead,measured,elapsed,GetTickCount64()<expiry);
    } else found->second.memory.stopped={};
   }
  }
 }
 if(sample){read(presence+0x28,sample->after);read(presence+0x20,sample->headAfter);read(result+0x30,sample->ticks);InterlockedExchange(&sample->ready,1);}

 return returned;
}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_PublishV2(const NimbySignalDrivingRule* input,uint32_t count,uint32_t lease,uint32_t options) noexcept {
 try {
  if(!installed||(options&~NIMBY_DRIVING_MAXIMUM_LINE_SPEED)||count>32768||(!input&&count)||lease<100||lease>5000)return NIMBY_INVALID_ARGUMENT;
  std::vector<NimbySignalDrivingRule> next;
  if(count)next.assign(input,input+count);
  for(const auto& r:next)if(!valid(r))return NIMBY_INVALID_ARGUMENT;
  std::sort(next.begin(),next.end(),[](const auto& a,const auto& b){return a.signal<b.signal;});
  for(size_t i=1;i<next.size();++i)if(next[i-1].signal==next[i].signal)return NIMBY_INVALID_ARGUMENT;
  Lock lock;const auto sim=currentSession();
  if(sim!=session||(!options&&next.empty()))trains.clear();
  session=sim;drivingOptions=options;rules=std::move(next);expiry=GetTickCount64()+lease;
  if(telemetry){telemetry->session=session;InterlockedExchange(&telemetry->ruleCount,static_cast<LONG>(rules.size()));}
  InterlockedExchange(&active,session&&(!rules.empty()||options));return NIMBY_OK;
 }catch(...){return NIMBY_INTERNAL_ERROR;}
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
  {0x378600,{0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x4c,0x24,0x08,0x55,0x56,0x57,0x41,0x55,0x41},reinterpret_cast<void*>(&integrate),reinterpret_cast<void**>(&nativeIntegrate)},
  {0x449700,{0x48,0x8b,0xc4,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x53,0x55,0x56,0x57,0x41},reinterpret_cast<void*>(&scan),reinterpret_cast<void**>(&nativeScan)},
  {0x4582d0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x50,0x4c},reinterpret_cast<void*>(&occupancy),reinterpret_cast<void**>(&nativeOccupancy)},
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
