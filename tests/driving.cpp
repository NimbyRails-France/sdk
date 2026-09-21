#include "engine/driving.h"
#include <array>
#include <map>
#include <vector>
#include <cstring>
#include <cstdio>
#include <limits>

struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> blocks;
    bool unstable=false;
    unsigned motionReads=0, reads=0;
    template<class T> void put(uint64_t address,T value) {
        auto& bytes=blocks[address];bytes.resize(sizeof value);std::memcpy(bytes.data(),&value,sizeof value);
    }
};
template<class T,size_t N> void put(std::array<unsigned char,N>& bytes,size_t offset,T value) {
    std::memcpy(bytes.data()+offset,&value,sizeof value);
}
bool read(void* ctx,uint64_t address,void* out,size_t size) {
    auto& m=*static_cast<Memory*>(ctx);++m.reads;
    auto it=m.blocks.upper_bound(address);if(it==m.blocks.begin())return false;--it;
    if(address-it->first>it->second.size()||size>it->second.size()-(address-it->first))return false;
    std::memcpy(out,it->second.data()+(address-it->first),size);
    if(address==0x700000&&size==0x638&&m.unstable) {
        const double speed=++m.motionReads;std::memcpy(static_cast<unsigned char*>(out)+0x3c8,&speed,8);
    }
    return true;
}
constexpr uint64_t id=0x5000000000001ULL;
Memory fixture() {
    Memory m;
    m.put<uint64_t>(0x140b81998,0x200000);
    m.put<uint64_t>(0x200540,0x300000);m.put<uint64_t>(0x2005c0,0x400000);m.put<uint64_t>(0x200680,0x500000);
    m.put(0x500020,std::array<int64_t,2>{1700000000,1000});
    for(auto [pool,table,block]:{std::array<uint64_t,3>{0x300200,0x800000,0x600000},
                               std::array<uint64_t,3>{0x5000a0,0x900000,0x700000}}) {
        std::array<unsigned char,48> h{};put<uint32_t>(h,4,1);put<uint32_t>(h,8,2);put<uint32_t>(h,16,1);
        put(h,24,table);put(h,32,table+8);put(h,40,table+8);m.put(pool,h);m.put(table,block);
    }
    std::array<unsigned char,0x178> model{};std::array<unsigned char,0x638> motion{};
    put(model,0,id);put(motion,0,id);
    constexpr float values[]{44.444444f,1,0.65f,1.1f,160000,1900000,163200,72.36f};
    for(size_t i=0;i<8;++i){put(model,0xc0+0x1c+4*i,values[i]);put(motion,8+0x1c+4*i,values[i]);}
    motion[0x4b0]=1;put<double>(motion,0x3c8,20);put<uint64_t>(motion,0x3a8,0x1000000000001);
    put<double>(motion,0x3b0,0.5);put<int8_t>(motion,0x3b8,-1);
    m.put(0x600000,model);m.put(0x700000,motion);return m;
}
#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main() {
    auto m=fixture();nimby::engine::LiveState state{};
    CHECK(nimby::engine::resolve_live_state(read,&m,0x140000000,true,state));
    NimbyDrivingObservation out{};
    CHECK(nimby::engine::read_train_driving(read,&m,state,id,out));
    CHECK(out.flags==7&&out.current.length_m>72&&out.train.direction==-1&&out.train.speed_mps==20);
    CHECK(out.elapsed_begin_ms==10000&&out.elapsed_end_ms==10000);
    CHECK(!nimby::engine::read_train_driving(read,&m,state,id+1,out)&&out.flags==0);
    m.unstable=true;CHECK(!nimby::engine::read_train_driving(read,&m,state,id,out)&&out.flags==0);
    m=fixture();m.blocks[0x700000][0x4b0]=0;
    CHECK(nimby::engine::read_train_driving(read,&m,state,id,out));
    CHECK(out.train.speed_mps==0&&(out.train.flags&NIMBY_TRAIN_SPEED_DEFAULTED)&&!(out.train.flags&NIMBY_TRAIN_POSITION_VALID));
    m=fixture();float invalid=std::numeric_limits<float>::quiet_NaN();
    std::memcpy(m.blocks[0x700000].data()+8+0x24,&invalid,4);
    CHECK(nimby::engine::read_train_driving(read,&m,state,id,out));
    CHECK(!(out.flags&NIMBY_DRIVING_CURRENT_VALID)&&(out.flags&NIMBY_DRIVING_PURCHASED_VALID));
    m=fixture();m.put<uint64_t>(0x200680,0x510000);
    CHECK(!nimby::engine::read_train_driving(read,&m,state,id,out)&&out.flags==0);
    m=fixture();m.put<uint64_t>(0x900000,0);
    CHECK(nimby::engine::read_train_driving(read,&m,state,id,out)&&out.flags==NIMBY_DRIVING_PURCHASED_VALID);
    CHECK(!(out.train.flags&NIMBY_TRAIN_SPEED_VALID));
    std::puts("PASS targeted read, identity, movement race, missing motion, defaults, NaN and session replacement");
}
