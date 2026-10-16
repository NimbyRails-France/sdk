#include <engine/native_passage_mailbox.h>
#include <array>
#include <barrier>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>
#include <thread>

using namespace nimby::engine::interlocking;
namespace {
std::atomic<bool> watchAllocations{};
std::atomic<size_t> allocationCount{};
size_t checks{};
#define CHECK(value) do {++checks;if(!(value))throw std::runtime_error(#value);}while(false)
constexpr uint64_t id(unsigned kind,unsigned ordinal){return (uint64_t(kind)<<48)|(uint64_t(ordinal)<<16)|1;}
NativePassageRow row(unsigned producer=1,uint64_t step=1) {
    return {id(5,producer),id(8,producer),id(1,producer),step,2,3,NativePassageRow::NumericComplete,
        double(step),double(step)+.5,double(step)+1,.25,.75};
}
}
void* operator new(std::size_t size) {
    if(watchAllocations.load(std::memory_order_relaxed))allocationCount.fetch_add(1,std::memory_order_relaxed);
    if(auto* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
void* operator new[](std::size_t size){return ::operator new(size);}
void operator delete(void* value) noexcept {std::free(value);}
void operator delete[](void* value) noexcept {std::free(value);}
void operator delete(void* value,std::size_t) noexcept {std::free(value);}
void operator delete[](void* value,std::size_t) noexcept {std::free(value);}

namespace nimby::engine::interlocking {
// Hold a real production reservation open to test publication order and owned
// slot safety deterministically, without introducing a callback on the hook.
struct NativePassageMailboxTestAccess {
    using Claim=NativePassageMailbox::Claim;
    static NativePassagePublish claim(NativePassageMailbox& queue,Claim& result){return queue.tryClaim(result);}
    static void publish(NativePassageMailbox& queue,Claim claim,std::span<const NativePassageRow> rows){queue.publishClaim(claim,rows);}
};
}
namespace {
void fifoAndOwnedSlots() {
    NativePassageMailbox queue;
    NativePassageBatch output{};output.sequence=999;
    CHECK(queue.tryConsume(output)==NativePassageConsume::Empty&&output.sequence==999);
    NativePassageMailboxTestAccess::Claim first{},second{};
    CHECK(NativePassageMailboxTestAccess::claim(queue,first)==NativePassagePublish::Accepted);
    CHECK(NativePassageMailboxTestAccess::claim(queue,second)==NativePassagePublish::Accepted);
    auto a=row(1,1),b=row(2,1);
    NativePassageMailboxTestAccess::publish(queue,second,std::span(&b,1));
    CHECK(queue.tryConsume(output)==NativePassageConsume::Pending&&output.sequence==999);
    NativePassageMailboxTestAccess::publish(queue,first,std::span(&a,1));
    a.train=id(5,99); // A published payload never borrows producer storage.
    CHECK(queue.tryConsume(output)==NativePassageConsume::Ready&&output.sequence==1&&output.count==1);
    CHECK(output.rows[0].train==id(5,1));
    CHECK(queue.tryConsume(output)==NativePassageConsume::Ready&&output.sequence==2&&output.rows[0].train==b.train);
    CHECK(queue.tryConsume(output)==NativePassageConsume::Empty);
    CHECK(queue.stats().accepted==2&&queue.stats().acceptedRows==2&&!queue.lossObserved());

    // A pending head owns its slot even after every later slot is published.
    CHECK(NativePassageMailboxTestAccess::claim(queue,first)==NativePassagePublish::Accepted);
    for(size_t i=1;i<NativePassageMailbox::slotCapacity;++i)
        CHECK(queue.tryPublish(std::span(&b,1))==NativePassagePublish::Accepted);
    CHECK(queue.tryPublish(std::span(&b,1))==NativePassagePublish::Full);
    CHECK(queue.tryConsume(output)==NativePassageConsume::Pending);
    NativePassageMailboxTestAccess::publish(queue,first,std::span(&b,1));
    for(size_t i=0;i<NativePassageMailbox::slotCapacity;++i)
        CHECK(queue.tryConsume(output)==NativePassageConsume::Ready&&output.sequence==i+3);
    CHECK(queue.tryConsume(output)==NativePassageConsume::Empty&&queue.lossObserved());
}

void saturationAndValidation() {
    NativePassageMailbox queue;
    std::array<NativePassageRow,NativePassageMailbox::rowsPerBatch> rows{};
    for(auto& value:rows)value=row();
    for(size_t i=0;i<NativePassageMailbox::slotCapacity;++i)CHECK(queue.tryPublish(rows)==NativePassagePublish::Accepted);
    CHECK(queue.tryPublish(rows)==NativePassagePublish::Full);
    auto stats=queue.stats();CHECK(stats.accepted==16&&stats.acceptedRows==128&&stats.full==1&&stats.lost==1&&queue.lossObserved());
    NativePassageBatch output{};
    for(size_t i=0;i<16;++i)CHECK(queue.tryConsume(output)==NativePassageConsume::Ready&&output.sequence==i+1&&output.count==8);
    CHECK(queue.tryPublish(rows)==NativePassagePublish::Accepted);
    CHECK(queue.tryConsume(output)==NativePassageConsume::Ready&&output.sequence==17);
    CHECK(queue.lossObserved()&&queue.stats().lost==1); // Draining does not erase history.
    CHECK(queue.tryPublish({})==NativePassagePublish::Invalid);
    std::array<NativePassageRow,9> oversized{};CHECK(queue.tryPublish(oversized)==NativePassagePublish::Invalid);
    auto invalid=row();invalid.train=0;CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.signal=id(1,1);CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.track=0;CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.threadStepSequence=0;CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.flags=2;CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.beforeHead=std::numeric_limits<double>::quiet_NaN();CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.boundaryHead=std::numeric_limits<double>::infinity();CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.finalFraction=1.01;CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    invalid=row();invalid.beforeFraction=-.01;CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Invalid);
    CHECK(queue.stats().invalid==11&&queue.stats().lost==12);
    // Unavailable diagnostics retain honest NaN values without qualifying them.
    invalid=row();invalid.flags=0;invalid.beforeHead=invalid.finalFraction=std::numeric_limits<double>::quiet_NaN();
    CHECK(queue.tryPublish(std::span(&invalid,1))==NativePassagePublish::Accepted);
    CHECK(queue.tryConsume(output)==NativePassageConsume::Ready&&output.rows[0].flags==0&&
        std::isnan(output.rows[0].beforeHead)&&std::isnan(output.rows[0].finalFraction));
    // Validate the whole batch before reserving/publishing any of its rows.
    rows[7].signal=0;CHECK(queue.tryPublish(rows)==NativePassagePublish::Invalid);
    CHECK(queue.tryConsume(output)==NativePassageConsume::Empty);
}

void noAllocationAndBoundedMemory() {
    static_assert(NativePassageMailbox::rowCapacity==128);
    static_assert(sizeof(NativePassageMailbox)<=16*1024);
    static_assert(std::is_trivial_v<NativePassageRow>);
    NativePassageMailbox queue;NativePassageBatch output{};
    std::array<NativePassageRow,8> rows{};for(auto& value:rows)value=row();
    bool valid=true;
    allocationCount.store(0);watchAllocations.store(true);
    for(size_t cycle=0;cycle<64;++cycle) {
        for(size_t i=0;i<16;++i)valid&=queue.tryPublish(rows)==NativePassagePublish::Accepted;
        valid&=queue.tryPublish(rows)==NativePassagePublish::Full;
        for(size_t i=0;i<16;++i)valid&=queue.tryConsume(output)==NativePassageConsume::Ready;
        valid&=queue.tryConsume(output)==NativePassageConsume::Empty;
        valid&=queue.tryPublish({})==NativePassagePublish::Invalid;
        const auto stats=queue.stats();valid&=stats.acceptedRows==(cycle+1)*128&&queue.lossObserved();
    }
    watchAllocations.store(false);
    CHECK(valid&&allocationCount.load()==0);
}

void concurrentProducersAndConsumer() {
    constexpr size_t producers=4,attempts=4096;
    NativePassageMailbox queue;
    std::array<std::thread,producers> threads;
    std::array<uint64_t,producers> accepted{};
    std::atomic<size_t> finished{};
    std::barrier start(static_cast<std::ptrdiff_t>(producers+1));
    for(size_t p=0;p<producers;++p)threads[p]=std::thread([&,p]{
        start.arrive_and_wait();
        for(uint64_t step=1;step<=attempts;++step) {
            std::array<NativePassageRow,2> rows{row(static_cast<unsigned>(p+1),step),row(static_cast<unsigned>(p+1),step)};
            rows[1].reason=99;
            if(queue.tryPublish(rows)==NativePassagePublish::Accepted)++accepted[p];
            if(step%32==0)std::this_thread::yield();
        }
        finished.fetch_add(1,std::memory_order_release);
    });
    start.arrive_and_wait();
    NativePassageBatch output{};uint64_t consumed{},expectedSequence=1;
    std::array<uint64_t,producers> previousStep{};
    bool valid=true,timedOut=false;
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(10);
    const auto consume=[&]{
        const auto result=queue.tryConsume(output);
        if(result!=NativePassageConsume::Ready)return result;
        ++consumed;valid&=output.sequence==expectedSequence++&&output.count==2;
        const auto& first=output.rows[0];const auto& second=output.rows[1];
        const auto p=static_cast<size_t>((first.train>>16)&0xffffffff)-1;
        if(p>=producers){valid=false;return result;}
        valid&=first.threadStepSequence>previousStep[p]&&first.signal==id(8,static_cast<unsigned>(p+1))&&
            first.track==id(1,static_cast<unsigned>(p+1))&&first.flags==NativePassageRow::NumericComplete&&
            first.beforeHead==double(first.threadStepSequence)&&first.boundaryHead==first.beforeHead+.5&&
            first.afterHead==first.beforeHead+1&&first.beforeFraction==.25&&first.finalFraction==.75&&
            first.reason==3&&second.reason==99&&second.train==first.train&&second.threadStepSequence==first.threadStepSequence&&
            second.signal==first.signal&&second.track==first.track&&second.flags==first.flags&&
            second.outcome==first.outcome&&second.beforeHead==first.beforeHead&&second.boundaryHead==first.boundaryHead&&
            second.afterHead==first.afterHead&&second.beforeFraction==first.beforeFraction&&second.finalFraction==first.finalFraction;
        previousStep[p]=first.threadStepSequence;
        return result;
    };
    while(finished.load(std::memory_order_acquire)!=producers) {
        if(consume()!=NativePassageConsume::Ready)std::this_thread::yield();
        if(std::chrono::steady_clock::now()>deadline){timedOut=true;break;}
    }
    for(auto& thread:threads)thread.join();
    for(size_t i=0;i<NativePassageMailbox::slotCapacity;++i)
        if(consume()==NativePassageConsume::Empty)break;
    CHECK(queue.tryConsume(output)==NativePassageConsume::Empty);
    uint64_t acceptedTotal{};for(const auto count:accepted)acceptedTotal+=count;
    const auto stats=queue.stats();
    CHECK(!timedOut&&valid&&acceptedTotal>0&&consumed==acceptedTotal&&stats.accepted==consumed&&stats.acceptedRows==consumed*2);
    CHECK(stats.accepted+stats.busy+stats.full==producers*attempts&&stats.invalid==0&&stats.lost==stats.busy+stats.full);
    CHECK(queue.lossObserved()==(stats.lost!=0));
}
}
int main() {
    try {
        fifoAndOwnedSlots();saturationAndValidation();noAllocationAndBoundedMemory();concurrentProducersAndConsumer();
        std::cout<<"PASS: "<<checks<<" native passage mailbox checks; bounded diagnostic records, never circulation proofs\n";
    } catch(const std::exception& error){watchAllocations.store(false);std::cerr<<error.what()<<'\n';return 1;}
}
