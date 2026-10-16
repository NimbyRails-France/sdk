#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/automatic_driving.h>
#include <nimby/detail/signal_routes.h>
#include <platform/windows/runtime/route_shadow_observer.h>
#include <windows.h>
#include <platform/windows/mod_host_releases.h>

namespace nimby::mod_host {
namespace {
unsigned releaseDriving(uint64_t owner,unsigned pending) noexcept {
    unsigned complete=0;
    if((pending&1)&&NimbyInternal_PublishDrivingRulesV3(nullptr,0,1000,0,owner)==NIMBY_OK)complete|=1;
    if((pending&2)&&NimbyInternal_PublishTrainConstraints(nullptr,0,1000,owner)==NIMBY_OK)complete|=2;
    return complete;
}
OwnerReleases& releases(){static auto* queue=new OwnerReleases(&releaseDriving,3);return *queue;}
bool track(Owners& owners){
    if(owners.drivingTracked)return true;
    return owners.drivingTracked=releases().track(owners.driving);
}
}
uint32_t dispatchDriving(const Request& request,Reply& reply,Owners& owners) {
    if(!owners.driving)return NIMBY_INVALID_ARGUMENT;
    if(request.operation==200) {
        if(request.args[0]>32768||request.args[1]<100||request.args[1]>5000||
           (request.args[2]&~uint64_t{NIMBY_DRIVING_MAXIMUM_LINE_SPEED})||
           request.data.size()!=request.args[0]*sizeof(NimbySignalDrivingRule))return NIMBY_INVALID_ARGUMENT;
        if(!track(owners))return NIMBY_RESOURCE_LIMIT;
        std::vector<NimbySignalDrivingRule> rows(static_cast<size_t>(request.args[0]));
        if(!rows.empty())std::memcpy(rows.data(),request.data.data(),request.data.size());
        // The trusted channel owns this identity; child tokens never cross over.
        return NimbyInternal_PublishDrivingRulesV3(rows.data(),static_cast<uint32_t>(rows.size()),
            static_cast<uint32_t>(request.args[1]),static_cast<uint32_t>(request.args[2]),owners.driving);
    }
    if(request.operation==201) {
        if(request.args[0]>8192||request.args[1]<100||request.args[1]>5000||
           request.data.size()!=request.args[0]*sizeof(NimbyTrainConstraint))return NIMBY_INVALID_ARGUMENT;
        if(!track(owners))return NIMBY_RESOURCE_LIMIT;
        std::vector<NimbyTrainConstraint> rows(static_cast<size_t>(request.args[0]));
        if(!rows.empty())std::memcpy(rows.data(),request.data.data(),request.data.size());
        return NimbyInternal_PublishTrainConstraints(rows.data(),static_cast<uint32_t>(rows.size()),
            static_cast<uint32_t>(request.args[1]),owners.driving);
    }
    if(request.operation==202) {
        if(!request.data.empty()||request.args[0]>>48!=5)return NIMBY_INVALID_ARGUMENT;
        NimbyTrainConstraintStatus status{};status.size=sizeof status;
        const auto result=NimbyInternal_ReadTrainConstraint(request.args[0],&status);
        if(result==NIMBY_OK)output(reply,&status);
        return result;
    }
    if(request.operation==203||request.operation==204) {
        const bool selectedRoute=request.operation==204;
        reply.data.clear();
        if(!request.args[0]||request.args[0]>4096||!request.args[1]||request.args[1]>16||request.args[2]>65536||request.args[3]>131072||
           (selectedRoute&&request.args[4]>131072)||request.data.size()!=request.args[0]*sizeof(uint64_t))return NIMBY_INVALID_ARGUMENT;
        const uint64_t replyBytes=request.args[2]*sizeof(NimbySignalRouteObservation)+request.args[3]*sizeof(NimbySignalRouteSection)+
            (selectedRoute?request.args[4]*sizeof(NimbySignalRouteAnnotation):0);
        if(replyBytes>payloadLimit)return NIMBY_RESOURCE_LIMIT;
        std::vector<uint64_t> signals(static_cast<size_t>(request.args[0]));
        std::memcpy(signals.data(),request.data.data(),request.data.size());
        if(selectedRoute)for(size_t i=0;i<signals.size();++i)if(signals[i]>>48!=8||!(signals[i]&0x0000ffffffffffffULL)||
            (i&&signals[i-1]>=signals[i]))return NIMBY_INVALID_ARGUMENT;
        std::vector<NimbySignalRouteObservation> rows(static_cast<size_t>(request.args[2]));uint32_t count=0;
        std::vector<NimbySignalRouteSection> sections(static_cast<size_t>(request.args[3]));uint32_t sectionCount=0;
        std::vector<NimbySignalRouteAnnotation> annotations(selectedRoute?static_cast<size_t>(request.args[4]):0);uint32_t annotationCount=0;
        const auto result=selectedRoute?NimbyInternal_CopySignalRoutesV2(signals.data(),static_cast<uint32_t>(signals.size()),
            static_cast<uint32_t>(request.args[1]),rows.data(),static_cast<uint32_t>(rows.size()),&count,
            sections.data(),static_cast<uint32_t>(sections.size()),&sectionCount,annotations.data(),static_cast<uint32_t>(annotations.size()),&annotationCount):
            NimbyInternal_CopySignalRoutes(signals.data(),static_cast<uint32_t>(signals.size()),
            static_cast<uint32_t>(request.args[1]),rows.data(),static_cast<uint32_t>(rows.size()),&count,
            sections.data(),static_cast<uint32_t>(sections.size()),&sectionCount);
        if(result==NIMBY_OK){
            if(count>rows.size()||sectionCount>sections.size()||annotationCount>annotations.size())return NIMBY_INVALID_BINARY;
            // Opt-in observation only, after the bridge released its native
            // train locks/publication pins. A failed comparison never changes
            // the copied route result, native admission or native reservations.
            // The old diagnostic expects normalized V1 ranges. V2 preserves
            // directed endpoints and never replays a second V1 capture for it.
            if(!selectedRoute)nimby::windows::automatic::observeShadowRoutes(
                std::span(rows.data(),count),std::span(sections.data(),sectionCount));
            reply.args[0]=count;reply.args[1]=sectionCount;
            if(selectedRoute)reply.args[2]=annotationCount;
            reply.data.resize(count*sizeof(NimbySignalRouteObservation)+sectionCount*sizeof(NimbySignalRouteSection)+annotationCount*sizeof(NimbySignalRouteAnnotation));
            if(count)std::memcpy(reply.data.data(),rows.data(),count*sizeof(NimbySignalRouteObservation));
            if(sectionCount)std::memcpy(reply.data.data()+count*sizeof(NimbySignalRouteObservation),sections.data(),sectionCount*sizeof(NimbySignalRouteSection));
            if(annotationCount)std::memcpy(reply.data.data()+count*sizeof(NimbySignalRouteObservation)+sectionCount*sizeof(NimbySignalRouteSection),annotations.data(),annotationCount*sizeof(NimbySignalRouteAnnotation));
        }
        return result;
    }
    return NIMBY_INVALID_ARGUMENT;
}
void cleanupDriving(Owners& owners) noexcept {
    if(!owners.drivingTracked)return;
    releases().retire(owners.driving);
    owners.drivingTracked=false;owners.driving=0;
}
}
