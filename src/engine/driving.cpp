#include "engine/driving.h"
#include "engine/simulation_clock.h"
#include <array>
#include <cmath>
#include <cstring>

namespace nimby::engine {
namespace {
template<class T> T field(const unsigned char* p, size_t offset) {
    T v{}; std::memcpy(&v,p+offset,sizeof v); return v;
}
enum class Lookup { Found, Missing, Invalid };
// Read one slot from the segmented pool, preserving the generation bits in the ID.
template<size_t N> Lookup record(ReadMemory read, void* ctx, uint64_t pool, uint64_t id,
                                std::array<unsigned char,N>& out) {
    std::array<unsigned char,48> h{},after{};
    if (!read(ctx,pool,h.data(),h.size())) return Lookup::Invalid;
    const auto shift=field<uint32_t>(h.data(),4),size=field<uint32_t>(h.data(),8),mask=field<uint32_t>(h.data(),16);
    const auto begin=field<uint64_t>(h.data(),24),end=field<uint64_t>(h.data(),32),cap=field<uint64_t>(h.data(),40);
    if(shift<1||shift>16||size!=(1u<<shift)||mask!=size-1||begin%8||end<begin||cap<end||
       cap-begin>8192||(end-begin)%8) return Lookup::Invalid;
    const auto index=(id>>16)&0xffffffffULL,blockIndex=index>>shift;
    if(blockIndex>=(end-begin)/8) return Lookup::Missing;
    uint64_t block{},blockAfter{};
    if(!read(ctx,begin+blockIndex*8,&block,8)) return Lookup::Invalid;
    if(!block) return Lookup::Missing;
    if(block<0x10000||block>0x7fffffff0000ULL-size*N||block%8) return Lookup::Invalid;
    const auto address=block+(index&mask)*N;
    if(!read(ctx,address,out.data(),out.size()) ||
       !read(ctx,begin+blockIndex*8,&blockAfter,8)||block!=blockAfter||
       !read(ctx,pool,after.data(),after.size())||h!=after) return Lookup::Invalid;
    return field<uint64_t>(out.data(),0)==id?Lookup::Found:Lookup::Missing;
}
bool dynamics(const unsigned char* p,NimbyTrainDynamics& out) {
    std::array<double,8> v{};
    for(size_t i=0;i<v.size();++i) {
        v[i]=field<float>(p,0x1c+i*4);
        if(!std::isfinite(v[i])||v[i]<0)return false;
    }
    out={v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]};return true;
}
bool position(const unsigned char* p,NimbyTrain& out) {
    const auto track=field<uint64_t>(p,0);
    const auto direction=field<int8_t>(p,16);
    const double fraction=field<double>(p,8);
    if((track>>48)!=1 || !std::isfinite(fraction)||fraction<0||fraction>1 ||
       (static_cast<int32_t>(direction)!=1&&static_cast<int32_t>(direction)!=-1))return false;
    out.track_id=track;out.track_fraction=fraction;out.direction=static_cast<int32_t>(direction);
    out.flags|=NIMBY_TRAIN_POSITION_VALID;return true;
}
}
bool read_train_driving(ReadMemory read,void* ctx,const LiveState& state,uint64_t id,
                        NimbyDrivingObservation& out) noexcept {
    out={};out.struct_size=sizeof out;
    if(!read||(id>>48)!=5)return false;
    // Bounded retries; no waiting or reuse of stale samples.
    for(int attempt=0;attempt<3;++attempt) {
        LiveState before{},after{};
        if(!resolve_live_state(read,ctx,state.module_base,true,state.profile,before)||before!=state)return false;
        SimulationClock start{},end{};
        if(!read_simulation_clock(read,ctx,state.simulation,start))return false;
        std::array<unsigned char,0x178> model{},modelAfter{};
        std::array<unsigned char,0x638> motion{},motionAfter{};
        if(record(read,ctx,state.database+0x200,id,model)!=Lookup::Found)return false;
        const auto motionResult=record(read,ctx,state.simulation+0xa0,id,motion);
        if(motionResult==Lookup::Invalid)continue;
        NimbyDrivingObservation value{};value.struct_size=sizeof value;value.train_id=id;value.train.id=id;
        if(dynamics(model.data()+0xc0,value.purchased))value.flags|=NIMBY_DRIVING_PURCHASED_VALID;
        if(record(read,ctx,state.database+0x200,id,modelAfter)!=Lookup::Found)continue;
        if(std::memcmp(model.data()+0xc0,modelAfter.data()+0xc0,0x44)!=0)continue;
        if(motionResult==Lookup::Found) {
            if(record(read,ctx,state.simulation+0xa0,id,motionAfter)!=Lookup::Found)continue;
            // Ignore unrelated service/cache bytes; validate only the fields we expose.
            if(std::memcmp(motion.data()+8,motionAfter.data()+8,0x44)!=0 ||
               motion[0x4b0]!=motionAfter[0x4b0] || motion[0x1d0]!=motionAfter[0x1d0] ||
               (motion[0x4b0] && std::memcmp(motion.data()+0x3a8,motionAfter.data()+0x3a8,0x28)!=0) ||
               (motion[0x1d0] && std::memcmp(motion.data()+0xb8,motionAfter.data()+0xb8,0x18)!=0))continue;
            if(motion[0x4b0]>1||motion[0x1d0]>1)continue;
            value.flags|=NIMBY_DRIVING_MOTION_VALID;
            if(dynamics(motion.data()+8,value.current))value.flags|=NIMBY_DRIVING_CURRENT_VALID;
            if(motion[0x4b0]) {
                value.train.speed_mps=field<double>(motion.data(),0x3c8);
                if(!std::isfinite(value.train.speed_mps)||value.train.speed_mps<0||value.train.speed_mps>10000)continue;
                value.train.flags|=NIMBY_TRAIN_ACTIVE_DRIVE|NIMBY_TRAIN_SPEED_VALID;
                position(motion.data()+0x3a8,value.train);
            } else value.train.flags|=NIMBY_TRAIN_SPEED_VALID|NIMBY_TRAIN_SPEED_DEFAULTED;
            if(!(value.train.flags&NIMBY_TRAIN_POSITION_VALID)&&motion[0x1d0])position(motion.data()+0xb8,value.train);
        } else {
            if(record(read,ctx,state.simulation+0xa0,id,motionAfter)!=Lookup::Missing)continue;
        }
        if(!read_simulation_clock(read,ctx,state.simulation,end)||end.epoch_seconds!=start.epoch_seconds||end.ticks<start.ticks)continue;
        if(!resolve_live_state(read,ctx,state.module_base,true,state.profile,after)||after!=state)return false;
        value.elapsed_begin_ms=start.ticks*10;value.elapsed_end_ms=end.ticks*10;
        out=value;return true;
    }
    return false;
}
}
