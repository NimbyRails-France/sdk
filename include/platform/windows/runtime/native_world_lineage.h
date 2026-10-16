#pragma once
#include <cstdint>
#include <limits>

namespace nimby::windows::native_world {

// Pure bookkeeping for observed hash-pinned lifecycle hooks. This proves only
// a controller/runner lineage. It does NOT certify an immutable tick, exhaustive
// consist, writer exclusion, historical continuity, permission or rear release.
// The bridge must serialize every access, including the process-lifetime Clock.
// No token may be synthesized from native pointer/time/header equality.
class Clock {
    uint64_t next_=1;
public:
    explicit constexpr Clock(uint64_t first=1) noexcept:next_(first){}
    Clock(const Clock&)=delete;
    Clock& operator=(const Clock&)=delete;
    uint64_t take() noexcept {
        if(!next_)return 0;
        const auto result=next_;
        next_=result==std::numeric_limits<uint64_t>::max()?0:result+1;
        return result;
    }
};
enum class State : uint8_t { Unarmed, Stopping, Stopped, Starting, Running, Capturing, Lost };
enum class Reason : uint8_t {
    Ready, Unarmed, InvalidIdentity, UnexpectedTransition, WrongRunner,
    UnqualifiedScope, EmptyWorld, WorldChanged, SequenceExhausted, Lost
};
enum class Boundary : uint8_t { PostTrainShared, PostSharedExclusive };
struct Attempt {
    uint64_t nonce{};
    uintptr_t controller{},context{};
    bool operator==(const Attempt&) const = default;
    explicit operator bool() const noexcept {return nonce!=0;}
};
struct Scope {
    uintptr_t controller{},context{},sharedLock{},database{},simulation{};
    uint64_t thread{};
    // Relative return sites, verified by the bridge's owned TLS ancestry.
    uintptr_t advanceReturn{},trainLoopReturn{},trainPhaseReturn{};
    Boundary boundary=Boundary::PostTrainShared;
    uintptr_t releaseReturn{};
    // Provenance from the wrapper's real calls, not repeated memory reads.
    bool originalReleaseCompleted=false,exclusiveTrySucceeded=false;
    // Required only when adopting an already running native owner. The wrapper
    // has installed ALL lifecycle hooks and verified the current Root/Controller
    // relation under this actual exclusive lock, and read ctx+18's runner ID.
    bool lifecycleHooksActive=false,currentRootVerified=false;
    uint64_t nativeRunnerThread{};
};
struct Capture {
    uint64_t world{},phase{},continuity{};
    uintptr_t controller{},context{},database{},simulation{};
    uint64_t thread{};
    Boundary boundary=Boundary::PostTrainShared;
    bool operator==(const Capture&) const = default;
    explicit operator bool() const noexcept {return world&&phase&&continuity;}
};
struct Result {Reason reason=Reason::Unarmed;Capture capture{};};
struct MergeScope {
    uintptr_t controller{},context{},sharedLock{},database{},simulation{};
    uint64_t thread{};
    uintptr_t mergeReturn{};
};

class Lineage {
    State state_=State::Unarmed;
    Attempt attempt_{};
    uintptr_t controller_{},context_{},database_{},simulation_{};
    uint64_t world_{},continuity_{},thread_{};
    bool startReturned_=false;
    Capture capture_{};
    static bool identity(uintptr_t controller,uintptr_t context) noexcept {
        return controller>=0x10000&&controller<=std::numeric_limits<uintptr_t>::max()-0x5d0&&
            controller%8==0&&context==controller+0x5d0;
    }
    Result failed(Reason reason) noexcept {return {reason,{}};}
public:
    Lineage()=default;
    Lineage(const Lineage&)=delete;
    Lineage& operator=(const Lineage&)=delete;
    State state() const noexcept {return state_;}
    // Diagnostic label may remain after loss; it is never a readiness predicate.
    uint64_t world() const noexcept {return world_;}
    void lose() noexcept {
        state_=State::Lost;attempt_={};capture_={};thread_=0;
        startReturned_=false;database_=0;simulation_=0;
    }
    // Called BEFORE 348960's original. Even a still-running old callback loses
    // its token immediately. Recovery requires an observed successful stop,
    // then a new start/run; equal bytes never recover a lost generation.
    Attempt beginStop(Clock& clock,uintptr_t controller,uintptr_t context) noexcept {
        if(!identity(controller,context)||(controller_&&controller!=controller_)||
            state_==State::Stopping||state_==State::Starting) {
            lose();return {};
        }
        const auto nonce=clock.take();
        if(!nonce){lose();return {};}
        capture_={};database_=0;simulation_=0;startReturned_=false;
        controller_=controller;context_=context;attempt_={nonce,controller,context};
        state_=State::Stopping;return attempt_;
    }
    bool finishStop(Attempt attempt) noexcept {
        if(state_!=State::Stopping||!attempt||attempt!=attempt_){lose();return false;}
        thread_=0;attempt_={};state_=State::Stopped;return true;
    }
    // Called BEFORE 3488C0 launches its original. The new runner can enter on
    // another thread before finishStart; capture additionally needs normal return.
    Attempt beginStart(Clock& clock,uintptr_t controller,uintptr_t context) noexcept {
        if(state_!=State::Stopped||controller!=controller_||context!=context_){lose();return {};}
        const auto nonce=clock.take();
        if(!nonce){lose();return {};}
        world_=nonce;continuity_=nonce;thread_=0;database_=0;simulation_=0;
        capture_={};startReturned_=false;attempt_={nonce,controller,context};
        state_=State::Starting;return attempt_;
    }
    bool finishStart(Attempt attempt) noexcept {
        if(state_!=State::Starting||!attempt||attempt!=attempt_){lose();return false;}
        startReturned_=true;attempt_={};state_=State::Running;return true;
    }
    bool runnerEnter(uintptr_t controller,uintptr_t context,uint64_t thread) noexcept {
        if((state_!=State::Starting&&state_!=State::Running)||!thread||thread_||
            controller!=controller_||context!=context_){lose();return false;}
        thread_=thread;return true;
    }
    bool runnerExit(uint64_t thread) noexcept {
        if(!thread||thread!=thread_){lose();return false;}
        thread_=0;capture_={};
        // The normal old-runner exit belongs to the already invalidated stop.
        if(state_!=State::Stopping)lose();
        return true;
    }
    // A freshly attached bridge may have missed the long-lived runner's entry.
    // This creates an entirely NEW lineage at an actually owned native boundary;
    // it recovers neither earlier movement nor leases. Lost/previously armed
    // instances cannot use adoption as an equality-based recovery mechanism.
    Result adoptOwnedWorld(Clock& clock,const Scope& scope) noexcept {
        if(state_==State::Lost)return failed(Reason::Lost);
        if(state_!=State::Unarmed||world_||controller_||context_||thread_||attempt_||capture_) {
            lose();return failed(Reason::UnexpectedTransition);
        }
        if(!identity(scope.controller,scope.context)) {
            lose();return failed(Reason::InvalidIdentity);
        }
        if(scope.boundary!=Boundary::PostSharedExclusive||scope.sharedLock!=scope.controller+0x4c0||
            scope.releaseReturn!=0x349fa3||!scope.originalReleaseCompleted||!scope.exclusiveTrySucceeded||
            scope.advanceReturn||scope.trainLoopReturn||scope.trainPhaseReturn||
            !scope.lifecycleHooksActive||!scope.currentRootVerified||!scope.thread||
            scope.nativeRunnerThread!=scope.thread) {
            lose();return failed(Reason::UnqualifiedScope);
        }
        if(!scope.database||!scope.simulation)return failed(Reason::EmptyWorld);
        const auto nonce=clock.take();
        if(!nonce){lose();return failed(Reason::SequenceExhausted);}
        controller_=scope.controller;context_=scope.context;thread_=scope.thread;
        world_=nonce;continuity_=nonce;startReturned_=true;state_=State::Running;
        return capturePhase(clock,scope);
    }
    Result capturePhase(Clock& clock,const Scope& scope) noexcept {
        if(state_==State::Lost)return failed(Reason::Lost);
        if(state_==State::Unarmed)return failed(Reason::Unarmed);
        if(state_!=State::Running||!startReturned_){lose();return failed(Reason::UnexpectedTransition);}
        if(scope.controller!=controller_||scope.context!=context_||!thread_||scope.thread!=thread_) {
            lose();return failed(Reason::WrongRunner);
        }
        const bool trainBoundary=scope.boundary==Boundary::PostTrainShared&&
            (scope.advanceReturn==0x3495fe||scope.advanceReturn==0x349b64)&&
            scope.trainLoopReturn==0x4784f9&&scope.trainPhaseReturn==0x477f05&&
            !scope.originalReleaseCompleted&&!scope.exclusiveTrySucceeded&&!scope.releaseReturn;
        // The 349F9E call has COMPLETED its original shared release; the same
        // owned runner then acquired exclusive with a nonblocking try. This is
        // a new boundary after the entire shared block, including possible
        // merges. Do not reuse a 44D6B0 token or its phase designation.
        const bool exclusiveBoundary=scope.boundary==Boundary::PostSharedExclusive&&
            scope.releaseReturn==0x349fa3&&scope.originalReleaseCompleted&&scope.exclusiveTrySucceeded&&
            !scope.advanceReturn&&!scope.trainLoopReturn&&!scope.trainPhaseReturn;
        if(scope.sharedLock!=controller_+0x4c0||(!trainBoundary&&!exclusiveBoundary)) {
            lose();return failed(Reason::UnqualifiedScope);
        }
        // Empty runner created by 346CB0/348770 cannot prove world readiness.
        if(!scope.database||!scope.simulation)return failed(Reason::EmptyWorld);
        if(database_&&(scope.database!=database_||scope.simulation!=simulation_)) {
            lose();return failed(Reason::WorldChanged);
        }
        const auto phase=clock.take();
        if(!phase){lose();return failed(Reason::SequenceExhausted);}
        database_=scope.database;simulation_=scope.simulation;
        capture_={world_,phase,continuity_,controller_,context_,database_,simulation_,thread_,scope.boundary};
        state_=State::Capturing;return {Reason::Ready,capture_};
    }
    bool current(const Capture& token) const noexcept {
        return state_==State::Capturing&&token&&token==capture_;
    }
    bool endCapture(const Capture& token) noexcept {
        // Stop can run on the loading thread while the old owner is copying.
        // It already revoked this capture before joining that owner. Do not
        // overwrite the in-progress stop with a late stale endCapture.
        if(state_==State::Stopping||state_==State::Stopped||state_==State::Starting)return false;
        if(!current(token)){lose();return false;}
        capture_={};state_=State::Running;return true;
    }
    // Called BEFORE live 478B20 merge. This invalidates continuity, not the
    // world's lifetime. A new phase copies everything again; it does not thereby
    // qualify reversal/warp/replan history. Never splice pre/post merge records.
    bool invalidateMerge(Clock& clock,uintptr_t simulation) noexcept {
        if(state_!=State::Running||!simulation_||simulation!=simulation_){lose();return false;}
        const auto nonce=clock.take();if(!nonce){lose();return false;}
        continuity_=nonce;capture_={};return true;
    }
    // The first network merge can precede the first post-shared capture. Its
    // exact owned Shared callsite binds the world identity without claiming a
    // finished snapshot. Caller verifies readback under that native scope.
    bool invalidateMerge(Clock& clock,const MergeScope& scope) noexcept {
        if(state_!=State::Running||!startReturned_||!thread_||scope.thread!=thread_||
            scope.controller!=controller_||scope.context!=context_||
            scope.sharedLock!=controller_+0x4c0||scope.mergeReturn!=0x349871||
            !scope.database||!scope.simulation||
            (database_&&(scope.database!=database_||scope.simulation!=simulation_))) {
            lose();return false;
        }
        const auto nonce=clock.take();if(!nonce){lose();return false;}
        database_=scope.database;simulation_=scope.simulation;
        continuity_=nonce;capture_={};return true;
    }
};
}
