#pragma once
#include "live_state.h"
#include <map>
#include <string>
#include <vector>
namespace nimby::engine {
struct SignalTextureFile { int32_t source{}; uint64_t hash{}; std::string mod,relative_path; };
struct SignalTextureSet { uint64_t hash{}; std::string name; std::vector<SignalTextureFile> files; };
struct SignalTextureCatalog {
    std::map<uint64_t,SignalTextureSet> sets;
    std::vector<uint64_t> defaults;
    std::wstring local_mod_root;
};
bool read_signal_texture_catalog(ReadMemory,void*,const LiveState&,bool,SignalTextureCatalog&) noexcept;
// Matches native atlas lookup and selector clamping. GPU/resource-load fallback
// is not reproduced; callers must report a missing image instead of guessing.
const SignalTextureSet* select_signal_textures(const SignalTextureCatalog&,int kind,uint64_t hash) noexcept;
}
