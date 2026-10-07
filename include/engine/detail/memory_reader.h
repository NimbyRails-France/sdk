#pragma once
#include "engine/live_state.h"
#include <array>
#include <cstring>
#include <span>
#include <vector>
namespace nimby::engine::memory {
// Reads target-owned storage through a supplied callback. It never dereferences
// remote pointers locally. Ownership: buffers below belong to one traversal;
// consume must copy values it retains before the next block overwrites bytes.
// The pool header and block list are checked again after traversal; this detects
// replacement but does not make an external observation atomic.
template<class T> T field(const void* p,size_t off) { T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+off,sizeof v);return v; }
struct Collection { std::array<unsigned char,48> header{}; std::vector<uint64_t> blocks; };
inline bool pointer(uint64_t p) { return p>=0x10000 && p<0x7fffffff0000ULL && p%8==0; }
struct IgnoreBlock { void operator()(std::span<const unsigned char>,uint64_t) const {} };
// Optional block preparation supports batched secondary reads. The span is
// owned by this traversal and remains valid only until the next block.
template<class F,class B=IgnoreBlock> bool collect(ReadMemory read,void* ctx,uint64_t address,uint64_t tag,size_t stride,F consume,B beforeBlock={}) {
    Collection pool;
    if(!read(ctx,address,pool.header.data(),48)) return false;
    const auto shift=field<uint32_t>(pool.header.data(),4),size=field<uint32_t>(pool.header.data(),8),mask=field<uint32_t>(pool.header.data(),16);
    const auto begin=field<uint64_t>(pool.header.data(),24),end=field<uint64_t>(pool.header.data(),32),cap=field<uint64_t>(pool.header.data(),40);
    if(shift==0||shift>16||size!=(1u<<shift)||mask!=size-1||end<begin||cap<end||cap-begin>8192||(end-begin)%8) return false;
    if(end!=begin&&!pointer(begin)) return false;
    pool.blocks.resize((end-begin)/8);
    // Capacity includes unused slots. Large saves observed with 486 * 512 slots.
    // Bound allocation/work independently of active object count; read one block at a time.
    if(pool.blocks.size()*size>1048576 || (!pool.blocks.empty()&&!read(ctx,begin,pool.blocks.data(),pool.blocks.size()*8))) return false;
    std::vector<unsigned char> bytes(size*stride);
    for(size_t b=0;b<pool.blocks.size();++b) {
        if(!pointer(pool.blocks[b])||!read(ctx,pool.blocks[b],bytes.data(),bytes.size())) return false;
        beforeBlock(std::span<const unsigned char>(bytes),pool.blocks[b]);
        for(size_t slot=0;slot<size;++slot) {
            const auto* p=bytes.data()+slot*stride; const auto id=field<uint64_t>(p,0);
            if((id>>48)==0||(id>>48)==0xffff) continue;
            if((id>>48)!=tag||((id>>16)&0xffffffffULL)!=(b<<shift)+slot||!consume(p,pool.blocks[b]+slot*stride)) return false;
        }
    }
    Collection after;after.blocks.resize(pool.blocks.size());
    return read(ctx,address,after.header.data(),48)&&after.header==pool.header&&
        (after.blocks.empty()||(read(ctx,begin,after.blocks.data(),after.blocks.size()*8)&&after.blocks==pool.blocks));
}
}
