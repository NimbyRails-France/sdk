#pragma once
#include <nimby/detail/track_conflicts.h>
#include <engine/detail/memory_reader.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <utility>
#include <vector>

namespace nimby::engine {
enum class TrackConflictReason : uint32_t {
    Complete=NIMBY_TRACK_CONFLICTS_COMPLETE,
    TrackUnavailable=NIMBY_TRACK_CONFLICTS_TRACK_UNAVAILABLE,
    DescriptorInvalid=NIMBY_TRACK_CONFLICTS_DESCRIPTOR_INVALID,
    ReadUnavailable=NIMBY_TRACK_CONFLICTS_READ_UNAVAILABLE,
    IntervalInvalid=NIMBY_TRACK_CONFLICTS_INTERVAL_INVALID,
    ReferenceUnavailable=NIMBY_TRACK_CONFLICTS_REFERENCE_UNAVAILABLE,
    Unstable=NIMBY_TRACK_CONFLICTS_UNSTABLE,
    BudgetExceeded=NIMBY_TRACK_CONFLICTS_BUDGET_EXCEEDED
};
// Optional internal cooperative budget. Preflight never performs a target
// read or consumes quota; zero bytes asks whether CPU/allocation work may
// continue. A refusal is terminal for the entire request, even if a later
// smaller read would fit. Ordinary unavailable memory does not exhaust it.
struct TrackConflictReadBudget {
    using Preflight=bool (*)(void*,size_t);
    Preflight preflight{};void* context{};bool exhausted=false;
    bool available(size_t bytes=0){
        if(exhausted)return false;
        if(preflight&&!preflight(context,bytes)){exhausted=true;return false;}
        return true;
    }
};
// Lazily grown, at most two 4096-row native lists; reused for all requested
// IDs. Neither buffer initialization nor allocation precedes preflight.
struct TrackConflictDecodeWorkspace {std::vector<unsigned char> bytes;};
inline bool read_track_conflict_memory(ReadMemory read,void* context,uint64_t address,
    void* output,size_t bytes,TrackConflictReadBudget& budget){
    if(!budget.available(bytes))return false;
    const bool copied=read(context,address,output,bytes);
    return budget.available()&&copied;
}
// Windows119 qualification: native admission RVA 0x458560 reads Track+0xf8
// with stride 0x30. Serializer RVA 0x30fc40 confirms the scalar fields. The
// other lists (+0xe0/+0x110/+0x128) have different roles and are NEVER merged.
// Source ranges are canonical; the native predicates normalize other ranges.
// This decoder only reads through the callback, never invokes native code or
// dereferences target pointers. The caller qualifies the executable/profile,
// source slot address and validates each referenced full ID's incarnation.
// Success means complete (possibly empty); every failure clears output. A
// shared row budget bounds work even for lists that fail after being read.
template<class ValidateReference>
TrackConflictReason decode_track_conflicts(ReadMemory read,void* context,
    uint64_t trackId,uint64_t address,ValidateReference&& validateReference,
    size_t& remainingRows,std::vector<NimbyTrackConflict>& out,
    TrackConflictDecodeWorkspace& workspace,TrackConflictReadBudget* suppliedBudget=nullptr) {
    out.clear();
    TrackConflictReadBudget unlimited;
    auto& budget=suppliedBudget?*suppliedBudget:unlimited;
    const auto failure=[&](TrackConflictReason reason){out.clear();
        return budget.available()?reason:TrackConflictReason::BudgetExceeded;};
    if(!budget.available())return TrackConflictReason::BudgetExceeded;
    using memory::field;using memory::pointer;
    if(!read||trackId>>48!=1||!pointer(address)||address>0x7fffffff0000ULL-0x110)
        return TrackConflictReason::TrackUnavailable;
    std::array<unsigned char,0x110> header{},after{};
    if(!read_track_conflict_memory(read,context,address,header.data(),header.size(),budget)||field<uint64_t>(header.data(),0)!=trackId)
        return failure(TrackConflictReason::TrackUnavailable);
    const auto begin=field<uint64_t>(header.data(),0xf8),end=field<uint64_t>(header.data(),0x100),cap=field<uint64_t>(header.data(),0x108);
    constexpr size_t stride=0x30,maxRowsPerTrack=4096;
    if(end<begin||cap<end||cap-begin>maxRowsPerTrack*stride||
       (end-begin)%stride||(cap-begin)%stride||
       (cap&&(!pointer(begin)||cap>=0x7fffffff0000ULL)))
        return TrackConflictReason::DescriptorInvalid;
    const auto count=static_cast<size_t>((end-begin)/stride);
    if(count>remainingRows)return TrackConflictReason::BudgetExceeded;
    remainingRows-=count;
    const auto byteCount=count*stride;
    if(count){
        // Check the planned read size before growing scratch storage. A shared
        // byte-budget refusal must not allocate a list that cannot be read.
        if(!budget.available(byteCount))return TrackConflictReason::BudgetExceeded;
        if(workspace.bytes.size()<2*byteCount){
            workspace.bytes.reserve(2*byteCount); // Avoid geometric capacity growth beyond the largest list.
            workspace.bytes.resize(2*byteCount);
        }
        if(!read_track_conflict_memory(read,context,begin,workspace.bytes.data(),byteCount,budget)||
           !read_track_conflict_memory(read,context,begin,workspace.bytes.data()+byteCount,byteCount,budget))
            return failure(TrackConflictReason::ReadUnavailable);
    }
    if(!budget.available())return TrackConflictReason::BudgetExceeded;
    if(count&&std::memcmp(workspace.bytes.data(),workspace.bytes.data()+byteCount,byteCount)!=0)
        return TrackConflictReason::Unstable;
    if(!budget.available())return TrackConflictReason::BudgetExceeded;
    out.reserve(count);
    for(size_t i=0;i<count;++i){
        if(i%32==0&&!budget.available())return failure(TrackConflictReason::BudgetExceeded);
        const auto* row=workspace.bytes.data()+i*stride;const auto other=field<uint64_t>(row,0);
        // Native admission explicitly skips zero IDs. These tombstones do not
        // refer to another track and their unused scalar fields are irrelevant.
        if(!other)continue;
        if(other>>48!=1||!validateReference(other))return failure(TrackConflictReason::ReferenceUnavailable);
        if(!budget.available())return failure(TrackConflictReason::BudgetExceeded);
        const double a=field<float>(row,0x18),b=field<float>(row,0x1c),
            c=field<float>(row,0x20),d=field<float>(row,0x24);
        const auto fraction=[](double value){return std::isfinite(value)&&value>=0&&value<=1;};
        if(!fraction(a)||!fraction(b)||!fraction(c)||!fraction(d)||a>b)
            return failure(TrackConflictReason::IntervalInvalid);
        out.push_back({trackId,other,a,b,std::min(c,d),std::max(c,d)});
    }
    if(!read_track_conflict_memory(read,context,address,after.data(),after.size(),budget))return failure(TrackConflictReason::ReadUnavailable);
    // Geometry/pool metadata in this prefix is static during native admission.
    // Replaced descriptors or recycled IDs must not publish the old vector.
    if(header!=after)return failure(TrackConflictReason::Unstable);
    return TrackConflictReason::Complete;
}
// Standalone compatibility overload; batch readers supply a shared workspace.
template<class ValidateReference>
TrackConflictReason decode_track_conflicts(ReadMemory read,void* context,
    uint64_t trackId,uint64_t address,ValidateReference&& validateReference,
    size_t& remainingRows,std::vector<NimbyTrackConflict>& out,
    TrackConflictReadBudget* budget=nullptr){
    TrackConflictDecodeWorkspace workspace;
    return decode_track_conflicts(read,context,trackId,address,
        std::forward<ValidateReference>(validateReference),remainingRows,out,workspace,budget);
}
}
