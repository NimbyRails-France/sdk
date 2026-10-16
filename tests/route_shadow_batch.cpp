#include <engine/route_shadow_batch.h>
#include <iostream>
#include <stdexcept>
using namespace nimby::engine::interlocking;
namespace {
constexpr uint64_t train=uint64_t{5}<<48,source=uint64_t{8}<<48,next=(uint64_t{8}<<48)+1;
constexpr uint64_t track=uint64_t{1}<<48,crossing=(uint64_t{1}<<48)+1;
size_t checks{};
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error(#value);}while(false)
NimbySignalRouteObservation route(){
    NimbySignalRouteObservation value{};
    value.train_id=train;value.signal_id=source;value.next_signal_id=next;value.route_revision=2;value.observed_at_ms=100;
    value.head_track_id=value.source_track_id=value.next_track_id=track;
    value.head_fraction=.1;value.source_fraction=.2;value.next_fraction=.9;value.distance_m=10;
    value.head_direction=value.source_direction=value.next_direction=1;
    value.source_kind=value.next_kind=NIMBY_SIGNAL_PATH;
    value.blocks=1;
    value.flags=NIMBY_SIGNAL_ROUTE_ORDERED|NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE|NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR;
    return value;
}
void run(){
    auto row=route();NimbySignalRouteSection section{train,source,track,.2,.9};
    NimbyTrackConflict relation{track,crossing,.3,.4,.5,.6};
    ConflictList list{track,nimby::engine::TrackConflictReason::Complete,std::span(&relation,1)};
    auto build=[&]{return planShadowBatch(1,std::span(&row,1),std::span(&section,1),std::span(&list,1));};
    auto result=build();CHECK(result.routes.size()==1&&result.routes[0].request.routeQualified);
    CHECK(result.routes[0].request.resources==ResourceOutcome::Complete&&result.routes[0].resources.size()==2);
    CHECK(result.routes[0].request.nativeStage==NativeStage::Clear&&result.routes[0].request.entered==Proof::Unknown);
    ShadowCoordinator observer;CHECK(observer.beginSession(1));
    CHECK(observer.trySubmit(result.routes[0].request,result.routes[0].resources)==ShadowSubmit::Queued);
    const auto drained=observer.drain({1,100});CHECK(observer.leaseCount()==0);
    CHECK(drained.report&&drained.report->sampleCount==1&&
          drained.report->samples[0].result.outcome==Outcome::Unavailable&&
          drained.report->samples[0].entered==Proof::Unknown);
    // Copied route/F8 data alone cannot certify that a train has not entered.
    // Qualifying resources must not invent a prospective reservation proof.
    row.blocks=0;result=build();CHECK(!result.routes[0].request.routeQualified);
    row.blocks=17;result=build();CHECK(!result.routes[0].request.routeQualified);
    row=route();
    // Missing/conflicting tables may never be mistaken for an empty relation.
    list.reason=nimby::engine::TrackConflictReason::TrackUnavailable;result=build();
    CHECK(result.routes[0].request.resources==ResourceOutcome::UnknownTable&&result.routes[0].resources.empty());
    list.reason=nimby::engine::TrackConflictReason::Complete;
    row.flags&=~NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE;result=build();
    CHECK(!result.routes[0].request.routeQualified&&result.routes[0].resources.empty());
    row=route();row.flags|=NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;result=build();
    CHECK(!result.routes[0].request.routeQualified&&result.routes[0].request.nativeStage==NativeStage::Unknown);
    row=route();row.flags=(row.flags&~NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR)|NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;result=build();
    CHECK(result.routes[0].request.routeQualified&&result.routes[0].request.nativeStage==NativeStage::Blocked);
    for(const auto cause:{NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED,NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED}){
        row=route();row.flags=(row.flags&~NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR)|cause;result=build();
        CHECK(result.routes[0].request.routeQualified&&result.routes[0].request.nativeStage==NativeStage::Unknown);
        CHECK(result.routes[0].request.entered==Proof::Unknown&&observer.leaseCount()==0);
        row.flags|=NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;result=build();
        CHECK(result.routes[0].request.routeQualified&&result.routes[0].request.nativeStage==NativeStage::Blocked);
        row.flags|=NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR;result=build();CHECK(!result.routes[0].request.routeQualified);
    }
    row=route();row.flags=(row.flags&~NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR)|NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED;
    result=build();CHECK(!result.routes[0].request.routeQualified);
    row=route();section.end_fraction=.1;result=build();CHECK(!result.routes[0].request.routeQualified);
    section={train,source,track,.2,.9};
    const NimbySignalRouteObservation duplicate[]{row,row};
    result=planShadowBatch(1,duplicate,std::span(&section,1),std::span(&list,1));
    CHECK(result.routes.size()==1&&!result.routes[0].request.routeQualified);
    section.signal_id=next;result=build();CHECK(result.routes[0].resources.empty());
    row=route();row.source_direction=0;result=build();CHECK(!result.routes[0].request.routeQualified);
    row=route();row.train_id=1;result=build();CHECK(result.routes.empty()&&result.invalid==1);
    row=route();row.route_revision=0;result=build();CHECK(result.routes.empty()&&result.invalid==1);
    std::vector<NimbySignalRouteObservation> tooMany(65,route());
    result=planShadowBatch(1,tooMany,{},{});CHECK(result.routes.empty()&&result.deferred==65);
    result=planShadowBatch(0,std::span(&row,1),{},{});CHECK(result.routes.empty());
    // Empty/failed observations never manufacture release evidence.
    result=planShadowBatch(1,{},{},{});CHECK(result.routes.empty()&&observer.leaseCount()==0);
}
}
int main(){try{run();std::cout<<"PASS: "<<checks<<" copied route/shadow batch checks; no native permission or rear-clear inference\n";}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
