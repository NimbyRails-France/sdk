#pragma once
#include <nimby/block_observation.hpp>
#include <nimby/block_topology.hpp>
#include <set>
#include <nimby/signal_approach.hpp>

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
    std::string textureSet;
    Id approachingTrain=0;
};
struct SignalApproachScope {std::string_view textureSet;size_t blocks=1;};

// Select this mod's signals by their loaded texture catalogue. Native path
// signals delimit blocks even when they belong to another mod; their indication
// remains unknown to the consumer unless separately established.
inline std::vector<LiveSignalState> observeSignals(const Snapshot& snapshot,
    std::span<const std::string_view> textureSets, std::size_t maxSignals = 512, Milliseconds maxAge = Milliseconds{1000},
    std::string_view settingsType = {},bool includeApproaches=false,std::span<const SignalApproachScope> approachScopes={}) {
    if (textureSets.empty() || textureSets.size()>16 || !maxSignals || maxSignals > 4096 || maxAge.count() < 0)
        throw std::invalid_argument("Invalid signal observation scope");
    std::set<std::string_view> catalogues;
    for(const auto name:textureSets)
        if(name.empty()||!catalogues.insert(name).second)throw std::invalid_argument("Duplicate or empty signal catalogue");
    std::vector<Signal> boundaries;
    std::vector<std::pair<Id,std::string>> selected;
    boundaries.reserve(snapshot.getAllSignals().size());
    selected.reserve(std::min(maxSignals, snapshot.getAllSignalTextures().size()));
    for (const auto& signal : snapshot.getAllSignals()) {
        if (signal.getKind() != NIMBY_SIGNAL_PATH) continue;
        boundaries.push_back(signal);
    }
    // Utiliser les descriptions du snapshot sans recopier leurs chemins complets.
    for (const auto& texture : snapshot.getAllSignalTextures()) {
        const auto name = texture.getTexturesIdView();
        if (name && catalogues.contains(*name)) {
            const auto signal = snapshot.getSignalById(texture.getSignalId());
            if (!signal || signal->getKind() != NIMBY_SIGNAL_PATH) continue;
            if (selected.size() == maxSignals) throw std::invalid_argument("Signal observation capacity exceeded");
            selected.emplace_back(signal->getId(),std::string(*name));
        }
    }
    if (selected.empty()) return {};
    const BlockTopology topology(boundaries, snapshot.getAllTrackNodes(), snapshot.getAllTrackJunctions());
    const auto reader = observeBlocks(snapshot, maxAge);
    size_t maxApproach=includeApproaches?1:0;
    for(const auto& scope:approachScopes){
        if(!scope.blocks||scope.blocks>16||!catalogues.contains(scope.textureSet))throw std::invalid_argument("Invalid approach scope");
        maxApproach=std::max(maxApproach,scope.blocks);
    }
    const auto approaches=maxApproach?observeSignalApproaches(snapshot,boundaries,maxAge,maxApproach):std::map<Id,SignalApproach>{};
    std::vector<LiveSignalState> result;
    result.reserve(selected.size());
    for (const auto& [id,catalogue] : selected) {
        const auto block = topology.read(id);
        auto observation=block.observe(reader);
        size_t approachLimit=includeApproaches?1:0;
        for(const auto& scope:approachScopes)if(scope.textureSet==catalogue)approachLimit=scope.blocks;
        const auto approach=approaches.find(id);
        const auto train=approach!=approaches.end()&&approach->second.blocks<=approachLimit?approach->second.train:0;
        result.push_back({id, block.nextSignal, observation.occupation,
            block.hasBoundary(), !snapshot.isOlderThan(maxAge), snapshot.getSignalSettings(id,settingsType),
            std::move(observation.trains),observation.complete,catalogue,
            train});
    }
    return result;
}

// Single-type consumers keep their source and behaviour. A multi-type mod
// supplies all its catalogues in ONE capture/network, so cross-type links are
// resolved together rather than treating another of its types as unknown.
inline std::vector<LiveSignalState> observeSignals(const Snapshot& snapshot,
    std::string_view textureSet, std::size_t maxSignals = 512, Milliseconds maxAge = Milliseconds{1000},
    std::string_view settingsType = {}) {
    return observeSignals(snapshot,std::span<const std::string_view>(&textureSet,1),maxSignals,maxAge,settingsType);
}

} // namespace nimby
