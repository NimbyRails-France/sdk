#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <string>
namespace nimby::automatic_status {
inline std::wstring name(DWORD pid){return L"Local\\Nimby.AutomaticDriving.5."+std::to_wstring(pid);}
// Append-only records: the reader never races a reused diagnostic slot.
struct PermissionSample {
 volatile LONG ready=0;
 uint64_t train=0,signal=0,proof=0;
 // state bits: eligible=1, native approval=2, geometry=4, started=8, granted=16,
 // train known=32, lookahead=64, longitudinal reservation refusal replaced=128.
 uint32_t flags=0,state=0,ranges=0;
 double head=0,source=0,covered=0,free=0;
 // Monotonic GetTickCount64 milliseconds, not simulation time or UTC.
 uint64_t observedAtMs=0;
};
struct Sample {
 volatile LONG ready=0;uint64_t train=0;
 double before=0,after=0,headBefore=0,headAfter=0,ceiling=0,nativeCeiling=0,braking=0;
 int64_t ticks=0;
 uint64_t restrictedSource=0;
 double freeDistance=0;
 uint32_t visibilityVerified=0;
 uint64_t observedAtMs=0;
 double nativeBraking=0,nativeDistance=0,nativeTarget=0,chosenDistance=0,chosenTarget=0;
 // 0 planned, 1 same train busy, 2 train slot missing, 3 publication expired.
 uint32_t reason=0;
};
struct Shared {
 uint32_t size=sizeof(Shared),version=5;
 volatile LONG64 scans=0,applied=0,stops=0;
 volatile LONG64 restrictedChecks=0,restrictedGranted=0,restrictedDenied=0,restrictedSteps=0;
 uint64_t session=0,lastSignal=0;
 volatile LONG ruleCount=0,count=0;
 Sample samples[4096]{};
 volatile LONG permissionCount=0;
 PermissionSample permissions[512]{};
 volatile LONG64 integrationCalls=0,permissionCalls=0,committedIntegrations=0,discardedCommits=0;
 volatile LONG64 busyTrainScans=0,busyTrainPermissions=0,busyTrainIntegrations=0,missingTrainSlots=0;
 volatile LONG64 publicationBusy=0,publicationRetirementFull=0,publicationConflicts=0;
 // Unmanaged trains: native arguments unchanged, or only global cruise policy.
 volatile LONG64 unmanagedNativeIntegrations=0,unmanagedCruiseIntegrations=0;
};
static_assert(sizeof(PermissionSample)==88 && offsetof(PermissionSample,observedAtMs)==80);
static_assert(sizeof(Sample)==160 && offsetof(Sample,observedAtMs)==104 && offsetof(Sample,reason)==152);
static_assert(sizeof(Shared)==700616 && offsetof(Shared,integrationCalls)==700512 && offsetof(Shared,unmanagedNativeIntegrations)==700600);
}
