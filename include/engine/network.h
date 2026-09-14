#pragma once
#include "engine/live_state.h"
#include <string>
#include <vector>
namespace nimby::engine {
// Experimental, read-only layout profile. Evidence: docs/research/network.md.
struct Track { uint64_t id{}, station_id{}; float physical_mps{}, manual_mps{}, limit_mps{}; uint64_t links[2]{}; double x{},y{}; bool geometry{}; };
struct Station { uint64_t id{}; std::string name; }; // Empty = automatic name not resolved.
struct Signal { uint64_t id{}, track_id{}; double fraction{}; int direction{}, kind{}; };
struct Network { std::vector<Track> tracks; std::vector<Station> stations; std::vector<Signal> signals; };
struct TrainPosition { uint64_t track_id{}; double fraction{}; int direction{}; };
struct Train { uint64_t id{}; std::string name; bool present{}, positioned{}; double speed_mps{}; TrainPosition position{}; bool path_available{}; std::vector<uint64_t> path; };
bool read_trains(ReadMemory read, void* context, const LiveState& state,
                 bool recognized_build, std::vector<Train>& out) noexcept;
bool read_network(ReadMemory read, void* context, const LiveState& state,
                  bool recognized_build, Network& out) noexcept;
// Caller supplies a freshly resolved Motion record with a validated full train ID.
bool decode_train_position(const void* motion, size_t size, TrainPosition& out) noexcept;
const char* signal_kind_name(int kind) noexcept;
}
