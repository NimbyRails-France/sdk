#pragma once
#include <nimby/detail/observation_session.hpp>
#include <nimby/detail/automatic_driving.h>
#include <span>
namespace nimby {
// Generic numeric instructions. National aspects and their meaning belong to the mod.
using SignalDrivingRule = NimbySignalDrivingRule;
class AutomaticDriving {
public:
    // Publish a complete set of managed signals from the observation worker.
    // The SDK obtains path distances, train load and simulation timing natively.
    static void publish(std::span<const SignalDrivingRule> rules,Milliseconds lease=Milliseconds{1000},bool maximumLineSpeed=false) {
        if(rules.size()>32768||lease.count()<100||lease.count()>5000)
            throw std::invalid_argument("Invalid driving rules lease or count");
        const auto status=NimbyInternal_PublishDrivingRulesV2(rules.data(),static_cast<uint32_t>(rules.size()),static_cast<uint32_t>(lease.count()),maximumLineSpeed?NIMBY_DRIVING_MAXIMUM_LINE_SPEED:0);
        if(status!=NIMBY_OK)throw std::runtime_error("Native automatic driving unavailable, status="+std::to_string(status));
    }
    static void release() {publish({});}
};
}
