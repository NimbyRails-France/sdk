#pragma once
#include <nimby/detail/diagnostics.hpp>
#include <windows.h>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string_view>

namespace nimby::platform::windows::train_length::diagnostics {
enum class Level { Info, Warning, Error };

// A private, resident bridge writer. Initialize before installing game hooks.
// Producers only copy owned text into fixed storage; disk access and throttling
// happen on a precreated worker. Destruction requires producers to be stopped.
class Writer {
public:
    static constexpr size_t capacity=128;
    static constexpr size_t maximumMessageBytes=16*1024;
    static constexpr size_t maximumWritesPerSecond=12;
    using Sink=void(*)(void*,const char*,const char*) noexcept;
private:
    struct Message { Level level{}; std::array<char,maximumMessageBytes+1> text{}; };
    std::array<Message,capacity> queue_{};
    SRWLOCK queueLock_=SRWLOCK_INIT;
    size_t first_{},size_{};
    std::atomic<size_t> queued_{};
    std::atomic<uint64_t> dropped_{},pendingDrops_{},callbacks_{};
    std::atomic<bool> ready_{},scheduled_{},timerArmed_{};
    PTP_WORK work_{};
    PTP_TIMER timer_{};
    Sink sink_{};
    void* context_{};
    std::array<uint64_t,maximumWritesPerSecond> writtenAt_{};
    size_t writtenCount_{},oldest_{},sinceSummary_{};

    static const char* label(Level level) noexcept {
        switch(level){case Level::Warning:return "WARN";case Level::Error:return "ERROR";default:return "INFO";}
    }
    static void defaultSink(void*,const char* level,const char* text) noexcept {
        nimby::detail::diagnostics::write("sdk",level,text);
    }
    bool pending() const noexcept {
        return queued_.load(std::memory_order_acquire)||pendingDrops_.load(std::memory_order_acquire);
    }
    void wake() noexcept {
        if(!ready_.load(std::memory_order_acquire)||timerArmed_.load(std::memory_order_acquire))return;
        bool expected=false;
        if(scheduled_.compare_exchange_strong(expected,true,std::memory_order_acq_rel))SubmitThreadpoolWork(work_);
    }
    bool drop() noexcept {
        dropped_.fetch_add(1,std::memory_order_relaxed);
        pendingDrops_.fetch_add(1,std::memory_order_release);
        wake();
        return false;
    }
    static void CALLBACK workCallback(PTP_CALLBACK_INSTANCE,void* context,PTP_WORK) noexcept {
        static_cast<Writer*>(context)->drain();
    }
    static void CALLBACK timerCallback(PTP_CALLBACK_INSTANCE,void* context,PTP_TIMER) noexcept {
        auto& writer=*static_cast<Writer*>(context);
        writer.timerArmed_.store(false,std::memory_order_release);
        writer.wake();
    }
    void rateWait(uint64_t delayMilliseconds) noexcept {
        const int64_t ticks=-static_cast<int64_t>(delayMilliseconds?delayMilliseconds:1)*10000;
        FILETIME due{};
        std::memcpy(&due,&ticks,sizeof due);
        timerArmed_.store(true,std::memory_order_release);
        SetThreadpoolTimer(timer_,&due,0,0);
    }
    void recordWrite(uint64_t now) noexcept {
        if(writtenCount_<maximumWritesPerSecond)writtenAt_[writtenCount_++]=now;
        else {writtenAt_[oldest_]=now;oldest_=(oldest_+1)%maximumWritesPerSecond;}
    }
    void drain() noexcept {
        callbacks_.fetch_add(1,std::memory_order_relaxed);
        Message message;
        while(ready_.load(std::memory_order_acquire)&&pending()){
            const auto now=GetTickCount64();
            if(writtenCount_==maximumWritesPerSecond&&now-writtenAt_[oldest_]<1000){
                rateWait(1000-(now-writtenAt_[oldest_]));
                break;
            }
            // Summaries consume the same rate budget. During a flood preserve
            // eleven actual events for each coalesced loss report.
            if(pendingDrops_.load(std::memory_order_acquire)&&
               (sinceSummary_>=maximumWritesPerSecond-1||!queued_.load(std::memory_order_acquire))){
                const auto lost=pendingDrops_.exchange(0,std::memory_order_acq_rel);
                if(lost){
                    std::snprintf(message.text.data(),message.text.size(),
                        "Train length diagnostics dropped: %llu events (queue full, producer contention or oversized message); total=%llu",
                        static_cast<unsigned long long>(lost),static_cast<unsigned long long>(dropped_.load(std::memory_order_relaxed)));
                    message.level=Level::Warning;
                    sinceSummary_=0;
                }else continue;
            }else {
                AcquireSRWLockExclusive(&queueLock_);
                if(!size_){ReleaseSRWLockExclusive(&queueLock_);continue;}
                message=queue_[first_];
                first_=(first_+1)%capacity;
                --size_;
                queued_.store(size_,std::memory_order_release);
                ReleaseSRWLockExclusive(&queueLock_);
                ++sinceSummary_;
            }
            sink_(context_,label(message.level),message.text.data());
            // Account after the sink so a slow write only makes the limiter
            // more conservative; it cannot move two bursts closer together.
            recordWrite(GetTickCount64());
        }
        scheduled_.store(false,std::memory_order_release);
        // A producer can arrive between observing an empty queue and retiring
        // this worker. Rechecking after clearing scheduled closes that race.
        if(pending()&&!timerArmed_.load(std::memory_order_acquire))wake();
    }
    friend struct WriterTestAccess;
public:
    Writer()=default;
    Writer(const Writer&)=delete;
    Writer& operator=(const Writer&)=delete;
    ~Writer(){shutdown();}

    // Called once on a normal installation worker, never DllMain/game callback.
    bool initialize(Sink sink=nullptr,void* context=nullptr) noexcept {
        if(work_||timer_||ready_.load(std::memory_order_relaxed))return false;
        HMODULE module{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
            reinterpret_cast<LPCWSTR>(&workCallback),&module))return false;
        sink_=sink?sink:&defaultSink;
        context_=context;
        work_=CreateThreadpoolWork(&workCallback,this,nullptr);
        if(!work_)return false;
        timer_=CreateThreadpoolTimer(&timerCallback,this,nullptr);
        if(!timer_){CloseThreadpoolWork(work_);work_=nullptr;return false;}
        ready_.store(true,std::memory_order_release);
        return true;
    }
    bool publish(Level level,std::string_view text) noexcept {
        if(!ready_.load(std::memory_order_acquire))return false;
        if(text.size()>maximumMessageBytes)return drop();
        if(!TryAcquireSRWLockExclusive(&queueLock_))return drop();
        if(size_==capacity){ReleaseSRWLockExclusive(&queueLock_);return drop();}
        auto& message=queue_[(first_+size_)%capacity];
        message.level=level;
        if(!text.empty())std::memcpy(message.text.data(),text.data(),text.size());
        message.text[text.size()]='\0';
        ++size_;
        queued_.store(size_,std::memory_order_release);
        ReleaseSRWLockExclusive(&queueLock_);
        wake();
        return true;
    }
    uint64_t dropped() const noexcept{return dropped_.load(std::memory_order_relaxed);}
    size_t queued() const noexcept{return queued_.load(std::memory_order_acquire);}
    uint64_t callbacks() const noexcept{return callbacks_.load(std::memory_order_relaxed);}
    bool idle() const noexcept {
        return !pending()&&!scheduled_.load(std::memory_order_acquire)&&!timerArmed_.load(std::memory_order_acquire);
    }
    // Used by test-owned instances only. The production singleton is pinned and
    // deliberately remains alive for the lifetime of the game process.
    void shutdown() noexcept {
        ready_.store(false,std::memory_order_release);
        // Retire timer callbacks before work: a timer already past wake's ready
        // check can still submit work while shutdown is starting.
        if(timer_){SetThreadpoolTimer(timer_,nullptr,0,0);WaitForThreadpoolTimerCallbacks(timer_,TRUE);}
        if(work_)WaitForThreadpoolWorkCallbacks(work_,TRUE);
        // A callback that was already running may have armed the pending-only
        // timer after the first cancellation. Retire work before cancelling it.
        if(timer_){SetThreadpoolTimer(timer_,nullptr,0,0);WaitForThreadpoolTimerCallbacks(timer_,TRUE);}
        if(timer_){CloseThreadpoolTimer(timer_);timer_=nullptr;}
        if(work_){CloseThreadpoolWork(work_);work_=nullptr;}
    }
};
inline Writer& writer(){static auto* instance=new Writer;return *instance;}
}
