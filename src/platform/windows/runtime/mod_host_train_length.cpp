#include <platform/windows/mod_host_protocol.h>
#include <platform/windows/mod_host_releases.h>
#include <nimby/detail/train_length.h>
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/mod_options_bridge.h>
#include <runtime/mod_options_registry.h>
#include <engine/train_length_policy.h>
#include <engine/train_editor_messages.h>
#include <windows.h>
#include <atomic>
#include <bit>
#include <limits>
#include <string>

namespace nimby::mod_host {
namespace {
constexpr uint32_t registerOperation=400,updateOperation=401,removeOperation=402,editorRegisterOperation=403;
using Register=uint32_t(__cdecl*)(const char*,uint32_t,uint64_t*);
using RegisterEditor=uint32_t(__cdecl*)(const char*,uint32_t,const char*,uint32_t,uint64_t*);
using Update=uint32_t(__cdecl*)(uint64_t,uint32_t);
using Remove=uint32_t(__cdecl*)(uint64_t);
template<class F> uint32_t boundary(F&& f)noexcept {
    try{return f();}catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}
    catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
template<class T,class... Args> uint32_t call(const char* name,Args... args) {
    const auto module=GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll");
    const auto fn=module?std::bit_cast<T>(GetProcAddress(module,name)):nullptr;
    return fn?fn(args...):NIMBY_HOOKS_UNAVAILABLE;
}
bool validMaximum(uint64_t meters)noexcept {
    return meters>=engine::train_length::minimumMaximumMeters&&meters<=engine::train_length::maximumMaximumMeters;
}
bool unusedArguments(const Request& request,size_t first)noexcept {
    for(size_t i=first;i<request.args.size();++i)if(request.args[i])return false;
    return true;
}
// Reserve retirement capacity before publishing any native limit. IPC tokens
// and native bridge tokens are separate; neither namespace is client-chosen.
class Claims {
    struct Claim {uint64_t token{},nativeOwner{};};
    std::array<Claim,engine::train_length::maximumOwners> values_{};
    std::mutex mutex_;
    std::atomic<uint64_t> next_{1};
public:
    uint64_t reserve() {
        auto token=next_.load(std::memory_order_relaxed);
        do {
            if(!token||token==std::numeric_limits<uint64_t>::max())return 0;
        } while(!next_.compare_exchange_weak(token,token+1,std::memory_order_relaxed));
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return 0;
        for(auto& value:values_)if(!value.token){value={token,0};return token;}
        return 0;
    }
    void bind(uint64_t token,uint64_t nativeOwner) {
        std::lock_guard lock(mutex_);
        for(auto& value:values_)if(value.token==token){value.nativeOwner=nativeOwner;return;}
    }
    uint64_t nativeOwner(uint64_t token) {
        std::lock_guard lock(mutex_);
        for(const auto& value:values_)if(value.token==token)return value.nativeOwner;
        return 0;
    }
    void forget(uint64_t token) {
        std::lock_guard lock(mutex_);
        for(auto& value:values_)if(value.token==token){value={};return;}
    }
};
Claims& claims(){static auto* values=new Claims;return *values;}
void retirementLog(const char* phase,uint64_t token,uint64_t nativeOwner,uint32_t status,bool deferred)noexcept {
    char text[320]{};
    std::snprintf(text,sizeof text,"Train length owner cleanup: phase=%s ipcOwner=%llu nativeOwner=%llu status=%u statusScope=%s retryDeferred=%u",
        phase,static_cast<unsigned long long>(token),static_cast<unsigned long long>(nativeOwner),status,
        deferred?"retirement-queued":"native-remove",unsigned(deferred));
    nimby::detail::diagnostics::write("sdk","INFO",text);
}
unsigned releaseClaim(uint64_t token,unsigned pending)noexcept {
    try {
        const auto nativeOwner=claims().nativeOwner(token);
        uint32_t status=NIMBY_OK;
        if(nativeOwner){
            status=call<Remove>("NimbyTrainLength_Remove",nativeOwner);
            if(status!=NIMBY_OK&&status!=NIMBY_INVALID_HANDLE)return 0;
        }
        claims().forget(token);
        if(nativeOwner)retirementLog("owner-policy-released",token,nativeOwner,status,false);
        return pending;
    }catch(...){return 0;}
}
OwnerReleases& releases(){static auto* queue=new OwnerReleases(&releaseClaim,1);return *queue;}

uint32_t registerLimit(const char* modId,uint32_t maximumMeters,uint64_t* owner)noexcept {
    if(owner)*owner=0;
    if(!owner||!modId||!validMaximum(maximumMeters))return NIMBY_INVALID_ARGUMENT;
    size_t bytes=0;while(bytes<=128&&modId[bytes])++bytes;
    if(!bytes||bytes>128||!runtime::mod_options::Registry::validId({modId,bytes}))return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{
        // The existing parent endpoint installs and verifies this bridge. No
        // child-local game module or native address is used for registration.
        Request ensure;ensure.operation=1;Reply ensured;
        const auto ready=invoke(ensure,ensured,0);
        if(ready!=NIMBY_OK&&ready!=NIMBY_ALREADY_INITIALIZED)return ready;
        Request request;request.operation=registerOperation;request.args[0]=maximumMeters;append(request,modId,bytes);
        Reply reply;const auto status=invoke(request,reply,0);
        if(status==NIMBY_OK){if(!reply.args[0])return uint32_t(NIMBY_INVALID_BINARY);*owner=reply.args[0];}
        return status;
    });
}
uint32_t registerEditor(const char* modId,uint32_t maximumMeters,const char* declaration,
                        uint32_t declarationBytes,uint64_t* owner)noexcept {
    if(owner)*owner=0;
    if(!owner||!modId||!validMaximum(maximumMeters)||!declaration||!declarationBytes||
       declarationBytes>NIMBY_OPTIONS_SCHEMA_LIMIT)return NIMBY_INVALID_ARGUMENT;
    size_t idBytes=0;while(idBytes<=128&&modId[idBytes])++idBytes;
    if(!runtime::mod_options::Registry::validId({modId,idBytes})||
       !runtime::mod_options::Registry::validText({declaration,declarationBytes},NIMBY_OPTIONS_SCHEMA_LIMIT))
        return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{
        Request ensure;ensure.operation=1;Reply ensured;
        const auto ready=invoke(ensure,ensured,0);
        if(ready!=NIMBY_OK&&ready!=NIMBY_ALREADY_INITIALIZED)return ready;
        Request request;request.operation=editorRegisterOperation;
        request.args={maximumMeters,idBytes,declarationBytes};
        append(request,modId,idBytes);append(request,declaration,declarationBytes);
        Reply reply;const auto status=invoke(request,reply,0);
        if(status==NIMBY_OK){if(!reply.args[0])return uint32_t(NIMBY_INVALID_BINARY);*owner=reply.args[0];}
        return status;
    });
}
uint32_t updateLimit(uint64_t owner,uint32_t maximumMeters)noexcept {
    if(!owner||!validMaximum(maximumMeters))return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request;request.operation=updateOperation;request.args[0]=owner;request.args[1]=maximumMeters;
        Reply reply;return invoke(request,reply,0);});
}
uint32_t removeLimit(uint64_t owner)noexcept {
    if(!owner)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request;request.operation=removeOperation;request.args[0]=owner;
        Reply reply;return invoke(request,reply,0);});
}
}

uint32_t dispatchTrainLength(const Request& request,Reply& reply,Owners& owners) {
    return boundary([&]()->uint32_t{
        if(request.operation==registerOperation||request.operation==editorRegisterOperation){
            const bool editor=request.operation==editorRegisterOperation;
            if(owners.trainLength||!unusedArguments(request,editor?3:1)||!validMaximum(request.args[0]))
                return NIMBY_INVALID_ARGUMENT;
            size_t idBytes=request.data.size(),declarationBytes=0;
            if(editor){
                if(!request.args[1]||request.args[1]>128||!request.args[2]||request.args[2]>NIMBY_OPTIONS_SCHEMA_LIMIT||
                   request.args[1]+request.args[2]!=request.data.size())return NIMBY_INVALID_ARGUMENT;
                idBytes=static_cast<size_t>(request.args[1]);declarationBytes=static_cast<size_t>(request.args[2]);
            }
            const auto* bytes=reinterpret_cast<const char*>(request.data.data());
            if(!runtime::mod_options::Registry::validId({bytes,idBytes}))return NIMBY_INVALID_ARGUMENT;
            const std::string_view declaration=editor?std::string_view(bytes+idBytes,declarationBytes):std::string_view{};
            if(editor){
                if(!runtime::mod_options::Registry::validText(declaration,NIMBY_OPTIONS_SCHEMA_LIMIT))return NIMBY_INVALID_ARGUMENT;
                // Validate copied UTF-8, exact fields, duplicate keys, depth and
                // static translation references before reserving a capability.
                (void)engine::train_editor::Messages(declaration);
            }
            auto& queue=releases();const auto token=claims().reserve();if(!token)return NIMBY_RESOURCE_LIMIT;
            if(!queue.track(token)){claims().forget(token);return NIMBY_RESOURCE_LIMIT;}
            struct Reservation {
                OwnerReleases& queue;uint64_t token;bool committed=false;
                ~Reservation(){if(!committed)queue.retire(token);}
            } reservation{queue,token};
            // A mod may reuse another mod's visible identifier, but cannot
            // reuse its native policy identity. Prefix uniqueness comes from
            // the parent's monotone capability and remains before truncation.
            std::string bridgeId="NRF.TrainLength."+std::to_string(token)+".";
            bridgeId.append(bytes,std::min(idBytes,size_t{128}-bridgeId.size()));
            uint64_t nativeOwner{};
            const auto status=editor?call<RegisterEditor>("NimbyTrainEditor_RegisterV2",bridgeId.c_str(),
                static_cast<uint32_t>(request.args[0]),declaration.data(),static_cast<uint32_t>(declaration.size()),&nativeOwner):
                call<Register>("NimbyTrainLength_Register",bridgeId.c_str(),static_cast<uint32_t>(request.args[0]),&nativeOwner);
            if(nativeOwner)claims().bind(token,nativeOwner);
            if(status!=NIMBY_OK||!nativeOwner)return status==NIMBY_OK?NIMBY_INVALID_BINARY:status;
            owners.trainLength=token;reply.args[0]=token;reservation.committed=true;
            char text[320]{};std::snprintf(text,sizeof text,
                "Train editor declaration registered: ipcOwner=%llu nativeOwner=%llu maximum_m=%llu messageOwner=%s declarationBytes=%llu",
                static_cast<unsigned long long>(token),static_cast<unsigned long long>(nativeOwner),
                static_cast<unsigned long long>(request.args[0]),editor?"mod":"sdk",
                static_cast<unsigned long long>(declarationBytes));
            nimby::detail::diagnostics::write("sdk","INFO",text);return NIMBY_OK;
        }
        if(request.operation!=updateOperation&&request.operation!=removeOperation)return NIMBY_INVALID_ARGUMENT;
        if(!request.args[0]||request.args[0]!=owners.trainLength)return NIMBY_INVALID_HANDLE;
        if(!request.data.empty()||!unusedArguments(request,request.operation==updateOperation?2:1))return NIMBY_INVALID_ARGUMENT;
        const auto nativeOwner=claims().nativeOwner(owners.trainLength);if(!nativeOwner)return NIMBY_INVALID_HANDLE;
        if(request.operation==updateOperation){
            if(!validMaximum(request.args[1]))return NIMBY_INVALID_ARGUMENT;
            return call<Update>("NimbyTrainLength_Update",nativeOwner,static_cast<uint32_t>(request.args[1]));
        }
        const auto status=call<Remove>("NimbyTrainLength_Remove",nativeOwner);
        if(status==NIMBY_OK||status==NIMBY_INVALID_HANDLE){
            claims().bind(owners.trainLength,0);releases().retire(owners.trainLength);owners.trainLength=0;
        }
        return status;
    });
}
void cleanupTrainLength(Owners& owners)noexcept {
    if(!owners.trainLength)return;
    try{retirementLog("owner-channel-retired",owners.trainLength,claims().nativeOwner(owners.trainLength),NIMBY_OK,true);}
    catch(...){}
    releases().retire(owners.trainLength);owners.trainLength=0;
}
void* trainLengthSymbol(const char* name)noexcept {
    if(!name)return nullptr;
    if(std::strcmp(name,"NimbyTrainLength_Register")==0)return std::bit_cast<void*>(&registerLimit);
    if(std::strcmp(name,"NimbyTrainEditor_RegisterV2")==0)return std::bit_cast<void*>(&registerEditor);
    if(std::strcmp(name,"NimbyTrainLength_Update")==0)return std::bit_cast<void*>(&updateLimit);
    if(std::strcmp(name,"NimbyTrainLength_Remove")==0)return std::bit_cast<void*>(&removeLimit);
    return nullptr;
}
}

extern "C" uint32_t __cdecl NimbyInternal_TrainLengthRegister(const char* modId,uint32_t maximumMeters,uint64_t* owner)noexcept {
    return nimby::mod_host::registerLimit(modId,maximumMeters,owner);
}
extern "C" uint32_t __cdecl NimbyInternal_TrainEditorRegister(const char* modId,uint32_t maximumMeters,
    const char* declaration,uint32_t bytes,uint64_t* owner)noexcept {
    return nimby::mod_host::registerEditor(modId,maximumMeters,declaration,bytes,owner);
}
extern "C" uint32_t __cdecl NimbyInternal_TrainLengthUpdate(uint64_t owner,uint32_t maximumMeters)noexcept {
    return nimby::mod_host::updateLimit(owner,maximumMeters);
}
extern "C" uint32_t __cdecl NimbyInternal_TrainLengthRemove(uint64_t owner)noexcept {
    return nimby::mod_host::removeLimit(owner);
}
