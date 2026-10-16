// Portable synthetic fixture only. No game, memory inspection, native hooks,
// runtime ABI, native provider or universal writer ownership is certified.
#include <platform/windows/runtime/native_admission_authority.h>
#include <bit>
#include <cfenv>
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>

namespace {
std::atomic<bool> watchAllocations{};
std::atomic<size_t> allocations{};
std::atomic<int> failAllocation{-1};
size_t checks{};
}
#if defined(__GNUC__)
#define ADMISSION_NOINLINE __attribute__((noinline))
#else
#define ADMISSION_NOINLINE __declspec(noinline)
#endif
ADMISSION_NOINLINE void* operator new(size_t size) {
    if(watchAllocations.load())++allocations;
    const int fail=failAllocation.load();
    if(fail>=0&&failAllocation.fetch_sub(1)==0)throw std::bad_alloc();
    if(void* result=std::malloc(size?size:1))return result;
    throw std::bad_alloc();
}
ADMISSION_NOINLINE void* operator new[](size_t size){return ::operator new(size);}
ADMISSION_NOINLINE void operator delete(void* value) noexcept {std::free(value);}
ADMISSION_NOINLINE void operator delete[](void* value) noexcept {std::free(value);}
ADMISSION_NOINLINE void operator delete(void* value,size_t) noexcept {std::free(value);}
ADMISSION_NOINLINE void operator delete[](void* value,size_t) noexcept {std::free(value);}
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);} while(false)

namespace nimby::windows::admission {
struct NativeAdmissionAuthorityTestAccess {
    static bool lock(NativeAdmissionAuthority& authority) noexcept {
        return !authority.gate_.test_and_set(std::memory_order_acquire);
    }
    static void unlock(NativeAdmissionAuthority& authority) noexcept {authority.gate_.clear(std::memory_order_release);}
    static void exhaustSerial(NativeAdmissionAuthority& authority) noexcept {authority.serial_=UINT64_MAX;}
};
}
namespace {
using namespace nimby::windows::admission;
constexpr uint64_t id(unsigned type,uint64_t ordinal=1) {return (uint64_t(type)<<48)|(ordinal<<16)|1;}
constexpr auto t1=id(1),t2=id(1,2),t3=id(1,3),train=id(5),source=id(8);
constexpr NativeDecision allowed{true,true,NativeDomain::FinalAdmission};
struct Fixture {
    NativeAdmissionAuthority authority;
    Frame frame{1,1,100,{1,1,1,rear::Phase::BeforeMovement}};
    explicit Fixture(Limits limits={8,64,1024,1000}):authority(limits) {
        CHECK(authority.beginWorld(frame,ready()).reason==Reason::Ready);
    }
    // Synthetic completed owner for the older arithmetic/index contract. Real
    // owner loss, frame/thread matching and mid-call expiry are exercised by
    // native_admission_ownership.cpp, not inferred from this fixture predicate.
    static bool current(void*,const PublicationWitness&) noexcept {return true;}
    PublicationWitness publication() const {return {frame.lineage,frame.window,frame.at,true,true,current,nullptr};}
    WorldReadyWitness ready() const {return {publication(),true,true,true};}
    void advance(rear::Phase phase=rear::Phase::BeforeMovement) {
        ++frame.window;++frame.nowMs;++frame.at.tick;frame.at.phase=phase;
    }
    AdmissionRequest request(uint64_t ordinal=1,uint64_t track=t1) const {
        AdmissionRequest r;r.ticket={frame.at.session,id(5,ordinal),source,7};
        r.publication=publication();r.capturedAtMs=frame.nowMs;r.consistRevision=3;
        r.routeComplete=r.conflictsComplete=r.physicalOccupationComplete=r.scriptVetoComplete=true;
        r.physicalClear=r.scriptAllows=Proof::Yes;r.entered=Proof::No;
        r.resourceCount=r.physicalCount=1;r.resources[0]=r.ownPhysical[0]={track,.1,.3};return r;
    }
    Result admit(const AdmissionRequest& r,Mode mode=Mode::FinalVeto,NativeDecision native=allowed) {
        return authority.admit(frame,r,native,mode);
    }
    EntryWitness entry(const AdmissionRequest& r,const Result& admitted) const {
        return {r.ticket,admitted.leaseSerial,r.consistRevision,publication(),true,true,true,true};
    }
};
struct Body {
    std::array<rear::HistorySection,1> sections{{{t3,10,.875,.125,1000,-1,0,rear::HistoryEnd::QualifiedEnd,false,0,0,0}}};
    rear::Observation observation;
    Body(const Fixture& f,const AdmissionRequest& r):observation{
        {r.ticket.train,r.consistRevision,100,100,f.frame.at,true,true,true},
        {r.ticket.train,r.consistRevision,f.frame.at,true,true,sections},
        {r.ticket.train,t3,0,.625,-1,0,f.frame.at,true},true} {}
};
void prerequisitesAndDomains() {
    Fixture f;auto request=f.request();
    const auto check=[&](auto change,Reason reason) {
        auto bad=request;change(bad);const auto result=f.admit(bad);
        CHECK(result.reason==reason&&!result.mayPass&&result.leaseHeld==Proof::No);
    };
    check([](auto& r){r.publication.lineageOwned=false;},Reason::UnqualifiedLineage);
    check([](auto& r){r.publication.phaseOwned=false;},Reason::UnqualifiedPhase);
    check([](auto& r){++r.publication.window;},Reason::DifferentPublication);
    check([](auto& r){++r.publication.lineage;},Reason::LineageMismatch);
    check([](auto& r){r.routeComplete=false;},Reason::IncompleteRoute);
    check([](auto& r){r.conflictsComplete=false;},Reason::IncompleteConflicts);
    check([](auto& r){r.physicalOccupationComplete=false;},Reason::IncompleteOccupation);
    check([](auto& r){r.physicalClear=Proof::Unknown;},Reason::OccupationUnknown);
    check([](auto& r){r.physicalClear=Proof::No;},Reason::Occupied);
    check([](auto& r){r.scriptVetoComplete=false;},Reason::IncompleteVeto);
    check([](auto& r){r.scriptAllows=Proof::Unknown;},Reason::VetoUnknown);
    check([](auto& r){r.scriptAllows=Proof::No;},Reason::ScriptVeto);
    check([](auto& r){r.ticket.train=id(4);},Reason::InvalidTicket);
    check([](auto& r){r.consistRevision=0;},Reason::InvalidConsist);
    check([](auto& r){r.capturedAtMs=UINT64_MAX;},Reason::StaleCapture);
    check([](auto& r){r.resources[0].end=std::numeric_limits<double>::quiet_NaN();},Reason::InvalidIntervals);
    check([](auto& r){r.ownPhysical[0].end=.4;},Reason::PhysicalNotProtected);
    check([](auto& r){r.entered=Proof::Unknown;},Reason::EntryUnknown);
    CHECK(f.admit(request,Mode::FinalVeto,{false,true,NativeDomain::FinalAdmission}).reason==Reason::NativeRefusal);
    CHECK(f.admit(request,Mode::FinalVeto,{true,false,NativeDomain::FinalAdmission}).reason==Reason::NativeDecisionUnqualified);
    CHECK(f.admit(request,Mode::FinalVeto,{true,true,NativeDomain::ExclusiveReservationOnly}).reason==Reason::NativeDomainMismatch);
    CHECK(f.admit(request,Mode::ExclusiveSubstitution,{false,true,NativeDomain::FinalAdmission}).reason==Reason::NativeDomainMismatch);
    CHECK(f.admit(request,Mode::ExclusiveSubstitution,{false,true,NativeDomain::ExclusiveReservationOnly}).reason==Reason::NativeCompatibilityMissing);
    for(unsigned bit=0;bit<5;++bit) {
        auto incomplete=request;incomplete.compatibility={true,true,true,true,true};
        if(bit==0)incomplete.compatibility.abi=false;
        if(bit==1)incomplete.compatibility.allAdmissionSites=false;
        if(bit==2)incomplete.compatibility.allExclusiveReaders=false;
        if(bit==3)incomplete.compatibility.allExclusiveWriters=false;
        if(bit==4)incomplete.compatibility.writerOwnership=false;
        CHECK(f.admit(incomplete,Mode::ExclusiveSubstitution,{false,true,NativeDomain::ExclusiveReservationOnly}).reason==Reason::NativeCompatibilityMissing);
    }
    auto result=f.admit(request,Mode::ParallelAudit);CHECK(result.reason==Reason::AuditOnly&&!result.mayPass&&result.leaseHeld==Proof::Yes);
    result=f.admit(request);CHECK(result.reason==Reason::Granted&&result.mayPass&&!result.replaceNativeExclusive);
    Fixture replacement;auto full=replacement.request();full.compatibility={true,true,true,true,true};
    result=replacement.admit(full,Mode::ExclusiveSubstitution,{false,true,NativeDomain::ExclusiveReservationOnly});
    CHECK(result.mayPass&&result.replaceNativeExclusive&&!result.nativeReplacementConnected);
}
void atomicClaimsAndIdentity() {
    Fixture f;auto blocker=f.request(2,t2);CHECK(f.admit(blocker).mayPass);
    auto both=f.request();both.resourceCount=2;both.resources[1]={t2,.1,.3};
    auto result=f.admit(both);CHECK(result.reason==Reason::Conflict&&!result.mayPass&&result.blocker==blocker.ticket&&result.track==t2);
    // Failure on the second resource cannot leave a partial claim on the first.
    CHECK(f.admit(f.request(3,t1)).mayPass);
    auto other=f.request(4,t1);result=f.admit(other);CHECK(result.reason==Reason::Conflict);
    Fixture endpoint;auto first=endpoint.request();CHECK(endpoint.admit(first).mayPass);
    auto contact=endpoint.request(2);contact.resources[0]=contact.ownPhysical[0]={t1,.3,.5};
    CHECK(endpoint.admit(contact).reason==Reason::Conflict);
    contact.resources[0]=contact.ownPhysical[0]={t1,std::nextafter(.3,1.),.5};
    CHECK(endpoint.admit(contact).mayPass);
    // Consecutive signals on the SAME full train ID may protect shared track.
    auto same=first;same.ticket.source=id(8,2);CHECK(endpoint.admit(same).mayPass);
    auto generation=first;++generation.ticket.train;CHECK(endpoint.admit(generation).reason==Reason::Conflict);
    auto unprotected=first;unprotected.resourceCount=2;unprotected.resources[1]={t1,.2,.5};
    CHECK(endpoint.admit(unprotected).reason==Reason::InvalidIntervals);
}
void arithmeticQualification() {
    // Tests change controls; production only observes the caller's FP state.
    const auto originalRound=std::fegetround();
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP>=2)
    const auto originalControls=_mm_getcsr();
    struct Restore {
        int rounding;unsigned controls;
        ~Restore(){std::fesetround(rounding);_mm_setcsr(controls);}
    } restore{originalRound,originalControls};
    CHECK(std::fesetround(FE_TONEAREST)==0);
    _mm_setcsr((_mm_getcsr()&~(0x6000u|0x8000u|0x0040u))|0x1f80u);
    CHECK(rear::detail::arithmeticQualified());
    const auto strictControls=_mm_getcsr();
    // DAZ previously admitted -denorm_min as zero. FTZ and independent SSE
    // rounding are unqualified even with normal finite fractions.
    for(unsigned mode:{0x0040u,0x8000u,0x8040u,0x2000u,0x4000u,0x6000u}) {
        Fixture f;auto request=f.request();
        if(mode&0x0040u)request.resources[0].begin=request.ownPhysical[0].begin=
            std::bit_cast<double>(UINT64_C(0x8000000000000001));
        _mm_setcsr(strictControls|mode);
        const auto before=_mm_getcsr();const auto roundBefore=std::fegetround();
        const auto result=f.admit(request);const auto after=_mm_getcsr();
        const auto roundAfter=std::fegetround();_mm_setcsr(strictControls);
        CHECK(!result.mayPass&&!result.replaceNativeExclusive&&result.leaseHeld==Proof::No);
        CHECK(result.reason==Reason::ArithmeticUnqualified);
        CHECK(after==before&&roundAfter==roundBefore);
        RetainedSnapshot held;CHECK(f.authority.snapshot(request.ticket,held).reason==Reason::Missing);
        request=f.request();CHECK(f.admit(request).mayPass); // No partial claim.
        _mm_setcsr(strictControls|mode);const auto retainedBefore=_mm_getcsr();
        const auto retained=f.admit(request);const auto retainedAfter=_mm_getcsr();
        _mm_setcsr(strictControls);
        CHECK(retained.reason==Reason::ArithmeticUnqualified&&!retained.mayPass&&retained.leaseHeld==Proof::Yes);
        CHECK(retainedAfter==retainedBefore);
        CHECK(f.admit(f.request(2)).reason==Reason::Conflict); // Protection held.
    }
    for(int mode:{FE_DOWNWARD,FE_UPWARD,FE_TOWARDZERO}) {
        Fixture f;auto request=f.request();CHECK(std::fesetround(mode)==0);
        const auto controlsBefore=_mm_getcsr();
        const auto result=f.admit(request);const auto controlsAfter=_mm_getcsr();
        const auto roundAfter=std::fegetround();CHECK(std::fesetround(FE_TONEAREST)==0);
        _mm_setcsr(strictControls);
        CHECK(result.reason==Reason::ArithmeticUnqualified&&!result.mayPass&&result.leaseHeld==Proof::No);
        CHECK(roundAfter==mode&&controlsAfter==controlsBefore);
    }
    // Reject either subnormal sign and nonfinite endpoints before comparison.
    // Unmask invalid/denormal exceptions to catch accidental FP classification.
    for(uint64_t bits:{UINT64_C(0x0000000000000001),UINT64_C(0x8000000000000001),
                      UINT64_C(0x000fffffffffffff),UINT64_C(0x800fffffffffffff),
                      UINT64_C(0x7ff0000000000000),UINT64_C(0xfff0000000000000),
                      UINT64_C(0x7ff8000000000001),UINT64_C(0x7ff0000000000001)}) {
        for(unsigned endpoint=0;endpoint<4;++endpoint) {
            Fixture f;auto request=f.request();const auto value=std::bit_cast<double>(bits);
            if(endpoint==0)request.resources[0].begin=value;
            if(endpoint==1)request.resources[0].end=value;
            if(endpoint==2)request.ownPhysical[0].begin=value;
            if(endpoint==3)request.ownPhysical[0].end=value;
            _mm_setcsr(strictControls&~(0x0080u|0x0100u|0x003fu));
            const auto before=_mm_getcsr();const auto result=f.admit(request);const auto after=_mm_getcsr();
            _mm_setcsr(strictControls);
            CHECK(result.reason==Reason::InvalidIntervals&&!result.mayPass&&result.leaseHeld==Proof::No);
            CHECK(after==before);
            CHECK(f.admit(f.request(2)).mayPass);
        }
    }
    for(double invalid:{-.1,std::nextafter(1.,2.)}) {
        Fixture f;auto request=f.request();request.resources[0].begin=request.ownPhysical[0].begin=invalid;
        CHECK(f.admit(request).reason==Reason::InvalidIntervals);
    }
    for(double zero:{0.,-0.}) {
        Fixture f;auto request=f.request();request.resources[0]=request.ownPhysical[0]={t1,zero,1.};
        CHECK(f.admit(request).mayPass);
    }
    Fixture tiny;auto request=tiny.request();request.resources[0].begin=request.ownPhysical[0].begin=
        std::numeric_limits<double>::min();
    CHECK(tiny.admit(request).mayPass); // Smallest NORMAL double is still valid.
#else
    (void)originalRound;
    Fixture f;CHECK(f.admit(f.request()).reason==Reason::ArithmeticUnqualified);
#endif
}
void retentionAndEntry() {
    Fixture f;auto request=f.request();const auto granted=f.admit(request);CHECK(granted.mayPass);
    CHECK(f.authority.retain(request.ticket).reason==Reason::Retained);
    auto changed=request;++changed.ticket.routeRevision;changed.resources[0]=changed.ownPhysical[0]={t2,.1,.3};
    CHECK(f.admit(changed).reason==Reason::RouteChangedRetained);
    CHECK(f.admit(f.request(2)).reason==Reason::Conflict);
    changed=request;++changed.consistRevision;CHECK(f.admit(changed).reason==Reason::ConsistChangedRetained);
    changed=request;changed.resources[0].end=.4;CHECK(f.admit(changed).reason==Reason::ResourcesChangedRetained);
    RetainedSnapshot retained;CHECK(f.authority.snapshot(request.ticket,retained).leaseHeld==Proof::Yes);
    CHECK(retained.ticket==request.ticket&&retained.entered==Proof::Unknown&&retained.resources[0]==request.resources[0]);
    f.advance(rear::Phase::RetainedMovement);auto entry=f.entry(request,granted);
    entry.nativeReturned=false;CHECK(f.authority.commitEntry(f.frame,entry).reason==Reason::UnqualifiedEntry);
    entry.nativeReturned=true;entry.movementCommitted=false;CHECK(f.authority.commitEntry(f.frame,entry).reason==Reason::UnqualifiedEntry);
    entry.movementCommitted=true;entry.retainedMirrorMatches=false;CHECK(f.authority.commitEntry(f.frame,entry).reason==Reason::UnqualifiedEntry);
    entry.retainedMirrorMatches=true;entry.identityAndRouteUnchanged=false;CHECK(f.authority.commitEntry(f.frame,entry).reason==Reason::UnqualifiedEntry);
    entry.identityAndRouteUnchanged=true;++entry.leaseSerial;CHECK(f.authority.commitEntry(f.frame,entry).reason==Reason::SnapshotChanged);
    --entry.leaseSerial;CHECK(f.authority.commitEntry(f.frame,entry).reason==Reason::Entered);
    CHECK(f.authority.snapshot(request.ticket,retained).reason==Reason::Retained&&retained.entered==Proof::Yes);
    f.advance();changed=request;changed.publication=f.publication();changed.capturedAtMs=f.frame.nowMs;
    CHECK(f.admit(changed).reason==Reason::RouteChangedRetained); // No renewal after entry.
    CHECK(f.admit(f.request(2)).reason==Reason::Conflict);
}
void rearNeedsCompleteProvider() {
    Fixture f;auto request=f.request();request.resourceCount=2;request.resources[1]={t3,.5,.7};
    const auto granted=f.admit(request);CHECK(granted.mayPass);
    f.advance(rear::Phase::RetainedMovement);CHECK(f.authority.commitEntry(f.frame,f.entry(request,granted)).reason==Reason::Entered);
    RetainedSnapshot retained;CHECK(f.authority.snapshot(request.ticket,retained).leaseHeld==Proof::Yes);
    Body body(f,request);
    const auto release=[&](bool routeQualified=true) {
        return f.authority.releaseRear(f.frame,f.publication(),retained,routeQualified,body.observation);
    };
    CHECK(release(false).reason==Reason::RearUnknown);
    body.observation.stablePublicationQualified=false;
    auto result=release();CHECK(result.reason==Reason::RearUnknown&&result.rearClearance.coverage.reason==rear::Reason::MissingStablePublication);
    body.observation.stablePublicationQualified=true;body.observation.history.continuityQualified=false;
    CHECK(release().reason==Reason::RearUnknown);body.observation.history.continuityQualified=true;
    body.observation.consist.complete=false;CHECK(release().reason==Reason::RearUnknown);body.observation.consist.complete=true;
    ++body.observation.cursor.at.tick;CHECK(release().reason==Reason::RearUnknown);--body.observation.cursor.at.tick;
    auto forged=retained;forged.ownPhysical[0]={t2,.1,.3};
    CHECK(f.authority.releaseRear(f.frame,f.publication(),forged,true,body.observation).reason==Reason::SnapshotChanged);
    auto wrongPhase=f.publication();wrongPhase.phaseOwned=false;
    CHECK(f.authority.releaseRear(f.frame,wrongPhase,retained,true,body.observation).reason==Reason::UnqualifiedPhase);
    // Head moved to another track, but a 700 m body still spans the old track.
    body.sections[0].track=t1;body.observation.cursor.track=t1;body.observation.consist.lengthMeters=700;
    body.observation.consist.nativeLengthMeters=700;CHECK(release().reason==Reason::RearUnknown); // Too short: no guessed continuation.
    body.observation.consist.lengthMeters=500;body.observation.consist.nativeLengthMeters=500;
    result=release();CHECK(result.reason==Reason::RearOccupied&&result.rearClearance.proof==Proof::No);
    body.sections[0].track=t3;body.observation.cursor.track=t3;
    body.observation.consist.lengthMeters=100;body.observation.consist.nativeLengthMeters=100;
    // Alias T3 is occupied; actual retained physical T1 is completely clear.
    result=release();CHECK(result.reason==Reason::Released&&result.rearClearance.coverage.complete()&&result.leaseHeld==Proof::No);
    CHECK(f.authority.snapshot(request.ticket,retained).reason==Reason::Missing);
    CHECK(release().reason==Reason::Missing); // No double release / inherited serial.
    f.advance();CHECK(f.admit(f.request(2)).mayPass);
}
void boundsAndWorldReady() {
    Fixture capacity({2,4,128,1000});CHECK(capacity.admit(capacity.request()).mayPass);
    CHECK(capacity.admit(capacity.request(2,t2)).mayPass);CHECK(capacity.admit(capacity.request(3,t3)).reason==Reason::Capacity);
    Fixture rowCapacity({8,2,128,1000});CHECK(rowCapacity.admit(rowCapacity.request()).mayPass);
    CHECK(rowCapacity.admit(rowCapacity.request(2,t2)).reason==Reason::Capacity);
    Fixture budget({8,64,16,1000});auto big=budget.request();big.resourceCount=big.physicalCount=4;
    for(size_t i=0;i<4;++i)big.resources[i]=big.ownPhysical[i]={id(1,i+1),.1,.3};
    const auto failed=budget.admit(big);CHECK(failed.reason==Reason::BudgetExceeded&&failed.work<=budget.authority.operationLimit());
    RetainedSnapshot empty;
    CHECK(budget.authority.snapshot(big.ticket,empty).reason==Reason::Missing);
    const auto tooShort=budget.admit(budget.request(2));
    CHECK(tooShort.reason==Reason::BudgetExceeded&&!tooShort.mayPass&&tooShort.work<=budget.authority.operationLimit());
    CHECK(budget.authority.snapshot(budget.request(2).ticket,empty).reason==Reason::Missing);
    // Cache staging is budgeted too: retry under a sufficient quota must not
    // inherit rows or an owner from a larger request that failed before commit.
    Fixture recovery({8,64,32,1000});auto large=recovery.request();large.resourceCount=large.physicalCount=4;
    for(size_t i=0;i<4;++i)large.resources[i]=large.ownPhysical[i]={id(1,i+1),.1,.3};
    const auto notPublished=recovery.admit(large);
    CHECK(notPublished.reason==Reason::BudgetExceeded&&!notPublished.mayPass);
    CHECK(recovery.authority.snapshot(large.ticket,empty).reason==Reason::Missing);
    CHECK(recovery.admit(recovery.request(2)).mayPass);
    Fixture f;auto request=f.request();const auto granted=f.admit(request);CHECK(granted.mayPass);
    auto ready=f.ready();ready.loadComplete=false;
    CHECK(f.authority.beginWorld(f.frame,ready).reason==Reason::InvalidWorldReady);
    CHECK(f.authority.beginWorld(f.frame,f.ready()).reason==Reason::AlreadyReady);
    CHECK(f.admit(f.request(2)).reason==Reason::Conflict);
    auto next=f.frame;++next.lineage;++next.window;++next.nowMs;
    auto witness=f.ready();witness.publication.lineage=next.lineage;witness.publication.window=next.window;
    CHECK(f.authority.beginWorld(next,witness).reason==Reason::LineageNotMonotonic); // Reused session prohibited.
    ++next.at.session;witness.publication.at=next.at;
    CHECK(f.authority.beginWorld(next,witness).reason==Reason::Ready);
    auto obsolete=request;obsolete.publication=witness.publication;
    CHECK(f.authority.admit(next,obsolete,allowed,Mode::FinalVeto).reason==Reason::InvalidTicket);
    CHECK(f.authority.beginWorld(f.frame,f.ready()).reason==Reason::LineageNotMonotonic);
    f.frame=next;auto fresh=f.request();const auto newLease=f.admit(fresh);
    CHECK(newLease.mayPass&&newLease.leaseSerial>granted.leaseSerial);
    f.advance();fresh=f.request(2,t2);auto backwards=f.frame;--backwards.window;--backwards.nowMs;
    CHECK(f.admit(fresh).mayPass);
    auto bad=f.request(3,t3);bad.publication.window=backwards.window;bad.publication.at=backwards.at;
    CHECK(f.authority.admit(backwards,bad,allowed,Mode::FinalVeto).reason==Reason::StaleWindow);
    Fixture exhausted;NativeAdmissionAuthorityTestAccess::exhaustSerial(exhausted.authority);
    CHECK(exhausted.admit(exhausted.request()).reason==Reason::SerialExhausted);
    for(unsigned allocation=0;allocation<6;++allocation) {
        failAllocation=static_cast<int>(allocation);bool threw=false;
        try {NativeAdmissionAuthority unused;}catch(const std::bad_alloc&){threw=true;}
        failAllocation=-1;CHECK(threw);
    }
    bool invalid=false;try {NativeAdmissionAuthority badLimits({8193,1,1,1});}catch(const std::invalid_argument&){invalid=true;}
    CHECK(invalid);
}
void originalExactlyOnceOutsideGate() {
    Fixture f;auto request=f.request();size_t originals=0,captures=0;Reason originalGate=Reason::Busy,captureGate=Reason::Busy;
    const auto original=[&] {
        ++originals;RetainedSnapshot snapshot;originalGate=f.authority.snapshot(request.ticket,snapshot).reason;return allowed;
    };
    const auto capture=[&](NativeDecision native) {
        ++captures;RetainedSnapshot snapshot;captureGate=f.authority.snapshot(request.ticket,snapshot).reason;
        CHECK(native.admitted);return request;
    };
    auto result=f.authority.invoke(f.frame,request.ticket,Mode::FinalVeto,original,capture);
    CHECK(result.mayPass&&originals==1&&captures==1&&originalGate==Reason::Missing&&captureGate==Reason::Missing);
    CHECK(NativeAdmissionAuthorityTestAccess::lock(f.authority));
    result=f.authority.invoke(f.frame,request.ticket,Mode::FinalVeto,original,capture);
    NativeAdmissionAuthorityTestAccess::unlock(f.authority);
    CHECK(result.reason==Reason::Busy&&!result.mayPass&&originals==2&&captures==2);
    result=f.authority.invoke(f.frame,request.ticket,Mode::FinalVeto,[&]() -> NativeDecision {++originals;throw 7;},capture);
    CHECK(result.reason==Reason::OriginalException&&!result.mayPass&&result.leaseHeld==Proof::Yes&&originals==3&&captures==2);
    result=f.authority.invoke(f.frame,request.ticket,Mode::FinalVeto,original,[&](auto) -> AdmissionRequest {++captures;throw 8;});
    CHECK(result.reason==Reason::CaptureException&&!result.mayPass&&originals==4&&captures==3);
    result=f.authority.invoke(f.frame,request.ticket,Mode::FinalVeto,original,[&](auto) -> AdmissionRequest {++captures;throw std::bad_alloc();});
    CHECK(result.reason==Reason::AllocationFailure&&!result.mayPass&&originals==5&&captures==4);
    RetainedSnapshot snapshot;CHECK(f.authority.snapshot(request.ticket,snapshot).leaseHeld==Proof::Yes);
    CHECK(f.admit(f.request(2)).reason==Reason::Conflict);
    Fixture unready;Frame wrong=unready.frame;wrong.lineage=0;
    result=unready.authority.invoke(wrong,request.ticket,Mode::FinalVeto,original,capture);
    CHECK(!result.mayPass&&originals==6&&captures==5);
    auto mismatch=request;++mismatch.ticket.routeRevision;
    result=f.authority.invoke(f.frame,request.ticket,Mode::FinalVeto,original,[&](auto){return mismatch;});
    CHECK(result.reason==Reason::IdentityChanged&&!result.mayPass);
}
void nonAllocatingAndConcurrent() {
    Fixture f;auto first=f.request(),second=f.request(2);Result a,b;std::atomic<bool> start{};
    std::thread one([&]{while(!start.load(std::memory_order_acquire))std::this_thread::yield();a=f.admit(first);});
    std::thread two([&]{while(!start.load(std::memory_order_acquire))std::this_thread::yield();b=f.admit(second);});
    start.store(true,std::memory_order_release);one.join();two.join();
    CHECK(a.mayPass!=b.mayPass);
    CHECK((!a.mayPass&&(a.reason==Reason::Busy||a.reason==Reason::Conflict))||(!b.mayPass&&(b.reason==Reason::Busy||b.reason==Reason::Conflict)));
    Fixture bounded;auto request=bounded.request();RetainedSnapshot retained;
    allocations=0;watchAllocations=true;
    const auto result=bounded.admit(request);
    const auto snap=bounded.authority.snapshot(request.ticket,retained);
    const auto keep=bounded.authority.retain(request.ticket);
    const auto wrapped=bounded.authority.invoke(bounded.frame,request.ticket,Mode::FinalVeto,[]{return allowed;},[&](auto){return request;});
    bounded.advance(rear::Phase::RetainedMovement);
    const auto entry=bounded.authority.commitEntry(bounded.frame,bounded.entry(request,result));
    const auto entered=bounded.authority.snapshot(request.ticket,retained);
    Body body(bounded,request);
    const auto release=bounded.authority.releaseRear(bounded.frame,bounded.publication(),retained,true,body.observation);
    watchAllocations=false;
    CHECK(result.mayPass&&snap.leaseHeld==Proof::Yes&&keep.reason==Reason::Retained&&wrapped.mayPass);
    CHECK(entry.reason==Reason::Entered&&entered.leaseHeld==Proof::Yes&&release.reason==Reason::Released);
    CHECK(allocations==0&&result.work<=bounded.authority.operationLimit());
    CHECK(bounded.authority.storageBytes()>0&&bounded.authority.storageBytes()<1024*1024);
}
}
int main() {
    try {
        static_assert(!std::is_copy_constructible_v<NativeAdmissionAuthority>);
        prerequisitesAndDomains();atomicClaimsAndIdentity();arithmeticQualification();retentionAndEntry();rearNeedsCompleteProvider();
        boundsAndWorldReady();originalExactlyOnceOutsideGate();nonAllocatingAndConcurrent();
        std::cout<<"native_admission_authority: "<<checks<<" checks PASS; synthetic contract only; native ABI/provider/hooks not certified\n";
        return 0;
    } catch(const std::exception& error) {
        watchAllocations=false;failAllocation=-1;std::cerr<<error.what()<<'\n';return 1;
    }
}
