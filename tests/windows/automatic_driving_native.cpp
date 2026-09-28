// Exercise the actual bridge wrappers with controlled native callbacks. This
// verifies permission scoping; it does not validate game addresses or ABI.
#include "../../src/platform/windows/runtime/automatic_driving_bridge.cpp"
#include <iostream>
#include <stdexcept>
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t trainId=0x5000000000001,signalId=0x8000000000001;
constexpr uint64_t trackId=0x1000000000001;
std::array<unsigned char,0x500> fakeMotion{};
std::array<unsigned char,0x100> fakeTrack{},crossTrack{};
std::array<uint64_t,1> pathIds{trackId};
double obstruction=.6;
bool reservationConflict=false,crossConflict=false,crossReservationConflict=false,controllerConflict=false,invalidGeometry=false;
bool repeatFollowedRange=false,repeatOccupation=false,skipOccupation=false,wrongReservationMotion=false;
int reservationChecks=0;
double appliedDistance=-1,appliedTarget=-1,appliedCeiling=-1;
template<class T> void put(void* at,size_t offset,T value){std::memcpy(static_cast<unsigned char*>(at)+offset,&value,sizeof value);}
uint32_t __fastcall fakeStep(uintptr_t context,uintptr_t train,uintptr_t motion,uintptr_t service,double mass,uintptr_t budget){
 CHECK(currentStep.context==context&&currentStep.motion==motion);
 CHECK(train==11&&service==22&&mass==3.75&&budget==33);
 {StepScope nested(44,55);CHECK(currentStep.context==44&&currentStep.motion==55);}
 CHECK(currentStep.context==context&&currentStep.motion==motion);
 return 17;
}
uintptr_t __fastcall fakeScan(uintptr_t,uintptr_t){return 1;}
uintptr_t __fastcall fakeIntegrate(uintptr_t result,uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,
 double ceiling,double,double,double distance,double target,int64_t ticks,uintptr_t) {
 appliedDistance=distance;appliedTarget=target;appliedCeiling=ceiling;
 put(reinterpret_cast<void*>(result),0x30,ticks);return result;
}
struct NativeRange {uintptr_t track;double from,to,length;int kind,padding;uintptr_t signal;};
uint8_t __fastcall fakeOccupancy(uintptr_t context,uintptr_t track,double from,double to) {
 const bool occupied=track==reinterpret_cast<uintptr_t>(crossTrack.data())?crossConflict:
  std::max(from,to)>=obstruction&&std::min(from,to)<=.9;
 if(occupied)*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t*>(context)[2])=0;
 return !occupied;
}
uintptr_t __fastcall fakeRange(uintptr_t context,uintptr_t range) {
 auto& r=*reinterpret_cast<NativeRange*>(range);auto* ctx=reinterpret_cast<uintptr_t*>(context);
 if(r.kind==6)return 1;
 if(!skipOccupation&&!occupancy(ctx[3],r.track,r.from,r.to))return 0;
 if(!reservation(ctx[4],r.track,r.from,r.to))return 0;
 if(repeatFollowedRange&&!reservation(ctx[4],r.track,r.from,r.to))return 0;
 if(repeatOccupation&&!occupancy(ctx[3],r.track,r.from,r.to))return 0;
 if(!occupancy(ctx[3],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1))return 0;
 if(!reservation(ctx[4],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1))return 0;
 if(controllerConflict){*reinterpret_cast<uint8_t*>(ctx[8])=0;return 0;}
 return 1;
}
uint8_t __fastcall fakeReservation(uintptr_t context,uintptr_t track,double,double) {
 ++reservationChecks;
 const bool conflict=track==reinterpret_cast<uintptr_t>(crossTrack.data())?crossReservationConflict:reservationConflict;
 if(conflict)*reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t*>(context)[2])=0;
 return !conflict;
}
uint8_t __fastcall fakeCheck(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t motion,uint64_t signal,uint8_t) {
 uint8_t allowed=1;std::array<uintptr_t,3> occ{1,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&allowed)};
 uintptr_t reservationMotion=wrongReservationMotion?motion+1:motion;
 std::array<uintptr_t,3> reserved{2,reinterpret_cast<uintptr_t>(&reservationMotion),reinterpret_cast<uintptr_t>(&allowed)};
 std::array<uintptr_t,10> ctx{};ctx[3]=reinterpret_cast<uintptr_t>(occ.data());ctx[8]=reinterpret_cast<uintptr_t>(&allowed);
 ctx[4]=reinterpret_cast<uintptr_t>(reserved.data());
 NativeRange source{reinterpret_cast<uintptr_t>(fakeTrack.data()),0,0,0,6,0,reinterpret_cast<uintptr_t>(&signal)};
 permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&source));
 NativeRange section{source.track,0,1,invalidGeometry?50.0:1000.0,0,0,0};
 permissionRange(reinterpret_cast<uintptr_t>(ctx.data()),reinterpret_cast<uintptr_t>(&section));
 return allowed;
}
}
int main(){try{
 std::vector<unsigned char> module(0xb81998+8),root(0x688);
 base=reinterpret_cast<uintptr_t>(module.data());session=0x12340000;
 put(module.data(),0xb81998,reinterpret_cast<uintptr_t>(root.data()));put(root.data(),0x680,session);
 put(fakeMotion.data(),0,trainId);put(fakeMotion.data(),0x3c0,1000.0);put(fakeTrack.data(),0x88,1000.0);
 put(fakeTrack.data(),0,trackId);put(fakeMotion.data(),0x3a8,trackId);put(fakeMotion.data(),0x3b8,int8_t{1});
 put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x320,uint8_t{1});
 put(fakeMotion.data(),0x338,reinterpret_cast<uintptr_t>(pathIds.data()));
 put(fakeMotion.data(),0x340,reinterpret_cast<uintptr_t>(pathIds.data()+pathIds.size()));
 put(fakeMotion.data(),0x348,reinterpret_cast<uintptr_t>(pathIds.data()+pathIds.size()));
 const auto motion=reinterpret_cast<uintptr_t>(fakeMotion.data());
 nativeOccupancy=fakeOccupancy;nativeReservation=fakeReservation;nativePermissionRange=fakeRange;nativeCheck=fakeCheck;
 nativeStep=fakeStep;CHECK(stepMotion(66,11,motion,22,3.75,33)==17);
 CHECK(!currentStep.context&&!currentStep.motion);
 active=1;expiry=GetTickCount64()+100000;
 trains[trainId].motion=motion;trains[trainId].ahead={{signalId,1000}};
 rules={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP}};
 auto request=[&]{return check(0,0,0,0,0,motion,signalId,0);};
 // An absolute stop cannot inherit permission, even with an old stop proof.
 trains[trainId].memory.stopped={signalId,1000};CHECK(!request());
 rules[0].flags|=NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED;
 trains[trainId].memory.stopped={};CHECK(!request());
 trains[trainId].memory.stopped={signalId,1000};CHECK(request());
 CHECK(permissionQuery==nullptr);CHECK(trains[trainId].entry.at(1000,GetTickCount64()).distanceM==200); // Simulated visibility bound.
 obstruction=.12;CHECK(request());CHECK(trains[trainId].entry.at(1000,GetTickCount64()).distanceM>119.99&&trains[trainId].entry.at(1000,GetTickCount64()).distanceM<=120);
 obstruction=.6;
 // Regression: a stopped train may enter behind a leader even though the
 // game's exclusive reservation still covers this block. Permission is not a
 // measured passage and cannot remove the stop proof or start retained mode.
 reservationConflict=true;const auto beforeChecks=reservationChecks;CHECK(request());
 CHECK(reservationChecks>beforeChecks&&trains[trainId].memory.stopped.signal==signalId&&!trains[trainId].memory.sight);
 crossConflict=true;CHECK(!request());crossConflict=false;
 crossReservationConflict=true;CHECK(!request());crossReservationConflict=false;
 controllerConflict=true;CHECK(!request());controllerConflict=false;
 // Only the first longitudinal query is replaced, not an identical later
 // crossing query, another motion, or a call without physical observation.
 repeatFollowedRange=true;CHECK(!request());repeatFollowedRange=false;
 repeatOccupation=true;CHECK(!request());repeatOccupation=false;
 wrongReservationMotion=true;CHECK(!request());wrongReservationMotion=false;
 skipOccupation=true;CHECK(!request());skipOccupation=false;
 obstruction=.004;CHECK(!request());obstruction=.6;
 invalidGeometry=true;CHECK(!request());invalidGeometry=false;
 expiry=GetTickCount64();CHECK(!request());expiry=GetTickCount64()+100000;
 rules[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 trains[trainId].memory.stopped={};CHECK(request());
 rules[0]={signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP};CHECK(!request());
 // No rule is an ordinary native check, never an implicit permissive mode.
 rules.clear();CHECK(!request());CHECK(permissionQuery==nullptr);
 // Outside the permission scope both native reservations and their output
 // byte are unchanged, even when the physical route is empty.
 uint8_t allowed=1;uintptr_t ownMotion=motion;
 std::array<uintptr_t,3> unscoped{2,reinterpret_cast<uintptr_t>(&ownMotion),reinterpret_cast<uintptr_t>(&allowed)};
 CHECK(!reservation(reinterpret_cast<uintptr_t>(unscoped.data()),reinterpret_cast<uintptr_t>(fakeTrack.data()),0,1)&&!allowed);
 reservationConflict=false;
 skipOccupation=true;obstruction=2;rules={{signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT}};
 CHECK(!request()); // Native approval with no physical query cannot grant entry.
 skipOccupation=false;obstruction=.6;
 // Exercise the actual integration wrapper: stop evidence comes from elapsed
 // simulated time and measured displacement, including the game's idle floor.
 std::array<unsigned char,0x100> network{},config{},result{},worker{};
 put(network.data(),0x10,reinterpret_cast<uintptr_t>(config.data()));put(config.data(),0xd0,1.25);
 put(worker.data(),8,reinterpret_cast<uintptr_t>(network.data()));put(worker.data(),0x68,uintptr_t{1});
 float material[8]{200.0f/3.6f,1,.5f,0,0,0,40000,100};
 std::memcpy(fakeMotion.data()+0x24,material,sizeof material);put(fakeMotion.data(),0x3c8,.01);
 nativeIntegrate=fakeIntegrate;
 rules={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
 auto& state=trains[trainId];state.managed=true;state.memory={};state.entry={};
 auto step=[&]{put(worker.data(),0x18,session);StepScope scope(reinterpret_cast<uintptr_t>(worker.data()),motion);
  integrate(reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(network.data()),motion+0x290,
  motion+0x3a0,motion+8,0,200.0/3.6,1,.5,1000,0,1,0);};
 step();CHECK(state.memory.stopped.signal==signalId);CHECK(request());
 state.memory.sight=RestrictedMode{signalId,signalId+1,1000,2000,30.0/3.6};state.ahead.clear();
 // No new scan: the native occupation query sees an obstacle appear, move,
 // then disappear. An old view is refreshed before every movement calculation.
 state.view={1000,200,200,GetTickCount64()-1000,true};obstruction=.02;step();
 CHECK(std::abs(appliedDistance-15)<1e-5&&appliedTarget==0&&appliedCeiling<=30.0/3.6);
 obstruction=.004;step();CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0);
 obstruction=.6;state.view.observed=0;step();CHECK(appliedCeiling==30.0/3.6&&appliedDistance==195);
 // No current worker scope cannot borrow another worker's old occupancy map.
 integrate(reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(network.data()),0,
  motion+0x3a0,motion+8,0,200.0/3.6,1,.5,0,15.0/3.6,1,0);
 CHECK(appliedDistance==0&&appliedTarget==0); // Equal-distance target must also tighten to zero.
 put(worker.data(),8,uintptr_t{0});step();CHECK(appliedCeiling==0);
 put(worker.data(),8,reinterpret_cast<uintptr_t>(network.data()));
 expiry=GetTickCount64();step();
 CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0);
 // A loaded train can already be waiting: no scan/integration has populated
 // its cache. Repeated callbacks during pause must not prove an actual stop.
 std::array<unsigned char,0x30> waitingSimulation{};
 session=reinterpret_cast<uintptr_t>(waitingSimulation.data());put(root.data(),0x680,session);
 expiry=GetTickCount64()+100000;trains.clear();
 put(fakeMotion.data(),0x458,signalId);put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x3c8,0.0);
 put(waitingSimulation.data(),0x28,int64_t{100});
 CHECK(!request());CHECK(!request());CHECK(!trains[trainId].memory.stopped.signal);
 reservationConflict=true;
 put(waitingSimulation.data(),0x28,int64_t{101});CHECK(request());
 CHECK(trains[trainId].memory.stopped.signal==signalId);
 CHECK(!trains[trainId].boundary.empty());
 reservationConflict=false;
 trains.clear();rules[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 CHECK(request()); // Red flashing requires no stop proof.
 trains.clear();put(fakeMotion.data(),0x458,signalId+1);CHECK(!request());
 CHECK(trains.empty()); // A different waiting signal cannot seed permission.
 // Commands reach the real adapter, not just the standalone planner.
 installed=true;rules.clear();trains.clear();
 NimbyTrainConstraint command{trainId,signalId,1,6.75,0,0};
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);
 auto& controlled=trains[trainId];controlled.motion=motion;controlled.ahead={{signalId,1200}};
 step();CHECK(appliedCeiling==6.75);
 NimbyTrainConstraintStatus status{};status.size=sizeof status;
 CHECK(NimbyDriving_ReadTrainConstraint(trainId,&status)==NIMBY_OK&&status.state==2&&status.exit_signal==signalId);
 auto bad=command;bad.speed_mps=-1;CHECK(NimbyDriving_TrainConstraints(&bad,1,1000,91)==NIMBY_INVALID_ARGUMENT);
 step();CHECK(appliedCeiling==6.75); // bad publication leaves command intact
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,92)==NIMBY_RESOURCE_LIMIT);
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,92)==NIMBY_OK);
 step();CHECK(appliedCeiling==6.75); // another publisher cannot erase this command
 put(fakeMotion.data(),0x3c0,1201.0);put(fakeMotion.data(),0x3b0,.201);step();
 CHECK(NimbyDriving_ReadTrainConstraint(trainId,&status)==NIMBY_OK&&status.state==3);
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);step();CHECK(appliedCeiling>6.75);
 command.revision=2;command.exit_signal=signalId+1;command.mode=1;
 controlled.ahead={{signalId+1,1400}};
 controlled.physicalRoute.begin(motion,trainId,1201,readBytes);
 controlled.physicalRoute.append(reinterpret_cast<uintptr_t>(fakeTrack.data()),.201,1,799,0,readBytes);
 CHECK(NimbyDriving_TrainConstraints(&command,1,1000,91)==NIMBY_OK);step();CHECK(appliedCeiling==6.75);
 controlled.physicalRoute.clear();step();CHECK(appliedCeiling==0&&appliedDistance==0);
 // An ordinary native scan rebuilds geometry for an explicit on-sight command.
 // Its context is distinct from the integrator's Network argument.
 nativeScan=fakeScan;put(worker.data(),0x18,session);
 double offset=0;std::array<uintptr_t,4> scanContext{reinterpret_cast<uintptr_t>(&offset),0,
  reinterpret_cast<uintptr_t>(worker.data()),reinterpret_cast<uintptr_t>(&motion)};
 NativeRange scanSection{reinterpret_cast<uintptr_t>(fakeTrack.data()),.201,1,799,0,0,0};
 CHECK(scan(reinterpret_cast<uintptr_t>(scanContext.data()),reinterpret_cast<uintptr_t>(&scanSection))==1);
 step();CHECK(appliedCeiling==6.75);
 // The current worker can change its map between frames; never reuse a former
 // context or require the old map address to stay alive to refresh geometry.
 put(worker.data(),0x68,uintptr_t{2});step();CHECK(appliedCeiling==6.75);
 put(worker.data(),0x68,uintptr_t{0});step();CHECK(appliedCeiling==0);
 put(worker.data(),0x68,uintptr_t{1});
 constraintExpiry=GetTickCount64();step();CHECK(appliedCeiling>6.75);
 CHECK(NimbyDriving_TrainConstraints(nullptr,0,1000,91)==NIMBY_OK);
 CHECK(NimbyDriving_ReadTrainConstraint(trainId,&status)==NIMBY_OK&&status.state==0);
 std::cout<<"PASS: permission, measured stop, reservations, geometry, lease, scope and native train commands\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
