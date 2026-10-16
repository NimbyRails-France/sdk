#define NIMBY_SDK_BUILD
#include <nimby/detail/signal_route_values.hpp>
#include <iostream>
#include <stdexcept>
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {unsigned captures=0;uint32_t rowCapacity=0,sectionCapacity=0;}
extern "C" uint32_t __cdecl NimbyInternal_CopySignalRoutes(const uint64_t*,uint32_t,uint32_t,
    NimbySignalRouteObservation*,uint32_t capacity,uint32_t* count,
    NimbySignalRouteSection*,uint32_t sections,uint32_t* sectionCount) noexcept {
    ++captures;rowCapacity=capacity;sectionCapacity=sections;*count=*sectionCount=0;
    return NIMBY_BUFFER_TOO_SMALL;
}
extern "C" uint32_t __cdecl NimbyInternal_CopySignalRoutesV2(const uint64_t* ids,uint32_t count,uint32_t blocks,
    NimbySignalRouteObservation* rows,uint32_t capacity,uint32_t* copied,
    NimbySignalRouteSection* sections,uint32_t sectionCapacity,uint32_t* sectionCount,
    NimbySignalRouteAnnotation*,uint32_t,uint32_t* annotations) noexcept {
    *annotations=0;return NimbyInternal_CopySignalRoutes(ids,count,blocks,rows,capacity,copied,sections,sectionCapacity,sectionCount);
}
int main(){try{
 constexpr uint64_t train=(5ULL<<48)|1,a=(1ULL<<48)|1,b=(1ULL<<48)|0x10001;
 constexpr uint64_t sourceId=(8ULL<<48)|1,nextId=(8ULL<<48)|0x10001;
 NimbySignalRouteObservation row{train,sourceId,nextId,1,1000,a,.5,400,1,1,
     NIMBY_SIGNAL_ROUTE_ORDERED|NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE,0,a,.9,1,NIMBY_SIGNAL_PATH,b,.4,1,NIMBY_SIGNAL_PATH};
 NimbySignal source{};source.id=sourceId;source.track_id=a;source.track_fraction=.9;source.direction=-1;source.kind=NIMBY_SIGNAL_PATH;
 NimbySignal next{};next.id=nextId;next.track_id=b;next.track_fraction=.4;next.direction=-1;next.kind=NIMBY_SIGNAL_PATH;
 auto nextSignal=nimby::Signal(next);
 for(const unsigned field:{0,1,2,3,4,5}){
  auto invalid=row;
  if(field==0)invalid.train_id=5ULL<<48;
  if(field==1)invalid.signal_id=8ULL<<48;
  if(field==2)invalid.next_signal_id=8ULL<<48;
  if(field==3)invalid.head_track_id=1ULL<<48;
  if(field==4)invalid.source_track_id=1ULL<<48;
  if(field==5)invalid.next_track_id=1ULL<<48;
  CHECK(!nimby::detail::SignalRoutes::valid(invalid));
 }
 std::vector<NimbySignalRouteSection> parts{{train,sourceId,a,.9,1},{train,sourceId,b,0,.4}};
 nimby::detail::SignalRoutes routes({row},parts);
 const auto block=routes.block(row,nimby::Signal(source),&nextSignal);CHECK(block&&block->hasBoundary()&&block->nextSignal==nextId);
 CHECK(block->occupation(nimby::BlockReader({},true))==nimby::BlockOccupancy::Clear);
 CHECK(block->occupation(nimby::BlockReader({},false))==nimby::BlockOccupancy::Unknown);
 const std::array<nimby::TrainFootprint,1> occupied{{{train+1,b,.2,.3}}};
 CHECK(block->occupation(nimby::BlockReader(occupied,true))==nimby::BlockOccupancy::Occupied);
 source.direction=1;CHECK(!routes.block(row,nimby::Signal(source),&nextSignal));source.direction=-1;
 source.track_fraction=.8;CHECK(!routes.block(row,nimby::Signal(source),&nextSignal));source.track_fraction=.9;
 CHECK(!routes.block(row,nimby::Signal(source)));
 next.track_fraction=.5;nextSignal=nimby::Signal(next);CHECK(!routes.block(row,nimby::Signal(source),&nextSignal));next.track_fraction=.4;
 next.direction=1;nextSignal=nimby::Signal(next);CHECK(!routes.block(row,nimby::Signal(source),&nextSignal));next.direction=-1;
 next.kind=NIMBY_SIGNAL_ONE_WAY;nextSignal=nimby::Signal(next);CHECK(!routes.block(row,nimby::Signal(source),&nextSignal));next.kind=NIMBY_SIGNAL_PATH;
 nextSignal=nimby::Signal(next);
 row.flags=NIMBY_SIGNAL_ROUTE_ORDERED;row.next_signal_id=0;
 row.next_track_id=0;row.next_fraction=0;row.next_direction=0;row.next_kind=0;
 nimby::detail::SignalRoutes truncated({row},parts);
 const auto prefix=truncated.block(row,nimby::Signal(source));CHECK(prefix&&!prefix->hasBoundary());
 CHECK(prefix->occupation(nimby::BlockReader({},true))==nimby::BlockOccupancy::Unknown);
 CHECK(prefix->occupation(nimby::BlockReader(occupied,true))==nimby::BlockOccupancy::Occupied);
 source.track_fraction=1;row.source_fraction=1;row.flags|=NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE;row.next_signal_id=nextId;
 row.next_track_id=b;row.next_fraction=.4;row.next_direction=1;row.next_kind=NIMBY_SIGNAL_PATH;
 nimby::detail::SignalRoutes endpoint({row},{{train,sourceId,b,0,.4}});
 CHECK(endpoint.block(row,nimby::Signal(source),&nextSignal)->hasBoundary());
 row.flags|=NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR|NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;
 CHECK(!nimby::detail::SignalRoutes::valid(row));
 nimby::detail::SignalRoutes invalid({row},parts);CHECK(invalid.rows().empty());
 row.flags=NIMBY_SIGNAL_ROUTE_ORDERED;row.reserved=1;CHECK(!nimby::detail::SignalRoutes::valid(row));
 // An undersized batch must not issue repeated native reads on this observation
 // or preserve facts from the preceding successful observation.
 nimby::detail::SignalRoutes growing({*routes.at(sourceId,train)},parts);
 const std::array<uint64_t,1> selected{sourceId};
 CHECK(growing.capture(selected,2)==NIMBY_BUFFER_TOO_SMALL&&captures==1);
 CHECK(growing.rows().empty()&&rowCapacity==128&&sectionCapacity==1024);
 CHECK(growing.capture(selected,2)==NIMBY_BUFFER_TOO_SMALL&&captures==2);
 CHECK(rowCapacity==256&&sectionCapacity==2048);
 std::cout<<"signal route observation contract: PASS\n";return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
