#include "platform/windows/runtime/construction_bridge.h"
#include "nimby/detail/observation.h"
#include "platform/windows/runtime/clock_bridge.h"
#include "platform/windows/runtime/mod_host_client.h"
#include <stdexcept>
#include <cstdio>
#include <bit>
#include <thread>
#include <chrono>
#include <array>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
int main(int argc,char** argv){try{
    if(argc==4&&std::string(argv[1])=="--request"){
        const auto target=static_cast<DWORD>(std::stoul(argv[2]));
        const auto host=LoadLibraryW(L"NimbyRailsFranceSDK.dll");CHECK(host);
        const auto setTarget=std::bit_cast<void(*)(uint32_t)>(GetProcAddress(host,"NimbyTest_SetTarget"));CHECK(setTarget);
        setTarget(target);
        const auto process=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,target);CHECK(process);
        NimbyConstructionRequest request{};request.size=sizeof request;request.version=1;
        request.action=NIMBY_CONSTRUCTION_PREPARE;request.source_signal=0x8000000000001;
        NimbyConstructionResult result{};NimbyBinaryInfo binary{};
        const auto status=nimby::construction_bridge::exchange(process,target,binary,&request,0,result);
        CloseHandle(process);
        if(status==NIMBY_OK)CHECK(result.state==NIMBY_CONSTRUCTION_READY&&result.token==123);
        return static_cast<int>(status);
    }
    if(argc==3){
        const auto mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,nimby::construction_bridge::name(static_cast<DWORD>(std::stoul(argv[2]))).c_str());CHECK(mapping);
        auto* shared=static_cast<nimby::construction_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(nimby::construction_bridge::Shared)));CHECK(shared);
        shared->lease=nimby::platform::windows::bridgeRequestLease(15000);shared->expires=shared->lease.expires;
        InterlockedExchange(&shared->state,nimby::construction_bridge::pending);Sleep(30000);return 0;
    }
    using nimby::construction_bridge::exchange;
    NimbyConstructionRequest request{};request.size=sizeof request;request.version=1;
    request.action=NIMBY_CONSTRUCTION_PREPARE;request.source_signal=0x8000000000001;
    NimbyConstructionResult result{};result.size=sizeof result;result.version=1;
    NimbyBinaryInfo binary{};
    // Current-process bootstrap must work without external injection. The fake
    // endpoint lives beside this test executable in an isolated fixture folder.
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    CHECK(result.token==123&&result.state==NIMBY_CONSTRUCTION_READY);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,123,result)==NIMBY_OK);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,124,result)==NIMBY_INVALID_HANDLE);
    // Repeated preparation accepts ALREADY_INITIALIZED from the resident DLL.
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    request.action=999;
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_INVALID_ARGUMENT);
    request.action=NIMBY_CONSTRUCTION_PREPARE;
    const auto host=LoadLibraryW(L"NimbyRailsFranceSDK.dll");CHECK(host);
    const auto calls=std::bit_cast<uint32_t(*)()>(GetProcAddress(host,"NimbyTest_HostCalls"));CHECK(calls);
    CHECK(nimby::mod_host::client::language()==std::optional<std::string>("fra"));
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    CHECK(calls()==1);
    // Hosted transport asks the broker to bootstrap, then cancels a pending
    // request within its callback budget. Resuming the native endpoint later
    // must not apply the cancelled request.
    const auto fixture=GetModuleHandleW(L"NimbyConstructionBridge-experimental-v1.dll");CHECK(fixture);
    const auto pause=std::bit_cast<void(*)(uint32_t)>(GetProcAddress(fixture,"NimbyTest_Pause"));CHECK(pause);
    const auto mutations=std::bit_cast<uint32_t(*)()>(GetProcAddress(fixture,"NimbyTest_Mutations"));CHECK(mutations);
    pause(1);const auto before=mutations();const auto started=GetTickCount64();
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_DATA_UNAVAILABLE);
    CHECK(GetTickCount64()-started>=900&&GetTickCount64()-started<1800);
    pause(0);std::this_thread::sleep_for(std::chrono::milliseconds(50));CHECK(mutations()==before);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    CHECK(mutations()==before+1);
    // A slow peer holding the native mutation transport cannot hold another
    // isolated callback for the former one-second client mutex timeout.
    HANDLE acquired=CreateEventW(nullptr,TRUE,FALSE,nullptr),release=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    CHECK(acquired&&release);
    std::thread peer([&]{const auto mutex=CreateMutexW(nullptr,FALSE,(nimby::construction_bridge::name(GetCurrentProcessId())+L".Client").c_str());
        WaitForSingleObject(mutex,INFINITE);SetEvent(acquired);WaitForSingleObject(release,INFINITE);ReleaseMutex(mutex);CloseHandle(mutex);});
    WaitForSingleObject(acquired,INFINITE);const auto blocked=GetTickCount64();
    const auto status=exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result);
    const auto elapsed=GetTickCount64()-blocked;SetEvent(release);peer.join();CloseHandle(acquired);CloseHandle(release);
    CHECK(status==NIMBY_RESOURCE_LIMIT&&elapsed<100);
    // A worker killed after publishing a request cannot cause a later mutation.
    pause(1);
    const auto mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,nimby::construction_bridge::name(GetCurrentProcessId()).c_str());CHECK(mapping);
    auto* shared=static_cast<nimby::construction_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(nimby::construction_bridge::Shared)));CHECK(shared);
    std::array<wchar_t,32768> path{};CHECK(GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size())));
    std::wstring command=L"\""+std::wstring(path.data())+L"\" --queued "+std::to_wstring(GetCurrentProcessId());
    STARTUPINFOW startup{};startup.cb=sizeof startup;PROCESS_INFORMATION child{};
    CHECK(CreateProcessW(path.data(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&child));
    struct ChildCleanup{PROCESS_INFORMATION& child;~ChildCleanup(){TerminateProcess(child.hProcess,0);CloseHandle(child.hThread);CloseHandle(child.hProcess);}} childCleanup{child};
    const auto readyDeadline=GetTickCount64()+3000;
    while(InterlockedCompareExchange(&shared->state,0,0)!=nimby::construction_bridge::pending&&GetTickCount64()<readyDeadline)Sleep(1);
    CHECK(InterlockedCompareExchange(&shared->state,0,0)==nimby::construction_bridge::pending);
    CHECK(TerminateProcess(child.hProcess,7));CHECK(WaitForSingleObject(child.hProcess,1000)==WAIT_OBJECT_0);
    const auto beforeDeath=mutations();pause(0);
    const auto completedDeadline=GetTickCount64()+1000;
    while(InterlockedCompareExchange(&shared->state,0,0)!=nimby::construction_bridge::complete&&GetTickCount64()<completedDeadline)Sleep(1);
    CHECK(shared->result.state==NIMBY_CONSTRUCTION_REJECTED&&mutations()==beforeDeath);

    // Real transport, two OS requesters: A times out after execution begins,
    // then gets a completed response. B must not erase it before A's poll.
    const auto hold=std::bit_cast<void(*)(uint32_t)>(GetProcAddress(fixture,"NimbyTest_HoldCompletion"));CHECK(hold);
    const auto peerRequest=[&]{
        std::wstring command=L"\""+std::wstring(path.data())+L"\" --request "+std::to_wstring(GetCurrentProcessId())+L" once";
        STARTUPINFOW startup{};startup.cb=sizeof startup;PROCESS_INFORMATION peer{};
        CHECK(CreateProcessW(path.data(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&peer));
        struct Close{PROCESS_INFORMATION& value;~Close(){if(WaitForSingleObject(value.hProcess,0)==WAIT_TIMEOUT)TerminateProcess(value.hProcess,99);CloseHandle(value.hThread);CloseHandle(value.hProcess);}} close{peer};
        CHECK(WaitForSingleObject(peer.hProcess,3000)==WAIT_OBJECT_0);DWORD code{};CHECK(GetExitCodeProcess(peer.hProcess,&code));return code;
    };
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    auto create=request;create.action=NIMBY_CONSTRUCTION_CREATE;create.token=123;create.count=1;
    create.positions[0]={0x1000000000001,0.5,1,0};
    hold(1);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&create,0,result)==NIMBY_OK);
    CHECK(result.state==NIMBY_CONSTRUCTION_PENDING&&result.token==123&&shared->reserved==0);
    CHECK(GetTickCount64()>=shared->lease.expires);
    hold(0);const auto responseDeadline=GetTickCount64()+1000;
    while(InterlockedCompareExchange(&shared->state,0,0)!=nimby::construction_bridge::complete&&GetTickCount64()<responseDeadline)Sleep(1);
    CHECK(shared->state==nimby::construction_bridge::complete&&shared->result.state==NIMBY_CONSTRUCTION_APPLIED);
    const auto responseOwner=shared->lease;
    CHECK(peerRequest()==NIMBY_RESOURCE_LIMIT);
    CHECK(shared->request.action==NIMBY_CONSTRUCTION_CREATE&&shared->result.state==NIMBY_CONSTRUCTION_APPLIED&&
          shared->result.ids[0]==0x8000000000002&&shared->reserved==0&&
          nimby::platform::windows::sameBridgeRequester(shared->lease,responseOwner));
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,124,result)==NIMBY_INVALID_HANDLE);
    CHECK(shared->reserved==0);
    const auto validSize=shared->result.size;shared->result.size=0;
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,123,result)==NIMBY_INVALID_BINARY);
    CHECK(shared->reserved==0);shared->result.size=validSize;
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,123,result)==NIMBY_OK);
    CHECK(result.state==NIMBY_CONSTRUCTION_APPLIED&&result.count==1&&result.ids[0]==0x8000000000002&&shared->reserved==1);
    CHECK(peerRequest()==NIMBY_OK);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,123,result)==NIMBY_INVALID_HANDLE);
    std::puts("PASS: late completed response survives foreign PREPARE; only valid owner/ticket copy acknowledges it");
    UnmapViewOfFile(shared);CloseHandle(mapping);
    // Clock changes use the same broker bootstrap and bounded native lease,
    // including the no-recalculation path used by ObservationProcess::setClock.
    pause(1);NimbySimulationClock clock{};uint32_t interventions{};
    const auto bootstrapClock=std::bit_cast<DWORD(WINAPI*)(void*)>(GetProcAddress(fixture,"NimbyTest_ClockBootstrap"));CHECK(bootstrapClock);
    CHECK(bootstrapClock(nullptr)==NIMBY_OK);pause(0);
    CHECK(nimby::clock_bridge::change(GetCurrentProcess(),GetCurrentProcessId(),1,binary,12345,clock,interventions,false)==NIMBY_OK);
    CHECK(clock.epoch_seconds==12345&&interventions==0);
    CHECK(nimby::clock_bridge::change(GetCurrentProcess(),GetCurrentProcessId(),1,binary,23456,clock,interventions)==NIMBY_OK);
    CHECK(clock.epoch_seconds==23456&&interventions==3);
    pause(1);const auto beforeClock=mutations(),clockCalls=calls();const auto clockStarted=GetTickCount64();
    CHECK(nimby::clock_bridge::change(GetCurrentProcess(),GetCurrentProcessId(),1,binary,34567,clock,interventions)==NIMBY_DATA_UNAVAILABLE);
    CHECK(GetTickCount64()-clockStarted>=900&&GetTickCount64()-clockStarted<1800&&calls()==clockCalls+1);
    pause(0);std::this_thread::sleep_for(std::chrono::milliseconds(50));CHECK(mutations()==beforeClock);
    std::puts("PASS: hosted construction/clock routing, cancellation, dead-worker rejection, peer isolation and polling");
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
