// Synthetic SDK ownership contract only. This fixture installs no native hook,
// supplies no production owner and cannot certify replacement in the game.
#include <platform/windows/runtime/native_admission_authority.h>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace {
using namespace nimby::windows::admission;
size_t checks=0;
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);} while(false)
constexpr uint64_t id(unsigned kind,uint64_t index=1){return (uint64_t(kind)<<48)|(index<<16)|1;}
constexpr auto track=id(1),elsewhere=id(1,3);
constexpr NativeDecision allowed{true,true,NativeDomain::FinalAdmission};
struct Owner {
    Frame expected;
    std::thread::id thread=std::this_thread::get_id();
    bool held=true;
    size_t calls=0,rejectAt=SIZE_MAX;
    static bool current(void* context,const PublicationWitness& publication) noexcept {
        auto& owner=*static_cast<Owner*>(context);
        ++owner.calls;
        return owner.held&&owner.calls<owner.rejectAt&&std::this_thread::get_id()==owner.thread&&
            publication.lineage==owner.expected.lineage&&publication.window==owner.expected.window&&
            publication.at==owner.expected.at;
    }
    void reset(const Frame& frame,size_t reject=SIZE_MAX){expected=frame;calls=0;rejectAt=reject;held=true;}
};
struct Fixture {
    NativeAdmissionAuthority authority{{8,64,1024,1000}};
    Frame frame{1,1,100,{1,1,1,rear::Phase::BeforeMovement}};
    Owner owner;
    Fixture(){owner.reset(frame);CHECK(authority.beginWorld(frame,ready()).reason==Reason::Ready);owner.reset(frame);}
    PublicationWitness publication(){return {frame.lineage,frame.window,frame.at,true,true,Owner::current,&owner};}
    WorldReadyWitness ready(){return {publication(),true,true,true};}
    void advance(rear::Phase phase){++frame.window;++frame.nowMs;++frame.at.tick;frame.at.phase=phase;owner.reset(frame);}
    AdmissionRequest request(uint64_t train=1){
        AdmissionRequest value;value.ticket={frame.at.session,id(5,train),id(8),7};
        value.publication=publication();value.consistRevision=3;value.capturedAtMs=frame.nowMs;
        value.routeComplete=value.conflictsComplete=value.physicalOccupationComplete=value.scriptVetoComplete=true;
        value.physicalClear=value.scriptAllows=Proof::Yes;value.entered=Proof::No;
        value.resourceCount=value.physicalCount=1;value.resources[0]=value.ownPhysical[0]={track,.1,.3};return value;
    }
    Result admit(const AdmissionRequest& request){return authority.admit(frame,request,allowed,Mode::FinalVeto);}
    EntryWitness entry(const AdmissionRequest& request,const Result& admitted){
        return {request.ticket,admitted.leaseSerial,request.consistRevision,publication(),true,true,true,true};
    }
};
struct Body {
    std::array<rear::HistorySection,1> sections{{{elsewhere,10,.875,.125,1000,-1,0,rear::HistoryEnd::QualifiedEnd,false,0,0,0}}};
    rear::Observation observation;
    Body(const Fixture& fixture,const AdmissionRequest& request):observation{
        {request.ticket.train,request.consistRevision,100,100,fixture.frame.at,true,true,true},
        {request.ticket.train,request.consistRevision,fixture.frame.at,true,true,sections},
        {request.ticket.train,elsewhere,0,.625,-1,0,fixture.frame.at,true},true}{}
};
void declarationsAreNotCurrentOwnership(){
    NativeAdmissionAuthority authority{{8,64,1024,1000}};
    Frame frame{1,1,100,{1,1,1,rear::Phase::BeforeMovement}};
    PublicationWitness declaration{1,1,frame.at,true,true};
    CHECK(authority.beginWorld(frame,{declaration,true,true,true}).reason==Reason::InvalidWorldReady);
    Fixture fixture;auto request=fixture.request();request.publication.current=nullptr;
    auto denied=fixture.admit(request);CHECK(!denied.mayPass&&denied.reason==Reason::UnqualifiedPhase);
    fixture.owner.held=false;request=fixture.request();
    denied=fixture.admit(request);CHECK(!denied.mayPass&&denied.reason==Reason::UnqualifiedPhase);
    fixture.owner.reset(fixture.frame);Result foreign;
    std::thread worker([&]{foreign=fixture.admit(request);});worker.join();
    CHECK(!foreign.mayPass&&foreign.reason==Reason::UnqualifiedPhase);
    ++fixture.owner.expected.window;denied=fixture.admit(request);
    CHECK(!denied.mayPass&&denied.reason==Reason::UnqualifiedPhase);
}
void admissionRechecksBeforePublication(){
    Fixture fixture;auto request=fixture.request();fixture.owner.reset(fixture.frame,2);
    auto denied=fixture.admit(request);CHECK(!denied.mayPass&&denied.reason==Reason::UnqualifiedPhase);
    RetainedSnapshot snapshot;CHECK(fixture.authority.snapshot(request.ticket,snapshot).reason==Reason::Missing);
    fixture.owner.reset(fixture.frame);auto granted=fixture.admit(request);CHECK(granted.mayPass);
    fixture.owner.reset(fixture.frame,2);denied=fixture.admit(request);
    CHECK(!denied.mayPass&&denied.reason==Reason::UnqualifiedPhase&&denied.leaseHeld==Proof::Yes);
    fixture.owner.reset(fixture.frame);CHECK(fixture.admit(fixture.request(2)).reason==Reason::Conflict);
}
void entryAndReleaseRetainAfterOwnershipLoss(){
    Fixture fixture;auto request=fixture.request();auto granted=fixture.admit(request);CHECK(granted.mayPass);
    fixture.advance(rear::Phase::RetainedMovement);
    fixture.owner.reset(fixture.frame,2);auto entry=fixture.authority.commitEntry(fixture.frame,fixture.entry(request,granted));
    CHECK(entry.reason==Reason::UnqualifiedPhase&&entry.leaseHeld==Proof::Yes);
    RetainedSnapshot snapshot;CHECK(fixture.authority.snapshot(request.ticket,snapshot).leaseHeld==Proof::Yes);
    CHECK(snapshot.entered==Proof::Unknown);
    fixture.owner.reset(fixture.frame);CHECK(fixture.authority.commitEntry(fixture.frame,fixture.entry(request,granted)).reason==Reason::Entered);
    CHECK(fixture.authority.snapshot(request.ticket,snapshot).leaseHeld==Proof::Yes&&snapshot.entered==Proof::Yes);
    Body body(fixture,request);
    for(size_t refusal=1;refusal<=3;++refusal){
        fixture.owner.reset(fixture.frame,refusal);
        const auto denied=fixture.authority.releaseRear(fixture.frame,fixture.publication(),snapshot,true,body.observation);
        CHECK(denied.reason==Reason::UnqualifiedPhase&&!denied.mayPass&&denied.leaseHeld==Proof::Yes);
        CHECK(fixture.authority.snapshot(request.ticket,snapshot).leaseHeld==Proof::Yes&&snapshot.entered==Proof::Yes);
    }
    fixture.owner.reset(fixture.frame);
    const auto released=fixture.authority.releaseRear(fixture.frame,fixture.publication(),snapshot,true,body.observation);
    CHECK(released.reason==Reason::Released&&released.leaseHeld==Proof::No);
}
void newWorldCannotEraseProtectionAfterOwnershipLoss(){
    Fixture fixture;auto request=fixture.request();CHECK(fixture.admit(request).mayPass);
    ++fixture.frame.lineage;++fixture.frame.window;++fixture.frame.nowMs;++fixture.frame.at.session;
    fixture.owner.reset(fixture.frame,2);
    CHECK(fixture.authority.beginWorld(fixture.frame,fixture.ready()).reason==Reason::InvalidWorldReady);
    RetainedSnapshot snapshot;CHECK(fixture.authority.snapshot(request.ticket,snapshot).leaseHeld==Proof::Yes);
    fixture.owner.reset(fixture.frame);CHECK(fixture.authority.beginWorld(fixture.frame,fixture.ready()).reason==Reason::Ready);
    CHECK(fixture.authority.snapshot(request.ticket,snapshot).reason==Reason::Missing);
}
}
int main(){
    try {
        static_assert(!Result::nativeReplacementConnected);
        declarationsAreNotCurrentOwnership();admissionRechecksBeforePublication();
        entryAndReleaseRetainAfterOwnershipLoss();newWorldCannotEraseProtectionAfterOwnershipLoss();
        std::cout<<"native_admission_ownership: "<<checks<<" checks PASS; synthetic contract only; no production owner or native replacement\n";return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
