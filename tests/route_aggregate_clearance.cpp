#include <engine/route_aggregate_clearance.h>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>

using namespace nimby::engine::interlocking;
namespace rear=nimby::engine::native_rear;
namespace {
std::atomic<bool> watchAllocations{};
std::atomic<size_t> allocations{};
size_t checks{};
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);} while(false)
constexpr uint64_t id(unsigned type,uint64_t ordinal=1) {return (uint64_t(type)<<48)|(ordinal<<16)|1;}
constexpr uint64_t train=id(5),source=id(8),a=id(1),b=id(1,2),c=id(1,3);
constexpr rear::Publication publication{7,3,0,rear::Phase::RetainedMovement};

// Synthetic, caller-qualified publications test the pure contract only. They
// do not implement or claim a native producer, certificate or game validation.
struct Fixture {
    std::array<rear::HistorySection,3> history{{
        {a,10,.875,.125,1000,-1,0,rear::HistoryEnd::Continues,false,0,0,0},
        {b,11,.25,1,400,1,1,rear::HistoryEnd::Continues,true,a,.125,0},
        {c,12,.8,.2,1000,-1,2,rear::HistoryEnd::QualifiedEnd,true,b,1,1}
    }};
    std::array<Interval,1> own{{{c,.1,.2}}},aliases{{{b,.3,.9}}};
    AggregateClearanceRoute route{{7,train,source,9},4,Proof::Yes,true,publication,own,aliases};
    rear::Observation observation{{train,4,850,850,publication,true,true,true},
        {train,4,publication,true,true,history},{train,a,0,.625,-1,0,publication,true},true};
    std::array<rear::Portion,8> body{};
    AggregateClearanceResult assess(AggregateClearanceLimits limits={}) {
        return assessAggregateRearClearance(route,observation,publication,body,limits);
    }
};
void completeIsNotClear() {
    Fixture clear;auto result=clear.assess();
    CHECK(result.proof==Proof::Yes&&result.reason==AggregateClearanceReason::Clear);
    CHECK(result.coverageAssessed&&result.coverage.complete()&&result.coverage.portions==3);
    CHECK(result.coverage.train==train&&result.coverage.consistRevision==4&&result.coverage.at==publication);
    // The train's head is on A, but its last 50 m on C still retain C's zone.
    Fixture trailing;trailing.own[0]={c,.76,.77};result=trailing.assess();
    CHECK(result.coverage.complete()&&result.proof==Proof::No);
    CHECK(result.reason==AggregateClearanceReason::Occupied&&result.track==c);
    for(const auto entered:{Proof::No,Proof::Unknown}) {
        Fixture f;f.route.entered=entered;result=f.assess();
        CHECK(result.coverage.complete()&&result.proof==Proof::Unknown&&result.reason==AggregateClearanceReason::NotEntered);
        f.own[0]={a,.1,.6};CHECK(f.assess().proof==Proof::No);
    }
    // Opposite fraction direction does not change physical overlap. Contact
    // with a front, tail or zero-length endpoint remains a restrictive No.
    for(unsigned point=0;point<3;++point) {
        Fixture f;const auto body=f.assess();CHECK(body.coverage.complete());
        const double tail=f.body[2].to;
        if(point==0)f.own[0]={c,.7,tail};
        if(point==1)f.own[0]={a,.625,.7};
        if(point==2)f.own[0]={c,tail,tail};
        CHECK(f.assess().proof==Proof::No);
    }
    Fixture adjacent;CHECK(adjacent.assess().coverage.complete());
    adjacent.own[0]={c,.2,std::nextafter(adjacent.body[2].to,0.)};
    CHECK(adjacent.assess().proof==Proof::Yes); // Only strict separation clears.
    Fixture exact;exact.observation.consist.lengthMeters=exact.observation.consist.nativeLengthMeters=500;
    exact.own[0]={b,.25,.25};result=exact.assess();
    CHECK(result.coverage.complete()&&result.coverage.portions==2&&result.proof==Proof::No);
    CHECK(exact.body[1].lengthMeters==0); // Adjacent track rear contact is real.
    Fixture rounded;rounded.observation.consist.nativeLengthMeters=std::nextafter(850.f,1000.f);
    rounded.own[0]={c,.75,.75};result=rounded.assess();
    CHECK(result.coverage.complete()&&result.coverage.requestedMeters>850&&result.proof==Proof::No);
    Fixture generations;generations.own[0]={c+1,.75,.8};CHECK(generations.assess().proof==Proof::Yes);
}
void aliasesAndResources() {
    Fixture f;CHECK(f.assess().proof==Proof::Yes); // B alias overlaps B body; physical C does not.
    f.route.aliases={};CHECK(f.assess().proof==Proof::Yes);
    std::array<Interval,5> unsorted{{{c,.3,.5},{a,.8,.9},{c,.1,.4},{c,.5,.7},{a,.75,.8}}};
    f.route.ownPhysical=unsorted;CHECK(f.assess().proof==Proof::Yes);
    unsorted[0]={c,.78,.79};CHECK(f.assess().proof==Proof::No);
    // Later malformed aliases/resources must not be hidden by an early overlap.
    for(unsigned location=0;location<2;++location) {
        Fixture invalid;invalid.own[0]={a,.1,.6};
        std::array<Interval,2> rows{{{a,.1,.6},{c,.3,std::numeric_limits<double>::quiet_NaN()}}};
        if(location==0)invalid.route.ownPhysical=rows;else invalid.route.aliases=rows;
        const auto result=invalid.assess();CHECK(result.proof==Proof::Unknown&&result.reason==AggregateClearanceReason::InvalidInterval);
        CHECK(!result.coverageAssessed);
    }
    for(uint64_t wrong:{uint64_t{0},uint64_t{1}<<48,train}) {
        Fixture invalid;invalid.own[0].track=wrong;CHECK(invalid.assess().reason==AggregateClearanceReason::InvalidInterval);
        Fixture alias;alias.aliases[0].track=wrong;CHECK(alias.assess().reason==AggregateClearanceReason::InvalidInterval);
    }
    for(double wrong:{-.1,1.1,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}) {
        Fixture invalid;invalid.own[0].begin=wrong;CHECK(invalid.assess().reason==AggregateClearanceReason::InvalidInterval);
        Fixture alias;alias.aliases[0].end=wrong;CHECK(alias.assess().reason==AggregateClearanceReason::InvalidInterval);
    }
    Fixture reverse;reverse.own[0]={c,.8,.2};CHECK(reverse.assess().reason==AggregateClearanceReason::InvalidInterval);
    Fixture missing;missing.route.ownPhysical={};CHECK(missing.assess().reason==AggregateClearanceReason::UnqualifiedRoute);
    Fixture unqualified;unqualified.route.qualified=false;CHECK(unqualified.assess().reason==AggregateClearanceReason::UnqualifiedRoute);
}
void publicationAndIdentity() {
    for(unsigned change=0;change<5;++change) {
        Fixture f;auto current=publication;
        if(change==0)current.session=0;
        if(change==1)current.epoch=0;
        if(change==2)current.phase=rear::Phase::Unknown;
        if(change==3)current.phase=static_cast<rear::Phase>(99);
        if(change==4)current.phase=rear::Phase::BeforeMovement;
        const auto result=assessAggregateRearClearance(f.route,f.observation,current,f.body);
        CHECK(result.proof==Proof::Unknown);
        CHECK(result.reason==(change==4?AggregateClearanceReason::UnretainedPublication:AggregateClearanceReason::InvalidPublication));
    }
    for(unsigned change=0;change<4;++change) {
        Fixture f;auto& stamp=f.route.qualifiedAt;
        if(change==0)++stamp.session;
        if(change==1)++stamp.epoch;
        if(change==2)++stamp.tick;
        if(change==3)stamp.phase=rear::Phase::BeforeMovement;
        CHECK(f.assess().reason==AggregateClearanceReason::DifferentPublication);
    }
    for(unsigned change=0;change<9;++change) {
        Fixture f;
        if(change==0)++f.route.ticket.session;
        if(change==1)f.route.ticket.routeRevision=0;
        if(change==2)f.route.ticket.train=a;
        if(change==3)f.route.ticket.train=uint64_t{5}<<48;
        if(change==4)f.route.ticket.source=train;
        if(change==5)f.route.ticket.source=uint64_t{8}<<48;
        if(change==6)f.route.entered=static_cast<Proof>(99);
        if(change==7)f.route.consistRevision=0;
        if(change==8)++f.route.consistRevision;
        const auto result=f.assess();CHECK(result.proof==Proof::Unknown);
        CHECK(result.reason==(change==7?AggregateClearanceReason::InvalidConsistRevision:
            change==8?AggregateClearanceReason::ConsistChanged:AggregateClearanceReason::InvalidTicket));
    }
    Fixture recycled;recycled.observation.consist.train++;
    CHECK(recycled.assess().reason==AggregateClearanceReason::TrainIdentityChanged);
    Fixture differentHistory;differentHistory.observation.history.train++;
    const auto different=differentHistory.assess();CHECK(different.proof==Proof::Unknown);
    CHECK(different.coverageAssessed&&different.coverage.reason==rear::Reason::TrainIdentityChanged);
    // Tick zero is valid, but session, epoch, phase and every publication stamp
    // must be qualified; a coincident monotonic timestamp is never used here.
    Fixture zero;CHECK(zero.assess().proof==Proof::Yes&&publication.tick==0);
    Fixture stale;stale.observation.cursor.at.tick++;
    CHECK(stale.assess().coverage.reason==rear::Reason::DifferentPublication);
}
void incompleteCoverageAndBudgets() {
    for(unsigned missing=0;missing<7;++missing) {
        Fixture f;
        if(missing==0)f.observation.stablePublicationQualified=false;
        if(missing==1)f.observation.consist.complete=false;
        if(missing==2)f.observation.history.qualified=false;
        if(missing==3)f.observation.history.continuityQualified=false;
        if(missing==4)f.history[1].joinQualified=false;
        if(missing==5)f.observation.history.sections=std::span(f.history).first(2);
        if(missing==6)f.observation.consist.nativeLengthQualified=false;
        const auto result=f.assess();CHECK(result.proof==Proof::Unknown&&result.reason==AggregateClearanceReason::UnqualifiedCoverage);
        constexpr std::array reasons{rear::Reason::MissingStablePublication,rear::Reason::IncompleteConsist,
            rear::Reason::UnqualifiedHistory,rear::Reason::UnqualifiedContinuity,rear::Reason::MissingTransition,
            rear::Reason::HistoryTooShort,rear::Reason::UnqualifiedNativeLength};
        CHECK(result.coverageAssessed&&!result.coverage.complete()&&result.coverage.reason==reasons[missing]);
        // Repeating identical external data cannot invent the missing evidence.
        CHECK(f.assess().proof==Proof::Unknown);
    }
    Fixture gap;gap.history[1]={a,11,.125001,0,1000,-1,0,rear::HistoryEnd::Continues,true,a,.125,0};
    CHECK(gap.assess().coverage.reason==rear::Reason::HistoryGap);
    Fixture capacity;const auto small=assessAggregateRearClearance(capacity.route,capacity.observation,publication,
        std::span(capacity.body).first(2));CHECK(small.proof==Proof::Unknown&&small.coverage.reason==rear::Reason::OutputCapacity);
    Fixture f;AggregateClearanceLimits limits;limits.overlapChecks=1;
    const auto bounded=f.assess(limits);CHECK(bounded.proof==Proof::Unknown&&bounded.reason==AggregateClearanceReason::BudgetExceeded);
    CHECK(bounded.coverage.complete()&&bounded.checks==1); // Complete still does not certify clear.
    const auto measured=f.assess();limits.overlapChecks=measured.checks;CHECK(f.assess(limits).proof==Proof::Yes);
    limits.overlapChecks=0;CHECK(f.assess(limits).reason==AggregateClearanceReason::InvalidLimits);
    limits={};limits.coverage.work=1;CHECK(f.assess(limits).coverage.reason==rear::Reason::BudgetExceeded);
    for(unsigned field=0;field<3;++field) {
        limits={};if(field==0)limits.ownPortions=513;if(field==1)limits.aliases=513;if(field==2)limits.overlapChecks=4194305;
        CHECK(f.assess(limits).reason==AggregateClearanceReason::InvalidLimits);
    }
    std::array<Interval,513> many{};for(auto& row:many)row={c,.1,.2};
    f.route.ownPhysical=many;CHECK(f.assess().reason==AggregateClearanceReason::BudgetExceeded);
    f.route.ownPhysical=std::span(many).first(512);CHECK(f.assess().proof==Proof::Yes);
    f.route.aliases=many;CHECK(f.assess().reason==AggregateClearanceReason::BudgetExceeded);
    f.route.aliases=std::span(many).first(512);CHECK(f.assess().proof==Proof::Yes);
    // Allocation checks cover qualification, normalization and overlap search,
    // including populated resources at the allowed maximum (not the lease map).
    allocations=0;watchAllocations=true;bool correct=true;
    for(unsigned iteration=0;iteration<1000;++iteration)correct&=f.assess().proof==Proof::Yes;
    watchAllocations=false;CHECK(correct&&allocations.load()==0);
}
void roundedDistanceCannotRelease() {
    Fixture shortTrace;
    shortTrace.history[0]={a,10,.8,.2,1000,-1,0,rear::HistoryEnd::QualifiedEnd};
    shortTrace.observation.history.sections=std::span(shortTrace.history).first(1);
    shortTrace.observation.cursor.fraction=.8;
    shortTrace.observation.consist.lengthMeters=(.8-.2)*1000;
    shortTrace.observation.consist.nativeLengthMeters=600;
    auto result=shortTrace.assess();
    CHECK(result.proof==Proof::Unknown&&result.coverage.reason==rear::Reason::HistoryTooShort);
    CHECK(result.coverage.remainingMeters>0);
    RouteInterlocking locks;CHECK(locks.beginSession(publication.session));
    const std::array<Interval,2> resources{shortTrace.own[0],shortTrace.aliases[0]};
    CHECK(locks.acquire({shortTrace.route.ticket,resources,100,true,Proof::Yes},{7,100}).granted());
    CHECK(locks.observe(shortTrace.route.ticket,{100,9,Proof::Yes,result.proof},{7,100}).outcome==Outcome::Updated);
    CHECK(locks.holds(shortTrace.route.ticket));
    // A following section receives every positive uncertain remainder. The
    // former rounded-equality logic produced only B's zero-length endpoint,
    // which could have released a zone just beyond that point prematurely.
    shortTrace.history[0].end=rear::HistoryEnd::Continues;
    shortTrace.history[1]={b,11,0,1,1,1,0,rear::HistoryEnd::QualifiedEnd,true,a,.2,0};
    shortTrace.observation.history.sections=std::span(shortTrace.history).first(2);
    shortTrace.own[0]={b,1e-14,1e-12};result=shortTrace.assess();
    CHECK(result.coverage.complete()&&result.proof==Proof::No&&result.track==b);
    CHECK(shortTrace.body[1].lengthMeters>0);
    shortTrace.history[1].metricMeters=1000;
    shortTrace.observation.consist.lengthMeters=650;shortTrace.observation.consist.nativeLengthMeters=650;
    shortTrace.own[0]={b,.04999999999999995,.04999999999999995};result=shortTrace.assess();
    CHECK(result.coverage.complete()&&result.proof==Proof::No&&result.track==b);
    CHECK(shortTrace.body[1].to>=.05);
}
struct LongFixture {
    rear::Publication current=publication;
    std::array<rear::HistorySection,129> history{};
    std::array<rear::Portion,129> body{};
    // The original physical zone is touched only by the closed tail endpoint.
    // B is a conflict alias, deliberately still inside the later body envelope.
    std::array<Interval,1> own{{{id(1,86),.75,.75}}},aliases{{{b,.3,.7}}};
    AggregateClearanceRoute route{{7,train,source,9},4,Proof::No,true,current,own,aliases};
    rear::Observation observation;
    LongFixture() {
        for(size_t i=0;i<history.size();++i) {
            const bool forward=i%2==0;
            history[i]={id(1,i+1),i+1,forward?.25:.75,forward?.75:.25,20,
                int8_t(forward?1:-1),uint8_t(i%3),rear::HistoryEnd::Continues,false,0,0,0};
        }
        qualifyHistory();
        observation={{train,4,850,850,current,true,true,true},
            {train,4,current,true,true,history},{train,a,0,.25,1,0,current,true},true};
    }
    void qualifyHistory() {
        for(size_t i=0;i<history.size();++i) {
            auto& row=history[i];row.sequence=i+1;
            row.end=i+1==history.size()?rear::HistoryEnd::QualifiedEnd:rear::HistoryEnd::Continues;
            row.joinQualified=i!=0;row.previousTrack=i?history[i-1].track:0;
            row.previousFraction=i?history[i-1].to:0;row.previousOrientation=i?history[i-1].orientation:0;
        }
    }
    void advanceHeadOneSection() {
        // A synthetic provider publishes the actual retained history with one
        // new head section, preserving the older branches and full identities.
        for(size_t i=history.size()-1;i;--i)history[i]=history[i-1];
        history[0]={id(1,999),1,.25,.75,20,1,0,rear::HistoryEnd::Continues,false,0,0,0};
        qualifyHistory();++current.tick;
        route.qualifiedAt=observation.consist.at=observation.history.at=observation.cursor.at=current;
        observation.cursor.track=history[0].track;observation.cursor.fraction=.25;
        observation.cursor.rearDirection=1;observation.cursor.orientation=0;
    }
    AggregateClearanceResult assess() {return assessAggregateRearClearance(route,observation,current,body);}
};
void retainedLeaseScenario() {
    LongFixture f;RouteInterlocking locks;CHECK(locks.beginSession(publication.session));
    const std::array<Interval,2> claimed{f.own[0],f.aliases[0]};
    const Ticket other{7,id(5,2),id(8,2),1};const std::array<Interval,1> conflicting{{{b,.4,.5}}};
    uint64_t now=100;
    const auto attemptOther=[&] {return locks.acquire({other,conflicting,now,true,Proof::No},{7,now});};
    CHECK(locks.acquire({f.route.ticket,claimed,now,true,Proof::No},{7,now}).granted());
    auto blocked=attemptOther();CHECK(blocked.outcome==Outcome::Conflict&&blocked.blocker==f.route.ticket);
    auto result=f.assess();CHECK(result.coverage.complete()&&result.coverage.portions==86&&result.proof==Proof::No);
    CHECK(f.body[84].lengthMeters==10&&f.body[85].lengthMeters==0&&f.body[85].track==id(1,86));
    // Entry is its own qualified fact; a complete body or a granted route does
    // not manufacture it. Once entered, the original zone cannot be replaced.
    f.route.entered=Proof::Yes;++now;
    CHECK(locks.observe(f.route.ticket,{now,9,Proof::Yes,result.proof},{7,now}).outcome==Outcome::Updated);
    auto replanned=f.route.ticket;++replanned.routeRevision;
    const std::array<Interval,1> newResources{{{id(1,400),0,1}}};++now;
    CHECK(locks.acquire({replanned,newResources,now,true,Proof::Yes},{7,now}).outcome==Outcome::RetainedAfterEntry);
    CHECK(locks.holds(f.route.ticket)&&!locks.holds(replanned)&&locks.resourceCount()==2);
    CHECK(attemptOther().outcome==Outcome::Conflict);
    ++now;result=f.assess();CHECK(result.proof==Proof::No);
    CHECK(locks.observe(f.route.ticket,{now,replanned.routeRevision,Proof::Yes,result.proof},{7,now}).outcome==Outcome::RetainedAfterEntry);
    CHECK(locks.holds(f.route.ticket)&&attemptOther().outcome==Outcome::Conflict);
    // A missing mod, provider or history continuity cannot clear the old lease.
    f.observation.history.qualified=false;++now;result=f.assess();CHECK(result.proof==Proof::Unknown);
    CHECK(locks.observe(f.route.ticket,{now,std::nullopt,Proof::Unknown,result.proof},{7,now}).outcome==Outcome::Unavailable);
    CHECK(locks.holds(f.route.ticket)&&attemptOther().outcome==Outcome::Conflict);
    f.observation.history.qualified=true;f.observation.stablePublicationQualified=false;++now;
    result=f.assess();CHECK(result.coverage.reason==rear::Reason::MissingStablePublication);
    CHECK(locks.observe(f.route.ticket,{now,replanned.routeRevision,Proof::Unknown,result.proof},{7,now}).outcome==Outcome::RetainedAfterEntry);
    CHECK(locks.holds(f.route.ticket)&&attemptOther().outcome==Outcome::Conflict);
    f.observation.stablePublicationQualified=true;
    // The new finished publication moves the tail strictly beyond the retained
    // zone. No route-revision match is needed: clearance belongs to OLD ticket.
    f.advanceHeadOneSection();result=f.assess();
    CHECK(result.coverage.complete()&&result.proof==Proof::Yes&&result.reason==AggregateClearanceReason::Clear);
    bool aliasOccupied=false;for(const auto& row:std::span(f.body).first(result.coverage.portions))aliasOccupied|=row.track==b;
    CHECK(aliasOccupied); // Aliases never masquerade as the original physical zone.
    ++now;CHECK(locks.observe(f.route.ticket,{now,replanned.routeRevision,Proof::Yes,result.proof},{7,now}).outcome==Outcome::Released);
    CHECK(!locks.holds(f.route.ticket)&&attemptOther().granted()&&locks.leaseCount()==1);
}
}
void* operator new(std::size_t size) {
    if(watchAllocations.load(std::memory_order_relaxed))allocations.fetch_add(1,std::memory_order_relaxed);
    if(auto* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) {return ::operator new(size);}
void operator delete(void* value) noexcept {std::free(value);}
void operator delete[](void* value) noexcept {std::free(value);}
void operator delete(void* value,std::size_t) noexcept {std::free(value);}
void operator delete[](void* value,std::size_t) noexcept {std::free(value);}
int main() {
    try {
        completeIsNotClear();aliasesAndResources();publicationAndIdentity();incompleteCoverageAndBudgets();roundedDistanceCannotRelease();retainedLeaseScenario();
        std::cout<<"synthetic aggregate rear clearance: PASS ("<<checks<<" checks, no native provider/game claim)\n";return 0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
