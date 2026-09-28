#pragma once
#include <nimby/detail/construction.h>
#include <windows.h>
#include <string>

namespace nimby::construction_bridge {
constexpr uint32_t protocol=1;
enum TransportState : LONG { idle, pending, executing, complete };
// One in-flight request, serialized across clients by a named mutex. Endpoint
// ownership transfers only through Interlocked state changes. The last response
// stays available for polling after a timeout; no blind retry is performed.
struct Shared {
    uint32_t version=protocol,size=sizeof(Shared);
    volatile LONG state=idle;
    uint32_t reserved{};
    uint64_t expires{};
    NimbyConstructionRequest request{};
    NimbyConstructionResult result{};
};
inline std::wstring name(DWORD pid){return L"Local\\NimbyRailsFranceSDK.Construction.v1."+std::to_wstring(pid);}
uint32_t exchange(HANDLE process,DWORD pid,const NimbyBinaryInfo&,const NimbyConstructionRequest*,uint64_t pollToken,NimbyConstructionResult&) noexcept;
}
