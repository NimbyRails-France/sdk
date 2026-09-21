#pragma once
#include <nimby/block_observation.hpp>
#include <nimby/block_topology.hpp>

namespace nimby {

// Observed facts only. The SDK does not choose a national indication.
struct LiveSignalState {
    Id id = 0;
    Id nextSignal = 0;
    BlockOccupancy occupation = BlockOccupancy::Unknown;
    bool boundaryKnown = false;
    bool fresh = false;
    SignalSettings settings;
    std::vector<Id> trains;
    bool trainListComplete=false;
};

// Select this mod's signals by their loaded texture catalogue. Native path
// signals delimit blocks even when they belong to another mod; their indication
// remains unknown to the consumer unless separately established.
inline std::vector<LiveSignalState> observeSignals(const Snapshot& snapshot,
    std::string_view textureSet, std::size_t maxSignals = 512, Milliseconds maxAge = Milliseconds{1000},
    std::string_view settingsType = {}) {
    if (textureSet.empty() || !maxSignals || maxSignals > 4096 || maxAge.count() < 0)
        throw std::invalid_argument("Invalid signal observation scope");
    std::vector<Signal> boundaries;
    std::vector<Id> selected;
    boundaries.reserve(snapshot.getAllSignals().size());
    selected.reserve(std::min(maxSignals, snapshot.getAllSignalTextures().size()));
    for (const auto& signal : snapshot.getAllSignals()) {
        if (signal.getKind() != NIMBY_SIGNAL_PATH) continue;
        boundaries.push_back(signal);
    }
    // Utiliser les descriptions du snapshot sans recopier leurs chemins complets.
    for (const auto& texture : snapshot.getAllSignalTextures()) {
        const auto name = texture.getTexturesIdView();
        if (name && *name == textureSet) {
            const auto signal = snapshot.getSignalById(texture.getSignalId());
            if (!signal || signal->getKind() != NIMBY_SIGNAL_PATH) continue;
            if (selected.size() == maxSignals) throw std::invalid_argument("Signal observation capacity exceeded");
            selected.push_back(signal->getId());
        }
    }
    if (selected.empty()) return {};
    const BlockTopology topology(boundaries, snapshot.getAllTrackNodes(), snapshot.getAllTrackJunctions());
    const auto reader = observeBlocks(snapshot, maxAge);
    std::vector<LiveSignalState> result;
    result.reserve(selected.size());
    for (const auto id : selected) {
        const auto block = topology.read(id);
        auto observation=block.observe(reader);
        result.push_back({id, block.nextSignal, observation.occupation,
            block.hasBoundary(), !snapshot.isOlderThan(maxAge), snapshot.getSignalSettings(id,settingsType),
            std::move(observation.trains),observation.complete});
    }
    return result;
}

} // namespace nimby
