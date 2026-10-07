// Fake endpoint for transport tests only. Never links or calls game functions.
#include "platform/windows/runtime/construction_bridge.h"
#include "platform/windows/runtime/clock_bridge.h"
#include <atomic>
using namespace nimby::construction_bridge;
namespace {
HANDLE mapping{};Shared* shared{};volatile LONG paused{},mutations{},holdCompletion{};
HANDLE clockMapping{};std::atomic<nimby::clock_bridge::Shared*> clockEndpoint{};
DWORD WINAPI respond(void*){
    for(;;){
        if(InterlockedCompareExchange(&paused,0,0)){Sleep(1);continue;}
        if(InterlockedCompareExchange(&shared->state,executing,pending)==pending){
            if(nimby::platform::windows::bridgeRequestAlive(shared->lease)){
                InterlockedIncrement(&mutations);
                const bool created=shared->request.action==NIMBY_CONSTRUCTION_CREATE;
                shared->result={sizeof(NimbyConstructionResult),1,created?NIMBY_CONSTRUCTION_APPLIED:NIMBY_CONSTRUCTION_READY,created?1u:0u,123,0,created?1u:0u,{}};
                if(created)shared->result.ids[0]=0x8000000000002;
            }else shared->result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_REJECTED,0,123,1,0,{}};
        }
        if(!InterlockedCompareExchange(&holdCompletion,0,0)&&
           InterlockedCompareExchange(&shared->state,executing,executing)==executing)
            completeResponse(*shared,GetTickCount64());
        auto* clockShared=clockEndpoint.load(std::memory_order_acquire);
        if(clockShared&&InterlockedCompareExchange(&clockShared->state,nimby::clock_bridge::executing,nimby::clock_bridge::pending)==nimby::clock_bridge::pending){
            if(nimby::platform::windows::bridgeRequestAlive(clockShared->lease)){
                InterlockedIncrement(&mutations);clockShared->result=NIMBY_OK;
                clockShared->epoch=clockShared->requested_utc;clockShared->ticks=0;clockShared->count=clockShared->recalculate?3:0;
            }else clockShared->result=NIMBY_DATA_UNAVAILABLE;
            InterlockedExchange(&clockShared->state,nimby::clock_bridge::complete);
        }
        Sleep(1);
    }
}
}
extern "C" __declspec(dllexport) void NimbyTest_Pause(uint32_t value){InterlockedExchange(&paused,value?1:0);}
extern "C" __declspec(dllexport) void NimbyTest_HoldCompletion(uint32_t value){InterlockedExchange(&holdCompletion,value?1:0);}
extern "C" __declspec(dllexport) uint32_t NimbyTest_Mutations(){return InterlockedCompareExchange(&mutations,0,0);}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyTest_ClockBootstrap(void*){
    if(clockEndpoint.load(std::memory_order_acquire))return NIMBY_ALREADY_INITIALIZED;
    clockMapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(nimby::clock_bridge::Shared),nimby::clock_bridge::name(GetCurrentProcessId()).c_str());
    if(!clockMapping)return NIMBY_IO_ERROR;
    auto data=static_cast<nimby::clock_bridge::Shared*>(MapViewOfFile(clockMapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(nimby::clock_bridge::Shared)));
    if(!data)return NIMBY_IO_ERROR;
    *data=nimby::clock_bridge::Shared{};
    clockEndpoint.store(data,std::memory_order_release);return NIMBY_OK;
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void*){
    if(shared)return NIMBY_ALREADY_INITIALIZED;
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared)return NIMBY_IO_ERROR;
    *shared=Shared{};
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&NimbyInternal_Bootstrap),&self))return NIMBY_INTERNAL_ERROR;
    auto worker=CreateThread(nullptr,0,respond,nullptr,0,nullptr);
    if(!worker)return NIMBY_INTERNAL_ERROR;
    CloseHandle(worker);return NIMBY_OK;
}
