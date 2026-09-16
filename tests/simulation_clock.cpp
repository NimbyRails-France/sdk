#include "engine/simulation_clock.h"
#include <cstdio>
#include <cstring>
#include <limits>
using namespace nimby::engine;
struct Memory { SimulationClock clock{1781123300,542534653};bool fail=false,changing=false; };
bool read(void* ctx,uint64_t address,void* out,size_t size) {
    auto& m=*static_cast<Memory*>(ctx);
    if(m.fail || address!=0x100020 || size!=sizeof m.clock)return false;
    std::memcpy(out,&m.clock,size);if(m.changing)++m.clock.ticks;return true;
}
#define CHECK(x) do { if(!(x)){std::printf("FAIL %d: %s\n",__LINE__,#x);return 1;} } while(false)
int main() {
    Memory m;SimulationClock before{},after{};
    CHECK(read_simulation_clock(read,&m,0x100000,before));
    CHECK(before.epoch_seconds+before.ticks/100==1786548646);
    CHECK(rebase_clock(before,-946771200,after)); // 1940-01-01 00:00:00 UTC.
    CHECK(after.ticks==before.ticks && after.epoch_seconds+after.ticks/100==-946771200);
    CHECK(after.ticks%100==53); // Calendar edit must preserve timer phase.
    m.clock=after;CHECK(read_simulation_clock(read,&m,0x100000,before));
    CHECK(before==after);
    CHECK(!rebase_clock(before,std::numeric_limits<int64_t>::max(),after));
    CHECK(!rebase_clock(before,calendar_min-1,after));
    m.fail=true;CHECK(!read_simulation_clock(read,&m,0x100000,after));CHECK(after==SimulationClock{});
    m.fail=false;m.changing=true;CHECK(!read_simulation_clock(read,&m,0x100000,after));
    m.changing=false;m.clock.ticks=-1;CHECK(!read_simulation_clock(read,&m,0x100000,after));
    m.clock={calendar_max,100};CHECK(!read_simulation_clock(read,&m,0x100000,after));
    m.clock={calendar_min,0};CHECK(read_simulation_clock(read,&m,0x100000,after));
    std::array<unsigned char,0x638> motion{};
    motion[0x4d0]=motion[0x5d0]=1;
    auto put=[&](size_t at,int64_t value){std::memcpy(motion.data()+at,&value,8);};
    put(0x4b8,1786572360);put(0x5c0,1786546740);
    const int64_t delta=-932615164LL-1781123300LL;
    std::vector<CalendarWrite> edits;
    CHECK(plan_motion_calendar(motion.data(),0x200000,delta,edits));
    CHECK(edits.size()==2);
    CHECK(edits[0].after-(-932615164LL)==1786572360LL-1781123300LL);
    CHECK(edits[1].after==1786546740LL+delta);
    // A subsequent arrival derives its departure from the run's calendar start.
    // Old code left that start in 2026, producing an 86-year wait in 1940.
    const int64_t stopOffset=25620;
    CHECK((edits[1].after+stopOffset-(-932615164LL))*1000000==5449060000000LL);
    put(0x630,1786570065);edits.clear();
    CHECK(plan_motion_calendar(motion.data(),0x200000,delta,edits)&&edits.size()==3);
    CHECK(edits.back().after==1786570065LL+delta);
    put(0x630,0);
    // Unengaged optional bytes are stale storage, not calendar values.
    motion[0x4d0]=motion[0x5d0]=0;put(0x4b8,INT64_MAX);edits.clear();
    CHECK(plan_motion_calendar(motion.data(),0x200000,delta,edits)&&edits.empty());
    motion[0x4d0]=2;CHECK(!plan_motion_calendar(motion.data(),0x200000,delta,edits));
    motion[0x4d0]=1;CHECK(!plan_motion_calendar(motion.data(),0x200000,delta,edits));
    std::puts("Simulation clock: historical dates, stable reads, bounds and preserved elapsed time passed.");
}
