#pragma once
#include <engine/native_rear_coverage.h>
#include <engine/route_interlocking.h>

namespace nimby::engine::interlocking {

// An entered lease retains its ORIGINAL ticket and actual physical portions,
// even when the current route changes or its mod disappears. The provider must
// requalify those retained portions in qualifiedAt; copying an old timestamp or
// substituting the new itinerary does not establish that qualification.
struct AggregateClearanceRoute {
    Ticket ticket;
    uint64_t consistRevision=0;
    Proof entered=Proof::Unknown;
    bool qualified=false;
    native_rear::Publication qualifiedAt;
    // Fractions use the same qualified canonical frames as the body history.
    // Aliases protect conflicts but are not claims of actual train traversal.
    std::span<const Interval> ownPhysical,aliases;
};
enum class AggregateClearanceReason : uint8_t {
    Clear,Occupied,InvalidLimits,BudgetExceeded,InvalidPublication,
    UnretainedPublication,InvalidTicket,InvalidConsistRevision,
    TrainIdentityChanged,ConsistChanged,UnqualifiedRoute,DifferentPublication,
    InvalidInterval,UnqualifiedCoverage,NotEntered
};
struct AggregateClearanceLimits {
    size_t ownPortions=512,aliases=512,overlapChecks=524288;
    native_rear::Limits coverage;
};
struct AggregateClearanceResult {
    Proof proof=Proof::Unknown;
    AggregateClearanceReason reason=AggregateClearanceReason::UnqualifiedRoute;
    uint64_t track=0;
    size_t checks=0;
    bool coverageAssessed=false;
    // Preserves exact provider/geometry/completeness failures, rather than
    // presenting every missing rear envelope as an occupied or cleared route.
    native_rear::Result coverage;
};
namespace aggregate_clearance_detail {
inline bool identity(uint64_t value,uint64_t kind) noexcept {
    return value>>48==kind&&(value&0x0000ffffffffffffULL)!=0;
}
inline bool interval(const Interval& value) noexcept {
    return identity(value.track,1)&&std::isfinite(value.begin)&&std::isfinite(value.end)&&
        value.begin>=0&&value.begin<=value.end&&value.end<=1;
}
}

// Pure bridge from an explicitly qualified aggregate body to rear clearance.
// It creates neither a native provider nor a tick certificate, infers no cars
// from track rows and never mutates a lease. The caller supplies immutable input
// and disjoint, owned body storage; assess() verifies the WHOLE envelope itself.
// A Complete envelope can still occupy the retained route (including contact).
// Only a finished RetainedMovement publication, a confirmed entry and complete
// disjoint body can yield Yes. Unknown must be passed through as Unknown.
//
// No heap is used. At most 512 physical intervals are copied/sorted on stack;
// bounded binary searches consume overlapChecks. The body storage contains a
// valid envelope only when result.coverageAssessed && coverage.complete(),
// which is independent of this result's Yes/No/Unknown clearance decision.
inline AggregateClearanceResult assessAggregateRearClearance(
    const AggregateClearanceRoute& route,const native_rear::Observation& observation,
    const native_rear::Publication& current,std::span<native_rear::Portion> body,
    AggregateClearanceLimits limits={}) noexcept {
    AggregateClearanceResult result;
    const auto unknown=[&](AggregateClearanceReason reason,uint64_t track=0) {
        result.reason=reason;result.track=track;return result;
    };
    if(!limits.ownPortions||limits.ownPortions>512||!limits.aliases||limits.aliases>512||
       !limits.overlapChecks||limits.overlapChecks>4194304)
        return unknown(AggregateClearanceReason::InvalidLimits);
    if(route.ownPhysical.size()>limits.ownPortions||route.aliases.size()>limits.aliases)
        return unknown(AggregateClearanceReason::BudgetExceeded);
    if(!current.session||!current.epoch||
       (current.phase!=native_rear::Phase::BeforeMovement&&current.phase!=native_rear::Phase::RetainedMovement))
        return unknown(AggregateClearanceReason::InvalidPublication);
    if(current.phase!=native_rear::Phase::RetainedMovement)
        return unknown(AggregateClearanceReason::UnretainedPublication);
    if(route.ticket.session!=current.session||!route.ticket.routeRevision||
       !aggregate_clearance_detail::identity(route.ticket.train,5)||
       !aggregate_clearance_detail::identity(route.ticket.source,8)||
       (route.entered!=Proof::Unknown&&route.entered!=Proof::No&&route.entered!=Proof::Yes))
        return unknown(AggregateClearanceReason::InvalidTicket);
    if(!route.consistRevision)return unknown(AggregateClearanceReason::InvalidConsistRevision);
    if(route.ticket.train!=observation.consist.train)
        return unknown(AggregateClearanceReason::TrainIdentityChanged);
    if(route.consistRevision!=observation.consist.revision)
        return unknown(AggregateClearanceReason::ConsistChanged);
    if(!route.qualified||route.ownPhysical.empty())return unknown(AggregateClearanceReason::UnqualifiedRoute);
    if(route.qualifiedAt!=current)return unknown(AggregateClearanceReason::DifferentPublication);
    // Validate ALL resources before an early occupied result can hide malformed
    // later evidence. Aliases are validated but never included in body overlap.
    for(const auto& row:route.ownPhysical)if(!aggregate_clearance_detail::interval(row))
        return unknown(AggregateClearanceReason::InvalidInterval,row.track);
    for(const auto& row:route.aliases)if(!aggregate_clearance_detail::interval(row))
        return unknown(AggregateClearanceReason::InvalidInterval,row.track);
    result.coverageAssessed=true;
    result.coverage=native_rear::assess(observation,current,body,limits.coverage);
    if(!result.coverage.complete())return unknown(AggregateClearanceReason::UnqualifiedCoverage,result.coverage.track);

    std::array<Interval,512> own;
    std::copy(route.ownPhysical.begin(),route.ownPhysical.end(),own.begin());
    auto end=own.begin()+route.ownPhysical.size();
    std::sort(own.begin(),end,[](const auto& a,const auto& b) {
        return std::tie(a.track,a.begin,a.end)<std::tie(b.track,b.begin,b.end);
    });
    size_t count=0;
    for(auto it=own.begin();it!=end;++it) {
        if(count&&own[count-1].track==it->track&&it->begin<=own[count-1].end)
            own[count-1].end=std::max(own[count-1].end,it->end);
        else own[count++]=*it;
    }
    for(const auto& row:body.first(result.coverage.portions)) {
        const double begin=std::min(row.from,row.to),endFraction=std::max(row.from,row.to);
        size_t first=0,last=count;
        while(first<last) {
            if(result.checks==limits.overlapChecks)return unknown(AggregateClearanceReason::BudgetExceeded);
            ++result.checks;
            const auto middle=first+(last-first)/2;const auto& part=own[middle];
            if(part.track<row.track||(part.track==row.track&&part.end<begin))first=middle+1;
            else last=middle;
        }
        if(first<count) {
            if(result.checks==limits.overlapChecks)return unknown(AggregateClearanceReason::BudgetExceeded);
            ++result.checks;
            const auto& part=own[first];
            if(part.track==row.track&&part.begin<=endFraction) {
                result.proof=Proof::No;result.reason=AggregateClearanceReason::Occupied;result.track=row.track;
                return result;
            }
        }
    }
    if(route.entered!=Proof::Yes)return unknown(AggregateClearanceReason::NotEntered);
    result.proof=Proof::Yes;result.reason=AggregateClearanceReason::Clear;return result;
}
} // namespace nimby::engine::interlocking
