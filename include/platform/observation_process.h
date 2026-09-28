#pragma once
#include <nimby/detail/observation.h>
#include <nimby/detail/construction.h>
#include "engine/live_state.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace nimby::platform {
// One owned, read-only connection to a game process. The backend validates the
// executable before publishing metadata. No OS handle escapes this interface.
// Observation's registry mutex serializes all calls and destruction; this class
// does not create threads or add a second lock. Never copy a process connection.
class ObservationProcess {
public:
    ObservationProcess();
    ~ObservationProcess();
    ObservationProcess(const ObservationProcess&) = delete;
    ObservationProcess& operator=(const ObservationProcess&) = delete;

    uint32_t open(uint32_t requestedPid);
    bool alive() const noexcept;
    bool read(uint64_t address, void* output, size_t size) const noexcept;
    static engine::LiveStateProfile profile() noexcept;

    // Explicit control operations. Ordinary observation never gains write
    // permission. Unsupported Linux control returns CLOCK_WRITE_FAILED.
    uint32_t setClock(int64_t utc, NimbySimulationClock& output) noexcept;
    uint32_t setClockAndRecalculate(int64_t utc, NimbySimulationClock& output,
                                    uint32_t& count) noexcept;
    uint32_t construction(const NimbyConstructionRequest*,uint64_t pollToken,NimbyConstructionResult&) noexcept;

    // Initialized by open(); thereafter borrowed, immutable session metadata.
    uint32_t pid{};
    uint64_t base{};
    NimbyBinaryInfo binary{};
    std::filesystem::path game_directory;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
}
