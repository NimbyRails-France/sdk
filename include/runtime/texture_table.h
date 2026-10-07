#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>
namespace nimby::texture_bridge {
// Owned by the runtime, never placed in shared memory. Workers publish immutable
// versions; a render callback pins a version only while copying one entry.
struct Command {
    uint64_t signal=0,set_hash=0,expires=0;
    uint32_t index=0;
    uint32_t alternate_index=0, half_period_ms=0;
    uint64_t owner=0;
};
// Select against simulation time, never polling/wall time. A paused simulation
// therefore keeps the same phase even while frames continue to be drawn.
inline uint32_t frame_index(const Command& command, uint64_t simulation_ms) noexcept {
    return command.half_period_ms && (simulation_ms/command.half_period_ms)%2
        ? command.alternate_index : command.index;
}
// Sorted by full signal ID: bounded binary search in the render hook.
struct Table {
    std::vector<Command> entries;
    size_t count() const noexcept { return entries.size(); }
    size_t lower(uint64_t id) const noexcept {
        return std::lower_bound(entries.begin(),entries.end(),id,
            [](const Command& entry,uint64_t key){return entry.signal<key;})-entries.begin();
    }
    void prune(uint64_t now) noexcept {
        entries.erase(std::remove_if(entries.begin(),entries.end(),
            [now](const Command& entry){return entry.expires<=now;}),entries.end());
    }
    bool put(Command command) {
        const auto at=lower(command.signal);
        if(at<count()&&entries[at].signal==command.signal)entries[at]=command;
        else entries.insert(entries.begin()+at,command);
        return true;
    }
    bool erase(uint64_t id) noexcept {
        const auto at=lower(id);if(at==count()||entries[at].signal!=id)return false;
        entries.erase(entries.begin()+at);return true;
    }
};
}
