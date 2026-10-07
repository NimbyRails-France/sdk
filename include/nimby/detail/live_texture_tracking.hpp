#pragma once
#include <algorithm>
#include <cstdint>
#include <span>
#include <unordered_set>
#include <vector>

namespace nimby::detail {
inline constexpr size_t liveTextureCapacity=4096;

// Keep failed/uncertain publications until cleanup succeeds. Admission is
// bounded independently from the number of different IDs seen over time.
inline bool trackLiveTexture(uint64_t signal,std::vector<uint64_t>& owned,std::unordered_set<uint64_t>& ids) {
    if(ids.contains(signal))return true;
    if(owned.size()>=liveTextureCapacity)return false;
    ids.insert(signal);owned.push_back(signal);return true;
}

// A previous runtime can leave more than one batch pending after ID rotation.
// Each successful batch is retired independently; one failure must not make
// later batches unreachable or forget IDs whose cleanup outcome is uncertain.
template<class Required,class Restore,class Retire>
void restoreTrackedTextures(std::vector<uint64_t>& owned,Required required,Restore restore,Retire retire) {
    const auto first=std::find_if(owned.begin(),owned.end(),required);
    if(first==owned.end())return;
    std::vector<uint64_t> kept;kept.reserve(owned.size());kept.insert(kept.end(),owned.begin(),first);
    std::vector<uint64_t> batch;batch.reserve(std::min(owned.size(),liveTextureCapacity));
    auto flush=[&] {
        if(batch.empty())return;
        bool restored=false;
        try {restore(std::span<const uint64_t>{batch});restored=true;}catch(...){}
        if(restored)for(const auto id:batch)retire(id);
        else kept.insert(kept.end(),batch.begin(),batch.end());
        batch.clear();
    };
    for(auto at=first;at!=owned.end();++at){
        if(required(*at)){batch.push_back(*at);if(batch.size()==liveTextureCapacity)flush();}
        else kept.push_back(*at);
    }
    flush();owned.swap(kept);
}
}
