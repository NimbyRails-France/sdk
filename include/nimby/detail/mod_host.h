#pragma once
#include <nimby/detail/sdk.h>
// Internal diagnostic markers. They carry no gameplay data and never renew
// the watchdog's outer callback deadline. Older hosts may omit this export.
enum NimbyModWorkStage {
    NIMBY_MOD_WORK_UNSPECIFIED=0,
    NIMBY_MOD_WORK_CONNECT_SETTINGS=1,
    NIMBY_MOD_WORK_CONNECT_SERVICES=2,
    NIMBY_MOD_WORK_OPEN_OBSERVATION=3,
    NIMBY_MOD_WORK_CAPTURE_SIGNALLING=4,
    NIMBY_MOD_WORK_CAPTURE_SESSION=5,
    NIMBY_MOD_WORK_CAPTURE_COMPLETE=6,
    NIMBY_MOD_WORK_SYNCHRONIZE_SETTINGS=7,
    NIMBY_MOD_WORK_OBSERVE_SERVICES=8,
    NIMBY_MOD_WORK_MOD_OBSERVE=9,
    NIMBY_MOD_WORK_POLL_ACTIONS=10,
    NIMBY_MOD_WORK_MOD_ACTION=11,
    NIMBY_MOD_WORK_OBSERVATION_LOST=12,
    NIMBY_MOD_WORK_SUSPEND_SETTINGS=13,
    NIMBY_MOD_WORK_SUSPEND_SERVICES=14,
    NIMBY_MOD_WORK_REFRESH_OPTIONS=15,
    NIMBY_MOD_WORK_OBSERVE_SIGNALS=16,
    NIMBY_MOD_WORK_SIGNAL_CONTROL=17,
    NIMBY_MOD_WORK_SIGNAL_SETTINGS=18,
    NIMBY_MOD_WORK_PREPARE_NETWORK=19,
    NIMBY_MOD_WORK_EVALUATE_NETWORK=20,
    NIMBY_MOD_WORK_PREPARE_DRIVING=21,
    NIMBY_MOD_WORK_PUBLISH_DRIVING=22,
    NIMBY_MOD_WORK_PUBLISH_TRAINS=23,
    NIMBY_MOD_WORK_SIGNAL_DIAGNOSTICS=24,
    NIMBY_MOD_WORK_TEXTURE_CLEANUP=25,
    NIMBY_MOD_WORK_PREPARE_TEXTURES=26,
    NIMBY_MOD_WORK_PUBLISH_TEXTURES=27,
    NIMBY_MOD_WORK_TEXTURE_COMMIT=28
};
#ifdef __cplusplus
extern "C" {
#endif
// Internal process-host ABI. Zero means the caller is not an isolated mod host.
NIMBY_API uint32_t __cdecl NimbyInternal_ModHostTarget(void) NIMBY_NOEXCEPT;
// phase: 1 starting, 2 running callback, 3 idle, 4 stopping. The watchdog
// observes work completion, not an unrelated timer thread's liveness.
NIMBY_API void __cdecl NimbyInternal_ModHostPulse(uint32_t phase) NIMBY_NOEXCEPT;
NIMBY_API void __cdecl NimbyInternal_ModHostStage(uint32_t stage, uint64_t detail) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation, uint64_t* arguments,
    const void* input, uint32_t input_size, void* output, uint32_t capacity,
    uint32_t* written) NIMBY_NOEXCEPT;
NIMBY_API void* __cdecl NimbyInternal_ModHostUiSymbol(const char* name) NIMBY_NOEXCEPT;
// Child-local wait handles, duplicated with SYNCHRONIZE only. The caller owns
// both returned handles and closes them only after its waiter has stopped.
NIMBY_API uint32_t __cdecl NimbyInternal_ModHostActionWaits(uint64_t* remote,
    uint64_t* local) NIMBY_NOEXCEPT;
// Windows tool forms enqueue locally; this signals only their own child event.
NIMBY_API void __cdecl NimbyInternal_ModHostWakeLocal(void) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
