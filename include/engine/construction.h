#pragma once
#include <nimby/detail/construction.h>
#include <cmath>

namespace nimby::engine::construction {
// Pure validation shared by the transport and resident endpoint. No railway
// policy, route choice, spacing, model defaults or platform addresses live here.
inline bool valid(const NimbyConstructionRequest& request) noexcept {
    if(request.size!=sizeof(request)||request.version!=NIMBY_CONSTRUCTION_VERSION)return false;
    if(request.action==NIMBY_CONSTRUCTION_PREPARE)
        return !request.token&&!request.count&&(request.source_signal>>48)==8;
    if(request.action==NIMBY_CONSTRUCTION_UNDO)
        return request.token&&!request.count&&!request.source_signal;
    if(request.action!=NIMBY_CONSTRUCTION_CREATE||!request.token||
       (request.source_signal>>48)!=8||!request.count||request.count>NIMBY_CONSTRUCTION_CAPACITY)return false;
    for(uint32_t i=0;i<request.count;++i){
        const auto& p=request.positions[i];
        if((p.track_id>>48)!=1||!std::isfinite(p.fraction)||p.fraction<=0||p.fraction>=1||
           (p.direction!=1&&p.direction!=-1)||p.reserved)return false;
        for(uint32_t j=0;j<i;++j)if(p.track_id==request.positions[j].track_id&&
            p.fraction==request.positions[j].fraction)return false;
    }
    return true;
}
}
