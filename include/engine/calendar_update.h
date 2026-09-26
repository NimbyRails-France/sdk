#pragma once
#include "engine/simulation_clock.h"
#include <nimby/detail/observation.h>
#include <span>

namespace nimby::engine {
// Common calendar transaction planning. The platform must first stop the target
// safely and verify its process identity. No writes occur during preparation.
inline uint32_t prepare_calendar_update(ReadMemory read,void* context,
    const LiveState& state,int64_t utc,std::vector<CalendarWrite>& edits,
    SimulationClock& after) {
    edits.clear(); after={};
    SimulationClock before{},copyBefore{},copyAfter{};
    if(!read_simulation_clock(read,context,state.simulation,before) ||
       !read_simulation_clock(read,context,state.copy,copyBefore) ||
       before.epoch_seconds!=copyBefore.epoch_seconds)return NIMBY_DATA_UNAVAILABLE;
    if(!rebase_clock(before,utc,after))return NIMBY_INVALID_ARGUMENT;
    copyAfter={after.epoch_seconds,copyBefore.ticks};
    if(!valid_clock(copyAfter))return NIMBY_INVALID_ARGUMENT;
    const auto delta=after.epoch_seconds-before.epoch_seconds;
    if(!plan_simulation_calendar(read,context,state.simulation,delta,edits) ||
       !plan_simulation_calendar(read,context,state.copy,delta,edits))return NIMBY_DATA_UNAVAILABLE;
    edits.push_back({state.simulation+0x20,before.epoch_seconds,after.epoch_seconds});
    edits.push_back({state.copy+0x20,copyBefore.epoch_seconds,copyAfter.epoch_seconds});
    return NIMBY_OK;
}

// writeVerified(address,value) must not throw, and must read back each write.
// A failed write may have changed a prefix: rollback includes that write, then
// all earlier edits in reverse order. Rollback is best effort, never success:
// callers must re-read the clock after CLOCK_WRITE_FAILED, and always resume.
template<class WriteVerified>
uint32_t apply_calendar_update(std::span<const CalendarWrite> edits,WriteVerified writeVerified) noexcept {
    for(size_t i=0;i<edits.size();++i) {
        if(writeVerified(edits[i].address,edits[i].after))continue;
        for(size_t undo=i+1;undo>0;--undo)
            writeVerified(edits[undo-1].address,edits[undo-1].before);
        return NIMBY_CLOCK_WRITE_FAILED;
    }
    return NIMBY_OK;
}
}
