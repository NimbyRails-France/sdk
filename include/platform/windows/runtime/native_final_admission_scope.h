#pragma once
#include <platform/windows/runtime/native_admission_authority.h>

namespace nimby::windows::final_admission {
// Pinned interoperability facts only. This helper installs no hook, grants no
// lease, owns no game phase and does not certify any native pointer lifetime.
inline constexpr uintptr_t bypassFirstReturnRva=0x448b29,bypassSecondReturnRva=0x4493e9;
inline constexpr size_t maximumDepth=8;
enum class Stage : uint8_t { Created,Native,Returned,Collecting,Collected,Judging,Finished,Failed };
enum class Reason : uint8_t {
    Passed,NativeRefusal,SdkRefusal,IncompleteCapture,IdentityChanged,RouteChanged,
    OriginalException,CollectionException,DecisionException,ProjectionException,
    DepthExceeded,RepeatedInvocation,PrivateContextCopied,UnqualifiedProjection,
    NativeUnchanged,ForcedNativeRecheck
};
struct Identity {
    uintptr_t context=0,train=0,motion=0,signal=0;
    uint64_t trainId=0,sourceId=0,routeRevision=0;
    bool copiedComplete=false;
};
struct Facts {
    Identity before,after;
    Stage stage=Stage::Created;
    uint8_t nativeReturn=0;
    bool originalAttempted=false,originalReturned=false,collectionAttempted=false;
    bool collectionReturned=false,decisionAttempted=false,projectionFailed=false,overflow=false;
    size_t exclusiveCalls=0,exclusiveRefusals=0;
    // With the pinned 448210 implementation, a positive native return means
    // 451B60 returned true AND 4273D0 completed with EAX != 1. A zero cannot
    // distinguish early physical/exclusive refusal from the script veto.
    bool nativeCompositeCompleted() const noexcept {return originalReturned&&nativeReturn!=0;}
};
struct Output {
    uint8_t value=0;
    Reason reason=Reason::IncompleteCapture;
    Facts facts;
    admission::NativeDecision nativeDecision() const noexcept {
        return {value!=0,facts.collectionReturned&&facts.originalReturned&&
            (reason==Reason::Passed||reason==Reason::SdkRefusal||reason==Reason::NativeRefusal)&&
            !facts.projectionFailed&&!facts.overflow,admission::NativeDomain::FinalAdmission};
    }
};
namespace detail {
struct Context {std::array<Facts,maximumDepth> stack{};size_t depth=0;};
inline thread_local Context context;
inline bool id(uint64_t value,uint64_t kind) noexcept {return value>>48==kind&&(value&0x0000ffffffffffffULL)!=0;}
inline bool complete(const Identity& value) noexcept {
    return value.copiedComplete&&value.context&&value.train&&value.motion&&value.signal&&
        id(value.trainId,5)&&id(value.sourceId,8)&&value.routeRevision;
}
inline bool sameIdentity(const Identity& a,const Identity& b) noexcept {
    return a.context==b.context&&a.train==b.train&&a.motion==b.motion&&a.signal==b.signal&&
        a.trainId==b.trainId&&a.sourceId==b.sourceId;
}
}
class FinalAdmissionScope {
    size_t index_=maximumDepth;
    Facts fallback_;
    bool invoked_=false;
    Facts& facts() noexcept {return index_<maximumDepth?detail::context.stack[index_]:fallback_;}
public:
    explicit FinalAdmissionScope(const Identity& before) noexcept {
        auto& context=detail::context;
        if(context.depth<maximumDepth) {
            index_=context.depth++;context.stack[index_]=Facts{};context.stack[index_].before=before;
        } else {
            fallback_.before=before;fallback_.overflow=true;
            // Unsupported recursion invalidates authority in active parents;
            // it cannot overwrite another train's current scope.
            for(size_t i=0;i<context.depth;++i)context.stack[i].overflow=true;
        }
    }
    ~FinalAdmissionScope() {
        if(index_<maximumDepth) {
            detail::context.stack[index_]=Facts{};
            --detail::context.depth;
        }
    }
    FinalAdmissionScope(const FinalAdmissionScope&)=delete;
    FinalAdmissionScope& operator=(const FinalAdmissionScope&)=delete;
    static const Facts* current() noexcept {
        const auto& context=detail::context;
        return context.depth?&context.stack[context.depth-1]:nullptr;
    }
    static size_t depth() noexcept {return detail::context.depth;}
    // Every wrapper creates its own scope. No lock/gate/wait is acquired here.
    // Original is always attempted once, including incomplete input/overflow.
    // Collection is attempted once after a normal native return, and before
    // the SDK decision. A native zero can NEVER become a positive final result.
    template<class Original,class Collect,class Decide>
    Output invoke(Original&& original,Collect&& collect,Decide&& decide) noexcept {
        auto& value=facts();
        const auto finish=[&](Reason reason,uint8_t allowed=0) {
            value.stage=allowed?Stage::Finished:Stage::Failed;
            return Output{allowed,reason,value};
        };
        if(invoked_)return finish(Reason::RepeatedInvocation);
        invoked_=true;value.originalAttempted=true;value.stage=Stage::Native;
        try {value.nativeReturn=std::forward<Original>(original)();value.originalReturned=true;value.stage=Stage::Returned;}
        catch(...) {return finish(Reason::OriginalException);}
        value.collectionAttempted=true;value.stage=Stage::Collecting;
        try {value.after=std::forward<Collect>(collect)(value);value.collectionReturned=true;value.stage=Stage::Collected;}
        catch(...) {return finish(Reason::CollectionException);}
        if(value.overflow)return finish(Reason::DepthExceeded);
        if(value.projectionFailed)return finish(Reason::ProjectionException);
        // Preserve the native refusal even if its exact cause is unavailable.
        if(!value.nativeReturn)return finish(Reason::NativeRefusal);
        if(!detail::complete(value.before)||!detail::complete(value.after))return finish(Reason::IncompleteCapture);
        if(!detail::sameIdentity(value.before,value.after))return finish(Reason::IdentityChanged);
        if(value.before.routeRevision!=value.after.routeRevision)return finish(Reason::RouteChanged);
        value.decisionAttempted=true;value.stage=Stage::Judging;
        bool approved=false;
        try {approved=std::forward<Decide>(decide)(value);}
        catch(...) {return finish(Reason::DecisionException);}
        return finish(approved?Reason::Passed:Reason::SdkRefusal,approved?value.nativeReturn:0);
    }
    struct ContextCopy {
        uintptr_t original=0,motion=0;
        uint64_t trainId=0,sourceId=0;
        std::array<uintptr_t,3> words{};
        uint8_t continuation=0;
        bool copiedComplete=false;
    };
    struct PrivateExclusiveFacts {
        uint8_t nativeReturn=0,continuationBefore=0,continuationAfter=0;
        bool originalReturned=false,privateContextUsed=false;
        Reason reason=Reason::UnqualifiedProjection;
        // No replacement/permission flag: the bridge must separately possess
        // a real qualified authority lease before ignoring this raw refusal.
    };
    // 458460 mutates context[2] on exclusive refusal. Copying all three words
    // and redirecting ONLY word2 preserves lookup/locking and prevents that
    // exclusive side effect from stopping the native physical/veto traversal.
    // This returns raw facts ONLY. It never upgrades the original bool itself.
    // The caller supplies a copied context; this helper performs no memory read.
    template<class Original>
    static PrivateExclusiveFacts inspectPrivateExclusive(const ContextCopy& copy,Original&& original) noexcept {
        PrivateExclusiveFacts output;output.continuationBefore=output.continuationAfter=copy.continuation;
        auto* current=detail::context.depth?&detail::context.stack[detail::context.depth-1]:nullptr;
        const bool matched=current&&current->stage==Stage::Native&&!current->overflow&&
            detail::complete(current->before)&&copy.copiedComplete&&copy.original&&copy.words[0]&&copy.words[1]&&copy.words[2]&&
            copy.motion==current->before.motion&&copy.trainId==current->before.trainId&&copy.sourceId==current->before.sourceId;
        std::array<uintptr_t,3> words=copy.words;uint8_t byte=copy.continuation;
        if(matched)words[2]=reinterpret_cast<uintptr_t>(&byte);
        const auto context=matched?reinterpret_cast<uintptr_t>(words.data()):copy.original;
        if(current)++current->exclusiveCalls;
        try {
            output.nativeReturn=std::forward<Original>(original)(context);
            output.originalReturned=true;output.privateContextUsed=matched;
            output.continuationAfter=matched?byte:copy.continuation;
            output.reason=matched?Reason::PrivateContextCopied:Reason::UnqualifiedProjection;
            if(current&&!output.nativeReturn)++current->exclusiveRefusals;
        } catch(...) {
            output.reason=Reason::ProjectionException;
            if(current)current->projectionFailed=true;
        }
        return output;
    }
};

struct BypassFacts {
    uintptr_t imageBase=0,returnAddress=0;
    uintptr_t context=0,motion=0,signal=0,currentStepContext=0,currentStepMotion=0;
    uint64_t sourceId=0;
    bool copiedComplete=false,managed=false;
};
struct BypassOutput {uint8_t value=0,nativeReturn=0;Reason reason=Reason::NativeUnchanged;bool originalReturned=false;};
inline bool targetedBypass(const BypassFacts& facts) noexcept {
    if(!facts.copiedComplete||!facts.managed||!facts.imageBase||!facts.context||!facts.motion||!facts.signal||
       facts.context!=facts.currentStepContext||facts.motion!=facts.currentStepMotion||!detail::id(facts.sourceId,8)||
       facts.returnAddress<facts.imageBase)return false;
    const auto rva=facts.returnAddress-facts.imageBase;
    return rva==bypassFirstReturnRva||rva==bypassSecondReturnRva;
}
// 447310 only tests membership / existence of change_path callbacks. At these
// TWO pinned return addresses, forcing AL=0 selects the existing 448210 path;
// it does not execute/replay/delete the change_path callback. Other callers,
// including 449700 lookahead (return 449C9B), keep the exact native result.
template<class Original,class Collect>
BypassOutput invokeBypass(Original&& original,Collect&& collect) noexcept {
    BypassOutput output;
    try {output.value=output.nativeReturn=std::forward<Original>(original)();output.originalReturned=true;}
    catch(...) {output.reason=Reason::OriginalException;return output;}
    try {
        if(targetedBypass(std::forward<Collect>(collect)())&&output.nativeReturn) {
            output.value=0;output.reason=Reason::ForcedNativeRecheck;
        }
    } catch(...) {output.value=0;output.reason=Reason::CollectionException;}
    return output;
}
} // namespace nimby::windows::final_admission
