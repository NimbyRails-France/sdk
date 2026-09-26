#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace nimby::engine {
// One explicit experiment, bounded by both wall time and simulated ticks.
// No shared-memory fields or atomics here: the platform publishes transitions.
struct DrivingCommand {
    uint64_t train=0,expiry=0;
    double ceiling=0,brakeUse=0.8;
    int64_t remaining=0,sampleBudget=0;
    bool accept(uint64_t id,uint64_t now,uint64_t expires,double cap,double ratio,int64_t durationMs) {
        if(!std::isfinite(cap)||cap<=0||!std::isfinite(ratio)||ratio<=0||ratio>1||
           durationMs<=0||durationMs>120000||expires<=now||expires-now>30000)return false;
        *this={id,expires,cap,ratio,(durationMs+9)/10,0};return true;
    }
    bool active(uint64_t id,uint64_t now,int64_t budget) const {
        return train==id&&now<expiry&&remaining>0&&budget>0&&budget<=120000;
    }
    bool parameters(double extraMass,double emptyMass,double service,double nativeCeiling,
                    double nativeBraking,double& effectiveCeiling,double& effectiveBraking) const {
        if(!std::isfinite(extraMass)||extraMass<0||!std::isfinite(emptyMass)||emptyMass<=0||
           !std::isfinite(service)||service<=0)return false;
        effectiveCeiling=std::min(nativeCeiling,ceiling);
        effectiveBraking=std::min(nativeBraking,service*brakeUse*emptyMass/(emptyMass+extraMass));
        return true;
    }
    bool consume(int64_t used,int64_t budget) {
        if(used<0||used>budget)return false;
        remaining-=used;sampleBudget-=used;return true;
    }
};
}
