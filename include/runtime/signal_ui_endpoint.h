#pragma once
#include <nimby/detail/sdk.h>
#include <nimby/detail/observation.h>
#include <nimby/detail/signal_ui_bridge.h>
#include <nimby/detail/signal_settings_file.hpp>
#include "runtime/signal_ui_host.h"
#include <cstring>

namespace nimby::runtime {
// C ABI operations are serialized with unregister. A request cannot resurrect
// a removed store, even if the native renderer retains an older frame.
class SignalUiEndpoint {
public:
    SignalUiHost host;
    uint32_t add(const NimbyUiPanelV1* source,uint64_t* token) noexcept {
        if(token)*token=0;
        return boundary([&]() -> uint32_t {
            if(!source||!token||source->size!=sizeof(*source)||source->version!=NIMBY_SIGNAL_UI_ABI||
               source->reserved||!source->count||source->count>64)return NIMBY_INVALID_ARGUMENT;
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
    uint32_t begin(uint64_t token,const char* identity,uint32_t length,uint64_t* session) noexcept {
        if(session)*session=0;
        return boundary([&]() -> uint32_t {
            if(!identity||!session||!length||length>512)return NIMBY_INVALID_ARGUMENT;
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            *session=store->beginSession({identity,length});return NIMBY_OK;
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
            *session=store->beginSession({identity,identityLength},&saved);return NIMBY_OK;
        });
    }
    uint32_t suspend(uint64_t token) noexcept {
        return boundary([&]() -> uint32_t {
            const auto store=host.store(token);if(!store)return NIMBY_INVALID_HANDLE;
            store->suspendObservations();return NIMBY_OK;
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
private:
    template<size_t N> static std::string_view text(const char (&value)[N]) {
        const auto end=static_cast<const char*>(std::memchr(value,0,N));
        if(!end)throw std::invalid_argument("Unterminated UI text");
        return {value,static_cast<size_t>(end-value)};
    }
    template<class F> uint32_t boundary(F&& operation) noexcept {
        try {std::lock_guard lock(mutex_);return operation();}
        catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}
        catch(const std::length_error&){return NIMBY_RESOURCE_LIMIT;}
        catch(...){return NIMBY_INTERNAL_ERROR;}
    }
    std::mutex mutex_;
};
}
