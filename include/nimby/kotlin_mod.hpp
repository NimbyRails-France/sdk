#pragma once
#include <nimby/signalling_runtime.hpp>

// Transport SDK v1, intentionally independent of any national signalling rules.
// Mod authors use kotlin/src/nimby/Mod.kt, never this internal adapter header.
namespace nimby::kotlin {
struct Settings { std::uint64_t mask=0; };
struct Observation {
    BlockOccupancy block=BlockOccupancy::Unknown;
    bool fresh=false,routeKnown=false,forcedStop=false,lampFailed=false,redFlashCondition=false;
    std::int32_t next=0;
};
struct Decision { std::int32_t aspect=0,reason=0; };
struct Signal {
    Id id=0,nextSignal=0; Settings settings; Observation observation;
    SettingsStatus settingsStatus=SettingsStatus::Present; bool live=false;
    std::uint32_t typeIndex=0;
    Id approachingTrain=0;
};
struct Vehicle { double maxSpeedMps=0,maxAccelerationMps2=0,serviceBrakingMps2=0,tractiveEffortN=0,powerW=0,emptyMassKg=0,extraMassKg=0,lengthM=0; };
struct DrivingSettings { double brakeUse=0.8,responseSeconds=2,marginM=10; };
struct Constraint { Id source=0;double beginM=0,endM=0,speedMps=0;bool releaseByRear=true; };
struct DrivingInput { double headM=0,speedMps=0,lineSpeedMps=0;bool fresh=false,routeKnown=false,onSight=false;std::optional<double> visibleClearM; };
struct Plan { bool available=false;double speedCeilingMps=0,serviceDecelerationMps2=0,accelerationMps2=0;bool brakingRequired=false;Id limitingSource=0; };
struct Rules {
    using Settings=kotlin::Settings;using Observation=kotlin::Observation;using Decision=kotlin::Decision;
    using Signal=kotlin::Signal;using Vehicle=kotlin::Vehicle;using DrivingSettings=kotlin::DrivingSettings;
    using Constraint=kotlin::Constraint;using DrivingInput=kotlin::DrivingInput;using Plan=kotlin::Plan;
    static constexpr std::size_t maxSignals=512;
    static inline std::string textureSet,controlId;
    static std::optional<Decision> forcedDecision(int aspect);
    static std::optional<Decision> forcedDecision(std::string_view catalogue,int aspect);
    static inline bool maximumLineSpeed=false;
    static constexpr SignallingCommandNames names{"nrf.kotlin.evaluate.v1","nrf.kotlin.plan.v1","nrf.kotlin.read-train-plan.v1",
        "nrf.kotlin.render.v1","nrf.kotlin.network.v1","nrf.kotlin.occupancy.v1","nrf.kotlin.read-occupancy.v1"};
    static Decision evaluate(const Settings&,const Observation&);
    static std::optional<Decision> decide(const Signal&,const std::optional<Decision>&);
    static Signal fromLive(const LiveSignalState&);
    static std::vector<LiveSignalState> observe(const Snapshot&);
    static bool multipleTypes();
    static Decision unknownDecision();
    static Decision invalidNetworkDecision();
    static Decision invalidNetworkDecision(const Signal&);
    static Decision diagnosticDecision(const Decision&);
    static std::string texture(const Decision&,std::int64_t,std::int64_t);
    static std::optional<detail::SignalAnimation> animation(const Decision&);
    static std::optional<SignalDrivingRule> drivingRule(Id,const Decision&);
    static Plan plan(const Vehicle&,const DrivingSettings&,const DrivingInput&,std::span<const Constraint>);
    static std::span<const SignalCheckbox> checkboxes();
    static std::span<const SignalCheckbox> checkboxes(std::string_view catalogue);
    static std::string settingsId();
    static std::string diagnosticFile();
    static std::string aspectName(int);
    static std::string reasonName(int);
    static bool isFault(const Decision&);
    static bool isActive(const Decision&);
    static Vehicle withDynamics(const Vehicle& c,const TrainDynamics& o) {
        return {o.maxSpeedMps,o.maxAccelerationMps2,o.serviceBrakingMps2,o.tractiveEffortN,o.powerW,o.emptyMassKg,c.extraMassKg,o.lengthM};
    }
    static void diagnose(const Snapshot&,const std::vector<LiveSignalState>&,std::span<const Decision>);
    template<class Result> static void diagnoseLive(const Snapshot& snapshot,const std::vector<LiveSignalState>& states,const Result& result) {
        std::vector<Decision> decisions;for(const auto& row:result.signals.values())decisions.push_back(row.result.decision);
        diagnose(snapshot,states,decisions);
    }
};
using Runtime=SignallingRuntime<Rules>;
}
