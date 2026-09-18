#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace nimby::loader {
// V1: DWORD WINAPI NRFMod_StartV1(void* reserved=nullptr), NRFMod_StopV1(void*).
// Return 0 on success, 4 when already started. No work in DllMain.
// Called after SDK initialization and before SDK shutdown, outside loader lock.
class Mods {
    using Entry = DWORD (WINAPI*)(void*);
    struct Module { HMODULE handle; Entry start, stop; bool active; };
    std::vector<Module> modules_;
    bool scanned_ = false;
public:
    using Log = void (*)(const char*);
    void start(const std::wstring& directory, Log log);
    bool stop(Log log);
};
}
