// Compiled into each consumer DLL via NimbyRailsFranceSDK::Mod.
#include <nimby/mod.hpp>

namespace {
SRWLOCK lock = SRWLOCK_INIT;
nimby::Mod callbacks{};
bool initialized = false;
struct Guard {
    bool exclusive;
    explicit Guard(bool value) : exclusive(value) {
        if (exclusive) AcquireSRWLockExclusive(&lock); else AcquireSRWLockShared(&lock);
    }
    ~Guard() {
        if (exclusive) ReleaseSRWLockExclusive(&lock); else ReleaseSRWLockShared(&lock);
    }
};
template<class Function> DWORD boundary(Function operation) noexcept {
    try { return operation(); }
    catch (const nimby::Exception& error) { return static_cast<DWORD>(error.code()); }
    catch (...) { return NIMBY_INTERNAL_ERROR; }
}
}

extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_StartV1(void* reserved) noexcept {
    if (reserved) return NIMBY_INVALID_ARGUMENT;
    Guard guard(true);
    if (initialized) return NIMBY_ALREADY_INITIALIZED;
    return boundary([]() -> DWORD {
        auto candidate = nimby::createMod();
        if (candidate.start) candidate.start();
        callbacks = candidate;
        initialized = true;
        return NIMBY_OK;
    });
}
extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_StopV1(void* reserved) noexcept {
    if (reserved) return NIMBY_INVALID_ARGUMENT;
    Guard guard(true);
    return boundary([]() -> DWORD {
        if (initialized && callbacks.stop) callbacks.stop();
        callbacks = {};
        initialized = false;
        return NIMBY_OK;
    });
}
extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_IsInitializedV1(void*) noexcept {
    Guard guard(false);
    return initialized ? 1 : 0;
}
extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_ShowTextureV1(uint64_t signal, const char* path) noexcept {
    Guard guard(false);
    if (!initialized) return NIMBY_INVALID_HANDLE;
    if (!signal || !path || !*path) return NIMBY_INVALID_ARGUMENT;
    if (!callbacks.showTexture) return NIMBY_HOOKS_UNAVAILABLE;
    return boundary([&]() -> DWORD { callbacks.showTexture(signal, path); return NIMBY_OK; });
}
extern "C" __declspec(dllexport) DWORD WINAPI NRFMod_RestoreTextureV1(uint64_t signal) noexcept {
    Guard guard(false);
    if (!initialized) return NIMBY_INVALID_HANDLE;
    if (!signal) return NIMBY_INVALID_ARGUMENT;
    if (!callbacks.restoreTexture) return NIMBY_HOOKS_UNAVAILABLE;
    return boundary([&]() -> DWORD { callbacks.restoreTexture(signal); return NIMBY_OK; });
}
BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
