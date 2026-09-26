#include <nimby/detail/diagnostics.hpp>
#include "platform/runtime.h"
#include "platform/windows/hooks/backend.h"
#include "engine/binary_identity.h"
#include <array>

namespace nimby::platform {
namespace { hooks::Backend backend; }
void diagnosticLog(const char* message) noexcept {
    nimby::detail::diagnostics::write("sdk", "INFO", message);
}

uint32_t identifyHost(NimbyBinaryInfo& output) noexcept {
    std::array<wchar_t,32768> path{};
    const auto length=GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()));
    if(!length || length>=path.size()) return NIMBY_IO_ERROR;
    return engine::identify(path.data(),output);
}
uint32_t initializeHookBackend() noexcept {
    const auto status=backend.initialize();
    if(status==MH_OK) return NIMBY_OK;
    diagnosticLog(MH_StatusToString(status));
    return NIMBY_INTERNAL_ERROR;
}
uint32_t shutdownHookBackend() noexcept {
    const auto status=backend.shutdown();
    if(status==MH_OK) return NIMBY_OK;
    diagnosticLog(MH_StatusToString(status));
    return NIMBY_INTERNAL_ERROR;
}
}
