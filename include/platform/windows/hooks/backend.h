#pragma once
#include <MinHook.h>

namespace nimby::hooks {
// Internal lifetime owner, serialized by the runtime lock. Not a public mod API.
// Explicit shutdown only: never implicitly disable hooks during DLL destruction.
class Backend final {
public:
    constexpr Backend() noexcept = default;
    Backend(const Backend&) = delete;
    Backend& operator=(const Backend&) = delete;
    MH_STATUS initialize() noexcept;
    MH_STATUS shutdown() noexcept;
private:
    bool owned_ = false;
};
}
