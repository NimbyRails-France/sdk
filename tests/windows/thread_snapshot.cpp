#include <windows.h>
#include "thread_snapshot.h"
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

#define CHECK(x) do { if (!(x)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return false; } } while (false)

namespace {
// Check the Windows x64 ABI with both system and compatibility declarations.
static_assert(sizeof(PSS_THREAD_ENTRY) == 120);
static_assert(offsetof(PSS_THREAD_ENTRY, ProcessId) == 16);
static_assert(offsetof(PSS_THREAD_ENTRY, ThreadId) == 20);
static_assert(offsetof(PSS_THREAD_ENTRY, Flags) == 104);
static_assert(offsetof(PSS_THREAD_ENTRY, ContextRecord) == 112);
static_assert(PSS_CAPTURE_THREADS == 0x80 && PSS_WALK_THREADS == 3);
static_assert(PSS_THREAD_FLAGS_TERMINATED == 1);

struct Fake {
    std::vector<PSS_THREAD_ENTRY> entries;
    DWORD captureStatus{ERROR_SUCCESS}, markerStatus{ERROR_SUCCESS};
    DWORD freeSnapshotStatus{ERROR_SUCCESS}, freeMarkerStatus{ERROR_SUCCESS};
    int failWalk{-1}, failAllocate{}, failReallocate{};
    unsigned captureCalls{}, snapshotFrees{}, markerCalls{}, markerFrees{}, walks{};
    unsigned allocations{}, reallocations{}, frees{}, liveBlocks{}, fallbackCalls{};
    bool valid{true}, fallbackEmpty{true}, fallbackSucceeds{true};
};
Fake* active{};
HPSS snapshot() { return reinterpret_cast<HPSS>(static_cast<ULONG_PTR>(0x1234)); }
HPSSWALK marker() { return reinterpret_cast<HPSSWALK>(static_cast<ULONG_PTR>(0x5678)); }
DWORD WINAPI capture(HANDLE process, PSS_CAPTURE_FLAGS flags, DWORD context, HPSS* result) {
    auto& f = *active;
    ++f.captureCalls;
    f.valid &= process == GetCurrentProcess() && flags == PSS_CAPTURE_THREADS && context == 0;
    if (f.captureStatus == ERROR_SUCCESS) *result = snapshot();
    return f.captureStatus;
}
DWORD WINAPI freeSnapshot(HANDLE process, HPSS value) {
    auto& f = *active;
    ++f.snapshotFrees;
    f.valid &= process == GetCurrentProcess() && value == snapshot();
    return f.freeSnapshotStatus;
}
DWORD WINAPI createMarker(const PSS_ALLOCATOR* allocator, HPSSWALK* result) {
    auto& f = *active;
    ++f.markerCalls;
    f.valid &= allocator == nullptr;
    if (f.markerStatus == ERROR_SUCCESS) *result = marker();
    return f.markerStatus;
}
DWORD WINAPI freeMarker(HPSSWALK value) {
    auto& f = *active;
    ++f.markerFrees;
    f.valid &= value == marker();
    return f.freeMarkerStatus;
}
DWORD WINAPI walk(HPSS value, PSS_WALK_INFORMATION_CLASS kind, HPSSWALK position, void* out, DWORD size) {
    auto& f = *active;
    f.valid &= value == snapshot() && position == marker() && kind == PSS_WALK_THREADS && size == sizeof(PSS_THREAD_ENTRY);
    const auto index = f.walks++;
    if (f.failWalk >= 0 && index == static_cast<unsigned>(f.failWalk)) return ERROR_BAD_LENGTH;
    if (index >= f.entries.size()) return ERROR_NO_MORE_ITEMS;
    *static_cast<PSS_THREAD_ENTRY*>(out) = f.entries[index];
    return ERROR_SUCCESS;
}
LPVOID WINAPI allocate(HANDLE heap, DWORD flags, SIZE_T bytes) {
    auto& f = *active;
    if (++f.allocations == static_cast<unsigned>(f.failAllocate)) return nullptr;
    auto* value = HeapAlloc(heap, flags, bytes);
    if (value) ++f.liveBlocks;
    return value;
}
LPVOID WINAPI reallocate(HANDLE heap, DWORD flags, LPVOID value, SIZE_T bytes) {
    auto& f = *active;
    if (++f.reallocations == static_cast<unsigned>(f.failReallocate)) return nullptr;
    return HeapReAlloc(heap, flags, value, bytes);
}
BOOL WINAPI freeMemory(HANDLE heap, DWORD flags, LPVOID value) {
    auto& f = *active;
    ++f.frees;
    const auto result = HeapFree(heap, flags, value);
    if (result) --f.liveBlocks;
    return result;
}
MH_THREAD_SNAPSHOT_API fakeApi() {
    return {capture, freeSnapshot, createMarker, freeMarker, walk, allocate, reallocate, freeMemory};
}
BOOL fallback(MH_THREAD_LIST* list) {
    auto& f = *active;
    ++f.fallbackCalls;
    f.fallbackEmpty &= list->pItems == nullptr && list->size == 0 && list->capacity == 0;
    return f.fallbackSucceeds ? TRUE : FALSE;
}
PSS_THREAD_ENTRY entry(DWORD pid, DWORD tid, PSS_THREAD_FLAGS flags = PSS_THREAD_FLAGS_NONE) {
    PSS_THREAD_ENTRY result{};
    result.ProcessId = pid;
    result.ThreadId = tid;
    result.Flags = flags;
    return result;
}
void release(const MH_THREAD_SNAPSHOT_API& api, MH_THREAD_LIST& list) {
    if (list.pItems) api.free(GetProcessHeap(), 0, list.pItems);
    list = {};
}
bool fakeContracts() {
    const auto api = fakeApi();
    {
        Fake f; active = &f;
        f.entries = {entry(42, 7), entry(42, 8), entry(41, 9), entry(42, 10, PSS_THREAD_FLAGS_TERMINATED), entry(42, 0)};
        MH_THREAD_LIST list{};
        CHECK(MHEnumerateThreads(&api, GetProcessHeap(), 42, 7, &list, fallback));
        CHECK(f.valid && f.fallbackCalls == 0 && f.snapshotFrees == 1 && f.markerFrees == 1);
        CHECK(list.size == 1 && list.pItems[0] == 8 && f.liveBlocks == 1);
        release(api, list);
        CHECK(f.liveBlocks == 0 && f.frees == 1);
    }
    {
        Fake f; active = &f;
        MH_THREAD_LIST list{};
        CHECK(MHEnumerateThreads(&api, GetProcessHeap(), 42, 7, &list, fallback));
        CHECK(f.valid && list.size == 0 && !list.pItems && f.fallbackCalls == 0);
        CHECK(f.snapshotFrees == 1 && f.markerFrees == 1 && f.allocations == 0);
    }
    for (int missing = 0; missing < 8; ++missing) {
        Fake f; active = &f;
        auto incomplete = api;
        switch (missing) {
            case 0: incomplete.capture = nullptr; break;
            case 1: incomplete.freeSnapshot = nullptr; break;
            case 2: incomplete.createMarker = nullptr; break;
            case 3: incomplete.freeMarker = nullptr; break;
            case 4: incomplete.walk = nullptr; break;
            case 5: incomplete.allocate = nullptr; break;
            case 6: incomplete.reallocate = nullptr; break;
            case 7: incomplete.free = nullptr; break;
        }
        MH_THREAD_LIST list{};
        CHECK(MHEnumerateThreads(&incomplete, GetProcessHeap(), 42, 7, &list, fallback));
        CHECK(f.fallbackCalls == 1 && f.fallbackEmpty && f.captureCalls == 0 && f.liveBlocks == 0);
    }
    for (int failure = 0; failure < 8; ++failure) {
        Fake f; active = &f;
        for (DWORD id = 1; id <= 140; ++id) f.entries.push_back(entry(42, id));
        switch (failure) {
            case 0: f.captureStatus = ERROR_ACCESS_DENIED; break;
            case 1: f.markerStatus = ERROR_NOT_ENOUGH_MEMORY; break;
            case 2: f.failWalk = 0; break;
            case 3: f.failWalk = 2; break; // Never accept the already copied partial list.
            case 4: f.failAllocate = 1; break;
            case 5: f.failReallocate = 1; break;
            case 6: f.freeMarkerStatus = ERROR_INVALID_HANDLE; break;
            case 7: f.freeSnapshotStatus = ERROR_INVALID_HANDLE; break;
        }
        MH_THREAD_LIST list{};
        CHECK(MHEnumerateThreads(&api, GetProcessHeap(), 42, 999, &list, fallback));
        CHECK(f.valid && f.fallbackCalls == 1 && f.fallbackEmpty && f.liveBlocks == 0);
        CHECK(!list.pItems && list.size == 0 && list.capacity == 0);
        CHECK(f.snapshotFrees == (failure == 0 ? 0U : 1U));
        CHECK(f.markerFrees == (failure <= 1 ? 0U : 1U));
        if (failure == 3 || failure == 5) CHECK(f.frees == 1);
    }
    {
        Fake f; active = &f;
        f.entries = {entry(42, 8), entry(42, 9)};
        f.failWalk = 1;
        f.fallbackSucceeds = false;
        MH_THREAD_LIST list{};
        CHECK(!MHEnumerateThreads(&api, GetProcessHeap(), 42, 7, &list, fallback));
        CHECK(f.valid && f.fallbackCalls == 1 && f.fallbackEmpty && f.liveBlocks == 0);
        CHECK(!list.pItems && list.size == 0 && list.capacity == 0);
    }
    {
        Fake f; active = &f;
        for (DWORD id = 1; id <= 140; ++id) f.entries.push_back(entry(42, id));
        MH_THREAD_LIST list{};
        CHECK(MHCollectPssThreads(&api, GetProcessHeap(), 42, 999, &list));
        CHECK(f.valid && list.size == 140 && f.reallocations == 1);
        for (DWORD index = 0; index < list.size; ++index) CHECK(list.pItems[index] == index + 1);
        release(api, list);
        CHECK(f.liveBlocks == 0);
    }
    active = nullptr;
    return true;
}

template<class T> T function(HMODULE module, const char* name) {
    return std::bit_cast<T>(GetProcAddress(module, name));
}
MH_THREAD_SNAPSHOT_API realApi() {
    const auto kernel = GetModuleHandleW(L"kernel32.dll");
    MH_THREAD_SNAPSHOT_API result{};
    result.capture = function<decltype(result.capture)>(kernel, "PssCaptureSnapshot");
    result.freeSnapshot = function<decltype(result.freeSnapshot)>(kernel, "PssFreeSnapshot");
    result.createMarker = function<decltype(result.createMarker)>(kernel, "PssWalkMarkerCreate");
    result.freeMarker = function<decltype(result.freeMarker)>(kernel, "PssWalkMarkerFree");
    result.walk = function<decltype(result.walk)>(kernel, "PssWalkSnapshot");
    result.allocate = HeapAlloc;
    result.reallocate = HeapReAlloc;
    result.free = HeapFree;
    return result;
}
DWORD WINAPI waitForEvent(void* event) { return WaitForSingleObject(event, INFINITE) == WAIT_OBJECT_0 ? 0 : 1; }
DWORD WINAPI finishImmediately(void*) { return 0; }
struct LiveThreads {
    HANDLE event{CreateEventW(nullptr, TRUE, FALSE, nullptr)};
    std::vector<HANDLE> handles;
    std::vector<DWORD> ids;
    LiveThreads() {
        if (!event) return;
        for (int i = 0; i < 16; ++i) {
            DWORD id{};
            if (const auto thread = CreateThread(nullptr, 0, waitForEvent, event, 0, &id)) {
                handles.push_back(thread);
                ids.push_back(id);
            }
        }
    }
    ~LiveThreads() {
        if (event) SetEvent(event);
        if (!handles.empty()) WaitForMultipleObjects(static_cast<DWORD>(handles.size()), handles.data(), TRUE, 5000);
        for (const auto handle : handles) CloseHandle(handle);
        if (event) CloseHandle(event);
    }
};
bool realContracts() {
    const auto api = realApi();
    CHECK(api.capture && api.freeSnapshot && api.createMarker && api.freeMarker && api.walk);
    LiveThreads threads;
    CHECK(threads.handles.size() == 16);
    DWORD terminatedId{};
    const auto terminated = CreateThread(nullptr, 0, finishImmediately, nullptr, 0, &terminatedId);
    CHECK(terminated && WaitForSingleObject(terminated, 5000) == WAIT_OBJECT_0);
    // Keep the terminated thread handle open: PSS can still enumerate this record.
    struct ClosedHandle { HANDLE handle; ~ClosedHandle() { CloseHandle(handle); } } closed{terminated};
    auto captureAndCheck = [&]() -> bool {
        MH_THREAD_LIST list{};
        CHECK(MHCollectPssThreads(&api, GetProcessHeap(), GetCurrentProcessId(), GetCurrentThreadId(), &list));
        std::vector<DWORD> actual;
        if (list.size) actual.assign(list.pItems, list.pItems + list.size);
        release(api, list);
        std::sort(actual.begin(), actual.end());
        CHECK(std::adjacent_find(actual.begin(), actual.end()) == actual.end());
        CHECK(!std::binary_search(actual.begin(), actual.end(), GetCurrentThreadId()));
        CHECK(!std::binary_search(actual.begin(), actual.end(), terminatedId));
        for (const auto id : threads.ids) CHECK(std::binary_search(actual.begin(), actual.end(), id));
        return true;
    };
    CHECK(captureAndCheck());
    DWORD before{}, after{};
    CHECK(GetProcessHandleCount(GetCurrentProcess(), &before));
    for (int cycle = 0; cycle < 100; ++cycle) CHECK(captureAndCheck());
    CHECK(GetProcessHandleCount(GetCurrentProcess(), &after));
    CHECK(before == after);
    std::printf("PASS PSS 16 live threads, terminated thread excluded, 100 captures handles %lu -> %lu\n", before, after);
    return true;
}
}

int main(int argc, char** argv) {
    if (argc == 2 && std::strcmp(argv[1], "--live") == 0) {
        const auto api = realApi();
        if (!api.capture || !api.freeSnapshot || !api.createMarker || !api.freeMarker || !api.walk) {
            std::puts("SKIP live PSS capture: required Windows exports are unavailable; fake/fallback contracts run separately.");
            return 77;
        }
        return realContracts() ? 0 : 1;
    }
    if (argc != 1 || !fakeContracts()) return 1;
    std::puts("PASS complete PSS capture, exact cleanup, partial/allocation failures and clean Toolhelp fallback contract");
    return 0;
}
