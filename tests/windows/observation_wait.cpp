#include <nimby/detail/platform/windows/system.hpp>
#include <atomic>
#include <array>
#include <bit>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cwchar>
#include <utility>

namespace {
enum Failure { None,CreateTimer,ArmTimer,WaitTimer };
std::atomic<Failure> failure=None;
std::atomic<HANDLE> enteredWait{};
std::array<HANDLE,2> actionEvents{};
uint32_t __cdecl actionWaits(uint64_t* remote,uint64_t* local) noexcept {
    *remote=*local=0;
    if(!actionEvents[0]||!actionEvents[1])return 5;
    HANDLE first{},second{};
    if(!DuplicateHandle(GetCurrentProcess(),actionEvents[0],GetCurrentProcess(),&first,SYNCHRONIZE,FALSE,0))return 2;
    if(!DuplicateHandle(GetCurrentProcess(),actionEvents[1],GetCurrentProcess(),&second,SYNCHRONIZE,FALSE,0)){
        CloseHandle(first);return 2;
    }
    *remote=reinterpret_cast<uintptr_t>(first);*local=reinterpret_cast<uintptr_t>(second);return 0;
}
HMODULE testModule(LPCWSTR name){
    return name&&std::wcscmp(name,L"NimbyRailsFranceSDK.dll")==0?GetModuleHandleW(nullptr):GetModuleHandleW(name);
}
FARPROC testSymbol(HMODULE module,LPCSTR name){
    return std::strcmp(name,"NimbyInternal_ModHostActionWaits")==0
        ? std::bit_cast<FARPROC>(&actionWaits):GetProcAddress(module,name);
}
HANDLE testCreateTimer(LPSECURITY_ATTRIBUTES security,LPCWSTR name,DWORD flags,DWORD access) {
    if(failure==CreateTimer){SetLastError(ERROR_NOT_SUPPORTED);return nullptr;}
    return CreateWaitableTimerExW(security,name,flags,access);
}
BOOL testArmTimer(HANDLE timer,const LARGE_INTEGER* due,LONG period,PTIMERAPCROUTINE callback,
                  LPVOID argument,PREASON_CONTEXT context,ULONG delay) {
    if(failure==ArmTimer){SetLastError(ERROR_INVALID_HANDLE);return FALSE;}
    return SetWaitableTimerEx(timer,due,period,callback,argument,context,delay);
}
DWORD testWait(DWORD count,const HANDLE* handles,BOOL all,DWORD timeout) {
    if(const auto event=enteredWait.load())SetEvent(event);
    if(failure==WaitTimer){SetLastError(ERROR_INVALID_HANDLE);return WAIT_FAILED;}
    return WaitForMultipleObjects(count,handles,all,timeout);
}
}
// The real loop and backend receive failures at their actual OS boundary.
// No test branch, environment override or fault injection is shipped in the SDK.
#define CreateWaitableTimerExW testCreateTimer
#define SetWaitableTimerEx testArmTimer
#define WaitForMultipleObjects testWait
#define GetModuleHandleW testModule
#define GetProcAddress testSymbol
#include <nimby/observation_loop.hpp>
#undef GetProcAddress
#undef GetModuleHandleW
#undef CreateWaitableTimerExW
#undef SetWaitableTimerEx
#undef WaitForMultipleObjects

#define CHECK(x) do { if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x); } while(false)
using namespace std::chrono_literals;
using Clock=std::chrono::steady_clock;

struct FakeApi {
    using Clock=::Clock;
    static inline Clock::time_point time{};
    static inline bool stopFails=false,timerFails=false,armFails=false,actions=false,pending=false;
    static inline DWORD result=WAIT_OBJECT_0+1;
    static inline int timers=0,arms=0,waits=0,signals=0,closed=0;
    static inline std::int64_t ticks=0;
    static inline DWORD lastCount{},lastTimeout{};
    static Clock::time_point now(){return time;}
    static HANDLE createStop(){return stopFails?nullptr:reinterpret_cast<HANDLE>(1);}
    static HANDLE createTimer(){++timers;return timerFails?nullptr:reinterpret_cast<HANDLE>(2);}
    static bool arm(HANDLE,std::int64_t value){++arms;ticks=value;return !armFails;}
    static std::array<HANDLE,2> copyActions(){return actions?std::array<HANDLE,2>{reinterpret_cast<HANDLE>(3),reinterpret_cast<HANDLE>(4)}:std::array<HANDLE,2>{};}
    static bool consume(HANDLE){return std::exchange(pending,false);}
    static DWORD wait(const HANDLE*,DWORD count,DWORD timeout){++waits;lastCount=count;lastTimeout=timeout;return result;}
    static void signal(HANDLE){++signals;}
    static void close(HANDLE){++closed;}
    static void reset(){time={};stopFails=timerFails=armFails=actions=pending=false;result=WAIT_OBJECT_0+1;
        timers=arms=waits=signals=closed=0;ticks=0;lastCount=lastTimeout=0;}
};

int main(){try {
    using FakeWait=nimby::detail::platform::BasicObservationWait<FakeApi>;
    using Result=FakeWait::Result;
    FakeApi::reset();FakeWait fake;
    fake.prepare();CHECK(fake.available());
    CHECK(fake.waitUntil(FakeApi::time)==Result::Elapsed&&FakeApi::arms==0&&FakeApi::waits==0);
    CHECK(fake.waitUntil(FakeApi::time+99ns)==Result::Elapsed&&FakeApi::ticks==-1);
    CHECK(fake.waitUntil(FakeApi::time+100ns)==Result::Elapsed&&FakeApi::ticks==-1);
    CHECK(fake.waitUntil(FakeApi::time+101ns)==Result::Elapsed&&FakeApi::ticks==-2);
    CHECK(fake.waitUntil(FakeApi::time+1h)==Result::Elapsed&&FakeApi::ticks==-36000000000LL);
    FakeApi::result=WAIT_OBJECT_0;fake.interrupt();
    CHECK(fake.waitUntil(FakeApi::time+1h)==Result::Interrupted&&FakeApi::signals==1);
    FakeApi::armFails=true;const auto waits=FakeApi::waits;
    CHECK(fake.waitUntil(FakeApi::time+1s)==Result::Unavailable&&!fake.available());
    CHECK(FakeApi::waits==waits);
    const auto arms=FakeApi::arms;
    CHECK(fake.waitUntil(FakeApi::time+1s)==Result::Unavailable&&FakeApi::arms==arms);
    // Handles stay valid for a concurrent stop signal even after failure.
    fake.interrupt();CHECK(FakeApi::signals==2&&FakeApi::closed==0);
    fake.release();CHECK(FakeApi::closed==2);fake.release();CHECK(FakeApi::closed==2);
    FakeApi::reset();FakeApi::stopFails=true;fake.prepare();
    CHECK(!fake.available()&&FakeApi::timers==0&&FakeApi::closed==0);
    FakeApi::reset();FakeApi::timerFails=true;fake.prepare();
    CHECK(!fake.available()&&FakeApi::closed==1);
    FakeApi::reset();FakeApi::result=WAIT_FAILED;fake.prepare();
    CHECK(fake.waitUntil(FakeApi::time+1s)==Result::Unavailable&&!fake.available());
    CHECK(FakeApi::arms==1&&FakeApi::waits==1&&FakeApi::closed==0);
    fake.prepare();CHECK(fake.available()&&FakeApi::closed==2);fake.release();

    // Event-only timeout remains interruptible when high resolution is absent.
    FakeApi::reset();FakeApi::actions=true;FakeApi::timerFails=true;fake.prepare(true);
    CHECK(fake.available()&&fake.hasActions()&&!fake.highResolution());
    FakeApi::result=WAIT_OBJECT_0+1;
    CHECK(fake.waitUntil(FakeApi::time+1001us)==Result::Action);
    CHECK(FakeApi::lastCount==3&&FakeApi::lastTimeout==2&&FakeApi::arms==0);
    FakeApi::result=WAIT_TIMEOUT;
    CHECK(fake.waitUntil(FakeApi::time+1h,false)==Result::Elapsed&&FakeApi::lastCount==1);
    FakeApi::pending=true;CHECK(fake.consumeActions());CHECK(!fake.consumeActions());
    fake.release();CHECK(FakeApi::closed==3);
    FakeApi::reset();FakeApi::actions=true;fake.prepare(true);FakeApi::armFails=true;
    FakeApi::result=WAIT_OBJECT_0+2;
    CHECK(fake.waitUntil(FakeApi::time+1s)==Result::Action&&fake.available()&&!fake.highResolution());
    CHECK(FakeApi::lastCount==3&&FakeApi::lastTimeout==1000);
    const auto failedArms=FakeApi::arms;
    FakeApi::result=WAIT_OBJECT_0;
    CHECK(fake.waitUntil(FakeApi::time+1s)==Result::Interrupted&&FakeApi::arms==failedArms);
    fake.release();CHECK(FakeApi::closed==4);

    using NativeWait=nimby::detail::platform::ObservationWait;
    NativeWait native;native.prepare();
    const bool supported=native.available();
    if(supported) {
        native.interrupt();const auto started=Clock::now();
        CHECK(native.waitUntil(started+1h)==NativeWait::Result::Interrupted);
        CHECK(Clock::now()-started<1s);
        native.release();native.prepare();
        const auto deadline=Clock::now()+2ms;
        CHECK(native.waitUntil(deadline)==NativeWait::Result::Elapsed);
        // A timer wake is rechecked against the steady-clock deadline by the
        // loop. Restart must not retain the old manual stop event's signal.
        HANDLE waiting=CreateEventW(nullptr,TRUE,FALSE,nullptr);CHECK(waiting);
        enteredWait=waiting;
        auto pending=std::async(std::launch::async,[&]{return native.waitUntil(Clock::now()+1h);});
        const bool armed=WaitForSingleObject(waiting,2000)==WAIT_OBJECT_0;
        native.interrupt(); // Always unblock before a failed assertion unwinds.
        const auto result=pending.get();enteredWait=nullptr;CloseHandle(waiting);
        CHECK(armed&&result==NativeWait::Result::Interrupted);
    }
    native.release();

    // Exercise fallback in the actual ObservationLoop, including its stop
    // predicate and lock hand-off, for each failing OS operation.
    for(const auto injected:{CreateTimer,ArmTimer,WaitTimer}) {
        failure=injected;
        nimby::ObservationLoop loop;std::mutex mutex;std::condition_variable event;int calls=0;
        loop.start([&]{std::lock_guard guard(mutex);++calls;event.notify_all();},[]{},10ms);
        bool progressed=false;
        {std::unique_lock guard(mutex);progressed=event.wait_for(guard,2s,[&]{return calls>=3;});}
        const auto mode=loop.status().highResolutionWait;
        loop.stop();CHECK(progressed&&!mode);
        failure=None;
        loop.start([]{},[]{},1h);
        const auto start=Clock::now();loop.stop();CHECK(Clock::now()-start<1s);
    }
    failure=None;
    // Both actual action sources wake a one-hour loop. Retain the second
    // callback so an event during that callback cannot be mistaken for a
    // notification consumed before it. No timing-sensitive latency assertion.
    for(const auto injected:{None,CreateTimer,ArmTimer}) {
        actionEvents={CreateEventW(nullptr,FALSE,FALSE,nullptr),CreateEventW(nullptr,FALSE,FALSE,nullptr)};
        CHECK(actionEvents[0]&&actionEvents[1]);failure=injected;
        nimby::ObservationLoop actionLoop;
        std::mutex mutex;std::condition_variable changed;
        int calls=0;bool release=false;std::atomic<int> active=0,maximum=0;
        nimby::detail::ObservationLoopAccess::start(actionLoop,[&]{
            const auto current=++active;maximum=std::max(maximum.load(),current);
            std::unique_lock lock(mutex);++calls;changed.notify_all();
            if(calls==2)changed.wait(lock,[&]{return release;});
            --active;
        },[]{},1h,true);
        const auto await=[&](int count){std::unique_lock lock(mutex);return changed.wait_for(lock,2s,[&]{return calls>=count;});};
        const bool first=await(1);SetEvent(actionEvents[0]);const bool second=await(2);
        for(int i=0;i<32;++i)SetEvent(actionEvents[1]);
        int blockedCalls{};{std::lock_guard lock(mutex);blockedCalls=calls;release=true;}changed.notify_all();
        const bool third=await(3);
        actionLoop.stop();
        const auto status=actionLoop.status();
        CHECK(first&&second&&third&&blockedCalls==2&&maximum==1&&status.actionCycles==2);
        CHECK(status.successes==3&&status.failures==0);
        for(auto& handle:actionEvents){CloseHandle(handle);handle=nullptr;}
    }
    failure=None;
    actionEvents={CreateEventW(nullptr,FALSE,FALSE,nullptr),CreateEventW(nullptr,FALSE,FALSE,nullptr)};
    CHECK(actionEvents[0]&&actionEvents[1]);
    native.prepare(true);native.interrupt();SetEvent(actionEvents[0]);
    CHECK(native.waitUntil(Clock::now()+1h)==NativeWait::Result::Interrupted);native.release();
    // The wait owns only its duplicates. Original events remain usable and
    // repeated starts release all duplicated handles after joining the worker.
    DWORD handlesBefore{},handlesAfter{};CHECK(GetProcessHandleCount(GetCurrentProcess(),&handlesBefore));
    for(int i=0;i<100;++i){nimby::ObservationLoop loop;
        nimby::detail::ObservationLoopAccess::start(loop,[]{},[]{},1h,true);loop.stop();}
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&handlesAfter)&&handlesAfter<=handlesBefore+2);
    for(auto& handle:actionEvents){CloseHandle(handle);handle=nullptr;}
    // Warm up the runtime before checking repeated creation/stop/join/close.
    nimby::ObservationLoop loop;loop.start([]{},[]{},1h);loop.stop();
    DWORD before{},after{};CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(int i=0;i<100;++i){loop.start([]{},[]{},1h);loop.stop();}
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after)&&after<=before+2);
    std::cout<<"PASS relative deadlines, stop/restart, OS failures, fallback and handle lifetime; high-resolution="
             <<(supported?"available":"unavailable/fallback")<<'\n';
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
