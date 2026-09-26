#include <nimby/detail/diagnostics.hpp>
// Shared API validation, catalog resolution and world-consistency checks.
// Only the transport owns OS handles and wire-protocol publication.
#include <platform/texture_connection.h>
#include <engine/texture_file.h>
#include <engine/network.h>
#include <engine/signal_textures.h>
using Connection=nimby::platform::TextureConnection;

static uint32_t set_texture(uint32_t pid,uint64_t signal,const char* set_id,
                           uint32_t index,uint32_t duration,bool persistent,const char* file=nullptr,
                           const char* alternateFile=nullptr,uint32_t halfPeriod=0) noexcept {
    if((signal>>48)!=8||!set_id||!set_id[0]||(!persistent&&(duration<1000||duration>60000)))return NIMBY_INVALID_ARGUMENT;
    if(halfPeriod && (!file||!alternateFile||!*alternateFile||halfPeriod<100||halfPeriod>10000))return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        bool found=false;nimby::engine::SignalTextureCatalog catalog{};
        if(!nimby::engine::read_signal_membership(Connection::read,&c,c.live(),true,signal,found)||
           !nimby::engine::read_signal_texture_catalog(Connection::read,&c,c.live(),true,catalog))return NIMBY_DATA_UNAVAILABLE;
        if(!found)return NIMBY_INVALID_ARGUMENT;
        uint64_t hash{};uint32_t alternateIndex=0;
        if(file){
            const auto resolved=nimby::engine::resolve_texture_file(catalog,set_id,file,hash,index);
            if(resolved!=NIMBY_OK)return resolved;
        }else{
            for(const auto& [key,set]:catalog.sets)if(set.name==set_id){
                if(index>=set.files.size())return NIMBY_INVALID_ARGUMENT;
                hash=key;break;
            }
        }
        if(!hash)return NIMBY_DATA_UNAVAILABLE;
        if(halfPeriod){
            uint64_t alternateHash{};
            const auto resolved=nimby::engine::resolve_texture_file(catalog,set_id,alternateFile,alternateHash,alternateIndex);
            if(resolved!=NIMBY_OK)return resolved;
            if(alternateHash!=hash)return NIMBY_INVALID_ARGUMENT;
        }
        nimby::engine::LiveState after{};
        if(!nimby::engine::resolve_live_state(Connection::read,&c,c.live().module_base,true,c.live().profile,after)||after!=c.live())return NIMBY_DATA_UNAVAILABLE;
        return c.submit(1,signal,hash,persistent?UINT64_MAX:Connection::now()+duration,index,alternateIndex,halfPeriod);
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
uint32_t NimbyInternal_ShowSignalTexture(uint32_t pid,uint64_t signal,const char* set,const char* path) noexcept {
    if(!path||!path[0])return NIMBY_INVALID_ARGUMENT;
    return set_texture(pid?pid:Connection::currentPid(),signal,set,0,0,true,path);
}
uint32_t NimbyInternal_ShowSignalTextureFor(uint32_t pid,uint64_t signal,const char* set,const char* path,uint32_t duration) noexcept {
    if(!path||!path[0])return NIMBY_INVALID_ARGUMENT;
    return set_texture(pid?pid:Connection::currentPid(),signal,set,0,duration,false,path);
}
uint32_t NimbyInternal_ShowSignalAnimationFor(uint32_t pid,uint64_t signal,const char* set,
    const char* path,const char* alternate,uint32_t halfPeriod,uint32_t duration) noexcept {
    if(!path||!path[0]||!halfPeriod)return NIMBY_INVALID_ARGUMENT;
    return set_texture(pid?pid:Connection::currentPid(),signal,set,0,duration,false,path,alternate,halfPeriod);
}
uint32_t NimbyInternal_RestoreSignalTexture(uint32_t pid,uint64_t signal) noexcept {
    return NimbyInternal_ClearTexturePreview(pid?pid:Connection::currentPid(),signal);
}
uint32_t NimbyInternal_PreviewSignalTexture(uint32_t pid,uint64_t signal,const char* set_id,
                                                  uint32_t index,uint32_t duration) noexcept {
    return set_texture(pid,signal,set_id,index,duration,false);
}
uint32_t NimbyInternal_ForceSignalTexture(uint32_t pid,uint64_t signal,const char* set_id,
                                                uint32_t index) noexcept {
    return set_texture(pid,signal,set_id,index,0,true);
}
uint32_t NimbyInternal_TexturePreviewStatus(uint32_t pid,NimbyTexturePreviewStatus* out) noexcept {
    if(!out||out->struct_size!=sizeof(*out))return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        c.previewStatus(*out);
        return NIMBY_OK;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
uint32_t NimbyInternal_ClearTexturePreview(uint32_t pid,uint64_t signal) noexcept {
    if(!signal)return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        return c.submit(2,signal);
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
uint32_t NimbyInternal_SignalTextureOverrideStatus(uint32_t pid,uint64_t signal,
                                                         NimbySignalTextureOverrideStatus* out) noexcept {
    return NimbyInternal_SignalTextureOverrideStatuses(pid,&signal,1,out);
}
uint32_t NimbyInternal_SignalTextureOverrideStatuses(uint32_t pid,const uint64_t* signals,
    uint32_t count,NimbySignalTextureOverrideStatus* out) noexcept {
    if(!signals||!out||!count||count>32)return NIMBY_INVALID_ARGUMENT;
    for(uint32_t i=0;i<count;++i)
        if(!signals[i]||out[i].struct_size!=sizeof(*out))return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        for(uint32_t i=0;i<count;++i) {
            const auto result=c.submit(3,signals[i]);
            if(result!=NIMBY_OK) {
                for(uint32_t j=0;j<count;++j){out[j]={};out[j].struct_size=sizeof(*out);}
                return result;
            }
            c.overrideStatus(out[i]);
        }
        return NIMBY_OK;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
