#pragma once
#include <array>
#include <atomic>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <memory>
#include <span>

namespace nimby::engine::train_length {
using OwnerToken=uint64_t;
using ModelId=uint64_t;
inline constexpr double minimumMaximumMeters=1.0;
inline constexpr double maximumMaximumMeters=10000.0;
inline constexpr size_t maximumOwners=64;
// This is an input safety bound, not a train-size rule. The business rule is
// exclusively the sum of the loaded vehicle models' physical lengths.
inline constexpr size_t maximumElements=4096;

struct Limit {
    double maximumMeters=0;
    bool active()const noexcept{return maximumMeters!=0;}
};
inline bool validMaximum(double meters)noexcept {
    return std::isfinite(meters)&&meters>=minimumMaximumMeters&&meters<=maximumMaximumMeters;
}
enum class PublishResult { Applied, Busy, InvalidOwner, InvalidLimit, CapacityReached, NotFound };

// The parent assigns one opaque, non-repeating capability token per isolated
// mod channel. Clients never choose tokens or remove another channel's token.
// Mutations are bounded and never run callbacks; the game-side read is one
// lock-free atomic load. A zero publication means no owner: native behaviour.
template<class Presentation=std::nullptr_t>
class Registry {
public:
    struct Selection {
        Limit limit;
        OwnerToken owner=0;
        std::shared_ptr<const Presentation> presentation;
        uint64_t revision=0;
    };
private:
    struct Owner {OwnerToken token=0;double maximumMeters=0;std::shared_ptr<const Presentation> presentation;};
    std::array<Owner,maximumOwners> owners_{};
    std::mutex mutex_;
    std::atomic<uint64_t> effective_{0};
    std::atomic<std::shared_ptr<const Selection>> selected_;
    std::atomic<uint64_t> revision_{0};
    static_assert(std::atomic<uint64_t>::is_always_lock_free);

    void publish(std::array<Owner,maximumOwners> next) {
        const Owner* minimum=nullptr;
        for(const auto& owner:next)if(owner.token&&(!minimum||owner.maximumMeters<minimum->maximumMeters||
                (owner.maximumMeters==minimum->maximumMeters&&owner.token<minimum->token)))minimum=&owner;
        const auto revision=revision_.load(std::memory_order_relaxed)+1;
        // Allocate before accepting state: failed publication leaves the old
        // policy, its owner and its wording intact. Only broker threads write.
        auto selection=minimum?std::make_shared<const Selection>(Selection{{minimum->maximumMeters},minimum->token,
            minimum->presentation,revision}):std::shared_ptr<const Selection>{};
        const auto maximumMeters=minimum?minimum->maximumMeters:0.0;
        owners_=std::move(next);
        // Invalidate old UI notices first, including an owner replacement with
        // exactly the same limit. Active fast-path reads stay one atomic load.
        revision_.store(revision,std::memory_order_release);
        selected_.store(std::move(selection),std::memory_order_release);
        effective_.store(std::bit_cast<uint64_t>(maximumMeters),std::memory_order_release);
    }
public:
    Limit limit()const noexcept {
        return {std::bit_cast<double>(effective_.load(std::memory_order_acquire))};
    }
    std::shared_ptr<const Selection> selected()const noexcept{return selected_.load(std::memory_order_acquire);}
    uint64_t selectionRevision()const noexcept{return revision_.load(std::memory_order_acquire);}
    PublishResult replace(OwnerToken token,double maximumMeters,std::shared_ptr<const Presentation> presentation={}) {
        if(!token)return PublishResult::InvalidOwner;
        if(!validMaximum(maximumMeters))return PublishResult::InvalidLimit;
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock)return PublishResult::Busy;
        auto next=owners_;Owner* vacant=nullptr;
        for(auto& owner:next){
            if(owner.token==token){owner.maximumMeters=maximumMeters;owner.presentation=std::move(presentation);publish(std::move(next));return PublishResult::Applied;}
            if(!owner.token&&!vacant)vacant=&owner;
        }
        if(!vacant)return PublishResult::CapacityReached;
        *vacant={token,maximumMeters,std::move(presentation)};publish(std::move(next));return PublishResult::Applied;
    }
    PublishResult remove(OwnerToken token) {
        if(!token)return PublishResult::InvalidOwner;
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock)return PublishResult::Busy;
        auto next=owners_;
        for(auto& owner:next)if(owner.token==token){owner={};publish(std::move(next));return PublishResult::Applied;}
        return PublishResult::NotFound;
    }
    PublishResult update(OwnerToken token,double maximumMeters) {
        if(!token)return PublishResult::InvalidOwner;
        if(!validMaximum(maximumMeters))return PublishResult::InvalidLimit;
        std::unique_lock lock(mutex_,std::try_to_lock);
        if(!lock)return PublishResult::Busy;
        auto next=owners_;
        for(auto& owner:next)if(owner.token==token){
            owner.maximumMeters=maximumMeters;publish(std::move(next));return PublishResult::Applied;
        }
        return PublishResult::NotFound;
    }
};

struct ModelBlock {ModelId model;size_t count;};
enum class Decision { Allowed, LimitExceeded, UnknownModel, InvalidLength, TooManyElements, InvalidLimit, ResolverFailure };
struct Assessment {
    Decision decision=Decision::Allowed;
    double totalLengthMeters=0;
    double maximumMeters=0;
    ModelId problematicModel=0;
    size_t evaluatedElements=0;
    bool allowed()const noexcept{return decision==Decision::Allowed;}
};

namespace detail {
inline ModelBlock block(ModelId model)noexcept{return {model,1};}
inline ModelBlock block(ModelBlock value)noexcept{return value;}

// Resolve only a bounded candidate composition at its mutation boundary. The
// resolver returns optional<float/double> in metres from the loaded models;
// missing models and invalid lengths remain distinct diagnostic outcomes.
// Neither the native composition nor any policy state can be changed here.
template<class Element,class Resolver>
Assessment evaluate(Limit limit,std::span<const Element> existing,std::span<const Element> candidate,Resolver&& resolve)noexcept {
    Assessment result;result.maximumMeters=limit.maximumMeters;
    if(!limit.active())return result;
    if(!validMaximum(limit.maximumMeters)){result.decision=Decision::InvalidLimit;return result;}
    if(existing.size()>maximumElements||candidate.size()>maximumElements-existing.size()){
        result.decision=Decision::TooManyElements;return result;
    }
    size_t elements=0;
    for(auto range:{existing,candidate})for(const auto& entry:range){
        const auto value=block(entry);
        if(value.count>maximumElements-elements){result.decision=Decision::TooManyElements;return result;}
        elements+=value.count;
    }
    for(auto range:{existing,candidate})for(const auto& entry:range){
        const auto value=block(entry);if(!value.count)continue;
        result.problematicModel=value.model;
        try {
            const auto meters=resolve(value.model);
            if(!meters){result.decision=Decision::UnknownModel;return result;}
            const double length=static_cast<double>(*meters);
            if(!std::isfinite(length)||length<=0){result.decision=Decision::InvalidLength;return result;}
            const double total=result.totalLengthMeters+length*static_cast<double>(value.count);
            if(!std::isfinite(total)){result.decision=Decision::InvalidLength;return result;}
            result.totalLengthMeters=total;result.evaluatedElements+=value.count;
        } catch(...) {
            result.decision=Decision::ResolverFailure;return result;
        }
    }
    result.problematicModel=0;
    if(result.totalLengthMeters>limit.maximumMeters)result.decision=Decision::LimitExceeded;
    return result;
}
}

template<class Resolver>
Assessment evaluateAppend(Limit limit,std::span<const ModelId> existing,std::span<const ModelId> candidate,Resolver&& resolve)noexcept {
    return detail::evaluate(limit,existing,candidate,static_cast<Resolver&&>(resolve));
}
template<class Resolver>
Assessment evaluateAppend(Limit limit,std::span<const ModelBlock> existing,std::span<const ModelBlock> candidate,Resolver&& resolve)noexcept {
    return detail::evaluate(limit,existing,candidate,static_cast<Resolver&&>(resolve));
}
}
