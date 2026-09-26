#include <engine/automatic_controller.h>
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
}
