#pragma once
// Platform implementation. Include detail/native_library.hpp from consumers.
#include <nimby/detail/platform/windows/system.hpp>
#include <array>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <cstring>
#include <bit>
namespace nimby::detail::native {
using Module=HMODULE;
inline uint32_t modHostTarget() noexcept {
    using Target=uint32_t(__cdecl*)();
    const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
    const auto target=sdk?std::bit_cast<Target>(GetProcAddress(sdk,"NimbyInternal_ModHostTarget")):nullptr;
    return target?target():0;
}
inline void modHostPulse(uint32_t phase) noexcept {
    using Pulse=void(__cdecl*)(uint32_t);
    const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
    const auto pulse=sdk?std::bit_cast<Pulse>(GetProcAddress(sdk,"NimbyInternal_ModHostPulse")):nullptr;
    if(pulse)pulse(phase);
}
inline Module loadWithSearchFlags(const std::filesystem::path& path,bool pin,DWORD flags) {
    auto module=LoadLibraryExW(std::filesystem::absolute(path).c_str(),nullptr,
        flags);
    if(!module)throw std::runtime_error("Cannot load native library");
    if(pin){
        HMODULE pinned{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(module),&pinned)) {
            FreeLibrary(module);throw std::runtime_error("Cannot retain native runtime");
        }
    }
    return module;
}
inline Module load(const std::filesystem::path& path,bool pin=false) {
    return loadWithSearchFlags(path,pin,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
}
// Diagnostic mods use a stricter dependency search than general native clients.
inline Module loadIsolated(const std::filesystem::path& path) {
    return loadWithSearchFlags(path,false,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
}
inline Module remoteUi(){return reinterpret_cast<Module>(static_cast<intptr_t>(-2));}
inline auto symbol(Module module,const char* name){
    if(module!=remoteUi())return GetProcAddress(module,name);
    using Symbol=void*(__cdecl*)(const char*);
    const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
    const auto symbol=sdk?std::bit_cast<Symbol>(GetProcAddress(sdk,"NimbyInternal_ModHostUiSymbol")):nullptr;
    return symbol?reinterpret_cast<FARPROC>(symbol(name)):nullptr;
}
inline Module existing(const char* name){
    if(modHostTarget()&&name&&std::strcmp(name,"NimbySignalUiBridge-experimental-v1.dll")==0)return remoteUi();
    HMODULE module{};return GetModuleHandleExA(0,name,&module)?module:nullptr;
}
inline void unload(Module module){if(module&&module!=remoteUi())FreeLibrary(module);}
inline std::filesystem::path modulePath(const void* address) {
    HMODULE module{};
    if(address&&!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(address),&module))throw std::runtime_error("Cannot locate native module");
    std::array<wchar_t,32768> buffer{};
    const auto length=GetModuleFileNameW(module,buffer.data(),static_cast<DWORD>(buffer.size()));
    if(!length||length>=buffer.size())throw std::runtime_error("Cannot locate native module directory");
    return std::filesystem::path(buffer.data());
}
}
