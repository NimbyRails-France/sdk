// Only the two private host discovery exports needed by standalone transports.
// This DLL is built in construction-transport-fixture, never beside the game.
#include <windows.h>
#include <nimby/detail/observation.h>
#include <cstdint>
#include <bit>
#include <cstring>
namespace {uint32_t calls{},target{};}
extern "C" __declspec(dllexport) void NimbyTest_SetTarget(uint32_t value){target=value;}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyInternal_ModHostTarget(){return target?target:GetCurrentProcessId();}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTest_HostCalls(){return calls;}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* arguments,
    const void* input,uint32_t size,void* output,uint32_t capacity,uint32_t* written){
    if(operation==6){
        if(!arguments||input||size||!output||capacity<3||!written)return NIMBY_INVALID_ARGUMENT;
        std::memcpy(output,"fra",3);*written=3;return NIMBY_OK;
    }
    if((operation!=4&&operation!=5)||!arguments||input||size||output||capacity||!written)return NIMBY_INVALID_ARGUMENT;
    for(size_t i=0;i<8;++i)if(arguments[i])return NIMBY_INVALID_ARGUMENT;
    *written=0;++calls;
    // A child transport test targets the already initialized parent fixture;
    // it never injects a DLL or calls a real game bootstrap.
    if(target&&target!=GetCurrentProcessId())return NIMBY_OK;
    using Bootstrap=DWORD(WINAPI*)(void*);
    const auto module=GetModuleHandleW(L"NimbyConstructionBridge-experimental-v1.dll");
    const auto bootstrap=module?std::bit_cast<Bootstrap>(GetProcAddress(module,operation==4?"NimbyTest_ClockBootstrap":"NimbyInternal_Bootstrap")):nullptr;
    if(!bootstrap)return NIMBY_DATA_UNAVAILABLE;
    const auto status=bootstrap(nullptr);return status==NIMBY_ALREADY_INITIALIZED?NIMBY_OK:status;
}
