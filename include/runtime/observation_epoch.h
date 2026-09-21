#pragma once
#include "engine/versioning.h"
#include <optional>
#include <stdexcept>
namespace nimby::runtime {
// Runtime transition detector, not a persistent save-file identity. Heap roots
// are compared only to detect changes; they never leave the SDK or become keys.
class ObservationEpoch {
public:
    void unavailable(){previous_.reset();ticks_.reset();}
    uint64_t observe(const engine::VersioningObservation& value,std::optional<int64_t> ticks) {
        if(!previous_||previous_->state!=value.state||previous_->value!=value.value||
           previous_->history!=value.history||(ticks&&ticks_&&*ticks<*ticks_)){
            if(generation_==UINT64_MAX)throw std::length_error("Observation generation exhausted");
            ++generation_;
            ticks_.reset();
        }
        previous_=value;if(ticks)ticks_=ticks;return generation_;
    }
private:
    uint64_t generation_=0;
    std::optional<engine::VersioningObservation> previous_;
    std::optional<int64_t> ticks_;
};
}
