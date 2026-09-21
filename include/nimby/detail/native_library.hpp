#pragma once
#include <filesystem>
#include <stdexcept>
#include <array>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace nimby::detail::native {
#ifdef _WIN32
using Module=HMODULE;
inline Module load(const std::filesystem::path& path,bool pin=false) {
    auto module=LoadLibraryExW(std::filesystem::absolute(path).c_str(),nullptr,
        LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
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
#else
using Module=void*;
inline Module load(const std::filesystem::path& path,bool pin=false) {
    auto module=dlopen(std::filesystem::absolute(path).c_str(),RTLD_NOW|RTLD_LOCAL|(pin?RTLD_NODELETE:0));
    if(!module)throw std::runtime_error(std::string("Cannot load native library: ")+dlerror());
    return module;
}
inline auto symbol(Module module,const char* name){return dlsym(module,name);}
inline Module existing(const char* name){return dlopen(name,RTLD_NOW|RTLD_LOCAL|RTLD_NOLOAD);}
inline void unload(Module module){if(module)dlclose(module);}
inline std::filesystem::path modulePath(const void* address) {
    if(!address)return std::filesystem::read_symlink("/proc/self/exe");
    Dl_info info{};
    if(!dladdr(address,&info)||!info.dli_fname)throw std::runtime_error("Cannot locate native module");
    return std::filesystem::absolute(info.dli_fname);
}
#endif
}
