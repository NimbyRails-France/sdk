#include <windows.h>
#include <nimby/sdk.h>
uint32_t __cdecl NimbySdk_GetVersion(NimbySdkVersion* out) noexcept {
    if(!out || out->struct_size != sizeof(NimbySdkVersion)) return NIMBY_INVALID_ARGUMENT;
    *out={sizeof(NimbySdkVersion),NIMBY_ABI_VERSION,0,6,0};
    return NIMBY_OK;
}
#if !defined(_M_X64) && !defined(__x86_64__)
#error NimbyRailsSDK requires the AMD64 architecture
#endif

// No I/O, threads, locks, hooks or lifecycle work under the loader lock.
// Explicit SDK initialization and shutdown are implemented in runtime/runtime.cpp.
// Public read-only sessions/snapshots: runtime/observation.cpp; no work here.
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
