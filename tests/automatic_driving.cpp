#include <engine/automatic_driving.h>
#include <cassert>
#include <limits>
#include <iostream>
namespace {
// Donnees fictives de test, en m/s. Aucune de ces valeurs ne configure le jeu.
// Chaque scenario est rejoue avec plusieurs consommateurs pour verifier que
// le moteur execute leurs consignes sans choisir une vitesse reglementaire.
struct TestSpeeds {
 double maximumMps;
 double heldMps;
 double entryMps;
 double approachMps;
 double alternateApproachMps;
 double nativeCruiseMps;
};
void checkSignalStoppingMargin(){
 using namespace nimby::engine::automatic;
 constexpr uint64_t announcement=0x8000000000001,signal=0x8000000010001;
 const std::vector<Ahead> direct{{signal,1000}};
 const std::vector<NimbySignalDrivingRule> stop{{signal,0,10,0,NIMBY_DRIVING_STOP}};
 const auto planned=[&](double head,double margin=defaultStopBeforeSignalMeters){
  Memory memory;return plan(memory,direct,stop,head,100,100,.5,true,{},margin);
 };
 auto p=planned(900);
 assert(p.signalStopDistanceM==97&&p.stopSignal==signal&&p.ceiling==envelope(0,97,.5));
 p=planned(996.5);assert(p.signalStopDistanceM==.5&&p.ceiling>0);
 p=planned(997);assert(p.signalStopDistanceM==0&&p.ceiling==0);
 p=planned(999);assert(p.signalStopDistanceM==0&&p.ceiling==0);
 p=planned(1000);assert(p.signalStopDistanceM==0&&p.ceiling==0);
 p=planned(900,0);assert(p.signalStopDistanceM==100&&p.ceiling==envelope(0,100,.5));
 p=planned(900,8.5);assert(p.signalStopDistanceM==91.5);
 p=planned(900,100);assert(p.signalStopDistanceM==0);
 for(const double invalid:{-.1,100.0001,std::numeric_limits<double>::infinity(),
                           std::numeric_limits<double>::quiet_NaN()})
  assert(planned(900,invalid).signalStopDistanceM==97);
 assert(!planned(799).signalStopDistanceM); // Invisible signals do not invent stops.

 Memory memory;
 const std::vector<NimbySignalDrivingRule> speed{{signal,8,10,0,0}};
 p=plan(memory,direct,speed,900,100,100,.5,true);
 assert(!p.signalStopDistanceM&&p.stopSignal==0&&p.ceiling==envelope(8,100,.5));
 const std::vector<NimbySignalDrivingRule> clear{{signal,-1,10,0,NIMBY_DRIVING_CLEAR}};
 memory={};p=plan(memory,direct,clear,900,100,100,.5,true);
 assert(!p.active&&!p.signalStopDistanceM);
 memory={};p=plan(memory,direct,clear,900,100,100,.5,false);
 assert(p.signalStopDistanceM==97&&p.ceiling==envelope(0,97,.5)); // Stale indication stops before the real signal.

 const std::vector<Ahead> route{{announcement,100},{signal,1000}};
 const std::vector<NimbySignalDrivingRule> announced{
  {announcement,0,10,1,0},{signal,0,10,0,NIMBY_DRIVING_STOP}};
 memory={};p=plan(memory,route,announced,0,100,100,.5,true);
 assert(p.signalStopDistanceM==997&&p.stopSignal==signal&&memory.stops.empty());
 crossed(memory,passages(route,announced),99,101);
 p=plan(memory,{},announced,797,100,100,.5,true);
 assert(p.signalStopDistanceM==200&&memory.stops.front().position==1000);
 p=plan(memory,direct,clear,997,100,100,.5,true);
 assert(!p.signalStopDistanceM&&memory.stops.front().position==1000);
 crossed(memory,{},997,997);assert(memory.stops.size()==1); // A stop point is never a crossing.
 crossed(memory,{},997,999);assert(memory.stops.size()==1);
 crossed(memory,{},999,1000.1);assert(memory.stops.empty());
 memory={};
 const std::vector<Ahead> missing{{announcement,100}};
 const std::vector<NimbySignalDrivingRule> missingRule{{announcement,0,10,2,0}};
 p=plan(memory,missing,missingRule,0,100,100,.5,true);
 assert(p.signalStopDistanceM==97&&p.stopSignal==announcement&&memory.stops.empty());

 const NimbySignalDrivingRule permissive{signal,0,10,0,
  NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED};
 const std::vector<NimbySignalDrivingRule> permissiveRules{permissive};
 const SightClearance visible{true,200,0};
 memory={};observeStop(memory,direct,permissiveRules,997,997,0,1,true);
 assert(memory.stopped.signal==signal&&memory.stopped.position==1000);
 assert(restrictedEntry(memory,permissive,997,visible,true));
 p=plan(memory,direct,permissiveRules,997,100,100,.5,true,visible);
 assert(!p.signalStopDistanceM&&p.ceiling>0); // Valid stop proof releases this fence.
 observeStop(memory,direct,permissiveRules,997,997,0,0,true);
 assert(memory.stopped.signal==signal); // Existing proof persists, but a pause cannot create one.
 memory={};observeStop(memory,direct,permissiveRules,997,997,0,0,true);
 assert(!memory.stopped.signal);
 observeStop(memory,direct,permissiveRules,995.9,995.9,0,1,true);assert(!memory.stopped.signal);
 observeStop(memory,direct,permissiveRules,997,997,.01,1,true);assert(!memory.stopped.signal);
 observeStop(memory,direct,permissiveRules,1000.01,1000.01,0,1,true);assert(!memory.stopped.signal);
 observeStop(memory,direct,permissiveRules,990,990,0,1,true,10);
 assert(memory.stopped.position==1000&&restrictedEntry(memory,permissive,990,visible,true,10));
 assert(!restrictedEntry(memory,permissive,990,visible,true,3));
 observeStop(memory,direct,permissiveRules,990,990,0,1,true,3);assert(!memory.stopped.signal);
 memory={};observeStop(memory,direct,permissiveRules,1000,1000,0,1,true,0);
 assert(restrictedEntry(memory,permissive,1000,visible,true,0));
 assert(!restrictedEntry(memory,permissive,1000,{true,5,0},true,100)); // Obstacle clearance stays 5 m.
}
void checkHeldLimitAtCurrentSignal(){
 using namespace nimby::engine::automatic;
 constexpr uint64_t source=0x8000000000001,next=0x8000000010001;
 // Consumer-selected example: a retained 30 km/h ceiling starts HERE, not at
 // a signal which may be absent beyond a manoeuvre route's native boundary.
 constexpr double limit=30.0/3.6,vmax=160.0/3.6,length=100;
 const std::vector<Ahead> entry{{source,1000}};
 const std::vector<NimbySignalDrivingRule> instruction{{source,limit,0,0,
  NIMBY_DRIVING_HOLD_TO_CLEAR|NIMBY_DRIVING_APPROACH_PASSABLE}};
 assert(valid(instruction.front()));
 Memory memory;
 // A numeric passage instruction does not request physical-clearance entry
 // in place of native admission, even when a physical view is available.
 assert(!restrictedEntry(memory,instruction.front(),1000,{true,200,0},true));
 auto p=plan(memory,entry,instruction,900,length,vmax,.5,true);
 assert(p.active&&std::abs(p.ceiling-envelope(limit,100,.5))<1e-9);
 assert(memory.held.empty()&&!memory.sight);
 p=plan(memory,entry,instruction,1000,length,vmax,.5,true);
 assert(p.active&&p.ceiling==limit&&memory.held.empty());
 const auto observed=passages(entry,instruction);
 crossed(memory,observed,999,1000);
 crossed(memory,observed,1000,1000);
 assert(memory.held.empty()); // Reaching the panel or staying there is not passage.
 crossed(memory,observed,1000,1000.1);
 assert(memory.held.size()==1&&!memory.held.front().hasEnd);
 assert(memory.held.front().source==source&&memory.held.front().start==1000);
 assert(memory.held.front().speed==limit&&!memory.sight&&memory.stops.empty());
 // No next PATH signal, a truncated native scan, or a non-PATH endpoint is
 // represented by no further driving passage. None proves a CLEAR crossing.
 for(const double head:{1000.1,1300.0,2000.0}){
  crossed(memory,{},head,head);
  p=plan(memory,{}, {},head,length,vmax,.5,true);
  assert(p.active&&p.ceiling==limit&&memory.held.size()==1);
  assert(!memory.held.front().hasEnd&&!p.signalStopDistanceM&&!memory.sight);
 }
 const std::vector<Ahead> exit{{next,2500}};
 const std::vector<NimbySignalDrivingRule> clear{{next,-1,0,0,NIMBY_DRIVING_CLEAR}};
 for(const double head:{2300.0,2499.0,2500.0}){
  p=plan(memory,exit,clear,head,length,vmax,.5,true);
  assert(p.active&&p.ceiling==limit&&!memory.held.front().hasEnd);
 }
 crossed(memory,passages(exit,clear),2500,2500);
 assert(!memory.held.front().hasEnd); // Seeing CLEAR or waiting on it cannot release.
 crossed(memory,passages(exit,clear),2500,2500.1);
 assert(memory.held.front().hasEnd&&memory.held.front().end==2500);
 for(const double head:{2500.1,2599.0,2600.0}){
  p=plan(memory,{}, {},head,length,vmax,.5,true);
  assert(p.active&&p.ceiling==limit&&memory.held.size()==1);
 }
 p=plan(memory,{}, {},2600.1,length,vmax,.5,true);
 assert(!p.active&&p.ceiling==vmax&&memory.held.empty()&&!memory.sight);
 std::cout<<"PASS: current-signal 30 km/h hold survives absent downstream data; actual CLEAR passage and rear clearance release it\n";
}
void checkConsumerInstructions(const TestSpeeds& speeds){
 using namespace nimby::engine::automatic;
 constexpr uint64_t a=0x8000000000001,s=0x8000000010001;
 const double vmax=speeds.maximumMps;
 const std::vector<NimbySignalDrivingRule> closed{{a,0,speeds.approachMps,1,0},{s,0,speeds.approachMps,0,NIMBY_DRIVING_STOP}};
 const std::vector<NimbySignalDrivingRule> clear{{a,-1,speeds.approachMps,0,NIMBY_DRIVING_CLEAR},{s,-1,speeds.approachMps,0,NIMBY_DRIVING_CLEAR}};
 std::vector<Ahead> ahead{{a,500},{s,1000}};Memory memory;
 auto p=plan(memory,ahead,closed,0,100,vmax,0.5,true);
 assert(!p.active&&p.ceiling==vmax); // An unseen signal cannot drive the train.
 p=plan(memory,ahead,closed,299.99,100,vmax,0.5,true);
 assert(!p.active); // Strict maximum visibility: 200 m, not the whole lookahead.
 p=plan(memory,ahead,closed,300,100,vmax,0.5,true);
 assert(p.active&&p.ceiling<vmax);
 assert(std::abs(p.ceiling*2+p.ceiling*p.ceiling/(2*0.5)-697)<1e-6);
 assert(memory.stops.empty()); // Merely looking ahead must not latch an approach.
 p=plan(memory,ahead,clear,400,100,vmax,0.5,true);
 assert(!p.active&&p.ceiling==vmax); // No retained limit before receiving an instruction at passage.
 // Permission granted / zero movement is not a passage.
 const auto warnings=passages(ahead,closed);
 crossed(memory,warnings,499,499);assert(memory.stops.empty());
 crossed(memory,warnings,499,500);assert(memory.stops.empty());
 crossed(memory,warnings,500,501);assert(memory.stops.size()==1);
 p=plan(memory,ahead,clear,600,100,vmax,0.5,true);
 assert(std::abs(p.ceiling-envelope(0,397,.5))<1e-9); // Unseen reopening ignored; default 3 m stop clearance.
 p=plan(memory,ahead,clear,999.99,100,vmax,0.5,true);
 assert(p.active&&std::abs(p.ceiling-speeds.approachMps)<1e-9); // Actual warning passed: retain approach until target passed.
 crossed(memory,passages(ahead,clear),999.99,1000.1);
 p=plan(memory,ahead,clear,1000.1,100,vmax,0.5,true);
 assert(!p.active&&memory.stops.empty());
 // A retained target receives explicit passage permission and announces a
 // new target. Both instructions coexist until the first target is crossed.
 // Permission to pass is distinct from releasing other retained limits.
 {
  constexpr uint64_t nextSignal=s+0x10000;
  const double approach=speeds.approachMps;
  std::vector<Ahead> route{{a,500},{s,1000},{nextSignal,1600}};
  std::vector<NimbySignalDrivingRule> instructions{
   {a,0,approach,1,NIMBY_DRIVING_FOLLOW_TARGET},
   {s,0,approach,0,NIMBY_DRIVING_STOP},
   {nextSignal,0,approach,0,NIMBY_DRIVING_STOP}};
  Memory received;
  crossed(received,passages(route,instructions),499,501);
  instructions[1]={s,0,approach,1,NIMBY_DRIVING_FOLLOW_TARGET|NIMBY_DRIVING_APPROACH_PASSABLE};
  auto approachPlan=plan(received,route,instructions,799,100,vmax,.5,true);
  assert(std::abs(approachPlan.ceiling-envelope(0,198,.5))<1e-9); // Reopening not visible yet.
  approachPlan=plan(received,route,instructions,1000,100,vmax,.5,true);
  assert(approachPlan.active&&std::abs(approachPlan.ceiling-approach)<1e-9);
  assert(received.stops.size()==1&&received.stops.front().signal==s);
  // A downstream target alone grants nothing. Policy belongs to the mod.
  auto noPermission=instructions;noPermission[1].flags=NIMBY_DRIVING_FOLLOW_TARGET;
  assert(plan(received,route,noPermission,1000,100,vmax,.5,true).ceiling==0);
  // The retained target also accepts a different consumer-supplied speed.
  received.stops.front().reopenedSpeed=speeds.alternateApproachMps;
  assert(std::abs(plan(received,route,instructions,1000,100,vmax,.5,true).ceiling-speeds.alternateApproachMps)<1e-9);
  received.stops.front().reopenedSpeed=approach;
  assert(valid(instructions[1]));
  auto otherSpeed=instructions[1];otherSpeed.reopened_speed_mps=4*speeds.maximumMps;
  assert(valid(otherSpeed)); // No fixed national upper speed in publication.
  auto contradictory=instructions[1];contradictory.flags|=NIMBY_DRIVING_STOP;
  assert(!valid(contradictory));
  // Stale, absent or explicitly closed data cannot grant passage.
  assert(plan(received,route,instructions,1000,100,vmax,.5,false).ceiling==0);
  auto missing=instructions;missing.erase(missing.begin()+1);
  assert(plan(received,route,missing,1000,100,vmax,.5,true).ceiling==0);
  auto closedAgain=instructions;closedAgain[1]={s,0,approach,0,NIMBY_DRIVING_STOP};
  assert(plan(received,route,closedAgain,1000,100,vmax,.5,true).ceiling==0);
  // Passage permission does not clear an independent speed restriction.
  received.held.push_back({0,speeds.heldMps,0,false});
  crossed(received,passages(route,instructions),1000,1000);
  assert(received.stops.front().signal==s);
  crossed(received,passages(route,instructions),1000,1000.1);
  assert(received.stops.size()==1&&received.stops.front().signal==nextSignal);
  assert(!received.held.front().hasEnd);
  approachPlan=plan(received,route,instructions,1000.1,100,vmax,.5,true);
  assert(std::abs(approachPlan.ceiling-envelope(0,596.9,.5))<1e-9);
  // Release uses head passage, not rear clearance; the next stop still applies.
  assert(plan(received,route,instructions,1600,100,vmax,.5,true).ceiling==0);
 }
 // Native lookahead already removed the target: measured movement releases it.
 memory={};crossed(memory,warnings,499,501);
 crossed(memory,{},999.99,1000.1);assert(memory.stops.empty());
 // A reopened target permits a real movement. The native wrapper can miss
 // its post-integration commit when another callback owns the state lock.
 // The next scan no longer contains that passed target; reconcile only its
 // release from the two observed heads, never invent a received instruction.
 memory={};crossed(memory,warnings,499,501);
 p=plan(memory,ahead,clear,999,100,vmax,.5,true);
 assert(p.active&&p.ceiling>=speeds.approachMps&&memory.stops.size()==1);
 // Native motion advances 999 -> 1001 here, without a crossed() commit.
 p=plan(memory,{},clear,1001,100,vmax,.5,true);
 std::cerr<<"missed_commit_release ceiling="<<p.ceiling<<" retained="<<memory.stops.size()<<'\n';
 assert(!p.active&&p.ceiling==vmax&&memory.stops.empty());
 // Remaining still on the target never proves passage, even if it is clear.
 memory={};crossed(memory,warnings,499,501);
 plan(memory,ahead,clear,1000,100,vmax,.5,true);
 p=plan(memory,{},closed,1000,100,vmax,.5,true);
 assert(p.active&&p.ceiling==0&&memory.stops.size()==1);
 // A first observation alone has no earlier head to prove missed movement.
 memory={};crossed(memory,warnings,499,501);
 p=plan(memory,{},clear,1001,100,vmax,.5,true);
 assert(p.active&&p.ceiling==0&&memory.stops.size()==1);
 // Refresh the same target's route position BEFORE deciding it was passed.
 memory={};crossed(memory,warnings,499,501);
 plan(memory,ahead,clear,999,100,vmax,.5,true);
 const std::vector<Ahead> rerouted{{s,2000}};
 p=plan(memory,rerouted,clear,1001,100,vmax,.5,true);
 assert(p.active&&memory.stops.size()==1&&memory.stops.front().position==2000);
 assert(std::abs(p.ceiling-envelope(0,996,.5))<1e-9);
 // Progress which remains before the target cannot release it; stale data
 // also cannot use its visible reopening to grant movement through it.
 memory={};crossed(memory,warnings,499,501);
 plan(memory,ahead,clear,990,100,vmax,.5,true);
 p=plan(memory,ahead,clear,999,100,vmax,.5,false);
 assert(p.active&&memory.stops.size()==1&&p.ceiling==0); // Already inside the 3 m stop clearance.
 p=plan(memory,ahead,clear,1000,100,vmax,.5,false);
 assert(p.active&&p.ceiling==0&&memory.stops.size()==1);
 // Reconciliation consumes a passed stop only, preserving a further stop
 // and a separately received speed limit when publication becomes stale.
 memory={};crossed(memory,warnings,499,501);
 memory.stops.push_back({s+0x10000,1600,0,speeds.approachMps});
 memory.held.push_back({0,speeds.heldMps,0,false});
 plan(memory,ahead,clear,999,100,vmax,.5,true);
 p=plan(memory,{},clear,1001,100,vmax,.5,false);
 assert(p.active&&memory.stops.size()==1&&memory.stops.front().signal==s+0x10000);
 assert(memory.held.size()==1&&std::abs(p.ceiling-std::min(speeds.heldMps,envelope(0,596,.5)))<1e-9);
 // Invalid observations cannot overwrite the anchor; the existing backward
 // reset discards the old route rather than replaying its missing passage.
 memory={};crossed(memory,warnings,499,501);
 plan(memory,ahead,clear,999,100,vmax,.5,true);
 assert(!plan(memory,{},clear,std::numeric_limits<double>::quiet_NaN(),100,vmax,.5,true).active);
 assert(memory.lastHead==999&&memory.stops.size()==1);
 p=plan(memory,{},clear,998,100,vmax,.5,true);
 assert(!p.active&&memory.stops.empty());
 memory={};p=plan(memory,{},clear,1001,100,vmax,.5,true);
 assert(!p.active&&memory.stops.empty()); // New motion starts with new memory.
 // A native step ending precisely on the source cannot latch it yet. The
 // next step can confirm it even if the native scan no longer lists it.
 memory={};crossed(memory,warnings,499,500);assert(memory.stops.empty());
 auto pending=atBoundary(warnings,500);std::vector<Passage> next;
 appendBoundary(next,pending,closed,500);crossed(memory,next,500,501);assert(memory.stops.size()==1);
 memory={};next.clear();appendBoundary(next,pending,clear,500);crossed(memory,next,500,501);assert(memory.stops.empty());
 // A stop signal noticed, then reopened BEFORE passage does not create memory.
 memory={};plan(memory,ahead,closed,900,100,vmax,0.5,true);
 p=plan(memory,ahead,clear,950,100,vmax,0.5,true);assert(!p.active);
 // Source changes before crossing: use its current instruction.
 crossed(memory,passages(ahead,clear),499,501);assert(memory.stops.empty());
 p=plan(memory,ahead,clear,400,100,vmax,0.5,false);
 assert(p.active&&p.ceiling<11);
 auto rules=clear;rules[0]={a,speeds.heldMps,speeds.approachMps,1,NIMBY_DRIVING_HOLD_TO_CLEAR};
 memory={};p=plan(memory,ahead,rules,0,100,vmax,0.5,true);
 assert(!p.active&&memory.held.empty());
 p=plan(memory,ahead,rules,299.99,100,vmax,0.5,true);
 assert(!p.active&&memory.held.empty());
 p=plan(memory,ahead,rules,300,100,vmax,0.5,true);
 assert(p.active&&p.ceiling<vmax&&p.ceiling>speeds.heldMps&&memory.held.empty());
 p=plan(memory,ahead,rules,400,100,vmax,0.5,true);
 assert(p.active&&std::abs(p.ceiling-envelope(speeds.heldMps,600,.5))<1e-9);
 assert(p.ceiling>speeds.heldMps&&memory.held.empty()); // The consumer limit starts at the target, not the source.
 crossed(memory,passages(ahead,rules),499,501);assert(memory.held.size()==1);
 p=plan(memory,ahead,rules,501,100,vmax,.5,true);
 assert(p.active&&std::abs(p.ceiling-envelope(speeds.heldMps,499,.5))<1e-9);
 p=plan(memory,ahead,rules,1000,100,vmax,.5,true);
 assert(p.active&&p.ceiling==speeds.heldMps); // Target reached: the ceiling now applies.
 crossed(memory,passages(ahead,rules),999,1001);
 p=plan(memory,ahead,rules,1099,100,vmax,0.5,true);assert(p.active&&p.ceiling<=speeds.heldMps);
 p=plan(memory,ahead,rules,1101,100,vmax,0.5,true);assert(!p.active);
 // A new route with a reset distance must not inherit the previous approach.
 memory={};crossed(memory,warnings,499,501);plan(memory,ahead,closed,600,100,vmax,0.5,true);
 p=plan(memory,ahead,clear,0,100,vmax,0.5,true);assert(!p.active&&memory.stops.empty());
 memory={};ahead.resize(1);rules[0]={a,0,speeds.approachMps,2,0};
 p=plan(memory,ahead,rules,300,100,vmax,0.5,true);assert(p.active&&memory.stops.empty());
 crossed(memory,passages(ahead,rules),499,501);assert(memory.stops.empty());
 // Remove timetable cruising only; preserve explicit native target ceilings.
 assert(cruiseCeiling(speeds.nativeCruiseMps,speeds.heldMps,true,false,0)==speeds.heldMps);
 assert(cruiseCeiling(speeds.nativeCruiseMps,speeds.heldMps,false,false,0)==speeds.nativeCruiseMps);
 assert(cruiseCeiling(speeds.nativeCruiseMps,speeds.heldMps,true,true,speeds.entryMps)==speeds.entryMps);
 assert(cruiseCeiling(speeds.nativeCruiseMps,speeds.heldMps,true,true,0)==0);
 assert(cruiseCeiling(speeds.nativeCruiseMps,speeds.heldMps,true,true,std::numeric_limits<double>::quiet_NaN())==speeds.nativeCruiseMps);
 assert(!valid({1,0,speeds.approachMps,0,0})&&!valid({a,0,speeds.approachMps,3,0})&&!valid({a,0,speeds.approachMps,0,8}));
 // Follow the consumer-supplied numeric target when explicitly requested.
 // Passing the announcement must not turn that target back into zero.
 ahead={{a,500},{s,1000}};
 rules={{a,0,speeds.approachMps,1,NIMBY_DRIVING_FOLLOW_TARGET},
        {s,speeds.entryMps,speeds.approachMps,0,NIMBY_DRIVING_HOLD_TO_CLEAR}};
 memory={};p=plan(memory,ahead,rules,999,100,vmax,.5,true);
 assert(std::abs(p.ceiling-envelope(speeds.entryMps,1,.5))<1e-9);
 crossed(memory,passages(ahead,rules),499,501);
 p=plan(memory,ahead,rules,999,100,vmax,.5,true);
 assert(std::abs(p.ceiling-envelope(speeds.entryMps,1,.5))<1e-9);
 rules.pop_back();p=plan(memory,ahead,rules,999,100,vmax,.5,true);
 assert(p.ceiling<1); // Missing target never borrows its former numeric value.
 constexpr uint64_t b=0x8000000000002;
 ahead={{a,100},{b,500},{s,1000}};
 rules={{a,0,speeds.approachMps,2,NIMBY_DRIVING_FOLLOW_TARGET},
        {b,0,speeds.approachMps,1,NIMBY_DRIVING_FOLLOW_TARGET},
        {s,speeds.entryMps,speeds.approachMps,0,NIMBY_DRIVING_HOLD_TO_CLEAR}};
 memory={};p=plan(memory,ahead,rules,0,100,vmax,.5,true);
 assert(std::abs(p.ceiling-envelope(0,997,.5))<1e-9); // Numeric target not visible yet.

 // Live regression: a two-panel announcement was retained after its immediate
 // successor reopened. Cancellation is explicit policy supplied by the mod.
 rules[0].flags |= NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR;
 memory={};crossed(memory,passages(ahead,rules),99,101);
 rules[1]={b,-1,speeds.approachMps,0,NIMBY_DRIVING_CLEAR};
 p=plan(memory,ahead,rules,299.99,100,vmax,.5,true);
 assert(p.active&&!memory.stops.empty()); // Reopening is still out of sight.
 p=plan(memory,ahead,rules,300,100,vmax,.5,false);
 assert(p.active&&!memory.stops.empty()); // Stale rules cannot cancel.
 p=plan(memory,ahead,rules,300,100,vmax,.5,true);
 assert(!p.active&&memory.stops.empty()); // Visible next clear releases this announcement.
 memory={};crossed(memory,passages(ahead,rules),99,101);
 crossed(memory,passages(ahead,rules),499,501);
 assert(memory.stops.empty()); // Also release at measured passage if a step reaches it directly.
 assert(valid(rules[0]));
 auto invalidCancel=rules[0];invalidCancel.signals_ahead=1;assert(!valid(invalidCancel));
 invalidCancel=rules[0];invalidCancel.flags=NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR;assert(!valid(invalidCancel));
 // An unknown intermediate panel, or another received stop instruction, is
 // never cancelled just because this optional policy exists elsewhere.
 memory={};crossed(memory,passages(ahead,rules),99,101);
 auto unknownIntermediate=rules;unknownIntermediate.erase(unknownIntermediate.begin()+1);
 p=plan(memory,ahead,unknownIntermediate,300,100,vmax,.5,true);
 assert(p.active&&memory.stops.size()==1);
 rules[1]={b,0,speeds.approachMps,1,NIMBY_DRIVING_FOLLOW_TARGET};
 crossed(memory,passages(ahead,rules),499,501);
 assert(memory.stops.size()==1&&!memory.stops.front().cancelAt.signal);
 rules[1]={b,-1,speeds.approachMps,0,NIMBY_DRIVING_CLEAR};
 p=plan(memory,ahead,rules,501,100,vmax,.5,true);
 assert(p.active&&!memory.stops.empty()); // Reopening behind the train is irrelevant.
 memory={};crossed(memory,passages(ahead,rules),99,101);
 memory.held.push_back({1000,speeds.heldMps,0,false});
 p=plan(memory,ahead,rules,300,100,vmax,.5,true);
 assert(memory.stops.empty()&&memory.held.size()==1&&p.active);

 // Experimental restricted-mode model: these tests do not assert native
 // permission support. A runtime must supply fresh verified route clearance.
 const NimbySignalDrivingRule permissive{s,0,speeds.approachMps,0,
  NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED};
 assert(valid(permissive));
 Memory absoluteMemory;absoluteMemory.stopped={s,1000};
 assert(!restrictedEntry(absoluteMemory,{s,0,speeds.approachMps,0,NIMBY_DRIVING_STOP},1000,{true,200,0},true));
 assert(clearPrefix([](double){return true;})==1);
 assert(clearPrefix([](double){return false;})==0);
 const auto prefix=clearPrefix([](double end){return end<.347;});
 assert(prefix<.347&&.347-prefix<1e-8);
 assert(!valid({s,0,speeds.approachMps,0,NIMBY_DRIVING_STOP_THEN_PROCEED}));
 assert(!valid({s,0,speeds.approachMps,1,NIMBY_DRIVING_ON_SIGHT}));
 const SightClearance visible{true,200,0};
 rules={permissive};ahead={{s,1000}};memory={};
 assert(!restrictedEntry(memory,permissive,1000,visible,true));
 observeStop(memory,ahead,rules,990,990,0,1,true);
 assert(!memory.stopped.signal); // Stopped somewhere else.
 observeStop(memory,ahead,rules,1000,1000,0,0,true);
 assert(!memory.stopped.signal); // Pause does not prove a simulated stop.
 observeStop(memory,ahead,rules,999.99,1000,.01,1,true);
 assert(!memory.stopped.signal); // Native creeping floor is not a stop.
 observeStop(memory,ahead,rules,1000,1000,0,1,true);
 assert(restrictedEntry(memory,permissive,1000,visible,true));
 assert(!restrictedEntry(memory,permissive,1000,{},true));
 assert(!restrictedEntry(memory,permissive,1000,{true,5,0},true));
 assert(!restrictedEntry(memory,permissive,1000,visible,false));
 observeStop(memory,ahead,rules,1000,1000,std::numeric_limits<double>::quiet_NaN(),1,true);
 assert(!memory.stopped.signal);
 observeStop(memory,ahead,rules,1000,1000,0,1,true);
 observeStop(memory,ahead,rules,1000,1000,0,1,false);
 assert(!memory.stopped.signal);
 rules={{s,speeds.entryMps,speeds.approachMps,0,NIMBY_DRIVING_ON_SIGHT}};
 assert(restrictedEntry(memory,rules[0],1000,visible,true)); // No stop for this instruction.
 p=plan(memory,ahead,rules,1000,100,vmax,.5,true,visible);
 assert(p.ceiling==speeds.entryMps);
 crossed(memory,passages(ahead,rules),1000,1001);
 p=plan(memory,{},rules,1001,100,vmax,.5,true,visible);
 assert(memory.sight&&p.ceiling==speeds.approachMps);
 p=plan(memory,{},rules,1001,100,vmax,.5,true);
 assert(p.ceiling==0); // No optimistic movement without physical coverage.
 p=plan(memory,{},rules,1001,100,vmax,.5,false,visible);assert(p.ceiling==0);
 ahead={{a,1200}};plan(memory,ahead,{},1001,100,vmax,.5,true,visible);
 assert(memory.sight->endSignal==a);
 crossed(memory,{},1200,1200);assert(memory.sight);
 crossed(memory,{},1200,1201);assert(!memory.sight);

 // Closest physical tail, either direction, multiple trains, own footprint
 // excluded. A short observed route is not extended by an assumed clear area.
 std::vector<SightSection> route{{10,.2,.8,1000}};
 std::vector<nimby::TrainFootprint> feet{{1,10,.1,.25},{2,10,.5,.6},{3,10,.4,.45}};
 auto view=sightClearance(1,route,feet,true,500);
 assert(view.verified&&std::abs(view.distanceM-200)<1e-9&&view.obstacle==3);
 route={{10,.8,.2,1000}};view=sightClearance(1,route,feet,true,500);
 assert(std::abs(view.distanceM-200)<1e-9&&view.obstacle==2);
 route={{11,.9,1,1000},{10,.2,.8,1000}};
 view=sightClearance(1,route,feet,true,500);
 assert(std::abs(view.distanceM-300)<1e-9&&view.obstacle==3);
 assert(!sightClearance(1,route,feet,false,500).verified);
 route={{11,0,.1,1000}};view=sightClearance(1,route,feet,true,500);
 assert(view.distanceM==100&&view.obstacle==0);
 route={{10,.5,.8,1000}};view=sightClearance(1,route,feet,true,500);
 assert(view.distanceM==0&&sightCeiling(view,speeds.approachMps,.5)==0);
 assert(sightCeiling({},speeds.approachMps,.5)==0);
 assert(sightCeiling({true,5,0},speeds.approachMps,.5)==0);
 const double speed=sightCeiling({true,30,2},speeds.approachMps,.5);
 assert(speed>0&&speed<speeds.approachMps&&std::abs(speed*2+speed*speed/(2*.5)-25)<1e-9);
}
} // namespace

int main(){
 checkSignalStoppingMargin();
 checkHeldLimitAtCurrentSignal();
 // Valeurs arbitraires : ni profil BAL ni valeurs par defaut du SDK.
 constexpr TestSpeeds consumers[]{
  {60.0,36.0,3.25,6.0,4.5,43.0},
  {75.0,42.0,4.75,9.0,5.5,47.0}
 };
 for(const auto& speeds:consumers)checkConsumerInstructions(speeds);
 std::cout<<"PASS: generic consumer instructions with two independent speed profiles; crossings, target replacement, release, physical clearance\n";
}
