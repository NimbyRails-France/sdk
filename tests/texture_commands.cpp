#include <nimby/texture_preview.hpp>
#include <nimby/signal_textures.hpp>
#include "engine/texture_file.h"
#include <cassert>
#include "runtime/texture_bridge.h"
#include "runtime/texture_commands.h"
#include <memory>
int main() {
    if(NimbyInternal_ShowSignalTexture(0,1,"set",nullptr)!=NIMBY_INVALID_ARGUMENT)return 40;
    if(NimbyInternal_ShowSignalTexture(0,0,"set","imgs/test.svg")!=NIMBY_INVALID_ARGUMENT)return 41;
    if(NimbyInternal_ShowSignalTexture(0,1,"set","imgs/test.svg")!=NIMBY_INVALID_BINARY)return 42;
    if(NimbyInternal_RestoreSignalTexture(0,0)!=NIMBY_INVALID_ARGUMENT)return 43;
    try { nimby::SignalTextures::connect(0); return 44; }
    catch(const nimby::Exception& error) { if(error.code()!=nimby::ErrorCode::InvalidArgument)return 45; }
    nimby::engine::SignalTextureCatalog catalog;
    catalog.sets[123]={123,"set",{{0,10,"mod","imgs/second.svg"},{0,11,"mod","imgs/first.svg"}}};
    uint64_t texture_hash{};uint32_t texture_index{};
    if(nimby::engine::resolve_texture_file(catalog,"set","imgs\\first.svg",texture_hash,texture_index)!=NIMBY_OK ||
       texture_hash!=123 || texture_index!=1)return 46;
    if(nimby::engine::resolve_texture_file(catalog,"other","imgs/first.svg",texture_hash,texture_index)!=NIMBY_DATA_UNAVAILABLE)return 47;
    if(nimby::engine::resolve_texture_file(catalog,"set","imgs/missing.svg",texture_hash,texture_index)!=NIMBY_DATA_UNAVAILABLE)return 48;
    catalog.sets[123].files.push_back(catalog.sets[123].files.back());
    if(nimby::engine::resolve_texture_file(catalog,"set","imgs/first.svg",texture_hash,texture_index)!=NIMBY_INVALID_ARGUMENT)return 49;
    // Invalid inputs are rejected before loading anything into a game process.
    if(NimbyInternal_ForceSignalTexture(0,1,"set",0)!=NIMBY_INVALID_ARGUMENT)return 1;
    if(NimbyInternal_ForceSignalTexture(1,0,"set",0)!=NIMBY_INVALID_ARGUMENT)return 2;
    if(NimbyInternal_ForceSignalTexture(1,1,nullptr,0)!=NIMBY_INVALID_ARGUMENT)return 3;
    if(NimbyInternal_ForceSignalTexture(1,1,"",0)!=NIMBY_INVALID_ARGUMENT)return 4;
    for(auto duration:{0u,999u,60001u,UINT32_MAX})
        if(NimbyInternal_PreviewSignalTexture(1,1,"set",0,duration)!=NIMBY_INVALID_ARGUMENT)return 5;
    if(NimbyInternal_TexturePreviewStatus(0,nullptr)!=NIMBY_INVALID_ARGUMENT)return 6;
    if(NimbyInternal_ClearTexturePreview(1,0)!=NIMBY_INVALID_ARGUMENT)return 7;
    if(NimbyInternal_SignalTextureOverrideStatus(1,0,nullptr)!=NIMBY_INVALID_ARGUMENT)return 8;
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
    nimby::texture_bridge::Shared mailbox;
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
    return 0;
}
