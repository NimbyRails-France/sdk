#pragma once
#include <engine/automatic_driving.h>

namespace nimby::engine::automatic {
inline bool validConstraint(const NimbyTrainConstraint& r) {
    return r.train>>48==5&&(!r.exit_signal||r.exit_signal>>48==8)&&r.revision&&
        std::isfinite(r.speed_mps)&&r.speed_mps>=0&&r.mode<=2&&(r.flags&~1u)==0&&
        (r.mode==2?r.speed_mps==0:r.speed_mps>0);
}
struct ControlledMotion {
    NimbyTrainConstraint instruction{};
    uint64_t exitSignal=0;
    double exitPosition=0,lastHead=0;
    bool bound=false,completed=false,cancelled=false;
    void accept(const NimbyTrainConstraint& next,double head) {
        if(instruction.revision!=next.revision){*this={};instruction=next;lastHead=head;}
    }
    // The command belongs to this route instance. A backward reset must not
    // silently attach the same command to a new route or reverse movement.
    void observe(double head,double length,std::span<const Ahead> ahead) {
        if(head+0.01<lastHead){completed=cancelled=true;return;}lastHead=head;
        if(!bound)for(const auto& signal:ahead) {
            if(signal.position<=head+1e-6)continue;
            if(instruction.exit_signal&&signal.signal!=instruction.exit_signal)continue;
            exitSignal=signal.signal;exitPosition=signal.position;bound=true;break;
        }
        if(bound&&(instruction.flags&1u?head-length:head)>exitPosition+1e-6)completed=true;
    }
    uint32_t state() const {return cancelled?4u:completed?3u:bound?2u:1u;}
    double ceiling(double head,double length,std::span<const Ahead> ahead,double braking,
                   const SightClearance& view,double material) {
        observe(head,length,ahead);
        if(completed)return material;
        if(instruction.mode==2)return 0;
        // An unavailable exit is not guessed. Hold until the native route
        // supplies the requested boundary, rather than run an unbounded mode.
        if(!bound)return 0;
        if(instruction.mode==1)return sightCeiling(view,instruction.speed_mps,braking);
        return std::min(material,instruction.speed_mps);
    }
};
}
