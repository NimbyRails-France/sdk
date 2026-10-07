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
    // Concurrent connections can finish in the opposite order to their reads.
    // Delayed samples/failures must not rewind or invalidate a newer epoch.
    nimby::runtime::ObservationEpoch concurrent;
    const auto stable=concurrent.observe(world,100);
    const auto oldClock=concurrent.begin(),newClock=concurrent.begin();
    CHECK(concurrent.observe(newClock,world,300)==stable);
    CHECK(concurrent.observe(oldClock,world,200)==0);
    CHECK(concurrent.observe(world,301)==stable);
    const auto oldFailure=concurrent.begin(),newSuccess=concurrent.begin();
    CHECK(concurrent.observe(newSuccess,world,302)==stable);
    concurrent.unavailable(oldFailure);
    CHECK(concurrent.observe(world,303)==stable);
    const auto oldWorld=concurrent.begin(),changedWorld=concurrent.begin();
    auto changed=world;changed.value[0]^=1;
    const auto changedGeneration=concurrent.observe(changedWorld,changed,400);
    CHECK(changedGeneration==stable+1);
    CHECK(concurrent.observe(oldWorld,world,304)==0);
    concurrent.unavailable(oldWorld);
    CHECK(concurrent.observe(changed,401)==changedGeneration);
    const auto genuineRewind=concurrent.begin();
    CHECK(concurrent.observe(genuineRewind,changed,50)==changedGeneration+1);
    const auto unavailable=concurrent.begin();
    concurrent.unavailable(unavailable);
    CHECK(concurrent.observe(concurrent.begin(),changed,51)==changedGeneration+2);
    std::puts("PASS observed game generations: unload, roots, world, history, clock rollback, out-of-order completion and stale failures");
}
