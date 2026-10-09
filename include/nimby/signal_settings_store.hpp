#pragma once
#include <nimby/signal_settings.hpp>
#include <cstdint>
#include <mutex>
#include <set>
#include <stdexcept>
#include <vector>
#include <memory>
#include <algorithm>
#include <atomic>
#include <nimby/detail/number_input_draft.hpp>

namespace nimby {
// SDK-owned model for the injected panel. It never invokes native UI functions.
// Session identity must come from the game/save integration, never just a PID.
class SignalSettingsStore {
public:
    struct Checkbox { std::string name,label,description;bool defaultValue=false,onlyWhenEnabled=false; };
    struct Number {std::string name,label,visibleWhen;uint32_t bits=0,maximum=0;};
    struct Panel { std::string id,title,textureSet;std::vector<Checkbox> checkboxes; };
    struct Signal { uint64_t id;std::string textureSet; };
    struct Editor {
        uint64_t session=0,selection=0,signal=0;
        bool operator==(const Editor&) const = default;
    };
    struct SavedSignal { uint64_t id;std::map<std::string,bool,std::less<>> values; };
    struct SavedSettings { std::string sessionId,panelId;std::vector<SavedSignal> signals; };
    struct Copy { uint64_t session;std::map<std::string,bool,std::less<>> values; };
    // Freeze effective values, including defaults and explicit false values.
    std::optional<Copy> copySource(uint64_t source) const {
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock)throw std::runtime_error("Signal settings busy; retry copying");
        if(!signals_.contains(source))return {};
        if(!observed_)throw std::runtime_error("Source signal settings temporarily unavailable");
        Copy copy{session_,{}};
        for(const auto& box:panel_.checkboxes)copy.values.emplace(box.name,box.defaultValue);
        if(const auto saved=values_.find(source);saved!=values_.end())
            for(const auto& [name,value]:saved->second)if(copy.values.contains(name))copy.values[name]=value;
        return copy;
    }
    bool queueCopies(const Copy& copy,std::span<const uint64_t> targets) {
        std::lock_guard lock(mutex_);
        if(!current(copy.session)||targets.size()>64)return false;
        for(auto id:targets)if(id>>48!=8)return false;
        if(values_.size()+pendingCopies_.size()+targets.size()>16384)return false;
        if(!targets.empty())changed();
        for(auto id:targets)pendingCopies_[id]=copy.values;
        return true;
    }
    // Owned frame data: native layout and interactive drawing must use the same
    // fields, even if the observation worker invalidates the session meanwhile.
    struct Control { Checkbox checkbox;bool value=false; };
    struct NumberControl {Number field;int value=0;std::shared_ptr<detail::NumberInputDraft> draft;};
    struct Frame { Editor editor;std::string panelId,title;std::vector<Control> controls;bool available=true;std::vector<NumberControl> numbers;uint64_t presentationRevision=0; };

    void configure(const SignalSettingsPanel& source) {
        Panel candidate;
        if(!source.id.empty()) {
            checkText(source.id,128);checkText(source.title,256);checkText(source.textureSet,256);
            if(source.checkboxes.size()>64)throw std::invalid_argument("Invalid checkbox count");
            candidate={std::string(source.id),std::string(source.title),std::string(source.textureSet),{}};
            std::set<std::string_view> names;
            for(const auto& box:source.checkboxes){
                checkText(box.name,128);checkText(box.label,256);checkText(box.description,1024,true);
                if(!names.insert(box.name).second)throw std::invalid_argument("Duplicate checkbox name");
                candidate.checkboxes.push_back({std::string(box.name),std::string(box.label),std::string(box.description),box.defaultValue,box.onlyWhenEnabled});
            }
        }else if(!source.title.empty()||!source.textureSet.empty()||!source.checkboxes.empty())
            throw std::invalid_argument("Missing settings panel ID");
        std::lock_guard lock(mutex_);
        if(retired_)throw std::logic_error("Settings panel removed");
        panel_=std::move(candidate);invalidate();
        numbers_.clear();
        configureNumbersLocked(source.numbers);
    }
    bool configureNumbers(std::span<const SignalNumber> fields){
        std::lock_guard lock(mutex_);if(retired_||!sessionId_.empty())return false;
        configureNumbersLocked(fields);return true;
    }
    Panel panel() const {std::lock_guard lock(mutex_);return panel_;}
    enum class SignalState { Known, Unknown, Busy };
    enum class SignalStateReason { None, Busy, Retired, SessionMissing, ObservationsSuspended, WorldChanged, EpochChanged, SignalMissing };
    struct SignalStateDetails {SignalStateReason reason=SignalStateReason::None;uint64_t epoch=0;};
    uint64_t observationEpoch()const{return observationEpoch_.load(std::memory_order_acquire);}
    SignalState signalState(uint64_t signal,bool requireObserved=false,std::string_view world={},uint64_t epoch=0,SignalStateDetails* details=nullptr)const {
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock){if(details)*details={SignalStateReason::Busy,observationEpoch()};return SignalState::Busy;}
        const auto currentEpoch=(epoch||details)?observationEpoch():0;
        if(details)*details={SignalStateReason::None,currentEpoch};
        const auto unknown=[&](SignalStateReason reason){if(details)details->reason=reason;return SignalState::Unknown;};
        if(retired_)return unknown(SignalStateReason::Retired);
        if(sessionId_.empty())return unknown(SignalStateReason::SessionMissing);
        if(requireObserved&&!observed_)return unknown(SignalStateReason::ObservationsSuspended);
        if(!world.empty()&&sessionId_!=world)return unknown(SignalStateReason::WorldChanged);
        if(epoch&&epoch!=currentEpoch)return unknown(SignalStateReason::EpochChanged);
        if(!signals_.contains(signal))return unknown(SignalStateReason::SignalMissing);
        return SignalState::Known;
    }
    bool knowsSignal(uint64_t signal,bool requireObserved=false)const{return signalState(signal,requireObserved)==SignalState::Known;}
    // Runtime context only; this does not serialize a profile or copy all values.
    std::optional<std::string> sessionIdentity(uint64_t expected)const {
        std::lock_guard lock(mutex_);if(!current(expected))return {};
        return sessionId_;
    }
    std::optional<uint64_t> settingsRevision(uint64_t expected)const {
        std::lock_guard lock(mutex_);if(!current(expected))return {};
        return revision_;
    }
    // Only before opening a session: changing layout rules in an active editor
    // would invalidate the correspondence between layout and interactive passes.
    bool conditionalVisibility(uint64_t mask) {
        std::lock_guard lock(mutex_);
        if(retired_||!sessionId_.empty()||(panel_.checkboxes.size()<64&&(mask>>panel_.checkboxes.size())))return false;
        for(size_t i=0;i<panel_.checkboxes.size();++i)panel_.checkboxes[i].onlyWhenEnabled=(mask&(uint64_t{1}<<i))!=0;
        return true;
    }

    // Explicit transitions invalidate all outstanding UI events, even reopening
    // the same save. Import is transactional and matched to save + panel IDs.
    uint64_t beginSession(std::string_view identity,const SavedSettings* saved=nullptr) {
        checkText(identity,512);
        std::lock_guard lock(mutex_);
        if(retired_||panel_.id.empty())throw std::logic_error("Settings panel unavailable");
        std::map<uint64_t,std::map<std::string,bool,std::less<>>> candidate;
        if(saved){
            if(saved->sessionId!=identity||saved->panelId!=panel_.id||saved->signals.size()>16384)
                throw std::invalid_argument("Settings belong to another session or panel");
            for(const auto& signal:saved->signals){
                if(signal.id>>48!=8||signal.values.size()>64||!candidate.emplace(signal.id,std::map<std::string,bool,std::less<>>{}).second)
                    throw std::invalid_argument("Invalid saved signal settings");
                for(const auto& [name,value]:signal.values){checkText(name,128);
                    for(const auto& box:panel_.checkboxes)if(box.name==name)candidate.at(signal.id).emplace(name,value);
                }
            }
        }
        invalidate();sessionId_=identity;values_=std::move(candidate);return session_;
    }
    void endSession(){std::lock_guard lock(mutex_);invalidate();}
    // Unregister is permanent. An operation that retained this store before
    // removal cannot reopen it after the owner disappears from the registry.
    void retire(){std::lock_guard lock(mutex_);retired_=true;invalidate();}
    // A transient capture failure must disable reads/clicks without discarding
    // the user's values. The next complete observation can restore availability.
    void suspendObservations(){std::lock_guard lock(mutex_);invalidateObservation();observed_=false;editor_={};}

    // Caller supplies a complete, coherent set for this session. Unknown or
    // partial topology must not be passed as an empty list (which means deletion).
    bool observeSignals(uint64_t session,std::span<const Signal> signals) {
        if(signals.size()>1000000)throw std::invalid_argument("Signal list too large");
        std::set<uint64_t> all,matching;
        std::lock_guard lock(mutex_);
        if(!current(session))return false;
        for(const auto& signal:signals){
            if(signal.id>>48!=8||!all.insert(signal.id).second)throw std::invalid_argument("Invalid signal identity");
            if(signal.textureSet==panel_.textureSet)matching.insert(signal.id);
        }
        // A capture started before native construction can arrive afterwards.
        // Keep copies pending until their full IDs appear in this catalogue.
        for(auto i=pendingCopies_.begin();i!=pendingCopies_.end();){
            if(matching.contains(i->first)){
                changed();
                values_[i->first]=i->second;i=pendingCopies_.erase(i);
            }else if(all.contains(i->first)){changed();i=pendingCopies_.erase(i);}
            else ++i;
        }
        for(auto i=values_.begin();i!=values_.end();) {
            if(!matching.contains(i->first)){changed();i=values_.erase(i);}else ++i;
        }
        // Suspension clears editor_, and a signal using only defaults has no
        // values_ entry. Detect removed membership independently of both so a
        // later busy frame cannot revive an actually deleted signal's panel.
        if(!std::includes(matching.begin(),matching.end(),signals_.begin(),signals_.end())){invalidatePresentation();invalidateObservation();}
        if(!matching.contains(editor_.signal))editor_={};
        signals_=std::move(matching);observed_=true;return true;
    }
    Editor selectSignal(uint64_t session,uint64_t signal) {
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return {};
        if(!current(session)||!observed_||!signals_.contains(signal)){editor_={};return {};}
        if(editor_.session!=session_||editor_.signal!=signal)editor_={session_,++selection_,signal};
        return editor_;
    }
    std::optional<Frame> frame(Editor editor) const {
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return {};
        return frameLocked(editor);
    }
    bool accepts(Editor editor)const {
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return false;
        return current(editor.session)&&observed_&&editor.signal&&editor==editor_&&signals_.contains(editor.signal);
    }
    // UI host selects and snapshots under one lock; it never guesses the
    // current session token or keeps a reference into mod-owned declarations.
    std::optional<Frame> selectFrame(uint64_t signal,bool* busy=nullptr) {
        // The game's UI never waits behind a worker importing a large profile.
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(busy)*busy=!lock;
        if(!lock)return std::nullopt;
        if(sessionId_.empty()||!signals_.contains(signal)){editor_={};return std::nullopt;}
        if(editor_.session!=session_||editor_.signal!=signal)editor_={session_,++selection_,signal};
        // Last known controls remain visible during a transient read failure.
        // This presentation is not an observation and cannot authorize writes.
        return frameLocked(editor_,true);
    }
    // A busy store is not an absent panel. The UI may retain its last copied
    // shape, disabled, only until a value/structure/session mutation begins.
    // This atomic is not permission to edit: every write still takes the store
    // lock and checks the complete editor identity and observation state.
    bool canRetainFrame(const Frame& frame)const noexcept {
        return frame.presentationRevision&&frame.presentationRevision==presentationRevision_.load(std::memory_order_acquire);
    }
private:
    std::optional<Frame> frameLocked(Editor editor,bool presentationOnly=false) const {
        if(!current(editor.session)||(!observed_&&!presentationOnly)||!editor.signal||editor!=editor_||!signals_.contains(editor.signal))
            return std::nullopt;
        Frame result{editor,panel_.id,panel_.title,{},observed_,{},presentationRevision_.load(std::memory_order_relaxed)};
        if(numberSignal_!=editor.signal){numberDrafts_.clear();numberSignal_=editor.signal;}
        const auto saved=values_.find(editor.signal);
        const auto valueOf=[&](std::string_view name){
            if(saved!=values_.end())if(auto i=saved->second.find(name);i!=saved->second.end())return i->second;
            for(const auto& box:panel_.checkboxes)if(box.name==name)return box.defaultValue;
            return false;
        };
        std::set<std::string> hidden;
        for(const auto& field:numbers_){
            int value=0;
            for(uint32_t bit=0;bit<field.bits;++bit){auto key=numberKey(field.name,bit);hidden.insert(key);if(valueOf(key))value|=1<<bit;}
            if(!field.visibleWhen.empty()&&!valueOf(field.visibleWhen))continue;
            value=std::min(value,int(field.maximum));
            auto& draft=numberDrafts_[field.name];if(!draft)draft=std::make_shared<detail::NumberInputDraft>();
            draft->synchronize(value);result.numbers.push_back({field,value,draft});
        }
        for(const auto& box:panel_.checkboxes){
            if(hidden.contains(box.name))continue;
            bool value=box.defaultValue;
            if(saved!=values_.end()){
                const auto field=saved->second.find(box.name);
                if(field!=saved->second.end())value=field->second;
            }
            if(!box.onlyWhenEnabled||value)result.controls.push_back({box,value});
        }
        return result;
    }
public:
    bool setNumber(Editor editor,std::string_view name,int value){
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return false;
        if(!current(editor.session)||!observed_||editor!=editor_||!signals_.contains(editor.signal))return false;
        for(const auto& field:numbers_)if(field.name==name){
            if(value<0||uint32_t(value)>field.maximum)return false;
            if(!field.visibleWhen.empty()){
                bool enabled=false;for(const auto& box:panel_.checkboxes)if(box.name==field.visibleWhen)enabled=box.defaultValue;
                if(auto s=values_.find(editor.signal);s!=values_.end())if(auto v=s->second.find(field.visibleWhen);v!=s->second.end())enabled=v->second;
                if(!enabled)return false;
            }
            if(!values_.contains(editor.signal)&&values_.size()>=16384)throw std::length_error("Settings capacity exceeded");
            const auto previous=values_.find(editor.signal);
            bool identical=previous!=values_.end();
            if(identical)for(uint32_t bit=0;bit<field.bits;++bit){
                const auto found=previous->second.find(numberKey(field.name,bit));
                if(found==previous->second.end()||found->second!=bool(value&(1<<bit))){identical=false;break;}
            }
            if(identical)return true;
            changed();
            auto& saved=values_[editor.signal];
            for(uint32_t bit=0;bit<field.bits;++bit)saved[numberKey(field.name,bit)]=(value&(1<<bit))!=0;
            return true;
        }
        return false;
    }
    bool setBoolean(Editor editor,std::string_view name,bool value) {
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return false;
        if(!current(editor.session)||!observed_||!editor.signal||editor!=editor_||!signals_.contains(editor.signal))return false;
        for(const auto& box:panel_.checkboxes)if(box.name==name){
            if(!values_.contains(editor.signal)&&values_.size()>=16384)throw std::length_error("Settings capacity exceeded");
            if(const auto previous=values_.find(editor.signal);previous!=values_.end())
                if(const auto field=previous->second.find(name);field!=previous->second.end()&&field->second==value)return true;
            changed();
            values_[editor.signal][box.name]=value;return true;
        }
        return false;
    }
    SignalSettings read(uint64_t signal) const {
        std::lock_guard lock(mutex_);
        if(sessionId_.empty()||!observed_)return {};
        SignalSettings result;
        if(!signals_.contains(signal)){result.status=SettingsStatus::Absent;return result;}
        result.status=SettingsStatus::Present;
        for(const auto& box:panel_.checkboxes)result.booleans.emplace(box.name,box.defaultValue);
        const auto entry=values_.find(signal);
        if(entry!=values_.end())for(const auto& [name,value]:entry->second)
            if(result.booleans.contains(name))result.booleans[name]=value;
        return result;
    }
    struct SettingsBits {SettingsStatus status=SettingsStatus::Unavailable;uint64_t values{};};
    struct SettingsBatch {std::vector<std::string> fields;std::vector<SettingsBits> rows;};
    SettingsBatch readBatch(std::span<const uint64_t> signals) const {
        std::lock_guard lock(mutex_);
        SettingsBatch batch;batch.rows.resize(signals.size());
        std::vector<const Checkbox*> fields;fields.reserve(panel_.checkboxes.size());
        for(const auto& field:panel_.checkboxes)fields.push_back(&field);
        std::sort(fields.begin(),fields.end(),[](const auto* a,const auto* b){return a->name<b->name;});
        uint64_t defaults{};batch.fields.reserve(fields.size());
        for(size_t i=0;i<fields.size();++i){batch.fields.push_back(fields[i]->name);if(fields[i]->defaultValue)defaults|=uint64_t{1}<<i;}
        if(retired_||sessionId_.empty()||!observed_)return batch;
        for(size_t i=0;i<signals.size();++i){auto& row=batch.rows[i];
            row.status=signals_.contains(signals[i])?SettingsStatus::Present:SettingsStatus::Absent;
            if(row.status!=SettingsStatus::Present)continue;
            row.values=defaults;
            const auto saved=values_.find(signals[i]);if(saved==values_.end())continue;
            for(const auto& [name,value]:saved->second){
                const auto field=std::lower_bound(batch.fields.begin(),batch.fields.end(),name);
                if(field==batch.fields.end()||*field!=name)continue;
                const auto mask=uint64_t{1}<<std::distance(batch.fields.begin(),field);
                if(value)row.values|=mask;else row.values&=~mask;
            }
        }
        return batch;
    }
    SavedSettings save(uint64_t expectedSession=0) const {
        std::lock_guard lock(mutex_);
        if(expectedSession && !current(expectedSession))throw std::invalid_argument("Settings session changed");
        if(sessionId_.empty())throw std::logic_error("No settings session");
        SavedSettings saved{sessionId_,panel_.id,{}};
        for(const auto& [id,values]:values_)saved.signals.push_back({id,values});
        for(const auto& [id,values]:pendingCopies_)if(!values_.contains(id))saved.signals.push_back({id,values});
        return saved;
    }
private:
    static std::string numberKey(std::string_view name,uint32_t bit){return "nrf.number."+std::string(name)+"."+std::to_string(bit);}
    void configureNumbersLocked(std::span<const SignalNumber> fields){
        if(fields.size()>4)throw std::invalid_argument("Too many numeric settings");
        std::vector<Number> candidate;std::set<std::string_view> names;
        for(const auto& field:fields){
            checkText(field.name,96);checkText(field.label,256);checkText(field.visibleWhen,128,true);
            if(!names.insert(field.name).second||field.bits<1||field.bits>16||!field.maximum||field.maximum>65535||field.maximum>=(1u<<field.bits))throw std::invalid_argument("Invalid numeric setting");
            for(uint32_t bit=0;bit<field.bits;++bit){const auto key=numberKey(field.name,bit);
                if(std::none_of(panel_.checkboxes.begin(),panel_.checkboxes.end(),[&](const auto& box){return box.name==key;}))throw std::invalid_argument("Missing numeric storage field");}
            if(!field.visibleWhen.empty()&&std::none_of(panel_.checkboxes.begin(),panel_.checkboxes.end(),[&](const auto& box){return box.name==field.visibleWhen;}))throw std::invalid_argument("Missing numeric visibility setting");
            candidate.push_back({std::string(field.name),std::string(field.label),std::string(field.visibleWhen),field.bits,field.maximum});
        }
        numbers_=std::move(candidate);
    }
    static void checkText(std::string_view text,size_t limit,bool allowEmpty=false) {
        if((text.empty()&&!allowEmpty)||text.size()>limit||text.find('\0')!=std::string_view::npos)
            throw std::invalid_argument("Invalid settings text");
    }
    bool current(uint64_t session)const{return !retired_&&session && session==session_ && !sessionId_.empty();}
    void invalidatePresentation(){presentationRevision_.fetch_add(1,std::memory_order_release);}
    void invalidateObservation(){observationEpoch_.fetch_add(1,std::memory_order_release);}
    void changed(){invalidatePresentation();if(!++revision_)++revision_;}
    void invalidate(){changed();invalidateObservation();++session_;editor_={};sessionId_.clear();observed_=false;signals_.clear();values_.clear();pendingCopies_.clear();numberDrafts_.clear();numberSignal_=0;}
    mutable std::mutex mutex_;
    Panel panel_;
    std::vector<Number> numbers_;
    mutable uint64_t numberSignal_=0;
    mutable std::map<std::string,std::shared_ptr<detail::NumberInputDraft>> numberDrafts_;
    uint64_t session_=0,selection_=0;
    uint64_t revision_=0;
    std::atomic<uint64_t> presentationRevision_{1};
    std::atomic<uint64_t> observationEpoch_{1};
    std::string sessionId_;
    bool observed_=false,retired_=false;
    Editor editor_;
    std::set<uint64_t> signals_;
    std::map<uint64_t,std::map<std::string,bool,std::less<>>> values_;
    std::map<uint64_t,std::map<std::string,bool,std::less<>>> pendingCopies_;
};
}
