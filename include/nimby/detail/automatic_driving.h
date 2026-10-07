#pragma once
#include <nimby/detail/observation.h>
#include <nimby/detail/sdk.h>
// Private transport, SI units. No native pointers cross this boundary.
typedef struct NimbySignalDrivingRule {
    uint64_t signal;
    double speed_mps;             // -1: no new target; >=0: numeric target speed.
    // Consumer-selected approach speed retained until head passage when the
    // target explicitly permits approach passage. No national default value.
    double reopened_speed_mps;
    uint32_t signals_ahead;       // 0: this signal; 1/2: subsequent native path signals.
    uint32_t flags;
} NimbySignalDrivingRule;
#define NIMBY_DRIVING_MAXIMUM_LINE_SPEED 1u
#define NIMBY_DRIVING_CLEAR 1u
#define NIMBY_DRIVING_HOLD_TO_CLEAR 2u
#define NIMBY_DRIVING_STOP 4u
// Resolve an announced target from that signal's visible numeric instruction.
// Outside the driver's visibility horizon, retain the announced stop target.
// A missing/unmanaged target still means stop, never a guessed permission.
#define NIMBY_DRIVING_FOLLOW_TARGET 8u
// Explicit permission from the mod, never inferred from STOP or a texture.
// Windows replaces exclusive occupation/reservation admission on the followed
// range with fresh physical clearance. Crossing-track occupation/reservations
// and native controller ownership can still refuse entry. No reservation is erased.
// Retain a restricted mode after passing this signal. reopened_speed_mps is
// then the maximum speed of that mode, not a demanded cruising speed.
#define NIMBY_DRIVING_ON_SIGHT 16u
// Requires a measured stop at this signal before requesting restricted entry.
#define NIMBY_DRIVING_STOP_THEN_PROCEED 32u
// Explicit mod policy for a multi-signal announcement: cancel its retained
// approach when the immediate next panel is visibly CLEAR. Does not release
// an independent stop approach, speed limit or restricted mode.
#define NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR 64u
// This panel permits passage at the speed retained by an earlier announcement.
// Unlike CLEAR, this does not release held limits or cancel other announcements.
// The mod decides when to publish it; neither colour nor signals_ahead implies it.
#define NIMBY_DRIVING_APPROACH_PASSABLE 128u
// Additional per-train constraints. No speed or national indication is implicit.
// mode: 0 numeric ceiling, 1 physical-clearance ceiling, 2 stop.
// flags bit 0: release after rear clearance; otherwise after head passage.
typedef struct NimbyTrainConstraint {
    uint64_t train,exit_signal,revision;
    double speed_mps;
    uint32_t mode,flags;
} NimbyTrainConstraint;
typedef struct NimbyTrainConstraintStatus {
    uint32_t size,state; // 0 absent, 1 waiting for exit binding, 2 active, 3 completed, 4 cancelled by route reset
    uint64_t train,exit_signal,revision;
    double speed_mps;
} NimbyTrainConstraintStatus;
#ifdef __cplusplus
extern "C" {
#endif
NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRules(const NimbySignalDrivingRule* rules,
    uint32_t count,uint32_t lease_ms) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV2(const NimbySignalDrivingRule* rules,
    uint32_t count,uint32_t lease_ms,uint32_t options) NIMBY_NOEXCEPT;
// Complete replacement scoped to publisher. Disjoint publishers coexist;
// conflicting IDs fail without changing either active batch.
NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV3(const NimbySignalDrivingRule* rules,
    uint32_t count,uint32_t lease_ms,uint32_t options,uint64_t publisher) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint* rules,
    uint32_t count,uint32_t lease_ms,uint64_t publisher) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ReadTrainConstraint(uint64_t train,NimbyTrainConstraintStatus* out) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
static_assert(sizeof(NimbyTrainConstraint)==40);
static_assert(sizeof(NimbyTrainConstraintStatus)==40);
#endif
