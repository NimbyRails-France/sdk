// Fake endpoint for transport tests only. Never links or calls game functions.
#include "platform/windows/runtime/construction_bridge.h"
using namespace nimby::construction_bridge;
namespace {
HANDLE mapping{};Shared* shared{};
DWORD WINAPI respond(void*){
    for(;;){
        if(InterlockedCompareExchange(&shared->state,executing,pending)==pending){
            shared->result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_READY,0,123,0,0,{}};
            InterlockedExchange(&shared->state,complete);
        }
        Sleep(1);
    }
}
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
