#pragma once
#include <nimby/detail/observation.h>

// Independent optional transport: no existing observation record grows.
// Terminal means a qualified directed end of native track connectivity. It
// does not identify a graphical buffer stop or grant movement permission.
#define NIMBY_TRACK_ENDPOINT_UNKNOWN 0u
#define NIMBY_TRACK_ENDPOINT_CONNECTED 1u
#define NIMBY_TRACK_ENDPOINT_TERMINAL 2u
#define NIMBY_TRACK_ENDPOINTS_MAX_TRACKS 4096u
#define NIMBY_TRACK_ENDPOINTS_MAX_REFERENCES 131072u
#define NIMBY_TRACK_ENDPOINT_COMPLETE 0u
#define NIMBY_TRACK_ENDPOINT_TRACK_UNAVAILABLE 1u
#define NIMBY_TRACK_ENDPOINT_DESCRIPTOR_INVALID 2u
#define NIMBY_TRACK_ENDPOINT_READ_UNAVAILABLE 3u
#define NIMBY_TRACK_ENDPOINT_ATTACHMENT_INVALID 4u
#define NIMBY_TRACK_ENDPOINT_REFERENCE_UNAVAILABLE 5u
#define NIMBY_TRACK_ENDPOINT_UNSTABLE 6u
#define NIMBY_TRACK_ENDPOINT_BUDGET_EXCEEDED 7u

#pragma pack(push, 8)
typedef struct NimbyTrackEndpointStates {
    uint64_t track_id, station_id;
    // A is fraction=0 approached in direction=-1; B is fraction=1, +1.
    // Connected proves at least one native continuation, not every route.
    uint32_t state_a, state_b, reason_a, reason_b;
} NimbyTrackEndpointStates;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif
// Explicit bounded read of requested full IDs only, without native calls.
// Unknown is retained per row/end; an unsupported pool/profile has no snapshot.
// Station ID is an observed association, not a platform-end classification.
NIMBY_API uint32_t __cdecl NimbyInternal_CaptureTrackEndpointsSnapshot(
    NimbySession session, const uint64_t* track_ids, uint32_t count,
    NimbySnapshot* out, uint32_t* stage) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_CopyTrackEndpointStates(
    NimbySnapshot snapshot, NimbyTrackEndpointStates* records,
    uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
