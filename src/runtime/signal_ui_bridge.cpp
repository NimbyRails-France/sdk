// Experimental resident renderer for the verified NIMBY Rails binary.
// Registration does not enable a panel: a real session and a complete observed
// signal catalog must first be supplied by the SDK observation worker.
#include "runtime/signal_ui_endpoint.h"
#include "runtime/signal_settings_panel.h"
#include "engine/binary_identity.h"
#include <MinHook.h>
#include <array>

namespace {
using Body=void(*)(uint64_t,uint64_t);
Body original{};
uint64_t base{};
bool enabled=false;
SRWLOCK initialization=SRWLOCK_INIT;
nimby::runtime::SignalUiEndpoint endpoint;
bool readMemory(void*,uint64_t at,void* out,size_t bytes) {
    SIZE_T got{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(at),out,bytes,&got)&&got==bytes;
}
void render(uint64_t capture,uint64_t declaration) {
    original(capture,declaration);
    thread_local nimby::runtime::SignalUiPresentation presentation;
    try {
        nimby::runtime::draw_signal_settings_panels(readMemory,nullptr,base,capture,declaration,endpoint.host,presentation);
        if(presentation.takeFailedWrites())OutputDebugStringA("NIMBY SDK: signal checkbox write rejected by storage failure\n");
    }catch(...) {
        // Never unwind a C++ exception into the game's native UI.
        OutputDebugStringA("NIMBY SDK: signal settings rendering failed\n");
    }
}
}
#define UI_EXPORT extern "C" __declspec(dllexport) uint32_t
UI_EXPORT NimbyUi_RegisterV1(const NimbyUiPanelV1* panel,uint64_t* owner) noexcept {return endpoint.add(panel,owner);}
UI_EXPORT NimbyUi_RemoveV1(uint64_t owner) noexcept {return endpoint.remove(owner);}
UI_EXPORT NimbyUi_BeginV1(uint64_t owner,const char* identity,uint32_t length,uint64_t* session) noexcept {return endpoint.begin(owner,identity,length,session);}
UI_EXPORT NimbyUi_ObserveV1(uint64_t owner,uint64_t session,const NimbyUiSignalV1* signals,uint32_t count) noexcept {return endpoint.observe(owner,session,signals,count);}
UI_EXPORT NimbyUi_SuspendV1(uint64_t owner) noexcept {return endpoint.suspend(owner);}
UI_EXPORT NimbyUi_ReadV1(uint64_t owner,uint64_t signal,NimbyUiValuesV1* values) noexcept {return endpoint.read(owner,signal,values);}
UI_EXPORT NimbyUi_ExportV1(uint64_t owner,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written) noexcept {return endpoint.exportSettings(owner,session,bytes,capacity,written);}
UI_EXPORT NimbyUi_BeginSavedV1(uint64_t owner,const char* identity,uint32_t identityLength,const char* bytes,uint32_t length,uint64_t* session) noexcept {return endpoint.beginSaved(owner,identity,identityLength,bytes,length,session);}

extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    AcquireSRWLockExclusive(&initialization);
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t,32768> path{};NimbyBinaryInfo binary{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
       nimby::engine::identify(path.data(),binary)!=NIMBY_OK||!binary.recognized_research_build)return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr));
    constexpr unsigned char bytes[]{0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48};
    std::array<unsigned char,16> actual{};
    if(!readMemory(nullptr,base+0x7a0a40,actual.data(),actual.size())||std::memcmp(bytes,actual.data(),16))return NIMBY_INVALID_BINARY;
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    bool created=false;
    const auto target=reinterpret_cast<void*>(base+0x7a0a40);
    struct Cleanup{bool& created;void* target;~Cleanup(){if(enabled)return;
        if(created)MH_RemoveHook(target);
        MH_Uninitialize();
    }} cleanup{created,target};
    if(MH_CreateHook(target,reinterpret_cast<void*>(&render),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    created=true;
    // The trampoline, TLS frames and owned registry must survive mod unloads.
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&render),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_EnableHook(target)!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;return NIMBY_OK;
}
