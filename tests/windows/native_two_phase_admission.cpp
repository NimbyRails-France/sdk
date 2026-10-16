// Synthetic contract only: no hooks, native world reads or game invocation.
#include <platform/windows/runtime/native_two_phase_admission.h>
#include <platform/windows/runtime/native_mutation_continuity.h>
#include "allocation_fault_fixture.h"
#include <cstdio>
#include <stdexcept>
#include <thread>

namespace nimby::windows::admission {
struct NativeAdmissionAuthorityTestAccess {
    static bool lock(NativeAdmissionAuthority& owner)noexcept{
        return !owner.gate_.test_and_set(std::memory_order_acquire);
    }
    static void unlock(NativeAdmissionAuthority& owner)noexcept{owner.gate_.clear(std::memory_order_release);}
};
}
namespace {
namespace a=nimby::windows::admission;
namespace f=nimby::windows::final_admission;
namespace t=nimby::windows::two_phase_admission;
namespace mc=nimby::windows::mutation_continuity;
using a::Proof;
std::size_t checks{};
#define CHECK(x) do {++checks;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);} while(false)
constexpr uint64_t id(unsigned kind,uint64_t n=1){return (uint64_t(kind)<<48)|(n<<16)|1;}
constexpr auto track=id(1),secondTrack=id(1,2),train=id(5),source=id(8);
constexpr a::NativeDecision allowed{true,true,a::NativeDomain::FinalAdmission};
struct Fixture {
    a::NativeAdmissionAuthority authority;
    a::Frame frame{1,1,100,{1,1,1,a::rear::Phase::BeforeMovement}};
    bool current=true;
    std::size_t validationCalls{},loseAt{};
    bool publicationHeld=true;
    std::size_t publicationCalls{},publicationLoseAt{};
    std::thread::id ownerThread=std::this_thread::get_id();
    explicit Fixture(a::Limits limits={8,64,2048,1000}):authority(limits){
        CHECK(authority.beginWorld(frame,{publication(),true,true,true}).reason==a::Reason::Ready);
        publicationCalls=0;
    }
    static bool currentPublication(void* ctx,const a::PublicationWitness& value)noexcept {
        auto& self=*static_cast<Fixture*>(ctx);++self.publicationCalls;
        return self.current&&self.publicationHeld&&std::this_thread::get_id()==self.ownerThread&&
            (!self.publicationLoseAt||self.publicationCalls<self.publicationLoseAt)&&
            value.lineage==self.frame.lineage&&value.window==self.frame.window&&value.at==self.frame.at;
    }
    a::PublicationWitness publication()const {
        return {frame.lineage,frame.window,frame.at,true,true,currentPublication,const_cast<Fixture*>(this)};
    }
    a::OwnedAdmissionWindow owned(){return {publication(),9,true,[](void* ctx,const a::OwnedAdmissionWindow&)noexcept{
        auto& self=*static_cast<Fixture*>(ctx);++self.validationCalls;
        return self.current&&(!self.loseAt||self.validationCalls<self.loseAt);
    },this};}
    f::Identity identity(uint64_t n=1)const{return {0x100000,0x200000+n*0x100,0x300000+n*0x100,0x400000,
        id(5,n),source,7,true};}
    a::AdmissionRequest request(uint64_t n=1,uint64_t onTrack=track)const{
        a::AdmissionRequest r;r.ticket={frame.at.session,id(5,n),source,7};r.publication=publication();
        r.capturedAtMs=frame.nowMs;r.consistRevision=3;r.routeComplete=r.conflictsComplete=true;
        r.entered=Proof::No;r.resourceCount=r.physicalCount=1;r.resources[0]=r.ownPhysical[0]={onTrack,.1,.3};
        return r;
    }
    a::AdmissionRequest complete(uint64_t n=1,uint64_t onTrack=track)const{
        auto r=request(n,onTrack);r.physicalOccupationComplete=r.scriptVetoComplete=true;
        r.physicalClear=r.scriptAllows=Proof::Yes;return r;
    }
    a::Result claim(const a::AdmissionRequest& r,a::ProvisionalToken& token){
        return authority.claimProvisional(frame,r,owned(),token);
    }
    t::FreshCapture fresh(uint64_t n=1) {return {identity(n),frame,complete(n),owned()};}
    a::NoEntryWitness noEntry(const a::ProvisionalToken& token)const{
        return {token,publication(),frame.nowMs,token.continuity,token.window,frame.window,true,true,true,true,true};
    }
    void nextWindow(){++frame.window;++frame.nowMs;++frame.at.tick;}
};
void protectionBeforePhysicalAndScript(){
    Fixture x;auto before=x.request();a::ProvisionalToken token;
    CHECK(!before.physicalOccupationComplete&&!before.scriptVetoComplete&&before.physicalClear==Proof::Unknown);
    const auto provisional=x.claim(before,token);
    CHECK(provisional.reason==a::Reason::ProvisionalClaimed&&provisional.leaseHeld==Proof::Yes&&token);
    CHECK(!provisional.mayPass&&!provisional.replaceNativeExclusive&&!provisional.nativeReplacementConnected);
    a::ProvisionalToken other;
    CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict&&!other);
    CHECK(x.authority.admit(x.frame,x.complete(),allowed,a::Mode::FinalVeto).reason==a::Reason::FinalizationRequired);
    CHECK(x.claim(before,other).reason==a::Reason::FinalizationRequired&&!other);
    auto request=x.complete();++x.frame.nowMs;request.capturedAtMs=x.frame.nowMs;
    const auto finalized=x.authority.finalizeProvisional(x.frame,token,request,allowed,x.owned());
    CHECK(finalized.reason==a::Reason::Granted&&finalized.mayPass&&!finalized.replaceNativeExclusive);
    CHECK(x.authority.finalizeProvisional(x.frame,token,request,allowed,x.owned()).reason==a::Reason::FinalizationRequired);
    CHECK(x.authority.cancelBeforeEntry(x.frame,x.noEntry(token),x.owned()).reason==a::Reason::FinalizationRequired);
}
void originalAndScriptOnceOutsideGate(){
    for(const bool physical:{false,true})for(const bool script:{false,true}){
        Fixture x;std::size_t originals{},physicalCalls{},scriptCalls{},captures{};
        const auto out=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),[&]() -> uint8_t{
            ++originals;++physicalCalls;
            a::RetainedSnapshot snapshot;
            CHECK(x.authority.snapshot(x.request().ticket,snapshot).reason==a::Reason::Retained);
            CHECK(!physical||snapshot.leaseSerial);
            if(!physical)return 0;
            ++scriptCalls;return script?uint8_t{0x7f}:0;
        },[&](const f::Facts& facts){
            ++captures;CHECK(facts.originalReturned);
            auto capture=x.fresh();
            if(!physical){capture.request.physicalClear=Proof::No;capture.request.scriptVetoComplete=false;capture.request.scriptAllows=Proof::Unknown;}
            else if(!script)capture.request.scriptAllows=Proof::No;
            a::RetainedSnapshot snapshot;CHECK(x.authority.snapshot(capture.request.ticket,snapshot).reason==a::Reason::Retained);
            return capture;
        });
        CHECK(originals==1&&physicalCalls==1&&scriptCalls==std::size_t(physical)&&captures==1);
        CHECK(out.mayPass()==(physical&&script)&&!out.finalized.replaceNativeExclusive);
        CHECK(out.native.value==((physical&&script)?0x7f:0)&&out.claim);
        if(!physical||!script){
            CHECK(!out.native.facts.decisionAttempted&&out.finalized.leaseHeld==Proof::Yes);
            CHECK(out.finalized.reason==a::Reason::NativeRefusal);
            CHECK(x.authority.finalizeProvisional(x.frame,out.claim,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRepeated);
            a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
        }
    }
    // Even refusal to claim may not skip or replay the original callback.
    Fixture missing;auto request=missing.request();request.routeComplete=false;
    std::size_t originals{},captures{};
    const auto out=t::invoke(missing.authority,missing.frame,request,missing.identity(),missing.owned(),
        [&]{++originals;return uint8_t{1};},[&](auto&){++captures;return missing.fresh();});
    CHECK(!out.mayPass()&&!out.claim&&originals==1&&captures==1&&out.native.reason==f::Reason::SdkRefusal);
}
void allFinalizationPrerequisites(){
    for(unsigned bad=0;bad<18;++bad){
        Fixture x;a::ProvisionalToken token;CHECK(x.claim(x.request(),token).reason==a::Reason::ProvisionalClaimed);
        auto request=x.complete();auto owned=x.owned();auto native=allowed;
        if(bad==0)request.physicalOccupationComplete=false;
        if(bad==1)request.physicalClear=Proof::Unknown;
        if(bad==2)request.physicalClear=Proof::No;
        if(bad==3)request.scriptVetoComplete=false;
        if(bad==4)request.scriptAllows=Proof::Unknown;
        if(bad==5)request.scriptAllows=Proof::No;
        if(bad==6)request.routeComplete=false;
        if(bad==7)request.conflictsComplete=false;
        if(bad==8)++request.consistRevision;
        if(bad==9)request.resources[0].end=.4;
        if(bad==10)request.entered=Proof::Unknown;
        if(bad==11)native.admitted=false;
        if(bad==12)native.qualified=false;
        if(bad==13)native.domain=a::NativeDomain::ExclusiveReservationOnly;
        if(bad==14)--request.capturedAtMs;
        if(bad==15)++request.ticket.routeRevision;
        if(bad==16)++request.ticket.train;
        if(bad==17)++owned.publication.at.tick;
        const auto out=x.authority.finalizeProvisional(x.frame,token,request,native,owned);
        CHECK(!out.mayPass&&!out.replaceNativeExclusive&&out.leaseHeld==Proof::Yes);
        // A second direct attempt must never turn a failed observation into
        // another native invocation or a permission using the same token.
        const auto retry=x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned());
        CHECK(!retry.mayPass&&retry.reason==a::Reason::FinalizationRepeated);
        a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
    }
}
void ownershipAndContinuityCannotBeGuessed(){
    for(unsigned bad=0;bad<11;++bad){
        Fixture x;a::ProvisionalToken token;auto owned=x.owned();
        if(bad==0)owned.current=nullptr;
        if(bad==1)owned.allMutableInputsExcluded=false;
        if(bad==2)owned.continuity=0;
        if(bad==3)owned.publication.phaseOwned=false;
        if(bad==4)owned.publication.lineageOwned=false;
        if(bad==5)++owned.publication.window;
        if(bad==6)++owned.publication.at.tick;
        if(bad==7)x.current=false;
        if(bad==8)owned.publication.at.phase=a::rear::Phase::RetainedMovement;
        if(bad==9)owned.publication.current=nullptr;
        if(bad==10)x.publicationHeld=false;
        const auto out=x.authority.claimProvisional(x.frame,x.request(),owned,token);
        CHECK(!out.mayPass&&out.leaseHeld!=Proof::Yes&&!token);
    }
    {
        Fixture x;a::ProvisionalToken token;x.loseAt=3;
        const auto out=x.claim(x.request(),token);
        CHECK(out.reason==a::Reason::OwnedWindowLost&&!out.mayPass&&out.leaseHeld==Proof::Yes&&token);
    }
    {
        Fixture x;a::ProvisionalToken token;x.loseAt=2;
        const auto out=x.claim(x.request(),token);
        CHECK(out.reason==a::Reason::OwnedWindowLost&&!out.mayPass&&out.leaseHeld==Proof::No&&!token);
    }
    for(unsigned bad=0;bad<6;++bad){
        Fixture x;a::ProvisionalToken token;CHECK(x.claim(x.request(),token).leaseHeld==Proof::Yes);
        auto request=x.complete();auto owned=x.owned();
        if(bad==0)x.current=false;
        if(bad==1)++owned.continuity;
        if(bad==2){x.nextWindow();request=x.complete();owned=x.owned();}
        if(bad==3)owned.allMutableInputsExcluded=false;
        if(bad==4)x.loseAt=x.validationCalls+2; // Lost at the last owned publication check.
        if(bad==5)x.loseAt=x.validationCalls+3; // Lost after internal grant, before returning permission.
        const auto out=x.authority.finalizeProvisional(x.frame,token,request,allowed,owned);
        CHECK(!out.mayPass&&!out.replaceNativeExclusive&&out.leaseHeld==Proof::Yes);
    }
    // These are the actual production journal guarantees, not mock proof bits.
    static_assert(!mc::Result::nativeWritersComplete&&!mc::Result::progressionQualified&&!mc::Result::leaseReleaseQualified);
    Fixture observedOnly;a::ProvisionalToken token;auto owned=observedOnly.owned();
    owned.allMutableInputsExcluded=mc::Result::nativeWritersComplete;
    CHECK(observedOnly.authority.claimProvisional(observedOnly.frame,observedOnly.request(),owned,token).reason==a::Reason::OwnedWindowLost);
}
void immutableTokenAndArithmeticRefusals(){
    for(unsigned altered=0;altered<14;++altered) {
        Fixture x;a::ProvisionalToken token;CHECK(x.claim(x.request(),token).leaseHeld==Proof::Yes);
        auto wrong=token;
        if(altered==0)++wrong.ticket.session;
        if(altered==1)++wrong.ticket.train;
        if(altered==2)++wrong.ticket.source;
        if(altered==3)++wrong.ticket.routeRevision;
        if(altered==4)++wrong.leaseSerial;
        if(altered==5)++wrong.lineage;
        if(altered==6)++wrong.window;
        if(altered==7)++wrong.consistRevision;
        if(altered==8)++wrong.continuity;
        if(altered==9)++wrong.capturedAtMs;
        if(altered==10)++wrong.at.session;
        if(altered==11)++wrong.at.epoch;
        if(altered==12)++wrong.at.tick;
        if(altered==13)wrong.at.phase=a::rear::Phase::RetainedMovement;
        const auto refused=x.authority.finalizeProvisional(x.frame,wrong,x.complete(),allowed,x.owned());
        CHECK(refused.reason==a::Reason::AttemptMismatch&&!refused.mayPass);
        a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
        // An invalid token cannot consume a different owner's correct attempt.
        const auto accepted=x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned());
        CHECK(accepted.reason==a::Reason::Granted&&accepted.mayPass&&!accepted.replaceNativeExclusive);
    }
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP>=2)
    for(unsigned mode:{0x0040u,0x8000u,0x8040u,0x2000u}) {
        Fixture x;a::ProvisionalToken token;auto request=x.request();
        const auto controls=_mm_getcsr();_mm_setcsr(controls|mode);
        const auto refused=x.claim(request,token);const auto after=_mm_getcsr();_mm_setcsr(controls);
        CHECK(refused.reason==a::Reason::ArithmeticUnqualified&&!refused.mayPass&&!token);
        CHECK(after==(controls|mode));
        CHECK(x.claim(request,token).reason==a::Reason::ProvisionalClaimed);
        _mm_setcsr(controls|mode);
        const auto denied=x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned());
        const auto afterFinal=_mm_getcsr();_mm_setcsr(controls);
        CHECK(denied.reason==a::Reason::ArithmeticUnqualified&&!denied.mayPass&&denied.leaseHeld==Proof::Yes);
        CHECK(afterFinal==(controls|mode));
        CHECK(x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRepeated);
    }
#endif
    Fixture foreign;a::ProvisionalToken token;a::Result result;
    std::thread worker([&]{result=foreign.claim(foreign.request(),token);});worker.join();
    CHECK(result.reason==a::Reason::OwnedWindowLost&&!result.mayPass&&!token);
}
void cancelNeedsSeparatePositiveNonEntry(){
    for(unsigned bad=0;bad<14;++bad){
        Fixture x;a::ProvisionalToken token;CHECK(x.claim(x.request(),token).leaseHeld==Proof::Yes);
        auto witness=x.noEntry(token);auto owned=x.owned();
        if(bad==0)witness.positiveNoEntry=false;
        if(bad==1)witness.originalReturned=false;
        if(bad==2)witness.noPermissionOutstanding=false;
        if(bad==3)witness.allEntryWritersObserved=mc::Result::nativeWritersComplete;
        if(bad==4)witness.historyComplete=false;
        if(bad==5)++witness.continuity;
        if(bad==6)++witness.fromWindow;
        if(bad==7)++witness.throughWindow;
        if(bad==8)++witness.claim.leaseSerial;
        if(bad==9)++witness.claim.ticket.routeRevision;
        if(bad==10)++witness.publication.window;
        if(bad==11)witness.capturedAtMs=UINT64_MAX;
        if(bad==12)x.current=false;
        if(bad==13)x.loseAt=x.validationCalls+2; // Lost before the no-throw release commit.
        const auto out=x.authority.cancelBeforeEntry(x.frame,witness,owned);
        CHECK(!out.mayPass&&out.leaseHeld==Proof::Yes&&out.reason!=a::Reason::CancelledBeforeEntry);
        if(bad<12){a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);}
    }
    Fixture x;const auto out=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),
        []{return uint8_t{0};},[&](auto&){return x.fresh();});
    CHECK(out.finalized.leaseHeld==Proof::Yes&&!out.mayPass());
    // A native refusal alone did NOT cancel; only this independent synthetic
    // full certificate can do so. No current production producer supplies it.
    x.nextWindow();const auto cancelled=x.authority.cancelBeforeEntry(x.frame,x.noEntry(out.claim),x.owned());
    CHECK(cancelled.reason==a::Reason::CancelledBeforeEntry&&cancelled.leaseHeld==Proof::No&&!cancelled.mayPass);
    CHECK(x.authority.cancelBeforeEntry(x.frame,x.noEntry(out.claim),x.owned()).reason==a::Reason::Missing);
    a::ProvisionalToken newer;CHECK(x.claim(x.request(),newer).leaseHeld==Proof::Yes&&newer.leaseSerial>out.claim.leaseSerial);
    CHECK(x.authority.cancelBeforeEntry(x.frame,x.noEntry(out.claim),x.owned()).reason==a::Reason::AttemptMismatch);
    a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
}
void originalCaptureFailuresRetain(){
    for(unsigned failure=0;failure<6;++failure){
        Fixture x;std::size_t original{},capture{};
        const auto out=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),[&]() -> uint8_t{
            ++original;if(failure==0)throw 1;if(failure==1)throw std::bad_alloc();return 1;
        },[&](auto&) -> t::FreshCapture{
            ++capture;if(failure==2)throw 2;if(failure==3)throw std::bad_alloc();
            auto fresh=x.fresh();if(failure==4)++fresh.identity.trainId;if(failure==5)++fresh.identity.routeRevision;return fresh;
        });
        CHECK(original==1&&capture==(failure<2?0u:1u)&&!out.mayPass()&&out.finalized.leaseHeld==Proof::Yes);
        if(failure==1||failure==3)CHECK(out.finalized.reason==a::Reason::AllocationFailure);
        CHECK(x.authority.finalizeProvisional(x.frame,out.claim,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRepeated);
        a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
    }
}
void coordinatorTokensAreSpentBeforeOriginalAndJudge(){
    {
        Fixture x;a::ProvisionalToken token;std::size_t originals{},captures{};
        const auto out=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),[&]() -> uint8_t {
            ++originals;a::RetainedSnapshot retained;
            CHECK(x.authority.snapshot(x.request().ticket,retained).leaseHeld==Proof::Yes);
            token={x.request().ticket,retained.leaseSerial,x.frame.lineage,x.frame.window,3,9,x.frame.nowMs,x.frame.at};
            // The public token cannot judge even while its original is running.
            CHECK(x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRepeated);
            return 1;
        },[&](auto&){++captures;return x.fresh();});
        CHECK(originals==1&&captures==1&&out.mayPass()&&out.claim==token);
        CHECK(x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRequired);
    }
    for(bool admitted:{false,true}) {
        Fixture x;std::size_t originals{},captures{};
        const auto out=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),[&]() -> uint8_t {
            ++originals;CHECK(a::NativeAdmissionAuthorityTestAccess::lock(x.authority));
            return admitted?uint8_t{1}:uint8_t{0};
        },[&](auto&){++captures;return x.fresh();});
        a::NativeAdmissionAuthorityTestAccess::unlock(x.authority);
        CHECK(originals==1&&captures==1&&out.claim&&!out.mayPass());
        CHECK(out.finalized.reason==(admitted?a::Reason::Busy:a::Reason::NativeRefusal));
        CHECK(out.native.facts.decisionAttempted==admitted);
        // Contention cannot leave a post-native token available for a retry.
        CHECK(x.authority.finalizeProvisional(x.frame,out.claim,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRepeated);
        a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
        CHECK(x.authority.cancelBeforeEntry(x.frame,x.noEntry(out.claim),x.owned()).reason==a::Reason::CancelledBeforeEntry);
    }
    {
        Fixture x;const auto out=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),
            []{return uint8_t{0};},[&](auto&){return x.fresh();});
        CHECK(out.claim&&!out.mayPass());
        for(unsigned altered=0;altered<14;++altered) {
            auto wrong=out.claim;
            if(altered==0)++wrong.ticket.session;
            if(altered==1)++wrong.ticket.train;
            if(altered==2)++wrong.ticket.source;
            if(altered==3)++wrong.ticket.routeRevision;
            if(altered==4)++wrong.leaseSerial;
            if(altered==5)++wrong.lineage;
            if(altered==6)++wrong.window;
            if(altered==7)++wrong.consistRevision;
            if(altered==8)++wrong.continuity;
            if(altered==9)++wrong.capturedAtMs;
            if(altered==10)++wrong.at.session;
            if(altered==11)++wrong.at.epoch;
            if(altered==12)++wrong.at.tick;
            if(altered==13)wrong.at.phase=a::rear::Phase::RetainedMovement;
            CHECK(x.authority.finalizeProvisional(x.frame,wrong,x.complete(),allowed,x.owned()).reason==a::Reason::AttemptMismatch);
        }
        CHECK(x.authority.finalizeProvisional(x.frame,out.claim,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRepeated);
        CHECK(x.authority.cancelBeforeEntry(x.frame,x.noEntry(out.claim),x.owned()).reason==a::Reason::CancelledBeforeEntry);
        a::ProvisionalToken newer;CHECK(x.claim(x.request(),newer).leaseHeld==Proof::Yes);
        CHECK(x.authority.finalizeProvisional(x.frame,out.claim,x.complete(),allowed,x.owned()).reason==a::Reason::AttemptMismatch);
        CHECK(x.authority.finalizeProvisional(x.frame,newer,x.complete(),allowed,x.owned()).mayPass);
    }
}
void noPartialClaimAndNoHeap(){
    Fixture x;a::ProvisionalToken held;CHECK(x.claim(x.request(2,secondTrack),held).leaseHeld==Proof::Yes);
    auto both=x.request();both.resourceCount=2;both.resources[1]={secondTrack,.1,.3};a::ProvisionalToken failed;
    CHECK(x.claim(both,failed).reason==a::Reason::Conflict&&!failed);
    CHECK(x.claim(x.request(3,track),failed).reason==a::Reason::ProvisionalClaimed); // First portion was not partly installed.
    Fixture bounded({1,2,1024,1000});a::ProvisionalToken first,second;
    CHECK(bounded.claim(bounded.request(),first).leaseHeld==Proof::Yes);
    CHECK(bounded.claim(bounded.request(2,secondTrack),second).reason==a::Reason::Capacity&&!second);
    Fixture allocationFree;
    allocation_fault_fixture::arm(0,std::numeric_limits<unsigned>::max());
    const auto out=t::invoke(allocationFree.authority,allocationFree.frame,allocationFree.request(),allocationFree.identity(),allocationFree.owned(),
        []{return uint8_t{1};},[&](auto&){return allocationFree.fresh();});
    const auto fault=allocation_fault_fixture::stop();
    CHECK(out.mayPass()&&fault.allocations==0&&fault.failures==0&&!out.nativeReplacementConnected);
    Fixture cancelFree;a::ProvisionalToken token;CHECK(cancelFree.claim(cancelFree.request(),token).leaseHeld==Proof::Yes);
    const auto witness=cancelFree.noEntry(token);const auto owned=cancelFree.owned();
    allocation_fault_fixture::arm(0,std::numeric_limits<unsigned>::max());
    const auto cancelled=cancelFree.authority.cancelBeforeEntry(cancelFree.frame,witness,owned);
    const auto cancelFault=allocation_fault_fixture::stop();
    CHECK(cancelled.reason==a::Reason::CancelledBeforeEntry&&cancelFault.allocations==0);
}
void contentionAndCommitAreConservative(){
    Fixture x;a::ProvisionalToken token;CHECK(x.claim(x.request(),token).leaseHeld==Proof::Yes);
    CHECK(a::NativeAdmissionAuthorityTestAccess::lock(x.authority));
    std::size_t originals{},captures{};
    const auto blocked=t::invoke(x.authority,x.frame,x.request(),x.identity(),x.owned(),
        [&]{++originals;return uint8_t{1};},[&](auto&){++captures;return x.fresh();});
    a::NativeAdmissionAuthorityTestAccess::unlock(x.authority);
    CHECK(blocked.provisional.reason==a::Reason::Busy&&!blocked.mayPass()&&originals==1&&captures==1);
    a::ProvisionalToken other;CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
    x.nextWindow();x.frame.at.phase=a::rear::Phase::RetainedMovement;
    const a::EntryWitness entry{x.request().ticket,token.leaseSerial,3,x.publication(),true,true,true,true};
    CHECK(x.authority.commitEntry(x.frame,entry).reason==a::Reason::Entered);
    a::RetainedSnapshot snapshot;CHECK(x.authority.snapshot(token.ticket,snapshot).leaseHeld==Proof::Yes&&snapshot.entered==Proof::Yes);
    x.nextWindow();x.frame.at.phase=a::rear::Phase::BeforeMovement;
    CHECK(x.authority.cancelBeforeEntry(x.frame,x.noEntry(token),x.owned()).reason==a::Reason::FinalizationRequired);
    CHECK(x.authority.finalizeProvisional(x.frame,token,x.complete(),allowed,x.owned()).reason==a::Reason::FinalizationRequired);
    CHECK(x.claim(x.request(2),other).reason==a::Reason::Conflict);
}
void provisionalProtectionDoesNotReplaceNativeExclusive(){
    Fixture x;const auto identity=x.identity();uint8_t continuation=1;uintptr_t motion=identity.motion;
    std::array<uintptr_t,3> words{{0x500000,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&continuation)}};
    const f::FinalAdmissionScope::ContextCopy copy{reinterpret_cast<uintptr_t>(words.data()),identity.motion,
        identity.trainId,identity.sourceId,words,1,true};
    std::size_t native{},exclusive{},script{};
    const auto out=t::invoke(x.authority,x.frame,x.request(),identity,x.owned(),[&]() -> uint8_t{
        ++native;
        const auto inspected=f::FinalAdmissionScope::inspectPrivateExclusive(copy,[&](uintptr_t context){
            ++exclusive;const auto* privateWords=reinterpret_cast<const uintptr_t*>(context);
            *reinterpret_cast<uint8_t*>(privateWords[2])=0;return uint8_t{0};
        });
        CHECK(inspected.privateContextUsed&&inspected.originalReturned&&!inspected.nativeReturn);
        CHECK(continuation==1); // Only private continuation storage was touched.
        if(!inspected.nativeReturn)return 0;
        ++script;return 1;
    },[&](auto&){return x.fresh();});
    CHECK(native==1&&exclusive==1&&script==0&&out.claim&&!out.mayPass());
    CHECK(out.native.facts.exclusiveRefusals==1&&out.finalized.leaseHeld==Proof::Yes&&!out.finalized.replaceNativeExclusive);
}
}
int main(){try {
    protectionBeforePhysicalAndScript();originalAndScriptOnceOutsideGate();allFinalizationPrerequisites();
    ownershipAndContinuityCannotBeGuessed();cancelNeedsSeparatePositiveNonEntry();
    immutableTokenAndArithmeticRefusals();
    originalCaptureFailuresRetain();coordinatorTokensAreSpentBeforeOriginalAndJudge();
    noPartialClaimAndNoHeap();contentionAndCommitAreConservative();
    provisionalProtectionDoesNotReplaceNativeExclusive();
    std::printf("native_two_phase_admission: %zu checks PASS; no hooks, no native replacement, no production ownership proof\n",checks);
    a::NativeAdmissionAuthority measured;
    std::printf("Internal default storage: %zu bytes, token %zu bytes; allocated at bootstrap only\n",measured.storageBytes(),sizeof(a::ProvisionalToken));
}catch(const std::exception& error){std::fprintf(stderr,"%s\n",error.what());return 1;}
}
