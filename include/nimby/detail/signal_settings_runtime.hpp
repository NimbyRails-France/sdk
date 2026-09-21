#pragma once
#include <nimby/signal_settings_store.hpp>
#include <nimby/detail/sdk.h>
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_EnsureSignalUiBridge() NIMBY_NOEXCEPT;
namespace nimby::detail {
// Internal adapter endpoint for the future native UI/session bridge.
// Never hand the mod a native address or make it own UI event synchronization.
SignalSettingsStore& signalSettingsStore();
}
