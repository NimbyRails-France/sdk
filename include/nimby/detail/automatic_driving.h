#pragma once
#include <nimby/detail/sdk.h>
// Private transport, SI units. No native pointers cross this boundary.
typedef struct NimbySignalDrivingRule {
    uint64_t signal;
    double speed_mps;             // -1: no new target; >=0: numeric target speed.
    double reopened_speed_mps;    // Approach retained after an announced stop reopens.
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
// Native reservations and crossing-track checks can still refuse entry.
// Retain a restricted mode after passing this signal. reopened_speed_mps is
// then the maximum speed of that mode, not a demanded cruising speed.
#define NIMBY_DRIVING_ON_SIGHT 16u
// Requires a measured stop at this signal before requesting restricted entry.
#define NIMBY_DRIVING_STOP_THEN_PROCEED 32u
// Explicit mod policy for a multi-signal announcement: cancel its retained
// approach when the immediate next panel is visibly CLEAR. Does not release
// an independent stop approach, speed limit or restricted mode.
#define NIMBY_DRIVING_CANCEL_AT_NEXT_CLEAR 64u
#ifdef __cplusplus
extern "C" {
#endif
NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRules(const NimbySignalDrivingRule* rules,
    uint32_t count,uint32_t lease_ms) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV2(const NimbySignalDrivingRule* rules,
    uint32_t count,uint32_t lease_ms,uint32_t options) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
