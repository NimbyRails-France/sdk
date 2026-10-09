#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace nimby::engine::mod_shortcuts {
// Platform-independent keys. A native input adapter translates the game's
// keyboard events once; mods never register operating-system hotkeys.
enum class Key : uint16_t {
    None=0,
    D0='0',D1,D2,D3,D4,D5,D6,D7,D8,D9,
    A='A',B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
    F1=0x100,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
    F13,F14,F15,F16,F17,F18,F19,F20,F21,F22,F23,F24,
    Enter=0x120,Space,Tab,Escape,Backspace,Delete,Insert,Home,End,
    PageUp,PageDown,Left,Right,Up,Down,
};
enum Modifier : uint8_t { Control=1,Alt=2,Shift=4 };
struct Chord {
    Key key=Key::None;
    uint8_t modifiers=0;
    bool operator==(const Chord&)const=default;
    explicit operator bool()const noexcept{return key!=Key::None;}
};
struct Event {Key key=Key::None;uint8_t modifiers=0;bool repeat=false;};
struct Context {bool gameForeground=false,textInput=false,capturing=false;};

namespace detail {
struct NamedKey {std::string_view name;Key key;};
inline constexpr std::array names{
    NamedKey{"Enter",Key::Enter},NamedKey{"Space",Key::Space},
    NamedKey{"Tab",Key::Tab},NamedKey{"Escape",Key::Escape},
    NamedKey{"Backspace",Key::Backspace},NamedKey{"Delete",Key::Delete},
    NamedKey{"Insert",Key::Insert},NamedKey{"Home",Key::Home},
    NamedKey{"End",Key::End},NamedKey{"PageUp",Key::PageUp},
    NamedKey{"PageDown",Key::PageDown},NamedKey{"Left",Key::Left},
    NamedKey{"Right",Key::Right},NamedKey{"Up",Key::Up},NamedKey{"Down",Key::Down},
};
constexpr bool between(Key key,Key first,Key last)noexcept {
    return uint16_t(key)>=uint16_t(first)&&uint16_t(key)<=uint16_t(last);
}
}

inline bool valid(Chord chord)noexcept {
    if(chord.modifiers&~uint8_t(Control|Alt|Shift))return false;
    if(chord.key==Key::None)return !chord.modifiers;
    return detail::between(chord.key,Key::A,Key::Z)||detail::between(chord.key,Key::D0,Key::D9)||
        detail::between(chord.key,Key::F1,Key::F24)||detail::between(chord.key,Key::Enter,Key::Down);
}

// The wire and persisted representation is canonical: Ctrl, Alt, Shift in
// that order, followed by one key. Empty means deliberately unassigned.
// Reject aliases, duplicated modifiers and extra characters before storage.
inline std::optional<Chord> parse(std::string_view text)noexcept {
    if(text.empty())return Chord{};
    Chord chord;
    if(text.starts_with("Ctrl+")){chord.modifiers|=Control;text.remove_prefix(5);}
    if(text.starts_with("Alt+")){chord.modifiers|=Alt;text.remove_prefix(4);}
    if(text.starts_with("Shift+")){chord.modifiers|=Shift;text.remove_prefix(6);}
    if(text.size()==1&&((text[0]>='A'&&text[0]<='Z')||(text[0]>='0'&&text[0]<='9'))){
        chord.key=Key(text[0]);return chord;
    }
    if(text.size()>=2&&text.size()<=3&&text.front()=='F'&&text[1]>='1'&&text[1]<='9'){
        unsigned number=unsigned(text[1]-'0');
        if(text.size()==3){if(text[2]<'0'||text[2]>'9')return {};number=number*10+unsigned(text[2]-'0');}
        if(number<=24){chord.key=Key(uint16_t(Key::F1)+number-1);return chord;}
    }
    for(const auto& named:detail::names)if(text==named.name){chord.key=named.key;return chord;}
    return {};
}

inline std::string format(Chord chord){
    if(!valid(chord)||!chord)return {};
    std::string text;
    if(chord.modifiers&Control)text+="Ctrl+";
    if(chord.modifiers&Alt)text+="Alt+";
    if(chord.modifiers&Shift)text+="Shift+";
    if(detail::between(chord.key,Key::A,Key::Z)||detail::between(chord.key,Key::D0,Key::D9))text+=char(chord.key);
    else if(detail::between(chord.key,Key::F1,Key::F24))text+='F'+std::to_string(uint16_t(chord.key)-uint16_t(Key::F1)+1);
    else for(const auto& named:detail::names)if(chord.key==named.key){text+=named.name;break;}
    return text;
}

// A match does no allocation, polling, callback or shared-registry access.
// The caller applies its owner/session lease before queuing the action.
inline bool matches(Chord chord,const Event& event,const Context& context)noexcept {
    return context.gameForeground&&!context.textInput&&!context.capturing&&!event.repeat&&
        bool(chord)&&valid(chord)&&chord.key==event.key&&chord.modifiers==event.modifiers;
}
}
