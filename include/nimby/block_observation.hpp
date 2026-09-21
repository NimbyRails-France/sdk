#pragma once
#include <nimby/blocks.hpp>
#include <nimby/block_coverage.hpp>
#include <nimby/client.hpp>

namespace nimby {
// The provider copies the whole native CarOccupancy table (primary entries and
// overflow chains), validating buffers, roots and referenced IDs. Independently
// cross-check every train's physical presence before allowing absence -> Clear.
// A reservation, a head point, or an available but inconsistent table is not proof.
inline BlockCoverage observeBlockCoverage(const Snapshot& snapshot) {
    const auto occupations=snapshot.getAllOccupations();
    if(!occupations) return {};
    std::vector<TrainFootprint> footprints;
    for(const auto& row:*occupations)
        footprints.push_back({row.getTrainId(),row.getTrackId(),row.getBeginFraction(),row.getEndFraction()});
    std::vector<TrainPresence> presence;
    for(const auto& train:snapshot.getAllTrains()) {
        std::optional<bool> physicallyPresent;
        const auto service=snapshot.getTrainServiceById(train.getId());
        if(service) {
            const auto onNetwork=service->isOnNetwork(), hidden=service->isHidden();
            if(onNetwork && hidden) physicallyPresent=*onNetwork && !*hidden;
        }
        presence.push_back({train.getId(),physicallyPresent});
    }
    return checkBlockCoverage(presence,footprints,true);
}

// Build a reusable reader from one snapshot. Geometric overlap and freshness are
// separate from coverage; a missing footprint only means Clear after the check.
inline BlockReader observeBlocks(const Snapshot& snapshot, Milliseconds maxAge=Milliseconds{1000}) {
    if (maxAge.count()<0) throw std::invalid_argument("Negative observation age");
    const auto occupations=snapshot.getAllOccupations();
    std::vector<TrainFootprint> footprints;
    if (occupations) for(const auto& row:*occupations)
        footprints.push_back({row.getTrainId(),row.getTrackId(),row.getBeginFraction(),row.getEndFraction()});
    return BlockReader(footprints,observeBlockCoverage(snapshot).verified,
        occupations.has_value()&&!snapshot.isOlderThan(maxAge));
}
}
