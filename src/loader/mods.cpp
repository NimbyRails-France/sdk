#include "loader/mods.h"
#include <loader/manifest.h>
#include <platform/mod_library.h>
#include <algorithm>
#include <fstream>

namespace nimby::loader {
// Discovery and ordering are identical on both hosts. Filesystem failures are
// reported through error_code; a module is loaded only after its full manifest
// has been validated. Runtime references remain resident after stop.
void Mods::scan(const std::filesystem::path& directory, Log log) {
    namespace fs=std::filesystem;
    std::vector<fs::path> paths;
    std::error_code error;
    for(fs::directory_iterator cursor(directory,error),end;!error&&cursor!=end;cursor.increment(error)) {
        const auto& folder=*cursor;
        if(!folder.is_directory(error)||folder.path().filename().native().starts_with(fs::path::value_type('.')))continue;
        std::ifstream input(folder.path()/"nrf-mod.ini",std::ios::binary);
        if(!input)continue;
        ManifestError manifestError{};
        const auto name=manifest_library(input,platform::mod_library_suffix(),platform::mod_library_case_insensitive(),&manifestError);
        if(name.empty()){log((std::string("ERROR: ")+manifest_error_message(manifestError)).c_str());continue;}
        paths.push_back(folder.path()/name);
    }
    if(error)log("ERROR: cannot scan NRF mod directory");
    std::sort(paths.begin(),paths.end());
    modules_.reserve(paths.size());
    log(("Mod discovery: manifests="+std::to_string(paths.size())).c_str());
    for(const auto& path:paths)load(path,log);
}
void Mods::start(const std::filesystem::path& directory, Log log) {
    if (!scanned_) {
        scanned_ = true;
        scan(directory,log);
    }
    for (auto& module : modules_) {
        if (module.active) continue;
        const auto status = module.start(nullptr);
        module.active = status == 0 || status == 4;
        log((std::string(module.active ? "OK: NRF mod started: " : "ERROR: NRF mod start failed: ")+module.name+" status="+std::to_string(status)).c_str());
    }
}
bool Mods::stop(Log log) {
    bool success = true;
    for (auto it = modules_.rbegin(); it != modules_.rend(); ++it) {
        if (!it->active) continue;
        const auto status=it->stop(nullptr);
        if (status == 0) { it->active = false; log(("OK: NRF mod stopped: "+it->name).c_str()); }
        else { success = false; log(("ERROR: NRF mod stop failed; SDK retained: "+it->name+" status="+std::to_string(status)).c_str()); }
    }
    // References remain alive until process exit: no hot unload of mod code.
    return success;
}
}
