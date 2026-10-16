// Synthetic state-machine regression only. No native owner, game memory,
// hook, writer history or reservation replacement is qualified by this fixture.
#include <platform/windows/runtime/native_two_phase_admission.h>
#include <cstdio>
#include <cstdlib>

namespace nimby::windows::admission {
struct NativeAdmissionAuthorityTestAccess {
    static bool lock(NativeAdmissionAuthority& value) noexcept {
        return !value.gate_.test_and_set(std::memory_order_acquire);
    }
    static void unlock(NativeAdmissionAuthority& value) noexcept {
        value.gate_.clear(std::memory_order_release);
    }
    // Same-thread synthetic trigger, not a currentOwned implementation:
    // invalidate the mock owner on the first check AFTER actual publication.
    // Inspecting test state directly avoids a magic validator-call count and
    // never re-enters an authority operation from an ownership callback.
    static bool published(const NativeAdmissionAuthority& value,const Ticket& ticket) noexcept {
        for(size_t i=0;i<value.limits_.leases;++i)
            if(value.leases_[i].live&&value.leases_[i].ticket==ticket)return true;
        return false;
    }
};
}

namespace {
namespace a=nimby::windows::admission;
namespace f=nimby::windows::final_admission;
namespace t=nimby::windows::two_phase_admission;
size_t checks=0;
void require(bool value,const char* expression,unsigned line) {
    ++checks;
    if(!value){std::fprintf(stderr,"line %u: %s\n",line,expression);std::exit(1);}
}
#define CHECK(value) require(bool(value),#value,__LINE__)
constexpr uint64_t id(unsigned kind,uint64_t ordinal=1) {
    return (uint64_t(kind)<<48)|(ordinal<<16)|1;
}
constexpr uint64_t track=id(1),otherTrack=id(1,2),source=id(8);
constexpr a::NativeDecision allowed{true,true,a::NativeDomain::FinalAdmission};

struct World {
    a::NativeAdmissionAuthority authority{{4,16,2048,1000}};
    a::Frame frame{1,1,100,{1,1,1,a::rear::Phase::BeforeMovement}};
    bool held=true,loseAfterPublication=false;

    World(){CHECK(authority.beginWorld(frame,{publication(),true,true,true}).reason==a::Reason::Ready);}
    a::Ticket ticket(uint64_t train=1)const{return {frame.at.session,id(5,train),source,7};}
    a::PublicationWitness publication() {
        return {frame.lineage,frame.window,frame.at,true,true,
            [](void* context,const a::PublicationWitness& value)noexcept {
                const auto& self=*static_cast<World*>(context);
                return self.held&&value.lineage==self.frame.lineage&&
                    value.window==self.frame.window&&value.at==self.frame.at;
            },this};
    }
    a::OwnedAdmissionWindow owned() {
        return {publication(),9,true,[](void* context,const a::OwnedAdmissionWindow&)noexcept {
            auto& self=*static_cast<World*>(context);
            if(self.loseAfterPublication&&
               a::NativeAdmissionAuthorityTestAccess::published(self.authority,self.ticket()))self.held=false;
            return self.held;
        },this};
    }
    a::AdmissionRequest request(uint64_t train=1,uint64_t onTrack=track,bool complete=false) {
        a::AdmissionRequest value;
        value.ticket=ticket(train);value.publication=publication();
        value.capturedAtMs=frame.nowMs;value.consistRevision=3;
        value.routeComplete=value.conflictsComplete=true;value.entered=a::Proof::No;
        value.resourceCount=value.physicalCount=1;
        value.resources[0]=value.ownPhysical[0]={onTrack,.1,.3};
        if(complete){
            value.physicalOccupationComplete=value.scriptVetoComplete=true;
            value.physicalClear=value.scriptAllows=a::Proof::Yes;
        }
        return value;
    }
    f::Identity identity()const{return {0x100000,0x200000,0x300000,0x400000,id(5),source,7,true};}
    t::FreshCapture fresh(){return {identity(),frame,request(1,track,true),owned()};}
    a::NoEntryWitness noEntry(const a::ProvisionalToken& token) {
        // Every positive declaration here is synthetic; no production producer
        // supplies this complete non-entry/writer-history certificate yet.
        return {token,publication(),frame.nowMs,token.continuity,token.window,
            frame.window,true,true,true,true,true};
    }
    a::Result finalize(const a::ProvisionalToken& token,uint64_t train=1,uint64_t onTrack=track) {
        return authority.finalizeProvisional(frame,token,request(train,onTrack,true),allowed,owned());
    }
    a::Result claim(uint64_t train,uint64_t onTrack,a::ProvisionalToken& token) {
        return authority.claimProvisional(frame,request(train,onTrack),owned(),token);
    }
};

struct Contention {
    a::NativeAdmissionAuthority& authority;
    bool held=false;
    explicit Contention(a::NativeAdmissionAuthority& value):authority(value){}
    void lock(){CHECK(!held);CHECK(a::NativeAdmissionAuthorityTestAccess::lock(authority));held=true;}
    void unlock(){if(held){a::NativeAdmissionAuthorityTestAccess::unlock(authority);held=false;}}
    ~Contention(){unlock();}
};

void restoredOwnerCannotReopenPublishedAttempt() {
    World world;world.loseAfterPublication=true;
    size_t original=0,capture=0;
    const auto output=t::invoke(world.authority,world.frame,world.request(),world.identity(),world.owned(),
        [&]{++original;return uint8_t{1};},[&](const f::Facts&){++capture;return world.fresh();});
    CHECK(original==1&&capture==1&&output.claim&&!output.mayPass());
    CHECK(output.provisional.reason==a::Reason::OwnedWindowLost);
    CHECK(output.provisional.leaseHeld==a::Proof::Yes&&!world.held);
    world.loseAfterPublication=false;world.held=true;
    CHECK(world.finalize(output.claim).reason==a::Reason::FinalizationRepeated);

    auto noEntry=world.noEntry(output.claim);noEntry.historyComplete=false;
    CHECK(world.authority.cancelBeforeEntry(world.frame,noEntry,world.owned()).reason==a::Reason::ContinuityUnqualified);
    a::ProvisionalToken competitor;
    CHECK(world.claim(2,track,competitor).reason==a::Reason::Conflict);
    noEntry.historyComplete=true;
    CHECK(world.authority.cancelBeforeEntry(world.frame,noEntry,world.owned()).reason==a::Reason::CancelledBeforeEntry);
    CHECK(world.finalize(output.claim).reason==a::Reason::AttemptMismatch);
    CHECK(world.claim(2,track,competitor).reason==a::Reason::ProvisionalClaimed);
}

void foreignAttemptUnderContentionDoesNotConsumeItsOwner() {
    World world;a::ProvisionalToken foreign;
    CHECK(world.claim(2,otherTrack,foreign).reason==a::Reason::ProvisionalClaimed);
    Contention contention(world.authority);
    size_t original=0,capture=0;
    const auto output=t::invoke(world.authority,world.frame,world.request(),world.identity(),world.owned(),[&]{
        ++original;contention.lock();
        // The pending request belongs to train 1, but the supplied token is
        // train 2's. Busy must not consume or modify that foreign attempt.
        CHECK(world.finalize(foreign).reason==a::Reason::Busy);
        return uint8_t{1};
    },[&](const f::Facts&){++capture;return world.fresh();});
    contention.unlock();
    CHECK(original==1&&capture==1&&output.claim&&!output.mayPass());
    CHECK(output.finalized.reason==a::Reason::Busy);
    CHECK(world.finalize(output.claim).reason==a::Reason::FinalizationRepeated);
    CHECK(world.finalize(foreign,2,otherTrack).mayPass);
    CHECK(world.authority.cancelBeforeEntry(world.frame,world.noEntry(output.claim),world.owned()).reason==a::Reason::CancelledBeforeEntry);
    a::RetainedSnapshot retained;
    CHECK(world.authority.snapshot(foreign.ticket,retained).leaseHeld==a::Proof::Yes);
    CHECK(retained.leaseSerial==foreign.leaseSerial);
    a::ProvisionalToken other;
    CHECK(world.claim(3,otherTrack,other).reason==a::Reason::Conflict);
    CHECK(world.claim(3,track,other).reason==a::Reason::ProvisionalClaimed);
}

void recycledAttemptUnderContentionCannotAffectNewClaim() {
    World world;
    const auto old=t::invoke(world.authority,world.frame,world.request(),world.identity(),world.owned(),
        []{return uint8_t{0};},[&](const f::Facts&){return world.fresh();});
    CHECK(old.claim&&!old.mayPass());
    CHECK(world.authority.cancelBeforeEntry(world.frame,world.noEntry(old.claim),world.owned()).reason==a::Reason::CancelledBeforeEntry);
    Contention contention(world.authority);
    const auto current=t::invoke(world.authority,world.frame,world.request(),world.identity(),world.owned(),[&]{
        contention.lock();CHECK(world.finalize(old.claim).reason==a::Reason::Busy);return uint8_t{1};
    },[&](const f::Facts&){return world.fresh();});
    contention.unlock();
    CHECK(current.claim&&current.claim.leaseSerial>old.claim.leaseSerial&&!current.mayPass());
    CHECK(current.finalized.reason==a::Reason::Busy);
    CHECK(world.finalize(old.claim).reason==a::Reason::AttemptMismatch);
    CHECK(world.authority.cancelBeforeEntry(world.frame,world.noEntry(old.claim),world.owned()).reason==a::Reason::AttemptMismatch);
    CHECK(world.finalize(current.claim).reason==a::Reason::FinalizationRepeated);
    a::RetainedSnapshot retained;
    CHECK(world.authority.snapshot(current.claim.ticket,retained).leaseHeld==a::Proof::Yes);
    CHECK(retained.leaseSerial==current.claim.leaseSerial);

    auto noEntry=world.noEntry(current.claim);noEntry.allEntryWritersObserved=false;
    CHECK(world.authority.cancelBeforeEntry(world.frame,noEntry,world.owned()).reason==a::Reason::ContinuityUnqualified);
    noEntry.allEntryWritersObserved=true;
    CHECK(world.authority.cancelBeforeEntry(world.frame,noEntry,world.owned()).reason==a::Reason::CancelledBeforeEntry);
    const auto fresh=t::invoke(world.authority,world.frame,world.request(),world.identity(),world.owned(),
        []{return uint8_t{1};},[&](const f::Facts&){return world.fresh();});
    CHECK(fresh.mayPass()&&fresh.claim.leaseSerial>current.claim.leaseSerial);
    CHECK(!fresh.nativeReplacementConnected&&!fresh.finalized.replaceNativeExclusive);
}
}

int main() {
    restoredOwnerCannotReopenPublishedAttempt();
    foreignAttemptUnderContentionDoesNotConsumeItsOwner();
    recycledAttemptUnderContentionCannotAffectNewClaim();
    std::printf("native_two_phase_edges: %zu checks PASS; synthetic only, no native owner or reservation replacement\n",checks);
}
