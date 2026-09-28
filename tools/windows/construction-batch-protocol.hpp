#pragma once
#include <windows.h>
#include <cstdint>
#include <string>

// Private, one-shot experiment. A matching MANUAL CreateSignal command is
// expanded on its existing simulation thread, inside its existing native delta.
// This is not an API for consumers and must never be installed with the SDK.
namespace nimby::construction_batch_probe {
constexpr uint32_t protocol=1, capacity=16;
enum State : LONG { idle, armed, executing, complete };
struct Shared {
    uint32_t version=protocol, size=sizeof(Shared);
    volatile LONG state=idle;
    uint32_t count{}, created{}, error{}, thread{};
    uint64_t track{}, expires{}, ids[capacity]{};
    double fractions[capacity]{};
};
inline std::wstring name(DWORD pid) {return L"Local\\NimbyRailsFranceSDK.ConstructionBatchProbe.v1."+std::to_wstring(pid);}
}
