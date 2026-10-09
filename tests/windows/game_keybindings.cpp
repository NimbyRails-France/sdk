#include "platform/windows/game_keybindings.h"
#include "platform/windows/mod_options_capture.h"
#include "platform/windows/mod_options_layout.h"
#include <iostream>
#include <stdexcept>
#include <map>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Native game keybindings: " #x);}while(false)
namespace {
struct Memory {
    static constexpr uint64_t base=0x140000000,table=base+0xb5d940,first=0x100000;
    std::map<uint64_t,std::vector<unsigned char>> blocks;
    uint64_t allocated=0x200000;bool mutate=false;unsigned headerReads=0;
    Memory(){blocks[base+0xb819d0]={1};blocks[base+0xb5b0f8]={0};blocks[table].resize(40);}
    template<class T>void put(uint64_t at,size_t offset,T value){auto& bytes=blocks.at(at);CHECK(offset+sizeof value<=bytes.size());std::memcpy(bytes.data()+offset,&value,sizeof value);}
    void string(uint64_t at,size_t offset,std::string_view value){
        put(at,offset+16,uint64_t(value.size()));put(at,offset+24,uint64_t(value.size()<16?15:63));
        if(value.size()<16)std::memcpy(blocks.at(at).data()+offset,value.data(),value.size());
        else {const auto address=allocated;allocated+=0x100;blocks[address]={value.begin(),value.end()};put(at,offset,address);}
    }
    void entries(std::initializer_list<std::pair<std::string,uint32_t>> values){
        size_t i=0;for(const auto& [key,value]:values){const auto at=first+i*0x100;blocks[at].resize(96);
            put(at,16,i?at-0x100:table);string(at,32,key);string(at,64,std::to_string(value));
            if(i)put(at-0x100,0,at);
            ++i;}
        put(table,0,first);put(table,8,first+(i-1)*0x100);put(table,16,first);put(table,32,uint64_t(i));
    }
    static bool read(void* ctx,uint64_t at,void* output,size_t count){auto& m=*static_cast<Memory*>(ctx);
        if(at==table&&++m.headerReads==2&&m.mutate)m.put(table,32,uint64_t(99));
        auto it=m.blocks.upper_bound(at);if(it==m.blocks.begin())return false;--it;
        if(at-it->first>it->second.size()||count>it->second.size()-(at-it->first))return false;
        std::memcpy(output,it->second.data()+at-it->first,count);return true;
    }
    auto read(){return nimby::platform::windows::readGameKeyBindings(read,this,base);}
};
void activeAssignments(){
    Memory m;m.entries({{"kb1_speed_up",120},{"kb2_speed_up",0x40000057},{"kb1_mode_tracks",0},{"kb1_map_left",97}});
    auto value=m.read();CHECK(value&&value->size()==3);CHECK((*value)[0]==(nimby::platform::windows::GameKeyBinding{"map_left",97}));
    CHECK((*value)[1]==(nimby::platform::windows::GameKeyBinding{"speed_up",120})); // user's X, not factory '='
    CHECK((*value)[2].key==0x40000057);
    m.put(Memory::first,64,uint64_t('z')); // malformed decimal value: unavailable, never an empty conflict table
    CHECK(!m.read());
}
void rejectUnknown(){
    Memory m;m.entries({{"kb1_map_left",97}});m.blocks[Memory::base+0xb5b0f8][0]=1;CHECK(!m.read());
    m.blocks[Memory::base+0xb5b0f8][0]=0;m.blocks[Memory::base+0xb819d0][0]=0;CHECK(!m.read());
    m.blocks[Memory::base+0xb819d0][0]=1;m.put(Memory::table,32,uint64_t(2049));CHECK(!m.read());
    m.put(Memory::table,32,uint64_t(1));m.put(Memory::first,0,Memory::first);CHECK(!m.read());
    m.put(Memory::first,0,uint64_t(0));m.put(Memory::first,16,uint64_t(0));CHECK(!m.read());
    m.put(Memory::first,16,Memory::table);m.put(Memory::first,48,uint64_t(200));CHECK(!m.read());
    Memory replacement;replacement.entries({{"kb1_map_left",97}});replacement.mutate=true;CHECK(!replacement.read());
}
void keyboard(){
    using namespace nimby::platform::windows;using namespace nimby::engine::mod_shortcuts;
    CHECK(gameShortcutKey('a')==Key::A);CHECK(gameShortcutKey('A')==Key::A);CHECK(gameShortcutKey('9')==Key::D9);
    CHECK(gameShortcutKey(0x40000045)==Key::F12);CHECK(gameShortcutKey(0x40000073)==Key::F24);
    CHECK(gameShortcutKey(0x4000004f)==Key::Right);CHECK(!gameShortcutKey(0x400000e0));
    CHECK(gameShortcutModifiers(0x00c0|0x0300|0x0003)==uint8_t(Control|Alt|Shift));
    CHECK(gameShortcutModifiers(0x3000)==0); // Num/Caps do not invent a modifier chord
    CHECK(!gameShortcutModifiers(0x0400));CHECK(!gameShortcutModifiers(0x4000));CHECK(!gameShortcutModifiers(4));
}
void captureOwnership(){
    using nimby::platform::windows::mod_options::CapturedKeys;
    using R=CapturedKeys::Route;CapturedKeys keys;keys.window(1);
    // The candidate is withheld from the native dispatcher, then assignment
    // succeeds. Its repeat and key-up still belong to the SDK afterward.
    CHECK(keys.route(1,68,266,true,false,true)==R::Capture); // F11
    CHECK(keys.route(1,68,266,true,true,false)==R::Consumed);
    CHECK(keys.route(1,68,266,false,false,false)==R::Consumed);
    CHECK(keys.route(1,68,266,true,false,false)==R::Native);
    // Foreign windows and a key already held when capture starts are untouched.
    CHECK(keys.route(2,68,266,true,false,true)==R::Native);
    CHECK(keys.route(1,68,266,true,true,true)==R::Native);
    // Modifier-only events have no assignable key. Releases are still matched
    // by physical scancode if the keyboard layout/keycode changes while held.
    CHECK(keys.route(1,224,0,true,false,true)==R::Native);
    CHECK(keys.route(1,20,81,true,false,true)==R::Capture);
    CHECK(keys.route(1,20,65,false,false,false)==R::Consumed);
    // Losing focus resets stale capture ownership; new native presses work.
    CHECK(keys.route(1,68,266,true,false,true)==R::Capture);
    keys.finish(nullptr,0,false);
    CHECK(keys.route(1,68,266,true,false,false)==R::Native);
    // Reconcile physical state after drain without losing a still-held key.
    std::array<bool,512> states{};states[68]=true;
    CHECK(keys.route(1,68,266,true,false,true)==R::Capture);
    keys.finish(states.data(),states.size(),true);
    CHECK(keys.route(1,68,266,true,true,false)==R::Consumed);
    states[68]=false;keys.finish(states.data(),states.size(),true);
    CHECK(keys.route(1,68,266,true,false,false)==R::Native);
    // Unknown scancodes remain bounded and match by logical key; switching the
    // primary window drops old ownership rather than swallowing another window.
    CHECK(keys.route(1,0,65,true,false,true)==R::Capture);
    CHECK(keys.route(1,0,65,false,false,false)==R::Consumed);
    CHECK(keys.route(1,9999,9999,true,false,true)==R::Native);
    CHECK(keys.route(1,0,65,true,false,true)==R::Capture);
    keys.window(2);CHECK(keys.route(2,0,65,true,false,false)==R::Native);
}
void captureLifecycle(){
    using namespace nimby::runtime::mod_options;
    using nimby::platform::windows::mod_options::captureAvailable;
    using nimby::platform::windows::mod_options::CapturedKeys;
    using R=CapturedKeys::Route;
    Registry registry;
    const Definition original{"captureMod","Capture mod",{
        {"window.main","Open main window",{},Kind::Shortcut,"F8",0,0,{}},
        {"window.disabled","Open disabled window",{},Kind::Shortcut,"",0,0,{}},
        {"visible","Show details",{},Kind::Boolean,"true",0,0,{}}}};
    const auto first=registry.add(1,original);CHECK(first);
    const auto second=registry.add(2,{"otherMod","Other mod",{
        {"window.main","Other window",{},Kind::Shortcut,"F9",0,0,{}}}});CHECK(second);
    const auto snapshot=registry.snapshot();
    CHECK(captureAvailable(*snapshot,first.token,"window.main",true));
    // Disabled assignments can be edited, and unavailable native conflict data
    // must not prevent the user from clearing/cancelling an assignment.
    CHECK(!snapshot->nativeBindingsKnown);
    CHECK(captureAvailable(*snapshot,first.token,"window.disabled",true));
    CHECK(!captureAvailable(*snapshot,0,"window.main",true));
    CHECK(!captureAvailable(*snapshot,first.token+99,"window.main",true));
    CHECK(!captureAvailable(*snapshot,first.token,"",true));
    CHECK(!captureAvailable(*snapshot,first.token,"absent",true));
    CHECK(!captureAvailable(*snapshot,first.token,"visible",true));
    // Leaving Shortcuts (including native fallback or an invalid emitter)
    // cancels capture even though the owner and field still exist.
    CHECK(!captureAvailable(*snapshot,first.token,"window.main",false));

    CapturedKeys keys;keys.window(1);
    CHECK(keys.route(1,68,266,true,false,true)==R::Capture);
    CHECK(registry.remove(first.token));
    const auto retired=registry.snapshot();
    CHECK(retired->mods.size()==1); // another mod keeps the options page alive
    CHECK(!captureAvailable(*retired,first.token,"window.main",true));
    CHECK(captureAvailable(*retired,second.token,"window.main",true));
    // Invalidating a target must not discard ownership of its held key; native
    // commands must not receive the matching repeat or release afterwards.
    CHECK(keys.route(1,68,266,true,true,false)==R::Consumed);
    CHECK(keys.route(1,68,266,false,false,false)==R::Consumed);
    CHECK(keys.route(1,68,266,true,false,false)==R::Native);

    const auto reloaded=registry.add(3,original);CHECK(reloaded);
    CHECK(reloaded.token!=first.token);
    CHECK(!captureAvailable(*registry.snapshot(),first.token,"window.main",true));
    CHECK(captureAvailable(*registry.snapshot(),reloaded.token,"window.main",true));
    CHECK(registry.remove(reloaded.token));
    auto changed=original;
    changed.fields[0].kind=Kind::Boolean;changed.fields[0].defaultValue="true";
    const auto differentType=registry.add(4,changed);CHECK(differentType);
    CHECK(!captureAvailable(*registry.snapshot(),differentType.token,"window.main",true));
    CHECK(registry.remove(differentType.token));
    changed.fields.erase(changed.fields.begin());
    const auto removedField=registry.add(5,changed);CHECK(removedField);
    CHECK(!captureAvailable(*registry.snapshot(),removedField.token,"window.main",true));
    CHECK(captureAvailable(*registry.snapshot(),removedField.token,"window.disabled",true));
}
void optionsBodyPasses(){
    using nimby::platform::windows::mod_options::OptionsBodyLayout;
    using A=OptionsBodyLayout::Action;
    OptionsBodyLayout normal;
    CHECK(normal.prepare(true,true)==A::Sdk);
    CHECK(!normal.native()&&!normal.failed());
    normal.beginSdk();
    CHECK(normal.prepare(false,true)==A::Sdk);
    CHECK(!normal.native()&&!normal.failed());

    // A bad layout emitter falls back before creating any SDK declarations.
    // A now-valid interactive emitter must keep the native layout body.
    OptionsBodyLayout layoutInvalid;
    CHECK(layoutInvalid.prepare(true,false)==A::Native);
    CHECK(layoutInvalid.native()&&layoutInvalid.failed());
    layoutInvalid.beginSdk(); // a late callback cannot revive a failed tree
    CHECK(layoutInvalid.prepare(false,true)==A::Native);
    CHECK(layoutInvalid.prepare(false,false)==A::Native);

    // Snapshot/translation preparation may throw before the first declaration;
    // native fallback remains safe, and is frozen for the interaction pass.
    OptionsBodyLayout preparationFailed;
    CHECK(preparationFailed.prepare(true,true)==A::Sdk);
    CHECK(preparationFailed.fail()==A::Native);
    CHECK(preparationFailed.prepare(false,true)==A::Native);
    CHECK(preparationFailed.native()&&preparationFailed.failed());

    // Once SDK layout was emitted, failed validation in interaction cannot
    // substitute native Interface widgets into that different layout tree.
    OptionsBodyLayout interactionInvalid;
    CHECK(interactionInvalid.prepare(true,true)==A::Sdk);
    interactionInvalid.beginSdk();
    CHECK(interactionInvalid.prepare(false,false)==A::Skip);
    CHECK(interactionInvalid.failed()&&!interactionInvalid.native());
    interactionInvalid.beginSdk();
    CHECK(interactionInvalid.prepare(false,true)==A::Skip);
    CHECK(interactionInvalid.fail()==A::Skip);

    // Exceptions after the first SDK declaration suppress both the remaining
    // body and interaction. An interactive exception has the same constraint.
    OptionsBodyLayout layoutException;
    CHECK(layoutException.prepare(true,true)==A::Sdk);
    layoutException.beginSdk();
    CHECK(layoutException.fail()==A::Skip);
    CHECK(layoutException.prepare(false,true)==A::Skip);
    CHECK(layoutException.failed()&&!layoutException.native());
    CHECK(normal.fail()==A::Skip);
    CHECK(normal.prepare(false,true)==A::Skip);

    // Never infer a usable SDK tree from an interaction-only callback, nor
    // from layout validation that stopped before emitting any declarations.
    OptionsBodyLayout interactionOnly;
    CHECK(interactionOnly.prepare(false,true)==A::Native);
    CHECK(interactionOnly.native()&&interactionOnly.failed());
    OptionsBodyLayout noDeclarations;
    CHECK(noDeclarations.prepare(true,true)==A::Sdk);
    CHECK(noDeclarations.prepare(false,true)==A::Native);
    CHECK(noDeclarations.native()&&noDeclarations.failed());
}
void optionsCategories(){
    using namespace nimby::runtime::mod_options;
    using nimby::platform::windows::mod_options::OptionsCategories;
    using nimby::platform::windows::mod_options::OptionsPage;
    using nimby::platform::windows::mod_options::captureAvailable;
    Registry registry;
    auto categories=OptionsCategories::from(*registry.snapshot());
    CHECK(!categories.hasOptions&&!categories.hasShortcuts&&!categories.navigation());
    CHECK(categories.select(OptionsPage::Interface)==OptionsPage::Shortcuts);
    CHECK(categories.select(OptionsPage::Shortcuts)==OptionsPage::Shortcuts);

    const auto shortcuts=registry.add(1,{"windowMod","Window mod",{
        {"window.main","Open window",{},Kind::Shortcut,"F9",0,0,{}}}});CHECK(shortcuts);
    categories=OptionsCategories::from(*registry.snapshot());
    CHECK(!categories.hasOptions&&categories.hasShortcuts&&!categories.navigation());
    CHECK(categories.select(OptionsPage::Interface)==OptionsPage::Shortcuts);
    CHECK(categories.select(OptionsPage::Shortcuts)==OptionsPage::Shortcuts);

    const auto preferences=registry.add(2,{"preferenceMod","Preference mod",{
        {"enabled","Enabled",{},Kind::Boolean,"true",0,0,{}},
        {"count","Count",{},Kind::Integer,"2",1,5,{}},
        {"order","Order",{},Kind::Choice,"first",0,0,{{"first","First"},{"last","Last"}}}}});CHECK(preferences);
    const auto frozenSnapshot=registry.snapshot();
    const auto frozenCategories=OptionsCategories::from(*frozenSnapshot);
    CHECK(frozenCategories.hasOptions&&frozenCategories.hasShortcuts&&frozenCategories.navigation());
    CHECK(frozenCategories.select(OptionsPage::Interface)==OptionsPage::Interface);
    CHECK(frozenCategories.select(OptionsPage::Shortcuts)==OptionsPage::Shortcuts);
    CHECK(captureAvailable(*frozenSnapshot,shortcuts.token,"window.main",true));

    // Unloading a preferences-only mod cannot change the already prepared
    // interaction tree or cancel capture owned by the surviving window mod.
    CHECK(registry.remove(preferences.token));
    CHECK(frozenSnapshot->mods.size()==2);
    CHECK(OptionsCategories::from(*frozenSnapshot).navigation());
    CHECK(frozenCategories.select(OptionsPage::Interface)==OptionsPage::Interface);
    const auto withoutPreferences=registry.snapshot();
    categories=OptionsCategories::from(*withoutPreferences);
    CHECK(!categories.navigation()&&categories.hasShortcuts&&!categories.hasOptions);
    CHECK(categories.select(OptionsPage::Interface)==OptionsPage::Shortcuts);
    CHECK(captureAvailable(*withoutPreferences,shortcuts.token,"window.main",
        categories.select(OptionsPage::Shortcuts)==OptionsPage::Shortcuts));

    // A different mod keeps the page alive after the captured owner unloads.
    // The next layout is Interface-only; current identity also rejects capture
    // immediately even if an older frozen frame still displays Shortcuts.
    const auto replacement=registry.add(3,{"replacementMod","Replacement",{
        {"enabled","Enabled",{},Kind::Boolean,"false",0,0,{}}}});CHECK(replacement);
    CHECK(registry.remove(shortcuts.token));
    const auto withoutCapturedOwner=registry.snapshot();
    categories=OptionsCategories::from(*withoutCapturedOwner);
    CHECK(categories.hasOptions&&!categories.hasShortcuts&&!categories.navigation());
    CHECK(categories.select(OptionsPage::Shortcuts)==OptionsPage::Interface);
    CHECK(categories.select(OptionsPage::Interface)==OptionsPage::Interface);
    CHECK(!captureAvailable(*withoutCapturedOwner,shortcuts.token,"window.main",true));
    CHECK(!captureAvailable(*withoutCapturedOwner,shortcuts.token,"window.main",
        categories.select(OptionsPage::Shortcuts)==OptionsPage::Shortcuts));
}
}
int main(){activeAssignments();rejectUnknown();keyboard();captureOwnership();captureLifecycle();optionsBodyPasses();optionsCategories();std::cout<<"PASS: active native assignments, unavailable snapshots, exact SDL mapping, captured key ownership, capture lifecycle, native options body pass consistency and conditional categories\n";}
