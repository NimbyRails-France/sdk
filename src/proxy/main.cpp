#include <windows.h>
#include <stdint.h>
#include <bit>
#include "engine/binary_identity.h"

namespace {
HMODULE self{};
HMODULE original{};
HMODULE sdk{};
using Init = bool (__cdecl*)(uint32_t);
using Quit = void (__cdecl*)();
using Bootstrap = DWORD (WINAPI*)(void*);
using Shutdown = uint32_t (__cdecl*)();
Init init{};
Quit quit{};
Shutdown shutdown_sdk{};
SRWLOCK lock=SRWLOCK_INIT;
bool attempted=false;
bool initialized=false;

void log(const char* text) noexcept {
    OutputDebugStringA("[NimbyRailsFranceSDK proxy] "); OutputDebugStringA(text); OutputDebugStringA("\n");
    wchar_t path[32768]{};
    const DWORD length=GetEnvironmentVariableW(L"LOCALAPPDATA",path,32768);
    if (!length || length+64>=32768) return;
    lstrcatW(path,L"\\NimbyRailsFranceSDK");
    CreateDirectoryW(path,nullptr);
    lstrcatW(path,L"\\proxy.log");
    HANDLE file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE) return;
    DWORD written{};
    WriteFile(file,text,static_cast<DWORD>(lstrlenA(text)),&written,nullptr);
    WriteFile(file,"\r\n",2,&written,nullptr);
    CloseHandle(file);
}
bool sibling_path(const wchar_t* name,wchar_t (&path)[32768]) noexcept {
    const DWORD length=GetModuleFileNameW(self,path,32768);
    if(!length || length+64>=32768) return false;
    DWORD position=length;
    while(position && path[position-1]!=L'\\') --position;
    if(!position) return false;
    path[position]=L'\0'; lstrcatW(path,name); return true;
}
bool resolve_original() noexcept {
    if(init && quit) return true;
    wchar_t path[32768]{};
    if(!sibling_path(L"NimbyRailsSDL3Original.dll",path)) return false;
    original=LoadLibraryExW(path,nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!original) { log("ERROR: original SDL could not be loaded"); return false; }
    init=std::bit_cast<Init>(GetProcAddress(original,"SDL_Init"));
    quit=std::bit_cast<Quit>(GetProcAddress(original,"SDL_Quit"));
    return init && quit;
}
void initialize_sdk() noexcept {
    if(attempted) return;
    attempted=true;
    wchar_t path[32768]{};
    DWORD length=GetModuleFileNameW(nullptr,path,32768);
    NimbyBinaryInfo host{};
    if(!length || length>=32768 || nimby::engine::identify(path,host)!=NIMBY_OK || !host.recognized_research_build) {
        log("REFUSED: unknown executable; SDL continues normally"); return;
    }
    if(!sibling_path(L"NimbyRailsSDL3Original.dll",path)) return;
    NimbyBinaryInfo sdl{};
    constexpr char expected[]="2A2704678BF6C9C6A944270AB35079DF76F5AFE92B780394ED72D9C8218B98D8";
    if(nimby::engine::identify(path,sdl)!=NIMBY_OK || lstrcmpA(sdl.sha256,expected)!=0) {
        log("REFUSED: unknown SDL binary; SDL continues normally"); return;
    }
    if(!sibling_path(L"NimbyRailsFranceSDK.dll",path)) return;
    if(!sdk) sdk=LoadLibraryExW(path,nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    if(!sdk) { log("ERROR: SDK not loaded; SDL continues normally"); return; }
    auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(sdk,"NimbyInternal_Bootstrap"));
    shutdown_sdk=std::bit_cast<Shutdown>(GetProcAddress(sdk,"NimbyInternal_Shutdown"));
    if(!bootstrap || !shutdown_sdk) { log("ERROR: SDK protocol exports missing"); return; }
    const DWORD status=bootstrap(nullptr);
    // Another loader owning this lifecycle is not ours to stop.
    initialized=status==NIMBY_OK;
    log(initialized ? "OK: SDK initialized after SDL_Init; game hooks disabled" :
        status==NIMBY_ALREADY_INITIALIZED ? "INFO: SDK already initialized by another owner" : "ERROR: SDK initialization failed");
}
}

// SDL3 documented C ABI: bool SDL_Init(Uint32), void SDL_Quit(void).
// Called by the application on the main thread, not from our DllMain.
extern "C" bool __cdecl Proxy_SDL_Init(uint32_t flags) noexcept {
    AcquireSRWLockExclusive(&lock);
    if(!resolve_original()) { ReleaseSRWLockExclusive(&lock); return false; }
    ReleaseSRWLockExclusive(&lock);
    const bool result=init(flags);
    const DWORD last_error=GetLastError();
    if(result) {
        AcquireSRWLockExclusive(&lock);
        initialize_sdk();
        ReleaseSRWLockExclusive(&lock);
    }
    SetLastError(last_error);
    return result;
}
extern "C" void __cdecl Proxy_SDL_Quit() noexcept {
    AcquireSRWLockExclusive(&lock);
    const bool resolved=resolve_original();
    const DWORD last_error=GetLastError();
    if(initialized && shutdown_sdk) {
        if(shutdown_sdk()==NIMBY_OK) { initialized=false; attempted=false; log("OK: SDK stopped before SDL_Quit"); }
        else log("ERROR: SDK shutdown failed; module retained");
    }
    ReleaseSRWLockExclusive(&lock);
    SetLastError(last_error);
    if(resolved) quit();
    // Module references intentionally retained until process exit. No hot unload.
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID) {
    if(reason==DLL_PROCESS_ATTACH) self=module;
    return TRUE;
}
