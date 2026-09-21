#include "engine/live_state.h"
namespace nimby::engine {
bool resolve_live_state(ReadMemory read, void* context, uint64_t base, bool recognized, LiveState& out) noexcept {
    return resolve_live_state(read, context, base, recognized, LiveStateProfile::Windows119, out);
}
bool resolve_live_state(ReadMemory read, void* context, uint64_t base, bool recognized,
                        LiveStateProfile profile, LiveState& out) noexcept {
    out = {};
    if (!recognized || !read || base < 0x10000 || base > 0x7fffff000000ULL) return false;
    uint64_t root_rva{};
    switch (profile) {
        case LiveStateProfile::Windows119: root_rva = 0xb81998; break;
        case LiveStateProfile::Linux119: root_rva = 0x10ee020; break;
        default: return false;
    }
    auto pointer = [&](uint64_t at, uint64_t& value) {
        return read(context, at, &value, sizeof value) && value >= 0x10000 && value < 0x7fffffff0000ULL && value % 8 == 0;
    };
    auto capture = [&](LiveState& state) {
        state.module_base = base;
        state.profile = profile;
        return pointer(base + root_rva, state.root) &&
            pointer(state.root + 0x540, state.database) &&
            pointer(state.root + 0x5c0, state.copy) &&
            pointer(state.root + 0x680, state.simulation) && state.copy != state.simulation;
    };
    LiveState first{}, second{};
    if (!capture(first) || !capture(second) || first != second) return false;
    out = first;
    return true;
}
}
