#pragma once
#include <windows.h>

// Loader protocol, not part of the mod API. Called after LoadLibrary completes.
// Windows thread entry signature, including a 32-bit status result (never HMODULE).
extern "C" __declspec(dllexport) DWORD WINAPI NimbySdk_Bootstrap(void* reserved) noexcept;
