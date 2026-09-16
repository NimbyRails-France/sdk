#include "runtime/bootstrap.h"
#include <nimby/detail/sdk.h>

DWORD WINAPI NimbyInternal_Bootstrap(void* reserved) noexcept {
    if (reserved) return NIMBY_INVALID_ARGUMENT;
    return NimbyInternal_Initialize(NIMBY_ABI_VERSION, 0);
}
