#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/texture_updates.h>
#include <nimby/texture_preview.hpp>
#include <stdexcept>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <set>
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
std::atomic<uint64_t> lastOwner{};std::atomic<uint32_t> lastCount{};std::atomic<int> calls=0;
std::atomic<bool> busy=false;std::atomic<unsigned> releaseAttempts=0;
std::mutex releasedMutex;std::set<uint64_t> releasedOwners;
template<class Predicate> bool wait(Predicate predicate){
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(3);
    while(!predicate()&&std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(std::chrono::milliseconds(2));
    return predicate();
}
}
extern "C" uint32_t __cdecl NimbyInternal_PublishTextureUpdates(uint32_t,uint64_t owner,const NimbyTextureUpdate*,uint32_t count)noexcept{lastOwner=owner;lastCount=count;++calls;return NIMBY_OK;}
extern "C" uint32_t __cdecl NimbyInternal_ClearOwnedTextures(uint32_t,uint64_t owner,const uint64_t*,uint32_t count)noexcept{lastOwner=owner;lastCount=count;++calls;return NIMBY_OK;}
extern "C" uint32_t __cdecl NimbyInternal_ReleaseTextureOwner(uint32_t,uint64_t owner)noexcept{
    lastOwner=owner;lastCount=0;++calls;++releaseAttempts;if(busy)return NIMBY_RESOURCE_LIMIT;
    std::lock_guard lock(releasedMutex);releasedOwners.insert(owner);return NIMBY_OK;
}
extern "C" uint32_t __cdecl NimbyInternal_SignalTextureOverrideStatuses(uint32_t,const uint64_t*,uint32_t count,NimbySignalTextureOverrideStatus* out)noexcept{
    lastCount=count;++calls;for(uint32_t i=0;i<count;++i){out[i]={};out[i].struct_size=sizeof(out[i]);out[i].active=1;}return NIMBY_OK;
}
extern "C" uint32_t __cdecl NimbyInternal_TexturePreviewStatus(uint32_t,NimbyTexturePreviewStatus* out)noexcept{out->callbacks=777;++calls;return NIMBY_OK;}
int main(){try{
    using namespace nimby::mod_host;Owners owners;owners.textures=4242;
    Request request;Reply reply;request.operation=300;request.args[0]=1;request.args[7]=999;
    NimbyTextureUpdate row{};row.signal=0x8000000000001;row.duration_ms=2500;
    std::strcpy(row.texture_set,"test");std::strcpy(row.first_path,"a.svg");append(request,&row);
    CHECK(dispatchTextures(request,reply,owners)==NIMBY_OK&&lastOwner==4242&&lastCount==1);
    const auto before=calls.load();request.data.pop_back();CHECK(dispatchTextures(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&calls==before);
    request.args[0]=UINT64_MAX;CHECK(dispatchTextures(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&calls==before);
    request={};request.operation=301;request.args[0]=1;request.args[7]=123;append(request,&row.signal);
    CHECK(dispatchTextures(request,reply,owners)==NIMBY_OK&&lastOwner==4242&&lastCount==1);
    request.operation=303;CHECK(dispatchTextures(request,reply,owners)==NIMBY_OK&&reply.data.size()==sizeof(NimbySignalTextureOverrideStatus));
    request.args[0]=33;CHECK(dispatchTextures(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    request={};request.operation=304;CHECK(dispatchTextures(request,reply,owners)==NIMBY_OK&&reply.data.size()==sizeof(NimbyTexturePreviewStatus));
    NimbyTexturePreviewStatus status{};std::memcpy(&status,reply.data.data(),sizeof status);CHECK(status.callbacks==777);
    request={};request.operation=302;request.args[7]=999;
    CHECK(dispatchTextures(request,reply,owners)==NIMBY_OK&&lastOwner==4242);
    {std::lock_guard lock(releasedMutex);releasedOwners.clear();}
    busy=true;releaseAttempts=0;
    const auto started=std::chrono::steady_clock::now();cleanupTextures(owners);
    CHECK(std::chrono::steady_clock::now()-started<std::chrono::milliseconds(50));
    CHECK(!owners.textures&&!owners.texturesTracked);
    CHECK(wait([]{return releaseAttempts>=4;}));
    CHECK(dispatchTextures(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    // A stuck retirement retains its reserved slot; healthy peers can publish
    // until the fixed bound is reached, without losing the dead worker's token.
    request={};request.operation=300;request.args[0]=1;append(request,&row);
    std::array<Owners,64> peers;
    for(size_t i=0;i<63;++i){peers[i].textures=5000+i;CHECK(dispatchTextures(request,reply,peers[i])==NIMBY_OK);}
    peers[63].textures=6000;CHECK(dispatchTextures(request,reply,peers[63])==NIMBY_RESOURCE_LIMIT);
    {std::lock_guard lock(releasedMutex);CHECK(releasedOwners.empty());}
    busy=false;
    CHECK(wait([]{std::lock_guard lock(releasedMutex);return releasedOwners.contains(4242);}));
    CHECK(wait([&]{return dispatchTextures(request,reply,peers[63])==NIMBY_OK;}));
    for(auto& peer:peers)cleanupTextures(peer);
    CHECK(wait([]{std::lock_guard lock(releasedMutex);return releasedOwners.size()==65;}));
    std::cout<<"PASS texture RPC bounds, trusted owner and bounded retirement retries under contention\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
