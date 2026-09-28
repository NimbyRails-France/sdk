#include "platform/observation_process.h"
#include "engine/binary_identity.h"
#include "platform/windows/unique_handle.h"
#include "platform/windows/runtime/clock_bridge.h"
#include "platform/windows/runtime/construction_bridge.h"
#include "engine/calendar_update.h"
#include <tlhelp32.h>
#include <bit>
#include <array>
#include <cstring>

namespace nimby::platform {
struct ObservationProcess::Impl { windows::UniqueHandle process; };
ObservationProcess::ObservationProcess() : impl_(std::make_unique<Impl>()) {}
ObservationProcess::~ObservationProcess() = default;
engine::LiveStateProfile ObservationProcess::profile() noexcept {
    return engine::LiveStateProfile::Windows119;
}
bool ObservationProcess::alive() const noexcept { return impl_->process && WaitForSingleObject(impl_->process.get(),0)==WAIT_TIMEOUT; }
bool ObservationProcess::read(uint64_t address,void* output,size_t size) const noexcept {
    SIZE_T got{};
    return impl_->process && output && size<=0x7fffffffffffULL && address>=0x10000 &&
        address<=0x7fffffffffffULL-size &&
        ReadProcessMemory(impl_->process.get(),reinterpret_cast<const void*>(address),output,size,&got) && got==size;
}
uint32_t ObservationProcess::open(uint32_t requestedPid) {
    // A connection is created once per session; the registry owns cleanup even
    // if an intermediate OS call fails. Metadata is never published on failure.
        pid=requestedPid?requestedPid:GetCurrentProcessId();
        impl_->process.reset(OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|SYNCHRONIZE,FALSE,pid));
        if(!impl_->process)return NIMBY_IO_ERROR;
        std::array<wchar_t,32768> path{};DWORD length=static_cast<DWORD>(path.size());
        if(!QueryFullProcessImageNameW(impl_->process.get(),0,path.data(),&length))return NIMBY_IO_ERROR;
        game_directory=std::filesystem::path(path.data()).parent_path();
        auto result=nimby::engine::identify(path.data(),binary);
        if(result!=NIMBY_OK)return result;
        if(!binary.recognized_research_build)return NIMBY_UNSUPPORTED_GAME;
        windows::UniqueHandle modules(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid));
        if(!modules)return NIMBY_IO_ERROR;
        MODULEENTRY32W module{};module.dwSize=sizeof module;
        const bool found=Module32FirstW(modules.get(),&module)!=FALSE;
        if(!found)return NIMBY_IO_ERROR;
        base=reinterpret_cast<uint64_t>(module.modBaseAddr);
        if(WaitForSingleObject(impl_->process.get(),0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
    return NIMBY_OK;
}
namespace {
bool readProcess(void* context,uint64_t address,void* output,size_t size) {
    return static_cast<ObservationProcess*>(context)->read(address,output,size);
}
}

uint32_t ObservationProcess::setClock(int64_t utc_seconds,NimbySimulationClock& output) noexcept {
    auto& session=*this; auto* out=&output;
    if(session.pid==GetCurrentProcessId()) {
        // Never suspend our own process. The bridge applies the same calendar
        // translation at the simulation boundary before the UI copy is made.
        engine::LiveState state{};
        if(!engine::resolve_live_state(readProcess,&session,session.base,true,profile(),state))return NIMBY_DATA_UNAVAILABLE;
        uint32_t interventions{};
        return clock_bridge::change(session.impl_->process.get(),session.pid,state.simulation,session.binary,
            utc_seconds,output,interventions,false);
    }
    if(!session.alive())return NIMBY_PROCESS_EXITED;
    using ProcessControl=LONG (NTAPI*)(HANDLE);
    const auto ntdll=GetModuleHandleW(L"ntdll.dll");
    if(!ntdll)return NIMBY_CLOCK_WRITE_FAILED;
    const auto suspend=std::bit_cast<ProcessControl>(GetProcAddress(ntdll,"NtSuspendProcess"));
    const auto resume=std::bit_cast<ProcessControl>(GetProcAddress(ntdll,"NtResumeProcess"));
    if(!suspend || !resume)return NIMBY_CLOCK_WRITE_FAILED;
    const HANDLE process=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_VM_WRITE|
                                     PROCESS_VM_OPERATION|PROCESS_SUSPEND_RESUME,FALSE,session.pid);
    if(!process)return NIMBY_IO_ERROR;
    struct Transaction {
        HANDLE process;ProcessControl resume;bool suspended=false;
        ~Transaction(){if(suspended)resume(process);CloseHandle(process);}
    } transaction{process,resume};
    FILETIME originalCreation{},newCreation{},exit{},kernel{},user{};
    if(!GetProcessTimes(session.impl_->process.get(),&originalCreation,&exit,&kernel,&user) ||
       !GetProcessTimes(process,&newCreation,&exit,&kernel,&user) ||
       CompareFileTime(&originalCreation,&newCreation)!=0)return NIMBY_PROCESS_EXITED;
    // Briefly freeze the remote process: clock roots cannot be replaced mid-write.
    // No game functions, injected threads, hooks or file I/O run while frozen.
    if(suspend(process)<0)return NIMBY_CLOCK_WRITE_FAILED;
    transaction.suspended=true;
    engine::LiveState state{};
    engine::SimulationClock after{};
    if(!engine::resolve_live_state(readProcess,&session,session.base,true,profile(),state))
        return NIMBY_DATA_UNAVAILABLE;
    std::vector<engine::CalendarWrite> edits;
    try {
        const auto status=engine::prepare_calendar_update(readProcess,&session,state,utc_seconds,edits,after);
        if(status!=NIMBY_OK)return status;
    }catch(...){return NIMBY_CLOCK_WRITE_FAILED;}
    auto writeValue=[&](uint64_t address,int64_t value) {
        SIZE_T written{};int64_t verify{};
        return WriteProcessMemory(process,reinterpret_cast<void*>(address),&value,sizeof value,&written) &&
            written==sizeof value && readProcess(&session,address,&verify,sizeof verify) && verify==value;
    };
    const auto status=engine::apply_calendar_update(edits,writeValue);
    if(status!=NIMBY_OK)return status;
    if(resume(process)<0)return NIMBY_CLOCK_WRITE_FAILED; // Destructor retries on failure.
    transaction.suspended=false;
    out->epoch_seconds=after.epoch_seconds;out->ticks=after.ticks;
    return NIMBY_OK;
}
uint32_t ObservationProcess::setClockAndRecalculate(int64_t utc,NimbySimulationClock& output,uint32_t& countValue) noexcept {
    auto& session=*this; auto* out=&output; auto* count=&countValue;
    if(!session.alive())return NIMBY_PROCESS_EXITED;
    nimby::engine::LiveState state{};
    if(!nimby::engine::resolve_live_state(readProcess,&session,session.base,true,profile(),state))return NIMBY_DATA_UNAVAILABLE;
    return nimby::clock_bridge::change(session.impl_->process.get(),session.pid,state.simulation,session.binary,utc,*out,*count);
}
uint32_t ObservationProcess::construction(const NimbyConstructionRequest* request,uint64_t token,NimbyConstructionResult& result) noexcept {
    // A Kotlin tool runs on its mod observation worker inside the game. The
    // transport bootstraps locally there; only external tools need injection.
    if(!alive())return NIMBY_PROCESS_EXITED;
    return construction_bridge::exchange(impl_->process.get(),pid,binary,request,token,result);
}
}
