#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include "runtime/texture_table.h"
#include <algorithm>
namespace nimby::texture_bridge {
constexpr uint32_t version=4;
constexpr auto filename=L"NimbyRailsFranceTextureBridge-experimental-v4.dll";
struct alignas(8) Shared {
    uint32_t protocol=version,size=sizeof(Shared);
    volatile LONG sequence=0;
    uint32_t index=0;
    uint64_t signal=0,set_hash=0,expected_database=0,expected_simulation=0,expires=0;
    volatile LONG64 callbacks=0,valid_signals=0,applied=0,last_signal=0,last_rules=0;
    volatile LONG render_thread=0;
    // Fixed-size IPC mailbox only. Dynamic storage is owned by the bridge.
    volatile LONG pending=0;
    uint32_t operation=0,result=0,active=0,result_index=0;
    uint64_t request_signal=0,request_hash=0,request_expiry=0;
    uint64_t request_database=0,request_simulation=0,active_count=0,result_expiry=0;
    uint32_t request_index=0;
    uint32_t request_alternate_index=0, request_half_period_ms=0;

};
inline std::wstring name(DWORD pid){return L"Local\\NimbyRailsFranceSDK.TexturePreview.v4."+std::to_wstring(pid);}
}
