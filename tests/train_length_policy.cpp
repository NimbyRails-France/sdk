#include <engine/train_length_policy.h>
#include <atomic>
#include <cassert>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <thread>
#include <vector>

namespace length=nimby::engine::train_length;

static void registryContracts(){
    using namespace length;
    Registry registry;assert(!registry.limit().active());
    assert(registry.update(1,850)==PublishResult::NotFound&&!registry.limit().active());
    assert(registry.update(0,850)==PublishResult::InvalidOwner);
    assert(registry.update(1,0)==PublishResult::InvalidLimit);
    assert(registry.replace(0,850)==PublishResult::InvalidOwner);
    for(const double limit:{0.0,-1.0,.99,10000.01,std::numeric_limits<double>::infinity(),
            std::numeric_limits<double>::quiet_NaN()}){
        assert(registry.replace(1,limit)==PublishResult::InvalidLimit);
        assert(!registry.limit().active());
    }
    assert(registry.remove(0)==PublishResult::InvalidOwner);
    assert(registry.remove(1)==PublishResult::NotFound);
    assert(registry.replace(1,850)==PublishResult::Applied);
    assert(registry.limit().active()&&registry.limit().maximumMeters==850);
    assert(registry.update(2,700)==PublishResult::NotFound&&registry.limit().maximumMeters==850);
    assert(registry.update(1,900)==PublishResult::Applied&&registry.limit().maximumMeters==900);
    assert(registry.update(1,850)==PublishResult::Applied);
    assert(registry.replace(2,1200)==PublishResult::Applied&&registry.limit().maximumMeters==850);
    assert(registry.replace(1,1500)==PublishResult::Applied&&registry.limit().maximumMeters==1200);
    assert(registry.replace(2,650)==PublishResult::Applied&&registry.limit().maximumMeters==650);
    assert(registry.remove(3)==PublishResult::NotFound&&registry.limit().maximumMeters==650);
    assert(registry.remove(2)==PublishResult::Applied&&registry.limit().maximumMeters==1500);
    assert(registry.replace(1,minimumMaximumMeters)==PublishResult::Applied&&registry.limit().maximumMeters==1);
    assert(registry.replace(1,maximumMaximumMeters)==PublishResult::Applied&&registry.limit().maximumMeters==10000);
    assert(registry.remove(1)==PublishResult::Applied&&!registry.limit().active());

    for(size_t i=0;i<maximumOwners;++i)assert(registry.replace(100+i,900+i)==PublishResult::Applied);
    assert(registry.limit().maximumMeters==900);
    assert(registry.replace(999,850)==PublishResult::CapacityReached&&registry.limit().maximumMeters==900);
    // A full registry still allows its existing owners to update and remove.
    assert(registry.replace(100,850)==PublishResult::Applied&&registry.limit().maximumMeters==850);
    assert(registry.remove(100)==PublishResult::Applied&&registry.limit().maximumMeters==901);
    assert(registry.replace(999,800)==PublishResult::Applied&&registry.limit().maximumMeters==800);
    for(size_t i=1;i<maximumOwners;++i)assert(registry.remove(100+i)==PublishResult::Applied);
    assert(registry.remove(999)==PublishResult::Applied&&!registry.limit().active());
    assert(registry.remove(999)==PublishResult::NotFound);
    assert(registry.update(999,850)==PublishResult::NotFound&&!registry.limit().active());
}

static void compositionContracts(){
    using namespace length;
    size_t reads=0;
    const auto resolve=[&](ModelId model)->std::optional<float>{
        ++reads;
        switch(model){
            case 1:return 425;case 2:return 400;case 3:return 25;
            case 4:return .01f;case 5:return 500;case 6:return 350;
            case 7:return .125f;case 8:return std::numeric_limits<float>::quiet_NaN();
            case 9:return std::numeric_limits<float>::infinity();case 10:return -20;
            case 20:return 0;default:return std::nullopt;
        }
    };
    const Limit limit{850};
    const std::vector<ModelId> none{},existing{1,2},exact{3},over{3,4};
    auto result=evaluateAppend(limit,existing,exact,resolve);
    assert(result.allowed()&&result.totalLengthMeters==850&&result.evaluatedElements==3);
    result=evaluateAppend(limit,existing,over,resolve);
    assert(!result.allowed()&&result.decision==Decision::LimitExceeded&&result.totalLengthMeters>850.009);
    assert(result.totalLengthMeters<850.011&&result.evaluatedElements==4);

    // Candidate batches, repeated models and coupled compositions use their
    // full length. Evaluate completely even after crossing the maximum.
    const std::vector<ModelId> coupled{1},coupledCandidate{2,3},coupledOver{5};
    assert(evaluateAppend(limit,coupled,coupledCandidate,resolve).totalLengthMeters==850);
    result=evaluateAppend(limit,coupled,coupledOver,resolve);
    assert(result.decision==Decision::LimitExceeded&&result.totalLengthMeters==925);
    const std::vector<ModelId> repeated{5,5,5};reads=0;
    result=evaluateAppend(limit,none,repeated,resolve);
    assert(result.decision==Decision::LimitExceeded&&result.totalLengthMeters==1500&&reads==3);

    // The adaptor checks first, and calls the mutation only on acceptance.
    auto train=existing;const auto before=train;
    const auto append=[&](const std::vector<ModelId>& candidate){
        const auto decision=evaluateAppend(limit,train,candidate,resolve);
        if(decision.allowed())train.insert(train.end(),candidate.begin(),candidate.end());
        return decision;
    };
    assert(!append(over).allowed()&&train==before);
    assert(append(exact).allowed()&&train==std::vector<ModelId>({1,2,3}));

    const std::vector<ModelId> unknown{42},zero{20};
    result=evaluateAppend(limit,existing,unknown,resolve);
    assert(result.decision==Decision::UnknownModel&&result.problematicModel==42&&result.evaluatedElements==2);
    result=evaluateAppend(limit,existing,zero,resolve);
    assert(result.decision==Decision::InvalidLength&&result.problematicModel==20);
    assert(evaluateAppend(limit,unknown,exact,resolve).decision==Decision::UnknownModel);
    for(ModelId model:{8,9,10}){
        const std::vector<ModelId> invalid{model};
        result=evaluateAppend(limit,none,invalid,resolve);
        assert(result.decision==Decision::InvalidLength&&result.problematicModel==model);
    }
    result=evaluateAppend(limit,none,exact,[](ModelId)->std::optional<double>{throw std::runtime_error("model unavailable");});
    assert(result.decision==Decision::ResolverFailure&&result.problematicModel==3);
    const std::vector<ModelId> huge{1,1};
    result=evaluateAppend(limit,none,huge,[](ModelId)->std::optional<double>{return std::numeric_limits<double>::max();});
    assert(result.decision==Decision::InvalidLength);

    const std::vector<ModelId> bounded(maximumElements,7),tooMany(maximumElements+1,7);
    reads=0;result=evaluateAppend(limit,none,bounded,resolve);
    assert(result.allowed()&&result.totalLengthMeters==512&&reads==maximumElements);
    reads=0;result=evaluateAppend(limit,none,tooMany,resolve);
    assert(result.decision==Decision::TooManyElements&&reads==0);
    result=evaluateAppend(limit,bounded,exact,resolve);
    assert(result.decision==Decision::TooManyElements&&reads==0);
    result=evaluateAppend({std::numeric_limits<double>::quiet_NaN()},existing,exact,resolve);
    assert(result.decision==Decision::InvalidLimit&&reads==0);
    result=evaluateAppend({0},unknown,tooMany,resolve);
    assert(result.allowed()&&reads==0); // No mod: no native reads or safety override.

    const std::vector<ModelBlock> blocks{{1,2}},emptyBlocks{},oneCentimeter{{4,1}},zeroBlocks{{42,0}};
    result=evaluateAppend(limit,emptyBlocks,blocks,resolve);
    assert(result.allowed()&&result.totalLengthMeters==850&&result.evaluatedElements==2);
    result=evaluateAppend(limit,blocks,oneCentimeter,resolve);
    assert(result.decision==Decision::LimitExceeded&&result.totalLengthMeters>850.009);
    reads=0;result=evaluateAppend(limit,zeroBlocks,emptyBlocks,resolve);
    assert(result.allowed()&&reads==0);
    const std::vector<ModelBlock> blockBounded{{7,maximumElements}},blockOver{{7,maximumElements+1}},
        blockOverflow{{7,std::numeric_limits<size_t>::max()}},countOverflow{{7,maximumElements},{7,1}};
    result=evaluateAppend(limit,emptyBlocks,blockBounded,resolve);
    assert(result.allowed()&&result.totalLengthMeters==512&&result.evaluatedElements==maximumElements&&reads==1);
    for(const auto* blockRange:{&blockOver,&blockOverflow,&countOverflow}){
        reads=0;result=evaluateAppend(limit,emptyBlocks,*blockRange,resolve);
        assert(result.decision==Decision::TooManyElements&&reads==0);
    }
    const std::vector<ModelBlock> tooManyBlocks(maximumElements+1,{42,0});
    reads=0;assert(evaluateAppend(limit,emptyBlocks,tooManyBlocks,resolve).decision==Decision::TooManyElements&&reads==0);
}

static void concurrentContracts(){
    using namespace length;
    Registry registry;assert(registry.replace(1,650)==PublishResult::Applied);
    std::atomic<bool> start=false,stop=false;
    std::atomic<size_t> checks=0,operations=0;
    std::thread reader([&]{
        while(!start.load(std::memory_order_acquire))std::this_thread::yield();
        do {
            const auto limit=registry.limit();assert(limit.active()&&limit.maximumMeters==650);
            checks.fetch_add(1,std::memory_order_relaxed);
        } while(!stop.load(std::memory_order_acquire));
    });
    std::vector<std::thread> writers;
    for(uint64_t token=2;token<=5;++token)writers.emplace_back([&,token]{
        while(!start.load(std::memory_order_acquire))std::this_thread::yield();
        const auto retry=[&](auto operation){
            for(size_t attempt=0;attempt<1000000;++attempt){
                const auto result=operation();
                if(result==PublishResult::Busy){std::this_thread::yield();continue;}
                assert(result==PublishResult::Applied);operations.fetch_add(1,std::memory_order_relaxed);return;
            }
            assert(false&&"bounded registry mutation made no progress");
        };
        for(size_t i=0;i<2000;++i){
            retry([&]{return registry.replace(token,i%2?800:950);});
            retry([&]{return registry.remove(token);});
        }
    });
    start.store(true,std::memory_order_release);
    for(auto& writer:writers)writer.join();
    stop.store(true,std::memory_order_release);reader.join();
    assert(operations==16000&&checks>0&&registry.limit().maximumMeters==650);
    assert(registry.remove(1)==PublishResult::Applied&&!registry.limit().active());

    // Active state and value are a single publication; a reader cannot observe
    // an active flag paired with an old zero or partially updated length.
    stop=false;start=false;checks=0;
    std::thread snapshotReader([&]{
        start.store(true,std::memory_order_release);
        do {
            const auto value=registry.limit();
            assert((!value.active()&&value.maximumMeters==0)||
                (value.active()&&(value.maximumMeters==850||value.maximumMeters==10000)));
            checks.fetch_add(1,std::memory_order_relaxed);
        } while(!stop.load(std::memory_order_acquire));
    });
    while(!start.load(std::memory_order_acquire))std::this_thread::yield();
    for(size_t i=0;i<4000;++i){
        assert(registry.replace(6,i%2?850:10000)==PublishResult::Applied);
        assert(registry.remove(6)==PublishResult::Applied);
    }
    stop.store(true,std::memory_order_release);snapshotReader.join();assert(checks>0);
}

int main(){
    registryContracts();compositionContracts();concurrentContracts();
    std::cout<<"PASS: bounded per-owner train-length policy, full composition assessment and atomic native snapshots\n";
}
