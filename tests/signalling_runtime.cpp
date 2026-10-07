#include <nimby/signalling_runtime.hpp>
#include <iostream>

#define CHECK(x) do { if(!(x)) throw std::runtime_error("Failed line " + std::to_string(__LINE__) + ": " #x); } while(false)
namespace { int trainReads=0, blockReads=0; }
// Deterministic SDK adapter doubles: this test never attaches to a game.
namespace nimby {
std::optional<DrivingObservation> readTrain(Id id) {
    ++trainReads;
    if(!id) return std::nullopt;
    NimbyDrivingObservation value{};
    value.flags=NIMBY_DRIVING_CURRENT_VALID;
    value.current.max_speed_mps=20;
    value.train.flags=NIMBY_TRAIN_SPEED_VALID;
    if(id==2) value.train.flags|=NIMBY_TRAIN_SPEED_DEFAULTED;
    value.train.speed_mps=12;
    return DrivingObservation{value};
}
BlockReader readBlocks() {
    ++blockReads;
    const std::array trains{TrainFootprint{1,10,0.2,0.5}};
    return BlockReader(trains,false);
}
}
namespace {
// Deliberately not a national signalling system: numeric decisions and a toy model.
struct Rules {
    struct Settings {};
    struct Observation { int value=0; };
    using Decision=int;
    struct Signal { std::uint64_t id,nextSignal; int value; };
    struct Vehicle { double marker=0; };
    struct DrivingSettings {};
    struct DrivingInput {
        double headM,speedMps,lineSpeedMps;
        bool fresh,routeKnown,onSight;
        std::optional<double> visibleClearM;
    };
    struct Constraint { int value=0; };
    struct Plan { bool available=false; double value=0; };
    static constexpr std::size_t maxSignals=8;
    static constexpr auto textureSet="test_catalog";
    static constexpr nimby::SignallingCommandNames names{"evaluate","plan","read","render","network","block","live-block"};
    static Decision unknownDecision() { return 0; }
    static Decision invalidNetworkDecision() { return -1; }
    static Decision evaluate(const Settings&,const Observation& o) { return o.value; }
    static std::optional<Decision> decide(const Signal& s,const std::optional<Decision>& next) {
        if(s.value) return s.value;
        return next;
    }
    static std::string_view texture(Decision decision,std::int64_t,std::int64_t) { return decision>0 ? "known.svg" : "unknown.svg"; }
    static Plan plan(const Vehicle& v,const DrivingSettings&,const DrivingInput& in,std::span<const Constraint> constraints) {
        return {in.fresh,v.marker+in.speedMps+static_cast<double>(constraints.size())+in.visibleClearM.value_or(0)};
    }
    static Vehicle withDynamics(const Vehicle& v,const nimby::TrainDynamics& d) { return {v.marker+d.maxSpeedMps}; }
};
using Runtime=nimby::SignallingRuntime<Rules>;
// A different model's fallback must never be substituted on a broken link.
struct TypedRules : Rules {
    static Decision invalidNetworkDecision(const Signal& signal) { return -int(signal.id); }
};
struct AnimatedRules : Rules {
    static constexpr size_t maxLiveSignals=4096;
    static inline std::int64_t duration=250;
    static std::optional<nimby::detail::SignalAnimation> animation(Decision) {
        return nimby::detail::SignalAnimation{"on.svg","off.svg",duration};
    }
    static std::string texture(Decision,std::int64_t,std::int64_t) {
        throw std::runtime_error("A declared cadence must not be resampled at the legacy cadence");
    }
};
}
int main() {
    try {
        {
            // Cleanup can be transiently unavailable while thousands of IDs
            // rotate. Do not keep admitting new IDs into an unbounded backlog.
            std::vector<uint64_t> owned;std::unordered_set<uint64_t> ids;
            for(uint64_t id=1;id<=4096;++id)CHECK(nimby::detail::trackLiveTexture(id,owned,ids));
            size_t attempts=0;std::unordered_set<uint64_t> retired;
            const auto fail=[&](std::span<const uint64_t> batch){++attempts;CHECK(batch.size()<=4096);throw std::runtime_error("Transient transport failure");};
            for(int rotation=0;rotation<4;++rotation){
                nimby::detail::restoreTrackedTextures(owned,[](auto){return true;},fail,[&](auto id){retired.insert(id);});
                for(uint64_t id=4097+rotation*4096;id<=8192+rotation*4096;++id)CHECK(!nimby::detail::trackLiveTexture(id,owned,ids));
                CHECK(owned.size()==4096&&retired.empty());
                CHECK(nimby::detail::trackLiveTexture(1,owned,ids)); // An existing lease can still renew.
            }
            CHECK(attempts==4);
            nimby::detail::restoreTrackedTextures(owned,[](auto){return true;},[](auto batch){CHECK(batch.size()<=4096);},[&](auto id){retired.insert(id);ids.erase(id);});
            CHECK(owned.empty()&&retired.size()==4096);
            for(uint64_t id=20001;id<=24096;++id)CHECK(nimby::detail::trackLiveTexture(id,owned,ids));
            CHECK(owned.size()==4096);
        }
        {
            // Recover oversized pending lists left by the earlier algorithm.
            // A failed first chunk must not prevent successful later cleanup.
            std::vector<uint64_t> owned;for(uint64_t id=1;id<=10000;++id)owned.push_back(id);
            std::unordered_set<uint64_t> peer{20001,20002};size_t calls=0,retired=0;
            nimby::detail::restoreTrackedTextures(owned,[](auto){return true;},[&](auto batch){
                CHECK(batch.size()<=4096);for(auto id:batch)CHECK(!peer.contains(id));
                if(++calls==1)throw std::runtime_error("First chunk uncertain");
            },[&](auto){++retired;});
            CHECK(calls==3&&retired==5904&&owned.size()==4096&&peer.size()==2);
            nimby::detail::restoreTrackedTextures(owned,[](auto id){return id%2==0;},[](auto){},[](auto){});
            CHECK(owned.size()==2048);for(auto id:owned)CHECK(id%2==1);
            nimby::detail::restoreTrackedTextures(owned,[](auto){return true;},[](auto){},[](auto){});
            CHECK(owned.empty());
        }
        const auto blink=nimby::detail::signalAnimation<AnimatedRules>(1,500);
        CHECK(blink.first=="on.svg"&&blink.alternate=="off.svg"&&blink.everyMs==250);
        AnimatedRules::duration=750;
        CHECK(nimby::detail::signalAnimation<AnimatedRules>(1,500)!=blink); // Cache must notice cadence-only changes.
        AnimatedRules::duration=99;
        try { nimby::detail::signalAnimation<AnimatedRules>(1,500);CHECK(false); } catch(const std::invalid_argument&) {}
        CHECK(nimby::detail::signalAnimation<Rules>(1,500).everyMs==0); // Existing image callback.
        std::vector<AnimatedRules::Signal> largeNetwork;
        for(uint64_t id=1;id<=4096;++id)largeNetwork.push_back({id,id==4096?0:id+1,id==4096?7:0});
        const auto liveLarge=nimby::SignallingRuntime<AnimatedRules>::evaluateLiveNetwork(largeNetwork);
        CHECK(liveLarge.signals.values().size()==4096);
        for(const auto& row:liveLarge.signals.values())CHECK(row.result.decision==7);
        // The live evaluator must not call texture() before the urgent driving
        // publication. AnimatedRules throws if that redundant callback occurs.
        largeNetwork.push_back({4097,0,7});
        try {nimby::SignallingRuntime<AnimatedRules>::evaluateLiveNetwork(largeNetwork);CHECK(false);}catch(const std::invalid_argument&){}
        CHECK(sizeof(nimby::SignallingRuntime<AnimatedRules>::NetworkRequest)==sizeof(Runtime::NetworkRequest));
        Runtime::SignalRequest signal; signal.observation.value=3;
        CHECK(Runtime::evaluate(signal).texturePath.view()=="known.svg");
        signal.simulationMs=-1;
        try { Runtime::evaluate(signal); CHECK(false); } catch(const std::invalid_argument&) {}
        Runtime::NetworkRequest network;
        network.signals.push_back({1,2,0}); network.signals.push_back({2,0,7});
        const auto evaluated=Runtime::network(network);
        CHECK(evaluated.signals.items[0].result.decision==7 && evaluated.signals.items[0].signal==1);
        using TypedRuntime=nimby::SignallingRuntime<TypedRules>;
        TypedRuntime::NetworkRequest mixed;
        mixed.signals.push_back({3,4,0});mixed.signals.push_back({4,3,0});
        const auto cycle=TypedRuntime::network(mixed);
        CHECK(cycle.signals.items[0].result.decision==-3 && cycle.signals.items[1].result.decision==-4);
        mixed.signals.items[0].nextSignal=99;
        const auto missing=TypedRuntime::network(mixed);
        CHECK(missing.signals.items[0].result.decision==-3 && missing.signals.items[1].result.decision==-3);
        Runtime::DrivingRequest driving;
        driving.vehicle.marker=4; driving.speedMps=1; driving.fresh=true;
        driving.visibleClearKnown=true; driving.visibleClearM=3;
        driving.constraints.push_back({1});
        CHECK(Runtime::plan(driving).plan.value==9 && !Runtime::plan(driving).commandApplied);
        CHECK(Runtime::readTrainPlan({1,driving}).plan.value==40); // 4 + 20 + 12 + 1 + 3.
        CHECK(!Runtime::readTrainPlan({2,driving}).plan.available); // Default speed is not a reading.
        CHECK(!Runtime::readTrainPlan({0,driving}).plan.available);
        driving.constraints.count=4097;
        const auto before=trainReads;
        try { Runtime::readTrainPlan({1,driving}); CHECK(false); } catch(const std::invalid_argument&) {}
        CHECK(trainReads==before);
        Runtime::LiveBlockRequest live;
        CHECK(Runtime::readOccupancy(live).occupation==nimby::BlockOccupancy::Unknown && blockReads==0);
        live.sections.push_back({10,0,1});
        CHECK(Runtime::readOccupancy(live).occupation==nimby::BlockOccupancy::Occupied && blockReads==1);
        Runtime::BlockRequest block;
        block.sections.push_back({10,0,1});
        CHECK(Runtime::occupancy(block).occupation==nimby::BlockOccupancy::Unknown);
        block.coverageVerified=true;
        CHECK(Runtime::occupancy(block).occupation==nimby::BlockOccupancy::Clear);
        auto mod=Runtime::createMod();
        CHECK(mod.commands.size()==7 && mod.showTexture && mod.restoreTexture && mod.readTrain);
        Runtime::SignalResult result;
        signal.simulationMs=0;
        mod.commands[0].call(&signal,&result);
        CHECK(result.decision==3);
        std::cout << "PASS generic runtime dispatch, policy isolation, train conversion, blocks, invalid inputs\n";
    } catch(const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
