#pragma once
#include "runtime/texture_table.h"
#include <nimby/detail/observation.h>
namespace nimby::texture_bridge {
// Value-only request/state/result for the common command processor. The OS
// transport copies this value while holding its mailbox lock. No volatile,
// process handle, event, mapping name or platform clock leaks into this model.
struct Mailbox {
    uint64_t expected_database=0, expected_simulation=0, signal=0, expires=0;
    uint32_t operation=0, active=0, result_index=0;
    uint64_t request_signal=0, request_hash=0, request_expiry=0;
    uint64_t request_database=0, request_simulation=0, active_count=0, result_expiry=0;
    uint32_t request_index=0, request_alternate_index=0, request_half_period_ms=0;
    uint64_t request_generation=0,result_generation=0;
};
// Called with exclusive ownership of the table. No game memory is modified.
inline uint32_t execute(Mailbox& shared,Table& commands,uint64_t now) noexcept {
    uint32_t result=NIMBY_OK;
        try {
            const bool same=shared.expected_database==shared.request_database &&
                            shared.expected_simulation==shared.request_simulation;
            const auto id=shared.request_signal;
            if(shared.operation==1){
                const auto at=commands.lower(id);
                if(same&&at<commands.count()&&commands.entries[at].signal==id&&commands.entries[at].owner&&commands.entries[at].expires>now)
                    return NIMBY_RESOURCE_LIMIT;
                if(shared.request_half_period_ms && (shared.request_half_period_ms<100 || shared.request_half_period_ms>10000))
                    return NIMBY_INVALID_ARGUMENT;
                const nimby::texture_bridge::Command command{id,shared.request_hash,shared.request_expiry,
                    shared.request_index,shared.request_alternate_index,shared.request_half_period_ms};
                if(same){commands.prune(now);commands.put(command);}
                else {nimby::texture_bridge::Table fresh;fresh.put(command);commands.entries.swap(fresh.entries);}
                shared.expected_database=shared.request_database;
                shared.expected_simulation=shared.request_simulation;
                shared.signal=id;shared.expires=command.expires;
            }else if(shared.operation==2){
                const auto at=commands.lower(id);
                if(same&&at<commands.count()&&commands.entries[at].signal==id&&commands.entries[at].owner)return NIMBY_RESOURCE_LIMIT;
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
