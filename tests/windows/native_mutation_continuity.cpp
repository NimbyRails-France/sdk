// Actual journal, owned native-context contract with synthetic POD memory.
// This does NOT validate a game hook or complete native writer coverage.
#include <platform/windows/runtime/native_mutation_continuity.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <new>
#include <vector>
namespace mc=nimby::windows::mutation_continuity;
namespace owned=nimby::windows::owned_rear;
namespace world=nimby::windows::native_world;
namespace inv=nimby::windows::consist_inventory;
namespace rear=nimby::engine::native_rear;
namespace {
size_t checks{},allocations{};bool watchAllocations{};
#define CHECK(x) do{++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
constexpr uint64_t fullId(uint32_t ordinal,uint16_t generation=1){return (uint64_t(5)<<48)|(uint64_t(ordinal)<<16)|generation;}
constexpr uintptr_t controller=0x100000;
struct Fixture {
    world::Clock clock;world::Lineage lineage;
    mc::Storage<4,16,32> storage;mc::Journal<4,16,32> journal{storage};
    world::Capture capture;
    std::array<inv::Row,3> rows{};
    std::array<owned::provider::PhysicalCar,8> cars{};
    size_t carCount=2;
    bool worldCurrent=true;uint64_t thread=7;
    Fixture(){
        const auto stop=lineage.beginStop(clock,controller,controller+0x5d0);CHECK(stop);CHECK(lineage.finishStop(stop));
        const auto start=lineage.beginStart(clock,controller,controller+0x5d0);CHECK(start);CHECK(lineage.finishStart(start));
        CHECK(lineage.runnerEnter(controller,controller+0x5d0,thread));
        rows[0].train=fullId(1);rows[0].active=1;rows[0].firstChild=1;rows[0].childCount=1;
        rows[1].train=fullId(2);rows[1].parent=rows[0].train;rows[1].attached=1;rows[1].parentRow=0;rows[1].couplingOrder=1;
        rows[2].train=fullId(3);rows[2].active=1;
        cars[0].setup.model=11;cars[0].setup.owner=rows[0].train;cars[0].nativeMeters=20;cars[0].meters=20;
        cars[1].setup.model=12;cars[1].setup.owner=rows[1].train;cars[1].nativeMeters=24;cars[1].meters=24;
        open();
    }
    world::Scope scope()const {
        world::Scope value{controller,controller+0x5d0,controller+0x4c0,0x200000,0x300000,thread};
        value.boundary=world::Boundary::PostSharedExclusive;value.releaseReturn=0x349fa3;
        value.originalReleaseCompleted=value.exclusiveTrySucceeded=true;return value;
    }
    void open(){if(capture)CHECK(lineage.endCapture(capture));capture=lineage.capturePhase(clock,scope()).capture;CHECK(capture);}
    owned::Ownership ownership(){return {capture,
        [](void* p,const world::Capture& c)noexcept{auto& f=*static_cast<Fixture*>(p);return f.worldCurrent&&f.lineage.current(c);},
        [](void* p)noexcept{return static_cast<Fixture*>(p)->thread;},this};}
    owned::Roster roster(){
        inv::Result result;result.status=inv::Status::ObservedAllSlots;result.rows=rows.size();
        result.relationshipsIndexed=result.ownedPublication=true;return {capture,result,rows};
    }
    owned::Body body(uint64_t id=fullId(1)){
        owned::provider::PublicationProof proof{{capture.world,capture.phase,capture.phase,rear::Phase::RetainedMovement},
            id,capture.phase,owned::provider::Kind::CurrentPhysicalSnapshot,true,false};
        return {proof,{cars.data(),carCount},{},{}};
    }
    mc::BatchScope batch(uintptr_t caller=0x348f07){return {mc::key(capture),caller,true,true,true,
        [](void* p,const mc::WorldKey& key)noexcept{const auto& f=*static_cast<Fixture*>(p);
            return f.worldCurrent&&f.lineage.world()==key.world&&key.thread==f.thread;},this};}
    mc::Result observe(){mc::Budget budget;return journal.observe(ownership(),roster(),body(),budget);}
    mc::Stamp seed(){CHECK(journal.bind(ownership()).reason==mc::Reason::Ready);const auto r=observe();
        CHECK(r.reason==mc::Reason::Ready);CHECK(r.stamp);return r.stamp;}
};
// Feed the journal rows built by the actual inventory producer, including its
// descending inverse-child links. Only its process reader/ownership are fake.
void captureCoupledRoster(Fixture& f) {
    constexpr uintptr_t table=0x400000,block=0x500000;
    std::array<unsigned char,48> header{};
    std::array<uintptr_t,1> pointers{block};
    std::vector<unsigned char> memory(inv::detail::allocationBytes);
    const auto put=[](auto& bytes,size_t at,auto value){std::memcpy(bytes.data()+at,&value,sizeof value);};
    put(header,0,uint32_t(0x100000));put(header,4,uint32_t(9));put(header,8,uint32_t(512));
    put(header,12,uint32_t(512*0x638));put(header,16,uint32_t(511));
    put(header,24,table);put(header,32,table+8);put(header,40,table+8);
    for(size_t n=0;n<f.rows.size();++n){
        const auto& row=f.rows[n];const auto at=inv::detail::index(row.train)*inv::detail::stride;
        put(memory,at,row.train);put(memory,at+0x80,row.nativeMode);put(memory,at+0x1d0,row.active);
        put(memory,at+0x1d8,row.parent);put(memory,at+0x1e0,row.couplingOrder);
        put(memory,at+0x1e8,row.reversed);put(memory,at+0x1f0,row.attached);
        put(memory,at+0x5d0,row.running);put(memory,at+0x5f0,row.scheduled);
    }
    const auto reader=[&](uintptr_t address,void* target,size_t bytes){
        const auto copy=[&](uintptr_t base,const void* source,size_t extent){
            if(address<base||address-base>extent||bytes>extent-(address-base))return false;
            std::memcpy(target,static_cast<const unsigned char*>(source)+address-base,bytes);return true;
        };
        return copy(f.capture.simulation+0xa0,header.data(),header.size())||
            copy(table,pointers.data(),sizeof pointers)||copy(block,memory.data(),memory.size());
    };
    const auto valid=[](void* context)noexcept{return static_cast<Fixture*>(context)->ownership().valid();};
    inv::OwnedScope scope{valid,&f};inv::Budget budget{
        [](void* context,size_t)noexcept{return static_cast<Fixture*>(context)->ownership().valid();},&f};
    std::array<inv::Row,3> scratch{};std::array<uint64_t,512> ids{};
    const auto result=inv::captureMotionInventoryOwned(f.capture.simulation,true,scope,reader,budget,f.rows,{scratch,ids});
    CHECK(result.observedAllSlots()&&result.relationshipsIndexed&&result.ownedPublication);
    CHECK(result.rows==3);
}
}
#if defined(__GNUC__)
#define TEST_NOINLINE __attribute__((noinline))
#else
#define TEST_NOINLINE __declspec(noinline)
#endif
TEST_NOINLINE void* operator new(size_t n){if(watchAllocations)++allocations;if(auto* p=std::malloc(n?n:1))return p;throw std::bad_alloc();}
TEST_NOINLINE void* operator new[](size_t n){return ::operator new(n);}
TEST_NOINLINE void operator delete(void* p)noexcept{std::free(p);}
TEST_NOINLINE void operator delete[](void* p)noexcept{std::free(p);}
TEST_NOINLINE void operator delete(void* p,size_t)noexcept{std::free(p);}
TEST_NOINLINE void operator delete[](void* p,size_t)noexcept{std::free(p);}
int main(){try{
    static_assert(!mc::Result::nativeWritersComplete&&!mc::Result::progressionQualified&&!mc::Result::leaseReleaseQualified);
    {
        Fixture f;
        f.rows[2].active=0;f.rows[2].attached=1;f.rows[2].parent=f.rows[0].train;f.rows[2].couplingOrder=2;
        f.cars[2]=f.cars[1];f.cars[2].setup.owner=f.rows[2].train;f.cars[2].setup.model=13;f.carCount=3;
        captureCoupledRoster(f);
        CHECK(f.rows[0].firstChild==2&&f.rows[2].nextSibling==1&&f.rows[1].nextSibling==inv::noRow);
        CHECK(f.rows[0].childCount==2&&f.rows[1].parentRow==0&&f.rows[2].parentRow==0);
        const auto before=f.seed();CHECK(f.storage.records[0].members==3&&f.storage.records[0].cars==3);
        CHECK(f.storage.members[1].train==f.rows[2].train&&f.storage.members[2].train==f.rows[1].train);
        f.rows[2].reversed^=1;const auto changed=f.observe();CHECK(changed.reason==mc::Reason::Ready);
        CHECK(changed.stamp.consistRevision!=before.consistRevision);
        // An ascending chain is not a producer result; do not silently accept
        // it, a duplicate, a loop or a truncated inverse-child catalogue.
        const auto stamp=changed.stamp;
        f.rows[0].firstChild=1;f.rows[1].nextSibling=2;f.rows[2].nextSibling=inv::noRow;
        CHECK(f.observe().reason==mc::Reason::InvalidRoster);
        CHECK(!f.journal.unchanged(stamp).observedMutationsUnbroken);
    }
    {
        Fixture f;const auto first=f.seed();const auto again=f.observe();CHECK(first==again.stamp);
        f.open();CHECK(f.journal.bind(f.ownership()).reason==mc::Reason::Ready);CHECK(f.observe().stamp==first);
        CHECK(f.journal.unchanged(first).observedMutationsUnbroken);
        // Same counts/IDs but any field defining physical composition changes.
        for(unsigned change=0;change<7;++change){
            const auto before=f.observe().stamp;
            if(change==0)++f.cars[0].setup.model;
            if(change==1)f.cars[0].setup.settings[7]^=1;
            if(change==2)f.cars[1].nativeMeters=std::nextafter(f.cars[1].nativeMeters,100.f);
            if(change==3)f.rows[1].reversed^=1;
            if(change==4)++f.rows[1].couplingOrder;
            if(change==5)++f.rows[0].nativeMode;
            if(change==6)std::swap(f.cars[0],f.cars[1]);
            const auto after=f.observe();CHECK(after.reason==mc::Reason::Ready);
            CHECK(before.consistRevision!=after.stamp.consistRevision);
            CHECK(f.journal.unchanged(before).reason==mc::Reason::TupleChanged);
        }
        // A tuple changed then restored still has a NEW revision, not a hash.
        const auto before=f.observe().stamp;const auto original=f.cars[0].setup.model;
        ++f.cars[0].setup.model;CHECK(f.observe().reason==mc::Reason::Ready);
        f.cars[0].setup.model=original;CHECK(f.observe().stamp.consistRevision!=before.consistRevision);
    }
    for(const auto caller:{uintptr_t(0x348f07),uintptr_t(0x34928b),uintptr_t(0x3493d5)}){
        Fixture f;const auto before=f.seed();
        const auto token=f.journal.beginBatch(f.batch(caller));CHECK(token);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::MutationInProgress);
        CHECK(f.observe().reason==mc::Reason::MutationInProgress);
        CHECK(f.journal.finishBatch(token,true)==mc::Reason::Completed);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::MutationObserved);
        const auto after=f.observe();CHECK(after.stamp.consistRevision==before.consistRevision);
        CHECK(after.stamp.globalMutation!=before.globalMutation);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::MutationObserved);
        const auto counts=f.journal.counters();CHECK(counts.begun==1&&counts.completed==1&&counts.lastMutation==counts.lastCompleted);
    }
    {
        Fixture f;const auto before=f.seed();const auto token=f.journal.beginBatch(f.batch());CHECK(token);
        CHECK(f.journal.finishBatch(token,false)==mc::Reason::OriginalException);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::Lost);
        CHECK(f.journal.bind(f.ownership()).reason==mc::Reason::Lost);
        CHECK(f.journal.counters().exceptions==1);
    }
    {
        Fixture f;const auto before=f.seed();const auto one=f.journal.beginBatch(f.batch());CHECK(one);
        CHECK(!f.journal.beginBatch(f.batch()));CHECK(f.journal.counters().nested==1);
        CHECK(f.journal.finishBatch(one,true)==mc::Reason::Lost);
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
    }
    for(unsigned variation=0;variation<6;++variation){
        Fixture f;const auto before=f.seed();auto bad=f.batch();
        if(variation==0)bad.returnRva=0x348f02; // CALL address is not caller return.
        if(variation==1)bad.profileQualified=false;
        if(variation==2)bad.nativeExclusiveSite=false;
        if(variation==3)bad.currentRootVerified=false;
        if(variation==4)bad.owner.controller+=8;
        if(variation==5)f.worldCurrent=false;
        CHECK(!f.journal.beginBatch(bad));CHECK(f.journal.counters().unqualified==1);
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
    }
    {
        Fixture f;const auto before=f.seed();auto foreign=f.batch(0x4f3a6f);
        foreign.owner.database+=8;foreign.owner.simulation+=8;
        CHECK(!f.journal.beginBatch(foreign));CHECK(f.journal.counters().foreign==1);
        CHECK(f.journal.unchanged(before).observedMutationsUnbroken);
    }
    for(bool binding:{false,true}){
        Fixture f;const auto before=f.seed();f.worldCurrent=false;
        const auto refused=binding?f.journal.bind(f.ownership()):f.observe();
        CHECK(refused.reason==mc::Reason::OwnershipUnknown);
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
        f.worldCurrent=true;
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
        CHECK(f.journal.bind(f.ownership()).reason==mc::Reason::Lost);
    }
    {
        Fixture f;CHECK(!f.journal.beginBatch(f.batch()));CHECK(f.journal.counters().prebootstrap==1);
        const auto before=f.seed();f.journal.invalidateWorld();
        CHECK(f.journal.unchanged(before).reason==mc::Reason::MutationObserved);
        f.journal.invalidateWorld(true);CHECK(f.journal.unchanged(before).reason==mc::Reason::Lost);
        // Only a genuinely NEW lifecycle world establishes new metadata.
        CHECK(f.lineage.endCapture(f.capture));f.capture={};
        const auto stop=f.lineage.beginStop(f.clock,controller,controller+0x5d0);CHECK(stop);CHECK(f.lineage.finishStop(stop));
        const auto start=f.lineage.beginStart(f.clock,controller,controller+0x5d0);CHECK(start);CHECK(f.lineage.finishStart(start));
        CHECK(f.lineage.runnerEnter(controller,controller+0x5d0,f.thread));f.open();
        const auto newWorld=f.seed();CHECK(newWorld.owner.world!=before.owner.world);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::WorldChanged);
    }
    {
        Fixture f;const auto before=f.seed();
        CHECK(f.lineage.endCapture(f.capture));f.capture={};CHECK(f.lineage.invalidateMerge(f.clock,0x300000));f.open();
        CHECK(f.journal.bind(f.ownership()).reason==mc::Reason::Ready);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::WorldChanged);
        CHECK(f.observe().stamp.consistRevision==before.consistRevision);
    }
    {
        Fixture f;const auto before=f.seed();f.rows[0].train=fullId(1,2);f.rows[1].parent=f.rows[0].train;
        f.cars[0].setup.owner=f.rows[0].train;mc::Budget budget;
        const auto changed=f.journal.observe(f.ownership(),f.roster(),f.body(f.rows[0].train),budget);
        CHECK(changed.reason==mc::Reason::Ready);CHECK(changed.stamp.consistRevision!=before.consistRevision);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::IdentityChanged);
    }
    for(unsigned variation=0;variation<7;++variation){
        Fixture f;const auto before=f.seed();auto roster=f.roster();auto body=f.body();mc::Budget budget;
        if(variation==0)roster.result.relationshipsIndexed=false;
        if(variation==1)roster.result.ownedPublication=false;
        if(variation==2)f.rows[0].firstChild=999;
        if(variation==3)f.rows[1].parent=fullId(1,3);
        if(variation==4)f.cars[1]=f.cars[0];
        if(variation==5)f.cars[1].nativeMeters=0;
        if(variation==6)body.publication.progressionQualified=true;
        const auto r=f.journal.observe(f.ownership(),roster,body,budget);CHECK(r.reason!=mc::Reason::Ready);
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken); // Refusal revokes the old stamp, never the old claims.
        CHECK(f.journal.unchanged(before).stamp.consistRevision==before.consistRevision);
    }
    {
        Fixture f;const auto before=f.seed();f.cars[0].setup.model++;
        for(size_t limit=0;limit<15;++limit){mc::Budget budget{limit};
            CHECK(f.journal.observe(f.ownership(),f.roster(),f.body(),budget).reason==mc::Reason::Budget);
            CHECK(budget.used<=limit);CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);}
    }
    {
        Fixture f;const auto before=f.seed();
        CHECK(f.lineage.endCapture(f.capture));f.capture={};
        const auto stop=f.lineage.beginStop(f.clock,controller,controller+0x5d0);CHECK(stop);CHECK(f.lineage.finishStop(stop));
        const auto start=f.lineage.beginStart(f.clock,controller,controller+0x5d0);CHECK(start);CHECK(f.lineage.finishStart(start));
        CHECK(f.lineage.runnerEnter(controller,controller+0x5d0,f.thread));f.capture={};f.open();
        struct Validation {Fixture* fixture;unsigned calls{};} validation{&f};
        auto ownership=f.ownership();ownership.context=&validation;
        ownership.current=[](void* p,const world::Capture& capture)noexcept{
            auto& value=*static_cast<Validation*>(p);return ++value.calls!=2&&value.fixture->lineage.current(capture);
        };
        ownership.currentThread=[](void* p)noexcept{return static_cast<Validation*>(p)->fixture->thread;};
        // This is the second validity check, before replacing the old world's
        // records. Refusal must retire the old stamp even without a new bind.
        CHECK(f.journal.bind(ownership).reason==mc::Reason::OwnershipUnknown);
        CHECK(validation.calls==2);CHECK(f.journal.unchanged(before).reason==mc::Reason::Lost);
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
        // A genuinely new world may start NEW bookkeeping, never old history.
        CHECK(f.journal.bind(f.ownership()).reason==mc::Reason::Ready);
        CHECK(f.journal.unchanged(before).reason==mc::Reason::WorldChanged);
        CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
    }
    {
        Fixture f;const auto before=f.seed();const auto original=f.cars[1];f.carCount=1;
        CHECK(f.observe().reason==mc::Reason::Ready);CHECK(f.journal.counters().cars==1);
        f.carCount=2;f.cars[1]=original;CHECK(f.observe().reason==mc::Reason::Ready);
        CHECK(f.journal.counters().cars==2);CHECK(!f.journal.unchanged(before).observedMutationsUnbroken);
    }
    {
        Fixture f;f.seed();const auto firstCars=f.cars;const auto child=f.rows[1];
        f.carCount=1;f.cars[0].setup.owner=fullId(3);f.cars[0].setup.model=33;
        mc::Budget secondBudget;
        const auto second=f.journal.observe(f.ownership(),f.roster(),f.body(fullId(3)),secondBudget);
        CHECK(second.reason==mc::Reason::Ready);CHECK(f.storage.records[1].carBegin==2);
        f.cars=firstCars;f.carCount=3;f.cars[2]=f.cars[0];f.cars[2].setup.settings[0]=1;
        CHECK(f.observe().reason==mc::Reason::Ready);CHECK(f.storage.records[1].carBegin==3);
        CHECK(f.storage.cars[3].setup.owner==fullId(3)&&f.storage.cars[3].setup.model==33);
        CHECK(f.journal.unchanged(second.stamp).observedMutationsUnbroken);
        // Shrink members AND cars of the earlier record; the second record
        // keeps its exact tuple, revision and valid offsets after both moves.
        f.carCount=1;f.rows[0].firstChild=inv::noRow;f.rows[0].childCount=0;
        f.rows[1].attached=0;f.rows[1].parent=0;f.rows[1].parentRow=inv::noRow;
        CHECK(f.observe().reason==mc::Reason::Ready);
        CHECK(f.storage.records[1].carBegin==1&&f.storage.records[1].memberBegin==1);
        CHECK(f.storage.cars[1].setup.model==33&&f.storage.members[1].train==fullId(3));
        CHECK(f.journal.unchanged(second.stamp).observedMutationsUnbroken);
        f.rows[0].firstChild=1;f.rows[0].childCount=1;f.rows[1]=child;f.cars=firstCars;f.carCount=2;
        CHECK(f.observe().reason==mc::Reason::Ready);
        CHECK(f.storage.records[1].carBegin==2&&f.storage.records[1].memberBegin==2);
        CHECK(f.journal.unchanged(second.stamp).observedMutationsUnbroken);
    }
    {
        Fixture f;CHECK(f.journal.bind(f.ownership()).reason==mc::Reason::Ready);
        mc::Storage<1,1,1> tiny;mc::Journal<1,1,1> constrained(tiny);CHECK(constrained.bind(f.ownership()).reason==mc::Reason::Ready);
        mc::Budget budget;CHECK(constrained.observe(f.ownership(),f.roster(),f.body(),budget).reason==mc::Reason::Capacity);
        CHECK(constrained.counters().roots==0);
    }
    {
        Fixture f;mc::Storage<4,16,32> storage;mc::Journal<4,16,32> exhausted(storage,UINT64_MAX);
        CHECK(exhausted.bind(f.ownership()).reason==mc::Reason::Ready);mc::Budget budget;
        CHECK(exhausted.observe(f.ownership(),f.roster(),f.body(),budget).reason==mc::Reason::SequenceExhausted);
        CHECK(exhausted.counters().roots==0);
    }
    {
        Fixture f;f.seed();allocations=0;watchAllocations=true;
        for(size_t i=0;i<1000;++i){mc::Budget budget;
            const auto r=f.journal.observe(f.ownership(),f.roster(),f.body(),budget);
            CHECK(r.reason==mc::Reason::Ready);const auto m=f.journal.beginBatch(f.batch());CHECK(m);
            CHECK(f.journal.finishBatch(m,true)==mc::Reason::Completed);}
        watchAllocations=false;CHECK(allocations==0);
    }
    std::cout<<checks<<" checks PASS; no progression or lease-release certificate\n";return 0;
}catch(const std::exception& e){watchAllocations=false;std::cerr<<e.what()<<'\n';return 1;}}
