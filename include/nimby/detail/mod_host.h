#pragma once
#include <nimby/detail/sdk.h>
#ifdef __cplusplus
extern "C" {
#endif
// Internal process-host ABI. Zero means the caller is not an isolated mod host.
NIMBY_API uint32_t __cdecl NimbyInternal_ModHostTarget(void) NIMBY_NOEXCEPT;
// phase: 1 starting, 2 running callback, 3 idle, 4 stopping. The watchdog
// observes work completion, not an unrelated timer thread's liveness.
NIMBY_API void __cdecl NimbyInternal_ModHostPulse(uint32_t phase) NIMBY_NOEXCEPT;
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
