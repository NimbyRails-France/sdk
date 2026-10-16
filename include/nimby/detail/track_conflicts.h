#pragma once
#include <nimby/detail/observation.h>

// Private read-only transport. These rows are geometry conflicts, not an
// itinerary, a reservation or permission to drive. Only the qualified native
// admission list is observed; other neighbouring-track lists are not merged.
#define NIMBY_TRACK_CONFLICTS_VALID 1u
#define NIMBY_TRACK_CONFLICTS_MAX_TRACKS 4096u
#define NIMBY_TRACK_CONFLICTS_MAX_ROWS 131072u
#define NIMBY_TRACK_CONFLICTS_COMPLETE 0u
#define NIMBY_TRACK_CONFLICTS_TRACK_UNAVAILABLE 1u
#define NIMBY_TRACK_CONFLICTS_DESCRIPTOR_INVALID 2u
#define NIMBY_TRACK_CONFLICTS_READ_UNAVAILABLE 3u
#define NIMBY_TRACK_CONFLICTS_INTERVAL_INVALID 4u
#define NIMBY_TRACK_CONFLICTS_REFERENCE_UNAVAILABLE 5u
#define NIMBY_TRACK_CONFLICTS_UNSTABLE 6u
#define NIMBY_TRACK_CONFLICTS_BUDGET_EXCEEDED 7u

#pragma pack(push, 8)
typedef struct NimbyTrackConflictState {
    uint64_t track_id;
    uint32_t flags, reason;
} NimbyTrackConflictState;
typedef struct NimbyTrackConflict {
    uint64_t track_id, other_track_id;
    double begin_fraction, end_fraction, other_begin_fraction, other_end_fraction;
} NimbyTrackConflict;
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif
// Captures only the requested full Track IDs in one immutable snapshot.
// Repeated IDs are coalesced; states and rows retain first-request order.
// Missing/unreadable individual lists have state VALID=0; a verified empty
// list has VALID=1 and no rows. A failed global pool/session yields no snapshot.
NIMBY_API uint32_t __cdecl NimbyInternal_CaptureTrackConflictsSnapshot(
    NimbySession session, const uint64_t* track_ids, uint32_t count,
    NimbySnapshot* out, uint32_t* stage) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_CopyTrackConflictStates(
    NimbySnapshot snapshot, NimbyTrackConflictState* records, uint32_t capacity,
    uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_CopyTrackConflicts(
    NimbySnapshot snapshot, NimbyTrackConflict* records, uint32_t capacity,
    uint32_t* required) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
