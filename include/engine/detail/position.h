#pragma once
#include "engine/network.h"
#include "engine/detail/memory_reader.h"
#include <cmath>

namespace nimby::engine::memory {
// Shared target record: full track identity, fraction, signed direction.
// Caller provides a validated buffer containing at least 17 bytes.
inline bool position(const void* p,TrainPosition& out) {
    out={field<uint64_t>(p,0),field<double>(p,8),field<int8_t>(p,16)};
    return (out.track_id>>48)==1 && std::isfinite(out.fraction) &&
        out.fraction>=0 && out.fraction<=1 && (out.direction==1 || out.direction==-1);
}
}
