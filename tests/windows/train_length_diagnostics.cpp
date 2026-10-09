#include <platform/windows/train_length_diagnostics.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace diagnostics=nimby::platform::windows::train_length::diagnostics;
namespace nimby::platform::windows::train_length::diagnostics {
struct WriterTestAccess {
    static void lock(Writer& writer){AcquireSRWLockExclusive(&writer.queueLock_);}
    static void unlock(Writer& writer){ReleaseSRWLockExclusive(&writer.queueLock_);}
};
}
namespace {
using namespace std::chrono_literals;
struct Event {std::string level,text;uint64_t at;};
struct Capture {
    std::mutex mutex;
    std::condition_variable changed;
    std::vector<Event> events;
    bool block{},entered{},release{};
    static void sink(void* context,const char* level,const char* message) noexcept {
        auto& self=*static_cast<Capture*>(context);
        std::unique_lock lock(self.mutex);
        self.events.push_back({level,message,GetTickCount64()});
        self.entered=true;
        self.changed.notify_all();
        if(self.block)self.changed.wait(lock,[&]{return self.release;});
    }
    void await(size_t count,std::chrono::milliseconds timeout=2s){
        std::unique_lock lock(mutex);
        assert(changed.wait_for(lock,timeout,[&]{return events.size()>=count;}));
    }
    std::vector<Event> snapshot(){std::lock_guard lock(mutex);return events;}
    void unblock(){std::lock_guard lock(mutex);release=true;changed.notify_all();}
};
template<class F> void await(F&& predicate,std::chrono::milliseconds timeout=2s){
    const auto end=std::chrono::steady_clock::now()+timeout;
    while(!predicate()&&std::chrono::steady_clock::now()<end)std::this_thread::sleep_for(1ms);
    assert(predicate());
}
void copied_text_and_idle(){
    Capture capture;
    auto writer=std::make_unique<diagnostics::Writer>();
    assert(!writer->publish(diagnostics::Level::Info,"before initialization"));
    assert(writer->initialize(&Capture::sink,&capture));
    assert(!writer->initialize(&Capture::sink,&capture));
    auto text=std::string(diagnostics::Writer::maximumMessageBytes,'A');
    assert(writer->publish(diagnostics::Level::Error,text));
    std::fill(text.begin(),text.end(),'B');
    capture.await(1);
    await([&]{return writer->idle();});
    const auto events=capture.snapshot();
    assert(events.size()==1&&events[0].level=="ERROR");
    assert(events[0].text.size()==diagnostics::Writer::maximumMessageBytes);
    assert(std::all_of(events[0].text.begin(),events[0].text.end(),[](char value){return value=='A';}));
    const auto callbacks=writer->callbacks();
    std::this_thread::sleep_for(150ms);
    assert(writer->idle()&&writer->callbacks()==callbacks&&capture.snapshot().size()==1);
}
void contention_and_oversize_are_reported(){
    Capture capture;
    auto writer=std::make_unique<diagnostics::Writer>();
    assert(writer->initialize(&Capture::sink,&capture));
    diagnostics::WriterTestAccess::lock(*writer);
    bool accepted=true;
    const auto start=std::chrono::steady_clock::now();
    std::thread producer([&]{accepted=writer->publish(diagnostics::Level::Info,"busy");});
    producer.join();
    diagnostics::WriterTestAccess::unlock(*writer);
    assert(!accepted&&std::chrono::steady_clock::now()-start<100ms);
    assert(!writer->publish(diagnostics::Level::Info,std::string(diagnostics::Writer::maximumMessageBytes+1,'X')));
    await([&]{return writer->idle();});
    assert(writer->dropped()==2&&writer->queued()==0);
    const auto events=capture.snapshot();
    uint64_t reported{};
    for(const auto& event:events){
        assert(event.level=="WARN");
        unsigned long long count{};
        assert(std::sscanf(event.text.c_str(),"Train length diagnostics dropped: %llu events",&count)==1);
        reported+=count;
    }
    assert(reported==2);
}
void bounded_queue_and_rate(){
    Capture capture;
    capture.block=true;
    auto writer=std::make_unique<diagnostics::Writer>();
    assert(writer->initialize(&Capture::sink,&capture));
    assert(writer->publish(diagnostics::Level::Info,"event 0"));
    capture.await(1);
    for(size_t i=1;i<=diagnostics::Writer::capacity;++i)
        assert(writer->publish(diagnostics::Level::Info,"event "+std::to_string(i)));
    assert(writer->queued()==diagnostics::Writer::capacity);
    const auto start=std::chrono::steady_clock::now();
    assert(!writer->publish(diagnostics::Level::Info,"overflow"));
    assert(std::chrono::steady_clock::now()-start<50ms);
    assert(writer->dropped()==1);
    capture.unblock();
    capture.await(diagnostics::Writer::capacity+2,15s);
    await([&]{return writer->idle();});
    const auto events=capture.snapshot();
    assert(events.size()==diagnostics::Writer::capacity+2);
    size_t normal{},summaries{};
    for(size_t i=0;i<events.size();++i){
        if(events[i].text.starts_with("event "))++normal;
        else {++summaries;assert(events[i].level=="WARN"&&events[i].text.find("1 events")!=std::string::npos);}
        if(i>=diagnostics::Writer::maximumWritesPerSecond)
            assert(events[i].at-events[i-diagnostics::Writer::maximumWritesPerSecond].at>=1000);
    }
    assert(normal==diagnostics::Writer::capacity+1&&summaries==1);
    const auto callbacks=writer->callbacks();
    std::this_thread::sleep_for(100ms);
    assert(writer->callbacks()==callbacks&&writer->idle());
}
void pending_timer_can_be_retired(){
    Capture capture;
    auto writer=std::make_unique<diagnostics::Writer>();
    assert(writer->initialize(&Capture::sink,&capture));
    // Block the first sink to fill a deterministic batch without lock contention.
    capture.block=true;
    assert(writer->publish(diagnostics::Level::Info,"first"));
    capture.await(1);
    for(size_t i=0;i<20;++i)assert(writer->publish(diagnostics::Level::Info,"pending"));
    capture.unblock();
    capture.await(diagnostics::Writer::maximumWritesPerSecond);
    await([&]{return writer->queued()==9;});
    const auto start=std::chrono::steady_clock::now();
    writer->shutdown();
    assert(std::chrono::steady_clock::now()-start<100ms);
    const auto count=capture.snapshot().size();
    assert(count==diagnostics::Writer::maximumWritesPerSecond);
    std::this_thread::sleep_for(1050ms);
    assert(capture.snapshot().size()==count);
    assert(!writer->publish(diagnostics::Level::Info,"stopped"));
}
void concurrent_producers_stay_bounded(){
    Capture capture;capture.block=true;
    auto writer=std::make_unique<diagnostics::Writer>();
    assert(writer->initialize(&Capture::sink,&capture));
    assert(writer->publish(diagnostics::Level::Info,"block consumer"));
    capture.await(1);
    std::atomic<size_t> accepted{};
    std::vector<std::thread> producers;
    constexpr size_t workers=8,attempts=300;
    for(size_t i=0;i<workers;++i)producers.emplace_back([&]{
        for(size_t n=0;n<attempts;++n)if(writer->publish(diagnostics::Level::Info,"concurrent"))++accepted;
    });
    for(auto& producer:producers)producer.join();
    assert(accepted<=diagnostics::Writer::capacity);
    assert(writer->queued()==accepted&&accepted+writer->dropped()==workers*attempts);
    capture.unblock();
    // Lifecycle cancellation may discard queued diagnostics, never game work.
    // Full normal draining and loss summary delivery are covered above.
    writer->shutdown();
}
}
int main(){
    copied_text_and_idle();
    contention_and_oversize_are_reported();
    bounded_queue_and_rate();
    pending_timer_can_be_retired();
    concurrent_producers_stay_bounded();
    std::cout<<"Train length asynchronous diagnostics passed\n";
}
