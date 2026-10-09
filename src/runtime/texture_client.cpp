#include <nimby/detail/diagnostics.hpp>
// Shared API validation, catalog resolution and world-consistency checks.
// Only the transport owns OS handles and wire-protocol publication.
#include <platform/texture_connection.h>
#include <engine/texture_file.h>
#include <engine/network.h>
#include <engine/signal_textures.h>
#include <nimby/detail/texture_updates.h>
#include <runtime/texture_publication_diagnostics.h>
#include <algorithm>
using Connection=nimby::platform::TextureConnection;

namespace {
bool copyText(auto& output,const char* input){
    if(!input)return false;
    const auto length=strnlen(input,sizeof output);
    if(length==sizeof output)return false;
    std::memcpy(output,input,length+1);return true;
}
struct TextureReply{std::vector<uint8_t> data;};
uint32_t remote(uint32_t operation,uint32_t pid,const void* data,size_t bytes,uint64_t count,TextureReply& reply,uint32_t capacity=0){
    reply.data.resize(capacity);uint32_t written{};
    const auto status=Connection::forward(operation,pid,data,bytes,count,reply.data,written);
    if(written>capacity)return NIMBY_INVALID_BINARY;
    reply.data.resize(written);return status;
}
}

uint32_t NimbyInternal_PublishTextureUpdates(uint32_t pid,uint64_t owner,const NimbyTextureUpdate* updates,uint32_t count) noexcept {
    using Reason=nimby::runtime::TexturePublicationReason;
    static thread_local nimby::runtime::TexturePublicationDiagnostics diagnostics;
    const auto failure=[&](uint32_t status,Reason reason,const NimbyTextureUpdate* row=nullptr,int64_t index=-1,
                           const nimby::engine::LiveState* world=nullptr,const nimby::engine::LiveState* after=nullptr){
        const auto target=Connection::hostTarget();
        diagnostics.report(status,reason,{pid?pid:target?target:Connection::currentPid(),owner,count,index,row,world,after},
            [](const char* message) noexcept {nimby::detail::diagnostics::write("sdk","ERROR",message);});
        return status;
    };
    if(!owner||!updates||!count||count>4096)return failure(NIMBY_INVALID_ARGUMENT,Reason::InvalidBatch);
    try{
        std::vector<uint64_t> ids;ids.reserve(count);
        for(uint32_t i=0;i<count;++i){
            const auto reason=nimby::runtime::invalidTextureUpdate(updates[i]);
            if(reason!=Reason::None)return failure(NIMBY_INVALID_ARGUMENT,reason,&updates[i],i);
            ids.push_back(updates[i].signal);
        }
        std::sort(ids.begin(),ids.end());
        if(const auto duplicate=std::adjacent_find(ids.begin(),ids.end());duplicate!=ids.end()){
            for(uint32_t i=0;i<count;++i)if(updates[i].signal==*duplicate)return failure(NIMBY_INVALID_ARGUMENT,Reason::DuplicateSignal,&updates[i],i);
        }
        if(Connection::hostTarget()){
            TextureReply reply;const auto status=remote(300,pid,updates,count*sizeof(*updates),count,reply);
            return status==NIMBY_OK?status:failure(status,Reason::BrokerRejected);
        }
        Connection connection;const auto opened=connection.open(pid?pid:Connection::currentPid());
        if(opened!=NIMBY_OK)return failure(opened,Reason::OpenRejected);
        nimby::engine::SignalTextureCatalog catalog;
        std::vector<std::string_view> names;names.reserve(count);
        for(uint32_t i=0;i<count;++i)names.emplace_back(updates[i].texture_set);
        std::sort(names.begin(),names.end());names.erase(std::unique(names.begin(),names.end()),names.end());
        if(!nimby::engine::read_signal_texture_sets(Connection::read,&connection,connection.live(),true,names,catalog))
            return failure(NIMBY_DATA_UNAVAILABLE,Reason::CatalogueUnreadable,nullptr,-1,&connection.live());
        std::vector<nimby::texture_bridge::Command> commands;commands.reserve(count);
        const auto now=Connection::now();
        for(uint32_t i=0;i<count;++i){const auto& row=updates[i];bool found=false;
            if(!nimby::engine::read_signal_membership(Connection::read,&connection,connection.live(),true,row.signal,found))
                return failure(NIMBY_DATA_UNAVAILABLE,Reason::MembershipUnreadable,&row,i,&connection.live());
            if(!found)return failure(NIMBY_INVALID_ARGUMENT,Reason::SignalMissing,&row,i,&connection.live());
            uint64_t hash=0;uint32_t index=row.index,alternate=0;
            if(row.flags&1){
                for(const auto& [key,set]:catalog.sets)if(set.name==row.texture_set){
                    if(hash)return failure(NIMBY_INVALID_ARGUMENT,Reason::NumericCatalogueAmbiguous,&row,i,&connection.live());
                    if(index>=set.files.size())return failure(NIMBY_INVALID_ARGUMENT,Reason::NumericIndexOutOfRange,&row,i,&connection.live());
                    hash=key;
                }
                if(!hash)return failure(NIMBY_DATA_UNAVAILABLE,Reason::CatalogueMissing,&row,i,&connection.live());
            }else{
                const auto status=nimby::engine::resolve_texture_file(catalog,row.texture_set,row.first_path,hash,index);
                if(status!=NIMBY_OK)return failure(status,status==NIMBY_INVALID_ARGUMENT?Reason::FirstPathAmbiguous:Reason::FirstPathMissing,&row,i,&connection.live());
            }
            if(row.half_period_ms){uint64_t alternateHash{};
                const auto status=nimby::engine::resolve_texture_file(catalog,row.texture_set,row.alternate_path,alternateHash,alternate);
                if(status!=NIMBY_OK)return failure(status,status==NIMBY_INVALID_ARGUMENT?Reason::AlternatePathAmbiguous:Reason::AlternatePathMissing,&row,i,&connection.live());
                if(hash!=alternateHash)return failure(NIMBY_INVALID_ARGUMENT,Reason::AnimationCatalogueMismatch,&row,i,&connection.live());
            }
            commands.push_back({row.signal,hash,row.duration_ms?now+row.duration_ms:UINT64_MAX,index,alternate,row.half_period_ms,owner});
        }
        nimby::engine::LiveState after{};
        if(!nimby::engine::resolve_live_state(Connection::read,&connection,connection.live().module_base,true,connection.live().profile,after))
            return failure(NIMBY_DATA_UNAVAILABLE,Reason::WorldUnreadable,nullptr,-1,&connection.live(),&after);
        if(after!=connection.live())
            return failure(NIMBY_DATA_UNAVAILABLE,Reason::WorldChanged,nullptr,-1,&connection.live(),&after);
        const auto status=connection.publish(owner,commands);
        return status==NIMBY_OK?status:failure(status,Reason::BridgeRejected,nullptr,-1,&connection.live());
    }catch(const std::bad_alloc&){return failure(NIMBY_RESOURCE_LIMIT,Reason::AllocationFailed);}
    catch(...){nimby::detail::diagnostics::exception("sdk",__func__);return failure(NIMBY_INTERNAL_ERROR,Reason::InternalError);}
}
uint32_t NimbyInternal_ClearOwnedTextures(uint32_t pid,uint64_t owner,const uint64_t* signals,uint32_t count) noexcept {
    if(!owner||(!signals&&count)||count>4096)return NIMBY_INVALID_ARGUMENT;
    for(uint32_t i=0;i<count;++i)if(signals[i]>>48!=8)return NIMBY_INVALID_ARGUMENT;
    try{
        if(Connection::hostTarget()){TextureReply reply;return remote(301,pid,signals,count*sizeof(*signals),count,reply);}
        Connection connection;const auto opened=connection.open(pid?pid:Connection::currentPid());if(opened!=NIMBY_OK)return opened;
        return connection.clear(owner,{signals,count});
    }catch(...){nimby::detail::diagnostics::exception("sdk",__func__);return NIMBY_INTERNAL_ERROR;}
}
uint32_t NimbyInternal_ReleaseTextureOwner(uint32_t pid,uint64_t owner) noexcept {
    if(!owner)return NIMBY_INVALID_ARGUMENT;
    try{
        if(Connection::hostTarget()){TextureReply reply;return remote(302,pid,nullptr,0,0,reply);}
        if(pid&&pid!=Connection::currentPid())return NIMBY_INVALID_ARGUMENT;
        return Connection::releaseInGameOwner(owner);
    }catch(...){nimby::detail::diagnostics::exception("sdk",__func__);return NIMBY_INTERNAL_ERROR;}
}

static uint32_t set_texture(uint32_t pid,uint64_t signal,const char* set_id,
                           uint32_t index,uint32_t duration,bool persistent,const char* file=nullptr,
                           const char* alternateFile=nullptr,uint32_t halfPeriod=0) noexcept {
    if((signal>>48)!=8||!set_id||!set_id[0]||(!persistent&&(duration<1000||duration>60000)))return NIMBY_INVALID_ARGUMENT;
    if(halfPeriod && (!file||!alternateFile||!*alternateFile||halfPeriod<100||halfPeriod>10000))return NIMBY_INVALID_ARGUMENT;
    try {
        if(Connection::hostTarget()){
            NimbyTextureUpdate row{};row.signal=signal;row.duration_ms=persistent?0:duration;row.half_period_ms=halfPeriod;row.index=index;row.flags=file?0:1;
            if(!copyText(row.texture_set,set_id)||(file&&!copyText(row.first_path,file))||(halfPeriod&&!copyText(row.alternate_path,alternateFile)))return NIMBY_INVALID_ARGUMENT;
            return NimbyInternal_PublishTextureUpdates(pid,1,&row,1);
        }
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        bool found=false;nimby::engine::SignalTextureCatalog catalog{};
        const std::string_view name(set_id);
        if(!nimby::engine::read_signal_membership(Connection::read,&c,c.live(),true,signal,found)||
           !nimby::engine::read_signal_texture_sets(Connection::read,&c,c.live(),true,std::span(&name,1),catalog))return NIMBY_DATA_UNAVAILABLE;
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
        if(Connection::hostTarget()){
            TextureReply reply;const auto status=remote(304,pid,nullptr,0,0,reply,sizeof *out);
            if(status!=NIMBY_OK)return status;
            if(reply.data.size()!=sizeof *out)return NIMBY_INVALID_BINARY;
            std::memcpy(out,reply.data.data(),sizeof *out);return NIMBY_OK;
        }
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        c.previewStatus(*out);
        return NIMBY_OK;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
uint32_t NimbyInternal_ClearTexturePreview(uint32_t pid,uint64_t signal) noexcept {
    if(!signal)return NIMBY_INVALID_ARGUMENT;
    try {
        if(Connection::hostTarget())return NimbyInternal_ClearOwnedTextures(pid,1,&signal,1);
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
        if(Connection::hostTarget()){
            TextureReply reply;const auto status=remote(303,pid,signals,count*sizeof(*signals),count,reply,count*sizeof(*out));
            if(status!=NIMBY_OK)return status;
            if(reply.data.size()!=count*sizeof(*out))return NIMBY_INVALID_BINARY;
            std::memcpy(out,reply.data.data(),reply.data.size());return NIMBY_OK;
        }
        Connection c;const auto status=c.open(pid);if(status!=NIMBY_OK)return status;
        const auto result=c.statuses({signals,count},out);
        if(result!=NIMBY_OK)for(uint32_t j=0;j<count;++j){out[j]={};out[j].struct_size=sizeof(*out);}
        return result;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
