#pragma once
#include <cstddef>
#include <cstdint>

namespace nimby::windows::automatic {
// One budget belongs to one train callback scope. The adapter supplies clock
// samples; this value owns no clock, native state, allocation or shared counter.
class RouteReadBudget {
public:
    enum class Reason { None, InvalidClock, ClockRegression, Time, Bytes, Calls };
    static constexpr std::size_t maximumBytes=4u*1024u*1024u;
    static constexpr std::uint32_t maximumReads=8192;
private:
    std::int64_t started_,lastTick_,maximumTicks_;
    std::size_t bytes_=0;
    std::uint32_t reads_=0;
    Reason reason_=Reason::None;

    constexpr bool reject(Reason reason) noexcept {reason_=reason;return false;}
public:
    constexpr RouteReadBudget(std::int64_t startTick,std::int64_t frequency) noexcept
        :started_(startTick),lastTick_(startTick),maximumTicks_(frequency>0?frequency/500:0) {
        // 2 ms is frequency / 500 ticks. Division avoids overflowing a clock
        // frequency while retaining every representable sample within 2 ms.
        if(startTick<0||frequency<=0)reason_=Reason::InvalidClock;
    }
    // Validation consumes no read or bytes, including at an exact resource
    // boundary. A subsequent allow() decides whether another read still fits.
    constexpr bool available(std::int64_t nowTick) noexcept {
        if(reason_!=Reason::None)return false;
        if(nowTick<0)return reject(Reason::InvalidClock);
        if(nowTick<lastTick_)return reject(Reason::ClockRegression);
        lastTick_=nowTick;
        // Both operands are nonnegative and ordered, so subtraction is safe
        // even for samples near INT64_MAX. Never form start + maximumTicks.
        if(nowTick-started_>maximumTicks_)return reject(Reason::Time);
        return true;
    }
    // Account before invoking the reader. A failed native read still consumed
    // its attempt; no retry can refund the cost or recover a refused budget.
    constexpr bool allow(std::size_t bytes,std::int64_t nowTick) noexcept {
        if(!available(nowTick))return false;
        if(reads_==maximumReads)return reject(Reason::Calls);
        if(bytes>maximumBytes-bytes_)return reject(Reason::Bytes);
        bytes_+=bytes;++reads_;
        return true;
    }
    constexpr Reason reason() const noexcept {return reason_;}
    constexpr std::uint32_t reads() const noexcept {return reads_;}
    constexpr std::size_t bytes() const noexcept {return bytes_;}
};
}
