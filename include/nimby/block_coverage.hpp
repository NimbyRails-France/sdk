#pragma once
#include <nimby/blocks.hpp>
#include <optional>
#include <unordered_set>

namespace nimby {

struct TrainPresence {
    std::uint64_t train = 0;
    std::optional<bool> physicallyPresent;
};
struct BlockCoverage {
    bool verified = false;
    std::size_t unknownPresence = 0;
    std::size_t missingFootprints = 0;
    std::size_t unexpectedFootprints = 0;
};

// Additional cross-check for a COMPLETE native occupation collection. This is
// not a way to certify arbitrary/sampled footprints: full-table extraction must
// already be established by the provider. Presence does not reconstruct tails.
inline BlockCoverage checkBlockCoverage(std::span<const TrainPresence> trains,
    std::span<const TrainFootprint> footprints, bool nativeCollectionComplete) {
    BlockCoverage result;
    if (!nativeCollectionComplete) return result;
    std::unordered_set<std::uint64_t> occupiedTrains;
    for (const auto& row : footprints) {
        if (!row.train || !row.track || !std::isfinite(row.begin) || !std::isfinite(row.end)
            || row.begin < 0 || row.end < row.begin || row.end > 1) return result;
        occupiedTrains.insert(row.train);
    }
    std::unordered_set<std::uint64_t> seen;
    for (const auto& train : trains) {
        if (!train.train || !seen.insert(train.train).second) return result;
        const bool hasFootprint = occupiedTrains.erase(train.train) != 0;
        if (!train.physicallyPresent) ++result.unknownPresence;
        else if (*train.physicallyPresent && !hasFootprint) ++result.missingFootprints;
        else if (!*train.physicallyPresent && hasFootprint) ++result.unexpectedFootprints;
    }
    result.unexpectedFootprints += occupiedTrains.size();
    result.verified = !result.unknownPresence && !result.missingFootprints && !result.unexpectedFootprints;
    return result;
}

} // namespace nimby
