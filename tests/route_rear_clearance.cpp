#include <engine/route_rear_clearance.h>
#include <array>
#include <cstdio>
#include <limits>

using namespace nimby::engine::interlocking;
namespace {
constexpr uint64_t trackA=0x1000000010001,trackB=0x1000000020001,trackC=0x1000000030001;
constexpr uint64_t train=0x5000000010001,source=0x8000000010001;
constexpr RearClearanceTick frame{7,3,100};
struct Fixture {
    std::vector<Interval> own{{trackA,.2,.8}},aliases{{trackB,.4,.6}};
    std::vector<uint64_t> expected{11,12},vehicles{11,12};
    std::vector<VehicleFootprint> footprints{{train,11,trackC,.1,.2},{train,12,trackC,.3,.4}};
    RearClearanceRoute route{{7,train,source,9},4,Proof::Yes,true,frame,{},{},{}};
    RearClearanceObservation observation{7,train,4,frame,frame,true,true,frame,{},{}};
    RearClearanceResult assess(RearClearanceTick current=frame,RearClearanceLimits limits={}) {
        route.ownPhysical=own;route.aliases=aliases;route.expectedVehicles=expected;
        observation.vehicles=vehicles;observation.footprints=footprints;
        return assessRearClearance(route,observation,current,limits);
    }
};
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(){
    {Fixture f;const auto result=f.assess();CHECK(result.proof==Proof::Yes&&result.reason==RearClearanceReason::Clear);}
    {Fixture f;f.footprints[1]={train,12,trackA,.7,.9};const auto result=f.assess();
     CHECK(result.proof==Proof::No&&result.reason==RearClearanceReason::Occupied&&result.vehicle==12&&result.track==trackA);
     // A head outside the route does not hide a trailing vehicle still inside.
    }
    for(const auto interval:std::array{std::pair{.1,.2},std::pair{.8,.9},std::pair{.8,.8}}){
        Fixture f;f.footprints[0]={train,11,trackA,interval.first,interval.second};CHECK(f.assess().proof==Proof::No);
    }
    {Fixture f;f.footprints[0]={train,11,trackA,.8000001,.9};CHECK(f.assess().proof==Proof::Yes);}
    {Fixture f;f.footprints[0]={train,11,trackB,.45,.55};CHECK(f.assess().proof==Proof::Yes);
     // An alias exclusion is not a claim that the train travelled that track.
    }
    {Fixture f;f.aliases.clear();CHECK(f.assess().proof==Proof::Yes);}
    {Fixture f;f.own={{trackA,.6,.8},{trackA,.2,.5},{trackA,.4,.7},{trackC,.8,.9}};
     f.footprints[1]={train,12,trackA,.5,.5};CHECK(f.assess().proof==Proof::No);}
    {Fixture f;f.footprints.push_back({train,11,trackA,.3,.4});CHECK(f.assess().proof==Proof::No);
     // Multiple physical sections for one vehicle are necessary at track joins.
    }
    {Fixture f;f.footprints[0].track=trackA+1;CHECK(f.assess().proof==Proof::Yes);
     // Full track generations stay distinct; no ordinal masking is permitted.
    }
    for(const auto entered:{Proof::Unknown,Proof::No}){
        Fixture f;f.route.entered=entered;CHECK(f.assess().reason==RearClearanceReason::NotEntered);
        f.footprints[0]={train,11,trackA,.3,.4};CHECK(f.assess().proof==Proof::No);
    }
    {Fixture f;f.observation.completeConsist=false;CHECK(f.assess().reason==RearClearanceReason::IncompleteConsist);
     f.footprints[0]={train,11,trackA,.3,.4};CHECK(f.assess().proof==Proof::No);}
    {Fixture f;f.observation.stableTickCertificate.reset();CHECK(f.assess().reason==RearClearanceReason::MissingStableTick);
     // Repeated identical inputs do not invent a missing stable-tick certificate.
     CHECK(f.assess().proof==Proof::Unknown);f.footprints[0]={train,11,trackA,.3,.4};CHECK(f.assess().proof==Proof::No);}
    {Fixture f;f.observation.footprintsQualified=false;CHECK(f.assess().reason==RearClearanceReason::UnqualifiedFootprints);}
    {Fixture f;f.route.qualified=false;CHECK(f.assess().reason==RearClearanceReason::UnqualifiedRoute);}
    {Fixture f;f.own.clear();CHECK(f.assess().reason==RearClearanceReason::UnqualifiedRoute);}
    for(unsigned field=0;field<4;++field){
        Fixture f;auto different=frame;++different.tick;
        if(field==0)f.route.qualifiedAt=different;
        if(field==1)f.observation.consistAt=different;
        if(field==2)f.observation.footprintsAt=different;
        if(field==3)f.observation.stableTickCertificate=different;
        CHECK(f.assess().proof==Proof::Unknown&&f.assess().reason==RearClearanceReason::DifferentTick);
        f.footprints[0]={train,11,trackA,.3,.4};CHECK(f.assess().proof==Proof::Unknown);
    }
    {Fixture f;f.observation.footprintsAt.epoch++;CHECK(f.assess().reason==RearClearanceReason::DifferentTick);}
    {Fixture f;auto first=frame;first.tick=0;f.route.qualifiedAt=f.observation.consistAt=f.observation.footprintsAt=first;
     f.observation.stableTickCertificate=first;CHECK(f.assess(first).proof==Proof::Yes);}
    for(unsigned field=0;field<8;++field){
        Fixture f;
        if(field==0)f.observation.train++;
        if(field==1)f.observation.session++;
        if(field==2)f.observation.consistRevision++;
        if(field==3)f.route.ticket.session++;
        if(field==4)f.route.ticket.train=trackA;
        if(field==5)f.route.ticket.source=train;
        if(field==6)f.route.ticket.routeRevision=0;
        if(field==7)f.route.consistRevision=0;
        CHECK(f.assess().proof==Proof::Unknown&&f.assess().reason==RearClearanceReason::InvalidIdentity);
    }
    {Fixture f;auto wrong=frame;wrong.session=0;CHECK(f.assess(wrong).reason==RearClearanceReason::InvalidIdentity);}
    {Fixture f;auto wrong=frame;wrong.epoch=0;CHECK(f.assess(wrong).reason==RearClearanceReason::InvalidIdentity);}
    {Fixture f;f.footprints[0].train++;CHECK(f.assess().reason==RearClearanceReason::InvalidIdentity);}
    {Fixture f;f.route.entered=static_cast<Proof>(99);CHECK(f.assess().reason==RearClearanceReason::InvalidIdentity);}
    {Fixture f;f.expected.push_back(11);CHECK(f.assess().reason==RearClearanceReason::InvalidVehicles);}
    {Fixture f;f.vehicles.push_back(12);CHECK(f.assess().reason==RearClearanceReason::InvalidVehicles);}
    {Fixture f;f.expected[0]=0;CHECK(f.assess().reason==RearClearanceReason::InvalidVehicles);}
    {Fixture f;f.vehicles[0]=0;CHECK(f.assess().reason==RearClearanceReason::InvalidVehicles);}
    {Fixture f;f.expected.clear();f.vehicles.clear();f.footprints.clear();CHECK(f.assess().reason==RearClearanceReason::InvalidVehicles);}
    {Fixture f;f.vehicles.pop_back();CHECK(f.assess().reason==RearClearanceReason::MissingVehicle);}
    {Fixture f;f.vehicles[0]=13;CHECK(f.assess().reason==RearClearanceReason::MissingVehicle);}
    {Fixture f;f.footprints[0].vehicle=13;CHECK(f.assess().reason==RearClearanceReason::InvalidVehicles);}
    {Fixture f;f.footprints.pop_back();const auto result=f.assess();CHECK(result.reason==RearClearanceReason::MissingVehicle&&result.vehicle==12);}
    {Fixture f;f.footprints[0]={train,11,trackA,.3,.4};f.footprints.pop_back();CHECK(f.assess().proof==Proof::Unknown);}
    {Fixture f;f.footprints.clear();CHECK(f.assess().proof==Proof::Unknown&&f.assess().reason==RearClearanceReason::MissingVehicle);}
    {Fixture f;f.footprints.push_back(f.footprints.front());CHECK(f.assess().reason==RearClearanceReason::DuplicateFootprint);}
    {Fixture f;f.footprints[0]={train,11,trackA,.3,.4};f.footprints[1].vehicle=13;
     CHECK(f.assess().proof==Proof::Unknown); // An early overlap must not hide malformed later evidence.
    }
    for(const auto bad:{-.1,1.1,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){
        Fixture f;f.footprints[0].begin=bad;CHECK(f.assess().reason==RearClearanceReason::InvalidInterval);
        f=Fixture{};f.own[0].end=bad;CHECK(f.assess().reason==RearClearanceReason::InvalidInterval);
        f=Fixture{};f.aliases[0].begin=bad;CHECK(f.assess().reason==RearClearanceReason::InvalidInterval);
    }
    {Fixture f;f.own[0].track=train;CHECK(f.assess().reason==RearClearanceReason::InvalidInterval);}
    {Fixture f;f.footprints[0].begin=.8;f.footprints[0].end=.2;CHECK(f.assess().reason==RearClearanceReason::InvalidInterval);}
    {Fixture f;f.vehicles={12,11};f.expected={12,11};std::reverse(f.footprints.begin(),f.footprints.end());CHECK(f.assess().proof==Proof::Yes);}
    for(unsigned budget=0;budget<5;++budget){
        Fixture f;RearClearanceLimits limits;
        if(budget==0){limits.ownPortions=1;f.own.push_back({trackB,0,1});}
        if(budget==1){limits.aliases=1;f.aliases.push_back({trackC,0,1});}
        if(budget==2)limits.vehicles=1;
        if(budget==3)limits.footprints=1;
        if(budget==4)limits.overlapChecks=1;
        const auto result=f.assess(frame,limits);CHECK(result.proof==Proof::Unknown&&result.reason==RearClearanceReason::BudgetExceeded);
        CHECK(result.checks<=limits.overlapChecks);
    }
    {Fixture f;RearClearanceLimits limits;limits.overlapChecks=0;CHECK(f.assess(frame,limits).reason==RearClearanceReason::InvalidLimits);}
    {Fixture f;RearClearanceLimits limits;limits.vehicles=4097;CHECK(f.assess(frame,limits).reason==RearClearanceReason::InvalidLimits);}
    {Fixture f;f.expected.clear();f.vehicles.clear();f.footprints.clear();
     for(uint64_t vehicle=1;vehicle<=4096;++vehicle){f.expected.push_back(vehicle);f.vehicles.push_back(vehicle);
         f.footprints.push_back({train,vehicle,trackC,.1,.2});}
     const auto result=f.assess();CHECK(result.proof==Proof::Yes&&result.checks<=RearClearanceLimits{}.overlapChecks);
     f.expected.push_back(4097);CHECK(f.assess().reason==RearClearanceReason::BudgetExceeded);
    }
    // Consuming the result leaves ownership with RouteInterlocking. Missing
    // evidence cannot release a lease, while certified actual rear clearance can.
    {Fixture f;RouteInterlocking locks;CHECK(locks.beginSession(frame.session));
     std::vector<Interval> claimed=f.own;claimed.insert(claimed.end(),f.aliases.begin(),f.aliases.end());
     CHECK(locks.acquire({f.route.ticket,claimed,100,true,Proof::Yes},{7,100}).granted());
     f.observation.stableTickCertificate.reset();auto result=f.assess();
     CHECK(locks.observe(f.route.ticket,{100,9,Proof::Yes,result.proof},{7,100}).outcome==Outcome::Updated);
     CHECK(locks.holds(f.route.ticket));f.observation.stableTickCertificate=frame;result=f.assess();
     CHECK(locks.observe(f.route.ticket,{100,9,Proof::Yes,result.proof},{7,100}).outcome==Outcome::Released);
     CHECK(!locks.holds(f.route.ticket));
    }
    std::puts("PASS rear clearance: full identities, complete consist, explicit tick certificate, missing/duplicate data, closed contact, own/alias separation and bounded qualification.");
}
