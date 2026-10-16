// Actual 30E020 bridge wrapper with synthetic native memory/functions.
// Hash-pinned ABI/ownership in the real game is NOT proved by this fixture.
#include <windows.h>
uintptr_t batchFixtureCaller();
#define NRF_PHASE_CALLER() batchFixtureCaller()
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#include <iostream>
#include <stdexcept>
#include <memory>
namespace {
size_t checks{},calls{};
thread_local uintptr_t fixtureCaller{};
std::array<uintptr_t,5> expected{},received{};
DWORD expectedIncoming=0x933,returnedError=0x944;
bool throwOriginal=false;
}
uintptr_t batchFixtureCaller(){return fixtureCaller;}
#define CHECK(x) do{++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
void __fastcall originalBatch(uintptr_t db,uintptr_t sim,uintptr_t geo,uintptr_t commands,uintptr_t journal){
 ++calls;received={db,sim,geo,commands,journal};CHECK(received==expected);CHECK(GetLastError()==expectedIncoming);
 // Simulate a native side effect and native error even on the throwing path.
 *reinterpret_cast<uint64_t*>(commands)+=1;SetLastError(returnedError);
 if(throwOriginal)throw std::runtime_error("native batch exception");
}
template<class Buffer,class T> void put(Buffer& buffer,size_t offset,T value){std::memcpy(buffer.data()+offset,&value,sizeof value);}
struct Fixture {
 std::unique_ptr<unsigned char[]> image{std::make_unique<unsigned char[]>(0xb819a0)};
 alignas(16) static inline std::array<unsigned char,0xb40> root{};
 alignas(16) static inline std::array<unsigned char,32> database{},sim{},foreignDb{},foreignSim{};
 uint64_t commands{},geography{},nativeJournal{};
 world_lineage::Capture capture;
 Fixture(){
  root.fill(0);
  base=reinterpret_cast<uintptr_t>(image.get());
  const auto rootAddress=reinterpret_cast<uintptr_t>(root.data());
  std::memcpy(image.get()+0xb81998,&rootAddress,8);
  const auto controller=rootAddress+0x540;
  put(root,0xb10,controller);put(root,0xb28,GetCurrentThreadId());
  put(root,0x540,reinterpret_cast<uintptr_t>(database.data()));put(root,0x680,reinterpret_cast<uintptr_t>(sim.data()));
  InitializeSRWLock(lock());AcquireSRWLockExclusive(lock());
  world_lineage::Scope scope{controller,controller+0x5d0,controller+0x4c0,
   reinterpret_cast<uintptr_t>(database.data()),reinterpret_cast<uintptr_t>(sim.data()),GetCurrentThreadId()};
  scope.boundary=world_lineage::Boundary::PostSharedExclusive;scope.releaseReturn=0x349fa3;
  scope.originalReleaseCompleted=scope.exclusiveTrySucceeded=true;
  scope.lifecycleHooksActive=scope.currentRootVerified=true;scope.nativeRunnerThread=GetCurrentThreadId();
  nativeWorld.apply([&](auto& line,auto& clock){
   const auto stop=line.beginStop(clock,controller,controller+0x5d0);CHECK(stop);CHECK(line.finishStop(stop));
   const auto start=line.beginStart(clock,controller,controller+0x5d0);CHECK(start);CHECK(line.finishStart(start));
   CHECK(line.runnerEnter(controller,controller+0x5d0,GetCurrentThreadId()));
   capture=line.capturePhase(clock,scope).capture;CHECK(capture);
  });
  const nimby::windows::owned_rear::Ownership ownership{capture,
   [](void*,const world_lineage::Capture& c)noexcept{return nativeWorld.current(c);},
   [](void*)noexcept->uint64_t{return GetCurrentThreadId();},nullptr};
  CHECK(mutationContinuity.bind(ownership).reason==mutation_continuity::Reason::Ready);
  nativeWorld.apply([&](auto& line,auto&){CHECK(line.endCapture(capture));});
  nativeModelCommandBatch=originalBatch;phaseObservationEnabled=true;
 }
 ~Fixture(){ReleaseSRWLockExclusive(lock());phaseObservationEnabled=false;fixtureCaller=0;}
 PSRWLOCK lock(){return reinterpret_cast<PSRWLOCK>(root.data()+0xa00);}
 void call(uintptr_t rva=0x348f07,bool foreign=false){
  expected={reinterpret_cast<uintptr_t>((foreign?foreignDb:database).data()),
   reinterpret_cast<uintptr_t>((foreign?foreignSim:sim).data()),reinterpret_cast<uintptr_t>(&geography),
   reinterpret_cast<uintptr_t>(&commands),reinterpret_cast<uintptr_t>(&nativeJournal)};
  fixtureCaller=base+rva;SetLastError(expectedIncoming);
  modelCommandBatch(expected[0],expected[1],expected[2],expected[3],expected[4]);
  fixtureCaller=0;CHECK(GetLastError()==returnedError);
 }
};
}
int main(){try{
 {
  Fixture f;const auto before=mutationContinuity.counters();const auto nativeBefore=calls;
  phaseObservationEnabled=false;f.call();CHECK(calls==nativeBefore+1&&f.commands==1);
  const auto after=mutationContinuity.counters();CHECK(after.begun==before.begun&&after.globalMutation==before.globalMutation);
 }
 for(auto caller:{uintptr_t(0x348f07),uintptr_t(0x34928b),uintptr_t(0x3493d5)}){
  Fixture f;const auto before=mutationContinuity.counters();const auto nativeBefore=calls;
  f.call(caller);const auto after=mutationContinuity.counters();
  CHECK(calls==nativeBefore+1&&f.commands==1);
  CHECK(after.begun==before.begun+1&&after.completed==before.completed+1);
  CHECK(after.globalMutation!=before.globalMutation&&after.lastMutation==after.lastCompleted);
 }
 {
  Fixture f;const auto before=mutationContinuity.counters();const auto nativeBefore=calls;
  f.call(0x4f3a6f,true);const auto after=mutationContinuity.counters();
  CHECK(calls==nativeBefore+1&&f.commands==1&&after.foreign==before.foreign+1);
  CHECK(after.globalMutation==before.globalMutation&&after.begun==before.begun);
 }
 for(unsigned invalid=0;invalid<4;++invalid){
  Fixture f;const auto before=mutationContinuity.counters();const auto nativeBefore=calls;
  if(invalid==0)put(f.root,0xb28,DWORD{GetCurrentThreadId()+1});
  if(invalid==1)put(f.root,0xb10,uintptr_t{});
  if(invalid==2)put(f.root,0x680,uintptr_t{});
  f.call(invalid==3?0x348f02:0x348f07);const auto after=mutationContinuity.counters();
  CHECK(calls==nativeBefore+1&&f.commands==1&&after.unqualified==before.unqualified+1);
  CHECK(after.globalMutation!=before.globalMutation&&after.begun==before.begun);
 }
 {
  Fixture f;const auto before=mutationContinuity.counters();const auto nativeBefore=calls;
  throwOriginal=true;bool threw=false;
  try{f.call();}catch(const std::runtime_error& e){threw=std::string_view(e.what())=="native batch exception";}
  throwOriginal=false;CHECK(threw&&calls==nativeBefore+1&&f.commands==1);CHECK(GetLastError()==returnedError);
  const auto after=mutationContinuity.counters();CHECK(after.exceptions==before.exceptions+1&&after.completed==before.completed);
  CHECK(!mutationContinuity.owner().world);
 }
 std::cout<<checks<<" actual model-batch wrapper checks PASS; synthetic ownership only\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
