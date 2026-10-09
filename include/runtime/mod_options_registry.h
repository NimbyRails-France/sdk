#pragma once
#include <engine/mod_shortcuts.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nimby::runtime::mod_options {
namespace shortcuts=engine::mod_shortcuts;
enum class Kind {Boolean,Integer,Choice,Shortcut};
struct Choice {std::string id,label;};
struct Field {
    std::string id,label,description;
    Kind kind=Kind::Boolean;
    std::string defaultValue;
    int32_t minimum=0,maximum=0;
    std::vector<Choice> choices;
};
struct Definition {std::string id,title;std::vector<Field> fields;};
struct Entry {
    uint64_t token=0;
    std::string id,title;
    std::vector<Field> fields;
    std::vector<std::string> values;
    uint64_t revision=0;
    // Human-readable conflicting action labels, indexed by this mod's field.
    // The renderer localizes SDK diagnostics and each mod's catalogue refs.
    std::map<std::string,std::string> conflicts;
    // Keep identities separate from translated text: labels from another mod
    // must be resolved using that mod's catalogue, never concatenated refs.
    std::map<std::string,std::pair<uint64_t,std::string>> conflictSources;
};
struct Snapshot {uint64_t revision=0;bool nativeBindingsKnown=false;std::vector<Entry> mods;};
struct NativeBinding {shortcuts::Chord chord;std::string label;bool operator==(const NativeBinding&)const=default;};
enum class Status {Ok,Unchanged,Invalid,Duplicate,Limit,NotFound,Busy,Conflict,NativeUnavailable};
struct Result {
    Status status=Status::Ok;
    uint64_t token=0;
    std::string conflict;
    uint64_t conflictOwner=0;
    std::string conflictField;
    Result(Status status=Status::Ok,uint64_t token=0,std::string conflict={},uint64_t otherOwner=0,std::string otherField={})
        :status(status),token(token),conflict(std::move(conflict)),conflictOwner(otherOwner),conflictField(std::move(otherField)){}
    explicit operator bool()const noexcept{return status==Status::Ok||status==Status::Unchanged;}
};
struct Event {uint64_t sequence=0;std::string field;uint64_t revision=0;};
// SDK-owned notification only. Implementations signal the owning host's event;
// they must never wait or invoke mod code. notify runs after releasing locks.
struct Wake {virtual ~Wake()=default;virtual void notify()const noexcept=0;};

// Persistent values are encoded/written by the surrounding SDK, never here.
// Resident UI reads immutable snapshots; edits and keyboard input use try-lock
// so a worker registering options cannot stall the game's frame/input thread.
class Registry {
public:
    using Clock=std::chrono::steady_clock;
    static constexpr size_t maxMods=64,maxFields=64,maxChoices=16,maxPending=32;
    static constexpr auto eventLifetime=std::chrono::seconds(1);
    static constexpr std::string_view nativeUnavailable="Raccourcis du jeu non vérifiés";
    Registry(){published_.store(std::make_shared<const Snapshot>());}

    std::shared_ptr<const Snapshot> snapshot()const noexcept{return published_.load(std::memory_order_acquire);}
    static bool validId(std::string_view text)noexcept {
        if(text.empty()||text.size()>128||!((text.front()>='A'&&text.front()<='Z')||(text.front()>='a'&&text.front()<='z')))return false;
        for(const unsigned char c:text)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='.'||c=='-'))return false;
        return true;
    }
    static bool validText(std::string_view text,size_t limit,bool empty=false)noexcept {
        if((!empty&&text.empty())||text.size()>limit)return false;
        // Validate complete UTF-8, rejecting embedded NUL, overlong sequences,
        // surrogate codepoints and values outside Unicode's scalar range.
        for(size_t i=0;i<text.size();){
            const auto first=static_cast<unsigned char>(text[i++]);
            if(!first)return false;
            if(first<0x80)continue;
            unsigned count=0;uint32_t value=0,minimum=0;
            if(first>=0xc2&&first<=0xdf){count=1;value=first&0x1f;minimum=0x80;}
            else if(first>=0xe0&&first<=0xef){count=2;value=first&0x0f;minimum=0x800;}
            else if(first>=0xf0&&first<=0xf4){count=3;value=first&7;minimum=0x10000;}
            else return false;
            if(count>text.size()-i)return false;
            while(count--){const auto next=static_cast<unsigned char>(text[i++]);if((next&0xc0)!=0x80)return false;value=(value<<6)|(next&0x3f);}
            if(value<minimum||value>0x10ffff||(value>=0xd800&&value<=0xdfff))return false;
        }
        return true;
    }
    static bool validValue(const Field& field,std::string_view value)noexcept {
        switch(field.kind){
            case Kind::Boolean:return value=="true"||value=="false";
            case Kind::Integer:{
                if(value.empty()||value.size()>11||value.front()=='+'||(value.size()>1&&value.front()=='0')||value=="-0"||value.starts_with("-0"))return false;
                int32_t number{};const auto result=std::from_chars(value.data(),value.data()+value.size(),number);
                return result.ec==std::errc{}&&result.ptr==value.data()+value.size()&&number>=field.minimum&&number<=field.maximum;
            }
            case Kind::Choice:return std::any_of(field.choices.begin(),field.choices.end(),[&](const auto& choice){return choice.id==value;});
            case Kind::Shortcut:return shortcuts::parse(value).has_value();
        }
        return false;
    }
    static bool validDefinition(const Definition& definition){
        if(!validId(definition.id)||!validText(definition.title,256)||definition.fields.empty()||definition.fields.size()>maxFields)return false;
        std::unordered_set<std::string_view> ids;
        for(const auto& field:definition.fields){
            if(!validId(field.id)||!ids.insert(field.id).second||!validText(field.label,256)||!validText(field.description,1024,true))return false;
            if(field.kind==Kind::Integer&&field.minimum>field.maximum)return false;
            if(field.kind==Kind::Choice){
                if(field.choices.size()<2||field.choices.size()>maxChoices)return false;
                std::unordered_set<std::string_view> choices;
                for(const auto& choice:field.choices)if(!validId(choice.id)||!choices.insert(choice.id).second||!validText(choice.label,256))return false;
            }else if(!field.choices.empty())return false;
            if(!validValue(field,field.defaultValue))return false;
        }
        return true;
    }

    // owner is assigned by the trusted broker, never supplied by a mod. One
    // loaded mod owns one registration; tokens are never reused after removal.
    // Invalid saved values must be replaced by defaults by the storage codec.
    Result add(uint64_t owner,Definition definition,std::vector<std::string> values={}){
        if(!owner||!validDefinition(definition))return {Status::Invalid};
        if(values.empty()){values.reserve(definition.fields.size());for(const auto& field:definition.fields)values.push_back(field.defaultValue);}
        if(values.size()!=definition.fields.size())return {Status::Invalid};
        for(size_t i=0;i<values.size();++i)if(!validValue(definition.fields[i],values[i]))return {Status::Invalid};
        std::lock_guard lock(mutex_);
        if(records_.size()>=maxMods)return {Status::Limit};
        for(const auto& [token,record]:records_)if(record.owner==owner||record.entry.id==definition.id)return {Status::Duplicate};
        const auto token=++serial_;
        Record record;record.owner=owner;record.entry={token,std::move(definition.id),std::move(definition.title),std::move(definition.fields),std::move(values),0,{},{}};
        records_.emplace(token,std::move(record));
        try{rebuildLocked();}catch(...){records_.erase(token);throw;}
        return {Status::Ok,token};
    }
    // Optional finalEntry captures the last committed values atomically with
    // removal. The broker persists it outside this lock; a preceding UI edit
    // cannot be lost to a delayed background flush or unload race.
    Result remove(uint64_t token,Entry* finalEntry=nullptr){
        std::shared_ptr<const Wake> retired;
        std::optional<Entry> final;
        {
            std::lock_guard lock(mutex_);const auto found=records_.find(token);if(found==records_.end())return {Status::NotFound,token};
            if(finalEntry)final=found->second.entry;
            auto removed=records_.extract(found);
            try{rebuildLocked();}catch(...){records_.insert(std::move(removed));throw;}
            retired=std::move(removed.mapped().wake);
        }
        if(finalEntry)*finalEntry=std::move(*final);
        return {Status::Ok,token};
    }
    Result setWake(uint64_t token,std::shared_ptr<const Wake> wake){
        {
            std::lock_guard lock(mutex_);const auto found=records_.find(token);if(found==records_.end())return {Status::NotFound,token};
            found->second.wake.swap(wake);
        }
        return {Status::Ok,token};
    }
    Result change(uint64_t token,std::string_view field,std::string value){
        std::shared_ptr<const Wake> wake;
        {
            std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return {Status::Busy,token};
            const auto found=records_.find(token);if(found==records_.end())return {Status::NotFound,token};
            auto& entry=found->second.entry;const auto index=fieldIndex(entry,field);if(!index)return {Status::NotFound,token};
            if(!validValue(entry.fields[*index],value))return {Status::Invalid,token};
            if(entry.values[*index]==value)return {Status::Unchanged,token};
            auto next=entry.values;next[*index]=std::move(value);
            if(const auto conflict=checkChangesLocked(entry,next);!conflict)return conflict;
            entry.values.swap(next);
            try{rebuildLocked();}catch(...){entry.values.swap(next);throw;}
            wake=found->second.wake;
        }
        if(wake)wake->notify();
        return {Status::Ok,token};
    }
    // A reset is atomic: conflicts never leave a partially reset page. Empty
    // field resets this mod's full declaration, never another mod's settings.
    Result reset(uint64_t token,std::string_view field={}){
        std::shared_ptr<const Wake> wake;
        {
            std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return {Status::Busy,token};
            const auto found=records_.find(token);if(found==records_.end())return {Status::NotFound,token};
            auto& entry=found->second.entry;auto next=entry.values;
            if(field.empty())for(size_t i=0;i<next.size();++i)next[i]=entry.fields[i].defaultValue;
            else{const auto index=fieldIndex(entry,field);if(!index)return {Status::NotFound,token};next[*index]=entry.fields[*index].defaultValue;}
            if(next==entry.values)return {Status::Unchanged,token};
            if(const auto conflict=checkChangesLocked(entry,next);!conflict)return conflict;
            entry.values.swap(next);
            try{rebuildLocked();}catch(...){entry.values.swap(next);throw;}
            wake=found->second.wake;
        }
        if(wake)wake->notify();
        return {Status::Ok,token};
    }
    // The caller captures the native game's CURRENT assignments, including
    // user edits, and retries Busy on a later frame. Unknown is never an empty
    // conflict table: it suppresses assignment and dispatch until recaptured.
    Result listNativeBindings(std::vector<NativeBinding> bindings,bool known=true){
        if(bindings.size()>4096)return {Status::Limit};
        for(const auto& binding:bindings)if(!shortcuts::valid(binding.chord)||!validText(binding.label,256))return {Status::Invalid};
        if(!known)bindings.clear();
        std::sort(bindings.begin(),bindings.end(),[](const auto& a,const auto& b){return key(a.chord)<key(b.chord)||(key(a.chord)==key(b.chord)&&a.label<b.label);});
        bindings.erase(std::unique(bindings.begin(),bindings.end()),bindings.end());
        std::unique_lock lock(mutex_,std::try_to_lock);if(!lock)return {Status::Busy};
        if(nativeKnown_==known&&native_==bindings)return {Status::Unchanged};
        const auto previousKnown=nativeKnown_;nativeKnown_=known;native_.swap(bindings);
        try{rebuildLocked();}catch(...){nativeKnown_=previousKnown;native_.swap(bindings);throw;}
        return {};
    }

    // Only a matching, unambiguous action is queued. The fixed ring and
    // coalescing prevent key spam from creating an allocation or callback
    // backlog on the game thread. Poll also checks age and schema revision.
    size_t dispatch(const shortcuts::Event& event,const shortcuts::Context& context,Clock::time_point now=Clock::now()){
        if(!context.gameForeground||context.textInput||context.capturing||event.repeat)return 0;
        const shortcuts::Chord chord{event.key,event.modifiers};if(!shortcuts::valid(chord)||!chord)return 0;
        std::shared_ptr<const Wake> wake;
        {
            std::unique_lock lock(mutex_,std::try_to_lock);if(!lock||!nativeKnown_)return 0;
            const auto value=key(chord);const auto binding=std::lower_bound(bindings_.begin(),bindings_.end(),value,[](const auto& a,uint32_t b){return a.key<b;});
            if(binding==bindings_.end()||binding->key!=value)return 0;
            auto& record=records_.at(binding->token);
            size_t index=0;while(index<record.count&&record.pending[index].field!=binding->field)++index;
            if(index==record.count){if(record.count==maxPending)return 0;++record.count;}
            record.pending[index]={++sequence_,record.entry.revision,binding->field,now};wake=record.wake;
        }
        if(wake)wake->notify();
        return 1;
    }
    // A broker serializing an earlier immutable snapshot passes its revision.
    // If settings changed meanwhile, leave newer input queued for the next
    // read instead of consuming a valid event with incompatible old values.
    std::optional<Event> poll(uint64_t token,Clock::time_point now=Clock::now(),uint64_t expectedRevision=0){
        std::lock_guard lock(mutex_);const auto found=records_.find(token);if(found==records_.end())return {};
        auto& record=found->second;
        if(expectedRevision&&expectedRevision!=record.entry.revision)return {};
        while(record.count){
            const auto pending=record.pending[0];std::move(record.pending.begin()+1,record.pending.begin()+record.count,record.pending.begin());--record.count;
            if(pending.revision!=record.entry.revision||now<pending.created||now-pending.created>eventLifetime)continue;
            return Event{pending.sequence,record.entry.fields[pending.field].id,pending.revision};
        }
        return {};
    }
    // Observation loss / game session changes discard input accepted in the
    // old session, without changing the user's persisted configuration.
    Result discardEvents(uint64_t token){
        std::lock_guard lock(mutex_);const auto found=records_.find(token);if(found==records_.end())return {Status::NotFound,token};
        found->second.count=0;return {Status::Ok,token};
    }
private:
    struct Pending {uint64_t sequence=0,revision=0;size_t field=0;Clock::time_point created;};
    struct Record {uint64_t owner=0;Entry entry;std::shared_ptr<const Wake> wake;std::array<Pending,maxPending> pending;size_t count=0;};
    struct Binding {uint32_t key;uint64_t token;size_t field;};
    std::mutex mutex_;
    std::unordered_map<uint64_t,Record> records_;
    std::vector<NativeBinding> native_;bool nativeKnown_=false;
    std::vector<Binding> bindings_;
    uint64_t serial_=0,revision_=0,sequence_=0;
    std::atomic<std::shared_ptr<const Snapshot>> published_;
    static uint32_t key(shortcuts::Chord chord)noexcept{return uint32_t(uint16_t(chord.key))|(uint32_t(chord.modifiers)<<16);}
    static std::optional<size_t> fieldIndex(const Entry& entry,std::string_view field)noexcept {
        for(size_t i=0;i<entry.fields.size();++i)if(entry.fields[i].id==field)return i;
        return {};
    }
    std::string nativeConflict(shortcuts::Chord chord)const {
        const auto value=key(chord);const auto found=std::lower_bound(native_.begin(),native_.end(),value,[](const auto& a,uint32_t b){return key(a.chord)<b;});
        return found!=native_.end()&&found->chord==chord?found->label:std::string{};
    }
    Result checkChangesLocked(const Entry& entry,const std::vector<std::string>& next)const {
        for(size_t i=0;i<entry.fields.size();++i){
            if(entry.fields[i].kind!=Kind::Shortcut||next[i]==entry.values[i])continue;
            const auto chord=*shortcuts::parse(next[i]);if(!chord)continue;
            if(!nativeKnown_)return {Status::NativeUnavailable,entry.token,std::string(nativeUnavailable)};
            if(const auto conflict=nativeConflict(chord);!conflict.empty())return {Status::Conflict,entry.token,conflict};
            for(const auto& [token,other]:records_)for(size_t j=0;j<other.entry.fields.size();++j){
                if(other.entry.fields[j].kind!=Kind::Shortcut||(token==entry.token&&i==j))continue;
                const auto& value=token==entry.token?next[j]:other.entry.values[j];
                if(*shortcuts::parse(value)==chord)return {Status::Conflict,entry.token,other.entry.title+" — "+other.entry.fields[j].label,token,other.entry.fields[j].id};
            }
        }
        return {Status::Ok,entry.token};
    }
    void rebuildLocked(){
        // Build the entire replacement before touching live dispatch state.
        // Allocation failure leaves the previous snapshot and queues intact.
        auto snapshot=std::make_shared<Snapshot>();snapshot->revision=revision_+1;snapshot->nativeBindingsKnown=nativeKnown_;
        snapshot->mods.reserve(records_.size());for(const auto& [token,record]:records_)snapshot->mods.push_back(record.entry);
        std::sort(snapshot->mods.begin(),snapshot->mods.end(),[](const auto& a,const auto& b){return a.id<b.id;});
        std::vector<Binding> nextBindings,declared;
        std::unordered_map<uint64_t,Entry*> entries;
        for(auto& entry:snapshot->mods){
            const auto token=entry.token;entries.emplace(token,&entry);
            entry.revision=snapshot->revision;entry.conflicts.clear();entry.conflictSources.clear();
            for(size_t i=0;i<entry.fields.size();++i)if(entry.fields[i].kind==Kind::Shortcut){
                const auto chord=*shortcuts::parse(entry.values[i]);if(!chord)continue;
                declared.push_back({key(chord),token,i});
                if(!nativeKnown_)entry.conflicts.emplace(entry.fields[i].id,nativeUnavailable);
                else if(const auto conflict=nativeConflict(chord);!conflict.empty())entry.conflicts.emplace(entry.fields[i].id,conflict);
            }
        }
        std::sort(declared.begin(),declared.end(),[](const auto& a,const auto& b){return a.key<b.key||(a.key==b.key&&a.token<b.token);});
        for(size_t begin=0;begin<declared.size();){
            size_t end=begin+1;while(end<declared.size()&&declared[end].key==declared[begin].key)++end;
            if(end-begin>1)for(size_t i=begin;i<end;++i){
                auto& entry=*entries.at(declared[i].token);
                const auto& other=declared[i==begin?begin+1:begin];const auto& owner=*entries.at(other.token);
                const auto& id=entry.fields[declared[i].field].id;
                if(entry.conflicts.try_emplace(id,owner.title+" — "+owner.fields[other.field].label).second)
                    entry.conflictSources.emplace(id,std::make_pair(owner.token,owner.fields[other.field].id));
            }
            else{
                const auto& binding=declared[begin];const auto& entry=*entries.at(binding.token);
                if(!entry.conflicts.contains(entry.fields[binding.field].id))nextBindings.push_back(binding);
            }
            begin=end;
        }
        // Records need only values and revisions; conflict presentation stays
        // exclusively in the immutable snapshot to avoid another deep copy.
        revision_=snapshot->revision;
        for(auto& [token,record]:records_){record.entry.revision=revision_;record.count=0;}
        bindings_.swap(nextBindings);
        published_.store(std::move(snapshot),std::memory_order_release);
    }
};
}
