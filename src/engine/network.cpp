#include "engine/network.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <map>
#include <unordered_set>
namespace nimby::engine {
namespace {
template<class T> T field(const void* p,size_t off) { T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof v);return v; }
struct Collection { std::array<unsigned char,48> header{}; std::vector<uint64_t> blocks; };
bool pointer(uint64_t p) { return p>=0x10000 && p<0x7fffffff0000ULL && p%8==0; }
template<class F> bool collect(ReadMemory read,void* ctx,uint64_t address,uint64_t tag,size_t stride,F consume) {
    Collection pool;
    if(!read(ctx,address,pool.header.data(),48)) return false;
    const auto shift=field<uint32_t>(pool.header.data(),4),size=field<uint32_t>(pool.header.data(),8),mask=field<uint32_t>(pool.header.data(),16);
    const auto begin=field<uint64_t>(pool.header.data(),24),end=field<uint64_t>(pool.header.data(),32),cap=field<uint64_t>(pool.header.data(),40);
    if(shift==0||shift>16||size!=(1u<<shift)||mask!=size-1||end<begin||cap<end||cap-begin>8192||(end-begin)%8) return false;
    if(end!=begin&&!pointer(begin)) return false;
    pool.blocks.resize((end-begin)/8);
    // Capacity includes unused slots. Large saves observed with 486 * 512 slots.
    // Bound allocation/work independently of active object count; read one block at a time.
    if(pool.blocks.size()*size>1048576 || (!pool.blocks.empty()&&!read(ctx,begin,pool.blocks.data(),pool.blocks.size()*8))) return false;
    std::vector<unsigned char> bytes(size*stride);
    for(size_t b=0;b<pool.blocks.size();++b) {
        if(!pointer(pool.blocks[b])||!read(ctx,pool.blocks[b],bytes.data(),bytes.size())) return false;
        for(size_t slot=0;slot<size;++slot) {
            const auto* p=bytes.data()+slot*stride; const auto id=field<uint64_t>(p,0);
            if((id>>48)==0||(id>>48)==0xffff) continue;
            if((id>>48)!=tag||((id>>16)&0xffffffffULL)!=(b<<shift)+slot||!consume(p,pool.blocks[b]+slot*stride)) return false;
        }
    }
    Collection after;after.blocks.resize(pool.blocks.size());
    return read(ctx,address,after.header.data(),48)&&after.header==pool.header&&
        (after.blocks.empty()||(read(ctx,begin,after.blocks.data(),after.blocks.size()*8)&&after.blocks==pool.blocks));
}
bool position(const void* p,TrainPosition& out) {
    out={field<uint64_t>(p,0),field<double>(p,8),field<int8_t>(p,16)};
    return (out.track_id>>48)==1&&std::isfinite(out.fraction)&&out.fraction>=0&&out.fraction<=1&&(out.direction==1||out.direction==-1);
}
}
bool decode_train_position(const void* motion,size_t size,TrainPosition& out) noexcept {
    out={};if(!motion||size<0x4b1) return false;
    const auto* p=static_cast<const unsigned char*>(motion);
    if(p[0x4b0]!=1) return false;
    TrainPosition candidate;
    if(!position(p+0x3a8,candidate)) return false;
    out=candidate;return true;
}
const char* signal_kind_name(int kind) noexcept {
    switch(kind) {case 0:return "OneWay";case 1:return "PlatformStop";case 3:return "Balise";case 4:return "Path";case 5:return "NoWay";case 6:return "Marker";default:return "Unknown";}
}
bool read_trains(ReadMemory read,void* context,const LiveState& state,bool recognized,std::vector<Train>& out) noexcept {
    out.clear();if(!read||!recognized)return false;
    try {
        LiveState current;
        if(!resolve_live_state(read,context,state.module_base,true,current)||current!=state)return false;
        std::map<uint64_t,Train> trains;
        size_t path_budget=1048576;
        if(!collect(read,context,state.database+0x200,5,0x178,[&](const unsigned char* p,uint64_t){
            Train t;t.id=field<uint64_t>(p,0);
            const auto length=field<uint64_t>(p,0x20),capacity=field<uint64_t>(p,0x28);
            if(length>256||capacity<15||capacity<length||capacity>4096)return false;
            std::array<char,257> text{};
            if(capacity<=15)std::memcpy(text.data(),p+0x10,length+1);
            else if(!pointer(field<uint64_t>(p,0x10))||!read(context,field<uint64_t>(p,0x10),text.data(),length+1))return false;
            if(text[length])return false;
            t.name.assign(text.data(),length);trains.emplace(t.id,std::move(t));return true;
        }))return false;
        if(!collect(read,context,state.simulation+0xa0,5,0x638,[&](const unsigned char* p,uint64_t address){
            auto it=trains.find(field<uint64_t>(p,0));if(it==trains.end()||p[0x4b0]>1)return false;
            auto& t=it->second;t.present=p[0x4b0]==1;
            if(t.present){t.speed_mps=field<double>(p,0x3c8);if(!std::isfinite(t.speed_mps)||std::abs(t.speed_mps)>10000)return false;
                t.positioned=decode_train_position(p,0x638,t.position);
                // Serializer chain Motion -> Drive -> Path, see train-paths.md.
                const auto begin=field<uint64_t>(p,0x338),end=field<uint64_t>(p,0x340),cap=field<uint64_t>(p,0x348);
                if(p[0x320]==1&&end>=begin&&cap>=end&&(end-begin)%8==0&&end-begin<=16384*8&&cap-begin<=65536*8&&
                   (end==begin||pointer(begin))&&(end-begin)/8<=path_budget){
                    std::vector<uint64_t> entries((end-begin)/8),verify(entries.size());
                    std::array<unsigned char,0x638> after{};
                    if((entries.empty()||(read(context,begin,entries.data(),entries.size()*8)&&read(context,begin,verify.data(),verify.size()*8)&&entries==verify))&&
                        read(context,address,after.data(),after.size())&&field<uint64_t>(after.data(),0)==t.id&&after[0x4b0]==1&&
                        std::memcmp(p+0x290,after.data()+0x290,0x110)==0) {
                        if(std::all_of(entries.begin(),entries.end(),[](uint64_t id){return (id>>48)==1;})){
                            t.path_available=true;path_budget-=entries.size();t.path=std::move(entries);
                        }
                    }
                }
            }
            if(!t.positioned&&p[0x1d0]==1){t.positioned=position(p+0xb8,t.position);}
            return true;
        }))return false;
        if(!resolve_live_state(read,context,state.module_base,true,current)||current!=state)return false;
        for(auto& [id,t]:trains)out.push_back(std::move(t));
        return true;
    }catch(...){out.clear();return false;}
}
bool read_network(ReadMemory read,void* context,const LiveState& state,bool recognized,Network& out) noexcept {
    out={};if(!read||!recognized) return false;
    try {
        LiveState current{};
        if(!resolve_live_state(read,context,state.module_base,true,current)||current!=state) return false;
        Network result;
        if(!collect(read,context,state.database,1,0x4e8,[&](const unsigned char* p,uint64_t){
            Track t{field<uint64_t>(p,0),field<uint64_t>(p,0xd0),field<float>(p,0x80),field<float>(p,0x84),0};
            if(!std::isfinite(t.physical_mps)||!std::isfinite(t.manual_mps)||t.physical_mps<0||t.physical_mps>10000||t.manual_mps>10000) return false;
            if(t.station_id&&(t.station_id>>48)!=2) return false;
            t.links[0]=field<uint64_t>(p,8);t.links[1]=field<uint64_t>(p,16);
            t.x=field<double>(p,0x30);t.y=field<double>(p,0x38);
            t.geometry=std::isfinite(t.x)&&std::isfinite(t.y)&&std::abs(t.x)<1e9&&std::abs(t.y)<1e9;
            // RVA 0x4282f0 and threshold at RVA 0xaab904 (float 1/3.6).
            t.limit_mps=t.manual_mps<0.27777761220932007f?t.physical_mps:std::min(t.physical_mps,t.manual_mps);
            result.tracks.push_back(t);return true;
        })) return false;
        if(!collect(read,context,state.database+0x80,2,0x3e8,[&](const unsigned char* p,uint64_t){
            Station s{field<uint64_t>(p,0),{}};
            if(p[0x40]>1) return false;
            if(!p[0x40]) {
                const auto length=field<uint64_t>(p,0x30),capacity=field<uint64_t>(p,0x38);
                if(length>256||capacity<15||capacity<length||capacity>4096) return false;
                std::array<char,257> text{};
                if(capacity<=15) std::memcpy(text.data(),p+0x20,length+1);
                else if(!pointer(field<uint64_t>(p,0x20))||!read(context,field<uint64_t>(p,0x20),text.data(),length+1)) return false;
                if(text[length]) return false;
                s.name.assign(text.data(),length);
            }
            result.stations.push_back(std::move(s));return true;
        })) return false;
        if(!collect(read,context,state.database+0x380,8,0xc8,[&](const unsigned char* p,uint64_t){
            TrainPosition pos;
            if(!position(p+0x40,pos)) return false;
            const int kind=field<int32_t>(p,0x30);
            if(kind<0||kind>6) return false;
            result.signals.push_back({field<uint64_t>(p,0),pos.track_id,pos.fraction,pos.direction,kind});return true;
        })) return false;
        // Reject unresolved references, including IDs whose generation changed.
        std::unordered_set<uint64_t> station_ids,track_ids;
        for(const auto& s:result.stations)station_ids.insert(s.id);
        for(const auto& t:result.tracks)track_ids.insert(t.id);
        for(const auto& t:result.tracks) if(t.station_id&&!station_ids.contains(t.station_id)) return false;
        for(const auto& s:result.signals) if(!track_ids.contains(s.track_id)) return false;
        if(!resolve_live_state(read,context,state.module_base,true,current)||current!=state) return false;
        out=std::move(result);return true;
    } catch(...) {out={};return false;}
}
}
