#pragma once
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/platform/observation_wait.hpp>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <array>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace nimby {

namespace detail {
struct ObservationLoopAccess;
struct ObservationSchedule {
    using Clock = std::chrono::steady_clock;
    Clock::time_point next;
    std::uint64_t missed;
};

// Count deadlines strictly exceeded since the requested start, including a
// delayed wake and cleanup. Finishing exactly at a boundary is not late.
// Execution is separately anchored on the actual start: no phase debt and no
// catch-up burst when a callback takes slightly longer than its interval.
inline ObservationSchedule observationSchedule(ObservationSchedule::Clock::time_point requested,
        ObservationSchedule::Clock::time_point started, ObservationSchedule::Clock::time_point finished,
        std::chrono::milliseconds interval) noexcept {
    using Clock = ObservationSchedule::Clock;
    const auto elapsed=finished-requested;
    const auto missed=elapsed>Clock::duration::zero()
        ? static_cast<std::uint64_t>((elapsed-Clock::duration{1})/interval) : 0;
    const auto earliest=started+interval;
    return {earliest>finished?earliest:finished,missed};
}
inline ObservationSchedule::Clock::time_point observationActionDeadline(
        ObservationSchedule::Clock::time_point periodic,ObservationSchedule::Clock::time_point previous,
        ObservationSchedule::Clock::time_point notified) noexcept {
    return std::min(periodic,std::max(notified,previous+std::chrono::milliseconds{20}));
}
}

struct ObservationLoopStatus {
    bool running = false;
    bool available = false;
    std::uint64_t successes = 0;
    std::uint64_t failures = 0;
    std::uint64_t consecutiveFailures = 0;
    std::int64_t lastPollMs = 0;
    std::int64_t lastPollUs = 0, maxPollUs = 0;
    std::uint64_t missedDeadlines = 0;
    // Last failure retained after recovery, reset on start. Bounded, owned text:
    // recording an allocation failure must not itself allocate on the worker.
    std::array<char,256> lastError{};
    // lastPollUs retains callback-only timing. A failed cycle also suspends UI
    // and restores outputs; include that work when assessing the real cadence.
    std::int64_t lastCleanupUs = 0, maxCleanupUs = 0;
    std::int64_t lastCycleUs = 0, maxCycleUs = 0;
    // Wake delay is measured against the requested deadline. Start intervals
    // include the preceding callback, cleanup and wait; the first is zero.
    std::int64_t lastWakeDelayUs = 0, maxWakeDelayUs = 0;
    std::int64_t lastStartIntervalUs = 0, maxStartIntervalUs = 0;
    bool highResolutionWait = false;
    bool actionWait = false;
    std::uint64_t actionCycles = 0;
};

// Serial, interruptible polling owned by the SDK. A failed poll invalidates the
// last observation; it is never replayed as a fresh sample. Callbacks must not
// call start()/stop() on this loop. stop() waits for an in-flight poll to finish.
class ObservationLoop {
    friend struct detail::ObservationLoopAccess;
public:
    using Duration = std::chrono::milliseconds;
    ObservationLoop() = default;
    ObservationLoop(const ObservationLoop&) = delete;
    ObservationLoop& operator=(const ObservationLoop&) = delete;
    ~ObservationLoop() { stop(); }

    void start(std::function<void()> poll, std::function<void()> lost, Duration interval) {
        startInternal(std::move(poll),std::move(lost),interval,false);
    }

private:
    void startInternal(std::function<void()> poll, std::function<void()> lost, Duration interval,bool actions) {
        if (!poll || interval < Duration{10} || interval > std::chrono::hours{1})
            throw std::invalid_argument("Invalid observation loop");
        std::lock_guard lifecycle(lifecycleMutex_);
        std::lock_guard lock(mutex_);
        if (worker_.joinable()) throw std::logic_error("Observation loop already started");
        status_ = {};
        stopping_ = false;
        nativeWait_.prepare(actions);
        status_.highResolutionWait=nativeWait_.highResolution();
        status_.actionWait=nativeWait_.available()&&nativeWait_.hasActions();
        try {
            worker_ = std::thread([this, poll=std::move(poll), lost=std::move(lost), interval] {
                run(poll, lost, interval);
            });
        } catch(...) {nativeWait_.release();status_.highResolutionWait=status_.actionWait=false;throw;}
        status_.running = true;
    }

public:
    void stop() noexcept {
        std::lock_guard lifecycle(lifecycleMutex_);
        {
            std::lock_guard lock(mutex_);
            stopping_ = true;
        }
        nativeWait_.interrupt();
        wake_.notify_all();
        if (worker_.joinable()) worker_.join();
        nativeWait_.release();
        std::lock_guard lock(mutex_);
        status_.running = false;
        status_.available = false;
        status_.highResolutionWait = false;
        status_.actionWait = false;
    }

    ObservationLoopStatus status() const {
        std::lock_guard lock(mutex_);
        return status_;
    }

private:
    static void invalidate(const std::function<void()>& lost,bool report=true) noexcept {
        // Losing observations must not kill the process if restoration also fails.
        try { if (lost) lost(); } catch (...) { if(report)detail::diagnostics::exception("mods", "observation invalidation"); }
    }

    void run(const std::function<void()>& poll, const std::function<void()>& lost, Duration interval) noexcept {
        using Clock = std::chrono::steady_clock;
        auto next = Clock::now();
        auto nextSummary = next;
        auto nextFailure=next,nextRecovery=next;
        auto previousStarted=next;
        bool havePreviousStart=false;
        bool pendingAction=false;
        auto actionRequested=next;
        detail::diagnostics::write("mods","INFO","Observation worker started");
        std::unique_lock lock(mutex_);
        while (!stopping_) {
            // Notifications only advance this serial worker. Preserve one
            // pending hint during a callback or a burst, with at most one
            // anticipatory start per 20 ms. Idle cadence remains unchanged.
            const auto requested=[&]{
                if(!pendingAction||!havePreviousStart)return next;
                return detail::observationActionDeadline(next,previousStarted,actionRequested);
            };
            // Do not send an already-due deadline through the OS timed wait:
            // its timer conversion can add delay even though no wait is needed.
            while(!stopping_&&requested()>Clock::now()) {
                if(nativeWait_.available()) {
                    lock.unlock();
                    const auto result=nativeWait_.waitUntil(requested(),!pendingAction);
                    lock.lock();
                    // The manual stop event is signaled after stopping_ under
                    // this mutex, so stop cannot be lost between arm and wait.
                    // An unexpected signal also falls back instead of spinning.
                    if(result==detail::platform::ObservationWait::Result::Interrupted&&!stopping_)
                        nativeWait_.disable();
                    if(result==detail::platform::ObservationWait::Result::Action){
                        pendingAction=true;actionRequested=Clock::now();
                    }
                    status_.highResolutionWait=nativeWait_.highResolution();
                    status_.actionWait=nativeWait_.available()&&nativeWait_.hasActions();
                }else wake_.wait_until(lock,requested(),[this]{return stopping_;});
            }
            if(stopping_)break;
            const auto deadline=requested();
            // Consume preexisting hints before capture, never after draining
            // the queue: an action arriving during the callback must survive.
            const bool actionCycle=nativeWait_.consumeActions()||pendingAction;
            pendingAction=false;
            lock.unlock();
            bool success = false;
            const auto started=Clock::now();
            const auto wakeDelay=started>deadline
                ? std::chrono::duration_cast<std::chrono::microseconds>(started-deadline).count() : 0;
            const auto startInterval=havePreviousStart
                ? std::chrono::duration_cast<std::chrono::microseconds>(started-previousStarted).count() : 0;
            previousStarted=started;havePreviousStart=true;
            const bool reportFailure=started>=nextFailure;
            std::array<char,256> error{};
            const auto copyError=[&](const char* message) noexcept {
                if(!message)return;
                for(size_t i=0;i+1<error.size()&&message[i];++i)error[i]=message[i];
            };
            try { poll(); success = true; }
            catch (const std::exception& failure) { if(reportFailure)detail::diagnostics::write("mods", "ERROR", failure.what()); copyError(failure.what()); }
            catch (...) { if(reportFailure)detail::diagnostics::write("mods", "ERROR", "Unknown observation exception"); copyError("Unknown observation exception"); }
            const auto pollFinished=Clock::now();
            const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(pollFinished-started).count();
            if(!success){
                if(reportFailure)nextFailure=Clock::now()+std::chrono::seconds{30};
                invalidate(lost,reportFailure);
            }
            const auto cycleFinished=Clock::now();
            const auto cleanupElapsed=success?0:std::chrono::duration_cast<std::chrono::microseconds>(cycleFinished-pollFinished).count();
            const auto cycleElapsed=std::chrono::duration_cast<std::chrono::microseconds>(cycleFinished-started).count();
            lock.lock();
            const bool recovered=success&&!status_.available;
            status_.available = success;
            status_.lastPollMs=elapsed/1000;status_.lastPollUs=elapsed;
            if(elapsed>status_.maxPollUs)status_.maxPollUs=elapsed;
            status_.lastCleanupUs=cleanupElapsed;
            if(cleanupElapsed>status_.maxCleanupUs)status_.maxCleanupUs=cleanupElapsed;
            status_.lastCycleUs=cycleElapsed;
            if(cycleElapsed>status_.maxCycleUs)status_.maxCycleUs=cycleElapsed;
            status_.lastWakeDelayUs=wakeDelay;
            if(wakeDelay>status_.maxWakeDelayUs)status_.maxWakeDelayUs=wakeDelay;
            status_.lastStartIntervalUs=startInterval;
            if(actionCycle)++status_.actionCycles;
            if(startInterval>status_.maxStartIntervalUs)status_.maxStartIntervalUs=startInterval;
            if (success) {++status_.successes;status_.consecutiveFailures=0;}
            else {++status_.failures;++status_.consecutiveFailures;status_.lastError=error;}
            // One fresh poll as soon as its period or its predecessor finishes.
            // In particular, a 23 ms cycle with a 20 ms interval need not wait
            // for a 40 ms grid slot. No work is queued for missed deadlines.
            const auto now = Clock::now();
            const auto schedule=detail::observationSchedule(deadline,started,now,interval);
            status_.missedDeadlines+=schedule.missed;
            next=schedule.next;
            // Log transitions and a bounded heartbeat, never every simulation tick.
            // Copy counters under lock, then release it before file I/O.
            if((recovered&&now>=nextRecovery) || now>=nextSummary) {
                const auto snapshot=status_;
                nextSummary=now+std::chrono::seconds{30};
                nextRecovery=now+std::chrono::seconds{1};
                lock.unlock();
                char summary[960]{};
                std::snprintf(summary,sizeof(summary),"Observation %s: successes=%llu failures=%llu consecutiveFailures=%llu lastPollMs=%lld intervalMs=%lld lastPollUs=%lld maxPollUs=%lld missedDeadlines=%llu lastCleanupUs=%lld maxCleanupUs=%lld lastCycleUs=%lld maxCycleUs=%lld lastWakeDelayUs=%lld maxWakeDelayUs=%lld lastStartIntervalUs=%lld maxStartIntervalUs=%lld waitMode=%s actionCycles=%llu",
                    recovered?"available/recovered":"heartbeat",
                    static_cast<unsigned long long>(snapshot.successes),static_cast<unsigned long long>(snapshot.failures),
                    static_cast<unsigned long long>(snapshot.consecutiveFailures),static_cast<long long>(snapshot.lastPollMs),
                    static_cast<long long>(interval.count()),static_cast<long long>(snapshot.lastPollUs),
                    static_cast<long long>(snapshot.maxPollUs),static_cast<unsigned long long>(snapshot.missedDeadlines),
                    static_cast<long long>(snapshot.lastCleanupUs),static_cast<long long>(snapshot.maxCleanupUs),
                    static_cast<long long>(snapshot.lastCycleUs),static_cast<long long>(snapshot.maxCycleUs),
                    static_cast<long long>(snapshot.lastWakeDelayUs),static_cast<long long>(snapshot.maxWakeDelayUs),
                    static_cast<long long>(snapshot.lastStartIntervalUs),static_cast<long long>(snapshot.maxStartIntervalUs),
                    snapshot.highResolutionWait?"high-resolution":snapshot.actionWait?"events-timeout":"condition-variable",
                    static_cast<unsigned long long>(snapshot.actionCycles));
                detail::diagnostics::write("mods","INFO",summary);
                lock.lock();
            }
        }
        lock.unlock();
        invalidate(lost);
        detail::diagnostics::write("mods","INFO","Observation worker stopped");
    }

    mutable std::mutex mutex_;
    std::mutex lifecycleMutex_;
    std::condition_variable wake_;
    detail::platform::ObservationWait nativeWait_;
    ObservationLoopStatus status_;
    bool stopping_ = false;
    std::thread worker_;
};

namespace detail {
// The public polling API remains unchanged. Only the mod entry's session-only
// tool loop opts into host action handles; auxiliary loops cannot consume them.
struct ObservationLoopAccess {
    static void start(ObservationLoop& loop,std::function<void()> poll,std::function<void()> lost,
                      ObservationLoop::Duration interval,bool actions) {
        loop.startInternal(std::move(poll),std::move(lost),interval,actions);
    }
};
}

} // namespace nimby
