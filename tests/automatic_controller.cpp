#include <engine/automatic_controller.h>
#include <engine/driving_publishers.h>
#include <cassert>
#include <limits>
namespace {
void measuredApproach(){
    using namespace nimby::engine::automatic;
    constexpr uint64_t source=(8ULL<<48)|1,next=(8ULL<<48)|0x10001;
    const std::vector<NimbySignalDrivingRule> rules{
        {source,0,10,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED},
        {next,-1,10,0,NIMBY_DRIVING_CLEAR}};
    for(double margin:{3.0,100.0}){
        Train state;state.managed=true;state.ahead={{source,100},{next,200}};
        double head=100-margin;uint64_t now=1000;
        const auto prepare=[&](std::span<const NimbySignalDrivingRule> current,bool fresh=true){
            return prepareIntegration(state,current,head,100,40,1,100,50,1,fresh,now,30,.4,500,10,margin);
        };
        auto planned=prepare(rules);assert(planned.signalStopDistanceM==0&&planned.ceiling==0);
        commitIntegration(state,planned.encountered,rules,head,head,.01,10,10,true,true,margin);
        assert(state.memory.stopped.signal==source);
        // Integrator and permission callback order is not fixed. An initial
        // stop measurement survives a conservative plan before first admission.
        planned=prepare(rules);assert(planned.signalStopDistanceM==0&&planned.ceiling==0);
        assert(state.memory.stopped.signal==source&&!state.memory.stopped.admitted);
        const auto grant=[&]{state.entrySignal=source;state.entry={head,200,200,now,true};};
        const auto step=margin/10;
        for(unsigned i=0;i<10;++i){
            grant();planned=prepare(rules);assert(!planned.signalStopDistanceM&&planned.ceiling>0&&!state.memory.sight);
            assert(planned.restricted&&planned.ceiling<=10&&planned.distance==195&&planned.target==0);
            const auto after=std::min(100.0,head+step);
            commitIntegration(state,planned.encountered,rules,head,after,1,10,10,true,true,margin);
            head=after;++now;
            assert(state.memory.stopped.signal==source&&!state.memory.sight);
        }
        grant();planned=prepare(rules);assert(!planned.signalStopDistanceM&&planned.ceiling>0);
        commitIntegration(state,planned.encountered,rules,head,100.1,1,10,10,true,true,margin);
        assert(!state.memory.stopped.signal&&state.memory.sight&&state.memory.sight->source==source);
        assert(state.memory.sight->endSignal==next); // The real crossing, not stopPoint, starts the mode.

        state={};state.managed=true;state.ahead={{source,100},{next,200}};
        head=100-margin;now=1000;state.memory.stopped=captureStopProof(rules[0],100,head,margin);
        state.entrySignal=source;state.entry={head,200,40,now,true};planned=prepare(rules);
        assert(planned.restricted&&!planned.signalStopDistanceM&&planned.distance==35&&planned.target==0);
        assert(planned.ceiling==sightCeiling({true,40,0},10,.4)&&planned.ceiling<10);
        assert(!state.memory.sight); // An obstacle before the panel does not require a premature passage.

        // The scanner rebuilds absolute positions from head + section distances.
        // A few ULPs are rounding; a change beyond one micrometre is a new anchor.
        state.entry.free=200;state.ahead[0].position=std::nextafter(100.0,std::numeric_limits<double>::infinity());
        planned=prepare(rules);assert(!planned.signalStopDistanceM&&state.memory.stopped.signal==source);
        state.ahead[0].position=100+stopProofPositionToleranceM*.5;
        planned=prepare(rules);assert(!planned.signalStopDistanceM&&state.memory.stopped.signal==source);
        state.ahead[0].position=100+stopProofPositionToleranceM*2;
        planned=prepare(rules);assert(planned.signalStopDistanceM&&!state.memory.stopped.signal);

        // Each interruption starts from an already measured, advancing train.
        // Lost permission never becomes an optimistic continuation on return.
        for(unsigned interruption=0;interruption<7;++interruption){
            state={};state.managed=true;state.ahead={{source,100},{next,200}};
            head=100-margin;now=1000;state.memory.stopped=captureStopProof(rules[0],100,head,margin);
            grant();planned=prepare(rules);assert(!planned.signalStopDistanceM&&planned.ceiling>0);
            commitIntegration(state,planned.encountered,rules,head,head+step,1,10,10,true,true,margin);
            head+=step;++now;grant();auto changed=rules;
            if(interruption==0)state.entry.free=5;
            if(interruption==1)now+=251; // Physical observation expired.
            if(interruption==2)changed[0].reopened_speed_mps=9;
            if(interruption==3)state.ahead[0].position=101; // Same ID moved on the route.
            if(interruption==4)state.ahead.erase(state.ahead.begin());
            if(interruption==5)head-=.005; // Smaller than the old whole-Memory reset threshold.
            planned=prepare(changed,interruption!=6);
            assert(!state.memory.stopped.signal);
            if(interruption!=4)assert(planned.signalStopDistanceM);
            if(interruption!=3&&interruption!=4)assert(planned.ceiling==0);
            // Reappearing data cannot reuse the old proof before a new measured stop.
            state.ahead={{source,100},{next,200}};grant();planned=prepare(rules);
            assert(planned.signalStopDistanceM&&planned.ceiling==0);
        }
    }
}
}
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
    assert(planned.signalStopDistanceM==97&&planned.stopSignal==b&&planned.distance==97&&planned.target==0);
    assert(planned.encountered.size()==1);
    commitIntegration(state,planned.encountered,sorted,0,0,0,10,10,true,true);
    assert(state.memory.stops.empty()); // No native movement, no fictitious crossing.
    std::vector<NimbySignalDrivingRule> restricted{{b,0,10,0,NIMBY_DRIVING_STOP|NIMBY_DRIVING_ON_SIGHT|NIMBY_DRIVING_STOP_THEN_PROCEED}};
    state.ahead={{b,0}};
    commitIntegration(state,{},restricted,0,0,0.01,10,10,true,true);
    assert(state.memory.stopped.signal==b);
    commitIntegration(state,{},restricted,0,0,0.01,11,10,true,true);
    assert(!state.memory.stopped.signal); // Invalid elapsed ticks invalidate proof.
    // Explicit signal stopping targets complement the speed envelope. A closer
    // native target still wins; equal distance tightens its target speed to zero.
    state={};state.managed=true;state.ahead={{b,100}};
    auto target=prepareIntegration(state,restricted,0,100,40,1,100,50,1,true,1000,30,.4,50,10);
    assert(target.signalStopDistanceM==97&&target.distance==50&&target.target==10);
    target=prepareIntegration(state,restricted,0,100,40,1,100,50,1,true,1000,30,.4,97,10);
    assert(target.distance==97&&target.target==0);
    target=prepareIntegration(state,restricted,97,100,40,1,100,50,1,true,1000,30,.4,500,10);
    assert(target.signalStopDistanceM==0&&target.distance==0&&target.target==0&&target.ceiling==0);
    commitIntegration(state,{},restricted,97,97,.01,10,10,true,true);
    assert(state.memory.stopped.signal==b&&state.memory.stopped.position==100);
    state.entrySignal=b;state.entry={97,200,200,1000,true};
    target=prepareIntegration(state,restricted,97,100,40,1,100,50,1,true,1000,30,.4,500,10);
    assert(!target.signalStopDistanceM&&target.ceiling>0&&target.restricted&&target.distance==195&&target.target==0);
    state={};state.managed=true;state.ahead={{b,100}};
    target=prepareIntegration(state,restricted,90,100,40,1,100,50,1,false,1000,30,.4,500,10,10);
    assert(target.signalStopDistanceM==0&&target.distance==0&&target.target==0&&target.ceiling==0);
    commitIntegration(state,{},restricted,90,90,.01,10,10,true,true,10);
    assert(state.memory.stopped.position==100);
    // A simultaneous on-sight obstacle must not overwrite a closer signal
    // target. Conversely its own fixed 5 m stopping margin remains applicable.
    state={};state.managed=true;state.ahead={{b,100}};
    state.memory.sight=RestrictedMode{a,0,0,0,10};state.view={0,200,200,1000,true};
    const std::vector<NimbySignalDrivingRule> absolute{{b,0,10,0,NIMBY_DRIVING_STOP}};
    target=prepareIntegration(state,absolute,0,100,40,1,100,50,1,true,1000,30,.4,500,10,10);
    assert(target.signalStopDistanceM==90&&target.distance==90&&target.target==0);
    state.view.free=20;
    target=prepareIntegration(state,absolute,0,100,40,1,100,50,1,true,1000,30,.4,500,10,10);
    assert(target.signalStopDistanceM==90&&target.distance==15&&target.target==0);
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
    measuredApproach();
}
