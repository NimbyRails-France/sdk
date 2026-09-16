#pragma once
#include "engine/live_state.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>
namespace nimby::engine {
// Unix seconds for 0001-01-01 through 9999-12-31; negative epochs are valid.
inline constexpr int64_t calendar_min = -62135596800LL;
inline constexpr int64_t calendar_max = 253402300799LL;
struct SimulationClock {
    int64_t epoch_seconds{}, ticks{}; // One tick = 10 ms.
    bool operator==(const SimulationClock&) const = default;
};
inline bool valid_clock(const SimulationClock& clock) noexcept {
    if(clock.epoch_seconds < calendar_min || clock.epoch_seconds > calendar_max ||
       clock.ticks < 0 || clock.ticks >= 10000000000000LL) return false;
    return clock.epoch_seconds + clock.ticks / 100 <= calendar_max;
}
inline bool read_simulation_clock(ReadMemory read, void* context, uint64_t simulation,
                                  SimulationClock& out) noexcept {
    out = {};
    if(!read || simulation < 0x10000 || simulation > 0x7fffffff0000ULL) return false;
    for(int attempt=0;attempt<3;++attempt) {
        SimulationClock first{},second{};
        if(!read(context,simulation+0x20,&first,sizeof first) ||
           !read(context,simulation+0x20,&second,sizeof second)) return false;
        if(first==second && valid_clock(first)) { out=first; return true; }
    }
    return false;
}
inline bool rebase_clock(const SimulationClock& before, int64_t calendar_seconds,
                         SimulationClock& after) noexcept {
    after = {};
    if(!valid_clock(before) || calendar_seconds < calendar_min || calendar_seconds > calendar_max) return false;
    const SimulationClock candidate{calendar_seconds-before.ticks/100,before.ticks};
    if(!valid_clock(candidate)) return false;
    after=candidate;return true;
}
struct CalendarWrite { uint64_t address{};int64_t before{},after{}; };
// Called only while the remote simulation is suspended. All edits are planned
// before any write; inactive optional storage must never be interpreted.
inline bool plan_motion_calendar(const unsigned char* motion,uint64_t address,
                                  int64_t delta,std::vector<CalendarWrite>& edits) {
    if(motion[0x4d0]>1||motion[0x5d0]>1)return false;
    for(const auto [flag,offset]:{std::pair{0x4d0,0x4b8},std::pair{0x5d0,0x5c0}}) {
        if(!motion[flag])continue;
        int64_t value{};std::memcpy(&value,motion+offset,sizeof value);
        if(value<calendar_min||value>calendar_max)return false;
        const auto shifted=value+delta;
        if(shifted<calendar_min||shifted>calendar_max)return false;
        edits.push_back({address+offset,value,shifted});
    }
    // PaxUpdater::lost_check uses this absolute calendar timestamp (+0x630).
    // Zero is the native constructor's uninitialized value.
    int64_t checked{};std::memcpy(&checked,motion+0x630,8);
    if(checked!=0){
        if(checked<calendar_min||checked>calendar_max||checked+delta<calendar_min||checked+delta>calendar_max)return false;
        edits.push_back({address+0x630,checked,checked+delta});
    }
    return true;
}
bool plan_simulation_calendar(ReadMemory read,void* context,uint64_t simulation,
                              int64_t delta,std::vector<CalendarWrite>& edits);
}
