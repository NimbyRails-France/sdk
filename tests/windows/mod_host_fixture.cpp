// Loaded only by the standalone host regression executable, never by a game.
#include <windows.h>
#include <array>
#include <atomic>
#include <bit>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include <cwchar>
#include <platform/windows/mod_host.h>

#ifndef NIMBY_HOST_FIXTURE_MODE
#define NIMBY_HOST_FIXTURE_MODE 0
#endif
namespace {
using Pulse=void(__cdecl*)(uint32_t);
using Stage=void(__cdecl*)(uint32_t,uint64_t);
using Call=uint32_t(__cdecl*)(uint32_t,uint64_t*,const void*,uint32_t,void*,uint32_t,uint32_t*);
using ActionWaits=uint32_t(__cdecl*)(uint64_t*,uint64_t*);
using WakeLocal=void(__cdecl*)();
Pulse pulse{};Call call{};
Stage stage{};
ActionWaits actionWaits{};WakeLocal wakeLocal{};
std::atomic<bool> stopping=false;
std::atomic<uint64_t> throttled=0;
std::thread worker;
HANDLE actionStop{},remoteWait{},localWait{};
uint64_t remoteWakes{},localWakes{},actionChecks{};
std::array<HANDLE,3> channelHandles() {
    const auto afterExe=std::wcschr(GetCommandLineW()+1,L'"')+1;
    wchar_t* end{};std::array<HANDLE,3> result{};
    result[0]=reinterpret_cast<HANDLE>(std::wcstoull(afterExe,&end,10));
    result[1]=reinterpret_cast<HANDLE>(std::wcstoull(end,&end,10));
    result[2]=reinterpret_cast<HANDLE>(std::wcstoull(end,&end,10));
    return result;
}
HANDLE inheritedActionWait() {
    const auto afterExe=std::wcschr(GetCommandLineW()+1,L'"')+1;
    wchar_t* end{};uint64_t value{};const wchar_t* cursor=afterExe;
    for(unsigned i=0;i<5;++i){value=std::wcstoull(cursor,&end,10);cursor=end;}
    return reinterpret_cast<HANDLE>(value);
}
bool deniesModification(HANDLE handle) {
    SetLastError(ERROR_SUCCESS);
    if(SetEvent(handle)||GetLastError()!=ERROR_ACCESS_DENIED)return false;
    SetLastError(ERROR_SUCCESS);
    return !ResetEvent(handle)&&GetLastError()==ERROR_ACCESS_DENIED;
}
uint32_t reportAction(uint64_t stage) {
    const auto sequence=1000+stage;
    std::array<uint64_t,8> arguments{NIMBY_HOST_FIXTURE_MODE,sequence};
    const std::array<uint64_t,3> input{remoteWakes,localWakes,actionChecks};
    std::array<uint64_t,3> output{};uint32_t written{};
    const auto result=call(100,arguments.data(),input.data(),sizeof input,output.data(),sizeof output,&written);
    return result?result:(written==sizeof output&&output==input&&arguments[6]==sequence+1?0:6);
}
void runActionFixture() {
    uint64_t remote{},local{};
    if(actionWaits(&remote,&local)||!remote||!local)ExitProcess(6);
    remoteWait=reinterpret_cast<HANDLE>(remote);localWait=reinterpret_cast<HANDLE>(local);
    const auto inherited=inheritedActionWait();
    if(!deniesModification(inherited)||!deniesModification(remoteWait)||!deniesModification(localWait))ExitProcess(6);
    actionChecks|=1;
    if(WaitForSingleObject(inherited,0)!=WAIT_TIMEOUT||WaitForSingleObject(remoteWait,0)!=WAIT_TIMEOUT||
       WaitForSingleObject(localWait,0)!=WAIT_TIMEOUT)ExitProcess(6);
    actionChecks|=2;
    // Every getter result belongs to its caller. Repeated opt-in acquisition
    // and close must not leak either the inherited or the private local event.
    DWORD before{},after{};
    if(!GetProcessHandleCount(GetCurrentProcess(),&before))ExitProcess(6);
    for(unsigned i=0;i<100;++i){
        uint64_t a{},b{};if(actionWaits(&a,&b)||!a||!b)ExitProcess(6);
        if(!CloseHandle(reinterpret_cast<HANDLE>(a))||!CloseHandle(reinterpret_cast<HANDLE>(b)))ExitProcess(6);
    }
    if(!GetProcessHandleCount(GetCurrentProcess(),&after)||before!=after)ExitProcess(6);
    actionChecks|=4;
    wakeLocal();
    if(WaitForSingleObject(localWait,1000)!=WAIT_OBJECT_0||WaitForSingleObject(localWait,0)!=WAIT_TIMEOUT)ExitProcess(6);
    ++localWakes;actionChecks|=8;
    if(WaitForSingleObject(remoteWait,0)!=WAIT_TIMEOUT)ExitProcess(6);
    actionChecks|=16;
    if(reportAction(1))ExitProcess(6);
    const HANDLE waits[]{actionStop,remoteWait,localWait};
    for(;;){
        const auto result=WaitForMultipleObjects(3,waits,FALSE,INFINITE);
        if(result==WAIT_OBJECT_0)break;
        if(result==WAIT_OBJECT_0+1)++remoteWakes;
        else if(result==WAIT_OBJECT_0+2)++localWakes;
        else ExitProcess(6);
        if(reportAction(2))ExitProcess(6);
    }
    // A manual-reset event or a signal accidentally retained across the final
    // stop cannot be hidden by the stop handle winning the lower wait index.
    if(WaitForSingleObject(remoteWait,0)!=WAIT_TIMEOUT||WaitForSingleObject(localWait,0)!=WAIT_TIMEOUT)ExitProcess(6);
    if(!CloseHandle(remoteWait)||!CloseHandle(localWait))ExitProcess(6);
    remoteWait=localWait=nullptr;actionChecks|=32;
}
uint32_t invoke(uint64_t sequence,uint64_t previousNs=0) {
    std::array<uint64_t,8> arguments{NIMBY_HOST_FIXTURE_MODE,sequence,previousNs,throttled.load()};
    JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpu{};
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION memory{};
    if(!QueryInformationJobObject(nullptr,JobObjectCpuRateControlInformation,&cpu,sizeof(cpu),nullptr)||
        !QueryInformationJobObject(nullptr,JobObjectExtendedLimitInformation,&memory,sizeof(memory),nullptr))return 6;
    arguments[4]=cpu.CpuRate;arguments[5]=memory.ProcessMemoryLimit;
    arguments[7]=memory.BasicLimitInformation.PriorityClass;
    const std::array<uint64_t,3> input{0xfedcba9876543210ull,sequence,0x0123456789abcdefull};
    std::array<uint64_t,3> output{};uint32_t written=0;
    const auto result=call(100,arguments.data(),input.data(),sizeof input,output.data(),sizeof output,&written);
    if(result)return result;
    if(written!=sizeof output||output!=input||arguments[6]!=sequence+1)return 6;
    return 0;
}
}
extern "C" __declspec(dllexport) uint32_t WINAPI NRFMod_HostProtocolV1(void*) { return 1; }
extern "C" __declspec(dllexport) uint32_t WINAPI NRFMod_StartV1(void*) {
    const auto exe=GetModuleHandleW(nullptr);
    pulse=std::bit_cast<Pulse>(GetProcAddress(exe,"NimbyInternal_ModHostPulse"));
    stage=std::bit_cast<Stage>(GetProcAddress(exe,"NimbyInternal_ModHostStage"));
    call=std::bit_cast<Call>(GetProcAddress(exe,"NimbyInternal_ModHostCall"));
    if(!pulse||!call)return 3;
    if constexpr(NIMBY_HOST_FIXTURE_MODE==7||NIMBY_HOST_FIXTURE_MODE==8||NIMBY_HOST_FIXTURE_MODE==17||NIMBY_HOST_FIXTURE_MODE==18)
        if(!stage)return 3;
    if(const auto status=invoke(0))return status;
    if constexpr(NIMBY_HOST_FIXTURE_MODE==1)RaiseException(EXCEPTION_ACCESS_VIOLATION,0,0,nullptr);
    if constexpr(NIMBY_HOST_FIXTURE_MODE==2)for(;;)Sleep(1000);
    if constexpr(NIMBY_HOST_FIXTURE_MODE==5)throw std::runtime_error("injected start exception");
    if constexpr(NIMBY_HOST_FIXTURE_MODE==14||NIMBY_HOST_FIXTURE_MODE==15) {
        actionWaits=std::bit_cast<ActionWaits>(GetProcAddress(exe,"NimbyInternal_ModHostActionWaits"));
        wakeLocal=std::bit_cast<WakeLocal>(GetProcAddress(exe,"NimbyInternal_ModHostWakeLocal"));
        if(!actionWaits||!wakeLocal)return 3;
        actionStop=CreateEventW(nullptr,TRUE,FALSE,nullptr);if(!actionStop)return 6;
        worker=std::thread(runActionFixture);return 0;
    }
    worker=std::thread([] {
        if constexpr(NIMBY_HOST_FIXTURE_MODE==16) {
            pulse(2);if(invoke(1))ExitProcess(6);
            for(;;)Sleep(1000); // Remains blocked after the simulated resume.
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==12) {
            // Deliberately bypass the SDK call helper: a native faulty mod can
            // signal its inherited request event without publishing any request.
            const auto handles=channelHandles();for(;;)SetEvent(handles[1]);
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==13) {
            const auto handles=channelHandles();
            auto* shared=static_cast<nimby::mod_host::Shared*>(MapViewOfFile(handles[0],FILE_MAP_ALL_ACCESS,0,0,sizeof(nimby::mod_host::Shared)));
            if(!shared)ExitProcess(6);
            while(!stopping){
                pulse(2);shared->operation=100;shared->inputSize=nimby::mod_host::payloadLimit+1;shared->capacity=0;
                ResetEvent(handles[2]);InterlockedExchange(&shared->requestState,1);SetEvent(handles[1]);
                if(WaitForSingleObject(handles[2],2000)!=WAIT_OBJECT_0)ExitProcess(6);
                if(shared->result==NIMBY_RESOURCE_LIMIT)++throttled;
                else if(shared->result!=NIMBY_INVALID_ARGUMENT)ExitProcess(6);
                pulse(3);
            }
            UnmapViewOfFile(shared);return;
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==7) {
            // A second thread continually enters and leaves a different stage;
            // it must not overwrite this thread's blocked callback diagnostics.
            std::thread([]{for(;;){pulse(2);stage(NIMBY_MOD_WORK_MOD_ACTION,700);Sleep(2);pulse(3);Sleep(2);}}).detach();
            pulse(2);stage(NIMBY_MOD_WORK_MOD_OBSERVE,7);for(;;)Sleep(1000);
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==8) {
            // Nested work and repeated stage changes must retain the outermost
            // callback deadline, even though the current stage keeps progressing.
            pulse(2);stage(NIMBY_MOD_WORK_MOD_OBSERVE,8);Sleep(80);pulse(2);stage(NIMBY_MOD_WORK_MOD_ACTION,800);Sleep(80);pulse(3);
            for(;;){stage(NIMBY_MOD_WORK_MOD_OBSERVE,8);Sleep(40);}
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==17) {
            // Two completed slow callbacks use different details so duplicate
            // message suppression cannot hide an unbounded duration logger.
            // Their successor publishes stage progress while hung, proving
            // stage markers cannot renew the watchdog's outer callback deadline.
            for(uint64_t sequence=1;sequence<=2;++sequence) {
                pulse(2);stage(NIMBY_MOD_WORK_MOD_OBSERVE,1700+sequence);
                if(invoke(sequence))ExitProcess(6);
                Sleep(1200);pulse(3);
            }
            pulse(2);stage(NIMBY_MOD_WORK_MOD_OBSERVE,17);if(invoke(3))ExitProcess(6);
            for(;;){stage(NIMBY_MOD_WORK_MOD_OBSERVE,17);Sleep(40);}
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==18) {
            if(invoke(1))ExitProcess(6);
            pulse(2);stage(NIMBY_MOD_WORK_PUBLISH_TEXTURES,18);
            const auto status=invoke(2); // Parent dispatch deliberately exceeds the RPC reply deadline.
            ExitProcess(status==NIMBY_IO_ERROR?status:6);
        }
        if constexpr(NIMBY_HOST_FIXTURE_MODE==3) {
            pulse(2);std::atomic<uint64_t> busy{};for(;;)busy.fetch_add(1,std::memory_order_relaxed);
        }
        uint64_t sequence=1,previousNs=0;
        while(!stopping) {
            pulse(2);const auto begin=std::chrono::steady_clock::now();
            const auto status=invoke(sequence++,previousNs);
            previousNs=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-begin).count();
            pulse(3);
            if constexpr(NIMBY_HOST_FIXTURE_MODE==6||NIMBY_HOST_FIXTURE_MODE>=9) {
                if(status==12)++throttled;
                else if(status)ExitProcess(status);
            } else if(status)ExitProcess(status);
            if constexpr(NIMBY_HOST_FIXTURE_MODE!=6&&NIMBY_HOST_FIXTURE_MODE<9)Sleep(5);
        }
    });
    return 0;
}
extern "C" __declspec(dllexport) uint32_t WINAPI NRFMod_StopV1(void*) {
    if constexpr(NIMBY_HOST_FIXTURE_MODE==4||NIMBY_HOST_FIXTURE_MODE==12)for(;;)Sleep(1000);
    stopping=true;
    if constexpr(NIMBY_HOST_FIXTURE_MODE==14||NIMBY_HOST_FIXTURE_MODE==15) {
        SetEvent(actionStop);if(worker.joinable())worker.join();CloseHandle(actionStop);actionStop=nullptr;
        return reportAction(3);
    }
    if(worker.joinable())worker.join();
    if constexpr(NIMBY_HOST_FIXTURE_MODE==6||NIMBY_HOST_FIXTURE_MODE>=9)Sleep(200); // Allow local CPU/RPC debt to replenish before cleanup.
    // Real adapters release their panels/providers over RPC while stopping.
    // The supervisor must keep serving bounded cleanup work after stop=true.
    return invoke(UINT64_MAX-1);
}
