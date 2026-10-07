#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/automatic_driving.h>
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
    return NIMBY_INVALID_ARGUMENT;
}
void cleanupDriving(Owners& owners) noexcept {
    if(!owners.drivingTracked)return;
    releases().retire(owners.driving);
    owners.drivingTracked=false;owners.driving=0;
}
}
