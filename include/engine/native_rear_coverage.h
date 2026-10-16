#pragma once
#include <algorithm>
#include <array>
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP>=2)
#include <xmmintrin.h>
#endif

namespace nimby::engine::native_rear {

// These identities come from a provider which owns a finished publication.
// Equal timestamps / repeated external reads do not qualify a publication.
enum class Phase : uint8_t { Unknown, BeforeMovement, RetainedMovement };
struct Publication {
    uint64_t session{},epoch{},tick{};
    Phase phase=Phase::Unknown;
    bool operator==(const Publication&) const = default;
};
struct Consist {
    uint64_t train{},revision{};
    double lengthMeters{};       // Entire expected consist, including coupled members.
    float nativeLengthMeters{};  // Native float accumulation for the SAME consist.
    Publication at;
    bool complete=false,lengthQualified=false,nativeLengthQualified=false;
};

// Sections run from head towards rear, already in each track's canonical
// fraction frame. No native pointer or unqualified orientation transformation
// is accepted. Sequence numbers identify consecutive retained history records;
// they are not train/vehicle identities or simulation tick certificates.
enum class HistoryEnd : uint8_t { Unknown, Continues, QualifiedEnd };
struct HistorySection {
    uint64_t track{},sequence{};
    double from{},to{},metricMeters{};
    int8_t rearDirection{};
    uint8_t orientation{};
    HistoryEnd end=HistoryEnd::Unknown;
    // The provider qualifies the actual connection (including branch choice),
    // not a connection inferred from coordinates or a list of track IDs.
    bool joinQualified=false;
    uint64_t previousTrack{};
    double previousFraction{};
    uint8_t previousOrientation{};
};
struct History {
    uint64_t train{},consistRevision{};
    Publication at;
    bool qualified=false,continuityQualified=false;
    // Continuity must cover reversal, relocation and replan. If it is lost,
    // the provider must not qualify a newly fabricated head-minus-length path.
    std::span<const HistorySection> sections;
};
struct Cursor {
    uint64_t train{},track{};
    size_t section{};
    double fraction{};
    int8_t rearDirection{};
    uint8_t orientation{};
    Publication at;
    bool qualified=false;
};
struct Observation {
    Consist consist;
    History history;
    Cursor cursor;
    bool stablePublicationQualified=false;
};
struct Portion {
    uint64_t train{},track{};
    double from{},to{},lengthMeters{};
    uint8_t orientation{};
};
enum class Status : uint8_t { Unknown, Complete };
enum class Reason : uint8_t {
    Complete,InvalidLimits,InvalidPublication,MissingStablePublication,DifferentPublication,
    InvalidTrainIdentity,TrainIdentityChanged,InvalidConsistRevision,ConsistChanged,
    IncompleteConsist,UnqualifiedLength,UnqualifiedNativeLength,InvalidLength,
    UnqualifiedHistory,UnqualifiedContinuity,UnqualifiedCursor,InvalidCursor,
    InvalidTrackIdentity,InvalidSequence,InvalidGeometry,InvalidOrientation,
    CursorMismatch,DirectionMismatch,MissingTransition,TransitionMismatch,
    HistoryGap,DuplicateSection,UnqualifiedReversal,HistoryTooShort,
    EndpointUnqualified,BudgetExceeded,OutputCapacity,ArithmeticUnqualified
};
struct Limits {
    size_t historySections=4096,visitedSections=4096,outputPortions=4096,work=8192,uniqueTracks=512;
};
struct Result {
    Status status=Status::Unknown;
    Reason reason=Reason::UnqualifiedHistory;
    uint64_t train{},consistRevision{},track{};
    Publication at;
    size_t section{},visitedSections{},portions{},work{};
    double requestedMeters{},coveredMeters{},remainingMeters{};
    bool complete() const noexcept {return status==Status::Complete;}
};
namespace detail {
inline bool publication(const Publication& value) noexcept {
    return value.session&&value.epoch&&
        (value.phase==Phase::BeforeMovement||value.phase==Phase::RetainedMovement);
}
inline bool identity(uint64_t value,uint64_t kind) noexcept {
    return value>>48==kind&&(value&0x0000ffffffffffffULL)!=0;
}
inline bool fraction(double value) noexcept {return std::isfinite(value)&&value>=0&&value<=1;}
inline int direction(double from,double to) noexcept {return to>from?1:to<from?-1:0;}
inline bool arithmeticQualified() noexcept {
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__)
    return false; // Reassociation invalidates TwoDiff and even finite checks.
#else
    if(!std::numeric_limits<double>::is_iec559||std::fegetround()!=FE_TONEAREST)return false;
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP>=2)
    // FE_TONEAREST can report only x87 state; SSE rounding, FTZ and DAZ are
    // independent. Read controls directly, without FP probes which could set
    // status flags or trap on an unmasked denormal exception in the host game.
    return (_mm_getcsr()&(0x6000u|0x8000u|0x0040u))==0;
#else
    // Compiles portably, but other architectures need their own qualified
    // read-only FP-control accessor before they can provide clearance proof.
    return false;
#endif
#endif
}
inline bool subnormal(double value) noexcept {return std::fpclassify(value)==FP_SUBNORMAL;}
// Error-free TwoDiff under strict round-to-nearest IEEE double arithmetic.
// The exact difference is rounded + residual. No wider long double is needed.
inline double differenceResidual(double a,double b,double rounded) noexcept {
    const double bVirtual=a-rounded;
    const double aVirtual=rounded+bVirtual;
    return (a-aVirtual)+(bVirtual-b);
}
inline bool distanceLower(double from,double to,double metric,double& lower) noexcept {
    const double a=std::max(from,to),b=std::min(from,to);
    double span=a-b;
    if(differenceResidual(a,b,span)<0)span=std::nextafter(span,0.);
    if(span==0) {lower=0;return a==b;}
    lower=span*metric;
    if(!std::isfinite(lower)||lower==0)return false;
    // A product residual may underflow to zero and hide its sign. Reject this
    // very small regime, rather than treating a zero FMA residual as exact.
    // Above 2^-969 the lowest bit of a 53x53-bit product remains representable
    // even after cancellation, including when its residual is subnormal.
    constexpr double residualSafeProduct=0x1p-969;
    if(lower<residualSafeProduct)return false;
    if(std::fma(span,metric,-lower)<0)lower=std::nextafter(lower,0.);
    return true;
}
inline double remainingUpper(double before,double consumedLower) noexcept {
    double remaining=before-consumedLower;
    if(differenceResidual(before,consumedLower,remaining)>0)
        remaining=std::nextafter(remaining,std::numeric_limits<double>::infinity());
    return remaining;
}
inline bool tailOutward(double from,double to,double remainingUpper,double metric,double& tail) noexcept {
    // One successor bounds each correctly-rounded division/addition outward.
    // Clamping is safe: the caller proved remainingUpper < distanceLower, so
    // the exact required tail lies strictly within this qualified section.
    const double quotient=remainingUpper/metric;
    if(!std::isfinite(quotient)||quotient==0||subnormal(quotient))return false;
    const double movement=std::nextafter(quotient,std::numeric_limits<double>::infinity());
    const double rounded=to>from?from+movement:from-movement;
    if(subnormal(rounded))return false;
    tail=to>from?std::min(to,std::nextafter(rounded,std::numeric_limits<double>::infinity())):
        std::max(to,std::nextafter(rounded,-std::numeric_limits<double>::infinity()));
    return !subnormal(tail);
}
}

// Pure qualification of an aggregate body envelope, NOT a clearance decision.
// A Complete result neither certifies entry nor releases a lease. The provider
// must prove the entire consist, owned publication and historical connectivity.
// Every positive remaining distance is consumed; no epsilon turns an incomplete
// history into a complete body. Using the larger qualified length conserves both
// native float rounding and the entire expected consist's physical length.
//
// output is caller-owned storage, disjoint from immutable input. On Unknown it
// is untouched and result.portions is zero. Two bounded passes avoid partial
// output or a heap allocation. Portions stay ordered; repeated tracks are not
// merged because a real historical loop may visit one track more than once.
inline Result assess(const Observation& observation,const Publication& current,
                     std::span<Portion> output,Limits limits={}) noexcept {
    const auto& consist=observation.consist;
    const auto& history=observation.history;
    const auto& cursor=observation.cursor;
    Result result;result.train=consist.train;result.consistRevision=consist.revision;result.at=current;
    const auto fail=[&](Reason reason) {result.reason=reason;result.portions=0;return result;};
    if(!detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
    if(!limits.historySections||limits.historySections>4096||!limits.visitedSections||limits.visitedSections>4096||
       !limits.outputPortions||limits.outputPortions>4096||!limits.work||limits.work>65536||
       !limits.uniqueTracks||limits.uniqueTracks>512)
        return fail(Reason::InvalidLimits);
    if(!detail::publication(current))return fail(Reason::InvalidPublication);
    if(!observation.stablePublicationQualified)return fail(Reason::MissingStablePublication);
    if(consist.at!=current||history.at!=current||cursor.at!=current)return fail(Reason::DifferentPublication);
    if(!detail::identity(consist.train,5)||!detail::identity(history.train,5)||!detail::identity(cursor.train,5))
        return fail(Reason::InvalidTrainIdentity);
    if(consist.train!=history.train||consist.train!=cursor.train)return fail(Reason::TrainIdentityChanged);
    if(!consist.revision||!history.consistRevision)return fail(Reason::InvalidConsistRevision);
    if(consist.revision!=history.consistRevision)return fail(Reason::ConsistChanged);
    if(!consist.complete)return fail(Reason::IncompleteConsist);
    if(!consist.lengthQualified)return fail(Reason::UnqualifiedLength);
    if(!consist.nativeLengthQualified)return fail(Reason::UnqualifiedNativeLength);
    if(!std::isfinite(consist.lengthMeters)||consist.lengthMeters<=0||
       !std::isfinite(consist.nativeLengthMeters)||consist.nativeLengthMeters<=0)
        return fail(Reason::InvalidLength);
    if(detail::subnormal(consist.lengthMeters)||std::fpclassify(consist.nativeLengthMeters)==FP_SUBNORMAL)
        return fail(Reason::ArithmeticUnqualified);
    result.requestedMeters=std::max(consist.lengthMeters,double(consist.nativeLengthMeters));
    result.remainingMeters=result.requestedMeters;
    if(!history.qualified)return fail(Reason::UnqualifiedHistory);
    if(!history.continuityQualified)return fail(Reason::UnqualifiedContinuity);
    if(!cursor.qualified)return fail(Reason::UnqualifiedCursor);
    if(history.sections.size()>limits.historySections)return fail(Reason::BudgetExceeded);
    if(cursor.section>=history.sections.size()||!detail::identity(cursor.track,1)||!detail::fraction(cursor.fraction)||
       (cursor.rearDirection!=1&&cursor.rearDirection!=-1)||cursor.orientation>2)
        return fail(Reason::InvalidCursor);
    if(detail::subnormal(cursor.fraction))return fail(Reason::ArithmeticUnqualified);

    size_t count=0,last=cursor.section;double tail=cursor.fraction;
    const HistorySection* previous=nullptr;
    // A track revisited on a loop must retain its exact metric/frame within
    // this publication. Fixed witnesses avoid a quadratic consistency scan;
    // only the small presence bitset is initialized, not every witness cell.
    struct TrackWitness {uint64_t track;double metric;uint8_t orientation;};
    std::array<TrackWitness,1024> witnesses;
    std::array<uint64_t,16> present{};
    size_t unique=0;
    for(size_t index=cursor.section;;++index) {
        result.section=index;
        if(index>=history.sections.size())return fail(Reason::HistoryTooShort);
        if(result.visitedSections==limits.visitedSections||limits.work-result.work<2)
            return fail(Reason::BudgetExceeded);
        result.work+=2; // Qualification and the later emission of this section.
        ++result.visitedSections;
        const auto& part=history.sections[index];result.track=part.track;
        if(!detail::identity(part.track,1))return fail(Reason::InvalidTrackIdentity);
        if(!part.sequence)return fail(Reason::InvalidSequence);
        if(!detail::fraction(part.from)||!detail::fraction(part.to)||
           !std::isfinite(part.metricMeters)||part.metricMeters<=0)
            return fail(Reason::InvalidGeometry);
        if(detail::subnormal(part.from)||detail::subnormal(part.to)||detail::subnormal(part.metricMeters))
            return fail(Reason::ArithmeticUnqualified);
        if(part.orientation>2)return fail(Reason::InvalidOrientation);
        uint64_t hash=part.track;hash^=hash>>33;hash*=0xff51afd7ed558ccdULL;hash^=hash>>33;
        size_t slot=size_t(hash)&1023;
        for(;;slot=(slot+1)&1023) {
            if(result.work==limits.work)return fail(Reason::BudgetExceeded);
            ++result.work;
            const uint64_t mask=uint64_t{1}<<(slot%64);
            if(!(present[slot/64]&mask)) {
                if(unique==limits.uniqueTracks)return fail(Reason::BudgetExceeded);
                ++unique;present[slot/64]|=mask;
                witnesses[slot]={part.track,part.metricMeters,part.orientation};break;
            }
            const auto& witness=witnesses[slot];
            if(witness.track==part.track) {
                if(witness.metric!=part.metricMeters||witness.orientation!=part.orientation)
                    return fail(Reason::TransitionMismatch);
                break;
            }
        }
        if(part.end!=HistoryEnd::Unknown&&part.end!=HistoryEnd::Continues&&part.end!=HistoryEnd::QualifiedEnd)
            return fail(Reason::MissingTransition);
        double from=part.from;
        const auto sign=detail::direction(part.from,part.to);
        if((part.rearDirection!=1&&part.rearDirection!=-1)||(sign&&sign!=part.rearDirection))
            return fail(Reason::DirectionMismatch);
        if(!previous) {
            if(part.track!=cursor.track||part.orientation!=cursor.orientation||
               cursor.fraction<std::min(part.from,part.to)||cursor.fraction>std::max(part.from,part.to))
                return fail(Reason::CursorMismatch);
            if(part.rearDirection!=cursor.rearDirection)return fail(Reason::DirectionMismatch);
            from=cursor.fraction;
        } else {
            if(previous->sequence==UINT64_MAX||part.sequence!=previous->sequence+1)return fail(Reason::InvalidSequence);
            if(!part.joinQualified)return fail(Reason::MissingTransition);
            if(part.previousTrack!=previous->track||!detail::fraction(part.previousFraction)||
               part.previousFraction!=previous->to||part.previousOrientation!=previous->orientation)
                return fail(Reason::TransitionMismatch);
            if(part.track==previous->track) {
                if(part.orientation!=previous->orientation||part.metricMeters!=previous->metricMeters)
                    return fail(Reason::TransitionMismatch);
                if(part.from!=previous->to)return fail(Reason::HistoryGap);
                if(previous->rearDirection!=part.rearDirection)return fail(Reason::UnqualifiedReversal);
                if(previous->from==previous->to&&!sign)return fail(Reason::DuplicateSection);
            }
        }
        double available=0;
        if(!detail::distanceLower(from,part.to,part.metricMeters,available))return fail(Reason::ArithmeticUnqualified);
        if(count==limits.outputPortions)return fail(Reason::OutputCapacity);
        ++count;last=index;
        if(result.remainingMeters==0) {tail=from;break;} // Adjacent closed rear endpoint.
        if(result.remainingMeters<available) {
            if(!detail::tailOutward(from,part.to,result.remainingMeters,part.metricMeters,tail))
                return fail(Reason::ArithmeticUnqualified);
            // Every partial tail encloses the true remaining body, including
            // division/addition rounding. No epsilon erases positive length.
            if(tail==from||!detail::fraction(tail))return fail(Reason::InvalidGeometry);
            result.coveredMeters=result.requestedMeters;result.remainingMeters=0;
            break;
        }
        // Equality lands on the retained endpoint. A qualified continuation
        // contributes its own point too; endpoint contact remains occupation.
        tail=part.to;result.remainingMeters=detail::remainingUpper(result.remainingMeters,available);
        result.coveredMeters=result.requestedMeters-result.remainingMeters;
        if(result.remainingMeters==0) {
            if(part.end==HistoryEnd::QualifiedEnd)break;
            if(part.end==HistoryEnd::Unknown)return fail(Reason::EndpointUnqualified);
        } else if(part.end!=HistoryEnd::Continues)return fail(Reason::HistoryTooShort);
        previous=&part;
    }
    if(count>output.size())return fail(Reason::OutputCapacity);
    // Qualification completed. Emit only the prefix actually occupied by the
    // entire body; older, unused history cannot become an occupation interval.
    for(size_t index=cursor.section,at=0;index<=last;++index,++at) {
        const auto& part=history.sections[index];
        const double from=index==cursor.section?cursor.fraction:part.from;
        const double to=index==last?tail:part.to;
        output[at]={consist.train,part.track,from,to,std::abs(to-from)*part.metricMeters,part.orientation};
    }
    result.status=Status::Complete;result.reason=Reason::Complete;result.portions=count;
    return result;
}
}
