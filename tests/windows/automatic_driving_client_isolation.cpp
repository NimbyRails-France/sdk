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
}
namespace nimby::engine {
uint32_t identify(const wchar_t*,NimbyBinaryInfo&) noexcept {return NIMBY_INVALID_BINARY;}
}
extern "C" uint32_t __cdecl NimbyInternal_ModHostTarget() noexcept {return 0;}
extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t,uint64_t*,const void*,uint32_t,void*,uint32_t,uint32_t*) noexcept {return NIMBY_HOOKS_UNAVAILABLE;}
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
            samples.push_back(std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-started).count());
        }
        return samples;
    });
    const bool progressed=healthy.wait_for(500ms)==std::future_status::ready;
    SetEvent(release);CHECK(blocked.get()==NIMBY_OK);
    const auto samples=healthy.get();CHECK(progressed&&healthyCalls==192);
    auto sorted=samples;std::sort(sorted.begin(),sorted.end());
    std::cout<<"{\"scenario\":\"healthy_driving_during_stalled_peer\",\"samples\":64,\"p50_us\":"<<sorted[31]<<",\"p95_us\":"<<sorted[60]<<",\"p99_us\":"<<sorted[63]<<"}\n";

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
