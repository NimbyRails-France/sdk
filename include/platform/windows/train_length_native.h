#pragma once
#include <engine/train_length_policy.h>
#include <engine/live_state.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <optional>
#include <vector>

namespace nimby::platform::windows::train_length {
namespace policy=nimby::engine::train_length;
// Qualified Windows 1.19 layout. These views are borrowed only during a native
// composition operation; neither pointers nor native allocation cross the mod ABI.
struct Car { uint64_t model{}, train{}; std::array<uint8_t,16> settings{}; };
static_assert(sizeof(Car)==32);
enum class ReadFault { None, Address, Read, VectorLayout, ElementBudget, ModelCatalogue, ModelMissing, ModelChain, ModelLength, Selection };
struct ReadIssue {ReadFault fault=ReadFault::None;uint64_t address{},detail{};};
class View {
    nimby::engine::ReadMemory read_; void* context_;
    mutable ReadIssue issue_{};
public:
    View(nimby::engine::ReadMemory read,void* context=nullptr):read_(read),context_(context){}
    template<class T> bool get(uint64_t at,T& value)const noexcept {
        if(at<0x10000||at>0x7fffffff0000ULL-sizeof(T)||!read_)return reject(ReadFault::Address,at,sizeof(T));
        return read_(context_,at,&value,sizeof value)||reject(ReadFault::Read,at,sizeof(T));
    }
    ReadIssue issue()const noexcept{return issue_;}
    bool reject(ReadFault fault,uint64_t address,uint64_t detail=0)const noexcept {
        if(issue_.fault==ReadFault::None)issue_={fault,address,detail};
        return false;
    }
    bool bytes(uint64_t at,void* target,size_t count)const noexcept {
        if(!count)return true;
        if(at<0x10000||at>0x7fffffff0000ULL||count>0x7fffffff0000ULL-at||!read_)return reject(ReadFault::Address,at,count);
        return read_(context_,at,target,count)||reject(ReadFault::Read,at,count);
    }
    template<class T> bool vector(uint64_t at,std::vector<T>& values,size_t maximum=policy::maximumElements)const {
        std::array<uint64_t,3> pointers{};
        if(!get(at,pointers))return false;
        if(pointers[1]<pointers[0]||pointers[2]<pointers[1]||
           (pointers[1]-pointers[0])%sizeof(T)||(pointers[2]-pointers[0])%sizeof(T))return reject(ReadFault::VectorLayout,at);
        const auto count=(pointers[1]-pointers[0])/sizeof(T);
        if(count>maximum)return reject(ReadFault::ElementBudget,at,count);
        values.resize(static_cast<size_t>(count));
        return bytes(pointers[0],values.data(),values.size()*sizeof(T));
    }
    std::optional<uint64_t> modelNode(uint64_t rules,uint64_t model)const noexcept {
        uint64_t table{},sentinel{},node{};uint32_t buckets{};
        if(!get(rules+0x38,table)||!get(rules+0x40,buckets))return std::nullopt;
        if(!buckets||buckets>1048576){reject(ReadFault::ModelCatalogue,rules+0x40,buckets);return std::nullopt;}
        if(!get(table+uint64_t(buckets)*8,sentinel)||!get(table+(model%buckets)*8,node))return std::nullopt;
        for(size_t i=0;node&&i<policy::maximumElements;++i){
            uint64_t id{},next{};
            if(node==sentinel){reject(ReadFault::ModelMissing,table,model);return std::nullopt;}
            if(!get(node,id))return std::nullopt;
            if(id==model)return node;
            if(!get(node+0x2a0,next))return std::nullopt;
            if(next==node){reject(ReadFault::ModelChain,node,model);return std::nullopt;}
            node=next;
        }
        reject(node?ReadFault::ModelChain:ReadFault::ModelMissing,table,model);return std::nullopt;
    }
    std::optional<double> length(uint64_t rules,uint64_t model)const noexcept {
        auto node=modelNode(rules,model);float meters{};
        if(!node||!get(*node+0xfc,meters))return std::nullopt;
        if(!std::isfinite(meters)||meters<=0)reject(ReadFault::ModelLength,*node+0xfc,model);
        return static_cast<double>(meters);
    }
};

enum class Edit { None, Append, Replace, Paste, Reset };
struct Proposal {
    Edit edit=Edit::None;
    bool readable=true,requested=false,removalRequested=false;
    size_t beforeElements{},requestedElements{},selectedElements{};
    uint16_t actionFlags{};
    std::vector<policy::ModelBlock> beforeModels;
    std::vector<policy::ModelBlock> models;
};
// Mirror the native commit priority. A stale append flag must never prevent
// deleting, moving, selecting, or editing cosmetic settings of an existing train.
inline Proposal proposed(const View& view,uint64_t editor) {
    Proposal result;std::array<uint8_t,16> flags{};std::array<uint8_t,7> cosmetic{};
    uint8_t dirty{};uint64_t selected{};
    if(!view.get(editor+0x15d0,dirty)){result.readable=false;return result;}
    if(!dirty)return result;
    result.requested=true;
    if(!view.get(editor+0x15c0,flags)||!view.get(editor+0x1598,cosmetic)||!view.get(editor+0x1138,selected)){
        result.readable=false;return result;
    }
    result.selectedElements=static_cast<size_t>(selected);
    for(size_t i=0;i<flags.size();++i)if(flags[i])result.actionFlags|=uint16_t(1u<<i);
    // Qualify the selected removal from the flags already copied above. Higher
    // priority layout/cosmetic operations can leave a stale deletion flag;
    // they must not retire the notice for a rejected addition. This fact never
    // measures cars or changes which operations are delegated to the game.
    result.removalRequested=selected&&flags[0]&&!flags[12]&&!flags[13]&&
        std::none_of(cosmetic.begin(),cosmetic.end(),[](uint8_t value){return value!=0;});
    if(flags[12]||flags[13]||flags[0]||(selected&&(flags[1]||flags[2])))return result;
    for(auto value:cosmetic)if(value)return result;
    if(flags[4])result.edit=Edit::Append;
    else if(flags[5])result.edit=Edit::Replace;
    else if(!flags[6]&&!flags[7]&&!flags[8]){
        uint64_t clipboardBegin{},clipboardEnd{};
        if(flags[9]&&(!view.get(editor+0x1608,clipboardBegin)||!view.get(editor+0x1610,clipboardEnd))){
            result.readable=false;return result;
        }
        if(flags[9]&&clipboardBegin!=clipboardEnd)result.edit=Edit::Paste;
        else if(flags[11])result.edit=Edit::Reset;
        else return result;
    } else return result;
    int32_t pendingCount{};
    if(result.edit==Edit::Append){
        if(!view.get(editor+0x1600,pendingCount)||pendingCount<0){
            view.reject(ReadFault::ElementBudget,editor+0x1600,static_cast<uint64_t>(pendingCount));result.readable=false;return result;
        }
        if(!pendingCount){result.edit=Edit::None;return result;}
    } else if(result.edit==Edit::Replace&&!selected){result.edit=Edit::None;return result;}
    std::vector<Car> cars;
    if(!view.vector(editor+0xc0,cars)){result.readable=false;return result;}
    result.models.reserve(cars.size()+1);
    result.beforeElements=cars.size();
    for(const auto& car:cars)result.models.push_back({car.model,1});
    if(result.edit==Edit::Replace||result.edit==Edit::Reset)result.beforeModels=result.models;
    if(result.edit==Edit::Reset){
        if(!view.vector(editor+0x238,cars)){result.readable=false;return result;}
        result.models.clear();
        for(const auto& car:cars)result.models.push_back({car.model,1});
        result.requestedElements=cars.size();
    }
    if(result.edit==Edit::Append){
        Car pending{};
        if(!view.get(editor+0x15e0,pending)){result.readable=false;return result;}
        result.models.push_back({pending.model,static_cast<size_t>(pendingCount)});
        result.requestedElements=static_cast<size_t>(pendingCount);
    } else if(result.edit==Edit::Paste){
        std::vector<Car> clipboard;
        if(!view.vector(editor+0x1608,clipboard)){result.readable=false;return result;}
        for(const auto& car:clipboard)result.models.push_back({car.model,1});
        result.requestedElements=clipboard.size();
    } else if(result.edit==Edit::Replace){
        result.requestedElements=static_cast<size_t>(selected);
        Car pending{};uint64_t root{};
        if(selected>policy::maximumElements||!view.get(editor+0x15e0,pending)||!view.get(editor+0x1128,root)){
            view.reject(ReadFault::Selection,editor+0x1138,selected);result.readable=false;return result;
        }
        std::vector<uint64_t> nodes;nodes.reserve(static_cast<size_t>(selected));if(root)nodes.push_back(root);
        size_t visited=0;std::vector<bool> replaced(cars.size());
        while(!nodes.empty()){
            const auto node=nodes.back();nodes.pop_back();std::array<uint64_t,3> links{};int32_t index{};
            if(++visited>selected||!view.get(node,links)||!view.get(node+0x20,index)){
                view.reject(ReadFault::Selection,node,visited);result.readable=false;return result;
            }
            if(index<0||static_cast<size_t>(index)>=cars.size()||replaced[static_cast<size_t>(index)]){
                view.reject(ReadFault::Selection,node+0x20,static_cast<uint64_t>(index));result.readable=false;return result;
            }
            replaced[static_cast<size_t>(index)]=true;
            result.models[static_cast<size_t>(index)].model=pending.model;
            for(size_t i=0;i<2;++i)if(links[i])nodes.push_back(links[i]);
            if(nodes.size()>selected){view.reject(ReadFault::Selection,node,nodes.size());result.readable=false;return result;}
        }
        if(visited!=selected){view.reject(ReadFault::Selection,editor+0x1138,visited);result.readable=false;}
    }
    return result;
}
template<class Observer>
inline policy::Assessment assess(const View& view,uint64_t rules,policy::Limit limit,
                                 std::span<const policy::ModelBlock> models,Observer&& observe)noexcept {
    // Adjacent cars commonly share one model. One request-local cache entry
    // avoids repeating native table reads without allocation, global state or
    // retaining pointers beyond this composition operation. Missing/invalid
    // lengths keep exactly the same fail-closed result as an uncached read.
    uint64_t lastModel{};bool cached=false;std::optional<double> lastLength;
    return policy::evaluateAppend(limit,std::span<const policy::ModelBlock>{},models,
        [&](uint64_t model){
            if(!cached||model!=lastModel){lastLength=view.length(rules,model);lastModel=model;cached=true;}
            observe(model,lastLength);
            return lastLength;
        });
}
inline policy::Assessment assess(const View& view,uint64_t rules,policy::Limit limit,
                                 std::span<const policy::ModelBlock> models)noexcept {
    return assess(view,rules,limit,models,[](uint64_t,const std::optional<double>&){});
}
uint32_t install(uint64_t module)noexcept;
// Process-resident native mechanics, independent of a mod's length/UI policy.
// This is an internal SDK entry point, not a mod ABI or a rear-clear certificate.
uint32_t ensureRuntimeMechanics(uint64_t module)noexcept;
bool runtimeMechanicsReady()noexcept;

namespace aggregation {
inline constexpr size_t maximumMembers=64;
enum class Fault { None, Read, Layout, Identity, Topology, Changed, Elements, Members };
struct Prefix {uint64_t motion{},maximum{};};
struct Admission {
    bool allowed=false;
    bool extended=false;
    Fault fault=Fault::None;
    uint64_t train{},address{};
    size_t rootElements{},totalElements{},members{};
    std::array<Prefix,maximumMembers-1> prefixes{};
};
// Count exactly the inputs 44CF80 consumes: root DBTrain+C0 filtered by
// Motion+80, and each listed attached child's already-current Motion+8 vector.
// Root is ALWAYS appended, including after negative-order children. Its cars
// must be included in the whole bound before the native mutation starts.
// This double-read preflight is a local observation, not universal exclusion
// of native writers, an exhaustive parent roster, or an ownership certificate.
inline Admission admit(const View& view,uint64_t pool,uint64_t database,uint64_t root)noexcept {
    Admission result;result.address=root;
    const auto fail=[&](Fault fault,uint64_t at){result.fault=fault;result.address=at;return result;};
    struct Pool {
        uint64_t address{};
        std::array<uint8_t,48> bytes{};
        uint32_t shift{},mask{};
        uint64_t begin{},end{};
    };
    const auto poolRead=[&](uint64_t at,Pool& out){
        out.address=at;
        if(!view.get(at,out.bytes))return false;
        uint32_t count{};
        std::memcpy(&out.shift,out.bytes.data()+4,4);std::memcpy(&count,out.bytes.data()+8,4);
        std::memcpy(&out.mask,out.bytes.data()+16,4);std::memcpy(&out.begin,out.bytes.data()+24,8);
        std::memcpy(&out.end,out.bytes.data()+32,8);uint64_t cap{};std::memcpy(&cap,out.bytes.data()+40,8);
        return out.shift>=1&&out.shift<=16&&count==(1u<<out.shift)&&out.mask==count-1&&
            out.end>=out.begin&&cap>=out.end&&cap-out.begin<=8192&&
            (out.end-out.begin)%8==0&&((out.end-out.begin)/8)*count<=1048576;
    };
    struct Slot {uint64_t address{},blockAddress{},block{},id{};};
    const auto slot=[&](const Pool& from,uint64_t id,size_t stride,Slot& out){
        if((id>>48)!=5)return false;
        const uint64_t ordinal=(id>>16)&0xffffffffULL,index=ordinal>>from.shift;
        if(index>=(from.end-from.begin)/8)return false;
        out.id=id;out.blockAddress=from.begin+index*8;
        if(!view.get(out.blockAddress,out.block)||out.block<0x10000||
            out.block>0x7fffffff0000ULL-uint64_t(from.mask+1)*stride)return false;
        out.address=out.block+(ordinal&from.mask)*stride;uint64_t observed{};
        return view.get(out.address,observed)&&observed==id;
    };
    const auto vectorCount=[&](uint64_t at,std::array<uint64_t,3>& pointers,size_t& count){
        if(!view.get(at,pointers)||pointers[1]<pointers[0]||pointers[2]<pointers[1]||
            (pointers[1]-pointers[0])%32||(pointers[2]-pointers[0])%32||
            (pointers[2]&&pointers[2]>0x7fffffff0000ULL)||
            ((!pointers[0])!=(!pointers[2]))||
            (pointers[0]&&pointers[0]<0x10000))return false;
        count=static_cast<size_t>((pointers[1]-pointers[0])/32);return true;
    };
    if(database>0x7fffffff0000ULL-0x230)return fail(Fault::Layout,database);
    Pool motions,trains;
    if(!poolRead(pool,motions)||!poolRead(database+0x200,trains))return fail(Fault::Layout,pool);
    if(!view.get(root,result.train))return fail(Fault::Read,root);
    Slot rootSlot,trainSlot;
    if(!slot(motions,result.train,0x638,rootSlot)||rootSlot.address!=root||
        !slot(trains,result.train,0x178,trainSlot))return fail(Fault::Identity,root);
    uint64_t parent{};uint8_t attached{};int32_t mode{};
    if(!view.get(root+0x1d8,parent)||!view.get(root+0x1f0,attached)||!view.get(root+0x80,mode))return fail(Fault::Read,root);
    if(attached>1)return fail(Fault::Topology,root);
    //44EF70 schedules changed configurations for ALL Motion slots, including
    // attached members. Their own planned composition still needs the native
    // recalculation. Only an autonomous owner may raise child-append limits;
    // attached/transitional/nested scopes retain the game's CMP30 predicate.
    result.extended=!parent&&!attached;
    // Match the native clamp; no policy or metre limit is applied here.
    const auto selected=std::clamp(mode,0,8);
    std::array<uint64_t,3> planned{},listed{},current{};size_t rootCount{},childCount{},oldCurrent{};
    if(!vectorCount(trainSlot.address+0xc0,planned,rootCount)||!vectorCount(root+0x88,listed,childCount)||
        !vectorCount(root+8,current,oldCurrent))return fail(Fault::Layout,root);
    if(oldCurrent>policy::maximumElements)return fail(Fault::Elements,root+8);
    if(rootCount>policy::maximumElements)return fail(Fault::Elements,trainSlot.address+0xc0);
    if(childCount>=maximumMembers)return fail(Fault::Members,root+0x88);
    std::array<uint8_t,policy::maximumElements*sizeof(Car)> rootCars; // only used bytes are read
    std::array<uint8_t,(maximumMembers-1)*32> children;
    if(!view.bytes(planned[0],rootCars.data(),rootCount*32)||!view.bytes(listed[0],children.data(),childCount*32))return fail(Fault::Read,root);
    for(size_t i=0;i<rootCount;++i){
        if(!selected||(rootCars[i*32+31]&(uint8_t{1}<<(selected-1))))++result.rootElements;
    }
    result.totalElements=result.rootElements;result.members=1;
    struct Member {Slot slot;std::array<uint64_t,3> current{};uint64_t parent{};int64_t order{};uint8_t attached{},reverse{};};
    std::array<Member,maximumMembers-1> members{};
    for(size_t i=0;i<childCount;++i){
        uint64_t id{};std::memcpy(&id,children.data()+i*32,8);
        if(id==result.train)return fail(Fault::Topology,root+0x88);
        for(size_t j=0;j<i;++j)if(members[j].slot.id==id)return fail(Fault::Topology,root+0x88);
        auto& child=members[i];
        if(!slot(motions,id,0x638,child.slot))return fail(Fault::Identity,root+0x88);
        const auto at=child.slot.address;size_t count{};
        if(!view.get(at+0x1d8,child.parent)||!view.get(at+0x1f0,child.attached)||
            !view.get(at+0x1e0,child.order)||!view.get(at+0x1e8,child.reverse)||
            !vectorCount(at+8,child.current,count))return fail(Fault::Read,at);
        if((result.extended&&(child.parent!=result.train||child.attached!=1))||child.attached>1||child.reverse>1||
            child.order>INT64_MAX/1000000||child.order<INT64_MIN/1000000)return fail(Fault::Topology,at);
        if(count>policy::maximumElements-result.totalElements)return fail(Fault::Elements,at+8);
        result.totalElements+=count;++result.members;
        // Equal zero keys are conservatively treated as before-root: native
        // sorting need not be stable. A refused full-prefix never supplies a
        // complete aggregate to the independent physical/rear provider.
        result.prefixes[i]={at,child.order>0?policy::maximumElements:policy::maximumElements-result.rootElements};
        std::array<uint8_t,2048> readable;
        for(size_t offset=0;offset<count*32;offset+=readable.size()){
            if(!view.bytes(child.current[0]+offset,readable.data(),std::min(readable.size(),count*32-offset)))return fail(Fault::Read,child.current[0]+offset);
        }
    }
    // Read back every count-relevant value and identity before admission. No
    // native mutation, vector allocation, or partial admission occurs here.
    std::array<uint8_t,2048> again;
    for(size_t offset=0;offset<rootCount*32;offset+=again.size()){
        const size_t bytes=std::min(again.size(),rootCount*32-offset);
        if(!view.bytes(planned[0]+offset,again.data(),bytes)||std::memcmp(again.data(),rootCars.data()+offset,bytes))return fail(Fault::Changed,planned[0]+offset);
    }
    if(!view.bytes(listed[0],again.data(),childCount*32)||std::memcmp(again.data(),children.data(),childCount*32))return fail(Fault::Changed,root+0x88);
    for(size_t i=0;i<childCount;++i){
        const auto& expected=members[i];Member observed;observed.slot=expected.slot;
        const auto at=expected.slot.address;
        if(!view.get(at+8,observed.current)||!view.get(at+0x1d8,observed.parent)||
            !view.get(at+0x1e0,observed.order)||!view.get(at+0x1e8,observed.reverse)||!view.get(at+0x1f0,observed.attached))return fail(Fault::Read,at);
        uint64_t id{},block{};
        if(!view.get(at,id)||!view.get(expected.slot.blockAddress,block))return fail(Fault::Read,at);
        if(id!=expected.slot.id||block!=expected.slot.block||observed.current!=expected.current||
            observed.parent!=expected.parent||observed.order!=expected.order||observed.reverse!=expected.reverse||observed.attached!=expected.attached)return fail(Fault::Changed,at);
    }
    std::array<uint64_t,3> finalPlanned{},finalListed{},finalCurrent{};uint64_t finalId{},finalParent{},trainId{},block{},trainBlock{};
    int32_t finalMode{};uint8_t finalAttached{};std::array<uint8_t,48> finalPool{},finalTrains{};
    if(!view.get(root,finalId)||!view.get(root+0x1d8,finalParent)||!view.get(root+0x1f0,finalAttached)||!view.get(root+0x80,finalMode)||
        !view.get(root+0x88,finalListed)||!view.get(root+8,finalCurrent)||!view.get(trainSlot.address,trainId)||!view.get(trainSlot.address+0xc0,finalPlanned)||
        !view.get(rootSlot.blockAddress,block)||!view.get(trainSlot.blockAddress,trainBlock)||!view.get(pool,finalPool)||!view.get(trains.address,finalTrains))return fail(Fault::Read,root);
    if(finalId!=result.train||trainId!=result.train||finalParent!=parent||finalAttached!=attached||finalMode!=mode||
        finalListed!=listed||finalPlanned!=planned||finalCurrent!=current||block!=rootSlot.block||trainBlock!=trainSlot.block||finalPool!=motions.bytes||finalTrains!=trains.bytes)return fail(Fault::Changed,root);
    result.allowed=true;return result;
}
}
}
