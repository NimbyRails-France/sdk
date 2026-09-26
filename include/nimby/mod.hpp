#pragma once
#include <nimby/signal_textures.hpp>
#include <nimby/mod_commands.hpp>
#include <nimby/block_observation.hpp>
#include <nimby/observation_loop.hpp>
#include <nimby/detail/control.h>

namespace nimby {
// High-level reader for mods linked with NimbyRailsFranceSDK::Mod.
// Connects lazily, reuses the connection and releases it when the adapter stops.
// Missing observations return nullopt; connection/API errors throw Exception.
std::optional<DrivingObservation> readTrain(Id train);
// One full snapshot per call, reused to read many block definitions. No polling.
// Empty/missing live coverage cannot currently prove Clear; see blocks.md.
BlockReader readBlocks();
// Polling status of this mod instance, not the SDK's external diagnostic clients.
ObservationLoopStatus modObservationStatus();
// Current C++ panel values. Unavailable until the SDK has established the save
// identity and observed its signals; never falls back to NimbyScript.
SignalSettings readSignalSettings(Id signal);

// Callbacks are compiled into the same mod DLL as the SDK adapter.
// They may throw: the adapter converts exceptions to loader status codes.
struct Mod {
    void (*showTexture)(Id signal, const char* path) = nullptr;
    void (*restoreTexture)(Id signal) = nullptr;
    void (*start)() = nullptr;
    void (*stop)() = nullptr;
    // Optional targeted observation service. Exceptions are translated by the adapter.
    std::optional<DrivingObservation> (*readTrain)(Id train) = nullptr;
    // Static command definitions; lifecycle and ABI checks belong to the SDK.
    std::span<const ModCommand> commands{};
    // Optional live updates. The adapter polls only when hosted by NIMBYRails.exe;
    // diagnostic DLL hosts never start this service against an external game.
    // observe runs serially on the SDK worker, outside DllMain and the loader lock.
    void (*observe)(const Snapshot&) = nullptr;
    void (*observationLost)() = nullptr;
    std::uint32_t observationIntervalMs = 1000;
    // Static strings and checkbox storage must outlive the mod instance.
    // Renderer integration is under development; no NimbyScript is generated.
    SignalSettingsPanel signalSettings{};
    SnapshotScope observationScope = SnapshotScope::Complete;
    std::string_view observationTextureSet{};
    // Local recipe endpoint, explicitly stopped by the adapter before unload.
    const char* controlId=nullptr;
    NimbyControlHandler control=nullptr;
};

// Implement once in mod.cpp. Called explicitly by NRF Loader, never in DllMain.
// start() runs before a game is loaded; it must not assume a simulation exists.
Mod createMod();
}
