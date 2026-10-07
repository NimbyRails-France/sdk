#pragma once
#include <nimby/detail/observation_session.hpp>
#include <nimby/detail/texture_updates.h>

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
struct TextureUpdate {
    Id signal=0;
    TextureImage image;
    std::string alternatePath;
    Milliseconds halfPeriod{0},duration{2500};
};

// Visual substitution only; does not alter train permissions or saved data.
// Create once, then show(signal, {set, path}). No PID required inside a mod DLL.
// Call outside DllMain. The game must have loaded its simulation and resources.
class SignalTextures {
    uint32_t pid_;
    explicit SignalTextures(uint32_t pid) : pid_(pid) {}
    static uint64_t publisher(){static const unsigned char identity=0;return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&identity));}
public:
    static SignalTextures inGame() { return SignalTextures(0); }
    static SignalTextures connect(uint32_t pid) {
        if (!pid) detail::check(NIMBY_INVALID_ARGUMENT, "SignalTextures PID must be nonzero");
        return SignalTextures(pid);
    }
    void show(Id signal, const TextureImage& texture) const {
        if(!pid_){const TextureUpdate value{signal,texture,{},Milliseconds{0},Milliseconds{0}};publish({&value,1});return;}
        detail::check(NimbyInternal_ShowSignalTexture(pid_, signal, texture.set.c_str(), texture.path.c_str()),
            "ShowSignalTexture");
    }
    // In-game cleanup releases only this publisher's override. An absent ID
    // or one acquired by another publisher is already released for this caller.
    void restore(Id signal) const {
        if(!pid_){detail::check(NimbyInternal_ClearOwnedTextures(0,publisher(),&signal,1),"ClearOwnedTextures");return;}
        detail::check(NimbyInternal_RestoreSignalTexture(pid_, signal), "RestoreSignalTexture");
    }
    void restore(std::span<const Id> signals) const {
        if(signals.empty())return;
        if(signals.size()>4096)throw std::invalid_argument("Too many restored textures");
        if(pid_){for(const auto signal:signals)restore(signal);return;}
        detail::check(NimbyInternal_ClearOwnedTextures(0,publisher(),signals.data(),static_cast<uint32_t>(signals.size())),"ClearOwnedTextures");
    }
    // Expiring override for automatic observation loops. Renew with fresh data;
    // otherwise the native renderer regains control even if cleanup fails.
    void showFor(Id signal, const TextureImage& texture, Milliseconds duration) const {
        if (duration.count() < 1000 || duration.count() > 60000)
            throw std::invalid_argument("Texture lease must be 1000..60000 ms");
        if(!pid_){const TextureUpdate value{signal,texture,{},Milliseconds{0},duration};publish({&value,1});return;}
        detail::check(NimbyInternal_ShowSignalTextureFor(pid_, signal, texture.set.c_str(),
            texture.path.c_str(), static_cast<uint32_t>(duration.count())), "ShowSignalTextureFor");
    }
    // Both images belong to the same loaded catalogue. Phase is selected by the
    // render bridge from simulation time, independent of observation frequency.
    void animateFor(Id signal, const TextureImage& first, std::string_view alternatePath,
        Milliseconds halfPeriod, Milliseconds duration) const {
        if (duration.count()<1000 || duration.count()>60000 || halfPeriod.count()<100 || halfPeriod.count()>10000)
            throw std::invalid_argument("Invalid texture animation timing");
        if(!pid_){const TextureUpdate value{signal,first,std::string(alternatePath),halfPeriod,duration};publish({&value,1});return;}
        const std::string alternate(alternatePath);
        detail::check(NimbyInternal_ShowSignalAnimationFor(pid_,signal,first.set.c_str(),first.path.c_str(),
            alternate.c_str(),static_cast<uint32_t>(halfPeriod.count()),static_cast<uint32_t>(duration.count())),
            "ShowSignalAnimationFor");
    }
    // Changes and lease renewals share one validated catalogue and publication.
    void publish(std::span<const TextureUpdate> updates) const {
        if(updates.empty())return;
        if(updates.size()>4096)throw std::invalid_argument("Too many texture updates");
        std::vector<NimbyTextureUpdate> wire(updates.size());
        const auto copy=[](auto& out,std::string_view text){
            if(text.size()>=sizeof(out))throw std::invalid_argument("Texture update text too long");
            std::memcpy(out,text.data(),text.size());out[text.size()]=0;
        };
        for(size_t i=0;i<updates.size();++i){const auto& value=updates[i];auto& row=wire[i];
            if(value.duration.count()!=0&&(value.duration.count()<1000||value.duration.count()>60000))throw std::invalid_argument("Invalid texture lease");
            if(value.halfPeriod.count()!=0&&(value.halfPeriod.count()<100||value.halfPeriod.count()>10000))throw std::invalid_argument("Invalid animation timing");
            row.signal=value.signal;row.duration_ms=static_cast<uint32_t>(value.duration.count());row.half_period_ms=static_cast<uint32_t>(value.halfPeriod.count());
            copy(row.texture_set,value.image.set);copy(row.first_path,value.image.path);copy(row.alternate_path,value.alternatePath);
        }
        detail::check(NimbyInternal_PublishTextureUpdates(pid_,publisher(),wire.data(),static_cast<uint32_t>(wire.size())),"PublishTextureUpdates");
    }
};
}
