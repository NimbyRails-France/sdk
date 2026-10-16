// Portable cache-contract fixture only: no game calls, hooks or ownership proof.
#include <platform/windows/runtime/native_admission_authority.h>
#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {
std::atomic<bool> watchAllocations{};
std::atomic<size_t> allocations{};
std::atomic<int> failAllocation{-1};
size_t checks=0;
}
#if defined(__GNUC__)
#define INDEX_NOINLINE __attribute__((noinline))
#else
#define INDEX_NOINLINE __declspec(noinline)
#endif
INDEX_NOINLINE void* operator new(size_t size) {
    if(watchAllocations.load())++allocations;
    if(failAllocation.load()>=0&&failAllocation.fetch_sub(1)==0)throw std::bad_alloc();
    if(void* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
INDEX_NOINLINE void* operator new[](size_t size){return ::operator new(size);}
INDEX_NOINLINE void operator delete(void* value) noexcept {std::free(value);}
INDEX_NOINLINE void operator delete[](void* value) noexcept {std::free(value);}
INDEX_NOINLINE void operator delete(void* value,size_t) noexcept {std::free(value);}
INDEX_NOINLINE void operator delete[](void* value,size_t) noexcept {std::free(value);}
#define CHECK(value) do{++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);}while(false)

namespace nimby::windows::admission {
struct NativeAdmissionAuthorityTestAccess {
    static void operations(NativeAdmissionAuthority& a,size_t n){a.limits_.operations=n;}
    static void exhaustEpoch(NativeAdmissionAuthority& a){a.nextRowEpoch_=UINT64_MAX;}
    static uint64_t currentEpoch(const NativeAdmissionAuthority& a){return a.rowEpoch_;}
    static uint64_t stagedEpoch(const NativeAdmissionAuthority& a){return a.nextRowEpoch_;}
    static size_t hash(const NativeAdmissionAuthority& a,const Ticket& t){return a.ownerHash(t);}
    static size_t slots(const NativeAdmissionAuthority& a){return a.ownerSlots_;}
    static size_t slot(const NativeAdmissionAuthority& a,const Ticket& t){
        for(size_t i=0;i<a.ownerSlots_;++i){const auto& node=a.ownerIndex_[i];
            if(node.epoch==a.lineage_&&node.state==1&&node.session==t.session&&node.train==t.train&&node.source==t.source)return i;
        }
        return a.ownerSlots_;
    }
    static size_t lease(const NativeAdmissionAuthority& a,const Ticket& t){
        const auto i=slot(a,t);return i<a.ownerSlots_?a.ownerIndex_[i].lease:a.limits_.leases;
    }
    static size_t rowCount(const NativeAdmissionAuthority& a){return a.rowCount_;}
    static size_t live(const NativeAdmissionAuthority& a){
        size_t n=0;for(size_t i=0;i<a.limits_.leases;++i)if(a.leases_[i].live)++n;return n;
    }
    static size_t tombstones(const NativeAdmissionAuthority& a){
        size_t n=0;for(size_t i=0;i<a.ownerSlots_;++i)if(a.ownerIndex_[i].epoch==a.lineage_&&a.ownerIndex_[i].state==2)++n;return n;
    }
    static void ownerFault(NativeAdmissionAuthority& a,const Ticket& t,unsigned mode){
        auto& n=a.ownerIndex_[slot(a,t)];
        if(mode==0)n.state=0;else if(mode==1)--n.epoch;else if(mode==2)n.state=3;
        else if(mode==3)n.lease=UINT16_MAX;else if(mode==4)++n.train;else n.state=2;
    }
    static void chainFault(NativeAdmissionAuthority& a,const Ticket& t,unsigned mode){
        const auto index=lease(a,t);auto& h=a.heads_[index];
        if(mode==0)h.row=UINT16_MAX;
        else if(mode==1)--h.epoch;
        else if(mode==2)a.rows_[h.row].next=UINT16_MAX-1;
        else if(mode==3)a.rows_[h.row].next=h.row;
        else if(mode==4){auto second=a.rows_[h.row].next;a.rows_[second].next=h.row;}
        else if(mode==5){auto second=a.rows_[h.row].next;a.rows_[second].next=UINT16_MAX;}
        else if(mode==6){++h.resourceCount;}
        else if(mode==7){a.leases_[index].physicalCount=513;h.physicalCount=513;}
        else if(mode==8){a.rows_[h.row].lease=UINT16_MAX;}
    }
    static void globalFault(NativeAdmissionAuthority& a,unsigned mode){
        if(mode==0)a.rows_[0].lease=UINT16_MAX;else a.rowCount_=a.limits_.resourceRows+1;
    }
    static size_t bytes(const NativeAdmissionAuthority& a){return a.limits_.leases*sizeof(NativeAdmissionAuthority::Lease)+
        2*a.limits_.resourceRows*sizeof(NativeAdmissionAuthority::Row)+a.ownerSlots_*sizeof(NativeAdmissionAuthority::OwnerSlot)+
        2*a.limits_.leases*sizeof(NativeAdmissionAuthority::Head);}
    static void edgeRows(NativeAdmissionAuthority& a,const Ticket& t){
        const auto index=lease(a,t);const auto head=a.heads_[index].row;const auto next=a.rows_[head].next;
        const auto first=a.rows_[head],second=a.rows_[next];
        a.rows_[32766]=first;a.rows_[32767]=second;a.rows_[32766].next=32767;a.rows_[32767].next=UINT16_MAX;
        a.heads_[index].row=32766;a.rowCount_=32768;
    }
};
}
namespace {
using namespace nimby::windows::admission;
using Access=NativeAdmissionAuthorityTestAccess;
constexpr uint64_t id(unsigned kind,uint64_t ordinal=1){return(uint64_t(kind)<<48)|(ordinal<<16)|1;}
constexpr NativeDecision allowed{true,true,NativeDomain::FinalAdmission};
struct Fixture {
    NativeAdmissionAuthority authority;
    Frame frame{1,1,100,{1,1,1,rear::Phase::BeforeMovement}};
    std::thread::id thread=std::this_thread::get_id();
    bool held=true,expireAtStagedPublication=false;
    explicit Fixture(Limits limits={8,64,4096,1000}):authority(limits){
        CHECK(authority.beginWorld(frame,{publication(),true,true,true}).reason==Reason::Ready);
    }
    static bool current(void* context,const PublicationWitness& w) noexcept {
        auto& f=*static_cast<Fixture*>(context);
        if(f.expireAtStagedPublication&&Access::rowCount(f.authority)==2&&Access::live(f.authority)==1){
            // A complete second claim stage increments its scratch epoch but
            // has not published a second owner yet. See focused test below.
            ++f.ownerChecks;if(f.ownerChecks==2)f.held=false;
        }
        return f.held&&f.thread==std::this_thread::get_id()&&w.lineage==f.frame.lineage&&w.window==f.frame.window&&w.at==f.frame.at;
    }
    unsigned ownerChecks=0;
    static bool windowCurrent(void* context,const OwnedAdmissionWindow& w) noexcept {
        return w.continuity==1&&current(context,w.publication);
    }
    PublicationWitness publication(){return{frame.lineage,frame.window,frame.at,true,true,current,this};}
    OwnedAdmissionWindow window(){return{publication(),1,true,windowCurrent,this};}
    AdmissionRequest request(uint64_t ordinal=1,bool two=false){
        AdmissionRequest r;r.ticket={frame.at.session,id(5,ordinal),id(8),7};
        r.publication=publication();r.capturedAtMs=frame.nowMs;r.consistRevision=3;
        r.routeComplete=r.conflictsComplete=r.physicalOccupationComplete=r.scriptVetoComplete=true;
        r.physicalClear=r.scriptAllows=Proof::Yes;r.entered=Proof::No;
        r.resourceCount=r.physicalCount=two?2:1;
        r.resources[0]=r.ownPhysical[0]={id(1,ordinal*2),.1,.3};
        if(two)r.resources[1]=r.ownPhysical[1]={id(1,ordinal*2+1),.1,.3};
        return r;
    }
    Result admit(const AdmissionRequest& r){return authority.admit(frame,r,allowed,Mode::FinalVeto);}
    void advance(rear::Phase phase){++frame.window;++frame.nowMs;++frame.at.tick;frame.at.phase=phase;}
    Result cancel(const ProvisionalToken& token){
        NoEntryWitness witness{token,publication(),frame.nowMs,1,token.window,frame.window,true,true,true,true,true};
        return authority.cancelBeforeEntry(frame,witness,window());
    }
    Result remove(const AdmissionRequest& old){
        auto r=old;r.publication=publication();r.capturedAtMs=frame.nowMs;
        RetainedSnapshot snapshot;CHECK(authority.snapshot(r.ticket,snapshot).reason==Reason::Retained);
        advance(rear::Phase::RetainedMovement);
        EntryWitness entry{r.ticket,snapshot.leaseSerial,3,publication(),true,true,true,true};
        CHECK(authority.commitEntry(frame,entry).reason==Reason::Entered);
        CHECK(authority.snapshot(r.ticket,snapshot).reason==Reason::Retained);
        const std::array<rear::HistorySection,1> sections{{{id(1,1000000),10,.875,.125,1000,-1,0,rear::HistoryEnd::QualifiedEnd,false,0,0,0}}};
        const rear::Observation body{{r.ticket.train,3,100,100,frame.at,true,true,true},
            {r.ticket.train,3,frame.at,true,true,sections},{r.ticket.train,id(1,1000000),0,.625,-1,0,frame.at,true},true};
        const auto result=authority.releaseRear(frame,publication(),snapshot,true,body);
        advance(rear::Phase::BeforeMovement);return result;
    }
};
void collisionsAndReset(){
    Fixture f;std::vector<uint64_t> ordinals;const auto bucket=Access::hash(f.authority,f.request().ticket);
    for(uint64_t i=1;ordinals.size()<8;++i)if(Access::hash(f.authority,f.request(i).ticket)==bucket)ordinals.push_back(i);
    for(const auto i:ordinals)CHECK(f.admit(f.request(i)).mayPass);
    const auto middle=f.request(ordinals[3]);CHECK(f.remove(middle).reason==Reason::Released);
    CHECK(Access::tombstones(f.authority)==1);
    for(size_t i=0;i<ordinals.size();++i)if(i!=3)CHECK(f.admit(f.request(ordinals[i])).mayPass);
    auto generation=f.request(ordinals[3]);++generation.ticket.train;++generation.ticket.source;
    CHECK(f.admit(generation).mayPass);CHECK(f.authority.retain(middle.ticket).reason==Reason::Missing);
    auto route=generation;++route.ticket.routeRevision;CHECK(f.admit(route).reason==Reason::RouteChangedRetained);
    ++f.frame.lineage;++f.frame.at.session;++f.frame.at.epoch;f.advance(rear::Phase::BeforeMovement);
    CHECK(f.authority.beginWorld(f.frame,{f.publication(),true,true,true}).reason==Reason::Ready);
    CHECK(Access::rowCount(f.authority)==0&&Access::live(f.authority)==0);
    CHECK(f.authority.retain(generation.ticket).reason==Reason::Missing);
    auto fresh=f.request(ordinals[3]);fresh.ticket.train=generation.ticket.train;fresh.ticket.source=generation.ticket.source;
    CHECK(f.admit(fresh).mayPass);RetainedSnapshot out;
    CHECK(f.authority.snapshot(fresh.ticket,out).reason==Reason::Retained&&out.ticket==fresh.ticket&&out.resourceCount==1);
}
void fullTombstonesAndChurn(){
    Fixture f({2,4,4096,1000});const auto slots=Access::slots(f.authority);
    for(size_t bucket=0;bucket<slots;++bucket){
        uint64_t ordinal=1;while(Access::hash(f.authority,f.request(ordinal).ticket)!=bucket)++ordinal;
        auto r=f.request(ordinal);ProvisionalToken token;
        CHECK(f.authority.claimProvisional(f.frame,r,f.window(),token).reason==Reason::ProvisionalClaimed);
        CHECK(f.cancel(token).reason==Reason::CancelledBeforeEntry);
    }
    CHECK(Access::tombstones(f.authority)==slots&&Access::live(f.authority)==0);
    for(uint64_t i=1000;i<3000;++i){
        auto r=f.request(i);ProvisionalToken token;
        CHECK(f.authority.claimProvisional(f.frame,r,f.window(),token).reason==Reason::ProvisionalClaimed);
        CHECK(f.cancel(token).reason==Reason::CancelledBeforeEntry);
        CHECK(Access::live(f.authority)==0&&Access::rowCount(f.authority)==0&&Access::tombstones(f.authority)==slots);
    }
    auto r=f.request(4000);CHECK(f.admit(r).mayPass);
    CHECK(f.authority.storageBytes()==Access::bytes(f.authority));
}
void atomicStagingAndBudget(){
    Fixture full;auto a=full.request();CHECK(full.admit(a).mayPass);
    const auto needed=full.admit(full.request(2)).work;CHECK(needed>16);
    for(size_t budget=1;budget<needed;++budget){
        Fixture f;auto first=f.request();CHECK(f.admit(first).mayPass);
        RetainedSnapshot before;CHECK(f.authority.snapshot(first.ticket,before).reason==Reason::Retained);
        Access::operations(f.authority,budget);const auto failed=f.admit(f.request(2));
        CHECK(!failed.mayPass&&failed.work<=budget&&Access::live(f.authority)==1&&Access::rowCount(f.authority)==2);
        Access::operations(f.authority,4096);RetainedSnapshot after;
        CHECK(f.authority.snapshot(first.ticket,after).reason==Reason::Retained&&after.leaseSerial==before.leaseSerial&&
            after.resourceCount==before.resourceCount&&after.resources[0]==before.resources[0]);
        CHECK(f.authority.retain(f.request(2).ticket).reason==Reason::Missing);
        CHECK(f.admit(f.request(2)).mayPass);CHECK(f.admit(f.request()).mayPass);
    }
    Fixture loss;CHECK(loss.admit(loss.request()).mayPass);loss.expireAtStagedPublication=true;
    const auto currentEpoch=Access::currentEpoch(loss.authority),stagedEpoch=Access::stagedEpoch(loss.authority);
    const auto failed=loss.admit(loss.request(2));CHECK(failed.reason==Reason::UnqualifiedPhase&&!failed.mayPass);
    CHECK(Access::live(loss.authority)==1&&Access::rowCount(loss.authority)==2);
    CHECK(Access::currentEpoch(loss.authority)==currentEpoch&&Access::stagedEpoch(loss.authority)==stagedEpoch+1);
    loss.expireAtStagedPublication=false;loss.held=true;CHECK(loss.admit(loss.request(2)).mayPass);
    Fixture epoch;CHECK(epoch.admit(epoch.request()).mayPass);Access::exhaustEpoch(epoch.authority);
    const auto exhausted=epoch.admit(epoch.request(2));CHECK(exhausted.reason==Reason::IndexEpochExhausted&&!exhausted.mayPass);
    CHECK(Access::live(epoch.authority)==1&&epoch.admit(epoch.request()).mayPass);
}
void removalStagingAndBudget(){
    const auto claims=[](Fixture& f,ProvisionalToken& token){
        auto r=f.request();CHECK(f.authority.claimProvisional(f.frame,r,f.window(),token).reason==Reason::ProvisionalClaimed);
        CHECK(f.admit(f.request(2)).mayPass);
    };
    Fixture full;ProvisionalToken token;claims(full,token);
    const auto needed=full.cancel(token).work;
    for(size_t budget=1;budget<needed;++budget){
        Fixture f;ProvisionalToken claim;claims(f,claim);Access::operations(f.authority,budget);
        const auto denied=f.cancel(claim);CHECK(denied.reason!=Reason::CancelledBeforeEntry&&denied.work<=budget);
        CHECK(Access::live(f.authority)==2&&Access::rowCount(f.authority)==4);
        Access::operations(f.authority,4096);RetainedSnapshot first,other;
        CHECK(f.authority.snapshot(claim.ticket,first).reason==Reason::Retained&&first.leaseSerial==claim.leaseSerial);
        CHECK(f.authority.snapshot(f.request(2).ticket,other).reason==Reason::Retained);
        CHECK(f.cancel(claim).reason==Reason::CancelledBeforeEntry&&Access::live(f.authority)==1);
        CHECK(f.admit(f.request(2)).mayPass);
    }
    // Rear-release staging must also leave every retained resource live on a
    // budget refusal, then reuse its scratch safely after restoring quota.
    Fixture release;auto r=release.request();CHECK(release.admit(r).mayPass);CHECK(release.admit(release.request(2)).mayPass);
    RetainedSnapshot retained;CHECK(release.authority.snapshot(r.ticket,retained).reason==Reason::Retained);
    release.advance(rear::Phase::RetainedMovement);
    CHECK(release.authority.commitEntry(release.frame,{r.ticket,retained.leaseSerial,3,release.publication(),true,true,true,true}).reason==Reason::Entered);
    CHECK(release.authority.snapshot(r.ticket,retained).reason==Reason::Retained);
    const std::array<rear::HistorySection,1> sections{{{id(1,1000000),10,.875,.125,1000,-1,0,rear::HistoryEnd::QualifiedEnd,false,0,0,0}}};
    const rear::Observation body{{r.ticket.train,3,100,100,release.frame.at,true,true,true},
        {r.ticket.train,3,release.frame.at,true,true,sections},{r.ticket.train,id(1,1000000),0,.625,-1,0,release.frame.at,true},true};
    Access::operations(release.authority,1);
    const auto denied=release.authority.releaseRear(release.frame,release.publication(),retained,true,body);
    CHECK(denied.reason!=Reason::Released&&Access::live(release.authority)==2&&Access::rowCount(release.authority)==4);
    Access::operations(release.authority,4096);
    CHECK(release.authority.releaseRear(release.frame,release.publication(),retained,true,body).reason==Reason::Released);
}
void corruptCachesClose(){
    for(unsigned fault=0;fault<6;++fault){
        Fixture f;auto r=f.request();CHECK(f.admit(r).mayPass);Access::ownerFault(f.authority,r.ticket,fault);
        auto changed=r;++changed.ticket.routeRevision;const auto closed=f.admit(changed);
        CHECK(closed.reason==Reason::IndexInvalid&&!closed.mayPass&&Access::live(f.authority)==1&&Access::rowCount(f.authority)==2);
        CHECK(f.authority.retain(r.ticket).reason==Reason::IndexInvalid);
    }
    for(unsigned fault=0;fault<9;++fault){
        Fixture f;auto r=f.request(1,true);CHECK(f.admit(r).mayPass);Access::chainFault(f.authority,r.ticket,fault);
        const auto closed=f.admit(r);CHECK(closed.reason==Reason::IndexInvalid&&!closed.mayPass);
        RetainedSnapshot output;output.ticket={9,id(5,99),id(8),99};
        CHECK(f.authority.snapshot(r.ticket,output).reason==Reason::IndexInvalid&&output.ticket.session==9);
        auto shortened=r;shortened.resourceCount=shortened.physicalCount=1;
        CHECK(!f.admit(shortened).mayPass&&Access::live(f.authority)==1&&Access::rowCount(f.authority)==4);
    }
    for(unsigned fault=0;fault<2;++fault){
        Fixture f;auto r=f.request();CHECK(f.admit(r).mayPass);Access::globalFault(f.authority,fault);
        auto other=f.request(2);other.resources[0]=other.ownPhysical[0]=r.resources[0];
        CHECK(f.admit(other).reason==Reason::IndexInvalid&&Access::live(f.authority)==1);
    }
}
void rowSentinelAndNoAllocations(){
    Fixture edge({8,32768,65536,1000});auto r=edge.request();CHECK(edge.admit(r).mayPass);
    Access::edgeRows(edge.authority,r.ticket);RetainedSnapshot output;
    CHECK(edge.authority.snapshot(r.ticket,output).reason==Reason::Retained&&output.resourceCount==1&&output.physicalCount==1);
    CHECK(edge.admit(r).mayPass);Access::chainFault(edge.authority,r.ticket,2);CHECK(!edge.admit(r).mayPass);
    for(int failure=0;failure<6;++failure){
        bool thrown=false;failAllocation=failure;try{NativeAdmissionAuthority authority({8,64,4096,1000});}
        catch(const std::bad_alloc&){thrown=true;}failAllocation=-1;CHECK(thrown);
    }
    Fixture f;auto a=f.request(),b=f.request(2);RetainedSnapshot out;
    allocations=0;watchAllocations=true;
    CHECK(f.admit(a).mayPass);CHECK(f.admit(a).mayPass);CHECK(f.authority.snapshot(a.ticket,out).reason==Reason::Retained);
    CHECK(f.admit(b).mayPass);CHECK(f.admit(b).mayPass);CHECK(f.authority.retain(a.ticket).reason==Reason::Retained);
    CHECK(f.remove(a).reason==Reason::Released);
    auto c=f.request(3);ProvisionalToken token;
    CHECK(f.authority.claimProvisional(f.frame,c,f.window(),token).reason==Reason::ProvisionalClaimed);
    CHECK(f.cancel(token).reason==Reason::CancelledBeforeEntry);
    watchAllocations=false;CHECK(allocations==0);
}
}
int main(){try{
    collisionsAndReset();fullTombstonesAndChurn();atomicStagingAndBudget();removalStagingAndBudget();corruptCachesClose();rowSentinelAndNoAllocations();
    std::cout<<"native_admission_index: "<<checks<<" checks PASS; synthetic bounded cache only; no native replacement\n";
    return 0;
}catch(const std::exception& e){watchAllocations=false;failAllocation=-1;std::cerr<<e.what()<<'\n';return 1;}}
