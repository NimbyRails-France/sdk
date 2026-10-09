#pragma once
#include <nimby/detail/train_length.h>
#include <nimby/detail/platform/windows/observation_wait.hpp>
#include <chrono>
#include <bit>

namespace nimby::detail::platform {
struct TrainLengthFunctions {
    decltype(&NimbyInternal_TrainLengthRegister) add{};
    decltype(&NimbyInternal_TrainEditorRegister) addEditor{};
    decltype(&NimbyInternal_TrainLengthUpdate) update{};
    decltype(&NimbyInternal_TrainLengthRemove) remove{};
    bool resolve(bool editor=false)noexcept {
        const auto sdk=GetModuleHandleW(L"NimbyRailsFranceSDK.dll");
        if(!sdk)return false;
        add=std::bit_cast<decltype(add)>(GetProcAddress(sdk,"NimbyInternal_TrainLengthRegister"));
        addEditor=std::bit_cast<decltype(addEditor)>(GetProcAddress(sdk,"NimbyInternal_TrainEditorRegister"));
        update=std::bit_cast<decltype(update)>(GetProcAddress(sdk,"NimbyInternal_TrainLengthUpdate"));
        remove=std::bit_cast<decltype(remove)>(GetProcAddress(sdk,"NimbyInternal_TrainLengthRemove"));
        return (editor?bool(addEditor):bool(add))&&update&&remove;
    }
};

// Event-only preference worker. Duplicates are SYNCHRONIZE-only capabilities;
// release() belongs after the owning worker's join, never at DLL detach.
class TrainLengthOptionWait {
public:
    enum class Result { Stop, Action, Elapsed, Unavailable };
    TrainLengthOptionWait()=default;
    TrainLengthOptionWait(const TrainLengthOptionWait&)=delete;
    TrainLengthOptionWait& operator=(const TrainLengthOptionWait&)=delete;
    bool prepare()noexcept {
        release();stop_=ObservationWaitApi::createStop();actions_=ObservationWaitApi::copyActions();
        if(!stop_||!actions_[0]||!actions_[1]){release();return false;}
        return true;
    }
    bool available()const noexcept{return stop_&&actions_[0]&&actions_[1];}
    void interrupt()noexcept{if(stop_)ObservationWaitApi::signal(stop_);}
    void release()noexcept {
        for(auto& action:actions_)if(action){ObservationWaitApi::close(action);action=nullptr;}
        if(stop_){ObservationWaitApi::close(stop_);stop_=nullptr;}
    }
    Result wait(bool retryPending=false)noexcept {
        if(!available())return Result::Unavailable;
        const std::array<HANDLE,3> handles={stop_,actions_[0],actions_[1]};
        const auto wake=ObservationWaitApi::wait(handles.data(),DWORD(handles.size()),retryPending?100:INFINITE);
        if(wake==WAIT_OBJECT_0)return Result::Stop;
        if(wake==WAIT_OBJECT_0+1||wake==WAIT_OBJECT_0+2)return Result::Action;
        if(retryPending&&wake==WAIT_TIMEOUT)return Result::Elapsed;
        return Result::Unavailable;
    }
    Result pause(std::chrono::milliseconds duration)noexcept {
        if(!stop_)return Result::Unavailable;
        const auto wake=WaitForSingleObject(stop_,static_cast<DWORD>(duration.count()));
        if(wake==WAIT_OBJECT_0)return Result::Stop;
        return wake==WAIT_TIMEOUT?Result::Elapsed:Result::Unavailable;
    }
    void consumeActions()noexcept{for(const auto action:actions_)if(action)ObservationWaitApi::consume(action);}
private:
    HANDLE stop_{};
    std::array<HANDLE,2> actions_{};
};
}
