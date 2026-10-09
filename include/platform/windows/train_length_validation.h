#pragma once
#include <platform/windows/train_length_native.h>
#include <algorithm>

namespace nimby::platform::windows::train_length::validation {
struct RecompositionApproval {
    uint64_t rules{};
    double maximumMeters{};
    std::span<const Car> cars;
    bool preflightAccepted=false;
};

// A model command validates a temporary Train internally, so its address cannot
// identify the already-approved candidate. The command may lend this descriptor
// only across that synchronous native call. Match all cars, including active
// configuration masks, and the same policy snapshot; never waive coupler checks.
inline bool matchesApproval(const RecompositionApproval& approval,uint64_t rules,
                            policy::Limit limit,std::span<const Car> candidate)noexcept {
    if(!approval.preflightAccepted||!rules||approval.rules!=rules||
       !policy::validMaximum(limit.maximumMeters)||approval.maximumMeters!=limit.maximumMeters||
       approval.cars.size()>policy::maximumElements||candidate.size()!=approval.cars.size())return false;
    return std::equal(candidate.begin(),candidate.end(),approval.cars.begin(),
        [](const Car& a,const Car& b){return a.model==b.model&&a.train==b.train&&a.settings==b.settings;});
}

struct Recovery {
    uint32_t code=1;
    bool recovered=false;
    size_t flagBytes{};
    ReadIssue readIssue{};
};

// Native validation writes two one-byte coupler flags per vehicle, then replaces
// its incompatibility result (4) with the old vehicle-count result (1). Recover
// only that specific count result, after the metre policy has accepted the same
// complete candidate. This reader never changes the game's result or allocation.
// Any incomplete, unreadable or inconsistent flag vector preserves code 1.
inline Recovery recoverCountLimit(const View& view,uint64_t output,size_t cars)noexcept {
    Recovery result;
    const auto finish=[&](){result.readIssue=view.issue();return result;};
    if(!view.get(output,result.code))return finish();
    if(result.code!=1||cars<=30)return finish();
    if(cars>policy::maximumElements){
        view.reject(ReadFault::ElementBudget,output,cars);return finish();
    }
    std::array<uint64_t,3> flags{};
    if(!view.get(output+8,flags))return finish();
    const size_t expected=cars*2;
    if(flags[0]<0x10000||flags[1]<flags[0]||flags[2]<flags[1]||
       flags[2]>0x7fffffff0000ULL||flags[1]-flags[0]!=expected||
       (flags[2]-flags[0])%2){
        view.reject(ReadFault::VectorLayout,output+8,expected);return finish();
    }
    std::array<uint8_t,policy::maximumElements*2> bytes{};
    if(!view.bytes(flags[0],bytes.data(),expected))return finish();
    result.flagBytes=expected;
    result.code=0;
    for(size_t i=0;i<expected;++i)if(!bytes[i]){result.code=4;break;}
    result.recovered=true;
    return finish();
}
}
