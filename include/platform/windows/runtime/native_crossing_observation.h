#pragma once
#include <cstdint>
#include <cmath>
#include <optional>

namespace nimby::windows::automatic {
// Copied refusal facts from four pinned call sites inside 458560. This value
// performs no native read/call, allocates nothing and changes no permission.
// It does not prove that a route is free, entered, retained or rear-cleared.
enum class CrossingRefusal : uint8_t { CrossingOccupied=1,CrossingReserved=2 };
enum class CrossingPredicate : uint8_t { Occupation,Reservation };
struct CrossingIdentity {
    uintptr_t motion{};
    uint64_t train{},source{},routeRevision{};
    bool qualified=false;
};
struct CrossingRange {
    uintptr_t context{},occupationContext{},reservationContext{};
    uint64_t ownTrack{};
    double from{},to{};
    bool qualified=false;
};
class CrossingObservation {
    CrossingIdentity identity_;
    CrossingRange range_;
    bool valid_=false,sourceSeen_=false,active_=false;
    uint8_t followed_=0;
    uint32_t calls_=0;
    uint64_t crossingTrack_=0;
    double crossingFrom_=0,crossingTo_=0;
    bool crossingPhysicalClear_=false;
    std::optional<CrossingRefusal> refusal_;
    static bool id(uint64_t value,uint64_t kind) noexcept {
        return value>>48==kind&&(value&0x0000ffffffffffffULL)!=0;
    }
    static bool fraction(double value) noexcept {return std::isfinite(value)&&value>=0&&value<=1;}
    static bool complete(const CrossingIdentity& value) noexcept {
        return value.qualified&&value.motion&&id(value.train,5)&&id(value.source,8)&&value.routeRevision;
    }
    bool same(const CrossingIdentity& value) const noexcept {
        return complete(value)&&value.motion==identity_.motion&&value.train==identity_.train&&
            value.source==identity_.source&&value.routeRevision==identity_.routeRevision;
    }
public:
    static constexpr uintptr_t followedOccupationReturnRva=0x4585ef,followedReservationReturnRva=0x45860c;
    static constexpr uintptr_t crossingOccupationReturnRva=0x4586ad,crossingReservationReturnRva=0x4586c0;
    explicit CrossingObservation(CrossingIdentity identity={}) noexcept
        :identity_(identity),valid_(complete(identity)){}
    void invalidate() noexcept {valid_=false;refusal_.reset();}
    bool active() const noexcept {return valid_&&active_;}
    bool sourceSeen() const noexcept {return valid_&&sourceSeen_;}
    static bool caller(CrossingPredicate predicate,uintptr_t rva) noexcept {
        return rva==(predicate==CrossingPredicate::Occupation?followedOccupationReturnRva:followedReservationReturnRva)||
            rva==(predicate==CrossingPredicate::Occupation?crossingOccupationReturnRva:crossingReservationReturnRva);
    }
    // Called only after copying the native source marker AND its context's
    // source/motion references. A source may appear once in this traversal.
    bool source(const CrossingIdentity& identity) noexcept {
        if(!valid_||!same(identity)||sourceSeen_||active_){invalidate();return false;}
        sourceSeen_=true;return true;
    }
    bool beginRange(const CrossingRange& range) noexcept {
        if(!sourceSeen_)return false; // Prefix before the requested source is not its downstream block.
        if(!valid_||active_||refusal_||!range.qualified||!range.context||!range.occupationContext||
           !range.reservationContext||!id(range.ownTrack,1)||!fraction(range.from)||!fraction(range.to)){
            invalidate();return false;
        }
        range_=range;active_=true;followed_=0;crossingPhysicalClear_=false;return true;
    }
    // callerRva is the actual return address in the pinned game image, not a
    // track comparison. F8 may repeat the followed track/range; it is still a
    // crossing call only at these two distinct native call sites.
    void predicate(CrossingPredicate predicate,uintptr_t callerRva,uintptr_t context,
        const CrossingIdentity& identity,uint64_t track,double from,double to,uint8_t nativeResult) noexcept {
        if(!valid_||!sourceSeen_||!active_)return;
        const bool followed=callerRva==(predicate==CrossingPredicate::Occupation?
            followedOccupationReturnRva:followedReservationReturnRva);
        const bool crossed=callerRva==(predicate==CrossingPredicate::Occupation?
            crossingOccupationReturnRva:crossingReservationReturnRva);
        if(!followed&&!crossed)return; // Private SDK queries/other native callers provide no fact.
        if(++calls_>4096||!same(identity)||context!=(predicate==CrossingPredicate::Occupation?
           range_.occupationContext:range_.reservationContext)||!id(track,1)||!fraction(from)||!fraction(to)){
            invalidate();return;
        }
        if(followed){
            const uint8_t expected=predicate==CrossingPredicate::Occupation?0:1;
            if(followed_!=expected||track!=range_.ownTrack||from!=range_.from||to!=range_.to||refusal_){invalidate();return;}
            // This is the effective result returned to the native branch. A
            // followed-range on-sight substitution confirms branch traversal,
            // never native exclusive clearance. Its refusal is NOT a crossing.
            followed_=nativeResult?uint8_t(expected+1):uint8_t{3};return;
        }
        if(followed_!=2||refusal_){invalidate();return;}
        if(predicate==CrossingPredicate::Occupation){
            if(crossingPhysicalClear_){invalidate();return;}
            crossingTrack_=track;crossingFrom_=from;crossingTo_=to;
            if(nativeResult)crossingPhysicalClear_=true;
            else refusal_=CrossingRefusal::CrossingOccupied;
        }else{
            if(!crossingPhysicalClear_||track!=crossingTrack_||from!=crossingFrom_||to!=crossingTo_){invalidate();return;}
            crossingPhysicalClear_=false;
            if(!nativeResult)refusal_=CrossingRefusal::CrossingReserved;
        }
    }
    void endRange(uint8_t nativeResult) noexcept {
        if(!active_)return;
        active_=false;
        if(refusal_&&nativeResult)invalidate();
        if(crossingPhysicalClear_)invalidate(); // Missing matching reservation cannot donate a verdict.
    }
    std::optional<CrossingRefusal> finish(const CrossingIdentity& identity,uint8_t nativeStageResult) const noexcept {
        return valid_&&sourceSeen_&&!active_&&same(identity)&&!nativeStageResult?refusal_:std::nullopt;
    }
};
}
