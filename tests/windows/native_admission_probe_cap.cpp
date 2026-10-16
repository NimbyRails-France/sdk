// Synthetic cache-contract regression only: no game owner, native hooks,
// movement permission, writer history or replacement is proved by this test.
#include <platform/windows/runtime/native_admission_authority.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace nimby::windows::admission {
struct NativeAdmissionAuthorityTestAccess {
    static size_t hash(const NativeAdmissionAuthority& a,const Ticket& t) noexcept {return a.ownerHash(t);}
    static size_t slots(const NativeAdmissionAuthority& a) noexcept {return a.ownerSlots_;}
    static size_t slot(const NativeAdmissionAuthority& a,const Ticket& t) noexcept {
        for(size_t i=0;i<a.ownerSlots_;++i){const auto& n=a.ownerIndex_[i];
            if(n.epoch==a.lineage_&&n.state==1&&n.session==t.session&&n.train==t.train&&n.source==t.source)return i;
        }
        return a.ownerSlots_;
    }
    static size_t live(const NativeAdmissionAuthority& a) noexcept {
        size_t count=0;for(size_t i=0;i<a.limits_.leases;++i)count+=a.leases_[i].live;return count;
    }
    static size_t rows(const NativeAdmissionAuthority& a) noexcept {return a.rowCount_;}
    static uint64_t serial(const NativeAdmissionAuthority& a) noexcept {return a.serial_;}
    static uint64_t epoch(const NativeAdmissionAuthority& a) noexcept {return a.rowEpoch_;}
    static void operations(NativeAdmissionAuthority& a,size_t n) noexcept {a.limits_.operations=n;}
    static uint8_t state(const NativeAdmissionAuthority& a,size_t slot) noexcept {return a.ownerIndex_[slot].state;}
    static void corrupt(NativeAdmissionAuthority& a,const Ticket& target,const Ticket& foreign,unsigned mode) noexcept {
        auto& node=a.ownerIndex_[slot(a,target)];
        if(mode==0)node.state=0;
        else if(mode==1)node.state=2;
        else if(mode==2)node.lease=static_cast<uint16_t>(a.limits_.leases);
        else if(mode==3)++node.train;
        else if(mode==4)node=a.ownerIndex_[slot(a,foreign)]; // Valid foreign node, wrong target identity.
        else if(mode==5)--node.epoch;
        else node.state=3;
    }
    static void fillVacanciesWithValidForeignNodes(NativeAdmissionAuthority& a,const Ticket& foreign) noexcept {
        const auto copy=a.ownerIndex_[slot(a,foreign)];
        for(size_t i=0;i<a.ownerSlots_;++i)
            if(a.ownerIndex_[i].epoch!=a.lineage_||a.ownerIndex_[i].state!=1)a.ownerIndex_[i]=copy;
    }
};
}

namespace {
namespace a=nimby::windows::admission;
using Access=a::NativeAdmissionAuthorityTestAccess;
size_t checks=0;
void require(bool value,const char* expression,unsigned line) {
    ++checks;
    if(!value){std::fprintf(stderr,"line %u: %s\n",line,expression);std::exit(1);}
}
#define CHECK(value) require(bool(value),#value,__LINE__)
constexpr uint64_t id(unsigned kind,uint64_t ordinal=1) {return (uint64_t(kind)<<48)|(ordinal<<16)|1;}
constexpr size_t owners=80,quota=65536;
constexpr a::NativeDecision allowed{true,true,a::NativeDomain::FinalAdmission};

struct World {
    a::NativeAdmissionAuthority authority{{96,192,quota,1000}};
    a::Frame frame{1,1,100,{1,1,1,a::rear::Phase::BeforeMovement}};
    std::thread::id thread=std::this_thread::get_id();
    bool held=true;
    std::array<a::Ticket,owners> tickets{};
    std::array<a::ProvisionalToken,owners> tokens{};
    size_t bucket=0;

    World() {
        CHECK(authority.beginWorld(frame,{publication(),true,true,true}).reason==a::Reason::Ready);
        CHECK(Access::slots(authority)==256);
        bucket=Access::hash(authority,ticket(1));
        // Discover collisions using the real class hash, not a duplicate test
        // implementation or malformed/truncated IDs. Generation 1 is retained.
        size_t found=0;
        for(uint64_t n=1;n<1000000&&found<owners;++n){
            auto t=ticket(n);if(Access::hash(authority,t)==bucket)tickets[found++]=t;
        }
        CHECK(found==owners);
        for(size_t i=0;i<owners;++i){
            const auto result=authority.claimProvisional(frame,request(i),window(),tokens[i]);
            CHECK(result.reason==a::Reason::ProvisionalClaimed&&!result.mayPass&&!result.replaceNativeExclusive);
            CHECK(tokens[i]&&tokens[i].ticket==tickets[i]);
            CHECK(((Access::slot(authority,tickets[i])-bucket)&255)==i);
        }
        CHECK(Access::live(authority)==owners&&Access::rows(authority)==owners*2);
        CHECK(Access::slot(authority,tickets.back())!=Access::slots(authority));
        CHECK(((Access::slot(authority,tickets.back())-bucket)&255)+1==80);
    }
    a::Ticket ticket(uint64_t train)const {return {frame.at.session,id(5,train),id(8),7};}
    static bool current(void* context,const a::PublicationWitness& value) noexcept {
        const auto& self=*static_cast<World*>(context);
        return self.held&&self.thread==std::this_thread::get_id()&&value.lineage==self.frame.lineage&&
            value.window==self.frame.window&&value.at==self.frame.at;
    }
    static bool windowCurrent(void* context,const a::OwnedAdmissionWindow& value) noexcept {
        return value.continuity==9&&current(context,value.publication);
    }
    a::PublicationWitness publication(){return {frame.lineage,frame.window,frame.at,true,true,current,this};}
    a::OwnedAdmissionWindow window(){return {publication(),9,true,windowCurrent,this};}
    a::AdmissionRequest request(size_t index,bool complete=false) {
        a::AdmissionRequest r;r.ticket=tickets[index];r.publication=publication();
        r.capturedAtMs=frame.nowMs;r.consistRevision=3;r.routeComplete=r.conflictsComplete=true;
        r.entered=a::Proof::No;r.resourceCount=r.physicalCount=1;
        r.resources[0]=r.ownPhysical[0]={id(1,index+1),.1,.3};
        if(complete){r.physicalOccupationComplete=r.scriptVetoComplete=true;r.physicalClear=r.scriptAllows=a::Proof::Yes;}
        return r;
    }
    a::Result finalizeLast(){return authority.finalizeProvisional(frame,tokens.back(),request(owners-1,true),allowed,window());}
    a::Result renewLast(){return authority.admit(frame,request(owners-1,true),allowed,a::Mode::FinalVeto);}
    a::Result cancel(size_t index) {
        // Positive NoEntry and complete writer history are synthetic ONLY.
        a::NoEntryWitness witness{tokens[index],publication(),frame.nowMs,9,tokens[index].window,frame.window,true,true,true,true,true};
        return authority.cancelBeforeEntry(frame,witness,window());
    }
    void snapshot(size_t index) {
        a::RetainedSnapshot value;
        const auto result=authority.snapshot(tickets[index],value);
        CHECK(result.reason==a::Reason::Retained&&result.leaseHeld==a::Proof::Yes&&!result.mayPass);
        CHECK(!result.replaceNativeExclusive&&!result.nativeReplacementConnected);
        CHECK(value.ticket==tickets[index]&&value.leaseSerial==tokens[index].leaseSerial&&value.consistRevision==3);
        CHECK(value.resourceCount==1&&value.physicalCount==1);
        CHECK((value.resources[0]==a::Interval{id(1,index+1),.1,.3}));
        CHECK(value.ownPhysical[0]==value.resources[0]);
    }
    void allSnapshots(size_t except=owners) {for(size_t i=0;i<owners;++i)if(i!=except)snapshot(i);}
};

void beyond64AndMiddleRemoval() {
    World w;w.allSnapshots();
    for(const auto index:std::array<size_t,3>{62,63,64}){
        const auto result=w.authority.finalizeProvisional(w.frame,w.tokens[index],w.request(index,true),allowed,w.window());
        CHECK(result.mayPass&&!result.replaceNativeExclusive);
        CHECK(w.authority.admit(w.frame,w.request(index,true),allowed,a::Mode::FinalVeto).mayPass);
        w.snapshot(index);
    }
    const auto final=w.finalizeLast();
    CHECK(final.reason==a::Reason::Granted&&final.mayPass&&!final.replaceNativeExclusive);
    CHECK(w.renewLast().mayPass);w.allSnapshots();
    const auto removedSlot=Access::slot(w.authority,w.tickets[39]);
    CHECK(w.cancel(39).reason==a::Reason::CancelledBeforeEntry);
    CHECK(Access::state(w.authority,removedSlot)==2);
    CHECK(Access::live(w.authority)==owners-1&&Access::rows(w.authority)==(owners-1)*2);
    w.snapshot(79);CHECK(w.renewLast().mayPass);w.allSnapshots(39);
    const auto missing=w.authority.retain(w.tickets[39]);
    CHECK(missing.reason==a::Reason::Missing&&!missing.mayPass);
    // Tombstone traversal must preserve both the exact owner beyond 64 and
    // all neighboring claims; no collateral repair or duplicate insertion.
    CHECK(Access::state(w.authority,removedSlot)==2);
    // Removal AFTER the fast-probe boundary must also preserve the tail and
    // requalify the exact owning node, rather than returning the lease alone.
    const auto tailRemovedSlot=Access::slot(w.authority,w.tickets[69]);
    CHECK(w.cancel(69).reason==a::Reason::CancelledBeforeEntry);
    CHECK(Access::state(w.authority,tailRemovedSlot)==2);
    CHECK(Access::live(w.authority)==owners-2&&Access::rows(w.authority)==(owners-2)*2);
    w.snapshot(79);CHECK(w.renewLast().mayPass);
    for(size_t i=0;i<owners;++i)if(i!=39&&i!=69)w.snapshot(i);
}

void corruptExactOwnerMustClose() {
    for(unsigned mode=0;mode<7;++mode){
        World w;CHECK(w.finalizeLast().mayPass);
        const auto rows=Access::rows(w.authority),live=Access::live(w.authority);
        const auto serial=Access::serial(w.authority),epoch=Access::epoch(w.authority);
        Access::corrupt(w.authority,w.tickets.back(),w.tickets.front(),mode);
        a::RetainedSnapshot output;output.ticket=w.tickets.front();output.leaseSerial=UINT64_MAX;
        const auto snap=w.authority.snapshot(w.tickets.back(),output);
        CHECK(snap.reason==a::Reason::IndexInvalid&&!snap.mayPass&&snap.work<=quota);
        CHECK(output.ticket==w.tickets.front()&&output.leaseSerial==UINT64_MAX&&output.resourceCount==0);
        const auto renew=w.renewLast();
        CHECK(renew.reason==a::Reason::IndexInvalid&&!renew.mayPass&&!renew.replaceNativeExclusive);
        a::ProvisionalToken token;
        const auto duplicate=w.authority.claimProvisional(w.frame,w.request(79),w.window(),token);
        CHECK(duplicate.reason==a::Reason::IndexInvalid&&!duplicate.mayPass&&!token);
        CHECK(Access::rows(w.authority)==rows&&Access::live(w.authority)==live);
        CHECK(Access::serial(w.authority)==serial&&Access::epoch(w.authority)==epoch);
        w.allSnapshots(79);
    }
}

void unrelatedUnreadDamageDoesNotInventGlobalAbsence() {
    World w;
    const auto corruptSlot=Access::slot(w.authority,w.tickets.back());
    Access::corrupt(w.authority,w.tickets.back(),w.tickets.front(),2);
    // Select a genuinely new owner whose starting node is vacant and whose
    // lookup never encounters the damaged foreign node. The full lease scan
    // still proves its absence. This local cache miss must not clear or repair
    // the damaged owner's real retained claim.
    a::Ticket newcomer;
    for(uint64_t n=1000001;n<2000000;++n){
        const auto candidate=w.ticket(n);const auto bucket=Access::hash(w.authority,candidate);
        if(bucket!=corruptSlot&&Access::state(w.authority,bucket)==0){newcomer=candidate;break;}
    }
    CHECK(newcomer.train!=0);
    auto r=w.request(0);r.ticket=newcomer;r.resources[0]=r.ownPhysical[0]={id(1,1000),.1,.3};
    a::ProvisionalToken token;
    const auto result=w.authority.claimProvisional(w.frame,r,w.window(),token);
    CHECK(result.reason==a::Reason::ProvisionalClaimed&&!result.mayPass&&token.ticket==newcomer);
    CHECK(Access::live(w.authority)==owners+1&&Access::rows(w.authority)==(owners+1)*2);
    CHECK(w.authority.retain(w.tickets.back()).reason==a::Reason::IndexInvalid);
    w.allSnapshots(79);
    a::RetainedSnapshot retained;
    CHECK(w.authority.snapshot(newcomer,retained).reason==a::Reason::Retained);
    CHECK(retained.ticket==newcomer&&retained.resourceCount==1&&retained.resources[0].track==id(1,1000));
}

void quotaNeverPublishesOrDestroysOnExhaustion() {
    constexpr std::array<size_t,26> limits{64,65,79,80,81,82,85,95,96,97,127,128,159,160,175,176,177,178,179,180,181,182,191,192,255,256};
    size_t failedSnapshots=0,failedRenewals=0,successfulSnapshots=0;
    for(const auto limit:limits){
        World w;CHECK(w.finalizeLast().mayPass);
        const auto rows=Access::rows(w.authority),live=Access::live(w.authority);
        const auto serial=Access::serial(w.authority),epoch=Access::epoch(w.authority);
        Access::operations(w.authority,limit);
        a::RetainedSnapshot output;output.leaseSerial=UINT64_MAX;
        const auto snap=w.authority.snapshot(w.tickets.back(),output);
        CHECK(snap.work<=limit&&!snap.mayPass&&!snap.replaceNativeExclusive);
        CHECK(snap.reason==a::Reason::Retained||snap.reason==a::Reason::BudgetExceeded);
        if(snap.reason==a::Reason::BudgetExceeded){++failedSnapshots;CHECK(output.leaseSerial==UINT64_MAX&&output.resourceCount==0);}
        else {++successfulSnapshots;CHECK(output.ticket==w.tickets.back()&&output.resourceCount==1&&output.physicalCount==1);}
        const auto renew=w.renewLast();
        CHECK(renew.work<=limit&&!renew.replaceNativeExclusive);
        CHECK(renew.reason==a::Reason::Granted||renew.reason==a::Reason::BudgetExceeded);
        if(renew.reason==a::Reason::BudgetExceeded){++failedRenewals;CHECK(!renew.mayPass);}
        else CHECK(renew.mayPass);
        CHECK(Access::rows(w.authority)==rows&&Access::live(w.authority)==live);
        CHECK(Access::serial(w.authority)==serial&&Access::epoch(w.authority)==epoch);
        Access::operations(w.authority,quota);w.allSnapshots();CHECK(w.renewLast().mayPass);
        std::printf("quota=%zu snapshot=%s/%zu renewal=%s/%zu\n",limit,
            snap.reason==a::Reason::Retained?"retained":"budget",snap.work,
            renew.reason==a::Reason::Granted?"granted":"budget",renew.work);
    }
    CHECK(failedSnapshots>=3&&successfulSnapshots>=1&&failedRenewals>=3);
}

a::AdmissionRequest newCollision(World& w) {
    auto r=w.request(0);r.resources[0]=r.ownPhysical[0]={id(1,1000),.1,.3};r.ticket={};
    for(uint64_t n=1000001;n<2000000;++n){
        const auto candidate=w.ticket(n);
        if(Access::hash(w.authority,candidate)==w.bucket){r.ticket=candidate;break;}
    }
    CHECK(r.ticket.train!=0);return r;
}

void newOwnerStagingBudgetAndNoVacancy() {
    size_t needed=0;
    {
        World w;a::ProvisionalToken token;
        const auto result=w.authority.claimProvisional(w.frame,newCollision(w),w.window(),token);
        CHECK(result.reason==a::Reason::ProvisionalClaimed&&!result.mayPass&&token);
        needed=result.work;CHECK(needed>182&&needed<quota);
    }
    for(const auto limit:std::array<size_t,7>{64,96,160,176,needed-1,needed,needed+1}){
        World w;const auto request=newCollision(w);
        const auto epoch=Access::epoch(w.authority),serial=Access::serial(w.authority);
        Access::operations(w.authority,limit);a::ProvisionalToken token;
        const auto result=w.authority.claimProvisional(w.frame,request,w.window(),token);
        CHECK(result.work<=limit&&!result.mayPass&&!result.replaceNativeExclusive);
        if(limit<needed){
            CHECK(result.reason==a::Reason::BudgetExceeded&&!token);
            CHECK(Access::live(w.authority)==owners&&Access::rows(w.authority)==owners*2);
            CHECK(Access::epoch(w.authority)==epoch&&Access::serial(w.authority)==serial);
        } else {
            CHECK(result.reason==a::Reason::ProvisionalClaimed&&token.ticket==request.ticket);
            CHECK(Access::live(w.authority)==owners+1&&Access::rows(w.authority)==(owners+1)*2);
        }
        Access::operations(w.authority,quota);w.allSnapshots();
        if(token){
            a::ProvisionalToken duplicate;
            CHECK(w.authority.claimProvisional(w.frame,request,w.window(),duplicate).reason==a::Reason::FinalizationRequired);
            CHECK(!duplicate&&Access::live(w.authority)==owners+1);
        } else CHECK(w.authority.retain(request.ticket).reason==a::Reason::Missing);
        std::printf("new-owner quota=%zu required=%zu result=%s/%zu\n",limit,needed,token?"claimed":"budget",result.work);
    }
    {
        World w;const auto request=newCollision(w);
        const auto epoch=Access::epoch(w.authority),serial=Access::serial(w.authority);
        Access::fillVacanciesWithValidForeignNodes(w.authority,w.tickets.front());
        a::ProvisionalToken token;
        const auto result=w.authority.claimProvisional(w.frame,request,w.window(),token);
        // Baseline diagnoses full-cache staging as BudgetExceeded; the capped
        // version diagnoses IndexInvalid. Both must close without publishing.
        CHECK((result.reason==a::Reason::IndexInvalid||result.reason==a::Reason::BudgetExceeded)&&!token&&!result.mayPass);
        CHECK(result.work<quota&&Access::live(w.authority)==owners&&Access::rows(w.authority)==owners*2);
        CHECK(Access::epoch(w.authority)==epoch&&Access::serial(w.authority)==serial);
        w.allSnapshots();
        std::printf("full-valid-cache reason=%s work=%zu (closed)\n",result.reason==a::Reason::IndexInvalid?"index-invalid":"baseline-stage-budget",result.work);
    }
}

void recycledWorldCannotReusePriorOwner() {
    World w;const auto oldSerial=Access::serial(w.authority);
    ++w.frame.lineage;++w.frame.at.session;++w.frame.at.epoch;++w.frame.at.tick;++w.frame.window;++w.frame.nowMs;
    CHECK(w.authority.beginWorld(w.frame,{w.publication(),true,true,true}).reason==a::Reason::Ready);
    CHECK(Access::live(w.authority)==0&&Access::rows(w.authority)==0);
    for(const auto& ticket:w.tickets){
        a::RetainedSnapshot output;const auto result=w.authority.snapshot(ticket,output);
        CHECK(result.reason==a::Reason::Missing&&!result.mayPass&&output.resourceCount==0);
    }
    auto request=w.request(0);request.ticket.session=w.frame.at.session;
    a::ProvisionalToken token;
    CHECK(w.authority.claimProvisional(w.frame,request,w.window(),token).reason==a::Reason::ProvisionalClaimed);
    CHECK(token.ticket.session==w.frame.at.session&&token.leaseSerial>oldSerial&&!w.authority.retain(w.tickets[0]).mayPass);
    a::RetainedSnapshot retained;
    CHECK(w.authority.snapshot(token.ticket,retained).reason==a::Reason::Retained&&retained.ticket==token.ticket);
}
}

int main() {
    beyond64AndMiddleRemoval();corruptExactOwnerMustClose();unrelatedUnreadDamageDoesNotInventGlobalAbsence();
    quotaNeverPublishesOrDestroysOnExhaustion();newOwnerStagingBudgetAndNoVacancy();recycledWorldCannotReusePriorOwner();
    std::printf("native_admission_probe_cap: %zu checks PASS (synthetic only)\n",checks);
}
