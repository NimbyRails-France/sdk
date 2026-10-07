#include <nimby/detail/diagnostics.hpp>
// Compiled into each consumer DLL via NimbyRailsFranceSDK::Mod.
#include <nimby/mod.hpp>
#include <nimby/detail/platform/control_pipe.hpp>
#include <nimby/detail/signal_settings_runtime.hpp>
#include <nimby/detail/signal_settings_client.hpp>
#include <nimby/detail/mod_services_client.hpp>
#include <cwchar>
#include <shared_mutex>
#include <set>
#include <nimby/detail/platform/host.hpp>
#include <nimby/detail/platform/mod_abi.hpp>
#include <nimby/detail/platform/mod_entry.hpp>

namespace {
std::shared_mutex lock;
nimby::Mod callbacks{};
bool initialized = false;
nimby::detail::ModServicesClient& services(){static auto* value=new nimby::detail::ModServicesClient;return *value;}
void connectServices(const nimby::Mod& mod){
    if(!mod.id.empty()&&!services().connected()&&nimby::detail::platform::hostedByGame()&&NimbyInternal_EnsureSignalUiBridge()==NIMBY_OK)
        services().connect(mod.id,mod.services,mod.translationsJson);
}
nimby::detail::control::Server& controls(){static auto* server=new nimby::detail::control::Server;return *server;}
std::mutex readerMutex;
std::unique_ptr<nimby::detail::ObservationSession> reader;
nimby::detail::SignalSettingsClient& settingsBridge() {
    // Explicit close in Stop; no DLL release during static process-detach.
    static auto* bridge=new nimby::detail::SignalSettingsClient;
    return *bridge;
}
struct AdditionalPanel {
    nimby::SignalSettingsPanel declaration;
    std::unique_ptr<nimby::detail::SignalSettingsClient> client;
};
// Changed only while the observation worker is stopped. Each resident panel
// copies its own schema, owns its session and persists under its stable type ID.
std::vector<AdditionalPanel>& additionalPanels(){static auto* panels=new std::vector<AdditionalPanel>;return *panels;}
void closeSettings() {
    settingsBridge().close();
    for(auto& panel:additionalPanels())panel.client->close();
    additionalPanels().clear();
}
void connectSettings(const nimby::SignalSettingsPanel& primary,std::string_view translations) {
    if(!nimby::detail::platform::hostedByGame())return;
    bool needed=!primary.id.empty()&&!settingsBridge().connected();
    for(const auto& panel:additionalPanels())needed=needed||!panel.client->connected();
    if(!needed||NimbyInternal_EnsureSignalUiBridge()!=NIMBY_OK)return;
    if(!primary.id.empty()&&!settingsBridge().connected())settingsBridge().connectExisting(primary,translations);
    for(auto& panel:additionalPanels())if(!panel.client->connected())panel.client->connectExisting(panel.declaration,translations);
}
// Explicitly stopped by NRFMod_StopV1. No joining thread destructor is allowed
// to run during DLL process-detach under the Windows loader lock.
nimby::ObservationLoop& observations() {
    static auto* loop = new nimby::ObservationLoop;
    return *loop;
}

using nimby::detail::platform::hostedByGame;
using nimby::detail::platform::currentProcessId;

void startObservations(const nimby::Mod& mod) {
    if (!mod.observe || !hostedByGame()) return;
    // This client is worker-owned. Opening the current PID performs the SDK's
    // binary identity checks; never discover/attach another game from a DLL host.
    auto connection = std::make_shared<std::unique_ptr<nimby::detail::ObservationSession>>();
    // Tools already use Session scope and capture their requested network on
    // demand. Wake only that lightweight loop; signal loops retain their own
    // cadence. No Mod layout or Kotlin declaration change is needed.
    nimby::detail::ObservationLoopAccess::start(observations(),[connection, mod, observe=mod.observe, panel=mod.signalSettings, scope=mod.observationScope, textureSet=mod.observationTextureSet] {
        nimby::detail::platform::ModWork work;
        connectSettings(panel,mod.translationsJson);
        connectServices(mod);
        if (!*connection) connection->reset(new nimby::detail::ObservationSession(currentProcessId()));
        auto snapshot = !mod.observationSignals.empty()?(*connection)->captureSignalling(mod.observationSignals):
            textureSet.empty()?(*connection)->capture(scope):(*connection)->captureSignalling(textureSet);
        // Capture already retries the mutable presence/occupation pair together.
        // Repeating the entire topology capture here tripled response time when
        // coverage stayed incomplete, delaying approach detection and allowing
        // driving instructions to expire between observations. Deliver this
        // capture immediately; its unknown blocks remain unknown for the mod.
        if(settingsBridge().connected())settingsBridge().synchronize(*snapshot);
        for(auto& panel:additionalPanels())if(panel.client->connected())panel.client->synchronize(*snapshot);
        const auto& game=snapshot->getGameSession();
        const bool serviceReady=game&&services().observe(*game);
        if(!serviceReady)services().suspend();
        observe(*snapshot);
        if(serviceReady&&(mod.signalAction||mod.signalActionV2))for(unsigned i=0;i<64;++i){
            const auto event=services().pollWithValue();if(!event)break;
            const auto& base=event->base;
            if(base.generation==game->generation&&std::string_view(base.world)==game->worldId){
                if(mod.signalActionV2)mod.signalActionV2(*event,*snapshot);
                else if(!event->has_value){auto old=base;old.size=sizeof old;old.version=1;mod.signalAction(old,*snapshot);}
            }
        }
    }, [lost=mod.observationLost] {
        nimby::detail::platform::ModWork work;
        nimby::detail::signalSettingsStore().suspendObservations();
        settingsBridge().suspend();
        for(auto& panel:additionalPanels())panel.client->suspend();
        services().suspend();
        if(lost)lost();
    }, nimby::Milliseconds{mod.observationIntervalMs},mod.observationScope==nimby::SnapshotScope::Session);
}
void releaseReader() {
    std::lock_guard guard(readerMutex);
    reader.reset();
}
struct Guard {
    bool exclusive;
    explicit Guard(bool value) : exclusive(value) {
        if (exclusive) lock.lock(); else lock.lock_shared();
    }
    ~Guard() {
        if (exclusive) lock.unlock(); else lock.unlock_shared();
    }
};
template<class Function> uint32_t boundary(Function operation) noexcept {
    try { return operation(); }
    catch (const nimby::Exception& error) { nimby::detail::diagnostics::exception("mods", "mod callback"); return static_cast<uint32_t>(error.code()); }
    catch (const std::invalid_argument&) { nimby::detail::diagnostics::exception("mods", "mod callback"); return NIMBY_INVALID_ARGUMENT; }
    catch (...) { nimby::detail::diagnostics::exception("mods", "mod callback"); return NIMBY_INTERNAL_ERROR; }
}
}

nimby::ObservationLoopStatus nimby::modObservationStatus() { return observations().status(); }
uint32_t nimby::publishToolPanel(const NimbyUiToolPanelV1& panel){return services().publish(panel);}
uint32_t nimby::publishToolPanel(const NimbyUiToolPanelV2& panel){return services().publish(panel);}
uint32_t nimby::publishSignalPreview(const NimbyUiSignalPreviewV1& preview){return services().preview(preview);}
nimby::SignalSettingsStore& nimby::detail::signalSettingsStore() {
    static SignalSettingsStore store;
    return store;
}
nimby::SignalSettings nimby::readSignalSettings(Id signal) {
    auto result=settingsBridge().connected()?settingsBridge().read(signal):detail::signalSettingsStore().read(signal);
    if(result.status==SettingsStatus::Present)return result;
    bool unavailable=result.status==SettingsStatus::Unavailable;
    for(const auto& panel:additionalPanels()) {
        auto value=panel.client->read(signal);
        if(value.status==SettingsStatus::Present)return value;
        unavailable=unavailable||value.status==SettingsStatus::Unavailable;
    }
    if(unavailable)return {};
    return result;
}

std::optional<nimby::DrivingObservation> nimby::readTrain(Id train) {
    if ((train >> 48) != 5)
        detail::check(NIMBY_INVALID_ARGUMENT, "readTrain requires a train ID");
    std::lock_guard guard(readerMutex);
    if (!reader) reader.reset(new detail::ObservationSession(hostedByGame() ? currentProcessId() : detail::discoverProcess()));
    try { return reader->readTrain(train); }
    catch (const Exception& error) {
        if (error.code() == ErrorCode::ProcessExited) reader.reset();
        throw;
    }
}

nimby::BlockReader nimby::readBlocks() {
    std::lock_guard guard(readerMutex);
    if (!reader) reader.reset(new detail::ObservationSession(hostedByGame() ? currentProcessId() : detail::discoverProcess()));
    try { return observeBlocks(*reader->captureSignalling()); }
    catch (const Exception& error) {
        if (error.code()==ErrorCode::ProcessExited) reader.reset();
        throw;
    }
}

NRF_MOD_EXPORT NRFMod_HostProtocolV1(void*) noexcept { return 1; }
NRF_MOD_EXPORT NRFMod_StartV1(void* reserved) noexcept {
    if (reserved) return NIMBY_INVALID_ARGUMENT;
    Guard guard(true);
    if (initialized) return NIMBY_ALREADY_INITIALIZED;
    nimby::detail::diagnostics::write("mods", "INFO", "Starting mod adapter / ABI 2");
    return boundary([]() -> uint32_t {
        auto candidate = nimby::createMod();
        if(candidate.additionalSignalSettings.size()>15)return NIMBY_RESOURCE_LIMIT;
        std::set<std::string_view> panelIds,catalogues;
        const auto validatePanel=[&](const nimby::SignalSettingsPanel& panel) {
            nimby::SignalSettingsStore validator;validator.configure(panel);
            if(panel.id.empty()||!panelIds.insert(panel.id).second||!catalogues.insert(panel.textureSet).second)
                throw std::invalid_argument("Duplicate or missing signal panel identity/catalogue");
        };
        if(!candidate.signalSettings.id.empty())validatePanel(candidate.signalSettings);
        for(const auto& panel:candidate.additionalSignalSettings)validatePanel(panel);
        if (candidate.observe && (candidate.observationIntervalMs < 10 || candidate.observationIntervalMs > 3600000))
            return NIMBY_INVALID_ARGUMENT;
        if (candidate.commands.size() > 256) return NIMBY_RESOURCE_LIMIT;
        for (std::size_t i = 0; i < candidate.commands.size(); ++i) {
            const auto& command = candidate.commands[i];
            if (!command.name || !*command.name || !command.call || !command.requestSize || !command.responseSize
                || command.requestSize > 1024*1024 || command.responseSize > 1024*1024) return NIMBY_INVALID_ARGUMENT;
            for (std::size_t j = 0; j < i; ++j)
                if (std::strcmp(command.name,candidate.commands[j].name)==0) return NIMBY_INVALID_ARGUMENT;
        }
        nimby::detail::signalSettingsStore().configure(candidate.signalSettings);
        bool started=false;
        try {
            for(const auto& panel:candidate.additionalSignalSettings)
                additionalPanels().push_back({panel,std::make_unique<nimby::detail::SignalSettingsClient>()});
            connectSettings(candidate.signalSettings,candidate.translationsJson);
            if (candidate.start) candidate.start();
            started=true;
            connectServices(candidate);
            startObservations(candidate);
            if(candidate.control&&hostedByGame())controls().start(candidate.controlId,candidate.control);
        }
        catch (...) {
            controls().stop();
            observations().stop();
            services().close();
            closeSettings();
            nimby::detail::signalSettingsStore().configure({});
            if (started && candidate.stop) candidate.stop();
            throw;
        }
        callbacks = candidate;
        initialized = true;
        return NIMBY_OK;
    });
}
NRF_MOD_EXPORT NRFMod_StopV1(void* reserved) noexcept {
    nimby::detail::diagnostics::write("mods", "INFO", "Stopping mod adapter");
    if (reserved) return NIMBY_INVALID_ARGUMENT;
    Guard guard(true);
    return boundary([]() -> uint32_t {
        controls().stop();
        observations().stop();
        services().close();
        closeSettings();
        nimby::detail::signalSettingsStore().configure({});
        if (initialized && callbacks.stop) callbacks.stop();
        releaseReader();
        callbacks = {};
        initialized = false;
        return NIMBY_OK;
    });
}
NRF_MOD_EXPORT NRFMod_IsInitializedV1(void*) noexcept {
    Guard guard(false);
    return initialized ? 1 : 0;
}
NRF_MOD_EXPORT NRFMod_ShowTextureV1(uint64_t signal, const char* path) noexcept {
    Guard guard(false);
    if (!initialized) return NIMBY_INVALID_HANDLE;
    if (!signal || !path || !*path) return NIMBY_INVALID_ARGUMENT;
    if (!callbacks.showTexture) return NIMBY_HOOKS_UNAVAILABLE;
    return boundary([&]() -> uint32_t { callbacks.showTexture(signal, path); return NIMBY_OK; });
}
NRF_MOD_EXPORT NRFMod_RestoreTextureV1(uint64_t signal) noexcept {
    Guard guard(false);
    if (!initialized) return NIMBY_INVALID_HANDLE;
    if (!signal) return NIMBY_INVALID_ARGUMENT;
    if (!callbacks.restoreTexture) return NIMBY_HOOKS_UNAVAILABLE;
    return boundary([&]() -> uint32_t { callbacks.restoreTexture(signal); return NIMBY_OK; });
}
NRF_MOD_EXPORT NRFMod_ReadTrainV1(uint64_t train,NimbyDrivingObservation* out) noexcept {
    if(!out||out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={};out->struct_size=sizeof *out;
    Guard guard(false);
    if(!initialized)return NIMBY_INVALID_HANDLE;
    if((train>>48)!=5)return NIMBY_INVALID_ARGUMENT;
    if(!callbacks.readTrain)return NIMBY_HOOKS_UNAVAILABLE;
    return boundary([&]() -> uint32_t {
        auto value=callbacks.readTrain(train);
        if(!value)return NIMBY_DATA_UNAVAILABLE;
        *out=nimby::detail::DrivingAccess::native(*value);return NIMBY_OK;
    });
}
NRF_MOD_EXPORT NRFMod_InvokeV1(const char* name,
    const void* input, uint32_t inputSize, void* output, uint32_t outputSize) noexcept {
    if (!output || !outputSize || outputSize > 1024*1024) return NIMBY_INVALID_ARGUMENT;
    std::memset(output, 0, outputSize);
    if (!name || !*name || !input || !inputSize || inputSize > 1024*1024) return NIMBY_INVALID_ARGUMENT;
    Guard guard(false);
    if (!initialized) return NIMBY_INVALID_HANDLE;
    for (const auto& command : callbacks.commands) {
        if (std::strcmp(name,command.name)!=0) continue;
        if (inputSize != command.requestSize || outputSize != command.responseSize) return NIMBY_INVALID_ARGUMENT;
        const auto status = boundary([&]() -> uint32_t { command.call(input,output); return NIMBY_OK; });
        if (status != NIMBY_OK) std::memset(output,0,outputSize);
        return status;
    }
    return NIMBY_HOOKS_UNAVAILABLE;
}
