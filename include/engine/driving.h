#pragma once
#include "engine/live_state.h"
#include <nimby/detail/driving.h>
namespace nimby::engine {
// No cached object pointers and no heap/network enumeration.
bool read_train_driving(ReadMemory read, void* context, const LiveState& state,
                        uint64_t train_id, NimbyDrivingObservation& out) noexcept;
}
