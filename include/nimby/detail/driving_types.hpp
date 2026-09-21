#pragma once
// Included by client.hpp after Train/Position; implementation of the public value types.
#include <nimby/detail/driving.h>

namespace nimby {
struct DrivingCapabilities {
    bool targetedObservation;
    bool tractionCommand;
    bool serviceBrakeCommand;
    bool emergencyBrakeCommand;
    bool speedLimitCommand;
    bool nativeSettingsUi;
};
// SI units. Values supplied by the game's material model, not measured performance.
struct TrainDynamics {
    double maxSpeedMps, maxAccelerationMps2;
    double serviceBrakingMps2, emergencyBrakingMps2;
    double tractiveEffortN, powerW, emptyMassKg, lengthM;
};
namespace detail { struct DrivingAccess; }
class DrivingObservation {
    NimbyDrivingObservation data_;
    friend struct detail::DrivingAccess;
    static TrainDynamics convert(const NimbyTrainDynamics& v) {
        return {v.max_speed_mps,v.max_acceleration_mps2,v.service_braking_mps2,
            v.emergency_braking_mps2,v.tractive_effort_n,v.power_w,v.empty_mass_kg,v.length_m};
    }
public:
    explicit DrivingObservation(const NimbyDrivingObservation& data) : data_(data) {}
    Id getTrainId() const { return data_.train_id; }
    // Token is local to this Client. Reset state on reconnect as well as token changes.
    std::uint64_t getSessionGeneration() const { return data_.session_generation; }
    std::optional<TrainDynamics> getPurchasedDynamics() const {
        return data_.flags&NIMBY_DRIVING_PURCHASED_VALID?std::optional{convert(data_.purchased)}:std::nullopt;
    }
    std::optional<TrainDynamics> getCurrentDynamics() const {
        return data_.flags&NIMBY_DRIVING_CURRENT_VALID?std::optional{convert(data_.current)}:std::nullopt;
    }
    std::optional<double> getSpeedMps() const { return Train{data_.train}.getSpeedMps(); }
    bool isSpeedDefaulted() const { return Train{data_.train}.isSpeedDefaulted(); }
    std::optional<Position> getPosition() const { return Train{data_.train}.getPosition(); }
    Milliseconds getElapsedBegin() const { return Milliseconds{data_.elapsed_begin_ms}; }
    Milliseconds getElapsedEnd() const { return Milliseconds{data_.elapsed_end_ms}; }
    std::chrono::system_clock::time_point getCapturedAt() const {
        return std::chrono::system_clock::time_point{Milliseconds{data_.captured_unix_ms}};
    }
};
namespace detail {
struct DrivingAccess {
    static const NimbyDrivingObservation& native(const DrivingObservation& v) { return v.data_; }
};
}
}
