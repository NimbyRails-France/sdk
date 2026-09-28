#pragma once
#include <windows.h>
#include <cstdint>
#include <string>

// Diagnostic ABI only; never distributed as a mod construction API. Events
// contain process addresses and are meaningful only for this process instance.
namespace nimby::construction_probe {
constexpr uint32_t capacity=128;
enum Kind : uint32_t { Enqueued=1, CreateBefore, CreateAfter, UndoBefore, UndoAfter, HistoryBefore, HistoryAfter };
struct Event {
    uint64_t tick{},command{},context{},result{},db{},historyCount{};
    uint32_t kind{},thread{},valid{},stackCount{};
    // valid: 1=command, 2=result tag, 4=delta, 8=db pointer, 16=history count.
    // Enqueued has only header[0..3]; payload initialization follows the hook.
    // resultWords[62] is the variant tag; all other resultWords stay zero.
    // Delta bytes are native containers, NOT recursively copied owned objects.
    uint64_t header[16]{},resultWords[64]{},deltaWords[192]{},stack[16]{};
};
struct Shared {
    uint32_t version=1,size=sizeof(Shared);
    volatile LONG sequence=0;
    uint32_t reserved=0;
    volatile LONG64 expires=0;
    uint64_t base{},count{};
    Event events[capacity]{};
};
inline std::wstring name(DWORD pid){return L"Local\\NimbyRailsFranceSDK.ConstructionProbe.v1."+std::to_wstring(pid);}
}
