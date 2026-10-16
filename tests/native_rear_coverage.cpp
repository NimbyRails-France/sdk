#include <engine/native_rear_coverage.h>
#include <array>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#if defined(__SSE2__) || defined(_M_X64)
#include <xmmintrin.h>
#endif

using namespace nimby::engine::native_rear;
namespace native_rear=nimby::engine::native_rear;
namespace {
std::atomic<bool> watchAllocations{};
std::atomic<size_t> allocations{};
size_t checks{};
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);} while(false)
constexpr uint64_t id(unsigned type,uint64_t ordinal=1){return (uint64_t(type)<<48)|(ordinal<<16)|1;}
constexpr uint64_t train=id(5),a=id(1),b=id(1,2),c=id(1,3);
constexpr Publication tick{1,2,0,Phase::RetainedMovement};
struct Fixture {
    std::array<HistorySection,3> history{{
        {a,10,.875,.125,1000,-1,0,HistoryEnd::Continues,false,0,0,0},
        {b,11,.25,1,400,1,1,HistoryEnd::Continues,true,a,.125,0},
        {c,12,.8,.2,1000,-1,2,HistoryEnd::QualifiedEnd,true,b,1,1}
    }};
    Observation value;
    std::array<Portion,6> output{};
    Fixture() {
        value.consist={train,7,850,850,tick,true,true,true};
        value.history={train,7,tick,true,true,history};
        value.cursor={train,a,0,.625,-1,0,tick,true};
        value.stablePublicationQualified=true;
        for(auto& row:output)row={id(5,99),id(1,99),.01,.02,123,2};
    }
    void length(double meters) {value.consist.lengthMeters=meters;value.consist.nativeLengthMeters=float(meters);}
    Result assess(Limits limits={}) {return native_rear::assess(value,tick,output,limits);}
    // Every failure must discard the whole observation, without modifying the
    // caller's previous owned footprint while a replacement is unqualified.
    void unknown(Reason reason,Limits limits={}) {
        const auto result=assess(limits);
        CHECK(!result.complete()&&result.reason==reason&&result.portions==0);
        for(const auto& row:output)CHECK(row.train==id(5,99)&&row.track==id(1,99)&&row.lengthMeters==123);
    }
};
void branchesAndEndpoints() {
    Fixture f;auto result=f.assess();CHECK(result.complete()&&result.portions==3&&result.visitedSections==3);
    CHECK(result.train==train&&result.consistRevision==7&&result.at==tick);
    CHECK(result.requestedMeters==850&&result.coveredMeters==850&&result.remainingMeters==0);
    CHECK(f.output[0].track==a&&f.output[0].from==.625&&f.output[0].to==.125&&f.output[0].lengthMeters==500);
    CHECK(f.output[1].track==b&&f.output[1].from==.25&&f.output[1].to==1&&f.output[1].lengthMeters==300);
    CHECK(f.output[2].track==c&&std::abs(f.output[2].to-.75)<1e-14&&f.output[2].lengthMeters>=50);
    CHECK(f.output[0].orientation==0&&f.output[1].orientation==1&&f.output[2].orientation==2);
    for(double length:{499.,500.,800.}) {
        Fixture boundary;boundary.length(length);result=boundary.assess();CHECK(result.complete());
        if(length==499)CHECK(result.portions==1);
        if(length==500)CHECK(result.portions==2&&boundary.output[1].from==.25&&boundary.output[1].to==.25);
        if(length==800)CHECK(result.portions==3&&boundary.output[2].from==.8&&boundary.output[2].to==.8);
    }
    // Decimal endpoints .8/.2 do not have an exactly 600 m binary span.
    // Use exactly representable fractions for the QualifiedEnd contract.
    Fixture exactEnd;exactEnd.history[2].from=.875;exactEnd.history[2].to=.25;
    exactEnd.length(1425);result=exactEnd.assess();
    CHECK(result.complete()&&result.portions==3&&exactEnd.output[2].to==.25);
    // A point at the current track's endpoint is retained when the head has
    // not yet changed track. No positive interval is fabricated for that point.
    Fixture point;point.value.cursor.fraction=.125;point.length(350);result=point.assess();
    CHECK(result.complete()&&result.portions==3&&point.output[0].from==.125&&point.output[0].to==.125);
    CHECK(point.output[0].lengthMeters==0&&point.output[2].lengthMeters>=50);
    // The cursor can designate a later retained section; earlier history is
    // not projected onto the body or added to the coverage length.
    Fixture later;later.value.cursor={train,b,1,.5,1,1,tick,true};later.length(250);result=later.assess();
    CHECK(result.complete()&&result.portions==2&&later.output[0].track==b&&later.output[0].lengthMeters==200);
}
void completenessAndIdentity() {
    for(unsigned change=0;change<9;++change) {
        Fixture f;
        if(change==0)f.value.stablePublicationQualified=false;
        if(change==1)f.value.consist.complete=false;
        if(change==2)f.value.consist.lengthQualified=false;
        if(change==3)f.value.consist.nativeLengthQualified=false;
        if(change==4)f.value.history.qualified=false;
        if(change==5)f.value.history.continuityQualified=false;
        if(change==6)f.value.cursor.qualified=false;
        if(change==7)++f.value.history.consistRevision;
        if(change==8)f.value.consist.revision=0;
        constexpr std::array reasons{Reason::MissingStablePublication,Reason::IncompleteConsist,Reason::UnqualifiedLength,
            Reason::UnqualifiedNativeLength,Reason::UnqualifiedHistory,Reason::UnqualifiedContinuity,Reason::UnqualifiedCursor,
            Reason::ConsistChanged,Reason::InvalidConsistRevision};
        f.unknown(reasons[change]);
    }
    for(unsigned field=0;field<3;++field)for(unsigned change=0;change<4;++change) {
        Fixture f;auto& at=field==0?f.value.consist.at:field==1?f.value.history.at:f.value.cursor.at;
        if(change==0)++at.session;
        if(change==1)++at.epoch;
        if(change==2)++at.tick;
        if(change==3)at.phase=Phase::BeforeMovement;
        f.unknown(Reason::DifferentPublication);
    }
    for(uint64_t wrong:{uint64_t{0},a,id(5,0),std::numeric_limits<uint64_t>::max()}) {
        // A generation-only type tag is not a full nonzero record identity.
        if(wrong==id(5,0))wrong=uint64_t{5}<<48;
        Fixture f;f.value.consist.train=wrong;f.unknown(Reason::InvalidTrainIdentity);
    }
    for(unsigned field=0;field<2;++field) {
        Fixture f;
        if(field==0)f.value.history.train=train+(1ULL<<16);else f.value.cursor.train=train+(1ULL<<16);
        f.unknown(Reason::TrainIdentityChanged); // Same type, another complete incarnation.
    }
    Fixture revision;revision.value.history.consistRevision=0;revision.unknown(Reason::InvalidConsistRevision);
    Fixture current;auto invalid=tick;invalid.phase=Phase::Unknown;
    CHECK(native_rear::assess(current.value,invalid,current.output).reason==Reason::InvalidPublication);
}
void floatAndRemainingLength() {
    Fixture lower;lower.value.consist.lengthMeters=850.00002;lower.value.consist.nativeLengthMeters=850;
    auto result=lower.assess();CHECK(result.complete()&&result.requestedMeters==850.00002);
    CHECK(lower.output[2].lengthMeters>=lower.value.consist.lengthMeters-800);
    Fixture higher;higher.value.consist.lengthMeters=850;
    higher.value.consist.nativeLengthMeters=std::nextafter(850.f,1000.f);
    result=higher.assess();CHECK(result.complete()&&result.requestedMeters==double(higher.value.consist.nativeLengthMeters));
    CHECK(higher.output[2].lengthMeters>50);
    // Native float accumulation is not used as a reason to drop a positive
    // remainder from the qualified exact length, however small that remainder.
    Fixture shortTrace;shortTrace.history[2].from=.875;shortTrace.history[2].to=.25;
    shortTrace.length(1425);shortTrace.value.consist.lengthMeters=std::nextafter(1425.,1500.);
    result=shortTrace.assess();CHECK(!result.complete()&&result.reason==Reason::HistoryTooShort&&result.remainingMeters>0);
    for(double invalid:{0.,-1.,std::numeric_limits<double>::quiet_NaN(),std::numeric_limits<double>::infinity()}) {
        Fixture f;f.value.consist.lengthMeters=invalid;f.unknown(Reason::InvalidLength);
    }
    for(float invalid:{0.f,-1.f,std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::infinity()}) {
        Fixture f;f.value.consist.nativeLengthMeters=invalid;f.unknown(Reason::InvalidLength);
    }
}
void conservativeArithmetic() {
    // The rounded product is greater than the exact distance of these double
    // endpoints. Equality with that product must not certify a complete body.
    Fixture single;single.history[0]={a,10,.8,.2,1000,-1,0,HistoryEnd::QualifiedEnd};
    single.value.history.sections=std::span(single.history).first(1);
    single.value.cursor.fraction=.8;single.length(600);
    single.value.consist.lengthMeters=(.8-.2)*1000;
    auto result=single.assess();CHECK(!result.complete()&&result.reason==Reason::HistoryTooShort&&result.remainingMeters>0);
    single.unknown(Reason::HistoryTooShort);
    // Repeating sections must carry the upper remaining length, not accumulate
    // optimistic subtraction. The older track has positive body, not a point.
    Fixture continued;continued.history[0]={a,10,.8,.2,1000,-1,0,HistoryEnd::Continues};
    continued.history[1]={b,11,0,1,1,1,0,HistoryEnd::QualifiedEnd,true,a,.2,0};
    continued.value.history.sections=std::span(continued.history).first(2);
    continued.value.cursor.fraction=.8;continued.length(600);
    continued.value.consist.lengthMeters=(.8-.2)*1000;
    result=continued.assess();CHECK(result.complete()&&result.portions==2&&continued.output[1].lengthMeters>0);
    CHECK(continued.output[1].to>=1e-13);
    continued.history[1].metricMeters=1000;continued.length(650);result=continued.assess();
    CHECK(result.complete()&&continued.output[1].to>=.05);
    // Both partial travel directions bound division/addition outward. Every
    // positive interval includes a point at the rounded nominal tail as well.
    for(int direction:{-1,1}) {
        Fixture partial;partial.history[0]={a,10,direction>0?0.:1.,direction>0?1.:0.,1000,
            int8_t(direction),0,HistoryEnd::QualifiedEnd};
        partial.value.history.sections=std::span(partial.history).first(1);
        partial.value.cursor.fraction=direction>0?0:1;partial.value.cursor.rearDirection=int8_t(direction);
        partial.length(50);result=partial.assess();CHECK(result.complete());
        CHECK(direction>0?partial.output[0].to>.05:partial.output[0].to<.95);
    }
    // Very small products cannot use an underflowed zero FMA residual as an
    // exact-distance witness. Failed arithmetic leaves prior output untouched.
    for(double metric:{std::numeric_limits<double>::min(),0x1p-980}) {
        Fixture tiny;tiny.history[0].metricMeters=metric;tiny.unknown(Reason::ArithmeticUnqualified);
    }
    Fixture tinyMetric;tinyMetric.history[0].metricMeters=std::numeric_limits<double>::denorm_min();
    tinyMetric.unknown(Reason::ArithmeticUnqualified);
    Fixture tinyFraction;tinyFraction.value.cursor.fraction=std::numeric_limits<double>::denorm_min();
    tinyFraction.history[0].to=0;tinyFraction.unknown(Reason::ArithmeticUnqualified);
    Fixture tinyNative;tinyNative.value.consist.nativeLengthMeters=std::numeric_limits<float>::denorm_min();
    tinyNative.unknown(Reason::ArithmeticUnqualified);
    Fixture quotient;quotient.history[0]={a,10,0,1,std::numeric_limits<double>::max(),1,0,HistoryEnd::QualifiedEnd};
    quotient.value.history.sections=std::span(quotient.history).first(1);
    quotient.value.cursor.fraction=0;quotient.value.cursor.rearDirection=1;
    quotient.length(double(std::numeric_limits<float>::min()));quotient.unknown(Reason::ArithmeticUnqualified);
    const auto originalRounding=std::fegetround();
    if(std::fesetround(FE_DOWNWARD)==0) {
        Fixture changed;result=changed.assess();std::fesetround(originalRounding);
        CHECK(!result.complete()&&result.reason==Reason::ArithmeticUnqualified);
    }
#if defined(__SSE2__) || defined(_M_X64)
    const auto originalControl=_mm_getcsr();
    for(unsigned flag:{0x8000u,0x0040u,0x2000u,0x4000u,0x6000u}) {
        // FTZ/DAZ and direct SSE rounding changes, independently of x87.
        Fixture flush;_mm_setcsr(originalControl|flag);result=flush.assess();_mm_setcsr(originalControl);
        CHECK(!result.complete()&&result.reason==Reason::ArithmeticUnqualified);
    }
    _mm_setcsr((originalControl|0x1f80u|0x003fu)&~(0x6000u|0x8000u|0x0040u));
    const auto withStatus=_mm_getcsr();const bool qualified=detail::arithmeticQualified();const auto retainedStatus=_mm_getcsr();
    _mm_setcsr(originalControl);
    CHECK(qualified&&retainedStatus==withStatus); // No probe clears or raises status bits.
    _mm_setcsr(originalControl|0x4000u); // SSE upward while x87 can still be nearest.
    const auto sseRounding=_mm_getcsr();const auto x87Reported=std::fegetround();
    const bool directRoundingQualified=detail::arithmeticQualified();const auto retainedRounding=_mm_getcsr();
    _mm_setcsr(originalControl);
    CHECK(!directRoundingQualified&&retainedRounding==sseRounding);
    if(x87Reported==FE_TONEAREST)CHECK(!directRoundingQualified);
#endif
    CHECK(detail::arithmeticQualified());
}
void brokenHistory() {
    Fixture shortTrace;shortTrace.value.history.sections=std::span(shortTrace.history).first(2);shortTrace.unknown(Reason::HistoryTooShort);
    Fixture endpoint;endpoint.history[2].from=.875;endpoint.history[2].to=.25;
    endpoint.length(1425);endpoint.history[2].end=HistoryEnd::Unknown;endpoint.unknown(Reason::EndpointUnqualified);
    Fixture noJoin;noJoin.history[1].joinQualified=false;noJoin.unknown(Reason::MissingTransition);
    for(unsigned change=0;change<3;++change) {
        Fixture f;
        if(change==0)f.history[1].previousTrack=a+(1ULL<<16);
        if(change==1)f.history[1].previousFraction=std::nextafter(.125,.2);
        if(change==2)f.history[1].previousOrientation=1;
        f.unknown(Reason::TransitionMismatch);
    }
    for(unsigned change=0;change<3;++change) {
        Fixture f;
        if(change==0)f.history[1].sequence=f.history[0].sequence;
        if(change==1)f.history[1].sequence=0;
        if(change==2)f.history[0].sequence=std::numeric_limits<uint64_t>::max();
        f.unknown(Reason::InvalidSequence);
    }
    Fixture same;same.history[1]={a,11,.125,0,1000,-1,0,HistoryEnd::QualifiedEnd,true,a,.125,0};same.length(550);
    CHECK(same.assess().complete());
    Fixture gap;gap.history[1]={a,11,std::nextafter(.125,.2),0,1000,-1,0,HistoryEnd::QualifiedEnd,true,a,.125,0};gap.length(550);
    gap.unknown(Reason::HistoryGap); // No rounding epsilon can bridge an unobserved hole.
    Fixture reverse;reverse.history[1]={a,11,.125,.875,1000,1,0,HistoryEnd::QualifiedEnd,true,a,.125,0};
    reverse.unknown(Reason::UnqualifiedReversal);
    Fixture duplicate;duplicate.history[0].from=duplicate.history[0].to=.125;duplicate.value.cursor.fraction=.125;
    duplicate.history[1]={a,11,.125,.125,1000,-1,0,HistoryEnd::Continues,true,a,.125,0};duplicate.unknown(Reason::DuplicateSection);
    // Full track identities are never ordinal-masked into the cursor's track.
    Fixture recycled;recycled.history[0].track=a+(1ULL<<16);recycled.unknown(Reason::CursorMismatch);
    Fixture cursor;cursor.value.cursor.fraction=.95;cursor.unknown(Reason::CursorMismatch);
    Fixture orientation;orientation.value.cursor.orientation=1;orientation.unknown(Reason::CursorMismatch);
    Fixture direction;direction.value.cursor.rearDirection=1;direction.unknown(Reason::DirectionMismatch);
    for(unsigned field=0;field<4;++field) {
        Fixture f;
        if(field==0)f.history[1].track=train;
        if(field==1)f.history[1].orientation=3;
        if(field==2)f.history[1].metricMeters=std::numeric_limits<double>::infinity();
        if(field==3)f.history[1].from=std::numeric_limits<double>::quiet_NaN();
        constexpr std::array reasons{Reason::InvalidTrackIdentity,Reason::InvalidOrientation,Reason::InvalidGeometry,Reason::InvalidGeometry};
        f.unknown(reasons[field]);
    }
}
void longTraceAndBudgets() {
    std::array<HistorySection,256> history{};std::array<Portion,256> output{};
    for(size_t i=0;i<history.size();++i) {
        const bool forward=i%2==0;
        history[i]={id(1,i+1),i+1,forward?.25:.75,forward?.75:.25,20,int8_t(forward?1:-1),uint8_t(i%3),
            i+1==history.size()?HistoryEnd::QualifiedEnd:HistoryEnd::Continues,i!=0,
            i?id(1,i):0,i?history[i-1].to:0,uint8_t(i?history[i-1].orientation:0)};
    }
    Observation value{{train,1,850,850,tick,true,true,true},{train,1,tick,true,true,history},
        {train,a,0,.25,1,0,tick,true},true};
    auto result=native_rear::assess(value,tick,output);
    CHECK(result.complete()&&result.portions==86&&result.visitedSections==86);
    CHECK(output[84].lengthMeters==10&&output[85].lengthMeters==0&&output[85].track==id(1,86));
    // The capacity includes the adjacent endpoint, not just positive intervals.
    CHECK(native_rear::assess(value,tick,std::span(output).first(85)).reason==Reason::OutputCapacity);
    for(unsigned budget=0;budget<4;++budget) {
        Limits limits;
        if(budget==0)limits.historySections=255;
        if(budget==1)limits.visitedSections=85;
        if(budget==2)limits.work=257;
        if(budget==3)limits.outputPortions=85;
        result=native_rear::assess(value,tick,output,limits);
        CHECK(!result.complete()&&result.portions==0);
        CHECK(result.reason==(budget==3?Reason::OutputCapacity:Reason::BudgetExceeded));
    }
    const auto measured=native_rear::assess(value,tick,output);
    Limits exact;exact.historySections=256;exact.visitedSections=86;exact.outputPortions=86;exact.work=measured.work;
    CHECK(native_rear::assess(value,tick,output,exact).complete());
    exact.work=0;CHECK(native_rear::assess(value,tick,output,exact).reason==Reason::InvalidLimits);
    exact={};exact.historySections=4097;CHECK(native_rear::assess(value,tick,output,exact).reason==Reason::InvalidLimits);
    // Real repeated track visits are permitted when separately qualified by
    // distinct consecutive history records and explicit connections.
    history[2].track=a;history[2].orientation=history[0].orientation;
    history[3].previousTrack=a;history[3].previousOrientation=history[2].orientation;
    CHECK(native_rear::assess(value,tick,output).complete());
    history[2].metricMeters=21;
    CHECK(native_rear::assess(value,tick,output).reason==Reason::TransitionMismatch);
    history[2].metricMeters=20;history[2].orientation=1;
    CHECK(native_rear::assess(value,tick,output).reason==Reason::TransitionMismatch);
    history[2].orientation=history[0].orientation;
    Limits unique;unique.uniqueTracks=84;
    CHECK(native_rear::assess(value,tick,output,unique).reason==Reason::BudgetExceeded);
    allocations=0;watchAllocations=true;
    bool allComplete=true;
    for(unsigned i=0;i<1000;++i)allComplete&=native_rear::assess(value,tick,output).complete();
    watchAllocations=false;CHECK(allComplete&&allocations.load()==0);
}
}
void* operator new(std::size_t size) {
    if(watchAllocations.load(std::memory_order_relaxed))allocations.fetch_add(1,std::memory_order_relaxed);
    if(auto* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size){return ::operator new(size);}
void operator delete(void* value) noexcept {std::free(value);}
void operator delete[](void* value) noexcept {std::free(value);}
void operator delete(void* value,std::size_t) noexcept {std::free(value);}
void operator delete[](void* value,std::size_t) noexcept {std::free(value);}
int main() {
    try {branchesAndEndpoints();completenessAndIdentity();floatAndRemainingLength();conservativeArithmetic();brokenHistory();longTraceAndBudgets();
        std::cout<<"qualified aggregate rear coverage: PASS ("<<checks<<" checks)\n";return 0;
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
