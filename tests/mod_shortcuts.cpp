#include <engine/mod_shortcuts.h>
#include <cassert>
#include <iostream>

namespace shortcut=nimby::engine::mod_shortcuts;

int main(){
    using namespace shortcut;
    const auto disabled=parse("");assert(disabled&&!*disabled&&format(*disabled).empty());
    const auto clock=parse("Ctrl+Shift+T");assert(clock&&clock->key==Key::T&&clock->modifiers==(Control|Shift));
    const auto f8=parse("F8");assert(f8&&f8->key==Key::F8&&!f8->modifiers);
    for(const auto prefix:{"","Ctrl+","Alt+","Shift+","Ctrl+Alt+","Ctrl+Shift+","Alt+Shift+","Ctrl+Alt+Shift+"}){
        for(char key='A';key<='Z';++key){const std::string text=std::string(prefix)+key;const auto chord=parse(text);assert(chord&&format(*chord)==text);}
        for(char key='0';key<='9';++key){const std::string text=std::string(prefix)+key;const auto chord=parse(text);assert(chord&&format(*chord)==text);}
        for(unsigned key=1;key<=24;++key){const auto text=std::string(prefix)+"F"+std::to_string(key);const auto chord=parse(text);assert(chord&&format(*chord)==text);}
        for(const auto name:{"Backspace","Tab","Enter","Escape","Space","Left","Right","Up","Down","Home","End","PageUp","PageDown","Insert","Delete"}){
            const auto text=std::string(prefix)+name;const auto chord=parse(text);assert(chord&&format(*chord)==text);
        }
    }
    for(const auto text:{" ","F0","F00","F01","F25","F100","f8","t","Ctrl+","Ctrl","Ctrl+Ctrl+T",
            "Shift+Ctrl+T","Alt+Ctrl+T","Ctrl+Shift+Alt+T","Ctrl++T","Ctrl+T+T","Ctrl+Shift+t",
            "Ctrl +T"," Ctrl+T","Ctrl+T ","Ctrl+EscapeX","Win+T","Meta+T","Ctrl+Alt+Shift+"})assert(!parse(text));
    assert(!parse(std::string_view("F8\0",3)));
    assert(!valid({Key::None,Control})&&!valid({Key::T,8})&&!valid({Key(0xffff),0}));
    const Context game{true,false,false};
    assert(matches(*clock,{Key::T,uint8_t(Control|Shift),false},game));
    assert(matches(*f8,{Key::F8,0,false},game));
    assert(!matches(*f8,{Key::F8,0,true},game));
    assert(!matches(*f8,{Key::F8,0,false},{false,false,false}));
    assert(!matches(*f8,{Key::F8,0,false},{true,true,false}));
    assert(!matches(*f8,{Key::F8,0,false},{true,false,true}));
    assert(!matches(*clock,{Key::T,Control,false},game));
    assert(!matches(*clock,{Key::T,uint8_t(Control|Alt|Shift),false},game));
    assert(!matches(*clock,{Key::A,uint8_t(Control|Shift),false},game));
    assert(!matches(*disabled,{Key::None,0,false},game));
    // Canonical chords are directly comparable by a registry's conflict index.
    assert(*clock==*parse("Ctrl+Shift+T")&&*clock!=*parse("Ctrl+T"));
    std::cout<<"PASS: canonical mod shortcuts, disabled binding, exact modifiers and input guards\n";
}
