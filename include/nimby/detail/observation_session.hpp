#pragma once
#include <nimby/detail/observation_values.hpp>
#include <nimby/detail/construction.h>

namespace nimby::detail {
// SDK-internal discovery for diagnostic hosts. In-game adapters pass their
// current PID explicitly. Kotlin applications use GameProcesses and open(pid).
inline std::uint32_t discoverProcess() {
    const auto result = platform::discoverProcess();
    if (result.status == NIMBY_INVALID_ARGUMENT)
        throw Exception({result.status, "FindProcess", "Several game processes; an explicit PID is required"});
    check(result.status, "FindProcess");
    return result.pid;
}

// Private RAII transport used by the precompiled Kotlin adapter. It performs
// synchronous calls only: scheduling belongs to ObservationLoop / the Kotlin
// caller. There is no public C++ client, cached latest snapshot or second worker.
// Copies outlive this handle. Destruction requires all callers to have stopped.
class ObservationSession {
public:
    explicit ObservationSession(std::uint32_t pid) : pid_(pid) {
        if (!pid) check(NIMBY_INVALID_ARGUMENT, "ObservationSession(pid)");
        (void)getVersion();
        check(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION, pid, &session_), "OpenProcess");
    }
    ObservationSession(const ObservationSession&) = delete;
    ObservationSession& operator=(const ObservationSession&) = delete;
    ~ObservationSession() { if (session_) NimbyInternal_CloseSession(session_); }
    std::uint32_t getProcessId() const noexcept { return pid_; }
    uint32_t construction(const NimbyConstructionRequest& request,NimbyConstructionResult& result){
        std::lock_guard lock(mutex_);return NimbyInternal_Construction(session_,&request,&result);
    }
    uint32_t pollConstruction(uint64_t token,NimbyConstructionResult& result){
        std::lock_guard lock(mutex_);return NimbyInternal_ConstructionPoll(session_,token,&result);
    }

    Snapshot::Ptr capture(SnapshotScope scope = SnapshotScope::Complete, const char* textureSet = nullptr) {
        std::lock_guard guard(mutex_);
        return Snapshot::capture(session_, scope, textureSet);
    }
    Snapshot::Ptr captureSignalling() { return capture(SnapshotScope::Signalling); }
    Snapshot::Ptr captureSignalling(std::string_view textureSet) {
        if (textureSet.empty() || textureSet.size() > 256 || textureSet.find('\0') != std::string_view::npos)
            throw std::invalid_argument("Invalid texture set");
        const std::string name(textureSet);
        return capture(SnapshotScope::Signalling, name.c_str());
    }
    std::optional<DrivingObservation> readTrain(Id id) {
        std::lock_guard guard(mutex_);
        NimbyDrivingObservation value{}; value.struct_size = sizeof value;
        const auto status = NimbyInternal_ReadTrainDriving(session_, id, &value);
        if (status == NIMBY_DATA_UNAVAILABLE) return std::nullopt;
        check(status, "ReadTrainDriving");
        return DrivingObservation{value};
    }
    // Retained for internal command-line diagnostics. Public clock operations
    // are exposed by NimbyClient in Kotlin and tested against the native ABI.
    SimulationClock setSimulationDateTime(std::chrono::sys_seconds utc) {
        std::lock_guard guard(mutex_);
        NimbySimulationClock result{}; result.struct_size = sizeof result;
        check(NimbyInternal_SetSimulationDateTime(session_, utc.time_since_epoch().count(), &result), "SetSimulationDateTime");
        return SimulationClock{result};
    }
    SimulationTimeChange setSimulationDateTimeAndRecalculateTrains(std::chrono::sys_seconds utc) {
        std::lock_guard guard(mutex_);
        NimbySimulationClock result{}; result.struct_size = sizeof result;
        std::uint32_t count{};
        check(NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(session_, utc.time_since_epoch().count(), &result, &count),
              "SetSimulationDateTimeAndRecalculateTrains");
        return {SimulationClock{result}, count};
    }
private:
    NimbySession session_{};
    const std::uint32_t pid_;
    std::mutex mutex_;
};
} // namespace nimby::detail
