#pragma once
#include <nimby/signal_settings.hpp>
#include <cstdint>
#include <mutex>
#include <set>
#include <stdexcept>
#include <vector>

namespace nimby {
// SDK-owned model for the injected panel. It never invokes native UI functions.
// Session identity must come from the game/save integration, never just a PID.
class SignalSettingsStore {
public:
    struct Checkbox { std::string name,label,description;bool defaultValue=false; };
    struct Panel { std::string id,title,textureSet;std::vector<Checkbox> checkboxes; };
    struct Signal { uint64_t id;std::string textureSet; };
    struct Editor {
        uint64_t session=0,selection=0,signal=0;
        bool operator==(const Editor&) const = default;
    };
    struct SavedSignal { uint64_t id;std::map<std::string,bool,std::less<>> values; };
    struct SavedSettings { std::string sessionId,panelId;std::vector<SavedSignal> signals; };
    // Owned frame data: native layout and interactive drawing must use the same
    // fields, even if the observation worker invalidates the session meanwhile.
    struct Control { Checkbox checkbox;bool value=false; };
    struct Frame { Editor editor;std::string panelId,title;std::vector<Control> controls; };

    void configure(const SignalSettingsPanel& source) {
        Panel candidate;
        if(!source.id.empty()) {
            checkText(source.id,128);checkText(source.title,256);checkText(source.textureSet,256);
            if(source.checkboxes.empty()||source.checkboxes.size()>64)throw std::invalid_argument("Invalid checkbox count");
            candidate={std::string(source.id),std::string(source.title),std::string(source.textureSet),{}};
            std::set<std::string_view> names;
            for(const auto& box:source.checkboxes){
                checkText(box.name,128);checkText(box.label,256);checkText(box.description,1024,true);
                if(!names.insert(box.name).second)throw std::invalid_argument("Duplicate checkbox name");
                candidate.checkboxes.push_back({std::string(box.name),std::string(box.label),std::string(box.description),box.defaultValue});
            }
        }else if(!source.title.empty()||!source.textureSet.empty()||!source.checkboxes.empty())
            throw std::invalid_argument("Missing settings panel ID");
        std::lock_guard lock(mutex_);
        panel_=std::move(candidate);invalidate();
    }
    Panel panel() const {std::lock_guard lock(mutex_);return panel_;}

    // Explicit transitions invalidate all outstanding UI events, even reopening
    // the same save. Import is transactional and matched to save + panel IDs.
    uint64_t beginSession(std::string_view identity,const SavedSettings* saved=nullptr) {
        checkText(identity,512);
        std::lock_guard lock(mutex_);
        if(panel_.id.empty())throw std::logic_error("Settings panel not configured");
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
    // A transient capture failure must disable reads/clicks without discarding
    // the user's values. The next complete observation can restore availability.
    void suspendObservations(){std::lock_guard lock(mutex_);observed_=false;editor_={};}

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
        for(auto i=values_.begin();i!=values_.end();) {
            if(!matching.contains(i->first))i=values_.erase(i);else ++i;
        }
        if(!matching.contains(editor_.signal))editor_={};
        signals_=std::move(matching);observed_=true;return true;
    }
    Editor selectSignal(uint64_t session,uint64_t signal) {
        std::lock_guard lock(mutex_);
        if(!current(session)||!observed_||!signals_.contains(signal)){editor_={};return {};}
        if(editor_.session!=session_||editor_.signal!=signal)editor_={session_,++selection_,signal};
        return editor_;
    }
    std::optional<Frame> frame(Editor editor) const {
        std::lock_guard lock(mutex_);
        return frameLocked(editor);
    }
    // UI host selects and snapshots under one lock; it never guesses the
    // current session token or keeps a reference into mod-owned declarations.
    std::optional<Frame> selectFrame(uint64_t signal) {
        std::lock_guard lock(mutex_);
        if(sessionId_.empty()||!observed_||!signals_.contains(signal)){editor_={};return std::nullopt;}
        if(editor_.session!=session_||editor_.signal!=signal)editor_={session_,++selection_,signal};
        return frameLocked(editor_);
    }
private:
    std::optional<Frame> frameLocked(Editor editor) const {
        if(!current(editor.session)||!observed_||!editor.signal||editor!=editor_||!signals_.contains(editor.signal))
            return std::nullopt;
        Frame result{editor,panel_.id,panel_.title,{}};
        const auto saved=values_.find(editor.signal);
        for(const auto& box:panel_.checkboxes){
            bool value=box.defaultValue;
            if(saved!=values_.end()){
                const auto field=saved->second.find(box.name);
                if(field!=saved->second.end())value=field->second;
            }
            result.controls.push_back({box,value});
        }
        return result;
    }
public:
    bool setBoolean(Editor editor,std::string_view name,bool value) {
        std::lock_guard lock(mutex_);
        if(!current(editor.session)||!editor.signal||editor!=editor_||!signals_.contains(editor.signal))return false;
        for(const auto& box:panel_.checkboxes)if(box.name==name){
            if(!values_.contains(editor.signal)&&values_.size()>=16384)throw std::length_error("Settings capacity exceeded");
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
    SavedSettings save(uint64_t expectedSession=0) const {
        std::lock_guard lock(mutex_);
        if(expectedSession && !current(expectedSession))throw std::invalid_argument("Settings session changed");
        if(sessionId_.empty())throw std::logic_error("No settings session");
        SavedSettings saved{sessionId_,panel_.id,{}};
        for(const auto& [id,values]:values_)saved.signals.push_back({id,values});
        return saved;
    }
private:
    static void checkText(std::string_view text,size_t limit,bool allowEmpty=false) {
        if((text.empty()&&!allowEmpty)||text.size()>limit||text.find('\0')!=std::string_view::npos)
            throw std::invalid_argument("Invalid settings text");
    }
    bool current(uint64_t session)const{return session && session==session_ && !sessionId_.empty();}
    void invalidate(){++session_;editor_={};sessionId_.clear();observed_=false;signals_.clear();values_.clear();}
    mutable std::mutex mutex_;
    Panel panel_;
    uint64_t session_=0,selection_=0;
    std::string sessionId_;
    bool observed_=false;
    Editor editor_;
    std::set<uint64_t> signals_;
    std::map<uint64_t,std::map<std::string,bool,std::less<>>> values_;
};
}
