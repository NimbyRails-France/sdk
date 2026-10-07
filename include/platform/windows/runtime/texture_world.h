#pragma once
#include <engine/versioning.h>
#include <engine/simulation_clock.h>
#include <runtime/observation_epoch.h>
#include <nimby/detail/observation.h>
#include <atomic>
#include <memory>
#include <mutex>

namespace nimby::texture_bridge {
// A clock sample is ordered before its memory reads. Pairing the completed
// order and tick atomically prevents a delayed renderer from inventing a rewind.
class RenderClock {
    struct alignas(16) Pair {uint64_t ticks,completed;};
    Pair value_;
    std::atomic<uint64_t> started_{0};
    bool exchange(Pair& expected,Pair desired) noexcept {
        unsigned char changed;
        __asm__ volatile("lock cmpxchg16b %1; sete %0"
            : "=q"(changed), "+m"(value_), "+a"(expected.ticks), "+d"(expected.completed)
            : "b"(desired.ticks), "c"(desired.completed) : "cc", "memory");
        return changed!=0;
    }
public:
    explicit RenderClock(int64_t ticks):value_{static_cast<uint64_t>(ticks),0}{}
    bool exhausted()const noexcept {return started_.load(std::memory_order_relaxed)==UINT64_MAX;}
    uint64_t begin() noexcept {
        auto value=started_.load(std::memory_order_relaxed);
        for(unsigned attempt=0;attempt<3&&value!=UINT64_MAX;++attempt)
            if(started_.compare_exchange_weak(value,value+1,std::memory_order_relaxed))return value+1;
        return 0; // Contention is not evidence of a world transition.
    }
    bool rewind(uint64_t order,int64_t ticks) noexcept {
        if(!order)return false;
        Pair expected{};exchange(expected,{});
        for(unsigned attempt=0;attempt<3;++attempt){
            if(order<=expected.completed)return false;
            if(static_cast<uint64_t>(ticks)<expected.ticks)return true;
            // Completion also advances the sequence: reads which overlap this
            // one (even with a later start) are not evidence of a real rewind.
            const auto completed=begin();if(!completed)return false;
            if(exchange(expected,{static_cast<uint64_t>(ticks),completed}))return false;
        }
        return false;
    }
};

struct WorldGuard {
    uint64_t generation;
    engine::VersioningObservation observed;
    std::array<unsigned char,0x38> header;
    engine::SimulationClock clock;
    RenderClock renders;
    WorldGuard(uint64_t id,engine::VersioningObservation value,std::array<unsigned char,0x38> version,
               engine::SimulationClock time):generation(id),observed(std::move(value)),header(version),clock(time),renders(time.ticks){}
};

// One authority lives in the texture bridge, including when an external
// diagnostic loads that bridge without a resident SDK or a mod host.
class TextureWorld {
    std::mutex writers_;
    runtime::ObservationEpoch epoch_{false};
    uint64_t observedEpoch_=0;
    std::atomic<uint64_t> generation_{0};
    std::shared_ptr<WorldGuard> current_;
public:
    static_assert(std::atomic<uint64_t>::is_always_lock_free);
    bool current(uint64_t generation)const noexcept {return generation&&generation_.load(std::memory_order_acquire)==generation;}
    void invalidate(uint64_t generation) noexcept {
        if(generation&&generation!=UINT64_MAX)
            generation_.compare_exchange_strong(generation,generation+1,std::memory_order_acq_rel);
    }
    uint32_t capture(engine::ReadMemory read,void* context,uint64_t module,uint64_t database,uint64_t simulation,
                     std::shared_ptr<WorldGuard>& result) {
        std::lock_guard lock(writers_);
        engine::VersioningObservation before,after;engine::SimulationClock clock;
        std::array<unsigned char,0x38> header{};
        if(!engine::read_versioning_observation(read,context,module,true,before)) {
            epoch_.unavailable();if(current_)invalidate(current_->generation);return NIMBY_DATA_UNAVAILABLE;
        }
        // A delayed client's roots are not a failure of the healthy current
        // world. Reject that request without invalidating other publishers.
        if(before.state.database!=database||before.state.simulation!=simulation)return NIMBY_DATA_UNAVAILABLE;
        if(!engine::read_simulation_clock(read,context,simulation,clock)||
           !engine::read_versioning_observation(read,context,module,true,after)||
           before.state!=after.state||before.value!=after.value||before.history!=after.history||
           !read(context,database+engine::gameLayout(after.state.profile).versioning,header.data(),header.size())||
           std::memcmp(header.data(),after.value.data(),after.value.size())) {
            epoch_.unavailable();if(current_)invalidate(current_->generation);return NIMBY_DATA_UNAVAILABLE;
        }
        const auto observed=epoch_.observe(after,clock.ticks);
        auto generation=generation_.load(std::memory_order_acquire);
        const bool changed=!current_||observed!=observedEpoch_||current_->header!=header||current_->clock.epoch_seconds!=clock.epoch_seconds;
        if(changed&&(!current_||current_->generation==generation)) {
            if(generation==UINT64_MAX)return NIMBY_RESOURCE_LIMIT;
            // A renderer can invalidate concurrently; never overwrite its fence.
            for(;;){
                if(generation==UINT64_MAX)return NIMBY_RESOURCE_LIMIT;
                if(generation_.compare_exchange_weak(generation,generation+1,std::memory_order_acq_rel)){++generation;break;}
            }
        }
        if(generation==UINT64_MAX)return NIMBY_RESOURCE_LIMIT;
        if(changed||current_->generation!=generation)
            current_=std::make_shared<WorldGuard>(generation,std::move(after),header,clock);
        observedEpoch_=observed;result=current_;
        return current(result->generation)?NIMBY_OK:NIMBY_DATA_UNAVAILABLE;
    }
    // The renderer supplies only a pinned immutable WorldGuard. This path has
    // fixed-size stack reads, bounded atomics, no mutex, allocation or history scan.
    bool validate(WorldGuard* world,engine::ReadMemory read,void* context,uint64_t module,engine::SimulationClock* time=nullptr) noexcept {
        if(!world||!current(world->generation))return false;
        const auto order=world->renders.begin();
        if(world->renders.exhausted()){invalidate(world->generation);return false;}
        uint64_t root{};std::array<unsigned char,0x148> roots{};
        std::array<unsigned char,0x38> version{},repeated{};engine::SimulationClock clock;
        auto pointer=[&](size_t offset){uint64_t value{};std::memcpy(&value,roots.data()+offset,8);return value;};
        const auto& expected=world->observed.state;
        if(!read(context,module+0xb81998,&root,8)||root!=expected.root||
           !read(context,root+0x540,roots.data(),roots.size())||pointer(0)!=expected.database||
           pointer(0x80)!=expected.copy||pointer(0x140)!=expected.simulation||
           !read(context,expected.database+0xa48,version.data(),version.size())||version!=world->header||
           !engine::read_simulation_clock(read,context,expected.simulation,clock)||
           !read(context,expected.database+0xa48,repeated.data(),repeated.size())||version!=repeated||
           clock.epoch_seconds!=world->clock.epoch_seconds||clock.ticks<world->clock.ticks||world->renders.rewind(order,clock.ticks)) {
            invalidate(world->generation);return false;
        }
        if(time)*time=clock;
        return current(world->generation);
    }
};
}
