#pragma once
#include "engine/detail/memory_reader.h"
#include "engine/mod_shortcuts.h"
#include <algorithm>
#include <charconv>
#include <optional>
#include <string>
#include <vector>

namespace nimby::platform::windows {
// Qualified Windows 1.19.10 only. This is the ACTIVE config tree read by
// FUN_726750, not a list of factory defaults or an independently read file.
// Call on the native UI thread between game callbacks: replacement checks
// diagnose invalid observations, but do not turn foreign concurrent reads into
// a transaction. A pending native reload is deliberately unavailable.
struct GameKeyBinding {
    std::string action;
    uint32_t key=0;
    bool operator==(const GameKeyBinding&)const=default;
};
inline std::optional<std::vector<GameKeyBinding>> readGameKeyBindings(
        engine::ReadMemory read,void* context,uint64_t module) {
    using namespace engine::memory;
    constexpr uint64_t tableRva=0xb5d940,reloadRva=0xb5b0f8,initializedRva=0xb819d0;
    if(!read||!pointer(module)||module>0x7fffffff0000ULL-initializedRva-8)return {};
    uint8_t initialized{},reload{};
    if(!read(context,module+initializedRva,&initialized,1)||initialized!=1||
       !read(context,module+reloadRva,&reload,1)||reload)return {};
    const auto table=module+tableRva;
    std::array<unsigned char,40> header{},after{};
    if(!read(context,table,header.data(),header.size()))return {};
    const auto root=field<uint64_t>(header.data(),16),count=field<uint64_t>(header.data(),32);
    if(!count||count>2048||!pointer(root))return {};
    struct Node {uint64_t address,parent;std::array<unsigned char,96> bytes;};
    std::vector<Node> nodes;nodes.reserve(size_t(count));
    std::vector<std::pair<uint64_t,uint64_t>> pending{{root,table}};
    std::vector<GameKeyBinding> bindings;
    auto string=[&](uint64_t at,const unsigned char* metadata,size_t limit,std::string& out){
        const auto size=field<uint64_t>(metadata,16),capacity=field<uint64_t>(metadata,24);
        if(size>limit||capacity<size||capacity>1048576||(capacity<16&&size>=16))return false;
        const auto address=capacity<16?at:field<uint64_t>(metadata,0);
        if(!pointer(address))return false;
        out.resize(size_t(size));
        return !size||read(context,address,out.data(),out.size());
    };
    while(!pending.empty()){
        const auto [at,parent]=pending.back();pending.pop_back();
        if(!pointer(at)||at==table||nodes.size()>=count||
           std::any_of(nodes.begin(),nodes.end(),[&](const Node& node){return node.address==at;}))return {};
        Node node{at,parent,{}};
        if(!read(context,at,node.bytes.data(),node.bytes.size())||field<uint64_t>(node.bytes.data(),16)!=parent)return {};
        std::string name;
        if(!string(at+32,node.bytes.data()+32,128,name))return {};
        if(name.starts_with("kb1_")||name.starts_with("kb2_")){
            auto action=name.substr(4);
            if(action.empty()||!std::all_of(action.begin(),action.end(),[](unsigned char c){return (c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_';}))return {};
            std::string value;
            if(!string(at+64,node.bytes.data()+64,10,value))return {};
            uint32_t key{};const auto parsed=std::from_chars(value.data(),value.data()+value.size(),key);
            if(value.empty()||parsed.ec!=std::errc{}||parsed.ptr!=value.data()+value.size())return {};
            if(key)bindings.push_back({std::move(action),key});
        }
        for(size_t offset:{size_t(0),size_t(8)})if(const auto child=field<uint64_t>(node.bytes.data(),offset))pending.emplace_back(child,at);
        nodes.push_back(std::move(node));
    }
    if(nodes.size()!=count)return {};
    // Recheck owned string metadata and tree links before accepting a snapshot.
    for(const auto& node:nodes){std::array<unsigned char,96> again{};
        if(!read(context,node.address,again.data(),again.size())||again!=node.bytes)return {};}
    if(!read(context,table,after.data(),after.size())||header!=after||
       !read(context,module+reloadRva,&reload,1)||reload||
       !read(context,module+initializedRva,&initialized,1)||initialized!=1)return {};
    std::sort(bindings.begin(),bindings.end(),[](const auto& a,const auto& b){return a.action<b.action||(a.action==b.action&&a.key<b.key);});
    bindings.erase(std::unique(bindings.begin(),bindings.end()),bindings.end());
    return bindings;
}

// SDL3's event key is a layout-aware SDL_Keycode. Modifier bits must be taken
// from the SAME SDL_KeyboardEvent; the game's frame modifier flags are sampled
// before its event pump and may describe the preceding frame.
inline std::optional<engine::mod_shortcuts::Key> gameShortcutKey(uint32_t key)noexcept {
    using Key=engine::mod_shortcuts::Key;
    if(key>='a'&&key<='z')return Key(key-'a'+'A');
    if((key>='A'&&key<='Z')||(key>='0'&&key<='9'))return Key(key);
    if(key>=0x4000003a&&key<=0x40000045)return Key(uint16_t(Key::F1)+key-0x4000003a);
    if(key>=0x40000068&&key<=0x40000073)return Key(uint16_t(Key::F13)+key-0x40000068);
    switch(key){
        case 13:return Key::Enter;case 32:return Key::Space;case 9:return Key::Tab;
        case 27:return Key::Escape;case 8:return Key::Backspace;case 127:return Key::Delete;
        case 0x40000049:return Key::Insert;case 0x4000004a:return Key::Home;
        case 0x4000004d:return Key::End;case 0x4000004b:return Key::PageUp;case 0x4000004e:return Key::PageDown;
        case 0x40000050:return Key::Left;case 0x4000004f:return Key::Right;
        case 0x40000052:return Key::Up;case 0x40000051:return Key::Down;
        default:return {};
    }
}
inline std::optional<uint8_t> gameShortcutModifiers(uint16_t modifiers)noexcept {
    // GUI, AltGr and level-5 chords are not in the mod shortcut grammar; do
    // not reinterpret them as Ctrl+Alt or accidentally dispatch plain keys.
    if(modifiers&(0x0c00|0x4000|0x0004))return {};
    using namespace engine::mod_shortcuts;
    return uint8_t((modifiers&0x00c0?Control:0)|(modifiers&0x0300?Alt:0)|(modifiers&0x0003?Shift:0));
}
}
