#pragma once
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/signal_ui_bridge.h>
#include <nimby/detail/observation.h>
#include <nimby/signal_settings_store.hpp>
#include <nimby/detail/signal_settings_catalog.hpp>
#include <nimby/detail/signal_settings_file.hpp>
#include <nimby/detail/native_library.hpp>
#include <nimby/detail/platform/host.hpp>
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
        try { checkpoint(true); } catch(...) { nimby::detail::diagnostics::exception("mods", __func__);  std::fputs("NIMBY SDK: cannot save signal settings on close\n",stderr); }
        std::lock_guard lock(mutex_);
        closeLocked();
    }
    bool connectExisting(const SignalSettingsPanel& panel,std::string_view translations={}) {
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
        // Allocate/validate before acquiring the resident registration. A bad
        // declaration must not leave an owner behind if copying throws.
        if(panel.actions.size()>16)throw std::invalid_argument("Too many signal actions");
        std::vector<NimbyUiActionV1> declarations(panel.actions.size());
        for(size_t i=0;i<panel.actions.size();++i){const auto& a=panel.actions[i];auto& target=declarations[i];
            copy(target.id,a.id);copy(target.label,a.label);copy(target.provider,a.provider);copy(target.service,a.service);}
        std::string panelId(panel.id),textureSet(panel.textureSet);
        std::vector<NimbyUiNumberSettingV1> numbers(panel.numbers.size());
        for(size_t i=0;i<panel.numbers.size();++i){const auto& field=panel.numbers[i];auto& target=numbers[i];
            copy(target.name,field.name);copy(target.label,field.label);copy(target.visible_when,field.visibleWhen);
            target.bits=field.bits;target.maximum=field.maximum;}
        auto module=native::existing(platform::signalUiLibrary);
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
        if(!numbers.empty()){
            const auto configure=resolve<NimbyUiNumberSettingsV1>(module,"NimbyUi_NumberSettingsV1");
            if(!configure||configure(owner,numbers.data(),static_cast<uint32_t>(numbers.size()))!=NIMBY_OK){
                remove(owner);native::unload(module);return false;
            }
        }
        if(!translations.empty()){
            const auto set=resolve<NimbyUiTranslationsV1>(module,"NimbyUi_TranslationsV1");
            if(!set||set(0,owner,translations.data(),static_cast<uint32_t>(translations.size()))!=NIMBY_OK){
                remove(owner);native::unload(module);throw std::runtime_error("SDK UI bridge lacks compatible mod translations; update the SDK");
            }
        }
        uint64_t conditional=0;
        for(size_t i=0;i<panel.checkboxes.size();++i)if(panel.checkboxes[i].onlyWhenEnabled)conditional|=uint64_t{1}<<i;
        if(conditional){
            const auto visibility=resolve<NimbyUiConditionalVisibilityV1>(module,"NimbyUi_ConditionalVisibilityV1");
            if(!visibility||visibility(owner,conditional)!=NIMBY_OK){remove(owner);native::unload(module);return false;}
        }
        const auto actions=resolve<NimbyUiActionsV1>(module,"NimbyUi_ActionsV1");
        if(actions&&!panel.actions.empty()){
            if(actions(owner,declarations.data(),static_cast<uint32_t>(declarations.size()))!=NIMBY_OK){remove(owner);native::unload(module);return false;}
        }
        panelId_=std::move(panelId);
        textureSet_=std::move(textureSet);
        migrate_=panel.migrate;
        module_=module;owner_=owner;remove_=remove;read_=read;suspend_=suspend;
        begin_=begin;observe_=observe;
        export_=resolve<NimbyUiExportV1>(module,"NimbyUi_ExportV1");
        beginSaved_=resolve<NimbyUiBeginSavedV1>(module,"NimbyUi_BeginSavedV1");
        context_=resolve<NimbyUiPanelContextV1>(module,"NimbyUi_PanelContextV1");
        readBatch_=resolve<NimbyUiReadBatchV1>(module,"NimbyUi_ReadBatchV1");
        revision_=resolve<NimbyUiSettingsRevisionV1>(module,"NimbyUi_SettingsRevisionV1");
        return true;
    }
    // Only the SDK session coordinator may supply this identity and catalog.
    // Never substitute a process ID or a partial list for a loaded save.
    uint64_t beginSession(std::string_view identity) {
        std::lock_guard lock(mutex_);uint64_t session{};
        cached_=false;cache_.clear();
        if(!module_||identity.empty()||identity.size()>512)return 0;
        return begin_(owner_,identity.data(),static_cast<uint32_t>(identity.size()),&session)==NIMBY_OK?session:0;
    }
    // Explicit transport. Automatic persistence below uses a world profile,
    // independent of native save revisions (including Save As).
    uint64_t beginSession(std::string_view identity,const SignalSettingsStore::SavedSettings& saved) {
        const auto bytes=SignalSettingsFile::encode(saved);
        std::lock_guard lock(mutex_);uint64_t session{};
        cached_=false;cache_.clear();
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
        cached_=false; // Reuse allocations only after a fresh validated batch.
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
            auto saved=SignalSettingsFile::load(path);
            const auto original=saved?SignalSettingsFile::encode(*saved):std::string{};
            // Migration works on an owned copy and completes before the new
            // session is visible. Exceptions preserve the original file and
            // leave reads unavailable rather than accepting partial settings.
            if(saved&&migrate_)for(auto& signal:saved->signals)migrate_(panelId_,signal.values);
            const auto session=saved?beginSession(game.worldId,*saved):beginSession(game.worldId);
            if(!session){suspend();return false;}
            observedGame_=game;observedSession_=session;
            profilePath_=path;lastSaved_=original;
            savedRevision_=0;
            nextSave_={};
        }
        if(!observe(observedSession_,catalog))return false;
        if(context_&&context_(owner_,observedSession_,game.generation)!=NIMBY_OK){suspend();return false;}
        if(!refresh(catalog)){suspend();return false;}
        checkpoint(false);
        return true;
    }
    void suspend() noexcept {
        std::lock_guard lock(mutex_);
        cached_=false;cache_.clear();
        if(module_)suspend_(owner_);
    }
    SignalSettings read(uint64_t signal) const {
        std::lock_guard lock(mutex_);
        if(!module_)return {};
        if(cached_){
            const auto found=cache_.find(signal);
            if(found!=cache_.end())return found->second.settings;
            SignalSettings absent;absent.status=SettingsStatus::Absent;return absent;
        }
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
        const auto root=platform::stateDirectory();
        std::string encoded;constexpr char hex[]="0123456789abcdef";
        for(unsigned char c:panel){encoded+=hex[c>>4];encoded+=hex[c&15];}
        return std::filesystem::path(root)/L"NimbyRailsFrance"/L"signal-settings"/std::string(world)/(encoded+".settings");
    }
private:
    friend struct SignalSettingsClientTest;
    bool refresh(std::span<const SignalSettingsStore::Signal> catalog) {
        std::lock_guard lock(mutex_);
        if(!readBatch_)return true; // Older resident bridges retain their ABI.
        cached_=false;
        std::vector<uint64_t> ids;
        for(const auto& signal:catalog)if(signal.textureSet==textureSet_)ids.push_back(signal.id);
        if(ids.size()>16384)return false;
        NimbyUiReadBatchHeaderV1 header{};header.size=sizeof header;header.version=1;
        std::vector<NimbyUiReadBatchRowV1> rows(ids.size());
        if(readBatch_(owner_,ids.data(),static_cast<uint32_t>(ids.size()),&header,rows.data())!=NIMBY_OK||
           header.size!=sizeof header||header.version!=1||header.count!=ids.size()||header.field_count>64)return false;
        std::vector<std::string> names;names.reserve(header.field_count);
        std::set<std::string> unique;
        for(uint32_t i=0;i<header.field_count;++i){
            const auto end=static_cast<const char*>(std::memchr(header.names[i],0,sizeof header.names[i]));
            if(!end||end==header.names[i])return false;
            names.emplace_back(header.names[i],static_cast<size_t>(end-header.names[i]));
            if(!unique.insert(names.back()).second)return false;
        }
        const bool sameSchema=names==cacheNames_;
        std::map<uint64_t,CachedSettings> next;
        for(size_t i=0;i<rows.size();++i){const auto& row=rows[i];
            if(row.signal!=ids[i]||row.status>2||row.reserved||
               (header.field_count<64&&(row.values>>header.field_count))||(row.status!=2&&row.values))return false;
            if(next.contains(row.signal))return false;
            const auto status=row.status==2?SettingsStatus::Present:row.status==1?SettingsStatus::Absent:SettingsStatus::Unavailable;
            auto node=cache_.extract(row.signal);
            if(node.empty())node=next.extract(next.emplace(row.signal,CachedSettings{}).first);
            auto& value=node.mapped();
            if(!sameSchema||value.settings.status!=status||value.mask!=row.values){
                value.settings.status=status;value.settings.booleans.clear();value.mask=row.values;
                if(status==SettingsStatus::Present)for(size_t j=0;j<names.size();++j)
                    value.settings.booleans.emplace(names[j],bool(row.values&(uint64_t{1}<<j)));
            }
            next.insert(std::move(node));
        }
        cache_=std::move(next);cacheNames_=std::move(names);cached_=true;return true;
    }
    void checkpoint(bool force) {
        if(profilePath_.empty()||!observedSession_)return;
        const auto now=std::chrono::steady_clock::now();
        if(!force&&now<nextSave_)return;
        nextSave_=now+std::chrono::milliseconds(250);
        uint64_t before{};
        if(revision_){
            const auto status=revision_(owner_,observedSession_,&before);
            if(status==NIMBY_HOOKS_UNAVAILABLE)revision_=nullptr; // Older resident bridge.
            else if(status!=NIMBY_OK||!before)throw std::runtime_error("Cannot read signal settings revision");
            else if(savedRevision_==before)return;
        }
        const auto saved=exportSettings(observedSession_);
        if(!saved)throw std::runtime_error("Cannot export signal settings");
        const auto bytes=SignalSettingsFile::encode(*saved);
        if(bytes!=lastSaved_){
            SignalSettingsFile::save(profilePath_,*saved);
            lastSaved_=bytes;
        }
        // Remember the revision from BEFORE export. A click racing the copy or
        // disk write must remain dirty for the next checkpoint, including close.
        savedRevision_=before;
    }
    void closeLocked() noexcept {
        if(module_){remove_(owner_);native::unload(module_);}
        module_=nullptr;owner_=0;remove_=nullptr;read_=nullptr;suspend_=nullptr;
        begin_=nullptr;observe_=nullptr;
        export_=nullptr;beginSaved_=nullptr;context_=nullptr;
        readBatch_=nullptr;cached_=false;cache_.clear();cacheNames_.clear();textureSet_.clear();
        revision_=nullptr;savedRevision_=0;
        observedGame_.reset();observedSession_=0;profilePath_.clear();lastSaved_.clear();panelId_.clear();
        migrate_=nullptr;
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
    NimbyUiPanelContextV1 context_{};
    NimbyUiReadBatchV1 readBatch_{};
    NimbyUiSettingsRevisionV1 revision_{};
    uint64_t savedRevision_=0;
    bool cached_=false;
    struct CachedSettings {SignalSettings settings;uint64_t mask{};};
    std::map<uint64_t,CachedSettings> cache_;
    std::vector<std::string> cacheNames_;
    std::string textureSet_;
    // Accessed only by the serialized worker, or close after joining it.
    std::optional<GameSession> observedGame_;
    uint64_t observedSession_=0;
    std::string panelId_,lastSaved_;
    void (*migrate_)(std::string_view,std::map<std::string,bool,std::less<>>&)=nullptr;
    std::filesystem::path profilePath_;
    std::chrono::steady_clock::time_point nextSave_{};
};
}
