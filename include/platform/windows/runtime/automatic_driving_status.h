#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
namespace nimby::automatic_status {
inline std::wstring name(DWORD pid){return L"Local\\Nimby.AutomaticDriving.4."+std::to_wstring(pid);}
// Append-only records: the reader never races a reused diagnostic slot.
struct PermissionSample {
 volatile LONG ready=0;
 uint64_t train=0,signal=0,proof=0;
 uint32_t flags=0,state=0,ranges=0;
 double head=0,source=0,covered=0,free=0;
};
struct Sample {
 volatile LONG ready=0;uint64_t train=0;
 double before=0,after=0,headBefore=0,headAfter=0,ceiling=0,nativeCeiling=0,braking=0;
 int64_t ticks=0;
 uint64_t restrictedSource=0;
 double freeDistance=0;
 uint32_t visibilityVerified=0;
};
struct Shared {
 uint32_t size=sizeof(Shared),version=4;
 volatile LONG64 scans=0,applied=0,stops=0;
 volatile LONG64 restrictedChecks=0,restrictedGranted=0,restrictedDenied=0,restrictedSteps=0;
 uint64_t session=0,lastSignal=0;
 volatile LONG ruleCount=0,count=0;
 Sample samples[4096]{};
 volatile LONG permissionCount=0;
 PermissionSample permissions[512]{};
};
}
