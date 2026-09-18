#include "loader/mods.h"
#include <bit>
#include <algorithm>

namespace nimby::loader {
void Mods::start(const std::wstring& directory, Log log) {
    if (!scanned_) {
        scanned_ = true;
        WIN32_FIND_DATAW data{};
        HANDLE search = FindFirstFileW((directory + L"\\*").c_str(), &data);
        if (search != INVALID_HANDLE_VALUE) {
            std::vector<std::wstring> paths;
            do {
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || data.cFileName[0] == L'.') continue;
                const auto folder = directory + L"\\" + data.cFileName;
                wchar_t library[256]{};
                GetPrivateProfileStringW(L"NRFMod", L"library", L"", library, 256,
                    (folder + L"\\nrf-mod.ini").c_str());
                const std::wstring name = library;
                const bool valid = name.size() > 4 && name.size() < 200 &&
                    _wcsicmp(name.c_str() + name.size() - 4, L".dll") == 0 &&
                    name.find_first_not_of(L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-") == std::wstring::npos &&
                    name.find(L"..") == std::wstring::npos && name.front() != L'.';
                if (!valid) { log("ERROR: invalid NRF mod library manifest"); continue; }
                paths.push_back(folder + L"\\" + name);
            } while (FindNextFileW(search, &data));
            FindClose(search);
            std::sort(paths.begin(), paths.end());
            for (const auto& path : paths) {
                HMODULE module = LoadLibraryExW(path.c_str(), nullptr,
                    LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
                if (!module) { log("ERROR: NRF mod DLL or dependency could not be loaded"); continue; }
                auto start = std::bit_cast<Entry>(GetProcAddress(module, "NRFMod_StartV1"));
                auto stop = std::bit_cast<Entry>(GetProcAddress(module, "NRFMod_StopV1"));
                if (!start || !stop) {
                    log("ERROR: NRF mod V1 exports missing"); FreeLibrary(module); continue;
                }
                modules_.push_back({module, start, stop, false});
            }
        }
    }
    for (auto& module : modules_) {
        if (module.active) continue;
        const auto status = module.start(nullptr);
        module.active = status == 0 || status == 4;
        log(module.active ? "OK: NRF mod started" : "ERROR: NRF mod start failed");
    }
}
bool Mods::stop(Log log) {
    bool success = true;
    for (auto it = modules_.rbegin(); it != modules_.rend(); ++it) {
        if (!it->active) continue;
        if (it->stop(nullptr) == 0) { it->active = false; log("OK: NRF mod stopped"); }
        else { success = false; log("ERROR: NRF mod stop failed; SDK retained"); }
    }
    // References remain alive until process exit: no hot unload of mod code.
    return success;
}
}
