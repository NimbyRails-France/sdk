#include <platform/texture_connection.h>
#include <nimby/signal_textures.hpp>
#include "engine/texture_file.h"
#include "platform/windows/runtime/texture_bridge.h"
#include "engine/binary_identity.h"
#include "engine/network.h"
#include "engine/signal_textures.h"
#include "platform/windows/loader/loader.h"
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
           !_wcsicmp(entry.szModule,L"NimbyRailsFranceTextureBridge-experimental-v2.dll")||
           !_wcsicmp(entry.szModule,L"NimbyRailsFranceTextureBridge-experimental-v3.dll"))return NIMBY_INVALID_BINARY;
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
    const auto recoveryDeadline=GetTickCount64()+2000;
    while(InterlockedCompareExchange(&c.shared->pending,0,0)){
        if(WaitForSingleObject(c.process.value,10)==WAIT_OBJECT_0)return NIMBY_PROCESS_EXITED;
        if(GetTickCount64()>=recoveryDeadline)return NIMBY_RESOURCE_LIMIT;
    }
    return NIMBY_OK;
}
uint32_t submit(Connection& c,uint32_t operation,uint64_t signal,uint64_t hash=0,
                uint64_t expiry=0,uint32_t index=0,uint32_t alternate=0,uint32_t halfPeriod=0) {
    c.shared->operation=operation;c.shared->request_signal=signal;
    c.shared->request_hash=hash;c.shared->request_expiry=expiry;c.shared->request_index=index;
    c.shared->request_alternate_index=alternate;c.shared->request_half_period_ms=halfPeriod;
    c.shared->request_database=c.live.database;c.shared->request_simulation=c.live.simulation;
    InterlockedExchange(&c.shared->pending,1);
    if(!SetEvent(c.event.value)){InterlockedExchange(&c.shared->pending,0);return NIMBY_IO_ERROR;}
    const auto requestDeadline=GetTickCount64()+2000;
    while(InterlockedCompareExchange(&c.shared->pending,0,0)){
        // A batch may contain several immediately answered status requests.
        // A 10 ms polling floor per reply needlessly holds the shared command
        // mutex and delays the mod's render updates. Still sleep, never spin.
        if(WaitForSingleObject(c.process.value,1)==WAIT_OBJECT_0)return NIMBY_PROCESS_EXITED;
        // Leave pending intact: the next caller must finish recovery before
        // reusing the shared request. A timeout never cancels an in-flight write.
        if(GetTickCount64()>=requestDeadline)return NIMBY_RESOURCE_LIMIT;
    }
    return c.shared->result;
}
}
namespace nimby::platform {
struct TextureConnection::Impl { Connection native; };
TextureConnection::TextureConnection():impl_(std::make_unique<Impl>()){}
TextureConnection::~TextureConnection()=default;
uint32_t TextureConnection::open(uint32_t pid){return connect(pid,impl_->native);}
const engine::LiveState& TextureConnection::live() const{return impl_->native.live;}
bool TextureConnection::read(void* context,uint64_t address,void* out,size_t size){
    return ::read(&static_cast<TextureConnection*>(context)->impl_->native,address,out,size);
}
uint32_t TextureConnection::submit(uint32_t operation,uint64_t signal,uint64_t hash,uint64_t expiry,
                                  uint32_t index,uint32_t alternate,uint32_t halfPeriod){
    return ::submit(impl_->native,operation,signal,hash,expiry,index,alternate,halfPeriod);
}
uint64_t TextureConnection::now(){return GetTickCount64();}
uint32_t TextureConnection::currentPid(){return GetCurrentProcessId();}
void TextureConnection::previewStatus(NimbyTexturePreviewStatus& out) const {
    const auto& c=impl_->native;
        out.render_thread=c.shared->render_thread;
        out.callbacks=InterlockedCompareExchange64(&c.shared->callbacks,0,0);
        out.valid_signals=InterlockedCompareExchange64(&c.shared->valid_signals,0,0);
        out.applied=InterlockedCompareExchange64(&c.shared->applied,0,0);
        out.last_signal=InterlockedCompareExchange64(&c.shared->last_signal,0,0);
        out.last_rules=InterlockedCompareExchange64(&c.shared->last_rules,0,0);
        out.signal=c.shared->signal;out.expires_at_ms=c.shared->expires;
}
void TextureConnection::overrideStatus(NimbySignalTextureOverrideStatus& out) const {
    const auto& c=impl_->native;
            out.reserved=0;out.active=c.shared->active;out.index=c.shared->result_index;
            out.active_count=c.shared->active_count;out.expires_at_ms=c.shared->result_expiry;
}
}
