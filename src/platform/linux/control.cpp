#include <nimby/detail/control.h>
#include <nimby/detail/automatic_driving.h>
#include <nimby/detail/signal_routes.h>
// Explicit capability boundary; Linux native hooks have not been qualified.
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRules(const NimbySignalDrivingRule*,uint32_t,uint32_t) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV2(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishDrivingRulesV3(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t,uint64_t) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ModControl(uint32_t,const char*,const NimbyControlRequest*,NimbyControlResponse*) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint*,uint32_t,uint32_t,uint64_t) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ReadTrainConstraint(uint64_t,NimbyTrainConstraintStatus*) noexcept {
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_CopySignalRoutes(const uint64_t*,uint32_t,uint32_t,
    NimbySignalRouteObservation*,uint32_t,uint32_t* count,NimbySignalRouteSection*,uint32_t,uint32_t* sectionCount) noexcept {
    if(count)*count=0;
    if(sectionCount)*sectionCount=0;
    return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_CopySignalRoutesV2(const uint64_t*,uint32_t,uint32_t,
    NimbySignalRouteObservation*,uint32_t,uint32_t* count,NimbySignalRouteSection*,uint32_t,uint32_t* sectionCount,
    NimbySignalRouteAnnotation*,uint32_t,uint32_t* annotationCount) noexcept {
    if(count)*count=0;
    if(sectionCount)*sectionCount=0;
    if(annotationCount)*annotationCount=0;
    return NIMBY_HOOKS_UNAVAILABLE;
}
