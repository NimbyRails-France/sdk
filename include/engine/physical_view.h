#pragma once
#include "engine/on_sight.h"
#include <algorithm>
#include <cmath>

namespace nimby::engine::automatic {
// Owned observation, independent of OS clocks. The platform supplies elapsed
// monotonic milliseconds; simulation head distance is a separate input.
// This freshness check concerns only one physical observation, never the duration
// of an on-sight instruction. Windows refreshes occupation before each restricted
// integration step; the instruction itself ends at its measured exit signal.
struct PhysicalView {
    double head=0,covered=0,free=200;
    uint64_t observed=0;
    bool valid=false;
    SightClearance at(double currentHead,uint64_t monotonicMs) const {
        if(!valid || monotonicMs<observed || monotonicMs-observed>250 ||
           !std::isfinite(currentHead) || currentHead<head-1e-6)return {};
        return {true,std::max(0.0,std::min(free,covered)-(currentHead-head)),0};
    }
};
}
