#include "platform/windows/hooks/backend.h"
#include "platform/windows/bridge_installation.h"

namespace nimby::hooks {
MH_STATUS Backend::initialize() noexcept {
    if (owned_) return MH_ERROR_ALREADY_INITIALIZED;
    const auto status = MH_Initialize();
    // Never claim another owner's already initialized MinHook instance.
    if (status == MH_OK) owned_ = true;
    return status;
}
MH_STATUS Backend::shutdown() noexcept {
    if (!owned_) return MH_OK;
    nimby::platform::windows::BridgeInstallation installation;
    if (!installation) return MH_UNKNOWN;
    // Currently no production hooks. Once added, callers must quiesce and drain
    // all detours before this call; MinHook uninitialization alone is not a drain.
    const auto status = MH_Uninitialize();
    if (status == MH_OK) owned_ = false;
    return status;
}
}
