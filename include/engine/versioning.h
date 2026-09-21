#pragma once
#include "engine/live_state.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <vector>

namespace nimby::engine {
// Observed save metadata, not yet a guarantee of identity across load/Save As.
// See docs/research/save-identity.md. Never use a heap address as a save key.
struct VersioningObservation {
    using Value=std::array<unsigned char,32>;
    LiveState state;
    Value value{};
    std::vector<Value> history;
};
inline bool read_versioning_observation(ReadMemory read,void* context,uint64_t module,
                                        bool recognized,VersioningObservation& out,
                                        LiveStateProfile profile=LiveStateProfile::Windows119) {
    out={};
    LiveState before{},after{};
    if(!resolve_live_state(read,context,module,recognized,profile,before))return false;
    const auto offset=profile==LiveStateProfile::Linux119?0xa40:0xa48;
    constexpr uint64_t limit=0x7fffffff0000ULL,maxEntries=4096;
    if(before.database>limit-0xa80)return false;
    std::array<unsigned char,0x38> first{},second{};
    if(!read(context,before.database+offset,first.data(),first.size()))return false;
    VersioningObservation candidate;candidate.state=before;
    std::copy_n(first.begin(),32,candidate.value.begin());
    if(std::all_of(candidate.value.begin(),candidate.value.end(),[](auto byte){return byte==0;}))return false;
    uint64_t begin{},end{},capacity{};
    std::memcpy(&begin,first.data()+0x20,8);
    std::memcpy(&end,first.data()+0x28,8);
    std::memcpy(&capacity,first.data()+0x30,8);
    if(end<begin||capacity<end||(end-begin)%32||(capacity-begin)%32||
       capacity-begin>maxEntries*32)return false;
    if(begin && (begin<0x10000||begin>=limit||capacity>=limit))return false;
    if(!begin && (end||capacity))return false;
    candidate.history.resize(static_cast<size_t>((end-begin)/32));
    const auto bytes=candidate.history.size()*sizeof(VersioningObservation::Value);
    if(bytes&&!read(context,begin,candidate.history.data(),bytes))return false;
    if(!read(context,before.database+offset,second.data(),second.size())||first!=second)return false;
    if(bytes){
        std::vector<VersioningObservation::Value> repeated(candidate.history.size());
        if(!read(context,begin,repeated.data(),bytes)||repeated!=candidate.history)return false;
    }
    if(!resolve_live_state(read,context,module,recognized,profile,after)||before!=after)return false;
    // Repeated reads detect changes but cannot make an external capture atomic.
    out=std::move(candidate);return true;
}
}
