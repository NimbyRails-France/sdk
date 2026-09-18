#pragma once
#include <nimby/client.hpp>

// Experimental visual control only. Native aspect, permissions and snapshots
// are unchanged. Dynamically allocated independent signals; no save data is written.
struct NimbySignalTextureOverrideStatus {
    uint32_t struct_size,active,index,reserved;
    uint64_t active_count;
    uint64_t expires_at_ms;
};
struct NimbyTexturePreviewStatus {
    // Aggregate counters, followed by the LAST command's signal/expiry.
    // Use getSignalTextureOverrideStatus for authoritative per-signal state.
    uint32_t struct_size, render_thread;
    uint64_t callbacks, valid_signals, applied, last_signal, last_rules;
    uint64_t signal, expires_at_ms;
};
extern "C" {
NIMBY_API uint32_t __cdecl NimbyInternal_SignalTextureOverrideStatus(uint32_t pid,uint64_t signal,
    NimbySignalTextureOverrideStatus* out) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_PreviewSignalTexture(uint32_t pid, uint64_t signal,
    const char* set_id, uint32_t index, uint32_t duration_ms) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ForceSignalTexture(uint32_t pid, uint64_t signal,
    const char* set_id, uint32_t index) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_TexturePreviewStatus(uint32_t pid,
    NimbyTexturePreviewStatus* out) NIMBY_NOEXCEPT;
NIMBY_API uint32_t __cdecl NimbyInternal_ClearTexturePreview(uint32_t pid, uint64_t signal) NIMBY_NOEXCEPT;
}
namespace nimby {
inline NimbySignalTextureOverrideStatus getSignalTextureOverrideStatus(uint32_t pid,Id signal) {
    NimbySignalTextureOverrideStatus result{};result.struct_size=sizeof result;
    detail::check(NimbyInternal_SignalTextureOverrideStatus(pid,signal,&result),"SignalTextureOverrideStatus");
    return result;
}
// Remains active after the calling program exits. Replace on the same signal
// or call restoreSignalTexture to return to native rendering. Session-scoped:
// not saved, not automatically reapplied to another loaded game.
inline void forceSignalTexture(uint32_t pid, Id signal, const std::string& set,uint32_t index) {
    detail::check(NimbyInternal_ForceSignalTexture(pid,signal,set.c_str(),index),"ForceSignalTexture");
}
inline void previewSignalTexture(uint32_t pid, Id signal, const std::string& set,
                                 uint32_t index, Milliseconds duration) {
    if(duration.count()<1000 || duration.count()>60000)
        detail::check(NIMBY_INVALID_ARGUMENT,"preview duration must be 1000..60000 ms");
    detail::check(NimbyInternal_PreviewSignalTexture(pid,signal,set.c_str(),index,
        static_cast<uint32_t>(duration.count())),"PreviewSignalTexture");
}
inline NimbyTexturePreviewStatus getTexturePreviewStatus(uint32_t pid) {
    NimbyTexturePreviewStatus result{};result.struct_size=sizeof result;
    detail::check(NimbyInternal_TexturePreviewStatus(pid,&result),"TexturePreviewStatus");
    return result;
}
inline void clearTexturePreview(uint32_t pid,Id signal) {
    detail::check(NimbyInternal_ClearTexturePreview(pid,signal),"ClearTexturePreview");
}
inline void restoreSignalTexture(uint32_t pid,Id signal) { clearTexturePreview(pid,signal); }
}
