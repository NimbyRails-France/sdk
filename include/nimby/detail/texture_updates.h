#pragma once
#include <nimby/detail/observation.h>
// Bounded strings and values only; no child-process pointers cross the broker.
typedef struct NimbyTextureUpdate {
    uint64_t signal;
    uint32_t duration_ms,half_period_ms,index,flags; // flags bit 0: numeric index
    char texture_set[257],first_path[512],alternate_path[512];
} NimbyTextureUpdate;
#ifdef __cplusplus
extern "C" {
#endif
NIMBY_API uint32_t __cdecl NimbyInternal_PublishTextureUpdates(uint32_t pid,uint64_t owner,
    const NimbyTextureUpdate* updates,uint32_t count) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ClearOwnedTextures(uint32_t pid,uint64_t owner,
    const uint64_t* signals,uint32_t count) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ReleaseTextureOwner(uint32_t pid,uint64_t owner) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
