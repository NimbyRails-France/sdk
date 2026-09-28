#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <map>
#include <mutex>
#include <set>
#include <string>

namespace nimby::engine {
// Defaults apply only to the game's fresh construction result, never to a
// placed signal or an existing signal copied by a repetition tool.
inline bool leftConstruction(std::array<uint8_t,0x64>& signal,uint64_t sourceId) noexcept {
    uint64_t id{},track{};
    std::memcpy(&id,signal.data(),8);std::memcpy(&track,signal.data()+0x40,8);
    const auto direction=static_cast<int8_t>(signal[0x50]);
    if(sourceId||id||!track||(direction!=-1&&direction!=1))return false;
    // The native lateral offset uses the track's positive screen normal
    // (-dy,+dx), i.e. its right side. Its orientation byte handles later flips.
    const int32_t side=-direction;
    std::memcpy(signal.data()+0x58,&side,4);
    return true;
}
class ConstructionDefaultsRegistry {
    std::mutex mutex_;
    std::map<std::pair<int,std::string>,std::set<uint64_t>> owners_;
public:
    void replace(int kind,const std::string& id,std::set<uint64_t> hashes){
        std::lock_guard lock(mutex_);
        if(hashes.empty())owners_.erase({kind,id});else owners_.insert_or_assign({kind,id},std::move(hashes));
    }
    bool left(uint64_t hash){
        std::lock_guard lock(mutex_);size_t matches=0;
        for(const auto& [owner,hashes]:owners_)matches+=hashes.contains(hash);
        return matches==1; // Ambiguous ownership keeps the native default.
    }
};
}
