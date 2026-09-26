#pragma once
#include "engine/live_state.h"
#include <cstdint>
#include <optional>

namespace nimby::engine {
// Internal adapter for the fingerprinted 1.19.10 binary. The bootstrap must
// verify the full executable identity before using these RVAs. A UI object is
// borrowed for ONE synchronous editor callback, on the game's UI thread.
class SignalUi {
public:
    enum class Pass { Layout, Interactive };

    static std::optional<SignalUi> bind(ReadMemory read,void* context,
                                        uint64_t module,uint64_t object,
                                        LiveStateProfile profile=LiveStateProfile::Windows119) noexcept {
        const auto& layout=gameLayout(profile);
        if(!layout.root_rva)return std::nullopt;
        const auto layoutTable=layout.ui_layout_table,interactiveTable=layout.ui_interactive_table;
        const auto layoutCheckbox=layout.ui_layout_checkbox,interactiveCheckbox=layout.ui_interactive_checkbox;
        const auto slot=layout.ui_checkbox_slot;
        constexpr uint64_t addressLimit=0x7fffffff0000ULL;
        const uint64_t lastTable=layoutTable>interactiveTable?layoutTable:interactiveTable;
        if(!read || module<0x10000 || module>addressLimit-lastTable-slot-sizeof(uint64_t) ||
           object<0x10000 || object>addressLimit-sizeof(uint64_t))return std::nullopt;
        uint64_t table{},checkbox{};
        if(!read(context,object,&table,sizeof table))return std::nullopt;
        Pass pass;
        uint64_t expected;
        if(table==module+layoutTable){pass=Pass::Layout;expected=module+layoutCheckbox;}
        else if(table==module+interactiveTable){pass=Pass::Interactive;expected=module+interactiveCheckbox;}
        else return std::nullopt;
        if(!read(context,table+slot,&checkbox,sizeof checkbox)||checkbox!=expected)return std::nullopt;
        return SignalUi(object,checkbox,pass);
    }

    [[nodiscard]] Pass pass() const noexcept { return pass_; }

    // In the editor body at RVA 0x7a0a40, capture +0x38 borrows the original
    // Signal*. Copy only its full identity; never retain that native pointer.
    static std::optional<uint64_t> editorSignal(ReadMemory read,void* context,uint64_t capture,
            LiveStateProfile profile=LiveStateProfile::Windows119) noexcept {
        const auto captureOffset=gameLayout(profile).editor_signal_capture;
        if(!captureOffset)return std::nullopt;
        constexpr uint64_t limit=0x7fffffff0000ULL;
        uint64_t signal{},id{},again{};
        if(!read||capture<0x10000||capture>limit-0x40||
           !read(context,capture+captureOffset,&signal,sizeof signal)||signal<0x10000||signal>limit-8||
           !read(context,signal,&id,sizeof id)||id>>48!=8||
           !read(context,capture+captureOffset,&again,sizeof again)||again!=signal)return std::nullopt;
        return id;
    }

    // This is an in-process call, never a remote-process operation. The native
    // primitive updates uint32_t synchronously; neither bool* nor game-owned
    // signal bytes are passed. The caller keeps label storage alive for the
    // entire layout/render cycle. Tooltip rendering is not wired yet.
    void checkbox(const char* label,const char* /*description*/,uint32_t& value) const {
        using Checkbox=void(*)(uint64_t,const char*,uint32_t*);
        reinterpret_cast<Checkbox>(checkbox_)(object_,label,&value);
    }
private:
    SignalUi(uint64_t object,uint64_t checkbox,Pass pass)
        :object_(object),checkbox_(checkbox),pass_(pass){}
    uint64_t object_,checkbox_;
    Pass pass_;
};
}
