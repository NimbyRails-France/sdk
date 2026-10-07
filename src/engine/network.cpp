// Shared network topology: tracks, stations, signals and membership.
// Platform differences enter through ReadMemory and the target's GameLayout.
// Train decoding lives in trains.cpp; texture states in signal_texture_states.cpp.
#include "engine/network.h"
#include "engine/native_string.h"
#include "engine/simulation_clock.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <map>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include "engine/detail/memory_reader.h"
#include "engine/detail/position.h"
namespace nimby::engine {
using memory::field;
using memory::pointer;
using memory::collect;
using memory::position;
namespace {
// One bounded descriptor/table per referenced pool, never a global scan. The
// complete ID is checked at the addressed slot; an index alone is not identity.
class ReferencedPool {
    ReadMemory read_;void* context_;uint64_t pool_,tag_;size_t stride_;
    std::array<unsigned char,48> header_{};
    std::vector<uint64_t> blocks_;
    uint64_t table_{};uint32_t shift_{},mask_{};
    bool attempted_=false,ready_=false;
    bool load(){
        if(attempted_)return ready_;
        attempted_=true;
        if(!read_(context_,pool_,header_.data(),header_.size()))return false;
        shift_=field<uint32_t>(header_.data(),4);mask_=field<uint32_t>(header_.data(),16);
        const auto size=field<uint32_t>(header_.data(),8);
        table_=field<uint64_t>(header_.data(),24);
        const auto end=field<uint64_t>(header_.data(),32),cap=field<uint64_t>(header_.data(),40);
        if(shift_<1||shift_>16||size!=(1u<<shift_)||mask_!=size-1||end<table_||cap<end||
           cap-table_>8192||(end-table_)%8||((end-table_)/8)*size>1048576||
           (end!=table_&&(!pointer(table_)||cap>=0x7fffffff0000ULL)))return false;
        blocks_.resize((end-table_)/8);
        return ready_=blocks_.empty()||read_(context_,table_,blocks_.data(),blocks_.size()*8);
    }
public:
    ReferencedPool(ReadMemory read,void* context,uint64_t pool,uint64_t tag,size_t stride)
        :read_(read),context_(context),pool_(pool),tag_(tag),stride_(stride){}
    template<size_t N> bool record(uint64_t id,std::array<unsigned char,N>& out,uint64_t& address){
        if(id>>48!=tag_||N>stride_||!load())return false;
        const auto index=(id>>16)&0xffffffffULL,block=index>>shift_;
        if(block>=blocks_.size()||!pointer(blocks_[block])||
           blocks_[block]>0x7fffffff0000ULL-size_t(mask_+1)*stride_)return false;
        address=blocks_[block]+(index&mask_)*stride_;
        return read_(context_,address,out.data(),out.size())&&field<uint64_t>(out.data(),0)==id;
    }
    template<size_t N> bool same(uint64_t address,const std::array<unsigned char,N>& before){
        std::array<unsigned char,N> after{};
        return read_(context_,address,after.data(),after.size())&&before==after;
    }
    bool stable(){
        if(!attempted_)return true;
        if(!ready_)return false;
        std::array<unsigned char,48> after{};std::vector<uint64_t> blocks(blocks_.size());
        return read_(context_,pool_,after.data(),after.size())&&after==header_&&
            (blocks.empty()||(read_(context_,table_,blocks.data(),blocks.size()*8)&&blocks==blocks_))&&
            read_(context_,pool_,after.data(),after.size())&&after==header_;
    }
};
class BlockVerification {
    ReadMemory read_;void* context_;
    uint64_t blockAddress_{};bool blockRead_{};
    std::vector<unsigned char> verification_;
public:
    BlockVerification(ReadMemory read,void* context):read_(read),context_(context){}
    void prepare(std::span<const unsigned char> bytes,uint64_t address){
        blockRead_=false;blockAddress_=address;
        // Optional verification must also tolerate an allocation failure.
        // Its callers retain their existing bounded per-record fallback.
        try{verification_.resize(bytes.size());}catch(...){return;}
        blockRead_=read_(context_,address,verification_.data(),verification_.size());
    }
    const unsigned char* at(uint64_t address,size_t size)const{
        if(!blockRead_||address<blockAddress_||size>verification_.size()||
           address-blockAddress_>verification_.size()-size)return nullptr;
        return verification_.data()+static_cast<size_t>(address-blockAddress_);
    }
};
class TrackMetrics {
    ReadMemory read_;void* context_;size_t offset_{};BlockVerification verification_;
public:
    TrackMetrics(ReadMemory read,void* context,const LiveState& state,bool enabled)
        :read_(read),context_(context),offset_(enabled?gameLayout(state.profile).track_metric_offset:0),verification_(read,context){}
    void prepare(std::span<const unsigned char> bytes,uint64_t address){
        if(!offset_||offset_+sizeof(double)>0x90)return;
        // One bounded buffer per pool block, reused throughout the capture.
        // Only the same 0x90-byte prefix is compared for each individual row;
        // an unrelated edit never invalidates every length in the block.
        verification_.prepare(bytes,address);
    }
    double value(uint64_t address,const unsigned char* record){
        if(!offset_||offset_+sizeof(double)>0x90)return 0;
        const auto metric=field<double>(record,offset_);
        if(!std::isfinite(metric)||metric<=0)return 0;
        if(const auto* again=verification_.at(address,0x90))return std::memcmp(record,again,0x90)==0?metric:0;
        // A page becoming unreadable can fail the optional bulk verification.
        // Retain the previous per-row fallback so other readable rows keep
        // their lengths; no optional fault discards the base network.
        std::array<unsigned char,0x90> again{};
        return read_(context_,address,again.data(),again.size())&&
            std::memcmp(record,again.data(),again.size())==0?metric:0;
    }
};
bool geometry_endpoints(ReadMemory read,void* context,uint64_t begin,uint64_t end,
                        std::array<double,2>& first,std::array<double,2>& last){
    std::array<double,2> firstAgain{},lastAgain{};
    const auto size=static_cast<size_t>(end-begin);
    // Nearby endpoints share two small reads. Long geometries retain the
    // original four endpoint reads; no unbounded polyline copy is introduced.
    if(size<=4096){
        std::array<unsigned char,4096> before{},after{};
        if(read(context,begin,before.data(),size)&&read(context,begin,after.data(),size)){
            std::memcpy(first.data(),before.data(),16);std::memcpy(last.data(),before.data()+size-16,16);
            return !std::memcmp(before.data(),after.data(),16)&&
                !std::memcmp(before.data()+size-16,after.data()+size-16,16);
        }
        // A hole between endpoints must not hide endpoints that are readable.
    }
    return read(context,begin,first.data(),16)&&read(context,end-16,last.data(),16)&&
        read(context,begin,firstAgain.data(),16)&&read(context,end-16,lastAgain.data(),16)&&
        first==firstAgain&&last==lastAgain;
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
            std::string text;
            if(!read_native_string(read,context,address+8,p+8,state.profile,text))return true;
            // Reject missing terminators, embedded NULs and concurrent replacement.
            std::array<unsigned char,0x28> after{};
            if(!read(context,address,after.data(),after.size())||
               std::memcmp(p,after.data(),after.size())!=0)return true;
            names.emplace(id,std::move(text));
            return true;
        });
    // An unavailable cache must not hide otherwise valid network data.
    if(stable)for(auto& [id,name]:names)stations[automatic.at(id)].name=std::move(name);
}
}
const char* signal_kind_name(int kind) noexcept {
    switch(kind) {case 0:return "OneWay";case 1:return "PlatformStop";case 3:return "Balise";case 4:return "Path";case 5:return "NoWay";case 6:return "Marker";default:return "Unknown";}
}
bool read_train_network(ReadMemory read,void* context,const LiveState& state,bool recognized,
                        std::span<const Train> trains,Network& out) noexcept {
    out={};if(!read||!recognized)return false;
    LiveState current{};
    if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
    try{
        constexpr size_t maximumRecords=32768,maximumReferences=262144;
        std::set<uint64_t> trackIds,stationIds,lines;
        size_t references=0;
        const auto add=[&](std::set<uint64_t>& ids,uint64_t id,uint64_t tag){
            if((id>>48)==tag&&ids.size()<maximumRecords)ids.insert(id);
        };
        for(const auto& train:trains){
            if(++references>maximumReferences)break;
            if(train.positioned)add(trackIds,train.position.track_id,1);
            const auto& service=train.service;
            if(service.flags&NIMBY_SERVICE_LOCATION_VALID){add(trackIds,service.location_track_id,1);add(stationIds,service.location_station_id,2);}
            if(service.flags&NIMBY_SERVICE_STOP_VALID){add(trackIds,service.stop_track_id,1);add(stationIds,service.stop_station_id,2);}
            if(train.line_stops_available&&lines.insert(service.line_id).second){
                for(const auto& stop:train.line_stops){
                    if(++references>maximumReferences)break;
                    add(trackIds,stop.track_id,1);add(stationIds,stop.station_id,2);
                }
            }
        }
        Network result;
        ReferencedPool trackPool(read,context,state.database,1,0x4e8);
        ReferencedPool stationPool(read,context,state.database+0x80,2,0x3e8);
        ReferencedPool namesPool(read,context,state.database+0x430,2,0xf8);
        for(const auto id:trackIds){
            std::array<unsigned char,0xd8> bytes{};uint64_t address{};
            if(!trackPool.record(id,bytes,address)||!trackPool.same(address,bytes))continue;
            Track track{id,field<uint64_t>(bytes.data(),0xd0),field<float>(bytes.data(),0x80),field<float>(bytes.data(),0x84),0};
            if(!std::isfinite(track.physical_mps)||!std::isfinite(track.manual_mps)||track.physical_mps<0||
               track.physical_mps>10000||track.manual_mps>10000||(track.station_id&&track.station_id>>48!=2))continue;
            track.limit_mps=track.manual_mps<0.27777761220932007f?track.physical_mps:std::min(track.physical_mps,track.manual_mps);
            add(stationIds,track.station_id,2);result.tracks.push_back(track);
        }
        std::vector<size_t> automatic;
        for(const auto id:stationIds){
            std::array<unsigned char,0x48> bytes{};uint64_t address{};
            if(!stationPool.record(id,bytes,address)||bytes[0x40]>1)continue;
            Station station{id,{}};
            if(!bytes[0x40]){
                // Invalid/absent names do not invent a label for this station.
                read_native_string(read,context,address+0x20,bytes.data()+0x20,state.profile,station.name);
            }else{
                std::array<unsigned char,0x28> name{};uint64_t nameAddress{};
                if(!namesPool.record(id,name,nameAddress)||
                   !read_native_string(read,context,nameAddress+8,name.data()+8,state.profile,station.name)||
                   !namesPool.same(nameAddress,name))station.name.clear();
            }
            if(!stationPool.same(address,bytes))continue;
            if(bytes[0x40])automatic.push_back(result.stations.size());
            result.stations.push_back(std::move(station));
        }
        // A changed descriptor/table invalidates all rows selected through it,
        // even if the obsolete backing memory is still readable unchanged.
        if(!trackPool.stable())result.tracks.clear();
        if(!namesPool.stable())for(auto index:automatic)result.stations[index].name.clear();
        if(!stationPool.stable())result.stations.clear();
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
        out=std::move(result);return true;
    }catch(...){
        // These are optional labels/limits. Preserve otherwise valid trains
        // on allocation/read failure, but still reject a replaced world.
        return resolve_live_state(read,context,state.module_base,true,state.profile,current)&&current==state;
    }
}
bool read_network(ReadMemory read,void* context,const LiveState& state,bool recognized,Network& out,NetworkScope scope) noexcept {
    out={};if(!read||!recognized) return false;
    const bool signallingOnly=scope==NetworkScope::Signalling;
    const bool topologyOnly=scope==NetworkScope::Topology;
    const bool stationDetails=scope==NetworkScope::Complete;
    try {
        LiveState current{};
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state) return false;
        Network result;
        struct Label { NimbyPlatform value{}; bool automatic{},geometry_valid{}; int32_t number{}; double dx{},dy{}; uint64_t links[2]{}; };
        std::vector<Label> labels;
        struct Attachment { uint64_t address{}; std::array<unsigned char,0x30> identity{}; std::array<unsigned char,0x30> data{}; };
        std::unordered_map<uint64_t,Attachment> attachments;
        attachments.reserve(4096);
        TrackMetrics metrics(read,context,state,!signallingOnly);
        if(!collect(read,context,state.database,1,0x4e8,[&](const unsigned char* p,uint64_t address){
            Track t{field<uint64_t>(p,0),field<uint64_t>(p,0xd0),field<float>(p,0x80),field<float>(p,0x84),0};
            if(!std::isfinite(t.physical_mps)||!std::isfinite(t.manual_mps)||t.physical_mps<0||t.physical_mps>10000||t.manual_mps>10000) return false;
            if(t.station_id&&(t.station_id>>48)!=2) return false;
            t.links[0]=field<uint64_t>(p,8);t.links[1]=field<uint64_t>(p,16);
            t.x=field<double>(p,0x30);t.y=field<double>(p,0x38);
            t.geometry=std::isfinite(t.x)&&std::isfinite(t.y)&&std::abs(t.x)<1e9&&std::abs(t.y)<1e9;
            if(!signallingOnly)t.native_length_m=metrics.value(address,p);
            Attachment attachment;attachment.address=address;
            std::memcpy(attachment.identity.data(),p,0x30);
            std::memcpy(attachment.data.data(),p+0x3f0,0x30);
            if(field<uint64_t>(p,0x3f0)||field<uint64_t>(p,0x408)!=field<uint64_t>(p,0x410))
                attachments.emplace(t.id,attachment);
            // RVA 0x4282f0 and threshold at RVA 0xaab904 (float 1/3.6).
            t.limit_mps=t.manual_mps<0.27777761220932007f?t.physical_mps:std::min(t.physical_mps,t.manual_mps);
            if(t.station_id&&stationDetails){
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
                            std::array<double,2> first{},last{};
                            if(geometry_endpoints(read,context,begin,end,first,last)){
                                label.dx=last[0]-first[0];label.dy=last[1]-first[1];
                                label.geometry_valid=std::isfinite(label.dx)&&std::isfinite(label.dy);
                            }
                        }
                    }
                }else if(valid){
                    std::string name;
                    valid=read_native_string(read,context,address+0xa8,p+0xa8,state.profile,name);
                    if(valid)std::memcpy(label.value.name_utf8,name.data(),name.size());
                }
                std::array<unsigned char,0x1c0> verify{};
                valid=valid&&read(context,address,verify.data(),verify.size())&&
                    !std::memcmp(p,verify.data(),24)&&!std::memcmp(p+0xa0,verify.data()+0xa0,0x38)&&
                    !std::memcmp(p+0x1b0,verify.data()+0x1b0,16);
                if(valid)label.value.flags=NIMBY_PLATFORM_NAME_VALID;
                labels.push_back(label);
            }
            result.tracks.push_back(t);return true;
        },[&](std::span<const unsigned char> bytes,uint64_t address){metrics.prepare(bytes,address);})) return false;
        // Native topology: Track+3f0 parent ID, +3f8 attachment fraction,
        // +400 approach direction; parent+408 lists its attached branches.
        // Evidence: RVA 379d10 / 37cf30, docs/research/track-junctions.md.
        struct JunctionCandidate {const Track* track;const Attachment* branch;uint64_t parent;double fraction;int direction;};
        std::vector<JunctionCandidate> candidates;
        std::unordered_map<uint64_t,std::vector<uint64_t>> childrenByParent;
        for(const auto& t:result.tracks){
            const auto candidate=attachments.find(t.id);if(candidate==attachments.end())continue;
            const auto& a=candidate->second;const auto* p=a.data.data();
            const auto parent=field<uint64_t>(p,0);const auto fraction=field<double>(p,8);
            const auto direction=field<int32_t>(p,16);
            if(!parent||parent==t.id||!t.geometry||(t.links[0]==0)==(t.links[1]==0)||
               !std::isfinite(fraction)||fraction<0||fraction>1||(direction!=1&&direction!=-1))continue;
            const auto found=attachments.find(parent);if(found==attachments.end())continue;
            const auto& main=found->second;
            const auto [group,inserted]=childrenByParent.try_emplace(parent);
            auto& ids=group->second;
            if(inserted){
                const auto begin=field<uint64_t>(main.data.data(),24),end=field<uint64_t>(main.data.data(),32),cap=field<uint64_t>(main.data.data(),40);
                if(pointer(begin)&&end>=begin&&cap>=end&&(end-begin)%8==0&&cap-begin<=4096*8){
                    ids.resize((end-begin)/8);std::vector<uint64_t> again(ids.size());
                    if(!ids.empty()&&(!read(context,begin,ids.data(),ids.size()*8)||
                       !read(context,begin,again.data(),again.size()*8)||ids!=again))ids.clear();
                    std::sort(ids.begin(),ids.end());
                }
            }
            const auto matches=std::equal_range(ids.begin(),ids.end(),t.id);
            if(matches.second-matches.first==1)candidates.push_back({&t,&a,parent,fraction,direction});
        }
        // Read all indirect child lists first, then verify each identity and
        // attachment header once. Shared parents no longer cause repeated IO,
        // and every guard still runs after the dependent arrays were read.
        std::unordered_map<uint64_t,bool> stableAttachments;
        stableAttachments.reserve(candidates.size()+childrenByParent.size());
        const auto stable=[&](uint64_t id,const Attachment& row){
            const auto [entry,inserted]=stableAttachments.try_emplace(id,false);
            if(inserted){
                std::array<unsigned char,0x30> identity{},data{};
                entry->second=read(context,row.address,identity.data(),identity.size())&&identity==row.identity&&
                    read(context,row.address+0x3f0,data.data(),data.size())&&data==row.data;
            }
            return entry->second;
        };
        for(const auto& candidate:candidates){
            const auto& t=*candidate.track;
            if(stable(t.id,*candidate.branch)&&stable(candidate.parent,attachments.at(candidate.parent)))
                result.junctions.push_back({t.id,candidate.parent,candidate.fraction,candidate.direction,t.links[0]==0?1:-1});
        }
        std::unordered_map<uint64_t,uint64_t> track_stations;
        if(!labels.empty()){
            track_stations.reserve(result.tracks.size());
            for(const auto& track:result.tracks)track_stations.emplace(track.id,track.station_id);
        }
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
        if(stationDetails&&!collect(read,context,state.database+0x80,2,0x3e8,[&](const unsigned char* p,uint64_t address){
            Station s{field<uint64_t>(p,0),{}};
            if(p[0x40]>1) return false;
            if(!p[0x40]) {
                if(!read_native_string(read,context,address+0x20,p+0x20,state.profile,s.name))return false;
            }
            if(p[0x40])automatic_stations.emplace(s.id,result.stations.size());
            result.stations.push_back(std::move(s));return true;
        })) return false;
        if(stationDetails)resolve_station_names(read,context,state,automatic_stations,result.stations);
        BlockVerification signalVerification(read,context);
        if(!collect(read,context,state.database+0x380,8,0xc8,[&](const unsigned char* p,uint64_t address){
            TrainPosition pos;
            if(!position(p+0x40,pos)) return false;
            const int kind=field<int32_t>(p,0x30);
            if(kind<0||kind>6) return false;
            Signal signal{field<uint64_t>(p,0),pos.track_id,pos.fraction,pos.direction,kind,field<uint64_t>(p,0x38),false,false,0,false,{}};
            // Signal editor RVA 0x79f4d0: exceptions at +78/+80/+88.
            // RVA 0x7a0a40: default filter mode at +70 (0 applies, 1 ignored).
            const auto mode=field<uint32_t>(p,0x70);
            const auto begin=field<uint64_t>(p,0x78),end=field<uint64_t>(p,0x80),cap=field<uint64_t>(p,0x88);
            if(!topologyOnly&&mode<=1&&end>=begin&&cap>=end&&cap-begin<=32768&&(end-begin)%8==0&&
               (begin==0?(end==0&&cap==0):pointer(begin))){
                std::vector<uint64_t> tags((end-begin)/8),again(tags.size());
                std::array<unsigned char,32> header{};uint64_t verify_id{};
                const auto stableHeader=[&]{
                    // Empty filters have no indirect data to validate first.
                    // Compare just the original ID and filter header so an
                    // unrelated signal edit does not discard this filter.
                    if(tags.empty())if(const auto* verified=signalVerification.at(address,0x90))
                        return field<uint64_t>(verified,0)==signal.id&&std::memcmp(verified+0x70,p+0x70,32)==0;
                    return read(context,address+0x70,header.data(),header.size())&&std::memcmp(header.data(),p+0x70,header.size())==0&&
                        read(context,address,&verify_id,8)&&verify_id==signal.id;
                };
                if((tags.empty()||(read(context,begin,tags.data(),tags.size()*8)&&read(context,begin,again.data(),again.size()*8)&&tags==again))&&
                   stableHeader()){
                    signal.filter_available=true;signal.filter_default_ignored=mode==1;signal.exception_count=static_cast<uint32_t>(tags.size());
                }
            }
            // Native script-extension research stays opt-in. The C++ mod UI
            // must not require NimbyScript or add per-signal reads for it.
            result.signals.push_back(std::move(signal));return true;
        },[&](std::span<const unsigned char> bytes,uint64_t address){if(!topologyOnly)signalVerification.prepare(bytes,address);})) return false;
        // Reject unresolved references, including IDs whose generation changed.
        std::unordered_set<uint64_t> station_ids,track_ids;
        station_ids.reserve(result.stations.size());track_ids.reserve(result.tracks.size());
        for(const auto& s:result.stations)station_ids.insert(s.id);
        for(const auto& t:result.tracks)track_ids.insert(t.id);
        if(stationDetails)for(const auto& t:result.tracks) if(t.station_id&&!station_ids.contains(t.station_id)) return false;
        for(const auto& s:result.signals) if(!track_ids.contains(s.track_id)) return false;
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state) return false;
        out=std::move(result);return true;
    } catch(...) {out={};return false;}
}
bool read_signalling_network(ReadMemory read,void* context,const LiveState& state,uint64_t texturesHash,Network& out) noexcept {
    const SignallingScope scope{texturesHash,0};
    return read_signalling_network(read,context,state,std::span(&scope,1),out);
}
bool read_signalling_network(ReadMemory read,void* context,const LiveState& state,std::span<const SignallingScope> scopes,Network& out) noexcept {
    out={};if(!read)return false;
    try {
        if(scopes.empty()||scopes.size()>16)return false;
        std::map<uint64_t,uint32_t> selectedHashes;
        for(const auto& scope:scopes)
            if(scope.approach_blocks>16||!selectedHashes.emplace(scope.textures_hash,scope.approach_blocks).second)return false;
        LiveState current{};
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
        std::vector<Signal> signals;
        // Discover new/deleted/moved boundaries every time, including other mods.
        if(!collect(read,context,state.database+0x380,8,0xc8,[&](const unsigned char* p,uint64_t){
            TrainPosition pos;if(!position(p+0x40,pos))return false;
            const auto kind=field<int32_t>(p,0x30);if(kind<0||kind>6)return false;
            signals.push_back({field<uint64_t>(p,0),pos.track_id,pos.fraction,pos.direction,kind,field<uint64_t>(p,0x38),false,false,0,false,{}});
            return true;
        }))return false;
        std::map<uint64_t,std::array<unsigned char,0x4e8>> records;
        // Pool descriptors are shared by every track in this capture. Read
        // them once and validate again before publication; retain two reads
        // of each selected record to reject concurrent replacement/mutation.
        std::array<unsigned char,48> poolHeader{};
        std::vector<uint64_t> poolBlocks;
        uint64_t poolBegin{};
        uint32_t poolShift{},poolMask{};
        bool poolReady=false;
        auto stablePool=[&]() {
            std::array<unsigned char,48> again{};
            std::vector<uint64_t> blocks(poolBlocks.size());
            return read(context,state.database,again.data(),again.size())&&again==poolHeader&&
                (blocks.empty()||(read(context,poolBegin,blocks.data(),blocks.size()*8)&&blocks==poolBlocks))&&
                read(context,state.database,again.data(),again.size())&&again==poolHeader;
        };
        auto track=[&](uint64_t id)->const std::array<unsigned char,0x4e8>* {
            if(auto it=records.find(id);it!=records.end())return &it->second;
            if((id>>48)!=1||records.size()>=32768)return nullptr;
            if(!poolReady){
                if(!read(context,state.database,poolHeader.data(),poolHeader.size()))return nullptr;
                poolShift=field<uint32_t>(poolHeader.data(),4);poolMask=field<uint32_t>(poolHeader.data(),16);
                const auto size=field<uint32_t>(poolHeader.data(),8);
                poolBegin=field<uint64_t>(poolHeader.data(),24);
                const auto end=field<uint64_t>(poolHeader.data(),32),cap=field<uint64_t>(poolHeader.data(),40);
                if(!poolShift||poolShift>16||size!=(1u<<poolShift)||poolMask!=size-1||end<poolBegin||cap<end||
                    cap-poolBegin>8192||(end-poolBegin)%8||!pointer(poolBegin))return nullptr;
                poolBlocks.resize((end-poolBegin)/8);
                if(!poolBlocks.empty()&&!read(context,poolBegin,poolBlocks.data(),poolBlocks.size()*8))return nullptr;
                if(!stablePool())return nullptr;
                poolReady=true;
            }
            const auto index=(id>>16)&0xffffffffULL,blockIndex=index>>poolShift;
            if(blockIndex>=poolBlocks.size()||!pointer(poolBlocks[blockIndex]))return nullptr;
            const auto address=poolBlocks[blockIndex]+(index&poolMask)*0x4e8;
            std::array<unsigned char,0x4e8> bytes{},verify{};
            if(!read(context,address,bytes.data(),bytes.size())||field<uint64_t>(bytes.data(),0)!=id||
               !read(context,address,verify.data(),verify.size())||bytes!=verify)return nullptr;
            const auto x=field<double>(bytes.data(),0x30),y=field<double>(bytes.data(),0x38);
            if(!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>=1e9||std::abs(y)>=1e9)return nullptr;
            return &records.emplace(id,bytes).first->second;
        };
        std::map<uint64_t,NimbyTrackJunction> junctions;
        std::map<uint64_t,std::vector<NimbyTrackJunction>> junctionsByTrack;
        auto attachments=[&](uint64_t id,const unsigned char* p)->bool {
            if(junctionsByTrack.contains(id))return true;
            const auto begin=field<uint64_t>(p,0x408),end=field<uint64_t>(p,0x410),cap=field<uint64_t>(p,0x418);
            if(end<begin||cap<end||(end-begin)%8||cap-begin>4096*8||(end!=begin&&!pointer(begin)))return false;
            std::vector<uint64_t> children((end-begin)/8),again(children.size());
            if(!children.empty()&&(!read(context,begin,children.data(),children.size()*8)||
               !read(context,begin,again.data(),again.size()*8)||children!=again))return false;
            std::vector<NimbyTrackJunction> group;group.reserve(children.size());
            for(auto child:children){
                const auto* record=track(child);if(!record)return false;
                const auto* b=record->data();const auto a=field<uint64_t>(b,8),z=field<uint64_t>(b,16);
                const auto fraction=field<double>(b,0x3f8);const auto direction=field<int32_t>(b,0x400);
                if(field<uint64_t>(b,0x3f0)!=id||(a==0)==(z==0)||!std::isfinite(fraction)||fraction<0||fraction>1||(direction!=1&&direction!=-1))return false;
                junctions[child]={child,id,fraction,direction,a==0?1:-1};
                group.push_back(junctions[child]);
            }
            junctionsByTrack.emplace(id,std::move(group));
            return true;
        };
        // One contiguous index avoids a tree node and a separate allocation
        // for nearly every signal in large maps. It is rebuilt from this
        // capture; a new or moved signal remains a boundary immediately.
        std::vector<const Signal*> signalIndex;signalIndex.reserve(signals.size());
        for(const auto& signal:signals)if(signal.kind==4)signalIndex.push_back(&signal);
        std::sort(signalIndex.begin(),signalIndex.end(),[](const Signal* a,const Signal* b){
            return std::tie(a->track_id,a->id)<std::tie(b->track_id,b->id);
        });
        const auto signalsOn=[&](uint64_t id){
            const auto begin=std::lower_bound(signalIndex.begin(),signalIndex.end(),id,
                [](const Signal* signal,uint64_t track){return signal->track_id<track;});
            const auto end=std::upper_bound(begin,signalIndex.end(),id,
                [](uint64_t track,const Signal* signal){return track<signal->track_id;});
            return std::span<const Signal* const>(begin,end);
        };
        size_t selected=0;
        for(const auto& seed:signals)if(selectedHashes.contains(seed.textures_hash)&&seed.kind==4){
            if(++selected>4096)return false;
            auto id=seed.track_id;auto fraction=seed.fraction;auto direction=-seed.direction;
            for(unsigned step=0;step<256;++step){
                const auto* record=track(id);if(!record)return false;
                const auto* p=record->data();if(!attachments(id,p))return false;
                std::optional<double> boundary;
                for(const auto* signal:signalsOn(id))if(signal->id!=seed.id&&-signal->direction==direction&&
                    (direction==1?signal->fraction>=fraction:signal->fraction<=fraction))
                    if(!boundary||(direction==1?signal->fraction<*boundary:signal->fraction>*boundary))boundary=signal->fraction;
                bool fork=false;
                for(const auto& j:junctionsByTrack.at(id))if(j.main_direction==direction&&
                    (direction==1?j.main_fraction>=fraction:j.main_fraction<=fraction)&&
                    (!boundary||(direction==1?j.main_fraction<=*boundary:j.main_fraction>=*boundary)))fork=true;
                if(boundary||fork)break;
                const auto next=field<uint64_t>(p,direction==1?16:8);
                if(next){
                    const auto* following=track(next);if(!following)return false;
                    const bool a=field<uint64_t>(following->data(),8)==id,b=field<uint64_t>(following->data(),16)==id;
                    if(a==b)return false;
                    id=next;direction=a?1:-1;fraction=a?0.:1.;
                }else{
                    const auto parent=field<uint64_t>(p,0x3f0);if(!parent)break;
                    const auto* main=track(parent);if(!main||!attachments(parent,main->data()))return false;
                    const auto found=junctions.find(id);if(found==junctions.end())return false;
                    const auto& j=found->second;if(direction!=-j.branch_direction)break;
                    id=parent;fraction=j.main_fraction;direction=-j.main_direction;
                }
            }
            // Walk backwards from this model, counting facing signals rather
            // than metres or real time. A merging junction has several possible
            // upstream approaches: retain their geometry, then let the ordinary
            // forward topology reader reject ambiguous train routes as before.
            struct Cursor {uint64_t track;double fraction;int direction;uint32_t blocks;unsigned steps;uint64_t exclude;};
            std::vector<Cursor> pending;
            if(const auto blocks=selectedHashes.at(seed.textures_hash))
                pending.push_back({seed.track_id,seed.fraction,seed.direction,blocks,0,seed.id});
            std::set<std::tuple<uint64_t,double,int,uint32_t,uint64_t>> visited;
            while(!pending.empty()){
                const auto cursor=pending.back();pending.pop_back();
                if(cursor.steps>=256||!visited.emplace(cursor.track,cursor.fraction,cursor.direction,cursor.blocks,cursor.exclude).second)continue;
                if(visited.size()>32768)return false;
                const auto* record=track(cursor.track);if(!record)return false;
                const auto* p=record->data();if(!attachments(cursor.track,p))return false;
                const auto direction=cursor.direction;
                const Signal* boundary=nullptr;
                for(const auto* signal:signalsOn(cursor.track))
                    if(signal->id!=cursor.exclude&&signal->direction==direction&&
                       (direction==1?signal->fraction>=cursor.fraction:signal->fraction<=cursor.fraction)&&
                       (!boundary||(direction==1?signal->fraction<boundary->fraction:signal->fraction>boundary->fraction)))boundary=signal;
                const double end=boundary?boundary->fraction:(direction==1?1.:0.);
                // Branches merging into the observed forward direction. Include
                // every candidate, never infer which one an approaching train uses.
                for(const auto& j:junctionsByTrack.at(cursor.track))if(j.main_direction==direction&&
                    (direction==1?j.main_fraction>=cursor.fraction&&j.main_fraction<=end:j.main_fraction<=cursor.fraction&&j.main_fraction>=end))
                    pending.push_back({j.branch_track_id,j.branch_direction==1?0.:1.,j.branch_direction,cursor.blocks,cursor.steps+1,0});
                if(boundary){
                    if(cursor.blocks>1)pending.push_back({cursor.track,boundary->fraction,direction,cursor.blocks-1,cursor.steps+1,boundary->id});
                    continue;
                }
                const auto next=field<uint64_t>(p,direction==1?16:8);
                if(next){
                    const auto* following=track(next);if(!following)return false;
                    const bool a=field<uint64_t>(following->data(),8)==cursor.track,b=field<uint64_t>(following->data(),16)==cursor.track;
                    if(a==b)return false;
                    pending.push_back({next,a?0.:1.,a?1:-1,cursor.blocks,cursor.steps+1,0});
                }else if(const auto parent=field<uint64_t>(p,0x3f0)){
                    const auto* main=track(parent);if(!main||!attachments(parent,main->data()))return false;
                    const auto found=junctions.find(cursor.track);if(found==junctions.end())return false;
                    const auto& j=found->second;
                    if(direction==-j.branch_direction)pending.push_back({parent,j.main_fraction,-j.main_direction,cursor.blocks,cursor.steps+1,0});
                }
            }
        }
        if(poolReady&&!stablePool())return false;
        for(const auto& [id,record]:records){
            const auto* p=record.data();Track t{};t.id=id;t.links[0]=field<uint64_t>(p,8);t.links[1]=field<uint64_t>(p,16);
            t.x=field<double>(p,0x30);t.y=field<double>(p,0x38);
            t.geometry=std::isfinite(t.x)&&std::isfinite(t.y)&&std::abs(t.x)<1e9&&std::abs(t.y)<1e9;
            out.tracks.push_back(t);
        }
        for(const auto& signal:signals)if(records.contains(signal.track_id)||selectedHashes.contains(signal.textures_hash))out.signals.push_back(signal);
        for(const auto& [id,j]:junctions)out.junctions.push_back(j);
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state){out={};return false;}
        return true;
    }catch(...){out={};return false;}
}
bool read_signal_membership(ReadMemory read,void* context,const LiveState& state,
                            bool recognized,uint64_t signal,bool& found) noexcept {
    found=false;
    if(!read||!recognized||(signal>>48)!=8)return false;
    LiveState current{};
    if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
    std::array<unsigned char,48> header{},after{};
    const auto address=state.database+0x380;
    if(!read(context,address,header.data(),header.size()))return false;
    const auto shift=field<uint32_t>(header.data(),4),size=field<uint32_t>(header.data(),8);
    const auto mask=field<uint32_t>(header.data(),16);
    const auto begin=field<uint64_t>(header.data(),24),end=field<uint64_t>(header.data(),32),cap=field<uint64_t>(header.data(),40);
    if(!shift||shift>16||size!=(1u<<shift)||mask!=size-1||end<begin||cap<end||
       cap-begin>8192||(end-begin)%8||(end!=begin&&!pointer(begin))||
       ((end-begin)/8)*size>1048576)return false;
    const auto index=(signal>>16)&0xffffffffULL,blockIndex=index>>shift;
    uint64_t block{},blockAfter{},id{},idAfter{};
    const bool allocated=blockIndex<(end-begin)/8;
    if(allocated){
        if(!read(context,begin+blockIndex*8,&block,8)||!pointer(block))return false;
        const auto slot=block+(index&mask)*0xc8;
        // Verify the full ID, including generation, twice. Never accept a reused slot.
        if(!read(context,slot,&id,8)||!read(context,slot,&idAfter,8)||id!=idAfter||
           !read(context,begin+blockIndex*8,&blockAfter,8)||block!=blockAfter)return false;
    }
    if(!read(context,address,after.data(),after.size())||header!=after||
       !resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
    found=allocated&&id==signal;
    return true;
}
}
