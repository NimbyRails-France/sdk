#pragma once
#include <nimby/signal_textures.hpp>

namespace nimby {
// Callbacks are compiled into the same mod DLL as the SDK adapter.
// They may throw: the adapter converts exceptions to loader status codes.
struct Mod {
    void (*showTexture)(Id signal, const char* path) = nullptr;
    void (*restoreTexture)(Id signal) = nullptr;
    void (*start)() = nullptr;
    void (*stop)() = nullptr;
};

// Implement once in mod.cpp. Called explicitly by NRF Loader, never in DllMain.
// start() runs before a game is loaded; it must not assume a simulation exists.
Mod createMod();
}
