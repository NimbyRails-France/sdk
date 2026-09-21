#pragma once
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif
#include <cstdint>
#include <string>
#include <vector>

namespace nimby::loader {
// V1: DWORD WINAPI NRFMod_StartV1(void* reserved=nullptr), NRFMod_StopV1(void*).
// Return 0 on success, 4 when already started. No work in DllMain.
// Called after SDK initialization and before SDK shutdown, outside loader lock.
class Mods {
#ifdef _WIN32
    using Entry = uint32_t (WINAPI*)(void*);
#else
    using Entry = uint32_t (*)(void*);
#endif
#ifdef _WIN32
    using Handle=HMODULE;
#else
    using Handle=void*;
#endif
    struct Module { Handle handle; Entry start, stop; bool active; };
    std::vector<Module> modules_;
    bool scanned_ = false;
public:
    using Log = void (*)(const char*);
    void start(const std::filesystem::path& directory, Log log);
    bool stop(Log log);
};
}
