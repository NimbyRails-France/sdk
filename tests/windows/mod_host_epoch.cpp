#include <platform/windows/mod_host_epoch.h>
#include <runtime/observation_epoch.h>
#include "../observation_process_fixture.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
nimby::mod_host::Owners first,second,restarted;
nimby::mod_host::Owners* channel=&first;
nimby::mod_host::Request last;
uint32_t lastStatus{};
using Epoch=nimby::runtime::ObservationEpoch;
nimby::engine::VersioningObservation observed(nimby::platform::ObservationProcess& process) {
    nimby::engine::VersioningObservation value;
    auto reader=[](void* context,uint64_t address,void* output,size_t size){return static_cast<nimby::platform::ObservationProcess*>(context)->read(address,output,size);};
    CHECK(nimby::engine::read_versioning_observation(reader,&process,process.base,true,value,process.profile()));return value;
}
uint64_t capture(Epoch& epoch,nimby::platform::ObservationProcess& process) {
    const auto ticket=epoch.begin();return epoch.observe(ticket,observed(process),fixture::world(201)->ticks.load());
}
}
extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* args,
    const void* input,uint32_t size,void*,uint32_t capacity,uint32_t* written) noexcept {
    try {
        *written=0;if(capacity)return NIMBY_INVALID_ARGUMENT;
        last={};last.operation=operation;std::copy_n(args,8,last.args.begin());
        if(size)last.data.assign(static_cast<const uint8_t*>(input),static_cast<const uint8_t*>(input)+size);
        nimby::mod_host::Reply reply;lastStatus=nimby::mod_host::dispatchEpoch(last,reply,*channel);
        std::copy(reply.args.begin(),reply.args.end(),args);return lastStatus;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
int main(){try {
    using namespace nimby::mod_host;
    configureEpochTarget(201);installChildEpochAuthority();
    nimby::platform::ObservationProcess process;CHECK(process.open(201)==NIMBY_OK);
    auto world=fixture::world(201);Epoch childA,childB;
    const auto initial=capture(childA,process);CHECK(initial>0);
    channel=&second;CHECK(capture(childB,process)==initial);
    channel=&first;childA.unavailable();childA.unavailable(childA.begin());
    channel=&second;CHECK(capture(childB,process)==initial);
    // An earlier capture finishes after a genuine world switch in another mod.
    channel=&first;const auto staleTicket=childA.begin();const auto staleWorld=observed(process);
    world->identity=43;world->ticks=20000;
    channel=&second;const auto changed=capture(childB,process);CHECK(changed==initial+1);
    channel=&first;CHECK(childA.observe(staleTicket,staleWorld,10000)==0);
    CHECK(capture(childA,process)==changed);
    // A restarted worker must join the existing authority generation immediately.
    channel=&restarted;Epoch newWorker;CHECK(capture(newWorker,process)==changed);
    // Child-supplied identities and clocks are never authoritative inputs.
    auto invented=observed(process);invented.value[0]=99;
    CHECK(newWorker.observe(newWorker.begin(),invented,20000)==0);
    CHECK(newWorker.observe(newWorker.begin(),observed(process),999999)==0);
    CHECK(capture(newWorker,process)==changed);
    world->ticks=100;CHECK(capture(newWorker,process)==changed+1);
    const auto rewind=changed+1;
    // Tokens are worker-owned and one-use, including rejected completions.
    channel=&first;const auto owned=childA.begin();
    channel=&second;CHECK(childB.observe(owned,observed(process),100)==0);
    channel=&first;CHECK(childA.observe(owned,observed(process),100)==rewind);
    const auto replay=last;Reply reply;CHECK(dispatchEpoch(replay,reply,first)==NIMBY_DATA_UNAVAILABLE);
    auto malformed=replay;malformed.data.pop_back();CHECK(dispatchEpoch(malformed,reply,first)==NIMBY_INVALID_ARGUMENT);
    channel=&second;CHECK(capture(childB,process)==rewind);
    // Abandoned capture tickets consume bounded space without stalling later
    // captures or another worker's generation. No global invalidation request.
    channel=&first;for(unsigned i=0;i<200;++i)CHECK(childA.begin().revision==rewind);
    CHECK(first.epochs.size()<=64&&capture(childA,process)==rewind);
    channel=&second;CHECK(capture(childB,process)==rewind);
    Epoch::setAuthority(nullptr);
    Epoch ordinary;CHECK(ordinary.observe(observed(process),100)==1);
    std::cout<<"PASS authoritative epoch RPC: common generation, worker restart, failed/stale/forged sample isolation, real rewind, owned one-use tickets and bounded abandoned captures\n";
}catch(const std::exception& error){Epoch::setAuthority(nullptr);std::cerr<<error.what()<<'\n';return 1;}}
