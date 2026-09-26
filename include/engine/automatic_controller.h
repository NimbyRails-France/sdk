#pragma once
#include "engine/automatic_driving.h"
#include "engine/physical_view.h"
#include "engine/train_constraint.h"

namespace nimby::engine::automatic {
// Per-train simulation state. The opaque motion token detects object replacement;
// this layer never dereferences it. Callers serialize access and supply clocks.
struct Train {
    uintptr_t motion{};
    std::vector<Ahead> ahead;
    Memory memory;
    std::vector<Passage> boundary;
    bool managed=false;
    int64_t sampleBudget=0;
    PhysicalView view,entry;
    uint64_t entrySignal=0;
    WaitingSample waiting;
    ControlledMotion controlled;
};

// Validate a whole publication before replacing live rules. Failure leaves the
// caller's output untouched; duplicate signals are ambiguous and rejected.
inline bool prepareRules(const NimbySignalDrivingRule* input,uint32_t count,
                         uint32_t lease,uint32_t options,
                         std::vector<NimbySignalDrivingRule>& output) {
    if((options&~NIMBY_DRIVING_MAXIMUM_LINE_SPEED)||count>32768||
       (!input&&count)||lease<100||lease>5000)return false;
    std::vector<NimbySignalDrivingRule> next;
    if(count)next.assign(input,input+count);
    for(const auto& r:next)if(!valid(r))return false;
    std::sort(next.begin(),next.end(),[](const auto& a,const auto& b){return a.signal<b.signal;});
    for(size_t i=1;i<next.size();++i)if(next[i-1].signal==next[i].signal)return false;
    output=std::move(next);
    return true;
}

struct Integration {
    double ceiling,braking,distance,target;
    bool observed=false,restricted=false;
    std::vector<Passage> encountered;
};

// Plan only: the native integrator remains responsible for actual motion.
// Braking uses the loaded mass and native scale; restrictions cannot increase
// a native ceiling. Publication freshness and physical visibility are explicit.
// The adapter validates positive finite mass, service braking and brakeFactor,
// and finite nonnegative extraMass before invoking this planner.
inline Integration prepareIntegration(Train& state,std::span<const NimbySignalDrivingRule> rules,
    double head,double trainLength,double materialCeiling,double serviceBraking,
    double emptyMass,double extraMass,double brakeFactor,bool fresh,uint64_t now,
    double ceiling,double braking,double distance,double target) {
    Integration result{ceiling,braking,distance,target,false,false,{}};
    if(!state.managed&&state.memory.stops.empty()&&state.memory.held.empty()&&
       state.boundary.empty()&&!state.memory.sight&&!state.controlled.instruction.revision)return result;
    result.observed=true;
    const double physicalBraking=std::min(braking*brakeFactor,
        serviceBraking*0.8*emptyMass/(emptyMass+extraMass));
    const auto clearance=state.memory.sight?state.view.at(head,now):
        state.entrySignal==state.memory.stopped.signal?state.entry.at(head,now):SightClearance{};
    const auto proposed=plan(state.memory,state.ahead,rules,head,trainLength,materialCeiling,
                             physicalBraking,fresh,clearance);
    if(state.memory.sight) {
        result.restricted=true;
        const double stopDistance=fresh&&clearance.verified?std::max(0.0,clearance.distanceM-5):0;
        if(!std::isfinite(distance)||distance<0||stopDistance<=distance){result.distance=stopDistance;result.target=0;}
    }
    if(proposed.active){result.ceiling=std::min(ceiling,proposed.ceiling);result.braking=physicalBraking/brakeFactor;}
    if(state.controlled.instruction.revision) {
        const auto view=state.view.at(head,now);
        const auto limit=state.controlled.ceiling(head,trainLength,state.ahead,physicalBraking,view,materialCeiling);
        if(!state.controlled.completed){
            result.ceiling=std::min(result.ceiling,limit);result.braking=physicalBraking/brakeFactor;
            if(state.controlled.instruction.mode==1){
                result.restricted=true;
                const double stopDistance=view.verified?std::max(0.0,view.distanceM-5):0;
                if(!std::isfinite(result.distance)||result.distance<0||stopDistance<=result.distance){result.distance=stopDistance;result.target=0;}
            }
        }
    }
    if(fresh){result.encountered=passages(state.ahead,rules);appendBoundary(result.encountered,state.boundary,rules,head);}
    return result;
}

// Commit only the displacement returned by the native integrator. A waiting
// train's native 0.01 m/s floor is not evidence of movement; elapsed simulation
// time with zero displacement provides the measured stop proof instead.
inline void commitIntegration(Train& state,std::span<const Passage> encountered,
    std::span<const NimbySignalDrivingRule> rules,double before,double after,
    double speed,int64_t elapsed,int64_t budget,bool readable,bool fresh) {
    crossed(state.memory,encountered,before,after);
    state.boundary=atBoundary(encountered,after);
    auto stoppedAhead=state.ahead;
    for(const auto& p:encountered)
        if(std::none_of(stoppedAhead.begin(),stoppedAhead.end(),[&](const auto& row){return row.signal==p.source.signal;}))
            stoppedAhead.push_back(p.source);
    if(readable&&elapsed>0&&elapsed<=budget) {
        const double measured=(after==before&&speed>=0&&speed<=0.010001)?0:speed;
        observeStop(state.memory,stoppedAhead,rules,before,after,measured,elapsed,fresh);
    }else state.memory.stopped={};
}
}
