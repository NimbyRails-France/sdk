#pragma once
#include <nimby/client.hpp>
#include <unordered_map>
#include <nimby/signal_settings_store.hpp>

namespace nimby::detail {
// Project one successful SDK snapshot, without additional reads from the game.
// Every signal needs exactly one resolved texture set. A partial catalog is
// unavailable, never an empty catalog that would delete stored settings.
inline std::optional<std::vector<SignalSettingsStore::Signal>> signalSettingsCatalog(
        std::span<const Signal> signals,std::span<const SignalTexture> textures) {
    if(signals.size()>1000000||signals.size()!=textures.size())return std::nullopt;
    std::unordered_map<uint64_t,std::string_view> remaining;
    remaining.reserve(textures.size());
    for(const auto& texture:textures){
        const auto name=texture.getTexturesIdView();
        if(!name||texture.getSignalId()>>48!=8)return std::nullopt;
        if(name->empty()||name->size()>256||!remaining.emplace(texture.getSignalId(),*name).second)
            return std::nullopt;
    }
    std::vector<SignalSettingsStore::Signal> result;result.reserve(signals.size());
    for(const auto& signal:signals){
        const auto found=remaining.find(signal.getId());
        if(signal.getId()>>48!=8||found==remaining.end())return std::nullopt;
        result.push_back({signal.getId(),std::string{found->second}});
        remaining.erase(found);
    }
    return result;
}
inline std::optional<std::vector<SignalSettingsStore::Signal>> signalSettingsCatalog(const Snapshot& snapshot) {
    return signalSettingsCatalog(snapshot.getAllSignals(),snapshot.getAllSignalTextures());
}
}
