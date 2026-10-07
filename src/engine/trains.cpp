// Shared train observation and calendar planning. No OS calls or handles.
// A failed consistency check returns unavailable; never merge separate attempts.
#include "engine/network.h"
#include "engine/detail/train_metadata.h"
#include "engine/detail/train_composition.h"
#include "engine/native_string.h"
#include "engine/simulation_clock.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <map>
#include <unordered_set>
#include "engine/detail/memory_reader.h"
#include "engine/detail/position.h"
namespace nimby::engine {
using memory::field;
using memory::pointer;
using memory::collect;
using memory::position;
namespace {
// The native simulation can retain a Motion after its train was deleted. This
// is not an unknown active train: the model slot carries the matching tombstone
// (same index AND generation), and all physical-presence optionals are empty.
// Confirm both facts again before ignoring it. A live/reused/missing model slot,
// an active optional, or a concurrent change still invalidates the capture.
bool retired_motion(ReadMemory read,void* context,const LiveState& state,
                    const unsigned char* motion,uint64_t motionAddress) {
    constexpr std::array<size_t,3> presenceOffsets{0x1d0,0x218,0x4b0};
    for(auto offset:presenceOffsets)if(motion[offset]!=0)return false;
    const auto id=field<uint64_t>(motion,0);
    const auto tombstone=id|0xffff000000000000ULL;
    const auto pool=state.database+0x200;
    std::array<unsigned char,48> header{},headerAfter{};
    if(!read(context,pool,header.data(),header.size()))return false;
    const auto shift=field<uint32_t>(header.data(),4),size=field<uint32_t>(header.data(),8),mask=field<uint32_t>(header.data(),16);
    const auto begin=field<uint64_t>(header.data(),24),end=field<uint64_t>(header.data(),32),cap=field<uint64_t>(header.data(),40);
    if(shift<1||shift>16||size!=(1u<<shift)||mask!=size-1||!pointer(begin)||
       end<begin||cap<end||cap-begin>8192||(end-begin)%8)return false;
    const auto index=(id>>16)&0xffffffffULL,blockIndex=index>>shift;
    if(blockIndex>=(end-begin)/8)return false;
    uint64_t block{},blockAfter{},modelId{},modelAfter{};
    if(!read(context,begin+blockIndex*8,&block,8)||!pointer(block)||
       block>0x7fffffff0000ULL-size*0x178ULL)return false;
    const auto modelAddress=block+(index&mask)*0x178;
    if(!read(context,modelAddress,&modelId,8)||modelId!=tombstone)return false;
    std::array<unsigned char,0x638> after{};
    if(!read(context,motionAddress,after.data(),after.size())||field<uint64_t>(after.data(),0)!=id)return false;
    for(auto offset:presenceOffsets)if(after[offset]!=0)return false;
    return read(context,modelAddress,&modelAfter,8)&&modelAfter==modelId&&
        read(context,begin+blockIndex*8,&blockAfter,8)&&blockAfter==block&&
        read(context,pool,headerAfter.data(),headerAfter.size())&&headerAfter==header;
}

// Native Basics UI: RVA 0x7fd860 -> local_480 -> param15 of 0x7f3600.
// Sim+0x2208 passenger query, map +0x90: node {full train ID, int count,
// float passenger mass, next pointer}. Motion+0x48 is capacity, NOT occupancy.
bool passenger_counts(ReadMemory read,void* context,const LiveState& state,std::map<uint64_t,int32_t>& out){
    out.clear();
    for(int attempt=0;attempt<3;++attempt){
        uint64_t query{},query_after{};std::array<uint64_t,2> header{},after{};
        if(!read(context,state.simulation+gameLayout(state.profile).passenger_query,&query,8)||!pointer(query)||
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
           !read(context,state.simulation+gameLayout(state.profile).passenger_query,&query_after,8)||query!=query_after)continue;
        out=std::move(result);return true;
    }
    return false;
}

}
bool plan_simulation_calendar(ReadMemory read,void* context,uint64_t simulation,
                              int64_t delta,std::vector<CalendarWrite>& edits) {
    return collect(read,context,simulation+0xa0,5,0x638,[&](const unsigned char* p,uint64_t address){
        return plan_motion_calendar(p,address,delta,edits);
    });
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
bool read_trains(ReadMemory read,void* context,const LiveState& state,bool recognized,std::vector<Train>& out,bool presenceOnly,TrainMetadataCatalog* metadata,bool includePaths,uint32_t dataFlags) noexcept {
    out.clear();if(metadata)*metadata={};if(!read||!recognized)return false;
    try {
        LiveState current;
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
        std::map<uint64_t,Train> trains;
        std::map<uint64_t,uint64_t> model_addresses;
        // These additions use only the proven Windows layout and never add
        // reads to the signalling-only path or to callers not requesting them.
        bool wantMetadata=metadata&&!presenceOnly&&state.profile==LiveStateProfile::Windows119;
        if(!metadata)dataFlags=NIMBY_TRAIN_DATA_ALL;
        if(dataFlags&NIMBY_TRAIN_DATA_TIMETABLES)dataFlags|=NIMBY_TRAIN_DATA_SERVICE;
        if(dataFlags&NIMBY_TRAIN_DATA_TAGS)dataFlags|=NIMBY_TRAIN_DATA_LINES;
        const bool wantService=dataFlags&NIMBY_TRAIN_DATA_SERVICE,wantCharacteristics=dataFlags&NIMBY_TRAIN_DATA_CHARACTERISTICS;
        const bool wantTimetables=dataFlags&NIMBY_TRAIN_DATA_TIMETABLES,wantTags=dataFlags&NIMBY_TRAIN_DATA_TAGS;
        const bool wantPassengers=dataFlags&NIMBY_TRAIN_DATA_PASSENGERS,wantLines=dataFlags&NIMBY_TRAIN_DATA_LINES;
        const bool wantComposition=dataFlags&NIMBY_TRAIN_DATA_COMPOSITION;
        const bool wantLocations=dataFlags&NIMBY_TRAIN_DATA_LOCATIONS;
        const bool wantMetadataSources=wantService||wantCharacteristics||wantComposition;
        struct MetadataSource {
            std::array<unsigned char,0x50> configured{},current{};NimbyTrainMetadata value{};
            uint64_t motionAddress=0;bool currentStable=false;
        };
        std::map<uint64_t,MetadataSource> metadataSources;
        size_t path_budget=1048576;
        if(!collect(read,context,state.database+0x200,5,0x178,[&](const unsigned char* p,uint64_t address){
            Train t;t.id=field<uint64_t>(p,0);
            if(wantMetadata&&wantMetadataSources)try{
                auto& source=metadataSources[t.id];source.value.train_id=t.id;
                if(wantCharacteristics||wantComposition)std::memcpy(source.configured.data(),p+0xc0,source.configured.size());
            }catch(...){wantMetadata=false;metadataSources.clear();}
            // BAL : seules l'identite et la presence physique sont necessaires.
            if(presenceOnly){t.service.train_id=t.id;trains.emplace(t.id,std::move(t));return true;}
            if(!read_native_string(read,context,address+0x10,p+0x10,state.profile,t.name))return false;
            t.order_mode=field<int32_t>(p,0xb8);
            t.service.train_id=t.id;t.details.train_id=t.id;t.details.order_mode=t.order_mode;t.details.order_index=-1;
            model_addresses.emplace(t.id,address);trains.emplace(t.id,std::move(t));return true;
        }))return false;
        if(!collect(read,context,state.simulation+0xa0,5,0x638,[&](const unsigned char* p,uint64_t address){
            auto it=trains.find(field<uint64_t>(p,0));
            if(it==trains.end())return retired_motion(read,context,state,p,address);
            if(p[0x4b0]>1)return false;
            auto& t=it->second;t.present=p[0x4b0]==1;
            // Service validity is independent of speed/network availability.
            NimbyTrainService first{},second{};
            std::array<unsigned char,0x638> service_after{};
            const bool motionRechecked=read(context,address,service_after.data(),service_after.size())&&
                field<uint64_t>(service_after.data(),0)==t.id;
            if(wantMetadata&&wantMetadataSources&&motionRechecked){
                auto& source=metadataSources.at(t.id);source.motionAddress=address;
                if(wantComposition){
                    source.currentStable=std::memcmp(p+8,service_after.data()+8,source.current.size())==0;
                    if(source.currentStable)std::memcpy(source.current.data(),service_after.data()+8,source.current.size());
                }
            }
            if(wantMetadata&&wantCharacteristics&&motionRechecked)metadataSources.at(t.id).value.current=
                train_metadata::characteristics(p+8,service_after.data()+8,0x50);
            // Presence means network membership plus visibility, not whether a
            // Drive was engaged. Starting to move need not change occupancy.
            if(motionRechecked){
                uint32_t presenceFlags=0,afterFlags=0;bool stable=true;
                for(auto [offset,flag]:std::array<std::pair<size_t,uint32_t>,3>{{
                    {0x1d0,NIMBY_MOTION_PRESENCE},{0x218,NIMBY_MOTION_HIDDEN},{0x4b0,NIMBY_MOTION_DRIVE}}}){
                    if(p[offset]>1||service_after[offset]>1)stable=false;
                    if(p[offset]==1)presenceFlags|=flag;
                    if(service_after[offset]==1)afterFlags|=flag;
                }
                stable=stable && (presenceFlags!=0)==(afterFlags!=0) &&
                    (presenceFlags&NIMBY_MOTION_HIDDEN)==(afterFlags&NIMBY_MOTION_HIDDEN);
                if(stable){t.service.flags=NIMBY_SERVICE_PRESENCE_VALID;t.service.motion_flags=presenceFlags;}
            }
            if(presenceOnly){
                // The bulk Motion copy may precede the per-train copy by a
                // simulation tick. Requiring the fraction to be identical
                // made moving trains disappear from approach observations.
                // Keep the newest observed head when identity, membership,
                // track and direction agree and movement follows that direction.
                // A track change/reversal remains unavailable for this capture;
                // never mix fields, extrapolate a head or reuse a previous one.
                // Reuse the two copies: no path, timetable or speed reads.
                TrainPosition firstHead{},secondHead{};
                if(motionRechecked&&(t.service.flags&NIMBY_SERVICE_PRESENCE_VALID)&&
                   decode_train_position(p,0x638,firstHead)&&
                   decode_train_position(service_after.data(),service_after.size(),secondHead)&&
                   firstHead.track_id==secondHead.track_id&&firstHead.direction==secondHead.direction&&
                   (secondHead.fraction-firstHead.fraction)*firstHead.direction>=0){
                    t.position=secondHead;t.positioned=true;
                }
                return true;
            }
            if(wantLocations&&!wantService&&motionRechecked){
                // Location-only tools still need Hidden/Blackhole and stopped
                // Presence positions. Reuse the verified Motion pair without
                // decoding service state, clocks, assignments or timetables.
                const auto* after=service_after.data();
                bool stable=true;
                for(const auto offset:{0x1d0,0x218,0x4b0})
                    if(p[offset]>1||p[offset]!=after[offset])stable=false;
                const auto decode=[](const unsigned char* bytes,TrainPosition& location){
                    return (bytes[0x218]||bytes[0x4b0]||bytes[0x1d0])&&
                        position(bytes+(bytes[0x218]?0x1f8:bytes[0x4b0]?0x3a8:0xb8),location);
                };
                TrainPosition firstLocation{},lastLocation{};
                if(stable&&decode(p,firstLocation)&&decode(after,lastLocation)&&
                   firstLocation.track_id==lastLocation.track_id){
                    t.service.location_track_id=lastLocation.track_id;
                    t.service.flags|=NIMBY_SERVICE_LOCATION_VALID;
                }
            }
            if(wantService){
            int64_t ticks{},epoch{};
            const bool clock_read=read(context,state.simulation+0x28,&ticks,sizeof ticks);
            std::array<unsigned char,0xbc> model_after{};
            if(decode_train_service(p,0x638,t.order_mode,first)&&
               read(context,model_addresses.at(t.id),model_after.data(),model_after.size())&&
               field<uint64_t>(model_after.data(),0)==t.id&&field<int32_t>(model_after.data(),0xb8)==t.order_mode&&
               motionRechecked&&
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
                    if(read(context,state.simulation+0x20,&epoch,sizeof epoch)&&epoch>=-62135596800LL&&epoch<=253402300799LL-s.game_time_us/1000000){
                        s.game_epoch_seconds=epoch;s.flags|=NIMBY_SERVICE_CALENDAR_VALID;
                    }
                    if(s.flags&NIMBY_SERVICE_ARRIVAL_VALID)s.arrival_remaining_seconds=double(s.arrival_time_us-s.game_time_us)/1000000;
                    if(s.flags&NIMBY_SERVICE_DEPARTURE_VALID)s.departure_remaining_seconds=std::max(0.0,double(s.departure_time_us-s.game_time_us)/1000000);
                    if(s.flags&NIMBY_SERVICE_COOLDOWN_VALID)s.dispatch_remaining_seconds=std::max(0.0,double(s.dispatch_time_us-s.game_time_us)/1000000);
                    if(wantMetadata&&(s.flags&NIMBY_SERVICE_ARRIVAL_VALID)){
                        if(const auto estimate=train_metadata::predictedDelay(p,verified,0x638,s.game_time_us)){
                            auto& value=metadataSources.at(t.id).value;
                            value.flags=NIMBY_TRAIN_PREDICTED_DELAY_VALID;value.predicted_arrival_delay_us=*estimate;
                        }
                    }
                }
            }
            }
            if(t.present){t.speed_mps=field<double>(p,0x3c8);if(!std::isfinite(t.speed_mps)||std::abs(t.speed_mps)>10000)return false;
                t.positioned=decode_train_position(p,0x638,t.position);
                // Serializer chain Motion -> Drive -> Path, see train-paths.md.
                const auto begin=field<uint64_t>(p,0x338),end=field<uint64_t>(p,0x340),cap=field<uint64_t>(p,0x348);
                if(includePaths&&p[0x320]==1&&end>=begin&&cap>=end&&(end-begin)%8==0&&end-begin<=16384*8&&cap-begin<=65536*8&&
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
                t.speed_mps=0;
                // Reuse the verified Motion read. An optional Drive transition
                // invalidates the default speed, not the whole network snapshot.
                t.speed_available=motionRechecked&&service_after[0x4b0]==0;
            } else {
                t.speed_available=true;
            }
            if(!t.positioned&&p[0x1d0]==1){t.positioned=position(p+0xb8,t.position);}
            return true;
        }))return false;
        if(presenceOnly){
            if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
            for(auto& [id,t]:trains)out.push_back(std::move(t));
            return true;
        }
        std::map<uint64_t,int32_t> passengers;
        if(wantPassengers&&passenger_counts(read,context,state,passengers)){
            for(auto& [id,t]:trains)if((t.service.flags&(NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_PRESENCE_VALID))&&(t.service.motion_flags&NIMBY_MOTION_PRESENCE)){
                // The UI initializes a missing key to zero only after reading this map.
                const auto found=passengers.find(id);
                t.details.passenger_count=found==passengers.end()?0:found->second;
                t.details.flags|=NIMBY_TRAIN_PASSENGERS_VALID;
            }
        }
        // Run and stop data are optional; a missing catalog does not hide trains.
        std::map<uint64_t,std::array<unsigned char,0x280>> lines;
        std::map<uint64_t,uint64_t> line_addresses;
        const bool linesAvailable=(wantTimetables||wantLines)&&collect(read,context,state.database+0x180,4,0x280,[&](const unsigned char* p,uint64_t address){
            std::array<unsigned char,0x280> bytes{};std::memcpy(bytes.data(),p,bytes.size());
            const auto id=field<uint64_t>(p,0);lines.emplace(id,bytes);line_addresses.emplace(id,address);return true;
        });
        if(!linesAvailable)lines.clear();
        size_t stop_budget=1048576;
        struct SharedLine {
            std::string name;
            bool planAttempted=false,planAvailable=false,changed=false;
            uint64_t begin=0;
            std::vector<unsigned char> bytes;
            std::vector<NimbyLineStop> stops;
        };
        std::map<uint64_t,SharedLine> sharedLines;
        // Shared copies live only in this capture. Keep their raw comparison
        // evidence bounded independently of the per-train output copy budget.
        constexpr size_t maximumSharedLineBytes=32*1024*1024;
        size_t sharedLineBytes=0;
        std::vector<std::pair<Train*,int32_t>> lineUsers;
        if(wantTimetables)for(auto& [id,t]:trains){
            auto& s=t.service;
            if(!(s.flags&NIMBY_SERVICE_RUN_VALID)||!lines.contains(s.line_id))continue;
            const auto& line=lines.at(s.line_id);const auto* p=line.data();
            const auto kind=field<int32_t>(p,0xfc);
            if(kind<0||kind>2)continue;
            s.line_kind=kind;s.flags|=NIMBY_SERVICE_LINE_VALID;
            const auto [cached,inserted]=sharedLines.try_emplace(s.line_id);
            auto& shared=cached->second;
            if(inserted)read_native_string(read,context,line_addresses.at(s.line_id)+0x78,p+0x78,state.profile,shared.name);
            std::memcpy(s.line_name_utf8,shared.name.data(),shared.name.size());
            lineUsers.emplace_back(&t,s.status);
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
                if(shared.planAttempted){
                    // The separately re-read current stop must belong to the
                    // same plan. Never combine it with an earlier cached row.
                    if(shared.planAvailable&&std::memcmp(stop.data(),shared.bytes.data()+size_t(s.stop_index)*0x158,stop.size()))shared.changed=true;
                    if(shared.planAvailable&&!shared.changed){t.line_stops=shared.stops;t.line_stops_available=true;stop_budget-=count;}
                }else{
                    const bool retain=end-begin<=maximumSharedLineBytes-sharedLineBytes;
                    if(retain)shared.planAttempted=true;
                    std::vector<unsigned char> bytes(end-begin),check(bytes.size());
                    if(read(context,begin,bytes.data(),bytes.size())&&read(context,begin,check.data(),check.size())&&bytes==check&&
                       read(context,line_addresses.at(s.line_id),line_after.data(),line_after.size())&&line_after==line&&
                       !std::memcmp(stop.data(),bytes.data()+size_t(s.stop_index)*0x158,stop.size())){
                        std::vector<NimbyLineStop> stops;stops.reserve(count);bool valid=true;
                        for(uint32_t index=0;index<count;++index){
                            const auto* entry=bytes.data()+size_t(index)*0x158;
                            NimbyLineStop v{};v.line_id=s.line_id;v.index=index;
                            v.track_id=field<uint64_t>(entry,0x78);v.station_id=field<uint64_t>(entry,0x110);
                            if((v.track_id>>48)!=1||(v.station_id&&(v.station_id>>48)!=2)){valid=false;break;}
                            const auto arrival=field<int32_t>(entry,0xb8),departure=field<int32_t>(entry,0xbc);
                            if(arrival>=0&&departure>=arrival){v.arrival_offset_seconds=arrival;v.departure_offset_seconds=departure;v.flags=NIMBY_LINE_STOP_TIMES_VALID;}
                            stops.push_back(v);
                        }
                        if(valid){
                            if(retain){
                                sharedLineBytes+=bytes.size();shared.begin=begin;shared.bytes=std::move(bytes);
                                shared.stops=std::move(stops);shared.planAvailable=true;t.line_stops=shared.stops;
                            }else t.line_stops=std::move(stops);
                            t.line_stops_available=true;stop_budget-=count;
                        }
                    }
                }
            }
            if(kind==1&&(s.motion_flags&NIMBY_MOTION_HIDDEN)&&(s.motion_flags&NIMBY_MOTION_RUN_STOP)&&
               !(s.motion_flags&NIMBY_MOTION_DRIVE)&&s.status!=NIMBY_SERVICE_MOTHBALLED)s.status=NIMBY_SERVICE_DEPOT;
        }
        // A later train may be read while the player edits a shared line in
        // place. Validate the actual bytes once more, including distant stops
        // not selected by any train. A changed plan invalidates all consumers
        // from this capture, without dropping unrelated trains or their speed.
        for(auto& [id,shared]:sharedLines){
            const auto& line=lines.at(id);std::array<unsigned char,0x280> after{};
            if(shared.planAvailable&&!shared.changed){
                std::vector<unsigned char> verify(shared.bytes.size());
                shared.changed=!read(context,shared.begin,verify.data(),verify.size())||verify!=shared.bytes;
            }
            if(!read(context,line_addresses.at(id),after.data(),after.size())||after!=line)shared.changed=true;
            std::string name;
            if(!read_native_string(read,context,line_addresses.at(id)+0x78,line.data()+0x78,state.profile,name)||name!=shared.name)
                shared.name.clear();
        }
        for(const auto& [train,originalStatus]:lineUsers){
            auto& s=train->service;const auto& shared=sharedLines.at(s.line_id);
            if(shared.name.empty())s.line_name_utf8[0]=0;
            if(shared.changed){
                s.flags&=~(NIMBY_SERVICE_LINE_VALID|NIMBY_SERVICE_STOP_VALID);
                s.line_kind=-1;s.line_name_utf8[0]=0;s.stop_track_id=s.stop_station_id=0;s.status=originalStatus;
                train->line_stops_available=false;train->line_stops.clear();
            }
        }
        TrainMetadataCatalog capturedMetadata;
        if(wantMetadata)try{
            if(wantMetadataSources)capturedMetadata.trains.reserve(trains.size());
            if(wantLines)capturedMetadata.lines.reserve(lines.size());
            if(wantTags)capturedMetadata.tag_states.reserve(trains.size()+lines.size());
            train_metadata::PoolEvidence trainPool,linePool,motionPool;
            if(wantMetadataSources||wantTags)trainPool.load(read,context,state.database+0x200);
            if(wantLines)linePool.load(read,context,state.database+0x180);
            if(wantMetadataSources)motionPool.load(read,context,state.simulation+0xa0);
            size_t references=train_metadata::maximumReferences;
            size_t carsRemaining=train_metadata::maximumCars;
            const auto appendTags=[&](uint64_t id,uint64_t address,size_t offset){
                std::vector<uint64_t> tags;
                const bool selected=(id>>48)==5?trainPool.contains(id,address,0x178):linePool.contains(id,address,0x280);
                const bool available=selected&&train_metadata::objectTags(read,context,address,id,offset,references,tags);
                capturedMetadata.tag_states.push_back({id,available?1u:0u,0});
                if(available)for(const auto tag:tags)capturedMetadata.object_tags.push_back({id,tag});
            };
            if(wantMetadataSources||wantTags)for(const auto& [id,train]:trains){
                NimbyTrainMetadata item{};item.train_id=id;
                if(wantMetadataSources)item=metadataSources.at(id).value;
                const auto address=model_addresses.at(id);uint64_t before{},after{};std::array<unsigned char,0x50> dynamics{};
                const bool selected=trainPool.contains(id,address,0x178)&&read(context,address,&before,8)&&before==id;
                if(selected&&wantCharacteristics&&read(context,address+0xc0,dynamics.data(),dynamics.size()))
                    item.configured=train_metadata::characteristics(metadataSources.at(id).configured.data(),dynamics.data(),dynamics.size());
                const auto statesBefore=capturedMetadata.tag_states.size(),tagsBefore=capturedMetadata.object_tags.size();
                if(wantTags)appendTags(id,address,0x80);
                const auto vehiclesBefore=capturedMetadata.vehicles.size();
                bool currentSelected=false;
                if(wantMetadataSources){
                    const auto& source=metadataSources.at(id);uint64_t motionId{};
                    currentSelected=source.motionAddress&&motionPool.contains(id,source.motionAddress,0x638)&&
                        read(context,source.motionAddress,&motionId,8)&&motionId==id;
                    if(wantComposition){
                        std::vector<NimbyTrainVehicle> cars;
                        if(selected&&train_metadata::cars(read,context,address,id,0xc0,source.configured.data(),0,carsRemaining,cars)){
                            item.configured.flags|=NIMBY_CHARACTERISTICS_COMPOSITION_VALID;
                            capturedMetadata.vehicles.insert(capturedMetadata.vehicles.end(),cars.begin(),cars.end());
                        }
                        if(currentSelected&&source.currentStable&&train_metadata::cars(read,context,source.motionAddress,id,8,source.current.data(),1,carsRemaining,cars)){
                            item.current.flags|=NIMBY_CHARACTERISTICS_COMPOSITION_VALID;
                            capturedMetadata.vehicles.insert(capturedMetadata.vehicles.end(),cars.begin(),cars.end());
                        }
                    }
                    currentSelected=currentSelected&&read(context,source.motionAddress,&motionId,8)&&motionId==id;
                    if(!currentSelected){item.current={};item.flags=0;item.predicted_arrival_delay_us=0;}
                }
                if(!selected||!read(context,address,&after,8)||after!=id){
                    item={};item.train_id=id;
                    capturedMetadata.tag_states.resize(statesBefore);if(wantTags)capturedMetadata.tag_states.push_back({id,0,0});
                    capturedMetadata.object_tags.resize(tagsBefore);
                    capturedMetadata.vehicles.resize(vehiclesBefore);
                }else if(!currentSelected){
                    // Only this train's just-added current composition is affected.
                    capturedMetadata.vehicles.erase(std::remove_if(capturedMetadata.vehicles.begin()+vehiclesBefore,capturedMetadata.vehicles.end(),
                        [](const auto& car){return car.composition==1;}),capturedMetadata.vehicles.end());
                }
                if(wantMetadataSources)capturedMetadata.trains.push_back(item);
            }
            capturedMetadata.lines_available=wantLines&&linesAvailable;
            if(wantLines)for(const auto& [id,bytes]:lines){
                const auto address=line_addresses.at(id);const auto* p=bytes.data();
                std::array<unsigned char,0x280> verify{};
                NimbyLineMetadata line{};line.line_id=id;line.kind=-1;
                const auto parent=field<uint64_t>(p,0x10);
                if(!parent||((parent>>48)==4&&parent!=id&&lines.contains(parent))){
                    line.parent_line_id=parent;line.flags|=NIMBY_LINE_PARENT_VALID;
                }
                const auto kind=field<int32_t>(p,0xfc);
                if(kind>=0&&kind<=2){line.kind=kind;line.flags|=NIMBY_LINE_KIND_VALID;}
                std::string name;
                if(read_native_string(read,context,address+0x78,p+0x78,state.profile,name)){
                    std::memcpy(line.name_utf8,name.data(),name.size());line.flags|=NIMBY_LINE_NAME_VALID;
                }
                const auto statesBefore=capturedMetadata.tag_states.size(),tagsBefore=capturedMetadata.object_tags.size();
                // The native root owns +c8; descendants own +e0. Inheritance
                // stays explicit in the parent ID and is resolved by the SDK.
                if(wantTags)appendTags(id,address,parent?0xe0:0xc8);
                const auto shared=sharedLines.find(id);
                if(!linePool.contains(id,address,0x280)||!read(context,address,verify.data(),verify.size())||verify!=bytes||
                   (shared!=sharedLines.end()&&shared->second.changed)){
                    line={};line.line_id=id;line.kind=-1;
                    capturedMetadata.tag_states.resize(statesBefore);if(wantTags)capturedMetadata.tag_states.push_back({id,0,0});
                    capturedMetadata.object_tags.resize(tagsBefore);
                }
                capturedMetadata.lines.push_back(line);
            }
            if(wantTags)capturedMetadata.tags_available=train_metadata::tagCatalog(read,context,state,capturedMetadata.tags);
            const bool stableTrains=trainPool.stable(read,context),stableLines=linePool.stable(read,context),stableMotion=motionPool.stable(read,context);
            if(!stableTrains||!stableLines){
                for(auto& tags:capturedMetadata.tag_states)if(((tags.object_id>>48)==5&&!stableTrains)||((tags.object_id>>48)==4&&!stableLines))tags.available=0;
                std::erase_if(capturedMetadata.object_tags,[&](const auto& tag){return (tag.object_id>>48)==5?!stableTrains:!stableLines;});
                if(!stableTrains){
                    for(auto& train:capturedMetadata.trains){const auto id=train.train_id;train={};train.train_id=id;}
                    capturedMetadata.vehicles.clear();
                }
                if(!stableLines){capturedMetadata.lines_available=false;for(auto& line:capturedMetadata.lines){const auto id=line.line_id;line={};line.line_id=id;line.kind=-1;}}
            }
            if(!stableMotion){
                for(auto& train:capturedMetadata.trains){train.current={};train.flags=0;train.predicted_arrival_delay_us=0;}
                std::erase_if(capturedMetadata.vehicles,[](const auto& car){return car.composition==1;});
            }
            if(wantComposition)capturedMetadata.models_available=train_metadata::vehicleModels(read,context,state,capturedMetadata.vehicles,capturedMetadata.models);
        }catch(...){capturedMetadata={};} // Optional enrichment cannot hide trains.
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
        for(auto& [id,t]:trains)out.push_back(std::move(t));
        if(metadata)*metadata=std::move(capturedMetadata);
        return true;
    }catch(...){out.clear();return false;}
}
}
