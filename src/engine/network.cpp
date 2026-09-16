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
// Native Basics UI: RVA 0x7fd860 -> local_480 -> param15 of 0x7f3600.
// Sim+0x2208 passenger query, map +0x90: node {full train ID, int count,
// float passenger mass, next pointer}. Motion+0x48 is capacity, NOT occupancy.
bool passenger_counts(ReadMemory read,void* context,const LiveState& state,std::map<uint64_t,int32_t>& out){
    out.clear();
    for(int attempt=0;attempt<3;++attempt){
        uint64_t query{},query_after{};std::array<uint64_t,2> header{},after{};
        if(!read(context,state.simulation+0x2208,&query,8)||!pointer(query)||
           !read(context,query+0x90,header.data(),sizeof header))continue;
        const auto buckets=header[0],count=header[1];
        if(!pointer(buckets)||!count||count>1048576)continue;
        std::vector<uint64_t> heads(count+1),verify(heads.size());
        if(!read(context,buckets,heads.data(),heads.size()*8))continue;
        std::map<uint64_t,int32_t> result;std::unordered_set<uint64_t> seen;bool valid=true;
        for(size_t i=0;i<count&&valid;++i){
            for(auto node=heads[i];node&&node!=heads.back();){
                if(!pointer(node)||seen.size()>=1048576||!seen.insert(node).second){valid=false;break;}
                std::array<uint64_t,3> entry{},again{};
                if(!read(context,node,entry.data(),sizeof entry)||!read(context,node,again.data(),sizeof again)||entry!=again){valid=false;break;}
                const auto id=entry[0];const auto passengers=field<int32_t>(entry.data(),8);
                if((id>>48)!=5||id%count!=i||passengers<0||!result.emplace(id,passengers).second){valid=false;break;}
                node=entry[2];
            }
        }
        if(!valid||!read(context,buckets,verify.data(),verify.size()*8)||heads!=verify||
           !read(context,query+0x90,after.data(),sizeof after)||header!=after||
           !read(context,state.simulation+0x2208,&query_after,8)||query!=query_after)continue;
        out=std::move(result);return true;
    }
    return false;
}
bool position(const void* p,TrainPosition& out) {
    out={field<uint64_t>(p,0),field<double>(p,8),field<int8_t>(p,16)};
    return (out.track_id>>48)==1&&std::isfinite(out.fraction)&&out.fraction>=0&&out.fraction<=1&&(out.direction==1||out.direction==-1);
}
void resolve_station_names(ReadMemory read,void* context,const LiveState& state,
                           const std::map<uint64_t,size_t>& automatic,std::vector<Station>& stations) {
    if(automatic.empty())return;
    std::map<uint64_t,std::string> names;
    // Native station-name cache: DB+0x430, lookup RVA 0x3ca140 (stride 0xf8).
    // UI RVA 0x61acc0 selects cache+8 when Station+0x40 is set.
    const bool stable=collect(read,context,state.database+0x430,2,0xf8,
        [&](const unsigned char* p,uint64_t address){
            const auto id=field<uint64_t>(p,0);
            if(!automatic.contains(id))return true; // Includes the ID generation.
            const auto length=field<uint64_t>(p,0x18),capacity=field<uint64_t>(p,0x20);
            if(length>256||capacity<15||capacity<length||capacity>4096)return true;
            std::array<char,257> text{},verify{};
            if(capacity==15)std::memcpy(text.data(),p+8,length+1);
            else {
                const auto data=field<uint64_t>(p,8);
                if(!pointer(data)||!read(context,data,text.data(),length+1)||
                   !read(context,data,verify.data(),length+1)||text!=verify)return true;
            }
            // Reject missing terminators, embedded NULs and concurrent replacement.
            std::array<unsigned char,0x28> after{};
            if(text[length]||std::memchr(text.data(),0,length)||
               !read(context,address,after.data(),after.size())||
               std::memcmp(p,after.data(),after.size())!=0)return true;
            names.emplace(id,std::string(text.data(),length));
            return true;
        });
    // An unavailable cache must not hide otherwise valid network data.
    if(stable)for(auto& [id,name]:names)stations[automatic.at(id)].name=std::move(name);
}
}
bool decode_train_service(const void* motion,size_t size,int32_t order_mode,NimbyTrainService& out) noexcept {
    out={};if(!motion||size<0x638||order_mode<0||order_mode>2)return false;
    const auto* p=static_cast<const unsigned char*>(motion);
    out.train_id=field<uint64_t>(p,0);out.stop_index=-1;out.line_kind=-1;
    if((out.train_id>>48)!=5)return false;
    for(auto [offset,flag]:std::array<std::pair<size_t,uint32_t>,11>{{
        {0x1d0,NIMBY_MOTION_PRESENCE},{0x218,NIMBY_MOTION_HIDDEN},{0x4b0,NIMBY_MOTION_DRIVE},
        {0x4d0,NIMBY_MOTION_TIMED_STOP},{0x4f0,NIMBY_MOTION_SCHEDULE_STOP},{0x510,NIMBY_MOTION_RUN_STOP},
        {0x558,NIMBY_MOTION_STATION_STOP},{0x598,NIMBY_MOTION_DISPATCH_COOLDOWN},{0x5d0,NIMBY_MOTION_RUN},
        {0x5f0,NIMBY_MOTION_SCHEDULE},{0x1f0,NIMBY_MOTION_HITCH}}}){
        if(p[offset]>1)return false;
        if(p[offset])out.motion_flags|=flag;
    }
    if(p[0xb0]>1)return false;
    // Same alert precedence as native RVA 0x43e130.
    if(p[0x4b0]&&p[0x490])out.alert=5;
    else if(p[0xb0]){
        switch(field<int32_t>(p,0xa8)){case 0:out.alert=3;break;case 1:out.alert=4;break;case 3:out.alert=1;break;case 4:out.alert=7;break;default:break;}
    }
    if(!out.alert&&p[0x4b0]&&field<uint64_t>(p,0x458))out.alert=6;
    if(!out.alert&&!p[0x1d0]&&p[0x598]&&field<int32_t>(p,0x568)){
        if(field<int32_t>(p,0x574))out.alert=8;
        else if(field<int32_t>(p,0x570))out.alert=10;
        else if(!field<int32_t>(p,0x56c))out.alert=9;
    }
    TrainPosition location;
    // Blackhole stores a location even though the train is hidden from the tracks.
    const size_t off=p[0x218]?0x1f8:p[0x4b0]?0x3a8:0xb8;
    if((p[0x218]||p[0x4b0]||p[0x1d0])&&position(p+off,location)){
        out.location_track_id=location.track_id;out.flags|=NIMBY_SERVICE_LOCATION_VALID;
    }
    if(p[0x5d0]){
        out.line_id=field<uint64_t>(p,0x5a8);out.stop_index=field<int32_t>(p,0x5c8);
        if((out.line_id>>48)==4&&out.stop_index>=0)out.flags|=NIMBY_SERVICE_RUN_VALID;
        else {out.line_id=0;out.stop_index=-1;}
    }
    // Do not read stale optional storage when the matching state is absent.
    auto time=[&](size_t offset,int64_t& dest,uint32_t flag){
        const auto value=field<int64_t>(p,offset);
        if(value>0&&value<100000000000000000LL){dest=value;out.flags|=flag;}
    };
    if(p[0x4d0])time(0x4c0,out.departure_time_us,NIMBY_SERVICE_DEPARTURE_VALID);
    else if(p[0x4b0])time(0x488,out.arrival_time_us,NIMBY_SERVICE_ARRIVAL_VALID);
    if(p[0x598])time(0x560,out.dispatch_time_us,NIMBY_SERVICE_COOLDOWN_VALID);
    out.flags|=NIMBY_SERVICE_STATE_VALID;
    out.status=order_mode==2?NIMBY_SERVICE_MOTHBALLED:
        out.alert==6?NIMBY_SERVICE_SIGNAL_WAIT:
        p[0x4b0]?NIMBY_SERVICE_DRIVING:
        p[0x1d0]&&p[0x4d0]&&p[0x510]?NIMBY_SERVICE_STATION_STOP:
        p[0x4d0]?NIMBY_SERVICE_TIMED_STOP:
        p[0x598]?NIMBY_SERVICE_DISPATCH_WAIT:
        !p[0x1d0]&&!p[0x218]?NIMBY_SERVICE_NOT_PRESENT:NIMBY_SERVICE_OTHER;
    return true;
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
        std::map<uint64_t,uint64_t> model_addresses;
        size_t path_budget=1048576;
        if(!collect(read,context,state.database+0x200,5,0x178,[&](const unsigned char* p,uint64_t address){
            Train t;t.id=field<uint64_t>(p,0);
            const auto length=field<uint64_t>(p,0x20),capacity=field<uint64_t>(p,0x28);
            if(length>256||capacity<15||capacity<length||capacity>4096)return false;
            std::array<char,257> text{};
            if(capacity<=15)std::memcpy(text.data(),p+0x10,length+1);
            else if(!pointer(field<uint64_t>(p,0x10))||!read(context,field<uint64_t>(p,0x10),text.data(),length+1))return false;
            if(text[length])return false;
            t.name.assign(text.data(),length);t.order_mode=field<int32_t>(p,0xb8);
            t.service.train_id=t.id;t.details.train_id=t.id;t.details.order_mode=t.order_mode;t.details.order_index=-1;
            model_addresses.emplace(t.id,address);trains.emplace(t.id,std::move(t));return true;
        }))return false;
        if(!collect(read,context,state.simulation+0xa0,5,0x638,[&](const unsigned char* p,uint64_t address){
            auto it=trains.find(field<uint64_t>(p,0));if(it==trains.end()||p[0x4b0]>1)return false;
            auto& t=it->second;t.present=p[0x4b0]==1;
            // Service validity is independent of speed/network availability.
            NimbyTrainService first{},second{};
            std::array<unsigned char,0x638> service_after{};
            int64_t ticks{},epoch{};
            const bool clock_read=read(context,state.simulation+0x28,&ticks,sizeof ticks);
            std::array<unsigned char,0xbc> model_after{};
            if(decode_train_service(p,0x638,t.order_mode,first)&&
               read(context,model_addresses.at(t.id),model_after.data(),model_after.size())&&
               field<uint64_t>(model_after.data(),0)==t.id&&field<int32_t>(model_after.data(),0xb8)==t.order_mode&&
               read(context,address,service_after.data(),service_after.size())&&
               decode_train_service(service_after.data(),service_after.size(),t.order_mode,second)&&
               std::memcmp(&first,&second,sizeof first)==0){
                t.service=first;
                const auto* verified=service_after.data();
                if(p[0x5f0]==1&&verified[0x5f0]==1&&std::memcmp(p+0x5d8,verified+0x5d8,0x14)==0&&
                   (field<uint64_t>(p,0x5d8)>>48)==6&&field<uint64_t>(p,0x5e0)&&field<int32_t>(p,0x5e8)>=0){
                    t.details.schedule_id=field<uint64_t>(p,0x5d8);
                    t.details.shift_id=field<uint64_t>(p,0x5e0);
                    t.details.order_index=field<int32_t>(p,0x5e8);
                    t.details.flags|=NIMBY_TRAIN_ASSIGNMENT_VALID;
                }
                if(clock_read&&ticks>=0&&ticks<10000000000000LL){
                    auto& s=t.service;s.game_time_us=ticks*10000;s.flags|=NIMBY_SERVICE_CLOCK_VALID;
                    if(read(context,state.simulation+0x20,&epoch,sizeof epoch)&&epoch>=0&&epoch<32503680000LL){
                        s.game_epoch_seconds=epoch;s.flags|=NIMBY_SERVICE_CALENDAR_VALID;
                    }
                    if(s.flags&NIMBY_SERVICE_ARRIVAL_VALID)s.arrival_remaining_seconds=double(s.arrival_time_us-s.game_time_us)/1000000;
                    if(s.flags&NIMBY_SERVICE_DEPARTURE_VALID)s.departure_remaining_seconds=std::max(0.0,double(s.departure_time_us-s.game_time_us)/1000000);
                    if(s.flags&NIMBY_SERVICE_COOLDOWN_VALID)s.dispatch_remaining_seconds=std::max(0.0,double(s.dispatch_time_us-s.game_time_us)/1000000);
                }
            }
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
            if(!t.present) {
                // Native train-info UI (RVA 0x7f5be3): no Drive => display zero.
                // Never expose residual speed bytes from a disengaged optional.
                std::array<unsigned char,0x4b1> after{};
                if(!read(context,address,after.data(),after.size())||
                   field<uint64_t>(after.data(),0)!=t.id||after[0x4b0]!=0)return false;
                t.speed_mps=0;
            }
            t.speed_available=true; // A matching Motion was actually observed.
            if(!t.positioned&&p[0x1d0]==1){t.positioned=position(p+0xb8,t.position);}
            return true;
        }))return false;
        std::map<uint64_t,int32_t> passengers;
        if(passenger_counts(read,context,state,passengers)){
            for(auto& [id,t]:trains)if((t.service.flags&NIMBY_SERVICE_STATE_VALID)&&(t.service.motion_flags&NIMBY_MOTION_PRESENCE)){
                // The UI initializes a missing key to zero only after reading this map.
                const auto found=passengers.find(id);
                t.details.passenger_count=found==passengers.end()?0:found->second;
                t.details.flags|=NIMBY_TRAIN_PASSENGERS_VALID;
            }
        }
        // Run and stop data are optional; a missing catalog does not hide trains.
        std::map<uint64_t,std::array<unsigned char,0x280>> lines;
        std::map<uint64_t,uint64_t> line_addresses;
        if(!collect(read,context,state.database+0x180,4,0x280,[&](const unsigned char* p,uint64_t address){
            std::array<unsigned char,0x280> bytes{};std::memcpy(bytes.data(),p,bytes.size());
            const auto id=field<uint64_t>(p,0);lines.emplace(id,bytes);line_addresses.emplace(id,address);return true;
        }))lines.clear();
        size_t stop_budget=1048576;
        for(auto& [id,t]:trains){
            auto& s=t.service;
            if(!(s.flags&NIMBY_SERVICE_RUN_VALID)||!lines.contains(s.line_id))continue;
            const auto& line=lines.at(s.line_id);const auto* p=line.data();
            const auto kind=field<int32_t>(p,0xfc);
            if(kind<0||kind>2)continue;
            s.line_kind=kind;s.flags|=NIMBY_SERVICE_LINE_VALID;
            const auto length=field<uint64_t>(p,0x88),capacity=field<uint64_t>(p,0x90);
            if(length<=256&&capacity>=15&&capacity>=length&&capacity<=4096){
                std::array<char,257> name{},verify{};
                bool ok=true;
                if(capacity==15)std::memcpy(name.data(),p+0x78,length+1);
                else {auto address=field<uint64_t>(p,0x78);ok=pointer(address)&&read(context,address,name.data(),length+1)&&read(context,address,verify.data(),length+1)&&name==verify;}
                if(ok&&!name[length]&&!std::memchr(name.data(),0,length))std::memcpy(s.line_name_utf8,name.data(),length);
            }
            std::array<unsigned char,0x280> line_verify{};
            if(!read(context,line_addresses.at(s.line_id),line_verify.data(),line_verify.size())||line_verify!=line){
                s.flags&=~NIMBY_SERVICE_LINE_VALID;s.line_kind=-1;s.line_name_utf8[0]=0;continue;
            }
            const auto begin=field<uint64_t>(p,0x118),end=field<uint64_t>(p,0x120),cap=field<uint64_t>(p,0x128);
            if(!pointer(begin)||end<begin||cap<end||cap-begin>0x158*16384||(end-begin)%0x158||
                uint64_t(s.stop_index)>=(end-begin)/0x158)continue;
            std::array<unsigned char,0x158> stop{},again{};
            const auto address=begin+uint64_t(s.stop_index)*0x158;
            std::array<unsigned char,0x280> line_after{};
            if(!read(context,address,stop.data(),stop.size())||!read(context,address,again.data(),again.size())||stop!=again||
               !read(context,line_addresses.at(s.line_id),line_after.data(),line_after.size())||line_after!=line){
                s.flags&=~NIMBY_SERVICE_LINE_VALID;s.line_kind=-1;s.line_name_utf8[0]=0;continue;
            }
            const auto track=field<uint64_t>(stop.data(),0x78),station=field<uint64_t>(stop.data(),0x110);
            if((track>>48)!=1||(station&&(station>>48)!=2))continue;
            s.stop_track_id=track;s.stop_station_id=station;s.flags|=NIMBY_SERVICE_STOP_VALID;
            // Relative line plan only: partial runs/loops need further timing corrections.
            const auto count=(end-begin)/0x158;
            if(count<=stop_budget){
                std::vector<unsigned char> bytes(end-begin),check(bytes.size());
                if(read(context,begin,bytes.data(),bytes.size())&&read(context,begin,check.data(),check.size())&&bytes==check&&
                   read(context,line_addresses.at(s.line_id),line_after.data(),line_after.size())&&line_after==line){
                    std::vector<NimbyLineStop> stops;bool valid=true;
                    for(uint32_t index=0;index<count;++index){
                        const auto* entry=bytes.data()+size_t(index)*0x158;
                        NimbyLineStop v{};v.line_id=s.line_id;v.index=index;
                        v.track_id=field<uint64_t>(entry,0x78);v.station_id=field<uint64_t>(entry,0x110);
                        if((v.track_id>>48)!=1||(v.station_id&&(v.station_id>>48)!=2)){valid=false;break;}
                        const auto arrival=field<int32_t>(entry,0xb8),departure=field<int32_t>(entry,0xbc);
                        if(arrival>=0&&departure>=arrival){v.arrival_offset_seconds=arrival;v.departure_offset_seconds=departure;v.flags=NIMBY_LINE_STOP_TIMES_VALID;}
                        stops.push_back(v);
                    }
                    if(valid){t.line_stops=std::move(stops);t.line_stops_available=true;stop_budget-=count;}
                }
            }
            if(kind==1&&(s.motion_flags&NIMBY_MOTION_HIDDEN)&&(s.motion_flags&NIMBY_MOTION_RUN_STOP)&&
               !(s.motion_flags&NIMBY_MOTION_DRIVE)&&s.status!=NIMBY_SERVICE_MOTHBALLED)s.status=NIMBY_SERVICE_DEPOT;
        }
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
        struct Label { NimbyPlatform value{}; bool automatic{},geometry_valid{}; int32_t number{}; double dx{},dy{}; uint64_t links[2]{}; };
        std::vector<Label> labels;
        if(!collect(read,context,state.database,1,0x4e8,[&](const unsigned char* p,uint64_t address){
            Track t{field<uint64_t>(p,0),field<uint64_t>(p,0xd0),field<float>(p,0x80),field<float>(p,0x84),0};
            if(!std::isfinite(t.physical_mps)||!std::isfinite(t.manual_mps)||t.physical_mps<0||t.physical_mps>10000||t.manual_mps>10000) return false;
            if(t.station_id&&(t.station_id>>48)!=2) return false;
            t.links[0]=field<uint64_t>(p,8);t.links[1]=field<uint64_t>(p,16);
            t.x=field<double>(p,0x30);t.y=field<double>(p,0x38);
            t.geometry=std::isfinite(t.x)&&std::isfinite(t.y)&&std::abs(t.x)<1e9&&std::abs(t.y)<1e9;
            // RVA 0x4282f0 and threshold at RVA 0xaab904 (float 1/3.6).
            t.limit_mps=t.manual_mps<0.27777761220932007f?t.physical_mps:std::min(t.physical_mps,t.manual_mps);
            if(t.station_id){
                Label label;label.value.track_id=t.id;label.value.station_id=t.station_id;
                label.links[0]=t.links[0];label.links[1]=t.links[1];
                bool valid=p[0xc8]<=1;label.automatic=p[0xc8]==1;
                if(valid&&label.automatic){
                    label.number=field<int32_t>(p,0xa0);
                    valid=label.number>=0&&label.number<1000000;
                    const auto begin=field<uint64_t>(p,0x1b0),end=field<uint64_t>(p,0x1b8);
                    if(end>=begin&&(end-begin)%16==0&&end-begin<=16*65536){
                        if(end-begin<32)label.geometry_valid=true;
                        else if(pointer(begin)){
                            std::array<double,2> first{},last{},first2{},last2{};
                            if(read(context,begin,first.data(),16)&&read(context,end-16,last.data(),16)&&
                               read(context,begin,first2.data(),16)&&read(context,end-16,last2.data(),16)&&first==first2&&last==last2){
                                label.dx=last[0]-first[0];label.dy=last[1]-first[1];
                                label.geometry_valid=std::isfinite(label.dx)&&std::isfinite(label.dy);
                            }
                        }
                    }
                }else if(valid){
                    const auto length=field<uint64_t>(p,0xb8),capacity=field<uint64_t>(p,0xc0);
                    valid=length<=256&&capacity>=15&&capacity>=length&&capacity<=4096;
                    std::array<char,257> name{},again{};
                    if(valid){
                        if(capacity==15)std::memcpy(name.data(),p+0xa8,length+1);
                        else {const auto ptr=field<uint64_t>(p,0xa8);valid=pointer(ptr)&&read(context,ptr,name.data(),length+1)&&read(context,ptr,again.data(),length+1)&&name==again;}
                        valid=valid&&!name[length]&&!std::memchr(name.data(),0,length);
                        if(valid)std::memcpy(label.value.name_utf8,name.data(),length);
                    }
                }
                std::array<unsigned char,0x1c0> verify{};
                valid=valid&&read(context,address,verify.data(),verify.size())&&
                    !std::memcmp(p,verify.data(),24)&&!std::memcmp(p+0xa0,verify.data()+0xa0,0x38)&&
                    !std::memcmp(p+0x1b0,verify.data()+0x1b0,16);
                if(valid)label.value.flags=NIMBY_PLATFORM_NAME_VALID;
                labels.push_back(label);
            }
            result.tracks.push_back(t);return true;
        })) return false;
        std::map<uint64_t,uint64_t> track_stations;
        for(const auto& track:result.tracks)track_stations.emplace(track.id,track.station_id);
        for(auto& label:labels){
            if(label.automatic&&(label.value.flags&NIMBY_PLATFORM_NAME_VALID)){
                std::string name=std::to_string(label.number+1);
                if(label.links[0]&&label.links[1]){
                    if(!track_stations.contains(label.links[0])||!track_stations.contains(label.links[1]))label.value.flags=0;
                    else {
                        const bool a=track_stations.at(label.links[0])!=0,b=track_stations.at(label.links[1])!=0;
                        if(a!=b){
                            if(!label.geometry_valid)label.value.flags=0;
                            else {
                                bool reverse=!a;
                                if(std::abs(label.dx)<=std::abs(label.dy)){
                                    if(label.dy<0)reverse=!reverse;
                                    name+=reverse?'S':'N';
                                }else {if(label.dx<0)reverse=!reverse;name+=reverse?'W':'E';}
                            }
                        }
                    }
                }
                if(label.value.flags)std::memcpy(label.value.name_utf8,name.c_str(),name.size()+1);
            }
            if(!label.value.flags)label.value.name_utf8[0]=0;
            result.platforms.push_back(label.value);
        }
        std::map<uint64_t,size_t> automatic_stations;
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
            if(p[0x40])automatic_stations.emplace(s.id,result.stations.size());
            result.stations.push_back(std::move(s));return true;
        })) return false;
        resolve_station_names(read,context,state,automatic_stations,result.stations);
        if(!collect(read,context,state.database+0x380,8,0xc8,[&](const unsigned char* p,uint64_t){
            TrainPosition pos;
            if(!position(p+0x40,pos)) return false;
            const int kind=field<int32_t>(p,0x30);
            if(kind<0||kind>6) return false;
            result.signals.push_back({field<uint64_t>(p,0),pos.track_id,pos.fraction,pos.direction,kind,field<uint64_t>(p,0x38)});return true;
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
bool read_signal_texture_states(ReadMemory read,void* context,const LiveState& state,bool recognized,
                                std::vector<SignalTextureState>& out) noexcept {
    out.clear();if(!read||!recognized)return false;
    try {for(int attempt=0;attempt<3;++attempt){
        LiveState current{};uint64_t query{},after{};std::array<uint64_t,4> h{},verify{};
        if(!resolve_live_state(read,context,state.module_base,true,current)||current!=state)return false;
        // Native renderer RVA 0x620140: Sim+0x2200 -> query Swiss map +0x378.
        // Slot: full signal ID +0, signed texture selector +8, stride 16.
        if(!read(context,state.simulation+0x2200,&query,8)||!pointer(query)||
           !read(context,query+0x378,h.data(),sizeof h))continue;
        const auto ctrl=h[0],slots=h[1],size=h[2],mask=h[3];
        if(mask>1048575||size>mask||(mask&(mask+1))||!pointer(ctrl)||(mask&&!pointer(slots)))continue;
        std::vector<unsigned char> controls(mask),values(mask*16),controls_after(mask),values_after(mask*16);
        if(mask&&(!read(context,ctrl,controls.data(),controls.size())||!read(context,slots,values.data(),values.size())))continue;
        std::vector<SignalTextureState> result;std::unordered_set<uint64_t> ids;bool valid=true;
        for(size_t i=0;i<mask;++i)if(controls[i]<128){
            const auto id=field<uint64_t>(values.data()+i*16,0);
            if((id>>48)!=8||!ids.insert(id).second){valid=false;break;}
            result.push_back({id,field<int32_t>(values.data()+i*16,8)});
        }
        if(!valid||result.size()!=size)continue;
        if(mask&&(!read(context,ctrl,controls_after.data(),controls_after.size())||controls!=controls_after||
                  !read(context,slots,values_after.data(),values_after.size())||values!=values_after))continue;
        if(!read(context,query+0x378,verify.data(),sizeof verify)||h!=verify||
           !read(context,state.simulation+0x2200,&after,8)||query!=after||
           !resolve_live_state(read,context,state.module_base,true,current)||current!=state)continue;
        out=std::move(result);return true;
    }}catch(...){}return false;
}
}
