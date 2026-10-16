#pragma once
#include <nimby/detail/signal_route_values.hpp>
#include <nimby/route_conflict.hpp>
#include <array>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace nimby::detail {

// Diagnostic history only. This class neither captures native data nor returns
// route clearance, freshness, reservations or permission to move. Its caller
// supplies the already selected live source/train and validated copied rows.
class SignalRouteDiagnostic {
public:
    static constexpr size_t maxSources=4096,maxRows=65536,maxSamples=8,maxWorldBytes=256;
    struct Session {uint64_t generation=0;std::string_view worldId;};
    struct Source {
        uint64_t signal=0,approachingTrain=0;
        bool fresh=false;
        // Use LiveSignalState.routeConflict. A copied row rejected by live
        // source qualification must not manufacture a diagnostic conflict.
        std::optional<RouteConflict> conflict;
    };
    enum class Event : uint8_t {
        ConflictObserved,ObservationLost,TrainReplaced,RouteReplaced,
        SessionReplaced,SourceRemoved,TrackingReset,InputRejected,Count
    };
    enum class Reason : uint8_t {
        None,SessionUnavailable,StaleObservation,NoApproach,NoMatchingRow,
        ConflictCauseMissing,SessionChanged,SourceLimit,RowLimit,InvalidSource,
        ObservationLost,Restore,Stop
    };
    enum class Status : uint8_t {Accepted,SessionUnavailable,SourceLimit,RowLimit,InvalidSource};
    enum class Reset : uint8_t {ObservationLost,Restore,Stop};
    struct Fact {
        uint64_t train=0,revision=0,observedAtMs=0;
        std::optional<RouteConflict> conflict;
    };
    struct Sample {
        Event event=Event::ObservationLost;
        Reason reason=Reason::None;
        uint64_t signal=0,generation=0,previousGeneration=0;
        std::string worldId,previousWorldId;
        Fact previous,current;
    };
    struct Pending {
        std::array<uint64_t,size_t(Event::Count)> counts{};
        std::array<Sample,maxSamples> samples{};
        size_t sampleCount=0;
        uint64_t omittedSamples=0;
        bool saturated=false;
        bool empty()const noexcept {
            return std::all_of(counts.begin(),counts.end(),[](uint64_t value){return value==0;});
        }
        uint64_t count(Event event)const noexcept{return counts[size_t(event)];}
    };
    struct Summary {
        bool sessionQualified=false;
        size_t sources=0,qualifiedRoutes=0,observedConflicts=0,retainedHistoricalConflicts=0;
    };

    SignalRouteDiagnostic(){states_.reserve(maxSources);next_.reserve(maxSources);}

    // Pending counters survive every observation and reset. Throttle only the
    // writer, then acknowledge after successful output; a conflict that appears
    // and disappears between two log lines still contributes both transitions.
    const Pending& pending()const noexcept{return pending_;}
    const Summary& summary()const noexcept{return summary_;}
    void acknowledge()noexcept{pending_=Pending{};}
    size_t sourceStorageCapacity()const noexcept{return states_.capacity()+next_.capacity();}

    static constexpr std::string_view name(Event event)noexcept {
        constexpr std::array names{"conflict_observed","observation_lost","train_replaced","route_replaced",
            "session_replaced","source_removed","tracking_reset","input_rejected"};
        return size_t(event)<names.size()?names[size_t(event)]:"unknown";
    }
    static constexpr std::string_view name(Reason reason)noexcept {
        constexpr std::array names{"none","session_unavailable","stale_observation","no_approach","no_matching_row",
            "conflict_cause_missing","session_changed","source_limit","row_limit","invalid_source",
            "observation_lost","restore","stop"};
        return size_t(reason)<names.size()?names[size_t(reason)]:"unknown";
    }

    Status observe(std::optional<Session> session,std::span<const Source> sources,
        std::span<const NimbySignalRouteObservation> rows) {
        if(!session||!session->generation||session->worldId.empty()||session->worldId.size()>maxWorldBytes){
            retire(Reason::SessionUnavailable);return Status::SessionUnavailable;
        }
        if(sources.size()>maxSources)return reject(Status::SourceLimit,Reason::SourceLimit);
        if(rows.size()>maxRows)return reject(Status::RowLimit,Reason::RowLimit);
        next_.clear();
        for(const auto& source:sources){
            if(!identity(source.signal,8)||(source.approachingTrain&&!identity(source.approachingTrain,5))||
                (source.conflict&&*source.conflict!=RouteConflict::CrossingOccupied&&*source.conflict!=RouteConflict::CrossingReserved))
                return reject(Status::InvalidSource,Reason::InvalidSource);
            State state;state.source=source;state.fact.train=source.approachingTrain;next_.push_back(state);
        }
        std::sort(next_.begin(),next_.end(),[](const auto& a,const auto& b){return a.source.signal<b.source.signal;});
        if(std::adjacent_find(next_.begin(),next_.end(),[](const auto& a,const auto& b){return a.source.signal==b.source.signal;})!=next_.end())
            return reject(Status::InvalidSource,Reason::InvalidSource);

        // The first matching row has the same choice as SignalRoutes::at on its
        // indexed rows. Retain only one slot per source, never one per train or
        // revision. The pure row validator performs no native read.
        for(const auto& row:rows){
            auto it=std::lower_bound(next_.begin(),next_.end(),row.signal_id,
                [](const auto& state,uint64_t signal){return state.source.signal<signal;});
            if(it==next_.end()||it->source.signal!=row.signal_id||!it->source.fresh||
                it->source.approachingTrain!=row.train_id||!SignalRoutes::valid(row))continue;
            const auto conflict=cause(row.flags);
            if(conflict!=it->source.conflict||(it->rowKnown&&row.blocks>=it->blocks))continue;
            it->rowKnown=true;it->blocks=row.blocks;
            it->fact={row.train_id,row.route_revision,row.observed_at_ms,conflict};
        }

        if(summary_.sessionQualified&&(generation_!=session->generation||worldId_!=session->worldId)){
            // Replacement retires the old world's facts before installing any
            // new source, even if all object IDs and revisions were reused.
            record(Event::SessionReplaced,Reason::SessionChanged,0,{}, {},session);
            for(const auto& state:states_)if(state.rowKnown)
                record(Event::ObservationLost,Reason::SessionChanged,state.source.signal,state.fact,{});
            states_.clear();
        }
        generation_=session->generation;worldId_.assign(session->worldId);
        summary_.sessionQualified=true;

        size_t oldIndex=0;
        for(auto& state:next_){
            while(oldIndex<states_.size()&&states_[oldIndex].source.signal<state.source.signal){
                removed(states_[oldIndex]);++oldIndex;
            }
            const State* previous=oldIndex<states_.size()&&states_[oldIndex].source.signal==state.source.signal?&states_[oldIndex++]:nullptr;
            if(previous){
                state.lastRoute=previous->lastRoute;
                state.lastConflict=previous->lastConflict;
                const auto& earlier=previous->source.approachingTrain?previous->fact:previous->lastRoute;
                const bool trainChanged=earlier.train&&state.source.approachingTrain&&earlier.train!=state.source.approachingTrain;
                if(trainChanged)record(Event::TrainReplaced,Reason::None,state.source.signal,earlier,state.fact);
                // Last route identity survives a gap only as history. It can
                // explain replacement on recovery, never fill the current row.
                const bool routeChanged=!trainChanged&&state.rowKnown&&previous->lastRoute.revision&&
                    previous->lastRoute.train==state.fact.train&&previous->lastRoute.revision!=state.fact.revision;
                if(routeChanged)record(Event::RouteReplaced,Reason::None,state.source.signal,previous->lastRoute,state.fact);
                if(previous->rowKnown&&(!state.rowKnown||(previous->fact.conflict&&!state.fact.conflict)))
                    record(Event::ObservationLost,lossReason(state),state.source.signal,previous->fact,state.fact);
                if(state.fact.conflict&&(!previous->fact.conflict||!previous->rowKnown||trainChanged||routeChanged||
                    previous->fact.conflict!=state.fact.conflict))
                    record(Event::ConflictObserved,Reason::None,state.source.signal,previous->fact,state.fact);
            }else if(state.fact.conflict)record(Event::ConflictObserved,Reason::None,state.source.signal,{},state.fact);
            if(state.rowKnown)state.lastRoute=state.fact;
            if(state.fact.conflict)state.lastConflict=state.fact;
        }
        while(oldIndex<states_.size())removed(states_[oldIndex++]);
        states_.swap(next_);next_.clear();
        summarize();return Status::Accepted;
    }

    // Called by stop/lost/restore without discarding transitions waiting for the
    // next log write. No historical source can become current after this call.
    void reset(Reset resetReason) {
        const auto reason=resetReason==Reset::Stop?Reason::Stop:
            resetReason==Reset::Restore?Reason::Restore:Reason::ObservationLost;
        if(summary_.sessionQualified||!states_.empty())record(Event::TrackingReset,reason,0,{},{});
        retire(reason);
    }

private:
    struct State {
        Source source;
        Fact fact,lastRoute,lastConflict;
        bool rowKnown=false;
        uint32_t blocks=0;
    };
    std::vector<State> states_,next_;
    uint64_t generation_=0;
    std::string worldId_;
    Summary summary_;
    Pending pending_;

    static bool identity(uint64_t value,uint64_t kind)noexcept {
        return value>>48==kind&&(value&UINT64_C(0x0000ffffffffffff))!=0;
    }
    static std::optional<RouteConflict> cause(uint32_t flags)noexcept {
        if(flags&NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED)return RouteConflict::CrossingOccupied;
        if(flags&NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED)return RouteConflict::CrossingReserved;
        return {};
    }
    void increment(uint64_t& value)noexcept {
        if(value==std::numeric_limits<uint64_t>::max())pending_.saturated=true;
        else ++value;
    }
    void record(Event event,Reason reason,uint64_t signal,const Fact& previous,const Fact& current,
        std::optional<Session> replacement={}) {
        increment(pending_.counts[size_t(event)]);
        if(pending_.sampleCount==maxSamples){increment(pending_.omittedSamples);return;}
        auto& sample=pending_.samples[pending_.sampleCount++];
        sample.event=event;sample.reason=reason;sample.signal=signal;sample.previous=previous;sample.current=current;
        sample.generation=replacement?replacement->generation:generation_;
        sample.worldId=replacement?replacement->worldId:std::string_view(worldId_);
        if(replacement){sample.previousGeneration=generation_;sample.previousWorldId=worldId_;}
        else if(previous.train){sample.previousGeneration=generation_;sample.previousWorldId=worldId_;}
    }
    static Reason lossReason(const State& state)noexcept {
        if(!state.source.fresh)return Reason::StaleObservation;
        if(!state.source.approachingTrain)return Reason::NoApproach;
        if(!state.rowKnown)return Reason::NoMatchingRow;
        return Reason::ConflictCauseMissing;
    }
    void removed(const State& state){
        record(Event::SourceRemoved,Reason::None,state.source.signal,
            state.rowKnown||!state.lastConflict.conflict?state.fact:state.lastConflict,{});
    }
    void summarize()noexcept {
        summary_.sources=states_.size();summary_.qualifiedRoutes=0;summary_.observedConflicts=0;
        summary_.retainedHistoricalConflicts=0;
        for(const auto& state:states_){
            if(state.rowKnown)++summary_.qualifiedRoutes;
            if(state.fact.conflict)++summary_.observedConflicts;
            else if(state.lastConflict.conflict)++summary_.retainedHistoricalConflicts;
        }
    }
    void retire(Reason reason){
        for(const auto& state:states_)if(state.rowKnown)
            record(Event::ObservationLost,reason,state.source.signal,state.fact,{});
        states_.clear();next_.clear();generation_=0;worldId_.clear();summary_={};
    }
    Status reject(Status status,Reason reason){
        record(Event::InputRejected,reason,0,{},{});retire(reason);return status;
    }
};
}
