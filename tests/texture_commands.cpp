#include <nimby/texture_preview.hpp>
#include <nimby/signal_textures.hpp>
#include "engine/texture_file.h"
#include <cassert>
#include <memory>
int main() {
    constexpr uint64_t signal=0x8000000000001;
    for(auto duration:{0u,999u,60001u})
        if(NimbyInternal_ShowSignalTextureFor(0,signal,"set","a.svg",duration)!=NIMBY_INVALID_ARGUMENT)return 50;
    for(auto half:{0u,99u,10001u})
        if(NimbyInternal_ShowSignalAnimationFor(0,signal,"set","a.svg","b.svg",half,2500)!=NIMBY_INVALID_ARGUMENT)return 51;
    if(NimbyInternal_ShowSignalAnimationFor(0,signal,"set","a.svg",nullptr,500,2500)!=NIMBY_INVALID_ARGUMENT)return 52;
    if(NimbyInternal_ShowSignalTexture(0,1,"set",nullptr)!=NIMBY_INVALID_ARGUMENT)return 40;
    if(NimbyInternal_ShowSignalTexture(0,0,"set","imgs/test.svg")!=NIMBY_INVALID_ARGUMENT)return 41;
    if(NimbyInternal_ShowSignalTexture(0,signal,"set","imgs/test.svg")!=NIMBY_INVALID_BINARY)return 42;
    if(NimbyInternal_ShowSignalTexture(0,1,"set","imgs/test.svg")!=NIMBY_INVALID_ARGUMENT)return 59;
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
    // Batch bounds are checked before connecting or touching caller arrays.
    NimbySignalTextureOverrideStatus status{};status.struct_size=sizeof status;
    if(NimbyInternal_SignalTextureOverrideStatuses(1,&signal,33,&status)!=NIMBY_INVALID_ARGUMENT)return 9;
    if(NimbyInternal_SignalTextureOverrideStatuses(1,&signal,0,&status)!=NIMBY_INVALID_ARGUMENT)return 10;
    status.struct_size=0;
    if(NimbyInternal_SignalTextureOverrideStatuses(1,&signal,1,&status)!=NIMBY_INVALID_ARGUMENT)return 11;
    return 0;
}
