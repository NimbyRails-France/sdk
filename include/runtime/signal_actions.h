#pragma once
#include <nimby/signal_settings_store.hpp>
#include <algorithm>
#include <chrono>
#include <list>
#include <memory>
#include <cmath>
#include <thread>
#include <nimby/detail/signal_ui_bridge.h>

namespace nimby::runtime {
// Cross-mod actions are messages, never executable pointers. The resident SDK
// owns names, provider epochs and bounded queues. A UI click cannot enter a mod
// DLL; its observation worker drains messages after a fresh capture instead.
class SignalActions {
public:
    using Token=uint64_t;
    using Clock=std::chrono::steady_clock;
    // Resident SDK notification only: never a mod callback. It may signal an
    // owned OS event, but must not wait, allocate or execute worker code.
    struct ActionWake {
        virtual ~ActionWake()=default;
        virtual void notify()const noexcept=0;
    };
    struct Action {std::string id,label,provider,service;};
    struct NumberInput {std::string id,label;int32_t value=0,minimum=0,maximum=0;bool enabled=true;bool operator==(const NumberInput&)const=default;};
    // A UI intent waits for the next worker capture. Neither deadline grants
    // construction: the worker still validates the route and commit ticket.
    static constexpr auto workerLease=std::chrono::seconds(10);
    static constexpr auto intentLifetime=std::chrono::seconds(30);
    struct Context {
        std::string world;
        uint64_t generation=0;
        bool operator==(const Context&)const=default;
        explicit operator bool()const{return !world.empty()&&generation;}
    };
    struct Selection {
        Token panel=0,provider=0,epoch=0;
        SignalSettingsStore::Editor editor;
        Context context;
        Action action;
        std::string origin;
        uint64_t revision=0;
        bool enabled=true;
        std::optional<NumberInput> input;
        std::optional<int32_t> value;
    };
    struct Button {std::string id,label;bool enabled=true;bool operator==(const Button&)const=default;};
    struct Presentation {std::string origin,service,message;uint64_t revision;std::vector<Button> buttons;std::vector<NumberInput> inputs;bool edited=false;};
    struct Event {uint64_t sequence;Selection selection;Clock::time_point expires;};
    struct Preview {
        Token provider,panel,epoch;
        uint64_t signal;
        std::string origin,service;
        Context context;
        std::vector<NimbyUiPreviewPositionV1> positions;
        Clock::time_point expires;
        uint64_t storeEpoch=0,publication=0;
    };
    enum class PreviewStatus { Published, Invalid, Busy };
    // Internal diagnostic values only; the mod-facing C ABI keeps its statuses.
    enum class PreviewReason {
        None, RegistryBusy, ProviderMissing, PanelMissing, RequestCancelled,
        PanelInactive, ContextMissing, ProviderEpochChanged, PreviewExpired, ProviderExpired,
        ProviderWorldChanged, ProviderGenerationChanged, PanelWorldChanged, PanelGenerationChanged,
        ServiceMissing, ActionMissing, StoreBusy, StoreRetired, StoreSessionMissing,
        ObservationsSuspended, StoreWorldChanged, ObservationEpochChanged, SignalMissing
    };
    struct PreviewDiagnostic {
        PreviewReason reason=PreviewReason::None;
        char providerId[129]{};
        uint64_t expectedProviderEpoch=0,providerEpoch=0;
        uint64_t generation=0,providerGeneration=0,panelGeneration=0;
        uint64_t expectedObservationEpoch=0,observationEpoch=0;
        uint64_t expectedRevision=0,revision=0;
    };
    struct PreviewResult {
        PreviewStatus status;
        uint64_t publication=0;
        PreviewDiagnostic diagnostic{};
        explicit operator bool()const{return status==PreviewStatus::Published;}
    };
    struct PreviewRetry {
        Clock::time_point until=Clock::now()+std::chrono::milliseconds(2);
        unsigned remaining=4096;
        bool operator()(){
            if(!remaining--||Clock::now()>=until)return false;
            std::this_thread::yield();return true;
        }
    };
    struct PreviewClock {Clock::time_point operator()()const{return Clock::now();}};

    // A copied, expiring drawing request. Ownership and session are checked
    // both at publication and at rendering; a stopped mod cannot leave ghosts.
    template<class Retry=PreviewRetry,class Now=PreviewClock>
    PreviewResult publishPreview(Token provider,Token panel,uint64_t signal,std::string origin,std::string service,
            std::vector<NimbyUiPreviewPositionV1> positions,Clock::time_point now=Clock::now(),Retry retry={},Now clock={}) {
        if(positions.size()>64)throw std::length_error("Too many preview positions");
        for(const auto& p:positions)if(p.track>>48!=1||!std::isfinite(p.fraction)||p.fraction<=0||p.fraction>=1||
            (p.direction!=1&&p.direction!=-1)||p.reserved)throw std::invalid_argument("Invalid preview position");
        const auto started=clock();
        std::optional<Preview> next;
        uint64_t requestRevision=0;
        PreviewDiagnostic diagnostic;
        for(;;){
            {
                std::unique_lock lock(mutex_,std::try_to_lock);
                // Without the first registry snapshot we have no owner epoch
                // or cancellation ticket. Defer to a fresh worker tick rather
                // than crossing an unseen clear/suspend while trying to enter.
                if(!lock&&!next)return {PreviewStatus::Busy,0,{PreviewReason::RegistryBusy}};
                if(lock){
                    // Fields from a prior busy attempt are not current values
                    // for a later cancellation/suspension check.
                    diagnostic.providerEpoch=0;diagnostic.providerGeneration=0;diagnostic.panelGeneration=0;
                    diagnostic.observationEpoch=0;diagnostic.revision=0;
                    const auto owner=providers_.find(provider);
                    const auto rejected=[&](PreviewStatus status,PreviewReason reason){
                        diagnostic.reason=reason;
                        // Copy bounded failure context under the same registry
                        // lock as validation. Logging happens after this return.
                        if(owner!=providers_.end()){
                            const auto& p=owner->second;
                            std::copy(p.id.begin(),p.id.end(),diagnostic.providerId);
                            diagnostic.providerEpoch=p.epoch;diagnostic.providerGeneration=p.context.generation;
                            diagnostic.revision=p.previewRevision;
                        }
                        if(const auto consumer=panels_.find(panel);consumer!=panels_.end())
                            diagnostic.panelGeneration=consumer->second.context.generation;
                        if(next){diagnostic.expectedProviderEpoch=next->epoch;diagnostic.generation=next->context.generation;
                            diagnostic.expectedObservationEpoch=next->storeEpoch;diagnostic.expectedRevision=requestRevision;}
                        return PreviewResult{status,0,diagnostic};
                    };
                    if(owner==providers_.end())return rejected(PreviewStatus::Invalid,PreviewReason::ProviderMissing);
                    if(!next){
                        if(positions.empty()){
                            ++owner->second.previewRevision;
                            if(preview_&&preview_->provider==provider)preview_.reset();
                            return {PreviewStatus::Published};
                        }
                        const auto consumer=panels_.find(panel);
                        if(consumer==panels_.end())return rejected(PreviewStatus::Invalid,PreviewReason::PanelMissing);
                        next=Preview{provider,panel,owner->second.epoch,signal,std::move(origin),std::move(service),owner->second.context,
                            std::move(positions),now+std::chrono::seconds(2),consumer->second.store->observationEpoch(),0};
                        requestRevision=owner->second.previewRevision;
                    }
                    // A retry retains its original owner/session and expiry.
                    // Suspension, deletion/readdition or a new session cannot
                    // turn a previously pending drawing into a fresh request.
                    if(requestRevision!=owner->second.previewRevision)return rejected(PreviewStatus::Invalid,PreviewReason::RequestCancelled);
                    const auto state=previewStateLocked(*next,now+(clock()-started),&diagnostic);
                    if(state==SignalSettingsStore::SignalState::Unknown)return rejected(PreviewStatus::Invalid,diagnostic.reason);
                    if(state==SignalSettingsStore::SignalState::Known){
                        ++owner->second.previewRevision;next->publication=++serial_;const auto publication=next->publication;
                        preview_=std::move(*next);return {PreviewStatus::Published,publication};
                    }
                    diagnostic=rejected(PreviewStatus::Busy,diagnostic.reason).diagnostic;
                }else diagnostic.reason=PreviewReason::RegistryBusy;
            }
            // Never hold the action/store/render lock while waiting. Both a
            // wall-clock budget and an attempt cap bound worker-side retries.
            if(!retry()){
                // No registry snapshot is read after unlocking. The last
                // validation distinguishes store contention from registry contention.
                if(next){diagnostic.expectedProviderEpoch=next->epoch;diagnostic.generation=next->context.generation;
                    diagnostic.expectedObservationEpoch=next->storeEpoch;diagnostic.expectedRevision=requestRevision;}
                return {PreviewStatus::Busy,0,diagnostic};
            }
        }
    }
    std::optional<Preview> preview(uint64_t selectedSignal,Clock::time_point now=Clock::now())const {
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock||!preview_||preview_->signal!=selectedSignal||previewStateLocked(*preview_,now)!=SignalSettingsStore::SignalState::Known)return {};
        const auto& owner=providers_.at(preview_->provider);
        const auto panel=owner.panels.find({preview_->panel,preview_->signal});
        if(panel!=owner.panels.end()&&panel->second.edited)return {};
        return preview_;
    }

    Token addProvider(std::string id,std::vector<std::string> services) {
        checkText(id);if(services.size()>32)throw std::length_error("Too many mod services");
        std::set<std::string> unique;
        for(const auto& service:services){checkText(service);if(!unique.insert(service).second)throw std::invalid_argument("Duplicate service");}
        std::lock_guard lock(mutex_);
        if(providers_.size()>=512)throw std::length_error("Too many mod providers");
        for(const auto& [token,p]:providers_)if(p.id==id)throw std::invalid_argument("Mod provider already loaded");
        const auto token=++serial_;
        providers_.emplace(token,Provider{std::move(id),std::move(unique),{},{},1,{},{}});return token;
    }
    bool hasProvider(Token token)const{std::lock_guard lock(mutex_);return providers_.contains(token);}
    bool setProviderWake(Token token,std::shared_ptr<const ActionWake> wake){
        {
            std::lock_guard lock(mutex_);const auto p=providers_.find(token);if(p==providers_.end())return false;
            p->second.wake.swap(wake);
        }
        // Replacement/removal cannot destroy an OS resource while another
        // caller holds the registry. A queued click retains its own reference.
        return true;
    }
    bool removeProvider(Token token){
        decltype(providers_)::node_type retired;
        std::optional<Preview> retiredPreview;
        {
            std::lock_guard lock(mutex_);
            if(preview_&&preview_->provider==token){retiredPreview=std::move(preview_);preview_.reset();}
            retired=providers_.extract(token);
        }
        // Detach ownership atomically, then release the copied presentations,
        // events and drawing outside the registry used by every UI frame.
        return !retired.empty();
    }
    bool present(std::string_view id)const {
        std::lock_guard lock(mutex_);
        return std::any_of(providers_.begin(),providers_.end(),[&](const auto& p){return p.second.id==id;});
    }
    bool observeProvider(Token token,Context context,Clock::time_point now=Clock::now()) {
        if(!context)return false;
        checkText(context.world,512);
        std::map<std::pair<Token,uint64_t>,Presentation> retiredPresentations;
        std::list<Event> retiredEvents;
        std::optional<Preview> retiredPreview;
        {
            std::lock_guard lock(mutex_);const auto it=providers_.find(token);if(it==providers_.end())return false;
            auto& p=it->second;
            if(p.context!=context)retiredPresentations.swap(p.panels);
            if(p.context!=context||now>=p.expires){
                ++p.epoch;retiredEvents.splice(retiredEvents.end(),p.queue);
                if(preview_&&preview_->provider==token){retiredPreview=std::move(preview_);preview_.reset();}
            }
            p.context=std::move(context);p.expires=now+workerLease;
        }
        return true;
    }
    bool suspendProvider(Token token) {
        std::list<Event> retiredEvents;
        std::optional<Preview> retiredPreview;
        {
            std::lock_guard lock(mutex_);const auto it=providers_.find(token);if(it==providers_.end())return false;
            // Retain presentation, including an explicitly closed menu. Suspension
            // revokes commands; it must not recreate the initial Repeat button.
            it->second.expires={};++it->second.epoch;
            retiredEvents.splice(retiredEvents.end(),it->second.queue);
            if(preview_&&preview_->provider==token){retiredPreview=std::move(preview_);preview_.reset();}
        }
        return true;
    }
    void addPanel(Token owner,std::shared_ptr<SignalSettingsStore> store) {
        std::lock_guard lock(mutex_);panels_.emplace(owner,Panel{std::move(store),{}, {}});
    }
    void removePanel(Token owner){
        decltype(panels_)::node_type retiredPanel;
        std::multimap<std::pair<Token,uint64_t>,Presentation> retiredPresentations;
        std::list<Event> retiredEvents;
        std::optional<Preview> retiredPreview;
        {
            std::lock_guard lock(mutex_);retiredPanel=panels_.extract(owner);
            if(preview_&&preview_->panel==owner){retiredPreview=std::move(preview_);preview_.reset();}
            for(auto& [token,p]:providers_){
                for(auto i=p.panels.begin();i!=p.panels.end();){
                    if(i->first.first==owner)retiredPresentations.insert(p.panels.extract(i++));
                    else ++i;
                }
                for(auto i=p.queue.begin();i!=p.queue.end();){
                    if(i->selection.panel==owner){auto retired=i++;retiredEvents.splice(retiredEvents.end(),p.queue,retired);}
                    else ++i;
                }
            }
        }
        // Node transfers allocate nothing after revocation and keep every
        // other panel's event order. Release owned data after unlocking.
    }
    bool configure(Token owner,std::vector<Action> actions) {
        if(actions.size()>16)throw std::length_error("Too many signal actions");
        std::set<std::string> names;
        for(const auto& a:actions){checkText(a.id);checkText(a.label,256);checkText(a.provider);checkText(a.service);
            if(!names.insert(a.id).second)throw std::invalid_argument("Duplicate signal action");}
        std::lock_guard lock(mutex_);const auto it=panels_.find(owner);if(it==panels_.end()||it->second.context)return false;
        it->second.actions=std::move(actions);return true;
    }
    bool panelContext(Token owner,Context context) {
        if(context)checkText(context.world,512);
        std::lock_guard lock(mutex_);const auto it=panels_.find(owner);if(it==panels_.end())return false;
        it->second.active=bool(context);
        if(context)it->second.context=std::move(context);
        return true;
    }
    bool publish(Token provider,Token panel,uint64_t signal,std::string origin,std::string service,std::string message,std::vector<Button> buttons,std::vector<NumberInput> inputs={}) {
        checkText(origin);checkText(service);if(!message.empty())checkText(message,256);
        if(buttons.size()>12)throw std::length_error("Too many tool buttons");
        std::set<std::string> ids;
        for(const auto& b:buttons){checkText(b.id);checkText(b.label,256);if(!ids.insert(b.id).second)throw std::invalid_argument("Duplicate tool button");}
        if(inputs.size()>4)throw std::length_error("Too many number inputs");
        for(const auto& n:inputs){checkText(n.id);checkText(n.label,256);
            if(n.minimum>n.maximum||n.value<n.minimum||n.value>n.maximum||!ids.insert(n.id).second)
                throw std::invalid_argument("Invalid number input");}
        std::lock_guard lock(mutex_);const auto p=providers_.find(provider),end=providers_.end();
        const auto consumer=panels_.find(panel);
        if(p==end||consumer==panels_.end()||!p->second.context||p->second.context!=consumer->second.context||Clock::now()>=p->second.expires)return false;
        const auto declared=std::find_if(consumer->second.actions.begin(),consumer->second.actions.end(),[&](const auto& a){return a.id==origin&&a.service==service&&a.provider==p->second.id;});
        // Publication only updates presentation. A transient loss of signal
        // observations disables interaction below, without rejecting the tool
        // tick and triggering another provider suspension/recovery cycle.
        if(declared==consumer->second.actions.end()||!consumer->second.store->knowsSignal(signal))return false;
        auto& panels=p->second.panels;const auto key=std::make_pair(panel,signal);
        if(!panels.contains(key)&&panels.size()>=64)return false;
        // Workers can refresh their presentation every tick without revoking a
        // click that is about to be polled. Actual changes still get a revision.
        const auto previous=panels.find(key);
        if(previous!=panels.end()){const auto& old=previous->second;
            if(!old.edited&&old.origin==origin&&old.service==service&&old.message==message&&old.buttons==buttons&&old.inputs==inputs)return true;
        }
        const auto revision=++serial_;bool edited=false;
        // A worker may acknowledge field A while a newer edit of field B is
        // still queued. Preserve that pending value and retarget its revision,
        // otherwise republishing A would silently erase B's keystrokes.
        for(auto& event:p->second.queue){auto& s=event.selection;
            if(s.panel!=panel||s.editor.signal!=signal||s.origin!=origin||s.action.service!=service||!s.input||!s.value||Clock::now()>=event.expires||!validLocked(s,Clock::now()))continue;
            auto n=std::find_if(inputs.begin(),inputs.end(),[&](const auto& input){return input.id==s.action.id;});
            if(n==inputs.end()||!n->enabled||*s.value<n->minimum||*s.value>n->maximum)continue;
            n->value=*s.value;s.input=*n;s.revision=revision;edited=true;
        }
        panels[key]={std::move(origin),std::move(service),std::move(message),revision,std::move(buttons),std::move(inputs),edited};return true;
    }
    std::vector<Selection> prepare(Token owner,SignalSettingsStore::Editor editor,Clock::time_point now=Clock::now())const {
        std::lock_guard lock(mutex_);std::vector<Selection> result;
        const auto found=panels_.find(owner);if(found==panels_.end()||!found->second.context)return result;
        const auto& panel=found->second;
        for(const auto& action:panel.actions)for(const auto& [token,p]:providers_)
            if(p.id==action.provider&&p.services.contains(action.service)&&p.context==panel.context){
                const bool live=panel.active&&now<p.expires&&panel.store->accepts(editor);
                const auto current=p.panels.find({owner,editor.signal});
                if(current==p.panels.end()||current->second.origin!=action.id||current->second.service!=action.service)
                    result.push_back({owner,token,p.epoch,editor,panel.context,action,action.id,0,live,{},{}});
                else {
                    const auto& presentation=current->second;
                    for(const auto& n:presentation.inputs)result.push_back({owner,token,p.epoch,editor,panel.context,
                        {n.id,n.label,action.provider,action.service},action.id,presentation.revision,live&&n.enabled,n,{}});
                    if(!presentation.message.empty())result.push_back({owner,token,p.epoch,editor,panel.context,
                        {"",presentation.message,action.provider,action.service},action.id,presentation.revision,false,{},{}});
                    for(const auto& b:presentation.buttons)result.push_back({owner,token,p.epoch,editor,panel.context,
                        {b.id,b.label,action.provider,action.service},action.id,presentation.revision,live&&b.enabled,{},{}});
                }
            }
        return result;
    }
    bool available(const Selection& s,Clock::time_point now=Clock::now())const {
        std::lock_guard lock(mutex_);return validLocked(s,now);
    }
    bool click(const Selection& s,Clock::time_point now=Clock::now()) {
        std::shared_ptr<const ActionWake> wake;
        {
            std::lock_guard lock(mutex_);if(s.input||!validLocked(s,now))return false;
            auto& provider=providers_.at(s.provider);auto& queue=provider.queue;
            if(queue.size()>=64)return false;
            // Double clicks before consumption are one intent. Construction itself
            // still needs its own prepare/commit ticket and never auto-retries.
            if(std::any_of(queue.begin(),queue.end(),[&](const auto& e){const auto& v=e.selection;
                return v.panel==s.panel&&v.editor==s.editor&&v.action.id==s.action.id;}))return false;
            queue.push_back({++serial_,s,now+intentLifetime});wake=provider.wake;
        }
        // Removal may retire this provider now. The owned event remains valid;
        // at worst its former worker wakes to an empty, revalidated queue.
        if(wake)wake->notify();
        return true;
    }
    // An edit blocks action buttons immediately, including later rows in the
    // same UI frame. Only the worker's next publication can enable them again.
    bool beginNumberEdit(const Selection& s,Clock::time_point now=Clock::now()) {
        std::lock_guard lock(mutex_);if(!s.input||!validLocked(s,now))return false;
        auto& provider=providers_.at(s.provider);
        ++provider.previewRevision;
        provider.panels.at({s.panel,s.editor.signal}).edited=true;
        if(preview_&&preview_->provider==s.provider)preview_.reset();
        // An empty draft has no integer event. Still cancel an earlier command
        // queued against the value that the user has just erased.
        std::erase_if(provider.queue,[&](const auto& e){return e.selection.panel==s.panel&&e.selection.editor==s.editor&&
            (!e.selection.input||e.selection.action.id==s.action.id);});
        return true;
    }
    bool editNumber(const Selection& s,int32_t value,Clock::time_point now=Clock::now()) {
        std::shared_ptr<const ActionWake> wake;
        {
            std::lock_guard lock(mutex_);if(!s.input||!validLocked(s,now))return false;
            auto& provider=providers_.at(s.provider);auto& panel=provider.panels.at({s.panel,s.editor.signal});
            auto input=std::find_if(panel.inputs.begin(),panel.inputs.end(),[&](const auto& n){return n.id==s.action.id;});
            if(input==panel.inputs.end()||value<input->minimum||value>input->maximum)return false;
            auto pending=std::find_if(provider.queue.begin(),provider.queue.end(),[&](const auto& e){const auto& v=e.selection;
                return v.input&&v.panel==s.panel&&v.editor==s.editor&&v.action.id==s.action.id;});
            if(pending==provider.queue.end()&&provider.queue.size()>=64)return false;
            input->value=value;panel.edited=true;auto changed=s;changed.value=value;
            ++provider.previewRevision;
            if(preview_&&preview_->provider==s.provider)preview_.reset();
            if(pending!=provider.queue.end()){pending->selection=std::move(changed);pending->expires=now+intentLifetime;}
            else provider.queue.push_back({++serial_,std::move(changed),now+intentLifetime});
            wake=provider.wake;
        }
        if(wake)wake->notify();
        return true;
    }
    std::optional<Event> poll(Token provider,Clock::time_point now=Clock::now()) {
        std::lock_guard lock(mutex_);const auto found=providers_.find(provider);if(found==providers_.end())return {};
        auto& queue=found->second.queue;
        while(!queue.empty()){
            auto event=std::move(queue.front());queue.pop_front();
            if(now<event.expires&&validLocked(event.selection,now))return event;
        }
        return {};
    }
private:
    // One list node per user intent, never per frame/tick. Splicing lets a
    // panel retire its queued intents without allocating during cleanup.
    struct Provider {std::string id;std::set<std::string> services;Context context;Clock::time_point expires;Token epoch;std::list<Event> queue;std::map<std::pair<Token,uint64_t>,Presentation> panels;uint64_t previewRevision=0;std::shared_ptr<const ActionWake> wake{};};
    struct Panel {std::shared_ptr<SignalSettingsStore> store;Context context;std::vector<Action> actions;bool active=false;};
    SignalSettingsStore::SignalState previewStateLocked(const Preview& preview,Clock::time_point now,PreviewDiagnostic* diagnostic=nullptr)const {
        using State=SignalSettingsStore::SignalState;
        const auto p=providers_.find(preview.provider);const auto panel=panels_.find(preview.panel);
        const auto unknown=[&](PreviewReason reason){if(diagnostic)diagnostic->reason=reason;return State::Unknown;};
        if(p==providers_.end())return unknown(PreviewReason::ProviderMissing);
        if(panel==panels_.end())return unknown(PreviewReason::PanelMissing);
        if(!panel->second.active)return unknown(PreviewReason::PanelInactive);
        if(!preview.context)return unknown(PreviewReason::ContextMissing);
        if(p->second.epoch!=preview.epoch)return unknown(PreviewReason::ProviderEpochChanged);
        if(now>=preview.expires)return unknown(PreviewReason::PreviewExpired);
        if(now>=p->second.expires)return unknown(PreviewReason::ProviderExpired);
        if(p->second.context.world!=preview.context.world)return unknown(PreviewReason::ProviderWorldChanged);
        if(p->second.context.generation!=preview.context.generation)return unknown(PreviewReason::ProviderGenerationChanged);
        if(panel->second.context.world!=preview.context.world)return unknown(PreviewReason::PanelWorldChanged);
        if(panel->second.context.generation!=preview.context.generation)return unknown(PreviewReason::PanelGenerationChanged);
        if(!p->second.services.contains(preview.service))return unknown(PreviewReason::ServiceMissing);
        if(!std::any_of(panel->second.actions.begin(),panel->second.actions.end(),[&](const auto& a){
            return a.id==preview.origin&&a.service==preview.service&&a.provider==p->second.id;}))return unknown(PreviewReason::ActionMissing);
        SignalSettingsStore::SignalStateDetails details;
        const auto state=panel->second.store->signalState(preview.signal,true,preview.context.world,preview.storeEpoch,diagnostic?&details:nullptr);
        if(diagnostic){
            diagnostic->observationEpoch=details.epoch;
            using Reason=SignalSettingsStore::SignalStateReason;
            switch(details.reason){
                case Reason::None:diagnostic->reason=PreviewReason::None;break;
                case Reason::Busy:diagnostic->reason=PreviewReason::StoreBusy;break;
                case Reason::Retired:diagnostic->reason=PreviewReason::StoreRetired;break;
                case Reason::SessionMissing:diagnostic->reason=PreviewReason::StoreSessionMissing;break;
                case Reason::ObservationsSuspended:diagnostic->reason=PreviewReason::ObservationsSuspended;break;
                case Reason::WorldChanged:diagnostic->reason=PreviewReason::StoreWorldChanged;break;
                case Reason::EpochChanged:diagnostic->reason=PreviewReason::ObservationEpochChanged;break;
                case Reason::SignalMissing:diagnostic->reason=PreviewReason::SignalMissing;break;
            }
        }
        return state;
    }
    bool validLocked(const Selection& s,Clock::time_point now)const {
        const auto p=providers_.find(s.provider);const auto panel=panels_.find(s.panel);
        if(!s.enabled||p==providers_.end()||panel==panels_.end()||!panel->second.active||!s.context||p->second.epoch!=s.epoch||
           p->second.context!=s.context||panel->second.context!=s.context||now>=p->second.expires)return false;
        if(p->second.id!=s.action.provider||!p->second.services.contains(s.action.service))return false;
        const auto declared=std::find_if(panel->second.actions.begin(),panel->second.actions.end(),[&](const auto& a){
            return a.id==s.origin&&a.provider==s.action.provider&&a.service==s.action.service;});
        if(declared==panel->second.actions.end()||!panel->second.store->accepts(s.editor))return false;
        const auto current=p->second.panels.find({s.panel,s.editor.signal});
        if(!s.revision)return current==p->second.panels.end()||current->second.origin!=s.origin;
        if(current==p->second.panels.end()||current->second.revision!=s.revision)return false;
        if(s.input)return std::any_of(current->second.inputs.begin(),current->second.inputs.end(),[&](const auto& n){return n.enabled&&n.id==s.action.id;});
        if(current->second.edited)return false;
        return std::any_of(current->second.buttons.begin(),current->second.buttons.end(),[&](const auto& b){return b.enabled&&b.id==s.action.id;});
    }
    static void checkText(std::string_view value,size_t limit=128){
        if(value.empty()||value.size()>limit||value.find('\0')!=std::string_view::npos)throw std::invalid_argument("Invalid service text");
    }
    mutable std::mutex mutex_;
    Token serial_=0;
    std::map<Token,Provider> providers_;
    std::map<Token,Panel> panels_;
    std::optional<Preview> preview_;
};
}
