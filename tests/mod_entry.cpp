#include <nimby/mod.hpp>
#include <stdexcept>
#include <cstdio>

extern "C" DWORD WINAPI NRFMod_StartV1(void*) noexcept;
extern "C" DWORD WINAPI NRFMod_StopV1(void*) noexcept;
extern "C" DWORD WINAPI NRFMod_IsInitializedV1(void*) noexcept;
extern "C" DWORD WINAPI NRFMod_ShowTextureV1(uint64_t, const char*) noexcept;
extern "C" DWORD WINAPI NRFMod_RestoreTextureV1(uint64_t) noexcept;
namespace {
bool fail_start = false, fail_stop = false;
int starts = 0, stops = 0, shows = 0, restores = 0;
}
nimby::Mod nimby::createMod() {
    return {
        .showTexture = [](Id signal, const char*) {
            if (signal == 2) throw Exception({3, "test", "typed error"});
            if (signal == 3) throw std::runtime_error("unexpected error");
            ++shows;
        },
        .restoreTexture = [](Id) { ++restores; },
        .start = [] { if (fail_start) throw std::runtime_error("start failed"); ++starts; },
        .stop = [] { if (fail_stop) throw std::runtime_error("stop failed"); ++stops; }
    };
}
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "Failed line %d: %s\n", __LINE__, #x); return 1; } } while (false)
int main() {
    CHECK(NRFMod_IsInitializedV1(nullptr) == 0);
    CHECK(NRFMod_ShowTextureV1(1, "image.svg") == NIMBY_INVALID_HANDLE);
    CHECK(NRFMod_RestoreTextureV1(1) == NIMBY_INVALID_HANDLE);
    CHECK(NRFMod_StartV1(reinterpret_cast<void*>(1)) == NIMBY_INVALID_ARGUMENT);
    fail_start = true;
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_INTERNAL_ERROR && NRFMod_IsInitializedV1(nullptr) == 0);
    fail_start = false;
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_OK && starts == 1);
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_ALREADY_INITIALIZED && starts == 1);
    CHECK(NRFMod_ShowTextureV1(0, "image.svg") == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(1, nullptr) == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(1, "") == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_ShowTextureV1(1, "image.svg") == NIMBY_OK && shows == 1);
    CHECK(NRFMod_ShowTextureV1(2, "image.svg") == NIMBY_INVALID_BINARY);
    CHECK(NRFMod_ShowTextureV1(3, "image.svg") == NIMBY_INTERNAL_ERROR);
    CHECK(NRFMod_RestoreTextureV1(0) == NIMBY_INVALID_ARGUMENT);
    CHECK(NRFMod_RestoreTextureV1(1) == NIMBY_OK && restores == 1);
    CHECK(NRFMod_StopV1(reinterpret_cast<void*>(1)) == NIMBY_INVALID_ARGUMENT);
    fail_stop = true;
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_INTERNAL_ERROR && NRFMod_IsInitializedV1(nullptr) == 1);
    fail_stop = false;
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_OK && NRFMod_IsInitializedV1(nullptr) == 0 && stops == 1);
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_OK && stops == 1);
    CHECK(NRFMod_StartV1(nullptr) == NIMBY_OK && starts == 2);
    CHECK(NRFMod_StopV1(nullptr) == NIMBY_OK && stops == 2);
    std::puts("PASS adapter lifecycle, command dispatch, validation and exception boundaries");
}
