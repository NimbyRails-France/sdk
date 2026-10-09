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
#include <array>
#include <algorithm>
#include <atomic>

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
        SyncResult result;
        return beginSessionStatus(identity,nullptr,result);
    }
    // Explicit transport. Automatic persistence below uses a world profile,
    // independent of native save revisions (including Save As).
    uint64_t beginSession(std::string_view identity,const SignalSettingsStore::SavedSettings& saved) {
        const auto bytes=SignalSettingsFile::encode(saved);
        SyncResult result;
        return beginSessionStatus(identity,&bytes,result);
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
        return bool(observeStatus(session,signals));
    }
    bool connected() const {std::lock_guard lock(mutex_);return module_!=nullptr;}
    // A coordinator must establish the session before supplying its snapshot.
    // Texture resolution failure suspends reads/clicks without pruning values.
    bool observeSnapshot(uint64_t session,const Snapshot& snapshot) {
        const auto catalog=signalSettingsCatalog(snapshot);
        if(!catalog){
            suspendReads();
            const auto& game=snapshot.getGameSession();
            reportSynchronization({SyncReason::MissingCatalog},game?&*game:nullptr);
            return false;
        }
        return observe(session,*catalog);
    }
    // Serialized worker: latest settings belong to a world, not a save revision.
    // Loading an older save keeps checkbox edits. Full IDs and texture catalog
    // filter deleted signals before imported settings become accessible.
    bool synchronize(const Snapshot& snapshot) {
        const auto& game=snapshot.getGameSession();
        if(!game){suspendReads();reportSynchronization({SyncReason::MissingGame},nullptr);return false;}
        if(!game->generation){suspendReads();reportSynchronization({SyncReason::MissingGeneration},&*game);return false;}
        const auto catalog=signalSettingsCatalog(snapshot);
        if(!catalog){suspendReads();reportSynchronization({SyncReason::MissingCatalog},&*game);return false;}
        return synchronize(*game,*catalog);
    }
    bool synchronize(const GameSession& game,std::span<const SignalSettingsStore::Signal> catalog) {
        if(!game.generation){suspendReads();reportSynchronization({SyncReason::MissingGeneration,0,catalog.size()},&game);return false;}
        if(!observedGame_||*observedGame_!=game){
            checkpoint(true);
            const auto path=profilePath(game.worldId,panelId_);
            auto saved=SignalSettingsFile::load(path);
            const auto original=saved?SignalSettingsFile::encode(*saved):std::string{};
            // Migration works on an owned copy and completes before the new
            // session is visible. Exceptions preserve the original file and
            // leave reads unavailable rather than accepting partial settings.
            if(saved&&migrate_)for(auto& signal:saved->signals)migrate_(panelId_,signal.values);
            // Encode after migration exactly as the public saved-session path.
            const auto bytes=saved?SignalSettingsFile::encode(*saved):std::string{};
            SyncResult beginResult;
            const auto session=beginSessionStatus(game.worldId,saved?&bytes:nullptr,beginResult);
            if(!session){suspendReads();beginResult.count=catalog.size();reportSynchronization(beginResult,&game);return false;}
            observedGame_=game;observedSession_=session;
            profilePath_=path;lastSaved_=original;
            savedRevision_=0;
            nextSave_={};
        }
        const auto observed=observeStatus(observedSession_,catalog);
        if(!observed){reportSynchronization(observed,&game);return false;}
        if(context_){
            const auto status=context_(owner_,observedSession_,game.generation);
            if(status!=NIMBY_OK){suspendReads();reportSynchronization({SyncReason::ContextRejected,status,catalog.size()},&game);return false;}
        }
        const auto refreshed=refresh(catalog);
        if(!refreshed){suspendReads();reportSynchronization(refreshed,&game);return false;}
        checkpoint(false);
        // Healthy ticks add only one atomic read: no diagnostic lock, clock,
        // formatting or world-identity copy unless a recovery is pending.
        if(syncFailure_.load(std::memory_order_relaxed))reportSynchronization(refreshed,&game);
        return true;
    }
    void suspend() noexcept {
        suspendReads();
        // External coordinators can suspend without a captured snapshot. Do
        // not attribute that failure to the previous world's generation.
        reportSynchronization({SyncReason::ExternalSuspension},nullptr);
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
    enum class SyncReason {
        Ready,MissingGame,MissingGeneration,MissingCatalog,ExternalSuspension,Disconnected,
        InvalidIdentity,BeginSavedUnavailable,BeginRejected,BeginEmptySession,
        ObserveLimit,ObserveRejected,ContextRejected,BatchLimit,BatchRejected,
        BatchSize,BatchVersion,BatchCount,BatchFieldCount,NameUnterminated,
        NameEmpty,NameDuplicate,RowSignal,RowStatus,RowReserved,RowMask,
        RowUnexpectedValues,RowDuplicate,
    };
    // A result owns no game pointers or temporary strings. Transport status is
    // the raw return code; local validation failures use reason/detail instead.
    struct SyncResult {
        SyncReason reason=SyncReason::Ready;
        uint32_t status=NIMBY_OK;
        size_t count=0;
        uint64_t detail=0;
        explicit operator bool()const noexcept{return reason==SyncReason::Ready;}
    };
    static const char* reasonName(SyncReason reason)noexcept {
        switch(reason){
            case SyncReason::Ready:return "ready";
            case SyncReason::MissingGame:return "game_session_absent";
            case SyncReason::MissingGeneration:return "generation_absent";
            case SyncReason::MissingCatalog:return "texture_catalog_unresolved";
            case SyncReason::ExternalSuspension:return "external_suspension";
            case SyncReason::Disconnected:return "bridge_disconnected";
            case SyncReason::InvalidIdentity:return "session_identity_invalid";
            case SyncReason::BeginSavedUnavailable:return "begin_saved_unavailable";
            case SyncReason::BeginRejected:return "begin_session_rejected";
            case SyncReason::BeginEmptySession:return "begin_session_empty";
            case SyncReason::ObserveLimit:return "observe_count_limit";
            case SyncReason::ObserveRejected:return "observe_rejected";
            case SyncReason::ContextRejected:return "context_rejected";
            case SyncReason::BatchLimit:return "read_batch_count_limit";
            case SyncReason::BatchRejected:return "read_batch_rejected";
            case SyncReason::BatchSize:return "read_batch_header_size";
            case SyncReason::BatchVersion:return "read_batch_header_version";
            case SyncReason::BatchCount:return "read_batch_header_count";
            case SyncReason::BatchFieldCount:return "read_batch_field_count";
            case SyncReason::NameUnterminated:return "read_batch_name_unterminated";
            case SyncReason::NameEmpty:return "read_batch_name_empty";
            case SyncReason::NameDuplicate:return "read_batch_name_duplicate";
            case SyncReason::RowSignal:return "read_batch_row_signal";
            case SyncReason::RowStatus:return "read_batch_row_status";
            case SyncReason::RowReserved:return "read_batch_row_reserved";
            case SyncReason::RowMask:return "read_batch_row_mask";
            case SyncReason::RowUnexpectedValues:return "read_batch_row_unavailable_values";
            case SyncReason::RowDuplicate:return "read_batch_row_duplicate";
        }
        return "unknown";
    }
    void suspendReads() noexcept {
        std::lock_guard lock(mutex_);
        cached_=false;cache_.clear();
        if(module_)suspend_(owner_);
    }
    uint64_t beginSessionStatus(std::string_view identity,const std::string* saved,SyncResult& result) {
        std::lock_guard lock(mutex_);uint64_t session{};
        cached_=false;cache_.clear();
        if(!module_){result.reason=SyncReason::Disconnected;return 0;}
        if(saved&&!beginSaved_){result.reason=SyncReason::BeginSavedUnavailable;return 0;}
        if(identity.empty()||identity.size()>512){result.reason=SyncReason::InvalidIdentity;return 0;}
        const auto status=saved?beginSaved_(owner_,identity.data(),static_cast<uint32_t>(identity.size()),
            saved->data(),static_cast<uint32_t>(saved->size()),&session):
            begin_(owner_,identity.data(),static_cast<uint32_t>(identity.size()),&session);
        if(status!=NIMBY_OK){result={SyncReason::BeginRejected,status};return 0;}
        if(!session)result.reason=SyncReason::BeginEmptySession;
        return session;
    }
    SyncResult observeStatus(uint64_t session,std::span<const SignalSettingsStore::Signal> signals) {
        if(signals.size()>1000000)return {SyncReason::ObserveLimit,0,signals.size()};
        std::vector<NimbyUiSignalV1> wire(signals.size());
        for(size_t i=0;i<signals.size();++i){wire[i].id=signals[i].id;copy(wire[i].texture_set,signals[i].textureSet);}
        std::lock_guard lock(mutex_);
        cached_=false; // Reuse allocations only after a fresh validated batch.
        if(!module_)return {SyncReason::Disconnected,0,signals.size()};
        const auto status=observe_(owner_,session,wire.data(),static_cast<uint32_t>(wire.size()));
        return {status==NIMBY_OK?SyncReason::Ready:SyncReason::ObserveRejected,status,signals.size()};
    }
    // Selection and state updates are bounded and mutex-protected; formatting
    // and the diagnostics sink run after unlocking. Counts/details may change
    // every tick, so they do not defeat the repeat interval for one failure.
    void reportSynchronization(const SyncResult& result,const GameSession* game,
            std::chrono::steady_clock::time_point now={})noexcept {
        if(result&&!syncFailure_.load(std::memory_order_relaxed))return;
        if(now==std::chrono::steady_clock::time_point{})now=std::chrono::steady_clock::now();
        try {
            using Sink=void(*)(const char*,const char*,const char*)noexcept;
            Sink sink{};std::array<char,129> panel{};std::array<char,65> world{};
            std::array<char,257> textures{};
            if(game)diagnosticToken(world,game->worldId);
            uint64_t owner{},session{},generation=game?game->generation:0;
            bool recovered=false;
            {
                std::lock_guard lock(mutex_);
                owner=owner_;session=observedSession_;
                if(result){
                    if(!syncFailure_.load(std::memory_order_relaxed))return;
                    recovered=true;syncFailure_.store(false,std::memory_order_relaxed);
                }else{
                    const bool same=syncFailure_.load(std::memory_order_relaxed)&&syncReason_==result.reason&&syncStatus_==result.status&&
                        syncOwner_==owner&&syncSession_==session&&syncGeneration_==generation&&syncWorld_==world;
                    if(same&&now>=syncReported_&&now-syncReported_<std::chrono::seconds(5))return;
                    syncFailure_.store(true,std::memory_order_relaxed);syncReason_=result.reason;syncStatus_=result.status;
                    syncOwner_=owner;syncSession_=session;syncGeneration_=generation;syncWorld_=world;syncReported_=now;
                }
                diagnosticToken(panel,panelId_);
                diagnosticToken(textures,textureSet_);
                sink=syncDiagnosticSink_;
            }
            std::array<char,1024> message{};
            std::snprintf(message.data(),message.size(),
                "Signal settings synchronization %s panel=%s owner=%llu session=%llu generation=%llu world=%s textures=%s reason=%s status=%u count=%zu detail=%llu",
                recovered?"recovered":"unavailable",panel.data(),static_cast<unsigned long long>(owner),
                static_cast<unsigned long long>(session),static_cast<unsigned long long>(generation),world.data(),
                textures.data(),reasonName(result.reason),result.status,result.count,static_cast<unsigned long long>(result.detail));
            sink("mods",recovered?"INFO":"WARN",message.data());
        }catch(...){/* Diagnostics must never change synchronization behavior. */}
    }
    template<size_t N> static void diagnosticToken(std::array<char,N>& output,std::string_view value)noexcept {
        const auto count=std::min(value.size(),N-1);
        for(size_t i=0;i<count;++i){const auto c=static_cast<unsigned char>(value[i]);output[i]=c<=32||c==127?'_':char(c);}
        output[count]=0;
    }
    SyncResult refresh(std::span<const SignalSettingsStore::Signal> catalog) {
        std::lock_guard lock(mutex_);
        if(!readBatch_)return {}; // Older resident bridges retain their ABI.
        cached_=false;
        std::vector<uint64_t> ids;
        for(const auto& signal:catalog)if(signal.textureSet==textureSet_)ids.push_back(signal.id);
        if(ids.size()>16384)return {SyncReason::BatchLimit,0,ids.size()};
        NimbyUiReadBatchHeaderV1 header{};header.size=sizeof header;header.version=1;
        std::vector<NimbyUiReadBatchRowV1> rows(ids.size());
        const auto status=readBatch_(owner_,ids.data(),static_cast<uint32_t>(ids.size()),&header,rows.data());
        if(status!=NIMBY_OK)return {SyncReason::BatchRejected,status,ids.size()};
        if(header.size!=sizeof header)return {SyncReason::BatchSize,status,ids.size(),header.size};
        if(header.version!=1)return {SyncReason::BatchVersion,status,ids.size(),header.version};
        if(header.count!=ids.size())return {SyncReason::BatchCount,status,ids.size(),header.count};
        if(header.field_count>64)return {SyncReason::BatchFieldCount,status,ids.size(),header.field_count};
        std::vector<std::string> names;names.reserve(header.field_count);
        std::set<std::string> unique;
        for(uint32_t i=0;i<header.field_count;++i){
            const auto end=static_cast<const char*>(std::memchr(header.names[i],0,sizeof header.names[i]));
            if(!end)return {SyncReason::NameUnterminated,status,ids.size(),i};
            if(end==header.names[i])return {SyncReason::NameEmpty,status,ids.size(),i};
            names.emplace_back(header.names[i],static_cast<size_t>(end-header.names[i]));
            if(!unique.insert(names.back()).second)return {SyncReason::NameDuplicate,status,ids.size(),i};
        }
        const bool sameSchema=names==cacheNames_;
        std::map<uint64_t,CachedSettings> next;
        for(size_t i=0;i<rows.size();++i){const auto& row=rows[i];
            if(row.signal!=ids[i])return {SyncReason::RowSignal,status,ids.size(),i};
            if(row.status>2)return {SyncReason::RowStatus,status,ids.size(),i};
            if(row.reserved)return {SyncReason::RowReserved,status,ids.size(),i};
            if(header.field_count<64&&(row.values>>header.field_count))return {SyncReason::RowMask,status,ids.size(),i};
            if(row.status!=2&&row.values)return {SyncReason::RowUnexpectedValues,status,ids.size(),i};
            if(next.contains(row.signal))return {SyncReason::RowDuplicate,status,ids.size(),i};
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
        cache_=std::move(next);cacheNames_=std::move(names);cached_=true;return {SyncReason::Ready,0,ids.size()};
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
        syncFailure_.store(false,std::memory_order_relaxed);
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
    // The fast path only reads this flag; diagnostic detail stays under mutex_.
    // Relaxed ordering is sufficient because it never publishes other fields.
    std::atomic<bool> syncFailure_{false};
    SyncReason syncReason_=SyncReason::Ready;
    uint32_t syncStatus_=NIMBY_OK;
    uint64_t syncOwner_=0,syncSession_=0,syncGeneration_=0;
    std::array<char,65> syncWorld_{};
    std::chrono::steady_clock::time_point syncReported_{};
    void (*syncDiagnosticSink_)(const char*,const char*,const char*)noexcept=diagnostics::write;
};
}
