#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <limits>

namespace nimby::windows::phase_ownership {
// Pure, fixed-storage validation of one hash-qualified native call ancestry.
// Context belongs to ONE thread (the hooks declare it thread_local). No native
// reads, locks, callbacks, allocations or permissions are performed here.
// Controller entry alone owns no lock. Only the three exact descendant callsites
// identify the studied Shared(controller+4C0) window. This is diagnostic scope,
// not a stable tick, complete consist, retained movement or rear-clear certificate.
enum class Reason : uint32_t {
    QualifiedWindow, UnqualifiedProfile, InvalidAddress, ReadIncomplete,
    MissingAncestor, Reentrant, ScopeOrder, ControllerChanged, SimulationChanged,
    NetworkChanged, ResourceChanged, DescriptorChanged, ElapsedChanged,
    ForeignCallsite, ExceptionUnwind, ScopeEnded, NotTrainPhase, SequenceExhausted,
    Count
};
inline const char* reasonName(Reason reason) noexcept {
    switch(reason) {
    case Reason::QualifiedWindow:return "qualified_diagnostic_ancestry";
    case Reason::UnqualifiedProfile:return "unqualified_binary_profile";
    case Reason::InvalidAddress:return "invalid_context_address";
    case Reason::ReadIncomplete:return "incomplete_context_read";
    case Reason::MissingAncestor:return "missing_native_ancestor";
    case Reason::Reentrant:return "reentrant_native_ancestry";
    case Reason::ScopeOrder:return "crossed_native_scope";
    case Reason::ControllerChanged:return "controller_identity_changed";
    case Reason::SimulationChanged:return "simulation_identity_changed";
    case Reason::NetworkChanged:return "network_identity_changed";
    case Reason::ResourceChanged:return "shared_resource_changed";
    case Reason::DescriptorChanged:return "step_descriptor_changed";
    case Reason::ElapsedChanged:return "elapsed_argument_changed";
    case Reason::ForeignCallsite:return "foreign_native_callsite";
    case Reason::ExceptionUnwind:return "native_exception_unwind";
    case Reason::ScopeEnded:return "native_scope_ended";
    case Reason::NotTrainPhase:return "not_train_phase_scope";
    case Reason::SequenceExhausted:return "diagnostic_sequence_exhausted";
    default:return "unknown";
    }
}
struct Profile {uintptr_t imageBase{};bool qualified{};};
struct ResourceRef {
    uintptr_t value{},control{};
    bool operator==(const ResourceRef&) const = default;
};
struct ControllerFrame {
    uintptr_t controllerReference{},controller{};
    bool readComplete{};
};
struct AdvanceFrame {
    uintptr_t controllerReference{},controller{};
    uintptr_t controllerSimulation{},controllerNetwork{}; // bounded readback
    uintptr_t simulation{},network{},resourceReference{},descriptor{},returnAddress{};
    ResourceRef resource; // copy both values at resourceReference +0/+8
    ResourceRef controllerResource; // controller +0x220/+0x228
    uint64_t ordinal{};
    int64_t elapsed{}; // descriptor +0x10; no conversion to an unsigned duration
    bool readComplete{};
};
struct SimulationFrame {
    uintptr_t simulation{},network{},resourceReference{},returnAddress{};
    ResourceRef resource; // 478000 makes a NEW ref address with the same pair
    int64_t elapsed{};
    bool readComplete{};
};
struct TrainPhaseFrame {
    uintptr_t trainState{},network{},simulation{},returnAddress{};
    // Current, bounded reads of the ancestors, both copied resource refs, and
    // descriptor. Re-read these before AND after collecting diagnostic samples.
    uintptr_t controllerReference{},controller{},controllerSimulation{},controllerNetwork{};
    ResourceRef controllerResource,advanceResource,simulationResource;
    uint64_t ordinal{};
    int64_t requestedElapsed{};
    int64_t elapsed{};
    bool readComplete{};
};
struct Token {
    uintptr_t observerContext{}; // diagnostic owner identity, never dereferenced
    uintptr_t controllerReference{},controller{},simulation{},network{},descriptor{};
    uintptr_t advanceResourceReference{},simulationResourceReference{};
    uintptr_t advanceReturn{},simulationReturn{},trainPhaseReturn{}; // observed relative sites
    ResourceRef resource;
    uint64_t ordinal{},sequence{};
    int64_t requestedElapsed{},phaseElapsed{};
    bool operator==(const Token&) const = default;
};
struct Window {
    Reason reason=Reason::MissingAncestor;
    Token token;
    bool qualified() const noexcept {return reason==Reason::QualifiedWindow;}
    static constexpr bool completeCoverageCertified=false;
};
namespace detail {
inline constexpr uintptr_t addressLimit=0x7fffffff0000ULL;
inline bool address(uintptr_t value,size_t extent) noexcept {
    return value>=0x10000&&value%8==0&&extent<addressLimit&&value<addressLimit-extent;
}
inline bool resource(const ResourceRef& value) noexcept {
    return (!value.value||address(value.value,8))&&(!value.control||address(value.control,16));
}
enum class Level : uint8_t {Controller,Advance,Simulation,TrainPhase};
}
class Scope;
class Context {
    friend class Scope;
    struct Entry {Token token;Reason reason=Reason::QualifiedWindow;bool live{};};
    Profile profile_;
    std::array<Entry,4> entries_{};
    size_t depth_{},rejected_{};
    uint64_t sequence_{};
    // Failure is sticky through the active Advance. Once only the long-lived
    // Controller remains, a NEW exact Advance can be observed. Nested Controller
    // invocations also poison that Controller until its complete unwind.
    void invalidate(Reason reason,bool controller=false) noexcept {
        for(size_t i=controller?0:1;i<depth_;++i)
            if(entries_[i].reason==Reason::QualifiedWindow)entries_[i].reason=reason;
    }
    Window current() const noexcept {
        if(depth_!=4)return {Reason::MissingAncestor,{}};
        for(size_t i=0;i<depth_;++i) {
            if(!entries_[i].live)return {Reason::ScopeOrder,{}};
            if(entries_[i].reason!=Reason::QualifiedWindow)return {entries_[i].reason,{}};
        }
        if(rejected_)return {Reason::Reentrant,{}};
        return {Reason::QualifiedWindow,entries_[3].token};
    }
public:
    explicit Context(Profile profile) noexcept:profile_(profile){}
    Context(const Context&)=delete;
    Context& operator=(const Context&)=delete;
    bool empty() const noexcept {return depth_==0&&rejected_==0;}
    // Refresh a qualified module profile only between complete ancestries.
    // Keep the sequence: reconstructing this TLS object for every Controller
    // could otherwise recycle a token with the same addresses and ordinal.
    bool profileWhenEmpty(Profile profile) noexcept {
        if(!empty())return false;
        profile_=profile;return true;
    }
    // Copied addresses/values for the hook's bounded readback, NOT a qualified
    // window or permission. The native ancestor scopes must still be alive.
    Token expected() const noexcept {return depth_?entries_[depth_-1].token:Token{};}
    // A copied token is usable as a diagnostic identifier only. This check
    // refuses it once its originating frame ends; it grants no native rights.
    bool contains(const Token& token) const noexcept {
        const auto window=current();return window.qualified()&&window.token==token;
    }
};
class Scope {
    Context& context_;
    detail::Level level_;
    size_t index_{};
    uint64_t sequence_{};
    int exceptions_=std::uncaught_exceptions();
    Reason reason_=Reason::QualifiedWindow;
    bool entered_{},rejected_{};
    void reject(Reason reason) noexcept {
        reason_=reason;context_.invalidate(reason,level_==detail::Level::Controller);
        ++context_.rejected_;rejected_=true;
    }
    bool prepare() noexcept {
        if(!context_.profile_.qualified||
           !detail::address(context_.profile_.imageBase,0xb819a0)) {
            reject(Reason::UnqualifiedProfile);return false;
        }
        const auto expected=static_cast<size_t>(level_);
        if(context_.rejected_||context_.depth_>expected) {
            reject(Reason::Reentrant);return false;
        }
        if(context_.depth_<expected) {reject(Reason::MissingAncestor);return false;}
        for(size_t i=0;i<context_.depth_;++i) {
            if(!context_.entries_[i].live) {reject(Reason::ScopeOrder);return false;}
            if(context_.entries_[i].reason!=Reason::QualifiedWindow) {
                reject(context_.entries_[i].reason);return false;
            }
        }
        return true;
    }
    void enter(Token token) noexcept {
        if(context_.sequence_==std::numeric_limits<uint64_t>::max()) {
            reject(Reason::SequenceExhausted);return;
        }
        sequence_=++context_.sequence_;token.sequence=sequence_;
        token.observerContext=reinterpret_cast<uintptr_t>(&context_);
        index_=context_.depth_++;
        context_.entries_[index_]={token,Reason::QualifiedWindow,true};entered_=true;
    }
    Reason trainFrameReason(const TrainPhaseFrame& frame) const noexcept {
        if(!frame.readComplete)return Reason::ReadIncomplete;
        if(!detail::address(frame.simulation,0x2520)||!detail::address(frame.network,0x15a0)||
           !detail::address(frame.trainState,0x30))return Reason::InvalidAddress;
        const auto& parent=context_.entries_[2].token;
        if(frame.controllerReference!=parent.controllerReference||frame.controller!=parent.controller)
            return Reason::ControllerChanged;
        if(frame.controllerSimulation!=parent.simulation)return Reason::SimulationChanged;
        if(frame.controllerNetwork!=parent.network)return Reason::NetworkChanged;
        if(frame.controllerResource!=parent.resource||frame.advanceResource!=parent.resource||
           frame.simulationResource!=parent.resource)
            return Reason::ResourceChanged;
        if(frame.ordinal!=parent.ordinal||frame.requestedElapsed!=parent.requestedElapsed)
            return Reason::DescriptorChanged;
        if(frame.simulation!=parent.simulation||frame.trainState!=frame.simulation+0xa0)
            return Reason::SimulationChanged;
        if(frame.network!=parent.network)return Reason::NetworkChanged;
        if(frame.returnAddress!=context_.profile_.imageBase+0x477f05)return Reason::ForeignCallsite;
        const auto clamped=parent.requestedElapsed>166?int64_t{166}:parent.requestedElapsed;
        if(frame.elapsed!=clamped)return Reason::ElapsedChanged;
        return Reason::QualifiedWindow;
    }
public:
    Scope(Context& context,const ControllerFrame& frame) noexcept
        :context_(context),level_(detail::Level::Controller) {
        if(!prepare())return;
        if(!frame.readComplete){reject(Reason::ReadIncomplete);return;}
        if(!detail::address(frame.controllerReference,8)||!detail::address(frame.controller,0x4c8)) {
            reject(Reason::InvalidAddress);return;
        }
        Token token;token.controllerReference=frame.controllerReference;token.controller=frame.controller;
        enter(token);
    }
    Scope(Context& context,const AdvanceFrame& frame) noexcept
        :context_(context),level_(detail::Level::Advance) {
        if(!prepare())return;
        if(!frame.readComplete){reject(Reason::ReadIncomplete);return;}
        if(!detail::address(frame.controllerReference,8)||!detail::address(frame.controller,0x4c8)||
           !detail::address(frame.simulation,0x2520)||!detail::address(frame.network,0x15a0)||
           !detail::address(frame.resourceReference,16)||!detail::address(frame.descriptor,24)||
           !detail::resource(frame.resource)){reject(Reason::InvalidAddress);return;}
        auto token=context_.entries_[0].token;
        if(frame.controllerReference!=token.controllerReference||frame.controller!=token.controller) {
            reject(Reason::ControllerChanged);return;
        }
        if(frame.simulation!=frame.controllerSimulation){reject(Reason::SimulationChanged);return;}
        if(frame.network!=frame.controllerNetwork){reject(Reason::NetworkChanged);return;}
        if(frame.resource!=frame.controllerResource){reject(Reason::ResourceChanged);return;}
        const auto relative=frame.returnAddress-context_.profile_.imageBase;
        if(relative!=0x3495fe&&relative!=0x349b64){reject(Reason::ForeignCallsite);return;}
        if(frame.elapsed<1){reject(Reason::ElapsedChanged);return;}
        token.simulation=frame.simulation;token.network=frame.network;token.resource=frame.resource;
        token.descriptor=frame.descriptor;token.ordinal=frame.ordinal;token.requestedElapsed=frame.elapsed;
        token.advanceResourceReference=frame.resourceReference;
        token.advanceReturn=relative;
        enter(token);
    }
    Scope(Context& context,const SimulationFrame& frame) noexcept
        :context_(context),level_(detail::Level::Simulation) {
        if(!prepare())return;
        if(!frame.readComplete){reject(Reason::ReadIncomplete);return;}
        if(!detail::address(frame.simulation,0x2520)||!detail::address(frame.network,0x15a0)||
           !detail::address(frame.resourceReference,16)||!detail::resource(frame.resource)) {
            reject(Reason::InvalidAddress);return;
        }
        auto token=context_.entries_[1].token;
        if(frame.simulation!=token.simulation){reject(Reason::SimulationChanged);return;}
        if(frame.network!=token.network){reject(Reason::NetworkChanged);return;}
        if(frame.resource!=token.resource){reject(Reason::ResourceChanged);return;}
        if(frame.elapsed!=token.requestedElapsed){reject(Reason::ElapsedChanged);return;}
        if(frame.returnAddress!=context_.profile_.imageBase+0x4784f9){reject(Reason::ForeignCallsite);return;}
        token.simulationResourceReference=frame.resourceReference;
        token.simulationReturn=frame.returnAddress-context_.profile_.imageBase;
        enter(token);
    }
    Scope(Context& context,const TrainPhaseFrame& frame) noexcept
        :context_(context),level_(detail::Level::TrainPhase) {
        if(!prepare())return;
        const auto reason=trainFrameReason(frame);
        if(reason!=Reason::QualifiedWindow){reject(reason);return;}
        auto token=context_.entries_[2].token;token.phaseElapsed=frame.elapsed;
        token.trainPhaseReturn=frame.returnAddress-context_.profile_.imageBase;enter(token);
    }
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
    Scope(Scope&&)=delete;
    Scope& operator=(Scope&&)=delete;
    ~Scope() noexcept {
        if(std::uncaught_exceptions()>exceptions_)context_.invalidate(Reason::ExceptionUnwind);
        if(rejected_){--context_.rejected_;return;}
        if(!entered_)return;
        if(context_.depth_!=index_+1||context_.entries_[index_].token.sequence!=sequence_)
            context_.invalidate(Reason::ScopeOrder);
        context_.entries_[index_].live=false;
        while(context_.depth_&&!context_.entries_[context_.depth_-1].live)--context_.depth_;
    }
    Reason reason() const noexcept {return reason_;}
    // The hook calls this ONLY after its one original 44D6B0 invocation returns,
    // with bounded current context reads. An exception exits via RAII instead.
    // Native SEH that skips C++ destruction is NOT recovered by this helper.
    Window afterReturn(const TrainPhaseFrame& readback) noexcept {
        if(rejected_)return {reason_,{}};
        if(level_!=detail::Level::TrainPhase)return {Reason::NotTrainPhase,{}};
        if(!entered_||context_.depth_!=index_+1||!context_.entries_[index_].live||
           context_.entries_[index_].token.sequence!=sequence_)return {Reason::ScopeEnded,{}};
        auto window=context_.current();if(!window.qualified())return window;
        const auto reason=trainFrameReason(readback);
        if(reason!=Reason::QualifiedWindow){context_.invalidate(reason);return {reason,{}};}
        return window;
    }
};
}
