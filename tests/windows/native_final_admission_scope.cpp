// Synthetic fixture. It neither installs native hooks nor certifies phase,
// world lineage, ABI ownership, route capture or writer exclusion in game.
#include <platform/windows/runtime/native_final_admission_scope.h>
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
size_t checks{};
}
#if defined(__GNUC__)
#define FINAL_NOINLINE __attribute__((noinline))
#else
#define FINAL_NOINLINE __declspec(noinline)
#endif
FINAL_NOINLINE void* operator new(size_t size) {
    if(watchAllocations.load())++allocations;
    if(void* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
FINAL_NOINLINE void* operator new[](size_t size){return ::operator new(size);}
FINAL_NOINLINE void operator delete(void* value) noexcept {std::free(value);}
FINAL_NOINLINE void operator delete[](void* value) noexcept {std::free(value);}
FINAL_NOINLINE void operator delete(void* value,size_t) noexcept {std::free(value);}
FINAL_NOINLINE void operator delete[](void* value,size_t) noexcept {std::free(value);}
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #value);} while(false)
namespace {
using namespace nimby::windows::final_admission;
namespace admission=nimby::windows::admission;
constexpr uint64_t id(unsigned type,uint64_t ordinal=1){return (uint64_t(type)<<48)|(ordinal<<16)|1;}
constexpr uintptr_t base=0x140000000;
Identity identity(uint64_t ordinal=1) {return {0x100000,0x200000+ordinal*0x100,0x300000+ordinal*0x100,0x400000,id(5,ordinal),id(8),7,true};}
void returnAndCallbacks() {
    auto before=identity();size_t originals=0,collections=0,decisions=0;
    FinalAdmissionScope scope(before);
    auto output=scope.invoke([&] {
        ++originals;CHECK(FinalAdmissionScope::depth()==1);
        CHECK(FinalAdmissionScope::current()->stage==Stage::Native);
        // Synthetic native physical check and 4273D0 completion.
        return uint8_t{0x7f};
    },[&](const Facts& facts) {
        ++collections;CHECK(facts.originalReturned&&facts.nativeCompositeCompleted());
        CHECK(facts.stage==Stage::Collecting);return before;
    },[&](const Facts& facts) {
        ++decisions;CHECK(facts.collectionReturned&&facts.stage==Stage::Judging);return true;
    });
    CHECK(output.value==0x7f&&output.reason==Reason::Passed&&originals==1&&collections==1&&decisions==1);
    CHECK(output.nativeDecision().admitted&&output.nativeDecision().qualified);
    const auto repeated=scope.invoke([&]{++originals;return uint8_t{1};},[&](auto&){++collections;return before;},[&](auto&){++decisions;return true;});
    CHECK(repeated.reason==Reason::RepeatedInvocation&&!repeated.value&&originals==1&&collections==1&&decisions==1);
}
void allRefusalsAndMutation() {
    const auto before=identity();
    // Occupation, virtual owner, script veto and unknown early refusal all keep
    // native AL=0. No SDK decision runs to turn any of them into a permission.
    for(unsigned refusal=0;refusal<4;++refusal) {
        FinalAdmissionScope scope(before);size_t original=0,collect=0,decide=0;
        const auto output=scope.invoke([&]{++original;return uint8_t{0};},[&](auto& facts){
            ++collect;CHECK(facts.originalReturned&&!facts.nativeCompositeCompleted());return before;
        },[&](auto&){++decide;return true;});
        CHECK(output.reason==Reason::NativeRefusal&&!output.value&&original==1&&collect==1&&decide==0);
        CHECK(!output.nativeDecision().admitted);
    }
    for(unsigned mutation=0;mutation<8;++mutation) {
        FinalAdmissionScope scope(before);auto after=before;
        if(mutation==0)++after.context;
        if(mutation==1)++after.train;
        if(mutation==2)++after.motion;
        if(mutation==3)++after.signal;
        if(mutation==4)++after.trainId;
        if(mutation==5)++after.sourceId;
        if(mutation==6)++after.routeRevision;
        if(mutation==7)after.copiedComplete=false;
        size_t decide=0;
        const auto output=scope.invoke([]{return uint8_t{1};},[&](auto&){return after;},[&](auto&){++decide;return true;});
        CHECK(!output.value&&!output.nativeDecision().qualified&&decide==0);
        CHECK(output.reason==(mutation==6?Reason::RouteChanged:mutation==7?Reason::IncompleteCapture:Reason::IdentityChanged));
    }
    FinalAdmissionScope stopped(before);
    const auto output=stopped.invoke([]{return uint8_t{1};},[&](auto&){return before;},[](auto&){return false;});
    CHECK(!output.value&&output.reason==Reason::SdkRefusal&&!output.nativeDecision().admitted);
}
void exceptionsAndReentrancy() {
    const auto before=identity();size_t originals=0,collections=0,decisions=0;
    {
        FinalAdmissionScope scope(before);
        const auto output=scope.invoke([&]() -> uint8_t {++originals;throw 1;},[&](auto&){++collections;return before;},[&](auto&){++decisions;return true;});
        CHECK(output.reason==Reason::OriginalException&&originals==1&&collections==0&&decisions==0&&!output.nativeDecision().qualified);
    }
    {
        FinalAdmissionScope scope(before);
        const auto output=scope.invoke([&]{++originals;return uint8_t{1};},[&](auto&) -> Identity {++collections;throw 2;},[&](auto&){++decisions;return true;});
        CHECK(output.reason==Reason::CollectionException&&originals==2&&collections==1&&decisions==0);
    }
    {
        FinalAdmissionScope scope(before);
        const auto output=scope.invoke([&]{++originals;return uint8_t{1};},[&](auto&){++collections;return before;},[&](auto&) -> bool {++decisions;throw 3;});
        CHECK(output.reason==Reason::DecisionException&&originals==3&&collections==2&&decisions==1&&!output.nativeDecision().qualified);
    }
    CHECK(!FinalAdmissionScope::current());
    {
        FinalAdmissionScope parent(before);
        const auto output=parent.invoke([&] {
            const auto* saved=FinalAdmissionScope::current();const auto childIdentity=identity(2);
            {
                FinalAdmissionScope child(childIdentity);CHECK(FinalAdmissionScope::depth()==2);
                CHECK(FinalAdmissionScope::current()->before.motion==childIdentity.motion);
                const auto inner=child.invoke([]{return uint8_t{1};},[&](auto&){return childIdentity;},[](auto&){return true;});
                CHECK(inner.value==1);
            }
            CHECK(FinalAdmissionScope::current()==saved&&saved->stage==Stage::Native);return uint8_t{1};
        },[&](auto&){return before;},[](auto&){return true;});
        CHECK(output.value==1);
    }
    CHECK(!FinalAdmissionScope::current());
}
Output recursive(size_t depth,size_t& originals,size_t& collections) {
    const auto before=identity(depth+1);FinalAdmissionScope scope(before);
    return scope.invoke([&] {
        ++originals;
        if(depth<maximumDepth)(void)recursive(depth+1,originals,collections);
        return uint8_t{1};
    },[&](auto&){++collections;return before;},[](auto&){return true;});
}
void recursionBoundAndThreadLocal() {
    size_t originals=0,collections=0;const auto output=recursive(0,originals,collections);
    CHECK(!output.value&&output.reason==Reason::DepthExceeded&&originals==maximumDepth+1&&collections==maximumDepth+1);
    CHECK(FinalAdmissionScope::depth()==0&&!FinalAdmissionScope::current());
    bool isolated=false;const auto before=identity();FinalAdmissionScope parent(before);
    std::thread other([&] {
        isolated=FinalAdmissionScope::depth()==0&&FinalAdmissionScope::current()==nullptr;
        FinalAdmissionScope child(identity(2));isolated&=FinalAdmissionScope::depth()==1;
    });other.join();CHECK(isolated&&FinalAdmissionScope::depth()==1);
}
void privateContextFacts() {
    const auto before=identity();uint8_t liveByte=1;uintptr_t motion=before.motion;
    std::array<uintptr_t,3> live{{0x500000,reinterpret_cast<uintptr_t>(&motion),reinterpret_cast<uintptr_t>(&liveByte)}};
    FinalAdmissionScope::ContextCopy copy{reinterpret_cast<uintptr_t>(live.data()),before.motion,before.trainId,before.sourceId,live,1,true};
    size_t original=0;
    const auto exclusive=[&](uintptr_t context) {
        ++original;auto* words=reinterpret_cast<uintptr_t*>(context);
        CHECK(words[0]==live[0]&&words[1]==live[1]);*reinterpret_cast<uint8_t*>(words[2])=0;return uint8_t{0};
    };
    {
        FinalAdmissionScope scope(before);
        const auto output=scope.invoke([&] {
            const auto inspection=FinalAdmissionScope::inspectPrivateExclusive(copy,exclusive);
            CHECK(inspection.privateContextUsed&&inspection.originalReturned&&inspection.nativeReturn==0);
            CHECK(inspection.continuationBefore==1&&inspection.continuationAfter==0&&liveByte==1);
            // No fabricated lease: inspection alone cannot upgrade its result.
            return inspection.nativeReturn;
        },[&](auto&){return before;},[](auto&){return true;});
        CHECK(!output.value&&output.reason==Reason::NativeRefusal&&output.facts.exclusiveCalls==1&&output.facts.exclusiveRefusals==1&&original==1);
    }
    liveByte=1;
    {
        FinalAdmissionScope scope(before);auto mismatched=copy;++mismatched.motion;
        const auto output=scope.invoke([&] {
            const auto inspection=FinalAdmissionScope::inspectPrivateExclusive(mismatched,exclusive);
            CHECK(!inspection.privateContextUsed&&inspection.reason==Reason::UnqualifiedProjection&&liveByte==0);
            return inspection.nativeReturn;
        },[&](auto&){return before;},[](auto&){return true;});
        CHECK(!output.value&&original==2);
    }
    liveByte=1;
    {
        FinalAdmissionScope scope(before);
        const auto output=scope.invoke([&] {
            const auto inspection=FinalAdmissionScope::inspectPrivateExclusive(copy,[&](uintptr_t) -> uint8_t {++original;throw 4;});
            CHECK(inspection.reason==Reason::ProjectionException);return uint8_t{1};
        },[&](auto&){return before;},[](auto&){return true;});
        CHECK(!output.value&&output.reason==Reason::ProjectionException&&original==3&&liveByte==1);
    }
}
void onlyTwoBypassReturns() {
    BypassFacts facts{base,base+bypassFirstReturnRva,0x100,0x200,0x300,0x100,0x200,id(8),true,true};
    size_t originals=0,collections=0;
    const auto call=[&](uint8_t native=1) {
        return invokeBypass([&]{++originals;return native;},[&]{++collections;return facts;});
    };
    auto result=call();CHECK(result.value==0&&result.nativeReturn==1&&result.reason==Reason::ForcedNativeRecheck);
    facts.returnAddress=base+bypassSecondReturnRva;CHECK(call().reason==Reason::ForcedNativeRecheck);
    facts.returnAddress=base+0x449c9b;CHECK(call().value==1); // Native lookahead unchanged.
    facts.returnAddress=base+0x448b24;CHECK(call().value==1); // CALL address is NOT return address.
    facts.returnAddress=base+bypassFirstReturnRva;
    for(unsigned invalid=0;invalid<6;++invalid) {
        auto changed=facts;
        if(invalid==0)changed.managed=false;
        if(invalid==1)changed.copiedComplete=false;
        if(invalid==2)changed.currentStepContext=0;
        if(invalid==3)++changed.currentStepMotion;
        if(invalid==4)changed.sourceId=id(5);
        if(invalid==5)changed.signal=0;
        const auto unchanged=invokeBypass([]{return uint8_t{0x7f};},[&]{return changed;});
        CHECK(unchanged.value==0x7f&&unchanged.reason==Reason::NativeUnchanged);
    }
    result=call(0);CHECK(!result.value&&result.reason==Reason::NativeUnchanged&&originals==5&&collections==5);
    result=invokeBypass([&]() -> uint8_t {++originals;throw 5;},[&]{++collections;return facts;});
    CHECK(result.reason==Reason::OriginalException&&originals==6&&collections==5);
}
void actualAuthorityDecisionAndNoAllocation() {
    admission::NativeAdmissionAuthority authority({8,64,1024,1000});
    admission::Frame frame{1,1,100,{1,1,1,admission::rear::Phase::BeforeMovement}};
    admission::PublicationWitness publication{1,1,frame.at,true,true};
    publication.current=[](void*,const admission::PublicationWitness&) noexcept {return true;}; // Synthetic fixture owner only.
    CHECK(authority.beginWorld(frame,{publication,true,true,true}).reason==admission::Reason::Ready);
    admission::AdmissionRequest request;const auto before=identity();
    request.ticket={1,before.trainId,before.sourceId,before.routeRevision};request.publication=publication;
    request.capturedAtMs=100;request.consistRevision=3;
    request.routeComplete=request.conflictsComplete=request.physicalOccupationComplete=request.scriptVetoComplete=true;
    request.physicalClear=request.scriptAllows=admission::Proof::Yes;request.entered=admission::Proof::No;
    request.resourceCount=request.physicalCount=1;request.resources[0]=request.ownPhysical[0]={id(1),.1,.3};
    allocations=0;watchAllocations=true;Output output;
    {
        FinalAdmissionScope scope(before);
        output=scope.invoke([]{return uint8_t{1};},[&](auto&){return before;},[&](auto& facts) {
            // A real fixture authority result, not a fabricated permission bit.
            const admission::NativeDecision decision{facts.nativeReturn!=0,facts.nativeCompositeCompleted(),admission::NativeDomain::FinalAdmission};
            return authority.admit(frame,request,decision,admission::Mode::FinalVeto).mayPass;
        });
    }
    watchAllocations=false;
    CHECK(output.value==1&&output.nativeDecision().admitted&&allocations==0&&FinalAdmissionScope::depth()==0);
    auto other=request;other.ticket.train=id(5,2);
    CHECK(authority.admit(frame,other,{true,true,admission::NativeDomain::FinalAdmission},admission::Mode::FinalVeto).reason==admission::Reason::Conflict);
}
}
int main() {
    try {
        static_assert(!std::is_copy_constructible_v<FinalAdmissionScope>);
        returnAndCallbacks();allRefusalsAndMutation();exceptionsAndReentrancy();recursionBoundAndThreadLocal();
        privateContextFacts();onlyTwoBypassReturns();actualAuthorityDecisionAndNoAllocation();
        CHECK(FinalAdmissionScope::depth()==0);
        std::cout<<"native_final_admission_scope: "<<checks<<" checks PASS; synthetic contract only; hooks/ABI/provider not certified\n";return 0;
    } catch(const std::exception& error) {watchAllocations=false;std::cerr<<error.what()<<'\n';return 1;}
}
