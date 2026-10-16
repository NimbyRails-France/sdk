#pragma once
#include <engine/route_resources.h>
#include <array>
#include <atomic>
#include <memory>

namespace nimby::engine::interlocking {

// This is observation of the first native admission stage, NOT its final
// permission after script gates, and NOT a request to alter native booking.
enum class NativeStage { Unknown, Clear, Blocked };
enum class StageComparison { Unavailable, SameStage, ShadowRefusedNativeClear, ShadowGrantedNativeBlocked };
enum class ShadowSubmit { Queued, Busy, Full, Invalid };
struct ShadowRequest {
    Ticket ticket;
    uint64_t capturedAtMs{};
    bool routeQualified=false;
    Proof entered=Proof::Unknown;
    NativeStage nativeStage=NativeStage::Unknown;
    ResourceOutcome resources=ResourceOutcome::MissingTable;
};
struct ShadowEvidence {
    Ticket ticket;
    Evidence evidence;
    NativeStage nativeStage=NativeStage::Unknown;
};
struct ShadowSample {
    Ticket ticket;
    Result result;
    NativeStage nativeStage=NativeStage::Unknown;
    StageComparison comparison=StageComparison::Unavailable;
    ResourceOutcome resources=ResourceOutcome::MissingTable;
    bool evidence=false;
    Proof entered=Proof::Unknown,rearClear=Proof::Unknown;
    bool operator==(const ShadowSample& other)const noexcept {
        return ticket==other.ticket&&result.outcome==other.result.outcome&&result.blocker==other.result.blocker&&
            result.track==other.result.track&&nativeStage==other.nativeStage&&comparison==other.comparison&&
            resources==other.resources&&evidence==other.evidence&&entered==other.entered&&rearClear==other.rearClear;
    }
};
inline constexpr size_t shadowOutcomeCount=static_cast<size_t>(Outcome::Released)+1;
inline constexpr size_t shadowComparisonCount=static_cast<size_t>(StageComparison::ShadowGrantedNativeBlocked)+1;
struct ShadowReport {
    uint64_t session{},atMs{},processed{},busy{},full{},invalid{};
    size_t leases{},resources{},queued{};
    std::array<uint64_t,shadowOutcomeCount> outcomes{};
    std::array<uint64_t,shadowComparisonCount> comparisons{};
    std::array<ShadowSample,4> samples{};
    size_t sampleCount{};
};
struct ShadowDrain {
    size_t processed{},queued{};
    std::optional<ShadowReport> report;
};
struct ShadowQueueStats { uint64_t accepted{},busy{},full{},invalid{};size_t queued{}; };

// A bounded, multi-producer mailbox feeding ONE observation-worker owner.
// Producers only try a lock and copy already-qualified, owned numeric facts.
// They never allocate, wait, decode, call the game, or compute an interlocking
// decision. Queue loss retains every existing shadow lease. Engine allocation
// and resource construction happen outside the mailbox lock on the owner.
//
// beginSession/drain are single-owner operations, never concurrent with each
// other. Construction/destruction happen at bootstrap/retirement with producers
// stopped. This class intentionally exposes NO native movement permission.
class ShadowCoordinator {
public:
    static constexpr size_t queueCapacity=64,maximumResources=512;
private:
    struct Event {
        ShadowRequest request;
        Evidence evidence;
        bool isEvidence=false;
        size_t resourceCount{};
        std::array<Interval,maximumResources> resources{};
    };
    // Allocate fixed backing once at bootstrap, not on a native thread/stack.
    std::unique_ptr<std::array<Event,queueCapacity>> queue_;
    std::atomic_flag queueLock_=ATOMIC_FLAG_INIT;
    size_t first_{},size_{};
    std::atomic<size_t> queued_{};
    std::atomic<uint64_t> accepted_{},busy_{},full_{},invalid_{};
    RouteInterlocking engine_;
    ShadowReport window_;
    std::optional<ShadowSample> previous_;
    uint64_t emittedAt_{},reportedBusy_{},reportedFull_{},reportedInvalid_{};
    bool emitted_=false,pending_=false;
    friend struct ShadowCoordinatorTestAccess;

    static bool validTicket(const Ticket& ticket)noexcept {
        return ticket.session&&ticket.routeRevision&&ticket.train>>48==5&&ticket.source>>48==8;
    }
    static bool validProof(Proof proof)noexcept {
        return proof==Proof::Unknown||proof==Proof::No||proof==Proof::Yes;
    }
    static bool validStage(NativeStage stage)noexcept {
        return stage==NativeStage::Unknown||stage==NativeStage::Clear||stage==NativeStage::Blocked;
    }
    ShadowSubmit invalid()noexcept {invalid_.fetch_add(1,std::memory_order_relaxed);return ShadowSubmit::Invalid;}
    template<class Fill> ShadowSubmit push(Fill&& fill)noexcept {
        if(queueLock_.test_and_set(std::memory_order_acquire)){
            busy_.fetch_add(1,std::memory_order_relaxed);return ShadowSubmit::Busy;
        }
        if(size_==queueCapacity){
            queueLock_.clear(std::memory_order_release);
            full_.fetch_add(1,std::memory_order_relaxed);return ShadowSubmit::Full;
        }
        fill((*queue_)[(first_+size_)%queueCapacity]);
        ++size_;queued_.store(size_,std::memory_order_release);
        queueLock_.clear(std::memory_order_release);
        accepted_.fetch_add(1,std::memory_order_relaxed);return ShadowSubmit::Queued;
    }
    bool pop(Event& out)noexcept {
        if(queueLock_.test_and_set(std::memory_order_acquire))return false;
        if(!size_){queueLock_.clear(std::memory_order_release);return false;}
        const auto& event=(*queue_)[first_];
        out.request=event.request;out.evidence=event.evidence;out.isEvidence=event.isEvidence;
        out.resourceCount=event.resourceCount;
        std::copy_n(event.resources.begin(),event.resourceCount,out.resources.begin());
        first_=(first_+1)%queueCapacity;--size_;queued_.store(size_,std::memory_order_release);
        queueLock_.clear(std::memory_order_release);return true;
    }
    static StageComparison comparison(const Result& result,NativeStage stage,bool evidence)noexcept {
        if(evidence||stage==NativeStage::Unknown)return StageComparison::Unavailable;
        if(result.outcome==Outcome::Granted)
            return stage==NativeStage::Clear?StageComparison::SameStage:StageComparison::ShadowGrantedNativeBlocked;
        if(result.outcome==Outcome::Conflict)
            return stage==NativeStage::Blocked?StageComparison::SameStage:StageComparison::ShadowRefusedNativeClear;
        return StageComparison::Unavailable;
    }
    static unsigned diagnosticPriority(const ShadowSample& sample)noexcept {
        if(sample.result.outcome==Outcome::Conflict||sample.comparison==StageComparison::ShadowRefusedNativeClear||
           sample.comparison==StageComparison::ShadowGrantedNativeBlocked)return 3;
        return sample.result.outcome==Outcome::Granted||sample.result.outcome==Outcome::Updated?1:2;
    }
    void note(const ShadowSample& sample)noexcept {
        ++window_.processed;
        ++window_.outcomes[static_cast<size_t>(sample.result.outcome)];
        ++window_.comparisons[static_cast<size_t>(sample.comparison)];
        if(!previous_||*previous_!=sample){
            pending_=true;previous_=sample;
            if(std::find(window_.samples.begin(),window_.samples.begin()+window_.sampleCount,sample)!=
               window_.samples.begin()+window_.sampleCount)return;
            if(window_.sampleCount<window_.samples.size())window_.samples[window_.sampleCount++]=sample;
            else {
                // A burst of ordinary grants must not hide a later refusal's
                // train/source/blocker. Still retain at most four owned rows.
                const auto lowest=std::min_element(window_.samples.begin(),window_.samples.end(),
                    [](const auto& a,const auto& b){return diagnosticPriority(a)<diagnosticPriority(b);});
                if(diagnosticPriority(sample)>diagnosticPriority(*lowest))*lowest=sample;
            }
        }
    }
public:
    explicit ShadowCoordinator(Limits limits={}):queue_(std::make_unique<std::array<Event,queueCapacity>>()),engine_(limits) {}
    ShadowCoordinator(const ShadowCoordinator&)=delete;
    ShadowCoordinator& operator=(const ShadowCoordinator&)=delete;

    ShadowSubmit trySubmit(const ShadowRequest& request,std::span<const Interval> resources)noexcept {
        if(!validTicket(request.ticket)||!validProof(request.entered)||!validStage(request.nativeStage)||
           static_cast<size_t>(request.resources)>static_cast<size_t>(ResourceOutcome::Capacity)||resources.size()>maximumResources)
            return invalid();
        // Malformed intervals are handled by the worker's core. A failed or
        // incomplete qualification still reaches it to close this owner's
        // eligibility, retaining protection if entry is known or unknown.
        return push([&](Event& event){
            event.request=request;event.isEvidence=false;event.evidence={};
            event.resourceCount=resources.size();std::copy(resources.begin(),resources.end(),event.resources.begin());
        });
    }
    ShadowSubmit trySubmit(const ShadowEvidence& evidence)noexcept {
        if(!validTicket(evidence.ticket)||!validStage(evidence.nativeStage)||
           !validProof(evidence.evidence.entered)||!validProof(evidence.evidence.rearClear)||
           (evidence.evidence.currentRouteRevision&&!*evidence.evidence.currentRouteRevision))return invalid();
        return push([&](Event& event){
            event.request={evidence.ticket,evidence.evidence.capturedAtMs,false,evidence.evidence.entered,evidence.nativeStage,ResourceOutcome::MissingTable};
            event.evidence=evidence.evidence;event.isEvidence=true;event.resourceCount=0;
        });
    }
    ShadowQueueStats queueStats()const noexcept {
        return {accepted_.load(std::memory_order_relaxed),busy_.load(std::memory_order_relaxed),
            full_.load(std::memory_order_relaxed),invalid_.load(std::memory_order_relaxed),queued_.load(std::memory_order_acquire)};
    }
    // OWNER ONLY. The bridge must positively establish a new world lineage.
    // A missing train, source, route or partial snapshot never calls this.
    bool beginSession(uint64_t session)noexcept {
        if(!session)return false;
        if(engine_.session()==session)return true;
        if(!engine_.beginSession(session))return false;
        window_={};previous_.reset();emitted_=false;pending_=true;return true;
    }
    // OWNER ONLY. Bounded work; format/log the returned report AFTER this call.
    // No engine lock spans allocation, native decoding or a diagnostic sink.
    ShadowDrain drain(Frame frame,size_t maximumEvents=queueCapacity) {
        ShadowDrain drained;Event event;
        for(size_t count=0;count<std::min(maximumEvents,queueCapacity)&&pop(event);++count){
            const auto& input=event.request;
            const auto result=event.isEvidence?engine_.observe(input.ticket,event.evidence,frame):
                engine_.acquire(Request{input.ticket,std::span(event.resources.data(),event.resourceCount),input.capturedAtMs,
                    input.routeQualified&&input.resources==ResourceOutcome::Complete,input.entered},frame);
            note({input.ticket,result,input.nativeStage,comparison(result,input.nativeStage,event.isEvidence),input.resources,
                event.isEvidence,input.entered,event.isEvidence?event.evidence.rearClear:Proof::Unknown});
            ++drained.processed;
        }
        drained.queued=queued_.load(std::memory_order_acquire);
        const auto stats=queueStats();
        pending_|=stats.busy!=reportedBusy_||stats.full!=reportedFull_||stats.invalid!=reportedInvalid_;
        // An incoherent worker clock never resets the throttle to create a
        // burst. The core independently rejects stale frames and evidence.
        if(emitted_&&(frame.nowMs<emittedAt_||
           (frame.nowMs-emittedAt_<30000&&(!pending_||frame.nowMs-emittedAt_<1000))))return drained;
        window_.session=engine_.session();window_.atMs=frame.nowMs;
        window_.leases=engine_.leaseCount();window_.resources=engine_.resourceCount();window_.queued=drained.queued;
        window_.busy=stats.busy-reportedBusy_;window_.full=stats.full-reportedFull_;window_.invalid=stats.invalid-reportedInvalid_;
        drained.report=window_;window_={};emittedAt_=frame.nowMs;emitted_=true;pending_=false;
        reportedBusy_=stats.busy;reportedFull_=stats.full;reportedInvalid_=stats.invalid;
        return drained;
    }
    // OWNER ONLY; for qualification fixtures and worker diagnostics. These
    // functions must never be used to change a game's admission result.
    bool holds(const Ticket& ticket)const noexcept{return engine_.holds(ticket);}
    size_t leaseCount()const noexcept{return engine_.leaseCount();}
    size_t resourceCount()const noexcept{return engine_.resourceCount();}
};
} // namespace nimby::engine::interlocking
