// Owns an expiring, read-only substitution of a borrowed native texture view.
// No game objects, state tables, reservations or save data are written.
#include "runtime/texture_bridge.h"
#include "engine/binary_identity.h"
#include <nimby/detail/observation.h>
#include "runtime/texture_commands.h"
#include <MinHook.h>
#include <array>
#include <cstring>

namespace {
using Select=uintptr_t(__fastcall*)(uintptr_t,int,uint64_t);
Select original{};
uintptr_t base{};
nimby::texture_bridge::Shared* shared{};
SRWLOCK initialization=SRWLOCK_INIT;
bool enabled=false;
SRWLOCK table_lock=SRWLOCK_INIT;
nimby::texture_bridge::Table commands;
HANDLE request_event{};
DWORD WINAPI command_worker(void*) noexcept {
    for(;;){
        if(WaitForSingleObject(request_event,INFINITE)!=WAIT_OBJECT_0)return 1;
        if(InterlockedCompareExchange(&shared->pending,0,0)!=1)continue;
        AcquireSRWLockExclusive(&table_lock);
        const auto result=nimby::texture_bridge::execute(*shared,commands,GetTickCount64());
        ReleaseSRWLockExclusive(&table_lock);
        shared->result=result;
        InterlockedExchange(&shared->pending,0);
    }
}
bool read(uintptr_t address,void* out,size_t size) {
    SIZE_T got{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;
}
template<class T> bool read(uintptr_t address,T& out){return read(address,&out,sizeof out);}
}

extern "C" uintptr_t texture_select(uintptr_t rules,int kind,uint64_t hash,
                                     uintptr_t signal,uintptr_t caller) noexcept {
    const auto normal=original(rules,kind,hash);
    // R13 is only a Signal* at this exact, fingerprinted native draw call site.
    if(caller!=base+0x62062b || !shared)return normal;
    InterlockedIncrement64(&shared->callbacks);
    shared->render_thread=static_cast<LONG>(GetCurrentThreadId());
    std::array<uint64_t,8> record{};
    if(!read(signal,record.data(),sizeof record) || record[0]>>48!=8 ||
       static_cast<int>(record[6])!=kind || record[7]!=hash)return normal;
    InterlockedIncrement64(&shared->valid_signals);
    InterlockedExchange64(&shared->last_signal,static_cast<LONG64>(record[0]));
    InterlockedExchange64(&shared->last_rules,static_cast<LONG64>(rules));
    // The dynamic table belongs to this DLL, never to shared memory. Copy one
    // entry while locked; allocation and vector growth happen on the worker.
    nimby::texture_bridge::Command command{};
    uint64_t db{},sim{};
    AcquireSRWLockShared(&table_lock);
    const auto at=commands.lower(record[0]);
    if(at<commands.count())command=commands.entries[at];
    db=shared->expected_database;sim=shared->expected_simulation;
    ReleaseSRWLockShared(&table_lock);
    const auto id=command.signal,set=command.set_hash,expiry=command.expires;
    const auto index=command.index;
    if(id!=record[0]||!expiry||GetTickCount64()>=expiry)return normal;
    uintptr_t root{},current_db{},current_sim{};
    if(!read(base+0xb81998,root)||!read(root+0x540,current_db)||!read(root+0x680,current_sim)||
       current_db!=db||current_sim!=sim||rules!=db+0xa80)return normal;
    const auto selected=original(rules,kind,set);
    if(!selected)return normal;
    // Return a non-owning, thread-local view of ONE file. Native clamping then
    // chooses index zero regardless of its own computed texture selector.
    // Copying does not construct or destroy the game's MSVC strings/vectors.
    alignas(16) thread_local std::array<unsigned char,0x90> view{};
    if(!read(selected,view.data(),view.size()))return normal;
    uintptr_t begin{},end{};
    std::memcpy(&begin,view.data()+0x78,8);std::memcpy(&end,view.data()+0x80,8);
    if(end<begin || (end-begin)%0x50 || (end-begin)/0x50>4096 || index>=(end-begin)/0x50)return normal;
    begin+=index*0x50;end=begin+0x50;
    std::memcpy(view.data()+0x78,&begin,8);std::memcpy(view.data()+0x80,&end,8);
    std::memcpy(view.data()+0x88,&end,8);
    InterlockedIncrement64(&shared->applied);
    return reinterpret_cast<uintptr_t>(view.data());
}

// Preserve the native ABI; forward the caller's nonvolatile R13 as extra
// diagnostic context. The compiler-generated handler preserves all callee saves.
extern "C" __attribute__((naked)) uintptr_t texture_thunk() {
    __asm__ volatile("sub $0x38, %rsp\n"
                     "mov 0x38(%rsp), %rax\n"
                     "mov %rax, 0x20(%rsp)\n"
                     "mov %r13, %r9\n"
                     "call texture_select\n"
                     "add $0x38, %rsp\n"
                     "ret\n");
}

extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    AcquireSRWLockExclusive(&initialization);
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t,32768> path{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size())))return NIMBY_IO_ERROR;
    NimbyBinaryInfo info{};
    if(nimby::engine::identify(path.data(),info)!=NIMBY_OK||!info.recognized_research_build)return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
    constexpr unsigned char entry[]={0x48,0x89,0x5c,0x24,0x08,0x8b,0x99,0x48,0x01,0x00,0x00,0x49,0x8b,0xc0,0x4c,0x8b};
    constexpr unsigned char call[]={0x4d,0x8b,0x45,0x38,0x48,0x8b,0x8d,0x40,0x03,0x00,0x00,0xe8,0xa5,0x42,0xdf,0xff};
    if(std::memcmp(reinterpret_cast<void*>(base+0x4148d0),entry,sizeof entry)||
       std::memcmp(reinterpret_cast<void*>(base+0x62061b),call,sizeof call))return NIMBY_INVALID_BINARY;
    const auto mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(*shared),
        nimby::texture_bridge::name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);return NIMBY_IO_ERROR;}
    shared=static_cast<nimby::texture_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(*shared)));
    if(!shared){CloseHandle(mapping);return NIMBY_IO_ERROR;}
    *shared=nimby::texture_bridge::Shared{};
    const auto target=reinterpret_cast<void*>(base+0x4148d0);
    bool initialized=false,created=false;
    struct Cleanup{HANDLE mapping;void* target;bool& init;bool& created;
        ~Cleanup(){if(enabled)return;if(created)MH_RemoveHook(target);if(init)MH_Uninitialize();
            UnmapViewOfFile(shared);shared=nullptr;CloseHandle(mapping);}
    }cleanup{mapping,target,initialized,created};
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    initialized=true;
    if(MH_CreateHook(target,reinterpret_cast<void*>(&texture_thunk),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    created=true;
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&texture_select),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_EnableHook(target)!=MH_OK)return NIMBY_INTERNAL_ERROR;
    request_event=CreateEventW(nullptr,FALSE,FALSE,
        (nimby::texture_bridge::name(GetCurrentProcessId())+L".Request").c_str());
    if(!request_event){MH_DisableHook(target);return NIMBY_IO_ERROR;}
    const auto worker=CreateThread(nullptr,0,command_worker,nullptr,0,nullptr);
    if(!worker){CloseHandle(request_event);MH_DisableHook(target);return NIMBY_IO_ERROR;}
    CloseHandle(worker);
    enabled=true;
    return NIMBY_OK;
}
