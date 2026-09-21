#pragma once
#include <nimby/blocks.hpp>
#include <optional>
#include <span>

namespace nimby::engine::automatic {
// Distances follow the train's established route. Fractions are only used to
// project a physical footprint onto a section whose native metric is known.
struct SightSection {
 uint64_t track{};
 double from{},to{},trackLengthM{};
};
struct SightClearance {
 bool verified=false;
 double distanceM=0;
 uint64_t obstacle=0;
};
inline SightClearance sightClearance(uint64_t train,std::span<const SightSection> route,
 std::span<const nimby::TrainFootprint> footprints,bool coverageVerified,double visibilityM) {
 if(!train||!coverageVerified||route.empty()||!std::isfinite(visibilityM)||visibilityM<=0)return {};
 double covered=0,free=visibilityM;uint64_t obstacle=0;
 for(const auto& section:route) {
  if(!section.track||!std::isfinite(section.from)||!std::isfinite(section.to)||
     section.from<0||section.from>1||section.to<0||section.to>1||section.from==section.to||
     !std::isfinite(section.trackLengthM)||section.trackLengthM<=0)return {};
  const double low=std::min(section.from,section.to),high=std::max(section.from,section.to);
  for(const auto& row:footprints) {
   if(!row.train||!row.track||!std::isfinite(row.begin)||!std::isfinite(row.end)||
      row.begin<0||row.end>1||row.end<row.begin)return {};
   if(row.train==train||row.track!=section.track||row.end<low||row.begin>high)continue;
   const double first=section.to>section.from?std::max(section.from,row.begin):std::min(section.from,row.end);
   const double distance=covered+std::abs(first-section.from)*section.trackLengthM;
   if(distance<free){free=distance;obstacle=row.train;}
  }
  covered+=std::abs(section.to-section.from)*section.trackLengthM;
  if(covered>=visibilityM)break;
 }
 // An unobserved continuation is a stopping boundary, not additional free space.
 return {true,std::min(free,covered),obstacle};
}
struct RestrictedMode {
 uint64_t source=0,endSignal=0;
 double start=0,end=0,speed=0;
};
struct StopProof {uint64_t signal=0;double position=0;};
// A train already waiting when a mod starts may not call the motion integrator.
// Observe two native waiting samples separated by actual simulation time.
struct WaitingSample {uint64_t signal=0;double head=0;int64_t ticks=-1;};
inline bool observeWaitingStop(WaitingSample& previous,uint64_t signal,double head,double speed,int64_t ticks) {
 const bool valid=signal&&std::isfinite(head)&&std::isfinite(speed)&&speed>=0&&speed<=.001&&ticks>=0;
 const bool stopped=valid&&previous.signal==signal&&ticks>previous.ticks&&
  std::abs(head-previous.head)<=1e-6;
 previous=valid?WaitingSample{signal,head,ticks}:WaitingSample{};
 return stopped;
}
// Locate the first occupied point using a native, monotone clear-prefix query.
// Return the last proven clear fraction, never the occupied side of the bound.
// The caller must validate geometry and exclude its own physical footprint.
template<class Clear> double clearPrefix(Clear&& clear) {
 if(!clear(0.0))return 0;
 if(clear(1.0))return 1;
 double low=0,high=1;
 for(int i=0;i<28;++i){const double middle=(low+high)*.5;
  if(clear(middle))low=middle;else high=middle;
 }
 return low;
}
inline bool measuredStop(double before,double after,double speed,int64_t ticks,double signalPosition) {
 return ticks>0&&std::isfinite(before)&&std::isfinite(after)&&std::isfinite(speed)&&
  std::isfinite(signalPosition)&&speed>=0&&speed<=0.001&&std::abs(after-before)<=1e-6&&
  signalPosition>=after-1e-6&&signalPosition-after<=1.0;
}
// A bound, not a demanded speed. Missing visibility and a train inside the
// stopping margin both yield zero. National speed values are supplied by mods.
inline double sightCeiling(const SightClearance& view,double maximum,double braking,
 double marginM=5,double responseSeconds=2) {
 if(!view.verified||!std::isfinite(view.distanceM)||view.distanceM<0||
    !std::isfinite(maximum)||maximum<=0||!std::isfinite(braking)||braking<=0||
    !std::isfinite(marginM)||marginM<0||!std::isfinite(responseSeconds)||responseSeconds<0)return 0;
 const double distance=std::max(0.0,view.distanceM-marginM),delay=braking*responseSeconds;
 return std::min(maximum,std::max(0.0,std::sqrt(delay*delay+2*braking*distance)-delay));
}
}
