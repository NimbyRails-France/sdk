#pragma once
#include <filesystem>
#include <nimby/detail/platform/mod_abi.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace nimby::loader {
// V1: DWORD WINAPI NRFMod_StartV1(void* reserved=nullptr), NRFMod_StopV1(void*).
// Return 0 on success, 4 when already started. No work in DllMain.
// Called after SDK initialization and before SDK shutdown, outside loader lock.
class Mods {
    using Entry = uint32_t (NRF_CALL*)(void*);
    // Opaque retained module token; platform scanning owns native handle types.
    using Handle = void*;
    struct Module { Handle handle; Entry start, stop; bool active; std::string name{}; };
    std::vector<Module> modules_;
    bool scanned_ = false;
public:
    using Log = void (*)(const char*);
    void start(const std::filesystem::path& directory, Log log);
    bool stop(Log log);
private:
    void load(const std::filesystem::path& path, Log log);
    void scan(const std::filesystem::path& directory, Log log);
};
}
