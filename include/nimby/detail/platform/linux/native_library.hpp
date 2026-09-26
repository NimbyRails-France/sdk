#pragma once
// Platform implementation. Include detail/native_library.hpp from consumers.
#include <dlfcn.h>
#include <array>
#include <filesystem>
#include <stdexcept>
#include <string>
namespace nimby::detail::native {
using Module=void*;
inline Module load(const std::filesystem::path& path,bool pin=false) {
    auto module=dlopen(std::filesystem::absolute(path).c_str(),RTLD_NOW|RTLD_LOCAL|(pin?RTLD_NODELETE:0));
    if(!module)throw std::runtime_error(std::string("Cannot load native library: ")+dlerror());
    return module;
}
inline Module loadIsolated(const std::filesystem::path& path){return load(path);}
inline auto symbol(Module module,const char* name){return dlsym(module,name);}
inline Module existing(const char* name){return dlopen(name,RTLD_NOW|RTLD_LOCAL|RTLD_NOLOAD);}
inline void unload(Module module){if(module)dlclose(module);}
inline std::filesystem::path modulePath(const void* address) {
    if(!address)return std::filesystem::read_symlink("/proc/self/exe");
    Dl_info info{};
    if(!dladdr(address,&info)||!info.dli_fname)throw std::runtime_error("Cannot locate native module");
    return std::filesystem::absolute(info.dli_fname);
}
}
