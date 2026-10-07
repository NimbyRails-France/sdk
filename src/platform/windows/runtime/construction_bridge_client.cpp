#include "platform/windows/runtime/construction_bridge.h"
#include "platform/windows/loader/loader.h"
#include "platform/windows/runtime/mod_host_client.h"
#include "engine/binary_identity.h"
#include "engine/construction.h"
#include <nimby/detail/observation.h>
#include <tlhelp32.h>
#include <array>
#include <filesystem>
#include <cstring>
#include <bit>
#include <nimby/detail/diagnostics.hpp>

namespace nimby::construction_bridge {
namespace {
struct Handle {HANDLE value{};~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}};
std::filesystem::path bridgePath(DWORD pid){
    HMODULE sdk{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(&exchange),&sdk))return {};
    std::array<wchar_t,32768> path{};
    const auto n=GetModuleFileNameW(sdk,path.data(),static_cast<DWORD>(path.size()));
    if(!n||n>=path.size())return {};
    constexpr auto filename=L"NimbyConstructionBridge-experimental-v1.dll";
    const auto installed=std::filesystem::path(path.data()).parent_path()/filename;
    NimbyBinaryInfo expected{};
    if(engine::identify(installed.c_str(),expected)!=NIMBY_OK)return {};
    Handle modules{CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid)};
    if(modules.value==INVALID_HANDLE_VALUE)return {};
    MODULEENTRY32W entry{};entry.dwSize=sizeof entry;
    if(!Module32FirstW(modules.value,&entry))return {};
    do {
        if(_wcsicmp(entry.szModule,filename))continue;
        NimbyBinaryInfo loaded{};
        if(engine::identify(entry.szExePath,loaded)!=NIMBY_OK||std::strcmp(expected.sha256,loaded.sha256))return {};
        return entry.szExePath;
    }while(Module32NextW(modules.value,&entry));
    return installed;
}
}
uint32_t exchange(HANDLE process,DWORD pid,const NimbyBinaryInfo& binary,const NimbyConstructionRequest* request,
                  uint64_t pollToken,NimbyConstructionResult& result) noexcept {try {
    const auto hosted=mod_host::client::target();
    if(hosted&&hosted!=pid)return NIMBY_INVALID_ARGUMENT;
    if(request&&!engine::construction::valid(*request))return NIMBY_INVALID_ARGUMENT;
    if(!request&&!pollToken)return NIMBY_INVALID_ARGUMENT;
    if(WaitForSingleObject(process,0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
    const auto objectName=name(pid);
    Handle mutex{CreateMutexW(nullptr,FALSE,(objectName+L".Client").c_str())};
    if(!mutex.value)return NIMBY_IO_ERROR;
    const auto lock=WaitForSingleObject(mutex.value,hosted?0:1000);
    if(lock!=WAIT_OBJECT_0&&lock!=WAIT_ABANDONED)return NIMBY_RESOURCE_LIMIT;
    struct Unlock {HANDLE value;~Unlock(){ReleaseMutex(value);}} unlock{mutex.value};
    if(request){
        if(hosted){
            const auto status=mod_host::client::ensureBridge(pid,5);
            if(status!=NIMBY_OK)return status;
        }else {
        const auto bridge=bridgePath(pid);
        if(bridge.empty()){
            detail::diagnostics::write("sdk","ERROR","Construction bridge missing or different from the resident DLL; install a matching development SDK and restart the game.");
            return NIMBY_DATA_UNAVAILABLE;
        }
        if(pid==GetCurrentProcessId()){
            // No remote thread into ourselves. Bootstrap is outside DllMain,
            // on the caller's worker; the endpoint queues work on native threads.
            const auto module=LoadLibraryExW(bridge.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
            if(!module)return NIMBY_IO_ERROR;
            struct Release {HMODULE module;~Release(){FreeLibrary(module);}} release{module};
            using Bootstrap=DWORD(WINAPI*)(void*);
            const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
            if(!bootstrap)return NIMBY_INVALID_BINARY;
            const auto status=bootstrap(nullptr);
            if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED){
                detail::diagnostics::write("sdk","ERROR",("Construction local bootstrap refused: status="+std::to_string(status)).c_str());
                return status;
            }
        }else{
            std::array<wchar_t,32768> executable{};DWORD length=static_cast<DWORD>(executable.size());
            if(!QueryFullProcessImageNameW(process,0,executable.data(),&length))return NIMBY_PROCESS_EXITED;
            loader::Monitor monitor(executable.data(),binary.sha256,bridge.wstring());
            if(!monitor.attach_process(pid).success)return NIMBY_HOOKS_UNAVAILABLE;
        }
        }
    }
    Handle mapping{OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,objectName.c_str())};
    if(!mapping.value)return NIMBY_DATA_UNAVAILABLE;
    auto data=static_cast<Shared*>(MapViewOfFile(mapping.value,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!data)return NIMBY_IO_ERROR;
    struct Unmap {void* value;~Unmap(){UnmapViewOfFile(value);}} unmap{data};
    if(data->version!=protocol||data->size!=sizeof(Shared))return NIMBY_INVALID_BINARY;
    auto state=InterlockedCompareExchange(&data->state,idle,idle);
    const auto requester=platform::windows::bridgeRequestLease(hosted?1000:15000);
    if(!request&&!platform::windows::sameBridgeRequester(data->lease,requester))
        return NIMBY_INVALID_HANDLE;
    if(request){
        if(state!=idle&&state!=complete)return NIMBY_RESOURCE_LIMIT;
        // Admission precedes every shared write. Otherwise even a rejected
        // foreign PREPARE would erase a completed response not yet polled.
        if(responseReserved(*data,requester,GetTickCount64(),platform::windows::bridgeRequesterAlive))return NIMBY_RESOURCE_LIMIT;
        data->request=*request;
        data->result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_PENDING,0,request->token,0,0,{}};
        data->lease=requester;
        data->expires=data->lease.expires;
        InterlockedExchange(&data->reserved,0);
        InterlockedExchange(&data->state,pending);
        while((state=InterlockedCompareExchange(&data->state,complete,complete))!=complete){
            if(WaitForSingleObject(process,10)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
            if(GetTickCount64()>=data->expires){
                if(InterlockedCompareExchange(&data->state,idle,pending)==pending)return NIMBY_DATA_UNAVAILABLE;
                result={sizeof(result),1,NIMBY_CONSTRUCTION_PENDING,0,request->token,0,0,{}};
                return NIMBY_OK;
            }
        }
    }else if(state!=complete){
        if(data->request.token!=pollToken)return NIMBY_INVALID_HANDLE;
        result={sizeof(result),1,NIMBY_CONSTRUCTION_PENDING,0,pollToken,0,0,{}};return NIMBY_OK;
    }
    if(!platform::windows::sameBridgeRequester(data->lease,requester))return NIMBY_INVALID_HANDLE;
    if(!request&&data->result.token!=pollToken)return NIMBY_INVALID_HANDLE;
    if(request&&request->action!=NIMBY_CONSTRUCTION_PREPARE&&data->result.token!=request->token)return NIMBY_INVALID_HANDLE;
    if(data->result.size!=sizeof(result)||data->result.version!=1||data->result.count>NIMBY_CONSTRUCTION_CAPACITY||
       data->result.state<NIMBY_CONSTRUCTION_READY||data->result.state>NIMBY_CONSTRUCTION_PENDING)return NIMBY_INVALID_BINARY;
    result=data->result;
    // An uncertain/PENDING answer is never an acknowledgement of completion.
    if(result.state!=NIMBY_CONSTRUCTION_PENDING)InterlockedExchange(&data->reserved,1);
    if(request)detail::diagnostics::write("sdk","INFO",("Construction action="+std::to_string(request->action)+
        " token="+std::to_string(result.token)+" state="+std::to_string(result.state)+
        " count="+std::to_string(result.count)+" reason="+std::to_string(result.reason)).c_str());
    return NIMBY_OK;
}catch(...){detail::diagnostics::exception("sdk",__func__);return NIMBY_INTERNAL_ERROR;}}
}
