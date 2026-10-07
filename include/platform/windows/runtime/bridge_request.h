#pragma once
#include <windows.h>
#include <cstdint>

namespace nimby::platform::windows {
// A queued game mutation belongs to one live process and has a short deadline.
// The native boundary checks this again before starting the mutation: killing
// an isolated worker cannot leave work queued for a later simulation frame.
struct BridgeRequestLease {
    uint64_t expires{}, creation{};
    uint32_t process{}, reserved{};
};
inline uint64_t processCreation(HANDLE process) noexcept {
    FILETIME creation{},exit{},kernel{},user{};
    if(!GetProcessTimes(process,&creation,&exit,&kernel,&user))return 0;
    return (uint64_t(creation.dwHighDateTime)<<32)|creation.dwLowDateTime;
}
inline BridgeRequestLease bridgeRequestLease(uint32_t timeoutMs) noexcept {
    return {GetTickCount64()+timeoutMs,processCreation(GetCurrentProcess()),GetCurrentProcessId(),0};
}
inline bool sameBridgeRequester(const BridgeRequestLease& a,const BridgeRequestLease& b) noexcept {
    return a.process&&a.process==b.process&&a.creation&&a.creation==b.creation;
}
inline bool bridgeRequesterAlive(const BridgeRequestLease& lease) noexcept {
    if(!lease.process||!lease.creation)return false;
    const auto process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,lease.process);
    if(!process)return false;
    const bool alive=WaitForSingleObject(process,0)==WAIT_TIMEOUT&&processCreation(process)==lease.creation;
    CloseHandle(process);return alive;
}
inline bool bridgeRequestAlive(const BridgeRequestLease& lease) noexcept {
    return GetTickCount64()<lease.expires&&bridgeRequesterAlive(lease);
}
}
