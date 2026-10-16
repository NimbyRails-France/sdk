#include <engine/track_endpoints.h>
#include <cstdio>
#include <cstring>
#include <functional>
#include <limits>
#include <map>
using namespace nimby::engine;
namespace {
constexpr uint64_t a=0x1000000000001,b=0x1000000010001,c=0x1000000020001,missing=0x1000000030001;
constexpr uint64_t table=0x60000000,block=0x60010000,children=0x61000000;
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
        memory.put(state.database,4,uint32_t(6));memory.put(state.database,8,uint32_t(64));memory.put(state.database,16,uint32_t(63));
        memory.put(state.database,24,table);memory.put(state.database,32,table+8);memory.put(state.database,40,table+8);memory.put(table,0,block);
        for(auto [id,address]:{std::pair{a,addressA},std::pair{b,addressB},std::pair{c,addressC}}){
            memory.regions[address].resize(0x420);memory.put(address,0,id);
        }
    }
    void branch(double fraction=1,int direction=1){
        memory.put(addressA,0x408,children);memory.put(addressA,0x410,children+8);memory.put(addressA,0x418,children+8);
        memory.put(children,0,b);memory.put(addressB,0x3f0,a);memory.put(addressB,0x3f8,fraction);
        memory.put(addressB,0x400,int32_t(direction));memory.put(addressB,16,c);
    }
    bool capture(std::vector<NimbyTrackEndpointStates>& out,std::span<const uint64_t> ids=std::span(&a,1)){
        return read_track_endpoints(read,&memory,state,true,ids,out);
    }
};
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(){
    static_assert(sizeof(NimbyTrackEndpointStates)==32&&sizeof(NimbyTrackNode)==40);
    std::vector<NimbyTrackEndpointStates> out;
    {Fixture f;const uint64_t ids[]{a,b,a,missing};CHECK(f.capture(out,ids));
     CHECK(out.size()==3&&out[0].state_a==2&&out[0].state_b==2&&out[1].state_a==2&&out[2].state_a==0);
     CHECK(out[2].reason_a==NIMBY_TRACK_ENDPOINT_TRACK_UNAVAILABLE&&f.memory.reads<65);}
    {Fixture f;f.memory.put(addressA,0xd0,uint64_t(0x2000000000001));CHECK(f.capture(out));
     CHECK(out[0].station_id==0x2000000000001&&out[0].state_a==2&&out[0].state_b==2);}
    {Fixture f;f.memory.put(addressA,8,b);f.memory.put(addressA,16,c);f.memory.put(addressA,0xd0,uint64_t(0x2000000000001));
     CHECK(f.capture(out)&&out[0].state_a==1&&out[0].state_b==1);}
    {Fixture f;f.memory.put(addressA,8,missing);CHECK(f.capture(out)&&out[0].state_a==0&&out[0].state_b==2);
     CHECK(out[0].reason_a==NIMBY_TRACK_ENDPOINT_REFERENCE_UNAVAILABLE);}
    for(unsigned side:{0u,1u}){Fixture f;f.branch(double(side),side?1:-1);CHECK(f.capture(out));
        CHECK((side?out[0].state_b:out[0].state_a)==1&&(side?out[0].state_a:out[0].state_b)==2);}
    {Fixture f;f.branch(.5,1);CHECK(f.capture(out)&&out[0].state_a==2&&out[0].state_b==2);}
    {Fixture f;f.branch(1,-1);CHECK(f.capture(out)&&out[0].state_b==2);}
    {Fixture f;f.branch();CHECK(f.capture(out,std::span(&b,1))&&out[0].state_a==1&&out[0].state_b==1);}
    {Fixture f;f.branch();f.memory.put(addressB,16,uint64_t(0));CHECK(f.capture(out)&&out[0].state_a==0&&out[0].state_b==0);
     CHECK(f.capture(out,std::span(&b,1))&&out[0].state_a==0&&out[0].state_b==0);}
    {Fixture f;f.branch();f.memory.put(addressB,0x3f0,c);CHECK(f.capture(out)&&out[0].reason_b==NIMBY_TRACK_ENDPOINT_ATTACHMENT_INVALID);}
    {Fixture f;f.branch();f.memory.put(children,0,c);CHECK(f.capture(out,std::span(&b,1))&&out[0].state_a==0&&out[0].state_b==1);}
    {Fixture f;f.branch();f.memory.put(addressB,0,b+1);const uint64_t ids[]{a,c};CHECK(f.capture(out,ids)&&out[0].state_b==0&&out[1].state_b==2);
     CHECK(out[0].reason_b==NIMBY_TRACK_ENDPOINT_REFERENCE_UNAVAILABLE);}
    for(double bad:{-0.1,1.1,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){
        Fixture f;f.branch(bad);CHECK(f.capture(out)&&out[0].reason_b==NIMBY_TRACK_ENDPOINT_ATTACHMENT_INVALID);
    }
    {Fixture f;f.branch(1,0);CHECK(f.capture(out)&&out[0].state_b==0);}
    {Fixture f;f.branch();f.memory.refused=children;CHECK(f.capture(out)&&out[0].reason_b==NIMBY_TRACK_ENDPOINT_READ_UNAVAILABLE);}
    {Fixture f;f.memory.put(addressA,0x408,children);f.memory.put(addressA,0x410,children);f.memory.put(addressA,0x418,children+8);
     CHECK(f.capture(out)&&out[0].state_a==2&&out[0].state_b==2&&!f.memory.visits.contains({children,8}));}
    for(uint64_t delta:{uint64_t(9),uint64_t(4097*8)}){Fixture f;f.memory.put(addressA,0x408,children);f.memory.put(addressA,0x410,children+delta);f.memory.put(addressA,0x418,children+delta);
        CHECK(f.capture(out)&&out[0].reason_b==NIMBY_TRACK_ENDPOINT_DESCRIPTOR_INVALID);}
    {Fixture f;f.branch();f.memory.put(addressA,0x410,children+16);f.memory.put(addressA,0x418,children+16);f.memory.put(children,8,b);
     CHECK(f.capture(out)&&out[0].reason_b==NIMBY_TRACK_ENDPOINT_ATTACHMENT_INVALID);}
    // Source absence, generation, attachment and list contents must survive
    // the dependent reads; one unstable row never poisons unrelated endpoints.
    for(uint64_t target:{addressA,addressA+0x3f0,addressB,addressB+0x3f0,children}){
        Fixture f;f.branch();f.memory.afterRead=[=](auto address,void* bytes,auto,auto visit){if(address==target&&visit==2)static_cast<unsigned char*>(bytes)[0]^=1;};
        const uint64_t ids[]{a,c};CHECK(f.capture(out,ids)&&out[0].state_b==0&&out[0].reason_b==NIMBY_TRACK_ENDPOINT_UNSTABLE&&out[1].state_b==2);
    }
    {Fixture f;f.memory.afterRead=[](auto address,void* bytes,auto,auto visit){if(address==addressA+0xd0&&visit==2)static_cast<unsigned char*>(bytes)[0]^=1;};
     CHECK(f.capture(out)&&out[0].state_a==0&&out[0].reason_a==NIMBY_TRACK_ENDPOINT_UNSTABLE);}
    for(uint64_t target:{uint64_t(0x30000000),table}){Fixture f;f.memory.afterRead=[=](auto address,void* bytes,auto,auto visit){if(address==target&&visit==2)static_cast<unsigned char*>(bytes)[0]^=1;};CHECK(!f.capture(out)&&out.empty());}
    {Fixture f;f.memory.put(f.state.root,0x5c0,f.state.copy+8);CHECK(!f.capture(out)&&out.empty());}
    {Fixture f;std::vector<uint64_t> ids;for(unsigned index=0;index<33;++index){
        const auto id=a+(uint64_t(index)<<16),address=block+index*0x4e8;ids.push_back(id);
        f.memory.regions[address].resize(0x420);f.memory.put(address,0,id);
        const auto list=children+index*0x10000;f.memory.regions[list].resize(4096*8);
        f.memory.put(address,0x408,list);f.memory.put(address,0x410,list+4096*8);f.memory.put(address,0x418,list+4096*8);
      }
      CHECK(f.capture(out,ids)&&out.back().reason_a==NIMBY_TRACK_ENDPOINT_BUDGET_EXCEEDED);
      CHECK(!f.memory.visits.contains({children+32*0x10000,4096*8}));}
    {Fixture f;CHECK(!read_track_endpoints(read,&f.memory,f.state,false,std::span(&a,1),out)&&f.memory.reads==0);
     auto unsupported=f.state;unsupported.profile=LiveStateProfile::Linux119;CHECK(!read_track_endpoints(read,&f.memory,unsupported,true,std::span(&a,1),out)&&f.memory.reads==0);
     const uint64_t invalid=0x5000000000001;CHECK(!f.capture(out,std::span(&invalid,1))&&f.memory.reads==0);
     std::vector<uint64_t> tooMany(4097,a);CHECK(!f.capture(out,tooMany)&&f.memory.reads==0);}
    std::puts("PASS directed terminals, parent/endpoint forks, unknown references, stability, local isolation and targeted budgets.");
}
