#include <engine/track_conflicts.h>
#include <cstdio>
#include <functional>
#include <map>
#include <limits>
#include <cstdlib>
#include <new>
using namespace nimby::engine;
namespace allocation_fixture {
bool exhausted=false,watch=false;size_t afterExhaustion=0,total=0,readsAfterExhaustion=0;
}
#ifdef __GNUC__
#define FIXTURE_NOINLINE __attribute__((noinline))
#else
#define FIXTURE_NOINLINE __declspec(noinline)
#endif
FIXTURE_NOINLINE void* operator new(size_t size){
    if(allocation_fixture::watch){++allocation_fixture::total;
        if(allocation_fixture::exhausted)++allocation_fixture::afterExhaustion;}
    if(auto* result=std::malloc(size?size:1))return result;
    throw std::bad_alloc();
}
FIXTURE_NOINLINE void* operator new[](size_t size){return ::operator new(size);}
FIXTURE_NOINLINE void operator delete(void* value)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete(void* value,size_t)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete[](void* value,size_t)noexcept{std::free(value);}
#undef FIXTURE_NOINLINE
namespace {
constexpr uint64_t a=0x1000000000001,b=0x1000000010001,c=0x1000000020001,missing=0x1000000030001;
constexpr uint64_t table=0x60000000,block=0x60010000,vector=0x61000000;
constexpr uint64_t addressA=block,addressB=block+0x4e8,addressC=block+2*0x4e8;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> regions;
    std::map<std::pair<uint64_t,size_t>,unsigned> visits;
    uint64_t refused{};unsigned reads{};
    std::function<void(uint64_t,void*,size_t,unsigned)> afterRead;
    template<class T>void put(uint64_t address,size_t offset,T value){
        auto& bytes=regions[address];if(bytes.size()<offset+sizeof value)bytes.resize(offset+sizeof value);
        std::memcpy(bytes.data()+offset,&value,sizeof value);
    }
};
bool read(void* context,uint64_t address,void* out,size_t size){
    if(allocation_fixture::watch&&allocation_fixture::exhausted)++allocation_fixture::readsAfterExhaustion;
    auto& memory=*static_cast<Memory*>(context);++memory.reads;
    if(address==memory.refused)return false;
    auto found=memory.regions.upper_bound(address);if(found==memory.regions.begin())return false;--found;
    const auto offset=address-found->first;
    if(offset>found->second.size()||size>found->second.size()-offset)return false;
    std::memcpy(out,found->second.data()+offset,size);
    const auto visit=++memory.visits[{address,size}];
    if(memory.afterRead)memory.afterRead(address,out,size,visit);
    return true;
}
struct Fixture {
    Memory memory;LiveState state{0x140000000,0x20000000,0x30000000,0x40000000,0x50000000};
    Fixture(){
        memory.put(state.module_base+0xb81998,0,state.root);memory.put(state.root,0x540,state.database);
        memory.put(state.root,0x5c0,state.copy);memory.put(state.root,0x680,state.simulation);
        memory.put(state.database,4,uint32_t(2));memory.put(state.database,8,uint32_t(4));memory.put(state.database,16,uint32_t(3));
        memory.put(state.database,24,table);memory.put(state.database,32,table+8);memory.put(state.database,40,table+8);memory.put(table,0,block);
        for(auto [id,address]:{std::pair{a,addressA},std::pair{b,addressB},std::pair{c,addressC}}){
            memory.regions[address].resize(0x110);memory.put(address,0,id);
        }
        relation();
    }
    void relation(){
        memory.regions[vector].resize(0x30);memory.put(addressA,0xf8,vector);memory.put(addressA,0x100,vector+0x30);memory.put(addressA,0x108,vector+0x30);
        memory.put(vector,0,b);memory.put(vector,0x18,.2f);memory.put(vector,0x1c,.6f);memory.put(vector,0x20,.8f);memory.put(vector,0x24,.3f);
    }
    bool capture(TrackConflicts& out,std::span<const uint64_t> ids=std::span(&a,1)){return read_track_conflicts(read,&memory,state,true,ids,out);}
};
struct Budget {
    bool exhausted=false;size_t maximumRead=std::numeric_limits<size_t>::max(),checks=0;
    static bool available(void* context,size_t bytes){
        auto& budget=*static_cast<Budget*>(context);++budget.checks;
        if(budget.exhausted||bytes>budget.maximumRead){budget.exhausted=true;
            allocation_fixture::exhausted=true;return false;}
        return true;
    }
};
void beginAllocationWatch(bool exhausted=false){
    allocation_fixture::exhausted=exhausted;allocation_fixture::afterExhaustion=0;
    allocation_fixture::total=0;allocation_fixture::readsAfterExhaustion=0;allocation_fixture::watch=true;
}
void endAllocationWatch(){allocation_fixture::watch=false;allocation_fixture::exhausted=false;}
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(){
    static_assert(sizeof(NimbyTrackConflict)==48&&sizeof(NimbyTrackConflictState)==16);
    TrackConflicts out;
    {Fixture f;const uint64_t ids[]{a,b,a,missing};CHECK(f.capture(out,ids));
     CHECK(out.states.size()==3&&out.rows.size()==1&&out.states[0].flags==1&&out.states[1].flags==1&&out.states[2].flags==0);
     CHECK(out.states[2].reason==NIMBY_TRACK_CONFLICTS_TRACK_UNAVAILABLE);
     CHECK(out.rows[0].other_track_id==b&&out.rows[0].begin_fraction==double(.2f)&&out.rows[0].other_begin_fraction==double(.3f)&&out.rows[0].other_end_fraction==double(.8f));
     CHECK(f.memory.reads<60); // Targeted slots only, no pool/world scan.
    }
    {Fixture f;f.memory.put(addressA,0xe0,uint64_t(0xffff));f.memory.put(addressA,0x110,uint64_t(0xffff));f.memory.put(addressA,0x128,uint64_t(0xffff));CHECK(f.capture(out)&&out.states[0].flags==1);}
    {Fixture f;f.memory.put(vector,0,uint64_t(0));f.memory.put(vector,0x18,std::numeric_limits<float>::quiet_NaN());CHECK(f.capture(out)&&out.states[0].flags==1&&out.rows.empty());}
    for(float bad:{-.1f,1.1f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()}){
        Fixture f;f.memory.put(vector,0x24,bad);CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_INTERVAL_INVALID&&out.rows.empty());
    }
    {Fixture f;f.memory.put(vector,0x18,.7f);CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_INTERVAL_INVALID);}
    {Fixture f;f.memory.put(addressB,0,b+1);const uint64_t ids[]{a,c};CHECK(f.capture(out,ids)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_REFERENCE_UNAVAILABLE&&out.states[1].flags==1&&out.rows.empty());}
    {Fixture f;f.memory.put(vector,0,uint64_t(0x5000000000001));CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_REFERENCE_UNAVAILABLE);}
    {Fixture f;f.memory.refused=vector;const uint64_t ids[]{a,c};CHECK(f.capture(out,ids)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_READ_UNAVAILABLE&&out.states[1].flags==1);}
    for(uint64_t target:{vector,addressA}){Fixture f;f.memory.afterRead=[=](auto address,void* bytes,auto size,auto visit){if(address==target&&visit==2&&size!=8)static_cast<unsigned char*>(bytes)[0]^=1;};CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_UNSTABLE&&out.rows.empty());}
    {Fixture f;f.memory.afterRead=[](auto address,void* bytes,auto size,auto visit){if(address==addressB&&size==8&&visit==2)static_cast<unsigned char*>(bytes)[0]^=1;};CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_REFERENCE_UNAVAILABLE&&out.rows.empty());}
    {Fixture f;f.memory.afterRead=[](auto address,void* bytes,auto size,auto visit){if(address==addressA&&size==8&&visit==2)static_cast<unsigned char*>(bytes)[0]^=1;};CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_UNSTABLE&&out.rows.empty());}
    for(uint64_t target:{uint64_t(0x30000000),table}){Fixture f;f.memory.afterRead=[=](auto address,void* bytes,auto,auto visit){if(address==target&&visit==2)static_cast<unsigned char*>(bytes)[0]^=1;};CHECK(!f.capture(out)&&out.states.empty()&&out.rows.empty());}
    {Fixture f;f.memory.put(addressA,0x108,vector+4097*0x30);CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_DESCRIPTOR_INVALID);}
    {Fixture f;f.memory.put(addressA,0x100,vector+0x2f);CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_DESCRIPTOR_INVALID);}
    {Fixture f;f.memory.put(addressA,0xf8,uint64_t(0));CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_DESCRIPTOR_INVALID);}
    {Fixture f;size_t budget=0;std::vector<NimbyTrackConflict> rows;CHECK(decode_track_conflicts(read,&f.memory,a,addressA,[](auto){return true;},budget,rows)==TrackConflictReason::BudgetExceeded&&rows.empty());CHECK(!f.memory.visits.contains({vector,0x30}));}
    {Fixture f;std::vector<uint64_t> ids;
     f.memory.put(f.state.database,4,uint32_t(6));f.memory.put(f.state.database,8,uint32_t(64));f.memory.put(f.state.database,16,uint32_t(63));
     f.memory.regions[vector].assign(4096*0x30,0); // Native zero-ID tombstones still consume read budget.
     for(uint64_t i=0;i<33;++i){const auto id=a+(i<<16),address=block+i*0x4e8;ids.push_back(id);
        f.memory.regions[address].resize(0x110);f.memory.put(address,0,id);
        f.memory.put(address,0xf8,vector);f.memory.put(address,0x100,vector+4096*0x30);f.memory.put(address,0x108,vector+4096*0x30);
     }
     CHECK(f.capture(out,ids)&&out.states.size()==33&&out.rows.empty());
     for(size_t i=0;i<32;++i)CHECK(out.states[i].flags==1);
     CHECK(out.states.back().flags==0&&out.states.back().reason==NIMBY_TRACK_CONFLICTS_BUDGET_EXCEEDED);
     CHECK(f.memory.visits.at({vector,4096*0x30})==64); // The 33rd vector is never read.
    }
    {Fixture f;f.memory.put(addressA,0,a+1);CHECK(f.capture(out)&&out.states[0].reason==NIMBY_TRACK_CONFLICTS_TRACK_UNAVAILABLE);}
    {Fixture f;f.memory.put(f.state.root,0x5c0,f.state.copy+8);CHECK(!f.capture(out)&&out.states.empty());}
    {Fixture f;f.memory.reads=0;CHECK(!read_track_conflicts(read,&f.memory,f.state,false,std::span(&a,1),out)&&f.memory.reads==0);
     auto unsupported=f.state;unsupported.profile=LiveStateProfile::Linux119;CHECK(!read_track_conflicts(read,&f.memory,unsupported,true,std::span(&a,1),out)&&f.memory.reads==0);
     const uint64_t invalid=0x5000000000001;CHECK(!f.capture(out,std::span(&invalid,1))&&f.memory.reads==0);
     std::vector<uint64_t> tooMany(4097,a);CHECK(!f.capture(out,tooMany)&&f.memory.reads==0);
    }
    // A refused preflight does not sample memory, grow scratch, or construct
    // a misleading valid-empty list for any requested ID.
    {Fixture f;Budget limited;limited.exhausted=true;
     TrackConflictReadBudget gate{Budget::available,&limited};const uint64_t ids[]{a,b,c};
     beginAllocationWatch(true);
     const bool captured=read_track_conflicts(read,&f.memory,f.state,true,ids,out,&gate);
     endAllocationWatch();CHECK(!captured&&out.states.empty()&&out.rows.empty()&&gate.exhausted);
     CHECK(f.memory.reads==0&&allocation_fixture::total==0&&limited.checks==1);
    }
    {Fixture f;f.memory.refused=vector;Budget limited;TrackConflictReadBudget gate{Budget::available,&limited};
     const uint64_t ids[]{a,c};CHECK(read_track_conflicts(read,&f.memory,f.state,true,ids,out,&gate));
     CHECK(!gate.exhausted&&!limited.exhausted&&out.states.size()==2&&out.states[0].flags==0&&out.states[1].flags==1);
     CHECK(out.states[0].reason==NIMBY_TRACK_CONFLICTS_READ_UNAVAILABLE);
    }
    // Expire on a middle-list read after the first list was captured. No later
    // ID, identity readback, vector allocation or map insertion may proceed;
    // even the earlier complete list is discarded as an unqualified batch.
    {Fixture f;f.memory.put(addressB,0xf8,vector);f.memory.put(addressB,0x100,vector+0x30);f.memory.put(addressB,0x108,vector+0x30);
     Budget limited;TrackConflictReadBudget gate{Budget::available,&limited};const uint64_t ids[]{a,b,c};
     f.memory.afterRead=[&](auto address,void*,auto size,auto visit){
         if(address==vector&&size==0x30&&visit==3){limited.exhausted=true;allocation_fixture::exhausted=true;}};
     beginAllocationWatch();const bool captured=read_track_conflicts(read,&f.memory,f.state,true,ids,out,&gate);
     endAllocationWatch();CHECK(!captured&&out.states.empty()&&out.rows.empty()&&gate.exhausted);
     CHECK(f.memory.visits.at({vector,0x30})==3&&!f.memory.visits.contains({addressC,8}));
     CHECK(allocation_fixture::afterExhaustion==0&&allocation_fixture::readsAfterExhaustion==0);
     const auto checks=limited.checks;CHECK(!gate.available(1)&&limited.checks==checks); // Terminal gate, no callbacks.
    }
    // The planned read size is refused BEFORE allocating the two-list buffer,
    // including a list with 4096 tombstones whose references need no reads.
    {Fixture f;f.memory.regions[vector].assign(4096*0x30,0);
     f.memory.put(addressA,0x100,vector+4096*0x30);f.memory.put(addressA,0x108,vector+4096*0x30);
     Budget limited;limited.maximumRead=4096;TrackConflictReadBudget gate{Budget::available,&limited};
     TrackConflictDecodeWorkspace workspace;std::vector<NimbyTrackConflict> rows;size_t remaining=131072;
     beginAllocationWatch();const auto reason=decode_track_conflicts(read,&f.memory,a,addressA,[](auto){return true;},remaining,rows,workspace,&gate);
     endAllocationWatch();CHECK(reason==TrackConflictReason::BudgetExceeded&&rows.empty()&&rows.capacity()==0&&workspace.bytes.empty());
     CHECK(!f.memory.visits.contains({vector,4096*0x30})&&allocation_fixture::afterExhaustion==0&&allocation_fixture::readsAfterExhaustion==0);
    }
    // Scratch and decoded-row storage survive successful IDs; the next list
    // of the same bounded size performs no allocation at all. Warm the read
    // fixture separately so its visit counters cannot mask this assertion.
    {Fixture f;size_t remaining=131072;std::vector<NimbyTrackConflict> rows;TrackConflictDecodeWorkspace workspace;
     CHECK(decode_track_conflicts(read,&f.memory,a,addressA,[](auto){return true;},remaining,rows,workspace)==TrackConflictReason::Complete);
     const auto scratch=workspace.bytes.data();const auto decoded=rows.data();const auto capacity=workspace.bytes.capacity();
     beginAllocationWatch();
     const auto reason=decode_track_conflicts(read,&f.memory,a,addressA,[](auto){return true;},remaining,rows,workspace);
     endAllocationWatch();CHECK(reason==TrackConflictReason::Complete&&rows.size()==1&&allocation_fixture::total==0);
     CHECK(workspace.bytes.data()==scratch&&rows.data()==decoded&&workspace.bytes.capacity()==capacity&&capacity<=2*4096*0x30);
    }
    std::puts("PASS native conflict intervals, empty/unknown distinction, local isolation, identity/stability and bounded targeted reads.");
}
