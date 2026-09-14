#include <nimby/sdk.h>
#include "engine/binary_identity.h"
#include "hooks/policy.h"
#include "hooks/backend.h"
#include <windows.h>
#include <array>

namespace {
SRWLOCK lock = SRWLOCK_INIT;
bool initialized = false;
NimbyBinaryInfo host{};
nimby::hooks::Backend backend;
struct Guard {
    Guard() noexcept { AcquireSRWLockExclusive(&lock); }
    ~Guard() { ReleaseSRWLockExclusive(&lock); }
};
void log(const char* message) noexcept {
    OutputDebugStringA("[NimbyRailsSDK] ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}
}
uint32_t __cdecl NimbySdk_InspectBinary(const wchar_t* path, NimbyBinaryInfo* out) noexcept {
    if (!out || out->struct_size != sizeof *out) return NIMBY_INVALID_ARGUMENT;
    return nimby::engine::identify(path, *out);
}
uint32_t __cdecl NimbySdk_Initialize(uint32_t abi_version, uint32_t flags) noexcept {
    if (abi_version != NIMBY_ABI_VERSION || (flags & ~NIMBY_REQUEST_HOOKS)) return NIMBY_INVALID_ARGUMENT;
    Guard guard;
    if (flags & NIMBY_REQUEST_HOOKS) { log("Hook activation refused: no validated targets."); return nimby::hooks::request_activation(); }
    if (initialized) return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t, 32768> path{};
    const auto length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (!length || length >= path.size()) return NIMBY_IO_ERROR;
    NimbyBinaryInfo candidate{};
    const auto status = nimby::engine::identify(path.data(), candidate);
    if (status != NIMBY_OK) { log("Host identification failed."); return status; }
    const auto hook_status = backend.initialize();
    if (hook_status != MH_OK) { log(MH_StatusToString(hook_status)); return NIMBY_INTERNAL_ERROR; }
    host = candidate;
    initialized = true;
    log(host.recognized_research_build ? "Known research build; diagnostics only." : "Unknown host; diagnostics only.");
    log(host.sha256);
    return NIMBY_OK;
}
uint32_t __cdecl NimbySdk_GetHostInfo(NimbyBinaryInfo* out) noexcept {
    if (!out || out->struct_size != sizeof *out) return NIMBY_INVALID_ARGUMENT;
    Guard guard;
    if (!initialized) { *out = {}; out->struct_size = sizeof *out; return NIMBY_INVALID_ARGUMENT; }
    *out = host;
    return NIMBY_OK;
}
uint32_t __cdecl NimbySdk_Shutdown() noexcept {
    Guard guard;
    const auto hook_status = backend.shutdown();
    if (hook_status != MH_OK) { log(MH_StatusToString(hook_status)); return NIMBY_INTERNAL_ERROR; }
    if (initialized) log("Diagnostics stopped; no hooks installed.");
    initialized = false;
    host = {};
    return NIMBY_OK;
}
