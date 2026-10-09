#pragma once
#include <cstdint>
#include <runtime/mod_options_registry.h>

namespace nimby::platform::windows::mod_options {
enum class OptionsPage {Interface,Shortcuts};
// Frozen once with the displayed fields, not recomputed during interaction:
// a mod registering/unloading between native passes must not add/remove rows.
struct OptionsCategories {
    bool hasOptions=false,hasShortcuts=false;
    static OptionsCategories from(const runtime::mod_options::Snapshot& snapshot)noexcept {
        OptionsCategories result;
        for(const auto& mod:snapshot.mods)for(const auto& field:mod.fields){
            if(field.kind==runtime::mod_options::Kind::Shortcut)result.hasShortcuts=true;
            else result.hasOptions=true;
            if(result.navigation())return result;
        }
        return result;
    }
    bool navigation()const noexcept{return hasOptions&&hasShortcuts;}
    OptionsPage select(OptionsPage preferred)const noexcept {
        if(navigation())return preferred;
        return hasOptions?OptionsPage::Interface:OptionsPage::Shortcuts;
    }
};
// The NRF column uses the Interface shell's 400-unit body width and the
// native outer box's 700-unit height. The Uploader shell can be wider.
// Flow 3 alone centers children; flag 0x8 selects start justification. The
// column itself must also anchor at the top of its native parent (bit 0x40).
// Kept beside the body contract for solver-based geometry regression tests.
struct OptionsPanelGeometry {
    static constexpr float width=400.f,height=700.f,viewportHeight=560.f;
    static constexpr uint32_t columnFlow=0xb,columnAlign=0xe0,rowAlign=0xa0;
    static constexpr float viewport(bool navigation)noexcept{return viewportHeight+(navigation?0.f:68.f);}
};
// Internal two-pass body contract. The native framework's layout tree cannot
// be replaced by a different body during interaction. This is independent of
// Windows/game memory so its failure transitions can be tested without a game.
class OptionsBodyLayout {
public:
    enum class Action {Sdk,Native,Skip};
    Action prepare(bool layoutPass,bool valid)noexcept {
        if(native())return Action::Native;
        if(failed_)return Action::Skip;
        if(!valid||(!layoutPass&&mode_==Mode::Undecided))return fail();
        return Action::Sdk;
    }
    // Preparation (snapshot, translation, validation) emits nothing. Call this
    // immediately before the first SDK declaration, after preparation succeeds.
    void beginSdk()noexcept {if(!failed_&&mode_==Mode::Undecided)mode_=Mode::Sdk;}
    Action fail()noexcept {
        failed_=true;
        if(mode_==Mode::Undecided)mode_=Mode::Native;
        return native()?Action::Native:Action::Skip;
    }
    bool native()const noexcept {return mode_==Mode::Native;}
    bool failed()const noexcept {return failed_;}
private:
    enum class Mode {Undecided,Native,Sdk};
    Mode mode_=Mode::Undecided;
    bool failed_=false;
};
}
