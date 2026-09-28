#include <nimby/detail/observation_session.hpp>
#include <nimby/signal_approach.hpp>
#include <nimby/blocks.hpp>
#include <iostream>
#include <limits>
#define CHECK(x) do { if(!(x)) throw std::runtime_error("line " + std::to_string(__LINE__) + ": " #x); } while(false)
using namespace nimby;
int main(){try{
 std::vector<Signal> signals{Signal{{10,1,.8,-1,NIMBY_SIGNAL_BALISE}},Signal{{90,1,.2,1,NIMBY_SIGNAL_PATH}},
  Signal{{30,2,.1,-1,NIMBY_SIGNAL_PATH}},Signal{{20,2,.9,1,NIMBY_SIGNAL_PATH}}};
 // B -> B join reverses the native direction on the next segment.
 std::vector<TrackNode> nodes{TrackNode{{1,0,2,0,0}},TrackNode{{2,0,1,10,0}}};
 SignalTopology topology(signals,nodes);
 auto ordered=topology.getSignalsForTrack(1);CHECK(ordered[0].getId()==90&&ordered[1].getId()==10);
 CHECK(topology.getSignalsForTrack(1,-1)[0].getId()==10);
 CHECK(topology.getSignalsForTrack(1,1,true).size()==1);
 CHECK(topology.getSignalsForTrack(999).empty());
 auto trace=topology.traceFrom(Position{1,.2,1});
 CHECK(trace.sections.size()==2&&trace.sections[1].direction==-1);
 CHECK(trace.sections[0].orderedSignals.size()==2&&trace.sections[1].orderedSignals[0].getId()==20);
 CHECK(trace.stop==SignalTraceStop::UnobservedConnection);
 CHECK(topology.traceFrom(Position{1,.3,1}).sections[0].orderedSignals.size()==1);
 CHECK(topology.traceFrom(Position{1,.3,-1}).sections[0].orderedSignals[0].getId()==90);
 CHECK(topology.traceFrom(Position{1,0,1},1).stop==SignalTraceStop::TrackLimit);
 CHECK(topology.traceFrom(Position{1,0,1},0).sections.empty());
 CHECK(topology.traceFrom(Position{999,0,1}).stop==SignalTraceStop::UnknownTrack);
 auto axis=topology.getTrackAxis(1);CHECK(axis&&axis->x==1&&axis->y==0);
 CHECK(!topology.getTrackAxis(999));
 nodes={TrackNode{{1,3,2,0,0}},TrackNode{{2,1,3,1,0}},TrackNode{{3,2,1,2,0}}};
 CHECK(SignalTopology(signals,nodes).traceFrom(Position{1,0,1}).stop==SignalTraceStop::Cycle);
 nodes={TrackNode{{1,0,2,0,0}},TrackNode{{2,1,1,1,0}}};
 CHECK(SignalTopology(signals,nodes).traceFrom(Position{1,0,1}).stop==SignalTraceStop::AmbiguousConnection);
 nodes={TrackNode{{1,0,2,0,0}},TrackNode{{2,0,0,1,0}}};
 CHECK(SignalTopology(signals,nodes).traceFrom(Position{1,0,1}).stop==SignalTraceStop::InconsistentConnection);
 nodes.pop_back();
 CHECK(SignalTopology(signals,nodes).traceFrom(Position{1,0,1}).stop==SignalTraceStop::InconsistentConnection);
 CHECK(!SignalTopology(signals,nodes).getTrackAxis(1));
 bool rejected=false;try{topology.getSignalsForTrack(1,0);}catch(const std::invalid_argument&){rejected=true;}CHECK(rejected);
 rejected=false;try{topology.traceFrom(Position{1,std::numeric_limits<double>::quiet_NaN(),1});}catch(const std::invalid_argument&){rejected=true;}CHECK(rejected);
 std::reverse(signals.begin(),signals.end());CHECK(SignalTopology(signals,nodes).getSignalsForTrack(1)[0].getId()==90);
 // Main track -> turnout -> direct or diverging signal, with an attachment
 // inside a track rather than at its endpoint. IDs are deliberately unordered.
 signals={Signal{{90,1,.2,1,4}},Signal{{10,1,.8,1,4}},Signal{{30,4,.6,1,4}}};
 nodes={TrackNode{{1,0,2,0,0}},TrackNode{{2,1,0,1,0}},
        TrackNode{{3,0,4,0,1}},TrackNode{{4,3,0,1,1}}};
 std::vector<TrackJunction> junctions{TrackJunction{{3,1,.4,1,1}}};
 SignalTopology fork(signals,nodes,junctions);
 auto next=fork.findNextSignals(Position{1,.2,1},100,false,90);
 CHECK(next.nextSignals.size()==2&&next.nextSignals[0].getId()==10&&next.nextSignals[1].getId()==30);
 CHECK(!next.truncated&&!next.incomplete);
 trace=fork.traceFrom(Position{1,.2,1});
 CHECK(trace.stop==SignalTraceStop::Junction&&trace.continuations.size()==2);
 CHECK(trace.sections.size()==1&&trace.sections[0].orderedSignals.size()==1&&trace.sections[0].toFraction==.4);
 next=fork.findNextSignals(Position{4,.6,-1},100,false,30);
 CHECK(next.nextSignals.size()==1&&next.nextSignals[0].getId()==90); // Merge at .4, never sees .8.
 next=fork.findNextSignals(Position{1,.9,-1},100,false,10);
 CHECK(next.nextSignals.size()==1&&next.nextSignals[0].getId()==90); // Trailing turnout: no U-turn.
 CHECK(fork.findNextSignals(Position{1,.5,1}).nextSignals.size()==1); // Turnout behind us.
 CHECK(fork.findNextSignals(Position{1,.4,1}).nextSignals.size()==2); // Exact attachment.
 CHECK(fork.findNextSignals(Position{1,.2,1},0).truncated);
 CHECK(fork.findNextSignals(Position{999,0,1}).incomplete);
 // A second branch at the same fraction must not hide the first branch.
 nodes.push_back(TrackNode{{5,6,0,0,2}});nodes.push_back(TrackNode{{6,0,5,1,2}});
 signals.push_back(Signal{{20,6,.5,-1,4}});junctions.push_back(TrackJunction{{5,1,.4,1,-1}});
 SignalTopology multi(signals,nodes,junctions);
 CHECK(multi.findNextSignals(Position{1,.2,1},100,false,90).nextSignals.size()==3);
 CHECK(multi.findNextSignals(Position{1,.2,1},100,true,90).nextSignals.size()==3);
 auto reverse=multi.findNextSignals(Position{6,.5,1},100,false,20);
 CHECK(reverse.nextSignals.size()==1&&reverse.nextSignals[0].getId()==90);
 std::reverse(junctions.begin(),junctions.end());
 CHECK(SignalTopology(signals,nodes,junctions).findNextSignals(Position{1,.2,1},100,false,90).nextSignals.size()==3);
 // Invalid native direction / stale branch generation is never used as an edge.
 junctions={TrackJunction{{3,1,.4,0,1}},TrackJunction{{999,1,.4,1,1}}};
 CHECK(SignalTopology(signals,nodes,junctions).findNextSignals(Position{1,.2,1},100,false,90).nextSignals.size()==1);
 // An approach is the first facing boundary, not any signal further down a
 // line or a train's tail still occupying a block behind its head.
 {
  const std::vector<TrackNode> straight{TrackNode{{1,0,2,0,0}},TrackNode{{2,1,0,1,0}}};
  std::vector<Signal> panels{Signal{{80,1,.5,-1,NIMBY_SIGNAL_PATH}},Signal{{81,2,.7,-1,NIMBY_SIGNAL_PATH}}};
  SignalTopology route(panels,straight,{},SignalDirectionConvention::Forward);
  CHECK(firstApproachedSignal(route,{1,.2,1})==80);
  CHECK(firstApproachedSignal(route,{1,.5,1})==80);
  CHECK(firstApproachedSignal(route,{1,.6,1})==81);
  CHECK(approachedSignals(route,{1,.2,1},2)==std::vector<Id>({80,81}));
  CHECK(approachedSignals(route,{1,.6,1},2)==std::vector<Id>({81}));
  CHECK(approachedSignals(route,{2,.8,1},2).empty());
  CHECK(approachedSignals(route,{1,.2,-1},2).empty());
  for(size_t invalid:{size_t(0),size_t(17)}){bool rejected=false;
   try{approachedSignals(route,{1,.2,1},invalid);}catch(const std::invalid_argument&){rejected=true;}CHECK(rejected);}
  panels.push_back(Signal{{83,2,.9,-1,NIMBY_SIGNAL_PATH}});
  CHECK(approachedSignals(SignalTopology(panels,straight,{},SignalDirectionConvention::Forward),{1,.2,1},2)==std::vector<Id>({80,81}));
  CHECK(approachedSignals(SignalTopology(panels,straight,{},SignalDirectionConvention::Forward),{1,.6,1},2)==std::vector<Id>({81,83}));
  CHECK(!firstApproachedSignal(route,{1,.2,-1}));
  CHECK(!firstApproachedSignal(route,{2,.8,1}));
  panels.push_back(Signal{{82,1,.5,-1,NIMBY_SIGNAL_PATH}});
  CHECK(!firstApproachedSignal(SignalTopology(panels,straight,{},SignalDirectionConvention::Forward),{1,.2,1}));
 }
 // Live regression: a train held at a reverse-facing signal had a head one
 // rounding unit beyond the stored signal fraction. Occupancy correctly saw
 // entry contact, but approach skipped the panel and left the train stranded.
 for(int direction:{1,-1})for(double fraction:{.5,.24489791573599803}){
  const std::vector<TrackNode> track{TrackNode{{1,0,0,0,0}}};
  const std::vector<Signal> panels{Signal{{80,1,fraction,-direction,NIMBY_SIGNAL_PATH}}};
  const SignalTopology route(panels,track,{},SignalDirectionConvention::Forward);
  const BlockSection downstream{1,direction==1?fraction:0.,direction==1?1.:fraction};
  const BlockEntry entry{1,fraction,direction};
  const auto footprint=[&](double head){return direction==1?TrainFootprint{90,1,0.,head}:TrainFootprint{90,1,head,1.};};
  const double roundoff=std::nextafter(fraction,direction==1?1.:0.);
  const auto waiting=footprint(roundoff);
  CHECK(BlockReader(std::span(&waiting,1),true).read(std::span(&downstream,1),entry)==BlockOccupancy::Clear);
  CHECK(firstApproachedSignal(route,{1,roundoff,direction})==80);
  CHECK(firstApproachedSignal(route,{1,fraction,direction})==80);
  const double contactLimit=fraction+direction*detail::trackFractionRounding;
  const auto contact=footprint(contactLimit);
  CHECK(BlockReader(std::span(&contact,1),true).read(std::span(&downstream,1),entry)==BlockOccupancy::Clear);
  CHECK(firstApproachedSignal(route,{1,contactLimit,direction})==80);
  const double pastContact=std::nextafter(contactLimit,direction==1?1.:0.);
  const auto past=footprint(pastContact);
  CHECK(BlockReader(std::span(&past,1),true).read(std::span(&downstream,1),entry)==BlockOccupancy::Occupied);
  CHECK(!firstApproachedSignal(route,{1,pastContact,direction}));
  // A genuine head entry stops being an approach and occupies the block.
  const double moved=fraction+direction*1e-10;
  const auto entered=footprint(moved);
  CHECK(BlockReader(std::span(&entered,1),true).read(std::span(&downstream,1),entry)==BlockOccupancy::Occupied);
  CHECK(!firstApproachedSignal(route,{1,moved,direction}));
  CHECK(!firstApproachedSignal(route,{1,roundoff,-direction}));
  auto ambiguous=panels;
  ambiguous.push_back(Signal{{81,1,roundoff,-direction,NIMBY_SIGNAL_PATH}});
  CHECK(!firstApproachedSignal(SignalTopology(ambiguous,track,{},SignalDirectionConvention::Forward),{1,roundoff,direction}));
 }
 std::cout<<"Signal order, direction changes, cycles, missing and ambiguous connections: OK\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
