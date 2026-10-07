#include <platform/windows/mod_host_protocol.h>
#include <platform/windows/mod_host_releases.h>
#include <nimby/detail/texture_updates.h>
#include <nimby/texture_preview.hpp>
#include <windows.h>
namespace nimby::mod_host {
namespace {
unsigned releaseTextures(uint64_t owner,unsigned pending) noexcept {
    return NimbyInternal_ReleaseTextureOwner(GetCurrentProcessId(),owner)==NIMBY_OK?pending:0;
}
OwnerReleases& releases(){static auto* queue=new OwnerReleases(&releaseTextures,1);return *queue;}
bool track(Owners& owners){
    if(owners.texturesTracked)return true;
    return owners.texturesTracked=releases().track(owners.textures);
}
}
uint32_t dispatchTextures(const Request& request,Reply& reply,Owners& owners){
    if(!owners.textures)return NIMBY_INVALID_ARGUMENT;
    if(request.operation==300){
        if(!request.args[0]||request.args[0]>4096||request.data.size()!=request.args[0]*sizeof(NimbyTextureUpdate))return NIMBY_INVALID_ARGUMENT;
        if(!track(owners))return NIMBY_RESOURCE_LIMIT;
        std::vector<NimbyTextureUpdate> rows(static_cast<size_t>(request.args[0]));
        std::memcpy(rows.data(),request.data.data(),request.data.size());
        return NimbyInternal_PublishTextureUpdates(GetCurrentProcessId(),owners.textures,rows.data(),static_cast<uint32_t>(rows.size()));
    }
    if(request.operation==301||request.operation==303){
        const auto limit=request.operation==301?4096u:32u;
        if(request.args[0]>limit||(!request.args[0]&&request.operation==303)||request.data.size()!=request.args[0]*sizeof(uint64_t))return NIMBY_INVALID_ARGUMENT;
        std::vector<uint64_t> ids(static_cast<size_t>(request.args[0]));
        if(!ids.empty())std::memcpy(ids.data(),request.data.data(),request.data.size());
        if(request.operation==301)return NimbyInternal_ClearOwnedTextures(GetCurrentProcessId(),owners.textures,ids.data(),static_cast<uint32_t>(ids.size()));
        std::vector<NimbySignalTextureOverrideStatus> values(ids.size());
        for(auto& row:values)row.struct_size=sizeof row;
        const auto status=NimbyInternal_SignalTextureOverrideStatuses(GetCurrentProcessId(),ids.data(),static_cast<uint32_t>(ids.size()),values.data());
        if(status==NIMBY_OK)output(reply,values.data(),values.size());
        return status;
    }
    if(request.operation==302){
        if(!request.data.empty()||request.args[0])return NIMBY_INVALID_ARGUMENT;
        return NimbyInternal_ReleaseTextureOwner(GetCurrentProcessId(),owners.textures);
    }
    if(request.operation==304){
        if(!request.data.empty()||request.args[0])return NIMBY_INVALID_ARGUMENT;
        NimbyTexturePreviewStatus value{};value.struct_size=sizeof value;
        const auto status=NimbyInternal_TexturePreviewStatus(GetCurrentProcessId(),&value);
        if(status==NIMBY_OK)output(reply,&value);
        return status;
    }
    return NIMBY_INVALID_ARGUMENT;
}
void cleanupTextures(Owners& owners)noexcept{
    if(owners.texturesTracked)releases().retire(owners.textures);
    owners.texturesTracked=false;owners.textures=0;
}
}
