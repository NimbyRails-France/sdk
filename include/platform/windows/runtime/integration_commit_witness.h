#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace nimby::windows::automatic {
// Experimental, local 448710 transaction witness. No central interlocking,
// tick certificate, rear-clear evidence or native movement authority.
struct CommitPresence {
    int32_t index{},padding{};uint64_t track{};double fraction{};
    int8_t direction{},orientation{};std::array<unsigned char,6> reserved{};double head{};
};
static_assert(sizeof(CommitPresence)==0x28);
inline bool validCommitPresence(const CommitPresence& value)noexcept {
    return value.index>=0&&value.track>>48==1&&std::isfinite(value.fraction)&&
        value.fraction>=0&&value.fraction<=1&&(value.direction==1||value.direction==-1)&&
        std::isfinite(value.head)&&value.head>=0;
}
inline bool sameCommitPresence(const CommitPresence& a,const CommitPresence& b)noexcept {
    return a.index==b.index&&a.track==b.track&&a.fraction==b.fraction&&
        a.direction==b.direction&&a.orientation==b.orientation&&a.head==b.head;
}
struct CommitPathStamp {
    static constexpr size_t maximumTracks=512;
    std::array<unsigned char,0xc0> header{};
    std::array<uint64_t,maximumTracks> tracks{};
    size_t count{};
    template<class T> static T field(const auto& bytes,size_t offset)noexcept {
        T value{};std::memcpy(&value,bytes.data()+offset,sizeof value);return value;
    }
    template<class Read> bool capture(uintptr_t motion,Read&& read)noexcept {
        constexpr uintptr_t limit=0x7fffffff0000ULL;
        if(motion<0x10000||motion>limit-0x638)return false;
        if(!read(motion+0x290,header.data(),header.size())||header[0x90]!=1)return false;
        const auto begin=field<uintptr_t>(header,0xa8),end=field<uintptr_t>(header,0xb0),cap=field<uintptr_t>(header,0xb8);
        if(begin<0x10000||begin%8||end<=begin||end>=limit||cap<end||cap>=limit||
           (end-begin)%8||(cap-begin)%8||end-begin>tracks.size()*8)return false;
        count=(end-begin)/8;
        if(!read(begin,tracks.data(),count*8))return false;
        for(size_t i=0;i<count;++i)if(tracks[i]>>48!=1)return false;
        std::array<unsigned char,0xc0> confirmed{};
        return read(motion+0x290,confirmed.data(),confirmed.size())&&confirmed==header;
    }
    template<class Read> bool matches(uintptr_t motion,Read&& read)const noexcept {
        CommitPathStamp current;
        return current.capture(motion,read)&&current.header==header&&current.count==count&&
            std::equal(tracks.begin(),tracks.begin()+count,current.tracks.begin());
    }
};
enum class CommitReason : uint32_t {
    Conserved, RolledBack, OutsideNativeStep, IdentityUnqualified, ContextUnqualified,
    OwnerUnavailable, Reentrant, PathUnqualified, SourcesUnqualified,
    PresenceUnqualified, MirrorUnqualified, PublicationChanged, RouteChanged,
    ResultUnqualified, AlreadyLatched, Count
};
inline const char* commitReasonName(CommitReason value)noexcept {
    switch(value){
    case CommitReason::Conserved:return "conserved";
    case CommitReason::RolledBack:return "rolled_back";
    case CommitReason::OutsideNativeStep:return "outside_native_step";
    case CommitReason::IdentityUnqualified:return "identity_unqualified";
    case CommitReason::ContextUnqualified:return "context_unqualified";
    case CommitReason::OwnerUnavailable:return "owner_unavailable";
    case CommitReason::Reentrant:return "reentrant";
    case CommitReason::PathUnqualified:return "path_unqualified";
    case CommitReason::SourcesUnqualified:return "sources_unqualified";
    case CommitReason::PresenceUnqualified:return "presence_unqualified";
    case CommitReason::MirrorUnqualified:return "mirror_unqualified";
    case CommitReason::PublicationChanged:return "publication_changed";
    case CommitReason::RouteChanged:return "route_changed";
    case CommitReason::ResultUnqualified:return "result_unqualified";
    case CommitReason::AlreadyLatched:return "already_latched";
    case CommitReason::Count:break;
    }
    return "invalid_reason";
}
} // namespace nimby::windows::automatic
