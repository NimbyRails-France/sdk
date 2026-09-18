#pragma once
#include "engine/live_state.h"
#include <string>
#include <nimby/detail/observation.h>
#include <vector>
namespace nimby::engine {
// Experimental, read-only layout profile. Evidence: docs/research/network.md.
struct Track { uint64_t id{}, station_id{}; float physical_mps{}, manual_mps{}, limit_mps{}; uint64_t links[2]{}; double x{},y{}; bool geometry{}; };
struct Station { uint64_t id{}; std::string name; }; // Selected manual/automatic name; empty if unresolved.
struct Signal { uint64_t id{}, track_id{}; double fraction{}; int direction{}, kind{}; uint64_t textures_hash{}; bool filter_available{}, filter_default_ignored{}; uint32_t exception_count{}; };
struct SignalTextureState { uint64_t id{}; int32_t state{}; };
bool read_signal_texture_states(ReadMemory read, void* context, const LiveState& state,
                                bool recognized, std::vector<SignalTextureState>& out) noexcept;
struct Network { std::vector<NimbyPlatform> platforms; std::vector<Track> tracks; std::vector<Station> stations; std::vector<Signal> signals; std::vector<NimbyTrackJunction> junctions; };
struct TrainPosition { uint64_t track_id{}; double fraction{}; int direction{}; };
struct Train { uint64_t id{}; std::string name; bool present{}, positioned{}, speed_available{}; double speed_mps{}; TrainPosition position{}; bool path_available{}; std::vector<uint64_t> path; NimbyTrainService service{}; NimbyTrainDetails details{}; bool line_stops_available{}; std::vector<NimbyLineStop> line_stops; int32_t order_mode{}; };
bool decode_train_service(const void* motion,size_t size,int32_t order_mode,NimbyTrainService& out) noexcept;
bool read_trains(ReadMemory read, void* context, const LiveState& state,
                 bool recognized_build, std::vector<Train>& out) noexcept;
bool read_network(ReadMemory read, void* context, const LiveState& state,
                  bool recognized_build, Network& out) noexcept;
// Caller supplies a freshly resolved Motion record with a validated full train ID.
bool decode_train_position(const void* motion, size_t size, TrainPosition& out) noexcept;
const char* signal_kind_name(int kind) noexcept;
}
