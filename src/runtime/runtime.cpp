#include <nimby/detail/sdk.h>
#include "engine/binary_identity.h"
#include "hooks/policy.h"
#ifdef _WIN32
#include "hooks/backend.h"
#include <windows.h>
#endif
#include <algorithm>
#include <mutex>
#include <cstdio>
#include <array>

namespace {
std::mutex lock;
bool initialized = false;
NimbyBinaryInfo host{};
#ifdef _WIN32
nimby::hooks::Backend backend;
#endif
struct Guard {
    Guard() { lock.lock(); }
    ~Guard() { lock.unlock(); }
};
void log(const char* message) noexcept {
#ifdef _WIN32
    OutputDebugStringA("[NimbyRailsFranceSDK] ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
#else
    std::fprintf(stderr,"[NimbyRailsFranceSDK] %s\n",message);
#endif
}
}
uint32_t __cdecl NimbyInternal_InspectBinary(const wchar_t* path, NimbyBinaryInfo* out) noexcept {
    if (!out || out->struct_size != sizeof *out) return NIMBY_INVALID_ARGUMENT;
    return nimby::engine::identify(path, *out);
}
uint32_t __cdecl NimbyInternal_Initialize(uint32_t abi_version, uint32_t flags) noexcept {
    if (abi_version != NIMBY_ABI_VERSION || (flags & ~NIMBY_REQUEST_HOOKS)) return NIMBY_INVALID_ARGUMENT;
    Guard guard;
    if (flags & NIMBY_REQUEST_HOOKS) { log("Hook activation refused: no validated targets."); return nimby::hooks::request_activation(); }
    if (initialized) return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t, 32768> path{};
#ifdef _WIN32
    const auto length = GetModuleFileNameW(nullptr, path.data(), static_cast<DWORD>(path.size()));
    if (!length || length >= path.size()) return NIMBY_IO_ERROR;
#else
    constexpr wchar_t self[]=L"/proc/self/exe";
    std::copy(std::begin(self),std::end(self),path.begin());
#endif
    NimbyBinaryInfo candidate{};
    const auto status = nimby::engine::identify(path.data(), candidate);
    if (status != NIMBY_OK) { log("Host identification failed."); return status; }
#ifdef _WIN32
    const auto hook_status = backend.initialize();
    if (hook_status != MH_OK) { log(MH_StatusToString(hook_status)); return NIMBY_INTERNAL_ERROR; }
#endif
    host = candidate;
    initialized = true;
    log(host.recognized_research_build ? "Known research build; diagnostics only." : "Unknown host; diagnostics only.");
    log(host.sha256);
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_GetHostInfo(NimbyBinaryInfo* out) noexcept {
    if (!out || out->struct_size != sizeof *out) return NIMBY_INVALID_ARGUMENT;
    Guard guard;
    if (!initialized) { *out = {}; out->struct_size = sizeof *out; return NIMBY_INVALID_ARGUMENT; }
    *out = host;
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_Shutdown() noexcept {
    Guard guard;
#ifdef _WIN32
    const auto hook_status = backend.shutdown();
    if (hook_status != MH_OK) { log(MH_StatusToString(hook_status)); return NIMBY_INTERNAL_ERROR; }
#endif
    if (initialized) log("Diagnostics stopped; no hooks installed.");
    initialized = false;
    host = {};
    return NIMBY_OK;
}
