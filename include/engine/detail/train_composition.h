#pragma once
#include "engine/detail/train_metadata.h"
#include <map>
#include <unordered_map>
namespace nimby::engine::train_metadata {
inline constexpr size_t maximumCars=262144,maximumCarsPerTrain=4096,maximumModels=16384;
// Dynamics owns a vector<CarSetup>, stride 32. Only its first uint64 is the
// resource/model key; it is opaque and has no entity generation-domain tag.
inline bool cars(ReadMemory read,void* context,uint64_t owner,uint64_t id,size_t dynamicsOffset,
                 const void* expected,uint32_t composition,size_t& budget,std::vector<NimbyTrainVehicle>& result){
    result.clear();uint64_t beforeId{},afterId{};std::array<unsigned char,0x50> before{},after{};
    if(!expected||!read(context,owner,&beforeId,8)||beforeId!=id||
       !read(context,owner+dynamicsOffset,before.data(),before.size())||std::memcmp(expected,before.data(),before.size()))return false;
    const auto begin=memory::field<uint64_t>(before.data(),0),end=memory::field<uint64_t>(before.data(),8),cap=memory::field<uint64_t>(before.data(),16);
    if(end<begin||cap<end||cap-begin>maximumCarsPerTrain*32||(end-begin)%32||
       (!begin&&cap)||(begin&&(!memory::pointer(begin)||cap>=0x7fffffff0000ULL)))return false;
    const auto count=(end-begin)/32;if(count>budget)return false;budget-=static_cast<size_t>(count);
    std::vector<unsigned char> bytes(end-begin),verify(bytes.size());
    if(count&&(!read(context,begin,bytes.data(),bytes.size())||!read(context,begin,verify.data(),verify.size())||bytes!=verify))return false;
    if(!read(context,owner+dynamicsOffset,after.data(),after.size())||before!=after||
       !read(context,owner,&afterId,8)||beforeId!=afterId)return false;
    std::vector<NimbyTrainVehicle> captured;captured.reserve(count);
    for(uint32_t i=0;i<count;++i)captured.push_back({id,memory::field<uint64_t>(bytes.data(),size_t(i)*32),i,composition});
    result=std::move(captured);return true;
}

// Rules+0x38/+0x40 is the native hash table used by the train UI. Resolve only
// buckets requested by captured compositions, each once, with one capture's
// cache. This does not walk the catalogue for every train or every car.
inline bool vehicleModels(ReadMemory read,void* context,const LiveState& state,
                          std::span<const NimbyTrainVehicle> cars,std::vector<NimbyVehicleModel>& result){
    result.clear();if(state.profile!=LiveStateProfile::Windows119)return false;
    if(cars.empty())return true;
    std::unordered_set<uint64_t> wanted;
    for(const auto& car:cars)if(wanted.insert(car.model_id).second&&wanted.size()>maximumModels)return false;
    std::array<uint64_t,2> header{},after{};const auto address=state.database+0xa80+0x38;
    if(!read(context,address,header.data(),sizeof header))return false;
    const auto table=header[0],count=header[1];
    if(!count||count>maximumBuckets||!memory::pointer(table)||table>0x7fffffff0000ULL-(count+1)*8)return false;
    uint64_t sentinel{},sentinelAfter{};if(!read(context,table+count*8,&sentinel,8))return false;
    std::map<uint64_t,uint64_t> buckets;
    for(const auto id:wanted)buckets.try_emplace(id%count,0);
    std::unordered_set<uint64_t> nodes,keys;std::vector<NimbyVehicleModel> models;models.reserve(wanted.size());
    for(auto& [bucket,head]:buckets){
        if(!read(context,table+bucket*8,&head,8))return false;
        for(auto node=head;node&&node!=sentinel;){
            if(!memory::pointer(node)||node>0x7fffffff0000ULL-0x2a8||nodes.size()>=maximumModels||!nodes.insert(node).second)return false;
            std::array<unsigned char,0x2a8> entry{},entryAfter{};
            if(!read(context,node,entry.data(),entry.size()))return false;
            const auto key=memory::field<uint64_t>(entry.data(),0);
            if(key%count!=bucket||!keys.insert(key).second)return false;
            if(wanted.contains(key)){
                NimbyVehicleModel model{};model.model_id=key;std::string code,name,source;
                if(!read_native_string(read,context,node+8,entry.data()+8,state.profile,code)||
                   !read_native_string(read,context,node+0x50,entry.data()+0x50,state.profile,name)||
                   !read_native_string(read,context,node+0x70,entry.data()+0x70,state.profile,source))return false;
                std::memcpy(model.code_utf8,code.data(),code.size());std::memcpy(model.name_en_utf8,name.data(),name.size());
                std::memcpy(model.source_name_utf8,source.data(),source.size());models.push_back(model);
            }
            if(!read(context,node,entryAfter.data(),entryAfter.size())||entryAfter!=entry)return false;
            node=memory::field<uint64_t>(entry.data(),0x2a0);
        }
    }
    for(const auto& [bucket,head]:buckets){uint64_t again{};if(!read(context,table+bucket*8,&again,8)||head!=again)return false;}
    if(!read(context,table+count*8,&sentinelAfter,8)||sentinel!=sentinelAfter||
       !read(context,address,after.data(),sizeof after)||after!=header)return false;
    std::sort(models.begin(),models.end(),[](const auto& a,const auto& b){return a.model_id<b.model_id;});
    result=std::move(models);return true;
}
}
