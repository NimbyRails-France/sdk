#include <platform/windows/runtime/native_consist_inventory.h>
#include <platform/windows/runtime/route_read_budget.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

// Synthetic owned-memory fixture only. No process, game or native permission
// is exercised, and a successful local readback is not an ownership witness.
namespace fixture { bool watchAllocation=false;size_t allocations=0; }
#ifdef __GNUC__
#define FIXTURE_NOINLINE __attribute__((noinline))
#else
#define FIXTURE_NOINLINE __declspec(noinline)
#endif
FIXTURE_NOINLINE void* operator new(size_t bytes){
    if(fixture::watchAllocation)++fixture::allocations;
    if(auto* value=std::malloc(bytes?bytes:1))return value;
    throw std::bad_alloc();
}
FIXTURE_NOINLINE void* operator new[](size_t bytes){return ::operator new(bytes);}
FIXTURE_NOINLINE void operator delete(void* value)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete(void* value,size_t)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete[](void* value,size_t)noexcept{std::free(value);}
#undef FIXTURE_NOINLINE

namespace fixture {
namespace inventory=nimby::windows::consist_inventory;
using inventory::Row;using inventory::Reason;using inventory::Result;
constexpr uintptr_t simulation=0x20000000,table=0x30000000,blockA=0x40000000,blockB=0x41000000;
constexpr size_t stride=0x638,blockSize=0x100000,slots=512;
constexpr uint64_t train(size_t index,uint16_t generation=1){return (uint64_t(5)<<48)|(index<<16)|generation;}
size_t checks=0;
void require(bool condition,const char* expression,int line){
    ++checks;if(!condition){std::fprintf(stderr,"line%d: %s\n",line,expression);std::abort();}
}
#define CHECK(...) require(bool(__VA_ARGS__),#__VA_ARGS__,__LINE__)
template<class T>void put(auto& bytes,size_t at,T value){std::memcpy(bytes.data()+at,&value,sizeof value);}

struct Memory {
    std::array<unsigned char,0xa0+48> sim{};
    std::array<uintptr_t,128> pointers{};
    std::vector<unsigned char> first=std::vector<unsigned char>(blockSize);
    std::vector<unsigned char> second=std::vector<unsigned char>(blockSize);
    enum class Mutation {None,Header,Pointer,Mode,Parent,Order,Attached,Reversed,
        Active,Running,Scheduled,EmptyId,TombstoneId,Generation,FinalHeader};
    Mutation mutation=Mutation::None;
    size_t reads=0,headers=0,readBytes=0,denyRead=0,throwRead=0;
    bool readAfterRefusal=false,preflightRefused=false,readRoster=false,virtualEmptyBlocks=false;
    void reset(){
        sim.fill(0);pointers.fill(0);std::fill(first.begin(),first.end(),0);
        std::fill(second.begin(),second.end(),0);mutation=Mutation::None;
        reads=headers=readBytes=denyRead=throwRead=0;
        readAfterRefusal=preflightRefused=readRoster=virtualEmptyBlocks=false;
        header(1);pointers[0]=blockA;
    }
    Memory(){reset();}
    void header(size_t blocks){
        put(sim,0xa0,uint32_t(blockSize));put(sim,0xa4,uint32_t(9));
        put(sim,0xa8,uint32_t(slots));put(sim,0xac,uint32_t(slots*stride));
        put(sim,0xb0,uint32_t(slots-1));put(sim,0xb8,blocks?table:uintptr_t(0));
        put(sim,0xc0,blocks?table+blocks*8:uintptr_t(0));
        put(sim,0xc8,blocks?table+blocks*8:uintptr_t(0));
    }
    auto& data(size_t ordinal){return ordinal<slots?first:second;}
    void id(size_t ordinal,uint64_t value){put(data(ordinal),(ordinal%slots)*stride,value);}
    void row(size_t ordinal,uint64_t parent=0,bool attached=false){
        auto& bytes=data(ordinal);const auto at=(ordinal%slots)*stride;
        put(bytes,at,train(ordinal));put(bytes,at+0x1d0,uint8_t(0));
        put(bytes,at+0x1d8,parent);put(bytes,at+0x1e0,int64_t(-17));
        put(bytes,at+0x1e8,uint8_t(1));put(bytes,at+0x1f0,uint8_t(attached));
        put(bytes,at+0x80,int32_t(9));put(bytes,at+0x5d0,uint8_t(0));
        put(bytes,at+0x5f0,uint8_t(0));
        // +88 is intentionally absent: the complete inventory must derive its
        // inverse parent index from all slots, including inactive children.
    }
    void mutate(){
        switch(mutation){
        case Mutation::Header:case Mutation::FinalHeader:put(sim,0xb0,uint32_t(510));break;
        case Mutation::Pointer:pointers[0]=blockB;break;
        case Mutation::Mode:put(first,0x80,int32_t(8));break;
        case Mutation::Parent:put(first,0x1d8,train(3));break;
        case Mutation::Order:put(first,0x1e0,int64_t(1));break;
        case Mutation::Attached:put(first,0x1f0,uint8_t(1));break;
        case Mutation::Reversed:put(first,0x1e8,uint8_t(0));break;
        case Mutation::Active:put(first,0x1d0,uint8_t(1));break;
        case Mutation::Running:put(first,0x5d0,uint8_t(1));break;
        case Mutation::Scheduled:put(first,0x5f0,uint8_t(1));break;
        case Mutation::EmptyId:id(20,train(20));break;
        case Mutation::TombstoneId:id(20,0xffff000000140002ULL);break;
        case Mutation::Generation:id(0,train(0,2));break;
        case Mutation::None:break;
        }
    }
    bool read(uintptr_t address,void* out,size_t bytes){
        if(preflightRefused)readAfterRefusal=true;
        ++reads;readBytes+=bytes;
        if(reads==throwRead)throw 7;
        if(reads==denyRead)return false;
        if(address==simulation+0xa0){
            ++headers;
            if((headers==2&&mutation!=Mutation::FinalHeader)||
               (headers==3&&mutation==Mutation::FinalHeader))mutate();
        }
        if((address>=blockA&&address<blockA+blockSize)||
           (address>=blockB&&address<blockB+blockSize)){
            const auto offset=(address>=blockB?address-blockB:address-blockA)%stride;
            if(offset==0x88)readRoster=true;
        }
        const auto copy=[&](uintptr_t base,const void* source,size_t extent){
            if(address<base||address-base>extent||bytes>extent-(address-base))return false;
            std::memcpy(out,static_cast<const unsigned char*>(source)+(address-base),bytes);return true;
        };
        if(virtualEmptyBlocks&&address>=0x50000000&&address<0x51000000&&bytes<=0x51000000-address){
            std::memset(out,0,bytes);return true;
        }
        return copy(simulation,sim.data(),sim.size())||
            copy(table,pointers.data(),sizeof pointers)||copy(blockA,first.data(),first.size())||
            copy(blockB,second.data(),second.size());
    }
};
struct Gate {
    Memory& memory;size_t calls=0,readCalls=0,cpuCalls=0,deny=0,afterRefusal=0,lastBytes=0;
    bool refused=false;
    static bool preflight(void* context,size_t bytes)noexcept{
        auto& gate=*static_cast<Gate*>(context);
        if(gate.refused)++gate.afterRefusal;
        ++gate.calls;gate.lastBytes=bytes;if(bytes)++gate.readCalls;else ++gate.cpuCalls;
        if(gate.calls==gate.deny){gate.refused=true;gate.memory.preflightRefused=true;return false;}
        return true;
    }
};
struct Storage {
    std::array<Row,1024> output{},scratch{};
    std::array<uint64_t,8192> ids{};
    std::array<unsigned char,sizeof output> before{};
    void sentinel(){
        before.fill(0xa5);std::memcpy(output.data(),before.data(),sizeof output);
    }
    bool untouched()const{return std::memcmp(before.data(),output.data(),sizeof output)==0;}
};
Result capture(Memory& memory,Storage& storage,Gate& gate,inventory::Limits limits={},
    bool qualified=true,size_t outputSize=1024,size_t scratchSize=1024,size_t idSize=8192){
    inventory::Budget budget{Gate::preflight,&gate};
    const auto before=allocations;watchAllocation=true;
    auto result=inventory::captureMotionInventory(simulation,qualified,
        [&](uintptr_t at,void* value,size_t bytes){return memory.read(at,value,bytes);},budget,
        {storage.output.data(),outputSize},{{storage.scratch.data(),scratchSize},{storage.ids.data(),idSize}},limits);
    watchAllocation=false;CHECK(allocations==before);
    CHECK(result.relationshipsIndexed==result.observedAllSlots());
    if(!result.observedAllSlots()){CHECK(result.rows==0);CHECK(storage.untouched());}
    CHECK(!memory.readAfterRefusal);CHECK(gate.afterRefusal==0);
    // Once refused, not even a small follow-up preflight may reach the shared
    // clock/reader again. Local read/byte/work limits also terminate this scope.
    if(budget.exhausted){const auto calls=gate.calls;CHECK(!budget.available(1));CHECK(gate.calls==calls);}
    return result;
}
Result fresh(Memory& memory,Storage& storage,inventory::Limits limits={}){
    storage.sentinel();Gate gate{memory};return capture(memory,storage,gate,limits);
}
void relationshipChecks(Memory& memory,Storage& storage){
    memory.reset();memory.row(0);memory.row(7,train(0),true);memory.row(30,train(0),true);
    memory.row(31,0xffff000000000099ULL,false); // Detached stale reference is copied, never followed.
    auto result=fresh(memory,storage);CHECK(result.observedAllSlots());CHECK(result.rows==4);
    CHECK(result.visitedSlots==512);CHECK(!memory.readRoster);CHECK(result.bytes<10000);
    CHECK(storage.output[0].firstChild==2);CHECK(storage.output[2].nextSibling==1);
    CHECK(storage.output[0].childCount==2&&storage.output[1].childCount==0&&storage.output[3].childCount==0);
    CHECK(result.relationshipsIndexed);
    CHECK(storage.output[1].parentRow==0);CHECK(storage.output[2].parentRow==0);
    CHECK(storage.output[3].parentRow==inventory::noRow);
    CHECK(storage.output[1].train==train(7));CHECK(storage.output[1].nativeMode==9);
    CHECK(storage.output[1].couplingOrder==-17);CHECK(storage.output[1].reversed==1);
    CHECK(storage.output[1].active==0&&storage.output[1].running==0&&storage.output[1].scheduled==0);
    CHECK(!result.ownershipQualifier&&!result.universalWriterExclusion);
    CHECK(!result.completeConsistCertified&&!result.rearClearCertified);
    memory.reset();memory.row(0,train(1),true);
    CHECK(fresh(memory,storage).reason==Reason::MissingParent);
    memory.reset();memory.row(0);memory.row(1,train(0,2),true);
    CHECK(fresh(memory,storage).reason==Reason::ParentGenerationMismatch);
    memory.reset();memory.row(0,train(0),true);
    CHECK(fresh(memory,storage).reason==Reason::SelfParent);
    memory.reset();memory.row(0,0,true);
    CHECK(fresh(memory,storage).reason==Reason::InvalidParent);
    memory.reset();memory.row(0,train(1),true);memory.row(1,train(0),true);
    // A later unrelated valid attachment must not mask the IDs that caused
    // the refusal: logs need the offending relationship, not the last visited.
    memory.row(30);memory.row(31,train(30),true);
    result=fresh(memory,storage);CHECK(result.reason==Reason::CouplingCycle);
    CHECK(result.train==train(0));CHECK(result.parent==train(1));CHECK(result.ordinal==0);
    memory.reset();memory.row(0);memory.row(1,train(0),true);memory.row(2,train(1),true);
    memory.row(30);memory.row(31,train(30),true);
    result=fresh(memory,storage);CHECK(result.reason==Reason::UnsupportedCouplingChain);
    CHECK(result.train==train(2));CHECK(result.parent==train(1));CHECK(result.ordinal==2);
    memory.reset();memory.header(2);memory.pointers[1]=blockB;
    memory.row(0);memory.row(1023,train(0),true);
    CHECK(fresh(memory,storage).observedAllSlots());CHECK(storage.output[1].train==train(1023));
    CHECK(storage.output[1].parentRow==0);
    // A complete dense block is intentionally expensive but still bounded and
    // correctly indexed; this validates linear coloring instead of N^2 walks.
    memory.reset();for(size_t n=0;n<512;++n)memory.row(n,n?train(0):0,n!=0);
    result=fresh(memory,storage);CHECK(result.observedAllSlots());CHECK(result.rows==512);
    CHECK(result.reads==4102);CHECK(result.bytes<100000);CHECK(result.work<12000);
    CHECK(storage.output[0].firstChild==511);
    CHECK(storage.output[0].childCount==511);
    for(size_t n=1;n<result.rows;++n){
        CHECK(inventory::detail::index(storage.output[n-1].train)<inventory::detail::index(storage.output[n].train));
        CHECK(storage.output[n].childCount==0);
    }
}
void mutationChecks(Memory& memory,Storage& storage){
    using Mutation=Memory::Mutation;
    const std::array cases={Mutation::Header,Mutation::Pointer,Mutation::Mode,Mutation::Parent,
        Mutation::Order,Mutation::Attached,Mutation::Reversed,Mutation::Active,Mutation::Running,
        Mutation::Scheduled,Mutation::EmptyId,Mutation::TombstoneId,Mutation::Generation,Mutation::FinalHeader};
    for(const auto mutation:cases){
        memory.reset();memory.row(0);memory.mutation=mutation;
        const auto result=fresh(memory,storage);CHECK(!result.observedAllSlots());
        const auto expected=mutation==Mutation::Header||mutation==Mutation::Pointer||mutation==Mutation::FinalHeader?
            Reason::PoolChanged:mutation==Mutation::EmptyId||mutation==Mutation::TombstoneId||mutation==Mutation::Generation?
            Reason::SlotChanged:Reason::ValuesChanged;
        CHECK(result.reason==expected);
    }
    // Identity failures must report the affected slot and BOTH full IDs,
    // including creation in a formerly empty slot and generation reuse.
    // A later valid train previously left an unrelated ID in Result::train.
    for(const auto mutation:{Mutation::EmptyId,Mutation::TombstoneId,Mutation::Generation}){
        memory.reset();memory.row(0);memory.row(7);memory.mutation=mutation;
        const auto result=fresh(memory,storage);CHECK(result.reason==Reason::SlotChanged);
        const auto ordinal=mutation==Mutation::Generation?0u:20u;
        const auto before=mutation==Mutation::Generation?train(0):0;
        const auto after=mutation==Mutation::Generation?train(0,2):
            mutation==Mutation::EmptyId?train(20):0xffff000000140002ULL;
        CHECK(result.ordinal==ordinal);CHECK(result.train==before);
        CHECK(result.observedTrain==after);CHECK(result.parent==0);
    }
}
void limitsChecks(Memory& memory,Storage& storage){
    const auto reset=[&]{memory.reset();memory.row(0);storage.sentinel();};
    inventory::Limits limits;
    reset();Gate gate{memory};CHECK(capture(memory,storage,gate,limits,false).reason==Reason::UnqualifiedProfile);CHECK(memory.reads==0);
    const std::array<size_t inventory::Limits::*,6> members={&inventory::Limits::blocks,&inventory::Limits::slots,
        &inventory::Limits::rows,&inventory::Limits::reads,&inventory::Limits::bytes,&inventory::Limits::work};
    for(const auto member:members){reset();limits={};limits.*member=0;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);CHECK(memory.reads==0);}
    reset();limits={};limits.blocks=129;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);
    reset();limits={};limits.slots=65537;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);
    reset();limits={};limits.rows=4097;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);
    reset();limits={};limits.reads=8193;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);
    reset();limits={};limits.bytes=4u*1024u*1024u+1;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);
    reset();limits={};limits.work=262145;CHECK(fresh(memory,storage,limits).reason==Reason::InvalidLimits);
    reset();memory.header(17);CHECK(fresh(memory,storage).reason==Reason::PoolCapacity);CHECK(memory.reads==1);
    reset();limits={};limits.slots=511;CHECK(fresh(memory,storage,limits).reason==Reason::SlotCapacity);CHECK(memory.reads==1);
    reset();Gate shortIds{memory};CHECK(capture(memory,storage,shortIds,{},true,1024,1024,511).reason==Reason::SlotCapacity);CHECK(memory.reads==1);
    reset();memory.row(1);limits={};limits.rows=1;CHECK(fresh(memory,storage,limits).reason==Reason::RowCapacity);
    reset();memory.row(1);Gate shortRows{memory};CHECK(capture(memory,storage,shortRows,{},true,1).reason==Reason::RowCapacity);
    reset();limits={};limits.reads=1;CHECK(fresh(memory,storage,limits).reason==Reason::ReadLimit);CHECK(memory.reads==1);
    reset();limits={};limits.bytes=47;CHECK(fresh(memory,storage,limits).reason==Reason::ByteLimit);CHECK(memory.reads==0);
    reset();limits={};limits.bytes=55;CHECK(fresh(memory,storage,limits).reason==Reason::ByteLimit);CHECK(memory.reads==1);
    reset();limits={};limits.work=1;CHECK(fresh(memory,storage,limits).reason==Reason::WorkLimit);CHECK(memory.reads==2);
    reset();memory.id(0,(uint64_t(8)<<48)|1);CHECK(fresh(memory,storage).reason==Reason::UnexpectedTag);
    reset();memory.id(0,train(1));CHECK(fresh(memory,storage).reason==Reason::IdentityOrdinalMismatch);
    reset();memory.id(0,train(0xffffffffULL));CHECK(fresh(memory,storage).reason==Reason::IdentityOrdinalMismatch);
    for(size_t offset=0xa0;offset<=0xb0;offset+=4){reset();put(memory.sim,offset,uint32_t(0));CHECK(fresh(memory,storage).reason==Reason::InvalidPoolLayout);CHECK(memory.reads==1);}
    reset();put(memory.sim,0xc0,table-8);CHECK(fresh(memory,storage).reason==Reason::InvalidPoolLayout);
    reset();put(memory.sim,0xc8,table);CHECK(fresh(memory,storage).reason==Reason::InvalidPoolLayout);
    reset();put(memory.sim,0xc0,table+7);CHECK(fresh(memory,storage).reason==Reason::InvalidPoolLayout);
    reset();put(memory.sim,0xb8,std::numeric_limits<uintptr_t>::max()-7);put(memory.sim,0xc0,std::numeric_limits<uintptr_t>::max());
    put(memory.sim,0xc8,std::numeric_limits<uintptr_t>::max());CHECK(fresh(memory,storage).reason==Reason::InvalidPoolLayout);
    reset();memory.pointers[0]=std::numeric_limits<uintptr_t>::max()-0x10;CHECK(fresh(memory,storage).reason==Reason::InvalidAddress);CHECK(memory.reads==2);
    reset();memory.pointers[0]=blockA+1;CHECK(fresh(memory,storage).reason==Reason::InvalidAddress);
    reset();memory.pointers[0]=0x60000000;CHECK(fresh(memory,storage).reason==Reason::Unreadable);
    reset();memory.header(2);memory.pointers[1]=0;CHECK(fresh(memory,storage).observedAllSlots());CHECK(memory.reads==1036);
    // Configured storage capacity is not a promised time/read budget. Even an
    // empty 8192-slot world cannot finish its two passes under 8192 reads.
    reset();memory.header(16);memory.virtualEmptyBlocks=true;
    for(size_t n=0;n<16;++n)memory.pointers[n]=0x50000000+n*blockSize;
    const auto bounded=fresh(memory,storage);CHECK(bounded.reason==Reason::ReadLimit);CHECK(memory.reads==8192);
    reset();memory.header(0);auto empty=fresh(memory,storage);CHECK(empty.observedAllSlots());CHECK(empty.rows==0);CHECK(memory.reads==3);CHECK(storage.untouched());
    reset();memory.id(0,0x00000000fffffffdULL);memory.id(30,0xffff0000001e002aULL);
    auto nonlive=fresh(memory,storage);CHECK(nonlive.observedAllSlots());CHECK(nonlive.rows==0);CHECK(memory.reads==1030);CHECK(storage.untouched());
    CHECK(std::strcmp(inventory::reasonName(Reason::UnsupportedCouplingChain),"unqualified_nested_coupling")==0);
}
void failureBoundaryChecks(Memory& memory,Storage& storage){
    memory.reset();memory.row(0);memory.row(7,train(0),true);storage.sentinel();Gate baseline{memory};
    const auto good=capture(memory,storage,baseline);CHECK(good.observedAllSlots());
    const auto totalReads=memory.reads,totalPreflights=baseline.calls;
    // Every positive read boundary: a denied shared budget prevents this read
    // and every later read, regardless of how little the later read would cost.
    for(size_t refused=1;refused<=totalPreflights;++refused){
        memory.reset();memory.row(0);memory.row(7,train(0),true);storage.sentinel();Gate gate{memory};gate.deny=refused;
        auto result=capture(memory,storage,gate);CHECK(result.reason==Reason::PreflightRefused);
        CHECK(gate.calls==refused);CHECK(memory.reads==gate.readCalls-(gate.lastBytes?1:0));
    }
    // Native read faults are local: they do not fabricate a successful empty
    // list and consume exactly their attempted read, including metadata reads.
    for(size_t refused=1;refused<=totalReads;++refused){
        memory.reset();memory.row(0);memory.row(7,train(0),true);storage.sentinel();memory.denyRead=refused;Gate gate{memory};
        auto result=capture(memory,storage,gate);CHECK(result.reason==Reason::Unreadable);CHECK(memory.reads==refused);
    }
    memory.reset();memory.row(0);storage.sentinel();memory.throwRead=5;Gate throwing{memory};
    CHECK(capture(memory,storage,throwing).reason==Reason::ReadException);CHECK(memory.reads==5);
    // Exact local thresholds succeed, while one missing final read/byte/work
    // fails with output unchanged even after all first-pass rows are available.
    memory.reset();memory.row(0);storage.sentinel();Gate exact{memory};const auto reference=capture(memory,storage,exact);
    for(const auto kind:{0,1,2}){
        inventory::Limits limits;size_t* value=kind==0?&limits.reads:kind==1?&limits.bytes:&limits.work;
        const auto required=kind==0?reference.reads:kind==1?reference.bytes:reference.work;
        *value=required;memory.reset();memory.row(0);CHECK(fresh(memory,storage,limits).observedAllSlots());
        *value=required-1;memory.reset();memory.row(0);auto result=fresh(memory,storage,limits);
        CHECK(result.reason==(kind==0?Reason::ReadLimit:kind==1?Reason::ByteLimit:Reason::WorkLimit));
    }
}
void invalidStorageChecks(Memory& memory,Storage& storage){
    memory.reset();storage.sentinel();Gate gate{memory};inventory::Budget budget{Gate::preflight,&gate};
    const auto reader=[&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);};
    auto result=inventory::captureMotionInventory(simulation,true,reader,budget,storage.output,
        {storage.output,storage.ids});CHECK(result.reason==Reason::OverlappingBuffers);CHECK(memory.reads==0);CHECK(storage.untouched());
    inventory::Budget second{Gate::preflight,&gate};
    result=inventory::captureMotionInventory(simulation,true,reader,second,storage.output,
        {storage.scratch,{reinterpret_cast<uint64_t*>(storage.output.data()),8}});
    CHECK(result.reason==Reason::OverlappingBuffers);CHECK(memory.reads==0);
    inventory::Budget third{Gate::preflight,&gate};
    result=inventory::captureMotionInventory(simulation,true,reader,third,{}, {storage.scratch,storage.ids});
    CHECK(result.reason==Reason::InvalidBuffers);CHECK(memory.reads==0);
    inventory::Budget missing;
    result=inventory::captureMotionInventory(simulation,true,reader,missing,storage.output,{storage.scratch,storage.ids});
    CHECK(result.reason==Reason::InvalidBudget);CHECK(memory.reads==0);
    for(const auto at:{uintptr_t(0),uintptr_t(0xffff),std::numeric_limits<uintptr_t>::max()-0x10}){
        inventory::Budget address{Gate::preflight,&gate};
        result=inventory::captureMotionInventory(at,true,reader,address,storage.output,{storage.scratch,storage.ids});
        CHECK(result.reason==Reason::InvalidAddress);CHECK(memory.reads==0);CHECK(storage.untouched());
    }
}
void sharedBudgetChecks(Memory& memory,Storage& storage){
    using nimby::windows::automatic::RouteReadBudget;
    struct Shared {
        RouteReadBudget budget;int64_t tick=1000;size_t calls=0,expire=0;
        static bool preflight(void* value,size_t bytes)noexcept{
            auto& shared=*static_cast<Shared*>(value);++shared.calls;
            if(shared.calls==shared.expire)shared.tick=3001;
            return bytes?shared.budget.allow(bytes,shared.tick):shared.budget.available(shared.tick);
        }
    };
    const auto run=[&](Shared& shared){
        memory.reset();memory.row(0);storage.sentinel();inventory::Budget budget{Shared::preflight,&shared};
        const auto before=allocations;watchAllocation=true;
        auto result=inventory::captureMotionInventory(simulation,true,
            [&](uintptr_t at,void* value,size_t bytes){return memory.read(at,value,bytes);},budget,
            storage.output,{storage.scratch,storage.ids});
        watchAllocation=false;CHECK(allocations==before);
        if(!result.observedAllSlots())CHECK(storage.untouched());
        return result;
    };
    Shared good{RouteReadBudget(1000,1000000)};
    const auto success=run(good);CHECK(success.observedAllSlots());CHECK(good.budget.reads()==memory.reads);
    CHECK(good.budget.bytes()==memory.readBytes);CHECK(good.calls>memory.reads);
    Shared exhausted{RouteReadBudget(1000,1000000)};exhausted.expire=20;
    CHECK(run(exhausted).reason==Reason::PreflightRefused);CHECK(exhausted.calls==20);
    CHECK(exhausted.budget.reason()==RouteReadBudget::Reason::Time);
    Shared lowBytes{RouteReadBudget(1000,1000000)};
    CHECK(lowBytes.budget.allow(RouteReadBudget::maximumBytes-47,1000));
    CHECK(run(lowBytes).reason==Reason::PreflightRefused);CHECK(memory.reads==0);
    Shared lowCalls{RouteReadBudget(1000,1000000)};
    for(size_t n=0;n<RouteReadBudget::maximumReads;++n)CHECK(lowCalls.budget.allow(1,1000));
    CHECK(run(lowCalls).reason==Reason::PreflightRefused);CHECK(memory.reads==0);
    Shared invalid{RouteReadBudget(1000,0)};CHECK(run(invalid).reason==Reason::PreflightRefused);CHECK(memory.reads==0);
}
void ownedPublicationChecks(Memory& memory,Storage& storage){
    struct Seal {
        size_t calls=0,deny=0;
        static bool validate(void* opaque)noexcept{
            auto& seal=*static_cast<Seal*>(opaque);return ++seal.calls!=seal.deny;
        }
    };
    const auto run=[&](Seal& seal,Gate& gate,inventory::Limits limits={}){
        storage.sentinel();inventory::Budget budget{Gate::preflight,&gate};
        const inventory::OwnedScope scope{Seal::validate,&seal};
        const auto before=allocations;watchAllocation=true;
        const auto result=inventory::captureMotionInventoryOwned(simulation,true,scope,
            [&](uintptr_t at,void* output,size_t bytes){return memory.read(at,output,bytes);},budget,
            storage.output,{storage.scratch,storage.ids},limits);
        watchAllocation=false;CHECK(allocations==before);CHECK(!memory.readAfterRefusal);CHECK(!gate.afterRefusal);
        CHECK(result.relationshipsIndexed==result.observedAllSlots());CHECK(result.ownedPublication==result.observedAllSlots());
        if(!result.observedAllSlots()){CHECK(!result.rows);CHECK(storage.untouched());}
        return result;
    };
    memory.reset();memory.row(0);memory.row(7,train(0),true);memory.row(30);
    Seal good;Gate gate{memory};const auto result=run(good,gate);
    CHECK(result.observedAllSlots());CHECK(result.rows==3&&result.visitedSlots==512&&result.relationshipsIndexed);
    CHECK(result.reads==512+3*3+4);CHECK(memory.headers==2);CHECK(good.calls==2);
    CHECK(storage.output[0].childCount==1&&storage.output[0].firstChild==1);
    CHECK(storage.output[1].parentRow==0&&storage.output[2].attached==0);
    const auto reads=result.reads,bytes=result.bytes,work=result.work;
    memory.reset();memory.row(0);memory.row(7,train(0),true);memory.row(30);
    const auto generic=fresh(memory,storage);CHECK(!generic.ownedPublication);
    CHECK(generic.reads>reads&&generic.bytes>bytes&&generic.work>work);
    memory.reset();Seal missing;missing.deny=1;Gate missingGate{memory};
    CHECK(run(missing,missingGate).reason==Reason::UnqualifiedOwnership);CHECK(!memory.reads);
    memory.reset();memory.row(0);Seal lost;lost.deny=2;Gate lostGate{memory};
    CHECK(run(lost,lostGate).reason==Reason::OwnershipChanged);CHECK(lost.calls==2);
    for(auto quota:{Reason::ReadLimit,Reason::ByteLimit,Reason::WorkLimit}){
        memory.reset();memory.row(0);Seal sealed;Gate quotaGate{memory};inventory::Limits limits;
        if(quota==Reason::ReadLimit)limits.reads=3;
        if(quota==Reason::ByteLimit)limits.bytes=47;
        if(quota==Reason::WorkLimit)limits.work=1;
        CHECK(run(sealed,quotaGate,limits).reason==quota);
    }
    memory.reset();memory.row(0);Seal sealed;Gate denied{memory};denied.deny=8;
    CHECK(run(sealed,denied).reason==Reason::PreflightRefused);CHECK(denied.refused);
    memory.reset();memory.row(0);Seal thrown;Gate thrownGate{memory};memory.throwRead=3;
    CHECK(run(thrown,thrownGate).reason==Reason::ReadException);
    memory.reset();memory.row(0);Seal unreadable;Gate unreadableGate{memory};memory.denyRead=3;
    CHECK(run(unreadable,unreadableGate).reason==Reason::Unreadable);
    memory.reset();memory.row(0);Seal exhausted;Gate exhaustedGate{memory};inventory::Limits rowLimit;rowLimit.rows=1;
    memory.row(1);CHECK(run(exhausted,exhaustedGate,rowLimit).reason==Reason::RowCapacity);
    memory.reset();memory.row(0,train(1),true);Seal parent;Gate parentGate{memory};
    CHECK(run(parent,parentGate).reason==Reason::MissingParent);
    memory.reset();memory.row(0);memory.row(1,train(0),true);memory.row(2,train(1),true);
    Seal chain;Gate chainGate{memory};CHECK(run(chain,chainGate).reason==Reason::UnsupportedCouplingChain);
    memory.reset();memory.row(0);storage.sentinel();inventory::Budget budget{Gate::preflight,&gate};
    const auto noScope=inventory::captureMotionInventoryOwned(simulation,true,{},
        [&](uintptr_t at,void* output,size_t size){return memory.read(at,output,size);},budget,
        storage.output,{storage.scratch,storage.ids});
    CHECK(noScope.reason==Reason::UnqualifiedOwnership);CHECK(!memory.reads);CHECK(storage.untouched());
}
void benchmark(){
    using Clock=std::chrono::steady_clock;
    struct Scene {
        std::array<unsigned char,0xa0+48> sim{};
        std::array<uintptr_t,16> pointers{};
        std::array<std::vector<unsigned char>,16> blocks;
        std::array<Row,4096> output{},scratch{};
        std::array<uint64_t,8192> ids{};
        size_t blockCount{},rows{},reads{};
        Scene(){for(auto& block:blocks)block.resize(blockSize);}
        void prepare(size_t trains){
            sim.fill(0);pointers.fill(0);for(auto& block:blocks)std::fill(block.begin(),block.end(),0);
            rows=trains;blockCount=(trains+slots-1)/slots;
            put(sim,0xa0,uint32_t(blockSize));put(sim,0xa4,uint32_t(9));put(sim,0xa8,uint32_t(slots));
            put(sim,0xac,uint32_t(slots*stride));put(sim,0xb0,uint32_t(slots-1));
            put(sim,0xb8,table);put(sim,0xc0,table+blockCount*8);put(sim,0xc8,table+blockCount*8);
            for(size_t b=0;b<blockCount;++b)pointers[b]=blockA+b*0x1000000;
            for(size_t n=0;n<trains;++n){
                auto& block=blocks[n/slots];const auto at=(n%slots)*stride;
                put(block,at,train(n));put(block,at+0x1d0,uint8_t(1));
            }
        }
        bool read(uintptr_t address,void* target,size_t bytes)noexcept{
            ++reads;
            if(address>=simulation&&address-simulation<=sim.size()&&bytes<=sim.size()-(address-simulation)){
                std::memcpy(target,sim.data()+address-simulation,bytes);return true;
            }
            if(address>=table&&address-table<=sizeof pointers&&bytes<=sizeof pointers-(address-table)){
                std::memcpy(target,reinterpret_cast<const unsigned char*>(pointers.data())+address-table,bytes);return true;
            }
            if(address>=blockA){const auto index=(address-blockA)/0x1000000,offset=(address-blockA)%0x1000000;
                if(index<blockCount&&offset<=blockSize&&bytes<=blockSize-offset){
                    std::memcpy(target,blocks[index].data()+offset,bytes);return true;
                }
            }
            return false;
        }
    };
    struct Shared {
        Clock::time_point started=Clock::now();
        nimby::windows::automatic::RouteReadBudget budget{0,1000000000};
        static bool preflight(void* opaque,size_t bytes)noexcept{
            auto& shared=*static_cast<Shared*>(opaque);
            const auto tick=std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-shared.started).count();
            return bytes?shared.budget.allow(bytes,tick):shared.budget.available(tick);
        }
    };
    static Scene scene;
    std::printf("benchmark: synthetic memcpy reader, owned immutable bytes, inventory/index only; five warm repeats after one warmup; 2ms/8192reads/4MiB, 4096-row cap. Not game or whole body/index latency.\n");
    for(size_t trains:{size_t(51),size_t(500),size_t(5000)}){
        scene.prepare(trains);
        for(size_t repeat=0;repeat<6;++repeat){
            scene.reads=0;Shared shared;inventory::Budget budget{Shared::preflight,&shared};
            const inventory::OwnedScope owned{[](void*)noexcept{return true;},nullptr};
            inventory::Limits limits;limits.rows=4096;
            const auto start=Clock::now();
            const auto before=allocations;watchAllocation=true;
            const auto result=inventory::captureMotionInventoryOwned(simulation,true,owned,
                [&](uintptr_t at,void* output,size_t bytes){return scene.read(at,output,bytes);},budget,
                scene.output,{scene.scratch,scene.ids},limits);
            watchAllocation=false;
            const auto elapsed=std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-start).count();
            CHECK(allocations==before);CHECK(result.reads==scene.reads);
            CHECK(result.observedAllSlots()?result.rows==trains:result.rows==0);
            if(repeat)std::printf("scene=%zu repeat=%zu status=%s reason=%s ns=%lld reads=%zu bytes=%zu work=%zu rows=%zu slots=%zu budgetReason=%u\n",
                trains,repeat,result.observedAllSlots()?"complete":"unknown",inventory::reasonName(result.reason),
                static_cast<long long>(elapsed),result.reads,result.bytes,result.work,result.rows,result.visitedSlots,
                static_cast<unsigned>(shared.budget.reason()));
        }
    }
}
} // namespace fixture
int main(int argc,char** argv){
    if(argc==2&&std::strcmp(argv[1],"--benchmark")==0){fixture::benchmark();return 0;}
    fixture::Memory memory;fixture::Storage storage;
    fixture::require(sizeof(fixture::Row)==56,"sizeof(Row)==56",__LINE__);
    fixture::relationshipChecks(memory,storage);fixture::mutationChecks(memory,storage);
    fixture::limitsChecks(memory,storage);fixture::failureBoundaryChecks(memory,storage);
    fixture::invalidStorageChecks(memory,storage);fixture::sharedBudgetChecks(memory,storage);
    fixture::ownedPublicationChecks(memory,storage);
    std::printf("native_consist_inventory: %zu checks passed (synthetic local diagnostics only)\n",fixture::checks);
    std::printf("Row=%zu bytes; maximum caller output+scratch=%zu bytes; default 1024-row output+scratch=%zu bytes\n",
        sizeof(fixture::Row),size_t(4096*2*sizeof(fixture::Row)+65536*sizeof(uint64_t)),
        size_t(1024*2*sizeof(fixture::Row)+8192*sizeof(uint64_t)));
}
