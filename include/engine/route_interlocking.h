#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <map>
#include <new>
#include <optional>
#include <span>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace nimby::engine::interlocking {

// Caller-qualified resources only. The adapter must include ALL conflicting
// track aliases in the same coordinate system before asking for authority.
// Coordinates are fractions, not metres. Endpoints remain closed: contact is
// not proof of clearance, and reverse travel protects the same interval.
struct Interval {
    uint64_t track=0;
    double begin=0,end=0;
    bool operator==(const Interval&) const = default;
};
struct Ticket {
    uint64_t session=0,train=0,source=0,routeRevision=0;
    bool operator==(const Ticket&) const = default;
};
enum class Proof { Unknown, No, Yes };
struct Frame {
    uint64_t session=0,nowMs=0; // A caller-owned monotonic clock, not simulation date.
};
struct Request {
    Ticket ticket;
    std::span<const Interval> resources;
    uint64_t capturedAtMs=0;
    bool complete=false; // Complete, qualified route AND conflict resources.
    Proof entered=Proof::Unknown; // Physical entry past this source, independent of route revision.
};
struct Evidence {
    uint64_t capturedAtMs=0;
    std::optional<uint64_t> currentRouteRevision;
    Proof entered=Proof::Unknown,rearClear=Proof::Unknown;
};
enum class Outcome {
    Granted, Conflict, InvalidRequest, Unavailable, Stale, SessionMismatch,
    Capacity, BudgetExceeded, Missing, RetainedAfterEntry, Updated, Released
};
struct Result {
    Outcome outcome=Outcome::Unavailable;
    std::optional<Ticket> blocker;
    uint64_t track=0;
    size_t checks=0;
    Result() = default;
    Result(Outcome value,std::optional<Ticket> held={},uint64_t resource=0,size_t comparisons=0) noexcept
        : outcome(value),blocker(held),track(resource),checks(comparisons) {}
    bool granted() const noexcept { return outcome==Outcome::Granted; }
};
struct Limits {
    size_t leases=8192,totalResources=32768,resourcesPerRequest=512,conflictChecks=32768;
    uint64_t maximumAgeMs=1000;
};

// Pure single-owner state machine. The caller serializes calls and qualifies
// session lineage, route revisions, physical entry and rear clearance. No
// geometry guesses, game calls, mod callbacks, global mutex or native booking.
// Retention is separate from authority: expired/unknown evidence never frees
// another train's resources, but cannot renew a movement permission either.
class RouteInterlocking {
    struct Owner {
        uint64_t train,source;
        bool operator==(const Owner&) const = default;
        bool operator<(const Owner& value) const noexcept {
            return std::tie(train,source)<std::tie(value.train,value.source);
        }
    };
    struct Lease {
        Ticket ticket;
        std::vector<Interval> resources;
        uint64_t observedAt=0;
        bool entered=false,eligible=false;
    };
    struct Lock { double begin,end; Ticket ticket; };
    struct TrackIndex {
        std::vector<Lock> rows;
        std::vector<double> maximumEnd;
        void rebuild() {
            maximumEnd.resize(rows.size());
            double high=-1;
            for(size_t i=0;i<rows.size();++i)maximumEnd[i]=high=std::max(high,rows[i].end);
        }
    };
    Limits limits_;
    uint64_t session_=0,clock_=0;
    size_t resourceCount_=0;
    std::map<Owner,Lease> leases_;
    std::map<uint64_t,TrackIndex> tracks_;

    static Owner owner(const Ticket& value) noexcept { return {value.train,value.source}; }
    static bool valid(const Ticket& value) noexcept {
        return value.session&&value.routeRevision&&value.train>>48==5&&value.source>>48==8;
    }
    bool fresh(uint64_t at,const Frame& frame) const noexcept {
        return at<=frame.nowMs&&frame.nowMs-at<=limits_.maximumAgeMs;
    }
    Outcome frameStatus(const Frame& frame) const noexcept {
        if(!session_||frame.session!=session_)return Outcome::SessionMismatch;
        return frame.nowMs<clock_?Outcome::Stale:Outcome::Updated;
    }
    static bool intervalOrder(const Interval& a,const Interval& b) noexcept {
        return std::tie(a.track,a.begin,a.end)<std::tie(b.track,b.begin,b.end);
    }
    static bool lockOrder(const Lock& a,const Lock& b) noexcept {
        return std::tie(a.begin,a.end,a.ticket.train,a.ticket.source,a.ticket.routeRevision)<
            std::tie(b.begin,b.end,b.ticket.train,b.ticket.source,b.ticket.routeRevision);
    }
    // Canonicalization makes repeated renewals and diagnostics independent of
    // resource input order. Duplicate/overlapping aliases protect one interval.
    Outcome normalize(std::span<const Interval> input,std::vector<Interval>& output) const {
        if(input.empty())return Outcome::Unavailable;
        if(input.size()>limits_.resourcesPerRequest)return Outcome::Capacity;
        output.reserve(input.size());
        for(auto resource:input){
            if(resource.track>>48!=1||!std::isfinite(resource.begin)||!std::isfinite(resource.end)||
               resource.begin<0||resource.begin>1||resource.end<0||resource.end>1)return Outcome::InvalidRequest;
            if(resource.end<resource.begin)std::swap(resource.begin,resource.end);
            output.push_back(resource);
        }
        std::sort(output.begin(),output.end(),intervalOrder);
        size_t count=0;
        for(const auto& resource:output){
            if(count&&output[count-1].track==resource.track&&resource.begin<=output[count-1].end)
                output[count-1].end=std::max(output[count-1].end,resource.end);
            else output[count++]=resource;
        }
        output.resize(count);return Outcome::Updated;
    }
    void release(typename std::map<Owner,Lease>::iterator lease) noexcept {
        const auto key=owner(lease->second.ticket);
        uint64_t previousTrack=0;
        for(const auto& resource:lease->second.resources){
            if(resource.track==previousTrack)continue;
            previousTrack=resource.track; // Normalized resources are sorted by full track identity.
            const auto track=tracks_.find(resource.track);
            if(track==tracks_.end())continue;
            std::erase_if(track->second.rows,[&](const Lock& value){return owner(value.ticket)==key;});
            if(track->second.rows.empty())tracks_.erase(track);
            else track->second.rebuild(); // Shrinking the prefix requires no allocation.
        }
        resourceCount_-=lease->second.resources.size();leases_.erase(lease);
    }
    Result conflicts(const Ticket& ticket,std::span<const Interval> resources) const noexcept {
        Result result{Outcome::Granted};
        for(const auto& resource:resources){
            const auto track=tracks_.find(resource.track);if(track==tracks_.end())continue;
            const auto& index=track->second;
            const auto first=std::lower_bound(index.maximumEnd.begin(),index.maximumEnd.end(),resource.begin);
            for(size_t i=static_cast<size_t>(first-index.maximumEnd.begin());i<index.rows.size();++i){
                const auto& held=index.rows[i];
                if(held.begin>resource.end)break;
                if(result.checks==limits_.conflictChecks)return {Outcome::BudgetExceeded,std::nullopt,resource.track,result.checks};
                ++result.checks;
                // The same physical train may protect consecutive signals.
                if(held.ticket.train==ticket.train||held.end<resource.begin)continue;
                return {Outcome::Conflict,held.ticket,resource.track,result.checks};
            }
        }
        return result;
    }
    // All allocation/copying happens before the no-throw commit. Only affected
    // track indexes are prepared; healthy independent leases are never copied.
    void commit(Lease value) {
        const auto ticket=value.ticket;
        const auto count=value.resources.size();
        std::map<uint64_t,TrackIndex> prepared;
        for(const auto& resource:value.resources){
            auto [at,inserted]=prepared.try_emplace(resource.track);
            if(inserted){const auto current=tracks_.find(resource.track);if(current!=tracks_.end())at->second=current->second;}
            at->second.rows.push_back({resource.begin,resource.end,ticket});
        }
        for(auto& [id,index]:prepared){std::sort(index.rows.begin(),index.rows.end(),lockOrder);index.rebuild();}
        std::map<Owner,Lease> nextLease;
        nextLease.emplace(owner(ticket),std::move(value));
        leases_.insert(nextLease.extract(nextLease.begin()));
        while(!prepared.empty()){
            const auto current=tracks_.find(prepared.begin()->first);
            if(current==tracks_.end())tracks_.insert(prepared.extract(prepared.begin()));
            else {std::swap(current->second,prepared.begin()->second);prepared.erase(prepared.begin());}
        }
        resourceCount_+=count;
    }
public:
    explicit RouteInterlocking(Limits limits={}):limits_(limits) {
        if(!limits.leases||limits.leases>32768||!limits.totalResources||limits.totalResources>131072||
           !limits.resourcesPerRequest||limits.resourcesPerRequest>4096||!limits.conflictChecks||
           limits.conflictChecks>1048576||!limits.maximumAgeMs||limits.maximumAgeMs>60000)
            throw std::invalid_argument("Invalid interlocking limits");
    }
    // This is the ONLY unconditional cleanup. The caller must positively
    // establish a new loaded-world lineage; missing session data is not reset.
    bool beginSession(uint64_t session) noexcept {
        if(!session)return false;
        if(session==session_)return true;
        leases_.clear();tracks_.clear();resourceCount_=0;clock_=0;session_=session;return true;
    }
    Result acquire(const Request& request,const Frame& frame) {
        if(!valid(request.ticket))return {Outcome::InvalidRequest};
        const auto status=frameStatus(frame);if(status!=Outcome::Updated)return {status};
        if(request.ticket.session!=session_)return {Outcome::SessionMismatch};
        clock_=frame.nowMs;
        if(!fresh(request.capturedAtMs,frame))return {Outcome::Stale};
        auto existing=leases_.find(owner(request.ticket));
        if(existing!=leases_.end()){
            if(request.capturedAtMs<existing->second.observedAt)return {Outcome::Stale};
            // A newer incomplete/contradictory request closes only its owner.
            // Positive physical entry is independent of route qualification;
            // forgetting it could release an occupied former route on reroute.
            existing->second.observedAt=request.capturedAtMs;
            existing->second.eligible=false;
            existing->second.entered|=request.entered==Proof::Yes;
        }
        if(!request.complete||request.entered==Proof::Unknown)return {Outcome::Unavailable};
        try {
            std::vector<Interval> resources;
            const auto normalized=normalize(request.resources,resources);
            if(normalized!=Outcome::Updated)return {normalized};
            if(existing!=leases_.end()){
                if(existing->second.ticket.routeRevision==request.ticket.routeRevision){
                    if(existing->second.resources!=resources)return {Outcome::InvalidRequest};
                    existing->second.observedAt=request.capturedAtMs;
                    existing->second.entered|=request.entered==Proof::Yes;
                    existing->second.eligible=true;
                    return {Outcome::Granted};
                }
                existing->second.eligible=false;
                if(existing->second.entered||request.entered!=Proof::No){
                    existing->second.entered|=request.entered==Proof::Yes;
                    return {Outcome::RetainedAfterEntry};
                }
                // A freshly qualified different route with its head still
                // BEFORE entry positively revokes the former unentered route.
                // This invalidation stands even if acquiring the NEW route fails.
                release(existing);
            }
            if(leases_.size()==limits_.leases||resources.size()>limits_.totalResources-resourceCount_)
                return {Outcome::Capacity};
            const auto conflict=conflicts(request.ticket,resources);
            if(!conflict.granted())return conflict;
            commit({request.ticket,std::move(resources),request.capturedAtMs,request.entered==Proof::Yes,true});
            return conflict;
        }catch(const std::bad_alloc&){return {Outcome::Capacity};}
    }
    Result observe(const Ticket& ticket,const Evidence& evidence,const Frame& frame) noexcept {
        if(!valid(ticket)||(evidence.currentRouteRevision&&!*evidence.currentRouteRevision))return {Outcome::InvalidRequest};
        const auto status=frameStatus(frame);if(status!=Outcome::Updated)return {status};
        if(ticket.session!=session_)return {Outcome::SessionMismatch};
        clock_=frame.nowMs;
        auto lease=leases_.find(owner(ticket));
        if(lease==leases_.end())return {Outcome::Missing};
        if(lease->second.ticket!=ticket)return {Outcome::Missing};
        if(!fresh(evidence.capturedAtMs,frame)||evidence.capturedAtMs<lease->second.observedAt)return {Outcome::Stale};
        auto& value=lease->second;
        value.entered|=evidence.entered==Proof::Yes;
        if(evidence.rearClear==Proof::Yes&&value.entered){release(lease);return {Outcome::Released};}
        const bool routeKnown=evidence.currentRouteRevision.has_value();
        const bool sameRoute=routeKnown&&*evidence.currentRouteRevision==ticket.routeRevision;
        value.observedAt=evidence.capturedAtMs;
        value.eligible=sameRoute&&evidence.entered!=Proof::Unknown;
        if(routeKnown&&!sameRoute){
            if(!value.entered&&evidence.entered==Proof::No){release(lease);return {Outcome::Released};}
            return {Outcome::RetainedAfterEntry};
        }
        return {value.eligible?Outcome::Updated:Outcome::Unavailable};
    }
    // A granted ticket authorizes only its own train/source/route/world. A
    // caller must never treat mere retained ownership as a movement permission.
    Result permission(const Ticket& ticket,const Frame& frame) const noexcept {
        if(!valid(ticket))return {Outcome::InvalidRequest};
        const auto status=frameStatus(frame);if(status!=Outcome::Updated)return {status};
        if(ticket.session!=session_)return {Outcome::SessionMismatch};
        const auto lease=leases_.find(owner(ticket));
        if(lease==leases_.end()||lease->second.ticket!=ticket)return {Outcome::Missing};
        if(!fresh(lease->second.observedAt,frame))return {Outcome::Stale};
        return {lease->second.eligible?Outcome::Granted:Outcome::Unavailable};
    }
    bool holds(const Ticket& ticket) const noexcept {
        const auto lease=leases_.find(owner(ticket));
        return lease!=leases_.end()&&lease->second.ticket==ticket;
    }
    size_t leaseCount() const noexcept { return leases_.size(); }
    size_t resourceCount() const noexcept { return resourceCount_; }
    uint64_t session() const noexcept { return session_; }
};
} // namespace nimby::engine::interlocking
