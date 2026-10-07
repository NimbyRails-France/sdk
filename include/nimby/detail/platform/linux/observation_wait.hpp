#pragma once
#include <chrono>

namespace nimby::detail::platform {
// Linux support is paused; the common loop keeps its interruptible C++ wait.
class ObservationWait {
public:
    enum class Result { Elapsed, Interrupted, Action, Unavailable };
    void prepare(bool=false) noexcept {}
    void release() noexcept {}
    void interrupt() const noexcept {}
    bool available() const noexcept { return false; }
    bool highResolution() const noexcept { return false; }
    bool hasActions() const noexcept { return false; }
    bool consumeActions() noexcept { return false; }
    void disable() noexcept {}
    Result waitUntil(std::chrono::steady_clock::time_point,bool=true) noexcept { return Result::Unavailable; }
};
}
