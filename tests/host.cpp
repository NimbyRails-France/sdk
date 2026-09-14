#include <nimby/sdk.h>
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <thread>
#include <vector>
#include <string>
#include <bit>

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (false)
// Loads only our SDK into this standalone process. Never loads or launches the game.
int main(int argc, char** argv) {
    CHECK(argc >= 2);
    for (int cycle = 0; cycle < 3; ++cycle) {
        HMODULE dll = LoadLibraryA(argv[1]);
        CHECK(dll);
        auto init = std::bit_cast<decltype(&NimbySdk_Initialize)>(GetProcAddress(dll, "NimbySdk_Initialize"));
        auto stop = std::bit_cast<decltype(&NimbySdk_Shutdown)>(GetProcAddress(dll, "NimbySdk_Shutdown"));
        auto info = std::bit_cast<decltype(&NimbySdk_GetHostInfo)>(GetProcAddress(dll, "NimbySdk_GetHostInfo"));
        auto inspect = std::bit_cast<decltype(&NimbySdk_InspectBinary)>(GetProcAddress(dll, "NimbySdk_InspectBinary"));
        CHECK(init && stop && info && inspect);
        NimbyBinaryInfo b{}; b.struct_size = sizeof b;
        CHECK(info(&b) == NIMBY_INVALID_ARGUMENT);
        CHECK(init(99, 0) == NIMBY_INVALID_ARGUMENT);
        CHECK(init(1, 2) == NIMBY_INVALID_ARGUMENT);
        CHECK(init(1, NIMBY_REQUEST_HOOKS) == NIMBY_HOOKS_UNAVAILABLE);
        CHECK(info(&b) == NIMBY_INVALID_ARGUMENT);
        CHECK(init(1, 0) == NIMBY_OK);
        CHECK(init(1, 0) == NIMBY_ALREADY_INITIALIZED);
        CHECK(info(&b) == NIMBY_OK && !b.recognized_research_build && b.sha256[64] == 0 && b.file_size > 0);
        CHECK(init(1, NIMBY_REQUEST_HOOKS) == NIMBY_HOOKS_UNAVAILABLE);
        CHECK(info(&b) == NIMBY_OK);
        CHECK(inspect(nullptr, &b) == NIMBY_INVALID_ARGUMENT);
        CHECK(inspect(L"", &b) == NIMBY_INVALID_ARGUMENT);
        CHECK(info(nullptr) == NIMBY_INVALID_ARGUMENT);
        NimbyBinaryInfo wrong{};
        CHECK(info(&wrong) == NIMBY_INVALID_ARGUMENT);
        CHECK(inspect(L"unused", &wrong) == NIMBY_INVALID_ARGUMENT);
        std::atomic<bool> failed{};
        std::vector<std::thread> callers;
        for (int t=0; t<8; ++t) callers.emplace_back([&] {
            for(int n=0;n<100;++n) { NimbyBinaryInfo snapshot{}; snapshot.struct_size=sizeof snapshot;
                if(info(&snapshot)!=NIMBY_OK || snapshot.recognized_research_build) failed=true;
            }
        });
        for(auto& thread:callers) thread.join();
        CHECK(!failed);
        if (argc == 3 && cycle == 0) {
            const int length=MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, argv[2], -1, nullptr, 0);
            CHECK(length>0);
            std::wstring path(static_cast<size_t>(length), L'\0');
            CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, argv[2], -1, path.data(), length)>0);
            CHECK(inspect(path.c_str(), &b) == NIMBY_OK);
            std::printf("Inspected file SHA256=%s recognized=%u bytes=%llu\n", b.sha256, b.recognized_research_build, static_cast<unsigned long long>(b.file_size));
            CHECK(info(&b) == NIMBY_OK && !b.recognized_research_build); // Inspection cannot replace host identity.
        }
        CHECK(stop() == NIMBY_OK);
        CHECK(stop() == NIMBY_OK);
        CHECK(info(&b) == NIMBY_INVALID_ARGUMENT);
        CHECK(init(1, 0) == NIMBY_OK);
        CHECK(stop() == NIMBY_OK);
        CHECK(FreeLibrary(dll));
    }
    std::puts("PASS DLL lifecycle, concurrent snapshots, fail-closed hooks, reload (3 cycles)");
}
