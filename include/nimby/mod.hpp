#pragma once
#include <nimby/signal_textures.hpp>
#include <nimby/mod_commands.hpp>
#include <nimby/block_observation.hpp>
#include <nimby/observation_loop.hpp>
#include <nimby/detail/control.h>
#include <nimby/detail/signal_ui_bridge.h>

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
// SDK-private tool adapter transport. Only valid on this mod's worker.
uint32_t publishToolPanel(const NimbyUiToolPanelV1& panel);
uint32_t publishToolPanel(const NimbyUiToolPanelV2& panel);
uint32_t publishSignalPreview(const NimbyUiSignalPreviewV1& preview);

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
    // Owned, adapter-private declarations. Multi-model signalling stays scoped
    // instead of falling back to a complete world scan every 20 ms.
    std::vector<NimbySignalCaptureScope> observationSignals{};
    // Local recipe endpoint, explicitly stopped by the adapter before unload.
    const char* controlId=nullptr;
    NimbyControlHandler control=nullptr;
    // Additional model panels of the SAME mod. All declarations must outlive
    // the adapter. Each panel has its own catalogue, defaults and persistence.
    std::span<const SignalSettingsPanel> additionalSignalSettings{};
    std::string_view id{};
    std::span<const std::string> services{};
    // SDK-private UTF-8 JSON, owned by the adapter for its lifetime.
    std::string_view translationsJson{};
    // Invoked on this mod's observation worker, never on the native UI thread.
    void (*signalAction)(const NimbyUiActionEventV1&,const Snapshot&)=nullptr;
    void (*signalActionV2)(const NimbyUiActionEventV2&,const Snapshot&)=nullptr;
};

// Implement once in mod.cpp. Called explicitly by NRF Loader, never in DllMain.
// start() runs before a game is loaded; it must not assume a simulation exists.
Mod createMod();
}
