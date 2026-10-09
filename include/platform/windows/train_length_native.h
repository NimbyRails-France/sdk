#pragma once
#include <engine/train_length_policy.h>
#include <engine/live_state.h>
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
    bool readable=true,requested=false;
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
}
