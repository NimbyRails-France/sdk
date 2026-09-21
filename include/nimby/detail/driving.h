#pragma once
#include "observation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define NIMBY_DRIVING_PURCHASED_VALID 1u
#define NIMBY_DRIVING_CURRENT_VALID 2u
#define NIMBY_DRIVING_MOTION_VALID 4u
// Additive ABI: existing snapshot structures are unchanged. Since SDK 0.7.3.
#pragma pack(push, 8)
typedef struct NimbyTrainDynamics {
    double max_speed_mps, max_acceleration_mps2;
    double service_braking_mps2, emergency_braking_mps2;
    double tractive_effort_n, power_w, empty_mass_kg, length_m;
} NimbyTrainDynamics;
typedef struct NimbyDrivingObservation {
    uint32_t struct_size, flags;
    uint64_t train_id, session_generation, captured_unix_ms;
    int64_t elapsed_begin_ms, elapsed_end_ms;
    NimbyTrainDynamics purchased, current;
    NimbyTrain train;
} NimbyDrivingObservation;
#pragma pack(pop)
// Targeted optimistic read, never an atomic tick. On failure clears output except size.
NIMBY_API uint32_t __cdecl NimbyInternal_ReadTrainDriving(NimbySession session,
    uint64_t train_id, NimbyDrivingObservation* out) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
