#pragma once
#include "runtime/signal_actions.h"
#include <nimby/detail/sdk.h>
#include <array>
#include <cstdio>

namespace nimby::runtime {
inline const char* signalPreviewReason(SignalActions::PreviewReason reason,uint32_t status)noexcept {
    using Reason=SignalActions::PreviewReason;
    switch(reason){
        case Reason::None:break;
        case Reason::RegistryBusy:return "registry_busy";
        case Reason::ProviderMissing:return "provider_missing";
        case Reason::PanelMissing:return "panel_missing";
        case Reason::RequestCancelled:return "request_cancelled";
        case Reason::PanelInactive:return "panel_inactive";
        case Reason::ContextMissing:return "context_missing";
        case Reason::ProviderEpochChanged:return "provider_epoch_changed";
        case Reason::PreviewExpired:return "preview_expired";
        case Reason::ProviderExpired:return "provider_expired";
        case Reason::ProviderWorldChanged:return "provider_world_changed";
        case Reason::ProviderGenerationChanged:return "provider_generation_changed";
        case Reason::PanelWorldChanged:return "panel_world_changed";
        case Reason::PanelGenerationChanged:return "panel_generation_changed";
        case Reason::ServiceMissing:return "service_missing";
        case Reason::ActionMissing:return "action_missing";
        case Reason::StoreBusy:return "store_busy";
        case Reason::StoreRetired:return "store_retired";
        case Reason::StoreSessionMissing:return "store_session_missing";
        case Reason::ObservationsSuspended:return "observations_suspended";
        case Reason::StoreWorldChanged:return "store_world_changed";
        case Reason::ObservationEpochChanged:return "observation_epoch_changed";
        case Reason::SignalMissing:return "signal_missing";
    }
    switch(status){
        case NIMBY_OK:return "none";
        case NIMBY_HOOKS_UNAVAILABLE:return "hooks_unavailable";
        case NIMBY_DATA_UNAVAILABLE:return "native_world_unavailable";
        case NIMBY_INVALID_ARGUMENT:return "invalid_argument";
        case NIMBY_INVALID_HANDLE:return "invalid_handle";
        case NIMBY_RESOURCE_LIMIT:return "resource_limit";
        case NIMBY_INTERNAL_ERROR:return "internal_error";
        default:return "unspecified";
    }
}

// Failure-only, fixed-size limiter. A changing epoch or signal cannot generate
// unbounded disk writes; each provider gets at most four reports per five seconds.
// No successful tick touches this mutex and no logging runs under this lock.
class SignalPreviewDiagnostics {
public:
    using Clock=SignalActions::Clock;
    static constexpr size_t capacity=512; // Same bound as the provider registry.
    static constexpr auto interval=std::chrono::seconds(5);
    struct Admission {bool emit=false;uint64_t suppressed=0;};
    Admission admit(uint64_t provider,uint32_t status,SignalActions::PreviewReason reason,
                    uint64_t panel,uint64_t signal,Clock::time_point now=Clock::now()) {
        if(status==NIMBY_OK)return {};
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock)return {};
        auto slot=std::find_if(slots_.begin(),slots_.end(),[&](const Slot& value){return value.used&&value.provider==provider;});
        if(slot==slots_.end()){
            slot=std::find_if(slots_.begin(),slots_.end(),[](const Slot& value){return !value.used;});
            if(slot==slots_.end()){
                slot=std::min_element(slots_.begin(),slots_.end(),[](const Slot& a,const Slot& b){return a.window<b.window;});
                // Token churn cannot evict an active limiter window and restart
                // its burst allowance. This also bounds work before the sink.
                if(now-slot->window<interval)return {};
            }
            *slot={};slot->used=true;slot->provider=provider;slot->window=now;
        }
        auto& value=*slot;
        const bool elapsed=now-value.window>=interval;
        if(elapsed){value.window=now;value.burst=0;}
        const bool changed=value.status!=status||value.reason!=reason||value.panel!=panel||value.signal!=signal;
        if(value.burst>=4||(!changed&&!elapsed&&value.burst)){
            ++value.suppressed;return {};
        }
        const auto suppressed=value.suppressed;value.suppressed=0;
        value.status=status;value.reason=reason;value.panel=panel;value.signal=signal;++value.burst;
        return {true,suppressed};
    }
    // Called with a copied validation result, after all action/store locks are
    // released. The fixed buffer is also used by regression tests with a sink.
    template<class Sink>
    void report(uint64_t provider,const NimbyUiSignalPreviewV1* source,uint32_t status,
                const SignalActions::PreviewResult& result,Sink&& sink,Clock::time_point now=Clock::now()) {
        if(status==NIMBY_OK)return;
        const bool validHeader=source&&source->size==sizeof(*source)&&source->version==1;
        const auto panel=validHeader?source->panel:0,signal=validHeader?source->signal:0;
        const auto admission=admit(provider,status,result.diagnostic.reason,panel,signal,now);
        if(!admission.emit)return;
        const auto& d=result.diagnostic;
        char message[1024]{};
        // origin/service are fixed ABI arrays; precision never reads past them
        // even when reporting a malformed, non-terminated request.
        std::snprintf(message,sizeof message,
            "Signal preview rejected: status=%u reason=%s provider=%llu mod=%.128s panel=%llu signal=%llu count=%u "
            "origin=%.128s service=%.128s generation=%llu providerGeneration=%llu panelGeneration=%llu "
            "providerEpoch=%llu/%llu observationEpoch=%llu/%llu revision=%llu/%llu suppressed=%llu",
            status,signalPreviewReason(d.reason,status),number(provider),d.providerId,number(panel),number(signal),validHeader?source->count:0,
            validHeader?source->origin:"",validHeader?source->service:"",number(d.generation),number(d.providerGeneration),number(d.panelGeneration),
            number(d.expectedProviderEpoch),number(d.providerEpoch),number(d.expectedObservationEpoch),number(d.observationEpoch),
            number(d.expectedRevision),number(d.revision),number(admission.suppressed));
        sink(message);
    }
private:
    static unsigned long long number(uint64_t value){return static_cast<unsigned long long>(value);}
    struct Slot {
        bool used=false;uint64_t provider=0,panel=0,signal=0,suppressed=0;
        uint32_t status=0;SignalActions::PreviewReason reason=SignalActions::PreviewReason::None;
        Clock::time_point window{};unsigned burst=0;
    };
    std::mutex mutex_;
    std::array<Slot,capacity> slots_{};
};
}
