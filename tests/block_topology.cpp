#include <nimby/block_topology.hpp>
#include <iostream>

#define CHECK(x) do { if (!(x)) throw std::runtime_error("line " + std::to_string(__LINE__) + ": " #x); } while(false)
using namespace nimby;

int main() { try {
    // Regression: Path's stored arrow is opposite to Signal_M_forward.
    // On one track the signal at .8 protects the interval back to .2.
    const std::vector<TrackNode> straight{TrackNode{{1,0,0,0,0}}};
    const std::vector<Signal> reversePaths{
        Signal{{1407,1,.8,1,NIMBY_SIGNAL_PATH}},
        Signal{{1409,1,.2,1,NIMBY_SIGNAL_PATH}},
        Signal{{1410,1,.5,-1,NIMBY_SIGNAL_PATH}}};
    CHECK(reversePaths[0].getDirection()==1 && reversePaths[0].getForwardDirection()==-1);
    CHECK(Signal({1,1,.5,1,NIMBY_SIGNAL_BALISE}).getForwardDirection()==1);
    const auto reverseBlock=BlockTopology(reversePaths,straight).read(1407);
    CHECK(reverseBlock.hasBoundary() && reverseBlock.nextSignal==1409);
    CHECK(reverseBlock.sections.size()==1 && reverseBlock.sections[0].begin==.2 && reverseBlock.sections[0].end==.8);
    CHECK(!BlockTopology(reversePaths,straight).read(1409).hasBoundary());
    const std::vector<TrainFootprint> atEntrance{{99,1,std::nextafter(.8,0.0),.9}};
    CHECK(reverseBlock.occupation(BlockReader(atEntrance,true))==BlockOccupancy::Clear);
    CHECK(reverseBlock.observe(BlockReader(atEntrance,true)).trains.empty());
    const std::vector<TrackNode> nodes{TrackNode{{1,0,2,0,0}}, TrackNode{{2,0,1,1,0}}};
    const std::vector<Signal> signals{
        Signal{{10,1,.2,-1,NIMBY_SIGNAL_PATH}},
        Signal{{11,1,.5,1,NIMBY_SIGNAL_PATH}}, // Opposite facing signal is not the exit.
        Signal{{20,2,.3,1,NIMBY_SIGNAL_PATH}}};
    BlockTopology topology(signals,nodes);
    auto block = topology.read(10);
    CHECK(block.hasBoundary() && block.nextSignal == 20 && block.sections.size() == 2);
    CHECK(block.sections[0].begin == .2 && block.sections[0].end == 1);
    CHECK(block.sections[1].begin == .3 && block.sections[1].end == 1); // B -> B reverses direction.
    const std::vector<TrainFootprint> tail{{100,2,.3,.4}};
    CHECK(BlockReader(tail,true).read(block.sections) == BlockOccupancy::Occupied);
    CHECK(block.observe(BlockReader(tail,true)).trains==std::vector<std::uint64_t>{100});
    CHECK(topology.read(999).issue == BlockBoundaryIssue::MissingSignal);
    CHECK(!topology.read(10,1).hasBoundary());
    CHECK(!topology.read(10,0).hasBoundary());
    CHECK(!topology.read(20).hasBoundary()); // Missing continuation is not a buffer stop.

    auto sameTrack = signals;
    sameTrack.push_back(Signal{{30,1,.7,-1,NIMBY_SIGNAL_PATH}});
    block = BlockTopology(sameTrack,nodes).read(10);
    CHECK(block.hasBoundary() && block.nextSignal == 30 && block.sections.size() == 1);
    CHECK(block.sections[0].end == .7);
    CHECK(block.traceStop == SignalTraceStop::SignalReached);
    const auto limitedTrace=SignalTopology(sameTrack,nodes,{},SignalDirectionConvention::Forward).traceToNextSignal(Position{1,.2,1},10);
    CHECK(limitedTrace.sections.size()==1 && limitedTrace.stop==SignalTraceStop::SignalReached);
    // Full traces remain available to callers studying the whole geometric path.
    CHECK(SignalTopology(sameTrack,nodes).traceFrom(Position{1,.2,1}).sections.size()==2);
    sameTrack.push_back(Signal{{31,1,.7,-1,NIMBY_SIGNAL_PATH}});
    CHECK(BlockTopology(sameTrack,nodes).read(10).issue == BlockBoundaryIssue::AmbiguousSignal);
    sameTrack = signals;
    sameTrack.push_back(Signal{{31,1,.2,-1,NIMBY_SIGNAL_PATH}});
    CHECK(BlockTopology(sameTrack,nodes).read(10).issue == BlockBoundaryIssue::AmbiguousSignal);

    const std::vector<TrackNode> forkNodes{
        TrackNode{{1,0,2,0,0}}, TrackNode{{2,1,0,1,0}},
        TrackNode{{3,0,4,0,1}}, TrackNode{{4,3,0,1,1}}};
    const std::vector<TrackJunction> forks{TrackJunction{{3,1,.4,1,1}}};
    const std::vector<Signal> forkSignals{
        Signal{{10,1,.2,-1,NIMBY_SIGNAL_PATH}}, Signal{{20,1,.8,-1,NIMBY_SIGNAL_PATH}},
        Signal{{30,4,.6,-1,NIMBY_SIGNAL_PATH}}};
    block = BlockTopology(forkSignals,forkNodes,forks).read(10);
    CHECK(!block.hasBoundary() && block.nextSignal == 0 && block.traceStop == SignalTraceStop::Junction);
    CHECK(block.sections.size() == 1 && block.sections[0].end == .4);
    CHECK(block.occupation(BlockReader({},true)) == BlockOccupancy::Unknown);
    CHECK(!block.observe(BlockReader(tail,true)).complete);
    CHECK(block.observe(BlockReader(tail,true)).trains.empty());
    auto beforeFork = forkSignals;
    beforeFork.push_back(Signal{{40,1,.3,-1,NIMBY_SIGNAL_PATH}});
    CHECK(BlockTopology(beforeFork,forkNodes,forks).read(10).nextSignal == 40);
    const std::vector<TrackNode> cycle{
        TrackNode{{1,3,2,0,0}}, TrackNode{{2,1,3,1,0}}, TrackNode{{3,2,1,2,0}}};
    const std::vector<Signal> lone{signals[0]};
    block = BlockTopology(lone,cycle).read(10);
    CHECK(!block.hasBoundary() && block.traceStop == SignalTraceStop::Cycle);
    auto duplicate = signals;
    duplicate.push_back(signals[0]);
    bool rejected = false;
    try { BlockTopology invalid(duplicate,nodes); } catch(const std::invalid_argument&) { rejected = true; }
    CHECK(rejected);
    std::cout << "PASS observed blocks, facing directions, tails, forks, boundaries and incomplete topology\n";
} catch(const std::exception& error) { std::cerr << error.what() << '\n'; return 1; } }
