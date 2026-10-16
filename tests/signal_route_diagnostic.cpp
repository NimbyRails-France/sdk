#define NIMBY_SDK_BUILD
#include <nimby/detail/signal_route_diagnostic.hpp>
#include <iostream>
#include <stdexcept>

#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
using Diagnostic=nimby::detail::SignalRouteDiagnostic;
using Event=Diagnostic::Event;
using Reason=Diagnostic::Reason;
constexpr uint64_t signal=(UINT64_C(8)<<48)|1,train=(UINT64_C(5)<<48)|1,track=(UINT64_C(1)<<48)|1;
constexpr Diagnostic::Session session{7,"world-a"};
constexpr auto occupied=nimby::RouteConflict::CrossingOccupied,reserved=nimby::RouteConflict::CrossingReserved;

NimbySignalRouteObservation row(uint64_t trainId=train,uint64_t revision=1,
    std::optional<nimby::RouteConflict> conflict=occupied,uint64_t signalId=signal){
    NimbySignalRouteObservation value{};
    value.train_id=trainId;value.signal_id=signalId;value.route_revision=revision;value.observed_at_ms=1000;
    value.head_track_id=track;value.head_fraction=.1;value.head_direction=1;value.distance_m=100;
    value.blocks=1;value.flags=NIMBY_SIGNAL_ROUTE_ORDERED;
    if(conflict)value.flags|=*conflict==occupied?NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED:NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED;
    value.source_track_id=track;value.source_fraction=.4;value.source_direction=1;value.source_kind=NIMBY_SIGNAL_PATH;
    return value;
}
Diagnostic::Source source(uint64_t trainId=train,std::optional<nimby::RouteConflict> conflict=occupied,uint64_t signalId=signal){
    return {signalId,trainId,true,conflict};
}
void observe(Diagnostic& diagnostic,Diagnostic::Source selected,NimbySignalRouteObservation observed){
    CHECK(diagnostic.observe(session,std::span(&selected,1),std::span(&observed,1))==Diagnostic::Status::Accepted);
}

void transitionsSurviveWriterCadence(){
    Diagnostic diagnostic;auto selected=source();auto observed=row();
    observe(diagnostic,selected,observed);observe(diagnostic,selected,observed);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==1);
    CHECK(diagnostic.summary().observedConflicts==1);
    CHECK(diagnostic.observe(session,std::span(&selected,1),{})==Diagnostic::Status::Accepted);
    CHECK(diagnostic.summary().observedConflicts==0&&diagnostic.summary().retainedHistoricalConflicts==1);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    CHECK(diagnostic.pending().samples[1].reason==Reason::NoMatchingRow);
    CHECK(diagnostic.pending().samples[1].previous.conflict==occupied);
    // Neither a repeated outage nor a stable row creates repeated transitions.
    diagnostic.observe(session,std::span(&selected,1),{});
    observe(diagnostic,selected,observed);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==2);
    diagnostic.acknowledge();CHECK(diagnostic.pending().empty());
    CHECK(diagnostic.summary().observedConflicts==1);
}

void missingCauseNeverMeansClear(){
    Diagnostic diagnostic;observe(diagnostic,source(),row());diagnostic.acknowledge();
    observe(diagnostic,source(train,{}),row(train,1,{}));
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    CHECK(diagnostic.pending().samples[0].reason==Reason::ConflictCauseMissing);
    CHECK(diagnostic.summary().qualifiedRoutes==1&&diagnostic.summary().observedConflicts==0);
    CHECK(diagnostic.summary().retainedHistoricalConflicts==1);
    diagnostic.acknowledge();observe(diagnostic,source(train,{}),row(train,1,{}));
    CHECK(diagnostic.pending().empty());
    // A positive copied row cannot override a rejected live conflict.
    observe(diagnostic,source(train,{}),row());
    CHECK(diagnostic.summary().qualifiedRoutes==0&&diagnostic.summary().observedConflicts==0);
    CHECK(diagnostic.pending().samples[0].reason==Reason::NoMatchingRow);
    diagnostic.acknowledge();
    Diagnostic unknown;observe(unknown,source(train,{}),row(train,1,{}));
    CHECK(unknown.pending().empty()&&unknown.summary().observedConflicts==0);
}

void replacementsDoNotReuseEarlierFacts(){
    Diagnostic diagnostic;observe(diagnostic,source(),row());diagnostic.acknowledge();
    observe(diagnostic,source(train+1,reserved),row(train+1,1,reserved));
    CHECK(diagnostic.pending().count(Event::TrainReplaced)==1);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==1);
    CHECK(diagnostic.pending().samples[0].previous.train==train);
    CHECK(diagnostic.pending().samples[0].current.train==train+1);
    diagnostic.acknowledge();
    observe(diagnostic,source(train+1,reserved),row(train+1,2,reserved));
    CHECK(diagnostic.pending().count(Event::RouteReplaced)==1);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==1);
    CHECK(diagnostic.pending().samples[0].previous.revision==1&&diagnostic.pending().samples[0].current.revision==2);
    diagnostic.acknowledge();
    // A row for the previous train is not evidence for its replacement.
    observe(diagnostic,source(train+2),row(train+1,2,reserved));
    CHECK(diagnostic.pending().count(Event::TrainReplaced)==1);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    CHECK(diagnostic.summary().qualifiedRoutes==0&&diagnostic.summary().observedConflicts==0);
}

void recoveryComparesHistoryWithoutRefreshingIt(){
    Diagnostic diagnostic;auto selected=source();
    observe(diagnostic,selected,row());diagnostic.acknowledge();
    diagnostic.observe(session,std::span(&selected,1),{});
    CHECK(diagnostic.summary().qualifiedRoutes==0&&diagnostic.summary().retainedHistoricalConflicts==1);
    diagnostic.acknowledge();observe(diagnostic,selected,row(train,2));
    CHECK(diagnostic.pending().count(Event::RouteReplaced)==1);
    CHECK(diagnostic.pending().samples[0].previous.revision==1&&diagnostic.pending().samples[0].current.revision==2);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==1);
    diagnostic.acknowledge();auto absent=source(0,{});
    diagnostic.observe(session,std::span(&absent,1),{});
    CHECK(diagnostic.summary().qualifiedRoutes==0&&diagnostic.summary().observedConflicts==0);
    diagnostic.acknowledge();observe(diagnostic,source(train+1),row(train+1,3));
    CHECK(diagnostic.pending().count(Event::TrainReplaced)==1);
    CHECK(diagnostic.pending().samples[0].previous.train==train&&diagnostic.pending().samples[0].previous.revision==2);
    // A previous qualified row without a crossing cause is also history; no
    // positive conflict is invented when its revision changes after a gap.
    diagnostic.acknowledge();observe(diagnostic,source(train+1,{}),row(train+1,4,{}));
    diagnostic.acknowledge();selected=source(train+1,{});
    diagnostic.observe(session,std::span(&selected,1),{});diagnostic.acknowledge();
    observe(diagnostic,selected,row(train+1,5,{}));
    CHECK(diagnostic.pending().count(Event::RouteReplaced)==1);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==0&&diagnostic.summary().observedConflicts==0);
}

void sessionSourceAndLifecycleChanges(){
    Diagnostic diagnostic;auto selected=source();auto observed=row();observe(diagnostic,selected,observed);diagnostic.acknowledge();
    // Same generation and recycled full object IDs in a different world still
    // form a new session, including in delayed diagnostic samples.
    const Diagnostic::Session other{session.generation,"world-b"};
    diagnostic.observe(other,std::span(&selected,1),std::span(&observed,1));
    CHECK(diagnostic.pending().count(Event::SessionReplaced)==1);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==1);
    const auto& changed=diagnostic.pending().samples[0];
    CHECK(changed.previousWorldId=="world-a"&&changed.worldId=="world-b");
    diagnostic.acknowledge();
    const Diagnostic::Session generation{session.generation+1,"world-b"};
    diagnostic.observe(generation,std::span(&selected,1),std::span(&observed,1));
    CHECK(diagnostic.pending().count(Event::SessionReplaced)==1);
    diagnostic.acknowledge();diagnostic.observe(generation,{},{});
    CHECK(diagnostic.pending().count(Event::SourceRemoved)==1&&diagnostic.summary().sources==0);
    CHECK(diagnostic.pending().samples[0].previous.conflict==occupied);
    diagnostic.acknowledge();observe(diagnostic,selected,observed);
    diagnostic.observe(std::nullopt,std::span(&selected,1),std::span(&observed,1));
    CHECK(!diagnostic.summary().sessionQualified&&diagnostic.summary().sources==0);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    diagnostic.observe(std::nullopt,{},{});
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    for(const auto reset:{Diagnostic::Reset::ObservationLost,Diagnostic::Reset::Restore,Diagnostic::Reset::Stop}){
        diagnostic.acknowledge();observe(diagnostic,selected,observed);diagnostic.reset(reset);
        CHECK(diagnostic.pending().count(Event::TrackingReset)==1);
        CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
        CHECK(diagnostic.pending().count(Event::ConflictObserved)==1); // Not lost with reset.
        CHECK(!diagnostic.summary().sessionQualified&&diagnostic.summary().observedConflicts==0);
        diagnostic.reset(reset);CHECK(diagnostic.pending().count(Event::TrackingReset)==1);
    }
}

void unavailableAndInvalidInputsAreRestrictiveDiagnostics(){
    for(const auto unavailable:{0,1,2}){
        Diagnostic diagnostic;auto selected=source();auto observed=row();observe(diagnostic,selected,observed);diagnostic.acknowledge();
        if(unavailable==0)selected.fresh=false;
        if(unavailable==1){selected.approachingTrain=0;selected.conflict.reset();}
        if(unavailable==2)observed.route_revision=0;
        observe(diagnostic,selected,observed);
        CHECK(diagnostic.summary().observedConflicts==0&&diagnostic.summary().qualifiedRoutes==0);
        CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    }
    Diagnostic diagnostic;auto selected=source();auto observed=row();observe(diagnostic,selected,observed);diagnostic.acknowledge();
    std::array duplicates{selected,selected};
    CHECK(diagnostic.observe(session,duplicates,{})==Diagnostic::Status::InvalidSource);
    CHECK(diagnostic.summary().sources==0&&diagnostic.pending().count(Event::InputRejected)==1);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==1);
    for(const auto bad:std::array<Diagnostic::Session,3>{{{0,"world"},{1,""},{1,std::string_view{}}}})
        CHECK(diagnostic.observe(bad,{}, {})==Diagnostic::Status::SessionUnavailable);
    const std::string tooLong(Diagnostic::maxWorldBytes+1,'w');
    CHECK(diagnostic.observe(Diagnostic::Session{1,tooLong},{},{})==Diagnostic::Status::SessionUnavailable);
    selected.signal=UINT64_C(8)<<48;
    CHECK(diagnostic.observe(session,std::span(&selected,1),{})==Diagnostic::Status::InvalidSource);
    selected=source(UINT64_C(5)<<48);
    CHECK(diagnostic.observe(session,std::span(&selected,1),{})==Diagnostic::Status::InvalidSource);
}

void boundsAndLongRunningMemory(){
    Diagnostic diagnostic;const auto capacity=diagnostic.sourceStorageCapacity();
    CHECK(capacity==Diagnostic::maxSources*2);
    std::vector<Diagnostic::Source> sources;std::vector<NimbySignalRouteObservation> rows;
    sources.reserve(Diagnostic::maxSources+1);rows.reserve(Diagnostic::maxRows+1);
    for(size_t i=0;i<Diagnostic::maxSources;++i){sources.push_back(source(train,occupied,signal+i));rows.push_back(row(train,1,occupied,signal+i));}
    CHECK(diagnostic.observe(session,sources,rows)==Diagnostic::Status::Accepted);
    CHECK(diagnostic.summary().sources==Diagnostic::maxSources&&diagnostic.summary().observedConflicts==Diagnostic::maxSources);
    CHECK(diagnostic.pending().sampleCount==Diagnostic::maxSamples);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==Diagnostic::maxSources);
    CHECK(diagnostic.pending().omittedSamples==Diagnostic::maxSources-Diagnostic::maxSamples);
    diagnostic.acknowledge();sources.push_back(source(train,occupied,signal+Diagnostic::maxSources));
    CHECK(diagnostic.observe(session,sources,rows)==Diagnostic::Status::SourceLimit);
    CHECK(diagnostic.summary().sources==0&&diagnostic.summary().observedConflicts==0);
    CHECK(diagnostic.pending().count(Event::ObservationLost)==Diagnostic::maxSources);
    sources.resize(1);rows.assign(Diagnostic::maxRows,row());
    diagnostic.acknowledge();CHECK(diagnostic.observe(session,sources,rows)==Diagnostic::Status::Accepted);
    rows.push_back(row());
    CHECK(diagnostic.observe(session,sources,rows)==Diagnostic::Status::RowLimit);
    CHECK(diagnostic.summary().sources==0);
    // Many distinct trains/revisions consume the same source slot. Capped
    // samples and cumulative counters survive a deliberately silent writer.
    diagnostic.acknowledge();
    for(uint64_t revision=1;revision<=10000;++revision)
        observe(diagnostic,source(train+revision),row(train+revision,revision));
    CHECK(diagnostic.summary().sources==1&&diagnostic.summary().observedConflicts==1);
    CHECK(diagnostic.sourceStorageCapacity()==capacity);
    CHECK(diagnostic.pending().sampleCount==Diagnostic::maxSamples);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==10000);
    CHECK(diagnostic.pending().count(Event::TrainReplaced)==9999);
    CHECK(diagnostic.pending().omittedSamples==19999-Diagnostic::maxSamples);
    diagnostic.acknowledge();
    for(uint64_t revision=10001;revision<=20000;++revision)
        observe(diagnostic,source(train),row(train,revision));
    CHECK(diagnostic.sourceStorageCapacity()==capacity);
    CHECK(diagnostic.pending().count(Event::TrainReplaced)==1);
    CHECK(diagnostic.pending().count(Event::RouteReplaced)==9999);
    CHECK(diagnostic.pending().count(Event::ConflictObserved)==10000);
}
}
int main(){try{
    transitionsSurviveWriterCadence();missingCauseNeverMeansClear();replacementsDoNotReuseEarlierFacts();
    recoveryComparesHistoryWithoutRefreshingIt();sessionSourceAndLifecycleChanges();
    unavailableAndInvalidInputsAreRestrictiveDiagnostics();boundsAndLongRunningMemory();
    std::cout<<"signal route diagnostic transitions, unknowns and bounded history: PASS\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
