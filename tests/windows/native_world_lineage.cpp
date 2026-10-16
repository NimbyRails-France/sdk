// Synthetic lifecycle/ancestry only. No game/native ABI/writer certificate.
#include "platform/windows/runtime/native_world_lineage.h"
#include <iostream>
#include <stdexcept>
#include <string>
using namespace nimby::windows::native_world;
namespace {
size_t checks{};
#define CHECK(value) do{++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);}while(false)
constexpr uintptr_t owner=0x100000,context=owner+0x5d0;
Scope scope(uint64_t thread=7){return {owner,context,owner+0x4c0,0x200000,0x300000,thread,0x3495fe,0x4784f9,0x477f05};}
void start(Lineage& line,Clock& clock,bool earlyRunner=false){
 const auto stop=line.beginStop(clock,owner,context);CHECK(stop);CHECK(line.finishStop(stop));
 const auto begin=line.beginStart(clock,owner,context);CHECK(begin);
 if(earlyRunner)CHECK(line.runnerEnter(owner,context,7));
 CHECK(line.finishStart(begin));if(!earlyRunner)CHECK(line.runnerEnter(owner,context,7));
}
}
int main(){try{
 Clock clock;Lineage line;
 CHECK(line.capturePhase(clock,scope()).reason==Reason::Unarmed);
 CHECK(!line.runnerEnter(owner,context,7));CHECK(line.state()==State::Lost);
 start(line,clock,true);const auto first=line.capturePhase(clock,scope());
 CHECK(first.reason==Reason::Ready);CHECK(first.capture.world!=first.capture.phase);CHECK(line.current(first.capture));
 CHECK(line.endCapture(first.capture));CHECK(!line.current(first.capture));
 auto secondScope=scope();secondScope.advanceReturn=0x349b64;
 const auto second=line.capturePhase(clock,secondScope);CHECK(second.reason==Reason::Ready);
 CHECK(second.capture.world==first.capture.world);CHECK(second.capture.phase!=first.capture.phase);
 CHECK(line.endCapture(second.capture));CHECK(line.invalidateMerge(clock,second.capture.simulation));
 const auto merged=line.capturePhase(clock,scope());CHECK(merged.reason==Reason::Ready);
 CHECK(merged.capture.world==first.capture.world);CHECK(merged.capture.continuity!=first.capture.continuity);
 CHECK(line.endCapture(merged.capture));
 // Stop invalidates before original/join. Normal runner exit during stop is expected.
 const auto stop=line.beginStop(clock,owner,context);CHECK(stop);CHECK(!line.current(merged.capture));
 CHECK(line.runnerExit(7));CHECK(line.finishStop(stop));
 const auto begin=line.beginStart(clock,owner,context);CHECK(begin);CHECK(line.finishStart(begin));
 CHECK(line.runnerEnter(owner,context,7));const auto replaced=line.capturePhase(clock,scope());
 CHECK(replaced.reason==Reason::Ready);CHECK(replaced.capture.world!=first.capture.world);
 // Pointer, thread and full-ID reuse does not reuse a world generation.
 CHECK(line.endCapture(replaced.capture));
 // A stop concurrently revokes an open capture without destroying its join token.
 const auto open=line.capturePhase(clock,scope());CHECK(open.reason==Reason::Ready);
 const auto interrupted=line.beginStop(clock,owner,context);CHECK(interrupted);
 CHECK(!line.endCapture(open.capture));CHECK(line.state()==State::Stopping);
 CHECK(line.runnerExit(7));CHECK(line.finishStop(interrupted));
 const auto resumed=line.beginStart(clock,owner,context);CHECK(resumed);CHECK(line.finishStart(resumed));
 CHECK(line.runnerEnter(owner,context,7));
 const auto resumedCapture=line.capturePhase(clock,scope());CHECK(resumedCapture.reason==Reason::Ready);
 CHECK(line.endCapture(resumedCapture.capture));
 auto changed=scope();changed.database+=8;CHECK(line.capturePhase(clock,changed).reason==Reason::WorldChanged);
 CHECK(line.state()==State::Lost);CHECK(line.capturePhase(clock,scope()).reason==Reason::Lost);
 // Each concrete rejection uses a newly armed lineage; no equality recovery.
 for(unsigned variation=0;variation<6;++variation){
  Lineage candidate;start(candidate,clock);auto wrong=scope();
  if(variation==0)wrong.context+=8;
  if(variation==1)wrong.thread=9;
  if(variation==2)wrong.sharedLock+=8;
  if(variation==3)wrong.advanceReturn=0x4f3eae;
  if(variation==4)wrong.trainLoopReturn=0;
  if(variation==5)wrong.trainPhaseReturn=0;
  CHECK(candidate.capturePhase(clock,wrong).reason!=Reason::Ready);CHECK(candidate.state()==State::Lost);
 }
 Lineage empty;start(empty,clock);auto none=scope();none.database=0;
 CHECK(empty.capturePhase(clock,none).reason==Reason::EmptyWorld);CHECK(empty.state()==State::Running);
 CHECK(empty.capturePhase(clock,scope()).reason==Reason::Ready);
 // Nested capture, nested lifecycle, wrong attempt and native exception fail closed.
 CHECK(empty.capturePhase(clock,scope()).reason==Reason::UnexpectedTransition);CHECK(empty.state()==State::Lost);
 Lineage nested;const auto initialStop=nested.beginStop(clock,owner,context);CHECK(initialStop);
 CHECK(!nested.beginStop(clock,owner,context));CHECK(!nested.finishStop(initialStop));
 Lineage exception;start(exception,clock);exception.lose();CHECK(exception.capturePhase(clock,scope()).reason==Reason::Lost);
 Lineage startRace;auto raceStop=startRace.beginStop(clock,owner,context);CHECK(startRace.finishStop(raceStop));
 auto raceStart=startRace.beginStart(clock,owner,context);CHECK(raceStart);CHECK(startRace.runnerEnter(owner,context,7));
 CHECK(startRace.capturePhase(clock,scope()).reason==Reason::UnexpectedTransition);CHECK(!startRace.finishStart(raceStart));
 Lineage bad;CHECK(!bad.beginStop(clock,owner,context+8));CHECK(bad.state()==State::Lost);
 Clock exhausted(std::numeric_limits<uint64_t>::max());Lineage last;
 const auto lastStop=last.beginStop(exhausted,owner,context);CHECK(lastStop);CHECK(last.finishStop(lastStop));
 CHECK(!last.beginStart(exhausted,owner,context));CHECK(last.state()==State::Lost);CHECK(exhausted.take()==0);
 Clock nearEnd(std::numeric_limits<uint64_t>::max()-1);Lineage finalPhase;
 start(finalPhase,nearEnd);CHECK(finalPhase.capturePhase(nearEnd,scope()).reason==Reason::SequenceExhausted);
 Lineage other;start(other,clock);const auto otherCapture=other.capturePhase(clock,scope());
 CHECK(otherCapture.capture.world!=first.capture.world);CHECK(other.endCapture(otherCapture.capture));
 CHECK(!other.invalidateMerge(clock,0x400000));CHECK(other.state()==State::Lost);
 Lineage exit;start(exit,clock);CHECK(exit.runnerExit(7));CHECK(exit.state()==State::Lost);
 Lineage forged;start(forged,clock);const auto actual=forged.capturePhase(clock,scope());
 auto wrongToken=actual.capture;++wrongToken.phase;CHECK(!forged.current(wrongToken));
 CHECK(!forged.endCapture(wrongToken));CHECK(forged.state()==State::Lost);
 // Real exclusive provenance gives a NEW post-shared-boundary capture. It is
 // still a lineage token, not a whole-snapshot/historical-continuity proof.
 auto exclusiveScope=scope();exclusiveScope.boundary=Boundary::PostSharedExclusive;
 exclusiveScope.advanceReturn=0;exclusiveScope.trainLoopReturn=0;exclusiveScope.trainPhaseReturn=0;
 exclusiveScope.releaseReturn=0x349fa3;exclusiveScope.originalReleaseCompleted=true;exclusiveScope.exclusiveTrySucceeded=true;
 Lineage exclusive;start(exclusive,clock);const auto owned=exclusive.capturePhase(clock,exclusiveScope);
 CHECK(owned.reason==Reason::Ready);CHECK(owned.capture.boundary==Boundary::PostSharedExclusive);
 CHECK(owned.capture.phase!=first.capture.phase);CHECK(exclusive.endCapture(owned.capture));
 Lineage firstMerge;start(firstMerge,clock);
 const MergeScope mergeBeforeCapture{owner,context,owner+0x4c0,0x200000,0x300000,7,0x349871};
 CHECK(firstMerge.invalidateMerge(clock,mergeBeforeCapture));
 const auto firstMergedCapture=firstMerge.capturePhase(clock,exclusiveScope);
 CHECK(firstMergedCapture.reason==Reason::Ready);CHECK(firstMergedCapture.capture.world!=firstMergedCapture.capture.continuity);
 CHECK(firstMerge.endCapture(firstMergedCapture.capture));
 auto mismatchedMerge=mergeBeforeCapture;mismatchedMerge.simulation+=8;
 CHECK(!firstMerge.invalidateMerge(clock,mismatchedMerge));CHECK(firstMerge.state()==State::Lost);
 Lineage wrongMerge;start(wrongMerge,clock);auto wrongMergeCaller=mergeBeforeCapture;wrongMergeCaller.mergeReturn=0;
 CHECK(!wrongMerge.invalidateMerge(clock,wrongMergeCaller));CHECK(wrongMerge.state()==State::Lost);
 for(unsigned variation=0;variation<4;++variation){
  Lineage candidate;start(candidate,clock);auto invalid=exclusiveScope;
  if(variation==0)invalid.originalReleaseCompleted=false;
  if(variation==1)invalid.exclusiveTrySucceeded=false;
  if(variation==2)invalid.releaseReturn=0x3490bc;
  if(variation==3)invalid.trainPhaseReturn=0x477f05;
  CHECK(candidate.capturePhase(clock,invalid).reason==Reason::UnqualifiedScope);
  CHECK(candidate.state()==State::Lost);
 }
 // Attachment after native runner entry starts a new lineage only at a REAL
 // post-shared exclusive boundary, with installed lifecycle hooks and readback.
 auto adoptionScope=exclusiveScope;adoptionScope.lifecycleHooksActive=true;
 adoptionScope.currentRootVerified=true;adoptionScope.nativeRunnerThread=7;
 Lineage adopted;const auto adoptedCapture=adopted.adoptOwnedWorld(clock,adoptionScope);
 CHECK(adoptedCapture.reason==Reason::Ready);CHECK(adopted.current(adoptedCapture.capture));
 CHECK(adoptedCapture.capture.world!=first.capture.world);
 CHECK(adoptedCapture.capture.phase!=adoptedCapture.capture.world);
 CHECK(adopted.endCapture(adoptedCapture.capture));
 CHECK(adopted.invalidateMerge(clock,mergeBeforeCapture));
 const auto adoptedMerged=adopted.capturePhase(clock,exclusiveScope);
 CHECK(adoptedMerged.reason==Reason::Ready);CHECK(adoptedMerged.capture.world==adoptedCapture.capture.world);
 CHECK(adoptedMerged.capture.continuity!=adoptedCapture.capture.continuity);
 CHECK(adopted.endCapture(adoptedMerged.capture));
 const auto adoptedStop=adopted.beginStop(clock,owner,context);CHECK(adoptedStop);
 CHECK(adopted.runnerExit(7));CHECK(adopted.finishStop(adoptedStop));
 const auto adoptedStart=adopted.beginStart(clock,owner,context);CHECK(adoptedStart);
 CHECK(adopted.finishStart(adoptedStart));CHECK(adopted.runnerEnter(owner,context,7));
 const auto adoptedRestart=adopted.capturePhase(clock,exclusiveScope);
 CHECK(adoptedRestart.reason==Reason::Ready);CHECK(adoptedRestart.capture.world!=adoptedCapture.capture.world);
 CHECK(adopted.endCapture(adoptedRestart.capture));
 CHECK(adopted.adoptOwnedWorld(clock,adoptionScope).reason==Reason::UnexpectedTransition);
 CHECK(adopted.state()==State::Lost);
 CHECK(adopted.adoptOwnedWorld(clock,adoptionScope).reason==Reason::Lost);
 for(unsigned variation=0;variation<11;++variation){
  Lineage fresh;auto wrong=adoptionScope;
  if(variation==0)wrong.lifecycleHooksActive=false;
  if(variation==1)wrong.currentRootVerified=false;
  if(variation==2)wrong.nativeRunnerThread=9;
  if(variation==3)wrong.originalReleaseCompleted=false;
  if(variation==4)wrong.exclusiveTrySucceeded=false;
  if(variation==5)wrong.releaseReturn=0;
  if(variation==6)wrong.sharedLock+=8;
  if(variation==7)wrong.boundary=Boundary::PostTrainShared;
  if(variation==8)wrong.trainPhaseReturn=0x477f05;
  if(variation==9)wrong.thread=0;
  if(variation==10)wrong.context+=8;
  CHECK(fresh.adoptOwnedWorld(clock,wrong).reason!=Reason::Ready);CHECK(fresh.state()==State::Lost);
 }
 Lineage adoptingEmpty;auto emptyScope=adoptionScope;emptyScope.database=0;
 CHECK(adoptingEmpty.adoptOwnedWorld(clock,emptyScope).reason==Reason::EmptyWorld);
 CHECK(adoptingEmpty.state()==State::Unarmed);
 CHECK(adoptingEmpty.adoptOwnedWorld(clock,adoptionScope).reason==Reason::Ready);
 Clock oneNonce(std::numeric_limits<uint64_t>::max());Lineage exhaustedAdoption;
 CHECK(exhaustedAdoption.adoptOwnedWorld(oneNonce,adoptionScope).reason==Reason::SequenceExhausted);
 CHECK(exhaustedAdoption.state()==State::Lost);CHECK(oneNonce.take()==0);
 std::cout<<"native_world_lineage: "<<checks<<" checks PASS\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
