#pragma once
#include "engine/network.h"
#include "engine/detail/memory_reader.h"
#include "engine/native_string.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <unordered_set>
#include <utility>

namespace nimby::engine::train_metadata {
inline constexpr size_t maximumTags=16384,maximumObjectTags=4096,maximumReferences=262144,maximumBuckets=65536;
inline NimbyTrainCharacteristics characteristics(const void* before,const void* after,size_t size) {
    NimbyTrainCharacteristics result{};
    if(!before||!after||size<0x50||std::memcmp(before,after,0x50))return result;
    const auto* bytes=static_cast<const unsigned char*>(after);
    const auto scalar=[&](size_t offset,double maximum,uint32_t flag,double& target){
        const auto value=memory::field<float>(bytes,offset);
        if(std::isfinite(value)&&value>=0&&value<=maximum){target=value;result.flags|=flag;}
    };
    scalar(0x1c,10000,NIMBY_CHARACTERISTICS_MAX_SPEED_VALID,result.maximum_speed_mps);
    scalar(0x20,1000,NIMBY_CHARACTERISTICS_ACCELERATION_VALID,result.maximum_acceleration_mps2);
    scalar(0x2c,1e12,NIMBY_CHARACTERISTICS_TRACTIVE_FORCE_VALID,result.tractive_force_n);
    scalar(0x30,1e15,NIMBY_CHARACTERISTICS_POWER_VALID,result.power_w);
    scalar(0x34,1e12,NIMBY_CHARACTERISTICS_EMPTY_MASS_VALID,result.empty_mass_kg);
    scalar(0x38,1e7,NIMBY_CHARACTERISTICS_LENGTH_VALID,result.length_m);
    const auto capacity=memory::field<int32_t>(bytes,0x40);
    if(capacity>=0&&capacity<=100000000){result.passenger_capacity=capacity;result.flags|=NIMBY_CHARACTERISTICS_CAPACITY_VALID;}
    const auto begin=memory::field<uint64_t>(bytes,0),end=memory::field<uint64_t>(bytes,8),cap=memory::field<uint64_t>(bytes,0x10);
    if(end>=begin&&cap>=end&&cap-begin<=65536*32&&(end-begin)%32==0&&
       ((!begin&&!cap)||(memory::pointer(begin)&&cap<0x7fffffff0000ULL))){
        result.car_count=static_cast<uint32_t>((end-begin)/32);result.flags|=NIMBY_CHARACTERISTICS_CAR_COUNT_VALID;
    }
    return result;
}
struct PoolEvidence {
    std::array<unsigned char,48> header{};
    std::vector<uint64_t> blocks;
    uint64_t address{};
    bool available=false;
    bool load(ReadMemory read,void* context,uint64_t pool) {
        address=pool;available=false;
        if(!read(context,pool,header.data(),header.size()))return false;
        const auto shift=memory::field<uint32_t>(header.data(),4),size=memory::field<uint32_t>(header.data(),8),mask=memory::field<uint32_t>(header.data(),16);
        const auto begin=memory::field<uint64_t>(header.data(),24),end=memory::field<uint64_t>(header.data(),32),cap=memory::field<uint64_t>(header.data(),40);
        if(shift<1||shift>16||size!=(1u<<shift)||mask!=size-1||end<begin||cap<end||cap-begin>8192||(end-begin)%8||
           (end!=begin&&!memory::pointer(begin))||((end-begin)/8)*size>1048576)return false;
        blocks.resize((end-begin)/8);
        available=blocks.empty()||read(context,begin,blocks.data(),blocks.size()*8);
        return available;
    }
    bool contains(uint64_t id,uint64_t record,size_t stride) const {
        if(!available)return false;
        const auto shift=memory::field<uint32_t>(header.data(),4),mask=memory::field<uint32_t>(header.data(),16);
        const auto index=(id>>16)&0xffffffffULL,block=index>>shift;
        if(block>=blocks.size()||!memory::pointer(blocks[block]))return false;
        return blocks[block]<=0x7fffffff0000ULL-size_t(mask+1)*stride&&blocks[block]+(index&mask)*stride==record;
    }
    bool stable(ReadMemory read,void* context) const {
        if(!available)return false;
        std::array<unsigned char,48> after{};std::vector<uint64_t> again(blocks.size());
        return read(context,address,after.data(),after.size())&&header==after&&
            (blocks.empty()||(read(context,memory::field<uint64_t>(header.data(),24),again.data(),again.size()*8)&&blocks==again))&&
            read(context,address,after.data(),after.size())&&header==after;
    }
};
// Windows 1.19 native ETA helper 0x14043e0c0 uses float SUB/DIV, clamps to
// zero, truncates to whole seconds, then compares against the active deadline.
// Keep a qualifying zero distinct from an unavailable estimate.
inline std::optional<int64_t> predictedDelay(const void* before,const void* after,size_t size,int64_t gameTimeUs) {
    if(!before||!after||size<0x638||gameTimeUs<0)return {};
    const auto* a=static_cast<const unsigned char*>(before);const auto* b=static_cast<const unsigned char*>(after);
    using memory::field;
    if(field<uint64_t>(a,0)!=field<uint64_t>(b,0)||field<uint64_t>(a,0)>>48!=5||a[0x4b0]!=1||b[0x4b0]!=1||a[0x4d0]||b[0x4d0])return {};
    for(const auto& [offset,bytes]:std::array<std::pair<size_t,size_t>,6>{{{0x2c0,8},{0x2d8,8},{0x3a8,8},{0x46c,16},{0x488,8},{0x4b0,1}}})
        if(std::memcmp(a+offset,b+offset,bytes))return {};
    const auto current=field<uint64_t>(b,0x3a8),start=field<uint64_t>(b,0x2c0),end=field<uint64_t>(b,0x2d8);
    if(current&&(current>>48!=1||current==start||current==end))return {};
    const auto samples=field<int32_t>(b,0x470);const auto distance=field<float>(b,0x46c),progress=field<float>(b,0x474),speed=field<float>(b,0x478);
    const auto deadline=field<int64_t>(b,0x488);
    if(samples<10||!std::isfinite(distance)||!std::isfinite(progress)||!std::isfinite(speed)||distance<10||speed<1||deadline<=0)return {};
    const float secondsFloat=std::max((distance-progress)/speed,0.f);
    if(!std::isfinite(secondsFloat)||static_cast<long double>(secondsFloat)>static_cast<long double>(INT64_MAX/1000000))return {};
    const auto remaining=static_cast<int64_t>(secondsFloat)*1000000;
    // Both are nonnegative: subtraction is representable. Check the final sum.
    const auto difference=remaining-deadline;
    if(difference>INT64_MAX-gameTimeUs)return {};
    return difference+gameTimeUs;
}

inline bool objectTags(ReadMemory read,void* context,uint64_t address,uint64_t expectedId,size_t offset,
                       size_t& remaining,std::vector<uint64_t>& result) {
    result.clear();std::array<uint64_t,3> header{},after{};uint64_t id{},again{};
    if(!read(context,address,&id,sizeof id)||id!=expectedId||!read(context,address+offset,header.data(),sizeof header))return false;
    const auto begin=header[0],end=header[1],cap=header[2];
    if(end<begin||cap<end||(!begin&&cap)||cap-begin>maximumObjectTags*8||(end-begin)%8||
       (begin&&(!memory::pointer(begin)||cap>=0x7fffffff0000ULL)))return false;
    const auto count=(end-begin)/8;if(count>remaining)return false;
    remaining-=static_cast<size_t>(count);
    std::vector<uint64_t> values(static_cast<size_t>(count)),verify(values.size());
    if(count&&(!read(context,begin,values.data(),count*8)||!read(context,begin,verify.data(),count*8)||values!=verify))return false;
    std::unordered_set<uint64_t> unique;unique.reserve(values.size());
    for(const auto value:values)if(!unique.insert(value).second)return false;
    if(!read(context,address,&again,sizeof again)||again!=id||!read(context,address+offset,after.data(),sizeof after)||header!=after)return false;
    result=std::move(values);return true;
}

inline bool tagCatalog(ReadMemory read,void* context,const LiveState& state,std::vector<NimbyTag>& result) {
    result.clear();uint64_t catalog{},catalogAfter{};std::array<unsigned char,32> header{},after{};
    if(state.profile!=LiveStateProfile::Windows119||!read(context,state.database+0x420,&catalog,8)||!memory::pointer(catalog)||
       !read(context,catalog,header.data(),header.size()))return false;
    const auto buckets=memory::field<uint64_t>(header.data(),0x10),count=memory::field<uint64_t>(header.data(),0x18);
    if(!count||count>maximumBuckets||!memory::pointer(buckets)||buckets>0x7fffffff0000ULL-(count+1)*8)return false;
    std::vector<uint64_t> heads(count+1),verify(heads.size());
    if(!read(context,buckets,heads.data(),heads.size()*8))return false;
    std::unordered_set<uint64_t> nodes,ids;std::vector<NimbyTag> tags;
    for(size_t i=0;i<count;++i)for(auto node=heads[i];node&&node!=heads.back();) {
        if(!memory::pointer(node)||node>0x7fffffff0000ULL-0x88||nodes.size()>=maximumTags||!nodes.insert(node).second)return false;
        std::array<unsigned char,0x88> entry{},entryAfter{};std::string name;
        if(!read(context,node,entry.data(),entry.size())||
           !read_native_string(read,context,node+0x18,entry.data()+0x18,state.profile,name)||
           !read(context,node,entryAfter.data(),entryAfter.size())||entry!=entryAfter)return false;
        NimbyTag tag{};tag.tag_id=memory::field<uint64_t>(entry.data(),0);
        if(!ids.insert(tag.tag_id).second)return false;
        std::memcpy(tag.name_utf8,name.data(),name.size());tags.push_back(tag);
        node=memory::field<uint64_t>(entry.data(),0x80);
    }
    if(!read(context,buckets,verify.data(),verify.size()*8)||heads!=verify||
       !read(context,catalog,after.data(),after.size())||header!=after||
       !read(context,state.database+0x420,&catalogAfter,8)||catalog!=catalogAfter)return false;
    std::sort(tags.begin(),tags.end(),[](const auto& a,const auto& b){return a.tag_id<b.tag_id;});
    result=std::move(tags);return true;
}
}
