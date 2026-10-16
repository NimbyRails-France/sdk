#pragma once
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace nimby::kotlin::observation_trains {
inline constexpr std::size_t maximumBlockTrains=4096,maximumBatchTrains=131072;

// Transport only: these are already observed full identities, never decoded
// native addresses or reconstructed positions. A rejected list is not trimmed
// into a supposedly complete observation.
inline void validateBlockTrains(std::span<const std::uint64_t> trains) {
    if(trains.size()>maximumBlockTrains)throw std::invalid_argument("Too many observed block trains");
    for(const auto train:trains)
        if(train>>48!=5||!(train&UINT64_C(0x0000ffffffffffff)))
            throw std::invalid_argument("Invalid observed block train identity");
}

struct ObservationTrainsBatch {
    std::vector<int> offsets{0},completeness;
    std::vector<std::int64_t> trains;

    explicit ObservationTrainsBatch(std::size_t signals) {
        offsets.reserve(signals+1);completeness.reserve(signals);
    }
    void append(std::span<const std::uint64_t> observed,bool complete) {
        validateBlockTrains(observed);
        if(observed.size()>maximumBatchTrains-trains.size())
            throw std::invalid_argument("Too many observed trains in signal batch");
        trains.insert(trains.end(),observed.begin(),observed.end());
        offsets.push_back(static_cast<int>(trains.size()));completeness.push_back(complete?1:0);
    }
};
} // namespace nimby::kotlin::observation_trains
