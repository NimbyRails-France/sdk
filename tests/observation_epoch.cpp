#include "runtime/observation_epoch.h"
#include <cstdio>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"Failed line %d\n",__LINE__);return 1;}}while(false)
int main(){
    nimby::runtime::ObservationEpoch tracker;
    nimby::engine::VersioningObservation world;
    world.state={1,2,3,4,5};world.value[0]=42;
    auto first=tracker.observe(world,100);CHECK(first!=0);
    CHECK(tracker.observe(world,200)==first);
    CHECK(tracker.observe(world,std::nullopt)==first);
    auto rewind=tracker.observe(world,150);CHECK(rewind!=first);
    CHECK(tracker.observe(world,151)==rewind);
    tracker.unavailable();auto reload=tracker.observe(world,152);CHECK(reload!=rewind);
    world.state.database=6;auto roots=tracker.observe(world,153);CHECK(roots!=reload);
    world.value[0]=43;auto other=tracker.observe(world,154);CHECK(other!=roots);
    world.history.push_back(world.value);CHECK(tracker.observe(world,155)!=other);
    std::puts("PASS observed game generations: unload, roots, world, history, clock rollback");
}
