#pragma once
#include <cstdint>
namespace nimby::platform::windows::mod_options {
// Bootstrap has qualified the full executable and initialized MinHook. This
// prepares and queues hooks only; Bootstrap applies its complete hook set once.
// Adds NRF Hub to native Options, with separate Interface/Shortcuts categories.
// No overlay, OS hotkeys or additional value in the game's tab enum is used.
bool installUi(uint64_t module)noexcept;
// Bootstrap failure path, before enabling or after disabling all hooks. Never
// unload an active trampoline while a native callback may still be running.
void cleanupHooks()noexcept;
}
