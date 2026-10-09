#pragma once
#include <platform/windows/train_length_native.h>
#include <engine/detail/memory_reader.h>
#include <engine/detail/train_metadata.h>
#include <algorithm>
#include <unordered_set>

namespace nimby::platform::windows::train_length::groups {
// This immutable adapter is for the qualified Windows 1.19 mutation boundaries.
// SimCmd receives {DB*,SimState*}; model commands receive the SimState directly.
// No game function is called and no foreign memory is written by these checks.
struct Context { uint64_t database{}, simulation{}; };
enum class Issue { None, Unavailable, InvalidTopology, NativeAggregationLimit, CurrentCompositionLag };
struct ModelTelemetry {
    uint64_t model{};size_t count{};
    std::optional<double> length;
    bool observed=false;
};
struct Check {
    policy::Assessment assessment{}, before{}, ownBefore{}, ownAfter{}, planned{};
    ReadIssue readIssue{};
    uint64_t root{}, changedTrain{};
    size_t members{}, previousElements{}, candidateElements{};
    size_t laggingMembers{},modelCount{},omittedElements{};
    std::array<ModelTelemetry,128> models{};
    Issue issue=Issue::None;
    bool checked=false;
    bool allowed()const noexcept{return assessment.allowed();}
};
namespace detail {
using nimby::engine::memory::field;
using nimby::engine::memory::pointer;
using Pool=nimby::engine::train_metadata::PoolEvidence;
constexpr size_t motionStride=0x638,trainStride=0x178,nativeAggregationElements=30;
struct Motion {
    uint64_t id{},parent{},address{};
    std::array<uint64_t,3> current{};
    int32_t mode{};
    bool attached{},running{},scheduled{};
};
inline int32_t mode(int32_t value)noexcept{return std::clamp(value,0,8);}
inline bool included(const Car& car,int32_t configuration)noexcept {
    configuration=mode(configuration);
    return !configuration||(car.settings[15]&(uint8_t{1}<<(configuration-1)));
}
inline bool measured(const policy::Assessment& value)noexcept {
    return value.decision==policy::Decision::Allowed||value.decision==policy::Decision::LimitExceeded;
}
inline Check initial(policy::Limit limit,uint64_t train)noexcept {
    Check result;result.changedTrain=train;result.assessment.maximumMeters=limit.maximumMeters;return result;
}
inline Check failed(Check result,Issue issue=Issue::Unavailable)noexcept {
    result.checked=true;result.issue=issue;result.assessment.decision=policy::Decision::ResolverFailure;return result;
}

class Capture {
    const View view_;
    const Context context_;
    Pool motions_,trains_;
    struct Length {uint64_t model{};std::optional<double> meters;};
    mutable std::array<Length,128> lengths_{};
    mutable size_t lengthCount_{};
    static bool safeRead(void* context,uint64_t at,void* target,size_t count) {
        return static_cast<const View*>(context)->bytes(at,target,count);
    }
    std::optional<uint64_t> slot(const Pool& pool,uint64_t id,size_t stride)const {
        if((id>>48)!=5||!pool.available)return {};
        const auto shift=field<uint32_t>(pool.header.data(),4),mask=field<uint32_t>(pool.header.data(),16);
        const auto index=(id>>16)&0xffffffffULL,block=index>>shift;
        if(block>=pool.blocks.size()||!pointer(pool.blocks[block])||
           pool.blocks[block]>0x7fffffff0000ULL-size_t(mask+1)*stride)return {};
        const auto address=pool.blocks[block]+(index&mask)*stride;
        uint64_t actual{};if(!view_.get(address,actual)||actual!=id)return {};
        return address;
    }
public:
    uint64_t rules{};
    std::vector<Motion> nodes;
    Capture(nimby::engine::ReadMemory read,void* data,Context context):view_(read,data),context_(context){}
    const View& view()const noexcept{return view_;}
    template<class Observer> policy::Assessment measure(policy::Limit limit,std::span<const policy::ModelBlock> models,Observer&& observe)const noexcept {
        // Baseline, full train, active group and stale current vectors share
        // one request-local model cache. Telemetry reuses those results and
        // never asks native memory for a vehicle length a second time.
        return policy::evaluateAppend(limit,std::span<const policy::ModelBlock>{},models,[&](uint64_t model){
            size_t index=0;while(index<lengthCount_&&lengths_[index].model!=model)++index;
            const auto meters=index<lengthCount_?lengths_[index].meters:view_.length(rules,model);
            if(index==lengthCount_&&index<lengths_.size()){lengths_[index]={model,meters};++lengthCount_;}
            observe(model,meters);return meters;
        });
    }
    policy::Assessment measure(policy::Limit limit,std::span<const policy::ModelBlock> models)const noexcept {
        return measure(limit,models,[](uint64_t,const std::optional<double>&){});
    }
    bool loadMotions() {
        if(!pointer(context_.simulation)||context_.simulation>0x7fffffff0000ULL-0xd0||
           !motions_.load(safeRead,const_cast<View*>(&view_),context_.simulation+0xa0))
            return view_.reject(ReadFault::VectorLayout,context_.simulation+0xa0);
        // One bulk read per block. No per-train ReadProcessMemory request and
        // no recurring scan: this snapshot exists only for a mutation request.
        const auto captured=nimby::engine::memory::collect(safeRead,const_cast<View*>(&view_),context_.simulation+0xa0,5,motionStride,
            [&](const unsigned char* bytes,uint64_t address){
                if(nodes.size()>=1048576)return false;
                nodes.push_back({field<uint64_t>(bytes,0),field<uint64_t>(bytes,0x1d8),address,
                    {field<uint64_t>(bytes,8),field<uint64_t>(bytes,0x10),field<uint64_t>(bytes,0x18)},
                    mode(field<int32_t>(bytes,0x80)),bytes[0x1f0]!=0,bytes[0x5d0]!=0,bytes[0x5f0]!=0});
                return true;
            })&&motions_.stable(safeRead,const_cast<View*>(&view_));
        return captured||view_.reject(ReadFault::VectorLayout,context_.simulation+0xa0);
    }
    bool loadModels() {
        const auto captured=pointer(context_.database)&&context_.database<=0x7fffffff0000ULL-0x410&&
            view_.get(context_.database+0x408,rules)&&pointer(rules)&&
            trains_.load(safeRead,const_cast<View*>(&view_),context_.database+0x200);
        return captured||view_.reject(ReadFault::VectorLayout,context_.database+0x200);
    }
    const Motion* motion(uint64_t id)const noexcept {
        const auto index=(id>>16)&0xffffffffULL;
        const auto found=std::lower_bound(nodes.begin(),nodes.end(),index,[](const Motion& entry,uint64_t value){
            return ((entry.id>>16)&0xffffffffULL)<value;
        });
        return found!=nodes.end()&&found->id==id?&*found:nullptr;
    }
    bool cars(uint64_t id,std::vector<Car>& result)const {
        const auto address=slot(trains_,id,trainStride);
        if(!address)return view_.reject(ReadFault::VectorLayout,context_.database+0x200,id);
        std::array<uint64_t,3> before{},after{};uint64_t again{};
        const auto captured=view_.get(*address+0xc0,before)&&view_.vector(*address+0xc0,result)&&
            view_.get(*address,again)&&again==id&&view_.get(*address+0xc0,after)&&before==after;
        return captured||view_.reject(ReadFault::VectorLayout,*address+0xc0,id);
    }
    bool currentCars(const Motion& node,std::vector<Car>& result)const {
        std::array<uint64_t,3> before{},after{};
        const auto captured=view_.get(node.address+8,before)&&before==node.current&&
            view_.vector(node.address+8,result)&&view_.get(node.address+8,after)&&after==node.current;
        return captured||view_.reject(ReadFault::VectorLayout,node.address+8,node.id);
    }
    bool stable(std::span<const Motion* const> selected)const {
        for(const auto* node:selected){
            std::array<unsigned char,0x1f8> bytes{};
            if(!view_.bytes(node->address,bytes.data(),bytes.size())||field<uint64_t>(bytes.data(),0)!=node->id||
               field<uint64_t>(bytes.data(),0x1d8)!=node->parent||
               field<uint64_t>(bytes.data(),8)!=node->current[0]||field<uint64_t>(bytes.data(),0x10)!=node->current[1]||
               field<uint64_t>(bytes.data(),0x18)!=node->current[2]||
               mode(field<int32_t>(bytes.data(),0x80))!=node->mode||(bytes[0x1f0]!=0)!=node->attached)
                return view_.reject(ReadFault::VectorLayout,node->address,node->id);
        }
        return (motions_.stable(safeRead,const_cast<View*>(&view_))&&trains_.stable(safeRead,const_cast<View*>(&view_)))||
            view_.reject(ReadFault::VectorLayout,context_.simulation+0xa0);
    }
    // A root may have current children missing from its last normalized native
    // vector. Parent/attached fields include previous commands of this phase.
    bool members(const Motion& root,std::vector<const Motion*>& result)const {
        if(root.attached)return false;
        result.push_back(&root);
        std::unordered_set<uint64_t> children;
        for(const auto& node:nodes)if(node.attached&&node.parent==root.id){
            if(node.id==root.id||result.size()>=policy::maximumElements)return false;
            result.push_back(&node);children.insert(node.id);
        }
        // Native normalization only supports stars. A child with children is
        // ambiguous and can silently drop attachments during normalization.
        for(const auto& node:nodes)if(node.attached&&children.contains(node.parent))return false;
        return true;
    }
    bool hasChildren(uint64_t id)const noexcept {
        return std::any_of(nodes.begin(),nodes.end(),[&](const Motion& entry){return entry.attached&&entry.parent==id;});
    }
};

struct Override {
    uint64_t train{};
    bool replaceCars=false,replaceMode=false;
    std::span<const Car> cars;
    int32_t configuration{};
};
struct Slice {
    const Motion* member;
    size_t begin,count,beforeBegin{},beforeCount{};
    bool existing=true;
};
inline bool appendModels(std::vector<policy::ModelBlock>& models,std::span<const Car> cars,int32_t configuration) {
    for(const auto& car:cars)if(included(car,configuration)){
        if(models.size()>=policy::maximumElements)return false;
        models.push_back({car.model,1});
    }
    return true;
}
inline bool models(Capture& capture,std::span<const Motion* const> members,const Override& change,
                   std::vector<policy::ModelBlock>& before,std::vector<policy::ModelBlock>& after,
                   std::vector<Car>* originalChanged=nullptr,std::vector<Slice>* slices=nullptr) {
    for(const auto* node:members){
        std::vector<Car> configured;if(!capture.cars(node->id,configured))return false;
        const auto beforeBegan=before.size();
        if(!appendModels(before,configured,node->mode))return false;
        const auto changed=node->id==change.train;
        const auto candidate=changed&&change.replaceCars?change.cars:std::span<const Car>(configured);
        const auto began=after.size();
        if(!appendModels(after,candidate,changed&&change.replaceMode?mode(change.configuration):node->mode))return false;
        if(slices)slices->push_back({node,began,after.size()-began,beforeBegan,before.size()-beforeBegan});
        if(changed&&originalChanged)*originalChanged=std::move(configured);
    }
    return true;
}
inline void inventory(Check& result,std::span<const policy::ModelBlock> models)noexcept {
    result.models={};result.modelCount=0;result.omittedElements=0;
    for(const auto& entry:models){
        size_t index=0;while(index<result.modelCount&&result.models[index].model!=entry.model)++index;
        if(index==result.modelCount){
            if(index==result.models.size()){result.omittedElements+=entry.count;continue;}
            result.models[index].model=entry.model;++result.modelCount;
        }
        result.models[index].count+=entry.count;
    }
}
inline policy::Assessment candidate(Capture& capture,Check& result,policy::Limit limit,
                                    std::span<const policy::ModelBlock> models)noexcept {
    inventory(result,models);
    return capture.measure(limit,models,[&](uint64_t model,const std::optional<double>& meters){
        for(size_t i=0;i<result.modelCount;++i)if(result.models[i].model==model){
            result.models[i].observed=true;result.models[i].length=meters;break;
        }
    });
}
inline policy::Assessment whole(const Capture& capture,policy::Limit limit,std::span<const Car> cars) {
    std::vector<policy::ModelBlock> models;models.reserve(cars.size());
    for(const auto& car:cars)models.push_back({car.model,1});
    return capture.measure(limit,models);
}
struct Bounds {
    std::vector<policy::ModelBlock> before,after;
    size_t previousElements{},candidateElements{},laggingMembers{};
};
inline bool appendBound(std::vector<policy::ModelBlock>& target,std::span<const policy::ModelBlock> models) {
    if(models.size()>policy::maximumElements-target.size())return false;
    target.insert(target.end(),models.begin(),models.end());return true;
}
inline bool sameModels(std::span<const policy::ModelBlock> lhs,std::span<const policy::ModelBlock> rhs)noexcept {
    if(lhs.size()!=rhs.size())return false;
    for(size_t i=0;i<lhs.size();++i)if(lhs[i].model!=rhs[i].model)return false;
    return true;
}
inline bool bounds(Capture& capture,Check& result,policy::Limit limit,const Motion* root,
                   std::span<const policy::ModelBlock> before,std::span<const policy::ModelBlock> after,
                   std::span<const Slice> slices,Bounds& bounded) {
    // Native aggregation rebuilds the root, but copies each attached child's
    // current vector. Config/recompose changes may reach the next aggregation
    // in either order, so bound each child by the larger physical/intended
    // length. Capture its current vector once for both sides of the comparison.
    for(const auto& slice:slices){
        const auto oldProfile=before.subspan(slice.beforeBegin,slice.beforeCount);
        const auto newProfile=after.subspan(slice.begin,slice.count);
        if(slice.member==root){
            if(!appendBound(bounded.before,oldProfile)||!appendBound(bounded.after,newProfile))return false;
            bounded.previousElements+=oldProfile.size();bounded.candidateElements+=newProfile.size();continue;
        }
        std::vector<Car> current;if(!capture.currentCars(*slice.member,current))return false;
        std::vector<policy::ModelBlock> live;live.reserve(current.size());
        for(const auto& car:current)live.push_back({car.model,1});
        Check trace;const auto physical=candidate(capture,trace,limit,live);
        if(!measured(physical)){
            result.assessment=physical;result.models=trace.models;
            result.modelCount=trace.modelCount;result.omittedElements=trace.omittedElements;
            result.readIssue=capture.view().issue();return false;
        }
        const auto next=capture.measure(limit,newProfile);
        if(!measured(next))return false; // Callers already qualified the whole planned candidate.
        const auto chosenNext=physical.totalLengthMeters>next.totalLengthMeters?
            std::span<const policy::ModelBlock>(live):newProfile;
        if(!appendBound(bounded.after,chosenNext))return false;
        bounded.candidateElements+=std::max(live.size(),newProfile.size());
        if(!sameModels(live,newProfile))++bounded.laggingMembers;
        if(slice.existing){
            const auto prior=capture.measure(limit,oldProfile);
            // An unknown prior model cannot authorize an oversized edit, but
            // a measurable new candidate can still repair the individual train.
            const auto chosenPrior=measured(prior)&&physical.totalLengthMeters>prior.totalLengthMeters?
                std::span<const policy::ModelBlock>(live):oldProfile;
            if(!appendBound(bounded.before,chosenPrior))return false;
            bounded.previousElements+=std::max(live.size(),oldProfile.size());
        }
    }
    return true;
}
inline void preserveReduction(policy::Assessment& candidate,const policy::Assessment& current)noexcept {
    // Changing the option never truncates an existing train. A validated edit
    // that does not increase its metres may reduce it in steps. Count is only
    // checked separately for the retained native coupling aggregation guard.
    if(candidate.decision==policy::Decision::LimitExceeded&&measured(current)&&
       candidate.totalLengthMeters<=current.totalLengthMeters)
        candidate.decision=policy::Decision::Allowed;
}
inline void aggregation(Check& result)noexcept {
    if(result.members>1&&result.candidateElements>nativeAggregationElements&&
       result.candidateElements>result.previousElements){
        result.issue=Issue::NativeAggregationLimit;result.assessment.decision=policy::Decision::TooManyElements;
    }
}
}

inline Check hitch(nimby::engine::ReadMemory read,void* data,Context context,policy::Limit limit,
                   uint64_t driver,uint64_t child)noexcept {
    auto result=detail::initial(limit,child);result.root=driver;
    if(!limit.active()||!driver)return result; // Native unhitch must remain available.
    try{
        detail::Capture capture(read,data,context);
        const auto failure=[&](Issue issue=Issue::Unavailable){result.readIssue=capture.view().issue();return detail::failed(result,issue);};
        if(!capture.loadMotions())return failure();
        const auto* source=capture.motion(child);const auto* root=capture.motion(driver);
        // Missing driver is native unhitch; absent/ineligible child is native no-op.
        if(!root||!source||source->running||source->scheduled)return result;
        if(root->id==source->id||root->attached)return failure(Issue::InvalidTopology);
        if(source->attached&&source->parent==root->id)return result; // Reorder/orientation only.
        if(capture.hasChildren(source->id))return failure(Issue::InvalidTopology);
        if(source->attached){const auto* prior=capture.motion(source->parent);
            if(!prior||prior->attached||prior->id==source->id)return failure(Issue::InvalidTopology);}
        std::vector<const detail::Motion*> members;
        if(!capture.members(*root,members))return failure(Issue::InvalidTopology);
        if(!capture.loadModels())return failure();
        std::vector<policy::ModelBlock> before,after;std::vector<detail::Slice> slices;
        if(!detail::models(capture,members,{},before,after,nullptr,&slices))return failure();
        std::vector<Car> childCars;
        const auto childBegin=after.size();
        if(!capture.cars(source->id,childCars)||!detail::appendModels(after,childCars,source->mode))return failure();
        slices.push_back({source,childBegin,after.size()-childBegin,0,0,false});
        members.push_back(source);
        result.checked=true;result.members=members.size();result.previousElements=before.size();result.candidateElements=after.size();
        result.before=capture.measure(limit,before);
        result.planned=detail::candidate(capture,result,limit,after);
        if(!detail::measured(result.planned)){result.assessment=result.planned;result.readIssue=capture.view().issue();return result;}
        detail::Bounds conservative;
        if(!detail::bounds(capture,result,limit,root,before,after,slices,conservative)){
            if(!result.assessment.allowed())return result;
            return failure();
        }
        if(!capture.stable(members))return failure();
        result.before=capture.measure(limit,conservative.before);
        result.previousElements=conservative.previousElements;result.candidateElements=conservative.candidateElements;
        result.laggingMembers=conservative.laggingMembers;
        result.assessment=detail::candidate(capture,result,limit,conservative.after);
        if(!result.assessment.allowed()&&result.planned.allowed()&&result.laggingMembers)result.issue=Issue::CurrentCompositionLag;
        result.readIssue=capture.view().issue();
        // Every new attachment fits the physical policy and the retained native
        // aggregation guard. Never rely on native silently omitting a member.
        if(conservative.candidateElements>detail::nativeAggregationElements){
            result.issue=result.laggingMembers&&after.size()<=detail::nativeAggregationElements?
                Issue::CurrentCompositionLag:Issue::NativeAggregationLimit;
            if(result.assessment.allowed())result.assessment.decision=policy::Decision::TooManyElements;
        }
        return result;
    }catch(...){return detail::failed(result);}
}

inline Check setConfig(nimby::engine::ReadMemory read,void* data,Context context,policy::Limit limit,
                       uint64_t train,int32_t configuration)noexcept {
    auto result=detail::initial(limit,train);if(!limit.active())return result;
    try{
        detail::Capture capture(read,data,context);
        const auto failure=[&](Issue issue=Issue::Unavailable){result.readIssue=capture.view().issue();return detail::failed(result,issue);};
        if(!capture.loadMotions())return failure();
        const auto* source=capture.motion(train);if(!source||source->mode==detail::mode(configuration))return result;
        const auto* root=source->attached?capture.motion(source->parent):source;
        if(!root)return failure(Issue::InvalidTopology);
        result.root=root->id;
        std::vector<const detail::Motion*> members;
        if(!capture.members(*root,members))return failure(Issue::InvalidTopology);
        if(!capture.loadModels())return failure();
        std::vector<policy::ModelBlock> before,after;std::vector<detail::Slice> slices;
        if(!detail::models(capture,members,{train,false,true,{},configuration},before,after,nullptr,&slices))return failure();
        result.checked=true;result.members=members.size();result.previousElements=before.size();result.candidateElements=after.size();
        const auto plannedBefore=capture.measure(limit,before);
        result.planned=detail::candidate(capture,result,limit,after);
        if(!detail::measured(result.planned)){result.assessment=result.planned;result.readIssue=capture.view().issue();return result;}
        auto intended=result.planned;detail::preserveReduction(intended,plannedBefore);
        const auto intendedAllowed=intended.allowed()&&!(members.size()>1&&after.size()>detail::nativeAggregationElements&&after.size()>before.size());
        if(members.size()>1){
            detail::Bounds conservative;
            if(!detail::bounds(capture,result,limit,root,before,after,slices,conservative)){
                if(!result.assessment.allowed())return result;
                return failure();
            }
            result.before=capture.measure(limit,conservative.before);
            result.assessment=detail::candidate(capture,result,limit,conservative.after);
            result.previousElements=conservative.previousElements;result.candidateElements=conservative.candidateElements;
            result.laggingMembers=conservative.laggingMembers;
        }else{result.before=plannedBefore;result.assessment=result.planned;}
        if(!capture.stable(members))return failure();
        result.readIssue=capture.view().issue();
        detail::preserveReduction(result.assessment,result.before);detail::aggregation(result);
        if(!result.allowed()&&intendedAllowed&&result.laggingMembers)result.issue=Issue::CurrentCompositionLag;
        return result;
    }catch(...){return detail::failed(result);}
}

inline Check recompose(nimby::engine::ReadMemory read,void* data,Context context,policy::Limit limit,
                       uint64_t train,std::span<const Car> candidate)noexcept {
    auto result=detail::initial(limit,train);if(!limit.active())return result;
    try{
        detail::Capture capture(read,data,context);
        const auto failure=[&](Issue issue=Issue::Unavailable){result.readIssue=capture.view().issue();return detail::failed(result,issue);};
        if(candidate.size()>policy::maximumElements){capture.view().reject(ReadFault::ElementBudget,train,candidate.size());return failure();}
        if(!capture.loadMotions()||!capture.loadModels())return failure();
        const auto* source=capture.motion(train);
        std::vector<Car> configured;
        if(!source){
            // A purchased/retired train can have no Motion yet. The complete
            // configured train still obeys the individual composition rule.
            if(!capture.cars(train,configured))return failure();
            result.checked=true;result.root=train;result.members=1;
            result.ownBefore=detail::whole(capture,limit,configured);
            std::vector<policy::ModelBlock> ownModels;ownModels.reserve(candidate.size());
            for(const auto& car:candidate)ownModels.push_back({car.model,1});
            result.ownAfter=detail::candidate(capture,result,limit,ownModels);
            result.planned=result.ownAfter;
            result.before=result.ownBefore;result.assessment=result.ownAfter;
            result.readIssue=capture.view().issue();
            result.previousElements=configured.size();result.candidateElements=candidate.size();
            if(!capture.stable({}))return failure();
            detail::preserveReduction(result.assessment,result.before);return result;
        }
        const auto* root=source->attached?capture.motion(source->parent):source;
        if(!root)return failure(Issue::InvalidTopology);
        result.root=root->id;
        std::vector<const detail::Motion*> members;
        if(!capture.members(*root,members))return failure(Issue::InvalidTopology);
        std::vector<policy::ModelBlock> before,after;std::vector<detail::Slice> slices;
        if(!detail::models(capture,members,{train,true,false,candidate,0},before,after,&configured,&slices))return failure();
        result.checked=true;result.members=members.size();result.previousElements=before.size();result.candidateElements=after.size();
        result.ownBefore=detail::whole(capture,limit,configured);result.ownAfter=detail::whole(capture,limit,candidate);
        auto own=result.ownAfter;detail::preserveReduction(own,result.ownBefore);
        const auto plannedBefore=capture.measure(limit,before);
        result.planned=detail::candidate(capture,result,limit,after);
        if(!detail::measured(result.planned)){result.assessment=result.planned;result.readIssue=capture.view().issue();return result;}
        auto intended=result.planned;detail::preserveReduction(intended,plannedBefore);
        const auto intendedAllowed=own.allowed()&&intended.allowed()&&!(members.size()>1&&after.size()>detail::nativeAggregationElements&&after.size()>before.size());
        if(members.size()>1){
            detail::Bounds conservative;
            if(!detail::bounds(capture,result,limit,root,before,after,slices,conservative)){
                if(!result.assessment.allowed())return result;
                return failure();
            }
            result.before=capture.measure(limit,conservative.before);
            result.assessment=detail::candidate(capture,result,limit,conservative.after);
            result.previousElements=conservative.previousElements;result.candidateElements=conservative.candidateElements;
            result.laggingMembers=conservative.laggingMembers;
        }else{result.before=plannedBefore;result.assessment=result.planned;}
        if(!capture.stable(members))return failure();
        result.readIssue=capture.view().issue();
        detail::preserveReduction(result.assessment,result.before);
        if(!own.allowed())result.assessment=own;
        detail::aggregation(result);
        if(!result.allowed()&&intendedAllowed&&result.laggingMembers)result.issue=Issue::CurrentCompositionLag;
        return result;
    }catch(...){return detail::failed(result);}
}
}
