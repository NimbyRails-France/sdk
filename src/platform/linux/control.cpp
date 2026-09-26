#include <nimby/detail/control.h>
#include <nimby/detail/automatic_driving.h>
// Explicit capability boundary; Linux native hooks have not been qualified.
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ModControl(uint32_t,const char*,const NimbyControlRequest*,NimbyControlResponse*) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint*,uint32_t,uint32_t,uint64_t) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ReadTrainConstraint(uint64_t,NimbyTrainConstraintStatus*) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
