#include "platform/windows/hooks/backend.h"
#include <atomic>
#include <bit>
#include <chrono>
#include <cstdio>
#include <thread>
#include <vector>

extern "C" int __cdecl nimby_test_target(int);
using Target = decltype(&nimby_test_target);
static Target original{};
static std::atomic<unsigned> observations{};
static int __cdecl observe(int value) {
    observations.fetch_add(1, std::memory_order_relaxed);
    return original(value); // Preserve arguments and return value.
}
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(false)
struct CallingThreads {
    std::atomic<bool> stop{}, failed{};
    std::atomic<unsigned long long> calls{};
    std::vector<std::thread> workers;
    CallingThreads() {
        for (int t = 0; t < 16; ++t) workers.emplace_back([&, t] {
            while (!stop.load(std::memory_order_acquire)) {
                if (nimby_test_target(t) != (t + 3) * 2) failed.store(true, std::memory_order_relaxed);
                calls.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    ~CallingThreads() {
        stop.store(true, std::memory_order_release);
        for (auto& worker : workers) worker.join();
    }
    bool progress() const {
        const auto target = calls.load(std::memory_order_relaxed) + 1000;
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (calls.load(std::memory_order_relaxed) < target) {
            if (std::chrono::steady_clock::now() >= deadline) return false;
            std::this_thread::yield();
        }
        return !failed.load(std::memory_order_relaxed);
    }
};
int main() {
    nimby::hooks::Backend backend;
    for (int cycle=0; cycle<3; ++cycle) {
        CHECK(backend.initialize()==MH_OK);
        CHECK(backend.initialize()==MH_ERROR_ALREADY_INITIALIZED);
        void* target = std::bit_cast<void*>(&nimby_test_target);
        void* trampoline{};
        CHECK(MH_CreateHook(nullptr, std::bit_cast<void*>(&observe), &trampoline)==MH_ERROR_NOT_EXECUTABLE);
        CHECK(MH_CreateHook(target, std::bit_cast<void*>(&observe), &trampoline)==MH_OK);
        original=std::bit_cast<Target>(trampoline);
        observations=0;
        CHECK(nimby_test_target(7)==20 && observations==0);
        CHECK(MH_EnableHook(target)==MH_OK);
        CHECK(nimby_test_target(7)==20 && observations==1);
        std::atomic<bool> failed{};
        std::vector<std::thread> workers;
        for(int t=0;t<4;++t) workers.emplace_back([&] {
            for(int i=0;i<1000;++i) if(nimby_test_target(i)!=(i+3)*2) failed=true;
        });
        for(auto& worker:workers) worker.join(); // Drain before disable/remove.
        CHECK(!failed && observations==4001);
        CHECK(MH_DisableHook(target)==MH_OK);
        CHECK(nimby_test_target(7)==20 && observations==4001);
        CHECK(MH_RemoveHook(target)==MH_OK);
        CHECK(backend.shutdown()==MH_OK);
        CHECK(backend.shutdown()==MH_OK);
    }
    CHECK(backend.initialize() == MH_OK);
    void* target = std::bit_cast<void*>(&nimby_test_target);
    void* trampoline{};
    CHECK(MH_CreateHook(target, std::bit_cast<void*>(&observe), &trampoline) == MH_OK);
    original = std::bit_cast<Target>(trampoline);
    {
        CallingThreads callers;
        CHECK(callers.progress());
        for (int cycle = 0; cycle < 8; ++cycle) {
            const auto before = observations.load(std::memory_order_relaxed);
            CHECK(MH_QueueEnableHook(target) == MH_OK);
            CHECK(MH_ApplyQueued() == MH_OK);
            CHECK(callers.progress());
            CHECK(observations.load(std::memory_order_relaxed) > before);
            CHECK(MH_QueueDisableHook(target) == MH_OK);
            CHECK(MH_ApplyQueued() == MH_OK);
            CHECK(callers.progress());
        }
    } // Drain every caller before freeing the trampoline.
    const auto afterDrain = observations.load(std::memory_order_relaxed);
    CHECK(nimby_test_target(7) == 20 && observations == afterDrain);
    CHECK(MH_RemoveHook(target) == MH_OK);
    CHECK(backend.shutdown() == MH_OK);
    std::puts("PASS 16 concurrent callers preserve results during 8 queued enable/disable cycles, drained before remove");
    std::puts("PASS MinHook create/enable/observe/drain/disable/remove, original result preserved (3 cycles)");
}
