// Optional regression against the qualified game's real layout solver. Mapping
// DONT_RESOLVE_DLL_REFERENCES runs no entry point, game loop, UI or desktop input.
#include <windows.h>
#include "engine/binary_identity.h"
#include "platform/windows/game_layout.h"
#include "platform/windows/mod_options_layout.h"
#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

#define CHECK(x) do{if(!(x))throw std::runtime_error("Options geometry line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
using Geometry=nimby::platform::windows::mod_options::OptionsPanelGeometry;
using Calculate=void(*)(void*);
struct Item {uint32_t flags=0,child=~0u,next=~0u;float left=0,top=0,right=0,bottom=0,width=0,height=0;};
struct Rect {float x=0,y=0,width=0,height=0;};
static_assert(sizeof(Item)==0x24&&sizeof(Rect)==0x10);
template<class T>void put(void* p,size_t offset,T value){std::memcpy(static_cast<char*>(p)+offset,&value,sizeof value);}
bool approximatelyEqual(float a,float b){return std::abs(a-b)<0.05f;}

struct Tree {
    std::array<Item,8> items{};
    std::array<Rect,8> rects{};
    std::array<unsigned char,0x128> native{};
    size_t viewportIndex;
    Tree(float scale,bool fixed,bool navigation=true):viewportIndex(navigation?7:5){
        // Native Options outer row, sidebar and right-hand body. The native
        // wrapper owns these first three nodes; NRF owns a column inside it.
        items[0].flags=2|0x1800;items[0].child=1;
        items[0].width=620;items[0].height=700;
        items[1].flags=3|0x140|0x800|0x400;items[1].next=2;items[1].width=200;items[1].right=20;
        items[2].flags=0x1e0|0x400;items[2].child=3;
        auto& column=items[3];column.child=4;column.flags=0x400|0x800;
        column.flags|=fixed?Geometry::columnFlow|Geometry::columnAlign|0x1000:3|0xa0;
        column.width=Geometry::width;column.height=fixed?Geometry::height:0;
        for(size_t i=4;i<=viewportIndex;++i){
            items[i].flags=0x400|0x1000;
            items[i].next=i<viewportIndex?uint32_t(i+1):~0u;
        }
        items[4].height=24;items[4].left=10;items[4].right=10;items[4].top=8;items[4].bottom=4;
        if(navigation){
            items[5].height=30;items[5].bottom=4;items[5].flags|=0xa0;
            items[6].height=30;items[6].bottom=4;items[6].flags|=0xa0;
        }
        items[viewportIndex].height=Geometry::viewport(navigation);
        if(fixed){items[4].flags|=Geometry::rowAlign;items[viewportIndex].flags|=0xa0;}
        // Native declaration emission scales dimensions before solving.
        for(auto& item:items){
            item.left*=scale;item.top*=scale;item.right*=scale;item.bottom*=scale;
            item.width*=scale;item.height*=scale;
        }
        put(native.data(),0xc0,items.data());put(native.data(),0xc8,rects.data());
        put(native.data(),0xd4,uint32_t(viewportIndex+1));
    }
    void calculate(Calculate call){call(native.data());}
};
void hierarchy(Calculate calculate,float scale,bool navigation){
    Tree broken(scale,false);broken.calculate(calculate);
    CHECK(approximatelyEqual(broken.rects[4].width,0));CHECK(approximatelyEqual(broken.rects[7].width,0));
    CHECK(broken.rects[7].x>broken.rects[3].x+150*scale);

    Tree fixed(scale,true,navigation);fixed.calculate(calculate);
    const auto& r=fixed.rects;
    const auto& viewport=r[fixed.viewportIndex];
    CHECK(approximatelyEqual(r[1].x,0)&&approximatelyEqual(r[1].width,200*scale));
    CHECK(approximatelyEqual(r[2].x,220*scale)&&approximatelyEqual(r[2].width,400*scale));
    CHECK(approximatelyEqual(r[3].x,r[2].x)&&approximatelyEqual(r[3].y,r[2].y));
    CHECK(approximatelyEqual(r[3].height,700*scale));
    CHECK(approximatelyEqual(r[4].width,380*scale)&&approximatelyEqual(r[4].y,r[3].y+8*scale));
    if(navigation){
        CHECK(approximatelyEqual(r[5].y,r[4].y+r[4].height+4*scale));
        CHECK(approximatelyEqual(r[6].y,r[5].y+r[5].height+4*scale));
        CHECK(approximatelyEqual(viewport.y,r[6].y+r[6].height+4*scale));
    }else{
        // A single category goes straight from its title to content: there
        // are no hidden button rows leaving empty space above the shortcuts.
        CHECK(approximatelyEqual(viewport.y,r[4].y+r[4].height+4*scale));
        Tree tabs(scale,true,true);tabs.calculate(calculate);
        CHECK(approximatelyEqual(viewport.height-tabs.rects[7].height,68*scale));
        CHECK(approximatelyEqual(viewport.y+viewport.height,tabs.rects[7].y+tabs.rects[7].height));
    }
    CHECK(approximatelyEqual(viewport.x,r[3].x)&&approximatelyEqual(viewport.width,r[3].width));
    CHECK(viewport.height>500*scale&&viewport.y+viewport.height<=r[3].y+r[3].height);

    // Also test justification with spare space rather than only a tightly
    // packed auto-height column: flow3 centers, flow0xb starts at the top.
    Tree centered(scale,true,navigation);centered.items[3].flags=(centered.items[3].flags&~0x1fu)|3;
    centered.calculate(calculate);
    CHECK(centered.rects[4].y>r[4].y+10*scale);
    std::cout<<"PASS: native shell/sidebar/NRF column scale="<<scale<<" navigation="<<navigation
             <<" heading="<<r[4].x<<','<<r[4].y<<','<<r[4].width
             <<" viewport="<<viewport.x<<','<<viewport.y<<','<<viewport.width<<','<<viewport.height<<'\n';
}
}
int main(){try{
    std::array<wchar_t,32768> path{};
    const auto size=GetEnvironmentVariableW(L"NIMBY_TEST_GAME_EXE",path.data(),DWORD(path.size()));
    if(!size){std::cout<<"SKIP: set NIMBY_TEST_GAME_EXE to the recognized Windows executable\n";return 77;}
    CHECK(size<path.size());
    NimbyBinaryInfo binary{};CHECK(nimby::engine::identify(path.data(),binary)==NIMBY_OK&&binary.recognized_research_build);
    const auto image=LoadLibraryExW(path.data(),nullptr,DONT_RESOLVE_DLL_REFERENCES);CHECK(image);
    struct Unmap {HMODULE image;~Unmap(){FreeLibrary(image);}} unmap{image};
    const auto calculate=reinterpret_cast<Calculate>(reinterpret_cast<char*>(image)+nimby::platform::windows::game119.ui_layout_calculate);
    for(float scale:{1.f,1.25f,1.5f})for(bool navigation:{false,true})hierarchy(calculate,scale,navigation);
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
