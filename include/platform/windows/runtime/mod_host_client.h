#pragma once
#include <windows.h>
#include <nimby/detail/observation.h>
#include <bit>
#include <cstdint>
#include <optional>
#include <string>

namespace nimby::mod_host::client {
inline void wakeLocal() noexcept {
    using Function=void(__cdecl*)();
    const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
    const auto function=sdk?std::bit_cast<Function>(GetProcAddress(sdk,"NimbyInternal_ModHostWakeLocal")):nullptr;
    if(function)function();
}
// Dynamic lookup also lets the standalone diagnostic clients use these
// transports without linking the isolated-process runtime.
inline uint32_t target() noexcept {
    using Function=uint32_t(__cdecl*)();
    const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
    const auto function=sdk?std::bit_cast<Function>(GetProcAddress(sdk,"NimbyInternal_ModHostTarget")):nullptr;
    return function?function():0;
}
inline uint32_t call(uint32_t operation,void* output,uint32_t capacity,uint32_t& written) noexcept {
    using Function=uint32_t(__cdecl*)(uint32_t,uint64_t*,const void*,uint32_t,void*,uint32_t,uint32_t*);
    const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
    const auto function=sdk?std::bit_cast<Function>(GetProcAddress(sdk,"NimbyInternal_ModHostCall")):nullptr;
    if(!function)return NIMBY_HOOKS_UNAVAILABLE;
    uint64_t arguments[8]{};
    return function(operation,arguments,nullptr,0,output,capacity,&written);
}
inline uint32_t ensureBridge(uint32_t pid,uint32_t operation) noexcept {
    if(target()!=pid)return NIMBY_INVALID_ARGUMENT;
    uint32_t written{};return call(operation,nullptr,0,written);
}
inline std::optional<std::string> language() {
    char text[33]{};uint32_t written{};
    if(call(6,text,sizeof text,written)!=NIMBY_OK||written<2||written>32)return {};
    for(uint32_t i=0;i<written;++i)if(!((text[i]>='a'&&text[i]<='z')||(text[i]>='A'&&text[i]<='Z')||
        (text[i]>='0'&&text[i]<='9')||text[i]=='-'||text[i]=='_'))return {};
    return std::string(text,written);
}
}
