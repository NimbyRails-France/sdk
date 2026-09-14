#include "engine/live_state.h"
#include <array>
#include <cstdio>
#include <cstring>
struct Memory {
    std::array<std::array<uint64_t, 2>, 4> cells;
    unsigned reads{};
    bool replace{};
};
bool read(void* context, uint64_t address, void* out, size_t length) {
    auto& memory=*static_cast<Memory*>(context); ++memory.reads;
    if (memory.replace && memory.reads == 5) memory.cells[0][1] += 0x1000;
    if(length!=8) return false;
    for(auto cell:memory.cells) if(cell[0]==address) {std::memcpy(out,&cell[1],8);return true;}
    return false;
}
int main() {
    for(uint64_t relocation : {0ULL,0x12340000ULL}) {
        const uint64_t base=0x140000000ULL+relocation, root=0x200000000ULL+relocation;
        Memory memory{{{{base+0xb81998,root},{root+0x540,root+0x10000},{root+0x5c0,root+0x20000},{root+0x680,root+0x30000}}}};
        nimby::engine::LiveState state{};
        if(!nimby::engine::resolve_live_state(read,&memory,base,true,state) || state.module_base!=base || state.simulation!=root+0x30000 || memory.reads!=8) return 1;
        memory.reads=0;
        if(nimby::engine::resolve_live_state(read,&memory,base,false,state) || memory.reads || state.root) return 2;
        memory.replace=true;
        if(nimby::engine::resolve_live_state(read,&memory,base,true,state) || state.root) return 3;
        memory.replace=false; memory.cells[0][1]=0;
        if(nimby::engine::resolve_live_state(read,&memory,base,true,state)) return 4;
        memory.cells[0][1]=root; memory.cells[3][1]=memory.cells[2][1];
        if(nimby::engine::resolve_live_state(read,&memory,base,true,state)) return 5;
    }
    std::puts("Relocated module and heap, unknown version, replaced/null root and aliasing rejected.");
}
