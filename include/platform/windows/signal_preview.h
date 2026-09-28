#pragma once
#include "engine/detail/memory_reader.h"
#include <nimby/detail/signal_ui_bridge.h>
#include <span>

namespace nimby::platform::windows::signal_preview {
// Windows 1.19 renderer ABI, qualified by the executable fingerprint. The
// viewport draw owns all tiles. The normal signal-layer lists are UNSUITABLE:
// 0x61efd9 adds a track only when its existing signal vector is nonempty.
inline constexpr uint64_t viewportDrawRva=0x6236d0,temporaryDrawRva=0x6211d0;
inline constexpr unsigned char viewportEntry[]{0x48,0x8b,0xc4,0x4c,0x89,0x40,0x18,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08};
using ViewportDraw=void(*)(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t);
using TemporaryDraw=void(*)(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t);

class BorrowedFrame {
public:
    BorrowedFrame(engine::ReadMemory read,void* context):read_(read),context_(context){}
    template<class T>bool get(uint64_t at,T& value)const{return read_&&read_(context_,at,&value,sizeof value);}
    uint64_t object(uint64_t pool,uint64_t id,size_t stride)const {
        std::array<unsigned char,48> header{};
        if(!get(pool,header))return 0;
        const auto shift=engine::memory::field<uint32_t>(header.data(),4),size=engine::memory::field<uint32_t>(header.data(),8);
        const auto begin=engine::memory::field<uint64_t>(header.data(),24),end=engine::memory::field<uint64_t>(header.data(),32);
        const auto index=(id>>16)&0xffffffffULL;
        if(!shift||shift>16||size!=(1u<<shift)||engine::memory::field<uint32_t>(header.data(),16)!=size-1||
            !engine::memory::pointer(begin)||end<begin||(end-begin)%8||end-begin>8192||(index>>shift)>=(end-begin)/8)return 0;
        uint64_t block{},actual{};
        if(!get(begin+(index>>shift)*8,block)||!engine::memory::pointer(block))return 0;
        const auto address=block+(index&(size-1))*stride;
        return get(address,actual)&&actual==id?address:0;
    }
    // Same hidden-level tree queried in 0x62414a. Its keys are explicit hidden
    // levels, not tracks with existing signals. Bound traversal on corrupt data.
    bool levelVisible(uint64_t options,int32_t level)const {
        uint64_t node{};if(!get(options+0x48,node))return false;
        for(size_t depth=0;node&&depth<256;++depth){
            int32_t key{};if(!engine::memory::pointer(node)||!get(node+0x20,key))return false;
            if(key==level)return false;
            uint64_t child{};if(!get(node+(key<level?0:8),child))return false;
            node=child;
        }
        return !node;
    }
    // Rendering consumes a non-owning Signal view synchronously. Its strings
    // and filters stay borrowed from the frame's DB; no constructor/destructor
    // is invoked and none of these bytes are written back into a game pool.
    template<class Draw>size_t draw(uint64_t database,uint64_t options,uint64_t source,
            std::span<const NimbyUiPreviewPositionV1> positions,Draw render)const {
        if(positions.empty()||positions.size()>64)return 0;
        std::array<unsigned char,0xc8> model{};
        const auto sourceAt=object(database+0x380,source,model.size());
        if(!sourceAt||!get(sourceAt,model)||engine::memory::field<int32_t>(model.data(),0x30)<0||
            engine::memory::field<int32_t>(model.data(),0x30)>6)return 0;
        // Submit the entire list once per viewport. Native camera clipping
        // handles offscreen geometry; empty tracks need no persistent signal.
        size_t drawn=0;
        for(const auto& p:positions){
            const auto track=object(database,p.track,0x4e8);int32_t trackLevel{};uint8_t orientation{};
            if(!track||!get(track+0x28,trackLevel)||!levelVisible(options,trackLevel)||!get(track+0x2c,orientation))continue;
            auto ghost=model;
            const auto put=[&]<class T>(size_t offset,T value){std::memcpy(ghost.data()+offset,&value,sizeof value);};
            // Match NewSignalEditor's temporary object: no persistent ID and
            // blueprint flag set. Fraction/direction are the mod's inputs.
            put(0,uint64_t{0});put(8,uint8_t{1});put(0x40,p.track);put(0x48,p.fraction);
            put(0x50,int8_t(p.direction));put(0x51,orientation);
            render(reinterpret_cast<uint64_t>(ghost.data()));++drawn;
        }
        return drawn;
    }
private:
    engine::ReadMemory read_;void* context_;
};
}
