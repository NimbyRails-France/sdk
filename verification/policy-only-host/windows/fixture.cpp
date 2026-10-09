#define NIMBY_SDK_BUILD
#include "../fixture.h"
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/train_length.h>
#include <nimby/detail/observation.h>
#include <nimby/detail/vendor/json.hpp>
#include <nimby/detail/translations.hpp>
#include <engine/train_editor_messages.h>
#include <windows.h>
#include <bit>
#include <cstring>
#include <mutex>
#include <optional>
#include <string>

namespace {
using namespace policy_host_fixture;
using Json=nlohmann::json;
std::mutex mutex;
Stats stats;
std::uint32_t mode{};
HANDLE remoteAction{},localAction{};
constexpr std::uint64_t optionToken=101,limitToken=202;

std::uint32_t optionAdd(const char* bytes,std::uint32_t count,std::uint64_t* owner) {
    try {
        std::lock_guard guard(mutex);
        *owner=0;
        const auto schema=Json::parse(bytes,bytes+count);
        if(stats.optionOwner||schema.at("id")!="bc-train-super-long"||schema.at("fields").size()!=1)
            return NIMBY_INVALID_ARGUMENT;
        const auto& field=schema.at("fields").front();
        if(field.at("id")!="maxLengthMeters"||field.at("kind")!=1||field.at("default")!="850"||
           field.at("minimum")!=1||field.at("maximum")!=10000)
            return NIMBY_INVALID_ARGUMENT;
        ++stats.sequence;++stats.optionAdds;stats.optionOwner=*owner=optionToken;
        return NIMBY_OK;
    }catch(...){return NIMBY_INVALID_ARGUMENT;}
}
std::uint32_t optionRemove(std::uint64_t owner) {
    std::lock_guard guard(mutex);
    if(owner!=stats.optionOwner||!owner)return NIMBY_INVALID_HANDLE;
    ++stats.sequence;++stats.optionRemoves;stats.optionOwner=0;return NIMBY_OK;
}
std::uint32_t optionRead(std::uint64_t owner,std::uint64_t known,char* out,std::uint32_t capacity,
                         std::uint32_t* written,std::uint64_t* revision) {
    try {
        std::lock_guard guard(mutex);
        *written=0;*revision=0;
        ++stats.optionReads;stats.readSequence=++stats.sequence;stats.lastKnownRevision=known;
        if(!owner||owner!=stats.optionOwner)return NIMBY_INVALID_HANDLE;
        if(mode&FailPreferenceRead)return NIMBY_DATA_UNAVAILABLE;
        *revision=stats.revision;
        if(known==stats.revision)return NIMBY_OK;
        const auto payload=Json({{"values",Json::array({std::to_string(stats.savedMeters)})},
                                 {"events",Json::array()}}).dump();
        if(payload.size()>capacity)return NIMBY_RESOURCE_LIMIT;
        *written=static_cast<std::uint32_t>(payload.size());
        std::memcpy(out,payload.data(),payload.size());return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
std::uint32_t optionDiscard(std::uint64_t owner) {
    std::lock_guard guard(mutex);
    return owner&&owner==stats.optionOwner?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
template<class Function> void* address(Function function) {
    return std::bit_cast<void*>(function);
}
std::uint32_t registerLimit(const char* id,std::uint32_t maximum,std::uint64_t* owner) {
    ++stats.limitAdds;stats.registerSequence=++stats.sequence;stats.registeredMeters=maximum;*owner=0;
    if(mode&FailRegistration)return NIMBY_HOOKS_UNAVAILABLE;
    if(!id||std::strcmp(id,"bc-train-super-long")!=0||stats.limitOwner||!stats.optionOwner||maximum!=stats.savedMeters)
        return NIMBY_INVALID_ARGUMENT;
    stats.limitOwner=*owner=limitToken;stats.publishedMeters=maximum;return NIMBY_OK;
}
}

extern "C" __declspec(dllexport) std::uint32_t Test_Configure(std::uint32_t saved,std::uint32_t failureMode) noexcept {
    std::lock_guard guard(mutex);
    if(stats.optionOwner||stats.limitOwner||saved<1||saved>10000)return NIMBY_INVALID_ARGUMENT;
    if(!remoteAction)remoteAction=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if(!localAction)localAction=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if(!remoteAction||!localAction)return NIMBY_INTERNAL_ERROR;
    ResetEvent(remoteAction);ResetEvent(localAction);
    stats={};stats.savedMeters=saved;stats.revision=1;mode=failureMode;return NIMBY_OK;
}
extern "C" __declspec(dllexport) std::uint32_t Test_Snapshot(Stats* out) noexcept {
    if(!out)return NIMBY_INVALID_ARGUMENT;
    std::lock_guard guard(mutex);*out=stats;return NIMBY_OK;
}
extern "C" __declspec(dllexport) std::uint32_t Test_Change(std::uint32_t saved,std::uint32_t event) noexcept {
    std::lock_guard guard(mutex);
    if(!stats.optionOwner||saved<1||saved>10000||event>2)return NIMBY_INVALID_ARGUMENT;
    stats.savedMeters=saved;++stats.revision;
    if(event&&!SetEvent(event==1?remoteAction:localAction))return NIMBY_INTERNAL_ERROR;
    return NIMBY_OK;
}
extern "C" __declspec(dllexport) std::uint32_t Test_BusyUpdates(std::uint32_t count) noexcept {
    std::lock_guard guard(mutex);stats.busyUpdates=count;return NIMBY_OK;
}
extern "C" __declspec(dllexport) std::uint32_t __cdecl NimbyInternal_ModHostTarget() noexcept {
    // A real isolated mod reports its parent game PID. Here it reports only
    // this test PID, enabling hostedByGame without discovering a real game.
    return GetCurrentProcessId();
}
extern "C" __declspec(dllexport) void __cdecl NimbyInternal_ModHostPulse(std::uint32_t) noexcept {}
extern "C" __declspec(dllexport) void __cdecl NimbyInternal_ModHostStage(std::uint32_t,std::uint64_t) noexcept {}
extern "C" __declspec(dllexport) std::uint32_t __cdecl NimbyInternal_ModHostActionWaits(
    std::uint64_t* remote,std::uint64_t* local) noexcept {
    *remote=*local=0;
    std::lock_guard guard(mutex);
    if(mode&MissingActionWait)return NIMBY_RESOURCE_LIMIT;
    HANDLE first{},second{};const auto process=GetCurrentProcess();
    if(!DuplicateHandle(process,remoteAction,process,&first,SYNCHRONIZE,FALSE,0))return NIMBY_IO_ERROR;
    if(!DuplicateHandle(process,localAction,process,&second,SYNCHRONIZE,FALSE,0)){
        CloseHandle(first);return NIMBY_IO_ERROR;
    }
    *remote=reinterpret_cast<std::uintptr_t>(first);*local=reinterpret_cast<std::uintptr_t>(second);
    return NIMBY_OK;
}
extern "C" __declspec(dllexport) void* __cdecl NimbyInternal_ModHostUiSymbol(const char* name) noexcept {
    if(!name)return nullptr;
    if(std::strcmp(name,"NimbyOptions_RegisterV1")==0)return address(&optionAdd);
    if(std::strcmp(name,"NimbyOptions_RemoveV1")==0)return address(&optionRemove);
    if(std::strcmp(name,"NimbyOptions_ReadV1")==0)return address(&optionRead);
    if(std::strcmp(name,"NimbyOptions_DiscardV1")==0)return address(&optionDiscard);
    // No tool/signal service is declared by BC. Optional service exports are
    // intentionally absent, so a mistakenly added callback cannot hide here.
    return nullptr;
}
extern "C" __declspec(dllexport) std::uint32_t __cdecl NimbyInternal_EnsureSignalUiBridge() noexcept {return NIMBY_OK;}
extern "C" std::uint32_t __cdecl NimbyInternal_TrainLengthRegister(const char* id,
    std::uint32_t maximum,std::uint64_t* owner) noexcept {
    std::lock_guard guard(mutex);
    return registerLimit(id,maximum,owner);
}
extern "C" std::uint32_t __cdecl NimbyInternal_TrainEditorRegister(const char* id,
    std::uint32_t maximum,const char* declaration,std::uint32_t bytes,std::uint64_t* owner) noexcept {
    try {
        std::lock_guard guard(mutex);if(!owner)return NIMBY_INVALID_ARGUMENT;*owner=0;
        if(!declaration)return NIMBY_INVALID_ARGUMENT;
        (void)nimby::engine::train_editor::Messages({declaration,bytes});
        const auto schema=Json::parse(declaration,declaration+bytes);
        const auto catalogue=schema.at("translations").get<std::string>();
        std::optional<nimby::detail::Translations> translations;
        if(!catalogue.empty())translations.emplace(catalogue);
        std::uint64_t resolved=0;
        for(const auto& message:schema.at("messages"))for(const auto* locale:{"fr","en"}){
            const auto text=nimby::detail::Translations::resolve(translations?&*translations:nullptr,
                message.get<std::string>(),locale);
            if(text.empty()||text.starts_with("[")||nimby::detail::Translations::reference(text))return NIMBY_INVALID_ARGUMENT;
            ++resolved;
        }
        ++stats.editorAdds;stats.declarationBytes=bytes;stats.resolvedMessages=resolved;
        return registerLimit(id,maximum,owner);
    }catch(...){return NIMBY_INVALID_ARGUMENT;}
}
extern "C" std::uint32_t __cdecl NimbyInternal_TrainLengthUpdate(std::uint64_t owner,std::uint32_t maximum) noexcept {
    std::lock_guard guard(mutex);++stats.sequence;++stats.limitUpdates;
    if(!owner||owner!=stats.limitOwner)return NIMBY_INVALID_HANDLE;
    if(stats.busyUpdates){--stats.busyUpdates;return NIMBY_RESOURCE_LIMIT;}
    stats.publishedMeters=maximum;return NIMBY_OK;
}
extern "C" std::uint32_t __cdecl NimbyInternal_TrainLengthRemove(std::uint64_t owner) noexcept {
    std::lock_guard guard(mutex);
    if(!owner||owner!=stats.limitOwner)return NIMBY_INVALID_HANDLE;
    ++stats.sequence;++stats.limitRemoves;stats.limitOwner=0;stats.publishedMeters=0;return NIMBY_OK;
}
extern "C" std::uint32_t __cdecl NimbyInternal_OpenProcess(std::uint32_t,std::uint32_t,NimbySession* out) noexcept {
    std::lock_guard guard(mutex);++stats.openProcessCalls;if(out)*out=0;return NIMBY_HOOKS_UNAVAILABLE;
}
extern "C" std::uint32_t __cdecl NimbyInternal_CaptureSessionSnapshot(NimbySession,NimbySnapshot* out,std::uint32_t* stage) noexcept {
    std::lock_guard guard(mutex);++stats.captureCalls;if(out)*out=0;if(stage)*stage=0;return NIMBY_HOOKS_UNAVAILABLE;
}
