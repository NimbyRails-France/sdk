// Shared signal-state table reader. Missing entries and an unreadable table
// are distinct outcomes; the caller decides whether a native default is valid.
#include "engine/network.h"
#include "engine/native_string.h"
#include "engine/simulation_clock.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <map>
#include <unordered_set>
#include "engine/detail/memory_reader.h"
namespace nimby::engine {
using memory::field;
using memory::pointer;
using memory::collect;
bool read_signal_texture_states(ReadMemory read,void* context,const LiveState& state,bool recognized,
                                std::vector<SignalTextureState>& out) noexcept {
    out.clear();if(!read||!recognized)return false;
    try {for(int attempt=0;attempt<3;++attempt){
        LiveState current{};uint64_t query{},after{};std::array<uint64_t,4> h{},verify{};
        if(!resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)return false;
        // Native renderer RVA 0x620140: Sim+0x2200 -> query Swiss map +0x378.
        // Slot: full signal ID +0, signed texture selector +8, stride 16.
        if(!read(context,state.simulation+gameLayout(state.profile).texture_query,&query,8)||!pointer(query)||
           !read(context,query+0x378,h.data(),sizeof h))continue;
        const auto ctrl=h[0],slots=h[1],size=h[2],mask=h[3];
        if(mask>1048575||size>mask||(mask&(mask+1))||!pointer(ctrl)||(mask&&!pointer(slots)))continue;
        std::vector<unsigned char> controls(mask),values(mask*16),controls_after(mask),values_after(mask*16);
        if(mask&&(!read(context,ctrl,controls.data(),controls.size())||!read(context,slots,values.data(),values.size())))continue;
        std::vector<SignalTextureState> result;std::unordered_set<uint64_t> ids;bool valid=true;
        for(size_t i=0;i<mask;++i)if(controls[i]<128){
            const auto id=field<uint64_t>(values.data()+i*16,0);
            if((id>>48)!=8||!ids.insert(id).second){valid=false;break;}
            result.push_back({id,field<int32_t>(values.data()+i*16,8)});
        }
        if(!valid||result.size()!=size)continue;
        if(mask&&(!read(context,ctrl,controls_after.data(),controls_after.size())||controls!=controls_after||
                  !read(context,slots,values_after.data(),values_after.size())||values!=values_after))continue;
        if(!read(context,query+0x378,verify.data(),sizeof verify)||h!=verify||
           !read(context,state.simulation+gameLayout(state.profile).texture_query,&after,8)||query!=after||
           !resolve_live_state(read,context,state.module_base,true,state.profile,current)||current!=state)continue;
        out=std::move(result);return true;
    }}catch(...){}return false;
}
}
