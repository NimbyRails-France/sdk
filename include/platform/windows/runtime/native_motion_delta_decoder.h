#pragma once
#include <platform/windows/runtime/native_consist_inventory.h>

namespace nimby::windows::motion_delta {

// PRIVATE, hash-pinned Windows 1.19.10 decoder for the detached input of
// 478B20. Capture BEFORE calling the original: it moves, zeros and frees all
// four vectors. CALL 34986C returns at 349871, still under Controller+4C0
// Shared. Ownership here concerns ONLY that caller's detached packet, never
// exclusive DB/Sim ownership, a local finished step or historical progression.
//
// 478820/4785A0 snapshot another Sim in 4F3520. A matching sequence/full ID
// cannot turn a received row into an observed local 448710 completion. 450420
// replaces the full Motion, including Dynamics, CarSetup, parent/attachment,
// retained Presence/history and Path. There is no movement-only discriminator.
inline constexpr uintptr_t callerReturn=0x349871;
inline constexpr size_t motionStride=0x638,statisticsStride=0x40;
inline constexpr size_t record178Stride=0x178,record48Stride=0x48;
inline constexpr size_t maximumInputRows=8192,maximumVectorRows=4096;
using Budget=consist_inventory::Budget;

struct Triplet {
    uintptr_t begin{},end{},capacity{};
    bool operator==(const Triplet&) const = default;
};
struct Header {
    uint64_t unknown{},sequence{};
    Triplet motions,statistics,records178,records48;
    bool operator==(const Header&) const = default;
};
static_assert(sizeof(Triplet)==0x18&&sizeof(Header)==0x70);
static_assert(offsetof(Header,motions)==0x10&&offsetof(Header,statistics)==0x28&&
    offsetof(Header,records178)==0x40&&offsetof(Header,records48)==0x58);
static_assert(std::is_trivially_copyable_v<Header>);

enum class Boundary:uint8_t { BeforeMerge };
struct Witness {
    uintptr_t controller{},simulation{},delta{};
    uint64_t thread{},world{},serial{};
    uintptr_t returnOffset{};
    Boundary boundary=Boundary::BeforeMerge;
    bool detachedInputOwned=false,originalNotStarted=false;
    bool operator==(const Witness&) const = default;
};
// The production caller validates its real current-thread TLS ancestry,
// matching controller/world/Sim and lifetime of the detached stack packet.
// Repeated header reads, Shared ancestry alone and input booleans cannot
// manufacture this ownership. This decoder acquires no native or SDK lock.
struct Scope {
    Witness witness;
    bool (*current)(void*,const Witness&) noexcept{};
    void* context{};
    bool valid()const noexcept {
        using consist_inventory::detail::address;
        return witness.boundary==Boundary::BeforeMerge&&witness.returnOffset==callerReturn&&
            witness.thread&&witness.world&&witness.serial&&witness.detachedInputOwned&&
            witness.originalNotStarted&&address(witness.controller,0x5d0,true)&&
            address(witness.simulation,0x70,true)&&address(witness.delta,sizeof(Header),true)&&
            current&&current(context,witness);
    }
};
enum Effect:uint8_t { FullMotion=1,Statistics=2 };
struct AffectedTrain {
    uint64_t fullId{};
    uint32_t slot{};
    uint8_t effects{};
    bool operator==(const AffectedTrain&) const = default;
    bool motionMayBeReplaced()const noexcept {return (effects&FullMotion)!=0;}
    // 478B20 can destroy the old generation at this slot before inserting the
    // incoming full ID. Retain/revoke claims for ALL generations of that slot,
    // plus affected consist parents/children, not merely this incoming full ID.
    bool slotGenerationMayBeReplaced()const noexcept {return motionMayBeReplaced();}
};
static_assert(std::is_trivially_copyable_v<AffectedTrain>);
enum class Reason:uint8_t {
    Ready,UnqualifiedProfile,UnqualifiedScope,InvalidLimits,InvalidStorage,
    StorageOverlapsInput,InvalidBudget,PreflightRefused,ReadLimit,ByteLimit,
    WorkLimit,Unreadable,ReaderException,InvalidTriplet,InputAllocationsOverlap,VectorCapacity,
    InputRowCapacity,OutputCapacity,InvalidTrainId,HeaderChanged,ScopeChanged
};
struct Limits {
    size_t vectorRows=4096,inputRows=8192,reads=16384,bytes=256u*1024u,work=262144;
    size_t vectorAllocationBytes=32u*1024u*1024u;
};
struct Result {
    Reason reason=Reason::UnqualifiedProfile;
    Witness witness;
    uint64_t sequence{}; // Foreign packet label, deliberately not a local tick.
    size_t trains{},motionRows{},statisticsRows{},record178Rows{},record48Rows{};
    size_t reads{},bytes{},work{};
    bool unresolvedIndirectEffects=false;
    bool ready()const noexcept {return reason==Reason::Ready;}
    // A successful result lists potentially overwritten input identities.
    // It does not say which generations native code accepted or that a merge
    // completed, nor certify forward movement, occupancy or rear clearance.
    static constexpr bool localStepProvenance=false;
    static constexpr bool exclusiveWorldOwnership=false;
    static constexpr bool nativeMergeCompleted=false;
    static constexpr bool progressionQualified=false;
    static constexpr bool rearClearQualified=false;
};
static_assert(std::is_trivially_copyable_v<Result>);

namespace detail {
inline bool less(const AffectedTrain& a,const AffectedTrain& b)noexcept {
    return a.slot<b.slot||(a.slot==b.slot&&a.fullId<b.fullId);
}
inline bool trainId(uint64_t id)noexcept {
    // Index zero is a legitimate native slot; retain all 16 generation bits.
    return id>>48==5;
}
inline bool overlap(uintptr_t begin,uintptr_t end,const Triplet& vector)noexcept {
    return consist_inventory::detail::overlaps(begin,end,vector.begin,vector.capacity);
}
}

// Reader: bool(uintptr_t nativeAddress, void* ownedDestination, size_t bytes).
// All native reads use the shared terminal budget. Work, reads, bytes and
// accepted storage are bounded; no heap allocation, wait, native invocation,
// RPC, mod callback or publication occurs here. The caller owns output and
// scratch disjoint from each other and EVERY input allocation. On failure,
// output is unchanged and trains=0; scratch is unusable. On success, output is
// a single non-refusable bounded POD copy after the final scope/budget check.
template<class Read> Result capture(const Scope& scope,bool profileQualified,Read&& reader,
    Budget& budget,std::span<AffectedTrain> output,std::span<AffectedTrain> scratch,
    Limits limits={})noexcept {
    using consist_inventory::detail::address;
    Result result;Reason failure=Reason::PreflightRefused;
    const auto fail=[&](Reason why){result.reason=why;result.trains=0;return result;};
    if(!profileQualified)return fail(Reason::UnqualifiedProfile);
    if(!scope.valid())return fail(Reason::UnqualifiedScope);
    if(!limits.vectorRows||limits.vectorRows>maximumVectorRows||!limits.inputRows||
        limits.inputRows>maximumInputRows||!limits.reads||limits.reads>16384||
        limits.bytes<2*sizeof(Header)||limits.bytes>4u*1024u*1024u||
        !limits.work||limits.work>524288||!limits.vectorAllocationBytes||
        limits.vectorAllocationBytes>128u*1024u*1024u)return fail(Reason::InvalidLimits);
    if(output.empty()||scratch.empty()||output.size()>maximumInputRows||
        scratch.size()>maximumInputRows)return fail(Reason::InvalidStorage);
    uintptr_t o0{},o1{},s0{},s1{};
    if(!consist_inventory::detail::range(output.data(),output.size(),sizeof(AffectedTrain),o0,o1)||
        !consist_inventory::detail::range(scratch.data(),scratch.size(),sizeof(AffectedTrain),s0,s1)||
        consist_inventory::detail::overlaps(o0,o1,s0,s1))return fail(Reason::InvalidStorage);
    if(!budget.preflight)return fail(Reason::InvalidBudget);
    const auto available=[&]{
        if(!budget.available()){failure=Reason::PreflightRefused;return false;}return true;
    };
    const auto work=[&](size_t amount=1){
        if(amount>limits.work-result.work){budget.exhausted=true;failure=Reason::WorkLimit;return false;}
        const auto previous=result.work;result.work+=amount;
        if(previous/32!=result.work/32||previous==0)return available();
        return true;
    };
    const auto read=[&](uintptr_t at,void* destination,size_t bytes){
        if(result.reads==limits.reads){budget.exhausted=true;failure=Reason::ReadLimit;return false;}
        if(bytes>limits.bytes-result.bytes){budget.exhausted=true;failure=Reason::ByteLimit;return false;}
        if(!address(at,bytes,true)){failure=Reason::Unreadable;return false;}
        if(!scope.valid()){failure=Reason::ScopeChanged;return false;}
        if(!budget.available(bytes)){failure=Reason::PreflightRefused;return false;}
        ++result.reads;result.bytes+=bytes;
        try {if(!reader(at,destination,bytes)){failure=Reason::Unreadable;return false;}}
        catch(...){failure=Reason::ReaderException;return false;}
        return true;
    };
    Header header;
    if(!read(scope.witness.delta,&header,sizeof header))return fail(failure);
    result.sequence=header.sequence;
    const std::array<Triplet,4> vectors{{header.motions,header.statistics,header.records178,header.records48}};
    const std::array<size_t,4> strides{{motionStride,statisticsStride,record178Stride,record48Stride}};
    std::array<size_t,4> counts{};
    for(size_t i=0;i<vectors.size();++i){
        if(!work())return fail(failure);
        const auto& v=vectors[i];
        if(v.begin==0){if(v.end||v.capacity)return fail(Reason::InvalidTriplet);continue;}
        if(v.begin>v.end||v.end>v.capacity||!address(v.begin,0,true)||
            !address(v.capacity,0,true)||(v.end-v.begin)%strides[i]||
            (v.capacity-v.begin)%strides[i])return fail(Reason::InvalidTriplet);
        const auto allocated=v.capacity-v.begin;
        if(allocated>limits.vectorAllocationBytes)return fail(Reason::VectorCapacity);
        counts[i]=(v.end-v.begin)/strides[i];
        if(counts[i]>limits.vectorRows)return fail(Reason::VectorCapacity);
    }
    result.motionRows=counts[0];result.statisticsRows=counts[1];
    result.record178Rows=counts[2];result.record48Rows=counts[3];
    // These vectors address distinct Sim+40/Sim+70 objects. Their downstream
    // effects on train routes are not qualified by this Motion-ID decoder.
    result.unresolvedIndirectEffects=counts[2]!=0||counts[3]!=0;
    if(counts[0]>limits.inputRows||counts[1]>limits.inputRows-counts[0]||
        counts[0]+counts[1]>scratch.size())return fail(Reason::InputRowCapacity);
    const auto headerEnd=scope.witness.delta+sizeof(Header);
    if(consist_inventory::detail::overlaps(o0,o1,scope.witness.delta,headerEnd)||
        consist_inventory::detail::overlaps(s0,s1,scope.witness.delta,headerEnd))
        return fail(Reason::StorageOverlapsInput);
    for(const auto& v:vectors){
        if(detail::overlap(scope.witness.delta,headerEnd,v))return fail(Reason::InputAllocationsOverlap);
        if(detail::overlap(o0,o1,v)||detail::overlap(s0,s1,v))return fail(Reason::StorageOverlapsInput);
    }
    for(size_t a=0;a<vectors.size();++a){
        for(size_t b=a+1;b<vectors.size();++b){
            if(consist_inventory::detail::overlaps(vectors[a].begin,vectors[a].capacity,
                vectors[b].begin,vectors[b].capacity))return fail(Reason::InputAllocationsOverlap);
        }
    }
    size_t used=0;
    for(size_t group=0;group<2;++group){
        for(size_t row=0;row<counts[group];++row){
            if(!work())return fail(failure);
            uint64_t id{};
            if(!read(vectors[group].begin+row*strides[group],&id,sizeof id))return fail(failure);
            if(!detail::trainId(id))return fail(Reason::InvalidTrainId);
            scratch[used++]={id,static_cast<uint32_t>((id>>16)&0xffffffffULL),
                static_cast<uint8_t>(group==0?FullMotion:Statistics)};
        }
    }
    // In-place heapsort: explicit accounting bounds the complete worst-case
    // comparison/swap work, including adversarial duplicate and reverse IDs.
    const auto sift=[&](size_t first,size_t end){
        while(first<end/2){
            if(!work())return false;
            size_t child=first*2+1;
            if(child+1<end&&detail::less(scratch[child],scratch[child+1]))++child;
            if(!detail::less(scratch[first],scratch[child]))break;
            std::swap(scratch[first],scratch[child]);first=child;
        }
        return true;
    };
    for(size_t n=used/2;n;){--n;if(!sift(n,used))return fail(failure);}
    for(size_t n=used;n>1;){
        if(!work())return fail(failure);
        std::swap(scratch[0],scratch[--n]);if(!sift(0,n))return fail(failure);
    }
    size_t unique=0;
    for(size_t n=0;n<used;++n){
        if(!work())return fail(failure);
        if(unique&&scratch[unique-1].fullId==scratch[n].fullId)
            scratch[unique-1].effects|=scratch[n].effects;
        else scratch[unique++]=scratch[n];
    }
    if(unique>output.size())return fail(Reason::OutputCapacity);
    Header again;
    if(!read(scope.witness.delta,&again,sizeof again))return fail(failure);
    if(header!=again)return fail(Reason::HeaderChanged);
    if(!scope.valid())return fail(Reason::ScopeChanged);
    if(!available())return fail(failure);
    // Even an empty delta spends no permission or movement proof. Ancillary
    // effects remain explicit; output addresses never escape this operation.
    if(unique)std::memcpy(output.data(),scratch.data(),unique*sizeof(AffectedTrain));
    result.trains=unique;result.witness=scope.witness;result.reason=Reason::Ready;return result;
}
} // namespace nimby::windows::motion_delta
