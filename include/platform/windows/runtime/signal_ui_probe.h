#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
namespace nimby::signal_ui_probe {
struct Event {uint64_t capture{},signal{};uint32_t thread{},pass{};};
struct Shared {
    uint32_t version=1,size=sizeof(Shared);
    volatile LONG sequence=0;
    uint32_t reserved=0;
    volatile LONG64 expires=0;
    uint64_t count=0;
    Event events[16]{};
};
inline std::wstring name(DWORD pid){return L"Local\\NimbyRailsFranceSDK.SignalUiProbe.v1."+std::to_wstring(pid);}
}
