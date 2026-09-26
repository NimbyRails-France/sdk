#include <windows.h>
#include <bit>
#include <filesystem>
#include <cstdio>
#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"FAIL line %d: %s (Windows %lu)\n",__LINE__,#x,GetLastError());return 1;} } while(false)
int wmain(int argc,wchar_t** argv) {
    CHECK(argc==2);
    const auto path=std::filesystem::absolute(argv[1]);
    const auto original_path=path.parent_path()/L"NimbyRailsSDL3Original.dll";
    HMODULE original=LoadLibraryExW(original_path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    CHECK(original);
    HMODULE proxy=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
    CHECK(proxy);
    using Init=bool (__cdecl*)(unsigned);
    using Quit=void (__cdecl*)();
    using Version=int (__cdecl*)();
    using WasInit=unsigned (__cdecl*)(unsigned);
    const auto init=std::bit_cast<Init>(GetProcAddress(proxy,"SDL_Init"));
    const auto quit=std::bit_cast<Quit>(GetProcAddress(proxy,"SDL_Quit"));
    const auto was_init=std::bit_cast<WasInit>(GetProcAddress(proxy,"SDL_WasInit"));
    const auto version=std::bit_cast<Version>(GetProcAddress(proxy,"SDL_GetVersion"));
    CHECK(init && quit && was_init && version);
    CHECK(GetProcAddress(proxy,"SDL_GetVersion")==GetProcAddress(original,"SDL_GetVersion"));
    CHECK(GetProcAddress(proxy,"SDL_WasInit")==GetProcAddress(original,"SDL_WasInit"));
    CHECK(GetProcAddress(proxy,"SDL_Init")!=GetProcAddress(original,"SDL_Init"));
    for(int cycle=0;cycle<3;++cycle) {
        CHECK(init(0)); // No video/audio/game/saves. SDL flags zero is supported.
        CHECK(was_init(0)==0);
        CHECK(GetModuleHandleW(L"NimbyRailsFranceSDK.dll")==nullptr); // Unknown host: activation refused.
        quit();
    }
    std::printf("PASS SDL proxy forwarding, version=%d, unknown host refused, 3 init/quit cycles\n",version());
    FreeLibrary(proxy); FreeLibrary(original);
    return 0;
}
