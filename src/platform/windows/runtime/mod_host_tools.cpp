#include "platform/windows/mod_host_protocol.h"
#include "engine/binary_identity.h"
#include "platform/windows/game_language.h"
#include "platform/windows/bridge_installation.h"
#include <windows.h>
#include <array>
#include <atomic>
#include <bit>
#include <cstring>
#include <filesystem>
#include <mutex>

namespace nimby::mod_host {
namespace {
struct BootstrapState {std::mutex mutex;std::atomic<bool> ready{false};};
bool readGame(void*,uint64_t address,void* output,size_t size) {
    SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),output,size,&got)&&got==size;
}
uint32_t ensure(BootstrapState& state,const wchar_t* filename) {
    if(state.ready.load(std::memory_order_acquire))return NIMBY_OK;
    // A peer installing the same native bridge must not block this channel.
    std::unique_lock lock(state.mutex,std::try_to_lock);
    if(!lock.owns_lock())return NIMBY_RESOURCE_LIMIT;
    if(state.ready.load(std::memory_order_relaxed))return NIMBY_OK;
    platform::windows::BridgeInstallation installation;
    if(!installation)return installation.status();
    HMODULE sdk{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&ensure),&sdk))return NIMBY_IO_ERROR;
    std::array<wchar_t,32768> sdkPath{},residentPath{};
    auto length=GetModuleFileNameW(sdk,sdkPath.data(),static_cast<DWORD>(sdkPath.size()));
    if(!length||length>=sdkPath.size())return NIMBY_IO_ERROR;
    const auto path=std::filesystem::path(sdkPath.data()).parent_path()/filename;
    NimbyBinaryInfo expected{};
    if(engine::identify(path.c_str(),expected)!=NIMBY_OK)return NIMBY_DATA_UNAVAILABLE;
    HMODULE module{};
    if(GetModuleHandleExW(0,filename,&module)) {
        length=GetModuleFileNameW(module,residentPath.data(),static_cast<DWORD>(residentPath.size()));
        NimbyBinaryInfo resident{};
        if(!length||length>=residentPath.size()||engine::identify(residentPath.data(),resident)!=NIMBY_OK||
           std::strcmp(expected.sha256,resident.sha256)) {
            FreeLibrary(module);return NIMBY_INVALID_BINARY;
        }
    }else module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module)return NIMBY_IO_ERROR;
    struct Release {HMODULE module;~Release(){FreeLibrary(module);}} release{module};
    using Bootstrap=DWORD(WINAPI*)(void*);
    const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
    if(!bootstrap)return NIMBY_INVALID_BINARY;
    const auto status=bootstrap(nullptr);
    if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED)return status;
    state.ready.store(true,std::memory_order_release);return NIMBY_OK;
}
}
uint32_t dispatchTools(const Request& request,Reply& reply,Owners&) {
    if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
    for(const auto argument:request.args)if(argument)return NIMBY_INVALID_ARGUMENT;
    static BootstrapState clock,construction;
    if(request.operation==4)return ensure(clock,L"NimbyRailsFranceClockBridge-0.7.1.dll");
    if(request.operation==5)return ensure(construction,L"NimbyConstructionBridge-experimental-v1.dll");
    if(request.operation==6) {
        const auto language=platform::windows::gameLanguage(readGame,nullptr,reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr)));
        if(!language)return NIMBY_DATA_UNAVAILABLE;
        output(reply,language->data(),language->size());return NIMBY_OK;
    }
    return NIMBY_INVALID_ARGUMENT;
}
}
