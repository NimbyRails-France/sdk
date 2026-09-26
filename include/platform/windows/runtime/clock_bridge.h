#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include <nimby/detail/observation.h>
namespace nimby::clock_bridge {
constexpr uint32_t version=2;
enum : LONG { idle=0, pending=1, executing=2, complete=3 };
struct Shared {
    uint32_t protocol=version, size=sizeof(Shared);
    volatile LONG state=idle;
    uint32_t result=0, thread_id=0, count=0;
    uint64_t expected_sim=0, observed_sim=0, observed_db=0, callbacks=0;
    int64_t requested_utc=0, epoch=0, ticks=0;
};
inline std::wstring name(DWORD pid) {
    return L"Local\\NimbyRailsFranceSDK.ClockBridge.v2."+std::to_wstring(pid);
}
uint32_t change(HANDLE process,DWORD pid,uint64_t simulation,const NimbyBinaryInfo& binary,
                int64_t utc,NimbySimulationClock& clock,uint32_t& count) noexcept;
}
