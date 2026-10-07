#include <engine/automatic_controller.h>
#include <engine/driving_publishers.h>
#include <cassert>
int main(){
    using namespace nimby::engine::automatic;
    constexpr uint64_t a=0x8000000000001,b=0x8000000010001;
    std::vector<NimbySignalDrivingRule> rules{{b,0,10,0,NIMBY_DRIVING_STOP},{a,-1,10,0,NIMBY_DRIVING_CLEAR}},sorted;
    assert(prepareRules(rules.data(),2,1000,0,sorted)&&sorted[0].signal==a&&sorted[1].signal==b);
    rules[1].signal=b;
    assert(!prepareRules(rules.data(),2,1000,0,sorted)&&sorted[0].signal==a);
    assert(!prepareRules(nullptr,1,1000,0,sorted));
    assert(!prepareRules(nullptr,0,99,0,sorted));
    assert(!prepareRules(nullptr,0,5001,0,sorted));
    assert(!prepareRules(nullptr,0,1000,~0u,sorted));
    Train state;
    auto unchanged=prepareIntegration(state,sorted,0,100,40,1,100,50,1,true,1000,30,0.4,500,10);
    assert(!unchanged.observed&&unchanged.ceiling==30&&unchanged.braking==0.4&&unchanged.distance==500&&unchanged.target==10);
    state.managed=true;state.ahead={{b,100}};
    auto planned=prepareIntegration(state,sorted,0,100,40,1,100,50,1,true,1000,30,0.4,500,10);
    assert(planned.observed&&planned.ceiling<30&&state.memory.stops.empty());
    assert(planned.encountered.size()==1);
    commitIntegration(state,planned.encountered,sorted,0,0,0,10,10,true,true);
    assert(state.memory.stops.empty()); // No native movement, no fictitious crossing.
    std::vector<NimbySignalDrivingRule> restricted{{b,0,10,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
    state.ahead={{b,0}};
    commitIntegration(state,{},restricted,0,0,0.01,10,10,true,true);
    assert(state.memory.stopped.signal==b);
    commitIntegration(state,{},restricted,0,0,0.01,11,10,true,true);
    assert(!state.memory.stopped.signal); // Invalid elapsed ticks invalidate proof.
    assert(prepareRules(nullptr,0,100,0,sorted)&&sorted.empty());
    DrivingPublishers<NimbySignalDrivingRule> owners;
    const NimbySignalDrivingRule first{a,0,10,0,NIMBY_DRIVING_STOP},second{b,-1,10,0,NIMBY_DRIVING_CLEAR};
    assert(owners.publish(1,{first},1000)==NIMBY_OK);
    const auto initial=owners;
    auto prepared=initial;
    assert(prepared.publish(1,{second},1500)==NIMBY_OK);
    assert(owners.sameVersion(initial)&&!owners.sameVersion(prepared));
    assert(owners.owner(a)==1&&owners.owner(b)==0&&!owners.fresh(a,1100));
    auto heartbeat=initial;assert(heartbeat.publish(1,{first},2000)==NIMBY_OK);
    assert(!owners.sameOwnerVersion(heartbeat,1)&&!owners.fresh(a,1100));
    assert(owners.publish(2,{second},500)==NIMBY_OK&&owners.rows().size()==2);
    assert(owners.publish(2,{first},2000)==NIMBY_RESOURCE_LIMIT);
    assert(owners.fresh(a,600)&&!owners.fresh(b,600)&&owners.owner(b)==2);
    assert(owners.publish(1,{first},2000)==NIMBY_OK&&!owners.fresh(b,600));
    assert(owners.publish(3,{},3000)==NIMBY_OK&&owners.rows().size()==2);
    assert(owners.publish(2,{},3000)==NIMBY_OK&&owners.rows().size()==1&&owners.owner(a)==1);
    // A publisher flood exhausts only bounded slots, leaving A renewable.
    for(uint64_t token=2;token<=64;++token)assert(owners.publish(token,{},3000,NIMBY_DRIVING_MAXIMUM_LINE_SPEED)==NIMBY_OK);
    assert(owners.publish(65,{second},3000)==NIMBY_RESOURCE_LIMIT);
    assert(owners.publish(1,{first},4000)==NIMBY_OK&&owners.fresh(a,3500));
    assert(owners.publish(64,{},3000)==NIMBY_OK);
    assert(owners.publish(65,{second},3000)==NIMBY_OK);
}
