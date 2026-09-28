#pragma once
#include <cstdint>
#include <cstring>

namespace nimby::platform::windows {
// Native Signal position at +0x40 (CreateSignal command +0x60).
// The signed direction byte is immediately followed by track orientation.
// The remaining bytes belong to the copied native record and stay untouched.
inline void writeSignalPosition(void* record,uint64_t track,double fraction,int32_t direction,uint8_t orientation) {
    auto* bytes=static_cast<unsigned char*>(record);
    const auto nativeDirection=static_cast<int8_t>(direction);
    std::memcpy(bytes,&track,sizeof track);
    std::memcpy(bytes+8,&fraction,sizeof fraction);
    std::memcpy(bytes+16,&nativeDirection,sizeof nativeDirection);
    std::memcpy(bytes+17,&orientation,sizeof orientation);
}
}
