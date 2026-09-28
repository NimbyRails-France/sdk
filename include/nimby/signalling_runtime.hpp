#pragma once
#include <nimby/mod.hpp>
#include <nimby/signal_network.hpp>
#include <nimby/signal_observation.hpp>
#include <nimby/automatic_driving.hpp>
#include <nimby/signalling_control.hpp>
#include <nimby/detail/signal_animation.hpp>

namespace nimby {
// Generic command names supplied by a mod. All strings need static lifetime.
struct SignallingCommandNames {
    const char *evaluate, *plan, *readTrainPlan, *render, *network, *occupancy, *readOccupancy;
};
// The SDK owns orchestration and transport; Rules supplies national decisions,
// textures and the vehicle model. No national aspect/limit is defined here.
template<class Rules> class SignallingRuntime {
public:
    struct SignalRequest {
        typename Rules::Settings settings;
        typename Rules::Observation observation;
        std::int64_t simulationMs=0, halfPeriodMs=500;
    };
    struct SignalResult {
        typename Rules::Decision decision=Rules::unknownDecision();
        FixedText<96> texturePath;
    };
    struct DrivingRequest {
        typename Rules::Vehicle vehicle;
        typename Rules::DrivingSettings settings;
        double headM=0,speedMps=0,lineSpeedMps=0,visibleClearM=0;
        bool fresh=false,routeKnown=false,onSight=false,visibleClearKnown=false;
        FixedList<typename Rules::Constraint,4096> constraints;
    };
    struct DrivingResult { typename Rules::Plan plan; bool commandApplied=false; };
    struct TrainPlanRequest { std::uint64_t train=0; DrivingRequest driving; };
    struct RenderRequest { std::uint64_t signal=0; SignalRequest signalState; };
    struct NetworkRequest {
        FixedList<typename Rules::Signal,Rules::maxSignals> signals;
        std::int64_t simulationMs=0,halfPeriodMs=500;
    };
    struct NetworkSignalResult { std::uint64_t signal; SignalResult result; };
    struct NetworkResult { FixedList<NetworkSignalResult,Rules::maxSignals> signals; };
    struct BlockRequest {
        FixedList<BlockSection,512> sections;
        FixedList<TrainFootprint,4096> trains;
        bool coverageVerified=false;
    };
    struct BlockResult { BlockOccupancy occupation=BlockOccupancy::Unknown; };
    struct LiveBlockRequest { FixedList<BlockSection,512> sections; };

    // Public value API is useful to diagnostic hosts and unit tests as well as commands.
    static SignalResult evaluate(const SignalRequest& request) {
        checkClock(request.simulationMs,request.halfPeriodMs);
        return image(Rules::evaluate(request.settings,request.observation),request.simulationMs,request.halfPeriodMs);
    }
    static DrivingResult plan(const DrivingRequest& request) {
        typename Rules::DrivingInput input{request.headM,request.speedMps,request.lineSpeedMps,
            request.fresh,request.routeKnown,request.onSight,std::nullopt};
        if(request.visibleClearKnown) input.visibleClearM=request.visibleClearM;
        return {Rules::plan(request.vehicle,request.settings,input,request.constraints.values()),false};
    }
    static DrivingResult readTrainPlan(const TrainPlanRequest& request) {
        // Validate bounded inputs before any process access.
        request.driving.constraints.values();
        const auto observation=nimby::readTrain(request.train);
        if(!observation || observation->isSpeedDefaulted()) return {};
        const auto dynamics=observation->getCurrentDynamics();
        const auto speed=observation->getSpeedMps();
        if(!dynamics || !speed) return {};
        auto driving=request.driving;
        driving.speedMps=*speed;
        driving.vehicle=Rules::withDynamics(driving.vehicle,*dynamics);
        return plan(driving);
    }
    static SignalResult render(const RenderRequest& request) {
        const auto result=evaluate(request.signalState);
        showTexture(request.signal,std::string(result.texturePath.view()).c_str());
        return result;
    }
    static NetworkResult network(const NetworkRequest& request) {
        return evaluateNetwork(request,false);
    }
    static NetworkResult evaluateNetwork(const NetworkRequest& request,bool live) {
        checkClock(request.simulationMs,request.halfPeriodMs);
        const auto signals = [&] {
            if constexpr(requires { Rules::prepareNetwork(request.signals.values()); })return Rules::prepareNetwork(request.signals.values());
            else return request.signals.values();
        }();
        const auto decisions=nimby::evaluateSignalsWithFallback<typename Rules::Decision>(std::span<const typename Rules::Signal>{signals},[live](const auto& signal,const auto& next) {
                if constexpr(controllable)if(live)if(auto forced=controlState().forced(signal.id))return forced;
                return Rules::decide(signal,next);
            },
            [](const auto& signal) {
                if constexpr(requires { Rules::invalidNetworkDecision(signal); }) return Rules::invalidNetworkDecision(signal);
                else return Rules::invalidNetworkDecision();
            },Rules::maxSignals);
        NetworkResult result;
        for(const auto& decision:decisions)
            result.signals.push_back({decision.id,image(decision.decision,request.simulationMs,request.halfPeriodMs)});
        return result;
    }
    static BlockResult occupancy(const BlockRequest& request) {
        const BlockReader reader(request.trains.values(),request.coverageVerified);
        return {reader.read(request.sections.values())};
    }
    static BlockResult readOccupancy(const LiveBlockRequest& request) {
        const auto sections=request.sections.values();
        if(sections.empty()) return {};
        return {nimby::readBlocks().read(sections)};
    }
    static Mod createMod() {
        Mod mod{.showTexture=showTexture, .restoreTexture=restoreTexture,
            .readTrain=nimby::readTrain, .commands=commands()};
        // Live integration is explicit in the profile. Existing command-only
        // consumers keep their previous lifecycle and never start a worker.
        if constexpr (requires(const LiveSignalState& state) { Rules::fromLive(state); }) {
            mod.observe = observeLive;
            mod.observationScope = SnapshotScope::Signalling;
            mod.observationTextureSet = Rules::textureSet;
            if constexpr(requires { Rules::multipleTypes(); })
                if(Rules::multipleTypes())mod.observationTextureSet={};
            mod.observationIntervalMs = 20;
            mod.observationLost = restoreLive;
            mod.stop = stopLive;
        }
        if constexpr(controllable){mod.controlId=Rules::controlId.c_str();mod.control=handleControl;}
        return mod;
    }
private:
    static constexpr bool controllable=requires { Rules::forcedDecision(0);Rules::controlId; };
    static SignallingControl<Rules>& controlState(){static SignallingControl<Rules> state;return state;}
    static uint32_t handleControl(const NimbyControlRequest* r,NimbyControlResponse* out) {
        if(!r||!out||out->size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
        return controlState().handle(*r,*out);
    }
    static std::vector<Id>& liveTextures() {
        // The SDK worker alone accesses this list. No static destructor does
        // game work at DLL detach; the observationLost callback restores it.
        static std::vector<Id> signals;
        return signals;
    }
    static void restoreLive() {
        if constexpr(controllable){std::lock_guard guard(controlState().mutex);controlState().lost();
            controlState().releaseTrains();
        }
        rendered().clear();
        auto& owned = liveTextures();
        std::erase_if(owned, [](Id id) {
            try { restoreTexture(id); return true; } catch (...) { return false; }
        });
    }
    static void stopLive() {
        if constexpr(controllable){std::lock_guard guard(controlState().mutex);controlState().lost();
            controlState().releaseTrains();
        }
        if constexpr(requires(Id id,typename Rules::Decision decision){Rules::drivingRule(id,decision);})
            AutomaticDriving::release();
    }
    struct Rendered { detail::SignalAnimation animation; std::string catalogue; std::chrono::steady_clock::time_point renewed; };
    static std::unordered_map<Id,Rendered>& rendered() {
        static std::unordered_map<Id,Rendered> values;return values;
    }
    static void observeLive(const Snapshot& snapshot) {
        const auto clock = snapshot.getSimulationClock();
        if (!clock) throw std::runtime_error("Simulation clock unavailable");
        NetworkRequest request;
        request.simulationMs = clock->getElapsedTime().count();
        // Read C++ panel values owned by this mod's SDK adapter. Until the
        // native session/UI bridge is ready, this returns Unavailable.
        auto states = [&] {
            if constexpr(requires { Rules::observe(snapshot); })return Rules::observe(snapshot);
            else return observeSignals(snapshot, Rules::textureSet, Rules::maxSignals);
        }();
        std::unique_lock<std::mutex> controlLock;
        if constexpr(controllable){controlLock=std::unique_lock(controlState().mutex);controlState().observe(snapshot,states);}
        for (auto& state : states) {
            state.settings=readSignalSettings(state.id);
            if constexpr(controllable)controlState().overlay(state.id,state.settings);
            request.signals.push_back(Rules::fromLive(state));
        }
        const auto result = evaluateNetwork(request,true);
        if constexpr(controllable)for(const auto& row:result.signals.values())controlState().decisions[row.signal]=row.result.decision;
        // Optional consumer diagnostics receive precisely the observations used
        // for this decision, including the live panel values (no second capture).
        if constexpr(requires { Rules::diagnoseLive(snapshot,states,result); })
            Rules::diagnoseLive(snapshot,states,result);
        if constexpr(requires(Id id,typename Rules::Decision decision){Rules::drivingRule(id,decision);}) {
            std::vector<SignalDrivingRule> drivingRules;
            for(const auto& row:result.signals.values())
                if(const auto rule=Rules::drivingRule(row.signal,row.result.decision))drivingRules.push_back(*rule);
            const bool maximumLineSpeed=[] {if constexpr(requires{Rules::maximumLineSpeed;})return Rules::maximumLineSpeed;else return false;}();
            AutomaticDriving::publish(drivingRules,Milliseconds{1000},maximumLineSpeed);
        }
        auto& owned = liveTextures();
        if constexpr(controllable)controlState().publishTrains();
        // Signals deleted or reassigned to another catalogue cease to be owned.
        std::erase_if(owned, [&](Id id) {
            if (std::any_of(states.begin(), states.end(), [id](const auto& state) { return state.id == id; })) return false;
            try { restoreTexture(id); rendered().erase(id); return true; } catch (...) { return false; }
        });
        for (const auto& row : result.signals.values()) {
            // Record before sending: a partially successful batch is restored
            // by observationLost if any later render fails.
            if (std::find(owned.begin(), owned.end(), row.signal) == owned.end()) owned.push_back(row.signal);
            const auto animation=detail::signalAnimation<Rules>(row.result.decision,request.halfPeriodMs);
            const auto source=std::find_if(states.begin(),states.end(),[&](const auto& s){return s.id==row.signal;});
            if(source==states.end())throw std::logic_error("Missing rendered signal type");
            // React immediately to changes; renew identical leases less often.
            // Reading occupancy must not repeatedly reload the same texture.
            const auto now=std::chrono::steady_clock::now();
            const auto previous=rendered().find(row.signal);
            if(previous!=rendered().end()&&previous->second.animation==animation&&
               previous->second.catalogue==source->textureSet&&
               now-previous->second.renewed<Milliseconds{500})continue;
            const TextureImage image{source->textureSet,animation.first};
            const auto textures=SignalTextures::inGame();
            if(animation.first==animation.alternate) textures.showFor(row.signal,image,Milliseconds{2500});
            else textures.animateFor(row.signal,image,animation.alternate,Milliseconds{animation.everyMs},Milliseconds{2500});
            rendered()[row.signal]={animation,source->textureSet,now};
        }
    }
    static void checkClock(std::int64_t time,std::int64_t halfPeriod) {
        if(time<0 || halfPeriod<100 || halfPeriod>10000)
            throw std::invalid_argument("Invalid signal animation clock");
    }
    static SignalResult image(const typename Rules::Decision& decision,std::int64_t time,std::int64_t halfPeriod) {
        SignalResult result;
        result.decision=decision;
        result.texturePath.assign(Rules::texture(decision,time,halfPeriod));
        return result;
    }
    static void showTexture(Id signal,const char* path) {
        SignalTextures::inGame().show(signal,{Rules::textureSet,path});
    }
    static void restoreTexture(Id signal) { SignalTextures::inGame().restore(signal); }
    static std::span<const ModCommand> commands() {
        static const std::array entries{
            command<SignalRequest,SignalResult,evaluate>(Rules::names.evaluate),
            command<DrivingRequest,DrivingResult,plan>(Rules::names.plan),
            command<TrainPlanRequest,DrivingResult,readTrainPlan>(Rules::names.readTrainPlan),
            command<RenderRequest,SignalResult,render>(Rules::names.render),
            command<NetworkRequest,NetworkResult,network>(Rules::names.network),
            command<BlockRequest,BlockResult,occupancy>(Rules::names.occupancy),
            command<LiveBlockRequest,BlockResult,readOccupancy>(Rules::names.readOccupancy)
        };
        return entries;
    }
};
}
