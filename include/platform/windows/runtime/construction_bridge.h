#pragma once
#include <nimby/detail/construction.h>
#include <windows.h>
#include <string>
#include "platform/windows/runtime/bridge_request.h"

namespace nimby::construction_bridge {
constexpr uint32_t protocol=2;
enum TransportState : LONG { idle, pending, executing, complete };
// One in-flight request, serialized across clients by a named mutex. Endpoint
// ownership transfers only through Interlocked state changes. The last response
// stays available for polling after a timeout; no blind retry is performed.
struct Shared {
    uint32_t version=protocol,size=sizeof(Shared);
    volatile LONG state=idle;
    volatile LONG reserved{}; // Result acknowledgement, same protocol layout.
    uint64_t expires{};
    platform::windows::BridgeRequestLease lease;
    NimbyConstructionRequest request{};
    NimbyConstructionResult result{};
};
constexpr uint64_t resultRetentionMs=2000;
// Publishing completion renews only the response's read window. The mutation
// lease keeps its original deadline. A dead/slow reader cannot reserve the
// shared endpoint forever, and an acknowledged response needs no grace period.
inline void completeResponse(Shared& shared,uint64_t now) noexcept {
    shared.expires=now+resultRetentionMs;
    InterlockedExchange(&shared.reserved,0);
    InterlockedExchange(&shared.state,complete);
}
template<class Alive>
bool responseReserved(Shared& shared,const platform::windows::BridgeRequestLease& requester,
                      uint64_t now,Alive&& requesterAlive) {
    return InterlockedCompareExchange(&shared.state,complete,complete)==complete&&
        !platform::windows::sameBridgeRequester(shared.lease,requester)&&
        !InterlockedCompareExchange(&shared.reserved,0,0)&&now<shared.expires&&requesterAlive(shared.lease);
}
inline std::wstring name(DWORD pid){return L"Local\\NimbyRailsFranceSDK.Construction.v2."+std::to_wstring(pid);}
uint32_t exchange(HANDLE process,DWORD pid,const NimbyBinaryInfo&,const NimbyConstructionRequest*,uint64_t pollToken,NimbyConstructionResult&) noexcept;
}
