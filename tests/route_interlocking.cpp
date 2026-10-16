#include <engine/route_interlocking.h>
#include <iostream>
#include <limits>
#include <string>

using namespace nimby::engine::interlocking;
static unsigned checks=0;
#define CHECK(x) do { ++checks; if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x); } while(false)
constexpr uint64_t track(unsigned ordinal,unsigned generation=1){return (1ULL<<48)|(uint64_t(ordinal)<<16)|generation;}
constexpr uint64_t train(unsigned ordinal,unsigned generation=1){return (5ULL<<48)|(uint64_t(ordinal)<<16)|generation;}
constexpr uint64_t signal(unsigned ordinal,unsigned generation=1){return (8ULL<<48)|(uint64_t(ordinal)<<16)|generation;}
Ticket ticket(unsigned t,unsigned s,uint64_t revision=1,uint64_t session=7){return {session,train(t),signal(s),revision};}
Request request(Ticket id,std::span<const Interval> resources,uint64_t at=100){return {id,resources,at,true,Proof::No};}
Evidence evidence(uint64_t revision,Proof entered,Proof rear=Proof::No,uint64_t at=100){return {at,revision,entered,rear};}
constexpr Frame now{7,100};

static void atomicRoutesAndOpposition(){
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2),c=ticket(3,3);
    const std::vector<Interval> east{{track(1),.2,.8}};
    const std::vector<Interval> west{{track(1),.8,.2}};
    CHECK(locks.acquire(request(a,east),now).granted());
    const auto refused=locks.acquire(request(b,west),now);
    CHECK(refused.outcome==Outcome::Conflict&&refused.blocker==a&&refused.track==track(1));
    CHECK(locks.leaseCount()==1&&locks.resourceCount()==1&&!locks.holds(b));
    // Requesting two resources is indivisible: failure on track1 must not
    // leave a partial lock on track2 that obstructs an unrelated route.
    const std::vector<Interval> two{{track(2),0,1},{track(1),.1,.5}};
    CHECK(locks.acquire(request(b,two),now).outcome==Outcome::Conflict);
    const std::vector<Interval> free{{track(2),0,1}};
    CHECK(locks.acquire(request(c,free),now).granted());
    CHECK(locks.permission(a,now).granted()&&locks.permission(c,now).granted());
    const std::vector<Interval> touching{{track(1),.8,.9}};
    CHECK(locks.acquire(request(b,touching),now).outcome==Outcome::Conflict);
}

static void junctionAndQualifiedCrossingAliases(){
    //       A ---\ .
    //             >--- D       C ----- independent E
    //       B ---/
    // Two incoming routes share D; only one can hold the route through it.
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2),c=ticket(3,3);
    const std::vector<Interval> fromA{{track(1),0,1},{track(4),.1,.6}};
    const std::vector<Interval> fromB{{track(2),0,1},{track(4),.2,.9}};
    const std::vector<Interval> independent{{track(3),0,1},{track(5),0,1}};
    CHECK(locks.acquire(request(a,fromA),now).granted());
    CHECK(locks.acquire(request(b,fromB),now).outcome==Outcome::Conflict);
    CHECK(locks.acquire(request(c,independent),now).granted());
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::No),now).outcome==Outcome::Updated);
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(locks.acquire(request(b,fromB),now).granted());
    // The qualified adapter expands the geometric crossing into a shared
    // track interval alias. The core never guesses crossings from geometry.
    RouteInterlocking crossing;CHECK(crossing.beginSession(7));
    const std::vector<Interval> horizontal{{track(10),0,1},{track(12),.45,.55}};
    const std::vector<Interval> vertical{{track(11),0,1},{track(12),.5,.5}};
    CHECK(crossing.acquire(request(a,horizontal),now).granted());
    const auto result=crossing.acquire(request(b,vertical),now);
    CHECK(result.outcome==Outcome::Conflict&&result.track==track(12));
    CHECK(!crossing.holds(b)&&crossing.resourceCount()==2);
}

static void rearClearanceRequiresPositiveEnteredEvidence(){
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2);
    const std::vector<Interval> block{{track(1),0,1}};
    CHECK(locks.acquire(request(a,block),now).granted());
    // Empty before entry is not a rear-clearance proof for a granted route.
    CHECK(locks.observe(a,evidence(1,Proof::No,Proof::Yes),now).outcome==Outcome::Updated);
    CHECK(locks.holds(a));
    CHECK(locks.observe(a,evidence(1,Proof::Yes),now).outcome==Outcome::Updated);
    CHECK(locks.observe(a,evidence(1,Proof::Unknown,Proof::Unknown),now).outcome==Outcome::Unavailable);
    CHECK(locks.holds(a)&&!locks.permission(a,now).granted());
    CHECK(locks.acquire(request(b,block),now).outcome==Outcome::Conflict);
    // The head has crossed; a tail still present retains ALL resources.
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::No),now).outcome==Outcome::Updated);
    CHECK(locks.acquire(request(b,block),now).outcome==Outcome::Conflict);
    CHECK(locks.observe(a,evidence(1,Proof::Unknown,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(!locks.holds(a)&&locks.leaseCount()==0&&locks.resourceCount()==0);
    CHECK(locks.acquire(request(b,block),now).granted());
}

static void reroutingBeforeAndAfterEntry(){
    const std::vector<Interval> oldRoute{{track(1),0,1}},newRoute{{track(2),0,1}};
    const auto old=ticket(1,1),next=ticket(1,1,2),peer=ticket(2,2);
    RouteInterlocking before;CHECK(before.beginSession(7));
    CHECK(before.acquire(request(old,oldRoute),now).granted());
    CHECK(before.acquire(request(next,newRoute),now).granted());
    CHECK(!before.holds(old)&&before.holds(next));
    CHECK(before.acquire(request(peer,oldRoute),now).granted());
    // A failed NEW acquisition still revokes a positively abandoned route.
    RouteInterlocking blocked;CHECK(blocked.beginSession(7));
    CHECK(blocked.acquire(request(old,oldRoute),now).granted());
    CHECK(blocked.acquire(request(peer,newRoute),now).granted());
    CHECK(blocked.acquire(request(next,newRoute),now).outcome==Outcome::Conflict);
    CHECK(!blocked.holds(old)&&!blocked.holds(next));
    CHECK(blocked.acquire(request(ticket(3,3),oldRoute),now).granted());

    RouteInterlocking after;CHECK(after.beginSession(7));
    CHECK(after.acquire(request(old,oldRoute),now).granted());
    CHECK(after.observe(old,evidence(1,Proof::Yes),now).outcome==Outcome::Updated);
    CHECK(after.acquire(request(next,newRoute),now).outcome==Outcome::RetainedAfterEntry);
    CHECK(after.holds(old)&&!after.holds(next)&&!after.permission(old,now).granted());
    CHECK(after.acquire(request(peer,oldRoute),now).outcome==Outcome::Conflict);
    // Route revision2 cannot clear resources still owned by revision1.
    CHECK(after.observe(next,evidence(2,Proof::Yes,Proof::Yes),now).outcome==Outcome::Missing);
    CHECK(after.holds(old));
    CHECK(after.observe(old,evidence(2,Proof::Unknown,Proof::Unknown),now).outcome==Outcome::RetainedAfterEntry);
    CHECK(after.observe(old,evidence(2,Proof::Unknown,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(after.acquire(request(next,newRoute),now).granted());

    RouteInterlocking cancelled;CHECK(cancelled.beginSession(7));
    CHECK(cancelled.acquire(request(old,oldRoute),now).granted());
    CHECK(cancelled.observe(old,evidence(2,Proof::Unknown),now).outcome==Outcome::RetainedAfterEntry);
    CHECK(cancelled.holds(old)); // Unknown is not a proof of being before entry.
    CHECK(cancelled.observe(old,evidence(2,Proof::No),now).outcome==Outcome::Released);
}

static void staleMissingAndWorldLineage(){
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2);
    const std::vector<Interval> block{{track(1),0,1}};
    CHECK(locks.acquire(request(a,block),now).granted());
    CHECK(locks.observe(a,evidence(1,Proof::Yes),now).outcome==Outcome::Updated);
    const Frame later{7,1200};
    CHECK(locks.permission(a,later).outcome==Outcome::Stale);
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::Yes,100),later).outcome==Outcome::Stale);
    CHECK(locks.holds(a)); // No expiry cleanup or train-disappearance shortcut.
    CHECK(locks.acquire(request(b,block,1200),later).outcome==Outcome::Conflict);
    CHECK(locks.observe(a,{1200,std::nullopt,Proof::Unknown,Proof::Unknown},later).outcome==Outcome::Unavailable);
    CHECK(locks.holds(a));
    CHECK(locks.acquire(request(b,block,1200),later).outcome==Outcome::Conflict);
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::Yes,1300),later).outcome==Outcome::Stale);
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::Yes,100),now).outcome==Outcome::Stale);
    CHECK(!locks.beginSession(0)&&locks.holds(a));
    CHECK(locks.beginSession(7)&&locks.holds(a));
    CHECK(locks.acquire(request(ticket(2,2,1,8),block,1200),later).outcome==Outcome::SessionMismatch);
    CHECK(locks.beginSession(8)&&locks.leaseCount()==0&&locks.resourceCount()==0);
    const auto reloaded=ticket(2,2,1,8);
    CHECK(locks.acquire(request(reloaded,block),{8,100}).granted());
    CHECK(locks.observe(a,evidence(1,Proof::Yes,Proof::Yes),{8,100}).outcome==Outcome::SessionMismatch);
    CHECK(locks.holds(reloaded));
}

static void boundedFailuresDoNotObstructIndependentOwners(){
    const auto a=ticket(1,1),b=ticket(2,2),c=ticket(3,3);
    const std::vector<Interval> block{{track(1),.1,.9}},other{{track(2),.1,.9}};
    RouteInterlocking limited({1,2,2,2,1000});CHECK(limited.beginSession(7));
    CHECK(limited.acquire(request(a,block),now).granted());
    CHECK(limited.acquire(request(b,other),now).outcome==Outcome::Capacity);
    CHECK(limited.permission(a,now).granted()&&limited.leaseCount()==1);
    CHECK(limited.observe(a,evidence(1,Proof::Yes,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(limited.acquire(request(b,other),now).granted());

    RouteInterlocking budget({8,16,4,1,1000});CHECK(budget.beginSession(7));
    CHECK(budget.acquire(request(a,block),now).granted());
    // Two overlapping locks of the SAME train are compatible, but scanning
    // them still consumes the bounded comparison budget for a third request.
    const std::vector<Interval> nested{{track(1),.2,.8}};
    CHECK(budget.acquire(request(ticket(1,2),nested),now).granted());
    const auto refused=budget.acquire(request(ticket(1,3),nested),now);
    CHECK(refused.outcome==Outcome::BudgetExceeded&&refused.checks==1);
    CHECK(budget.leaseCount()==2&&budget.resourceCount()==2);
    CHECK(budget.acquire(request(c,other),now).granted());
    CHECK(budget.permission(a,now).granted()&&budget.permission(c,now).granted());
    // Disjoint sections on a crowded track are found through a prefix index,
    // not by spending the entire budget walking already-ended intervals.
    const std::vector<Interval> distant{{track(1),.95,1}};
    CHECK(budget.acquire(request(b,distant),now).granted());
}

static void incompleteOwnerEvidenceClosesWithoutFreeing(){
    const auto a=ticket(1,1),next=ticket(1,1,2),b=ticket(2,2),c=ticket(3,3);
    const std::vector<Interval> block{{track(1),.1,.9}},other{{track(2),.1,.9}},newRoute{{track(3),0,1}};
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    CHECK(locks.acquire(request(a,block),now).granted());
    CHECK(locks.acquire(request(c,other),now).granted());
    auto incomplete=request(a,block,101);incomplete.complete=false;incomplete.entered=Proof::Yes;
    CHECK(locks.acquire(incomplete,{7,101}).outcome==Outcome::Unavailable);
    CHECK(locks.holds(a)&&!locks.permission(a,{7,101}).granted());
    CHECK(locks.permission(c,{7,101}).granted());
    CHECK(locks.acquire(request(b,block,101),{7,101}).outcome==Outcome::Conflict);
    // The independently proved entry must survive the incomplete request.
    CHECK(locks.acquire(request(next,newRoute,102),{7,102}).outcome==Outcome::RetainedAfterEntry);
    CHECK(locks.holds(a)&&!locks.holds(next));
    CHECK(locks.observe(a,evidence(2,Proof::No,Proof::No,103),{7,103}).outcome==Outcome::RetainedAfterEntry);
    CHECK(locks.acquire(request(b,block,103),{7,103}).outcome==Outcome::Conflict);
    CHECK(locks.observe(a,evidence(2,Proof::Unknown,Proof::Yes,104),{7,104}).outcome==Outcome::Released);
    CHECK(locks.acquire(request(b,block,104),{7,104}).granted());

    // Invalid geometry cannot renew a previously valid movement permission.
    const std::vector<Interval> invalid{{track(1),0,std::numeric_limits<double>::quiet_NaN()}};
    CHECK(locks.acquire(request(b,invalid,105),{7,105}).outcome==Outcome::InvalidRequest);
    CHECK(locks.holds(b)&&!locks.permission(b,{7,105}).granted());
    CHECK(locks.permission(c,{7,105}).granted());
    CHECK(locks.acquire(request(b,block,104),{7,105}).outcome==Outcome::Stale);
    CHECK(locks.acquire(request(b,block,106),{7,106}).granted());

    // Entry data missing from a fully known footprint still closes authority.
    auto unknown=request(b,block,107);unknown.entered=Proof::Unknown;
    CHECK(locks.acquire(unknown,{7,107}).outcome==Outcome::Unavailable);
    CHECK(locks.holds(b)&&!locks.permission(b,{7,107}).granted());
}

static void consecutiveSourcesAndResourceCaps(){
    const auto first=ticket(1,1),second=ticket(1,2),otherTrain=ticket(2,3);
    const std::vector<Interval> split{{track(1),.1,.2},{track(1),.7,.8}};
    const std::vector<Interval> overlap{{track(1),.15,.75}},independent{{track(2),0,1}};
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    CHECK(locks.acquire(request(first,split),now).granted());
    CHECK(locks.acquire(request(second,overlap),now).granted());
    CHECK(locks.resourceCount()==3&&locks.leaseCount()==2);
    // A train's consecutive source leases may overlap. Clearing one source
    // removes its disjoint intervals once, without removing the second lease.
    CHECK(locks.observe(first,evidence(1,Proof::Yes,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(!locks.holds(first)&&locks.holds(second)&&locks.resourceCount()==1);
    CHECK(locks.acquire(request(otherTrain,overlap),now).outcome==Outcome::Conflict);
    CHECK(locks.observe(second,evidence(1,Proof::Yes,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(locks.acquire(request(otherTrain,overlap),now).granted());

    RouteInterlocking bounded({8,2,2,32,1000});CHECK(bounded.beginSession(7));
    CHECK(bounded.acquire(request(first,split),now).granted());
    CHECK(bounded.acquire(request(otherTrain,independent),now).outcome==Outcome::Capacity);
    CHECK(!bounded.holds(otherTrain)&&bounded.permission(first,now).granted());
    const std::vector<Interval> excess{{track(2),0,1},{track(3),0,1},{track(4),0,1}};
    CHECK(bounded.acquire(request(otherTrain,excess),now).outcome==Outcome::Capacity);
    CHECK(bounded.resourceCount()==2&&bounded.leaseCount()==1);
    CHECK(bounded.observe(first,evidence(1,Proof::Yes,Proof::Yes),now).outcome==Outcome::Released);
    CHECK(bounded.acquire(request(otherTrain,independent),now).granted());
    CHECK(bounded.resourceCount()==1&&bounded.leaseCount()==1);
}

static void fullIdentitiesCanonicalizationAndInvalidRequests(){
    RouteInterlocking locks;CHECK(locks.beginSession(7));
    const auto a=ticket(1,1),b=ticket(2,2);
    const std::vector<Interval> raw{{track(2),.6,.2},{track(1),.3,.1},{track(1),.2,.5},{track(2),.2,.6}};
    const std::vector<Interval> canonical{{track(1),.1,.5},{track(2),.2,.6}};
    CHECK(locks.acquire(request(a,raw),now).granted()&&locks.resourceCount()==2);
    CHECK(locks.acquire(request(a,canonical),now).granted()&&locks.resourceCount()==2);
    // An in-place mutation is NOT a renewal of a qualified route revision.
    const std::vector<Interval> mutated{{track(3),0,1}};
    CHECK(locks.acquire(request(a,mutated),now).outcome==Outcome::InvalidRequest&&locks.holds(a));
    CHECK(!locks.permission(a,now).granted());
    // Recycling an ordinal does not alias the old track generation.
    const std::vector<Interval> recycled{{track(1,2),.1,.5}};
    CHECK(locks.acquire(request(b,recycled),now).granted());
    auto wrong=a;wrong.train=train(1,2);
    CHECK(locks.permission(wrong,now).outcome==Outcome::Missing);
    CHECK(locks.observe(wrong,evidence(1,Proof::Yes,Proof::Yes),now).outcome==Outcome::Missing);
    CHECK(locks.holds(a));
    auto invalid=ticket(3,3);invalid.source=signal(3)&0xffffffffffffULL;
    CHECK(locks.acquire(request(invalid,mutated),now).outcome==Outcome::InvalidRequest);
    const std::vector<Interval> invalidTrack{{3,0,1}},nan{{track(3),0,std::numeric_limits<double>::quiet_NaN()}},outside{{track(3),-.1,1}};
    for(const auto* resources:{&invalidTrack,&nan,&outside})
        CHECK(locks.acquire(request(ticket(3,3),*resources),now).outcome==Outcome::InvalidRequest);
    auto unknown=request(ticket(3,3),mutated);unknown.complete=false;
    CHECK(locks.acquire(unknown,now).outcome==Outcome::Unavailable);
    unknown.complete=true;unknown.entered=Proof::Unknown;
    CHECK(locks.acquire(unknown,now).outcome==Outcome::Unavailable);
    CHECK(locks.acquire(request(ticket(3,3),{},100),now).outcome==Outcome::Unavailable);
    CHECK(locks.leaseCount()==2&&locks.resourceCount()==3);
    // Deterministic blocker, regardless of request interval order.
    auto reverse=canonical;std::reverse(reverse.begin(),reverse.end());
    const auto first=locks.acquire(request(ticket(3,3),canonical),now);
    const auto second=locks.acquire(request(ticket(3,3),reverse),now);
    CHECK(first.outcome==Outcome::Conflict&&second.outcome==Outcome::Conflict);
    CHECK(first.blocker==second.blocker&&first.track==second.track&&first.track==track(1));
}

int main(){try{
    atomicRoutesAndOpposition();junctionAndQualifiedCrossingAliases();rearClearanceRequiresPositiveEnteredEvidence();
    reroutingBeforeAndAfterEntry();staleMissingAndWorldLineage();boundedFailuresDoNotObstructIndependentOwners();
    incompleteOwnerEvidenceClosesWithoutFreeing();consecutiveSourcesAndResourceCaps();
    fullIdentitiesCanonicalizationAndInvalidRequests();
    std::cout<<"PASS: "<<checks<<" route interlocking checks; pure caller-qualified resources, no native bookings\n";
    return 0;
}catch(const std::exception& failure){std::cerr<<failure.what()<<'\n';return 1;}}
