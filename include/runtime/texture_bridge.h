#pragma once
#include <windows.h>
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
namespace nimby::texture_bridge {
constexpr uint32_t version=3;
constexpr auto filename=L"NimbyRailsFranceTextureBridge-experimental-v3.dll";
struct Command {
    uint64_t signal=0,set_hash=0,expires=0;
    uint32_t index=0;
};
// Sorted by full signal ID: bounded binary search in the render hook.
struct Table {
    std::vector<Command> entries;
    size_t count() const noexcept { return entries.size(); }
    size_t lower(uint64_t id) const noexcept {
        return std::lower_bound(entries.begin(),entries.end(),id,
            [](const Command& entry,uint64_t key){return entry.signal<key;})-entries.begin();
    }
    void prune(uint64_t now) noexcept {
        entries.erase(std::remove_if(entries.begin(),entries.end(),
            [now](const Command& entry){return entry.expires<=now;}),entries.end());
    }
    bool put(Command command) {
        const auto at=lower(command.signal);
        if(at<count()&&entries[at].signal==command.signal)entries[at]=command;
        else entries.insert(entries.begin()+at,command);
        return true;
    }
    bool erase(uint64_t id) noexcept {
        const auto at=lower(id);if(at==count()||entries[at].signal!=id)return false;
        entries.erase(entries.begin()+at);return true;
    }
};
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

};
inline std::wstring name(DWORD pid){return L"Local\\NimbyRailsFranceSDK.TexturePreview.v3."+std::to_wstring(pid);}
}
