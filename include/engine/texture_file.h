#pragma once
#include "signal_textures.h"
#include <nimby/detail/observation.h>
#include <algorithm>
#include <string_view>

namespace nimby::engine {
inline uint32_t resolve_texture_file(const SignalTextureCatalog& catalog, std::string_view set_name,
                                    std::string path, uint64_t& hash, uint32_t& index) {
    if (set_name.empty() || path.empty()) return NIMBY_INVALID_ARGUMENT;
    std::replace(path.begin(), path.end(), '\\', '/');
    bool found = false;
    for (const auto& [key, set] : catalog.sets) {
        if (set.name != set_name) continue;
        for (size_t i = 0; i < set.files.size(); ++i) {
            auto candidate = set.files[i].relative_path;
            std::replace(candidate.begin(), candidate.end(), '\\', '/');
            if (candidate != path) continue;
            if (found) return NIMBY_INVALID_ARGUMENT; // Never guess between ambiguous resources.
            found = true; hash = key; index = static_cast<uint32_t>(i);
        }
    }
    return found ? NIMBY_OK : NIMBY_DATA_UNAVAILABLE;
}
}
