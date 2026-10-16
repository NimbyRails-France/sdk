#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/automatic_driving.h>
#include <nimby/detail/signal_routes.h>
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
unsigned routeCalls=0;
unsigned routeV2Calls=0;
uint32_t routeStatus=NIMBY_OK;
bool invalidRouteCount=false;
bool invalidAnnotationCount=false;
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
extern "C" uint32_t __cdecl NimbyInternal_CopySignalRoutes(const uint64_t* signals,uint32_t count,uint32_t blocks,
 NimbySignalRouteObservation* rows,uint32_t capacity,uint32_t* written,
 NimbySignalRouteSection* sections,uint32_t sectionCapacity,uint32_t* sectionsWritten) noexcept {
    ++routeCalls;
    *written=*sectionsWritten=0;
    if(routeStatus!=NIMBY_OK)return routeStatus;
    if(count!=1||!signals||signals[0]>>48!=8||blocks!=2)return NIMBY_INVALID_ARGUMENT;
    if(!capacity||!sectionCapacity)return NIMBY_BUFFER_TOO_SMALL;
    rows[0]={};rows[0].signal_id=signals[0];rows[0].train_id=0x5000000000001;
    rows[0].next_track_id=0x1000000000002;rows[0].next_fraction=.75;
    sections[0]={rows[0].train_id,signals[0],0x1000000000001,.2,.7};
    *written=invalidRouteCount?capacity+1:1;*sectionsWritten=1;return NIMBY_OK;
}
extern "C" uint32_t __cdecl NimbyInternal_CopySignalRoutesV2(const uint64_t* signals,uint32_t count,uint32_t blocks,
 NimbySignalRouteObservation* rows,uint32_t capacity,uint32_t* written,
 NimbySignalRouteSection* sections,uint32_t sectionCapacity,uint32_t* sectionsWritten,
 NimbySignalRouteAnnotation* annotations,uint32_t annotationCapacity,uint32_t* annotationsWritten) noexcept {
    ++routeV2Calls;*written=*sectionsWritten=*annotationsWritten=0;
    if(routeStatus!=NIMBY_OK)return routeStatus;
    if(count!=1||!signals||signals[0]>>48!=8||blocks!=2)return NIMBY_INVALID_ARGUMENT;
    if(!capacity||!sectionCapacity||!annotationCapacity)return NIMBY_BUFFER_TOO_SMALL;
    rows[0]={};rows[0].signal_id=signals[0];rows[0].train_id=0x5000000000001;
    sections[0]={rows[0].train_id,signals[0],0x1000000000001,.7,.2};
    annotations[0]={rows[0].train_id,signals[0],signals[0]+0x10000,sections[0].track_id,.2,500,-1,NIMBY_SIGNAL_BALISE,0,13};
    *written=invalidRouteCount?capacity+1:1;*sectionsWritten=1;
    *annotationsWritten=invalidAnnotationCount?annotationCapacity+1:1;return NIMBY_OK;
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
    // Ordered-route transport retains the private ABI and rejects malformed
    // child budgets before invoking the resident capability. A failed or
    // overflowing batch never exposes partial facts from the native bridge.
    request={};request.operation=203;request.args[0]=1;request.args[1]=2;
    request.args[2]=2;request.args[3]=2;append(request,&rule.signal);
    reply={};CHECK(dispatchDriving(request,reply,owners)==NIMBY_OK&&routeCalls==1);
    CHECK(reply.args[0]==1&&reply.args[1]==1&&reply.data.size()==sizeof(NimbySignalRouteObservation)+sizeof(NimbySignalRouteSection));
    NimbySignalRouteObservation route{};std::memcpy(&route,reply.data.data(),sizeof route);
    CHECK(route.signal_id==rule.signal&&route.next_track_id==0x1000000000002&&route.next_fraction==.75);
    NimbySignalRouteSection part{};std::memcpy(&part,reply.data.data()+sizeof route,sizeof part);
    CHECK(part.train_id==route.train_id&&part.begin_fraction==.2&&part.end_fraction==.7);
    request.args[1]=17;reply={};CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&routeCalls==1&&reply.data.empty());
    request.args[1]=2;request.args[2]=65537;CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&routeCalls==1);
    request.args[2]=2;request.data.pop_back();CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&routeCalls==1);
    request.data.clear();append(request,&rule.signal);routeStatus=NIMBY_RESOURCE_LIMIT;
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_RESOURCE_LIMIT&&reply.data.empty());
    routeStatus=NIMBY_OK;invalidRouteCount=true;
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_BINARY&&reply.data.empty());invalidRouteCount=false;
    // Additive V2 travels through its own operation exactly once, preserving
    // directed fractions, annotation ordering and trusted channel ownership.
    request={};request.operation=204;request.args[0]=1;request.args[1]=2;
    request.args[2]=2;request.args[3]=2;request.args[4]=2;append(request,&rule.signal);
    const auto legacyCalls=routeCalls;reply={};
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_OK&&routeV2Calls==1&&routeCalls==legacyCalls);
    CHECK(reply.args[0]==1&&reply.args[1]==1&&reply.args[2]==1&&
          reply.data.size()==sizeof(NimbySignalRouteObservation)+sizeof(NimbySignalRouteSection)+sizeof(NimbySignalRouteAnnotation));
    std::memcpy(&part,reply.data.data()+sizeof route,sizeof part);CHECK(part.begin_fraction==.7&&part.end_fraction==.2);
    NimbySignalRouteAnnotation annotation{};
    std::memcpy(&annotation,reply.data.data()+sizeof route+sizeof part,sizeof annotation);
    CHECK(annotation.native_kind==NIMBY_SIGNAL_BALISE&&annotation.direction==-1&&annotation.flags==13);
    const auto v2Before=routeV2Calls;request.args[4]=131073;
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&routeV2Calls==v2Before&&reply.data.empty());
    request.args[4]=131072;request.args[2]=65536;request.args[3]=131072;
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_RESOURCE_LIMIT&&routeV2Calls==v2Before&&reply.data.empty());
    request.args[2]=request.args[3]=request.args[4]=2;request.args[0]=2;append(request,&rule.signal);
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&routeV2Calls==v2Before); // Duplicate IDs.
    request.args[0]=1;request.data.resize(sizeof(uint64_t));request.data.push_back(0);
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_ARGUMENT&&routeV2Calls==v2Before);
    request.data.resize(sizeof(uint64_t));invalidAnnotationCount=true;
    CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_BINARY&&reply.data.empty());invalidAnnotationCount=false;
    invalidRouteCount=true;CHECK(dispatchDriving(request,reply,owners)==NIMBY_INVALID_BINARY&&reply.data.empty());invalidRouteCount=false;
    routeStatus=NIMBY_RESOURCE_LIMIT;CHECK(dispatchDriving(request,reply,owners)==NIMBY_RESOURCE_LIMIT&&reply.data.empty());routeStatus=NIMBY_OK;
    Owners foreign;const auto beforeForeign=routeV2Calls;
    CHECK(dispatchDriving(request,reply,foreign)==NIMBY_INVALID_ARGUMENT&&routeV2Calls==beforeForeign);
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
