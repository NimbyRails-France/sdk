// Optional regression against the fingerprinted game's real text editor.
// The executable is mapped without its entry point, imports or game loop.
// All UI state and input below belong to this test; no desktop input is sent.
#include <windows.h>
#include "engine/binary_identity.h"
#include "platform/windows/game_layout.h"
#include "platform/windows/signal_preview.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#define CHECK(x) do { if(!(x))throw std::runtime_error("Line "+std::to_string(__LINE__)+": " #x); } while(false)
namespace {
template<class T>void put(void* p,size_t offset,T value){std::memcpy(static_cast<char*>(p)+offset,&value,sizeof value);}
float width(uint64_t,float,const char*,int length){return length*8.f;}
using Edit=uint32_t(*)(void*,uint32_t,char*,int*,int,void*);

// Offsets verified in 0x5137d0, 0x513b10, 0x5119a0 and 0x509740 for the
// Windows 1.19 fingerprint. Do not replace these with upstream struct layouts:
// the game's Nuklear fork has different sizes and uses a private row layout.
struct Editor {
    std::array<char,0x5000> context{};
    std::array<char,0x300> window{};
    std::array<char,0x200> panel{};
    std::array<char,0x80> commands{};
    std::array<char,0x20> font{};
    uint32_t scroll=0;
    std::array<char,32> text{};
    int length=4;
    Edit edit;
    explicit Editor(Edit function):edit(function) {
        std::memcpy(text.data(),"1000",4);
        put(context.data(),0x4778,window.data());put(window.data(),0xa8,panel.data());
        put(context.data(),0x258,font.data());put(font.data(),8,16.f);put(font.data(),16,&width);
        // Empty drawing buffer: text measurement runs but commands are not
        // rendered or allocated. The test does not create a native window.
        put(window.data(),0x68,commands.data());put(window.data(),0x78,400.f);put(window.data(),0x7c,300.f);
        put(panel.data(),0x18,&scroll);put(panel.data(),0x20,&scroll);
        put(panel.data(),0x4c,400.f);put(panel.data(),0x50,300.f);
        put(panel.data(),0x70,6);put(panel.data(),0x80,100);
        put(panel.data(),0xa8,180.f);put(panel.data(),0xac,28.f);
        // Focus on field zero, cursor at the end, before the first key event.
        put(window.data(),0x18c,1);select(4,4);
    }
    void select(int begin,int end) {
        put(window.data(),0x194,end);put(window.data(),0x198,begin);put(window.data(),0x19c,end);
    }
    uint32_t frame(uint32_t flags,std::string_view typed={},int key=-1) {
        // Reset only input and per-frame counters, preserving native focus,
        // selection, cursor and edit mode as a real sequence of frames does.
        std::fill_n(context.data(),0x250,0);
        put(window.data(),0x184,0);put(panel.data(),0x74,0);
        CHECK(typed.size()<256);
        if(!typed.empty())std::memcpy(context.data()+0xf0,typed.data(),typed.size());
        put(context.data(),0x1f0,int(typed.size()));
        if(key>=0){put(context.data(),size_t(key)*8,1);put(context.data(),size_t(key)*8+4,1);}
        auto state=edit(context.data(),flags,text.data(),&length,int(text.size()),nullptr);
        CHECK(length>=0&&length<int(text.size()));text[size_t(length)]=0;
        return state;
    }
    std::string value()const{return {text.data(),size_t(length)};}
};
}
int main(){try{
    std::array<wchar_t,32768> path{};
    const auto count=GetEnvironmentVariableW(L"NIMBY_TEST_GAME_EXE",path.data(),DWORD(path.size()));
    if(!count){std::cout<<"SKIP: set NIMBY_TEST_GAME_EXE to the recognized Windows game executable\n";return 77;}
    CHECK(count<path.size());
    NimbyBinaryInfo binary{};
    CHECK(nimby::engine::identify(path.data(),binary)==NIMBY_OK&&binary.recognized_research_build);
    const auto image=LoadLibraryExW(path.data(),nullptr,DONT_RESOLVE_DLL_REFERENCES);CHECK(image);
    struct Unmap{HMODULE image;~Unmap(){FreeLibrary(image);}} unmap{image};
    namespace preview=nimby::platform::windows::signal_preview;
    CHECK(std::memcmp(reinterpret_cast<const char*>(image)+preview::viewportDrawRva,preview::viewportEntry,sizeof preview::viewportEntry)==0);
    const auto edit=reinterpret_cast<Edit>(reinterpret_cast<char*>(image)+nimby::platform::windows::game119.ui_edit_string);

    // This is the reported failure: active caret, no insertion, no deletion.
    Editor old(edit);
    CHECK(old.frame(0x64,"7")&1);CHECK(old.value()=="1000");
    old.frame(0x64,{},6);CHECK(old.value()=="1000");

    // The adapter fixture separately asserts that SignalUi passes 0x264.
    Editor fixed(edit);
    CHECK(fixed.frame(0x264,"7")&1);CHECK(fixed.value()=="10007");
    fixed.frame(0x264,{},6);CHECK(fixed.value()=="1000"); // Backspace.
    for(int i=0;i<4;++i)fixed.frame(0x264,{},6);
    CHECK(fixed.value().empty());
    fixed.frame(0x264);CHECK(fixed.value().empty()); // Empty survives another frame.
    fixed.frame(0x264,"725");CHECK(fixed.value()=="725");
    fixed.select(0,3);fixed.frame(0x264,"900");CHECK(fixed.value()=="900");
    fixed.select(0,3);fixed.frame(0x264,{},3);CHECK(fixed.value().empty()); // Delete selection.
    fixed.frame(0x264,"123");fixed.select(1,1);
    fixed.frame(0x264,{},3);CHECK(fixed.value()=="13"); // Delete at cursor.
    fixed.select(0,2);fixed.frame(0x265,"8",6);CHECK(fixed.value()=="13"); // Read-only wins.
    std::cout<<"PASS: real native editor reproduces old failure; insertion, Backspace to empty, replacement, Delete and read-only work\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
