#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <span>
#include <optional>
#include <nimby/detail/track_fraction.hpp>
#include <unordered_map>
#include <vector>

namespace nimby {
enum class BlockOccupancy : std::uint32_t { Unknown, Clear, Occupied };
struct BlockSection { std::uint64_t track; double begin, end; };
// Entry is oriented: a train stopped at this signal has not entered its block.
struct BlockEntry { std::uint64_t track; double fraction; int direction; };
struct TrainFootprint { std::uint64_t train, track; double begin, end; };
// Owned list of full train IDs, unique and sorted. An incomplete empty list
// never proves clearance; incomplete nonempty lists still prove occupation.
struct BlockObservation {
    BlockOccupancy occupation=BlockOccupancy::Unknown;
    std::vector<std::uint64_t> trains;
    bool complete=false;
};

// Observations owned by this reader, indexed once by track for multiple blocks.
// Reservations must never be supplied as footprints. Sections use fractions [0,1].
class BlockReader {
    std::unordered_map<std::uint64_t,std::vector<TrainFootprint>> tracks_;
    bool valid_ = true, complete_ = false;
    static bool interval(double a,double b) {
        return std::isfinite(a) && std::isfinite(b) && a>=0 && a<=b && b<=1;
    }
    bool validSections(std::span<const BlockSection> sections) const {
        if(!valid_||sections.empty())return false;
        return std::all_of(sections.begin(),sections.end(),[](const auto& section){
            return section.track&&interval(section.begin,section.end)&&section.begin!=section.end;
        });
    }
    static bool overlaps(const BlockSection& section,const TrainFootprint& train,const std::optional<BlockEntry>& entry) {
        if(std::max(section.begin,train.begin)>std::min(section.end,train.end))return false;
        if(entry&&section.track==entry->track) {
            // Native signal and footprint endpoints can differ by a few double
            // rounding units. This is numerical tolerance, not a braking margin.
            constexpr double rounding=detail::trackFractionRounding;
            if(entry->direction>0 && train.end<=entry->fraction+rounding)return false;
            if(entry->direction<0 && train.begin>=entry->fraction-rounding)return false;
        }
        return true;
    }
public:
    // complete=true requires evidence of full coverage, not merely an available table.
    // stale observations are unknown even if they contain a previous occupation.
    explicit BlockReader(std::span<const TrainFootprint> observations, bool complete=false, bool fresh=true)
        : valid_(fresh), complete_(complete) {
        for (const auto& train:observations) {
            if (!train.train || !train.track || !interval(train.begin,train.end)) valid_=false;
            tracks_[train.track].push_back(train);
        }
    }
    BlockOccupancy read(std::span<const BlockSection> sections,std::optional<BlockEntry> entry={}) const {
        if (!validSections(sections)) return BlockOccupancy::Unknown;
        for (const auto& section:sections) {
            const auto found=tracks_.find(section.track);
            if (found==tracks_.end()) continue;
            // Exit contact still occupies the upstream block; only entry is excluded.
            for (const auto& train:found->second)
                if (overlaps(section,train,entry))
                    return BlockOccupancy::Occupied;
        }
        return complete_ ? BlockOccupancy::Clear : BlockOccupancy::Unknown;
    }
    BlockObservation inspect(std::span<const BlockSection> sections,std::optional<BlockEntry> entry={}) const {
        if(!validSections(sections))return {};
        BlockObservation result;
        result.complete=complete_;
        for(const auto& section:sections){
            const auto found=tracks_.find(section.track);
            if(found==tracks_.end())continue;
            for(const auto& train:found->second)
                if(overlaps(section,train,entry))
                    result.trains.push_back(train.train);
        }
        std::sort(result.trains.begin(),result.trains.end());
        result.trains.erase(std::unique(result.trains.begin(),result.trains.end()),result.trains.end());
        result.occupation=!result.trains.empty()?BlockOccupancy::Occupied:
            complete_?BlockOccupancy::Clear:BlockOccupancy::Unknown;
        return result;
    }
};
}
