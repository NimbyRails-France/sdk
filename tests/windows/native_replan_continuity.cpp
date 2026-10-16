// Actual 440840 wrapper, journal and lineage with synthetic native records.
// This fixture never installs hooks or proves runtime ABI/phase ownership.
#include <windows.h>
#include <cstdint>
uintptr_t replanFixtureCaller();
BOOL WINAPI fixtureReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
#define NRF_PHASE_CALLER() replanFixtureCaller()
#define ReadProcessMemory fixtureReadProcessMemory
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#undef ReadProcessMemory

#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <cstdlib>
#include <new>

namespace {
size_t checks{},calls{},rpmCalls{},callerReads{},allocations{};
bool watchAllocations{},failRead{};
uintptr_t failAddress{};
thread_local uintptr_t fixtureCaller{};
thread_local std::array<uintptr_t,8> expected{};
thread_local DWORD expectedIncoming=0x993,returnedError=0x994;
 thread_local bool throwOriginal{},recurse{},nestedForeign{},nativeRefused{};
thread_local uint64_t originalExpectedGlobal{};
thread_local mutation_continuity::Reason originalExpectedReason=mutation_continuity::Reason::Unarmed;
mutation_continuity::Stamp probeStamp;
constexpr uint64_t fullId(uint32_t index,uint16_t generation=1){return(uint64_t(5)<<48)|(uint64_t(index)<<16)|generation;}
constexpr uintptr_t returnedValue=0x6df135792468ace0ULL;
struct NativeException {int code=733;};
#define CHECK(x) do{++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
}
uintptr_t replanFixtureCaller(){++callerReads;return fixtureCaller;}
BOOL WINAPI fixtureReadProcessMemory(HANDLE process,LPCVOID address,LPVOID output,SIZE_T size,SIZE_T* copied){
 ++rpmCalls;
 if(failRead&&(failAddress==0||reinterpret_cast<uintptr_t>(address)==failAddress)){
  if(copied)*copied=0;
  SetLastError(ERROR_PARTIAL_COPY);return FALSE;
 }
 return ::ReadProcessMemory(process,address,output,size,copied);
}
#if defined(__GNUC__)
#define TEST_NOINLINE __attribute__((noinline))
#else
#define TEST_NOINLINE __declspec(noinline)
#endif
TEST_NOINLINE void* operator new(size_t n){if(watchAllocations)++allocations;if(auto*p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
TEST_NOINLINE void* operator new[](size_t n){return ::operator new(n);}
TEST_NOINLINE void operator delete(void*p)noexcept{std::free(p);}
TEST_NOINLINE void operator delete[](void*p)noexcept{std::free(p);}
TEST_NOINLINE void operator delete(void*p,size_t)noexcept{std::free(p);}
TEST_NOINLINE void operator delete[](void*p,size_t)noexcept{std::free(p);}
namespace {
uintptr_t __fastcall originalReplan(uintptr_t out,uintptr_t db,uintptr_t sim,uintptr_t reservations,
 uintptr_t train,uintptr_t motion,uintptr_t presence,uintptr_t service){
 ++calls;CHECK((std::array<uintptr_t,8>{out,db,sim,reservations,train,motion,presence,service}==expected));
 CHECK(GetLastError()==expectedIncoming);
 if(originalExpectedGlobal)CHECK(mutationContinuity.counters().globalMutation!=originalExpectedGlobal);
 if(probeStamp)CHECK(mutationContinuity.unchanged(probeStamp).reason==originalExpectedReason);
 // Native bytes differ on refusal/exception as well: the wrapper may not undo
 // or reinterpret them. Returning an intentionally different pointer checks
 // full result forwarding independently of the pinned function's arg1 return.
 auto* bytes=reinterpret_cast<unsigned char*>(out);
 for(size_t i=0;i<24;++i)bytes[i]=static_cast<unsigned char>(i+0x41);
 bytes[4]=nativeRefused?1:0;
 // Model the actual writer's direct Motion destination, independent of the
 // DB/Sim args. The prebootstrap opaque test deliberately uses a small token.
 if(motion>=0x10000)++*reinterpret_cast<uint64_t*>(motion+8);
 if(recurse){
  recurse=false;const auto saved=expected;const auto beforeReason=originalExpectedReason;
  const auto beforeGlobal=originalExpectedGlobal;originalExpectedGlobal=0;
  if(nestedForeign){expected[1]+=16;expected[2]+=16;}
  originalExpectedReason=mutation_continuity::Reason::Lost;
  SetLastError(expectedIncoming);
  const auto value=replanRoute440840(expected[0],expected[1],expected[2],expected[3],expected[4],expected[5],expected[6],expected[7]);
  CHECK(value==returnedValue);CHECK(GetLastError()==returnedError);
  expected=saved;originalExpectedReason=beforeReason;originalExpectedGlobal=beforeGlobal;
 }
 SetLastError(returnedError);
 if(throwOriginal)throw NativeException{};
 return returnedValue;
}
template<class Buffer,class T>void put(Buffer& buffer,size_t offset,T value){std::memcpy(buffer.data()+offset,&value,sizeof value);}
struct Fixture {
 std::unique_ptr<unsigned char[]> image{std::make_unique<unsigned char[]>(0xb819a0)};
 alignas(16) static inline std::array<unsigned char,0xb40> root{};
 alignas(16) static inline std::array<unsigned char,32> database{},sim{},foreignDb{},foreignSim{};
 alignas(16) std::array<unsigned char,32> train{},motion{},output{},presence{},service{},reservations{};
 std::array<nimby::windows::consist_inventory::Row,1> rows{};
 std::array<nimby::windows::owned_rear::provider::PhysicalCar,1> cars{};
 world_lineage::Capture capture;
 DWORD runner=GetCurrentThreadId();bool lockHeld=true;
 unsigned ownershipCalls{},invokeDuringOwnershipCall{};
 Fixture(){
  root.fill(0);base=reinterpret_cast<uintptr_t>(image.get());
  const auto rootAddress=reinterpret_cast<uintptr_t>(root.data());
  std::memcpy(image.get()+0xb81998,&rootAddress,8);
  const auto controller=rootAddress+0x540;
  put(root,0xb10,controller);put(root,0xb28,runner);
  put(root,0x540,reinterpret_cast<uintptr_t>(database.data()));put(root,0x680,reinterpret_cast<uintptr_t>(sim.data()));
  InitializeSRWLock(lock());AcquireSRWLockExclusive(lock());
  world_lineage::Scope scope{controller,controller+0x5d0,controller+0x4c0,
   reinterpret_cast<uintptr_t>(database.data()),reinterpret_cast<uintptr_t>(sim.data()),runner};
  scope.boundary=world_lineage::Boundary::PostSharedExclusive;scope.releaseReturn=0x349fa3;
  scope.originalReleaseCompleted=scope.exclusiveTrySucceeded=true;
  scope.lifecycleHooksActive=scope.currentRootVerified=true;scope.nativeRunnerThread=runner;
  nativeWorld.apply([&](auto& line,auto& clock){
   const auto stop=line.beginStop(clock,controller,controller+0x5d0);CHECK(stop);CHECK(line.finishStop(stop));
   const auto start=line.beginStart(clock,controller,controller+0x5d0);CHECK(start);CHECK(line.finishStart(start));
   CHECK(line.runnerEnter(controller,controller+0x5d0,runner));capture=line.capturePhase(clock,scope).capture;CHECK(capture);
  });
  const auto id=fullId(1);put(train,0,id);put(motion,0,id);rows[0].train=id;rows[0].active=1;
  cars[0].setup.owner=id;cars[0].setup.model=11;cars[0].nativeMeters=20;cars[0].meters=20;
  CHECK(mutationContinuity.bind(ownership()).reason==mutation_continuity::Reason::Ready);
  probeStamp=observe().stamp;CHECK(probeStamp);
  nativeWorld.apply([&](auto& line,auto&){CHECK(line.endCapture(capture));});
  ReleaseSRWLockExclusive(lock());lockHeld=false;
  nativeReplan440840=originalReplan;phaseObservationEnabled=true;
 }
 ~Fixture(){if(lockHeld)ReleaseSRWLockExclusive(lock());phaseObservationEnabled=false;fixtureCaller=0;probeStamp={};
  throwOriginal=recurse=nestedForeign=nativeRefused=false;failRead=false;failAddress=0;}
 PSRWLOCK lock(){return reinterpret_cast<PSRWLOCK>(root.data()+0xa00);}
 nimby::windows::owned_rear::Ownership ownership(){return{capture,
  [](void* context,const world_lineage::Capture& c)noexcept{
   auto& f=*static_cast<Fixture*>(context);++f.ownershipCalls;
   if(f.invokeDuringOwnershipCall==f.ownershipCalls){f.args();f.invoke();}
   return nativeWorld.current(c);
  },[](void*)noexcept->uint64_t{return GetCurrentThreadId();},this};}
 mutation_continuity::Result observe(){
  nimby::windows::consist_inventory::Result result;result.status=nimby::windows::consist_inventory::Status::ObservedAllSlots;
  result.rows=1;result.relationshipsIndexed=result.ownedPublication=true;
  nimby::windows::owned_rear::Roster roster{capture,result,rows};
  nimby::windows::owned_rear::provider::PublicationProof proof{
   {capture.world,capture.phase,capture.phase,nimby::engine::native_rear::Phase::RetainedMovement},
   rows[0].train,capture.phase,nimby::windows::owned_rear::provider::Kind::CurrentPhysicalSnapshot,true,false};
  nimby::windows::owned_rear::Body body{proof,cars,{},{}};mutation_continuity::Budget budget;
  return mutationContinuity.observe(ownership(),roster,body,budget);
 }
 void reopen(){
  CHECK(TryAcquireSRWLockExclusive(lock()));lockHeld=true;
  world_lineage::Scope scope{capture.controller,capture.context,capture.controller+0x4c0,capture.database,capture.simulation,runner};
  scope.boundary=world_lineage::Boundary::PostSharedExclusive;scope.releaseReturn=0x349fa3;
  scope.originalReleaseCompleted=scope.exclusiveTrySucceeded=true;
  nativeWorld.apply([&](auto& line,auto& clock){capture=line.capturePhase(clock,scope).capture;CHECK(capture);});
 }
 void args(uintptr_t rva=0x447c26,bool foreign=false){
  expected={reinterpret_cast<uintptr_t>(output.data()),reinterpret_cast<uintptr_t>((foreign?foreignDb:database).data()),
   reinterpret_cast<uintptr_t>((foreign?foreignSim:sim).data()),reinterpret_cast<uintptr_t>(reservations.data()),
   reinterpret_cast<uintptr_t>(train.data()),reinterpret_cast<uintptr_t>(motion.data()),
   reinterpret_cast<uintptr_t>(presence.data()),reinterpret_cast<uintptr_t>(service.data())};
  fixtureCaller=base+rva;
 }
 uintptr_t invoke(){SetLastError(expectedIncoming);return replanRoute440840(expected[0],expected[1],expected[2],expected[3],expected[4],expected[5],expected[6],expected[7]);}
 void call(uintptr_t rva=0x447c26,bool foreign=false){args(rva,foreign);CHECK(invoke()==returnedValue);CHECK(GetLastError()==returnedError);
  CHECK(output[4]==(nativeRefused?1:0));for(size_t i=0;i<24;++i)if(i!=4)CHECK(output[i]==i+0x41);}
};
}
int main(){try{
 static_assert(!mutation_continuity::Result::nativeWritersComplete&&!mutation_continuity::Result::progressionQualified&&
               !mutation_continuity::Result::leaseReleaseQualified);
 // Before any world: no historical stamp is manufactured or native read needed.
 {
  alignas(16)std::array<unsigned char,32>out{};expected={reinterpret_cast<uintptr_t>(out.data()),1,2,3,4,5,6,7};
  nativeReplan440840=originalReplan;phaseObservationEnabled=true;const auto reads=rpmCalls;
  SetLastError(expectedIncoming);CHECK(replanRoute440840(expected[0],1,2,3,4,5,6,7)==returnedValue);
  CHECK(GetLastError()==returnedError&&rpmCalls==reads&&replanDepth==0);phaseObservationEnabled=false;
 }
 {
  Fixture f;const auto before=mutationContinuity.counters();const auto reads=rpmCalls,callers=callerReads;
  phaseObservationEnabled=false;originalExpectedGlobal=0;originalExpectedReason=mutation_continuity::Reason::Ready;
  f.call();CHECK(rpmCalls==reads&&callerReads==callers);CHECK(mutationContinuity.counters().globalMutation==before.globalMutation);
 }
 for(auto caller:{uintptr_t(0x447c26),uintptr_t(0x448174)}){
  Fixture f;const auto stamp=probeStamp;originalExpectedGlobal=stamp.globalMutation;
  originalExpectedReason=mutation_continuity::Reason::MutationObserved;const auto reads=rpmCalls,nativeBefore=calls;
  f.call(caller);CHECK(calls==nativeBefore+1&&rpmCalls==reads+7);
  CHECK(mutationContinuity.unchanged(stamp).reason==mutation_continuity::Reason::MutationObserved);
  // Normal return does not restore the old stamp; a later actual owned current
  // observation gets a NEW serial even though all synthetic body bytes match.
  f.reopen();CHECK(mutationContinuity.bind(f.ownership()).reason==mutation_continuity::Reason::Ready);
  const auto after=f.observe();CHECK(after.reason==mutation_continuity::Reason::Ready);
  CHECK(after.stamp.globalMutation!=stamp.globalMutation&&after.stamp.consistRevision==stamp.consistRevision);
 }
 {
  Fixture f;nativeRefused=true;originalExpectedGlobal=probeStamp.globalMutation;
  originalExpectedReason=mutation_continuity::Reason::MutationObserved;f.call();
  CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::MutationObserved);
 }
 {
  // Foreign DB/Sim + current DBTrain/Motion is a mixed destination, not Foreign.
  Fixture f;const auto stamp=probeStamp;originalExpectedGlobal=stamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::Lost;
  const auto reads=rpmCalls;f.call(0x4f3a6f,true);CHECK(rpmCalls==reads+5);
  CHECK(mutationContinuity.unchanged(stamp).reason==mutation_continuity::Reason::Lost);
  uint64_t effects{};std::memcpy(&effects,f.motion.data()+8,8);CHECK(effects==1);
 }
 {
  // Even distinct destination buffers/IDs cannot prove foreign world ownership
  // or exclusion of a mixed reservationBase. The bounded overlay stays Lost.
  Fixture f;alignas(16)std::array<unsigned char,32>foreignTrain{},foreignMotion{};
  put(foreignTrain,0,fullId(700));put(foreignMotion,0,fullId(700));
  originalExpectedGlobal=probeStamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::Lost;
  f.args(0x447c26,true);expected[4]=reinterpret_cast<uintptr_t>(foreignTrain.data());
  expected[5]=reinterpret_cast<uintptr_t>(foreignMotion.data());
  const auto before=calls;CHECK(f.invoke()==returnedValue);CHECK(calls==before+1&&GetLastError()==returnedError);
  CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::Lost);
  uint64_t effects{};std::memcpy(&effects,foreignMotion.data()+8,8);CHECK(effects==1);
 }
 for(bool disabled:{false,true}){
  Fixture f;originalExpectedGlobal=disabled?0:probeStamp.globalMutation;
  originalExpectedReason=disabled?mutation_continuity::Reason::Ready:mutation_continuity::Reason::Lost;
  phaseObservationEnabled=!disabled;throwOriginal=true;
  const auto nativeBefore=calls,reads=rpmCalls,callers=callerReads;bool threw=false;
  try{f.call(0x4f3a6f,!disabled);}catch(const NativeException& error){threw=error.code==733;}
  CHECK(threw&&calls==nativeBefore+1&&rpmCalls==reads+(disabled?0:5));
  CHECK(callerReads==callers+(disabled?0:1));
  CHECK(GetLastError()==returnedError&&f.output[0]==0x41);
  CHECK(mutationContinuity.unchanged(probeStamp).reason==(disabled?mutation_continuity::Reason::Ready:mutation_continuity::Reason::Lost));
 }
 {
  // A reused ordinal with a NEW full generation invalidates the old record,
  // even when DBTrain and Motion agree on their new ID.
  Fixture f;put(f.train,0,fullId(1,2));put(f.motion,0,fullId(1,2));
  originalExpectedGlobal=probeStamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::MutationObserved;
  f.call();CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::MutationObserved);
 }
 // The replan executes on an actual different worker. The native Root runner
 // remains the thread that owns the previously captured publication.
 {
  Fixture f;DWORD worker{};const auto stamp=probeStamp;
  std::exception_ptr failure;
  std::thread task([&]{try{worker=GetCurrentThreadId();originalExpectedGlobal=stamp.globalMutation;
   originalExpectedReason=mutation_continuity::Reason::MutationObserved;f.call();}catch(...){failure=std::current_exception();}});
  task.join();if(failure)std::rethrow_exception(failure);CHECK(worker!=f.runner);
  CHECK(mutationContinuity.unchanged(stamp).reason==mutation_continuity::Reason::MutationObserved);
 }
 for(unsigned invalid=0;invalid<12;++invalid){
  Fixture f;const auto stamp=probeStamp;originalExpectedGlobal=stamp.globalMutation;
  originalExpectedReason=mutation_continuity::Reason::Lost;
  if(invalid==0)put(f.motion,0,fullId(1,2));
  if(invalid==1)put(f.train,0,uint64_t{8}<<48);
  if(invalid==2)put(f.root,0xb10,uintptr_t{});
  if(invalid==3)put(f.root,0xb28,DWORD{f.runner+1});
  if(invalid==4)put(f.root,0x680,uintptr_t{});
  if(invalid==5){failRead=true;failAddress=reinterpret_cast<uintptr_t>(f.motion.data());}
  if(invalid==6){failRead=true;failAddress=base+0xb81998;}
  if(invalid==7)nativeWorld.lose();
  f.args(invalid==8?0x447c21:invalid==9?0x44816f:0x447c26);
  if(invalid==10)expected[1]=reinterpret_cast<uintptr_t>(f.foreignDb.data());
  if(invalid==11)expected[2]=reinterpret_cast<uintptr_t>(f.foreignSim.data());
  const auto nativeBefore=calls;CHECK(f.invoke()==returnedValue);CHECK(calls==nativeBefore+1);CHECK(GetLastError()==returnedError);
  CHECK(mutationContinuity.unchanged(stamp).reason==mutation_continuity::Reason::Lost);
  // Restoring every byte/caller/read never heals the Lost state.
  put(f.motion,0,fullId(1));put(f.train,0,fullId(1));put(f.root,0xb10,f.capture.controller);
  put(f.root,0xb28,f.runner);put(f.root,0x680,reinterpret_cast<uintptr_t>(f.sim.data()));
  failRead=false;failAddress=0;originalExpectedGlobal=0;f.call();
  CHECK(mutationContinuity.unchanged(stamp).reason==mutation_continuity::Reason::Lost);
 }
 {
  Fixture f;originalExpectedGlobal=probeStamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::MutationObserved;
  throwOriginal=true;bool threw=false;const auto nativeBefore=calls,allocBefore=allocations;
  watchAllocations=true;
  try{f.call();}catch(const NativeException& error){threw=error.code==733;}
  watchAllocations=false;CHECK(allocations==allocBefore);
  CHECK(threw&&calls==nativeBefore+1);CHECK(GetLastError()==returnedError&&f.output[0]==0x41);
  CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::Lost);throwOriginal=false;
  originalExpectedGlobal=0;originalExpectedReason=mutation_continuity::Reason::Lost;f.call();
  CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::Lost);
 }
 for(bool foreign:{false,true}){
  Fixture f;originalExpectedGlobal=probeStamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::MutationObserved;
  recurse=true;nestedForeign=foreign;const auto nativeBefore=calls;f.call();CHECK(calls==nativeBefore+2);
  CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::Lost);
 }
 {
  // Contention is NEVER mistaken for an empty/unarmed journal. Trigger this
  // native writer in the final ownership callback while observe holds Gate.
  Fixture f;f.reopen();CHECK(mutationContinuity.bind(f.ownership()).reason==mutation_continuity::Reason::Ready);
  f.ownershipCalls=0;f.invokeDuringOwnershipCall=2;
  originalExpectedGlobal=0;originalExpectedReason=mutation_continuity::Reason::Busy;
  const auto nativeBefore=calls,reads=rpmCalls;
  CHECK(f.observe().reason==mutation_continuity::Reason::OwnershipUnknown);
  CHECK(calls==nativeBefore+1&&rpmCalls==reads);
  CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::Lost);
 }
 {
  Fixture f;CHECK(!nativeWorld.gate.test_and_set(std::memory_order_acquire));
  originalExpectedGlobal=probeStamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::Lost;
  f.call();nativeWorld.gate.clear(std::memory_order_release);
  originalExpectedGlobal=0;f.call();CHECK(mutationContinuity.unchanged(probeStamp).reason==mutation_continuity::Reason::Lost);
 }
 {
  Fixture f;f.args();originalExpectedGlobal=probeStamp.globalMutation;originalExpectedReason=mutation_continuity::Reason::MutationObserved;
  const auto nativeBefore=calls,reads=rpmCalls;const auto allocBefore=allocations;
  watchAllocations=true;for(unsigned i=0;i<1000;++i)CHECK(f.invoke()==returnedValue);watchAllocations=false;
  CHECK(calls==nativeBefore+1000&&rpmCalls==reads+7000&&allocations==allocBefore&&replanDepth==0);
 }
 std::cout<<checks<<" actual 440840 wrapper checks PASS; no installed hook, no progression/release proof\n";
 std::cout<<"1000 hot calls: 7000 actual ReadProcessMemory calls, 0 tracked new/new[] allocations\n";return 0;
}catch(const std::exception& error){watchAllocations=false;std::cerr<<error.what()<<'\n';return 1;}}
