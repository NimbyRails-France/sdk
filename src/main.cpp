#include <windows.h>
#if !defined(_M_X64) && !defined(__x86_64__)
#error NimbyRailsSDK requires the AMD64 architecture
#endif

// No I/O, threads, locks, hooks or lifecycle work under the loader lock.
// Explicit SDK initialization and shutdown are implemented in runtime/runtime.cpp.
// Public read-only sessions/snapshots: runtime/observation.cpp; no work here.
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
