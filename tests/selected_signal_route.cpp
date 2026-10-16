#define NIMBY_SDK_BUILD
#include <nimby/detail/selected_signal_route_values.hpp>
#include <cstring>
#include <iostream>
#include <stdexcept>
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t train=(5ULL<<48)|1,track=(1ULL<<48)|1,otherTrack=track+0x10000;
constexpr uint64_t source=(8ULL<<48)|1,marker=source+0x10000,balise=source+0x20000,next=source+0x30000;
nimby::Signal signal(uint64_t id,int kind,double fraction,uint64_t at=track,int direction=-1){
 NimbySignal row{};row.id=id;row.track_id=at;row.track_fraction=fraction;row.direction=direction;row.kind=kind;return nimby::Signal(row);
}
struct Fixture {
 std::vector<nimby::Signal> signals{signal(source,NIMBY_SIGNAL_PATH,.1),signal(marker,NIMBY_SIGNAL_MARKER,.3),
  signal(balise,NIMBY_SIGNAL_BALISE,.8),signal(next,NIMBY_SIGNAL_PATH,.9)};
 std::vector<nimby::SignalState> states;
 std::vector<nimby::SignalTexture> textures;
 NimbySignalRouteObservation row{train,source,next,1,100,track,0,100,1,1,
  NIMBY_SIGNAL_ROUTE_ORDERED|NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE,0,track,.1,1,NIMBY_SIGNAL_PATH,track,.9,1,NIMBY_SIGNAL_PATH};
 std::vector<NimbySignalRouteSection> parts{{train,source,track,.1,.9}};
 std::vector<NimbySignalRouteAnnotation> annotations{{train,source,marker,track,.3,200,1,NIMBY_SIGNAL_MARKER,0,0},
 {train,source,balise,track,.8,700,1,NIMBY_SIGNAL_BALISE,1,
  NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY|NIMBY_SELECTED_ROUTE_NATIVE_BOUNDARY|NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE}};
 Fixture(){
  NimbySignalState state{};state.signal_id=balise;state.flags=NIMBY_SIGNAL_FILTER_VALID;states.emplace_back(state);
  NimbySignalTexture texture{};texture.signal_id=marker;texture.flags=NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID;
  texture.state_count=1;texture.selected_index=0;std::strcpy(texture.textures_id_utf8,"test.marker.t32");textures.emplace_back(texture);
 }
 nimby::SelectedSignalRoute read(const nimby::BlockReader& reader=nimby::BlockReader({},true))const{
  return nimby::detail::SelectedRouteIndex(signals,states,textures).read(row,signals.front(),parts,annotations,reader);
 }
};
}
int main(){try{
 using nimby::RouteDestination;using nimby::BlockOccupancy;
 Fixture f;auto observed=f.read();
 CHECK(observed.complete&&observed.trainId==train&&observed.boundarySignalId==balise);
 CHECK(observed.destination==RouteDestination::NativeBoundary&&observed.occupation==BlockOccupancy::Clear);
 CHECK(observed.markers.size()==1&&observed.markers[0].id==marker&&observed.markers[0].textureSet=="test.marker.t32");
 // Bound the occupancy check at the native balise, not at a later PATH.
 const std::array<nimby::TrainFootprint,1> beyond{{{train+1,track,.85,.88}}};
 CHECK(f.read(nimby::BlockReader(beyond,true)).occupation==BlockOccupancy::Clear);
 const std::array<nimby::TrainFootprint,1> inside{{{train+1,track,.4,.5}}};
 CHECK(f.read(nimby::BlockReader(inside,true)).occupation==BlockOccupancy::Occupied);
 CHECK(f.read(nimby::BlockReader({},false)).occupation==BlockOccupancy::Unknown);
 // A geometrically nearby marker on a different branch is never on this route.
 f.signals[1]=signal(marker,NIMBY_SIGNAL_MARKER,.3,otherTrack);
 f.annotations.erase(f.annotations.begin());
 CHECK(f.read().complete&&f.read().markers.empty());f=Fixture{};
 // A contradictory native position, or a new marker lacking copied metadata,
 // invalidates this join even when the old snapshot point was outside it.
 f.signals[1]=signal(marker,NIMBY_SIGNAL_MARKER,.95);CHECK(!f.read().complete);f=Fixture{};
 f.annotations.insert(f.annotations.begin()+1,{train,source,marker+0x50000,track,.4,300,1,NIMBY_SIGNAL_MARKER,1,0});
 CHECK(!f.read().complete);f=Fixture{};
 // A native PATH endpoint wins independently of any marker's national meaning.
 f.annotations={{train,source,marker,track,.3,200,1,NIMBY_SIGNAL_MARKER,0,0},{train,source,next,track,.9,800,1,NIMBY_SIGNAL_PATH,1,3}};
 CHECK(f.read().destination==RouteDestination::ProtectingSignal&&f.read().markers.size()==1);f=Fixture{};
 // No positive boundary, a passive marker, moved endpoint or filtered balise
 // must never turn the end of a scan into a completed route.
 f.annotations.clear();CHECK(!f.read().complete);f=Fixture{};
 f.annotations={{train,source,marker,track,.3,200,1,NIMBY_SIGNAL_MARKER,0,5}};CHECK(!f.read().complete);f=Fixture{};
 f.signals[2]=signal(balise,NIMBY_SIGNAL_BALISE,.81);CHECK(!f.read().complete);f=Fixture{};
 f.annotations[1].flags&=~NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE;
 CHECK(!f.read().complete);f=Fixture{};
 // The native witness can qualify an unconditional endpoint on the chosen
 // branch even when the older geometric snapshot stopped at that junction.
 f.states.clear();f.signals.erase(f.signals.begin()+2);f.annotations[1].flags|=NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE;
 CHECK(f.read().complete&&f.read().destination==RouteDestination::NativeBoundary);f=Fixture{};
 NimbySignalState filtered{};filtered.signal_id=balise;filtered.flags=NIMBY_SIGNAL_FILTER_VALID;filtered.exception_count=1;
 f.states={nimby::SignalState(filtered)};CHECK(!f.read().complete);f=Fixture{};
 f.textures.clear();CHECK(!f.read().complete);f=Fixture{};
 f.annotations[0].train_id++;CHECK(!f.read().complete);f=Fixture{};
 f.annotations.push_back(f.annotations.back());CHECK(!f.read().complete);f=Fixture{};
 f.annotations.erase(f.annotations.begin());CHECK(f.read().complete&&f.read().markers.size()==1);f=Fixture{};
 f.annotations[0].track_fraction=.31;CHECK(!f.read().complete);f=Fixture{};
 // Directed V2 ranges must work in reverse and retain marker order.
 f.signals[0]=signal(source,NIMBY_SIGNAL_PATH,.9,track,1);f.signals[2]=signal(balise,NIMBY_SIGNAL_BALISE,.1);
 f.row.source_fraction=.9;f.row.source_direction=-1;f.parts={{train,source,track,.9,.1}};
 f.annotations={{train,source,marker,track,.3,600,-1,NIMBY_SIGNAL_MARKER,0,0},{train,source,balise,track,.1,800,-1,NIMBY_SIGNAL_BALISE,1,13}};
 CHECK(f.read().complete&&f.read().markers.size()==1);f=Fixture{};
 f.parts={{train,source,track,.1,.4},{train,source,track,.5,.9}};CHECK(!f.read().complete);
 std::cout<<"Selected route: directed extent, native boundaries, marker joins and conservative failures PASS\n";
 return 0;
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
