#pragma once
#include <engine/route_interlocking_shadow.h>
#include <nimby/detail/signal_routes.h>
#include <map>
#include <set>

namespace nimby::engine::interlocking {
// Copied observation facts only. A complete result qualifies the resource
// expansion for a SHADOW comparison, never a native permission or rear proof.
struct ShadowRoutePlan {
    ShadowRequest request;
    std::vector<Interval> resources;
};
struct ShadowBatchPlan {
    std::vector<ShadowRoutePlan> routes;
    size_t invalid{},deferred{};
};
inline ShadowBatchPlan planShadowBatch(uint64_t session,
    std::span<const NimbySignalRouteObservation> routes,
    std::span<const NimbySignalRouteSection> sections,
    std::span<const ConflictList> conflicts,TrackConflictReadBudget* suppliedBudget=nullptr) {
    ShadowBatchPlan result;
    TrackConflictReadBudget unlimited;auto& budget=suppliedBudget?*suppliedBudget:unlimited;
    // A budget refusal discards the WHOLE plan, including earlier completed
    // routes; it never turns an unread/unfinished route into a free resource.
    const auto stopped=[&](){return ShadowBatchPlan{{},0,routes.size()};};
    if(!budget.available())return stopped();
    if(!session||routes.size()>64||sections.size()>4096){result.deferred=routes.size();return result;}
    using Key=std::pair<uint64_t,uint64_t>;
    std::map<Key,std::vector<RoutePortion>> portions;
    std::set<Key> invalidSections,duplicates;
    for(const auto& section:sections){
        if(!budget.available())return stopped();
        const Key key{section.train_id,section.signal_id};
        if(section.track_id>>48!=1||!resource_detail::validFraction(section.begin_fraction)||
           !resource_detail::validFraction(section.end_fraction)||section.begin_fraction>section.end_fraction){
            invalidSections.insert(key);continue;
        }
        auto& rows=portions[key];
        if(rows.size()<512){
            if(rows.size()==rows.capacity()&&!budget.available())return stopped();
            rows.push_back({section.track_id,section.begin_fraction,section.end_fraction});
        }else {
            if(!budget.available())return stopped();
            invalidSections.insert(key);
        }
    }
    for(const auto& route:routes){
        if(!budget.available())return stopped();
        if(!duplicates.insert({route.train_id,route.signal_id}).second){
            if(!budget.available())return stopped();
            invalidSections.insert({route.train_id,route.signal_id});
        }
    }
    std::set<Key> submitted;
    for(const auto& route:routes){
        if(!budget.available())return stopped();
        const Key key{route.train_id,route.signal_id};
        if(!submitted.insert(key).second)continue;
        if(route.train_id>>48!=5||route.signal_id>>48!=8||!route.route_revision){++result.invalid;continue;}
        const bool clear=route.flags&NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR;
        const bool blocked=route.flags&NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;
        const auto crossing=route.flags&(NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED);
        ShadowRoutePlan plan;
        plan.request.ticket={session,route.train_id,route.signal_id,route.route_revision};
        plan.request.capturedAtMs=route.observed_at_ms;
        plan.request.nativeStage=clear!=blocked?(clear?NativeStage::Clear:NativeStage::Blocked):NativeStage::Unknown;
        const uint32_t known=NIMBY_SIGNAL_ROUTE_ORDERED|NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR|
            NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED|NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE|
            NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED;
        plan.request.routeQualified=(route.flags&NIMBY_SIGNAL_ROUTE_ORDERED)&&
            (route.flags&NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE)&&!(route.flags&~known)&&!route.reserved&&
            route.blocks>=1&&route.blocks<=16&&
            !(clear&&blocked)&&!(crossing&&clear)&&
            crossing!=(NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED)&&
            route.next_signal_id>>48==8&&route.next_signal_id!=route.signal_id&&
            route.head_track_id>>48==1&&route.source_track_id>>48==1&&route.next_track_id>>48==1&&
            resource_detail::validFraction(route.head_fraction)&&resource_detail::validFraction(route.source_fraction)&&
            resource_detail::validFraction(route.next_fraction)&&std::isfinite(route.distance_m)&&route.distance_m>=0&&
            (route.head_direction==1||route.head_direction==-1)&&
            (route.source_direction==1||route.source_direction==-1)&&
            (route.next_direction==1||route.next_direction==-1)&&
            route.source_kind==NIMBY_SIGNAL_PATH&&route.next_kind==NIMBY_SIGNAL_PATH&&!invalidSections.contains(key);
        // A source being ahead, or disappearing from this copied batch, is not
        // the historical physical-entry proof required to free/replace a lease.
        plan.request.entered=Proof::Unknown;
        const auto found=portions.find(key);
        if(found==portions.end())plan.request.resources=ResourceOutcome::InvalidPortion;
        else if(!plan.request.routeQualified)plan.request.resources=ResourceOutcome::InvalidPortion;
        else {
            auto expanded=buildRouteResources(found->second,conflicts,{},&budget);
            if(!budget.available())return stopped();
            plan.request.resources=expanded.outcome;plan.resources=std::move(expanded.resources);
        }
        if(!budget.available())return stopped();
        result.routes.push_back(std::move(plan));
    }
    if(!budget.available())return stopped();
    return result;
}
} // namespace nimby::engine::interlocking
