#include <nimby/observation_loop.hpp>
#include <atomic>
#include <iostream>

#define CHECK(x) do { if (!(x)) throw std::runtime_error("line " + std::to_string(__LINE__) + ": " #x); } while(false)
using namespace std::chrono_literals;

int main() { try {
    nimby::ObservationLoop loop;
    std::mutex mutex;
    std::condition_variable event;
    std::atomic<int> calls=0, losses=0;
    bool release=false, entered=false;
    loop.start([&] {
        const auto call = ++calls;
        if (call == 1) throw std::runtime_error("missing simulation");
        std::unique_lock lock(mutex);
        entered = true;
        event.notify_all();
        event.wait(lock,[&] { return release; });
    }, [&] { ++losses; }, 10ms);
    {
        std::unique_lock lock(mutex);
        const bool ready = event.wait_for(lock,2s,[&] { return entered; });
        if (!ready) { release=true; event.notify_all(); throw std::runtime_error("poll did not recover"); }
    }
    CHECK(loop.status().running && loop.status().failures == 1 && losses == 1);
    CHECK(!loop.status().available); // Previous failure is not replayed as fresh data.
    CHECK(loop.status().consecutiveFailures==1);
    CHECK(std::string(loop.status().lastError.data())=="missing simulation");
    std::atomic<bool> stopped=false;
    std::thread stopper([&] { loop.stop(); stopped=true; });
    // stop must wait for the in-flight handler, then invalidate it exactly once.
    {
        std::lock_guard lock(mutex);
        CHECK(!stopped);
        release = true;
    }
    event.notify_all();
    stopper.join();
    CHECK(stopped && !loop.status().running && !loop.status().available);
    CHECK(loop.status().successes >= 1 && losses == 2);
    CHECK(loop.status().consecutiveFailures==0&&loop.status().lastPollMs>=0);
    CHECK(std::string(loop.status().lastError.data())=="missing simulation");
    loop.stop();
    CHECK(losses == 2); // Repeated stop is idempotent.

    entered = false;
    loop.start([&] {
        std::lock_guard lock(mutex);
        entered=true;
        event.notify_all();
    }, [&] { ++losses; throw std::runtime_error("restoration unavailable"); }, 1h);
    {
        std::unique_lock lock(mutex);
        CHECK(event.wait_for(lock,2s,[&] { return entered; }));
    }
    const auto start = std::chrono::steady_clock::now();
    loop.stop(); // Interrupts a one-hour wait; restoration errors cannot escape.
    CHECK(std::chrono::steady_clock::now()-start < 1s && losses == 3);
    CHECK(loop.status().failures == 0 && loop.status().successes == 1);
    CHECK(loop.status().lastError[0]==0);
    bool rejected=false;
    try { loop.start([] {}, {}, 0ms); } catch(const std::invalid_argument&) { rejected=true; }
    CHECK(rejected);
    std::cout << "PASS loss/recovery, serial callbacks, in-flight stop, restart and interruptible wait\n";
} catch(const std::exception& error) { std::cerr << error.what() << '\n'; return 1; } }
