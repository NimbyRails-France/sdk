#pragma once
#include <nimby/detail/signal_ui_bridge.h>
#include <nimby/detail/observation.h>
#include <nimby/signal_settings_store.hpp>
#include <nimby/detail/signal_settings_catalog.hpp>
#include <nimby/detail/signal_settings_file.hpp>
#include <nimby/detail/native_library.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <chrono>

namespace nimby::detail {
// Internal mod-adapter connection. The DLL reference protects every export call;
// the resident renderer owns the copied panel and never calls back into the mod.
// Loading and enabling the native hook belong to the SDK's runtime loader.
class SignalSettingsClient {
public:
    SignalSettingsClient()=default;
    SignalSettingsClient(const SignalSettingsClient&)=delete;
    SignalSettingsClient& operator=(const SignalSettingsClient&)=delete;
    ~SignalSettingsClient(){close();}
    void close() noexcept {
        try { checkpoint(true); } catch(...) { std::fputs("NIMBY SDK: cannot save signal settings on close\n",stderr); }
        std::lock_guard lock(mutex_);
        closeLocked();
    }
    bool connectExisting(const SignalSettingsPanel& panel) {
        std::lock_guard lock(mutex_);
        if(module_)return true;
        if(panel.id.empty())return false;
        // Reuse schema validation, including duplicate names and text limits.
        SignalSettingsStore validator;validator.configure(panel);
        auto wire=std::make_unique<NimbyUiPanelV1>();
        wire->size=sizeof(*wire);wire->version=NIMBY_SIGNAL_UI_ABI;
        copy(wire->id,panel.id);copy(wire->title,panel.title);copy(wire->texture_set,panel.textureSet);
        wire->count=static_cast<uint32_t>(panel.checkboxes.size());
        for(size_t i=0;i<panel.checkboxes.size();++i){const auto& box=panel.checkboxes[i];
            auto& target=wire->checkboxes[i];
            copy(target.name,box.name);copy(target.label,box.label);copy(target.description,box.description);
            target.default_value=box.defaultValue?1u:0u;
        }
#ifdef _WIN32
        auto module=native::existing("NimbySignalUiBridge-experimental-v1.dll");
#else
        auto module=native::existing("libNimbySignalUiBridge-experimental-v1.so");
#endif
        if(!module)return false;
        auto add=resolve<NimbyUiRegisterV1>(module,"NimbyUi_RegisterV1");
        auto remove=resolve<NimbyUiRemoveV1>(module,"NimbyUi_RemoveV1");
        auto read=resolve<NimbyUiReadV1>(module,"NimbyUi_ReadV1");
        auto suspend=resolve<NimbyUiSuspendV1>(module,"NimbyUi_SuspendV1");
        auto begin=resolve<NimbyUiBeginV1>(module,"NimbyUi_BeginV1");
        auto observe=resolve<NimbyUiObserveV1>(module,"NimbyUi_ObserveV1");
        uint64_t owner{};
        if(!add||!remove||!read||!suspend||!begin||!observe){native::unload(module);return false;}
        const auto result=add(wire.get(),&owner);
        if(result!=NIMBY_OK||!owner){native::unload(module);return false;}
        panelId_=std::string(panel.id);
        module_=module;owner_=owner;remove_=remove;read_=read;suspend_=suspend;
        begin_=begin;observe_=observe;
        export_=resolve<NimbyUiExportV1>(module,"NimbyUi_ExportV1");
        beginSaved_=resolve<NimbyUiBeginSavedV1>(module,"NimbyUi_BeginSavedV1");
        return true;
    }
    // Only the SDK session coordinator may supply this identity and catalog.
    // Never substitute a process ID or a partial list for a loaded save.
    uint64_t beginSession(std::string_view identity) {
        std::lock_guard lock(mutex_);uint64_t session{};
        if(!module_||identity.empty()||identity.size()>512)return 0;
        return begin_(owner_,identity.data(),static_cast<uint32_t>(identity.size()),&session)==NIMBY_OK?session:0;
    }
    // Explicit transport. Automatic persistence below uses a world profile,
    // independent of native save revisions (including Save As).
    uint64_t beginSession(std::string_view identity,const SignalSettingsStore::SavedSettings& saved) {
        const auto bytes=SignalSettingsFile::encode(saved);
        std::lock_guard lock(mutex_);uint64_t session{};
        if(!module_||!beginSaved_||identity.empty()||identity.size()>512)return 0;
        return beginSaved_(owner_,identity.data(),static_cast<uint32_t>(identity.size()),
            bytes.data(),static_cast<uint32_t>(bytes.size()),&session)==NIMBY_OK?session:0;
    }
    std::optional<SignalSettingsStore::SavedSettings> exportSettings(uint64_t session) const {
        std::lock_guard lock(mutex_);
        if(!module_||!export_||!session)return std::nullopt;
        // A click can enlarge the payload between query and copy. Retry a bounded
        // number of times; never return a partial or previous-session payload.
        uint32_t needed{};
        if(export_(owner_,session,nullptr,0,&needed)!=NIMBY_OK)return std::nullopt;
        for(unsigned attempt=0;attempt<3;++attempt){
            if(!needed||needed>16*1024*1024)return std::nullopt;
            std::string bytes(needed,'\0');uint32_t written{};
            const auto status=export_(owner_,session,bytes.data(),needed,&written);
            if(status==NIMBY_RESOURCE_LIMIT){needed=written;continue;}
            if(status!=NIMBY_OK||written>bytes.size())return std::nullopt;
            bytes.resize(written);return SignalSettingsFile::decode(bytes);
        }
        return std::nullopt;
    }
    bool observe(uint64_t session,std::span<const SignalSettingsStore::Signal> signals) {
        if(signals.size()>1000000)return false;
        std::vector<NimbyUiSignalV1> wire(signals.size());
        for(size_t i=0;i<signals.size();++i){wire[i].id=signals[i].id;copy(wire[i].texture_set,signals[i].textureSet);}
        std::lock_guard lock(mutex_);
        return module_&&observe_(owner_,session,wire.data(),static_cast<uint32_t>(wire.size()))==NIMBY_OK;
    }
    bool connected() const {std::lock_guard lock(mutex_);return module_!=nullptr;}
    // A coordinator must establish the session before supplying its snapshot.
    // Texture resolution failure suspends reads/clicks without pruning values.
    bool observeSnapshot(uint64_t session,const Snapshot& snapshot) {
        const auto catalog=signalSettingsCatalog(snapshot);
        if(!catalog){suspend();return false;}
        return observe(session,*catalog);
    }
    // Serialized worker: latest settings belong to a world, not a save revision.
    // Loading an older save keeps checkbox edits. Full IDs and texture catalog
    // filter deleted signals before imported settings become accessible.
    bool synchronize(const Snapshot& snapshot) {
        const auto& game=snapshot.getGameSession();
        if(!game||!game->generation){suspend();return false;}
        const auto catalog=signalSettingsCatalog(snapshot);
        if(!catalog){suspend();return false;}
        return synchronize(*game,*catalog);
    }
    bool synchronize(const GameSession& game,std::span<const SignalSettingsStore::Signal> catalog) {
        if(!game.generation){suspend();return false;}
        if(!observedGame_||*observedGame_!=game){
            checkpoint(true);
            const auto path=profilePath(game.worldId,panelId_);
            const auto saved=SignalSettingsFile::load(path);
            const auto session=saved?beginSession(game.worldId,*saved):beginSession(game.worldId);
            if(!session){suspend();return false;}
            observedGame_=game;observedSession_=session;
            profilePath_=path;lastSaved_=saved?SignalSettingsFile::encode(*saved):std::string{};
            nextSave_={};
        }
        if(!observe(observedSession_,catalog))return false;
        checkpoint(false);
        return true;
    }
    void suspend() noexcept {
        std::lock_guard lock(mutex_);
        if(module_)suspend_(owner_);
    }
    SignalSettings read(uint64_t signal) const {
        std::lock_guard lock(mutex_);
        if(!module_)return {};
        NimbyUiValuesV1 wire{};wire.size=sizeof(wire);wire.version=NIMBY_SIGNAL_UI_ABI;
        if(read_(owner_,signal,&wire)!=NIMBY_OK||wire.size!=sizeof(wire)||
           wire.version!=NIMBY_SIGNAL_UI_ABI||wire.status>2||wire.count>64)return {};
        SignalSettings result;
        result.status=wire.status==2?SettingsStatus::Present:wire.status==1?SettingsStatus::Absent:SettingsStatus::Unavailable;
        if(result.status!=SettingsStatus::Present)return result;
        for(uint32_t i=0;i<wire.count;++i){const auto& field=wire.fields[i];
            const auto end=static_cast<const char*>(std::memchr(field.name,0,sizeof(field.name)));
            if(!end||end==field.name||field.value>1)return {};
            if(!result.booleans.emplace(std::string(field.name,end),field.value!=0).second)return {};
        }
        return result;
    }
    // Encoding prevents path separators, reserved filenames and collisions.
    static std::filesystem::path profilePath(std::string_view world,std::string_view panel) {
        if(world.size()!=64||world.find_first_not_of("0123456789abcdef")!=std::string_view::npos||panel.empty()||panel.size()>128)
            throw std::invalid_argument("Invalid settings profile identity");
#ifdef _WIN32
        wchar_t root[32768]{};
        const auto n=GetEnvironmentVariableW(L"LOCALAPPDATA",root,32768);
        if(!n||n>=32768)throw std::runtime_error("LOCALAPPDATA unavailable for settings");
#else
        std::filesystem::path root;
        const auto xdg=std::getenv("XDG_STATE_HOME");
        const auto home=std::getenv("HOME");
        if(xdg&&*xdg&&std::filesystem::path(xdg).is_absolute())root=xdg;
        else if(home&&*home&&std::filesystem::path(home).is_absolute())root=std::filesystem::path(home)/".local/state";
        else throw std::runtime_error("User state directory unavailable for settings");
#endif
        std::string encoded;constexpr char hex[]="0123456789abcdef";
        for(unsigned char c:panel){encoded+=hex[c>>4];encoded+=hex[c&15];}
        return std::filesystem::path(root)/L"NimbyRailsFrance"/L"signal-settings"/std::string(world)/(encoded+".settings");
    }
private:
    void checkpoint(bool force) {
        if(profilePath_.empty()||!observedSession_)return;
        const auto now=std::chrono::steady_clock::now();
        if(!force&&now<nextSave_)return;
        nextSave_=now+std::chrono::milliseconds(250);
        const auto saved=exportSettings(observedSession_);
        if(!saved)throw std::runtime_error("Cannot export signal settings");
        const auto bytes=SignalSettingsFile::encode(*saved);
        if(bytes==lastSaved_)return;
        SignalSettingsFile::save(profilePath_,*saved);
        lastSaved_=bytes;
    }
    void closeLocked() noexcept {
        if(module_){remove_(owner_);native::unload(module_);}
        module_=nullptr;owner_=0;remove_=nullptr;read_=nullptr;suspend_=nullptr;
        begin_=nullptr;observe_=nullptr;
        export_=nullptr;beginSaved_=nullptr;
        observedGame_.reset();observedSession_=0;profilePath_.clear();lastSaved_.clear();panelId_.clear();
    }
    template<class F> static F resolve(native::Module module,const char* name) {
        auto address=native::symbol(module,name);F result{};
        static_assert(sizeof(result)==sizeof(address));std::memcpy(&result,&address,sizeof(result));return result;
    }
    template<size_t N> static void copy(char (&out)[N],std::string_view in) {
        if(in.size()>=N)throw std::invalid_argument("Signal settings text too long");
        std::memcpy(out,in.data(),in.size());out[in.size()]=0;
    }
    mutable std::mutex mutex_;
    native::Module module_{};
    uint64_t owner_{};
    NimbyUiRemoveV1 remove_{};
    NimbyUiReadV1 read_{};
    NimbyUiSuspendV1 suspend_{};
    NimbyUiBeginV1 begin_{};
    NimbyUiObserveV1 observe_{};
    NimbyUiExportV1 export_{};
    NimbyUiBeginSavedV1 beginSaved_{};
    // Accessed only by the serialized worker, or close after joining it.
    std::optional<GameSession> observedGame_;
    uint64_t observedSession_=0;
    std::string panelId_,lastSaved_;
    std::filesystem::path profilePath_;
    std::chrono::steady_clock::time_point nextSave_{};
};
}
