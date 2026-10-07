// Test-only adapter. A Linux game texture bridge is not implemented by these doubles.
#include <nimby/signal_textures.hpp>
extern "C" const char* NimbyInternal_StatusString(uint32_t) noexcept { return "Test adapter: unavailable"; }
extern "C" uint32_t NimbyInternal_ShowSignalTexture(uint32_t,uint64_t,const char*,const char*) noexcept { return NIMBY_DATA_UNAVAILABLE; }
extern "C" uint32_t NimbyInternal_RestoreSignalTexture(uint32_t,uint64_t) noexcept { return NIMBY_DATA_UNAVAILABLE; }
extern "C" uint32_t NimbyInternal_PublishTextureUpdates(uint32_t,uint64_t,const NimbyTextureUpdate*,uint32_t) noexcept { return NIMBY_DATA_UNAVAILABLE; }
extern "C" uint32_t NimbyInternal_ClearOwnedTextures(uint32_t,uint64_t,const uint64_t*,uint32_t) noexcept { return NIMBY_DATA_UNAVAILABLE; }
extern "C" uint32_t NimbyInternal_ReleaseTextureOwner(uint32_t,uint64_t) noexcept { return NIMBY_DATA_UNAVAILABLE; }
