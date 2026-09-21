#pragma once
#include <chrono>
#include <array>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <thread>

namespace nimby {

struct ObservationLoopStatus {
    bool running = false;
    bool available = false;
    std::uint64_t successes = 0;
    std::uint64_t failures = 0;
    std::uint64_t consecutiveFailures = 0;
    std::int64_t lastPollMs = 0;
    // Last failure retained after recovery, reset on start. Bounded, owned text:
    // recording an allocation failure must not itself allocate on the worker.
    std::array<char,256> lastError{};
};

// Serial, interruptible polling owned by the SDK. A failed poll invalidates the
// last observation; it is never replayed as a fresh sample. Callbacks must not
// call start()/stop() on this loop. stop() waits for an in-flight poll to finish.
class ObservationLoop {
public:
    using Duration = std::chrono::milliseconds;
    ObservationLoop() = default;
    ObservationLoop(const ObservationLoop&) = delete;
    ObservationLoop& operator=(const ObservationLoop&) = delete;
    ~ObservationLoop() { stop(); }

    void start(std::function<void()> poll, std::function<void()> lost, Duration interval) {
        if (!poll || interval < Duration{10} || interval > std::chrono::hours{1})
            throw std::invalid_argument("Invalid observation loop");
        std::lock_guard lifecycle(lifecycleMutex_);
        std::lock_guard lock(mutex_);
        if (worker_.joinable()) throw std::logic_error("Observation loop already started");
        status_ = {};
        stopping_ = false;
        worker_ = std::thread([this, poll=std::move(poll), lost=std::move(lost), interval] {
            run(poll, lost, interval);
        });
        status_.running = true;
    }

    void stop() noexcept {
        std::lock_guard lifecycle(lifecycleMutex_);
        {
            std::lock_guard lock(mutex_);
            stopping_ = true;
        }
        wake_.notify_all();
        if (worker_.joinable()) worker_.join();
        std::lock_guard lock(mutex_);
        status_.running = false;
        status_.available = false;
    }

    ObservationLoopStatus status() const {
        std::lock_guard lock(mutex_);
        return status_;
    }

private:
    static void invalidate(const std::function<void()>& lost) noexcept {
        // Losing observations must not kill the process if restoration also fails.
        try { if (lost) lost(); } catch (...) {}
    }

    void run(const std::function<void()>& poll, const std::function<void()>& lost, Duration interval) noexcept {
        using Clock = std::chrono::steady_clock;
        auto next = Clock::now();
        std::unique_lock lock(mutex_);
        while (!stopping_) {
            if (wake_.wait_until(lock, next, [this] { return stopping_; })) break;
            lock.unlock();
            bool success = false;
            const auto started=Clock::now();
            std::array<char,256> error{};
            const auto copyError=[&](const char* message) noexcept {
                if(!message)return;
                for(size_t i=0;i+1<error.size()&&message[i];++i)error[i]=message[i];
            };
            try { poll(); success = true; }
            catch (const std::exception& failure) { copyError(failure.what()); }
            catch (...) { copyError("Unknown observation exception"); }
            const auto elapsed=std::chrono::duration_cast<Duration>(Clock::now()-started).count();
            if(!success)invalidate(lost);
            lock.lock();
            status_.available = success;
            status_.lastPollMs=elapsed;
            if (success) {++status_.successes;status_.consecutiveFailures=0;}
            else {++status_.failures;++status_.consecutiveFailures;status_.lastError=error;}
            // Skip missed deadlines rather than flooding a slow simulation.
            next += interval;
            const auto now = Clock::now();
            if (next <= now) next += interval * ((now-next)/interval + 1);
        }
        lock.unlock();
        invalidate(lost);
    }

    mutable std::mutex mutex_;
    std::mutex lifecycleMutex_;
    std::condition_variable wake_;
    ObservationLoopStatus status_;
    bool stopping_ = false;
    std::thread worker_;
};

} // namespace nimby
