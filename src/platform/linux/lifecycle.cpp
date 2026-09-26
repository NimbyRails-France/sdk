#include "platform/runtime.h"
#include "engine/binary_identity.h"
#include <cstdio>

namespace nimby::platform {
void diagnosticLog(const char* message) noexcept {
    std::fprintf(stderr,"[NimbyRailsFranceSDK] %s\n",message);
}
uint32_t identifyHost(NimbyBinaryInfo& output) noexcept {
    return engine::identify(L"/proc/self/exe",output);
}
// Linux currently provides observation/diagnostics. No hook backend is installed.
// The common activation policy still rejects requests for game hooks.
uint32_t initializeHookBackend() noexcept { return NIMBY_OK; }
uint32_t shutdownHookBackend() noexcept { return NIMBY_OK; }
}
