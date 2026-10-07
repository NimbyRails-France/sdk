#include <platform/windows/bridge_installation.h>
// Owns an expiring, read-only substitution of a borrowed native texture view.
// No game objects, state tables, reservations or save data are written.
#include "platform/windows/runtime/texture_bridge.h"
#include "engine/binary_identity.h"
#include "engine/simulation_clock.h"
#include <nimby/detail/observation.h>
#include "platform/windows/runtime/texture_dispatch.h"
#include "runtime/texture_publications.h"
#include "platform/windows/runtime/texture_world.h"
#include <nimby/texture_preview.hpp>
#include <MinHook.h>
#include <array>
#include <cstring>

extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTexture_Execute(nimby::texture_bridge::Mailbox*) noexcept;
namespace {
using Select=uintptr_t(__fastcall*)(uintptr_t,int,uint64_t);
Select original{};
uintptr_t base{};
nimby::texture_bridge::Shared* shared{};
SRWLOCK initialization=SRWLOCK_INIT;
bool enabled=false;
nimby::texture_bridge::TexturePublications publications;
nimby::texture_bridge::TextureWorld worlds;
HANDLE request_event{};
DWORD WINAPI command_worker(void*) noexcept {
    for(;;){
        if(WaitForSingleObject(request_event,INFINITE)!=WAIT_OBJECT_0)return 1;
        if(InterlockedCompareExchange(&shared->pending,0,0)!=1)continue;
        nimby::texture_bridge::Mailbox value;
        value.operation=shared->operation;value.request_signal=shared->request_signal;
        value.request_hash=shared->request_hash;value.request_expiry=shared->request_expiry;
        value.request_database=shared->request_database;value.request_simulation=shared->request_simulation;
        value.request_index=shared->request_index;value.request_alternate_index=shared->request_alternate_index;
        value.request_half_period_ms=shared->request_half_period_ms;
        value.request_generation=shared->request_generation;
        const auto result=NimbyTexture_Execute(&value);
        shared->active=value.active;shared->result_index=value.result_index;
        shared->active_count=value.active_count;shared->result_expiry=value.result_expiry;
        shared->result_generation=value.result_generation;
        shared->result=result;
        InterlockedExchange(&shared->pending,0);
    }
}
bool read(uintptr_t address,void* out,size_t size) {
    SIZE_T got{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&got)&&got==size;
}
template<class T> bool read(uintptr_t address,T& out){return read(address,&out,sizeof out);}
bool worldRead(void*,uint64_t address,void* out,size_t size){return read(address,out,size);}
void committed(const nimby::texture_bridge::TextureState& value) noexcept {
    shared->expected_database=value.database;shared->expected_simulation=value.simulation;
    shared->signal=value.signal;shared->expires=value.expires;
}
}

// SDK-owned calls execute directly in the game. The native render hook never
// waits for a client mailbox, a mod callback or a foreign process to finish.
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTexture_PublishV2(uint64_t owner,uint64_t database,uint64_t simulation,uint64_t generation,
    const nimby::texture_bridge::Command* rows,uint32_t count) noexcept {
    if(!owner||!rows||!count||count>nimby::texture_bridge::maxTextureBatch)return NIMBY_INVALID_ARGUMENT;
    if(!shared)return NIMBY_DATA_UNAVAILABLE;
    try{
        std::shared_ptr<nimby::texture_bridge::WorldGuard> world;
        const auto sampled=worlds.capture(worldRead,nullptr,base,database,simulation,world);
        if(sampled!=NIMBY_OK)return sampled;
        if(!generation||generation!=world->generation)return NIMBY_DATA_UNAVAILABLE;
        return publications.update(owner,[&](auto& value)->uint32_t{
            if(!worlds.current(generation))return NIMBY_DATA_UNAVAILABLE;
            if(!value.world||value.world->generation!=generation)value.table.entries.clear();
            const auto status=nimby::texture_bridge::publishTextures(value.table,owner,{rows,count},GetTickCount64());
            if(status!=NIMBY_OK)return status;
            value.database=database;value.simulation=simulation;
            value.world=world;
            value.signal=rows[count-1].signal;value.expires=rows[count-1].expires;
            return worlds.validate(world.get(),worldRead,nullptr,base)?NIMBY_OK:NIMBY_DATA_UNAVAILABLE;
        },committed);
    }catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTexture_Clear(uint64_t owner,const uint64_t* signals,uint32_t count) noexcept {
    if(!owner||(!signals&&count)||count>nimby::texture_bridge::maxTextureBatch)return NIMBY_INVALID_ARGUMENT;
    if(!shared)return NIMBY_DATA_UNAVAILABLE;
    try{return publications.update(owner,[&](auto& value){
        const auto before=value.table.count();
        const auto status=nimby::texture_bridge::clearTextures(value.table,owner,{signals,count});
        return nimby::texture_bridge::TexturePublications::PreparedUpdate{status,value.table.count()!=before};
    },committed,true);}
    catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTexture_Release(uint64_t owner) noexcept {
    if(!owner)return NIMBY_INVALID_ARGUMENT;
    if(!shared)return NIMBY_DATA_UNAVAILABLE;
    try{return publications.update(owner,[&](auto& value){nimby::texture_bridge::releaseTextures(value.table,owner);return uint32_t{NIMBY_OK};},committed,true);}
    catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTexture_Execute(nimby::texture_bridge::Mailbox* value) noexcept {
    if(!value)return NIMBY_INVALID_ARGUMENT;
    if(!shared)return NIMBY_DATA_UNAVAILABLE;
    try{
        std::shared_ptr<nimby::texture_bridge::WorldGuard> world;
        const auto sampled=worlds.capture(worldRead,nullptr,base,value->request_database,value->request_simulation,world);
        if(sampled!=NIMBY_OK)return sampled;
        if(value->operation==4){value->result_generation=world->generation;return NIMBY_OK;}
        if(value->operation==1&&value->request_generation!=world->generation)return NIMBY_DATA_UNAVAILABLE;
        if(value->operation==3){
            const auto current=publications.snapshot();
            value->expected_database=current->database;value->expected_simulation=current->simulation;
            value->signal=current->signal;value->expires=current->expires;
            value->active=0;value->active_count=0;value->result_index=0;value->result_expiry=0;
            if(current->world&&current->world->generation==world->generation){
                const auto now=GetTickCount64();
                for(const auto& command:current->table.entries)if(command.expires>now){
                    ++value->active_count;
                    if(command.signal==value->request_signal){value->active=1;value->result_index=command.index;value->result_expiry=command.expires;}
                }
            }
            return worlds.validate(world.get(),worldRead,nullptr,base)?NIMBY_OK:NIMBY_DATA_UNAVAILABLE;
        }
        return publications.update(0,[&](auto& state)->uint32_t{
            if(!worlds.current(world->generation))return NIMBY_DATA_UNAVAILABLE;
            if(!state.world||state.world->generation!=world->generation){state.table.entries.clear();state.database=0;state.simulation=0;}
            value->expected_database=state.database;value->expected_simulation=state.simulation;
            value->signal=state.signal;value->expires=state.expires;
            if(value->operation==1&&state.database==value->request_database&&state.simulation==value->request_simulation){
                const auto at=state.table.lower(value->request_signal);
                if(state.table.count()>=nimby::texture_bridge::maxOwnedTextures&&
                    (at==state.table.count()||state.table.entries[at].signal!=value->request_signal))return NIMBY_RESOURCE_LIMIT;
            }
            const auto status=nimby::texture_bridge::execute(*value,state.table,GetTickCount64());
            if(status!=NIMBY_OK)return status;
            state.database=value->expected_database;state.simulation=value->expected_simulation;
            state.world=world;
            state.signal=value->signal;state.expires=value->expires;
            return worlds.validate(world.get(),worldRead,nullptr,base)?NIMBY_OK:NIMBY_DATA_UNAVAILABLE;
        },committed,value->operation==2);
    }catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" __declspec(dllexport) uint32_t __cdecl NimbyTexture_Statuses(uint64_t database,uint64_t simulation,
    const uint64_t* signals,uint32_t count,NimbySignalTextureOverrideStatus* out) noexcept {
    if(!signals||!out||!count||count>32)return NIMBY_INVALID_ARGUMENT;
    if(!shared)return NIMBY_DATA_UNAVAILABLE;
    std::shared_ptr<nimby::texture_bridge::WorldGuard> world;
    try{const auto sampled=worlds.capture(worldRead,nullptr,base,database,simulation,world);if(sampled!=NIMBY_OK)return sampled;}
    catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}catch(...){return NIMBY_INTERNAL_ERROR;}
    const auto current=publications.snapshot();const auto& commands=current->table;
    const auto now=GetTickCount64();uint64_t active=0;
    const bool same=current->world&&current->world->generation==world->generation;
    if(same)for(const auto& row:commands.entries)if(row.expires>now)++active;
    for(uint32_t i=0;i<count;++i){
        out[i]={};out[i].struct_size=sizeof(out[i]);out[i].active_count=active;
        const auto at=commands.lower(signals[i]);
        if(same&&at<commands.count()&&commands.entries[at].signal==signals[i]&&commands.entries[at].expires>now){
            const auto& row=commands.entries[at];out[i].active=1;out[i].index=row.index;out[i].expires_at_ms=row.expires;
        }
    }
    return worlds.validate(world.get(),worldRead,nullptr,base)?NIMBY_OK:NIMBY_DATA_UNAVAILABLE;
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
    // Pin an immutable version only while copying one entry. No lock, allocation
    // or old-vector destruction can run on this native render thread.
    nimby::engine::SimulationClock clock;
    const auto selectedCommand=publications.read(record[0],[&](auto* world){return worlds.validate(world,worldRead,nullptr,base,&clock);});
    const auto command=selectedCommand.command;
    const auto db=selectedCommand.database;
    const auto id=command.signal,set=command.set_hash,expiry=command.expires;
    auto index=command.index;
    if(id!=record[0]||!expiry||GetTickCount64()>=expiry)return normal;
    if(rules!=db+0xa80)return normal;
    if(command.half_period_ms){
        index=nimby::texture_bridge::frame_index(command,static_cast<uint64_t>(clock.ticks)*10);
    }
    // Most aspects select another frame of the signal's own catalogue. Reuse
    // the native lookup already performed for fallback in that common case.
    const auto selected=set==hash?normal:original(rules,kind,set);
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
    if(!IsProcessorFeaturePresent(PF_COMPARE_EXCHANGE128))return NIMBY_INVALID_BINARY;
    nimby::platform::windows::BridgeInstallation installation;
    if(!installation)return installation.status();
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
