#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/automatic_driving.h>
#include <stdexcept>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <set>
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
std::atomic<uint64_t> publishedOwner{},constrainedOwner{};
std::atomic<uint32_t> publishedCount{},constrainedCount{};
std::atomic<bool> busyRules=false,busyConstraints=false;
std::atomic<unsigned> releaseAttempts=0,ruleSuccesses=0;
std::mutex releasedMutex;
std::set<uint64_t> releasedRules,releasedConstraints;
bool released(uint64_t owner){std::lock_guard lock(releasedMutex);return releasedRules.contains(owner)&&releasedConstraints.contains(owner);}
template<class Predicate> bool wait(Predicate predicate){
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(3);
    while(!predicate()&&std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(std::chrono::milliseconds(2));
    return predicate();
}
}
extern "C" uint32_t __cdecl NimbyInternal_PublishDrivingRulesV3(const NimbySignalDrivingRule*,uint32_t count,uint32_t,uint32_t,uint64_t owner) noexcept {
    publishedOwner=owner;publishedCount=count;
    if(!count){++releaseAttempts;if(busyRules)return NIMBY_RESOURCE_LIMIT;
        std::lock_guard lock(releasedMutex);releasedRules.insert(owner);++ruleSuccesses;}
    return NIMBY_OK;
}
extern "C" uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint*,uint32_t count,uint32_t,uint64_t owner) noexcept {
    constrainedOwner=owner;constrainedCount=count;
    if(!count){++releaseAttempts;if(busyConstraints)return NIMBY_RESOURCE_LIMIT;
        std::lock_guard lock(releasedMutex);releasedConstraints.insert(owner);}
    return NIMBY_OK;
}
extern "C" uint32_t __cdecl NimbyInternal_ReadTrainConstraint(uint64_t train,NimbyTrainConstraintStatus* out) noexcept {
    *out={sizeof *out,2,train,0x8000000000001,17,6.75};return NIMBY_OK;
}
int main(){try{
    using namespace nimby::mod_host;
    Owners owners;owners.driving=1234;Reply reply;Request request;
    NimbySignalDrivingRule rule{0x8000000000001,0,8,0,NIMBY_DRIVING_STOP};
    request.operation=200;request.args[0]=1;request.args[1]=1000;request.args[7]=9999;
    append(request,&rule);
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_OK&&publishedOwner==1234&&publishedCount==1);
    request.data.pop_back();CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    request.data.clear();request.args[0]=UINT64_MAX;CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    request.args[0]=0;request.args[1]=99;CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    request.args[1]=1000;request.args[2]=1ull<<40;CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    request={};request.operation=201;request.args[0]=1;request.args[1]=1000;request.args[7]=9999;
    NimbyTrainConstraint command{0x5000000000001,rule.signal,1,6.75,0,0};append(request,&command);
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_OK&&constrainedOwner==1234&&constrainedCount==1);
    request.data.push_back(0);CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    request={};request.operation=202;request.args[0]=command.train;
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_OK&&reply.data.size()==sizeof(NimbyTrainConstraintStatus));
    NimbyTrainConstraintStatus status{};std::memcpy(&status,reply.data.data(),sizeof status);
    CHECK(status.train==command.train&&status.state==2&&status.speed_mps==6.75);
    request.args[0]=rule.signal;CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT);
    busyRules=true;busyConstraints=true;
    const auto started=std::chrono::steady_clock::now();cleanupDriving(owners);
    CHECK(std::chrono::steady_clock::now()-started<std::chrono::milliseconds(50));
    CHECK(!owners.driving&&!owners.drivingTracked);
    CHECK(wait([]{return releaseAttempts>=4;}));CHECK(!released(1234));
    // Pending retirement reserves capacity, and a healthy peer can continue.
    request={};request.operation=200;request.args[0]=1;request.args[1]=1000;append(request,&rule);
    std::array<Owners,64> peers;
    for(size_t i=0;i<63;++i){peers[i].driving=2000+i;CHECK(dispatchDriving(request,reply,peers[i])==NIMBY_OK);}
    peers[63].driving=3000;CHECK(dispatchDriving(request,reply,peers[63])==NIMBY_RESOURCE_LIMIT);
    // A completed half is not published again while the other half is busy.
    busyRules=false;CHECK(wait([]{return ruleSuccesses==1;}));
    std::this_thread::sleep_for(std::chrono::milliseconds(30));CHECK(ruleSuccesses==1&&!released(1234));
    busyConstraints=false;CHECK(wait([]{return released(1234);}));
    CHECK(wait([&]{return dispatchDriving(request,reply,peers[63])==NIMBY_OK;}));
    for(auto& peer:peers)cleanupDriving(peer);
    CHECK(wait([]{std::lock_guard lock(releasedMutex);return releasedRules.size()==65&&releasedConstraints.size()==65;}));
    std::cout<<"PASS driving RPC sizes, bounded counts, trusted owner and queued retirement under contention\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
