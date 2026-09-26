#include "runtime/texture_commands.h"
#include <memory>
// Identical regression on Windows and Linux: time boundaries, large tables,
// replacement, world changes and invalid requests preserving the prior command.
int main() {
    const nimby::texture_bridge::Command blinking{1,123,2500,5,6,500};
    for(const auto time:{0u,499u,1000u,1499u,4000u})
        if(nimby::texture_bridge::frame_index(blinking,time)!=5)return 53;
    for(const auto time:{500u,999u,1500u,1999u,4500u})
        if(nimby::texture_bridge::frame_index(blinking,time)!=6)return 54;
    const nimby::texture_bridge::Command fixed{1,123,2500,4};
    if(nimby::texture_bridge::frame_index(fixed,999)!=4)return 55;
    // Exercise the exact sorted table used by the client and native hook.
    auto table=std::make_unique<nimby::texture_bridge::Table>();
    for(uint64_t id=100;id>0;--id)if(!table->put({id,123,UINT64_MAX,0}))return 10;
    if(table->count()!=100)return 11;
    if(!table->put({50,456,UINT64_MAX,2})||table->count()!=100)return 12;
    for(uint64_t id=1;id<=100;++id){
        const auto& entry=table->entries[table->lower(id)];
        if(entry.signal!=id||entry.index!=(id==50?2u:0u))return 13;
    }
    if(!table->erase(50)||table->erase(50)||table->count()!=99)return 14;
    if(table->entries[table->lower(51)].signal!=51)return 15;
    if(!table->put({50,123,100,3}))return 16;
    table->prune(100);
    if(table->count()!=99||table->entries[table->lower(50)].signal!=51)return 17;
    table->put({50,123,UINT64_MAX,0});
    // Grow well beyond the old ceiling; updates/removals retain other IDs.
    for(uint64_t id=101;id<=100000;++id)
        if(!table->put({id,123,UINT64_MAX,0}))return 18;
    if(table->count()!=100000)return 19;
    if(!table->put({100,456,UINT64_MAX,1})||table->count()!=100000)return 20;
    if(!table->erase(1)||!table->put({100001,123,UINT64_MAX,0}))return 21;
    for(uint64_t id=2;id<=100001;++id){
        const auto at=table->lower(id);
        if(at>=table->count()||table->entries[at].signal!=id)return 22;
    }
    if(!table->put({50000,123,100,3}))return 23;
    table->prune(100);
    if(table->count()!=99999||table->entries[table->lower(50000)].signal!=50001)return 24;
    nimby::texture_bridge::Mailbox mailbox;
    mailbox.request_database=10;mailbox.request_simulation=20;
    mailbox.request_signal=123;mailbox.request_hash=456;
    mailbox.request_expiry=UINT64_MAX;mailbox.operation=1;
    if(nimby::texture_bridge::execute(mailbox,*table,100)!=NIMBY_OK||table->count()!=1)return 25;
    mailbox.request_signal=124;mailbox.request_expiry=200;
    if(nimby::texture_bridge::execute(mailbox,*table,100)!=NIMBY_OK)return 26;
    mailbox.operation=3;mailbox.request_signal=123;
    if(nimby::texture_bridge::execute(mailbox,*table,100)!=NIMBY_OK||!mailbox.active||mailbox.active_count!=2)return 27;
    nimby::texture_bridge::execute(mailbox,*table,200);
    if(!mailbox.active||mailbox.active_count!=1)return 28;
    mailbox.operation=2;mailbox.request_signal=999;
    if(nimby::texture_bridge::execute(mailbox,*table,200)!=NIMBY_INVALID_ARGUMENT||table->count()!=2)return 29;
    mailbox.request_database=11;mailbox.operation=3;
    nimby::texture_bridge::execute(mailbox,*table,200);
    if(mailbox.active||mailbox.active_count)return 30;
    mailbox.operation=1;mailbox.request_expiry=UINT64_MAX;
    if(nimby::texture_bridge::execute(mailbox,*table,200)!=NIMBY_OK||table->count()!=1)return 31;
    mailbox.request_index=5;mailbox.request_alternate_index=6;mailbox.request_half_period_ms=500;
    if(nimby::texture_bridge::execute(mailbox,*table,200)!=NIMBY_OK)return 56;
    if(table->entries[0].half_period_ms!=500 || nimby::texture_bridge::frame_index(table->entries[0],500)!=6)return 57;
    mailbox.request_half_period_ms=1;
    if(nimby::texture_bridge::execute(mailbox,*table,200)!=NIMBY_INVALID_ARGUMENT || table->entries[0].half_period_ms!=500)return 58;
    return 0;
}
