#pragma once
#include "runtime/texture_bridge.h"
#include <nimby/detail/observation.h>
namespace nimby::texture_bridge {
// Called with exclusive ownership of the table. No game memory is modified.
inline uint32_t execute(Shared& shared,Table& commands,uint64_t now) noexcept {
    uint32_t result=NIMBY_OK;
        try {
            const bool same=shared.expected_database==shared.request_database &&
                            shared.expected_simulation==shared.request_simulation;
            const auto id=shared.request_signal;
            if(shared.operation==1){
                const nimby::texture_bridge::Command command{id,shared.request_hash,shared.request_expiry,shared.request_index};
                if(same){commands.prune(now);commands.put(command);}
                else {nimby::texture_bridge::Table fresh;fresh.put(command);commands.entries.swap(fresh.entries);}
                shared.expected_database=shared.request_database;
                shared.expected_simulation=shared.request_simulation;
                shared.signal=id;shared.expires=command.expires;
            }else if(shared.operation==2){
                if(!same||!commands.erase(id))result=NIMBY_INVALID_ARGUMENT;
                else {shared.signal=id;shared.expires=0;}
            }else if(shared.operation==3){
                shared.active=0;shared.active_count=0;shared.result_index=0;shared.result_expiry=0;
                if(same){
                    for(const auto& command:commands.entries){
                        if(command.expires<=now)continue;
                        ++shared.active_count;
                        if(command.signal==id){shared.active=1;shared.result_index=command.index;shared.result_expiry=command.expires;}
                    }
                }
            }else result=NIMBY_INVALID_ARGUMENT;
        }catch(const std::bad_alloc&){result=NIMBY_RESOURCE_LIMIT;}
         catch(...){result=NIMBY_INTERNAL_ERROR;}
    return result;
}
}
