#pragma once
#include <runtime/mod_options_registry.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace nimby::platform::windows::mod_options {
// Use the current registry snapshot, not the frozen layout snapshot: a mod may
// unload between drawing its Assign button and the next keyboard event. Tokens
// distinguish a reloaded mod from the owner that started the capture.
// Visibility must include pending navigation and validated emitter identity;
// a previous draw, another options page or a native fallback is not sufficient.
inline bool captureAvailable(const runtime::mod_options::Snapshot& snapshot,uint64_t ownerToken,
                             std::string_view fieldId,bool shortcutsPanelVisible)noexcept {
    if(!shortcutsPanelVisible||!ownerToken||fieldId.empty())return false;
    for(const auto& owner:snapshot.mods)if(owner.token==ownerToken){
        for(const auto& field:owner.fields)if(field.id==fieldId)
            return field.kind==runtime::mod_options::Kind::Shortcut;
        return false;
    }
    return false;
}

// Own only keys whose down event was captured by this SDK. A later key-up or
// repeat must not reach native commands, even after assignment/cancellation.
// Scancodes identify physical keys across modifier/layout changes. The bounded
// logical-key fallback supports SDL events with an unknown physical scancode.
class CapturedKeys {
public:
    enum class Route {Native,Capture,Consumed};
    static constexpr size_t scancodes=512,logicalKeys=512;
    bool empty()const noexcept {return !owned_;}
    void window(uint32_t id)noexcept {if(id!=window_){clear();window_=id;}}
    Route route(uint32_t window,uint32_t scancode,uint16_t key,bool down,bool repeat,bool capturing)noexcept {
        if(!window_||window!=window_)return Route::Native;
        const auto slot=index(scancode,key);
        if(slot==held_.size())return Route::Native;
        if(held_[slot]){
            if(!down){held_[slot]=false;--owned_;}
            return Route::Consumed;
        }
        if(down&&!repeat&&capturing&&key){held_[slot]=true;++owned_;return Route::Capture;}
        return Route::Native;
    }
    // Call only AFTER the native pump finishes draining events, so a queued
    // key-up is swallowed before released physical keys are retired.
    void finish(const bool* states,size_t count,bool focused)noexcept {
        if(!focused){clear();return;}
        if(states&&owned_)for(size_t i=1;i<scancodes;++i)if(held_[i]&&(i>=count||!states[i])){held_[i]=false;--owned_;}
    }
    void clear()noexcept {if(owned_)held_.fill(false);owned_=0;}
private:
    uint32_t window_=0;
    size_t owned_=0;
    std::array<bool,scancodes+logicalKeys> held_{};
    static size_t index(uint32_t scancode,uint16_t key)noexcept {
        if(scancode&&scancode<scancodes)return scancode;
        return key&&key<logicalKeys?scancodes+key:scancodes+logicalKeys;
    }
};
}
