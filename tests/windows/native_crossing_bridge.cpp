// Actual bridge callbacks with copied synthetic native ranges. The four return
// addresses are supplied explicitly; this fixture does not validate a game ABI.
// Reuse the native fixture's callbacks; its original main is never invoked.
// Renaming main removes C++'s implicit return-zero rule in that unused function.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif
#define main existingAutomaticDrivingRegressionMain
#include "automatic_driving_native.cpp"
#undef main
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace {
size_t crossingChecks=0,crossingRangesCalled=0,crossingChecksCalled=0;
bool crossingMarker=true,crossingWrongSource=false,crossingWrongCaller=false;
bool crossingForeignMotion=false,crossingMovedRevision=false,crossingThrow=false;
bool crossingDirtyError=false;
constexpr DWORD crossingNativeError=0x63a;
uintptr_t crossingCurrentMotion{};
struct CallerRestore {
 uintptr_t previous=fixtureCaller;
 explicit CallerRestore(uintptr_t rva){fixtureCaller=base+rva;}
 ~CallerRestore(){fixtureCaller=previous;}
};
#define CROSS_CHECK(x) do{++crossingChecks;CHECK(x);}while(false)
template<class Call> auto crossedCall(uintptr_t rva,Call&& call){CallerRestore caller(rva);return call();}
uintptr_t __fastcall crossingRangeOriginal(uintptr_t context,uintptr_t range){
 ++crossingRangesCalled;
 auto& r=*reinterpret_cast<NativeRange*>(range);auto* ctx=reinterpret_cast<uintptr_t*>(context);
 if(r.kind==6)return 1;
 const auto ownOccupation=crossedCall(crossingWrongCaller?0x4585ee:0x4585ef,
  [&]{return occupancy(ctx[3],r.track,r.from,r.to);});
 if(!ownOccupation)return 0;
 const auto ownReservation=crossedCall(0x45860c,[&]{return reservation(ctx[4],r.track,r.from,r.to);});
 if(!ownReservation)return 0;
 const auto crossedOccupation=crossedCall(0x4586ad,[&]{return occupancy(ctx[3],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1);});
 if(!crossedOccupation)return 0;
 return crossedCall(0x4586c0,[&]{return reservation(ctx[4],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1);});
}
uint8_t __fastcall crossingCheckOriginal(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,
 uintptr_t motion,uint64_t signal,uint8_t){
 ++crossingChecksCalled;
 if(crossingDirtyError)CROSS_CHECK(GetLastError()==crossingNativeError);
 uint8_t allowed=1;uintptr_t foreign=crossingForeignMotion?motion+1:motion;
 std::array<uintptr_t,3> occ{1,reinterpret_cast<uintptr_t>(&foreign),reinterpret_cast<uintptr_t>(&allowed)};
 std::array<uintptr_t,3> reserved{2,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&allowed)};
 std::array<uintptr_t,10> ctx{};uint64_t sourceId=crossingWrongSource?signal+1:signal;
 ctx[1]=reinterpret_cast<uintptr_t>(&sourceId);ctx[3]=reinterpret_cast<uintptr_t>(occ.data());
 ctx[4]=reinterpret_cast<uintptr_t>(reserved.data());ctx[8]=reinterpret_cast<uintptr_t>(&allowed);
 NativeRange source{reinterpret_cast<uintptr_t>(fakeTrack.data()),0,0,0,6,0,reinterpret_cast<uintptr_t>(&signal)};
 if(crossingMarker)permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&source));
 NativeRange section{source.track,0,1,1000,0,0,0};
 const auto result=permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&section));
 if(crossingMovedRevision){
  // Real native readback changes at the end of the callback, after the refusal.
  // A route with this altered path can no longer publish the old route's cause.
  put(fakeTrack.data(),0x88,1001.0);
 }
 SetLastError(crossingNativeError);
 if(crossingThrow)throw std::runtime_error("native crossing fixture exception");
 return result?allowed:0;
}
uint32_t crossingRowFlags(){
 uint32_t flags=0;size_t emitted=0;const std::array<uint64_t,1> selected{signalId};
 auto& state=testTrain(trainId);
 const auto reason=state.signalRoute.copyDiagnosed(selected,2,GetTickCount64(),readBytes,
  [&](const NimbySignalRouteObservation& row){++emitted;flags=row.flags;},[](auto){});
 if(reason!=nimby::windows::automatic::RouteCopyReason::Ready)return 0;
 CROSS_CHECK(emitted==1);return flags;
}
void prepareCrossingFixture(uint32_t flags){
 resetTrains();rawRules=true;expiry=GetTickCount64()+100000;
 fakeMotion.fill(0);fakeTrack.fill(0);crossTrack.fill(0);
 put(fakeMotion.data(),0,trainId);put(fakeMotion.data(),0x3c0,1000.0);
 put(fakeMotion.data(),0x3a8,trackId);put(fakeMotion.data(),0x3b0,0.0);put(fakeMotion.data(),0x3b8,int8_t{1});
 put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x320,uint8_t{1});
 put(fakeMotion.data(),0x338,reinterpret_cast<uintptr_t>(pathIds.data()));
 put(fakeMotion.data(),0x340,reinterpret_cast<uintptr_t>(pathIds.data()+pathIds.size()));
 put(fakeMotion.data(),0x348,reinterpret_cast<uintptr_t>(pathIds.data()+pathIds.size()));
 put(fakeTrack.data(),0,trackId);put(fakeTrack.data(),0x88,1000.0);
 put(crossTrack.data(),0,trackId+0x10000);put(crossTrack.data(),0x88,1000.0);
 crossingCurrentMotion=reinterpret_cast<uintptr_t>(fakeMotion.data());
 auto& state=testTrain(trainId);state.motion=crossingCurrentMotion;state.managed=true;state.ahead={{signalId,1000}};
 rulePublishers.rows()={{signalId,flags?15.0/3.6:120.0/3.6,30.0/3.6,0,flags}};
 refreshRawRules();captureSourceApproach(state,crossingCurrentMotion,1000);
 state.signalRoute.append(reinterpret_cast<uintptr_t>(fakeTrack.data()),0,1,1000,0,readBytes);
 obstruction=2;reservationConflict=crossConflict=crossReservationConflict=controllerConflict=false;
 crossingMarker=true;crossingWrongSource=crossingWrongCaller=crossingForeignMotion=crossingMovedRevision=crossingThrow=false;
 crossingDirtyError=false;fixtureCaller=0;
}
uint8_t crossingRequest(){
 return check(0,0,0,0,0,crossingCurrentMotion,signalId,0);
}
void crossingRegression(){
 using namespace nimby::windows::automatic;
 const uint32_t causeMask=NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED;
 for(uint32_t policy:{uint32_t{0},uint32_t{NIMBY_DRIVING_ON_SIGHT}}){
  for(bool reservationDenial:{false,true}){
   prepareCrossingFixture(policy);crossConflict=!reservationDenial;crossReservationConflict=reservationDenial;
   const auto before=crossingChecksCalled,rangeBefore=crossingRangesCalled;
   CROSS_CHECK(!crossingRequest());CROSS_CHECK(crossingChecksCalled==before+1&&crossingRangesCalled==rangeBefore+2);
   CROSS_CHECK(permissionQuery==nullptr);
   const auto flags=crossingRowFlags();
   CROSS_CHECK((flags&causeMask)==(reservationDenial?NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED:NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED));
   CROSS_CHECK(!(flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR));
   CROSS_CHECK(bool(flags&NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED)==!policy);
   // A subsequent coherent native clear replaces, rather than retains, its old
   // crossed reason. On-sight traversal is not promoted to native clearance.
   crossConflict=crossReservationConflict=false;CROSS_CHECK(crossingRequest());
   const auto next=crossingRowFlags();CROSS_CHECK(!(next&causeMask));
   CROSS_CHECK(bool(next&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR)==!policy);
  }
 }
 for(unsigned invalid=0;invalid<7;++invalid){
  prepareCrossingFixture(0);crossConflict=true;
  if(invalid==0)crossingMarker=false;
  if(invalid==1)crossingWrongSource=true;
  if(invalid==2)crossingWrongCaller=true;
  if(invalid==3)crossingForeignMotion=true;
  if(invalid==4)crossingMovedRevision=true;
  if(invalid==5){crossConflict=false;obstruction=.6;} // Longitudinal occupation refusal.
  if(invalid==6){crossConflict=false;reservationConflict=true;} // Longitudinal reservation refusal.
  CROSS_CHECK(!crossingRequest());CROSS_CHECK(!(crossingRowFlags()&causeMask));
 }
 // The incoming context error and original exception are propagated. No
 // diagnostic callback can replay the native traversal while unwinding.
 prepareCrossingFixture(0);crossConflict=true;crossingDirtyError=true;crossingThrow=true;
 SetLastError(crossingNativeError);const auto before=crossingChecksCalled;bool thrown=false;
 try{crossingRequest();}catch(const std::runtime_error& e){thrown=std::string_view(e.what())=="native crossing fixture exception";}
 CROSS_CHECK(thrown&&crossingChecksCalled==before+1);CROSS_CHECK(GetLastError()==crossingNativeError);
 CROSS_CHECK(permissionQuery==nullptr);CROSS_CHECK(!(crossingRowFlags()&causeMask));
 // Existing per-train quota remains effective even with the diagnostic reads.
 prepareCrossingFixture(0);crossConflict=true;routeClockFixture=true;routeClockTick=1000000;routeReadCost=400;
 CROSS_CHECK(!crossingRequest());routeClockFixture=false;routeReadCost=0;
 CROSS_CHECK(!(crossingRowFlags()&causeMask));
}
}
int main(){try{
 std::vector<unsigned char> module(0xb81998+8),root(0xb40);
 base=reinterpret_cast<uintptr_t>(module.data());session=0x12340000;
 put(module.data(),0xb81998,reinterpret_cast<uintptr_t>(root.data()));put(root.data(),0x680,session);
 nativeOccupancy=fakeOccupancy;nativeReservation=fakeReservation;
 nativePermissionRange=crossingRangeOriginal;nativeCheck=crossingCheckOriginal;
 crossingRegression();std::cout<<crossingChecks<<" actual crossed-refusal wrapper checks PASS; no game ABI assertion\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
