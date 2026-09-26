#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
namespace nimby::driving_bridge {
inline std::wstring name(DWORD pid){return L"Local\\Nimby.DrivingBridge.1."+std::to_wstring(pid);}
// Temporary experiment protocol. Not exported as a supported driving API.
struct Sample {
 volatile LONG ready=0;
 uint64_t train=0,trackBefore=0,trackAfter=0;
 double speedBefore=0,speedAfter=0,headBefore=0,headAfter=0,fractionBefore=0,fractionAfter=0;
 double ceiling=0,braking=0,extraMass=0;
 int64_t budget=0,used=0;
 uint64_t wallMs=0;
};
struct Shared {
 uint32_t size=sizeof(Shared),version=1;
 volatile LONG state=0,count=0; // 0 idle, 1 pending, 2 running, 3 expired/withdrawn
 uint64_t train=0,expiresWallMs=0;
 double ceilingMps=0,brakeUse=0.8;
 int64_t durationMs=0;
 volatile LONG64 applied=0;
 Sample samples[1024]{};
};
}
