// Dedicated, explicitly loaded bridge. DllMain never installs a hook.
#include "runtime/clock_bridge.h"
#include <nimby/detail/observation.h>
#include "engine/binary_identity.h"
#include "engine/simulation_clock.h"
#include <MinHook.h>
#include <array>
#include <cstring>
#include <vector>

namespace {
using PostUpdate=double(__fastcall*)(uintptr_t,void*,uintptr_t,uint8_t);
PostUpdate original{};
nimby::clock_bridge::Shared* shared{};
HANDLE mapping{};
uintptr_t base{};
SRWLOCK initialization=SRWLOCK_INIT;
bool enabled=false;
bool read_local(void*,uint64_t address,void* out,size_t size) {
    SIZE_T got{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;
}
uint32_t apply(uintptr_t sim,uintptr_t db) {
    using namespace nimby::engine;
    SimulationClock before{},after{};
    uint64_t root{},currentSim{},currentDb{};
    if(!read_local(nullptr,base+0xb81998,&root,8) ||
       !read_local(nullptr,root+0x680,&currentSim,8) || !read_local(nullptr,root+0x540,&currentDb,8) ||
       currentSim!=sim || currentDb!=db)return NIMBY_DATA_UNAVAILABLE;
    if(sim!=shared->expected_sim || !read_simulation_clock(read_local,nullptr,sim,before) ||
        !rebase_clock(before,shared->requested_utc,after))return NIMBY_INVALID_ARGUMENT;
    std::vector<CalendarWrite> edits;
    if(!plan_simulation_calendar(read_local,nullptr,sim,after.epoch_seconds-before.epoch_seconds,edits))
        return NIMBY_DATA_UNAVAILABLE;
    // Exactly the native mode-3 collection and intervention used by the UI.
    // Its temporary tree is created and destroyed by the game's own allocator.
    using Collect=void*(__fastcall*)(uintptr_t,void*,uintptr_t,uintptr_t,uint8_t);
    using Intervene=uintptr_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,void*);
    using WeekSeconds=int32_t(__fastcall*)(void*);
    alignas(16) std::array<uint64_t,6> tree{};
    reinterpret_cast<Collect>(base+0x44cc70)(sim+0xa0,tree.data(),db,sim,0);
    shared->count=static_cast<uint32_t>(tree[4]);
    for(const auto& edit:edits)std::memcpy(reinterpret_cast<void*>(edit.address),&edit.after,8);
    std::memcpy(reinterpret_cast<void*>(sim+0x20),&after.epoch_seconds,8);
    std::memcpy(reinterpret_cast<void*>(sim+0x30),&after.epoch_seconds,8);
    const auto weekSeconds=reinterpret_cast<WeekSeconds>(base+0x4775e0)(reinterpret_cast<void*>(sim+0x20));
    const int64_t utc=after.epoch_seconds+after.ticks/100,weekStart=utc-weekSeconds;
    std::memcpy(reinterpret_cast<void*>(sim+0x20f8),&weekSeconds,4);
    std::memcpy(reinterpret_cast<void*>(sim+0x2100),&weekStart,8);
    std::memcpy(reinterpret_cast<void*>(sim+0x2108),&utc,8);
    reinterpret_cast<Intervene>(base+0x44c9b0)(sim+0xa0,db,sim,tree.data());
    ++*reinterpret_cast<uint64_t*>(sim+0x2120);
    shared->epoch=after.epoch_seconds;shared->ticks=after.ticks;
    return NIMBY_OK;
}

double __fastcall post_update(uintptr_t sim,void* context,uintptr_t db,uint8_t flag) {
    const double result=original(sim,context,db,flag);
    shared->thread_id=GetCurrentThreadId();
    shared->observed_sim=sim;
    shared->observed_db=db;
    if(InterlockedCompareExchange(&shared->state,nimby::clock_bridge::idle,nimby::clock_bridge::idle)==nimby::clock_bridge::idle) {
        std::memcpy(&shared->epoch,reinterpret_cast<void*>(sim+0x20),8);
        std::memcpy(&shared->ticks,reinterpret_cast<void*>(sim+0x28),8);
    }
    InterlockedIncrement64(reinterpret_cast<volatile LONG64*>(&shared->callbacks));
    if(InterlockedCompareExchange(&shared->state,nimby::clock_bridge::executing,nimby::clock_bridge::pending)
        ==nimby::clock_bridge::pending) {
        try {shared->result=apply(sim,db);} catch(...) {shared->result=NIMBY_INTERNAL_ERROR;}
        InterlockedExchange(&shared->state,nimby::clock_bridge::complete);
    }
    return result;
}
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    AcquireSRWLockExclusive(&initialization);
    struct Unlock {~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t,32768> path{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size())))return NIMBY_IO_ERROR;
    NimbyBinaryInfo identity{};
    if(nimby::engine::identify(path.data(),identity)!=NIMBY_OK || !identity.recognized_research_build)
        return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    struct Entry {uintptr_t rva;std::array<unsigned char,16> bytes;};
    constexpr Entry entries[]={
        {0x478f40,{0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x55,0x56,0x57,0x48,0x83,0xec}},
        {0x44cc70,{0x48,0x89,0x5c,0x24,0x08,0x4c,0x89,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48}},
        {0x44c9b0,{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x4c,0x89,0x4c,0x24,0x20,0x56}},
        {0x4775e0,{0x48,0x89,0x5c,0x24,0x18,0x48,0x89,0x6c,0x24,0x20,0x89,0x54,0x24,0x10,0x56,0x57}}
    };
    for(const auto& entry:entries)if(std::memcmp(reinterpret_cast<void*>(base+entry.rva),entry.bytes.data(),16)!=0)
        return NIMBY_INVALID_BINARY;
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,
        sizeof(nimby::clock_bridge::Shared),nimby::clock_bridge::name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    shared=static_cast<nimby::clock_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(*shared)));
    if(!shared){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    *shared=nimby::clock_bridge::Shared{};
    bool initialized=false,created=false;
    struct Cleanup {
        bool& initialized;bool& created;
        ~Cleanup(){if(enabled)return;if(created)MH_RemoveHook(reinterpret_cast<void*>(base+0x478f40));
            if(initialized)MH_Uninitialize();
            original=nullptr;UnmapViewOfFile(shared);shared=nullptr;CloseHandle(mapping);mapping=nullptr;}
    } cleanup{initialized,created};
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    initialized=true;
    const auto target=reinterpret_cast<void*>(base+0x478f40);
    if(MH_CreateHook(target,reinterpret_cast<void*>(&post_update),reinterpret_cast<void**>(&original))!=MH_OK)
        return NIMBY_INTERNAL_ERROR;
    created=true;
    // The bridge owns a live detour and must remain mapped until game exit.
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&post_update),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_EnableHook(target)!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;
    return NIMBY_OK;
}
