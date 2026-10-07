// Compile the real host with a test-only uptime seam. A parent-side jump models
// the supervisor running first after resume, before its child gets CPU time.
// The watchdog's awake clock is deliberately the real Windows API.
#include <windows.h>
#include <atomic>
#include <cstdint>
namespace {
std::atomic<uint64_t> wallAdvanceMs{};
ULONGLONG WINAPI advancedTickCount64() {
    return GetTickCount64()+wallAdvanceMs.load(std::memory_order_relaxed);
}
}
void advanceHostTestWall(uint64_t milliseconds) noexcept {wallAdvanceMs=milliseconds;}
#define GetTickCount64 advancedTickCount64
#ifndef NIMBY_MOD_HOST_RUNTIME_SOURCE
#define NIMBY_MOD_HOST_RUNTIME_SOURCE "../../src/platform/windows/runtime/mod_host.cpp"
#endif
#include NIMBY_MOD_HOST_RUNTIME_SOURCE
#undef GetTickCount64
