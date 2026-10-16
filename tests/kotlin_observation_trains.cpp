#include <nimby/detail/kotlin_observation_trains.hpp>
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
namespace transport=nimby::kotlin::observation_trains;
#define CHECK(condition) do {if(!(condition))throw std::runtime_error(#condition);} while(false)
constexpr std::uint64_t train(std::uint64_t ordinal,std::uint64_t generation=1) {
    return (UINT64_C(5)<<48)|(ordinal<<16)|generation;
}
template<class Call> void rejects(Call&& call) {
    bool rejected=false;
    try {call();}catch(const std::invalid_argument&){rejected=true;}
    CHECK(rejected);
}
void copiedGroupsAndFullIdentities() {
    transport::ObservationTrainsBatch batch(4);
    const std::array first{train(1,7),train(1,9)};
    const std::array second{train(8)};
    batch.append({},false);batch.append(first,true);batch.append(second,false);batch.append({},true);
    CHECK(batch.offsets==std::vector<int>({0,0,2,3,3}));
    CHECK(batch.completeness==std::vector<int>({0,1,0,1}));
    CHECK(batch.trains==std::vector<std::int64_t>({std::int64_t(first[0]),std::int64_t(first[1]),std::int64_t(second[0])}));
    CHECK(first[0]!=first[1]); // Generations are never masked from an identity.
}
void invalidAndPerSignalLimit() {
    for(const auto invalid:{UINT64_C(0),UINT64_C(5)<<48,UINT64_C(1)<<48|1,UINT64_MAX}) {
        const std::array ids{invalid};rejects([&]{transport::validateBlockTrains(ids);});
    }
    std::vector<std::uint64_t> ids(transport::maximumBlockTrains,train(1));
    transport::ObservationTrainsBatch batch(2);batch.append(ids,true);
    CHECK(batch.trains.size()==4096&&batch.offsets.back()==4096&&batch.completeness.back()==1);
    ids.push_back(train(2));rejects([&]{batch.append(ids,true);});
    CHECK(batch.trains.size()==4096&&batch.offsets.size()==2&&batch.completeness.size()==1);
    const std::array invalid{train(2),UINT64_C(0)};
    rejects([&]{batch.append(invalid,true);});
    CHECK(batch.trains.size()==4096&&batch.offsets.size()==2&&batch.completeness.size()==1);
}
void exactBatchLimitAndNoTruncation() {
    std::vector<std::uint64_t> ids(transport::maximumBlockTrains,train(3));
    transport::ObservationTrainsBatch batch(34);
    for(int i=0;i<32;++i)batch.append(ids,i%2==0);
    CHECK(batch.trains.size()==transport::maximumBatchTrains&&batch.offsets.back()==131072);
    batch.append({},true);CHECK(batch.offsets.size()==34&&batch.completeness.size()==33);
    const std::array extra{train(4)};rejects([&]{batch.append(extra,true);});
    CHECK(batch.trains.size()==131072&&batch.offsets.size()==34&&batch.completeness.size()==33);
}
}
int main() {
    try {
        copiedGroupsAndFullIdentities();invalidAndPerSignalLimit();exactBatchLimitAndNoTruncation();
        std::cout<<"PASS Kotlin block-train transport: full identities, grouped completeness, exact limits and no truncation\n";
        return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
