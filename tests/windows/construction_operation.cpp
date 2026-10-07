#include "platform/windows/runtime/construction_operation.h"
#include "platform/windows/runtime/construction_bridge.h"
#include <cstdio>
#include <stdexcept>
#include <string>

#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__)+": " #x);}while(false)
using namespace nimby::construction_bridge;
using nimby::platform::windows::BridgeRequestLease;

namespace {
const Preparation world{11,22,33,44,55,true};
const BridgeRequestLease ownerA{1100,100,10,0},ownerB{2100,200,20,0};
const NimbyConstructionRequest request=[] {
    NimbyConstructionRequest value{};value.size=sizeof value;value.version=1;
    value.action=NIMBY_CONSTRUCTION_PREPARE;value.source_signal=0x8000000000001;
    return value;
}();
struct Fixture {
    Operation operation{};uint64_t token=1;unsigned aliveCalls=0;bool alive=true;
    uint32_t prepare(const BridgeRequestLease& owner,uint64_t now,Preparation next=world) {
        return prepareOperation(operation,token,next,request,owner,now,[&](const BridgeRequestLease&){++aliveCalls;return alive;});
    }
    Fixture(){CHECK(prepare(ownerA,100)==0);CHECK(operation.token==1&&token==2);}
    void preserve(uint32_t expected,const BridgeRequestLease& owner,uint64_t now,Preparation next=world) {
        const auto old=operation;const auto nextToken=token;
        CHECK(prepare(owner,now,next)==expected);
        CHECK(token==nextToken&&operation.token==old.token&&operation.root==old.root&&operation.simulation==old.simulation);
        CHECK(operation.history==old.history&&operation.command==old.command&&operation.identity==old.identity);
        CHECK(operation.waiting==old.waiting&&operation.result.state==old.result.state&&operation.result.can_undo==old.result.can_undo);
        CHECK(operation.result.count==old.result.count&&operation.result.ids[0]==old.result.ids[0]);
        CHECK(operation.request.source_signal==old.request.source_signal&&operation.lease.expires==old.lease.expires);
    }
};
}
int main(){try{
    {
        Fixture f;f.preserve(8,ownerB,1099);CHECK(f.aliveCalls==1);
        CHECK(f.prepare(ownerB,1100)==0); // Exact expiry, no sleeping or timing race.
        CHECK(f.operation.token==2&&f.token==3&&f.operation.lease.process==ownerB.process);
    }
    {
        Fixture f;auto same=ownerA;same.expires=5000;
        CHECK(f.prepare(same,1200)==0);CHECK(f.aliveCalls==0);
        // An owner may keep using its old ticket after its reservation expires;
        // only a successful new PREPARE supersedes it. No background eviction.
        CHECK(f.operation.token==2&&f.operation.lease.expires==5000);
    }
    {
        Fixture f;auto recycled=ownerA;recycled.creation++;recycled.expires=2100;
        f.preserve(8,recycled,500); // PID alone never means same requester.
        f.alive=false;CHECK(f.prepare(recycled,500)==0);CHECK(f.operation.token==2);
    }
    for(const auto state:{NIMBY_CONSTRUCTION_APPLIED,NIMBY_CONSTRUCTION_PARTIAL,NIMBY_CONSTRUCTION_UNDONE,NIMBY_CONSTRUCTION_REJECTED}){
        Fixture f;f.operation.result.state=state;f.operation.result.can_undo=1;f.operation.history=777;
        CHECK(f.prepare(ownerB,500)==0);CHECK(f.aliveCalls==0);
        CHECK(f.operation.token==2&&f.operation.history==0&&!f.operation.result.can_undo);
    }
    for(const bool requesterAlive:{false,true}){
        Fixture f;f.operation.waiting=true;f.operation.result.state=NIMBY_CONSTRUCTION_PENDING;
        f.operation.command=888;f.operation.history=777;f.operation.identity={1,2,3};f.alive=requesterAlive;
        auto future=ownerB;future.expires=10000;
        f.preserve(8,future,5000); // Queue/result ownership outlives request TTL.
        auto changed=world;changed.root++;
        f.preserve(8,future,5000,changed);CHECK(f.aliveCalls==0);
    }
    for(const auto state:{NIMBY_CONSTRUCTION_READY,NIMBY_CONSTRUCTION_APPLIED}){
        Fixture f;f.operation.result.state=state;f.operation.result.can_undo=1;f.operation.history=777;
        f.operation.result.count=1;f.operation.result.ids[0]=0x8000000000002;
        auto invalid=world;invalid.sourceValid=false;f.preserve(5,ownerB,500,invalid);
        invalid=world;invalid.simulation=0;f.preserve(4,ownerB,500,invalid);
        auto expired=ownerB;expired.expires=500;f.preserve(1,expired,500);
        CHECK(f.aliveCalls==0); // Rejected preparation never touches peer liveness.
    }
    for(unsigned change=0;change<3;++change){
        Fixture f;auto next=world;
        if(change==0)next.root++;else if(change==1)next.simulation++;else next.revision++;
        CHECK(f.prepare(ownerB,500,next)==0);CHECK(f.operation.token==2&&f.aliveCalls==0);
    }
    {
        Fixture f;f.token=0;f.preserve(8,ownerA,500);
    }
    {
        Shared response{};response.lease=ownerA;
        response.result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_APPLIED,1,123,0,1,{0x8000000000002}};
        completeResponse(response,5000); // Completion can follow lease expiry.
        CHECK(response.expires==7000&&response.lease.expires==1100);
        const auto alive=[](const BridgeRequestLease&){return true;};
        CHECK(responseReserved(response,ownerB,6999,alive));
        CHECK(!responseReserved(response,ownerB,7000,alive));
        CHECK(!responseReserved(response,ownerA,5000,alive));
        CHECK(!responseReserved(response,ownerB,5000,[](const BridgeRequestLease&){return false;}));
        InterlockedExchange(&response.reserved,1);
        CHECK(!responseReserved(response,ownerB,5000,alive));
        auto recycled=ownerA;++recycled.creation;InterlockedExchange(&response.reserved,0);
        CHECK(responseReserved(response,recycled,5000,alive));
        InterlockedExchange(&response.state,executing);
        CHECK(!responseReserved(response,ownerB,5000,alive)); // Caller separately rejects any in-flight transport.
    }
    std::puts("PASS: construction PREPARE reservation expiry, owner identity, pending protection and failed preparation preserve the current operation");
}catch(const std::exception& error){std::fprintf(stderr,"%s\n",error.what());return 1;}}
