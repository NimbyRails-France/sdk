#pragma once
#include <nimby/detail/train_length.h>
#include <chrono>
namespace nimby::detail::platform {
// Linux train composition integration is not available. Declaration inspection
// still works; a live policy must fail explicitly rather than pretend to apply.
struct TrainLengthFunctions {
    decltype(&NimbyInternal_TrainLengthRegister) add{};
    decltype(&NimbyInternal_TrainEditorRegister) addEditor{};
    decltype(&NimbyInternal_TrainLengthUpdate) update{};
    decltype(&NimbyInternal_TrainLengthRemove) remove{};
    bool resolve(bool=false)noexcept{return false;}
};
class TrainLengthOptionWait {
public:
    enum class Result { Stop, Action, Elapsed, Unavailable };
    bool prepare()noexcept{return false;}
    bool available()const noexcept{return false;}
    void interrupt()noexcept{}
    void release()noexcept{}
    Result wait(bool=false)noexcept{return Result::Unavailable;}
    Result pause(std::chrono::milliseconds)noexcept{return Result::Unavailable;}
    void consumeActions()noexcept{}
};
}
