// Exercise the actual client and its process-global bridge cache. Exported
// doubles are native functions from this executable; no game is opened.
#include "../../src/platform/windows/runtime/automatic_driving_client.cpp"
#include <chrono>
#include <future>
#include <iostream>
#include <vector>
#include <algorithm>

#define CHECK(x) do { if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x); } while(false)
namespace {
HANDLE entered{},release{};
std::atomic<unsigned> healthyCalls{};
bool hosted=false;
unsigned routeRpcCalls=0,routeRpcMode=0;
}
namespace nimby::engine {
uint32_t identify(const wchar_t*,NimbyBinaryInfo&) noexcept {return NIMBY_INVALID_BINARY;}
}
extern "C" uint32_t __cdecl NimbyInternal_EnsureTrainRuntimeMechanics()noexcept{return NIMBY_INVALID_BINARY;}
extern "C" uint32_t __cdecl NimbyInternal_ModHostTarget() noexcept {return hosted?1:0;}
extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* args,const void* input,uint32_t bytes,
 void* output,uint32_t capacity,uint32_t* written) noexcept {
    *written=0;++routeRpcCalls;
    if(operation!=204||args[0]!=1||args[1]!=2||bytes!=8)return NIMBY_INVALID_ARGUMENT;
    if(routeRpcMode==3)return NIMBY_RESOURCE_LIMIT;
    uint64_t signal{};std::memcpy(&signal,input,sizeof signal);
    NimbySignalRouteObservation row{};row.train_id=0x5000000000001;row.signal_id=signal;
    NimbySignalRouteSection section{row.train_id,signal,0x1000000000001,.7,.2};
    NimbySignalRouteAnnotation annotation{row.train_id,signal,signal+0x10000,section.track_id,.2,500,-1,NIMBY_SIGNAL_BALISE,0,13};
    const size_t required=sizeof row+sizeof section+sizeof annotation;
    if(capacity<required)return NIMBY_BUFFER_TOO_SMALL;
    if(routeRpcMode==1){args[0]=args[2]+1;args[1]=args[2]=0;return NIMBY_OK;}
    args[0]=args[1]=args[2]=1;
    std::memcpy(output,&row,sizeof row);std::memcpy(static_cast<unsigned char*>(output)+sizeof row,&section,sizeof section);
    std::memcpy(static_cast<unsigned char*>(output)+sizeof row+sizeof section,&annotation,sizeof annotation);
    *written=static_cast<uint32_t>(required-(routeRpcMode==2?1:0));return NIMBY_OK;
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_PublishV3(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t,uint64_t owner) noexcept {
    if(owner==1){SetEvent(entered);WaitForSingleObject(release,3000);}else ++healthyCalls;
    return NIMBY_OK;
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_TrainConstraints(const NimbyTrainConstraint*,uint32_t,uint32_t,uint64_t) noexcept {
    ++healthyCalls;return NIMBY_OK;
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_ReadTrainConstraint(uint64_t,NimbyTrainConstraintStatus*) noexcept {
    ++healthyCalls;return NIMBY_OK;
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_CopySignalRoutes(const uint64_t*,uint32_t,uint32_t,
 NimbySignalRouteObservation*,uint32_t,uint32_t* count,NimbySignalRouteSection*,uint32_t,uint32_t* sectionCount) noexcept {
    *count=*sectionCount=0;++healthyCalls;return NIMBY_OK;
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyDriving_CopySignalRoutesV2(const uint64_t*,uint32_t,uint32_t,
 NimbySignalRouteObservation*,uint32_t,uint32_t* count,NimbySignalRouteSection*,uint32_t,uint32_t* sectionCount,
 NimbySignalRouteAnnotation*,uint32_t,uint32_t* annotationCount) noexcept {
    *count=*sectionCount=*annotationCount=0;++healthyCalls;return NIMBY_OK;
}
int main(){try{
    using namespace std::chrono_literals;
    bridge=GetModuleHandleW(nullptr);
    entered=CreateEventW(nullptr,TRUE,FALSE,nullptr);release=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    CHECK(entered&&release);
    auto blocked=std::async(std::launch::async,[]{return NimbyInternal_PublishDrivingRulesV3(nullptr,0,1000,0,1);});
    CHECK(WaitForSingleObject(entered,1000)==WAIT_OBJECT_0);
    auto healthy=std::async(std::launch::async,[]{
        std::vector<double> samples;
        for(int i=0;i<64;++i){
            const auto started=std::chrono::steady_clock::now();
            NimbyTrainConstraintStatus status{};status.size=sizeof status;
            CHECK(NimbyInternal_ReadTrainConstraint(0x5000000000001,&status)==NIMBY_OK);
            CHECK(NimbyInternal_PublishDrivingRulesV3(nullptr,0,1000,0,2)==NIMBY_OK);
            CHECK(NimbyInternal_PublishTrainConstraints(nullptr,0,1000,2)==NIMBY_OK);
            const uint64_t signal=0x8000000000001;uint32_t count=1,sectionCount=1;
            NimbySignalRouteObservation unusedRow{};NimbySignalRouteSection unusedSection{};
            CHECK(NimbyInternal_CopySignalRoutes(&signal,1,2,&unusedRow,0,&count,&unusedSection,0,&sectionCount)==NIMBY_OK);
            CHECK(!count&&!sectionCount);
            uint32_t annotationCount=1;
            CHECK(NimbyInternal_CopySignalRoutesV2(&signal,1,2,nullptr,0,&count,nullptr,0,&sectionCount,nullptr,0,&annotationCount)==NIMBY_OK);
            CHECK(!count&&!sectionCount&&!annotationCount);
            samples.push_back(std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-started).count());
        }
        return samples;
    });
    const bool progressed=healthy.wait_for(500ms)==std::future_status::ready;
    SetEvent(release);CHECK(blocked.get()==NIMBY_OK);
    const auto samples=healthy.get();CHECK(progressed&&healthyCalls==320);
    auto sorted=samples;std::sort(sorted.begin(),sorted.end());
    std::cout<<"{\"scenario\":\"healthy_driving_during_stalled_peer\",\"samples\":64,\"p50_us\":"<<sorted[31]<<",\"p95_us\":"<<sorted[60]<<",\"p99_us\":"<<sorted[63]<<"}\n";

    // Exercise the actual child's V2 wire parser, including invalid replies;
    // no resident callback or legacy V1 capture supplies the annotations.
    hosted=true;const uint64_t signal=0x8000000000001;
    std::array<NimbySignalRouteObservation,2> rows{};std::array<NimbySignalRouteSection,2> sections{};
    std::array<NimbySignalRouteAnnotation,2> annotations{};uint32_t count=0,sectionCount=0,annotationCount=0;
    const auto copied=[&](uint32_t rowCapacity=2,uint32_t sectionCapacity=2,uint32_t annotationCapacity=2){
        return NimbyInternal_CopySignalRoutesV2(&signal,1,2,rows.data(),rowCapacity,&count,
            sections.data(),sectionCapacity,&sectionCount,annotations.data(),annotationCapacity,&annotationCount);
    };
    CHECK(copied()==NIMBY_OK&&routeRpcCalls==1&&count==1&&sectionCount==1&&annotationCount==1);
    CHECK(sections[0].begin_fraction==.7&&sections[0].end_fraction==.2&&annotations[0].direction==-1&&annotations[0].flags==13);
    for(routeRpcMode=1;routeRpcMode<=3;++routeRpcMode){
        const auto status=copied();CHECK(status==(routeRpcMode==3?NIMBY_RESOURCE_LIMIT:NIMBY_INVALID_BINARY));
        CHECK(count==0&&sectionCount==0&&annotationCount==0);
    }
    routeRpcMode=0;const auto beforeOversize=routeRpcCalls;
    CHECK(copied(65536,131072,131072)==NIMBY_RESOURCE_LIMIT&&routeRpcCalls==beforeOversize);
    CHECK(count==0&&sectionCount==0&&annotationCount==0);
    const std::array<uint64_t,2> duplicate{signal,signal};
    CHECK(NimbyInternal_CopySignalRoutesV2(duplicate.data(),2,2,rows.data(),2,&count,sections.data(),2,&sectionCount,
        annotations.data(),2,&annotationCount)==NIMBY_INVALID_ARGUMENT&&routeRpcCalls==beforeOversize);
    hosted=false;
    bridge=GetModuleHandleW(L"kernel32.dll");
    CHECK(copied()==NIMBY_HOOKS_UNAVAILABLE&&count==0&&sectionCount==0&&annotationCount==0); // Old bridge: explicit lack of V2.

    // The first caller may still be installing the pinned bridge. A peer gets
    // a retryable resource limit without waiting on the installer's mutex.
    bridge=nullptr;
    std::unique_lock initialization(bridgeMutex);
    auto installing=std::async(std::launch::async,[]{
        NimbySignalDrivingRule rule{};return NimbyInternal_PublishDrivingRulesV3(&rule,1,1000,0,2);
    });
    const bool bounded=installing.wait_for(500ms)==std::future_status::ready;
    initialization.unlock();CHECK(installing.get()==NIMBY_RESOURCE_LIMIT&&bounded);
    CloseHandle(entered);CloseHandle(release);
    std::cout<<"PASS driving publication isolation and bounded initialization\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
