#include <platform/windows/bridge_installation.h>
// Diagnostic only: observes the native editor after its original body returns.
// It does not draw widgets or change signal/save data. Observation expires.
#include "platform/windows/runtime/signal_ui_probe.h"
#include "engine/signal_ui.h"
#include "engine/binary_identity.h"
#include <MinHook.h>
#include <array>
#include <cstring>
namespace {
using Body=void(*)(uint64_t,uint64_t);
Body original{};uint64_t base{};
nimby::signal_ui_probe::Shared* shared{};
HANDLE mapping{};
SRWLOCK initialization=SRWLOCK_INIT,writing=SRWLOCK_INIT;
bool enabled=false;
bool read(void*,uint64_t at,void* out,size_t bytes){
    SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(at),out,bytes,&got)&&got==bytes;
}
void observe(uint64_t capture,uint64_t declaration){
    original(capture,declaration);
    if(GetTickCount64()>=static_cast<uint64_t>(InterlockedCompareExchange64(&shared->expires,0,0)))return;
    const auto ui=nimby::engine::SignalUi::bind(read,nullptr,base,declaration);
    const auto signal=nimby::engine::SignalUi::editorSignal(read,nullptr,capture);
    const uint32_t pass=ui?(ui->pass()==nimby::engine::SignalUi::Pass::Layout?1u:2u):0u;
    AcquireSRWLockExclusive(&writing);
    InterlockedIncrement(&shared->sequence);
    shared->events[shared->count%16]={capture,signal.value_or(0),GetCurrentThreadId(),pass};
    ++shared->count;
    InterlockedIncrement(&shared->sequence);
    ReleaseSRWLockExclusive(&writing);
}
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    nimby::platform::windows::BridgeInstallation installation;
    if(!installation)return installation.status();
    AcquireSRWLockExclusive(&initialization);
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t,32768> path{};NimbyBinaryInfo binary{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
       nimby::engine::identify(path.data(),binary)!=NIMBY_OK||!binary.recognized_research_build)return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr));
    constexpr unsigned char bytes[]{0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48};
    std::array<unsigned char,16> actual{};
    if(!read(nullptr,base+0x7a0a40,actual.data(),actual.size())||std::memcmp(bytes,actual.data(),16))return NIMBY_INVALID_BINARY;
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(*shared),nimby::signal_ui_probe::name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    shared=static_cast<nimby::signal_ui_probe::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(*shared)));
    if(!shared){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    *shared={};
    bool initialized=false,created=false;
    struct Cleanup{bool& initialized;bool& created;~Cleanup(){if(enabled)return;
        if(created)MH_RemoveHook(reinterpret_cast<void*>(base+0x7a0a40));
        if(initialized)MH_Uninitialize();
        UnmapViewOfFile(shared);shared=nullptr;CloseHandle(mapping);mapping=nullptr;
    }} cleanup{initialized,created};
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    initialized=true;
    const auto target=reinterpret_cast<void*>(base+0x7a0a40);
    if(MH_CreateHook(target,reinterpret_cast<void*>(&observe),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    created=true;
    // Keep the trampoline valid even after the diagnostic client exits.
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&observe),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_EnableHook(target)!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;return NIMBY_OK;
}
