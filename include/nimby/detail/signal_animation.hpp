#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>

namespace nimby::detail {
// Private rendering description, shared by the Kotlin adapter and live worker.
// Period is part of the cached value: changing only the cadence must republish.
struct SignalAnimation {
    std::string first,alternate;
    std::int64_t everyMs=0;
    bool operator==(const SignalAnimation&) const = default;
};
inline SignalAnimation checkedAnimation(SignalAnimation value) {
    const auto path=[](const auto& s){return !s.empty()&&s.size()<96&&s.find('\0')==s.npos;};
    if(!path(value.first)||!path(value.alternate)||
       (value.everyMs!=0&&(value.everyMs<100||value.everyMs>10000))||
       (!value.everyMs&&value.first!=value.alternate))throw std::invalid_argument("Invalid signal animation");
    return value;
}
template<class Rules> SignalAnimation signalAnimation(const typename Rules::Decision& decision,std::int64_t legacyHalfPeriod) {
    if constexpr(requires { Rules::animation(decision); })
        if(auto value=Rules::animation(decision))return checkedAnimation(std::move(*value));
    // Older callback-based mods keep their established two-frame behaviour.
    auto first=std::string(Rules::texture(decision,0,legacyHalfPeriod));
    auto alternate=std::string(Rules::texture(decision,legacyHalfPeriod,legacyHalfPeriod));
    const auto period=first==alternate?0:legacyHalfPeriod;
    return checkedAnimation({std::move(first),std::move(alternate),period});
}
}
