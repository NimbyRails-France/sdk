#pragma once
#include <nimby/detail/sdk.h>
#include <nimby/detail/observation.h>
#include <nimby/detail/signal_ui_bridge.h>
#include <nimby/detail/signal_settings_file.hpp>
#include "runtime/signal_ui_host.h"
#include <cstring>

namespace nimby::runtime {
// Registries and stores own their locks. Slow import/export on one panel never
// holds an endpoint-wide lock. Removed stores reject late retained operations.
class SignalUiEndpoint {
public:
    uint32_t numbers(uint64_t owner,const NimbyUiNumberSettingV1* fields,uint32_t count)noexcept {
        return boundary([&]() -> uint32_t {
            if(count>4||(!fields&&count))return NIMBY_INVALID_ARGUMENT;
            auto store=host.store(owner);if(!store)return NIMBY_INVALID_HANDLE;
            std::vector<SignalNumber> definitions;
            for(uint32_t i=0;i<count;++i)definitions.push_back({text(fields[i].name),text(fields[i].label),text(fields[i].visible_when),fields[i].bits,fields[i].maximum});
            return store->configureNumbers(definitions)?NIMBY_OK:NIMBY_INVALID_ARGUMENT;
        });
    }
    SignalUiHost host;
    uint32_t add(const NimbyUiPanelV1* source,uint64_t* token) noexcept {
        if(token)*token=0;
        return boundary([&]() -> uint32_t {
            if(!source||!token||source->size!=sizeof(*source)||source->version!=NIMBY_SIGNAL_UI_ABI||
               source->reserved||source->count>64)return NIMBY_INVALID_ARGUMENT;
            std::vector<SignalCheckbox> fields;
            for(uint32_t i=0;i<source->count;++i){const auto& box=source->checkboxes[i];
                if(box.default_value>1)return NIMBY_INVALID_ARGUMENT;
                fields.push_back({text(box.name),text(box.label),text(box.description),box.default_value!=0});
            }
            *token=host.add({text(source->id),text(source->title),text(source->texture_set),fields});
            return NIMBY_OK;
        });
    }
    uint32_t remove(uint64_t token) noexcept {
        return boundary([&]{return host.remove(token)?NIMBY_OK:NIMBY_INVALID_HANDLE;});
    }
    uint32_t translations(uint32_t kind,uint64_t token,const char* bytes,uint32_t length)noexcept {
        return boundary([&]() -> uint32_t {
            if(kind>1||!bytes||!length||length>detail::Translations::maximumBytes)return NIMBY_INVALID_ARGUMENT;
            auto catalog=std::make_shared<const detail::Translations>(std::string_view(bytes,length));
            return host.translations(kind==1,token,std::move(catalog))?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t actions(uint64_t owner,const NimbyUiActionV1* source,uint32_t count)noexcept {
        return boundary([&]() -> uint32_t {
            if(count>16||(!source&&count))return NIMBY_INVALID_ARGUMENT;
            std::vector<SignalActions::Action> actions;
            for(uint32_t i=0;i<count;++i)actions.push_back({std::string(text(source[i].id)),std::string(text(source[i].label)),
                std::string(text(source[i].provider)),std::string(text(source[i].service))});
            return host.actions->configure(owner,std::move(actions))?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t panelContext(uint64_t owner,uint64_t session,uint64_t generation)noexcept {
        return boundary([&]() -> uint32_t {
            if(!generation)return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(owner);if(!store)return NIMBY_INVALID_HANDLE;
            const auto world=store->sessionIdentity(session);if(!world)return NIMBY_INVALID_HANDLE;
            return host.actions->panelContext(owner,{*world,generation})?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t addProvider(const NimbyUiProviderV1* source,uint64_t* token)noexcept {
        if(token)*token=0;
        return boundary([&]() -> uint32_t {
            if(!source||!token||source->size!=sizeof(*source)||source->version!=1||source->reserved||source->count>32)return NIMBY_INVALID_ARGUMENT;
            std::vector<std::string> services;
            for(uint32_t i=0;i<source->count;++i)services.emplace_back(text(source->services[i]));
            *token=host.actions->addProvider(std::string(text(source->id)),std::move(services));return NIMBY_OK;
        });
    }
    uint32_t removeProvider(uint64_t token)noexcept {return boundary([&]{return host.removeProvider(token)?NIMBY_OK:NIMBY_INVALID_HANDLE;});}
    // Internal resident binding; no mod-facing ABI accepts a notifier or handle.
    uint32_t providerWake(uint64_t token,std::shared_ptr<const SignalActions::ActionWake> wake)noexcept {
        return boundary([&]{return host.actions->setProviderWake(token,std::move(wake))?NIMBY_OK:NIMBY_INVALID_HANDLE;});
    }
    uint32_t observeProvider(uint64_t token,const char* world,uint32_t length,uint64_t generation)noexcept {
        return boundary([&]() -> uint32_t {
            if(!world||!length||length>512||!generation)return NIMBY_INVALID_ARGUMENT;
            return host.actions->observeProvider(token,{std::string(world,length),generation})?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t suspendProvider(uint64_t token)noexcept {return boundary([&]{return host.actions->suspendProvider(token)?NIMBY_OK:NIMBY_INVALID_HANDLE;});}
    uint32_t pollProvider(uint64_t token,NimbyUiActionEventV1* out)noexcept {
        return boundary([&]() -> uint32_t {
            if(!out||out->size!=sizeof(*out)||out->version!=1)return NIMBY_INVALID_ARGUMENT;
            *out={};out->size=sizeof(*out);out->version=1;
            const auto event=host.actions->poll(token);if(!event)return NIMBY_DATA_UNAVAILABLE;
            out->sequence=event->sequence;out->signal=event->selection.editor.signal;
            out->generation=event->selection.context.generation;out->panel=event->selection.panel;
            copy(out->origin,event->selection.origin);
            copy(out->action,event->selection.action.id);copy(out->service,event->selection.action.service);copy(out->world,event->selection.context.world);
            return NIMBY_OK;
        });
    }
    uint32_t pollProviderV2(uint64_t token,NimbyUiActionEventV2* out)noexcept {
        return boundary([&]() -> uint32_t {
            if(!out||out->base.size!=sizeof(*out)||out->base.version!=2)return NIMBY_INVALID_ARGUMENT;
            *out={};out->base.size=sizeof(*out);out->base.version=2;
            const auto event=host.actions->poll(token);if(!event)return NIMBY_DATA_UNAVAILABLE;
            auto& base=out->base;const auto& s=event->selection;
            base.sequence=event->sequence;base.signal=s.editor.signal;base.generation=s.context.generation;base.panel=s.panel;
            copy(base.origin,s.origin);copy(base.action,s.action.id);copy(base.service,s.action.service);copy(base.world,s.context.world);
            out->has_value=s.value.has_value();out->value=s.value.value_or(0);return NIMBY_OK;
        });
    }
    uint32_t present(const char* id,uint32_t length,uint32_t* out)noexcept {
        if(out)*out=0;
        return boundary([&]() -> uint32_t {
            if(!out||!id||!length||length>128||std::memchr(id,0,length))return NIMBY_INVALID_ARGUMENT;
            *out=host.actions->present({id,length})?1:0;return NIMBY_OK;
        });
    }
    uint32_t publishToolPanel(uint64_t provider,const NimbyUiToolPanelV1* panel)noexcept {
        return boundary([&]() -> uint32_t {
            if(!panel||panel->size!=sizeof(*panel)||panel->version!=1||panel->reserved||panel->count>12||panel->signal>>48!=8)return NIMBY_INVALID_ARGUMENT;
            std::vector<SignalActions::Button> buttons;
            for(uint32_t i=0;i<panel->count;++i){const auto& b=panel->buttons[i];if(b.enabled>1)return NIMBY_INVALID_ARGUMENT;
                buttons.push_back({std::string(text(b.id)),std::string(text(b.label)),b.enabled!=0});}
            return host.actions->publish(provider,panel->panel,panel->signal,std::string(text(panel->origin)),std::string(text(panel->service)),std::string(text(panel->message)),std::move(buttons))?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t publishToolPanelV2(uint64_t provider,const NimbyUiToolPanelV2* source)noexcept {
        return boundary([&]() -> uint32_t {
            if(!source)return NIMBY_INVALID_ARGUMENT;
            const auto& p=source->base;
            if(p.size!=sizeof(*source)||p.version!=2||p.reserved||p.count>12||p.signal>>48!=8||source->reserved||source->input_count>4)return NIMBY_INVALID_ARGUMENT;
            std::vector<SignalActions::Button> buttons;std::vector<SignalActions::NumberInput> inputs;
            for(uint32_t i=0;i<p.count;++i){const auto& b=p.buttons[i];if(b.enabled>1)return NIMBY_INVALID_ARGUMENT;
                buttons.push_back({std::string(text(b.id)),std::string(text(b.label)),b.enabled!=0});}
            for(uint32_t i=0;i<source->input_count;++i){const auto& n=source->inputs[i];if(n.enabled>1)return NIMBY_INVALID_ARGUMENT;
                inputs.push_back({std::string(text(n.id)),std::string(text(n.label)),n.value,n.minimum,n.maximum,n.enabled!=0});}
            return host.actions->publish(provider,p.panel,p.signal,std::string(text(p.origin)),std::string(text(p.service)),std::string(text(p.message)),std::move(buttons),std::move(inputs))?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t publishPreview(uint64_t provider,const NimbyUiSignalPreviewV1* source,uint64_t* publication=nullptr,SignalActions::PreviewResult* diagnostic=nullptr)noexcept {
        if(publication)*publication=0;
        if(diagnostic)*diagnostic={SignalActions::PreviewStatus::Invalid};
        return boundary([&]() -> uint32_t {
            if(!source||source->size!=sizeof(*source)||source->version!=1||source->reserved||source->count>64)return NIMBY_INVALID_ARGUMENT;
            if(source->count&&source->signal>>48!=8)return NIMBY_INVALID_ARGUMENT;
            const auto result=source->count?host.actions->publishPreview(provider,source->panel,source->signal,std::string(text(source->origin)),
                std::string(text(source->service)),{source->positions,source->positions+source->count}):host.actions->publishPreview(provider,0,0,{},{},{});
            if(publication)*publication=result.publication;
            if(diagnostic)*diagnostic=result;
            return result.status==SignalActions::PreviewStatus::Published?NIMBY_OK:
                result.status==SignalActions::PreviewStatus::Busy?NIMBY_RESOURCE_LIMIT:NIMBY_INVALID_HANDLE;
        });
    }
    uint32_t conditionalVisibility(uint64_t token,uint64_t mask) noexcept {
        return boundary([&]() -> uint32_t {
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            return store->conditionalVisibility(mask)?NIMBY_OK:NIMBY_INVALID_ARGUMENT;
        });
    }
    uint32_t begin(uint64_t token,const char* identity,uint32_t length,uint64_t* session) noexcept {
        if(session)*session=0;
        return boundary([&]() -> uint32_t {
            if(!identity||!session||!length||length>512)return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            *session=store->beginSession({identity,length});host.actions->panelContext(token,{});return NIMBY_OK;
        });
    }
    uint32_t observe(uint64_t token,uint64_t session,const NimbyUiSignalV1* source,uint32_t count) noexcept {
        return boundary([&]() -> uint32_t {
            if(count>1000000||(!source&&count))return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            std::vector<SignalSettingsStore::Signal> signals;signals.reserve(count);
            for(uint32_t i=0;i<count;++i)signals.push_back({source[i].id,std::string(text(source[i].texture_set))});
            return store->observeSignals(session,signals)?NIMBY_OK:NIMBY_INVALID_HANDLE;
        });
    }
    // Copy resident UI values while checking the worker's session token. No
    // disk I/O on the native UI thread, and no STL objects across DLL boundaries.
    uint32_t settingsRevision(uint64_t token,uint64_t session,uint64_t* revision) noexcept {
        if(revision)*revision=0;
        return boundary([&]() -> uint32_t {
            if(!revision||!session)return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            const auto value=store->settingsRevision(session);if(!value)return NIMBY_INVALID_HANDLE;
            *revision=*value;return NIMBY_OK;
        });
    }
    uint32_t exportSettings(uint64_t token,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written) noexcept {
        if(written)*written=0;
        return boundary([&]() -> uint32_t {
            if(!written||!session||capacity>16*1024*1024||(!bytes&&capacity))return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            const auto encoded=detail::SignalSettingsFile::encode(store->save(session));
            *written=static_cast<uint32_t>(encoded.size());
            if(!bytes&&!capacity)return NIMBY_OK;
            if(capacity<encoded.size())return NIMBY_RESOURCE_LIMIT;
            std::memcpy(bytes,encoded.data(),encoded.size());return NIMBY_OK;
        });
    }
    // Import starts a new epoch. Validation completes before changing the store;
    // fresh catalog observation is still required before reads or clicks resume.
    uint32_t beginSaved(uint64_t token,const char* identity,uint32_t identityLength,
        const char* bytes,uint32_t length,uint64_t* session) noexcept {
        if(session)*session=0;
        return boundary([&]() -> uint32_t {
            if(!session||!identity||!identityLength||identityLength>512||!bytes||!length||length>16*1024*1024)
                return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            const auto saved=detail::SignalSettingsFile::decode({bytes,length});
            *session=store->beginSession({identity,identityLength},&saved);host.actions->panelContext(token,{});return NIMBY_OK;
        });
    }
    uint32_t suspend(uint64_t token) noexcept {
        return boundary([&]() -> uint32_t {
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            store->suspendObservations();host.actions->panelContext(token,{});return NIMBY_OK;
        });
    }
    uint32_t read(uint64_t token,uint64_t signal,NimbyUiValuesV1* out) noexcept {
        return boundary([&]() -> uint32_t {
            if(!out||out->size!=sizeof(*out)||out->version!=NIMBY_SIGNAL_UI_ABI)return NIMBY_INVALID_ARGUMENT;
            *out={};out->size=sizeof(*out);out->version=NIMBY_SIGNAL_UI_ABI;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            const auto values=store->read(signal);
            out->status=values.status==SettingsStatus::Present?2:values.status==SettingsStatus::Absent?1:0;
            for(const auto& [name,value]:values.booleans){
                auto& field=out->fields[out->count++];
                std::memcpy(field.name,name.data(),name.size());field.value=value.value_or(false)?1u:0u;
            }
            return NIMBY_OK;
        });
    }
    uint32_t readBatch(uint64_t token,const uint64_t* ids,uint32_t count,
                       NimbyUiReadBatchHeaderV1* header,NimbyUiReadBatchRowV1* rows) noexcept {
        return boundary([&]() -> uint32_t {
            if(!header||header->size!=sizeof(*header)||header->version!=1||count>16384||(!ids&&count)||(!rows&&count))return NIMBY_INVALID_ARGUMENT;
            *header={};header->size=sizeof(*header);header->version=1;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            for(uint32_t i=0;i<count;++i)if(ids[i]>>48!=8)return NIMBY_INVALID_ARGUMENT;
            const auto values=store->readBatch({ids,count});
            if(values.fields.size()>64||values.rows.size()!=count)return NIMBY_INTERNAL_ERROR;
            header->field_count=static_cast<uint32_t>(values.fields.size());
            for(size_t i=0;i<values.fields.size();++i)copy(header->names[i],values.fields[i]);
            for(uint32_t i=0;i<count;++i){
                const auto& value=values.rows[i];auto& row=rows[i];row={};row.signal=ids[i];
                row.status=value.status==SettingsStatus::Present?2:value.status==SettingsStatus::Absent?1:0;
                row.values=value.values;
            }
            header->count=count;return NIMBY_OK;
        });
    }
private:
    std::map<uint64_t,std::vector<SignalUiHost::SettingsCopy>> copies_;
    uint64_t copySerial_=0;
public:
    uint32_t beginCopy(uint64_t source,uint64_t* token)noexcept {
        if(token)*token=0;
        return boundary([&]() -> uint32_t {
            std::lock_guard lock(copyMutex_);
            if(!token||source>>48!=8)return NIMBY_INVALID_ARGUMENT;
            if(copies_.size()>=16)return NIMBY_RESOURCE_LIMIT;
            auto copies=host.copySource(source);
            *token=++copySerial_;copies_.emplace(*token,std::move(copies));return NIMBY_OK;
        });
    }
    // count=0 cancels a prepared copy. Called before the native creation delta
    // is finalized; the next signal observation imports the frozen values.
    uint32_t finishCopy(uint64_t token,const uint64_t* ids,uint32_t count)noexcept {
        return boundary([&]() -> uint32_t {
            std::lock_guard lock(copyMutex_);
            if(count>64||(!ids&&count))return NIMBY_INVALID_ARGUMENT;
            auto found=copies_.find(token);if(found==copies_.end())return NIMBY_INVALID_HANDLE;
            auto copies=std::move(found->second);copies_.erase(found);
            if(!count)return NIMBY_OK;
            for(auto& copy:copies)if(!copy.store->queueCopies(copy.values,{ids,count}))return NIMBY_DATA_UNAVAILABLE;
            return NIMBY_OK;
        });
    }
private:
    template<size_t N> static void copy(char (&out)[N],std::string_view value){
        if(value.size()>=N)throw std::length_error("Signal action text too long");
        std::memcpy(out,value.data(),value.size());out[value.size()]=0;
    }
    template<size_t N> static std::string_view text(const char (&value)[N]) {
        const auto end=static_cast<const char*>(std::memchr(value,0,N));
        if(!end)throw std::invalid_argument("Unterminated UI text");
        return {value,static_cast<size_t>(end-value)};
    }
    template<class F> uint32_t boundary(F&& operation) noexcept {
        try {return operation();}
        catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}
        catch(const std::length_error&){return NIMBY_RESOURCE_LIMIT;}
        catch(...){return NIMBY_INTERNAL_ERROR;}
    }
    std::mutex copyMutex_;
};
}
