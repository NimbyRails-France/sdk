#include "platform/windows/signal_preview.h"
#include "runtime/signal_ui_endpoint.h"
#include <iostream>
#include <limits>
#include <stdexcept>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
void positionBytes(){
    // Both travel directions and both destination orientations, including a
    // change of track orientation in one batch. Guard every neighbouring byte.
    std::array<unsigned char,0xe8> command;
    command.fill(0xa5);
    for(int direction:{-1,1})for(uint8_t orientation:{0,1}){
        nimby::platform::windows::writeSignalPosition(command.data()+0x60,
            0x1000000000001ULL,.375,direction,orientation);
        uint64_t track{};double fraction{};int8_t actual{};
        std::memcpy(&track,command.data()+0x60,8);
        std::memcpy(&fraction,command.data()+0x68,8);
        std::memcpy(&actual,command.data()+0x70,1);
        CHECK(track==0x1000000000001ULL&&fraction==.375&&actual==direction);
        CHECK(command[0x71]==orientation);
        for(size_t i=0;i<command.size();++i)
            if(i<0x60||i>=0x72)CHECK(command[i]==0xa5);
    }
}
constexpr uint64_t source=0x8000000000001,track=0x1000000000001;
struct Memory {
    static constexpr uint64_t base=0x100000;
    std::array<unsigned char,0x7000> bytes{};
    template<class T>void put(size_t offset,T value){std::memcpy(bytes.data()+offset,&value,sizeof value);}
    static bool read(void* context,uint64_t at,void* out,size_t size){
        const auto& self=*static_cast<Memory*>(context);
        if(at<base||at-base>self.bytes.size()||size>self.bytes.size()-(at-base))return false;
        std::memcpy(out,self.bytes.data()+at-base,size);return true;
    }
    void pool(size_t offset,size_t pointers,size_t block){
        put(offset+4,uint32_t{2});put(offset+8,uint32_t{4});put(offset+16,uint32_t{3});
        put(offset+24,base+pointers);put(offset+32,base+pointers+8);put(pointers,base+block);
    }
    Memory(){
        pool(0,0x800,0x1000);pool(0x380,0x808,0x3000);
        for(size_t i=0;i<3;++i){
            const auto at=0x1000+i*0x4e8;
            put(at,track+i*0x10000);put(at+0x28,int32_t{3});put(at+0x2c,uint8_t(i%2));
        }
        // Only the source track has an existing signal. The other two have
        // empty signal vectors, so the old native tile list excluded them.
        put(0x1140,base+0x4800);put(0x1148,base+0x4808);put(0x4800,source);
        put(0x3000,source);put(0x3030,int32_t{4});put(0x3038,uint64_t{987});
        put(0x3040,track);put(0x3048,.1);put(0x3050,int8_t{1});
        // Render options at +0x5000: the hidden-level tree starts empty.
    }
};
void renderer(){
    Memory memory;const auto before=memory.bytes;
    nimby::platform::windows::signal_preview::BorrowedFrame frame(Memory::read,&memory);
    std::vector<NimbyUiPreviewPositionV1> positions{
        {track,.25,-1,0},{track+0x10000,.5,1,0},{track+0x20000,.75,-1,0}};
    size_t calls=0;
    auto draw=[&](uint64_t p){
        const auto* bytes=reinterpret_cast<const unsigned char*>(p);
        const auto field=[&]<class T>(size_t offset){T value;std::memcpy(&value,bytes+offset,sizeof value);return value;};
        CHECK(field.operator()<uint64_t>(0)==0&&field.operator()<uint8_t>(8)==1);
        CHECK(field.operator()<int32_t>(0x30)==4&&field.operator()<uint64_t>(0x38)==987);
        CHECK(field.operator()<uint64_t>(0x40)==positions[calls].track&&field.operator()<double>(0x48)==positions[calls].fraction);
        CHECK(field.operator()<int8_t>(0x50)==positions[calls].direction&&field.operator()<uint8_t>(0x51)==(calls%2));
        ++calls;
    };
    const auto render=[&](auto callback){return frame.draw(Memory::base,Memory::base+0x5000,source,positions,callback);};
    CHECK(render(draw)==3&&calls==3);
    CHECK(memory.bytes==before); // Rendering leaves the source and every pool byte intact.
    const auto noDraw=[&](uint64_t){throw std::runtime_error("Unexpected ghost");};
    CHECK(frame.draw(Memory::base,Memory::base+0x5000,source+1,positions,noDraw)==0);
    memory.put(0x1000,track+1); // Same pool slot, different generation.
    calls=1;CHECK(render(draw)==2&&calls==3); // Other targets must still render.
    memory.bytes=before;memory.put(0x1028,int32_t{4});
    calls=0;CHECK(render(draw)==3); // Different levels are visible unless hidden.
    memory.put(0x5048,Memory::base+0x5100);memory.put(0x5120,int32_t{4});
    calls=1;CHECK(render(draw)==2&&calls==3); // Hide only level 4.
    CHECK(!frame.levelVisible(Memory::base+0x5000,4));
    CHECK(frame.levelVisible(Memory::base+0x5000,3)&&frame.levelVisible(Memory::base+0x5000,5));
    memory.put(0x5108,Memory::base+0x5200);memory.put(0x5220,int32_t{3});
    CHECK(render(noDraw)==0); // Matching lower-key child is also hidden.
    memory.put(0x5100,Memory::base+0x5300);memory.put(0x5320,int32_t{5});
    CHECK(!frame.levelVisible(Memory::base+0x5000,5));
    memory.put(0x5108,Memory::base+0x5100); // Corrupt cycle must be bounded.
    CHECK(!frame.levelVisible(Memory::base+0x5000,3));
    memory.put(0x5048,uint64_t{1});CHECK(render(noDraw)==0);
    memory.bytes=before;
    positions.clear();for(size_t i=0;i<64;++i)positions.push_back({track,double(i+1)/65,1,0});
    calls=0;
    CHECK(render([&](uint64_t p){double fraction{};std::memcpy(&fraction,reinterpret_cast<void*>(p+0x48),8);
        CHECK(fraction==positions[calls++].fraction);})==64&&calls==64);
    CHECK(memory.bytes==before);
    CHECK(frame.draw(Memory::base,Memory::base+0x5000,source,std::vector<NimbyUiPreviewPositionV1>(65),noDraw)==0);
}
void lifecycle(){
    using namespace nimby::runtime;
    SignalUiEndpoint endpoint;
    NimbyUiPanelV1 panel{};panel.size=sizeof panel;panel.version=1;
    std::strcpy(panel.id,"signals");std::strcpy(panel.title,"Signals");std::strcpy(panel.texture_set,"atlas");
    uint64_t owner{},session{},provider{};
    CHECK(endpoint.add(&panel,&owner)==NIMBY_OK);
    NimbyUiActionV1 action{};std::strcpy(action.id,"repeat");std::strcpy(action.label,"Repeat");
    std::strcpy(action.provider,"placement");std::strcpy(action.service,"repeat.v1");
    CHECK(endpoint.actions(owner,&action,1)==NIMBY_OK);
    CHECK(endpoint.begin(owner,"world",5,&session)==NIMBY_OK);
    CHECK(endpoint.panelContext(owner,session,1)==NIMBY_OK);
    NimbyUiSignalV1 signal{};signal.id=source;std::strcpy(signal.texture_set,"atlas");
    CHECK(endpoint.observe(owner,session,&signal,1)==NIMBY_OK);
    NimbyUiProviderV1 mod{};mod.size=sizeof mod;mod.version=1;mod.count=1;
    std::strcpy(mod.id,"placement");std::strcpy(mod.services[0],"repeat.v1");
    CHECK(endpoint.addProvider(&mod,&provider)==NIMBY_OK);
    CHECK(endpoint.observeProvider(provider,"world",5,1)==NIMBY_OK);
    NimbyUiSignalPreviewV1 wire{};wire.size=sizeof wire;wire.version=1;wire.panel=owner;wire.signal=source;wire.count=1;
    std::strcpy(wire.origin,"repeat");std::strcpy(wire.service,"repeat.v1");wire.positions[0]={track,.5,1,0};
    CHECK(endpoint.publishPreview(provider,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    auto& actions=*endpoint.host.actions;
    CHECK(actions.preview(source)&&!actions.preview(source+1));
    wire.positions[0].fraction=.7;CHECK(actions.preview(source)->positions[0].fraction==.5); // Owned copy.
    auto invalid=wire;invalid.count=65;CHECK(endpoint.publishPreview(provider,&invalid)==NIMBY_INVALID_ARGUMENT);
    invalid=wire;invalid.positions[0].fraction=std::numeric_limits<double>::quiet_NaN();
    CHECK(endpoint.publishPreview(provider,&invalid)==NIMBY_INVALID_ARGUMENT);
    invalid=wire;invalid.positions[0].track=source;CHECK(endpoint.publishPreview(provider,&invalid)==NIMBY_INVALID_ARGUMENT);
    invalid=wire;invalid.positions[0].direction=0;CHECK(endpoint.publishPreview(provider,&invalid)==NIMBY_INVALID_ARGUMENT);
    invalid=wire;std::strcpy(invalid.service,"undeclared");CHECK(endpoint.publishPreview(provider,&invalid)==NIMBY_INVALID_HANDLE);
    CHECK(!actions.preview(source,SignalActions::Clock::now()+std::chrono::seconds(3)));
    CHECK(endpoint.suspendProvider(provider)==NIMBY_OK&&!actions.preview(source));
    CHECK(endpoint.observeProvider(provider,"world",5,2)==NIMBY_OK);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_INVALID_HANDLE); // Old panel generation.
    CHECK(endpoint.observeProvider(provider,"world",5,1)==NIMBY_OK);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    NimbyUiToolPanelV2 tool{};tool.base.size=sizeof tool;tool.base.version=2;tool.base.panel=owner;tool.base.signal=source;
    std::strcpy(tool.base.origin,"repeat");std::strcpy(tool.base.service,"repeat.v1");tool.input_count=1;
    auto& input=tool.inputs[0];std::strcpy(input.id,"spacing");std::strcpy(input.label,"Spacing");input.value=1000;input.minimum=3;input.maximum=100000;input.enabled=1;
    CHECK(endpoint.publishToolPanelV2(provider,&tool)==NIMBY_OK);
    const auto selection=endpoint.host.prepare(1,source).actions.front();
    CHECK(actions.beginNumberEdit(selection)&&!actions.preview(source));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK&&!actions.preview(source)); // Worker cannot resurrect a stale draft.
    CHECK(endpoint.publishToolPanelV2(provider,&tool)==NIMBY_OK&&actions.preview(source));
    invalid=wire;invalid.count=0;CHECK(endpoint.publishPreview(provider,&invalid)==NIMBY_OK&&!actions.preview(source));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    CHECK(endpoint.remove(owner)==NIMBY_OK&&!actions.preview(source));
    CHECK(endpoint.removeProvider(provider)==NIMBY_OK);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_INVALID_HANDLE);
}
}
int main(){try{positionBytes();renderer();lifecycle();std::cout<<"PASS transient signal preview geometry, ownership and lifecycle\n";}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
