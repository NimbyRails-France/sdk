#pragma once
#include <platform/windows/runtime/native_final_admission_scope.h>

namespace nimby::windows::two_phase_admission {
namespace authority=nimby::windows::admission;
namespace final=nimby::windows::final_admission;

// Internal executable contract, no native adapter. In particular this wrapper
// never changes an exclusive native refusal into success and never supplies
// world/phase ownership. Its claim is protection, not movement permission.
struct FreshCapture {
    final::Identity identity;
    authority::Frame frame;
    authority::AdmissionRequest request;
    authority::OwnedAdmissionWindow owned;
};
struct Output {
    authority::ProvisionalToken claim;
    authority::Result provisional,finalized;
    final::Output native;
    static constexpr bool nativeReplacementConnected=false;
    bool mayPass()const noexcept {
        return native.reason==final::Reason::Passed&&native.value&&finalized.mayPass&&
            !finalized.replaceNativeExclusive;
    }
};

// Original and capture are each attempted once, outside the authority gate.
// Native zero never reaches finalization: it cannot prove script completion.
// No failure here automatically cancels a claim. Positive non-entry and a
// separate complete continuity certificate are required by cancelBeforeEntry.
// Its public token is already spent by claim publication. A private, non-copyable
// permit lives only in this invocation and is spent before attempting the judge.
class InvocationAccess {
public:
template<class Original,class Capture>
static Output invoke(authority::NativeAdmissionAuthority& owner,const authority::Frame& beforeFrame,
              const authority::AdmissionRequest& beforeRequest,const final::Identity& beforeIdentity,
              const authority::OwnedAdmissionWindow& beforeOwned,Original&& original,Capture&& capture) noexcept {
    Output out;
    authority::NativeAdmissionAuthority::InvocationPermit permit;
    const bool identity=final::detail::complete(beforeIdentity)&&
        beforeIdentity.trainId==beforeRequest.ticket.train&&beforeIdentity.sourceId==beforeRequest.ticket.source&&
        beforeIdentity.routeRevision==beforeRequest.ticket.routeRevision;
    if(identity)out.provisional=owner.claimForInvocation(beforeFrame,beforeRequest,beforeOwned,out.claim,permit);
    else {out.provisional=owner.retain(beforeRequest.ticket);out.provisional.reason=authority::Reason::IdentityChanged;}
    FreshCapture fresh;
    bool originalAllocation=false,captureAllocation=false;
    final::FinalAdmissionScope scope(beforeIdentity);
    out.native=scope.invoke([&]() -> uint8_t {
        try{return std::forward<Original>(original)();}
        catch(const std::bad_alloc&){originalAllocation=true;throw;}
    },[&](const final::Facts& facts) {
        try{fresh=std::forward<Capture>(capture)(facts);return fresh.identity;}
        catch(const std::bad_alloc&){captureAllocation=true;throw;}
    },[&](const final::Facts& facts) {
        if(out.provisional.reason!=authority::Reason::ProvisionalClaimed||!out.claim) {
            out.finalized=owner.retain(beforeRequest.ticket);out.finalized.reason=out.provisional.reason;return false;
        }
        const authority::NativeDecision native{facts.nativeReturn!=0,facts.nativeCompositeCompleted(),
            authority::NativeDomain::FinalAdmission};
        out.finalized=owner.finalizeForInvocation(fresh.frame,fresh.request,native,fresh.owned,permit);
        return out.finalized.mayPass;
    });
    if(!out.native.facts.decisionAttempted) {
        out.finalized=owner.retain(beforeRequest.ticket);
        if(originalAllocation||captureAllocation)out.finalized.reason=authority::Reason::AllocationFailure;
        else if(out.native.reason==final::Reason::NativeRefusal)out.finalized.reason=authority::Reason::NativeRefusal;
        else if(out.native.reason==final::Reason::OriginalException)out.finalized.reason=authority::Reason::OriginalException;
        else if(out.native.reason==final::Reason::IdentityChanged||out.native.reason==final::Reason::RouteChanged)
            out.finalized.reason=authority::Reason::IdentityChanged;
        else out.finalized.reason=authority::Reason::CaptureException;
    }
    return out;
}
};
template<class Original,class Capture>
Output invoke(authority::NativeAdmissionAuthority& owner,const authority::Frame& beforeFrame,
              const authority::AdmissionRequest& beforeRequest,const final::Identity& beforeIdentity,
              const authority::OwnedAdmissionWindow& beforeOwned,Original&& original,Capture&& capture) noexcept {
    return InvocationAccess::invoke(owner,beforeFrame,beforeRequest,beforeIdentity,beforeOwned,
        std::forward<Original>(original),std::forward<Capture>(capture));
}
} // namespace nimby::windows::two_phase_admission
