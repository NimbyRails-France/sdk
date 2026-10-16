#pragma once
#include <engine/live_state.h>
#include <nimby/detail/track_endpoints.h>
#include <span>
#include <vector>

namespace nimby::engine {
enum class TrackEndpointKind : uint32_t {
    Unknown=NIMBY_TRACK_ENDPOINT_UNKNOWN,
    Connected=NIMBY_TRACK_ENDPOINT_CONNECTED,
    Terminal=NIMBY_TRACK_ENDPOINT_TERMINAL
};
enum class TrackEndpointReason : uint32_t {
    Complete=NIMBY_TRACK_ENDPOINT_COMPLETE,
    TrackUnavailable=NIMBY_TRACK_ENDPOINT_TRACK_UNAVAILABLE,
    DescriptorInvalid=NIMBY_TRACK_ENDPOINT_DESCRIPTOR_INVALID,
    ReadUnavailable=NIMBY_TRACK_ENDPOINT_READ_UNAVAILABLE,
    AttachmentInvalid=NIMBY_TRACK_ENDPOINT_ATTACHMENT_INVALID,
    ReferenceUnavailable=NIMBY_TRACK_ENDPOINT_REFERENCE_UNAVAILABLE,
    Unstable=NIMBY_TRACK_ENDPOINT_UNSTABLE,
    BudgetExceeded=NIMBY_TRACK_ENDPOINT_BUDGET_EXCEEDED
};
struct TrackEndpointState {
    TrackEndpointKind kind=TrackEndpointKind::Unknown;
    TrackEndpointReason reason=TrackEndpointReason::TrackUnavailable;
};
// All buffers/cache belong to this capture. False rejects the qualified pool
// or world; optional local faults retain explicit Unknown records instead.
bool read_track_endpoints(ReadMemory,void*,const LiveState&,bool recognized,
                          std::span<const uint64_t> trackIds,
                          std::vector<NimbyTrackEndpointStates>&) noexcept;
}
