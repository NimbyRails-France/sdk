#pragma once
// Synthetic read-only process memory shared by ABI and authority regressions.
#include <platform/observation_process.h>
#include <engine/simulation_clock.h>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <map>
#include <memory>
#include <mutex>
using namespace std::chrono_literals;
namespace fixture {
constexpr uint64_t base=0x140000000,root=0x20000000,db=0x30000000,network=0x40000000,simulation=0x50000000;
struct World {
    std::atomic<unsigned char> identity{42};
    std::atomic<int64_t> ticks{10000};
    std::atomic<bool> emptyTrainPools{false};
    // Optional immutable regions for integration fixtures; populated before
    // capture starts, never changed concurrently with an observation.
    std::map<uint64_t,std::vector<unsigned char>> regions;
    std::atomic<uint64_t> refusedAddress{0},refusedReads{0};
    template<class T>void put(uint64_t address,size_t offset,T value){
        auto& bytes=regions[address];if(bytes.size()<offset+sizeof value)bytes.resize(offset+sizeof value);
        std::memcpy(bytes.data()+offset,&value,sizeof value);
    }
};
struct Connection {
    std::atomic<uint64_t> reads{0},blockAt{0};
    std::mutex mutex;
    std::condition_variable event;
    bool entered=false,released=false;
    void arm(uint64_t afterReads=1) { std::lock_guard lock(mutex);entered=released=false;blockAt=reads+afterReads; }
    bool wait() { std::unique_lock lock(mutex);return event.wait_for(lock,2s,[&]{return entered;}); }
    void release() { std::lock_guard lock(mutex);released=true;event.notify_all(); }
    bool delay(uint64_t number) noexcept {
        if(number!=blockAt.load())return true;
        std::unique_lock lock(mutex);entered=true;event.notify_all();
        return event.wait_for(lock,5s,[&]{return released;});
    }
};
std::mutex worldsMutex;
std::map<uint32_t,std::shared_ptr<World>> worlds;
std::map<uint32_t,std::shared_ptr<Connection>> latest;
std::shared_ptr<Connection> connection(uint32_t pid) { std::lock_guard lock(worldsMutex);return latest.at(pid); }
std::shared_ptr<World> world(uint32_t pid) { std::lock_guard lock(worldsMutex);return worlds.at(pid); }
template<class T> bool copy(void* out,size_t bytes,const T& value) {
    if(bytes!=sizeof value)return false;
    std::memcpy(out,&value,bytes);return true;
}
}
namespace nimby::platform {
struct ObservationProcess::Impl { std::shared_ptr<fixture::World> world;std::shared_ptr<fixture::Connection> connection; };
ObservationProcess::ObservationProcess():impl_(std::make_unique<Impl>()) {}
ObservationProcess::~ObservationProcess()=default;
uint32_t ObservationProcess::open(uint32_t requestedPid) {
    std::lock_guard lock(fixture::worldsMutex);
    auto& world=fixture::worlds[requestedPid];if(!world)world=std::make_shared<fixture::World>();
    impl_->world=world;impl_->connection=std::make_shared<fixture::Connection>();fixture::latest[requestedPid]=impl_->connection;
    pid=requestedPid;base=fixture::base;return NIMBY_OK;
}
bool ObservationProcess::alive() const noexcept { return bool(impl_->world); }
engine::LiveStateProfile ObservationProcess::profile() noexcept { return engine::LiveStateProfile::Windows119; }
bool ObservationProcess::read(uint64_t address,void* out,size_t bytes) const noexcept {
    const auto n=++impl_->connection->reads;
    bool valid=false;
    if(address==impl_->world->refusedAddress){++impl_->world->refusedReads;}
    else if(address==fixture::base+0xb81998)valid=fixture::copy(out,bytes,fixture::root);
    else if(address==fixture::root+0x540)valid=fixture::copy(out,bytes,fixture::db);
    else if(address==fixture::root+0x5c0)valid=fixture::copy(out,bytes,fixture::network);
    else if(address==fixture::root+0x680)valid=fixture::copy(out,bytes,fixture::simulation);
    else if(address==fixture::db+0xa48) {
        std::array<unsigned char,0x38> header{};header[0]=impl_->world->identity;
        valid=fixture::copy(out,bytes,header);
    } else if(address==fixture::simulation+0x20) {
        const engine::SimulationClock clock{0,impl_->world->ticks.load()};valid=fixture::copy(out,bytes,clock);
    } else if(impl_->world->emptyTrainPools&&
              (address==fixture::db+0x200||address==fixture::simulation+0xa0)) {
        std::array<unsigned char,48> header{};
        const uint32_t shift=1,size=2,mask=1;
        std::memcpy(header.data()+4,&shift,4);std::memcpy(header.data()+8,&size,4);
        std::memcpy(header.data()+16,&mask,4);
        valid=fixture::copy(out,bytes,header);
    } else if(!impl_->world->regions.empty()) {
        const auto& regions=impl_->world->regions;auto found=regions.upper_bound(address);
        if(found!=regions.begin()){
            --found;const auto offset=address-found->first;
            if(offset<=found->second.size()&&bytes<=found->second.size()-offset){
                std::memcpy(out,found->second.data()+offset,bytes);valid=true;
            }
        }
    }
    // Stall after copying: a complete old observation may finish after a newer
    // capture on another connection. No fake-memory lock hides that race.
    return impl_->connection->delay(n)&&valid;
}
uint32_t ObservationProcess::setClock(int64_t,NimbySimulationClock&) noexcept { return NIMBY_CLOCK_WRITE_FAILED; }
uint32_t ObservationProcess::setClockAndRecalculate(int64_t,NimbySimulationClock&,uint32_t&) noexcept { return NIMBY_CLOCK_WRITE_FAILED; }
uint32_t ObservationProcess::construction(const NimbyConstructionRequest*,uint64_t,NimbyConstructionResult&) noexcept { return NIMBY_DATA_UNAVAILABLE; }
}
