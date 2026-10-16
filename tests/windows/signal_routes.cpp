#include <platform/windows/runtime/signal_route.h>
#include <platform/windows/runtime/signal_route_diagnostics.h>
#include <platform/windows/runtime/physical_route.h>
#include <nimby/detail/signal_route_values.hpp>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <limits>
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t train=(5ULL<<48)|1,a=(1ULL<<48)|1,b=(1ULL<<48)|0x10001,c=(1ULL<<48)|0x20001;
constexpr uint64_t sa=(8ULL<<48)|1,sb=(8ULL<<48)|0x10001,sc=(8ULL<<48)|0x20001;
template<class T>void put(auto& bytes,size_t offset,T value){std::memcpy(bytes.data()+offset,&value,sizeof value);}
template<class T>T get(const auto& bytes,size_t offset){T value{};std::memcpy(&value,bytes.data()+offset,sizeof value);return value;}
struct Fixture {
 std::array<unsigned char,0x500> motion{};
 std::array<unsigned char,0x420> ta{},tb{},tc{};
 std::array<unsigned char,0x100> siga{},sigb{},sigc{};
 std::array<uint64_t,3> ids{a,b,c};
 std::vector<uint64_t> children;
 nimby::windows::automatic::SignalRoute route;
 std::vector<NimbySignalRouteObservation> rows;
 std::vector<NimbySignalRouteSection> sections;
 std::vector<NimbySignalRouteAnnotation> annotations;
 std::function<void(uintptr_t,size_t)> afterRead;
 bool read(uintptr_t address,void* out,size_t length){
  const auto region=[&](const auto& bytes){const auto begin=reinterpret_cast<uintptr_t>(bytes.data());
   const auto size=bytes.size()*sizeof(*bytes.data());
   return address>=begin&&address-begin<=size&&length<=size-(address-begin);};
  if(!region(motion)&&!region(ta)&&!region(tb)&&!region(tc)&&!region(siga)&&!region(sigb)&&!region(sigc)&&!region(ids)&&!region(children))return false;
  std::memcpy(out,reinterpret_cast<void*>(address),length);if(afterRead)afterRead(address,length);return true;
 }
 auto reader(){return [this](uintptr_t at,void* out,size_t length){return read(at,out,length);};}
 Fixture(){
  put(motion,0,train);put(motion,0x4b0,uint8_t{1});put(motion,0x290+0x90,uint8_t{1});
  const auto first=reinterpret_cast<uintptr_t>(ids.data());
  put(motion,0x290+0xa8,first);put(motion,0x290+0xb0,first+sizeof ids);put(motion,0x290+0xb8,first+sizeof ids);
  put(ta,0,a);put(ta,0x88,1000.0);put(tb,0,b);put(tb,0x88,700.0);put(tc,0,c);put(tc,0x88,400.0);
  const auto signal=[](auto& bytes,uint64_t id,uint64_t track,double fraction){
   put(bytes,0,id);put(bytes,0x30,int32_t{NIMBY_SIGNAL_PATH});put(bytes,0x40,track);put(bytes,0x48,fraction);put(bytes,0x50,int8_t{-1});};
  signal(siga,sa,a,.8);signal(sigb,sb,b,.4);signal(sigc,sc,c,.5);head(a,.1,100,1);build();
 }
 void head(uint64_t track,double fraction,double position,int8_t direction){
  put(motion,0x3a8,track);put(motion,0x3b0,fraction);put(motion,0x3b8,direction);put(motion,0x3c0,position);
 }
 void attachChildren(auto& track,std::initializer_list<uint64_t> values){
  children=values;const auto first=reinterpret_cast<uintptr_t>(children.data());
  put(track,0x408,first);put(track,0x410,first+children.size()*sizeof(uint64_t));put(track,0x418,first+children.size()*sizeof(uint64_t));
 }
 nimby::Signal observedSignal(const auto& bytes)const{
  NimbySignal value{};value.id=get<uint64_t>(bytes,0);value.track_id=get<uint64_t>(bytes,0x40);
  value.track_fraction=get<double>(bytes,0x48);value.direction=get<int8_t>(bytes,0x50);value.kind=get<int32_t>(bytes,0x30);
  return nimby::Signal(value);
 }
 void build(uint64_t now=1000,bool observeObjects=true){
  auto r=reader();route.begin(reinterpret_cast<uintptr_t>(motion.data()),train,100,now,r,observeObjects);
  route.append(reinterpret_cast<uintptr_t>(ta.data()),.1,.8,700,0,r);route.boundary(reinterpret_cast<uintptr_t>(siga.data()),sa,700,r);
  route.append(reinterpret_cast<uintptr_t>(ta.data()),.8,1,200,700,r);route.append(reinterpret_cast<uintptr_t>(tb.data()),0,.4,280,900,r);
  route.boundary(reinterpret_cast<uintptr_t>(sigb.data()),sb,1180,r);route.append(reinterpret_cast<uintptr_t>(tb.data()),.4,1,420,1180,r);
  route.append(reinterpret_cast<uintptr_t>(tc.data()),0,.5,200,1600,r);route.boundary(reinterpret_cast<uintptr_t>(sigc.data()),sc,1800,r);
  route.append(reinterpret_cast<uintptr_t>(tc.data()),.5,1,200,1800,r);
 }
 bool copy(std::span<const uint64_t> selected,uint64_t now=1100){
  rows.clear();sections.clear();const bool good=route.copy(selected,2,now,reader(),
   [&](auto row){rows.push_back(row);},[&](auto row){sections.push_back(row);});
  if(!good){rows.clear();sections.clear();}return good;
 }
 bool copyV2(std::span<const uint64_t> selected,uint64_t now=1100){
  rows.clear();sections.clear();annotations.clear();
  const auto reason=route.copyDiagnosed(selected,2,now,reader(),
   [&](auto row){rows.push_back(row);},[&](auto row){sections.push_back(row);},[&](auto row){annotations.push_back(row);});
  if(reason!=nimby::windows::automatic::RouteCopyReason::Ready){rows.clear();sections.clear();annotations.clear();return false;}
  return true;
 }
};
void ordered(){
 Fixture f;const std::array<uint64_t,2> selected{sa,sb};CHECK(f.copy(selected));CHECK(f.rows.size()==2);
 CHECK(f.rows[0].signal_id==sa&&f.rows[0].blocks==1&&f.rows[0].distance_m==700&&f.rows[0].next_signal_id==sb);
 CHECK(f.rows[1].signal_id==sb&&f.rows[1].blocks==2&&f.rows[1].next_signal_id==sc);
 CHECK(f.rows[0].next_track_id==b&&f.rows[0].next_fraction==.4&&f.rows[0].next_direction==1&&f.rows[0].next_kind==NIMBY_SIGNAL_PATH);
 CHECK(f.rows[1].next_track_id==c&&f.rows[1].next_fraction==.5&&f.rows[1].next_direction==1&&f.rows[1].next_kind==NIMBY_SIGNAL_PATH);
 CHECK(f.sections.size()==4);CHECK(f.sections[0].track_id==a&&f.sections[0].begin_fraction==.8);
 nimby::detail::SignalRoutes routes(f.rows,f.sections);
 NimbySignal source{};source.id=sa;source.track_id=a;source.track_fraction=.8;source.direction=-1;source.kind=NIMBY_SIGNAL_PATH;
 const auto next=f.observedSignal(f.sigb);
 const auto block=routes.block(*routes.at(sa,train),nimby::Signal(source),&next);CHECK(block&&block->hasBoundary()&&block->nextSignal==sb);
 const nimby::BlockReader clear({},true);CHECK(block->occupation(clear)==nimby::BlockOccupancy::Clear);
 const std::array<nimby::TrainFootprint,1> occupied{{{train+1,b,.2,.3}}};
 CHECK(block->occupation(nimby::BlockReader(occupied,true))==nimby::BlockOccupancy::Occupied);
 source.direction=1;CHECK(!routes.block(*routes.at(sa,train),nimby::Signal(source),&next));
 source.direction=-1;source.track_fraction=.9;CHECK(!routes.block(*routes.at(sa,train),nimby::Signal(source),&next));
 const std::array<uint64_t,1> farSelection{sc};CHECK(f.copy(farSelection));CHECK(f.rows.empty());
}
void invalidate(){
 const std::array<uint64_t,2> selected{sa,sb};
 Fixture path;const auto old=path.route.revision();path.ids[1]=c;CHECK(!path.copy(selected));path.build();CHECK(path.route.revision()>old);
 Fixture geometry;put(geometry.tb,0x88,701.0);CHECK(!geometry.copy(selected));
 Fixture identity;put(identity.tb,0,b+1);CHECK(!identity.copy(selected));
 Fixture signal;put(signal.siga,0x50,int8_t{1});CHECK(!signal.copy(selected));
 Fixture moved;put(moved.siga,0x48,.9);CHECK(!moved.copy(selected));
 Fixture stale;CHECK(!stale.copy(selected,2001));
 Fixture hidden;put(hidden.motion,0x4b0,uint8_t{0});CHECK(!hidden.copy(selected));
 Fixture head;head.head(b,.1,100,1);CHECK(!head.copy(selected));
 Fixture changing;bool changed=false;
 changing.afterRead=[&](uintptr_t address,size_t){if(!changed&&address==reinterpret_cast<uintptr_t>(changing.siga.data())+0x50){changed=true;changing.ids[1]=c;}};
 CHECK(!changing.copy(selected));CHECK(changing.rows.empty());
}
void movement(){
 const std::array<uint64_t,2> selected{sa,sb};Fixture f;
 f.head(a,.8,800,1);CHECK(f.copy(selected));CHECK(f.rows[0].signal_id==sa&&f.rows[0].distance_m==0);
 f.head(a,.8+nimby::detail::trackFractionRounding/2,800+1000*nimby::detail::trackFractionRounding/2,1);
 CHECK(f.copy(selected));CHECK(f.rows[0].signal_id==sa);
 f.head(a,.801,801,1);CHECK(f.copy(selected));CHECK(f.rows.size()==1&&f.rows[0].signal_id==sb&&f.rows[0].blocks==1);
 f.head(b,0,1000,1);CHECK(f.copy(selected));CHECK(f.rows.size()==1&&f.rows[0].signal_id==sb);
 auto read=f.reader();f.route.admission(sb,false,1100,read);CHECK(f.copy(selected));CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED);
 f.route.admission(sb,true,1100,read);CHECK(f.copy(selected));CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
}
void crossingAdmission(){
 const std::array<uint64_t,1> selected{sa};Fixture f;auto r=f.reader();
 for(const auto cause:{NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED,NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED}){
  f.route.admission(sa,false,1100,r,cause);CHECK(f.copy(selected));
  CHECK(f.rows.size()==1&&(f.rows[0].flags&cause)&&(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED));
  f.route.admission(sa,false,1100,r,cause,false);CHECK(f.copy(selected));
  CHECK((f.rows[0].flags&cause)&&!(f.rows[0].flags&(NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR|NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED)));
  f.route.admission(sa,false,1100,r,0,false);CHECK(f.copy(selected));
  CHECK(!(f.rows[0].flags&(NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED)));
 }
 f.route.admission(sa,false,1100,r,NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED);
 f.build(2000);CHECK(f.copy(selected,2100));CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED);
 f.build(2101);CHECK(f.copy(selected,2101));
 CHECK(!(f.rows[0].flags&(NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED)));
 f.route.admission(sa,true,2101,r);CHECK(f.copy(selected,2101));CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
 f.route.admission(sa,false,2101,r,NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED);
 CHECK(f.copy(selected,2101));CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
 f.route.admission(sa,true,2101,r,NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED);
 CHECK(f.copy(selected,2101));CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
 f.route.admission(sa,false,2101,r,NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED);
 put(f.siga,0x48,.9);CHECK(!f.copy(selected,2101));
 // The next native scan sees the edit, retires its old admission, then builds
 // a new coherent route once the source is placed at its original anchor.
 f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,2101,r);
 put(f.siga,0x48,.8);f.build(2101);CHECK(f.copy(selected,2101));CHECK(!(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED));
}
void previousTrackContact(){
 Fixture f;put(f.siga,0x48,1.0);auto r=f.reader();
 f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,1000,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.1,1,900,0,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,900,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.4,280,900,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,1180,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),.4,1,420,1180,r);
 const std::array<uint64_t,2> selected{sa,sb};
 // At the old signal on its own track, its outgoing block begins on b.
 f.head(a,1,1000,1);CHECK(f.copy(selected));CHECK(f.rows.size()==2&&f.rows[0].signal_id==sa);
 nimby::detail::SignalRoutes routes(f.rows,f.sections);
 NimbySignal source{};source.id=sa;source.track_id=a;source.track_fraction=1;source.direction=-1;source.kind=NIMBY_SIGNAL_PATH;
 const auto next=f.observedSignal(f.sigb);const auto block=routes.block(*routes.at(sa,train),nimby::Signal(source),&next);
 CHECK(block&&block->hasBoundary()&&block->sections.size()==1&&block->sections[0].track==b);
 // Moving onto b must release a. Its rounding tolerance cannot be borrowed
 // from another track, even just a fraction rounding unit after the join.
 f.head(b,0,1000,1);CHECK(f.copy(selected));
 CHECK(f.rows.size()==1&&f.rows[0].signal_id==sb&&f.rows[0].blocks==1);
 const auto fraction=nimby::detail::trackFractionRounding/2;
 f.head(b,fraction,1000+700*fraction,1);CHECK(f.copy(selected));
 CHECK(f.rows.size()==1&&f.rows[0].signal_id==sb&&f.rows[0].blocks==1);
}
void zeroDistanceBoundary(){
 Fixture f;put(f.siga,0x48,0.0);f.head(a,0,0,1);auto r=f.reader();
 f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,0,1000,r);
 // A native marker may precede the first positive section. Zero-length
 // callback geometry must never be exported as an occupation interval.
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),0,0,0,0,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,0,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),0,1,1000,0,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.4,280,1000,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,1280,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),.4,1,420,1280,r);
 const std::array<uint64_t,2> selected{sa,sb};CHECK(f.copy(selected));
 CHECK(f.rows.size()==2&&f.rows[0].signal_id==sa&&f.rows[0].distance_m==0&&f.rows[0].blocks==1);
 CHECK(f.rows[0].source_fraction==0&&f.rows[0].source_track_id==a);
 CHECK(f.sections.size()==3);
 CHECK(std::all_of(f.sections.begin(),f.sections.end(),[](const auto& part){return part.begin_fraction<part.end_fraction;}));
}
void zeroLengthHeadEndpoint(){
 Fixture f;put(f.siga,0x48,1.0);f.head(a,1,1000,1);auto r=f.reader();
 f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,1000,1000,r);
 // The train is exactly at a's outgoing endpoint. No positive a interval
 // exists in this scan; the native zero-length marker is its point proof.
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),1,1,0,0,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,0,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.4,280,0,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,280,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),.4,1,420,280,r);
 const std::array<uint64_t,2> selected{sa,sb};CHECK(f.copy(selected));
 CHECK(f.rows.size()==2&&f.rows[0].signal_id==sa&&f.rows[0].distance_m==0);
 CHECK(f.rows[0].source_track_id==a&&f.rows[0].source_fraction==1&&f.rows[0].source_direction==1);
 CHECK(f.sections.size()==2&&f.sections[0].track_id==b&&f.sections[0].begin_fraction==0);
 CHECK(std::all_of(f.sections.begin(),f.sections.end(),[](const auto& part){return part.begin_fraction<part.end_fraction;}));
 nimby::detail::SignalRoutes routes(f.rows,f.sections);
 NimbySignal source{};source.id=sa;source.track_id=a;source.track_fraction=1;source.direction=-1;source.kind=NIMBY_SIGNAL_PATH;
 const auto next=f.observedSignal(f.sigb);const auto block=routes.block(*routes.at(sa,train),nimby::Signal(source),&next);
 CHECK(block&&block->hasBoundary()&&block->sections.size()==1&&block->sections[0].track==b);
 // The same absolute path coordinate on the next track is a passed source,
 // despite a zero metric distance and a still-identical native Path vector.
 f.head(b,0,1000,1);CHECK(f.copy(selected));
 CHECK(f.rows.size()==1&&f.rows[0].signal_id==sb&&f.rows[0].blocks==1);
}
void missingNextBoundary(){
 Fixture f;auto r=f.reader();f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,1000,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.1,.8,700,0,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,700,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.8,1,200,700,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.1,70,900,r);
 const std::array<uint64_t,1> selected{sa};CHECK(f.copy(selected));CHECK(f.rows.size()==1);
 CHECK(f.rows[0].next_signal_id==0&&!(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE));
 CHECK(f.rows[0].next_track_id==0&&f.rows[0].next_fraction==0&&f.rows[0].next_direction==0&&f.rows[0].next_kind==0);
 nimby::detail::SignalRoutes routes(f.rows,f.sections);
 NimbySignal source{};source.id=sa;source.track_id=a;source.track_fraction=.8;source.direction=-1;source.kind=NIMBY_SIGNAL_PATH;
 const auto block=routes.block(*routes.at(sa,train),nimby::Signal(source));CHECK(block&&!block->hasBoundary());
 const nimby::BlockReader clear({},true);CHECK(block->occupation(clear)==nimby::BlockOccupancy::Unknown);
 const std::array<nimby::TrainFootprint,1> occupied{{{train+1,b,.02,.03}}};
 CHECK(block->occupation(nimby::BlockReader(occupied,true))==nimby::BlockOccupancy::Occupied);
}
void boundedPathReadback(){
 Fixture f;std::array<uint64_t,16384> ids;ids.fill(a);ids[1]=b;ids[2]=c;
 const auto first=reinterpret_cast<uintptr_t>(ids.data());
 put(f.motion,0x290+0xa8,first);put(f.motion,0x290+0xb0,first+sizeof ids);put(f.motion,0x290+0xb8,first+sizeof ids);
 size_t calls=0,bytes=0,largest=0;
 const auto read=[&](uintptr_t address,void* out,size_t length){
  ++calls;bytes+=length;largest=std::max(largest,length);
  if(address>=first&&address-first<=sizeof ids&&length<=sizeof ids-(address-first)){
   std::memcpy(out,reinterpret_cast<const void*>(address),length);return true;
  }
  return f.read(address,out,length);
 };
 // The longest accepted native ID vector still describes only the observed
 // callback prefix; it is never used to fabricate 16384 route sections.
 f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,1000,read);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.1,.8,700,0,read);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,700,read);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.8,1,200,700,read);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.4,280,900,read);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,1180,read);
 f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),.4,1,420,1180,read);
 CHECK(largest<=512*sizeof(uint64_t));
 const std::array<uint64_t,2> selected{sa,sb};calls=bytes=largest=0;
 const auto emit=[&](auto row){f.rows.push_back(row);};const auto emitSection=[&](auto part){f.sections.push_back(part);};
 CHECK(f.route.copy(selected,2,1100,read,emit,emitSection));CHECK(f.rows.size()==2);
 CHECK(largest<=512*sizeof(uint64_t)&&calls<200&&bytes<4*sizeof ids);
 calls=bytes=largest=0;size_t prefixes=0;
 const auto gap=f.route.copyPrefix(sa,800,100,1100,read,[&](auto){++prefixes;});
 CHECK(gap&&std::abs(*gap-700)<1e-10&&prefixes==1);
 CHECK(largest<=512*sizeof(uint64_t)&&calls<200&&bytes<4*sizeof ids);
 // The production bridge supplies one shared read/byte/time budget. An
 // exhausted callback must produce no usable route and must stop its reads.
 for(bool byBytes:{true,false}){
  f.rows.clear();f.sections.clear();size_t remainingBytes=byBytes?1024:8*1024*1024,remainingReads=byBytes?32768:64;
  size_t acceptedReads=0;bool exhausted=false;
  const auto limited=[&](uintptr_t address,void* out,size_t length){
   if(exhausted)return false;
   if(!remainingReads||length>remainingBytes){exhausted=true;return false;}
   --remainingReads;remainingBytes-=length;++acceptedReads;return read(address,out,length);
  };
  CHECK(!f.route.copy(selected,2,1100,limited,emit,emitSection));CHECK(exhausted);
  f.rows.clear();f.sections.clear();CHECK(acceptedReads<=(byBytes?32768:64));
  remainingBytes=byBytes?1024:8*1024*1024;remainingReads=byBytes?32768:64;
  acceptedReads=0;exhausted=false;
  CHECK(!f.route.copyPrefix(sa,800,100,1100,limited,[](auto){}));CHECK(exhausted);
  CHECK(acceptedReads<=(byBytes?32768:64));
 }
 // A route whose signal scope is disjoint never pays for a Path readback.
 calls=bytes=largest=0;const std::array<uint64_t,1> other{sc};
 CHECK(!f.route.copy(other,2,1100,read,emit,emitSection)&&calls==0);
 CHECK(!f.route.copyPrefix(sc,1900,100,1100,read,[](auto){})&&calls==0);
}
void topologyEdits(){
 const std::array<uint64_t,2> selected{sa,sb};
 for(size_t offset:{size_t{8},size_t{0x10}}){
  Fixture f;put(f.ta,offset,b);f.build();CHECK(f.copy(selected));
  put(f.ta,offset,c);CHECK(!f.copy(selected));CHECK(f.rows.empty());
 }
 // A branch can retain its ID, metric and Path while its junction moves or
 // changes direction. None of those edits may keep an old approach alive.
 const auto branch=[](Fixture& f){
  put(f.ta,0x10,b);put(f.ta,0x3f0,c);put(f.ta,0x3f8,.25);put(f.ta,0x400,int32_t{1});f.build();
 };
 for(unsigned change=0;change<4;++change){
  Fixture f;branch(f);CHECK(f.copy(selected));
  if(change==0)put(f.ta,0x3f0,b);
  if(change==1)put(f.ta,0x3f8,.5);
  if(change==2)put(f.ta,0x400,int32_t{-1});
  if(change==3)put(f.ta,0x404,uint32_t{7}); // Exact attachment bytes, including padding.
  CHECK(!f.copy(selected));
 }
 Fixture child;child.attachChildren(child.ta,{b,c});child.build();CHECK(child.copy(selected));
 child.children[1]=c+1;CHECK(!child.copy(selected));
 Fixture descriptor;descriptor.attachChildren(descriptor.ta,{b,c});descriptor.build();CHECK(descriptor.copy(selected));
 const auto first=reinterpret_cast<uintptr_t>(descriptor.children.data());
 put(descriptor.ta,0x410,first+sizeof(uint64_t));CHECK(!descriptor.copy(selected));
 // Native retained empty storage is a known empty list, not a failed read.
 Fixture empty;empty.attachChildren(empty.ta,{b});const auto retained=reinterpret_cast<uintptr_t>(empty.children.data());
 put(empty.ta,0x410,retained);empty.build();CHECK(empty.copy(selected));
 Fixture revision;revision.attachChildren(revision.ta,{b});revision.build();auto r=revision.reader();
 revision.route.admission(sa,true,1100,r);CHECK(revision.copy(selected));
 CHECK(revision.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
 const auto before=revision.route.revision();revision.children[0]=c;revision.build();CHECK(revision.route.revision()>before);
 CHECK(revision.copy(selected));CHECK(!(revision.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR));
 // Stable scans retain their revision and a still-fresh native stage.
 revision.route.admission(sa,true,1100,r);const auto unchanged=revision.route.revision();revision.build();
 CHECK(revision.route.revision()==unchanged&&revision.copy(selected));
 CHECK(revision.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
}
void boundaryRescan(){
 const std::array<uint64_t,1> selected{sa};
 for(const bool reversed:{false,true}){
  Fixture f;auto read=f.reader();f.route.admission(sa,true,1050,read);
  CHECK(f.copy(selected,1050));CHECK(f.rows.size()==1);
  CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
  const auto oldRevision=f.route.revision();
  // A new native scan may see a modified signal while the Path and track
  // attachments stay identical. Its old admission belongs to the old anchor.
  if(reversed){
   put(f.siga,0x50,int8_t{1});f.head(a,.9,900,-1);
   f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,900,1100,read);
   f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.9,.8,100,0,read);
   f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,100,read);
   f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.8,.5,300,100,read);
  }else{
   put(f.siga,0x48,.9);
   f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,1100,read);
   f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.1,.9,800,0,read);
   f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,800,read);
   f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.9,1,100,800,read);
   f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.4,280,900,read);
   f.route.boundary(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,1180,read);
  }
  CHECK(f.copy(selected,1150));CHECK(f.rows.size()==1);
  CHECK(f.rows[0].source_fraction==(reversed?.8:.9));
  CHECK(f.rows[0].source_direction==(reversed?-1:1));
  CHECK(!(f.rows[0].flags&(NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR|NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED)));
  CHECK(f.route.revision()>oldRevision);
  // The new anchor can receive its own fresh stage; invalidation does not
  // permanently disable approaches or native admission diagnostics.
  f.route.admission(sa,false,1150,read);CHECK(f.copy(selected,1150));
  CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED);
  f.route.admission(sa,true,1200,read);CHECK(f.copy(selected,1200));
  CHECK(f.rows[0].flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR);
 }
}
void copyDiagnostics(){
 using namespace nimby::windows::automatic;
 const std::array<uint64_t,2> selected{sa,sb};
 const auto check=[&](Fixture& f,RouteCopyReason expected,uint64_t now=1100,std::span<const uint64_t> wanted={}){
  if(wanted.empty())wanted=selected;
  size_t reads=0;f.afterRead=[&](uintptr_t,size_t){++reads;};
  f.rows.clear();f.sections.clear();
  const auto reason=f.route.copyDiagnosed(wanted,2,now,f.reader(),
   [&](auto row){f.rows.push_back(row);},[&](auto row){f.sections.push_back(row);});
  CHECK(reason==expected);
  if(reason!=RouteCopyReason::Ready){f.rows.clear();f.sections.clear();}
  const auto rows=f.rows;const auto parts=f.sections;const auto diagnosedReads=reads;reads=0;
  CHECK(f.copy(wanted,now)==(reason==RouteCopyReason::Ready));
  CHECK(reads==diagnosedReads&&f.rows.size()==rows.size()&&f.sections.size()==parts.size());
  if(!rows.empty())CHECK(!std::memcmp(rows.data(),f.rows.data(),rows.size()*sizeof rows[0]));
  if(!parts.empty())CHECK(!std::memcmp(parts.data(),f.sections.data(),parts.size()*sizeof parts[0]));
  f.afterRead={};
 };
 Fixture ready;check(ready,RouteCopyReason::Ready);
 const std::array<uint64_t,1> third{sc};check(ready,RouteCopyReason::Ready,1100,third); // Outside requested blocks, no fabricated exclusion.
 const std::array<uint64_t,1> other{sc+0x10000};check(ready,RouteCopyReason::NoSelectedBoundary,1100,other);
 Fixture absent;absent.route=SignalRoute{};check(absent,RouteCopyReason::NoScan);
 Fixture failed;failed.head(a,std::numeric_limits<double>::quiet_NaN(),100,1);
 failed.route.begin(reinterpret_cast<uintptr_t>(failed.motion.data()),train,100,1100,failed.reader());
 check(failed,RouteCopyReason::InvalidatedScan);
 Fixture invalid;invalid.route.invalidate();check(invalid,RouteCopyReason::InvalidatedScan);
 Fixture expired;check(expired,RouteCopyReason::Expired,2001);
 Fixture clock;check(clock,RouteCopyReason::CaptureClockUnqualified,999);
 Fixture path;path.ids[1]=c;check(path,RouteCopyReason::PathUnqualified);
 Fixture topology;put(topology.tb,0x88,701.0);check(topology,RouteCopyReason::TopologyUnqualified);
 Fixture head;head.head(a,std::numeric_limits<double>::quiet_NaN(),100,1);check(head,RouteCopyReason::HeadUnqualified);
 Fixture boundary;put(boundary.siga,0x48,.9);check(boundary,RouteCopyReason::BoundaryUnqualified);
 Fixture late;size_t emitted=0;
 const auto reason=late.route.copyDiagnosed(selected,2,1100,late.reader(),[&](auto){++emitted;late.ids[1]=c;},[](auto){});
 CHECK(emitted>0&&reason==RouteCopyReason::ReadbackUnqualified); // Caller discards provisional output.
 // A failed read and a changed value are equally unqualified; the diagnostic
 // must never label the former as a proved mutation or a blocked itinerary.
 Fixture unreadable;const auto deny=[](uintptr_t,void*,size_t){return false;};
 CHECK(unreadable.route.copyDiagnosed(selected,2,1100,deny,[](auto){},[](auto){})==RouteCopyReason::PathUnqualified);
 CHECK(!unreadable.route.copy(selected,2,1100,deny,[](auto){},[](auto){}));
}
void diagnosticThrottle(){
 using namespace nimby::windows::automatic;
 RouteBatchDiagnostic healthy;healthy.status=NIMBY_OK;healthy.requestedSignals=2;healthy.blocks=2;healthy.rows=2;healthy.sections=4;
 healthy.note(RouteCopyReason::Ready,train);CHECK(!healthy.sampleCount);
 RouteDiagnosticWindow window;CHECK(window.observe(healthy,100));window.clearWindow();
 CHECK(!window.observe(healthy,200));
 auto stale=healthy;stale.rows=stale.sections=0;stale.reasons.fill(0);stale.note(RouteCopyReason::Expired,train);
 CHECK(!window.observe(stale,300));CHECK(!window.observe(healthy,500));
 CHECK(window.observe(healthy,1100)); // Recovered before logging: retain the brief failure.
 CHECK(window.reasons[static_cast<size_t>(RouteCopyReason::Expired)]==1&&window.sampleCount==1);
 CHECK(window.samples[0].train==train&&window.samples[0].reason==RouteCopyReason::Expired);
 window.clearWindow();CHECK(!window.observe(healthy,1200));CHECK(!window.observe(healthy,31099));
 CHECK(window.observe(healthy,31100));window.clearWindow(); // Exact 30 s heartbeat.
 auto busy=healthy;busy.busySlots=3;CHECK(!window.observe(busy,31101));
 CHECK(window.observe(busy,32100)&&window.busySlots==6&&!window.sampleCount);window.clearWindow();
 RouteBatchDiagnostic bounded;bounded.status=NIMBY_OK;
 for(uint64_t i=0;i<32;++i)bounded.note(RouteCopyReason::HeadUnqualified,train+i);
 CHECK(bounded.sampleCount==4&&bounded.reasons[static_cast<size_t>(RouteCopyReason::HeadUnqualified)]==32);
 CHECK(window.observe(bounded,33100)&&window.sampleCount==4);window.clearWindow();
 window.batches=std::numeric_limits<uint64_t>::max();CHECK(!window.observe(bounded,33101));
 CHECK(window.batches==std::numeric_limits<uint64_t>::max());
 CHECK(window.observe(bounded,1)); // Clock regression cannot suppress diagnostics forever.
}
void topologyInvalid(){
 const std::array<uint64_t,2> selected{sa,sb};
 for(uint64_t child:{uint64_t{0},train,a}){
  Fixture f;f.attachChildren(f.ta,{child});f.build();CHECK(!f.copy(selected));
 }
 Fixture duplicate;duplicate.attachChildren(duplicate.ta,{b,b});duplicate.build();CHECK(!duplicate.copy(selected));
 for(unsigned malformed=0;malformed<8;++malformed){
  Fixture f;f.attachChildren(f.ta,{b});const auto first=reinterpret_cast<uintptr_t>(f.children.data());
  if(malformed==0)put(f.ta,0x408,first+1); // Unaligned begin.
  if(malformed==1)put(f.ta,0x410,first-8); // Reversed range.
  if(malformed==2)put(f.ta,0x418,first); // Capacity below end.
  if(malformed==3)put(f.ta,0x418,first+4097*sizeof(uint64_t));
  if(malformed==4){put(f.ta,0x408,uintptr_t{0});put(f.ta,0x410,uintptr_t{0});}
  if(malformed==5){put(f.ta,0x408,uintptr_t{0x7fffffff0000ULL});put(f.ta,0x410,uintptr_t{0x7fffffff0008ULL});put(f.ta,0x418,uintptr_t{0x7fffffff0008ULL});}
  if(malformed==6){put(f.ta,0x408,uintptr_t{0x10000});put(f.ta,0x410,uintptr_t{0x10008});put(f.ta,0x418,uintptr_t{0x10008});}
  if(malformed==7)put(f.ta,0x418,first+9); // Misaligned capacity.
  f.build();CHECK(!f.copy(selected));
 }
 for(unsigned malformed=0;malformed<5;++malformed){
  Fixture f;put(f.ta,0x3f0,b);put(f.ta,0x3f8,.5);put(f.ta,0x400,int32_t{1});put(f.ta,0x10,c);
  if(malformed==0)put(f.ta,0x3f0,train);
  if(malformed==1)put(f.ta,0x3f0,a);
  if(malformed==2)put(f.ta,0x3f8,std::numeric_limits<double>::quiet_NaN());
  if(malformed==3)put(f.ta,0x400,int32_t{0});
  if(malformed==4)put(f.ta,8,b); // A branch must have exactly one open primary.
  f.build();CHECK(!f.copy(selected));
 }
 for(uint64_t link:{train,a}){
  Fixture f;put(f.ta,8,link);f.build();CHECK(!f.copy(selected));
 }
 Fixture alias;auto aliasRead=alias.reader();alias.route.begin(reinterpret_cast<uintptr_t>(alias.motion.data()),train,100,1000,aliasRead);
 alias.route.append(reinterpret_cast<uintptr_t>(alias.ta.data()),.1,.8,700,0,aliasRead);
 alias.route.boundary(reinterpret_cast<uintptr_t>(alias.siga.data()),sa,700,aliasRead);
 put(alias.tb,0,a);put(alias.tb,0x88,1000.0);
 alias.route.append(reinterpret_cast<uintptr_t>(alias.tb.data()),.8,1,200,700,aliasRead);
 CHECK(!alias.copy(selected)); // A second address cannot borrow the first incarnation's witness.
 for(uintptr_t address:{uintptr_t{0},uintptr_t{0x7fffffff0000ULL},std::numeric_limits<uintptr_t>::max()}){
  Fixture f;size_t invalidReads=0;
  f.route.append(address,.1,.8,700,0,[&](uintptr_t,void*,size_t){++invalidReads;return false;});
  CHECK(invalidReads==0&&!f.copy(selected)); // Check the record bound before address arithmetic.
 }
 Fixture unavailable;unavailable.attachChildren(unavailable.ta,{b});unavailable.build();size_t reads=0;
 const auto deny=[&](uintptr_t address,void* out,size_t length){
  ++reads;return address!=reinterpret_cast<uintptr_t>(unavailable.children.data())&&unavailable.read(address,out,length);
 };
 CHECK(!unavailable.route.copy(selected,2,1100,deny,[](auto){},[](auto){}));CHECK(reads>0);
}
void topologyConcurrent(){
 const std::array<uint64_t,2> selected{sa,sb};
 Fixture during;during.attachChildren(during.ta,{b,c});bool changed=false;
 during.afterRead=[&](uintptr_t address,size_t){
  if(!changed&&address==reinterpret_cast<uintptr_t>(during.children.data())){changed=true;during.children[0]=c+1;}
 };
 during.build();CHECK(changed);CHECK(!during.copy(selected));
 Fixture descriptor;descriptor.attachChildren(descriptor.ta,{b});descriptor.build();changed=false;
 descriptor.afterRead=[&](uintptr_t address,size_t){
  if(!changed&&address==reinterpret_cast<uintptr_t>(descriptor.children.data())){
   changed=true;put(descriptor.ta,8,c);
  }
 };
 CHECK(!descriptor.copy(selected));CHECK(changed&&descriptor.rows.empty());
 // A mutation after emission must reject the entire route. The bridge rolls
 // back the emitted counts on false, just as Fixture::copy discards its rows.
 for(bool childEdit:{false,true}){
  Fixture f;f.attachChildren(f.ta,{b});f.build();size_t emitted=0;
  const bool good=f.route.copy(selected,2,1100,f.reader(),[&](auto row){
   f.rows.push_back(row);if(++emitted==1){if(childEdit)f.children[0]=c;else put(f.ta,0x10,c);}
  },[&](auto row){f.sections.push_back(row);});
  CHECK(!good&&emitted>0);f.rows.clear();f.sections.clear();CHECK(f.rows.empty()&&f.sections.empty());
 }
}
void boundaryConcurrent(){
 const std::array<uint64_t,2> selected{sa,sb};
 for(bool downstream:{false,true})for(unsigned change=0;change<4;++change){
  Fixture f;size_t emitted=0;auto& signal=downstream?f.sigb:f.siga;
  const bool good=f.route.copy(selected,2,1100,f.reader(),[&](auto row){
   f.rows.push_back(row);if(++emitted==1){
    if(change==0)put(signal,0,get<uint64_t>(signal,0)+1);
    if(change==1)put(signal,0x48,.9);
    if(change==2)put(signal,0x50,int8_t{1});
    if(change==3)put(signal,0x30,int32_t{0});
   }
  },[&](auto row){f.sections.push_back(row);});
  CHECK(!good&&emitted>0);f.rows.clear();f.sections.clear();
 }
}
void topologyBudgets(){
 const std::array<uint64_t,1> selected{sa};
 for(size_t trackCount:{size_t{32},size_t{33}}){
  Fixture f;std::vector<std::array<unsigned char,0x420>> tracks(trackCount);std::vector<uint64_t> ids(trackCount),children(4096);
  for(size_t i=0;i<children.size();++i)children[i]=(1ULL<<48)|((uint64_t{1000}+i)<<16)|1;
  const auto childBegin=reinterpret_cast<uintptr_t>(children.data()),pathBegin=reinterpret_cast<uintptr_t>(ids.data());
  for(size_t i=0;i<tracks.size();++i){
   ids[i]=(1ULL<<48)|(uint64_t{i}<<16)|1;put(tracks[i],0,ids[i]);put(tracks[i],0x88,100.0);
   put(tracks[i],0x408,childBegin);put(tracks[i],0x410,childBegin+children.size()*8);put(tracks[i],0x418,childBegin+children.size()*8);
  }
  put(f.motion,0x290+0xa8,pathBegin);put(f.motion,0x290+0xb0,pathBegin+ids.size()*8);put(f.motion,0x290+0xb8,pathBegin+ids.size()*8);
  put(f.siga,0x48,0.0);f.head(a,0,0,1);size_t childReads=0,largest=0;
  const auto read=[&](uintptr_t address,void* out,size_t length){
   largest=std::max(largest,length);
   const auto region=[&](uintptr_t begin,size_t size){return address>=begin&&address-begin<=size&&length<=size-(address-begin);};
   if(region(childBegin,children.size()*8)||region(pathBegin,ids.size()*8)||
      region(reinterpret_cast<uintptr_t>(tracks.data()),tracks.size()*sizeof(tracks[0]))){
    if(region(childBegin,children.size()*8))++childReads;
    std::memcpy(out,reinterpret_cast<const void*>(address),length);return true;
   }
   return f.read(address,out,length);
  };
  f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,0,1000,read);
  f.route.append(reinterpret_cast<uintptr_t>(tracks[0].data()),0,0,0,0,read);
  f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,0,read);
  const auto afterMarker=childReads;
  f.route.append(reinterpret_cast<uintptr_t>(tracks[0].data()),0,1,100,0,read);
  CHECK(childReads==afterMarker); // Repeated track sections capture children once.
  for(size_t i=1;i<tracks.size();++i)f.route.append(reinterpret_cast<uintptr_t>(tracks[i].data()),0,1,100,i*100,read);
  CHECK(childReads==32*16); // At the aggregate limit, the 33rd list is never read.
  const bool good=f.route.copy(selected,2,1100,read,[](auto){},[](auto){});
  CHECK(good==(trackCount==32));CHECK(largest<=4096);
 }
}
using PrefixSection=nimby::windows::automatic::SignalRoute::PhysicalPrefixSection;
void physicalPrefixGeometry(){
 Fixture f;std::vector<PrefixSection> parts;
 const auto prefix=[&](uint64_t source,double position,double head,uint64_t now=1100){
  parts.clear();const auto result=f.route.copyPrefix(source,position,head,now,f.reader(),[&](auto part){parts.push_back(part);});
  if(!result)parts.clear();
  return result;
 };
 f.head(a,.75,750,1);auto gap=prefix(sa,800,750);
 CHECK(gap&&std::abs(*gap-50)<1e-10&&parts.size()==1);
 CHECK(parts[0].address==reinterpret_cast<uintptr_t>(f.ta.data())&&parts[0].track==a);
 CHECK(parts[0].from==.75&&parts[0].to==.8&&parts[0].offset==0&&std::abs(parts[0].length-50)<1e-10);
 f.head(a,.95,950,1);gap=prefix(sb,1280,950);
 CHECK(gap&&std::abs(*gap-330)<1e-10&&parts.size()==2);
 CHECK(parts[0].from==.95&&parts[0].to==1&&parts[1].from==0&&parts[1].to==.4);
 CHECK(parts[1].track==b&&std::abs(parts[1].offset-50)<1e-10);
 f.head(b,.3,1210,1);gap=prefix(sb,1280,1210);
 CHECK(gap&&std::abs(*gap-70)<1e-10&&parts.size()==1&&parts[0].from==.3&&parts[0].to==.4&&parts[0].offset==0);
 f.head(a,.8,800,1);gap=prefix(sa,800,800);CHECK(gap&&*gap==0&&parts.empty());
 CHECK(prefix(sa,std::nextafter(800.0,1000.0),800));
 CHECK(prefix(sa,std::nextafter(800.0,0.0),800)); // Associativity of head + offset + part.
 CHECK(!prefix(sa,800+2e-6,800));CHECK(!prefix(sa,800-2e-6,800));
 f.head(a,std::nextafter(.8,1.0),std::nextafter(800.0,1000.0),1);
 gap=prefix(sa,800,std::nextafter(800.0,1000.0));CHECK(gap&&*gap==0&&parts.empty());
 f.head(a,std::nextafter(.8,0.0),std::nextafter(800.0,0.0),1);
 gap=prefix(sa,800,std::nextafter(800.0,0.0));CHECK(gap&&*gap==0&&parts.empty());
 f.head(a,.8,800,1);
 CHECK(!prefix(sa,801,800));CHECK(!prefix(sa,800,799)); // Ahead/head must agree exactly.
 CHECK(!prefix(sa+1,800,800));CHECK(!prefix(sa,800,800,999));CHECK(!prefix(sa,800,800,2001));
 f.head(a,.801,801,1);CHECK(!prefix(sa,800,801)); // No former source contact.
 Fixture reverse;put(reverse.siga,0x48,.2);put(reverse.siga,0x50,int8_t{1});reverse.head(a,.9,100,-1);auto r=reverse.reader();
 reverse.route.begin(reinterpret_cast<uintptr_t>(reverse.motion.data()),train,100,1000,r);
 reverse.route.append(reinterpret_cast<uintptr_t>(reverse.ta.data()),.9,.2,700,0,r);
 reverse.route.boundary(reinterpret_cast<uintptr_t>(reverse.siga.data()),sa,700,r);
 reverse.head(a,.25,750,-1);parts.clear();gap=reverse.route.copyPrefix(sa,800,750,1100,r,[&](auto part){parts.push_back(part);});
 CHECK(gap&&std::abs(*gap-50)<1e-10&&parts.size()==1&&parts[0].from==.25&&parts[0].to==.2);
 // A native point at a previous track's endpoint proves contact, not a
 // positive occupation interval. Moving onto the next track passes it.
 Fixture point;put(point.siga,0x48,1.0);point.head(a,1,1000,1);auto pr=point.reader();
 point.route.begin(reinterpret_cast<uintptr_t>(point.motion.data()),train,1000,1000,pr);
 point.route.append(reinterpret_cast<uintptr_t>(point.ta.data()),1,1,0,0,pr);
 point.route.boundary(reinterpret_cast<uintptr_t>(point.siga.data()),sa,0,pr);
 point.route.append(reinterpret_cast<uintptr_t>(point.tb.data()),0,.4,280,0,pr);
 point.route.boundary(reinterpret_cast<uintptr_t>(point.sigb.data()),sb,280,pr);
 parts.clear();gap=point.route.copyPrefix(sa,1000,1000,1100,pr,[&](auto part){parts.push_back(part);});
 CHECK(gap&&*gap==0&&parts.empty());
 gap=point.route.copyPrefix(sb,1280,1000,1100,pr,[&](auto part){parts.push_back(part);});
 CHECK(gap&&*gap==280&&parts.size()==1&&parts[0].track==b&&parts[0].from==0);
 point.head(b,0,1000,1);CHECK(!point.route.copyPrefix(sa,1000,1000,1100,pr,[](auto){}));
}
void physicalPrefixInvalidation(){
 for(unsigned change=0;change<9;++change){
  Fixture f;f.head(a,.75,750,1);size_t emitted=0;
  if(change==0)put(f.siga,0,sa+1);
  if(change==1)put(f.siga,0x48,.81);
  if(change==2)put(f.siga,0x50,int8_t{1});
  if(change==3)put(f.siga,0x30,int32_t{0});
  if(change==4)f.ids[1]=c;
  if(change==5)put(f.ta,0x10,c);
  if(change==6)f.head(a,.75,750,-1);
  if(change==7)f.head(b,.75,750,1);
  if(change==8)put(f.ta,0x88,1001.0);
  CHECK(!f.route.copyPrefix(sa,800,750,1100,f.reader(),[&](auto){++emitted;}));CHECK(emitted==0);
 }
 // Append permits native millimetre rounding, but even a tiny unobserved
 // hole between callbacks is never granted physical clearance by export.
 Fixture hole;auto r=hole.reader();hole.route.begin(reinterpret_cast<uintptr_t>(hole.motion.data()),train,100,1000,r);
 hole.route.append(reinterpret_cast<uintptr_t>(hole.ta.data()),.1,.5,400,0,r);
 hole.route.append(reinterpret_cast<uintptr_t>(hole.ta.data()),.500001,.8,299.999,400.001,r);
 hole.route.boundary(reinterpret_cast<uintptr_t>(hole.siga.data()),sa,700,r);
 size_t emitted=0;CHECK(!hole.route.copyPrefix(sa,800,100,1100,r,[&](auto){++emitted;}));CHECK(emitted==0);
 Fixture marker;marker.route.begin(reinterpret_cast<uintptr_t>(marker.motion.data()),train,100,1000,marker.reader());
 marker.route.append(reinterpret_cast<uintptr_t>(marker.ta.data()),.1,.1,0,0,marker.reader());
 marker.route.boundary(reinterpret_cast<uintptr_t>(marker.siga.data()),sa,0,marker.reader());
 CHECK(!marker.route.copyPrefix(sa,100,100,1100,marker.reader(),[](auto){})); // Marker cannot invent .8 contact.
 Fixture incomplete;incomplete.route.begin(reinterpret_cast<uintptr_t>(incomplete.motion.data()),train,100,1000,incomplete.reader());
 incomplete.route.append(reinterpret_cast<uintptr_t>(incomplete.ta.data()),.1,.5,400,0,incomplete.reader());
 CHECK(!incomplete.route.copyPrefix(sa,800,100,1100,incomplete.reader(),[](auto){}));
 for(unsigned change=0;change<5;++change){
  Fixture f;f.head(a,.75,750,1);emitted=0;
  CHECK(!f.route.copyPrefix(sa,800,750,1100,f.reader(),[&](auto){++emitted;
   if(change==0)f.ids[1]=c;
   if(change==1)put(f.ta,8,c);
   if(change==2)put(f.siga,0x48,.9);
   if(change==3)f.head(a,.76,760,1);
   if(change==4)put(f.siga,0x50,int8_t{1});
  }));CHECK(emitted==1); // Caller must discard queries/geometry after false.
 }
 Fixture exhausted;emitted=0;
 CHECK(!exhausted.route.copyPrefix(sa,800,100,1100,[](uintptr_t,void*,size_t){return false;},[&](auto){++emitted;}));CHECK(emitted==0);
}
void physicalPrefixOccupation(){
 Fixture f;f.head(a,.75,750,1);auto r=f.reader();const auto motion=reinterpret_cast<uintptr_t>(f.motion.data());
 nimby::windows::automatic::PhysicalRoute physical;physical.begin(motion,train,750,r);
 const auto gap=f.route.copyPrefix(sa,800,750,1100,r,[&](auto part){physical.append(part.address,part.from,part.to,part.length,part.offset,r);});
 CHECK(gap&&physical.valid());
 physical.append(reinterpret_cast<uintptr_t>(f.ta.data()),.8,.95,150,*gap,r);CHECK(physical.valid());
 unsigned queries=0;double obstacle=20;
 const auto fresh=[&](double head,uint64_t now){return physical.refresh(motion,head,now,r,[&](uintptr_t,double from,double,double length){
  ++queries;return from<.8?std::min(obstacle,length):length;
 });};
 auto view=fresh(750,1100);CHECK(view.valid&&view.free==20&&queries==1);
 obstacle=1000;view=fresh(750,1101);CHECK(view.valid&&std::abs(view.covered-200)<1e-10&&view.free==200&&queries==3);
 f.head(a,.76,760,1);view=fresh(760,1102);CHECK(view.valid&&std::abs(view.covered-190)<1e-10&&view.free==200&&queries==5);
 obstacle=-1;CHECK(!fresh(760,1103).valid); // Unknown physical observation never reuses clearance.
}
void physicalPrefixBracket(){
 for(bool contact:{false,true}){
  Fixture f;const double head=contact?800:750;f.head(a,contact?.8:.75,head,1);
  size_t emitted=0,callbacks=0,pathReads=0;
  const auto read=[&](uintptr_t address,void* out,size_t bytes){
   if(address==reinterpret_cast<uintptr_t>(f.motion.data())+0x290)++pathReads;
   return f.read(address,out,bytes);
  };
  const auto result=f.route.withPrefix(sa,800,head,1100,read,[&](auto){++emitted;},[&](double gap){
   ++callbacks;CHECK(emitted==(contact?0:1));CHECK(std::abs(gap-(contact?0:50))<1e-10);
  });
  CHECK(result&&callbacks==1&&pathReads==2); // Initial and final complete Path only.
  for(unsigned change=0;change<9;++change){
   Fixture changed;changed.head(a,contact?.8:.75,head,1);callbacks=0;
   CHECK(!changed.route.withPrefix(sa,800,head,1100,changed.reader(),[](auto){},[&](double){
    ++callbacks;
    if(change==0)changed.ids[1]=c; // Same vector allocation, changed contents.
    if(change==1)put(changed.motion,0x2a0,uint64_t{1}); // Same vector, changed Path descriptor.
    if(change==2)put(changed.ta,8,c);
    if(change==3)put(changed.ta,0x88,1001.0);
    if(change==4)put(changed.siga,0,sa+1);
    if(change==5)put(changed.siga,0x48,.81);
    if(change==6)put(changed.siga,0x50,int8_t{1});
    if(change==7)put(changed.siga,0x30,int32_t{0});
    if(change==8)changed.head(a,contact?.801:.751,head+1,1);
   }));CHECK(callbacks==1);
  }
 }
 Fixture children;children.attachChildren(children.ta,{b,c});children.build();children.head(a,.75,750,1);
 size_t callbacks=0;
 CHECK(!children.route.withPrefix(sa,800,750,1100,children.reader(),[](auto){},[&](double){
  ++callbacks;children.children[1]=b+1;
 }));CHECK(callbacks==1);
 Fixture unavailable;bool exhausted=false;callbacks=0;
 const auto limited=[&](uintptr_t address,void* out,size_t bytes){return !exhausted&&unavailable.read(address,out,bytes);};
 CHECK(!unavailable.route.withPrefix(sa,800,100,1100,limited,[](auto){},[&](double){++callbacks;exhausted=true;}));CHECK(callbacks==1);
 // A missing / incoherent prefix performs no occupation callback, even when
 // the intended query itself would have returned clear.
 Fixture invalid;callbacks=0;
 CHECK(!invalid.route.withPrefix(sa,801,100,1100,invalid.reader(),[](auto){},[&](double){++callbacks;}));CHECK(callbacks==0);
 // A mutation while reading the head need not trigger a second full Path
 // traversal before occupation: final readback still rejects the whole view.
 Fixture during;bool changed=false;callbacks=0;
 during.afterRead=[&](uintptr_t address,size_t){if(!changed&&address==reinterpret_cast<uintptr_t>(during.motion.data())+0x3b0){changed=true;during.ids[1]=c;}};
 CHECK(!during.route.withPrefix(sa,800,100,1100,during.reader(),[](auto){},[&](double){++callbacks;}));CHECK(changed&&callbacks==1);
}
void deferredCommitSources(){
 using Stamp=nimby::windows::automatic::SignalRoute::CommitSourceStamp;
 const std::array<uint64_t,2> selected{sa,sb};
 const std::array<double,2> positions{800,1280};
 std::array<Stamp,2> stamps{};
 Fixture baseline;
 CHECK(baseline.route.captureCommitSources(selected,positions,stamps,baseline.reader()));
 CHECK(stamps[0].signal==sa&&stamps[0].track==a&&stamps[0].position==800);
 CHECK(stamps[1].signal==sb&&stamps[1].track==b&&stamps[1].position==1280);
 CHECK(stamps[0].orientation==0&&stamps[0].trackOrientation==0);
 CHECK(baseline.route.matchesCommitSources(stamps,baseline.reader()));
 // The projected coordinate and scan offset can associate additions in a
 // different order. The witness accepts only the shared micrometre tolerance.
 for(double expected:{100+(.8-.1)*1000,std::nextafter(800.0,1000.0),
                       800-1e-6,800+1e-6}){
  Fixture f;const std::array<double,2> rounded{expected,1280};
  CHECK(std::abs(expected-800)<=1e-6);
  CHECK(f.route.captureCommitSources(selected,rounded,stamps,f.reader()));
  CHECK(stamps[0].position==800); // Store observed geometry, not the caller's estimate.
  CHECK(f.route.matchesCommitSources(stamps,f.reader()));
 }
 for(double expected:{800-2e-6,800+2e-6,std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::infinity()}){
  Fixture f;const std::array<double,2> incoherent{expected,1280};
  CHECK(!f.route.captureCommitSources(selected,incoherent,stamps,f.reader()));
 }
 // Full incarnations, native source orientation and the track frame are all
 // requalified after integration, even when allocation addresses are unchanged.
 for(unsigned change=0;change<4;++change){
  Fixture f;CHECK(f.route.captureCommitSources(selected,positions,stamps,f.reader()));
  if(change==0)put(f.siga,0x51,int8_t{1});
  if(change==1)put(f.ta,0x2c,int8_t{1});
  if(change==2)put(f.siga,0,sa+(1ULL<<16));
  if(change==3)put(f.ta,0,a+(1ULL<<16));
  CHECK(!f.route.matchesCommitSources(stamps,f.reader()));
 }
 Fixture mismatch;put(mismatch.siga,0x51,int8_t{1});
 CHECK(!mismatch.route.captureCommitSources(selected,positions,stamps,mismatch.reader()));
 Fixture oriented;put(oriented.siga,0x51,int8_t{1});put(oriented.ta,0x2c,int8_t{1});
 CHECK(oriented.route.captureCommitSources(selected,positions,stamps,oriented.reader()));
 CHECK(stamps[0].orientation==1&&stamps[0].trackOrientation==1);
 CHECK(oriented.route.matchesCommitSources(stamps,oriented.reader()));
 // Capture also brackets its reads: changing orientation after the first
 // read cannot produce a usable mixed-incarnation stamp.
 Fixture during;bool changed=false;
 during.afterRead=[&](uintptr_t address,size_t){
  if(!changed&&address==reinterpret_cast<uintptr_t>(during.siga.data())+0x51){
   changed=true;put(during.siga,0x51,int8_t{1});
  }
 };
 CHECK(!during.route.captureCommitSources(selected,positions,stamps,during.reader()));
 CHECK(changed);
}
void selectedRouteObjects(){
 const std::array<uint64_t,1> selected{sa};
 Fixture optIn;optIn.build(1000,false);
 CHECK(!optIn.route.observesObjects()&&optIn.copy(selected)&&!optIn.copyV2(selected));
 optIn.build();CHECK(optIn.route.observesObjects()&&optIn.copyV2(selected));
 optIn.build(1000,false);CHECK(!optIn.copyV2(selected)); // A new non-opt-in scan clears old annotations.
 const auto build=[](Fixture& f,int kind,bool nextPath=true){
  put(f.sigb,0x30,int32_t{kind});put(f.sigb,0x40,b);put(f.sigb,0x48,.25);
  // Object orientation is retained for readback, not used as travel direction.
  put(f.sigb,0x50,int8_t{1});
  put(f.sigc,0x40,b);put(f.sigc,0x48,.75);
  auto r=f.reader();f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,1000,r);
  f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.1,.8,700,0,r);
  f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,700,r);
  f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.8,1,200,700,r);
  f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),0,.25,175,900,r);
  f.route.encounteredObject(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,1075,r);
  if(nextPath){
   f.route.append(reinterpret_cast<uintptr_t>(f.tb.data()),.25,.75,350,1075,r);
   f.route.boundary(reinterpret_cast<uintptr_t>(f.sigc.data()),sc,1425,r);
  }
 };
 for(const int kind:{NIMBY_SIGNAL_MARKER,NIMBY_SIGNAL_BALISE}){
  Fixture f;build(f,kind);CHECK(f.copyV2(selected));
  CHECK(f.rows.size()==1&&f.rows[0].next_signal_id==sc&&f.annotations.size()==2);
  const auto& marker=f.annotations[0];const auto& protecting=f.annotations[1];
  CHECK(marker.train_id==train&&marker.signal_id==sa&&marker.object_id==sb&&marker.track_id==b);
  CHECK(marker.track_fraction==.25&&marker.distance_m==375&&marker.direction==1&&marker.native_kind==kind&&marker.ordinal==0);
  CHECK(protecting.object_id==sc&&protecting.native_kind==NIMBY_SIGNAL_PATH&&protecting.ordinal==1&&protecting.distance_m==725);
  if(kind==NIMBY_SIGNAL_BALISE){
   CHECK(marker.flags==(NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY|NIMBY_SELECTED_ROUTE_NATIVE_BOUNDARY|NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE));
   CHECK(protecting.flags==NIMBY_SELECTED_ROUTE_PROTECTING_SIGNAL);
  }else{
   CHECK(marker.flags==0); // Passive marker cannot manufacture a terminus.
   CHECK(protecting.flags==(NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY|NIMBY_SELECTED_ROUTE_PROTECTING_SIGNAL));
  }
  CHECK(f.sections.size()==3&&f.sections[0].track_id==a&&f.sections[1].track_id==b&&f.sections[2].track_id==b);
  CHECK(f.sections[1].begin_fraction==0&&f.sections[1].end_fraction==.25&&f.sections[2].begin_fraction==.25);
 }
 Fixture passive;build(passive,NIMBY_SIGNAL_MARKER,false);CHECK(passive.copyV2(selected));
 CHECK(!(passive.rows[0].flags&NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE)&&passive.rows[0].next_signal_id==0);
 CHECK(passive.annotations.size()==1&&passive.annotations[0].flags==0); // Scan ending is unknown.
 Fixture balise;build(balise,NIMBY_SIGNAL_BALISE,false);CHECK(balise.copyV2(selected));
 CHECK(balise.annotations.size()==1&&balise.annotations[0].flags==(NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY|NIMBY_SELECTED_ROUTE_NATIVE_BOUNDARY|NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE));
 for(unsigned filter=0;filter<5;++filter){
  Fixture f;
  if(filter==0)put(f.sigb,0x70,uint32_t{1}); // Default ignored.
  if(filter==1)put(f.sigb,0x70,uint32_t{2}); // Unknown mode.
  if(filter==2){ // Exception list present, never evaluated by this producer.
   const auto begin=reinterpret_cast<uintptr_t>(f.ids.data());
   put(f.sigb,0x78,begin);put(f.sigb,0x80,begin+8);put(f.sigb,0x88,begin+8);
  }
  if(filter==3)put(f.sigb,0x88,uint64_t{8}); // Malformed null descriptor.
  if(filter==4){ // A valid retained empty vector is still unconditional.
   const auto begin=reinterpret_cast<uintptr_t>(f.ids.data());
   put(f.sigb,0x78,begin);put(f.sigb,0x80,begin);put(f.sigb,0x88,begin+8);
  }
  build(f,NIMBY_SIGNAL_BALISE,false);CHECK(f.copyV2(selected)&&f.annotations.size()==1);
  CHECK(bool(f.annotations[0].flags&NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE)==(filter==4));
 }
 Fixture changedFilter;build(changedFilter,NIMBY_SIGNAL_BALISE,false);
 put(changedFilter.sigb,0x70,uint32_t{1});CHECK(!changedFilter.copyV2(selected));
 Fixture duringFilter;build(duringFilter,NIMBY_SIGNAL_BALISE,false);bool filterChanged=false;
 duringFilter.afterRead=[&](uintptr_t at,size_t){if(!filterChanged&&at==reinterpret_cast<uintptr_t>(duringFilter.sigb.data())+0x70){
  filterChanged=true;put(duringFilter.sigb,0x70,uint32_t{1});
 }};
 CHECK(!duringFilter.copyV2(selected)&&filterChanged);
 Fixture empty;auto r=empty.reader();empty.route.begin(reinterpret_cast<uintptr_t>(empty.motion.data()),train,100,1000,r);
 empty.route.append(reinterpret_cast<uintptr_t>(empty.ta.data()),.1,.8,700,0,r);
 empty.route.boundary(reinterpret_cast<uintptr_t>(empty.siga.data()),sa,700,r);
 empty.route.append(reinterpret_cast<uintptr_t>(empty.ta.data()),.8,.9,100,700,r);
 CHECK(empty.copyV2(selected)&&empty.annotations.empty()&&empty.rows[0].next_signal_id==0);
 // Copying annotations adds no second native route capture/full Path traversal.
 Fixture same;build(same,NIMBY_SIGNAL_MARKER);size_t pathReads=0;
 same.afterRead=[&](uintptr_t at,size_t){if(at==reinterpret_cast<uintptr_t>(same.motion.data())+0x290)++pathReads;};
 CHECK(same.copy(selected));const auto v1Reads=pathReads;pathReads=0;
 CHECK(same.copyV2(selected)&&pathReads==v1Reads);
 // V2 fails closed for every marker anchor change, including after emission.
 for(unsigned phase=0;phase<2;++phase)for(unsigned change=0;change<5;++change){
  Fixture f;build(f,NIMBY_SIGNAL_BALISE);bool changed=false;
  const auto mutate=[&]{
   if(change==0)put(f.sigb,0,sb+(1ULL<<16));
   if(change==1)put(f.sigb,0x30,int32_t{NIMBY_SIGNAL_MARKER});
   if(change==2)put(f.sigb,0x40,c);
   if(change==3)put(f.sigb,0x48,.26);
   if(change==4)put(f.sigb,0x50,int8_t{-1});
   changed=true;
  };
  if(phase==0)mutate();
  else f.afterRead=[&](uintptr_t at,size_t){if(!changed&&at==reinterpret_cast<uintptr_t>(f.sigb.data())+0x30)mutate();};
  CHECK(!f.copyV2(selected)&&changed&&f.annotations.empty()&&f.rows.empty()&&f.sections.empty());
 }
 Fixture invalid;build(invalid,NIMBY_SIGNAL_MARKER);invalid.route.invalidateObjects();
 CHECK(!invalid.copyV2(selected));CHECK(invalid.copy(selected)); // V1 remains available.
 Fixture expired;build(expired,NIMBY_SIGNAL_BALISE);CHECK(!expired.copyV2(selected,2001));
}
void selectedRouteReverse(){
 Fixture f;f.head(a,.9,100,-1);
 put(f.siga,0x50,int8_t{1});
 put(f.sigb,0x30,int32_t{NIMBY_SIGNAL_MARKER});put(f.sigb,0x40,a);put(f.sigb,0x48,.6);
 put(f.sigc,0x40,a);put(f.sigc,0x48,.2);put(f.sigc,0x50,int8_t{1});
 auto r=f.reader();f.route.begin(reinterpret_cast<uintptr_t>(f.motion.data()),train,100,1000,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.9,.8,100,0,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.siga.data()),sa,100,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.8,.6,200,100,r);
 f.route.encounteredObject(reinterpret_cast<uintptr_t>(f.sigb.data()),sb,300,r);
 f.route.append(reinterpret_cast<uintptr_t>(f.ta.data()),.6,.2,400,300,r);
 f.route.boundary(reinterpret_cast<uintptr_t>(f.sigc.data()),sc,700,r);
 const std::array<uint64_t,1> selected{sa};CHECK(f.copyV2(selected));
 CHECK(f.sections.size()==2&&f.sections[0].begin_fraction==.8&&f.sections[0].end_fraction==.6);
 CHECK(f.sections[1].begin_fraction==.6&&f.sections[1].end_fraction==.2);
 CHECK(f.annotations.size()==2&&f.annotations[0].direction==-1&&f.annotations[1].direction==-1);
 CHECK(f.annotations[0].distance_m==200&&f.annotations[0].flags==0);
 CHECK(f.annotations[1].distance_m==600&&f.annotations[1].flags==3);
 CHECK(f.copy(selected)&&std::abs(f.sections[0].begin_fraction-.6)<1e-12&&
       std::abs(f.sections[0].end_fraction-.8)<1e-12); // V1 normalized, unchanged rounding.
}
}
int main(){try{ordered();invalidate();movement();crossingAdmission();previousTrackContact();zeroDistanceBoundary();zeroLengthHeadEndpoint();missingNextBoundary();boundedPathReadback();topologyEdits();boundaryRescan();copyDiagnostics();diagnosticThrottle();topologyInvalid();topologyConcurrent();boundaryConcurrent();topologyBudgets();physicalPrefixGeometry();physicalPrefixInvalidation();physicalPrefixOccupation();physicalPrefixBracket();deferredCommitSources();selectedRouteObjects();selectedRouteReverse();std::cout<<"ordered native signal routes, selected objects and physical approach prefix: PASS\n";return 0;}
 catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
