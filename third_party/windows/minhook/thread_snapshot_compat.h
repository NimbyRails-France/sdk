/* Private Win32 PSS declarations for MinGW toolchains predating processsnapshot.h.
 * Only the dynamically resolved thread-walk ABI used by MinHook is declared.
 * Contract: https://learn.microsoft.com/windows/win32/api/processsnapshot/
 * Keep the complete thread entry: PssWalkSnapshot checks its buffer size. */
#pragma once
#include <windows.h>

typedef enum { PSS_CAPTURE_THREADS = 0x00000080 } PSS_CAPTURE_FLAGS;
typedef enum { PSS_WALK_THREADS = 3 } PSS_WALK_INFORMATION_CLASS;
typedef enum
{
    PSS_THREAD_FLAGS_NONE = 0,
    PSS_THREAD_FLAGS_TERMINATED = 1
} PSS_THREAD_FLAGS;

DECLARE_HANDLE(HPSS);
DECLARE_HANDLE(HPSSWALK);

typedef struct
{
    DWORD ExitStatus;
    void* TebBaseAddress;
    DWORD ProcessId;
    DWORD ThreadId;
    ULONG_PTR AffinityMask;
    int Priority;
    int BasePriority;
    void* LastSyscallFirstArgument;
    WORD LastSyscallNumber;
    FILETIME CreateTime;
    FILETIME ExitTime;
    FILETIME KernelTime;
    FILETIME UserTime;
    void* Win32StartAddress;
    FILETIME CaptureTime;
    PSS_THREAD_FLAGS Flags;
    WORD SuspendCount;
    WORD SizeOfContextRecord;
    PCONTEXT ContextRecord;
} PSS_THREAD_ENTRY;

typedef struct
{
    void* Context;
    void* (WINAPI *AllocRoutine)(void* Context, DWORD Size);
    void (WINAPI *FreeRoutine)(void* Context, void* Address);
} PSS_ALLOCATOR;
