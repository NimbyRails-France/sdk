#include "loader/mods.h"
#include <bit>
#include <algorithm>
#include <fstream>
#ifndef _WIN32
#include <nimby/detail/native_library.hpp>
#endif

namespace nimby::loader {
void Mods::start(const std::filesystem::path& directory, Log log) {
    if (!scanned_) {
        scanned_ = true;
#ifdef _WIN32
        const auto directoryName=directory.wstring();
        WIN32_FIND_DATAW data{};
        HANDLE search = FindFirstFileW((directoryName + L"\\*").c_str(), &data);
        if (search != INVALID_HANDLE_VALUE) {
            std::vector<std::wstring> paths;
            do {
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || data.cFileName[0] == L'.') continue;
                const auto folder = directoryName + L"\\" + data.cFileName;
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
#else
        namespace fs=std::filesystem;
        namespace native=nimby::detail::native;
        std::vector<fs::path> paths;
        std::error_code error;
        for(fs::directory_iterator cursor(directory,error),end;!error&&cursor!=end;cursor.increment(error)) {
            const auto& folder=*cursor;
            if(!folder.is_directory(error)||folder.path().filename().string().starts_with('.'))continue;
            std::ifstream input(folder.path()/"nrf-mod.ini");
            if(!input)continue;
            std::string line,name;bool section=false,duplicate=false;
            auto trim=[](std::string value){
                const auto begin=value.find_first_not_of(" \t\r");
                if(begin==std::string::npos)return std::string{};
                return value.substr(begin,value.find_last_not_of(" \t\r")-begin+1);
            };
            while(std::getline(input,line)) {
                line=trim(line);
                if(line.empty()||line.front()==';'||line.front()=='#')continue;
                if(line.front()=='['){section=line=="[NRFMod]";continue;}
                const auto equal=line.find('=');
                if(section&&equal!=std::string::npos&&trim(line.substr(0,equal))=="library") {
                    if(!name.empty())duplicate=true;
                    name=trim(line.substr(equal+1));
                }
            }
            const bool valid=!duplicate&&name.size()>3&&name.size()<200&&name.ends_with(".so")&&
                name.front()!='.'&&name.find("..")==std::string::npos&&
                name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-")==std::string::npos;
            if(!valid){log("ERROR: invalid NRF mod library manifest");continue;}
            paths.push_back(folder.path()/name);
        }
        if(error)log("ERROR: cannot scan NRF mod directory");
        std::sort(paths.begin(),paths.end());
        for(const auto& path:paths) {
            native::Module handle{};
            try {
                handle=native::load(path);
                auto start=std::bit_cast<Entry>(native::symbol(handle,"NRFMod_StartV1"));
                auto stop=std::bit_cast<Entry>(native::symbol(handle,"NRFMod_StopV1"));
                if(!start||!stop){native::unload(handle);log("ERROR: NRF mod V1 exports missing");continue;}
                modules_.push_back({handle,start,stop,false});
            }catch(...){native::unload(handle);log("ERROR: NRF mod library or dependency could not be loaded");}
        }
#endif
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
