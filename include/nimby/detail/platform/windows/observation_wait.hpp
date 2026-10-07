#pragma once
#include <nimby/detail/platform/windows/system.hpp>
#include <array>
#include <algorithm>
#include <bit>
#include <chrono>
#include <cstdint>

namespace nimby::detail::platform {
struct ObservationWaitApi {
    using Clock=std::chrono::steady_clock;
    static Clock::time_point now() noexcept { return Clock::now(); }
    static HANDLE createStop() noexcept { return CreateEventW(nullptr,TRUE,FALSE,nullptr); }
    static HANDLE createTimer() noexcept {
        // Windows 10 1803+. Action notifications still work with a timed event
        // wait when the high-resolution timer is unavailable.
        return CreateWaitableTimerExW(nullptr,nullptr,CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                      TIMER_MODIFY_STATE|SYNCHRONIZE);
    }
    static std::array<HANDLE,2> copyActions() noexcept {
        using Function=uint32_t(__cdecl*)(uint64_t*,uint64_t*);
        const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
        const auto function=sdk?std::bit_cast<Function>(GetProcAddress(sdk,"NimbyInternal_ModHostActionWaits")):nullptr;
        uint64_t remote{},local{};
        if(!function||function(&remote,&local)!=0)return {};
        return {reinterpret_cast<HANDLE>(static_cast<uintptr_t>(remote)),reinterpret_cast<HANDLE>(static_cast<uintptr_t>(local))};
    }
    static bool arm(HANDLE timer,std::int64_t relativeTicks) noexcept {
        LARGE_INTEGER due{};due.QuadPart=relativeTicks;
        return SetWaitableTimerEx(timer,&due,0,nullptr,nullptr,nullptr,0)!=FALSE;
    }
    static DWORD wait(const HANDLE* handles,DWORD count,DWORD timeout) noexcept {
        return WaitForMultipleObjects(count,handles,FALSE,timeout);
    }
    static bool consume(HANDLE event) noexcept {return WaitForSingleObject(event,0)==WAIT_OBJECT_0;}
    static void signal(HANDLE stop) noexcept { SetEvent(stop); }
    static void close(HANDLE handle) noexcept { CloseHandle(handle); }
};

// Private stop/timer and optional, owned SYNCHRONIZE-only action duplicates.
// No global timer-resolution change, extra thread, periodic timer, APC or spin.
// prepare/release require no active waiter; stop signals before join and all
// handles stay alive until that join completes.
template<class Api=ObservationWaitApi> class BasicObservationWait {
public:
    using Clock=typename Api::Clock;
    enum class Result { Elapsed, Interrupted, Action, Unavailable };
    BasicObservationWait()=default;
    BasicObservationWait(const BasicObservationWait&)=delete;
    BasicObservationWait& operator=(const BasicObservationWait&)=delete;
    ~BasicObservationWait(){release();}

    void prepare(bool actions=false) noexcept {
        release();
        stop_=Api::createStop();
        if(!stop_)return;
        if(actions)actions_=Api::copyActions();
        timer_=Api::createTimer();
        highResolution_=timer_!=nullptr;
        if(!timer_&&!hasActions()){release();return;}
        usable_=true;
    }
    void release() noexcept {
        usable_=highResolution_=false;
        for(auto& action:actions_)if(action){Api::close(action);action=nullptr;}
        if(timer_){Api::close(timer_);timer_=nullptr;}
        if(stop_){Api::close(stop_);stop_=nullptr;}
    }
    void interrupt() const noexcept { if(stop_)Api::signal(stop_); }
    bool available() const noexcept { return usable_; }
    bool highResolution() const noexcept { return usable_&&highResolution_; }
    bool hasActions() const noexcept {return actions_[0]||actions_[1];}
    // A failed wait stays disabled until restart; no repeated broken OS calls.
    // Stop may still signal concurrently, so release belongs after join.
    void disable() noexcept { usable_=highResolution_=false; }
    bool consumeActions() noexcept {
        if(!usable_)return false;
        bool pending=false;
        for(const auto action:actions_)if(action)pending=Api::consume(action)||pending;
        return pending;
    }
    Result waitUntil(Clock::time_point deadline,bool listenActions=true) noexcept {
        if(!usable_)return Result::Unavailable;
        const auto remaining=deadline-Api::now();
        if(remaining<=Clock::duration::zero())return Result::Elapsed;
        using Ticks=std::chrono::duration<std::int64_t,std::ratio<1,10000000>>;
        if(highResolution_&&!Api::arm(timer_,-std::chrono::ceil<Ticks>(remaining).count())){
            highResolution_=false;
            if(!hasActions()){disable();return Result::Unavailable;}
        }
        // Stop always wins. Once one action is pending, omit both action
        // handles until the bounded earliest start; repeated SetEvent cannot
        // turn the throttled portion into an immediate-wake busy loop.
        std::array<HANDLE,4> handles{};DWORD count=0;
        handles[count++]=stop_;
        if(highResolution_)handles[count++]=timer_;
        const auto actionBegin=count;
        if(listenActions)for(const auto action:actions_)if(action)handles[count++]=action;
        const auto timeout=highResolution_?INFINITE:static_cast<DWORD>(std::min<int64_t>(
            std::chrono::ceil<std::chrono::milliseconds>(remaining).count(),INFINITE-1));
        const auto result=Api::wait(handles.data(),count,timeout);
        if(result==WAIT_OBJECT_0)return Result::Interrupted;
        if(result==WAIT_TIMEOUT||(highResolution_&&result==WAIT_OBJECT_0+1))return Result::Elapsed;
        if(result>=WAIT_OBJECT_0+actionBegin&&result<WAIT_OBJECT_0+count)return Result::Action;
        disable();return Result::Unavailable;
    }
private:
    HANDLE stop_{},timer_{};
    std::array<HANDLE,2> actions_{};
    bool usable_=false,highResolution_=false; // Worker-owned after prepare.
};
using ObservationWait=BasicObservationWait<>;
}
