#pragma once
#include "game_layout_types.h"
#include "platform/windows/game_layout.h"
#include "platform/linux/game_layout.h"

namespace nimby::engine {
// The sole registry of target layouts. These headers contain immutable data
// only; including both must never pull in Win32/POSIX headers or host OS calls.
inline constexpr GameLayout unsupportedLayout{};
inline constexpr const GameLayout& gameLayout(LiveStateProfile profile) noexcept {
    switch(profile) {
    case LiveStateProfile::Windows119: return platform::windows::game119;
    case LiveStateProfile::Linux119: return platform::linux_os::game119;
    default: return unsupportedLayout;
    }
}
}
