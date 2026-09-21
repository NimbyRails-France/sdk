#pragma once
#include <nimby/detail/automatic_driving.h>
#include <engine/on_sight.h>
#include <algorithm>
#include <cmath>
#include <span>
#include <vector>
namespace nimby::engine::automatic {
struct Ahead {uint64_t signal;double position;};
// Simulation assumption, not a railway regulation or an optical ray cast.
// Route geometry may be known farther away; live indications may not.
inline constexpr double signalVisibilityM=200.0;
inline bool indicationVisible(double position,double head) {
 return std::isfinite(position)&&std::isfinite(head)&&position>=head&&position-head<=signalVisibilityM;
}
struct Target {uint64_t signal;double position,speed,reopenedSpeed;bool followTarget=false;
 Ahead cancelAt{};};
struct HeldLimit {double start,speed,end;bool hasEnd=false;};
struct Memory {std::vector<Target> stops;std::vector<HeldLimit> held;double lastHead=-1;
 std::optional<RestrictedMode> sight;StopProof stopped;};
inline bool valid(const NimbySignalDrivingRule& r){
 if(!(r.signal>>48==8&&std::isfinite(r.speed_mps)&&r.speed_mps>=-1&&r.speed_mps<=166.667&&
 std::isfinite(r.reopened_speed_mps)&&r.reopened_speed_mps>=0&&r.reopened_speed_mps<=166.667&&r.signals_ahead<=2&&(r.flags&~127u)==0))return false;
 if((r.flags&NIMBY_DRIVING_FOLLOW_TARGET)&&(!r.signals_ahead||r.speed_mps!=0||(r.flags&~(NIMBY_DRIVING_FOLLOW_TARGET|NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR))))return false;
 if((r.flags&NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR)&&(!(r.flags&NIMBY_DRIVING_FOLLOW_TARGET)||r.signals_ahead<2))return false;
 if((r.flags&NIMBY_DRIVING_ON_SIGHT)&&(r.signals_ahead||r.speed_mps<0||r.reopened_speed_mps<=0||
    (r.flags&(NIMBY_DRIVING_CLEAR|NIMBY_DRIVING_HOLD_TO_CLEAR))))return false;
 if((r.flags&NIMBY_DRIVING_STOP_THEN_PROCEED)&&(!(r.flags&NIMBY_DRIVING_ON_SIGHT)||!(r.flags&NIMBY_DRIVING_STOP)||r.speed_mps!=0))return false;
 return true;
}
inline const NimbySignalDrivingRule* rule(std::span<const NimbySignalDrivingRule> rules,uint64_t id){
 const auto at=std::lower_bound(rules.begin(),rules.end(),id,[](const auto& r,uint64_t value){return r.signal<value;});
 return at!=rules.end()&&at->signal==id?&*at:nullptr;
}
inline void observeStop(Memory& memory,std::span<const Ahead> ahead,std::span<const NimbySignalDrivingRule> rules,
 double before,double after,double speed,int64_t ticks,bool fresh) {
 if(!fresh||!std::isfinite(before)||!std::isfinite(after)||!std::isfinite(speed)||
    speed<0||speed>0.001||after<before||after-before>1e-6){memory.stopped={};return;}
 if(memory.stopped.signal) {
  const auto* current=rule(rules,memory.stopped.signal);
  if(!current||!(current->flags&NIMBY_DRIVING_STOP_THEN_PROCEED)||after>memory.stopped.position+1e-6||
     after<memory.stopped.position-1.0)memory.stopped={};
 }
 for(const auto& signal:ahead)if(const auto* current=rule(rules,signal.signal))
  if((current->flags&NIMBY_DRIVING_STOP_THEN_PROCEED)&&measuredStop(before,after,speed,ticks,signal.position))
   memory.stopped={signal.signal,signal.position};
}
inline bool restrictedEntry(const Memory& memory,const NimbySignalDrivingRule& instruction,
 double head,const SightClearance& clearance,bool fresh) {
 if(!fresh||!valid(instruction)||!(instruction.flags&NIMBY_DRIVING_ON_SIGHT)||
    !clearance.verified||!std::isfinite(clearance.distanceM)||clearance.distanceM<=5||!std::isfinite(head))return false;
 if(instruction.flags&NIMBY_DRIVING_STOP_THEN_PROCEED)
  return memory.stopped.signal==instruction.signal&&head>=memory.stopped.position-1.0&&head<=memory.stopped.position+1e-6;
 return !(instruction.flags&NIMBY_DRIVING_STOP);
}
// Compute the response-time envelope without changing native state.
inline double envelope(double target,double distance,double braking,double response=2){
 if(distance<=0)return target;
 const double delay=braking*response;
 return std::max(target,std::sqrt(delay*delay+target*target+2*braking*distance)-delay);
}
// A planned indication is provisional. Only an actual native movement across
// its position can turn it into a retained instruction.
struct Passage { Ahead source,target; NimbySignalDrivingRule instruction; Ahead next{}; };
inline std::vector<Passage> passages(std::span<const Ahead> ahead,std::span<const NimbySignalDrivingRule> rules) {
 std::vector<Passage> result;
 for(size_t i=0;i<ahead.size();++i)if(const auto* r=rule(rules,ahead[i].signal)) {
  const auto target=i+r->signals_ahead;
  if(target<ahead.size())result.push_back({ahead[i],ahead[target],*r,i+1<ahead.size()?ahead[i+1]:Ahead{}});
 }
 return result;
}
inline void crossed(Memory& memory,std::span<const Passage> observed,double before,double after) {
 if(!std::isfinite(before)||!std::isfinite(after)||after<before){memory={};return;}
 // Native lookahead may drop the just-crossed boundary before its next scan.
 // Release known targets from verified movement itself, not from continued
 // presence in that lookahead list. Never release on zero movement.
 if(after>before)std::erase_if(memory.stops,[&](const auto& t){return after>t.position+1e-6;});
 if(after>before&&memory.sight&&memory.sight->endSignal&&after>memory.sight->end+1e-6)memory.sight.reset();
 for(const auto& p:observed) {
  if(!(before<=p.source.position+1e-6 && after>p.source.position+1e-6))continue;
  // Passing the target ends its approach instruction, regardless of what a
  // later observation says. Looking ahead at a green does not count as passing.
  std::erase_if(memory.stops,[&](const auto& t){return t.signal==p.source.signal;});
  const auto& r=p.instruction;
  // Only an announcement carrying this cancellation policy may end here.
  // An actual restrictive passage consumes the provisional cancellation point;
  // a later reopening behind the train must not erase a received warning.
  if(r.flags&NIMBY_DRIVING_CLEAR)
   std::erase_if(memory.stops,[&](const auto& t){return t.cancelAt.signal==p.source.signal;});
  else for(auto& target:memory.stops)if(target.cancelAt.signal==p.source.signal)target.cancelAt={};
  if(r.flags&NIMBY_DRIVING_ON_SIGHT) {
   memory.sight=RestrictedMode{p.source.signal,p.next.signal,p.source.position,p.next.position,r.reopened_speed_mps};
   memory.stopped={};
  }
  if(r.flags&NIMBY_DRIVING_CLEAR)
   for(auto& h:memory.held)if(!h.hasEnd&&p.source.position>=h.start){h.end=p.source.position;h.hasEnd=true;}
  if(r.speed_mps==0 && r.signals_ahead>0 && p.target.position>=after) {
   const auto found=std::find_if(memory.stops.begin(),memory.stops.end(),[&](const auto& t){return t.signal==p.target.signal;});
   if(found==memory.stops.end())memory.stops.push_back({p.target.signal,p.target.position,0,r.reopened_speed_mps,
     (r.flags&NIMBY_DRIVING_FOLLOW_TARGET)!=0,
     (r.flags&NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR)?p.next:Ahead{}});
  }
  if(r.speed_mps>0 && (r.flags&NIMBY_DRIVING_HOLD_TO_CLEAR)) {
   // Keep the strictest encountered limit, not a less restrictive earlier one.
   if(memory.held.empty())memory.held.push_back({p.target.position,r.speed_mps,0,false});
   else if(r.speed_mps<memory.held.front().speed)memory.held={{p.target.position,r.speed_mps,0,false}};
  }
 }
}
// Retain only a provisional boundary at the current head. Its indication is
// re-read next step; stopping on a boundary is still not a crossing.
inline std::vector<Passage> atBoundary(std::span<const Passage> seen,double head) {
 std::vector<Passage> result;
 for(const auto& p:seen)if(std::abs(p.source.position-head)<=1e-6)result.push_back(p);
 return result;
}
inline void appendBoundary(std::vector<Passage>& seen,std::span<const Passage> pending,
 std::span<const NimbySignalDrivingRule> rules,double head) {
 for(auto p:pending) {
  if(std::abs(p.source.position-head)>1e-6)continue;
  const auto* current=rule(rules,p.source.signal);
  if(!current||current->signals_ahead!=p.instruction.signals_ahead)continue;
  if(std::any_of(seen.begin(),seen.end(),[&](const auto& row){return row.source.signal==p.source.signal;}))continue;
  p.instruction=*current;seen.push_back(p);
 }
 std::sort(seen.begin(),seen.end(),[](const auto& a,const auto& b){return a.source.position<b.source.position;});
}
// The native integrator still applies the current track limit and upcoming
// braking targets. Only the timetable/cruise ceiling is replaced here.
inline double cruiseCeiling(double native,double material,bool enabled,bool targetActive,double targetCeiling) {
 if(!enabled||!std::isfinite(material)||material<=0)return native;
 if(targetActive && (!std::isfinite(targetCeiling)||targetCeiling<0))return native;
 return targetActive?std::min(material,targetCeiling):material;
}
struct Plan {bool active=false;double ceiling=0;};
inline Plan plan(Memory& memory,std::span<const Ahead> ahead,std::span<const NimbySignalDrivingRule> rules,
 double head,double length,double vmax,double braking,bool fresh,const SightClearance& clearance={}){
 if(!std::isfinite(head)||!std::isfinite(length)||length<=0||!std::isfinite(braking)||braking<=0||!std::isfinite(vmax)||vmax<=0)return {};
 if(head+0.01<memory.lastHead)memory={};
 memory.lastHead=head;
 // A short native lookahead can omit the exit when the mode begins. Bind it
 // once it becomes observable; absence alone never releases the restriction.
 if(memory.sight&&!memory.sight->endSignal)
  for(const auto& next:ahead)if(next.signal!=memory.sight->source&&next.position>memory.sight->start){
   memory.sight->endSignal=next.signal;memory.sight->end=next.position;break;
  }
 std::erase_if(memory.held,[&](const auto& t){return t.hasEnd&&head-length>t.end;});
 auto visibleClear=[&](const Ahead& signal){
  const auto* current=fresh&&signal.signal&&indicationVisible(signal.position,head)?rule(rules,signal.signal):nullptr;
  return current&&(current->flags&NIMBY_DRIVING_CLEAR);
 };
 // Refresh the cancellation point from the current route, as for stop targets.
 for(auto& target:memory.stops)if(target.cancelAt.signal)
  for(const auto& signal:ahead)if(signal.signal==target.cancelAt.signal){target.cancelAt=signal;break;}
 std::erase_if(memory.stops,[&](const auto& target){return visibleClear(target.cancelAt);});
 Plan result{false,vmax};
 for(size_t i=0;i<ahead.size();++i){
  const auto& source=ahead[i];if(!indicationVisible(source.position,head))continue;
  const auto* r=rule(rules,source.signal);if(!r)continue;
  if(!fresh){result.active=true;result.ceiling=std::min(result.ceiling,envelope(0,source.position-head,braking));continue;}
  if((r->flags&NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR)&&i+1<ahead.size()&&visibleClear(ahead[i+1]))continue;
  if(r->speed_mps<0)continue;
  // A visible numeric announcement starts an approach to its TARGET panel.
  // Waiting until the source is crossed can waste the last 200 m of braking
  // distance. Looking at it still does not create retained memory.
  const auto targetIndex=i+r->signals_ahead;
  // Missing targets remain restrictive without fabricating persistent memory.
  const double position=targetIndex<ahead.size()?ahead[targetIndex].position:source.position;
  double speed=targetIndex<ahead.size()?r->speed_mps:0;
  if(restrictedEntry(memory,*r,head,clearance,fresh))
   speed=(r->flags&NIMBY_DRIVING_STOP_THEN_PROCEED)?r->reopened_speed_mps:r->speed_mps;
  if((r->flags&NIMBY_DRIVING_FOLLOW_TARGET)&&targetIndex<ahead.size()) {
   const auto* targetRule=indicationVisible(position,head)?rule(rules,ahead[targetIndex].signal):nullptr;
   // Only a direct numeric target may replace the announcement's zero.
   // In particular, another announcement or missing target cannot imply clear.
   speed=targetRule&&targetRule->signals_ahead==0&&targetRule->speed_mps>=0?targetRule->speed_mps:0;
  }
  result.active=true;result.ceiling=std::min(result.ceiling,envelope(speed,position-head,braking));
 }
 for(auto& stop:memory.stops){
  // Refresh positions from the actual native route, rather than moving the
  // instruction to an unrelated signal after a path update.
  const auto at=std::find_if(ahead.begin(),ahead.end(),[&](const auto& a){return a.signal==stop.signal;});
  if(at!=ahead.end())stop.position=at->position;
  // Keep the received stop instruction outside the visibility horizon. A
  // remote reopening must not change the driver's retained approach curve.
  const auto* current=indicationVisible(stop.position,head)?rule(rules,stop.signal):nullptr;
  const bool open=fresh&&current&&(current->flags&NIMBY_DRIVING_CLEAR);
  result.active=true;
  const double targetSpeed=current&&restrictedEntry(memory,*current,head,clearance,fresh)?
   ((current->flags&NIMBY_DRIVING_STOP_THEN_PROCEED)?current->reopened_speed_mps:current->speed_mps):open?stop.reopenedSpeed:
   fresh&&current&&stop.followTarget&&current->signals_ahead==0&&current->speed_mps>=0?current->speed_mps:stop.speed;
  result.ceiling=std::min(result.ceiling,envelope(targetSpeed,stop.position-head,braking));
 }
 // After passage, keep the same approach curve to the announced location,
 // then the numeric ceiling. The source panel is not the speed-limit location.
 // Rear clearance of an explicit clear indication releases the retained limit.
 for(const auto& held:memory.held){result.active=true;
  result.ceiling=std::min(result.ceiling,envelope(held.speed,held.start-head,braking));}
 // Restricted-mode clearance is supplied separately by the runtime, from
 // current physical footprints. Until it is verified this mode cannot move.
 if(memory.sight){result.active=true;result.ceiling=std::min(result.ceiling,
  sightCeiling(fresh?clearance:SightClearance{},memory.sight->speed,braking));}
 return result;
}
}
