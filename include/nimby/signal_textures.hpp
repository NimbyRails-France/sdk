#pragma once
#include <nimby/client.hpp>

extern "C" {
// pid=0 means the current game process. Paths refer to the loaded mod catalogue,
// never arbitrary files on disk. SDK owns resolution and bridge initialization.
NIMBY_API uint32_t __cdecl NimbyInternal_ShowSignalTexture(uint32_t pid, uint64_t signal,
    const char* texture_set, const char* relative_path) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_RestoreSignalTexture(uint32_t pid, uint64_t signal) NIMBY_NOEXCEPT;
}

namespace nimby {
struct TextureImage {
    std::string set;
    std::string path;
};

// Visual substitution only; does not alter train permissions or saved data.
// Create once, then show(signal, {set, path}). No PID required inside a mod DLL.
// Call outside DllMain. The game must have loaded its simulation and resources.
class SignalTextures {
    uint32_t pid_;
    explicit SignalTextures(uint32_t pid) : pid_(pid) {}
public:
    static SignalTextures inGame() { return SignalTextures(0); }
    static SignalTextures connect(uint32_t pid) {
        if (!pid) detail::check(NIMBY_INVALID_ARGUMENT, "SignalTextures PID must be nonzero");
        return SignalTextures(pid);
    }
    void show(Id signal, const TextureImage& texture) const {
        detail::check(NimbyInternal_ShowSignalTexture(pid_, signal, texture.set.c_str(), texture.path.c_str()),
            "ShowSignalTexture");
    }
    void restore(Id signal) const {
        detail::check(NimbyInternal_RestoreSignalTexture(pid_, signal), "RestoreSignalTexture");
    }
};
}
