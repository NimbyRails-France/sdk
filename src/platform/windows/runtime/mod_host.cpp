#include <platform/windows/mod_host.h>
#include <platform/windows/mod_host_epoch.h>
#include <platform/windows/mod_host_watchdog.h>
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/native_library.hpp>
#include <nimby/detail/signal_settings_runtime.hpp>
#include <engine/binary_identity.h>
#include <loader/manifest.h>
#include <tlhelp32.h>
#include <algorithm>
#include <atomic>
#include <bit>
#include <fstream>
#include <mutex>
#include <thread>
#include <cstdio>

namespace nimby::mod_host {
namespace {
struct Handle {
    HANDLE value{};
    explicit Handle(HANDLE v=nullptr):value(v){}
    ~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}
    Handle(const Handle&)=delete; Handle& operator=(const Handle&)=delete;
};
void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
std::filesystem::path sdkDirectory(){return detail::native::modulePath(reinterpret_cast<void*>(&NimbyInternal_ModHostTarget)).parent_path();}
void log(std::string_view message,const char* level="INFO") noexcept {
    try {const std::string text(message);detail::diagnostics::write("loader",level,text.c_str());}catch(...){}
}
// One endpoint per process. A timeout poisons this endpoint permanently; a late
// reply can never satisfy a later command with different buffers.
struct Client {
    Shared* shared{}; HANDLE request{},reply{},stop{},game{},action{},localAction{};
    std::mutex mutex; bool poisoned=false;
    uint32_t target=0;
};
Client& client(){static Client value;return value;}
std::mutex managerMutex;
std::vector<std::unique_ptr<Worker>>& workers(){static auto* value=new std::vector<std::unique_ptr<Worker>>;return *value;}
std::atomic<uint64_t> nextOwner{1};
double threadCpuMilliseconds() noexcept {
    FILETIME created{},exited{},kernel{},user{};
    if(!GetThreadTimes(GetCurrentThread(),&created,&exited,&kernel,&user))return 0;
    const auto ticks=[](FILETIME value){return (uint64_t(value.dwHighDateTime)<<32)|value.dwLowDateTime;};
    return double(ticks(kernel)+ticks(user))/10000.;
}
}

bool planResources(size_t count,uint32_t processors,uint64_t totalPhysical,
                   uint64_t availableMemory,LaunchOptions& options) noexcept {
    if(!count||count>32||!processors)return false;
    // Reserve the mapped mailbox and simultaneous private request/reply copies
    // as well as the child's commit limit. These are admission reservations,
    // not a promise that every other SDK/game allocation fits this envelope.
    const uint64_t overhead=sizeof(Shared)+2ull*payloadLimit;
    const uint64_t budget=std::min(totalPhysical/4,availableMemory/2);
    if(budget/count<overhead+256ull*1024*1024)return false;
    options.memoryLimit=static_cast<size_t>(std::min<uint64_t>(1024ull*1024*1024,budget/count-overhead));
    options.cpuRate=std::min<uint32_t>(10000/std::max<uint32_t>(2,processors),2500/static_cast<uint32_t>(count));
    if(!options.cpuRate)return false;
    options.rpcCallsPerSecond=std::min<uint32_t>(1500,6000/static_cast<uint32_t>(count));
    options.rpcCallBurst=std::min<uint32_t>(256,1024/static_cast<uint32_t>(count));
    options.rpcBytesPerSecond=std::min<uint64_t>(32ull*1024*1024,128ull*1024*1024/count);
    options.brokerCpuFraction=std::min(0.1,double(processors)*0.05/double(count));
    return true;
}

struct Worker::Impl {
    Handle mapping,request,reply,stop,action,job,process;
    Shared* shared{};
    LaunchOptions options;
    Owners owners;
    std::thread supervisor;
    std::atomic<bool> done{false};
    mutable std::mutex stateMutex;
    std::string result="starting",name;
    DWORD childPid{};
    ULONGLONG budgetAt=GetTickCount64();
    double calls=0,bytes=0,cpuMilliseconds=50;
    ULONGLONG lastDispatchedAt=0,lastRejectionLogAt=0;
    uint32_t lastDispatchedOperation=0;
    double lastDispatchedCpuMilliseconds=0;
    uint64_t rejectedRequests=0;
    bool rejectionLogged=false;
    ~Impl(){if(shared)UnmapViewOfFile(shared);}
    void record(std::string_view text) noexcept {
        try {std::lock_guard lock(stateMutex);result=text;log("Mod host "+name+": "+result);}catch(...){}
    }
    void serve(){
        if(InterlockedCompareExchange(&shared->requestState,1,1)!=1){Sleep(10);return;}
        Request requestCopy; Reply response;
        requestCopy.operation=shared->operation;requestCopy.args=shared->args;
        const auto size=shared->inputSize,capacity=shared->capacity;
        uint32_t status=NIMBY_INVALID_ARGUMENT;
        const auto cpuBefore=threadCpuMilliseconds();
        const auto now=GetTickCount64(),elapsed=now-budgetAt;budgetAt=now;
        calls=std::min(double(options.rpcCallBurst),calls+double(elapsed)*options.rpcCallsPerSecond/1000.);
        // A single maximum wire transaction must remain admissible even when
        // the per-second share is smaller. Its debt belongs only to this mod.
        constexpr double byteBurst=2.0*payloadLimit;
        bytes=std::min(byteBurst,bytes+double(elapsed)*double(options.rpcBytesPerSecond)/1000.);
        cpuMilliseconds=std::min(50.0,cpuMilliseconds+double(elapsed)*options.brokerCpuFraction);
        const auto cost=static_cast<double>(size)+capacity;
        const bool limitedCalls=calls<1,limitedBytes=bytes<cost,limitedCpu=cpuMilliseconds<0;
        const bool allowed=!limitedCalls&&!limitedBytes&&!limitedCpu;
        if(!allowed){
            status=NIMBY_RESOURCE_LIMIT;
            ++rejectedRequests;
            // Report the first rejection, then at most once per 30 seconds per
            // channel. In particular, distinguish bootstrap CPU debt from a
            // busy UI lock without weakening the caller's resource envelope.
            // Fixed stack formatting and the existing non-waiting diagnostic
            // sink keep a flood from allocating or logging on every request.
            if(!rejectionLogged||now-lastRejectionLogAt>=30000){
                rejectionLogged=true;lastRejectionLogAt=now;
                char message[768]{};
                std::snprintf(message,sizeof message,
                    "Mod RPC resource limit: mod=%s child_pid=%lu operation=%u limited_cpu=%u limited_calls=%u limited_bytes=%u "
                    "cpu_credit_ms=%.3f call_credit=%.3f byte_credit=%.0f request_bytes=%u reply_capacity=%u rejected_total=%llu "
                    "last_dispatched_operation=%u last_dispatched_cpu_ms=%.3f last_dispatch_age_ms=%llu",
                    name.c_str(),static_cast<unsigned long>(childPid),requestCopy.operation,
                    unsigned(limitedCpu),unsigned(limitedCalls),unsigned(limitedBytes),cpuMilliseconds,calls,bytes,size,capacity,
                    static_cast<unsigned long long>(rejectedRequests),lastDispatchedOperation,lastDispatchedCpuMilliseconds,
                    static_cast<unsigned long long>(lastDispatchedAt?now-lastDispatchedAt:0));
                detail::diagnostics::write("loader","WARN",message);
            }
            // This sleep belongs only to the offending channel; no SDK lock
            // is held. Flooding rejections cannot spin a broker thread.
            Sleep(2);
        }else{
            calls-=1;bytes-=cost;
            if(size<=payloadLimit&&capacity<=payloadLimit)try {
                requestCopy.data.assign(shared->data,shared->data+size);
                // The dispatchers receive a private copy, never mutable shared
                // pointers. No mod callbacks are invoked by these functions.
                if(requestCopy.operation==1){
                    status=size?NIMBY_INVALID_ARGUMENT:NimbyInternal_EnsureSignalUiBridge();
                } else if(requestCopy.operation==2||requestCopy.operation==3)
                    status=dispatchEpoch(requestCopy,response,owners);
                else if(requestCopy.operation>=4&&requestCopy.operation<=6)
                    status=dispatchTools(requestCopy,response,owners);
                else if(requestCopy.operation>=100&&requestCopy.operation<200)
                    status=dispatchUi(requestCopy,response,owners);
                else if(requestCopy.operation>=200&&requestCopy.operation<300)
                    status=dispatchDriving(requestCopy,response,owners);
                else if(requestCopy.operation>=300&&requestCopy.operation<400)
                    status=dispatchTextures(requestCopy,response,owners);
            } catch(const std::bad_alloc&) {status=NIMBY_RESOURCE_LIMIT;}
              catch(...) {status=NIMBY_INTERNAL_ERROR;}
        }
        if(response.data.size()>capacity){response.data.clear();status=NIMBY_RESOURCE_LIMIT;}
        shared->args=response.args;
        shared->outputSize=static_cast<uint32_t>(response.data.size());
        if(!response.data.empty())std::memcpy(shared->data,response.data.data(),response.data.size());
        shared->result=status;
        std::vector<uint8_t>().swap(requestCopy.data);std::vector<uint8_t>().swap(response.data);
        // Expensive SDK dispatch is charged to its originating channel. A
        // sibling's token bucket never observes this debt or waits on its lock.
        const auto spentCpuMilliseconds=std::max(0.0,threadCpuMilliseconds()-cpuBefore);
        cpuMilliseconds-=spentCpuMilliseconds;
        if(allowed){lastDispatchedOperation=requestCopy.operation;lastDispatchedCpuMilliseconds=spentCpuMilliseconds;lastDispatchedAt=GetTickCount64();}
        InterlockedExchange(&shared->requestState,2);SetEvent(reply.value);
    }
    void run() noexcept {
        try {
            HANDLE waits[]{process.value,request.value,stop.value};
            ULONGLONG stoppingAt=0;
            for(;;){
                // Request events may be kept signaled by a faulty child. Stop
                // must acquire its deadline even when that lower wait index wins.
                if(!stoppingAt&&WaitForSingleObject(stop.value,0)==WAIT_OBJECT_0)stoppingAt=watchdogNow();
                const auto wait=WaitForMultipleObjects(stoppingAt?2:3,waits,FALSE,20);
                if(wait==WAIT_OBJECT_0){DWORD code{};GetExitCodeProcess(process.value,&code);
                    char message[64]{};std::snprintf(message,sizeof message,"terminated, exit=%lu",static_cast<unsigned long>(code));
                    record(code==0?"stopped":message);break;}
                if(wait==WAIT_FAILED){record("supervision failed");TerminateJobObject(job.value,NIMBY_INTERNAL_ERROR);break;}
                if(wait==WAIT_OBJECT_0+2&&!stoppingAt)stoppingAt=watchdogNow();
                if(wait==WAIT_OBJECT_0+1)serve();
                const auto now=watchdogNow();
                const auto phase=InterlockedCompareExchange(&shared->phase,0,0);
                const auto since=static_cast<ULONGLONG>(InterlockedCompareExchange64(&shared->phaseSince,0,0));
                const bool startExpired=phase==1&&watchdogExpired(since,now,options.startupTimeoutMs);
                bool workExpired=false;
                for(auto& slot:shared->work){
                    const auto began=static_cast<ULONGLONG>(InterlockedCompareExchange64(&slot.since,0,0));
                    if(watchdogExpired(began,now,options.callbackTimeoutMs)){workExpired=true;break;}
                }
                if(startExpired||workExpired||watchdogExpired(stoppingAt,now,options.shutdownTimeoutMs)){
                    record(startExpired?"startup timeout; quarantined":workExpired?"callback timeout; quarantined":"stop timeout; quarantined");
                    TerminateJobObject(job.value,ERROR_TIMEOUT);break;
                }
            }
        }catch(...){record("supervisor exception; quarantined");TerminateJobObject(job.value,NIMBY_INTERNAL_ERROR);}
        // TerminateJobObject initiates termination asynchronously. Give the OS
        // a bounded opportunity to close the child's inherited mapping before
        // publishing completion; never keep the supervisor waiting forever.
        if(WaitForSingleObject(process.value,1000)!=WAIT_OBJECT_0)
            log("Mod host termination is still pending; releasing parent channel");
        // All cleanup is scoped to this process's opaque registration tokens.
        // Driving retirement is retained by its bounded broker queue until
        // native contention clears; it does not keep this channel allocated.
        cleanupUi(owners);cleanupDriving(owners);cleanupTextures(owners);
        // The manager retains quarantined Workers for their diagnostics. The
        // 16 MiB channel must not remain committed merely to retain an outcome.
        // Only this thread touches the view; requestStop uses its own event.
        if(shared){UnmapViewOfFile(shared);shared=nullptr;}
        if(mapping.value){CloseHandle(mapping.value);mapping.value=nullptr;}
        done=true;
    }
};

Worker::Worker(const std::filesystem::path& executable,const std::filesystem::path& library,LaunchOptions options)
    :impl_(std::make_unique<Impl>()) {
    auto& w=*impl_;w.options=options;w.name=library.filename().string();
    require(options.memoryLimit&&options.rpcCallsPerSecond&&options.rpcCallBurst&&options.rpcBytesPerSecond&&
        options.brokerCpuFraction>0&&options.brokerCpuFraction<=1&&options.cpuRate<=10000,"Invalid isolated mod resource envelope");
    w.calls=options.rpcCallBurst;w.bytes=2.0*payloadLimit;
    if(!w.options.targetPid)w.options.targetPid=GetCurrentProcessId();
    SECURITY_ATTRIBUTES inherit{sizeof(inherit),nullptr,TRUE};
    w.mapping.value=CreateFileMappingW(INVALID_HANDLE_VALUE,&inherit,PAGE_READWRITE,0,sizeof(Shared),nullptr);
    w.request.value=CreateEventW(&inherit,FALSE,FALSE,nullptr);
    w.reply.value=CreateEventW(&inherit,FALSE,FALSE,nullptr);
    w.stop.value=CreateEventW(&inherit,TRUE,FALSE,nullptr);
    // The child may wait for UI work, but cannot forge a parent notification.
    // Keep the writable event private; only this restricted duplicate enters
    // the inherited-handle allowlist.
    w.action.value=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    Handle childAction;
    if(w.action.value)DuplicateHandle(GetCurrentProcess(),w.action.value,GetCurrentProcess(),
        &childAction.value,SYNCHRONIZE,TRUE,0);
    w.job.value=CreateJobObjectW(nullptr,nullptr);
    require(w.mapping.value&&w.request.value&&w.reply.value&&w.stop.value&&w.action.value&&
        childAction.value&&w.job.value,"Cannot create isolated mod channel");
    w.shared=static_cast<Shared*>(MapViewOfFile(w.mapping.value,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    require(w.shared,"Cannot map isolated mod channel");
    w.shared->version=channelProtocol;w.shared->size=sizeof(Shared);w.shared->targetPid=w.options.targetPid;
    w.shared->phase=1;w.shared->phaseSince=static_cast<LONG64>(watchdogNow());
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
    limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE|JOB_OBJECT_LIMIT_PROCESS_MEMORY|JOB_OBJECT_LIMIT_ACTIVE_PROCESS|JOB_OBJECT_LIMIT_PRIORITY_CLASS;
    limits.BasicLimitInformation.ActiveProcessLimit=1;limits.ProcessMemoryLimit=options.memoryLimit;
    limits.BasicLimitInformation.PriorityClass=BELOW_NORMAL_PRIORITY_CLASS;
    require(SetInformationJobObject(w.job.value,JobObjectExtendedLimitInformation,&limits,sizeof(limits))!=0,"Cannot bound mod process resources");
    // A busy-loop mod cannot consume all cores. Normal callbacks are short and
    // use well below one logical processor; the game retains normal priority.
    SYSTEM_INFO system{};GetSystemInfo(&system);
    JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpu{};
    cpu.ControlFlags=JOB_OBJECT_CPU_RATE_CONTROL_ENABLE|JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;
    cpu.CpuRate=options.cpuRate?options.cpuRate:std::max<DWORD>(1,10000/std::max<DWORD>(2,system.dwNumberOfProcessors));
    require(SetInformationJobObject(w.job.value,JobObjectCpuRateControlInformation,&cpu,sizeof(cpu))!=0,"Cannot bound mod CPU consumption");
    STARTUPINFOEXW startup{};startup.StartupInfo.cb=sizeof(startup);startup.StartupInfo.dwFlags=STARTF_USESHOWWINDOW;startup.StartupInfo.wShowWindow=SW_HIDE;
    SIZE_T attributeSize{};InitializeProcThreadAttributeList(nullptr,1,0,&attributeSize);
    std::vector<uint8_t> attributes(attributeSize);startup.lpAttributeList=reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attributes.data());
    require(InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&attributeSize)!=0,"Cannot initialize mod handle inheritance");
    struct DeleteAttributes {LPPROC_THREAD_ATTRIBUTE_LIST p;~DeleteAttributes(){DeleteProcThreadAttributeList(p);}} deleteAttributes{startup.lpAttributeList};
    HANDLE inherited[]{w.mapping.value,w.request.value,w.reply.value,w.stop.value,childAction.value};
    require(UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,inherited,sizeof(inherited),nullptr,nullptr)!=0,"Cannot restrict inherited mod handles");
    // Executable and DLL are absolute filesystem paths, never shell commands.
    const auto exe=std::filesystem::absolute(executable),dll=std::filesystem::absolute(library);
    std::wstring command=L"\""+exe.wstring()+L"\"";
    for(auto handle:inherited)command+=L" "+std::to_wstring(reinterpret_cast<uintptr_t>(handle));
    command+=L" "+std::to_wstring(w.options.targetPid)+L" \""+dll.wstring()+L"\"";
    PROCESS_INFORMATION child{};
    require(CreateProcessW(exe.c_str(),command.data(),nullptr,nullptr,TRUE,
        CREATE_SUSPENDED|CREATE_NO_WINDOW|EXTENDED_STARTUPINFO_PRESENT|BELOW_NORMAL_PRIORITY_CLASS,
        nullptr,exe.parent_path().c_str(),&startup.StartupInfo,&child)!=0,"Cannot start isolated mod executable");
    w.process.value=child.hProcess;Handle mainThread(child.hThread);w.childPid=child.dwProcessId;
    if(!AssignProcessToJobObject(w.job.value,w.process.value)){
        TerminateProcess(w.process.value,NIMBY_INTERNAL_ERROR);throw std::runtime_error("Cannot assign isolated mod resource job");
    }
    w.owners.driving=0x4e52460000000000ull|nextOwner.fetch_add(1);
    w.owners.textures=w.owners.driving;
    w.owners.actionWake=reinterpret_cast<uintptr_t>(w.action.value);
    try {w.supervisor=std::thread([&w]{w.run();});}
    catch(...){TerminateJobObject(w.job.value,NIMBY_INTERNAL_ERROR);throw;}
    if(ResumeThread(mainThread.value)==DWORD(-1)){TerminateJobObject(w.job.value,NIMBY_INTERNAL_ERROR);w.supervisor.join();throw std::runtime_error("Cannot resume isolated mod");}
    // Logging/allocation must never unwind a constructor with a live thread.
    try {log("Mod isolated: "+w.name+" pid="+std::to_string(w.childPid));}catch(...){}
}
Worker::~Worker(){requestStop();join();}
void Worker::requestStop() noexcept {if(impl_)SetEvent(impl_->stop.value);}
void Worker::join(){if(impl_&&impl_->supervisor.joinable())impl_->supervisor.join();}
uint32_t Worker::pid()const noexcept{return impl_->childPid;}
bool Worker::finished()const noexcept{return impl_->done.load();}
std::string Worker::outcome()const{std::lock_guard lock(impl_->stateMutex);return impl_->result;}

uint32_t attach(HANDLE mapping,HANDLE request,HANDLE reply,HANDLE stop,HANDLE action,uint32_t targetPid) {
    auto& c=client();if(c.shared||!targetPid)return NIMBY_INVALID_ARGUMENT;
    for(const auto handle:{mapping,request,reply,stop,action}){
        DWORD flags{};if(!handle||handle==INVALID_HANDLE_VALUE||!GetHandleInformation(handle,&flags))return NIMBY_INVALID_ARGUMENT;
    }
    Handle game(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,targetPid));
    if(!game.value||WaitForSingleObject(game.value,0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
    std::array<wchar_t,32768> path{};DWORD length=static_cast<DWORD>(path.size());
    if(!QueryFullProcessImageNameW(game.value,0,path.data(),&length))return NIMBY_IO_ERROR;
#ifndef NIMBY_MOD_HOST_TESTING
    NimbyBinaryInfo identity{};
    if(engine::identify(path.data(),identity)!=NIMBY_OK||!identity.recognized_research_build)return NIMBY_INVALID_BINARY;
#endif
    auto* shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared)return NIMBY_IO_ERROR;
    if(shared->version!=channelProtocol||shared->size!=sizeof(Shared)||shared->targetPid!=targetPid){UnmapViewOfFile(shared);return NIMBY_INVALID_BINARY;}
    Handle localAction(CreateEventW(nullptr,FALSE,FALSE,nullptr));
    if(!localAction.value){UnmapViewOfFile(shared);return NIMBY_IO_ERROR;}
    c.shared=shared;c.request=request;c.reply=reply;c.stop=stop;c.action=action;
    c.localAction=localAction.value;localAction.value=nullptr;
    c.game=game.value;game.value=nullptr;c.target=targetPid;
    installChildEpochAuthority();
    return NIMBY_OK;
}
int runChild(int argc,wchar_t** argv){
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX|SEM_NOOPENFILEERRORBOX);
    if(argc!=8)return NIMBY_INVALID_ARGUMENT;
    try {
        HANDLE values[5]{};
        for(int i=0;i<5;++i){size_t used{};values[i]=reinterpret_cast<HANDLE>(std::stoull(argv[i+1],&used));if(argv[i+1][used])return NIMBY_INVALID_ARGUMENT;}
        size_t used{};const auto target=std::stoul(argv[6],&used);if(argv[6][used])return NIMBY_INVALID_ARGUMENT;
        auto status=attach(values[0],values[1],values[2],values[3],values[4],target);if(status!=NIMBY_OK)return status;
        status=NimbyInternal_Initialize(NIMBY_ABI_VERSION,0);if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED)return status;
        // Retain runtime code until process exit, including Kotlin GC threads.
        const auto module=detail::native::loadIsolated(std::filesystem::absolute(argv[7]));
        using Entry=uint32_t(WINAPI*)(void*);
        const auto version=std::bit_cast<Entry>(GetProcAddress(module,"NRFMod_HostProtocolV1"));
        const auto start=std::bit_cast<Entry>(GetProcAddress(module,"NRFMod_StartV1"));
        const auto stop=std::bit_cast<Entry>(GetProcAddress(module,"NRFMod_StopV1"));
        if(!version||version(nullptr)!=protocol||!start||!stop){
            log("Mod refuses isolated host protocol; rebuild the mod against the current SDK");return NIMBY_INVALID_BINARY;
        }
        status=start(nullptr);if(status!=NIMBY_OK&&status!=NIMBY_ALREADY_INITIALIZED)return status;
        // Observe worker may already be inside a callback: never replace its
        // active watchdog marker with a main-thread heartbeat.
        if(InterlockedCompareExchange(&client().shared->phase,3,1)==1)
            InterlockedExchange64(&client().shared->phaseSince,static_cast<LONG64>(watchdogNow()));
        InterlockedExchange(&client().shared->ready,1);
        HANDLE waits[]{client().stop,client().game};WaitForMultipleObjects(2,waits,FALSE,INFINITE);
        NimbyInternal_ModHostPulse(4);
        return stop(nullptr);
    }catch(...){detail::diagnostics::exception("loader","isolated mod main");return NIMBY_INTERNAL_ERROR;}
}
bool isChildOf(uint32_t workerPid,uint32_t gamePid) noexcept {
    try {
        Handle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,workerPid));
        Handle game(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,gamePid));
        std::array<wchar_t,32768> gamePath{};DWORD gameLength=static_cast<DWORD>(gamePath.size());
        if(!game.value||WaitForSingleObject(game.value,0)!=WAIT_TIMEOUT||
            !QueryFullProcessImageNameW(game.value,0,gamePath.data(),&gameLength))return false;
        std::array<wchar_t,32768> path{};DWORD length=static_cast<DWORD>(path.size());
        // External tools may load their SDK from a different installation.
        // The worker belongs beside the target game, not beside that SDK.
        if(!process.value||!QueryFullProcessImageNameW(process.value,0,path.data(),&length)||
            !std::filesystem::equivalent(path.data(),std::filesystem::path(gamePath.data()).parent_path()/L"NimbyRailsFranceModHost.exe"))return false;
        Handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0));PROCESSENTRY32W row{};row.dwSize=sizeof(row);
        if(snapshot.value==INVALID_HANDLE_VALUE||!Process32FirstW(snapshot.value,&row))return false;
        do{if(row.th32ProcessID==workerPid)return row.th32ParentProcessID==gamePid&&WaitForSingleObject(process.value,0)==WAIT_TIMEOUT;}while(Process32NextW(snapshot.value,&row));
    }catch(...){}return false;
}
}

extern "C" uint32_t __cdecl NimbyInternal_ModHostTarget() noexcept {return nimby::mod_host::client().target;}
extern "C" uint32_t __cdecl NimbyInternal_ModHostActionWaits(uint64_t* remote,uint64_t* local) noexcept {
    if(remote)*remote=0;
    if(local)*local=0;
    if(!remote||!local||remote==local)return NIMBY_INVALID_ARGUMENT;
    const auto& c=nimby::mod_host::client();
    if(!c.target||!c.action||!c.localAction)return NIMBY_HOOKS_UNAVAILABLE;
    HANDLE first{},second{};
    if(!DuplicateHandle(GetCurrentProcess(),c.action,GetCurrentProcess(),&first,SYNCHRONIZE,FALSE,0))return NIMBY_IO_ERROR;
    if(!DuplicateHandle(GetCurrentProcess(),c.localAction,GetCurrentProcess(),&second,SYNCHRONIZE,FALSE,0)){
        CloseHandle(first);return NIMBY_IO_ERROR;
    }
    *remote=reinterpret_cast<uintptr_t>(first);*local=reinterpret_cast<uintptr_t>(second);return NIMBY_OK;
}
extern "C" void __cdecl NimbyInternal_ModHostWakeLocal() noexcept {
    const auto event=nimby::mod_host::client().localAction;if(event)SetEvent(event);
}
extern "C" int __cdecl NimbyInternal_ModHostMain(int argc,wchar_t** argv) noexcept {
    try{return nimby::mod_host::runChild(argc,argv);}catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" void __cdecl NimbyInternal_ModHostPulse(uint32_t phase) noexcept {
    auto& c=nimby::mod_host::client();if(!c.shared)return;
    // A short callback cannot hide another thread's blocked callback. Nested
    // work keeps the outermost start time, rather than renewing its deadline.
    static thread_local unsigned depth=0;
    static thread_local nimby::mod_host::WorkSlot* active=nullptr;
    if(phase==2){
        if(depth++)return;
        const auto thread=static_cast<LONG>(GetCurrentThreadId());
        for(auto& slot:c.shared->work)if(InterlockedCompareExchange(&slot.thread,thread,0)==0){
            active=&slot;InterlockedExchange64(&slot.since,static_cast<LONG64>(nimby::mod_host::watchdogNow()));return;
        }
        // More simultaneous callbacks than the host can supervise is a local
        // resource failure. Do not let unsupervised code continue in this mod.
        ExitProcess(NIMBY_RESOURCE_LIMIT);
    }
    if(phase==3){
        if(!depth||--depth)return;
        if(active){InterlockedExchange64(&active->since,0);InterlockedExchange(&active->thread,0);active=nullptr;}
        return;
    }
    InterlockedExchange64(&c.shared->phaseSince,static_cast<LONG64>(nimby::mod_host::watchdogNow()));
    InterlockedExchange(&c.shared->phase,static_cast<LONG>(phase));
}
extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* arguments,
    const void* input,uint32_t size,void* output,uint32_t capacity,uint32_t* written) noexcept {
    using namespace nimby::mod_host;
    if(written)*written=0;
    if(!arguments||!written||(!input&&size)||(!output&&capacity)||size>payloadLimit||capacity>payloadLimit)return NIMBY_INVALID_ARGUMENT;
    try {
        auto& c=client();std::lock_guard lock(c.mutex);
        if(!c.shared)return NIMBY_HOOKS_UNAVAILABLE;
        if(c.poisoned)return NIMBY_PROCESS_EXITED;
        auto& shared=*c.shared;
        shared.operation=operation;shared.inputSize=size;shared.capacity=capacity;
        std::copy_n(arguments,8,shared.args.begin());if(size)std::memcpy(shared.data,input,size);
        ResetEvent(c.reply);InterlockedExchange(&shared.requestState,1);SetEvent(c.request);
        HANDLE waits[]{c.reply,c.game};
        if(WaitForMultipleObjects(2,waits,FALSE,2000)!=WAIT_OBJECT_0||InterlockedCompareExchange(&shared.requestState,2,2)!=2){
            c.poisoned=true;return NIMBY_IO_ERROR;
        }
        if(shared.outputSize>capacity){c.poisoned=true;return NIMBY_INVALID_BINARY;}
        *written=shared.outputSize;std::copy(shared.args.begin(),shared.args.end(),arguments);
        if(*written)std::memcpy(output,shared.data,*written);
        return shared.result;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" uint32_t __cdecl NimbyInternal_StartModHosts(const wchar_t* directory) noexcept {
    using namespace nimby::mod_host;
    try {
        if(!directory||!*directory||NimbyInternal_ModHostTarget())return NIMBY_INVALID_ARGUMENT;
        std::lock_guard lock(managerMutex);if(!workers().empty())return NIMBY_ALREADY_INITIALIZED;
        std::vector<std::filesystem::path> paths;std::error_code error;
        for(std::filesystem::directory_iterator it(directory,error),end;!error&&it!=end;it.increment(error)){
            if(!it->is_directory(error)||it->path().filename().wstring().starts_with(L'.'))continue;
            std::ifstream manifest(it->path()/"nrf-mod.ini",std::ios::binary);if(!manifest)continue;
            nimby::loader::ManifestError manifestError{};
            const auto name=nimby::loader::manifest_library(manifest,".dll",true,&manifestError);
            if(name.empty()){log("Mod "+it->path().filename().string()+": "+nimby::loader::manifest_error_message(manifestError),"ERROR");continue;}
            paths.push_back(it->path()/name);
            if(paths.size()>32){log("Isolated-mod admission limit exceeded (more than 32 mods): no mods started", "ERROR");return NIMBY_RESOURCE_LIMIT;}
        }
        if(error)return NIMBY_IO_ERROR;
        if(paths.empty())return NIMBY_OK;
        SYSTEM_INFO system{};GetSystemInfo(&system);
        MEMORYSTATUSEX memory{};memory.dwLength=sizeof(memory);
        LaunchOptions options;
        if(!GlobalMemoryStatusEx(&memory)||!planResources(paths.size(),system.dwNumberOfProcessors,
            memory.ullTotalPhys,std::min(memory.ullAvailPhys,memory.ullAvailPageFile),options)){
            log("Insufficient isolated-mod resources: no mods started (requested="+std::to_string(paths.size())+
                ", available memory MiB="+std::to_string(memory.ullAvailPhys/(1024*1024))+
                "); close other applications or enable fewer mods", "ERROR");
            return NIMBY_RESOURCE_LIMIT;
        }
        log("Mod batch resource reservations: count="+std::to_string(paths.size())+
            ", CPU basis points per mod="+std::to_string(options.cpuRate)+
            ", private memory MiB per mod="+std::to_string(options.memoryLimit/(1024*1024))+
            ", RPC/s per mod="+std::to_string(options.rpcCallsPerSecond));
        configureEpochTarget(GetCurrentProcessId());
        std::sort(paths.begin(),paths.end());
        for(const auto& path:paths){
            try{workers().push_back(std::make_unique<Worker>(sdkDirectory()/L"NimbyRailsFranceModHost.exe",path,options));}
            catch(const std::exception& e){
                log("Cannot isolate "+path.filename().string()+": "+e.what()+"; stopping the entire mod batch", "ERROR");
                for(auto& worker:workers())worker->requestStop();
                for(auto& worker:workers())worker->join();
                workers().clear();return NIMBY_RESOURCE_LIMIT;
            }
        }
        return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
extern "C" uint32_t __cdecl NimbyInternal_StopModHosts() noexcept {
    using namespace nimby::mod_host;
    try{
        std::lock_guard lock(managerMutex);
        for(auto& worker:workers())worker->requestStop();
        for(auto& worker:workers())worker->join();
        workers().clear();return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
