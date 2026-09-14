#include "runtime/bootstrap.h"
#include <nimby/sdk.h>

DWORD WINAPI NimbySdk_Bootstrap(void* reserved) noexcept {
    if (reserved) return NIMBY_INVALID_ARGUMENT;
    return NimbySdk_Initialize(NIMBY_ABI_VERSION, 0);
}
