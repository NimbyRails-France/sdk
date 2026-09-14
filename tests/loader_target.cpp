#include <windows.h>
#include <nimby/sdk.h>
#include <bit>
#include <MinHook.h>

using LoadLibraryFunction=HMODULE (WINAPI*)(LPCWSTR);
static LoadLibraryFunction original_load{};
static volatile LONG load_calls{};
static HMODULE WINAPI observe_load(LPCWSTR path) {
    InterlockedIncrement(&load_calls);
    return original_load(path);
}

// Owned process fixture. Never launches, emulates, or reads game data.
int wmain(int argc,wchar_t** argv) {
    if (argc!=4) return 1;
    HANDLE ready=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[1]);
    HANDLE stop=OpenEventW(SYNCHRONIZE,FALSE,argv[2]);
    HANDLE observed=OpenEventW(EVENT_MODIFY_STATE,FALSE,argv[3]);
    if (!ready || !stop || !observed) return 2;
    // Regression: existing instrumentation of the Windows loader entry must
    // remain installed and be called normally, never bypassed by our loader.
    const auto load=GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"LoadLibraryW");
    void* trampoline{};
    if (MH_Initialize()!=MH_OK || MH_CreateHook(std::bit_cast<void*>(load),std::bit_cast<void*>(&observe_load),&trampoline)!=MH_OK) return 5;
    original_load=std::bit_cast<LoadLibraryFunction>(trampoline);
    if(MH_EnableHook(std::bit_cast<void*>(load))!=MH_OK) return 6;
    SetEvent(ready);
    bool initialized=false;
    HMODULE sdk{};
    for (int iteration=0;iteration<3000;++iteration) {
        if (WaitForSingleObject(stop,10)==WAIT_OBJECT_0) break;
        sdk=GetModuleHandleW(L"NimbyRailsSDK.dll");
        if (!sdk) continue;
        auto info=std::bit_cast<decltype(&NimbySdk_GetHostInfo)>(GetProcAddress(sdk,"NimbySdk_GetHostInfo"));
        if (!info) return 3;
        NimbyBinaryInfo host{}; host.struct_size=sizeof host;
        if (info(&host)==NIMBY_OK && host.file_size && !host.recognized_research_build && InterlockedCompareExchange(&load_calls,0,0)>0) {
            initialized=true; SetEvent(observed);
        }
    }
    if(MH_DisableHook(std::bit_cast<void*>(load))!=MH_OK || MH_RemoveHook(std::bit_cast<void*>(load))!=MH_OK || MH_Uninitialize()!=MH_OK) return 7;
    if (initialized) {
        auto stop_sdk=std::bit_cast<decltype(&NimbySdk_Shutdown)>(GetProcAddress(sdk,"NimbySdk_Shutdown"));
        if (!stop_sdk || stop_sdk()!=NIMBY_OK) return 4;
        FreeLibrary(sdk); // Release the load reference after all fixture callers stop.
    }
    CloseHandle(ready); CloseHandle(stop); CloseHandle(observed);
    return 0;
}
