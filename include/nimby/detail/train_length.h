#pragma once
#include <nimby/detail/sdk.h>

#ifdef __cplusplus
extern "C" {
#endif
// Private additive ABI: a copied mod identifier and metre limit enter the
// isolated host mailbox. The parent creates the capability; a child may only
// update or remove its own capability. No native pointer crosses the channel.
NIMBY_API uint32_t __cdecl NimbyInternal_TrainLengthRegister(const char* mod_id,
    uint32_t maximum_meters,uint64_t* owner) NIMBY_NOEXCEPT;
// ABI 11 copies the mod's three static messages and translation catalogue once
// at registration. Updates retain this immutable declaration and its owner.
NIMBY_API uint32_t __cdecl NimbyInternal_TrainEditorRegister(const char* mod_id,
    uint32_t maximum_meters,const char* declaration,uint32_t bytes,
    uint64_t* owner) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_TrainLengthUpdate(uint64_t owner,
    uint32_t maximum_meters) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_TrainLengthRemove(uint64_t owner) NIMBY_NOEXCEPT;
#ifdef __cplusplus
}
#endif
