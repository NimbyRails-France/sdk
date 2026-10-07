#include <platform/windows/mod_host_epoch.h>
#include <runtime/observation_epoch.h>
#include <platform/observation_process.h>
#include <engine/simulation_clock.h>
#include <windows.h>
#include <algorithm>
#include <bit>
#include <memory>
#include <mutex>

namespace nimby::mod_host {
namespace {
using Epoch=runtime::ObservationEpoch;
// Fixed-width wire fields; no mod pointers or compiler-specific containers.
struct Sample {
    uint64_t module{},root{},database{},copy{},simulation{};
    uint32_t profile{},historyCount{};
    engine::VersioningObservation::Value world{};
    int64_t ticks{};
    uint32_t hasTicks{},reserved{};
};
static_assert(sizeof(Sample)==96);
struct Parent {
    std::mutex mutex;
    uint32_t target{};
    uint64_t serial{};
    std::unique_ptr<platform::ObservationProcess> process;
    // The broker always derives its epoch locally, even in facade tests that
    // install a child authority in this same executable.
    std::unique_ptr<Epoch> epoch=std::make_unique<Epoch>(false);
};
Parent& parent(){static Parent state;return state;}
bool readMemory(void* context,uint64_t address,void* output,size_t size) {
    return static_cast<platform::ObservationProcess*>(context)->read(address,output,size);
}
uint32_t refresh(Parent& state,engine::VersioningObservation& observed,engine::SimulationClock& clock,uint64_t& generation) {
    if(!state.process){
        auto process=std::make_unique<platform::ObservationProcess>();
        const auto status=process->open(state.target?state.target:GetCurrentProcessId());
        if(status!=NIMBY_OK)return status;
        state.process=std::move(process);
    }
    auto& process=*state.process;
    if(!process.alive()){state.process.reset();state.epoch->unavailable();return NIMBY_PROCESS_EXITED;}
    engine::VersioningObservation before;
    if(!engine::read_versioning_observation(readMemory,&process,process.base,true,before,process.profile())||
       !engine::read_simulation_clock(readMemory,&process,before.state.simulation,clock)||
       !engine::read_versioning_observation(readMemory,&process,process.base,true,observed,process.profile())||
       before.state!=observed.state||before.value!=observed.value||before.history!=observed.history){
        state.epoch->unavailable();return NIMBY_DATA_UNAVAILABLE;
    }
    generation=state.epoch->observe(observed,clock.ticks);return NIMBY_OK;
}
Epoch::Ticket remoteBegin(void*) {
    Request request;request.operation=2;Reply reply;
    if(invoke(request,reply,0)!=NIMBY_OK)return {};
    return {reply.args[0],reply.args[1],std::bit_cast<int64_t>(reply.args[2])};
}
uint64_t remoteObserve(void*,Epoch::Ticket ticket,const engine::VersioningObservation& observation,std::optional<int64_t> ticks) {
    if(!ticket.started||!ticket.revision||observation.history.size()>4096)return 0;
    Request request;request.operation=3;
    request.args[0]=ticket.started;request.args[1]=ticket.revision;request.args[2]=std::bit_cast<uint64_t>(ticket.clockTicks);
    const auto& state=observation.state;
    Sample sample{state.module_base,state.root,state.database,state.copy,state.simulation,
        static_cast<uint32_t>(state.profile),static_cast<uint32_t>(observation.history.size()),observation.value,
        ticks.value_or(0),ticks.has_value()?1u:0u,0};
    append(request,&sample);append(request,observation.history.data(),observation.history.size());
    Reply reply;if(invoke(request,reply,0)!=NIMBY_OK)return 0;return reply.args[0];
}
}
void configureEpochTarget(uint32_t targetPid) {
    auto& state=parent();std::lock_guard lock(state.mutex);
    state.target=targetPid;state.process.reset();state.epoch=std::make_unique<Epoch>(false);
    // Keep ticket IDs monotonic across supervisor restarts.
}
void installChildEpochAuthority() {
    static const Epoch::Authority authority{nullptr,remoteBegin,remoteObserve};Epoch::setAuthority(&authority);
}
uint32_t dispatchEpoch(const Request& request,Reply& reply,Owners& owners) {
    if(request.operation!=2&&request.operation!=3)return NIMBY_INVALID_ARGUMENT;
    Sample sample{};
    if(request.operation==2){if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;}
    else if(!read(request,0,sample)||sample.hasTicks>1||sample.reserved||sample.historyCount>4096||
        request.data.size()!=sizeof sample+sample.historyCount*sizeof(engine::VersioningObservation::Value))return NIMBY_INVALID_ARGUMENT;
    const auto now=GetTickCount64();
    std::erase_if(owners.epochs,[&](const auto& ticket){return now>=ticket.expires;});
    Owners::EpochTicket issued{};
    if(request.operation==3){
        const auto found=std::find_if(owners.epochs.begin(),owners.epochs.end(),[&](const auto& entry){return entry.id==request.args[0];});
        if(found==owners.epochs.end())return NIMBY_DATA_UNAVAILABLE;
        issued=*found;owners.epochs.erase(found); // A ticket is consumed once, even on rejection.
        if(issued.generation!=request.args[1]||std::bit_cast<uint64_t>(issued.ticks)!=request.args[2])return NIMBY_DATA_UNAVAILABLE;
    }else if(owners.epochs.size()>=64){
        // Failed/abandoned captures never stall later observations. Retain a
        // bounded window; only this worker's oldest unfinished ticket expires.
        owners.epochs.erase(owners.epochs.begin());
    }
    auto& state=parent();std::lock_guard lock(state.mutex);
    engine::VersioningObservation observed;engine::SimulationClock clock;uint64_t generation{};
    const auto status=refresh(state,observed,clock,generation);if(status!=NIMBY_OK)return status;
    if(request.operation==2){
        if(state.serial==UINT64_MAX)return NIMBY_RESOURCE_LIMIT;
        owners.epochs.push_back({++state.serial,generation,clock.ticks,now+10000});
        reply.args[0]=state.serial;reply.args[1]=generation;reply.args[2]=std::bit_cast<uint64_t>(clock.ticks);return NIMBY_OK;
    }
    const auto& current=observed.state;
    if(generation!=issued.generation||clock.ticks<issued.ticks||sample.module!=current.module_base||sample.root!=current.root||
        sample.database!=current.database||sample.copy!=current.copy||sample.simulation!=current.simulation||
        sample.profile!=static_cast<uint32_t>(current.profile)||sample.world!=observed.value||sample.historyCount!=observed.history.size()||
        (sample.historyCount&&std::memcmp(request.data.data()+sizeof sample,observed.history.data(),sample.historyCount*sizeof(engine::VersioningObservation::Value)))||
        (sample.hasTicks&&(sample.ticks<issued.ticks||sample.ticks>clock.ticks)))return NIMBY_DATA_UNAVAILABLE;
    reply.args[0]=generation;return NIMBY_OK;
}
}
