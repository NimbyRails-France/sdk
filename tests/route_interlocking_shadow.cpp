#include <engine/route_interlocking_shadow.h>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

using namespace nimby::engine;
using namespace nimby::engine::interlocking;
static unsigned checks=0;
#define CHECK(x) do { ++checks; if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x); } while(false)
constexpr uint64_t track(unsigned ordinal,unsigned generation=1){return (1ULL<<48)|(uint64_t(ordinal)<<16)|generation;}
constexpr uint64_t train(unsigned ordinal,unsigned generation=1){return (5ULL<<48)|(uint64_t(ordinal)<<16)|generation;}
constexpr uint64_t signal(unsigned ordinal,unsigned generation=1){return (8ULL<<48)|(uint64_t(ordinal)<<16)|generation;}
Ticket ticket(unsigned t,unsigned s,uint64_t revision=1,uint64_t session=7){return {session,train(t),signal(s),revision};}
ShadowRequest request(Ticket id,uint64_t at=100,NativeStage stage=NativeStage::Clear){
    return {id,at,true,Proof::No,stage,ResourceOutcome::Complete};
}
ShadowEvidence evidence(Ticket id,uint64_t at,Proof entered,Proof rear=Proof::Unknown,
                        std::optional<uint64_t> revision=1){return {id,{at,revision,entered,rear},NativeStage::Unknown};}
namespace nimby::engine::interlocking {
struct ShadowCoordinatorTestAccess {
    static void lock(ShadowCoordinator& coordinator){coordinator.queueLock_.test_and_set(std::memory_order_acquire);}
    static void unlock(ShadowCoordinator& coordinator){coordinator.queueLock_.clear(std::memory_order_release);}
};
}
static ShadowDrain process(ShadowCoordinator& coordinator,uint64_t at=100){return coordinator.drain({7,at});}

static void qualificationAndOwnedMailbox(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2),c=ticket(3,3);
    const NimbyTrackConflict cross{track(1),track(2),.2,.8,.1,.9};
    const ConflictList list{track(1),TrackConflictReason::Complete,std::span(&cross,1)};
    const RoutePortion portion{track(1),.3,.6};
    auto resources=buildRouteResources(std::span(&portion,1),std::span(&list,1));
    CHECK(resources.complete()&&resources.resources.size()==2);
    CHECK(coordinator.trySubmit(request(a),resources.resources)==ShadowSubmit::Queued);
    resources.resources.assign(1,Interval{track(99),0,1}); // Producer storage is never borrowed.
    const Interval crossing{track(2),.5,.6},independent{track(3),0,1};
    CHECK(coordinator.trySubmit(request(b),std::span(&crossing,1))==ShadowSubmit::Queued);
    CHECK(coordinator.trySubmit(request(c,100,NativeStage::Blocked),std::span(&independent,1))==ShadowSubmit::Queued);
    const auto drained=process(coordinator);CHECK(drained.processed==3&&drained.queued==0&&drained.report);
    CHECK(drained.report->outcomes[size_t(Outcome::Granted)]==2&&drained.report->outcomes[size_t(Outcome::Conflict)]==1);
    CHECK(drained.report->comparisons[size_t(StageComparison::SameStage)]==1);
    CHECK(drained.report->comparisons[size_t(StageComparison::ShadowRefusedNativeClear)]==1);
    CHECK(drained.report->comparisons[size_t(StageComparison::ShadowGrantedNativeBlocked)]==1);
    CHECK(coordinator.holds(a)&&!coordinator.holds(b)&&coordinator.holds(c));
    CHECK(drained.report->samples[1].result.blocker==a);
    CHECK(drained.report->samples[1].result.track==track(2));
    // A source's unavailable F8 data is not an empty complete conflict list.
    const auto unavailable=buildRouteResources(std::span(&portion,1),{});
    CHECK(unavailable.outcome==ResourceOutcome::MissingTable);
    auto incomplete=request(ticket(4,4));incomplete.resources=unavailable.outcome;
    CHECK(coordinator.trySubmit(incomplete,{})==ShadowSubmit::Queued);
    CHECK(process(coordinator,1100).report->outcomes[size_t(Outcome::Unavailable)]==1);
    CHECK(!coordinator.holds(incomplete.ticket));
}
static void incompleteUnknownAndRearRetention(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2);const Interval interval{track(1),.1,.9};
    auto unknown=request(a);unknown.entered=Proof::Unknown;
    CHECK(coordinator.trySubmit(unknown,std::span(&interval,1))==ShadowSubmit::Queued);
    auto initial=process(coordinator);CHECK(initial.report->outcomes[size_t(Outcome::Unavailable)]==1&&!coordinator.holds(a));
    CHECK(initial.report->samples[0].resources==ResourceOutcome::Complete&&initial.report->samples[0].entered==Proof::Unknown);
    auto incomplete=request(a,101);incomplete.routeQualified=false;
    CHECK(coordinator.trySubmit(incomplete,std::span(&interval,1))==ShadowSubmit::Queued);
    process(coordinator,101);CHECK(!coordinator.holds(a));
    CHECK(coordinator.trySubmit(request(a,102),std::span(&interval,1))==ShadowSubmit::Queued);
    process(coordinator,102);CHECK(coordinator.holds(a));
    CHECK(coordinator.trySubmit(evidence(a,103,Proof::No,Proof::Yes))==ShadowSubmit::Queued);
    process(coordinator,103);CHECK(coordinator.holds(a)); // Empty before entry is not a rear release.
    CHECK(coordinator.trySubmit(evidence(a,103,Proof::Yes))==ShadowSubmit::Queued);
    process(coordinator,103);CHECK(coordinator.holds(a));
    auto lost=request(a,104);lost.routeQualified=false;lost.entered=Proof::Unknown;
    lost.resources=ResourceOutcome::UnknownTable;
    CHECK(coordinator.trySubmit(lost,{})==ShadowSubmit::Queued);
    CHECK(coordinator.trySubmit(request(b,104),std::span(&interval,1))==ShadowSubmit::Queued);
    process(coordinator,104);CHECK(coordinator.holds(a)&&!coordinator.holds(b));
    // No timer/empty route/absence clears an entered or unqualified lease.
    const auto stale=process(coordinator,31000);CHECK(stale.report&&stale.report->leases==1&&coordinator.holds(a));
    CHECK(coordinator.trySubmit(evidence(a,105,Proof::Unknown,Proof::Yes))==ShadowSubmit::Queued);
    process(coordinator,31001);CHECK(coordinator.holds(a)); // Expired rear proof is rejected.
    CHECK(coordinator.trySubmit(evidence(a,31002,Proof::Unknown,Proof::Unknown,std::nullopt))==ShadowSubmit::Queued);
    process(coordinator,31002);CHECK(coordinator.holds(a));
    CHECK(coordinator.trySubmit(evidence(a,31003,Proof::Unknown,Proof::Yes,std::nullopt))==ShadowSubmit::Queued);
    process(coordinator,31003);CHECK(!coordinator.holds(a));
    CHECK(coordinator.trySubmit(request(b,31004),std::span(&interval,1))==ShadowSubmit::Queued);
    process(coordinator,31004);CHECK(coordinator.holds(b));
}
static void rerouteAndWorldLineage(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    const auto old=ticket(1,1),next=ticket(1,1,2),b=ticket(2,2);
    const Interval original{track(1),0,1},different{track(2),0,1};
    CHECK(coordinator.trySubmit(request(old),std::span(&original,1))==ShadowSubmit::Queued);
    process(coordinator);CHECK(coordinator.holds(old));
    CHECK(coordinator.trySubmit(request(next,101),std::span(&different,1))==ShadowSubmit::Queued);
    process(coordinator,101);CHECK(!coordinator.holds(old)&&coordinator.holds(next));
    CHECK(coordinator.trySubmit(request(b,102),std::span(&original,1))==ShadowSubmit::Queued);
    process(coordinator,102);CHECK(coordinator.holds(b));
    CHECK(coordinator.trySubmit(evidence(next,103,Proof::Yes,Proof::Unknown,2))==ShadowSubmit::Queued);
    process(coordinator,103);
    CHECK(coordinator.trySubmit(request(ticket(1,1,3),104),std::span(&original,1))==ShadowSubmit::Queued);
    process(coordinator,104);CHECK(coordinator.holds(next)&&coordinator.holds(b));
    CHECK(!coordinator.beginSession(0)&&coordinator.holds(next));
    CHECK(coordinator.beginSession(7)&&coordinator.holds(next));
    // Requesting a different world is not a world transition.
    CHECK(coordinator.trySubmit(request(ticket(3,3,1,8),105),std::span(&different,1))==ShadowSubmit::Queued);
    process(coordinator,105);CHECK(coordinator.leaseCount()==2);
    // A positively qualified world change alone permits unconditional cleanup.
    CHECK(coordinator.beginSession(8)&&coordinator.leaseCount()==0);
    CHECK(coordinator.trySubmit(request(b,106),std::span(&original,1))==ShadowSubmit::Queued);
    const auto changed=coordinator.drain({8,106});
    CHECK(changed.report&&changed.report->outcomes[size_t(Outcome::SessionMismatch)]==1&&coordinator.leaseCount()==0);
}
static void queueLimitsAndProducerContention(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    const Interval interval{track(1),0,1};const auto a=ticket(1,1);
    CHECK(coordinator.trySubmit(request(a),std::span(&interval,1))==ShadowSubmit::Queued);
    process(coordinator);CHECK(coordinator.holds(a));
    ShadowCoordinatorTestAccess::lock(coordinator);
    CHECK(coordinator.trySubmit(request(ticket(2,2)),std::span(&interval,1))==ShadowSubmit::Busy);
    CHECK(process(coordinator,101).processed==0); // The reader never waits for a producer either.
    ShadowCoordinatorTestAccess::unlock(coordinator);
    for(size_t i=0;i<ShadowCoordinator::queueCapacity;++i)
        CHECK(coordinator.trySubmit(evidence(a,102,Proof::Unknown))==ShadowSubmit::Queued);
    CHECK(coordinator.trySubmit(evidence(a,102,Proof::Unknown,Proof::Yes))==ShadowSubmit::Full);
    CHECK(coordinator.holds(a));
    const auto limited=coordinator.drain({7,102},1);CHECK(limited.processed==1&&limited.queued==63&&coordinator.holds(a));
    const auto rest=process(coordinator,1100);CHECK(rest.processed==63&&rest.report&&rest.report->busy==1&&rest.report->full==1);
    CHECK(coordinator.holds(a)&&coordinator.queueStats().queued==0);
    auto invalid=request(a);invalid.ticket.source=train(1);
    CHECK(coordinator.trySubmit(invalid,{})==ShadowSubmit::Invalid);
    const std::vector<Interval> oversized(ShadowCoordinator::maximumResources+1,interval);
    CHECK(coordinator.trySubmit(request(a),oversized)==ShadowSubmit::Invalid);
    CHECK(coordinator.queueStats().invalid==2&&coordinator.holds(a));
    auto bad=evidence(a,1101,Proof::Unknown);bad.evidence.currentRouteRevision=0;
    CHECK(coordinator.trySubmit(bad)==ShadowSubmit::Invalid);
}
static void boundedWorkerFailuresAndClock(){
    Limits limits;limits.leases=1;ShadowCoordinator coordinator(limits);CHECK(coordinator.beginSession(7));
    const Interval one{track(1),0,1},two{track(2),0,1};
    CHECK(coordinator.trySubmit(request(ticket(1,1)),std::span(&one,1))==ShadowSubmit::Queued);
    CHECK(coordinator.trySubmit(request(ticket(2,2)),std::span(&two,1))==ShadowSubmit::Queued);
    const auto first=process(coordinator);CHECK(first.report->outcomes[size_t(Outcome::Capacity)]==1);
    CHECK(first.report->comparisons[size_t(StageComparison::Unavailable)]==1);
    const Interval malformed{track(1),std::numeric_limits<double>::quiet_NaN(),1};
    CHECK(coordinator.trySubmit(request(ticket(1,1),101),std::span(&malformed,1))==ShadowSubmit::Queued);
    CHECK(!process(coordinator,101).report);CHECK(coordinator.holds(ticket(1,1)));
    CHECK(coordinator.trySubmit(request(ticket(3,3),102),std::span(&two,1))==ShadowSubmit::Queued);
    CHECK(!process(coordinator,99).report);CHECK(coordinator.leaseCount()==1);
    const auto report=process(coordinator,1100);CHECK(report.report);
    CHECK(report.report->outcomes[size_t(Outcome::InvalidRequest)]==1&&report.report->outcomes[size_t(Outcome::Stale)]==1);
}
static void diagnosticThrottleAndHeartbeat(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    const auto a=ticket(1,1);const Interval interval{track(1),0,1};
    CHECK(coordinator.trySubmit(request(a),std::span(&interval,1))==ShadowSubmit::Queued);
    const auto first=process(coordinator);CHECK(first.report&&first.report->sampleCount==1);
    CHECK(coordinator.trySubmit(request(a,101),std::span(&interval,1))==ShadowSubmit::Queued);
    CHECK(!process(coordinator,101).report);
    CHECK(!process(coordinator,1100).report); // Same stage/outcome has no transition.
    CHECK(process(coordinator,30100).report); // Idle heartbeat retains protected lease.
    CHECK(coordinator.trySubmit(request(a,30101,NativeStage::Blocked),std::span(&interval,1))==ShadowSubmit::Queued);
    CHECK(!process(coordinator,30101).report);
    const auto changed=process(coordinator,31100);CHECK(changed.report&&changed.report->sampleCount==1);
    CHECK(changed.report->comparisons[size_t(StageComparison::ShadowGrantedNativeBlocked)]==1);
    CHECK(!process(coordinator,31099).report); // Clock regression cannot cause a log burst.
}
static void manyProducersOneOwner(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    std::array<std::thread,4> producers;std::atomic<unsigned> invalid{};
    for(unsigned p=0;p<producers.size();++p)producers[p]=std::thread([&,p]{
        for(unsigned i=0;i<100;++i){const auto id=p*100+i+1;const Interval interval{track(id),0,1};
            if(coordinator.trySubmit(request(ticket(id,id)),std::span(&interval,1))==ShadowSubmit::Invalid)++invalid;
        }
    });
    for(auto& producer:producers)producer.join();
    const auto stats=coordinator.queueStats();CHECK(!invalid&&stats.accepted>0&&stats.accepted<=64&&stats.queued==stats.accepted);
    CHECK(stats.accepted+stats.busy+stats.full==400);
    const auto drained=process(coordinator);CHECK(drained.processed==stats.accepted&&drained.report&&drained.report->sampleCount==std::min<uint64_t>(4,stats.accepted));
    CHECK(drained.report->outcomes[size_t(Outcome::Granted)]==stats.accepted&&coordinator.leaseCount()==stats.accepted);
    CHECK(drained.report->busy==stats.busy&&drained.report->full==stats.full);
}
static void refusalsRemainVisibleInLargeBatch(){
    ShadowCoordinator coordinator;CHECK(coordinator.beginSession(7));
    for(unsigned i=1;i<=8;++i){const Interval interval{track(i),0,1};
        CHECK(coordinator.trySubmit(request(ticket(i,i)),std::span(&interval,1))==ShadowSubmit::Queued);
    }
    const Interval conflict{track(1),.4,.6};
    CHECK(coordinator.trySubmit(request(ticket(9,9)),std::span(&conflict,1))==ShadowSubmit::Queued);
    const auto report=process(coordinator).report;CHECK(report&&report->sampleCount==4);
    CHECK(report->outcomes[size_t(Outcome::Granted)]==8&&report->outcomes[size_t(Outcome::Conflict)]==1);
    CHECK(std::any_of(report->samples.begin(),report->samples.end(),[](const auto& sample){
        return sample.ticket==ticket(9,9)&&sample.result.outcome==Outcome::Conflict&&sample.result.blocker==ticket(1,1);
    }));
}
int main(){
    qualificationAndOwnedMailbox();incompleteUnknownAndRearRetention();rerouteAndWorldLineage();
    queueLimitsAndProducerContention();boundedWorkerFailuresAndClock();diagnosticThrottleAndHeartbeat();manyProducersOneOwner();
    refusalsRemainVisibleInLargeBatch();
    std::cout<<"PASS: "<<checks<<" bounded shadow coordinator checks; owned resources, single worker, no native admission writes\n";
}
