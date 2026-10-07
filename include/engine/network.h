#pragma once
#include "engine/live_state.h"
#include <string>
#include <nimby/detail/observation.h>
#include <vector>
#include <span>
#include "engine/named_signal_extensions.h"
namespace nimby::engine {
// Experimental, read-only layout profile. Evidence: docs/research/network.md.
struct Track { uint64_t id{}, station_id{}; float physical_mps{}, manual_mps{}, limit_mps{}; uint64_t links[2]{}; double x{},y{}; bool geometry{};
    // Zero means unavailable, never a zero-length rail. This metric is used by
    // the native route scanner; it is not a chord reconstructed from x/y.
    double native_length_m{};
};
struct Station { uint64_t id{}; std::string name; }; // Selected manual/automatic name; empty if unresolved.
struct Signal { uint64_t id{}, track_id{}; double fraction{}; int direction{}, kind{}; uint64_t textures_hash{}; bool filter_available{}, filter_default_ignored{}; uint32_t exception_count{};
    bool extensions_available{};
    std::vector<NamedSignalExtension> extensions;
};
struct SignalTextureState { uint64_t id{}; int32_t state{}; };
bool read_signal_texture_states(ReadMemory read, void* context, const LiveState& state,
                                bool recognized, std::vector<SignalTextureState>& out) noexcept;
struct Network { std::vector<NimbyPlatform> platforms; std::vector<Track> tracks; std::vector<Station> stations; std::vector<Signal> signals; std::vector<NimbyTrackJunction> junctions; };
struct TrainPosition { uint64_t track_id{}; double fraction{}; int direction{}; };
struct TrainMetadataCatalog {
    std::vector<NimbyTrainMetadata> trains;
    std::vector<NimbyLineMetadata> lines;
    std::vector<NimbyTag> tags;
    std::vector<NimbyObjectTagsState> tag_states;
    std::vector<NimbyObjectTag> object_tags;
    std::vector<NimbyTrainVehicle> vehicles;
    std::vector<NimbyVehicleModel> models;
    bool tags_available=false,lines_available=false;
    bool models_available=false;
};
struct Train { uint64_t id{}; std::string name; bool present{}, positioned{}, speed_available{}; double speed_mps{}; TrainPosition position{}; bool path_available{}; std::vector<uint64_t> path; NimbyTrainService service{}; NimbyTrainDetails details{}; bool line_stops_available{}; std::vector<NimbyLineStop> line_stops; int32_t order_mode{}; };
bool decode_train_service(const void* motion,size_t size,int32_t order_mode,NimbyTrainService& out) noexcept;
bool read_trains(ReadMemory read, void* context, const LiveState& state,
                 bool recognized_build, std::vector<Train>& out, bool presenceOnly=false,
                 TrainMetadataCatalog* metadata=nullptr,bool includePaths=true,
                 uint32_t dataFlags=NIMBY_TRAIN_DATA_ALL) noexcept;
enum class NetworkScope { Complete, Signalling, Topology };
// Topology retains the same validated nodes, junctions, native lengths and
// signal positions. It omits station/platform names and signal filters.
bool read_network(ReadMemory read, void* context, const LiveState& state,
                  bool recognized_build, Network& out, NetworkScope scope=NetworkScope::Complete) noexcept;
// Train tools need only referenced speed limits and station names. Missing
// objects remain unavailable locally; no map-wide topology/signals are read.
bool read_train_network(ReadMemory,void*,const LiveState&,bool recognized_build,
                        std::span<const Train>,Network&) noexcept;
// Fresh bounded topology around one texture set; no cross-capture geometry cache.
bool read_signalling_network(ReadMemory,void*,const LiveState&,uint64_t textures_hash,Network&) noexcept;
// Each model supplies its own upstream observation range. Include the next
// downstream boundary and all possible upstream approaches; this is geometry,
// never a permission to select a branch. No records survive the capture.
struct SignallingScope { uint64_t textures_hash{}; uint32_t approach_blocks{}; };
bool read_signalling_network(ReadMemory,void*,const LiveState&,std::span<const SignallingScope>,Network&) noexcept;
// Targeted membership check for visual commands. No native pointer escapes.
// false means unreadable/changing data; a stable missing generation sets found=false.
bool read_signal_membership(ReadMemory read, void* context, const LiveState& state,
                            bool recognized_build, uint64_t signal, bool& found) noexcept;
// Caller supplies a freshly resolved Motion record with a validated full train ID.
bool decode_train_position(const void* motion, size_t size, TrainPosition& out) noexcept;
const char* signal_kind_name(int kind) noexcept;
}
