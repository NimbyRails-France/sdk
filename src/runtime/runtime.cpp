#include <nimby/detail/sdk.h>
#include "engine/binary_identity.h"
#include "hooks/policy.h"
#include "platform/runtime.h"
#include <algorithm>
#include <mutex>
#include <cstdio>
#include <array>

namespace {
std::mutex lock;
bool initialized = false;
NimbyBinaryInfo host{};
using Guard=std::lock_guard<std::mutex>;
void log(const char* message) noexcept { nimby::platform::diagnosticLog(message); }
}
uint32_t __cdecl NimbyInternal_InspectBinary(const wchar_t* path, NimbyBinaryInfo* out) noexcept {
    if (!out || out->struct_size != sizeof *out) return NIMBY_INVALID_ARGUMENT;
    return nimby::engine::identify(path, *out);
}
uint32_t __cdecl NimbyInternal_Initialize(uint32_t abi_version, uint32_t flags) noexcept {
    if (abi_version != NIMBY_ABI_VERSION || (flags & ~NIMBY_REQUEST_HOOKS)) return NIMBY_INVALID_ARGUMENT;
    Guard guard(lock);
    if (flags & NIMBY_REQUEST_HOOKS) { log("Hook activation refused: no validated targets."); return nimby::hooks::request_activation(); }
    if (initialized) return NIMBY_ALREADY_INITIALIZED;
    NimbyBinaryInfo candidate{};
    const auto status = nimby::platform::identifyHost(candidate);
    if (status != NIMBY_OK) { log("Host identification failed."); return status; }
    const auto hook_status = nimby::platform::initializeHookBackend();
    if (hook_status != NIMBY_OK) return hook_status;
    host = candidate;
    initialized = true;
    log(host.recognized_research_build ? "Known research build; diagnostics only." : "Unknown host; diagnostics only.");
    log(host.sha256);
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_GetHostInfo(NimbyBinaryInfo* out) noexcept {
    if (!out || out->struct_size != sizeof *out) return NIMBY_INVALID_ARGUMENT;
    Guard guard(lock);
    if (!initialized) { *out = {}; out->struct_size = sizeof *out; return NIMBY_INVALID_ARGUMENT; }
    *out = host;
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_Shutdown() noexcept {
    Guard guard(lock);
    const auto hook_status = nimby::platform::shutdownHookBackend();
    if (hook_status != NIMBY_OK) return hook_status;
    if (initialized) log("Diagnostics stopped; no hooks installed.");
    initialized = false;
    host = {};
    return NIMBY_OK;
}
