#pragma once
#include <nimby/detail/observation_session.hpp>

extern "C" {
// pid=0 means the current game process. Paths refer to the loaded mod catalogue,
// never arbitrary files on disk. SDK owns resolution and bridge initialization.
NIMBY_API uint32_t __cdecl NimbyInternal_ShowSignalTexture(uint32_t pid, uint64_t signal,
    const char* texture_set, const char* relative_path) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_RestoreSignalTexture(uint32_t pid, uint64_t signal) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ShowSignalTextureFor(uint32_t pid, uint64_t signal,
    const char* texture_set, const char* relative_path, uint32_t duration_ms) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ShowSignalAnimationFor(uint32_t pid, uint64_t signal,
    const char* texture_set, const char* first_path, const char* alternate_path,
    uint32_t half_period_ms, uint32_t duration_ms) NIMBY_NOEXCEPT;
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
    // Expiring override for automatic observation loops. Renew with fresh data;
    // otherwise the native renderer regains control even if cleanup fails.
    void showFor(Id signal, const TextureImage& texture, Milliseconds duration) const {
        if (duration.count() < 1000 || duration.count() > 60000)
            throw std::invalid_argument("Texture lease must be 1000..60000 ms");
        detail::check(NimbyInternal_ShowSignalTextureFor(pid_, signal, texture.set.c_str(),
            texture.path.c_str(), static_cast<uint32_t>(duration.count())), "ShowSignalTextureFor");
    }
    // Both images belong to the same loaded catalogue. Phase is selected by the
    // render bridge from simulation time, independent of observation frequency.
    void animateFor(Id signal, const TextureImage& first, std::string_view alternatePath,
        Milliseconds halfPeriod, Milliseconds duration) const {
        if (duration.count()<1000 || duration.count()>60000 || halfPeriod.count()<100 || halfPeriod.count()>10000)
            throw std::invalid_argument("Invalid texture animation timing");
        const std::string alternate(alternatePath);
        detail::check(NimbyInternal_ShowSignalAnimationFor(pid_,signal,first.set.c_str(),first.path.c_str(),
            alternate.c_str(),static_cast<uint32_t>(halfPeriod.count()),static_cast<uint32_t>(duration.count())),
            "ShowSignalAnimationFor");
    }
};
}
