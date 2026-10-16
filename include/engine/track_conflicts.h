#pragma once
#include <engine/track_conflict_decode.h>
#include <span>
namespace nimby::engine {
struct TrackConflicts {
    std::vector<NimbyTrackConflictState> states;
    std::vector<NimbyTrackConflict> rows;
};
// One bounded descriptor/table for the requested pool, no world-wide scan.
// Individual lists fail closed independently; false means the root/pool or
// qualification was unavailable and no list in the batch can be trusted.
// An optional shared budget stops before scratch allocation/iteration and
// invalidates the entire batch on exhaustion. This is not a native ABI.
bool read_track_conflicts(ReadMemory,void*,const LiveState&,bool recognized,
                         std::span<const uint64_t> trackIds,TrackConflicts&,
                         TrackConflictReadBudget* budget=nullptr) noexcept;
}
