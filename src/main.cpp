#include <nimby/detail/sdk.h>
uint32_t __cdecl NimbyInternal_GetVersion(NimbySdkVersion* out) noexcept {
    if(!out || out->struct_size != sizeof(NimbySdkVersion)) return NIMBY_INVALID_ARGUMENT;
    *out={sizeof(NimbySdkVersion),NIMBY_ABI_VERSION,0,8,0};
    return NIMBY_OK;
}
#if !defined(_M_X64) && !defined(__x86_64__)
#error NimbyRailsFranceSDK requires the AMD64 architecture
#endif
