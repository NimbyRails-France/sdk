#include <nimby/texture_preview.hpp>
#include <nimby/signal_textures.hpp>
#include "engine/texture_file.h"
#include "runtime/texture_bridge.h"
#include "engine/binary_identity.h"
#include "engine/network.h"
#include "engine/signal_textures.h"
#include "loader/loader.h"
#include <tlhelp32.h>
#include <array>
#include <filesystem>
#include <cstring>
#include <bit>

namespace {
struct Handle {HANDLE value{};~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}};
struct Connection {
    Handle process,mapping,mutex,event;
    nimby::texture_bridge::Shared* shared{};
    nimby::engine::LiveState live{};
    bool locked=false;
    ~Connection(){if(shared)UnmapViewOfFile(shared);if(locked)ReleaseMutex(mutex.value);}
};
bool read(void* context,uint64_t at,void* out,size_t size){
    SIZE_T got{};auto& c=*static_cast<Connection*>(context);
    return ReadProcessMemory(c.process.value,reinterpret_cast<void*>(at),out,size,&got)&&got==size;
}
uint32_t connect(uint32_t pid,Connection& c) {
    if(!pid)return NIMBY_INVALID_ARGUMENT;
    c.process.value=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|SYNCHRONIZE,FALSE,pid);
    if(!c.process.value)return NIMBY_IO_ERROR;
    if(WaitForSingleObject(c.process.value,0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
    std::array<wchar_t,32768> path{};DWORD length=static_cast<DWORD>(path.size());
    if(!QueryFullProcessImageNameW(c.process.value,0,path.data(),&length))return NIMBY_IO_ERROR;
    NimbyBinaryInfo binary{};
    if(nimby::engine::identify(path.data(),binary)!=NIMBY_OK||!binary.recognized_research_build)return NIMBY_INVALID_BINARY;
    Handle modules{CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid)};
    MODULEENTRY32W entry{};entry.dwSize=sizeof entry;
    if(modules.value==INVALID_HANDLE_VALUE||!Module32FirstW(modules.value,&entry))return NIMBY_IO_ERROR;
    const auto base=reinterpret_cast<uint64_t>(entry.modBaseAddr);
    if(!nimby::engine::resolve_live_state(read,&c,base,true,c.live))return NIMBY_DATA_UNAVAILABLE;
    c.mutex.value=CreateMutexW(nullptr,FALSE,(nimby::texture_bridge::name(pid)+L".Client").c_str());
    if(!c.mutex.value)return NIMBY_IO_ERROR;
    const auto wait=WaitForSingleObject(c.mutex.value,1000);
    if(wait!=WAIT_OBJECT_0&&wait!=WAIT_ABANDONED)return NIMBY_RESOURCE_LIMIT;
    c.locked=true;
    HMODULE owner{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&connect),&owner))return NIMBY_IO_ERROR;
    std::array<wchar_t,32768> sdk{};
    if(!GetModuleFileNameW(owner,sdk.data(),static_cast<DWORD>(sdk.size())))return NIMBY_IO_ERROR;
    const auto bridge=std::filesystem::path(sdk.data()).parent_path()/nimby::texture_bridge::filename;
    // Verify any resident module has exactly this build before using its mapping.
    NimbyBinaryInfo expected{};
    if(nimby::engine::identify(bridge.c_str(),expected)!=NIMBY_OK)return NIMBY_DATA_UNAVAILABLE;
    bool loaded=false;
    do {
        // Two bridges must never hook the same native function. Restart the game
        // when upgrading from the pinned v1 bridge.
        if(!_wcsicmp(entry.szModule,L"NimbyRailsFranceTextureBridge-experimental-v1.dll")||
           !_wcsicmp(entry.szModule,L"NimbyRailsFranceTextureBridge-experimental-v2.dll"))return NIMBY_INVALID_BINARY;
        if(_wcsicmp(entry.szModule,nimby::texture_bridge::filename))continue;
        NimbyBinaryInfo resident{};
        if(nimby::engine::identify(entry.szExePath,resident)!=NIMBY_OK||std::strcmp(resident.sha256,expected.sha256))return NIMBY_INVALID_BINARY;
        loaded=true;
    }while(Module32NextW(modules.value,&entry));
    if(!loaded){
        if(pid==GetCurrentProcessId()) {
            // Already inside the game: initialize directly, outside loader lock.
            // Do not create a remote thread targeting our own process.
            const auto module=LoadLibraryExW(bridge.c_str(),nullptr,
                LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
            if(!module)return NIMBY_IO_ERROR;
            using Bootstrap=DWORD (WINAPI*)(void*);
            const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
            const auto status=bootstrap?bootstrap(nullptr):NIMBY_INVALID_BINARY;
            // Bridge pins itself before enabling hooks. Release our load reference.
            FreeLibrary(module);
            if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED)return status;
        }else{
            nimby::loader::Monitor monitor(path.data(),binary.sha256,bridge.wstring());
            if(!monitor.attach_process(pid).success)return NIMBY_IO_ERROR;
        }
    }
    c.mapping.value=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,nimby::texture_bridge::name(pid).c_str());
    if(!c.mapping.value)return NIMBY_DATA_UNAVAILABLE;
    c.shared=static_cast<nimby::texture_bridge::Shared*>(MapViewOfFile(c.mapping.value,FILE_MAP_ALL_ACCESS,0,0,sizeof(*c.shared)));
    if(!c.shared)return NIMBY_IO_ERROR;
    if(c.shared->protocol!=nimby::texture_bridge::version||c.shared->size!=sizeof(*c.shared))return NIMBY_INVALID_ARGUMENT;
    c.event.value=OpenEventW(EVENT_MODIFY_STATE,FALSE,(nimby::texture_bridge::name(pid)+L".Request").c_str());
    if(!c.event.value)return NIMBY_IO_ERROR;
    // Recover the wakeup if a caller exited after publishing its request.
    if(InterlockedCompareExchange(&c.shared->pending,0,0) && !SetEvent(c.event.value))return NIMBY_IO_ERROR;
    // Complete any request left in flight by an interrupted caller before reuse.
    while(InterlockedCompareExchange(&c.shared->pending,0,0)){
        if(WaitForSingleObject(c.process.value,10)==WAIT_OBJECT_0)return NIMBY_PROCESS_EXITED;
    }
    return NIMBY_OK;
}
uint32_t submit(Connection& c,uint32_t operation,uint64_t signal,uint64_t hash=0,
                uint64_t expiry=0,uint32_t index=0) {
    c.shared->operation=operation;c.shared->request_signal=signal;
    c.shared->request_hash=hash;c.shared->request_expiry=expiry;c.shared->request_index=index;
    c.shared->request_database=c.live.database;c.shared->request_simulation=c.live.simulation;
    InterlockedExchange(&c.shared->pending,1);
    if(!SetEvent(c.event.value)){InterlockedExchange(&c.shared->pending,0);return NIMBY_IO_ERROR;}
    while(InterlockedCompareExchange(&c.shared->pending,0,0)){
        if(WaitForSingleObject(c.process.value,10)==WAIT_OBJECT_0)return NIMBY_PROCESS_EXITED;
    }
    return c.shared->result;
}
}
static uint32_t set_texture(uint32_t pid,uint64_t signal,const char* set_id,
                           uint32_t index,uint32_t duration,bool persistent,const char* file=nullptr) noexcept {
    if(!signal||!set_id||!set_id[0]||(!persistent&&(duration<1000||duration>60000)))return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=connect(pid,c);if(status!=NIMBY_OK)return status;
        nimby::engine::Network network{};nimby::engine::SignalTextureCatalog catalog{};
        if(!nimby::engine::read_network(read,&c,c.live,true,network)||
           !nimby::engine::read_signal_texture_catalog(read,&c,c.live,true,catalog))return NIMBY_DATA_UNAVAILABLE;
        bool found=false;for(const auto& s:network.signals)if(s.id==signal){found=true;break;}
        if(!found)return NIMBY_INVALID_ARGUMENT;
        uint64_t hash{};
        if(file){
            const auto resolved=nimby::engine::resolve_texture_file(catalog,set_id,file,hash,index);
            if(resolved!=NIMBY_OK)return resolved;
        }else{
            for(const auto& [key,set]:catalog.sets)if(set.name==set_id){
                if(index>=set.files.size())return NIMBY_INVALID_ARGUMENT;
                hash=key;break;
            }
        }
        if(!hash)return NIMBY_DATA_UNAVAILABLE;
        nimby::engine::LiveState after{};
        if(!nimby::engine::resolve_live_state(read,&c,c.live.module_base,true,after)||after!=c.live)return NIMBY_DATA_UNAVAILABLE;
        return submit(c,1,signal,hash,persistent?UINT64_MAX:GetTickCount64()+duration,index);
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_ShowSignalTexture(uint32_t pid,uint64_t signal,const char* set,const char* path) noexcept {
    if(!path||!path[0])return NIMBY_INVALID_ARGUMENT;
    return set_texture(pid?pid:GetCurrentProcessId(),signal,set,0,0,true,path);
}
uint32_t __cdecl NimbyInternal_RestoreSignalTexture(uint32_t pid,uint64_t signal) noexcept {
    return NimbyInternal_ClearTexturePreview(pid?pid:GetCurrentProcessId(),signal);
}
uint32_t __cdecl NimbyInternal_PreviewSignalTexture(uint32_t pid,uint64_t signal,const char* set_id,
                                                  uint32_t index,uint32_t duration) noexcept {
    return set_texture(pid,signal,set_id,index,duration,false);
}
uint32_t __cdecl NimbyInternal_ForceSignalTexture(uint32_t pid,uint64_t signal,const char* set_id,
                                                uint32_t index) noexcept {
    return set_texture(pid,signal,set_id,index,0,true);
}
uint32_t __cdecl NimbyInternal_TexturePreviewStatus(uint32_t pid,NimbyTexturePreviewStatus* out) noexcept {
    if(!out||out->struct_size!=sizeof(*out))return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=connect(pid,c);if(status!=NIMBY_OK)return status;
        out->render_thread=c.shared->render_thread;
        out->callbacks=InterlockedCompareExchange64(&c.shared->callbacks,0,0);
        out->valid_signals=InterlockedCompareExchange64(&c.shared->valid_signals,0,0);
        out->applied=InterlockedCompareExchange64(&c.shared->applied,0,0);
        out->last_signal=InterlockedCompareExchange64(&c.shared->last_signal,0,0);
        out->last_rules=InterlockedCompareExchange64(&c.shared->last_rules,0,0);
        out->signal=c.shared->signal;out->expires_at_ms=c.shared->expires;
        return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_ClearTexturePreview(uint32_t pid,uint64_t signal) noexcept {
    if(!signal)return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=connect(pid,c);if(status!=NIMBY_OK)return status;
        return submit(c,2,signal);
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_SignalTextureOverrideStatus(uint32_t pid,uint64_t signal,
                                                         NimbySignalTextureOverrideStatus* out) noexcept {
    if(!signal||!out||out->struct_size!=sizeof(*out))return NIMBY_INVALID_ARGUMENT;
    try {
        Connection c;const auto status=connect(pid,c);if(status!=NIMBY_OK)return status;
        const auto result=submit(c,3,signal);if(result!=NIMBY_OK)return result;
        out->reserved=0;out->active=c.shared->active;out->index=c.shared->result_index;
        out->active_count=c.shared->active_count;out->expires_at_ms=c.shared->result_expiry;
        return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
