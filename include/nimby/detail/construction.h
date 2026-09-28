#pragma once
#include "sdk.h"

// Private transport for the Kotlin construction client. These are values, never
// native pointers. Loading/capturing observations does not enable construction.
#define NIMBY_CONSTRUCTION_VERSION 1u
#define NIMBY_CONSTRUCTION_CAPACITY 64u
enum NimbyConstructionAction { NIMBY_CONSTRUCTION_PREPARE=1, NIMBY_CONSTRUCTION_CREATE=2, NIMBY_CONSTRUCTION_UNDO=3 };
enum NimbyConstructionState {
    NIMBY_CONSTRUCTION_READY=1, NIMBY_CONSTRUCTION_APPLIED=2,
    NIMBY_CONSTRUCTION_UNDONE=3, NIMBY_CONSTRUCTION_REJECTED=4,
    NIMBY_CONSTRUCTION_PARTIAL=5, NIMBY_CONSTRUCTION_PENDING=6
};
typedef struct NimbyConstructionPosition {
    uint64_t track_id;
    double fraction;
    int32_t direction; // Native stored direction, supplied by the caller.
    uint32_t reserved;
} NimbyConstructionPosition;
typedef struct NimbyConstructionRequest {
    uint32_t size, version, action, count;
    uint64_t token, source_signal;
    NimbyConstructionPosition positions[NIMBY_CONSTRUCTION_CAPACITY];
} NimbyConstructionRequest;
typedef struct NimbyConstructionResult {
    uint32_t size, version, state, count;
    uint64_t token;
    uint32_t reason, can_undo;
    uint64_t ids[NIMBY_CONSTRUCTION_CAPACITY];
} NimbyConstructionResult;

#ifdef __cplusplus
extern "C" {
#endif
// PREPARE must precede the observation used to calculate positions. Any native
// command or session change invalidates that ticket. Results may be PENDING:
// poll explicitly; never retry CREATE with a new ticket after an uncertain wait.
NIMBY_API uint32_t __cdecl NimbyInternal_Construction(uint64_t session, const NimbyConstructionRequest*, NimbyConstructionResult*) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ConstructionPoll(uint64_t session, uint64_t token, NimbyConstructionResult*) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
