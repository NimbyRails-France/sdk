#include "engine/calendar_update.h"
#include <cassert>
#include <map>
#include <vector>

int main() {
    using namespace nimby::engine;
    const std::vector<CalendarWrite> edits{{1,10,100},{2,20,200},{3,30,300}};
    for(size_t failing=0;failing<edits.size();++failing) {
        std::map<uint64_t,int64_t> values{{1,10},{2,20},{3,30}};
        std::vector<uint64_t> writes;
        bool failed=false;
        const auto result=apply_calendar_update(edits,[&](uint64_t address,int64_t value) noexcept {
            writes.push_back(address);
            values[address]=value; // Simulate a partial/unchecked write on failure.
            if(!failed && address==edits[failing].address){failed=true;return false;}
            return true;
        });
        assert(result==NIMBY_CLOCK_WRITE_FAILED);
        assert((values==std::map<uint64_t,int64_t>{{1,10},{2,20},{3,30}}));
        assert(writes.size()==2*(failing+1));
        for(size_t i=0;i<=failing;++i)assert(writes[failing+1+i]==edits[failing-i].address);
    }
    std::map<uint64_t,int64_t> values;
    assert(apply_calendar_update(edits,[&](uint64_t address,int64_t value) noexcept {
        values[address]=value;return true;
    })==NIMBY_OK);
    assert((values==std::map<uint64_t,int64_t>{{1,100},{2,200},{3,300}}));
    // Failure of rollback must not be reported as success or stop later attempts.
    size_t calls=0;
    assert(apply_calendar_update(edits,[&](uint64_t,int64_t) noexcept {++calls;return false;})==NIMBY_CLOCK_WRITE_FAILED);
    assert(calls==2);
}
