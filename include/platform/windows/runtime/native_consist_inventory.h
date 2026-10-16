#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>
#include <type_traits>
#include <utility>

namespace nimby::windows::consist_inventory {
static_assert(sizeof(uintptr_t)==8,"The private native Motion profile requires 64-bit pointers");

// PRIVATE DIAGNOSTIC ADAPTER for the hash-identified Windows 1.19.10 profile.
// Evidence: .validation/native-consist-inventory-20261010/evidence.json.
// 461CC0 constructs 512 Motion slots/block, stride 0x638, in a 1 MiB allocation;
// 45ECE0 visits ALL +18..+20 block pointers and skips null blocks, then copies
// every tag5 slot. 45F300 requires the whole ID, including generation, and the
// ID's index must equal blockOrdinal*512 + slotOrdinal. We DO NOT invoke these
// native functions (45ECE0 allocates and invokes native copy/destructor code).
//
// 441070 writes the CHILD's parent/order/reversal/attached fields +1D8..+1F0.
// 45CD50 normalization precedes commands; 44CF80 can omit children. Therefore
// +88 is deliberately NEVER read as an exhaustive parent-to-child inventory.
// No active/running/scheduled filter can exclude a slot from this inventory.
//
// Equal readbacks detect local changes only. They do NOT exclude all writers,
// prove a world/session lineage, or certify current composition/rear clearance.
inline constexpr uint32_t noRow=UINT32_MAX;
struct Row {
    uint64_t train{},parent{};
    int64_t couplingOrder{};
    int32_t nativeMode{}; // Raw +80; never clamp an unresolved native mode.
    uint32_t parentRow=noRow,firstChild=noRow,nextSibling=noRow,childCount{};
    uint8_t active{},attached{},reversed{},running{},scheduled{};
    bool operator==(const Row&) const = default;
};
static_assert(std::is_trivially_copyable_v<Row> && std::is_standard_layout_v<Row>);
enum class Status : uint8_t { Unknown, ObservedAllSlots };
enum class Reason : uint8_t {
    ObservedAllSlots,UnqualifiedProfile,InvalidLimits,InvalidBuffers,
    OverlappingBuffers,InvalidBudget,PreflightRefused,ReadLimit,ByteLimit,WorkLimit,
    InvalidAddress,Unreadable,ReadException,InvalidPoolLayout,PoolCapacity,
    SlotCapacity,RowCapacity,UnexpectedTag,IdentityOrdinalMismatch,
    PoolChanged,SlotChanged,ValuesChanged,InvalidParent,MissingParent,
    ParentGenerationMismatch,SelfParent,CouplingCycle,UnqualifiedOwnership,OwnershipChanged,UnsupportedCouplingChain
};
inline const char* reasonName(Reason reason) noexcept {
    switch(reason) {
    case Reason::ObservedAllSlots:return "all_slots_locally_observed";
    case Reason::UnqualifiedProfile:return "unqualified_binary_profile";
    case Reason::InvalidLimits:return "invalid_limits";
    case Reason::InvalidBuffers:return "invalid_caller_storage";
    case Reason::OverlappingBuffers:return "overlapping_caller_storage";
    case Reason::InvalidBudget:return "missing_shared_preflight";
    case Reason::PreflightRefused:return "shared_budget_refused";
    case Reason::ReadLimit:return "read_limit";
    case Reason::ByteLimit:return "byte_limit";
    case Reason::WorkLimit:return "work_limit";
    case Reason::InvalidAddress:return "invalid_address";
    case Reason::Unreadable:return "unreadable";
    case Reason::ReadException:return "reader_exception";
    case Reason::InvalidPoolLayout:return "invalid_motion_pool_layout";
    case Reason::PoolCapacity:return "motion_block_capacity";
    case Reason::SlotCapacity:return "motion_slot_capacity";
    case Reason::RowCapacity:return "motion_row_capacity";
    case Reason::UnexpectedTag:return "unexpected_slot_tag";
    case Reason::IdentityOrdinalMismatch:return "identity_slot_ordinal_mismatch";
    case Reason::PoolChanged:return "pool_readback_changed";
    case Reason::SlotChanged:return "slot_identity_readback_changed";
    case Reason::ValuesChanged:return "coupling_values_readback_changed";
    case Reason::InvalidParent:return "invalid_parent_identity";
    case Reason::MissingParent:return "parent_not_in_observed_pool";
    case Reason::ParentGenerationMismatch:return "parent_generation_mismatch";
    case Reason::SelfParent:return "self_parent";
    case Reason::CouplingCycle:return "coupling_cycle";
    case Reason::UnqualifiedOwnership:return "missing_actual_exclusive_scope";
    case Reason::OwnershipChanged:return "exclusive_scope_or_lineage_changed";
    case Reason::UnsupportedCouplingChain:return "unqualified_nested_coupling";
    }
    return "unknown";
}
struct Limits {
    // Structural/storage caps, not a promise that a full scan fits. In
    // particular 8192 slots require >16384 ID reads for the two traversals,
    // exceeding reads=8192 even without metadata or caller's previous reads.
    // A shared caller budget can refuse earlier than any of these local caps.
    size_t blocks=16,slots=8192,rows=1024,reads=8192,bytes=4u*1024u*1024u,work=65536;
};
struct Result {
    Status status=Status::Unknown;
    Reason reason=Reason::UnqualifiedProfile;
    size_t rows{},blocks{},nonnullBlocks{},visitedSlots{},reads{},bytes{},work{};
    uint64_t train{},parent{}; // Owned full IDs for a refused relationship.
    uint64_t observedTrain{}; // SlotChanged: expected train versus actual reread ID.
    size_t ordinal{};
    // True only after this producer checked strict ordinal order, all full-ID
    // parent relations, cycles and the complete inverse-child index. It is not
    // ownership: an external owned publication must seal these output rows.
    bool relationshipsIndexed=false;
    bool ownedPublication=false;
    bool observedAllSlots() const noexcept {return status==Status::ObservedAllSlots;}
    // These are explicit, immutable contract flags, including on success.
    static constexpr bool ownershipQualifier=false;
    static constexpr bool universalWriterExclusion=false;
    static constexpr bool completeConsistCertified=false;
    static constexpr bool rearClearCertified=false;
};
// Caller-owned temporary storage. IDs include empty/tombstone values so the
// second traversal compares EVERY visited slot, not a collision-prone hash.
// Only prefixes actually read are initialized. No native address is retained.
struct Scratch {std::span<Row> rows;std::span<uint64_t> slotIds;};
// PRIVATE contract for the native PostSharedExclusive owner. The caller holds
// the real, hash-pinned local Controller lock on the matching runner thread for
// this entire capture and seals its current world/phase token in validate().
// A double read, timestamp, ancestry under Shared or boolean ready flag cannot
// supply this contract. No native/game callbacks may mutate the input here.
struct OwnedScope {
    bool (*validate)(void*) noexcept{};
    void* context{};
    bool valid()const noexcept{return validate&&validate(context);}
};
struct Budget {
    using Preflight=bool (*)(void*,size_t) noexcept;
    Preflight preflight{};
    void* context{};
    bool exhausted=false;
    // Positive bytes preflight/account ONE impending read (RouteReadBudget
    // allow); zero bytes checks CPU time only (RouteReadBudget available).
    // A zero-byte preflight NEVER calls the reader or accounts a native read.
    bool available(size_t bytes=0) noexcept {
        if(exhausted)return false;
        if(!preflight||!preflight(context,bytes)){exhausted=true;return false;}
        return true;
    }
};
namespace detail {
inline constexpr size_t maximumBlocks=128,maximumSlots=65536,maximumRows=4096;
inline constexpr uintptr_t addressLimit=0x7fffffff0000ULL;
inline constexpr size_t stride=0x638,slotsPerBlock=512,allocationBytes=0x100000;
template<class T> T field(const auto& bytes,size_t at) noexcept {
    T value{};std::memcpy(&value,bytes.data()+at,sizeof value);return value;
}
inline bool address(uintptr_t value,size_t bytes,bool aligned=false) noexcept {
    return value>=0x10000&&value<addressLimit&&bytes<=addressLimit-value&&
        (!aligned||value%8==0);
}
inline bool fullTrain(uint64_t id) noexcept {return id>>48==5;}
inline uint64_t index(uint64_t id) noexcept {return (id>>16)&0xffffffffULL;}
inline bool range(const void* data,size_t count,size_t stride,uintptr_t& begin,uintptr_t& end) noexcept {
    begin=reinterpret_cast<uintptr_t>(data);
    if(count>std::numeric_limits<size_t>::max()/stride)return false;
    const auto bytes=count*stride;
    if(bytes&&(!begin||bytes>std::numeric_limits<uintptr_t>::max()-begin))return false;
    end=begin+bytes;return true;
}
inline bool overlaps(uintptr_t a,uintptr_t b,uintptr_t c,uintptr_t d) noexcept {
    return a<b&&c<d&&a<d&&c<b;
}
}

// Reader signature: bool(uintptr_t address, void* ownedTarget, size_t bytes).
// Caller qualifies the binary/layout and supplies its shared TERMINAL budget.
// This function grants no permission to dereference foreign storage directly.
// A normal native fault is Unreadable; a denied preflight is terminal and never
// causes another read. All long CPU loops have periodic preflight checks.
//
// On any Unknown result, output is UNCHANGED and rows=0; scratch is unusable.
// Output and both scratch ranges must be disjoint. On success, a final budget
// check precedes ONE non-refusable bounded POD copy. No native read/preflight
// can subsequently turn that committed output into a refused partial result.
// This is cooperative, not a hard deadline or protection from ABA writers.
template<class Read> Result captureMotionInventoryImpl(uintptr_t simulation,bool profileQualified,
    Read&& read,Budget& budget,std::span<Row> output,Scratch scratch,Limits limits,const OwnedScope* owned) noexcept {
    using namespace detail;
    Result result;
    const auto fail=[&](Reason reason){result.reason=reason;result.rows=0;result.relationshipsIndexed=false;result.ownedPublication=false;return result;};
    if(!profileQualified)return fail(Reason::UnqualifiedProfile);
    if(owned&&!owned->valid())return fail(Reason::UnqualifiedOwnership);
    if(!limits.blocks||limits.blocks>maximumBlocks||!limits.slots||limits.slots>maximumSlots||
       !limits.rows||limits.rows>maximumRows||!limits.reads||limits.reads>8192||
       !limits.bytes||limits.bytes>4u*1024u*1024u||!limits.work||limits.work>262144)
        return fail(Reason::InvalidLimits);
    if(output.empty()||scratch.rows.empty()||output.size()>maximumRows||scratch.rows.size()>maximumRows||
       scratch.slotIds.size()>maximumSlots)return fail(Reason::InvalidBuffers);
    uintptr_t o0{},o1{},r0{},r1{},i0{},i1{};
    if(!range(output.data(),output.size(),sizeof(Row),o0,o1)||
       !range(scratch.rows.data(),scratch.rows.size(),sizeof(Row),r0,r1)||
       !range(scratch.slotIds.data(),scratch.slotIds.size(),sizeof(uint64_t),i0,i1))
        return fail(Reason::InvalidBuffers);
    if(overlaps(o0,o1,r0,r1)||overlaps(o0,o1,i0,i1)||overlaps(r0,r1,i0,i1))
        return fail(Reason::OverlappingBuffers);
    if(!budget.preflight)return fail(Reason::InvalidBudget);
    if(!address(simulation,0xa0+48,true))return fail(Reason::InvalidAddress);
    const size_t rowCapacity=std::min({limits.rows,output.size(),scratch.rows.size()});
    Reason failure=Reason::PreflightRefused;
    const auto available=[&]{
        if(!budget.available()){failure=Reason::PreflightRefused;return false;}
        return true;
    };
    const auto work=[&]{
        if(result.work==limits.work){budget.exhausted=true;failure=Reason::WorkLimit;return false;}
        if(result.work++%32==0&&!available())return false;
        return true;
    };
    const auto copy=[&](uintptr_t addressValue,void* target,size_t bytes){
        if(result.reads==limits.reads){budget.exhausted=true;failure=Reason::ReadLimit;return false;}
        if(bytes>limits.bytes-result.bytes){budget.exhausted=true;failure=Reason::ByteLimit;return false;}
        if(!budget.available(bytes)){failure=Reason::PreflightRefused;return false;}
        ++result.reads;result.bytes+=bytes;
        if(!address(addressValue,bytes)){failure=Reason::InvalidAddress;return false;}
        if(!read(addressValue,target,bytes)){failure=Reason::Unreadable;return false;}
        return true;
    };
    try {
        if(!available())return fail(failure);
        std::array<unsigned char,48> header,again;
        if(!copy(simulation+0xa0,header.data(),header.size()))return fail(failure);
        if(field<uint32_t>(header,0)!=allocationBytes||field<uint32_t>(header,4)!=9||
           field<uint32_t>(header,8)!=slotsPerBlock||field<uint32_t>(header,12)!=slotsPerBlock*stride||
           field<uint32_t>(header,16)!=slotsPerBlock-1)return fail(Reason::InvalidPoolLayout);
        const auto begin=field<uintptr_t>(header,24),end=field<uintptr_t>(header,32),cap=field<uintptr_t>(header,40);
        if(end<begin||cap<end||(end-begin)%8||(cap-begin)%8||
           ((begin||end||cap)&&(!address(begin,cap-begin,true)||end%8||cap%8)))
            return fail(Reason::InvalidPoolLayout);
        const auto blockCount=static_cast<size_t>((end-begin)/8);
        if(blockCount>limits.blocks)return fail(Reason::PoolCapacity);
        result.blocks=blockCount;
        const auto slotCount=blockCount*slotsPerBlock; // blockCount already <=128.
        if(slotCount>limits.slots||slotCount>scratch.slotIds.size())return fail(Reason::SlotCapacity);
        std::array<uintptr_t,maximumBlocks> blocks,blocksAgain;
        const size_t blockBytes=blockCount*sizeof(uintptr_t);
        if(blockBytes&&!copy(begin,blocks.data(),blockBytes))return fail(failure);
        for(size_t b=0;b<blockCount;++b){
            if(!work())return fail(failure);
            if(!blocks[b])continue;
            if(!address(blocks[b],allocationBytes,true))return fail(Reason::InvalidAddress);
            ++result.nonnullBlocks;
        }
        const auto values=[&](uintptr_t motion,uint64_t id,Row& row){
            std::array<unsigned char,0x21> coupling,activity;
            int32_t mode{};
            if(!copy(motion+0x1d0,coupling.data(),coupling.size())||
               !copy(motion+0x80,&mode,sizeof mode)||
               !copy(motion+0x5d0,activity.data(),activity.size()))return false;
            row={};row.train=id;row.parent=field<uint64_t>(coupling,8);
            row.couplingOrder=field<int64_t>(coupling,16);row.nativeMode=mode;
            row.active=coupling[0];row.reversed=coupling[0x18];row.attached=coupling[0x20];
            row.running=activity[0];row.scheduled=activity[0x20];return true;
        };
        size_t rows=0;
        for(size_t b=0;b<blockCount;++b){
            if(!blocks[b])continue;
            for(size_t s=0;s<slotsPerBlock;++s){
                if(!work())return fail(failure);
                const auto ordinal=b*slotsPerBlock+s;
                const auto motion=blocks[b]+s*stride;
                uint64_t id{};if(!copy(motion,&id,sizeof id))return fail(failure);
                ++result.visitedSlots;scratch.slotIds[ordinal]=id;
                const auto tag=id>>48;
                if(tag==0||tag==0xffff)continue;
                result.train=id;result.ordinal=ordinal;
                if(tag!=5)return fail(Reason::UnexpectedTag);
                if(index(id)!=ordinal)return fail(Reason::IdentityOrdinalMismatch);
                if(rows==rowCapacity)return fail(Reason::RowCapacity);
                if(!values(motion,id,scratch.rows[rows]))return fail(failure);
                ++rows;
            }
        }
        const auto poolReadback=[&]{
            if(!copy(simulation+0xa0,again.data(),again.size()))return false;
            if(again!=header){failure=Reason::PoolChanged;return false;}
            if(blockBytes&&!copy(begin,blocksAgain.data(),blockBytes))return false;
            if(!std::equal(blocks.begin(),blocks.begin()+blockCount,blocksAgain.begin())){
                failure=Reason::PoolChanged;return false;
            }
            return true;
        };
        if(!owned){
            if(!poolReadback())return fail(failure);
            size_t row=0;
            for(size_t b=0;b<blockCount;++b){
                if(!blocks[b])continue;
                for(size_t s=0;s<slotsPerBlock;++s){
                    if(!work())return fail(failure);
                    const auto ordinal=b*slotsPerBlock+s;
                    const auto motion=blocks[b]+s*stride;
                    uint64_t id{};if(!copy(motion,&id,sizeof id))return fail(failure);
                    if(id!=scratch.slotIds[ordinal]){
                        result.train=scratch.slotIds[ordinal];result.observedTrain=id;
                        result.parent=0;result.ordinal=ordinal;return fail(Reason::SlotChanged);
                    }
                    if(id>>48!=5)continue;
                    Row confirmed;if(!values(motion,id,confirmed))return fail(failure);
                    if(row>=rows||confirmed!=scratch.rows[row]){
                        result.train=id;result.parent=confirmed.parent;result.ordinal=ordinal;
                        return fail(Reason::ValuesChanged);
                    }
                    ++row;
                }
            }
            if(row!=rows)return fail(Reason::SlotChanged);
        }
        if(!available())return fail(failure);
        // Rows are already ordered by slot index. Find parents by index, then
        // compare the FULL ID, so a generation-recycled parent is never used.
        for(size_t n=0;n<rows;++n){
            if(!work())return fail(failure);
            auto& child=scratch.rows[n];
            if(!fullTrain(child.train)||(n&&index(scratch.rows[n-1].train)>=index(child.train)))
                return fail(Reason::IdentityOrdinalMismatch);
            if(!child.attached)continue; // Stale parent/order after unhitch is raw data.
            result.train=child.train;result.parent=child.parent;result.ordinal=static_cast<size_t>(index(child.train));
            if(!fullTrain(child.parent))return fail(Reason::InvalidParent);
            if(child.parent==child.train)return fail(Reason::SelfParent);
            size_t low=0,high=rows;const auto parentIndex=index(child.parent);
            while(low<high){
                if(!work())return fail(failure);
                const auto mid=low+(high-low)/2;
                if(index(scratch.rows[mid].train)<parentIndex)low=mid+1;else high=mid;
            }
            if(low==rows||index(scratch.rows[low].train)!=parentIndex)return fail(Reason::MissingParent);
            if(scratch.rows[low].train!=child.parent)return fail(Reason::ParentGenerationMismatch);
            child.parentRow=static_cast<uint32_t>(low);
        }
        // Linear parent-chain coloring, not O(N^2) walks or a hash allocation.
        std::array<uint8_t,maximumRows> colors;
        for(size_t n=0;n<rows;++n){if(!work())return fail(failure);colors[n]=0;}
        for(size_t n=0;n<rows;++n){
            if(!work())return fail(failure);
            size_t cursor=n;
            while(colors[cursor]==0){
                if(!work())return fail(failure);
                colors[cursor]=1;
                if(scratch.rows[cursor].parentRow==noRow)break;
                cursor=scratch.rows[cursor].parentRow;
            }
            if(colors[cursor]==1&&scratch.rows[cursor].parentRow!=noRow){
                result.train=scratch.rows[cursor].train;result.parent=scratch.rows[cursor].parent;
                result.ordinal=static_cast<size_t>(index(result.train));return fail(Reason::CouplingCycle);
            }
            cursor=n;
            while(colors[cursor]==1){
                if(!work())return fail(failure);
                colors[cursor]=2;
                if(scratch.rows[cursor].parentRow==noRow)break;
                cursor=scratch.rows[cursor].parentRow;
            }
        }
        for(size_t n=0;n<rows;++n){
            if(!work())return fail(failure);
            auto& child=scratch.rows[n];
            if(child.parentRow==noRow)continue;
            auto& parent=scratch.rows[child.parentRow];
            // The studied normalization supports stars. Preserve uncertainty
            // rather than assigning physical membership to an unproved chain.
            if(parent.attached){
                result.train=child.train;result.parent=child.parent;result.ordinal=static_cast<size_t>(index(child.train));
                return fail(Reason::UnsupportedCouplingChain);
            }
            child.nextSibling=parent.firstChild;parent.firstChild=static_cast<uint32_t>(n);
            ++parent.childCount;
        }
        if(!poolReadback())return fail(failure);
        if(!available())return fail(failure);
        if(owned&&!owned->valid())return fail(Reason::OwnershipChanged);
        // Bounded POD commit, with no subsequent refusable operation.
        if(rows)std::memcpy(output.data(),scratch.rows.data(),rows*sizeof(Row));
        result.status=Status::ObservedAllSlots;result.reason=Reason::ObservedAllSlots;result.rows=rows;
        result.relationshipsIndexed=true;
        result.ownedPublication=owned!=nullptr;
        result.train=result.parent=result.observedTrain=0;result.ordinal=0;return result;
    }catch(...){return fail(Reason::ReadException);}
}
template<class Read> Result captureMotionInventory(uintptr_t simulation,bool profileQualified,
    Read&& read,Budget& budget,std::span<Row> output,Scratch scratch,Limits limits={}) noexcept {
    return captureMotionInventoryImpl(simulation,profileQualified,std::forward<Read>(read),budget,output,scratch,limits,nullptr);
}
// Actual exclusive ownership replaces the generic second readback pass; every
// allocated slot is still visited, including vanilla/inactive/attached slots.
// Scope loss, insufficient storage or any budget refusal commits NO output.
template<class Read> Result captureMotionInventoryOwned(uintptr_t simulation,bool profileQualified,const OwnedScope& scope,
    Read&& read,Budget& budget,std::span<Row> output,Scratch scratch,Limits limits={}) noexcept {
    return captureMotionInventoryImpl(simulation,profileQualified,std::forward<Read>(read),budget,output,scratch,limits,&scope);
}
} // namespace nimby::windows::consist_inventory
