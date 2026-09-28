#include <nimby/detail/diagnostics.hpp>
#include "platform/windows/runtime/clock_bridge.h"
#include "engine/binary_identity.h"
#include "platform/windows/loader/loader.h"
#include <tlhelp32.h>
#include <array>
#include <bit>
#include <filesystem>

namespace nimby::clock_bridge {
namespace {
struct Handle {HANDLE value{};~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}};
constexpr auto filename=L"NimbyRailsFranceClockBridge-0.7.1.dll";
std::filesystem::path bridge_path(DWORD pid) {
    HMODULE module{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        std::bit_cast<LPCWSTR>(&change),&module))return {};
    std::array<wchar_t,32768> path{};
    if(!GetModuleFileNameW(module,path.data(),static_cast<DWORD>(path.size())))return {};
    const auto installed=std::filesystem::path(path.data()).parent_path()/filename;
    NimbyBinaryInfo expected{};
    if(engine::identify(installed.c_str(),expected)!=NIMBY_OK)return {};
    Handle modules{CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid)};
    if(modules.value==INVALID_HANDLE_VALUE)return {};
    MODULEENTRY32W entry{};entry.dwSize=sizeof entry;
    if(!Module32FirstW(modules.value,&entry))return {};
    do {
        if(_wcsicmp(entry.szModule,filename)!=0)continue;
        NimbyBinaryInfo loaded{};
        if(engine::identify(entry.szExePath,loaded)!=NIMBY_OK ||
           std::string(expected.sha256)!=loaded.sha256)return {};
        return entry.szExePath;
    }while(Module32NextW(modules.value,&entry));
    return installed;
}
}
uint32_t change(HANDLE process,DWORD pid,uint64_t simulation,const NimbyBinaryInfo& binary,
                int64_t utc,NimbySimulationClock& clock,uint32_t& count,bool recalculate) noexcept {
    try {
        const auto objectName=name(pid);
        Handle mutex{CreateMutexW(nullptr,FALSE,(objectName+L".Client").c_str())};
        if(!mutex.value)return NIMBY_IO_ERROR;
        const auto lock=WaitForSingleObject(mutex.value,1000);
        if(lock!=WAIT_OBJECT_0 && lock!=WAIT_ABANDONED)return NIMBY_RESOURCE_LIMIT;
        struct Unlock {HANDLE value;~Unlock(){ReleaseMutex(value);}} unlock{mutex.value};
        const auto bridge=bridge_path(pid);if(bridge.empty())return NIMBY_DATA_UNAVAILABLE;
        std::array<wchar_t,32768> executable{};DWORD length=static_cast<DWORD>(executable.size());
        if(!QueryFullProcessImageNameW(process,0,executable.data(),&length))return NIMBY_PROCESS_EXITED;
        if(pid==GetCurrentProcessId()) {
            const auto module=LoadLibraryExW(bridge.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
            if(!module)return NIMBY_IO_ERROR;
            struct Release {HMODULE module;~Release(){FreeLibrary(module);}} release{module};
            using Bootstrap=DWORD(WINAPI*)(void*);
            const auto bootstrap=std::bit_cast<Bootstrap>(GetProcAddress(module,"NimbyInternal_Bootstrap"));
            if(!bootstrap)return NIMBY_INVALID_BINARY;
            const auto status=bootstrap(nullptr);
            if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED)return status;
        }else{
            loader::Monitor monitor(executable.data(),binary.sha256,bridge.wstring());
            if(!monitor.attach_process(pid).success)return NIMBY_IO_ERROR;
        }
        Handle mapping{OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,objectName.c_str())};
        if(!mapping.value)return NIMBY_DATA_UNAVAILABLE;
        auto* data=static_cast<Shared*>(MapViewOfFile(mapping.value,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
        if(!data)return NIMBY_IO_ERROR;
        struct Unmap {void* value;~Unmap(){UnmapViewOfFile(value);}} unmap{data};
        if(data->protocol!=version || data->size!=sizeof(Shared))return NIMBY_INVALID_ARGUMENT;
        const auto state=InterlockedCompareExchange(&data->state,idle,idle);
        if(state!=idle && state!=complete)return NIMBY_RESOURCE_LIMIT;
        data->expected_sim=simulation;data->requested_utc=utc;data->count=0;
        data->recalculate=recalculate?1u:0u;
        InterlockedExchange(&data->state,pending);
        const auto deadline=GetTickCount64()+15000;
        while(InterlockedCompareExchange(&data->state,complete,complete)!=complete) {
            if(WaitForSingleObject(process,10)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
            if(GetTickCount64()>=deadline) {
                // A request not yet started is cancelled; never leave a delayed mutation queued.
                if(InterlockedCompareExchange(&data->state,idle,pending)==pending)return NIMBY_DATA_UNAVAILABLE;
                return NIMBY_CLOCK_WRITE_FAILED; // Already executing: outcome must be re-read.
            }
        }
        if(data->result!=NIMBY_OK)return data->result;
        clock={sizeof clock,0,data->epoch,data->ticks};count=data->count;
        return NIMBY_OK;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_IO_ERROR;}
}
}
