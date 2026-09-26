#include <loader/mods.h>
#include <platform/mod_library.h>
#include <nimby/detail/platform/windows/system.hpp>
#include <bit>
namespace nimby::platform {
std::string_view mod_library_suffix() noexcept{return ".dll";}
bool mod_library_case_insensitive() noexcept{return true;}
}
namespace nimby::loader {
// Only native loading and export resolution remain platform-specific.
void Mods::load(const std::filesystem::path& path,Log log) {
    const auto utf8=path.u8string();
    const std::string name(utf8.begin(),utf8.end());
    log(("Loading mod library: "+name).c_str());
    HMODULE module=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!module){log(("ERROR: NRF mod library or dependency could not be loaded: "+name+" win32="+std::to_string(GetLastError())).c_str());return;}
    auto start=std::bit_cast<Entry>(GetProcAddress(module,"NRFMod_StartV1"));
    auto stop=std::bit_cast<Entry>(GetProcAddress(module,"NRFMod_StopV1"));
    if(!start||!stop){log(("ERROR: NRF mod V1 exports missing: "+name).c_str());FreeLibrary(module);return;}
    modules_.push_back({module,start,stop,false,name});
    log(("OK: mod library loaded, V1 exports resolved: "+name).c_str());
}
}
