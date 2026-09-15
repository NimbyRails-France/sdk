#pragma once
#include "sdk.h"

#ifdef __cplusplus
extern "C" {
#endif
// Read-only observation ABI v1. All IDs are opaque, never memory addresses.
#define NIMBY_OBSERVATION_ABI_VERSION 1u
#define NIMBY_UNSUPPORTED_GAME 7u
#define NIMBY_DATA_UNAVAILABLE 8u
#define NIMBY_INVALID_HANDLE 9u
#define NIMBY_BUFFER_TOO_SMALL 10u
#define NIMBY_PROCESS_EXITED 11u
#define NIMBY_RESOURCE_LIMIT 12u
#define NIMBY_TRAIN_PRESENT 1u
#define NIMBY_TRAIN_POSITION_VALID 2u
#define NIMBY_SNAPSHOT_EXPERIMENTAL 1u
#define NIMBY_SNAPSHOT_NON_ATOMIC 2u
#define NIMBY_SIGNAL_ONE_WAY 0
#define NIMBY_SIGNAL_PLATFORM_STOP 1
#define NIMBY_SIGNAL_BALISE 3
#define NIMBY_SIGNAL_PATH 4
#define NIMBY_SIGNAL_NO_WAY 5
#define NIMBY_SIGNAL_MARKER 6
// General display aspects, independent of the signal kind and national rules.
#define NIMBY_SIGNAL_ASPECT_UNKNOWN 0u
#define NIMBY_SIGNAL_ASPECT_STOP 1u
#define NIMBY_SIGNAL_ASPECT_CAUTION 2u
#define NIMBY_SIGNAL_ASPECT_PROCEED 3u
#define NIMBY_SIGNAL_ASPECT_DARK 4u
#define NIMBY_SIGNAL_ASPECT_VALID 1u
#define NIMBY_SIGNAL_SPECIFIC_STATE_VALID 2u
#define NIMBY_SIGNAL_TEXTURE_STATE_VALID 4u
#define NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID 1u
#define NIMBY_SIGNAL_TEXTURE_FILE_VALID 2u
#define NIMBY_SIGNAL_TEXTURE_DEFAULT_SET 4u
#define NIMBY_SIGNAL_TEXTURE_CLAMPED 8u
typedef uint64_t NimbySession;
typedef uint64_t NimbySnapshot;

#pragma pack(push, 8)
typedef struct NimbyTrain {
    uint64_t id, track_id;
    double speed_mps, track_fraction;
    int32_t direction;
    uint32_t flags;
    char name_utf8[257];
    uint8_t reserved[7];
} NimbyTrain;
typedef struct NimbyTrack {
    uint64_t id, station_id;
    double speed_limit_mps;
} NimbyTrack;
typedef struct NimbyStation {
    uint64_t id;
    char name_utf8[257]; // Empty when automatic name is not resolved.
    uint8_t reserved[7];
} NimbyStation;
typedef struct NimbySignal {
    uint64_t id, track_id;
    double track_fraction;
    int32_t direction, kind;
} NimbySignal;

// Separate record preserves the existing 32-byte NimbySignal ABI.
// Missing validity bit means UNKNOWN, including a zero texture_state.
// Specific IDs are namespaced, NUL-terminated UTF-8 (e.g. system="fr",
// state="carre"); they are identifiers, not a translated display label.
// No national aspect is inferred from a native kind or track usage.
typedef struct NimbySignalState {
    uint64_t signal_id;
    uint32_t flags, aspect;
    int32_t texture_state; // Native selector before renderer clamping/fallback.
    uint32_t reserved;
    char system_utf8[32];
    char specific_state_utf8[64];
} NimbySignalState;

// Texture selected from the game's loaded catalog, separate from the raw state.
// All strings are NUL-terminated UTF-8; unavailable fields are zero/empty.
// source: 0 built-in, 1 local mod, 2 Steam Workshop. FILE_VALID means a local
// file was resolved at capture time; consumers must still handle image errors.
// Does not guarantee the game's GPU upload succeeded or expose a railway aspect.
typedef struct NimbySignalTexture {
    uint64_t signal_id, textures_hash, file_hash;
    uint32_t flags;
    int32_t selected_index;
    uint32_t state_count, source;
    char textures_id_utf8[128];
    char mod_id_utf8[256];
    char relative_path_utf8[512];
    char file_path_utf8[1024];
} NimbySignalTexture;

// Experimental node graph. Existing primary-link targets are exposed, including
// non-reciprocal branch references. Edges do not establish a switch state.
// Zero link means unknown/absent, NOT a proven buffer stop. Junction graph incomplete.
// Coordinates are native projected units, not geographic latitude/longitude.
typedef struct NimbyTrackNode {
    uint64_t id, link_a, link_b;
    double x, y;
} NimbyTrackNode;

// Experimental native query interval on one track, normalized low <= high.
// Not an ordered route, switch position, signal permission or proof of a free block.
// Multiple entries may overlap (including multiple cars of the same train).
typedef struct NimbyTrackUsage {
    uint64_t train_id, track_id;
    double fraction_begin, fraction_end;
} NimbyTrackUsage;
typedef struct NimbySnapshotInfo {
    uint32_t struct_size, abi_version, process_id, flags;
    uint64_t captured_unix_ms;
    uint32_t train_count, track_count, station_count, signal_count;
    char game_sha256[65];
    uint8_t reserved[7];
} NimbySnapshotInfo;
#pragma pack(pop)

// Independent of diagnostics Initialize/Shutdown. pid=0 means the calling process.
// Outside DllMain only. Requires a recognized executable; never injects or writes.
NIMBY_API uint32_t __cdecl NimbySdk_OpenProcess(uint32_t abi_version, uint32_t pid, NimbySession* out) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_CloseSession(NimbySession session) NIMBY_NOEXCEPT;
// Immutable owned copy; not an atomic simulation tick. Zeroes out on failure.
NIMBY_API uint32_t __cdecl NimbySdk_CaptureSnapshot(NimbySession session, NimbySnapshot* out) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_ReleaseSnapshot(NimbySnapshot snapshot) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_GetSnapshotInfo(NimbySnapshot snapshot, NimbySnapshotInfo* out) NIMBY_NOEXCEPT;
// Query count with records=NULL, capacity=0. No partial copy when too small.
// required is mandatory; capacity and required are record counts, not byte sizes.
NIMBY_API uint32_t __cdecl NimbySdk_CopyTrains(NimbySnapshot snapshot, NimbyTrain* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_CopyTracks(NimbySnapshot snapshot, NimbyTrack* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_CopyStations(NimbySnapshot snapshot, NimbyStation* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_CopySignals(NimbySnapshot snapshot, NimbySignal* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
// One state per CopySignals record, joined by full signal_id, same snapshot.
// OK means the records were copied; inspect each record's validity flags.
// Native selector and atlas-scoped specific ID are observed independently of
// the general aspect, which remains unknown without a validated rule mapping.
// Does not grant movement permission and does not change the game.
NIMBY_API uint32_t __cdecl NimbySdk_CopySignalStates(NimbySnapshot snapshot, NimbySignalState* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
// One row per signal; uses the same buffer/count protocol. Missing catalog/state
// leaves flags=0. Read-only lookup, native default atlas and index clamping.
NIMBY_API uint32_t __cdecl NimbySdk_CopySignalTextures(NimbySnapshot snapshot, NimbySignalTexture* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_CopyTrackNodes(NimbySnapshot snapshot, NimbyTrackNode* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
// These components are captured independently. DATA_UNAVAILABLE means unknown,
// not zero reservations/occupations. NIMBY_OK with count=0 means observed empty.
// Read-only native reservations; does not invoke script-defined virtual reservations.
NIMBY_API uint32_t __cdecl NimbySdk_CopyTrackReservations(NimbySnapshot snapshot, NimbyTrackUsage* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbySdk_CopyTrackOccupations(NimbySnapshot snapshot, NimbyTrackUsage* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
// Stored Path membership in native vector order. NOT reserved/occupied tracks.
// Direction, branching semantics and remaining-vs-complete route are not established.
// DATA_UNAVAILABLE if train/path is absent or unstable; count is zero on failure.
NIMBY_API uint32_t __cdecl NimbySdk_CopyTrainPathTracks(NimbySnapshot snapshot, uint64_t train_id, uint64_t* records, uint32_t capacity, uint32_t* required) NIMBY_NOEXCEPT;
NIMBY_API const char* __cdecl NimbySdk_StatusString(uint32_t status) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
