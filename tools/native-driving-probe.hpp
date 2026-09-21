#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
namespace driving_probe {
inline std::wstring name(DWORD pid){return L"Local\\Nimby.NativeDrivingProbe.2."+std::to_wstring(pid);}
struct Sample {
    volatile LONG ready=0;
    DWORD thread=0;
    uint64_t train=0,context=0,motion=0,simulation=0,trackBefore=0,trackAfter=0;
    int64_t ticks=0,budgetBefore=0,budgetAfter=0;
    double speedBefore=0,speedAfter=0,fractionBefore=0,fractionAfter=0;
    float dynamics[8]{};
    uint64_t signal=0;
    double distanceM=0,headM=0;
};
struct Shared {
    uint32_t size=sizeof(Shared),version=1;
    volatile LONG enabled=0,count=0;
    uint64_t target=0;
    Sample samples[2048]{};
};
}
