#pragma once
#include "engine/live_state.h"
#include <vector>
namespace nimby::engine {
struct TrackUsage { uint64_t train_id{},track_id{};double begin{},end{}; };
// Native query ranges, NOT an ordered route. Each component fails closed independently.
bool read_reservations(ReadMemory,void*,const LiveState&,bool,std::vector<TrackUsage>&) noexcept;
bool read_occupations(ReadMemory,void*,const LiveState&,bool,std::vector<TrackUsage>&) noexcept;
}
