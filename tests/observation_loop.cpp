#include <nimby/observation_loop.hpp>
#include <atomic>
#include <iostream>

#define CHECK(x) do { if (!(x)) throw std::runtime_error("line " + std::to_string(__LINE__) + ": " #x); } while(false)
using namespace std::chrono_literals;

int main() { try {
    // Exact virtual-clock boundaries distinguish pacing from queued catch-up.
    // These checks do not depend on Windows timer resolution or machine load.
    using Clock=std::chrono::steady_clock;
    const auto origin=Clock::time_point{};
    const auto schedule=[&](auto requested,auto started,auto finished) {
        return nimby::detail::observationSchedule(origin+requested,origin+started,origin+finished,20ms);
    };
    CHECK(schedule(0ms,0ms,18ms).next==origin+20ms);
    CHECK(schedule(0ms,0ms,18ms).missed==0);
    CHECK(schedule(0ms,0ms,20ms).next==origin+20ms);
    CHECK(schedule(0ms,0ms,20ms).missed==0);
    CHECK(schedule(0ms,0ms,23ms).next==origin+23ms);
    CHECK(schedule(0ms,0ms,23ms).missed==1);
    CHECK(schedule(0ms,0ms,40ms).next==origin+40ms);
    CHECK(schedule(0ms,0ms,40ms).missed==1);
    CHECK(schedule(0ms,0ms,105ms).next==origin+105ms);
    CHECK(schedule(0ms,0ms,105ms).missed==5);
    // A late wake never shortens the period measured from the actual start.
    CHECK(schedule(20ms,27ms,32ms).next==origin+47ms);
    CHECK(schedule(20ms,27ms,32ms).missed==0);
    CHECK(schedule(20ms,27ms,45ms).next==origin+47ms);
    CHECK(schedule(20ms,27ms,45ms).missed==1);
    // Failure: 3 ms callback + 23 ms cleanup; the second poll starts at 26,
    // then a short callback still waits until 46, never until the old grid's 40.
    const auto cleanedDeadline=schedule(0ms,0ms,26ms);
    CHECK(cleanedDeadline.next==origin+26ms&&cleanedDeadline.missed==1);
    CHECK(schedule(26ms,26ms,27ms).next==origin+46ms);
    CHECK(schedule(105ms,105ms,106ms).next==origin+125ms);
    CHECK(schedule(0ms,0ms,20ms+1ns).missed==1);
    CHECK(schedule(0ms,0ms,40ms+1ns).missed==2);
    const auto action=[&](auto periodic,auto previous,auto notified){
        return nimby::detail::observationActionDeadline(origin+periodic,origin+previous,origin+notified);
    };
    CHECK(action(250ms,0ms,1ms)==origin+20ms);
    CHECK(action(250ms,0ms,20ms)==origin+20ms);
    CHECK(action(250ms,0ms,150ms)==origin+150ms);
    CHECK(action(250ms,0ms,300ms)==origin+250ms); // Periodic deadline never postponed.
    CHECK(action(10ms,0ms,1ms)==origin+10ms); // Existing shorter cadence is unchanged.

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
    CHECK(loop.status().lastStartIntervalUs==0&&loop.status().maxStartIntervalUs==0);
    CHECK(loop.status().lastWakeDelayUs>=0&&loop.status().maxWakeDelayUs>=loop.status().lastWakeDelayUs);
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
    CHECK(loop.status().lastStartIntervalUs>=10000);
    CHECK(loop.status().maxStartIntervalUs>=loop.status().lastStartIntervalUs);
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
    CHECK(loop.status().lastStartIntervalUs==0&&loop.status().maxStartIntervalUs==0);
    bool rejected=false;
    try { loop.start([] {}, {}, 0ms); } catch(const std::invalid_argument&) { rejected=true; }
    CHECK(rejected);
    // A poll slower than its period records missed deadlines and skips catchup.
    std::atomic<int> slowCalls=0;
    loop.start([&]{++slowCalls;std::this_thread::sleep_for(35ms);}, {}, 10ms);
    const auto deadline=std::chrono::steady_clock::now()+2s;
    while(loop.status().successes<2&&std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(1ms);
    loop.stop();
    const auto slow=loop.status();
    CHECK(slow.successes>=2&&slow.maxPollUs>=30000&&slow.lastPollUs>=30000&&slow.missedDeadlines>=6);
    CHECK(slowCalls==static_cast<int>(slow.successes));

    // Failure recovery consumes the same worker budget as capture/evaluation.
    // Freeze the following callback so the failed cycle's measurements cannot
    // be replaced before they are examined.
    entered=false;release=false;calls=0;losses=0;
    loop.start([&] {
        if(++calls==1)throw std::runtime_error("publication temporarily unavailable");
        std::unique_lock guard(mutex);entered=true;event.notify_all();
        event.wait(guard,[&]{return release;});
    },[&]{if(++losses==1)std::this_thread::sleep_for(35ms);},10ms);
    {
        std::unique_lock guard(mutex);
        if(!event.wait_for(guard,2s,[&]{return entered;})){
            release=true;event.notify_all();throw std::runtime_error("cycle timing poll did not recover");
        }
    }
    const auto failed=loop.status();
    const auto lossesBeforeStop=losses.load();
    {std::lock_guard guard(mutex);release=true;}event.notify_all();
    loop.stop();
    CHECK(failed.failures==1&&failed.successes==0&&!failed.available&&lossesBeforeStop==1);
    CHECK(failed.lastCleanupUs>=30000&&failed.lastCycleUs>=failed.lastPollUs+failed.lastCleanupUs);
    CHECK(failed.maxCycleUs==failed.lastCycleUs&&failed.maxCleanupUs==failed.lastCleanupUs&&failed.missedDeadlines>=3);
    const auto cleaned=loop.status();
    CHECK(cleaned.successes>=1&&cleaned.lastCleanupUs==0&&cleaned.maxCleanupUs>=30000);
    CHECK(cleaned.maxCycleUs>=failed.lastCycleUs&&cleaned.lastCycleUs>=cleaned.lastPollUs);
    CHECK(cleaned.maxStartIntervalUs>=failed.lastCycleUs);
    CHECK(cleaned.maxStartIntervalUs>=cleaned.lastStartIntervalUs&&cleaned.maxWakeDelayUs>=0);
    std::cout << "PASS loss/recovery, serial callbacks, in-flight stop, restart and interruptible wait\n";
} catch(const std::exception& error) { std::cerr << error.what() << '\n'; return 1; } }
