/* Project-owned, private MinHook adaptation. See ../../README.md.
 * The upstream MinHook license and thread suspension/context code are unchanged. */
#pragma once
#include <windows.h>
#if !defined(MH_FORCE_PSS_COMPAT) && __has_include(<processsnapshot.h>)
#include <processsnapshot.h>
#else
#include "thread_snapshot_compat.h"
#endif
#include <limits.h>

typedef struct _MH_THREAD_LIST
{
    LPDWORD pItems;
    UINT capacity;
    UINT size;
} MH_THREAD_LIST;

typedef struct _MH_THREAD_SNAPSHOT_API
{
    DWORD (WINAPI *capture)(HANDLE, PSS_CAPTURE_FLAGS, DWORD, HPSS*);
    DWORD (WINAPI *freeSnapshot)(HANDLE, HPSS);
    DWORD (WINAPI *createMarker)(const PSS_ALLOCATOR*, HPSSWALK*);
    DWORD (WINAPI *freeMarker)(HPSSWALK);
    DWORD (WINAPI *walk)(HPSS, PSS_WALK_INFORMATION_CLASS, HPSSWALK, void*, DWORD);
    LPVOID (WINAPI *allocate)(HANDLE, DWORD, SIZE_T);
    LPVOID (WINAPI *reallocate)(HANDLE, DWORD, LPVOID, SIZE_T);
    BOOL (WINAPI *free)(HANDLE, DWORD, LPVOID);
} MH_THREAD_SNAPSHOT_API;

/* Called with an empty list, before any thread is suspended. PSS captures only
 * the current process's thread metadata: no address-space clone or contexts.
 * Only a complete walk may become a suspension list. Every failure discards
 * the partial list, so the caller can perform the original Toolhelp walk. */
static BOOL MHCollectPssThreads(const MH_THREAD_SNAPSHOT_API* api, HANDLE heap,
    DWORD processId, DWORD currentThreadId, MH_THREAD_LIST* list)
{
    HPSS snapshot = NULL;
    HPSSWALK marker = NULL;
    BOOL succeeded = FALSE;
    DWORD status;
    if (!api || !api->capture || !api->freeSnapshot || !api->createMarker ||
        !api->freeMarker || !api->walk || !api->allocate || !api->reallocate || !api->free)
        return FALSE;
    status = api->capture(GetCurrentProcess(), PSS_CAPTURE_THREADS, 0, &snapshot);
    if (status != ERROR_SUCCESS)
        return FALSE;
    if (!snapshot)
        return FALSE;
    status = api->createMarker(NULL, &marker);
    if (status != ERROR_SUCCESS)
        marker = NULL;
    if (status == ERROR_SUCCESS && marker)
    {
        for (;;)
        {
            PSS_THREAD_ENTRY entry;
            status = api->walk(snapshot, PSS_WALK_THREADS, marker, &entry, sizeof(entry));
            if (status == ERROR_NO_MORE_ITEMS)
            {
                succeeded = TRUE;
                break;
            }
            if (status != ERROR_SUCCESS)
                break;
            if (entry.ProcessId != processId || !entry.ThreadId || entry.ThreadId == currentThreadId ||
                (entry.Flags & PSS_THREAD_FLAGS_TERMINATED))
                continue;
            if (list->size == list->capacity)
            {
                UINT capacity;
                LPDWORD items;
                if (list->capacity > UINT_MAX / 2)
                    break;
                capacity = list->capacity ? list->capacity * 2 : 128;
                if ((SIZE_T)capacity > (SIZE_T)-1 / sizeof(DWORD))
                    break;
                items = list->pItems ? (LPDWORD)api->reallocate(heap, 0, list->pItems, capacity * sizeof(DWORD)) :
                    (LPDWORD)api->allocate(heap, 0, capacity * sizeof(DWORD));
                if (!items)
                    break;
                list->pItems = items;
                list->capacity = capacity;
            }
            list->pItems[list->size++] = entry.ThreadId;
        }
    }
    /* Snapshot/marker handles are PSS resources, never CloseHandle targets.
     * Release both before Freeze() can suspend a thread owning a system lock. */
    if (marker && api->freeMarker(marker) != ERROR_SUCCESS)
        succeeded = FALSE;
    if (api->freeSnapshot(GetCurrentProcess(), snapshot) != ERROR_SUCCESS)
        succeeded = FALSE;
    if (!succeeded)
    {
        if (list->pItems)
            api->free(heap, 0, list->pItems);
        list->pItems = NULL;
        list->size = list->capacity = 0;
    }
    return succeeded;
}

static BOOL MHEnumerateThreads(const MH_THREAD_SNAPSHOT_API* api, HANDLE heap,
    DWORD processId, DWORD currentThreadId, MH_THREAD_LIST* list, BOOL (*fallback)(MH_THREAD_LIST*))
{
    if (MHCollectPssThreads(api, heap, processId, currentThreadId, list))
        return TRUE;
    return fallback ? fallback(list) : FALSE;
}
