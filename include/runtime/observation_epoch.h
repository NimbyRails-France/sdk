#pragma once
#include "engine/versioning.h"
#include <optional>
#include <atomic>
#include <mutex>
#include <stdexcept>
namespace nimby::runtime {
// Runtime transition detector, not a persistent save-file identity. Heap roots
// are compared only to detect changes; they never leave the SDK or become keys.
class ObservationEpoch {
public:
    struct Ticket { uint64_t started{},revision{}; int64_t clockTicks{}; };
    // Isolated workers delegate identity to the SDK in the game process. The
    // hooks carry no OS types and are installed once before opening sessions.
    // The authority owns its lifetime and never trusts a mod to advance time.
    struct Authority {
        void* context{};
        Ticket (*begin)(void*){};
        uint64_t (*observe)(void*,Ticket,const engine::VersioningObservation&,std::optional<int64_t>){};
    };
    explicit ObservationEpoch(bool useAuthority=true):useAuthority_(useAuthority) {}
    static void setAuthority(const Authority* authority) noexcept {authority_.store(authority,std::memory_order_release);}
    // Tickets span memory reads, but the shared lock never does. A delayed
    // reader may not rewind the epoch established by a newer observation.
    Ticket begin() {
        if(const auto* source=authority())return source->begin(source->context);
        std::lock_guard guard(mutex_);
        return {advance(),revision_};
    }
    void unavailable(){if(authority())return;std::lock_guard guard(mutex_);invalidate();}
    void unavailable(Ticket ticket) {
        if(authority())return; // A worker failure cannot invalidate its peers.
        std::lock_guard guard(mutex_);
        if(ticket.revision==revision_&&ticket.started>completed_)invalidate();
    }
    uint64_t observe(Ticket ticket,const engine::VersioningObservation& value,std::optional<int64_t> ticks) {
        if(const auto* source=authority())return source->observe(source->context,ticket,value,ticks);
        std::lock_guard guard(mutex_);
        if(ticket.revision!=revision_)return 0;
        // Overlapping reads have no reliable order. Older clock/world data
        // must be rejected, not mistaken for a load or a calendar rollback.
        // A subsequent non-overlapping sample still detects real rewinds.
        if(ticket.started<=completed_&&(!matches(value)||(ticks&&ticks_&&*ticks<*ticks_)))return 0;
        const auto result=observeValue(value,ticks);
        completed_=advance();
        return result;
    }
    uint64_t observe(const engine::VersioningObservation& value,std::optional<int64_t> ticks) {
        if(const auto* source=authority())return source->observe(source->context,source->begin(source->context),value,ticks);
        std::lock_guard guard(mutex_);
        const auto result=observeValue(value,ticks);completed_=advance();return result;
    }
private:
    const Authority* authority() const noexcept {return useAuthority_?authority_.load(std::memory_order_acquire):nullptr;}
    inline static std::atomic<const Authority*> authority_{};
    bool useAuthority_;
    uint64_t advance(){if(sequence_==UINT64_MAX)throw std::length_error("Observation order exhausted");return ++sequence_;}
    void invalidate(){if(previous_){advance();++revision_;}previous_.reset();ticks_.reset();}
    bool matches(const engine::VersioningObservation& value)const {
        return previous_&&previous_->state==value.state&&previous_->value==value.value&&previous_->history==value.history;
    }
    uint64_t observeValue(const engine::VersioningObservation& value,std::optional<int64_t> ticks) {
        if(!previous_||previous_->state!=value.state||previous_->value!=value.value||
           previous_->history!=value.history||(ticks&&ticks_&&*ticks<*ticks_)){
            if(generation_==UINT64_MAX)throw std::length_error("Observation generation exhausted");
            ++generation_;
            advance();++revision_;
            ticks_.reset();
        }
        previous_=value;if(ticks)ticks_=ticks;return generation_;
    }
    std::mutex mutex_;
    uint64_t sequence_=0,completed_=0,revision_=0;
    uint64_t generation_=0;
    std::optional<engine::VersioningObservation> previous_;
    std::optional<int64_t> ticks_;
};
}
