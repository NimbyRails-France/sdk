#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/sdk.h>
#include <nimby/detail/observation.h>
#include "engine/binary_identity.h"
#include <windows.h>
#include <array>
#include <bit>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <atomic>
#include <nimby/detail/mod_host.h>
#include <platform/windows/bridge_installation.h>

// Same-process SDK loader. Called outside DllMain, before registering a mod's
// panel. Never searches for another game or loads from the working directory.
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_EnsureSignalUiBridge() noexcept {
    if(NimbyInternal_ModHostTarget()){
        uint64_t arguments[8]{};uint32_t written{};
        return NimbyInternal_ModHostCall(1,arguments,nullptr,0,nullptr,0,&written);
    }
    try {
        static std::mutex mutex;
        static std::atomic<bool> ready{false};
        if(ready.load(std::memory_order_acquire))return NIMBY_OK;
        std::unique_lock lock(mutex,std::try_to_lock);
        if(!lock.owns_lock())return NIMBY_RESOURCE_LIMIT;
        // Successful bootstrap pins the bridge for the life of this game.
        // Repeated registrations do not rehash either binary under this lock.
        if(ready.load(std::memory_order_relaxed))return NIMBY_OK;
        nimby::platform::windows::BridgeInstallation installation;
        if(!installation)return installation.status();
        std::array<wchar_t,32768> executable{},sdkPath{},residentPath{};
        auto length=GetModuleFileNameW(nullptr,executable.data(),static_cast<DWORD>(executable.size()));
        if(!length||length>=executable.size())return NIMBY_IO_ERROR;
        NimbyBinaryInfo game{};
        if(nimby::engine::identify(executable.data(),game)!=NIMBY_OK||!game.recognized_research_build)return NIMBY_INVALID_BINARY;
        // The probe patches the same entry point and is pinned until game exit.
        if(GetModuleHandleW(L"NimbySignalUiProbe-v1.dll"))return NIMBY_HOOKS_UNAVAILABLE;
        HMODULE sdk{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&NimbyInternal_EnsureSignalUiBridge),&sdk))return NIMBY_IO_ERROR;
        length=GetModuleFileNameW(sdk,sdkPath.data(),static_cast<DWORD>(sdkPath.size()));
        if(!length||length>=sdkPath.size())return NIMBY_IO_ERROR;
        constexpr auto filename=L"NimbySignalUiBridge-experimental-v1.dll";
        const auto path=std::filesystem::path(sdkPath.data()).parent_path()/filename;
        NimbyBinaryInfo expected{};
        if(nimby::engine::identify(path.c_str(),expected)!=NIMBY_OK)return NIMBY_DATA_UNAVAILABLE;
        HMODULE module{};
        if(GetModuleHandleExW(0,filename,&module)){
            length=GetModuleFileNameW(module,residentPath.data(),static_cast<DWORD>(residentPath.size()));
            NimbyBinaryInfo resident{};
            if(!length||length>=residentPath.size()||
               nimby::engine::identify(residentPath.data(),resident)!=NIMBY_OK||
               std::strcmp(expected.sha256,resident.sha256)){
                FreeLibrary(module);return NIMBY_INVALID_BINARY;
            }
        }else module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
        if(!module)return NIMBY_IO_ERROR;
        struct Release {HMODULE module;~Release(){FreeLibrary(module);}} release{module};
        using Bootstrap=DWORD (WINAPI*)(void*);
        const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
        if(!bootstrap)return NIMBY_INVALID_BINARY;
        const auto status=bootstrap(nullptr);
        if(status==NIMBY_OK||status==NIMBY_ALREADY_INITIALIZED){ready.store(true,std::memory_order_release);return NIMBY_OK;}
        return status;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
