#pragma once
#include <stdint.h>
#include <wchar.h>

#ifdef NIMBY_SDK_BUILD
#define NIMBY_API __declspec(dllexport)
#else
#define NIMBY_API __declspec(dllimport)
#endif
#ifdef __cplusplus
#define NIMBY_NOEXCEPT noexcept
extern "C" {
#else
#define NIMBY_NOEXCEPT
#endif

// Experimental ABI v1: fixed-width values, caller-owned buffers, no C++ objects.
#define NIMBY_ABI_VERSION 1u
#define NIMBY_REQUEST_HOOKS 1u
#define NIMBY_OK 0u
#define NIMBY_INVALID_ARGUMENT 1u
#define NIMBY_IO_ERROR 2u
#define NIMBY_INVALID_BINARY 3u
#define NIMBY_ALREADY_INITIALIZED 4u
#define NIMBY_HOOKS_UNAVAILABLE 5u
#define NIMBY_INTERNAL_ERROR 6u

typedef struct NimbyBinaryInfo {
    uint32_t struct_size;
    uint32_t recognized_research_build;
    uint64_t file_size;
    char sha256[65];
    uint8_t reserved[7];
} NimbyBinaryInfo;

// Read-only file inspection; recognition does NOT authorize any hook.
NIMBY_API uint32_t __cdecl NimbySdk_InspectBinary(const wchar_t* path, NimbyBinaryInfo* out) NIMBY_NOEXCEPT;
// Call explicitly after LoadLibrary returns, outside every DllMain/TLS callback.
// flags=0 starts diagnostics and identifies the actual host executable.
// NIMBY_REQUEST_HOOKS always fails in this milestone, leaving state unchanged.
NIMBY_API uint32_t __cdecl NimbySdk_Initialize(uint32_t abi_version, uint32_t flags) NIMBY_NOEXCEPT;
// Idempotent. Caller must join all SDK callers before Shutdown + FreeLibrary.
NIMBY_API uint32_t __cdecl NimbySdk_Shutdown(void) NIMBY_NOEXCEPT;
// Copies an immutable snapshot. Fails when stopped. No borrowed game objects.
NIMBY_API uint32_t __cdecl NimbySdk_GetHostInfo(NimbyBinaryInfo* out) NIMBY_NOEXCEPT;

#ifdef __cplusplus
}
#endif
