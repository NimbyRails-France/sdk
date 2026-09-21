// Exercise the actual bridge wrappers with controlled native callbacks. This
// verifies permission scoping; it does not validate game addresses or ABI.
#include "../src/runtime/automatic_driving_bridge.cpp"
#include <iostream>
#include <stdexcept>
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t trainId=0x5000000000001,signalId=0x8000000000001;
std::array<unsigned char,0x500> fakeMotion{};
std::array<unsigned char,0x100> fakeTrack{},crossTrack{};
double obstruction=.6;
bool reservationConflict=false,crossConflict=false,invalidGeometry=false;
double appliedDistance=-1,appliedTarget=-1,appliedCeiling=-1;
template<class T> void put(void* at,size_t offset,T value){std::memcpy(static_cast<unsigned char*>(at)+offset,&value,sizeof value);}
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
 if(!occupancy(ctx[3],r.track,r.from,r.to))return 0;
 if(reservationConflict){*reinterpret_cast<uint8_t*>(ctx[8])=0;return 0;}
 return occupancy(ctx[3],reinterpret_cast<uintptr_t>(crossTrack.data()),0,1);
}
uint8_t __fastcall fakeCheck(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t motion,uint64_t signal,uint8_t) {
 uint8_t allowed=1;std::array<uintptr_t,3> occ{1,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&allowed)};
 std::array<uintptr_t,10> ctx{};ctx[3]=reinterpret_cast<uintptr_t>(occ.data());ctx[8]=reinterpret_cast<uintptr_t>(&allowed);
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
 const auto motion=reinterpret_cast<uintptr_t>(fakeMotion.data());
 nativeOccupancy=fakeOccupancy;nativePermissionRange=fakeRange;nativeCheck=fakeCheck;
 active=1;expiry=GetTickCount64()+100000;
 trains[trainId].motion=motion;trains[trainId].ahead={{signalId,1000}};
 rules={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP}};
 auto request=[&]{return check(0,0,0,0,0,motion,signalId,0);};
 // An absolute stop cannot inherit permission, even with an old stop proof.
 trains[trainId].memory.stopped={signalId,1000};CHECK(!request());
 rules[0].flags|=NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED;
 trains[trainId].memory.stopped={};CHECK(!request());
 trains[trainId].memory.stopped={signalId,1000};CHECK(request());
 CHECK(permissionQuery==nullptr);CHECK(trains[trainId].entry.at(1000).distanceM==200); // Simulated visibility bound.
 obstruction=.12;CHECK(request());CHECK(trains[trainId].entry.at(1000).distanceM>119.99&&trains[trainId].entry.at(1000).distanceM<=120);
 obstruction=.6;
 // Same-track occupation may be permissive; reservations and crossings may not.
 reservationConflict=true;CHECK(!request());reservationConflict=false;
 crossConflict=true;CHECK(!request());crossConflict=false;
 obstruction=.004;CHECK(!request());obstruction=.6;
 invalidGeometry=true;CHECK(!request());invalidGeometry=false;
 expiry=GetTickCount64();CHECK(!request());expiry=GetTickCount64()+100000;
 rules[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 trains[trainId].memory.stopped={};CHECK(request());
 rules[0]={signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP};CHECK(!request());
 // No rule is an ordinary native check, never an implicit permissive mode.
 rules.clear();CHECK(!request());CHECK(permissionQuery==nullptr);
 // Exercise the actual integration wrapper: stop evidence comes from elapsed
 // simulated time and measured displacement, including the game's idle floor.
 std::array<unsigned char,0x100> network{},config{},result{};
 put(network.data(),0x10,reinterpret_cast<uintptr_t>(config.data()));put(config.data(),0xd0,1.25);
 float material[8]{200.0f/3.6f,1,.5f,0,0,0,40000,100};
 std::memcpy(fakeMotion.data()+0x24,material,sizeof material);put(fakeMotion.data(),0x3c8,.01);
 nativeIntegrate=fakeIntegrate;
 rules={{signalId,0,30.0/3.6,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
 auto& state=trains[trainId];state.managed=true;state.memory={};state.entry={};
 auto step=[&]{integrate(reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(network.data()),0,
  motion+0x3a0,motion+8,0,200.0/3.6,1,.5,1000,0,1,0);};
 step();CHECK(state.memory.stopped.signal==signalId);CHECK(request());
 state.memory.sight=RestrictedMode{signalId,signalId+1,1000,2000,30.0/3.6};state.ahead.clear();
 state.view={1000,200,20,GetTickCount64(),true};step();
 CHECK(appliedDistance==15&&appliedTarget==0&&appliedCeiling<=30.0/3.6);
 state.view.valid=false;step();CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0);
 integrate(reinterpret_cast<uintptr_t>(result.data()),reinterpret_cast<uintptr_t>(network.data()),0,
  motion+0x3a0,motion+8,0,200.0/3.6,1,.5,0,15.0/3.6,1,0);
 CHECK(appliedDistance==0&&appliedTarget==0); // Equal-distance target must also tighten to zero.
 state.view={1000,200,200,GetTickCount64(),true};expiry=GetTickCount64();step();
 CHECK(appliedDistance==0&&appliedTarget==0&&appliedCeiling==0);
 // A loaded train can already be waiting: no scan/integration has populated
 // its cache. Repeated callbacks during pause must not prove an actual stop.
 std::array<unsigned char,0x30> waitingSimulation{};
 session=reinterpret_cast<uintptr_t>(waitingSimulation.data());put(root.data(),0x680,session);
 expiry=GetTickCount64()+100000;trains.clear();
 put(fakeMotion.data(),0x458,signalId);put(fakeMotion.data(),0x4b0,uint8_t{1});put(fakeMotion.data(),0x3c8,0.0);
 put(waitingSimulation.data(),0x28,int64_t{100});
 CHECK(!request());CHECK(!request());CHECK(!trains[trainId].memory.stopped.signal);
 put(waitingSimulation.data(),0x28,int64_t{101});CHECK(request());
 CHECK(trains[trainId].memory.stopped.signal==signalId);
 CHECK(!trains[trainId].boundary.empty());
 trains.clear();rules[0]={signalId,15.0/3.6,30.0/3.6,0,NIMBY_DRIVING_ON_SIGHT};
 CHECK(request()); // Red flashing requires no stop proof.
 trains.clear();put(fakeMotion.data(),0x458,signalId+1);CHECK(!request());
 CHECK(trains.empty()); // A different waiting signal cannot seed permission.
 std::cout<<"PASS: explicit permission, measured-stop requirement, absolute stop, reservations, crossing tracks, gap, geometry, lease and scope\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
