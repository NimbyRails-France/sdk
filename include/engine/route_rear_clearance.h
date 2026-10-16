#pragma once
#include <engine/route_interlocking.h>

namespace nimby::engine::interlocking {

// A simulation publication identity, not an OS timestamp. Epoch changes when
// that publication lineage is replaced; tick zero is a valid first tick.
struct RearClearanceTick {
    uint64_t session=0,epoch=0,tick=0;
    bool operator==(const RearClearanceTick&) const = default;
};

// Vehicle keys are opaque, nonzero identities within one qualified consist
// revision. They are NOT model IDs or presumed native CarOccupancy IDs: the
// current public occupation table does not identify individual vehicles.
struct VehicleFootprint {
    uint64_t train=0,vehicle=0,track=0;
    double begin=0,end=0;
};
struct RearClearanceRoute {
    Ticket ticket;
    uint64_t consistRevision=0;
    Proof entered=Proof::Unknown;
    bool qualified=false;
    RearClearanceTick qualifiedAt;
    // Retained, complete actual route portions, requalified in qualifiedAt.
    // Conflict aliases are kept separately; no vehicle must have visited them.
    std::span<const Interval> ownPhysical,aliases;
    std::span<const uint64_t> expectedVehicles;
};
struct RearClearanceObservation {
    uint64_t session=0,train=0,consistRevision=0;
    RearClearanceTick consistAt,footprintsAt;
    bool footprintsQualified=false;
    // Certifies the entire actual consist AND every physical section of every
    // vehicle. A matching count / one footprint per vehicle cannot certify it.
    bool completeConsist=false;
    // Must come from a provider that proves a finished immutable/native tick.
    // Equal external reads, freshness, pause or head-length do NOT create it.
    std::optional<RearClearanceTick> stableTickCertificate;
    std::span<const uint64_t> vehicles;
    std::span<const VehicleFootprint> footprints;
};
enum class RearClearanceReason {
    Clear,Occupied,InvalidIdentity,UnqualifiedRoute,UnqualifiedFootprints,
    DifferentTick,InvalidInterval,InvalidVehicles,MissingVehicle,
    DuplicateFootprint,IncompleteConsist,MissingStableTick,NotEntered,
    InvalidLimits,BudgetExceeded,Capacity
};
struct RearClearanceResult {
    Proof proof=Proof::Unknown;
    RearClearanceReason reason=RearClearanceReason::UnqualifiedRoute;
    uint64_t vehicle=0,track=0;
    size_t checks=0;
};
struct RearClearanceLimits {
    size_t ownPortions=512,aliases=512,vehicles=4096,footprints=32768,overlapChecks=524288;
};
namespace rear_clearance_detail {
inline bool tick(const RearClearanceTick& value) noexcept {return value.session&&value.epoch;}
inline bool interval(const Interval& value) noexcept {
    return value.track>>48==1&&std::isfinite(value.begin)&&std::isfinite(value.end)&&
        value.begin>=0&&value.begin<=value.end&&value.end<=1;
}
inline auto footprintKey(const VehicleFootprint& value) noexcept {
    return std::tie(value.train,value.vehicle,value.track,value.begin,value.end);
}
}

// Pure, bounded proof qualification. This function never samples the game,
// derives a certificate, infers an itinerary or releases a lease itself.
inline RearClearanceResult assessRearClearance(const RearClearanceRoute& route,
    const RearClearanceObservation& observation,const RearClearanceTick& current,
    RearClearanceLimits limits={}) {
    const auto unknown=[](RearClearanceReason reason,uint64_t vehicle=0,uint64_t track=0,size_t checks=0){
        return RearClearanceResult{Proof::Unknown,reason,vehicle,track,checks};
    };
    if(!limits.ownPortions||limits.ownPortions>4096||!limits.aliases||limits.aliases>4096||
       !limits.vehicles||limits.vehicles>4096||!limits.footprints||limits.footprints>131072||
       !limits.overlapChecks||limits.overlapChecks>4194304)return unknown(RearClearanceReason::InvalidLimits);
    if(route.ownPhysical.size()>limits.ownPortions||route.aliases.size()>limits.aliases||
       route.expectedVehicles.size()>limits.vehicles||observation.vehicles.size()>limits.vehicles||
       observation.footprints.size()>limits.footprints)return unknown(RearClearanceReason::BudgetExceeded);
    if(!rear_clearance_detail::tick(current)||route.ticket.session!=current.session||
       !route.ticket.session||route.ticket.train>>48!=5||route.ticket.source>>48!=8||!route.ticket.routeRevision||
       !route.consistRevision||observation.session!=current.session||observation.train!=route.ticket.train||
       observation.consistRevision!=route.consistRevision||
       (route.entered!=Proof::Unknown&&route.entered!=Proof::No&&route.entered!=Proof::Yes))
        return unknown(RearClearanceReason::InvalidIdentity);
    if(!route.qualified||route.ownPhysical.empty())return unknown(RearClearanceReason::UnqualifiedRoute);
    if(!observation.footprintsQualified)return unknown(RearClearanceReason::UnqualifiedFootprints);
    if(route.qualifiedAt!=current||observation.consistAt!=current||observation.footprintsAt!=current)
        return unknown(RearClearanceReason::DifferentTick);
    if(observation.stableTickCertificate&&*observation.stableTickCertificate!=current)
        return unknown(RearClearanceReason::DifferentTick);
    for(const auto& row:route.ownPhysical)if(!rear_clearance_detail::interval(row))
        return unknown(RearClearanceReason::InvalidInterval,0,row.track);
    for(const auto& row:route.aliases)if(!rear_clearance_detail::interval(row))
        return unknown(RearClearanceReason::InvalidInterval,0,row.track);
    if(route.expectedVehicles.empty())return unknown(RearClearanceReason::InvalidVehicles);
    try {
        std::vector<uint64_t> expected(route.expectedVehicles.begin(),route.expectedVehicles.end());
        std::vector<uint64_t> actual(observation.vehicles.begin(),observation.vehicles.end());
        std::sort(expected.begin(),expected.end());std::sort(actual.begin(),actual.end());
        const auto invalid=[](const auto& rows){
            return (!rows.empty()&&!rows.front())||std::adjacent_find(rows.begin(),rows.end())!=rows.end();
        };
        if(invalid(expected)||invalid(actual))return unknown(RearClearanceReason::InvalidVehicles);
        if(expected!=actual)return unknown(RearClearanceReason::MissingVehicle);
        std::vector<VehicleFootprint> footprints(observation.footprints.begin(),observation.footprints.end());
        std::vector<bool> covered(expected.size(),false);
        for(const auto& row:footprints){
            if(row.train!=route.ticket.train)return unknown(RearClearanceReason::InvalidIdentity,row.vehicle,row.track);
            if(!rear_clearance_detail::interval({row.track,row.begin,row.end}))
                return unknown(RearClearanceReason::InvalidInterval,row.vehicle,row.track);
            const auto at=std::lower_bound(expected.begin(),expected.end(),row.vehicle);
            if(at==expected.end()||*at!=row.vehicle)return unknown(RearClearanceReason::InvalidVehicles,row.vehicle,row.track);
            covered[static_cast<size_t>(at-expected.begin())]=true;
        }
        std::sort(footprints.begin(),footprints.end(),[](const auto& a,const auto& b){
            return rear_clearance_detail::footprintKey(a)<rear_clearance_detail::footprintKey(b);
        });
        if(std::adjacent_find(footprints.begin(),footprints.end(),[](const auto& a,const auto& b){
            return rear_clearance_detail::footprintKey(a)==rear_clearance_detail::footprintKey(b);
        })!=footprints.end())return unknown(RearClearanceReason::DuplicateFootprint);
        for(size_t i=0;i<covered.size();++i)if(!covered[i])return unknown(RearClearanceReason::MissingVehicle,expected[i]);
        // Normalize only real route portions. Never mix in alias intervals.
        std::vector<Interval> own(route.ownPhysical.begin(),route.ownPhysical.end());
        std::sort(own.begin(),own.end(),[](const auto& a,const auto& b){
            return std::tie(a.track,a.begin,a.end)<std::tie(b.track,b.begin,b.end);
        });
        size_t count=0;
        for(const auto& row:own){
            if(count&&own[count-1].track==row.track&&row.begin<=own[count-1].end)
                own[count-1].end=std::max(own[count-1].end,row.end);
            else own[count++]=row;
        }
        own.resize(count);size_t checks=0;
        for(const auto& row:footprints){
            size_t first=0,last=own.size();
            while(first<last){
                if(checks==limits.overlapChecks)return unknown(RearClearanceReason::BudgetExceeded,0,0,checks);
                ++checks;const auto middle=first+(last-first)/2;const auto& part=own[middle];
                if(part.track<row.track||(part.track==row.track&&part.end<row.begin))first=middle+1;
                else last=middle;
            }
            if(first<own.size()){
                if(checks==limits.overlapChecks)return unknown(RearClearanceReason::BudgetExceeded,0,0,checks);
                ++checks;const auto& part=own[first];
                // Closed intervals: front or rear endpoint contact still retains.
                if(part.track==row.track&&part.begin<=row.end)
                    return {Proof::No,RearClearanceReason::Occupied,row.vehicle,row.track,checks};
            }
        }
        if(!observation.completeConsist)return unknown(RearClearanceReason::IncompleteConsist,0,0,checks);
        if(!observation.stableTickCertificate)return unknown(RearClearanceReason::MissingStableTick,0,0,checks);
        if(route.entered!=Proof::Yes)return unknown(RearClearanceReason::NotEntered,0,0,checks);
        return {Proof::Yes,RearClearanceReason::Clear,0,0,checks};
    }catch(const std::bad_alloc&){return unknown(RearClearanceReason::Capacity);}
}
}
