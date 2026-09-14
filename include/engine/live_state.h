#pragma once
#include <cstddef>
#include <cstdint>
namespace nimby::engine {
using ReadMemory = bool (*)(void*, uint64_t, void*, size_t);
struct LiveState {
    uint64_t module_base{}, root{}, database{}, copy{}, simulation{};
    bool operator==(const LiveState&) const = default;
};
// Caller verifies the executable SHA-256. No retained pointers, no target calls.
// Profile evidence: docs/research/stable-resolution.md. Null/changed roots fail closed.
bool resolve_live_state(ReadMemory read, void* context, uint64_t module_base,
                        bool recognized_build, LiveState& out) noexcept;
}
