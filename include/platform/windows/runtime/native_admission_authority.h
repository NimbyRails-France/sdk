#pragma once
#include <engine/route_aggregate_clearance.h>
#include <atomic>
#include <bit>
#include <memory>
#include <utility>

namespace nimby::windows::two_phase_admission { class InvocationAccess; }
namespace nimby::windows::admission {
using engine::interlocking::Interval;
using engine::interlocking::Proof;
using engine::interlocking::Ticket;
namespace rear=engine::native_rear;

// Private, portable adapter contract. This header installs no hooks and proves
// neither a native ABI nor world/phase ownership. In particular 45F860 readers
// and the 451C90/450B40/4512D0/451820 writers are NOT redirected by this class.
// All witnesses below must be produced by an owner of the complete native
// publication, not by pointer equality, tick/header double reads or a timeout.
enum class Mode : uint8_t { ParallelAudit, FinalVeto, ExclusiveSubstitution };
enum class NativeDomain : uint8_t { FinalAdmission, ExclusiveReservationOnly };
struct NativeDecision {
    bool admitted=false,qualified=false;
    NativeDomain domain=NativeDomain::FinalAdmission;
};
struct Frame {
    uint64_t lineage=0,window=0,nowMs=0;
    rear::Publication at;
};
struct PublicationWitness {
    uint64_t lineage=0,window=0;
    rear::Publication at;
    bool lineageOwned=false,phaseOwned=false;
    // This must be SDK-owned, current ownership bookkeeping for this exact
    // publication. It is noexcept, allocation-free and performs no wait, RPC,
    // native game call, mod callback or authority re-entry. Declaration flags
    // and a copied stamp alone never prove that the owner is still held.
    bool (*current)(void*,const PublicationWitness&) noexcept=nullptr;
    void* context=nullptr;
    bool currentOwned() const noexcept {
        return lineageOwned&&phaseOwned&&current&&current(context,*this);
    }
};
struct WorldReadyWitness {
    PublicationWitness publication;
    bool worldReadyBoundary=false,loadComplete=false,allWritersQuiescent=false;
};
struct NativeCompatibilityWitness {
    // These declarations have no production producer yet. A synthetic fixture
    // may qualify them; merely selecting ExclusiveSubstitution does not.
    bool abi=false,allAdmissionSites=false,allExclusiveReaders=false;
    bool allExclusiveWriters=false,writerOwnership=false;
    bool complete() const noexcept {
        return abi&&allAdmissionSites&&allExclusiveReaders&&allExclusiveWriters&&writerOwnership;
    }
};
inline constexpr size_t maximumRequestPortions=512;
struct AdmissionRequest {
    Ticket ticket;
    PublicationWitness publication;
    uint64_t capturedAtMs=0,consistRevision=0;
    bool routeComplete=false,conflictsComplete=false;
    bool physicalOccupationComplete=false,scriptVetoComplete=false;
    Proof physicalClear=Proof::Unknown,scriptAllows=Proof::Unknown;
    Proof entered=Proof::Unknown;
    // Protected resources include ownPhysical AND every conflicting alias.
    // They are copied, canonical, sorted, disjoint closed intervals; no native
    // pointers/spans survive admission. Aliases are not physical train history.
    std::array<Interval,maximumRequestPortions> resources{},ownPhysical{};
    size_t resourceCount=0,physicalCount=0;
    NativeCompatibilityWitness compatibility;
};
// Internal contract only. A production producer must own the exact admission
// phase through physical/script traversal and exclude every mutable input.
// The current-only retained-body atlas cannot manufacture this BeforeMovement
// window, and an observed-only mutation journal cannot certify continuity.
struct OwnedAdmissionWindow {
    PublicationWitness publication;
    uint64_t continuity=0;
    bool allMutableInputsExcluded=false;
    // As for PublicationWitness.current: SDK-owned noexcept POD/TLS checks only;
    // no waits, allocations, native/mod calls, RPC or authority re-entry.
    bool (*current)(void*,const OwnedAdmissionWindow&) noexcept=nullptr;
    void* context=nullptr;
    bool valid() const noexcept {
        return publication.lineage&&publication.window&&publication.at.session&&publication.at.epoch&&
            publication.at.phase==rear::Phase::BeforeMovement&&continuity&&allMutableInputsExcluded&&
            current&&publication.currentOwned()&&current(context,*this);
    }
};
struct ProvisionalToken {
    Ticket ticket;
    uint64_t leaseSerial=0,lineage=0,window=0,consistRevision=0,continuity=0,capturedAtMs=0;
    rear::Publication at;
    bool operator==(const ProvisionalToken&) const=default;
    explicit operator bool() const noexcept{return leaseSerial!=0;}
};
struct NoEntryWitness {
    ProvisionalToken claim;
    PublicationWitness publication;
    uint64_t capturedAtMs=0,continuity=0,fromWindow=0,throughWindow=0;
    // Separate positive non-entry and COMPLETE writer history, not native zero,
    // unchanged current geometry, elapsed time or the observed-only journal.
    bool positiveNoEntry=false,originalReturned=false,noPermissionOutstanding=false;
    bool allEntryWritersObserved=false,historyComplete=false;
};
struct EntryWitness {
    Ticket ticket;
    uint64_t leaseSerial=0,consistRevision=0;
    PublicationWitness publication;
    bool nativeReturned=false,movementCommitted=false,retainedMirrorMatches=false;
    bool identityAndRouteUnchanged=false;
};
enum class Reason : uint8_t {
    Granted,AuditOnly,Ready,AlreadyReady,Released,Entered,Retained,Missing,
    Busy,NotReady,InvalidLimits,InvalidWorldReady,LineageMismatch,LineageNotMonotonic,
    UnqualifiedLineage,UnqualifiedPhase,DifferentPublication,StaleWindow,WrongPhase,
    InvalidTicket,InvalidConsist,IncompleteRoute,IncompleteConflicts,
    IncompleteOccupation,OccupationUnknown,Occupied,IncompleteVeto,VetoUnknown,ScriptVeto,InvalidIntervals,
    PhysicalNotProtected,StaleCapture,NativeDecisionUnqualified,NativeRefusal,
    NativeDomainMismatch,NativeCompatibilityMissing,Conflict,Capacity,BudgetExceeded,
    SerialExhausted,RouteChangedRetained,ConsistChangedRetained,ResourcesChangedRetained,
    UnqualifiedEntry,EntryUnknown,SnapshotChanged,RearUnknown,RearOccupied,
    OriginalException,CaptureException,AllocationFailure,IdentityChanged,
    ArithmeticUnqualified,
    ProvisionalClaimed,FinalizationRequired,FinalizationRepeated,OwnedWindowLost,
    AttemptMismatch,NoEntryUnqualified,ContinuityUnqualified,ContinuityChanged,CancelledBeforeEntry,IndexInvalid,IndexEpochExhausted
};
struct Result {
    Reason reason=Reason::NotReady;
    Proof leaseHeld=Proof::Unknown;
    uint64_t leaseSerial=0,track=0;
    Ticket blocker;
    size_t work=0;
    bool mayPass=false,replaceNativeExclusive=false;
    // A contract-only result must never be reported as runtime hook coverage.
    static constexpr bool nativeReplacementConnected=false;
    engine::interlocking::AggregateClearanceResult rearClearance;
    Result()=default;
    Result(Reason value) noexcept:reason(value){}
};
struct Limits {
    size_t leases=1024,resourceRows=8192,operations=65536;
    uint64_t maximumAgeMs=1000;
};
struct RetainedSnapshot {
    Ticket ticket;
    uint64_t leaseSerial=0,consistRevision=0;
    Proof entered=Proof::Unknown;
    std::array<Interval,maximumRequestPortions> resources{},ownPhysical{};
    size_t resourceCount=0,physicalCount=0;
};

// No thread waits: contention closes THIS query and retains existing claims.
// Memory is allocated once at construction: leases, two resource buffers,
// bounded owner hash slots and two per-lease row-head buffers.
// Admission, entry, snapshots and release use no allocation, RPC, game call or
// mod callback under the gate. Each scan/merge/search consumes a bounded budget.
// A permission is usable only while the provider continues to own the exact
// publication window through native movement commit. The gate itself does NOT
// stop native workers or qualify that window. Retention is not permission.
class NativeAdmissionAuthority {
    // Only the coordinator scope can construct this capability. Publication
    // spends its public token before the original is called; this private,
    // non-copyable permit can judge once and is consumed before trying gate_.
    class InvocationPermit {
        friend class NativeAdmissionAuthority;
        friend class nimby::windows::two_phase_admission::InvocationAccess;
        ProvisionalToken claim_;
        bool available_=false;
        InvocationPermit() noexcept=default;
    public:
        InvocationPermit(const InvocationPermit&)=delete;
        InvocationPermit& operator=(const InvocationPermit&)=delete;
        InvocationPermit(InvocationPermit&&)=delete;
        InvocationPermit& operator=(InvocationPermit&&)=delete;
    };
    friend class nimby::windows::two_phase_admission::InvocationAccess;
    struct Lease {
        Ticket ticket;
        uint64_t serial=0,consistRevision=0;
        bool live=false,entered=false,uncertain=false;
        bool provisional=false,finalizationAttempted=false;
        ProvisionalToken claim;
        // Immutable original cardinalities: a damaged/truncated cache must not
        // accept an equally shortened request or manufacture a partial snapshot.
        uint16_t resourceCount=0,physicalCount=0;
    };
    // Private slots/row indices only. Full 64-bit world/train/source identities
    // never narrow. Constructor caps 8192 slots and 32768 rows below sentinel.
    static constexpr uint16_t noRow=UINT16_MAX;
    struct Row {Interval interval;uint16_t lease=0;bool physical=false;uint16_t next=noRow;};
    struct OwnerSlot {uint64_t session{},train{},source{},epoch{};uint16_t lease{};uint8_t state{};};
    struct Head {uint16_t row=noRow,resourceCount=0,physicalCount=0,touchedNext=noRow;uint64_t epoch{};};
    struct Gate {
        std::atomic_flag& flag;
        bool held;
        explicit Gate(std::atomic_flag& value) noexcept:flag(value),held(!flag.test_and_set(std::memory_order_acquire)){}
        ~Gate(){if(held)flag.clear(std::memory_order_release);}
    };
    struct Budget {
        size_t limit,used=0;
        bool invalidIndex=false;
        bool invalidate() noexcept {invalidIndex=true;return false;}
        bool take(size_t amount=1) noexcept {
            if(amount>limit-used)return false;
            used+=amount;return true;
        }
    };
    Limits limits_;
    std::unique_ptr<Lease[]> leases_;
    std::unique_ptr<Row[]> rows_,prepared_;
    std::unique_ptr<OwnerSlot[]> ownerIndex_;
    std::unique_ptr<Head[]> heads_,preparedHeads_;
    size_t ownerSlots_=0;
    uint64_t rowEpoch_=0,nextRowEpoch_=0;
    std::atomic_flag gate_=ATOMIC_FLAG_INIT;
    size_t rowCount_=0;
    uint64_t lineage_=0,session_=0,readyEpoch_=0,lastWindow_=0,lastNow_=0,serial_=0;
    friend struct NativeAdmissionAuthorityTestAccess;

    static bool identity(uint64_t value,uint64_t kind) noexcept {
        return value>>48==kind&&(value&0x0000ffffffffffffULL)!=0;
    }
    static bool ticketValid(const Ticket& value) noexcept {
        return value.session&&value.routeRevision&&identity(value.train,5)&&identity(value.source,8);
    }
    static bool publicationValid(const rear::Publication& value) noexcept {
        return value.session&&value.epoch&&(value.phase==rear::Phase::BeforeMovement||value.phase==rear::Phase::RetainedMovement);
    }
    static bool lessInterval(const Interval& a,const Interval& b) noexcept {
        return std::tie(a.track,a.begin,a.end)<std::tie(b.track,b.begin,b.end);
    }
    static bool lessRow(const Row& a,const Row& b) noexcept {
        return std::tie(a.interval.track,a.interval.begin,a.interval.end,a.physical,a.lease)<
               std::tie(b.interval.track,b.interval.begin,b.interval.end,b.physical,b.lease);
    }
    static bool fraction(double value) noexcept {
        // Classify before any comparison: DAZ can turn a negative subnormal
        // into zero, and a signalling NaN can change/trap the caller's FP state.
        // Reject both subnormal signs even under otherwise qualified controls.
        const auto bits=std::bit_cast<uint64_t>(value);
        const auto exponent=bits&UINT64_C(0x7ff0000000000000);
        const auto mantissa=bits&UINT64_C(0x000fffffffffffff);
        return exponent!=UINT64_C(0x7ff0000000000000)&&!(exponent==0&&mantissa)&&
            value>=0&&value<=1;
    }
    static bool canonical(const Interval* rows,size_t count,Budget& budget) noexcept {
        for(size_t i=0;i<count;++i) {
            if(!budget.take())return false;
            const auto& r=rows[i];
            if(!identity(r.track,1)||!fraction(r.begin)||!fraction(r.end)||r.begin>r.end)return false;
            if(i&&(!lessInterval(rows[i-1],r)||
                   (rows[i-1].track==r.track&&rows[i-1].end>=r.begin)))return false;
        }
        return true;
    }
    Reason frameReason(const Frame& frame,const PublicationWitness& witness) const noexcept {
        if(!lineage_)return Reason::NotReady;
        if(!witness.lineageOwned)return Reason::UnqualifiedLineage;
        if(!witness.phaseOwned)return Reason::UnqualifiedPhase;
        if(frame.lineage!=lineage_||witness.lineage!=lineage_||frame.at.session!=session_)
            return Reason::LineageMismatch;
        if(!publicationValid(frame.at)||witness.at!=frame.at||witness.window!=frame.window||!frame.window)
            return Reason::DifferentPublication;
        if(frame.at.epoch<readyEpoch_||frame.window<lastWindow_||frame.nowMs<lastNow_)
            return Reason::StaleWindow;
        if(!witness.currentOwned())return Reason::UnqualifiedPhase;
        return Reason::Granted;
    }
    void acceptFrame(const Frame& frame) noexcept {lastWindow_=frame.window;lastNow_=frame.nowMs;}
    static uint64_t mix(uint64_t x) noexcept {
        x^=x>>30;x*=UINT64_C(0xbf58476d1ce4e5b9);x^=x>>27;
        x*=UINT64_C(0x94d049bb133111eb);return x^(x>>31);
    }
    size_t ownerHash(const Ticket& ticket) const noexcept {
        return size_t(mix(ticket.session)^std::rotl(mix(ticket.train),21)^std::rotl(mix(ticket.source),42))&(ownerSlots_-1);
    }
    bool nodeValid(const OwnerSlot& node,Budget& budget) const noexcept {
        if(node.state!=1||node.lease>=limits_.leases)return budget.invalidate();
        const auto& lease=leases_[node.lease];
        if(!lease.live||lease.ticket.session!=node.session||lease.ticket.train!=node.train||
           lease.ticket.source!=node.source)return budget.invalidate();
        return true;
    }
    size_t owner(const Ticket& ticket,Budget& budget,size_t& free,size_t* vacancy=nullptr) const noexcept {
        free=limits_.leases;if(vacancy)*vacancy=ownerSlots_;
        size_t candidate=ownerSlots_,slot=ownerHash(ticket),visited=0;
        bool ended=false;
        const auto matches=[&](const OwnerSlot& node) {
            return node.session==ticket.session&&node.train==ticket.train&&node.source==ticket.source;
        };
        // A bounded fast prefix handles ordinary hits without making a long
        // tombstone chain the cost of every new owner. It is NOT absence proof.
        for(;visited<std::min(size_t{64},ownerSlots_);++visited,slot=(slot+1)&(ownerSlots_-1)) {
            if(!budget.take())return limits_.leases;
            const auto& node=ownerIndex_[slot];
            if(node.epoch!=lineage_||!node.state) {
                if(candidate==ownerSlots_)candidate=slot;
                ended=true;break;
            }
            if(node.state==2){if(candidate==ownerSlots_)candidate=slot;continue;}
            if(!nodeValid(node,budget))return limits_.leases;
            if(matches(node))return node.lease;
        }
        // Only the complete authoritative lease scan proves absence. Local
        // outputs remain empty until every scan unit and required cache node
        // is qualified; budget/invalid failures cannot become an insertion.
        size_t firstFree=limits_.leases,existing=limits_.leases;
        for(size_t i=0;i<limits_.leases;++i) {
            if(!budget.take())return limits_.leases;
            const auto& lease=leases_[i];
            if(!lease.live){if(firstFree==limits_.leases)firstFree=i;continue;}
            if(lease.ticket.session==ticket.session&&lease.ticket.train==ticket.train&&lease.ticket.source==ticket.source) {
                if(existing!=limits_.leases){budget.invalidate();return limits_.leases;}
                existing=i;
            }
        }
        if(existing!=limits_.leases) {
            // More than 64 real collisions are valid. Resume the same probe
            // chain and require the exact authoritative lease, never heal a
            // missing owner cache or accept a different full identity/index.
            if(!ended)for(;visited<ownerSlots_;++visited,slot=(slot+1)&(ownerSlots_-1)) {
                if(!budget.take())return limits_.leases;
                const auto& node=ownerIndex_[slot];
                if(node.epoch!=lineage_||!node.state)break;
                if(node.state==2)continue;
                if(!nodeValid(node,budget))return limits_.leases;
                if(matches(node)) {
                    if(node.lease!=existing){budget.invalidate();return limits_.leases;}
                    return existing;
                }
            }
            budget.invalidate();return limits_.leases;
        }
        // Read-only misses need no insertion slot. A new claim may reuse a
        // vacancy observed under this same Gate after complete absence proof.
        // This is local cache qualification, not a global corruption scan.
        if(vacancy&&firstFree!=limits_.leases&&candidate==ownerSlots_&&!ended) {
            for(;visited<ownerSlots_;++visited,slot=(slot+1)&(ownerSlots_-1)) {
                if(!budget.take())return limits_.leases;
                const auto& node=ownerIndex_[slot];
                if(node.epoch!=lineage_||!node.state||node.state==2){candidate=slot;break;}
                if(!nodeValid(node,budget))return limits_.leases;
            }
            if(candidate==ownerSlots_){budget.invalidate();return limits_.leases;}
        }
        free=firstFree;if(vacancy)*vacancy=candidate;
        return limits_.leases;
    }
    size_t prepareOwner(const Ticket& ticket,Budget& budget,bool removing=false) const noexcept {
        size_t firstTombstone=ownerSlots_,slot=ownerHash(ticket);
        for(size_t visited=0;visited<ownerSlots_;++visited,slot=(slot+1)&(ownerSlots_-1)) {
            if(!budget.take())return ownerSlots_;
            const auto& node=ownerIndex_[slot];
            if(node.epoch!=lineage_||!node.state)return removing?ownerSlots_:
                firstTombstone<ownerSlots_?firstTombstone:slot;
            if(node.state==2){if(firstTombstone==ownerSlots_)firstTombstone=slot;continue;}
            if(!nodeValid(node,budget))return ownerSlots_;
            if(node.session==ticket.session&&node.train==ticket.train&&node.source==ticket.source)return slot;
        }
        return removing?ownerSlots_:firstTombstone;
    }
    size_t firstRow(size_t index,Budget& budget) const noexcept {
        if(index>=limits_.leases||!leases_[index].live||rowCount_>limits_.resourceRows||rowCount_>=noRow) {
            budget.invalidate();return noRow;
        }
        const auto& lease=leases_[index];const auto& head=heads_[index];
        if(!lease.resourceCount||!lease.physicalCount||lease.resourceCount>maximumRequestPortions||
           lease.physicalCount>maximumRequestPortions||head.epoch!=rowEpoch_||head.row>=rowCount_||
           head.resourceCount!=lease.resourceCount||head.physicalCount!=lease.physicalCount) {
            budget.invalidate();return noRow;
        }
        return head.row;
    }
    bool chainStep(size_t index,size_t row,size_t& previous,Budget& budget) const noexcept {
        if(!budget.take())return false;
        if(row>=rowCount_||rows_[row].lease!=index||(previous!=noRow&&row<=previous))return budget.invalidate();
        previous=row;return true;
    }
    bool linkPrepared(size_t row,uint64_t epoch,uint16_t& touched,Budget& budget) noexcept {
        auto& value=prepared_[row];if(value.lease>=limits_.leases)return budget.invalidate();
        auto& head=preparedHeads_[value.lease];
        if(head.epoch!=epoch) {
            head={noRow,0,0,touched,epoch};touched=value.lease;
        }
        value.next=head.row;head.row=static_cast<uint16_t>(row);
        auto& count=value.physical?head.physicalCount:head.resourceCount;
        if(count==maximumRequestPortions)return budget.invalidate();
        ++count;return true;
    }
    bool validatePrepared(uint16_t touched,uint64_t epoch,Budget& budget,size_t pending,
                          size_t resources=0,size_t physical=0) const noexcept {
        size_t visited=0;
        for(size_t i=touched;i!=noRow;i=preparedHeads_[i].touchedNext) {
            if(!budget.take())return false;
            if(i>=limits_.leases||++visited>limits_.leases)return budget.invalidate();
            const auto& head=preparedHeads_[i];const auto& lease=leases_[i];
            const auto wantedResources=i==pending?resources:lease.resourceCount;
            const auto wantedPhysical=i==pending?physical:lease.physicalCount;
            if(head.epoch!=epoch||(!lease.live&&i!=pending)||!wantedResources||!wantedPhysical||
               wantedResources>maximumRequestPortions||wantedPhysical>maximumRequestPortions||
               head.resourceCount!=wantedResources||head.physicalCount!=wantedPhysical)return budget.invalidate();
        }
        return true;
    }
    bool prepareHeads(Budget& budget,size_t count,uint64_t& epoch,size_t pending,
                      size_t resources,size_t physical) noexcept {
        if(nextRowEpoch_==UINT64_MAX)return false;
        if(count>limits_.resourceRows||count>=noRow)return budget.invalidate();
        epoch=++nextRowEpoch_;uint16_t touched=noRow;
        // Reverse traversal preserves the global sorted index and builds each
        // local chain in strictly increasing row order. Epochs keep abandoned
        // staging scratch unreachable and avoid clearing all heads per edit.
        for(size_t n=count;n;) {
            if(!budget.take())return false;
            if(!linkPrepared(--n,epoch,touched,budget))return false;
        }
        return validatePrepared(touched,epoch,budget,pending,resources,physical);
    }
    bool prepareRemoval(size_t index,Budget& budget,size_t& count,uint64_t& epoch) noexcept {
        if(nextRowEpoch_==UINT64_MAX)return false;
        size_t resource=0,physical=0,previous=noRow;
        for(size_t i=firstRow(index,budget);i!=noRow;i=rows_[i].next) {
            if(!chainStep(index,i,previous,budget))return false;
            auto& seen=rows_[i].physical?physical:resource;
            const auto wanted=rows_[i].physical?leases_[index].physicalCount:leases_[index].resourceCount;
            if(++seen>wanted)return budget.invalidate();
        }
        if(budget.invalidIndex)return false;
        const auto& lease=leases_[index];
        if(resource!=lease.resourceCount||physical!=lease.physicalCount)return budget.invalidate();
        const auto removed=resource+physical;
        if(removed>rowCount_)return budget.invalidate();
        count=rowCount_-removed;size_t out=count;epoch=++nextRowEpoch_;uint16_t touched=noRow;
        // rowCount_ visits were charged by the caller. Reverse stable
        // compaction and local-chain creation share this single pass.
        for(size_t n=rowCount_;n;) {
            const auto& row=rows_[--n];if(row.lease>=limits_.leases)return budget.invalidate();
            if(row.lease==index)continue;
            if(!out)return budget.invalidate();
            prepared_[--out]=row;
            if(!linkPrepared(out,epoch,touched,budget))return false;
        }
        if(out)return budget.invalidate();
        return validatePrepared(touched,epoch,budget,limits_.leases);
    }
    Result result(Reason reason,size_t index,const Budget& budget) const noexcept {
        Result r;r.reason=budget.invalidIndex?Reason::IndexInvalid:reason;r.work=budget.used;
        r.leaseHeld=index<limits_.leases&&leases_[index].live?Proof::Yes:Proof::No;
        if(r.leaseHeld==Proof::Yes)r.leaseSerial=leases_[index].serial;
        else if(budget.invalidIndex)r.leaseHeld=Proof::Unknown;
        return r;
    }
    bool snapshotRows(size_t index,RetainedSnapshot& value,Budget& budget) const noexcept {
        value.resourceCount=value.physicalCount=0;size_t previous=noRow;
        const auto& lease=leases_[index];
        for(size_t i=firstRow(index,budget);i!=noRow;i=rows_[i].next) {
            if(!chainStep(index,i,previous,budget))return false;
            if(rows_[i].physical) {
                if(value.physicalCount>=lease.physicalCount)return budget.invalidate();
                value.ownPhysical[value.physicalCount++]=rows_[i].interval;
            } else {
                if(value.resourceCount>=lease.resourceCount)return budget.invalidate();
                value.resources[value.resourceCount++]=rows_[i].interval;
            }
        }
        if(budget.invalidIndex)return false;
        if(value.resourceCount!=lease.resourceCount||value.physicalCount!=lease.physicalCount)return budget.invalidate();
        return true;
    }
    template<class Packet>
    bool sameRows(size_t index,const Packet& value,Budget& budget) const noexcept {
        const auto& lease=leases_[index];
        const auto first=firstRow(index,budget);if(budget.invalidIndex)return false;
        if(value.resourceCount!=lease.resourceCount||value.physicalCount!=lease.physicalCount)return false;
        size_t resource=0,physical=0,previous=noRow;
        for(size_t i=first;i!=noRow;i=rows_[i].next) {
            if(!chainStep(index,i,previous,budget))return false;
            const auto& row=rows_[i];
            if(row.physical) {
                if(physical>=lease.physicalCount)return budget.invalidate();
                if(row.interval!=value.ownPhysical[physical++])return false;
            } else {
                if(resource>=lease.resourceCount)return budget.invalidate();
                if(row.interval!=value.resources[resource++])return false;
            }
        }
        if(budget.invalidIndex)return false;
        if(resource!=lease.resourceCount||physical!=lease.physicalCount)return budget.invalidate();
        return true;
    }
    bool identicalRows(size_t index,const RetainedSnapshot& value,Budget& budget) const noexcept {
        return sameRows(index,value,budget);
    }
    bool sameRequest(size_t index,const AdmissionRequest& value,Budget& budget) const noexcept {
        return sameRows(index,value,budget);
    }
    Result retainedFailure(Reason reason,size_t index,const Budget& budget) noexcept {
        if(index<limits_.leases&&leases_[index].live)leases_[index].uncertain=true;
        return result(reason,index,budget);
    }
public:
    explicit NativeAdmissionAuthority(Limits limits={}):limits_(limits) {
        if(!limits.leases||limits.leases>8192||!limits.resourceRows||limits.resourceRows>32768||
           !limits.operations||limits.operations>4194304||!limits.maximumAgeMs)
            throw std::invalid_argument("NativeAdmissionAuthority limits");
        leases_=std::make_unique<Lease[]>(limits.leases);
        rows_=std::make_unique<Row[]>(limits.resourceRows);
        prepared_=std::make_unique<Row[]>(limits.resourceRows);
        ownerSlots_=std::bit_ceil(limits.leases*2);
        ownerIndex_=std::make_unique<OwnerSlot[]>(ownerSlots_);
        heads_=std::make_unique<Head[]>(limits.leases);
        preparedHeads_=std::make_unique<Head[]>(limits.leases);
    }
    NativeAdmissionAuthority(const NativeAdmissionAuthority&)=delete;
    NativeAdmissionAuthority& operator=(const NativeAdmissionAuthority&)=delete;
    size_t storageBytes() const noexcept {
        return limits_.leases*sizeof(Lease)+2*limits_.resourceRows*sizeof(Row)+
            ownerSlots_*sizeof(OwnerSlot)+2*limits_.leases*sizeof(Head);
    }
    size_t operationLimit() const noexcept {return limits_.operations;}

    // The only reset. New lineage must be an SDK-issued, strictly increasing
    // identity at a qualified completed world-ready boundary. Same-world calls
    // do NOT clear claims, including claims retained after a crash or reroute.
    Result beginWorld(const Frame& frame,const WorldReadyWitness& ready) noexcept {
        Gate gate(gate_);if(!gate.held)return {Reason::Busy};
        Budget budget{limits_.operations};const auto& w=ready.publication;
        if(!frame.lineage||!frame.window||!publicationValid(frame.at)||!w.lineageOwned||!w.phaseOwned||
           w.lineage!=frame.lineage||w.window!=frame.window||w.at!=frame.at||
           !ready.worldReadyBoundary||!ready.loadComplete||!ready.allWritersQuiescent||!w.currentOwned())
            return result(Reason::InvalidWorldReady,limits_.leases,budget);
        if(frame.lineage==lineage_) {
            if(frame.at.session!=session_)return result(Reason::LineageMismatch,limits_.leases,budget);
            return result(Reason::AlreadyReady,limits_.leases,budget);
        }
        if(frame.lineage<lineage_||frame.at.session<=session_||frame.nowMs<lastNow_)
            return result(Reason::LineageNotMonotonic,limits_.leases,budget);
        if(!budget.take(limits_.leases))return result(Reason::BudgetExceeded,limits_.leases,budget);
        // Ownership may have ended while this reset was prepared. Do not erase
        // any retained protection unless the same completed boundary is owned.
        if(!w.currentOwned())return result(Reason::InvalidWorldReady,limits_.leases,budget);
        for(size_t i=0;i<limits_.leases;++i)leases_[i]=Lease{};
        rowCount_=0;lineage_=frame.lineage;session_=frame.at.session;readyEpoch_=frame.at.epoch;
        acceptFrame(frame);return result(Reason::Ready,limits_.leases,budget);
    }

private:
    Result admitPhase(const Frame& frame,const AdmissionRequest& request,NativeDecision native,Mode mode,
                      const OwnedAdmissionWindow* provisional,const ProvisionalToken* finalizing,
                      const OwnedAdmissionWindow* finalOwned=nullptr,InvocationPermit* invocation=nullptr,
                      bool invocationFinalizing=false) noexcept {
        Gate gate(gate_);if(!gate.held)return {Reason::Busy};
        Budget budget{limits_.operations};size_t free=limits_.leases,ownerSlot=ownerSlots_;
        const auto index=owner(finalizing?finalizing->ticket:request.ticket,budget,free,&ownerSlot);
        const auto fail=[&](Reason why){return retainedFailure(why,index,budget);};
        if(rowCount_>limits_.resourceRows||rowCount_>=noRow) {
            budget.invalidate();return fail(Reason::IndexInvalid);
        }
        if(finalizing) {
            if(index==limits_.leases||leases_[index].serial!=finalizing->leaseSerial||
               leases_[index].claim!=*finalizing||leases_[index].ticket!=finalizing->ticket)
                return result(Reason::AttemptMismatch,index,budget);
            auto& lease=leases_[index];
            if(!lease.provisional||lease.entered)return result(Reason::FinalizationRequired,index,budget);
            if(lease.finalizationAttempted&&!invocationFinalizing)return result(Reason::FinalizationRepeated,index,budget);
            if(invocationFinalizing&&!lease.finalizationAttempted)return result(Reason::AttemptMismatch,index,budget);
            // Consume an exact attempt before testing fresh evidence. A failed
            // observation is never retried as a second permission/original call.
            lease.finalizationAttempted=true;
            if(request.ticket!=finalizing->ticket)return fail(Reason::IdentityChanged);
            if(!finalOwned||!finalOwned->valid()||finalOwned->continuity!=finalizing->continuity)
                return fail(Reason::OwnedWindowLost);
            if(finalOwned->publication.at!=request.publication.at||
               finalOwned->publication.window!=request.publication.window||
               finalOwned->publication.lineage!=request.publication.lineage)
                return fail(Reason::DifferentPublication);
            if(frame.lineage!=finalizing->lineage||frame.window!=finalizing->window||frame.at!=finalizing->at)
                return fail(Reason::DifferentPublication);
            if(request.capturedAtMs<finalizing->capturedAtMs)return fail(Reason::StaleCapture);
        }
        // Observe the caller's controls without changing them. Retention is
        // still safe, but unqualified arithmetic must never grant permission.
        if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
        if(budget.used==budget.limit&&index==limits_.leases)return fail(Reason::BudgetExceeded);
        if(const auto why=frameReason(frame,request.publication);why!=Reason::Granted)return fail(why);
        acceptFrame(frame);
        if(frame.at.phase!=rear::Phase::BeforeMovement)return fail(Reason::WrongPhase);
        if(!ticketValid(request.ticket)||request.ticket.session!=session_)return fail(Reason::InvalidTicket);
        if(!request.consistRevision)return fail(Reason::InvalidConsist);
        if(!request.routeComplete)return fail(Reason::IncompleteRoute);
        if(!request.conflictsComplete)return fail(Reason::IncompleteConflicts);
        // A provisional claim protects a complete route without manufacturing
        // physical clearance or script completion. Both are mandatory to pass.
        if(!provisional) {
            if(!request.physicalOccupationComplete)return fail(Reason::IncompleteOccupation);
            if(request.physicalClear!=Proof::Yes)return fail(request.physicalClear==Proof::No?Reason::Occupied:Reason::OccupationUnknown);
            if(!request.scriptVetoComplete)return fail(Reason::IncompleteVeto);
            if(request.scriptAllows!=Proof::Yes)return fail(request.scriptAllows==Proof::No?Reason::ScriptVeto:Reason::VetoUnknown);
        }
        if(request.capturedAtMs>frame.nowMs||frame.nowMs-request.capturedAtMs>limits_.maximumAgeMs)
            return fail(Reason::StaleCapture);
        if(!request.resourceCount||request.resourceCount>maximumRequestPortions||!request.physicalCount||
           request.physicalCount>maximumRequestPortions)return fail(Reason::InvalidIntervals);
        if(!canonical(request.resources.data(),request.resourceCount,budget)||
           !canonical(request.ownPhysical.data(),request.physicalCount,budget))
            return fail(budget.used==budget.limit?Reason::BudgetExceeded:Reason::InvalidIntervals);
        size_t claim=0;
        for(size_t p=0;p<request.physicalCount;++p) {
            const auto& physical=request.ownPhysical[p];
            while(claim<request.resourceCount) {
                if(!budget.take())return fail(Reason::BudgetExceeded);
                const auto& resource=request.resources[claim];
                if(resource.track<physical.track||(resource.track==physical.track&&resource.end<physical.begin))++claim;
                else break;
            }
            if(claim==request.resourceCount||request.resources[claim].track!=physical.track||
               request.resources[claim].begin>physical.begin||request.resources[claim].end<physical.end)
                return fail(Reason::PhysicalNotProtected);
        }
        if(!provisional) {
        if(!native.qualified)return fail(Reason::NativeDecisionUnqualified);
        if(mode==Mode::FinalVeto) {
            if(native.domain!=NativeDomain::FinalAdmission)return fail(Reason::NativeDomainMismatch);
            if(!native.admitted)return fail(Reason::NativeRefusal);
        } else if(mode==Mode::ExclusiveSubstitution) {
            if(native.domain!=NativeDomain::ExclusiveReservationOnly)return fail(Reason::NativeDomainMismatch);
            if(!request.compatibility.complete())return fail(Reason::NativeCompatibilityMissing);
        } else if(mode!=Mode::ParallelAudit)return fail(Reason::NativeDomainMismatch);
        }
        if(index<limits_.leases) {
            auto& lease=leases_[index];
            if(provisional||(lease.provisional&&!finalizing))return fail(Reason::FinalizationRequired);
            if(lease.ticket!=request.ticket)return fail(Reason::RouteChangedRetained);
            if(lease.consistRevision!=request.consistRevision)return fail(Reason::ConsistChangedRetained);
            if(!sameRequest(index,request,budget))return fail(budget.used==budget.limit?Reason::BudgetExceeded:Reason::ResourcesChangedRetained);
            // A lost entry observation cannot become an assertion of non-entry.
            // A fresh complete request may renew the SAME protection only.
            if(request.entered!=Proof::No)return fail(Reason::EntryUnknown);
            if(lease.entered)return fail(Reason::RouteChangedRetained);
            if(finalizing&&(!finalOwned||!finalOwned->valid()))return fail(Reason::OwnedWindowLost);
            if(!request.publication.currentOwned())return fail(Reason::UnqualifiedPhase);
            if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
            lease.uncertain=false;
            if(finalizing)lease.provisional=false;
            auto r=result(mode==Mode::ParallelAudit?Reason::AuditOnly:Reason::Granted,index,budget);
            r.mayPass=mode!=Mode::ParallelAudit;r.replaceNativeExclusive=mode==Mode::ExclusiveSubstitution;return r;
        }
        if(request.entered!=Proof::No)return fail(Reason::EntryUnknown);
        if(free==limits_.leases)return fail(Reason::Capacity);
        if(serial_==std::numeric_limits<uint64_t>::max())return fail(Reason::SerialExhausted);
        if(request.resourceCount+request.physicalCount>limits_.resourceRows-rowCount_)return fail(Reason::Capacity);
        for(size_t p=0;p<request.resourceCount;++p) {
            const auto& resource=request.resources[p];size_t first=0,last=rowCount_;
            while(first<last) {
                if(!budget.take())return fail(Reason::BudgetExceeded);
                const auto middle=first+(last-first)/2;
                if(rows_[middle].interval.track<resource.track)first=middle+1;else last=middle;
            }
            for(size_t r=first;r<rowCount_;++r) {
                if(!budget.take())return fail(Reason::BudgetExceeded);
                const auto& row=rows_[r];
                if(row.lease>=limits_.leases||!leases_[row.lease].live) {
                    budget.invalidate();return fail(Reason::IndexInvalid);
                }
                if(row.interval.track!=resource.track||row.interval.begin>resource.end)break;
                if(row.physical||leases_[row.lease].ticket.train==request.ticket.train)continue;
                if(row.interval.end>=resource.begin) {
                    auto blocked=fail(Reason::Conflict);blocked.blocker=leases_[row.lease].ticket;
                    blocked.track=resource.track;return blocked;
                }
            }
        }
        // Prepare the ENTIRE new index before publishing a lease or permission.
        // Merging two canonical input lists avoids sorting/heap work under gate.
        const size_t added=request.resourceCount+request.physicalCount;
        if(!budget.take(rowCount_+added))return fail(Reason::BudgetExceeded);
        size_t old=0,resource=0,physical=0,out=0;
        while(old<rowCount_||resource<request.resourceCount||physical<request.physicalCount) {
            Row next;bool haveNext=resource<request.resourceCount||physical<request.physicalCount;
            if(haveNext) {
                const bool usePhysical=resource==request.resourceCount||
                    (physical<request.physicalCount&&lessInterval(request.ownPhysical[physical],request.resources[resource]));
                next={usePhysical?request.ownPhysical[physical]:request.resources[resource],static_cast<uint16_t>(free),usePhysical};
            }
            if(old<rowCount_&&(!haveNext||lessRow(rows_[old],next)))prepared_[out++]=rows_[old++];
            else {prepared_[out++]=next;if(next.physical)++physical;else ++resource;}
        }
        // Prepare/validate all cache state before publishing any owner/permission.
        // A failed stage leaves current rows, heads and owner slots untouched.
        if(nextRowEpoch_==UINT64_MAX)return fail(Reason::IndexEpochExhausted);
        // The lookup supplied a local vacancy under this same Gate. SDK
        // ownership callbacks cannot reenter/mutate this authority; still
        // qualify its bounds/state once more before staging or publication.
        if(!budget.take())return fail(Reason::BudgetExceeded);
        if(ownerSlot>=ownerSlots_||(ownerIndex_[ownerSlot].epoch==lineage_&&
           ownerIndex_[ownerSlot].state!=0&&ownerIndex_[ownerSlot].state!=2)) {
            budget.invalidate();return fail(Reason::IndexInvalid);
        }
        uint64_t headsEpoch=0;
        if(!prepareHeads(budget,out,headsEpoch,free,request.resourceCount,request.physicalCount))
            return fail(Reason::BudgetExceeded);
        // From here to publication every operation is noexcept and bounded.
        if(provisional&&!provisional->valid())return fail(Reason::OwnedWindowLost);
        if(!request.publication.currentOwned())return fail(Reason::UnqualifiedPhase);
        if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
        leases_[free]={request.ticket,++serial_,request.consistRevision,true,false,false,false,false,{}};
        leases_[free].resourceCount=static_cast<uint16_t>(request.resourceCount);
        leases_[free].physicalCount=static_cast<uint16_t>(request.physicalCount);
        if(provisional) {
            leases_[free].provisional=true;
            leases_[free].claim={request.ticket,serial_,frame.lineage,frame.window,request.consistRevision,
                provisional->continuity,request.capturedAtMs,frame.at};
            if(invocation) {
                leases_[free].finalizationAttempted=true;
                invocation->claim_=leases_[free].claim;
                invocation->available_=true;
            }
        }
        rows_.swap(prepared_);rowCount_=out;heads_.swap(preparedHeads_);rowEpoch_=headsEpoch;
        ownerIndex_[ownerSlot]={request.ticket.session,request.ticket.train,request.ticket.source,lineage_,static_cast<uint16_t>(free),1};
        auto r=result(provisional?Reason::ProvisionalClaimed:
            mode==Mode::ParallelAudit?Reason::AuditOnly:Reason::Granted,free,budget);
        r.mayPass=!provisional&&mode!=Mode::ParallelAudit;
        r.replaceNativeExclusive=!provisional&&mode==Mode::ExclusiveSubstitution;return r;
    }
    // A post-publication loss retains the claim and still outputs its identity,
    // so callers never mistake protected resources for a missing/free claim.
    Result claimPhase(const Frame& frame,const AdmissionRequest& request,const OwnedAdmissionWindow& owned,
                      ProvisionalToken& token,InvocationPermit* invocation) noexcept {
        token={};
        if(owned.publication.at!=request.publication.at||owned.publication.lineage!=request.publication.lineage||
           owned.publication.window!=request.publication.window||!owned.valid()) {
            auto r=retain(request.ticket);r.reason=Reason::OwnedWindowLost;return r;
        }
        auto r=admitPhase(frame,request,{},Mode::ParallelAudit,&owned,nullptr,nullptr,invocation);
        if(r.reason==Reason::ProvisionalClaimed) {
            token={request.ticket,r.leaseSerial,frame.lineage,frame.window,request.consistRevision,
                owned.continuity,request.capturedAtMs,frame.at};
            if(!owned.valid()){r=retain(request.ticket);r.reason=Reason::OwnedWindowLost;}
        }
        return r;
    }
    Result claimForInvocation(const Frame& frame,const AdmissionRequest& request,const OwnedAdmissionWindow& owned,
                              ProvisionalToken& token,InvocationPermit& permit) noexcept {
        return claimPhase(frame,request,owned,token,&permit);
    }
    Result finalizeForInvocation(const Frame& frame,const AdmissionRequest& request,NativeDecision native,
                                 const OwnedAdmissionWindow& owned,InvocationPermit& permit) noexcept {
        if(!permit.available_) {auto r=retain(permit.claim_.ticket);r.reason=Reason::FinalizationRepeated;return r;}
        // Busy, a lost owner or bad fresh evidence consumes the private permit.
        // No second original, second judge or direct-token fallback is allowed.
        permit.available_=false;
        auto r=admitPhase(frame,request,native,Mode::FinalVeto,nullptr,&permit.claim_,&owned,nullptr,true);
        if(r.mayPass&&!owned.valid()){r=retain(permit.claim_.ticket);r.reason=Reason::OwnedWindowLost;}
        if(r.mayPass&&!rear::detail::arithmeticQualified()){r=retain(permit.claim_.ticket);r.reason=Reason::ArithmeticUnqualified;}
        r.replaceNativeExclusive=false;return r;
    }
public:
    Result admit(const Frame& frame,const AdmissionRequest& request,NativeDecision native,Mode mode) noexcept {
        return admitPhase(frame,request,native,mode,nullptr,nullptr);
    }
    Result claimProvisional(const Frame& frame,const AdmissionRequest& request,
                            const OwnedAdmissionWindow& owned,ProvisionalToken& token) noexcept {
        return claimPhase(frame,request,owned,token,nullptr);
    }
    Result finalizeProvisional(const Frame& frame,const ProvisionalToken& token,const AdmissionRequest& request,
                               NativeDecision native,const OwnedAdmissionWindow& owned) noexcept {
        // Exact matching is separate from current ownership; neither copied
        // metadata nor a different owned phase can renew this immutable token.
        auto r=admitPhase(frame,request,native,Mode::FinalVeto,nullptr,&token,&owned);
        if(r.mayPass&&!owned.valid()){r=retain(token.ticket);r.reason=Reason::OwnedWindowLost;}
        if(r.mayPass&&!rear::detail::arithmeticQualified()){r=retain(token.ticket);r.reason=Reason::ArithmeticUnqualified;}
        r.replaceNativeExclusive=false;return r;
    }
    Result cancelBeforeEntry(const Frame& frame,const NoEntryWitness& witness,
                             const OwnedAdmissionWindow& owned) noexcept {
        if(!owned.valid()){auto r=retain(witness.claim.ticket);r.reason=Reason::OwnedWindowLost;return r;}
        Gate gate(gate_);if(!gate.held)return {Reason::Busy};
        Budget budget{limits_.operations};size_t free;const auto index=owner(witness.claim.ticket,budget,free);
        const auto fail=[&](Reason why){return retainedFailure(why,index,budget);};
        if(index==limits_.leases)return fail(budget.used==budget.limit?Reason::BudgetExceeded:Reason::Missing);
        const auto& lease=leases_[index];
        if(lease.claim!=witness.claim||lease.serial!=witness.claim.leaseSerial||lease.ticket!=witness.claim.ticket)
            return fail(Reason::AttemptMismatch);
        if(!lease.provisional||lease.entered)return fail(Reason::FinalizationRequired);
        if(const auto why=frameReason(frame,witness.publication);why!=Reason::Granted)return fail(why);
        if(frame.at.phase!=rear::Phase::BeforeMovement)return fail(Reason::WrongPhase);
        if(owned.publication.at!=witness.publication.at||owned.publication.lineage!=witness.publication.lineage||
           owned.publication.window!=witness.publication.window)return fail(Reason::DifferentPublication);
        if(witness.capturedAtMs>frame.nowMs||witness.capturedAtMs<witness.claim.capturedAtMs||
           frame.nowMs-witness.capturedAtMs>limits_.maximumAgeMs)return fail(Reason::StaleCapture);
        if(!witness.positiveNoEntry||!witness.originalReturned||!witness.noPermissionOutstanding)
            return fail(Reason::NoEntryUnqualified);
        if(!witness.allEntryWritersObserved||!witness.historyComplete)return fail(Reason::ContinuityUnqualified);
        if(witness.continuity!=witness.claim.continuity||owned.continuity!=witness.continuity||
           witness.fromWindow!=witness.claim.window||witness.throughWindow!=frame.window)
            return fail(Reason::ContinuityChanged);
        if(!budget.take(rowCount_))return fail(Reason::BudgetExceeded);
        size_t count=0;
        if(nextRowEpoch_==UINT64_MAX)return fail(Reason::IndexEpochExhausted);
        const auto ownerSlot=prepareOwner(witness.claim.ticket,budget,true);uint64_t headsEpoch=0;
        if(ownerSlot==ownerSlots_||!prepareRemoval(index,budget,count,headsEpoch))return fail(Reason::BudgetExceeded);
        if(!owned.valid())return fail(Reason::OwnedWindowLost);
        if(!witness.publication.currentOwned())return fail(Reason::UnqualifiedPhase);
        rows_.swap(prepared_);rowCount_=count;leases_[index]=Lease{};
        heads_.swap(preparedHeads_);rowEpoch_=headsEpoch;ownerIndex_[ownerSlot].state=2;
        acceptFrame(frame);return result(Reason::CancelledBeforeEntry,index,budget);
    }
    Result retain(const Ticket& ticket) noexcept {
        Gate gate(gate_);if(!gate.held)return {Reason::Busy};
        Budget budget{limits_.operations};size_t free;
        const auto index=owner(ticket,budget,free);
        if(index==limits_.leases)return result(budget.used==budget.limit?Reason::BudgetExceeded:Reason::Missing,index,budget);
        return retainedFailure(Reason::Retained,index,budget);
    }
    Result commitEntry(const Frame& frame,const EntryWitness& entry) noexcept {
        Gate gate(gate_);if(!gate.held)return {Reason::Busy};
        Budget budget{limits_.operations};size_t free;const auto index=owner(entry.ticket,budget,free);
        if(index==limits_.leases)return result(budget.used==budget.limit?Reason::BudgetExceeded:Reason::Missing,index,budget);
        const auto fail=[&](Reason why){return retainedFailure(why,index,budget);};
        if(const auto why=frameReason(frame,entry.publication);why!=Reason::Granted)return fail(why);
        acceptFrame(frame);auto& lease=leases_[index];
        if(frame.at.phase!=rear::Phase::RetainedMovement)return fail(Reason::WrongPhase);
        if(lease.ticket!=entry.ticket||lease.serial!=entry.leaseSerial)return fail(Reason::SnapshotChanged);
        if(lease.consistRevision!=entry.consistRevision)return fail(Reason::ConsistChangedRetained);
        // pass_by is deliberately insufficient: 448710 can still roll back.
        if(!entry.nativeReturned||!entry.movementCommitted||!entry.retainedMirrorMatches||!entry.identityAndRouteUnchanged)
            return fail(Reason::UnqualifiedEntry);
        if(!entry.publication.currentOwned())return fail(Reason::UnqualifiedPhase);
        lease.entered=true;lease.uncertain=false;return result(Reason::Entered,index,budget);
    }
    Result snapshot(const Ticket& ticket,RetainedSnapshot& output) noexcept {
        Gate gate(gate_);if(!gate.held)return {Reason::Busy};
        Budget budget{limits_.operations};size_t free;const auto index=owner(ticket,budget,free);
        if(index==limits_.leases)return result(budget.used==budget.limit?Reason::BudgetExceeded:Reason::Missing,index,budget);
        const auto& lease=leases_[index];RetainedSnapshot value;
        value.ticket=lease.ticket;value.leaseSerial=lease.serial;value.consistRevision=lease.consistRevision;
        value.entered=lease.entered?Proof::Yes:Proof::Unknown;
        if(!snapshotRows(index,value,budget))return result(Reason::BudgetExceeded,index,budget);
        output=value;return result(Reason::Retained,index,budget);
    }
    Result releaseRear(const Frame& frame,const PublicationWitness& publication,const RetainedSnapshot& retained,
                       bool originalRouteRequalified,const rear::Observation& observation) noexcept {
        // The caller owns immutable observation storage through this call. The
        // complete envelope is assessed OUTSIDE the gate (up to 8192 coverage
        // units + operationLimit overlap checks; a separate operationLimit
        // bounds the gate work). No boolean shortcut,
        // timeout, head-track change or lease expiry can free resources.
        if(retained.resourceCount>maximumRequestPortions||retained.physicalCount>maximumRequestPortions)
            return {Reason::InvalidIntervals};
        // Validate the owner before the expensive immutable coverage assessment,
        // again under the gate, and once more before publishing the removal.
        if(!publication.currentOwned()) {
            auto r=retain(retained.ticket);r.reason=Reason::UnqualifiedPhase;return r;
        }
        engine::interlocking::AggregateClearanceRoute route;
        route.ticket=retained.ticket;route.consistRevision=retained.consistRevision;
        route.entered=retained.entered;route.qualified=originalRouteRequalified;route.qualifiedAt=frame.at;
        route.ownPhysical={retained.ownPhysical.data(),retained.physicalCount};
        route.aliases={retained.resources.data(),retained.resourceCount};
        std::array<rear::Portion,maximumRequestPortions> body;
        engine::interlocking::AggregateClearanceLimits bounds;
        bounds.overlapChecks=limits_.operations;bounds.coverage.outputPortions=maximumRequestPortions;
        const auto clear=engine::interlocking::assessAggregateRearClearance(route,observation,frame.at,body,bounds);
        Gate gate(gate_);if(!gate.held){Result r{Reason::Busy};r.rearClearance=clear;return r;}
        Budget budget{limits_.operations};size_t free;const auto index=owner(retained.ticket,budget,free);
        const auto fail=[&](Reason why){auto r=retainedFailure(why,index,budget);r.rearClearance=clear;return r;};
        if(index==limits_.leases)return fail(budget.used==budget.limit?Reason::BudgetExceeded:Reason::Missing);
        if(const auto why=frameReason(frame,publication);why!=Reason::Granted)return fail(why);
        acceptFrame(frame);const auto& lease=leases_[index];
        if(lease.ticket!=retained.ticket||lease.serial!=retained.leaseSerial||lease.consistRevision!=retained.consistRevision||!lease.entered)
            return fail(Reason::SnapshotChanged);
        if(!identicalRows(index,retained,budget))return fail(budget.used==budget.limit?Reason::BudgetExceeded:Reason::SnapshotChanged);
        if(clear.proof!=Proof::Yes)return fail(clear.proof==Proof::No?Reason::RearOccupied:Reason::RearUnknown);
        if(!budget.take(rowCount_))return fail(Reason::BudgetExceeded);
        size_t count=0;
        if(nextRowEpoch_==UINT64_MAX)return fail(Reason::IndexEpochExhausted);
        const auto ownerSlot=prepareOwner(retained.ticket,budget,true);uint64_t headsEpoch=0;
        if(ownerSlot==ownerSlots_||!prepareRemoval(index,budget,count,headsEpoch))return fail(Reason::BudgetExceeded);
        if(!publication.currentOwned())return fail(Reason::UnqualifiedPhase);
        rows_.swap(prepared_);rowCount_=count;leases_[index]=Lease{};
        heads_.swap(preparedHeads_);rowEpoch_=headsEpoch;ownerIndex_[ownerSlot].state=2;
        auto r=result(Reason::Released,index,budget);r.rearClearance=clear;return r;
    }

    // Wrappers are fixtures for an eventual ABI adapter, not game hooks. The
    // original is invoked exactly once OUTSIDE the gate, even on contention or
    // unready world. Capture/revalidation also runs once outside the gate and
    // only after native return (which may have changed the route/script result).
    template<class Original,class Capture>
    Result invoke(const Frame& frame,const Ticket& ticket,Mode mode,Original&& original,Capture&& capture) noexcept {
        NativeDecision native;
        try {native=std::forward<Original>(original)();}
        catch(const std::bad_alloc&) {auto r=retain(ticket);r.reason=Reason::AllocationFailure;return r;}
        catch(...) {auto r=retain(ticket);r.reason=Reason::OriginalException;return r;}
        try {
            const auto request=std::forward<Capture>(capture)(native);
            if(request.ticket!=ticket){auto r=retain(ticket);r.reason=Reason::IdentityChanged;return r;}
            return admit(frame,request,native,mode);
        } catch(const std::bad_alloc&) {auto r=retain(ticket);r.reason=Reason::AllocationFailure;return r;}
        catch(...) {auto r=retain(ticket);r.reason=Reason::CaptureException;return r;}
    }
};
} // namespace nimby::windows::admission
