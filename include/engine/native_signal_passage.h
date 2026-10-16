#pragma once
#include <nimby/detail/observation.h>
#include <nimby/detail/track_fraction.hpp>
#include <cmath>
#include <cstdint>
#include <limits>

namespace nimby::engine::native_passage {

// Local tolerances for the qualified Windows native profile. The fraction
// tolerance reuses the SDK's qualified arithmetic rounding tolerance, NOT a
// metric clearance or a train length.
// It must not be widened to turn an ambiguous observation into a crossing.
inline constexpr double fractionTolerance=nimby::detail::trackFractionRounding;
inline constexpr double headToleranceMeters=1e-6;

struct SourceAnchor {
    uint64_t signal=0,track=0;
    double fraction=0;
    // Direction of travel (+1/-1) in the anchor's declared orientation. A PATH
    // signal's raw stored panel-facing direction is not this value.
    int direction=0;
    uint32_t kind=0;
    int orientation=0;
};
struct Presence {
    uint64_t track=0;
    double fraction=0;
    int direction=0;
    double head=0; // Curvilinear metres in the same unchanged native Path.
    int orientation=0;
};
struct Observation {
    uint64_t trainBefore=0,trainAfter=0;
    SourceAnchor source;
    Presence candidate;     // Copied at the native pass_by dispatcher.
    Presence finalPresence; // Conserved state AFTER nativeStep (0x448710).
    double beforeHead=0;
    bool pathUnchanged=false,sourceUnchanged=false,finalPresenceMatches=false;
    bool readComplete=false,candidatesComplete=false;
};
enum class Status { Crossed,AtBoundary,BeforeSource,Unavailable };
enum class Reason {
    CrossedSource,OnSourceBoundary,StillBeforeSource,
    ReadIncomplete,CandidatesIncomplete,PathChanged,SourceChanged,FinalPresenceMismatch,
    InvalidTrainIdentity,TrainIdentityChanged,InvalidSignalIdentity,InvalidTrackIdentity,
    WrongSignalKind,InvalidFraction,InvalidHead,HeadPrecisionUnavailable,
    InvalidDirection,TrackTransition,DirectionMismatch,CandidateNotAtSource,
    BeforeHeadAfterCandidate,FinalHeadBeforeStep,HeadFractionDisagreement,
    OrientationMismatch,Count
};
struct Assessment {
    Status status=Status::Unavailable;
    Reason reason=Reason::ReadIncomplete;
};
inline const char* statusName(Status status) noexcept {
    switch(status){
    case Status::Crossed:return "crossed";
    case Status::AtBoundary:return "at_boundary";
    case Status::BeforeSource:return "before_source";
    case Status::Unavailable:return "unavailable";
    }
    return "invalid_status";
}
inline const char* reasonName(Reason reason) noexcept {
    switch(reason){
    case Reason::CrossedSource:return "crossed_source";
    case Reason::OnSourceBoundary:return "on_source_boundary";
    case Reason::StillBeforeSource:return "still_before_source";
    case Reason::ReadIncomplete:return "read_incomplete";
    case Reason::CandidatesIncomplete:return "candidates_incomplete";
    case Reason::PathChanged:return "path_changed";
    case Reason::SourceChanged:return "source_changed";
    case Reason::FinalPresenceMismatch:return "final_presence_mismatch";
    case Reason::InvalidTrainIdentity:return "invalid_train_identity";
    case Reason::TrainIdentityChanged:return "train_identity_changed";
    case Reason::InvalidSignalIdentity:return "invalid_signal_identity";
    case Reason::InvalidTrackIdentity:return "invalid_track_identity";
    case Reason::WrongSignalKind:return "wrong_signal_kind";
    case Reason::InvalidFraction:return "invalid_fraction";
    case Reason::InvalidHead:return "invalid_head";
    case Reason::HeadPrecisionUnavailable:return "head_precision_unavailable";
    case Reason::InvalidDirection:return "invalid_direction";
    case Reason::TrackTransition:return "track_transition";
    case Reason::DirectionMismatch:return "direction_mismatch";
    case Reason::CandidateNotAtSource:return "candidate_not_at_source";
    case Reason::BeforeHeadAfterCandidate:return "before_head_after_candidate";
    case Reason::FinalHeadBeforeStep:return "final_head_before_step";
    case Reason::HeadFractionDisagreement:return "head_fraction_disagreement";
    case Reason::OrientationMismatch:return "orientation_mismatch";
    case Reason::Count:break; // Array bound, never an assessment reason.
    }
    return "invalid_reason";
}
namespace detail {
inline bool fraction(double value) noexcept {
    return std::isfinite(value)&&value>=0&&value<=1;
}
inline bool head(double value) noexcept {return std::isfinite(value)&&value>=0;}
inline bool direction(int value) noexcept {return value==1||value==-1;}
inline bool preciseHead(double value) noexcept {
    // At extreme magnitudes double cannot resolve the declared crossing
    // tolerance. Reject that state instead of relying on overflowing sums or
    // a rounded `source + epsilon` comparison.
    const auto adjacent=std::nextafter(value,std::numeric_limits<double>::infinity());
    return std::isfinite(adjacent)&&adjacent-value<=headToleranceMeters;
}
}

// Pure, constant-cost qualification of a LOCAL observation. No native reads,
// callbacks, allocations, inferred branches, leases or permission changes.
// A dispatcher notification is only a candidate: admission at rest and a later
// native rollback must never become Crossed. Track joins/aliases are deliberately
// unavailable until an ordered metric continuation is qualified separately.
// This result has no world epoch / completed-tick certificate, proves nothing
// about the train's rear and MUST NOT be used as central interlocking evidence.
inline Assessment assess(const Observation& observation) noexcept {
    const auto unavailable=[](Reason reason){return Assessment{Status::Unavailable,reason};};
    if(!observation.readComplete)return unavailable(Reason::ReadIncomplete);
    if(!observation.candidatesComplete)return unavailable(Reason::CandidatesIncomplete);
    if(!observation.pathUnchanged)return unavailable(Reason::PathChanged);
    if(!observation.sourceUnchanged)return unavailable(Reason::SourceChanged);
    if(!observation.finalPresenceMatches)return unavailable(Reason::FinalPresenceMismatch);
    if(observation.trainBefore>>48!=5||observation.trainAfter>>48!=5)
        return unavailable(Reason::InvalidTrainIdentity);
    if(observation.trainBefore!=observation.trainAfter)return unavailable(Reason::TrainIdentityChanged);
    const auto& source=observation.source;
    const auto& candidate=observation.candidate;
    const auto& final=observation.finalPresence;
    if(source.signal>>48!=8)return unavailable(Reason::InvalidSignalIdentity);
    if(source.track>>48!=1||candidate.track>>48!=1||final.track>>48!=1)
        return unavailable(Reason::InvalidTrackIdentity);
    if(source.kind!=NIMBY_SIGNAL_PATH)return unavailable(Reason::WrongSignalKind);
    if(!detail::fraction(source.fraction)||!detail::fraction(candidate.fraction)||!detail::fraction(final.fraction))
        return unavailable(Reason::InvalidFraction);
    if(!detail::head(observation.beforeHead)||!detail::head(candidate.head)||!detail::head(final.head))
        return unavailable(Reason::InvalidHead);
    if(!detail::preciseHead(observation.beforeHead)||!detail::preciseHead(candidate.head)||!detail::preciseHead(final.head))
        return unavailable(Reason::HeadPrecisionUnavailable);
    if(!detail::direction(source.direction)||!detail::direction(candidate.direction)||!detail::direction(final.direction))
        return unavailable(Reason::InvalidDirection);
    if(source.track!=candidate.track||source.track!=final.track)return unavailable(Reason::TrackTransition);
    // Raw fractions and directions are comparable only in one orientation.
    // The native projector can invert both when the source uses another track
    // frame. Do not infer that conversion from coincident values (notably .5).
    if(source.orientation!=candidate.orientation||source.orientation!=final.orientation)
        return unavailable(Reason::OrientationMismatch);
    if(source.direction!=candidate.direction||source.direction!=final.direction)
        return unavailable(Reason::DirectionMismatch);
    if(std::abs(candidate.fraction-source.fraction)>fractionTolerance)
        return unavailable(Reason::CandidateNotAtSource);
    // The dispatcher must belong to this step's forward movement, not an old
    // candidate whose Path coordinate has already been passed or reset.
    if(observation.beforeHead>candidate.head)return unavailable(Reason::BeforeHeadAfterCandidate);
    if(final.head<observation.beforeHead)return unavailable(Reason::FinalHeadBeforeStep);
    const auto headDelta=final.head-candidate.head;
    const auto fractionDelta=(final.fraction-source.fraction)*source.direction;
    // Subtraction avoids adding epsilon to large coordinates. Fraction inputs
    // are bounded and head inputs nonnegative/finite, so these cannot overflow.
    const int headSide=headDelta>headToleranceMeters?1:headDelta<-headToleranceMeters?-1:0;
    const int fractionSide=fractionDelta>fractionTolerance?1:fractionDelta<-fractionTolerance?-1:0;
    if(headSide!=fractionSide){
        // A genuine same-direction movement smaller than the metric crossing
        // tolerance stays conservatively at the boundary. A zero head delta,
        // opposite signs or rounding-only fraction with a large head delta do
        // not qualify a coherent movement.
        if(headSide==0&&((headDelta>0&&fractionSide>0)||(headDelta<0&&fractionSide<0)))
            return {Status::AtBoundary,Reason::OnSourceBoundary};
        return unavailable(Reason::HeadFractionDisagreement);
    }
    if(headSide>0)return {Status::Crossed,Reason::CrossedSource};
    if(headSide<0)return {Status::BeforeSource,Reason::StillBeforeSource};
    return {Status::AtBoundary,Reason::OnSourceBoundary};
}
}
