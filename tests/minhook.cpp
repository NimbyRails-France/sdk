#include "hooks/backend.h"
#include <atomic>
#include <bit>
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
    std::puts("PASS MinHook create/enable/observe/drain/disable/remove, original result preserved (3 cycles)");
}
