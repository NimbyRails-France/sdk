#pragma once
// Platform implementation. Include detail/native_library.hpp from consumers.
#include <nimby/detail/platform/windows/system.hpp>
#include <array>
#include <filesystem>
#include <stdexcept>
#include <string>
namespace nimby::detail::native {
using Module=HMODULE;
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
inline auto symbol(Module module,const char* name){return GetProcAddress(module,name);}
inline Module existing(const char* name){HMODULE module{};return GetModuleHandleExA(0,name,&module)?module:nullptr;}
inline void unload(Module module){if(module)FreeLibrary(module);}
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
