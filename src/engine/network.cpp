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
#include <unordered_set>
#include "engine/detail/memory_reader.h"
#include "engine/detail/position.h"
namespace nimby::engine {
using memory::field;
using memory::pointer;
using memory::collect;
using memory::position;
namespace {
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
bool read_network(ReadMemory read,void* context,const LiveState& state,bool recognized,Network& out,bool signallingOnly) noexcept {
    out={};if(!read||!recognized) return false;
    try {
        LiveState current{};
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state) return false;
        Network result;
        struct Label { NimbyPlatform value{}; bool automatic{},geometry_valid{}; int32_t number{}; double dx{},dy{}; uint64_t links[2]{}; };
        std::vector<Label> labels;
        struct Attachment { uint64_t address{}; std::array<unsigned char,0x30> identity{}; std::array<unsigned char,0x30> data{}; };
        std::map<uint64_t,Attachment> attachments;
        if(!collect(read,context,state.database,1,0x4e8,[&](const unsigned char* p,uint64_t address){
            Track t{field<uint64_t>(p,0),field<uint64_t>(p,0xd0),field<float>(p,0x80),field<float>(p,0x84),0};
            if(!std::isfinite(t.physical_mps)||!std::isfinite(t.manual_mps)||t.physical_mps<0||t.physical_mps>10000||t.manual_mps>10000) return false;
            if(t.station_id&&(t.station_id>>48)!=2) return false;
            t.links[0]=field<uint64_t>(p,8);t.links[1]=field<uint64_t>(p,16);
            t.x=field<double>(p,0x30);t.y=field<double>(p,0x38);
            t.geometry=std::isfinite(t.x)&&std::isfinite(t.y)&&std::abs(t.x)<1e9&&std::abs(t.y)<1e9;
            Attachment attachment;attachment.address=address;
            std::memcpy(attachment.identity.data(),p,0x30);
            std::memcpy(attachment.data.data(),p+0x3f0,0x30);
            if(field<uint64_t>(p,0x3f0)||field<uint64_t>(p,0x408)!=field<uint64_t>(p,0x410))
                attachments.emplace(t.id,attachment);
            // RVA 0x4282f0 and threshold at RVA 0xaab904 (float 1/3.6).
            t.limit_mps=t.manual_mps<0.27777761220932007f?t.physical_mps:std::min(t.physical_mps,t.manual_mps);
            if(t.station_id&&!signallingOnly){
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
        })) return false;
        // Native topology: Track+3f0 parent ID, +3f8 attachment fraction,
        // +400 approach direction; parent+408 lists its attached branches.
        // Evidence: RVA 379d10 / 37cf30, docs/research/track-junctions.md.
        for(const auto& t:result.tracks){
            const auto candidate=attachments.find(t.id);if(candidate==attachments.end())continue;
            const auto& a=candidate->second;const auto* p=a.data.data();
            const auto parent=field<uint64_t>(p,0);const auto fraction=field<double>(p,8);
            const auto direction=field<int32_t>(p,16);
            if(!parent||parent==t.id||!t.geometry||(t.links[0]==0)==(t.links[1]==0)||
               !std::isfinite(fraction)||fraction<0||fraction>1||(direction!=1&&direction!=-1))continue;
            const auto found=attachments.find(parent);if(found==attachments.end())continue;
            const auto& main=found->second;
            const auto begin=field<uint64_t>(main.data.data(),24),end=field<uint64_t>(main.data.data(),32),cap=field<uint64_t>(main.data.data(),40);
            if(!pointer(begin)||end<begin||cap<end||(end-begin)%8||cap-begin>4096*8)continue;
            std::vector<uint64_t> ids((end-begin)/8),again(ids.size());
            if(ids.empty()||!read(context,begin,ids.data(),ids.size()*8)||
               std::count(ids.begin(),ids.end(),t.id)!=1||
               !read(context,begin,again.data(),again.size()*8)||ids!=again)continue;
            auto stable=[&](const Attachment& row){
                std::array<unsigned char,0x30> identity{},data{};
                return read(context,row.address,identity.data(),identity.size())&&identity==row.identity&&
                    read(context,row.address+0x3f0,data.data(),data.size())&&data==row.data;
            };
            if(stable(a)&&stable(main))result.junctions.push_back({t.id,parent,fraction,direction,t.links[0]==0?1:-1});
        }
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
        if(!signallingOnly&&!collect(read,context,state.database+0x80,2,0x3e8,[&](const unsigned char* p,uint64_t address){
            Station s{field<uint64_t>(p,0),{}};
            if(p[0x40]>1) return false;
            if(!p[0x40]) {
                if(!read_native_string(read,context,address+0x20,p+0x20,state.profile,s.name))return false;
            }
            if(p[0x40])automatic_stations.emplace(s.id,result.stations.size());
            result.stations.push_back(std::move(s));return true;
        })) return false;
        if(!signallingOnly)resolve_station_names(read,context,state,automatic_stations,result.stations);
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
            if(mode<=1&&end>=begin&&cap>=end&&cap-begin<=32768&&(end-begin)%8==0&&
               (begin==0?(end==0&&cap==0):pointer(begin))){
                std::vector<uint64_t> tags((end-begin)/8),again(tags.size());
                std::array<unsigned char,32> header{};uint64_t verify_id{};
                if((tags.empty()||(read(context,begin,tags.data(),tags.size()*8)&&read(context,begin,again.data(),again.size()*8)&&tags==again))&&
                   read(context,address+0x70,header.data(),header.size())&&std::memcmp(header.data(),p+0x70,header.size())==0&&
                   read(context,address,&verify_id,8)&&verify_id==signal.id){
                    signal.filter_available=true;signal.filter_default_ignored=mode==1;signal.exception_count=static_cast<uint32_t>(tags.size());
                }
            }
            // Native script-extension research stays opt-in. The C++ mod UI
            // must not require NimbyScript or add per-signal reads for it.
            result.signals.push_back(std::move(signal));return true;
        })) return false;
        // Reject unresolved references, including IDs whose generation changed.
        std::unordered_set<uint64_t> station_ids,track_ids;
        for(const auto& s:result.stations)station_ids.insert(s.id);
        for(const auto& t:result.tracks)track_ids.insert(t.id);
        if(!signallingOnly)for(const auto& t:result.tracks) if(t.station_id&&!station_ids.contains(t.station_id)) return false;
        for(const auto& s:result.signals) if(!track_ids.contains(s.track_id)) return false;
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state) return false;
        out=std::move(result);return true;
    } catch(...) {out={};return false;}
}
bool read_signalling_network(ReadMemory read,void* context,const LiveState& state,uint64_t texturesHash,Network& out) noexcept {
    out={};if(!read)return false;
    try {
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
        auto track=[&](uint64_t id)->const std::array<unsigned char,0x4e8>* {
            if(auto it=records.find(id);it!=records.end())return &it->second;
            if((id>>48)!=1||records.size()>=32768)return nullptr;
            std::array<unsigned char,48> header{},again{};
            if(!read(context,state.database,header.data(),header.size()))return nullptr;
            const auto shift=field<uint32_t>(header.data(),4),size=field<uint32_t>(header.data(),8),mask=field<uint32_t>(header.data(),16);
            const auto begin=field<uint64_t>(header.data(),24),end=field<uint64_t>(header.data(),32),cap=field<uint64_t>(header.data(),40);
            if(!shift||shift>16||size!=(1u<<shift)||mask!=size-1||end<begin||cap<end||cap-begin>8192||(end-begin)%8||!pointer(begin))return nullptr;
            const auto index=(id>>16)&0xffffffffULL,blockIndex=index>>shift;
            if(blockIndex>=(end-begin)/8)return nullptr;
            uint64_t block{},blockAgain{};
            if(!read(context,begin+blockIndex*8,&block,8)||!pointer(block))return nullptr;
            const auto address=block+(index&mask)*0x4e8;
            std::array<unsigned char,0x4e8> bytes{},verify{};
            if(!read(context,address,bytes.data(),bytes.size())||field<uint64_t>(bytes.data(),0)!=id||
               !read(context,address,verify.data(),verify.size())||bytes!=verify||
               !read(context,begin+blockIndex*8,&blockAgain,8)||block!=blockAgain||
               !read(context,state.database,again.data(),again.size())||header!=again)return nullptr;
            const auto x=field<double>(bytes.data(),0x30),y=field<double>(bytes.data(),0x38);
            if(!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>=1e9||std::abs(y)>=1e9)return nullptr;
            return &records.emplace(id,bytes).first->second;
        };
        std::map<uint64_t,NimbyTrackJunction> junctions;
        auto attachments=[&](uint64_t id,const unsigned char* p)->bool {
            const auto begin=field<uint64_t>(p,0x408),end=field<uint64_t>(p,0x410),cap=field<uint64_t>(p,0x418);
            if(end<begin||cap<end||(end-begin)%8||cap-begin>4096*8||(end!=begin&&!pointer(begin)))return false;
            std::vector<uint64_t> children((end-begin)/8),again(children.size());
            if(!children.empty()&&(!read(context,begin,children.data(),children.size()*8)||
               !read(context,begin,again.data(),again.size()*8)||children!=again))return false;
            for(auto child:children){
                const auto* record=track(child);if(!record)return false;
                const auto* b=record->data();const auto a=field<uint64_t>(b,8),z=field<uint64_t>(b,16);
                const auto fraction=field<double>(b,0x3f8);const auto direction=field<int32_t>(b,0x400);
                if(field<uint64_t>(b,0x3f0)!=id||(a==0)==(z==0)||!std::isfinite(fraction)||fraction<0||fraction>1||(direction!=1&&direction!=-1))return false;
                junctions[child]={child,id,fraction,direction,a==0?1:-1};
            }
            return true;
        };
        size_t selected=0;
        for(const auto& seed:signals)if(seed.textures_hash==texturesHash&&seed.kind==4){
            if(++selected>4096)return false;
            auto id=seed.track_id;auto fraction=seed.fraction;auto direction=-seed.direction;
            for(unsigned step=0;step<256;++step){
                const auto* record=track(id);if(!record)return false;
                const auto* p=record->data();if(!attachments(id,p))return false;
                std::optional<double> boundary;
                for(const auto& signal:signals)if(signal.track_id==id&&signal.kind==4&&signal.id!=seed.id&&-signal.direction==direction&&
                    (direction==1?signal.fraction>=fraction:signal.fraction<=fraction))
                    if(!boundary||(direction==1?signal.fraction<*boundary:signal.fraction>*boundary))boundary=signal.fraction;
                bool fork=false;
                for(const auto& [child,j]:junctions)if(j.main_track_id==id&&j.main_direction==direction&&
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
        }
        for(const auto& [id,record]:records){
            const auto* p=record.data();Track t{};t.id=id;t.links[0]=field<uint64_t>(p,8);t.links[1]=field<uint64_t>(p,16);
            t.x=field<double>(p,0x30);t.y=field<double>(p,0x38);
            t.geometry=std::isfinite(t.x)&&std::isfinite(t.y)&&std::abs(t.x)<1e9&&std::abs(t.y)<1e9;
            out.tracks.push_back(t);
        }
        for(const auto& signal:signals)if(records.contains(signal.track_id)||signal.textures_hash==texturesHash)out.signals.push_back(signal);
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
