#pragma once
#include <nimby/detail/sdk.h>

namespace nimby::platform {
// Called only by runtime.cpp under its lifecycle mutex, never under DllMain.
// Hooks remain disabled until the separate policy authorizes validated targets.
void diagnosticLog(const char* message) noexcept;
uint32_t identifyHost(NimbyBinaryInfo& output) noexcept;
uint32_t initializeHookBackend() noexcept;
uint32_t shutdownHookBackend() noexcept;
}
