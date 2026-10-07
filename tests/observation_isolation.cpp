// Production observation ABI with an owned, deterministic process connection.
// Only the OS memory boundary is replaced: registry, capture, epoch, copies and
// lifecycle use the same implementation as the SDK DLL. Never touches the game.
#include <nimby/detail/observation.h>
#include <nimby/observation_loop.hpp>
#include "platform/observation_process.h"
#include "engine/simulation_clock.h"
#include "engine/detail/texture_name_hash.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <future>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std::chrono_literals;
using Clock = std::chrono::steady_clock;
#define CHECK(x) do { if(!(x)) throw std::runtime_error("line " + std::to_string(__LINE__) + ": " #x); } while(false)
#include "observation_process_fixture.hpp"

namespace {
struct Session {
    NimbySession value{};
    explicit Session(uint32_t pid) { CHECK(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,pid,&value)==NIMBY_OK); }
    ~Session() { if(value)NimbyInternal_CloseSession(value); }
};
struct Capture {
    uint32_t status{},stage{};NimbySnapshot snapshot{};uint64_t generation{};
};
Capture capture(NimbySession session,bool release=true) {
    Capture out;out.status=NimbyInternal_CaptureSessionSnapshot(session,&out.snapshot,&out.stage);
    if(out.status==NIMBY_OK) {
        NimbyGameSession world{};world.struct_size=sizeof world;
        CHECK(NimbyInternal_GetGameSession(out.snapshot,&world)==NIMBY_OK);out.generation=world.generation;
        NimbySimulationClock clock{};clock.struct_size=sizeof clock;
        CHECK(NimbyInternal_GetSimulationClock(out.snapshot,&clock)==NIMBY_OK);
        if(release)CHECK(NimbyInternal_ReleaseSnapshot(out.snapshot)==NIMBY_OK);
    }
    return out;
}
std::vector<double> samples(NimbySession session,unsigned count=64,std::chrono::milliseconds pause=0ms) {
    std::vector<double> result;result.reserve(count);
    for(unsigned i=0;i<count;++i) {
        const auto start=Clock::now();CHECK(capture(session).status==NIMBY_OK);
        result.push_back(std::chrono::duration<double,std::micro>(Clock::now()-start).count());
        if(pause.count())std::this_thread::sleep_for(pause);
    }
    return result;
}
void report(const char* scenario,std::vector<double> times) {
    CHECK(!times.empty());std::sort(times.begin(),times.end());
    const auto percentile=[&](unsigned p){return times[std::min(times.size()-1,(times.size()*p+99)/100-1)];};
    std::cout<<"{\"scenario\":\""<<scenario<<"\",\"samples\":"<<times.size()<<",\"p50_us\":"<<percentile(50)
        <<",\"p95_us\":"<<percentile(95)<<",\"p99_us\":"<<percentile(99)<<",\"max_us\":"<<times.back()<<"}\n";
}
void blockedRead(bool baseline) {
    Session healthy(101),slow(102);const auto gate=fixture::connection(102);
    report("healthy_baseline",samples(healthy.value));gate->arm();
    auto offender=std::async(std::launch::async,[&]{return capture(slow.value);});
    if(!gate->wait()){gate->release();offender.wait();CHECK(false);}
    auto good=std::async(std::launch::async,[&]{return samples(healthy.value);});
    const bool progressed=good.wait_for(350ms)==std::future_status::ready;
    gate->release();CHECK(offender.get().status==NIMBY_OK);report("stalled_peer",good.get());
    std::cout<<"{\"healthy_completed_while_peer_stalled\":"<<(progressed?"true":"false")<<"}\n";
    if(!baseline)CHECK(progressed);
}
void faultyLoop() {
    Session healthy(103),faulty(104);nimby::ObservationLoop bad;
    std::atomic<unsigned> unexpected=0;
    bad.start([&]{if(capture(faulty.value).status!=NIMBY_OK)++unexpected;throw std::runtime_error("injected mod callback failure");},[]{},10ms);
    report("throwing_peer",samples(healthy.value,32,10ms));bad.stop();
    CHECK(unexpected==0&&bad.status().failures>0&&bad.status().successes==0);
    // An overrun is skipped, with no backlog that can flood either SDK session.
    std::atomic<unsigned> calls=0;nimby::ObservationLoop slow;
    slow.start([&]{++calls;CHECK(capture(faulty.value).status==NIMBY_OK);std::this_thread::sleep_for(40ms);},[]{},10ms);
    report("slow_callback_peer",samples(healthy.value,32,10ms));slow.stop();
    CHECK(calls>=1&&slow.status().failures==0);
    std::cout<<"{\"injected_exceptions\":"<<bad.status().failures<<",\"overrunning_callback_calls\":"<<calls<<"}\n";
}
void retainedSnapshots(bool baseline=false) {
    Session offender(105),healthy(106);std::vector<NimbySnapshot> held;
    uint32_t status=NIMBY_OK;
    for(unsigned i=0;i<64;++i){auto out=capture(offender.value,false);status=out.status;if(status!=NIMBY_OK)break;held.push_back(out.snapshot);}
    CHECK(status==NIMBY_RESOURCE_LIMIT&&!held.empty()&&held.size()<64);
    const auto otherStatus=capture(healthy.value).status;
    if(!baseline){CHECK(otherStatus==NIMBY_OK);report("snapshot_flood_peer",samples(healthy.value));}
    CHECK(NimbyInternal_CloseSession(offender.value)==NIMBY_OK);offender.value=0;
    for(const auto snapshot:held) {
        NimbySnapshotInfo info{};info.struct_size=sizeof info;CHECK(NimbyInternal_GetSnapshotInfo(snapshot,&info)==NIMBY_OK);
        CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
        CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_INVALID_HANDLE);
    }
    CHECK(capture(healthy.value).status==NIMBY_OK);
    std::cout<<"{\"retained_snapshots_at_limit\":"<<held.size()<<",\"other_session_status\":"<<otherStatus
        <<",\"other_session_available\":"<<(otherStatus==NIMBY_OK?"true":"false")<<"}\n";
}
void closeDuringCapture() {
    Session closing(107),healthy(108);auto gate=fixture::connection(107);gate->arm();
    const auto handle=closing.value;auto pending=std::async(std::launch::async,[&]{return capture(handle);});
    if(!gate->wait()){gate->release();pending.wait();CHECK(false);}
    auto close=std::async(std::launch::async,[&]{return NimbyInternal_CloseSession(handle);});
    const bool detached=close.wait_for(350ms)==std::future_status::ready;
    gate->release();CHECK(close.get()==NIMBY_OK);closing.value=0;const auto result=pending.get();
    CHECK(detached&&result.status==NIMBY_INVALID_HANDLE&&result.snapshot==0);
    CHECK(capture(handle).status==NIMBY_INVALID_HANDLE&&capture(healthy.value).status==NIMBY_OK);
}
void epochOrdering(bool changeWorld) {
    const uint32_t pid=changeWorld?109:110;Session old(pid);auto oldConnection=fixture::connection(pid);Session newer(pid);
    const auto beginReads=oldConnection->reads.load();const auto initial=capture(old.value);
    CHECK(initial.status==NIMBY_OK);const auto readsPerCapture=oldConnection->reads.load()-beginReads;
    CHECK(readsPerCapture>0);oldConnection->arm(readsPerCapture);
    auto late=std::async(std::launch::async,[&]{return capture(old.value);});
    if(!oldConnection->wait()){oldConnection->release();late.wait();CHECK(false);}
    auto world=fixture::world(pid);if(changeWorld)world->identity=43;world->ticks=20000;
    auto recent=std::async(std::launch::async,[&]{return capture(newer.value);});
    const bool progressed=recent.wait_for(350ms)==std::future_status::ready;
    oldConnection->release();const auto current=recent.get();const auto previous=late.get();
    CHECK(progressed&&current.status==NIMBY_OK);
    CHECK(current.generation==(changeWorld?initial.generation+1:initial.generation));
    CHECK(previous.status==NIMBY_DATA_UNAVAILABLE&&previous.snapshot==0);
    CHECK(capture(newer.value).generation==current.generation);
    CHECK(capture(old.value).generation==current.generation);
    // A sequential, confirmed time rewind still changes the observation epoch.
    world->ticks=100;const auto rewind=capture(newer.value);CHECK(rewind.status==NIMBY_OK&&rewind.generation==current.generation+1);
}
void trainQueryContract() {
    Session session(111);fixture::world(111)->emptyTrainPools=true;
    const auto connection=fixture::connection(111);
    NimbySnapshot snapshot=123;uint32_t stage=123,count=123;
    const auto before=connection->reads.load();
    CHECK(NimbyInternal_CaptureTrainDataSnapshotWithOptions(session.value,256,&snapshot,&stage)==NIMBY_INVALID_ARGUMENT);
    CHECK(snapshot==0&&stage==0&&connection->reads==before);
    CHECK(NimbyInternal_CaptureTrainDataSnapshotWithOptions(session.value,0,nullptr,&stage)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CaptureTrainDataSnapshotWithOptions(session.value,0,&snapshot,nullptr)==NIMBY_INVALID_ARGUMENT);
    // This world deliberately provides no network/texture/line pool. Every
    // optional train query must still capture the stable, empty train set.
    for(const auto flags:{0u,1u,2u,4u,8u,16u,32u,64u,128u,NIMBY_TRAIN_DATA_ALL}){
        CHECK(NimbyInternal_CaptureTrainDataSnapshotWithOptions(session.value,flags,&snapshot,&stage)==NIMBY_OK);
        NimbySnapshotInfo info{};info.struct_size=sizeof info;
        CHECK(NimbyInternal_GetSnapshotInfo(snapshot,&info)==NIMBY_OK);
        CHECK(info.train_count==0&&info.track_count==0&&info.signal_count==0);
        CHECK(NimbyInternal_CopyTrainMetadata(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
        CHECK(NimbyInternal_CopyTrainVehicles(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
        CHECK(NimbyInternal_CopyTrainVehicles(snapshot,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
        CHECK(NimbyInternal_CopyVehicleModels(snapshot,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
        CHECK(NimbyInternal_CopyTrackOccupations(snapshot,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
        CHECK(NimbyInternal_CopyTrackReservations(snapshot,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
        CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
        CHECK(NimbyInternal_CopyTrainVehicles(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE);
    }
}
void foreignTextureFault(){
    Session session(112);auto world=fixture::world(112);world->emptyTrainPools=true;
    constexpr uint64_t table=0x60000000,node=0x61000000,foreign=0x62000000;
    const auto hash=nimby::engine::detail::textureNameHash("test_atlas");
    constexpr uint64_t buckets=257;const auto bucket=hash%buckets,other=(bucket+1)%buckets;
    const auto descriptor=fixture::db+0xa80+0x138;
    world->regions[descriptor].resize(32);world->put(descriptor,8,table);
    world->put(descriptor,16,buckets);world->put(descriptor,24,uint64_t(2));
    world->regions[table].resize((buckets+1)*8);world->put(table,bucket*8,node);world->put(table,other*8,foreign);
    world->regions[node].resize(0xa0);world->put(node,0,hash);
    std::memcpy(world->regions[node].data()+8,"test_atlas",11);world->put(node,24,uint64_t(10));world->put(node,32,uint64_t(15));
    world->refusedAddress=foreign; // Another mod's catalogue node is unreadable.
    const auto signals=fixture::db+0x380;world->regions[signals].resize(48);
    world->put(signals,4,uint32_t(1));world->put(signals,8,uint32_t(2));world->put(signals,16,uint32_t(1));
    NimbySignalCaptureScope scope{};std::strcpy(scope.texture_set,"test_atlas");scope.approach_blocks=2;
    NimbySnapshot snapshot{};uint32_t stage{};
    CHECK(NimbyInternal_CaptureSignallingScope(session.value,&scope,1,&snapshot,&stage)==NIMBY_OK);
    CHECK(world->refusedReads==0&&snapshot);
    CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
    world->put(node,0x88,uint64_t(1)); // The requested model's corrupt vector still fails closed.
    CHECK(NimbyInternal_CaptureSignallingScope(session.value,&scope,1,&snapshot,&stage)==NIMBY_DATA_UNAVAILABLE&&snapshot==0);
    CHECK(world->refusedReads==0);
}
void topologyQueryContract(){
    Session session(113);auto world=fixture::world(113);const auto connection=fixture::connection(113);
    constexpr uint64_t tracks=0x70000000,signals=0x71000000;
    constexpr uint64_t track=0x1000000000001,signal=0x8000000000001;
    const auto pool=[&](uint64_t offset,uint64_t block,size_t stride,uint64_t id){
        const auto descriptor=fixture::db+offset,table=block-0x1000;
        world->regions[descriptor].resize(48);world->put(descriptor,4,uint32_t(1));
        world->put(descriptor,8,uint32_t(2));world->put(descriptor,16,uint32_t(1));
        world->put(descriptor,24,table);world->put(descriptor,32,table+8);world->put(descriptor,40,table+8);
        world->put(table,0,block);world->regions[block].resize(stride*2);world->put(block,0,id);
    };
    pool(0,tracks,0x4e8,track);pool(0x380,signals,0xc8,signal);
    world->put(tracks,0x88,1200.);
    world->put(signals,0x30,int32_t(4));world->put(signals,0x40,track);
    world->put(signals,0x48,.25);world->put(signals,0x50,int8_t(-1));
    NimbySnapshot snapshot=123;uint32_t stage=123,count=123;
    const auto before=connection->reads.load();
    CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,nullptr,&stage)==NIMBY_INVALID_ARGUMENT);
    CHECK(stage==0&&connection->reads==before);
    CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,&snapshot,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(snapshot==0&&connection->reads==before);
    // These pools are deliberately missing. Additionally count attempts at
    // their roots so an optional failed read cannot silently hide extra work.
    const auto& layout=nimby::engine::gameLayout(nimby::engine::LiveStateProfile::Windows119);
    for(const uint64_t forbidden:{fixture::db+0x80,fixture::db+0x200,fixture::simulation+0xa0,
                                  fixture::db+0xa80+0x138,fixture::simulation+layout.occupations,
                                  fixture::simulation+layout.reservations+layout.reservation_header}){
        world->refusedAddress=forbidden;world->refusedReads=0;
        CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,&snapshot,&stage)==NIMBY_OK);
        CHECK(world->refusedReads==0&&snapshot);
        NimbyTrackNode node{};NimbyTrackMetric metric{};NimbySignal observed{};
        CHECK(NimbyInternal_CopyTrackNodes(snapshot,&node,1,&count)==NIMBY_OK&&count==1&&node.id==track);
        CHECK(NimbyInternal_CopyTrackMetrics(snapshot,&metric,1,&count)==NIMBY_OK&&count==1&&metric.length_m==1200.);
        CHECK(NimbyInternal_CopyTrackJunctions(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
        CHECK(NimbyInternal_CopySignals(snapshot,&observed,1,&count)==NIMBY_OK&&count==1&&observed.id==signal&&observed.track_fraction==.25);
        CHECK(NimbyInternal_CopyTrains(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
        CHECK(NimbyInternal_CopyStations(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
        CHECK(NimbyInternal_CopySignalTextures(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
        CHECK(NimbyInternal_CopyTrackOccupations(snapshot,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
        CHECK(NimbyInternal_CopyTrackReservations(snapshot,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
        CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
    }
    world->refusedAddress=0;
    CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,&snapshot,&stage)==NIMBY_OK);
    const auto previous=snapshot;NimbyGameSession oldWorld{};oldWorld.struct_size=sizeof oldWorld;
    CHECK(NimbyInternal_GetGameSession(previous,&oldWorld)==NIMBY_OK);
    world->put(signals,0x48,.75);world->identity=43;
    CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,&snapshot,&stage)==NIMBY_OK);
    NimbySignal oldSignal{},freshSignal{};NimbyGameSession newWorld{};newWorld.struct_size=sizeof newWorld;
    CHECK(NimbyInternal_CopySignals(previous,&oldSignal,1,&count)==NIMBY_OK&&oldSignal.track_fraction==.25);
    CHECK(NimbyInternal_CopySignals(snapshot,&freshSignal,1,&count)==NIMBY_OK&&freshSignal.track_fraction==.75);
    CHECK(NimbyInternal_GetGameSession(snapshot,&newWorld)==NIMBY_OK&&newWorld.generation==oldWorld.generation+1);
    CHECK(NimbyInternal_ReleaseSnapshot(previous)==NIMBY_OK&&NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
    world->refusedAddress=fixture::db;
    CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,&snapshot,&stage)==NIMBY_DATA_UNAVAILABLE&&snapshot==0);
    world->refusedAddress=0;world->put(signals,0,uint64_t(0));
    CHECK(NimbyInternal_CaptureNetworkSnapshotDiagnostic(session.value,&snapshot,&stage)==NIMBY_OK);
    CHECK(NimbyInternal_CopySignals(snapshot,nullptr,0,&count)==NIMBY_OK&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
}
}
int main(int argc,char** argv) {
    try {
        const bool baseline=argc==2&&std::string(argv[1])=="--baseline";
        CHECK(argc==1||baseline);blockedRead(baseline);if(baseline){retainedSnapshots(true);return 0;}
        faultyLoop();retainedSnapshots();closeDuringCapture();epochOrdering(true);epochOrdering(false);trainQueryContract();foreignTextureFault();topologyQueryContract();
        std::cout<<"PASS actual observation ABI: stalled/throwing/slow/flooding peers, close during capture, snapshot lifetime, epoch completion order\n";
    } catch(const std::exception& error) { std::cerr<<error.what()<<'\n';return 1; }
}
