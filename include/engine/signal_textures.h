#pragma once
#include "live_state.h"
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <span>
#include <string_view>
#include <set>
namespace nimby::engine {
struct SignalTextureFile { int32_t source{}; uint64_t hash{}; std::string mod,relative_path; };
struct SignalTextureSet { uint64_t hash{}; std::string name; std::vector<SignalTextureFile> files; };
struct SignalTextureCatalog {
    std::map<uint64_t,SignalTextureSet> sets;
    std::vector<uint64_t> defaults;
    std::filesystem::path local_mod_root;
    // Targeted optional reads distinguish an absent key (native default is
    // allowed) from an unreadable present key (never invent a default image).
    std::set<uint64_t> unavailable_sets;
};
bool read_signal_texture_catalog(ReadMemory,void*,const LiveState&,bool,SignalTextureCatalog&) noexcept;
// Required named sets only. Windows 1.19 uses the qualified native name hash;
// unrelated buckets/assets are not dependencies. Other profiles retain the
// historical full reader until their hash algorithm is qualified.
bool read_signal_texture_sets(ReadMemory,void*,const LiveState&,bool,
                              std::span<const std::string_view> names,SignalTextureCatalog&) noexcept;
struct SignalTextureReference {int kind{};uint64_t hash{};};
// Optional metadata for observed boundary signals. A malformed selected set
// becomes unknown locally; valid neighbours remain available. Native defaults
// are used only when that default vector can itself be validated.
bool read_signal_texture_references(ReadMemory,void*,const LiveState&,bool,
                                   std::span<const SignalTextureReference>,SignalTextureCatalog&) noexcept;
// Matches native atlas lookup and selector clamping. GPU/resource-load fallback
// is not reproduced; callers must report a missing image instead of guessing.
const SignalTextureSet* select_signal_textures(const SignalTextureCatalog&,int kind,uint64_t hash) noexcept;
}
