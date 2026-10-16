#pragma once
#include <engine/route_shadow_batch.h>
#include <engine/track_conflicts.h>
#include <engine/versioning.h>
#include <nimby/detail/diagnostics.hpp>
#include <windows.h>
#include <atomic>
#include <sstream>

namespace nimby::windows::automatic {
// Diagnostic prototype, explicitly opt-in. It observes only batches requested
// by today's mod route readers; it is NOT universal train observation. Native
// callbacks never call it. There is no permission/writer hook or rear-clear
// producer in this adapter.
inline bool shadowObservationRequested()noexcept {
    static const bool enabled=[](){wchar_t value[4]{};
        return GetEnvironmentVariableW(L"NRF_INTERLOCKING_SHADOW",value,4)==1&&value[0]==L'1';}();
    return enabled;
}
struct ShadowReadBudget {
    size_t reads=8192,bytes=2*1024*1024;
    LARGE_INTEGER started{},frequency{};
    bool exhausted=false;
    ShadowReadBudget()noexcept {
        if(!QueryPerformanceFrequency(&frequency)||!QueryPerformanceCounter(&started))exhausted=true;
    }
    // Explicit preflight used before sampling CPU work and allocations. A
    // terminal refusal cannot be bypassed by a subsequent smaller request.
    static bool available(void* context,size_t size)noexcept {
        auto& budget=*static_cast<ShadowReadBudget*>(context);
        if(budget.exhausted)return false;
        LARGE_INTEGER now{};
        if(!budget.reads||!budget.bytes||size>budget.bytes||budget.frequency.QuadPart<=0||
           !QueryPerformanceCounter(&now)||now.QuadPart<budget.started.QuadPart||
           now.QuadPart-budget.started.QuadPart>budget.frequency.QuadPart/250){
            budget.exhausted=true;return false;
        }
        return true;
    }
    static bool copy(void* context,uint64_t address,void* output,size_t size)noexcept {
        auto& budget=*static_cast<ShadowReadBudget*>(context);
        if(!available(context,size))return false;
        --budget.reads;budget.bytes-=size;SIZE_T copied{};
        const bool complete=ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(address),output,size,&copied)&&copied==size;
        // A read that crosses the cooperative limit must not authorize the
        // caller to allocate or process the bytes after returning from it.
        return available(context,0)&&complete;
    }
};
struct RouteShadowWorker {
    nimby::engine::interlocking::ShadowCoordinator coordinator;
    std::optional<nimby::engine::VersioningObservation> bootstrap;
    // Versioning is not a qualified world-load event. If its continuity is
    // lost, latch unavailable until process retirement; NEVER clear retained
    // leases by mistaking an edit/reload/missing read for a new world.
    bool lineageLost=false;
    uint64_t skipped{},nextSkipReport{};
};
// Cooperative owner-side work shares the sampling budget. Checks precede
// allocations and occur periodically inside pure expansion/sorting; no hard
// deadline can preempt an OS read, allocator call or scheduling interruption.
// A stopped plan is wholly unavailable and changes no native permissions.
inline bool prepareShadowPlan(const nimby::engine::TrackConflicts& captured,
    std::span<const NimbySignalRouteObservation> routes,
    std::span<const NimbySignalRouteSection> sections,
    nimby::engine::TrackConflictReadBudget& budget,
    nimby::engine::interlocking::ShadowBatchPlan& output){
    using namespace nimby::engine;using namespace nimby::engine::interlocking;
    output={};if(!budget.available())return false;
    std::map<uint64_t,std::vector<NimbyTrackConflict>> grouped;
    for(const auto& row:captured.rows){
        if(!budget.available())return false; // Map insertion may allocate.
        auto& rows=grouped[row.track_id];
        if(rows.size()==rows.capacity()&&!budget.available())return false;
        rows.push_back(row);
    }
    if(!budget.available())return false;
    std::vector<ConflictList> lists;lists.reserve(captured.states.size());
    size_t checked=0;
    for(const auto& row:captured.states){
        if(checked++%32==0&&!budget.available())return false;
        const auto found=grouped.find(row.track_id);
        lists.push_back({row.track_id,row.flags&NIMBY_TRACK_CONFLICTS_VALID?
            TrackConflictReason::Complete:static_cast<TrackConflictReason>(row.reason),
            found==grouped.end()?std::span<const NimbyTrackConflict>{}:std::span<const NimbyTrackConflict>(found->second)});
    }
    if(!budget.available())return false;
    auto plan=planShadowBatch(1,routes,sections,lists,&budget);
    if(!budget.available())return false; // An expired plan must never be submitted.
    output=std::move(plan);return true;
}
inline bool submitShadowPlan(nimby::engine::interlocking::ShadowCoordinator& coordinator,
    const nimby::engine::interlocking::ShadowBatchPlan& plan,
    nimby::engine::TrackConflictReadBudget& budget,
    std::optional<nimby::engine::interlocking::ShadowReport>& report){
    using namespace nimby::engine::interlocking;
    if(!budget.available())return false;
    for(const auto& row:plan.routes){
        if(!budget.available())return false;
        // This observer only queues copied routes, without a physical-entry
        // proof. The core's Unknown-entry path retains existing leases and
        // performs no resource allocation or conflict expansion when drained.
        if(row.request.entered!=Proof::Unknown)return false;
        coordinator.trySubmit(row.request,row.resources);
    }
    if(!budget.available())return false;
    const auto at=GetTickCount64();
    if(!budget.available())return false;
    // At most 64 Unknown-entry events, each copying at most 512 intervals.
    // Already submitted events stay queued if a preceding check expires;
    // no reset or release is used to discard a partial submission.
    auto drained=coordinator.drain({1,at});
    if(!budget.available())return false;
    report=std::move(drained.report);return true;
}
inline void observeShadowRoutes(std::span<const NimbySignalRouteObservation> routes,
    std::span<const NimbySignalRouteSection> sections)noexcept {
    if(!shadowObservationRequested())return; // No memory sample/read when off.
    if(routes.empty())return; // Missing observations never release a lease.
    using namespace nimby::engine;
    using namespace nimby::engine::interlocking;
    static std::atomic_flag owner=ATOMIC_FLAG_INIT;
    static std::atomic<uint64_t> ownerBusy{};
    if(owner.test_and_set(std::memory_order_acquire)){
        ownerBusy.fetch_add(1,std::memory_order_relaxed);return; // Peers never wait.
    }
    struct OwnerExit {std::atomic_flag* flag;~OwnerExit(){flag->clear(std::memory_order_release);}};
    std::optional<ShadowReport> report;
    uint64_t skipped=0;bool lineageLost=false,reportSkip=false;
    try {
        {OwnerExit release{&owner};
        static std::unique_ptr<RouteShadowWorker> worker;
        if(!worker)worker=std::make_unique<RouteShadowWorker>();
        auto& state=*worker;const auto now=GetTickCount64();
        const auto skip=[&]{++state.skipped;
            if(now>=state.nextSkipReport){state.nextSkipReport=now+1000;reportSkip=true;skipped=state.skipped;lineageLost=state.lineageLost;}};
        if(state.lineageLost||routes.size()>64||sections.size()>4096){skip();}
        else {
            ShadowReadBudget budget;VersioningObservation before{},after{};TrackConflicts captured;
            TrackConflictReadBudget conflictBudget{ShadowReadBudget::available,&budget};
            const auto module=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
            std::vector<uint64_t> tracks;bool prepared=conflictBudget.available();
            if(prepared){
                tracks.reserve(sections.size());
                for(const auto& section:sections){
                    if(!conflictBudget.available()){prepared=false;break;}
                    if(section.track_id>>48==1)tracks.push_back(section.track_id);
                }
                if(prepared&&conflictBudget.available()){
                    std::sort(tracks.begin(),tracks.end());tracks.erase(std::unique(tracks.begin(),tracks.end()),tracks.end());
                }else prepared=false;
            }
            const bool beforeRead=prepared&&conflictBudget.available()&&
                read_versioning_observation(ShadowReadBudget::copy,&budget,module,true,before);
            const bool conflictsRead=beforeRead&&!tracks.empty()&&
                read_track_conflicts(ShadowReadBudget::copy,&budget,before.state,true,tracks,captured,&conflictBudget);
            // Check continuity even if F8 is unavailable. Missing conflict
            // rows alone do not imply a new world; missing identity metadata
            // after bootstrap prevents safely resuming a historical observer.
            const bool afterRead=beforeRead&&
                read_versioning_observation(ShadowReadBudget::copy,&budget,module,true,after);
            const bool continuous=beforeRead&&afterRead&&before.state==after.state&&
                before.value==after.value&&before.history==after.history;
            if(state.bootstrap&&(!continuous||state.bootstrap->state!=before.state||
               state.bootstrap->value!=before.value||state.bootstrap->history!=before.history))state.lineageLost=true;
            const bool qualified=continuous&&conflictsRead;
            if(!qualified){skip();}
            else {
                if(state.lineageLost){skip();}
                else {
                    const auto process=[&](){
                        ShadowBatchPlan plan;
                        if(!prepareShadowPlan(captured,routes,sections,conflictBudget,plan))return false;
                        if(!state.bootstrap){
                            if(!conflictBudget.available())return false;
                            auto bootstrap=before;
                            if(!conflictBudget.available())return false;
                            state.coordinator.beginSession(1);state.bootstrap=std::move(bootstrap);
                        }
                        return submitShadowPlan(state.coordinator,plan,conflictBudget,report);
                    };
                    if(!process()){report.reset();skip();}
                }
            }
        }
        } // Owner gate released before all formatting and diagnostic writes.
        if(report){
            std::ostringstream message;
            message<<"Interlocking shadow: scope=requested_mod_routes non_atomic=1 native_permissions_changed=0 entry_proof_producer=0 rear_clear_producer=0"
                   <<" session="<<report->session<<" processed="<<report->processed<<" leases="<<report->leases
                   <<" resources="<<report->resources<<" busy="<<report->busy<<" full="<<report->full<<" invalid="<<report->invalid
                   <<" owner_busy_total="<<ownerBusy.load(std::memory_order_relaxed)<<" outcomes={";
            for(size_t i=0;i<report->outcomes.size();++i){if(i)message<<',';message<<i<<':'<<report->outcomes[i];}
            message<<"} comparisons={";
            for(size_t i=0;i<report->comparisons.size();++i){if(i)message<<',';message<<i<<':'<<report->comparisons[i];}
            message<<'}';
            for(size_t i=0;i<report->sampleCount;++i){const auto& row=report->samples[i];
                message<<" sample={train="<<row.ticket.train<<" signal="<<row.ticket.source<<" revision="<<row.ticket.routeRevision
                       <<" outcome="<<static_cast<unsigned>(row.result.outcome)<<" native_stage="<<static_cast<unsigned>(row.nativeStage)
                       <<" comparison="<<static_cast<unsigned>(row.comparison)<<" resource_reason="<<static_cast<unsigned>(row.resources)
                       <<" entered="<<static_cast<unsigned>(row.entered)<<" rear_clear="<<static_cast<unsigned>(row.rearClear);
                if(row.result.blocker)message<<" blocker_train="<<row.result.blocker->train<<" blocker_signal="<<row.result.blocker->source;
                message<<'}';
            }
            nimby::detail::diagnostics::write("sdk","INFO",message.str().c_str());
        }
        if(reportSkip){std::ostringstream message;message<<"Interlocking shadow unavailable: scope=requested_mod_routes skipped="<<skipped
            <<" lineage_lost="<<lineageLost<<" native_permissions_changed=0 retained_leases_not_released=1";
            nimby::detail::diagnostics::write("sdk","WARN",message.str().c_str());}
    }catch(...){/* A shadow observer failure never changes the native/RPC result. */}
}
} // namespace nimby::windows::automatic
