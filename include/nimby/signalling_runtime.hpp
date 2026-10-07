#pragma once
#include <nimby/mod.hpp>
#include <nimby/signal_network.hpp>
#include <nimby/signal_observation.hpp>
#include <nimby/automatic_driving.hpp>
#include <nimby/signalling_control.hpp>
#include <nimby/detail/signal_animation.hpp>
#include <nimby/detail/live_texture_tracking.hpp>
#include <unordered_set>

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
    // Live maps use bounded heap storage; the versioned command ABI above
    // keeps its original capacity and never grows the Windows worker stack.
    struct LiveNetworkSignalResult {
        std::uint64_t signal;
        struct { typename Rules::Decision decision; } result;
    };
    struct LiveNetworkResult {
        struct {
            std::vector<LiveNetworkSignalResult> items;
            std::span<const LiveNetworkSignalResult> values() const {return items;}
        } signals;
    };
    static constexpr std::size_t liveSignalLimit=[] {
        if constexpr(requires {Rules::maxLiveSignals;})return Rules::maxLiveSignals;
        else return Rules::maxSignals;
    }();
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
        const auto decisions=networkDecisions(request.signals.values(),live,Rules::maxSignals);
        NetworkResult result;
        for(const auto& decision:decisions)
            result.signals.push_back({decision.id,image(decision.decision,request.simulationMs,request.halfPeriodMs)});
        return result;
    }
    static LiveNetworkResult evaluateLiveNetwork(std::span<const typename Rules::Signal> input) {
        const auto decisions=networkDecisions(input,true,liveSignalLimit);
        LiveNetworkResult result;result.signals.items.reserve(decisions.size());
        for(const auto& decision:decisions)result.signals.items.push_back({decision.id,{decision.decision}});
        return result;
    }
private:
    static auto networkDecisions(std::span<const typename Rules::Signal> input,bool live,std::size_t limit) {
        if(input.size()>limit)throw std::invalid_argument("Signal evaluation limit exceeded");
        const auto signals = [&] {
            if constexpr(requires { Rules::prepareNetwork(input); })return Rules::prepareNetwork(input);
            else return input;
        }();
        return nimby::evaluateSignalsWithFallback<typename Rules::Decision>(std::span<const typename Rules::Signal>{signals},[live](const auto& signal,const auto& next) {
                if constexpr(controllable)if(live)if(auto forced=controlState().forced(signal.id))return forced;
                return Rules::decide(signal,next);
            },
            [](const auto& signal) {
                if constexpr(requires { Rules::invalidNetworkDecision(signal); }) return Rules::invalidNetworkDecision(signal);
                else return Rules::invalidNetworkDecision();
            },limit);
    }
public:
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
    template<class Cleanup> static void cleanupLive(Cleanup cleanup) {
        if constexpr(controllable)controlState().invalidateAndCleanup(std::move(cleanup));
        else cleanup();
    }
    static void restoreLive() {
        cleanupLive([] {
            rendered().clear();
            auto& owned = liveTextures();
            detail::restoreTrackedTextures(owned,[](Id){return true;},
                [](std::span<const Id> ids){SignalTextures::inGame().restore(ids);},[](Id){});
        });
    }
    static void stopLive() {
        cleanupLive([] {
            if constexpr(requires(Id id,typename Rules::Decision decision){Rules::drivingRule(id,decision);})
                AutomaticDriving::release();
        });
    }
    struct Rendered { detail::SignalAnimation animation; std::string catalogue; std::chrono::steady_clock::time_point renewed; };
    static std::unordered_map<Id,Rendered>& rendered() {
        static std::unordered_map<Id,Rendered> values;return values;
    }
    static void observeLive(const Snapshot& snapshot) {
        const auto clock = snapshot.getSimulationClock();
        if (!clock) throw std::runtime_error("Simulation clock unavailable");
        const auto simulationMs=clock->getElapsedTime().count();
        constexpr int64_t halfPeriodMs=500;
        checkClock(simulationMs,halfPeriodMs);
        // Read C++ panel values owned by this mod's SDK adapter. Until the
        // native session/UI bridge is ready, this returns Unavailable.
        auto states = [&] {
            if constexpr(requires { Rules::observe(snapshot); })return Rules::observe(snapshot);
            else return observeSignals(snapshot, Rules::textureSet, liveSignalLimit);
        }();
        std::vector<typename Rules::Signal> signals;signals.reserve(states.size());
        std::unique_lock<std::mutex> controlLock;
        if constexpr(controllable){controlLock=std::unique_lock(controlState().mutex);controlState().observe(snapshot,states);}
        for (auto& state : states) {
            state.settings=readSignalSettings(state.id);
            if constexpr(controllable)controlState().overlay(state.id,state.settings);
            signals.push_back(Rules::fromLive(state));
        }
        const auto result = evaluateLiveNetwork(signals);
        if constexpr(controllable)for(const auto& row:result.signals.values())controlState().decisions[row.signal]=row.result.decision;
        if constexpr(requires(Id id,typename Rules::Decision decision){Rules::drivingRule(id,decision);}) {
            std::vector<SignalDrivingRule> drivingRules;
            for(const auto& row:result.signals.values())
                if(const auto rule=Rules::drivingRule(row.signal,row.result.decision))drivingRules.push_back(*rule);
            const bool maximumLineSpeed=[] {if constexpr(requires{Rules::maximumLineSpeed;})return Rules::maximumLineSpeed;else return false;}();
            AutomaticDriving::publish(drivingRules,Milliseconds{1000},maximumLineSpeed);
        }
        auto& owned = liveTextures();
        if constexpr(controllable)controlState().publishTrains();
        if(controlLock.owns_lock())controlLock.unlock();
        // Diagnostics receive the same facts, after urgent driving publication.
        if constexpr(requires { Rules::diagnoseLive(snapshot,states,result); })
            Rules::diagnoseLive(snapshot,states,result);
        std::unordered_map<Id,std::string_view> catalogues;
        catalogues.reserve(states.size());
        for(const auto& state:states)catalogues.emplace(state.id,state.textureSet);
        // Signals deleted or reassigned to another catalogue cease to be owned.
        detail::restoreTrackedTextures(owned,[&](Id id){return !catalogues.contains(id);},
            [](std::span<const Id> ids){SignalTextures::inGame().restore(ids);},[](Id id){rendered().erase(id);});
        std::unordered_set<Id> ownedIds(owned.begin(),owned.end());
        std::vector<TextureUpdate> updates;updates.reserve(result.signals.values().size());
        std::vector<std::pair<Id,Rendered>> changed;changed.reserve(result.signals.values().size());
        const auto now=std::chrono::steady_clock::now();
        size_t refused=0;
        for (const auto& row : result.signals.values()) {
            // Record before sending: a partially successful batch is restored
            // by observationLost if any later render fails.
            if(!detail::trackLiveTexture(row.signal,owned,ownedIds)){++refused;continue;}
            const auto animation=detail::signalAnimation<Rules>(row.result.decision,halfPeriodMs);
            const auto source=catalogues.find(row.signal);
            if(source==catalogues.end())throw std::logic_error("Missing rendered signal type");
            // React immediately to changes; renew identical leases less often.
            // Reading occupancy must not repeatedly reload the same texture.
            const auto previous=rendered().find(row.signal);
            if(previous!=rendered().end()&&previous->second.animation==animation&&
               previous->second.catalogue==source->second&&
               now-previous->second.renewed<Milliseconds{500})continue;
            const bool animated=animation.first!=animation.alternate;
            updates.push_back({row.signal,{std::string(source->second),animation.first},animated?animation.alternate:std::string{},
                Milliseconds{animated?animation.everyMs:0},Milliseconds{2500}});
            changed.push_back({row.signal,{animation,std::string(source->second),now}});
        }
        SignalTextures::inGame().publish(updates);
        for(auto& [signal,value]:changed)rendered()[signal]=std::move(value);
        // Do not throw here: that would invalidate healthy driving rules and
        // renewed peers. Native visuals for refused IDs are not proven safe.
        static bool capacityReported=false;
        if(refused&&!capacityReported){
            detail::diagnostics::write("mods","ERROR","Automatic textures: 4096 tracked IDs reached while cleanup is pending. New overrides refused; their native visual indications are unverified. Driving and existing texture renewals continue.");
            capacityReported=true;
        }else if(!refused&&capacityReported){
            detail::diagnostics::write("mods","INFO","Automatic textures: cleanup recovered; all current overrides admitted.");
            capacityReported=false;
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
