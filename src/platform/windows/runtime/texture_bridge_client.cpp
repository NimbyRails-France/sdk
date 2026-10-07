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
#include <runtime/texture_commands.h>
#include <platform/windows/mod_host_protocol.h>
#include <platform/windows/bridge_installation.h>

namespace {
struct Handle {HANDLE value{};~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}};
struct Connection {
    Handle process,mapping,mutex,event;
    nimby::texture_bridge::Shared* shared{};
    nimby::engine::LiveState live{};
    uint32_t pid=0;
    using Publish=uint32_t(__cdecl*)(uint64_t,uint64_t,uint64_t,uint64_t,const nimby::texture_bridge::Command*,uint32_t);
    using Clear=uint32_t(__cdecl*)(uint64_t,const uint64_t*,uint32_t);
    using Release=uint32_t(__cdecl*)(uint64_t);
    using Execute=uint32_t(__cdecl*)(nimby::texture_bridge::Mailbox*);
    using Statuses=uint32_t(__cdecl*)(uint64_t,uint64_t,const uint64_t*,uint32_t,NimbySignalTextureOverrideStatus*);
    Publish publish{};Clear clear{};Release release{};Execute execute{};Statuses statuses{};
    nimby::texture_bridge::Mailbox response{};
    ~Connection(){if(shared)UnmapViewOfFile(shared);}
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
            nimby::platform::windows::BridgeInstallation installation;
            if(!installation)return installation.status();
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
    if(pid==GetCurrentProcessId())if(const auto module=GetModuleHandleW(nimby::texture_bridge::filename)){
        c.publish=std::bit_cast<Connection::Publish>(GetProcAddress(module,"NimbyTexture_PublishV2"));
        c.clear=std::bit_cast<Connection::Clear>(GetProcAddress(module,"NimbyTexture_Clear"));
        c.release=std::bit_cast<Connection::Release>(GetProcAddress(module,"NimbyTexture_Release"));
        c.execute=std::bit_cast<Connection::Execute>(GetProcAddress(module,"NimbyTexture_Execute"));
        c.statuses=std::bit_cast<Connection::Statuses>(GetProcAddress(module,"NimbyTexture_Statuses"));
    }
    c.pid=pid;return NIMBY_OK;
}
uint32_t recover(Connection& c){
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
                uint64_t expiry=0,uint32_t index=0,uint32_t alternate=0,uint32_t halfPeriod=0,uint64_t generation=0) {
    if(c.execute){
        c.response={};auto& request=c.response;request.operation=operation;request.request_signal=signal;
        request.request_hash=hash;request.request_expiry=expiry;request.request_index=index;
        request.request_alternate_index=alternate;request.request_half_period_ms=halfPeriod;
        request.request_database=c.live.database;request.request_simulation=c.live.simulation;
        request.request_generation=generation;
        return c.execute(&request);
    }
    // Legacy external diagnostics acquire the mailbox only for publication.
    // Catalogue/membership reads and other status requests cannot hold it idle.
    const auto wait=WaitForSingleObject(c.mutex.value,1000);
    if(wait!=WAIT_OBJECT_0&&wait!=WAIT_ABANDONED)return NIMBY_RESOURCE_LIMIT;
    struct Unlock{HANDLE mutex;~Unlock(){ReleaseMutex(mutex);}} unlock{c.mutex.value};
    const auto recovery=recover(c);if(recovery!=NIMBY_OK)return recovery;
    c.shared->operation=operation;c.shared->request_signal=signal;
    c.shared->request_hash=hash;c.shared->request_expiry=expiry;c.shared->request_index=index;
    c.shared->request_alternate_index=alternate;c.shared->request_half_period_ms=halfPeriod;
    c.shared->request_database=c.live.database;c.shared->request_simulation=c.live.simulation;
    c.shared->request_generation=generation;
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
    c.response.active=c.shared->active;c.response.result_index=c.shared->result_index;
    c.response.active_count=c.shared->active_count;c.response.result_expiry=c.shared->result_expiry;
    c.response.result_generation=c.shared->result_generation;
    return c.shared->result;
}
}
namespace nimby::platform {
struct TextureConnection::Impl { std::shared_ptr<Connection> native;uint64_t generation=0; };
TextureConnection::TextureConnection():impl_(std::make_unique<Impl>()){}
TextureConnection::~TextureConnection()=default;
uint32_t TextureConnection::open(uint32_t pid){
    // One live process connection per calling SDK thread. Handles and binary
    // identity are retained; world roots are reread for EVERY operation.
    thread_local std::shared_ptr<Connection> cached;
    if(!cached||cached->pid!=pid||WaitForSingleObject(cached->process.value,0)!=WAIT_TIMEOUT){
        auto connection=std::make_shared<Connection>();
        const auto status=connect(pid,*connection);if(status!=NIMBY_OK)return status;
        cached=std::move(connection);
    }else{
        engine::LiveState current;
        if(!engine::resolve_live_state(::read,cached.get(),cached->live.module_base,true,cached->live.profile,current))return NIMBY_DATA_UNAVAILABLE;
        cached->live=current;
    }
    impl_->native=cached;
    // Capture the bridge-owned world ticket before catalog/membership decoding.
    // Keep it on this operation, never in the cached connection: a later open
    // must not relabel a previously prepared request with a newer generation.
    const auto status=::submit(*cached,4,0);if(status!=NIMBY_OK)return status;
    impl_->generation=cached->response.result_generation;
    return impl_->generation?NIMBY_OK:NIMBY_DATA_UNAVAILABLE;
}
const engine::LiveState& TextureConnection::live() const{return impl_->native->live;}
bool TextureConnection::read(void* context,uint64_t address,void* out,size_t size){
    return ::read(static_cast<TextureConnection*>(context)->impl_->native.get(),address,out,size);
}
uint32_t TextureConnection::submit(uint32_t operation,uint64_t signal,uint64_t hash,uint64_t expiry,
                                  uint32_t index,uint32_t alternate,uint32_t halfPeriod){
    return ::submit(*impl_->native,operation,signal,hash,expiry,index,alternate,halfPeriod,impl_->generation);
}
uint32_t TextureConnection::publish(uint64_t owner,std::span<const texture_bridge::Command> updates){
    auto& c=*impl_->native;return c.publish?c.publish(owner,c.live.database,c.live.simulation,impl_->generation,updates.data(),static_cast<uint32_t>(updates.size())):NIMBY_HOOKS_UNAVAILABLE;
}
uint32_t TextureConnection::clear(uint64_t owner,std::span<const uint64_t> signals){
    auto& c=*impl_->native;return c.clear?c.clear(owner,signals.data(),static_cast<uint32_t>(signals.size())):NIMBY_HOOKS_UNAVAILABLE;
}
uint32_t TextureConnection::release(uint64_t owner){auto& c=*impl_->native;return c.release?c.release(owner):NIMBY_HOOKS_UNAVAILABLE;}
uint32_t TextureConnection::statuses(std::span<const uint64_t> signals,NimbySignalTextureOverrideStatus* out){
    auto& c=*impl_->native;
    if(c.statuses)return c.statuses(c.live.database,c.live.simulation,signals.data(),static_cast<uint32_t>(signals.size()),out);
    for(size_t i=0;i<signals.size();++i){const auto status=submit(3,signals[i]);if(status!=NIMBY_OK)return status;overrideStatus(out[i]);}
    return NIMBY_OK;
}
uint64_t TextureConnection::now(){return GetTickCount64();}
uint32_t TextureConnection::currentPid(){const auto target=NimbyInternal_ModHostTarget();return target?target:GetCurrentProcessId();}
uint32_t TextureConnection::hostTarget(){return NimbyInternal_ModHostTarget();}
uint32_t TextureConnection::forward(uint32_t operation,uint32_t pid,const void* data,size_t bytes,uint64_t count,
                                    std::span<uint8_t> output,uint32_t& written){
    written=0;const auto target=hostTarget();if(!target||(pid&&pid!=target))return NIMBY_INVALID_ARGUMENT;
    nimby::mod_host::Request request;request.operation=operation;request.args[0]=count;
    nimby::mod_host::append(request,static_cast<const uint8_t*>(data),bytes);
    nimby::mod_host::Reply reply;const auto status=nimby::mod_host::invoke(request,reply,static_cast<uint32_t>(output.size()));
    if(reply.data.size()>output.size())return NIMBY_INVALID_BINARY;
    if(!reply.data.empty())std::memcpy(output.data(),reply.data.data(),reply.data.size());
    written=static_cast<uint32_t>(reply.data.size());return status;
}
uint32_t TextureConnection::releaseInGameOwner(uint64_t owner){
    if(!owner)return NIMBY_INVALID_ARGUMENT;
    const auto module=GetModuleHandleW(nimby::texture_bridge::filename);if(!module)return NIMBY_OK;
    const auto release=std::bit_cast<Connection::Release>(GetProcAddress(module,"NimbyTexture_Release"));
    return release?release(owner):NIMBY_HOOKS_UNAVAILABLE;
}
void TextureConnection::previewStatus(NimbyTexturePreviewStatus& out) const {
    const auto& c=*impl_->native;
        out.render_thread=c.shared->render_thread;
        out.callbacks=InterlockedCompareExchange64(&c.shared->callbacks,0,0);
        out.valid_signals=InterlockedCompareExchange64(&c.shared->valid_signals,0,0);
        out.applied=InterlockedCompareExchange64(&c.shared->applied,0,0);
        out.last_signal=InterlockedCompareExchange64(&c.shared->last_signal,0,0);
        out.last_rules=InterlockedCompareExchange64(&c.shared->last_rules,0,0);
        out.signal=c.shared->signal;out.expires_at_ms=c.shared->expires;
}
void TextureConnection::overrideStatus(NimbySignalTextureOverrideStatus& out) const {
    const auto& value=impl_->native->response;
    out.reserved=0;out.active=value.active;out.index=value.result_index;
    out.active_count=value.active_count;out.expires_at_ms=value.result_expiry;
}
}
