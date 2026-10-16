#pragma once
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <type_traits>

namespace nimby::engine::interlocking {
// Native callback facts for diagnostics only. A delivered row is not an entry,
// rear-clear, world-lifetime or permission proof. NumericComplete qualifies
// only the representation of the numbers, never their railway interpretation.
struct NativePassageRow {
    static constexpr uint32_t NumericComplete=1;
    uint64_t train,signal,track,threadStepSequence;
    uint32_t outcome,reason,flags;
    double beforeHead,boundaryHead,afterHead,beforeFraction,finalFraction;
};
static_assert(std::is_trivial_v<NativePassageRow> &&
              std::is_standard_layout_v<NativePassageRow> &&
              std::is_trivially_copyable_v<NativePassageRow>);

enum class NativePassagePublish : uint8_t {Accepted,Busy,Full,Invalid};
enum class NativePassageConsume : uint8_t {Ready,Empty,Pending};
struct NativePassageBatch {
    uint64_t sequence{}; // Reservation order, starting at one.
    uint32_t count{};
    std::array<NativePassageRow,8> rows{};
};
struct NativePassageStats {
    uint64_t accepted{},busy{},full{},invalid{},lost{},acceptedRows{};
};

// Bounded MPSC mailbox with exactly one broker consumer. The producer performs
// one reservation CAS, never waits for another producer and never calls user
// code. Payload storage remains exclusively owned until the release sequence
// publishes it; the consumer releases it only after copying its payload.
// Construct/destroy only outside callbacks, with no producer/consumer active.
class NativePassageMailbox {
public:
    static constexpr size_t slotCapacity=16,rowsPerBatch=8;
    static constexpr size_t rowCapacity=slotCapacity*rowsPerBatch;
    NativePassageMailbox() noexcept {
        for(size_t i=0;i<slotCapacity;++i)slots_[i].sequence.store(i,std::memory_order_relaxed);
    }
    NativePassageMailbox(const NativePassageMailbox&)=delete;
    NativePassageMailbox& operator=(const NativePassageMailbox&)=delete;

    NativePassagePublish tryPublish(std::span<const NativePassageRow> rows) noexcept {
        if(rows.empty()||rows.size()>rowsPerBatch)return reject(NativePassagePublish::Invalid);
        for(const auto& row:rows)if(!valid(row))return reject(NativePassagePublish::Invalid);
        Claim claim{};
        const auto result=tryClaim(claim);
        if(result!=NativePassagePublish::Accepted)return result;
        publishClaim(claim,rows);
        return NativePassagePublish::Accepted;
    }

    // Pending means the FIFO head is owned but not yet published. Later ready
    // slots are deliberately not skipped. Empty/Pending leave output untouched.
    NativePassageConsume tryConsume(NativePassageBatch& output) noexcept {
        auto& slot=slots_[readTicket_%slotCapacity];
        if(slot.sequence.load(std::memory_order_acquire)!=readTicket_+1)
            return writeTicket_.load(std::memory_order_acquire)>readTicket_?
                NativePassageConsume::Pending:NativePassageConsume::Empty;
        output.sequence=readTicket_+1;
        output.count=slot.count;
        for(size_t i=0;i<slot.count;++i)output.rows[i]=slot.rows[i];
        slot.sequence.store(readTicket_+slotCapacity,std::memory_order_release);
        ++readTicket_;
        return NativePassageConsume::Ready;
    }

    // Counters count submission attempts, except acceptedRows. Individual
    // atomic samples need not form one transaction while producers are active.
    // Every rejected batch increments lost, even a contention-only rejection.
    NativePassageStats stats() const noexcept {
        return {accepted_.load(std::memory_order_relaxed),busy_.load(std::memory_order_relaxed),
            full_.load(std::memory_order_relaxed),invalid_.load(std::memory_order_relaxed),
            lost_.load(std::memory_order_relaxed),acceptedRows_.load(std::memory_order_relaxed)};
    }
    // Sticky for this mailbox's entire lifetime. Draining never repairs a gap.
    bool lossObserved() const noexcept {return loss_.load(std::memory_order_relaxed);}

private:
    friend struct NativePassageMailboxTestAccess;
    static_assert(std::atomic<uint64_t>::is_always_lock_free &&
                  std::atomic<bool>::is_always_lock_free,
                  "Native callbacks require lock-free mailbox atomics");
    struct alignas(64) Slot {
        std::atomic<uint64_t> sequence{};
        uint32_t count{};
        std::array<NativePassageRow,rowsPerBatch> rows{};
    };
    struct Claim {Slot* slot{};uint64_t ticket{};};
    static bool valid(const NativePassageRow& row) noexcept {
        if(row.train>>48!=5||row.signal>>48!=8||row.track>>48!=1||!row.threadStepSequence||
           row.flags&~NativePassageRow::NumericComplete)return false;
        if(!(row.flags&NativePassageRow::NumericComplete))return true;
        return std::isfinite(row.beforeHead)&&std::isfinite(row.boundaryHead)&&std::isfinite(row.afterHead)&&
            std::isfinite(row.beforeFraction)&&row.beforeFraction>=0&&row.beforeFraction<=1&&
            std::isfinite(row.finalFraction)&&row.finalFraction>=0&&row.finalFraction<=1;
    }
    NativePassagePublish reject(NativePassagePublish reason) noexcept {
        if(reason==NativePassagePublish::Busy)busy_.fetch_add(1,std::memory_order_relaxed);
        else if(reason==NativePassagePublish::Full)full_.fetch_add(1,std::memory_order_relaxed);
        else invalid_.fetch_add(1,std::memory_order_relaxed);
        lost_.fetch_add(1,std::memory_order_relaxed);
        loss_.store(true,std::memory_order_relaxed);
        return reason;
    }
    NativePassagePublish tryClaim(Claim& claim) noexcept {
        auto ticket=writeTicket_.load(std::memory_order_relaxed);
        // Never wrap the sequence number and accidentally revive a stale slot.
        if(ticket>std::numeric_limits<uint64_t>::max()-slotCapacity)
            return reject(NativePassagePublish::Full);
        auto& slot=slots_[ticket%slotCapacity];
        const auto sequence=slot.sequence.load(std::memory_order_acquire);
        if(sequence!=ticket)return reject(sequence<ticket?NativePassagePublish::Full:NativePassagePublish::Busy);
        if(!writeTicket_.compare_exchange_strong(ticket,ticket+1,std::memory_order_acq_rel,std::memory_order_relaxed))
            return reject(NativePassagePublish::Busy);
        claim={&slot,ticket};
        return NativePassagePublish::Accepted;
    }
    void publishClaim(Claim claim,std::span<const NativePassageRow> rows) noexcept {
        for(size_t i=0;i<rows.size();++i)claim.slot->rows[i]=rows[i];
        claim.slot->count=static_cast<uint32_t>(rows.size());
        claim.slot->sequence.store(claim.ticket+1,std::memory_order_release);
        acceptedRows_.fetch_add(rows.size(),std::memory_order_relaxed);
        accepted_.fetch_add(1,std::memory_order_relaxed);
    }
    std::array<Slot,slotCapacity> slots_;
    alignas(64) std::atomic<uint64_t> writeTicket_{};
    alignas(64) uint64_t readTicket_{}; // Single consumer only.
    std::atomic<uint64_t> accepted_{},busy_{},full_{},invalid_{},lost_{},acceptedRows_{};
    std::atomic<bool> loss_{};
};
static_assert(sizeof(NativePassageMailbox)<=16*1024,
              "Diagnostic mailbox memory must stay fixed and small");
} // namespace nimby::engine::interlocking
