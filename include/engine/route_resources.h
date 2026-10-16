#pragma once
#include <engine/route_interlocking.h>
#include <engine/track_conflict_decode.h>
#include <unordered_map>

namespace nimby::engine::interlocking {
// The adapter must qualify the exact travelled portions and fresh native F8
// tables in the same world. This builder cannot prove an itinerary, native
// permission, current incarnation or clearance from arbitrary caller values.
struct RoutePortion {uint64_t track{};double from{},to{};};
struct ConflictList {
    uint64_t track{};
    TrackConflictReason reason=TrackConflictReason::TrackUnavailable;
    std::span<const NimbyTrackConflict> rows;
};
enum class ResourceOutcome {Complete,InvalidPortion,MissingTable,UnknownTable,InvalidTable,InvalidConflict,BudgetExceeded,Capacity};
struct ResourceLimits {
    size_t portions=512,tables=4096,relations=131072,candidates=8192,resources=512;
};
struct ResourceResult {
    ResourceOutcome outcome=ResourceOutcome::MissingTable;
    uint64_t track{};
    size_t checkedRelations{};
    std::vector<Interval> resources;
    bool complete()const noexcept{return outcome==ResourceOutcome::Complete;}
};
namespace resource_detail {
inline bool validFraction(double value)noexcept{return std::isfinite(value)&&value>=0&&value<=1;}
inline bool order(const Interval& a,const Interval& b)noexcept{
    return std::tie(a.track,a.begin,a.end)<std::tie(b.track,b.begin,b.end);
}
inline bool merge(std::vector<Interval>& rows,TrackConflictReadBudget& budget){
    if(!budget.available())return false;
    if(!budget.preflight)std::sort(rows.begin(),rows.end(),order);
    else {
        // Budgeted heapsort has no scratch allocation or exception-based
        // cancellation. Check at most every 32 sift/extraction iterations;
        // even a maximum-size candidate set can stop inside its sort.
        size_t steps=0;
        const auto proceed=[&](){return steps++%32!=0||budget.available();};
        const auto sift=[&](size_t root,size_t count){
            while(root<count/2){
                if(!proceed())return false;
                size_t child=2*root+1;
                if(child+1<count&&order(rows[child],rows[child+1]))++child;
                if(!order(rows[root],rows[child]))break;
                std::swap(rows[root],rows[child]);root=child;
            }
            return true;
        };
        for(size_t root=rows.size()/2;root>0;){if(!sift(--root,rows.size()))return false;}
        for(size_t count=rows.size();count>1;){
            if(!proceed())return false;
            std::swap(rows[0],rows[--count]);if(!sift(0,count))return false;
        }
    }
    if(!budget.available())return false;
    size_t size=0,checked=0;
    for(const auto& row:rows){
        if(checked++%32==0&&!budget.available())return false;
        if(size&&rows[size-1].track==row.track&&row.begin<=rows[size-1].end)
            rows[size-1].end=std::max(rows[size-1].end,row.end);
        else rows[size++]=row;
    }
    rows.resize(size);return budget.available();
}
}
// Pure bounded qualification/expansion. A portion claims its own normalized
// interval, plus the FULL other interval of each directly overlapping F8 row.
// Endpoints are inclusive: contact is not proof of physical clearance.
// No recursive/transitive expansion and no geometric inference are performed.
//
// Uniform alias claims can conservatively exclude two otherwise independent
// routes that each alias the same third track. Partial F8 overlap also claims
// its full other interval; fractional projection would be an unqualified guess.
// Crossings absent from F8 are NOT inferred from endpoint/geometry coincidence.
// A future exact conflict-pair model can narrow these exclusions after proof.
inline ResourceResult buildRouteResources(std::span<const RoutePortion> portions,
                                         std::span<const ConflictList> lists,
                                         ResourceLimits limits={},TrackConflictReadBudget* suppliedBudget=nullptr) {
    const auto failed=[](ResourceOutcome outcome,uint64_t track=0,size_t checked=0){
        return ResourceResult{outcome,track,checked,{}};
    };
    TrackConflictReadBudget unlimited;auto& budget=suppliedBudget?*suppliedBudget:unlimited;
    if(!budget.available())return failed(ResourceOutcome::BudgetExceeded);
    if(!limits.portions||limits.portions>4096||!limits.tables||limits.tables>4096||
       !limits.relations||limits.relations>NIMBY_TRACK_CONFLICTS_MAX_ROWS||
       !limits.candidates||limits.candidates>NIMBY_TRACK_CONFLICTS_MAX_ROWS||
       !limits.resources||limits.resources>4096)
        return failed(ResourceOutcome::InvalidTable);
    if(portions.empty())return failed(ResourceOutcome::InvalidPortion);
    if(portions.size()>limits.portions||lists.size()>limits.tables)return failed(ResourceOutcome::BudgetExceeded);
    try{
        if(!budget.available())return failed(ResourceOutcome::BudgetExceeded);
        std::vector<Interval> own;own.reserve(portions.size());
        size_t checkedPortions=0;
        for(const auto& portion:portions){
            if(checkedPortions++%32==0&&!budget.available())return failed(ResourceOutcome::BudgetExceeded);
            if(portion.track>>48!=1||!resource_detail::validFraction(portion.from)||!resource_detail::validFraction(portion.to))
                return failed(ResourceOutcome::InvalidPortion,portion.track);
            own.push_back({portion.track,std::min(portion.from,portion.to),std::max(portion.from,portion.to)});
        }
        if(!resource_detail::merge(own,budget))return failed(ResourceOutcome::BudgetExceeded);
        if(own.size()>limits.candidates)return failed(ResourceOutcome::BudgetExceeded);
        // Each full track identity is looked up once. Recycled IDs never alias
        // through an ordinal/index mask; contradictory duplicate tables fail
        // only when their track is required by this route.
        if(!budget.available())return failed(ResourceOutcome::BudgetExceeded);
        std::unordered_map<uint64_t,const ConflictList*> index;index.reserve(lists.size());
        for(const auto& list:lists){
            if(!budget.available())return failed(ResourceOutcome::BudgetExceeded);
            auto [at,inserted]=index.emplace(list.track,&list);if(!inserted)at->second=nullptr;
        }
        if(!budget.available())return failed(ResourceOutcome::BudgetExceeded);
        ResourceResult result{ResourceOutcome::Complete,0,0,own};
        for(size_t first=0;first<own.size();){
            if(!budget.available())return failed(ResourceOutcome::BudgetExceeded,0,result.checkedRelations);
            size_t last=first+1;
            while(last<own.size()&&own[last].track==own[first].track){
                if((last-first)%32==0&&!budget.available())return failed(ResourceOutcome::BudgetExceeded,own[first].track,result.checkedRelations);
                ++last;
            }
            const auto track=own[first].track;const auto found=index.find(track);
            if(found==index.end())return failed(ResourceOutcome::MissingTable,track,result.checkedRelations);
            if(!found->second)return failed(ResourceOutcome::InvalidTable,track,result.checkedRelations);
            const auto& list=*found->second;
            if(list.reason!=TrackConflictReason::Complete)return failed(ResourceOutcome::UnknownTable,track,result.checkedRelations);
            if(list.rows.size()>4096||list.rows.size()>limits.relations-result.checkedRelations)
                return failed(ResourceOutcome::BudgetExceeded,track,result.checkedRelations);
            for(const auto& relation:list.rows){
                if(result.checkedRelations%32==0&&!budget.available())return failed(ResourceOutcome::BudgetExceeded,track,result.checkedRelations);
                ++result.checkedRelations;
                if(relation.track_id!=track||relation.other_track_id>>48!=1||
                   !resource_detail::validFraction(relation.begin_fraction)||!resource_detail::validFraction(relation.end_fraction)||
                   !resource_detail::validFraction(relation.other_begin_fraction)||!resource_detail::validFraction(relation.other_end_fraction)||
                   relation.begin_fraction>relation.end_fraction||relation.other_begin_fraction>relation.other_end_fraction)
                    return failed(ResourceOutcome::InvalidConflict,track,result.checkedRelations);
                // Own ranges are sorted/disjoint for this full ID. Find the
                // first ending at/after the conflict, avoiding portion*row scans.
                const auto intersection=std::lower_bound(own.begin()+first,own.begin()+last,relation.begin_fraction,
                    [](const Interval& range,double begin){return range.end<begin;});
                if(intersection==own.begin()+last||intersection->begin>relation.end_fraction)continue;
                if(result.resources.size()==limits.candidates)return failed(ResourceOutcome::BudgetExceeded,track,result.checkedRelations);
                // A periodic CPU check suffices between non-allocating rows;
                // every growth allocation additionally gets its own preflight.
                if(result.resources.size()==result.resources.capacity()&&!budget.available())
                    return failed(ResourceOutcome::BudgetExceeded,track,result.checkedRelations);
                result.resources.push_back({relation.other_track_id,relation.other_begin_fraction,relation.other_end_fraction});
            }
            first=last;
        }
        if(!resource_detail::merge(result.resources,budget))return failed(ResourceOutcome::BudgetExceeded,0,result.checkedRelations);
        if(result.resources.size()>limits.resources)return failed(ResourceOutcome::Capacity,0,result.checkedRelations);
        return result;
    }catch(const std::bad_alloc&){return failed(ResourceOutcome::Capacity);}
}
}
