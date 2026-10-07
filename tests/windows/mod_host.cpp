#include <platform/windows/mod_host.h>
#include <platform/windows/mod_host_epoch.h>
#include <platform/windows/mod_host_watchdog.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <stdexcept>
#include <thread>
#include <limits>

void advanceHostTestWall(uint64_t milliseconds) noexcept;

using namespace std::chrono_literals;
#define CHECK(x) do { if(!(x)) throw std::runtime_error("line " + std::to_string(__LINE__) + ": " #x); } while(false)
namespace fixture {
struct Stats {
    uint64_t calls{},owner{},throttled{},cpuLimit{},memoryLimit{},priority{},actionWake{};
    uint64_t actionStage{},remoteWakes{},localWakes{},actionChecks{};
    std::vector<double> latency;
};
std::mutex mutex;
std::condition_variable changed;
std::map<uint64_t,Stats> stats;
std::set<uint64_t> active;
unsigned cleaned=0;
std::set<uint64_t> releasedActionFixtures;
Stats get(uint64_t mode) {std::lock_guard lock(mutex);return stats[mode];}
bool wait(uint64_t mode,uint64_t count,std::chrono::milliseconds timeout=3s) {
    std::unique_lock lock(mutex);return changed.wait_for(lock,timeout,[&]{return stats[mode].calls>=count;});
}
bool waitAction(uint64_t mode,uint64_t stage,uint64_t remote=0) {
    std::unique_lock lock(mutex);return changed.wait_for(lock,3s,[&]{
        return stats[mode].actionStage>=stage&&stats[mode].remoteWakes>=remote;});
}
void releaseActionFixtures() {
    std::lock_guard lock(mutex);releasedActionFixtures.insert(14);releasedActionFixtures.insert(15);changed.notify_all();
}
void report(const char* scenario,size_t from) {
    auto value=get(0);std::vector<double> times(value.latency.begin()+from,value.latency.end());CHECK(!times.empty());
    std::sort(times.begin(),times.end());const auto p=[&](size_t n){return times[std::min(times.size()-1,(times.size()*n+99)/100-1)];};
    std::cout<<"{\"scenario\":\""<<scenario<<"\",\"rpc_samples\":"<<times.size()<<",\"p50_us\":"<<p(50)
        <<",\"p95_us\":"<<p(95)<<",\"p99_us\":"<<p(99)<<",\"max_us\":"<<times.back()<<"}\n";
}
}
// No game hooks or game binary dependencies in this executable. The production
// host uses explicit parent-identity and test-only uptime seams.
extern "C" uint32_t __cdecl NimbyInternal_Initialize(uint32_t,uint32_t) noexcept {return NIMBY_OK;}
extern "C" uint32_t __cdecl NimbyInternal_EnsureSignalUiBridge() noexcept {return NIMBY_HOOKS_UNAVAILABLE;}
namespace nimby::mod_host {
uint32_t dispatchUi(const Request& request,Reply& reply,Owners& owners) {
    if(request.operation!=100||request.data.size()!=24||request.args[0]>16)return NIMBY_INVALID_ARGUMENT;
    if(request.args[0]>=9&&request.args[0]<=11){
        // Bounded expensive broker work, with no shared fixture lock held.
        // Charge it to the caller's CPU bucket rather than slowing all channels.
        const auto until=std::chrono::steady_clock::now()+2ms;
        while(std::chrono::steady_clock::now()<until)std::atomic_signal_fence(std::memory_order_seq_cst);
    }
    std::unique_lock lock(fixture::mutex);auto& stats=fixture::stats[request.args[0]];
    if(stats.owner&&stats.owner!=owners.driving)return NIMBY_INVALID_HANDLE;
    stats.owner=owners.driving;++stats.calls;stats.throttled=request.args[3];if(request.args[2])stats.latency.push_back(double(request.args[2])/1000.);
    stats.cpuLimit=request.args[4];stats.memoryLimit=request.args[5];stats.priority=request.args[7];
    stats.actionWake=owners.actionWake;
    owners.panels.insert(owners.driving);fixture::active.insert(owners.driving);
    reply.args=request.args;reply.args[6]=request.args[1]+1;reply.data=request.data;
    if((request.args[0]==14||request.args[0]==15)&&request.args[1]>=1001&&request.args[1]<=1003) {
        std::array<uint64_t,3> report{};std::memcpy(report.data(),request.data.data(),sizeof report);
        stats.actionStage=request.args[1]-1000;stats.remoteWakes=report[0];stats.localWakes=report[1];stats.actionChecks=report[2];
        fixture::changed.notify_all();
        if(stats.actionStage==1&&!fixture::changed.wait_for(lock,1500ms,[&]{return fixture::releasedActionFixtures.contains(request.args[0]);}))
            return NIMBY_DATA_UNAVAILABLE;
    }
    fixture::changed.notify_all();return NIMBY_OK;
}
void cleanupUi(Owners& owners) noexcept {
    std::lock_guard lock(fixture::mutex);for(auto token:owners.panels)fixture::active.erase(token);
    owners.panels.clear();++fixture::cleaned;fixture::changed.notify_all();
}
uint32_t dispatchDriving(const Request&,Reply&,Owners&) {return NIMBY_HOOKS_UNAVAILABLE;}
uint32_t dispatchTools(const Request&,Reply&,Owners&) {return NIMBY_HOOKS_UNAVAILABLE;}
void cleanupDriving(Owners&) noexcept {}
uint32_t dispatchTextures(const Request&,Reply&,Owners&) {return NIMBY_HOOKS_UNAVAILABLE;}
void cleanupTextures(Owners&) noexcept {}
// The authority's real read/validation path has its own synthetic-memory test;
// these fault DLLs only use the echo endpoint and never open observations.
void configureEpochTarget(uint32_t) {}
void installChildEpochAuthority() {}
uint32_t dispatchEpoch(const Request&,Reply&,Owners&) {return NIMBY_HOOKS_UNAVAILABLE;}
}
namespace {
std::filesystem::path executable() {
    std::array<wchar_t,32768> path{};const auto count=GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()));
    CHECK(count&&count<path.size());return path.data();
}
struct IsolatedDiagnostics {
    std::filesystem::path directory;
    std::wstring previous;
    bool hadPrevious=false;
    IsolatedDiagnostics() {
        const auto size=GetEnvironmentVariableW(L"NRF_LOG_DIR",nullptr,0);
        if(size){previous.resize(size);const auto copied=GetEnvironmentVariableW(L"NRF_LOG_DIR",previous.data(),size);
            CHECK(copied&&copied<size);previous.resize(copied);hadPrevious=true;}
        directory=std::filesystem::temp_directory_path()/(L"nimby-host-diagnostics-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(GetTickCount64()));
        CHECK(std::filesystem::create_directory(directory));
        CHECK(SetEnvironmentVariableW(L"NRF_LOG_DIR",directory.c_str()));
    }
    ~IsolatedDiagnostics() {
        SetEnvironmentVariableW(L"NRF_LOG_DIR",hadPrevious?previous.c_str():nullptr);
        // Only these known test-owned files are removed; never recursively
        // traverse a user-supplied diagnostic directory.
        std::error_code error;
        std::filesystem::remove(directory/"loader"/"nimby_mod_host_tests_exe.log",error);
        std::filesystem::remove(directory/"loader",error);
        std::filesystem::remove(directory,error);
    }
    void verifyRejections() const {
        std::ifstream input(directory/"loader"/"nimby_mod_host_tests_exe.log");CHECK(input.good());
        std::map<uint64_t,unsigned> perChild;
        bool cpu=false,calls=false,bytes=false;
        for(std::string line;std::getline(input,line);){
            if(line.find("Mod RPC resource limit:")==std::string::npos)continue;
            const auto field=[&](const char* key){const auto at=line.find(key);CHECK(at!=std::string::npos);return line.substr(at+std::char_traits<char>::length(key));};
            const auto child=std::stoull(field("child_pid="));CHECK(child&&child!=GetCurrentProcessId());
            CHECK(++perChild[child]==1); // Every individual offender runs well below the 30-second diagnostic interval.
            CHECK(std::stoul(field("operation="))==100);
            CHECK(std::stoull(field("rejected_total="))>=1);
            CHECK(std::stoul(field("last_dispatched_operation="))==100);
            if(line.find("limited_cpu=1")!=std::string::npos){
                cpu=true;CHECK(std::stod(field("cpu_credit_ms="))<0);CHECK(std::stod(field("last_dispatched_cpu_ms="))>0);
            }
            calls|=line.find("limited_calls=1")!=std::string::npos;
            bytes|=line.find("limited_bytes=1")!=std::string::npos;
            std::cout<<line<<'\n';
        }
        CHECK(cpu&&calls&&bytes);
        std::cout<<"PASS real RPC rejection diagnostics distinguish CPU/call/byte limits, retain prior charged operation and stay bounded per channel\n";
    }
};
bool waitFinished(nimby::mod_host::Worker& worker,std::chrono::milliseconds timeout=4s) {
    const auto deadline=std::chrono::steady_clock::now()+timeout;
    while(!worker.finished()&&std::chrono::steady_clock::now()<deadline)std::this_thread::sleep_for(10ms);
    return worker.finished();
}
void checkExited(uint32_t pid) {
    const auto process=OpenProcess(SYNCHRONIZE,FALSE,pid);
    if(!process)return;
    const auto status=WaitForSingleObject(process,1000);CloseHandle(process);CHECK(status==WAIT_OBJECT_0);
}
size_t channelMappings() {
    SYSTEM_INFO system{};GetSystemInfo(&system);
    const auto bytes=(sizeof(nimby::mod_host::Shared)+system.dwPageSize-1)/system.dwPageSize*system.dwPageSize;
    size_t result=0;uintptr_t address=0;MEMORY_BASIC_INFORMATION region{};
    while(VirtualQuery(reinterpret_cast<void*>(address),&region,sizeof region)){
        if(region.Type==MEM_MAPPED&&region.BaseAddress==region.AllocationBase&&region.RegionSize==bytes)++result;
        const auto next=reinterpret_cast<uintptr_t>(region.BaseAddress)+region.RegionSize;
        if(next<=address)break;
        address=next;
    }
    return result;
}
void resourcePlans() {
    constexpr uint64_t gib=1024ull*1024*1024;
    constexpr uint64_t overhead=sizeof(nimby::mod_host::Shared)+2ull*nimby::mod_host::payloadLimit;
    for(const auto processors:{2u,8u,24u,64u})for(const auto count:{1u,3u,4u,16u,32u}){
        nimby::mod_host::LaunchOptions options;
        CHECK(nimby::mod_host::planResources(count,processors,64*gib,40*gib,options));
        CHECK(options.cpuRate*count<=2500);
        CHECK(options.cpuRate<=10000/processors);
        CHECK(options.memoryLimit>=256ull*1024*1024&&options.memoryLimit<=gib);
        CHECK((options.memoryLimit+overhead)*count<=16*gib);
        CHECK(options.rpcCallsPerSecond*count<=6000&&options.rpcCallsPerSecond<=1500);
        CHECK(options.rpcCallBurst*count<=1024);
        CHECK(options.rpcBytesPerSecond*count<=128ull*1024*1024);
        CHECK(options.brokerCpuFraction*count<=double(processors)*0.05+0.00001);
    }
    nimby::mod_host::LaunchOptions constrained;
    CHECK(nimby::mod_host::planResources(3,24,32*gib,4*gib,constrained));
    CHECK(constrained.memoryLimit<gib&&constrained.cpuRate==10000/24);
    CHECK(!nimby::mod_host::planResources(8,24,32*gib,4*gib,constrained));
    CHECK(!nimby::mod_host::planResources(0,24,32*gib,4*gib,constrained));
    CHECK(!nimby::mod_host::planResources(33,24,32*gib,32*gib,constrained));
    CHECK(!nimby::mod_host::planResources(1,24,32*gib,128*1024*1024,constrained));
    std::cout<<"PASS resource reservations: fixed fair shares, CPU/memory/RPC aggregate bounds and low-memory admission rejection\n";
}
void rejectedBatch() {
    const auto directory=std::filesystem::temp_directory_path()/(L"nimby-host-admission-"+std::to_wstring(GetCurrentProcessId()));
    CHECK(std::filesystem::create_directory(directory));
    struct Cleanup {
        std::filesystem::path root;
        ~Cleanup(){std::error_code error;for(unsigned i=0;i<33;++i){const auto child=root/std::to_string(i);
            std::filesystem::remove(child/"nrf-mod.ini",error);std::filesystem::remove(child,error);}std::filesystem::remove(root,error);}
    } cleanup{directory};
    for(unsigned i=0;i<33;++i){const auto child=directory/std::to_string(i);CHECK(std::filesystem::create_directory(child));
        std::ofstream manifest(child/"nrf-mod.ini");manifest<<"[NRFMod]\nlibrary=fixture.dll\n";}
    const auto before=channelMappings();
    CHECK(NimbyInternal_StartModHosts(directory.c_str())==NIMBY_RESOURCE_LIMIT);
    CHECK(channelMappings()==before);CHECK(NimbyInternal_StopModHosts()==NIMBY_OK);
    std::cout<<"PASS rejected batch reports RESOURCE_LIMIT before creating any worker channel\n";
}
void actionWakeProcesses(const std::filesystem::path& exe,const std::filesystem::path& directory,
                         nimby::mod_host::LaunchOptions options) {
    const auto mappings=channelMappings();DWORD handlesBefore{};CHECK(GetProcessHandleCount(GetCurrentProcess(),&handlesBefore));
    unsigned cleanedBefore{};{std::lock_guard lock(fixture::mutex);cleanedBefore=fixture::cleaned;}
    // Declare owners first: the release guard runs before Worker destructors
    // even if any check throws while a child is held in the startup barrier.
    std::unique_ptr<nimby::mod_host::Worker> a,b;
    struct Release {~Release(){fixture::releaseActionFixtures();}} release;
    struct OwnedHandle {HANDLE value{};~OwnedHandle(){if(value)CloseHandle(value);}} oldEvent;
    options.startupTimeoutMs=3000;
    a=std::make_unique<nimby::mod_host::Worker>(exe,directory/L"host_fixture_14.dll",options);
    b=std::make_unique<nimby::mod_host::Worker>(exe,directory/L"host_fixture_15.dll",options);
    CHECK(fixture::waitAction(14,1)&&fixture::waitAction(15,1));
    auto first=fixture::get(14),second=fixture::get(15);
    CHECK(first.actionChecks==31&&second.actionChecks==31);
    CHECK(first.localWakes==1&&second.localWakes==1&&first.remoteWakes==0&&second.remoteWakes==0);
    const auto eventA=reinterpret_cast<HANDLE>(first.actionWake),eventB=reinterpret_cast<HANDLE>(second.actionWake);
    CHECK(eventA&&eventB);
    // Neither child can consume the signal while held in its RPC barrier.
    // This distinguishes kernel objects, not merely different HANDLE values.
    CHECK(SetEvent(eventA));CHECK(WaitForSingleObject(eventB,0)==WAIT_TIMEOUT);
    CHECK(WaitForSingleObject(eventA,0)==WAIT_OBJECT_0);CHECK(WaitForSingleObject(eventA,0)==WAIT_TIMEOUT);
    CHECK(DuplicateHandle(GetCurrentProcess(),eventA,GetCurrentProcess(),&oldEvent.value,0,FALSE,DUPLICATE_SAME_ACCESS));
    CHECK(SetEvent(eventA));fixture::releaseActionFixtures();
    CHECK(fixture::waitAction(14,2,1));
    CHECK(fixture::get(15).remoteWakes==0);
    CHECK(SetEvent(eventB));CHECK(fixture::waitAction(15,2,1));
    a->requestStop();b->requestStop();
    CHECK(waitFinished(*a)&&waitFinished(*b));a->join();b->join();
    CHECK(a->outcome()=="stopped"&&b->outcome()=="stopped");
    CHECK(fixture::get(14).actionStage==3&&fixture::get(15).actionStage==3);
    CHECK(fixture::get(14).remoteWakes==1&&fixture::get(15).remoteWakes==1);
    CHECK(fixture::get(14).actionChecks==63&&fixture::get(15).actionChecks==63);
    CHECK(channelMappings()==mappings);a.reset();b.reset();

    // Keep the old event alive deliberately, then start a fresh channel. A
    // stale producer for the old owner must never wake the replacement worker.
    {std::lock_guard lock(fixture::mutex);fixture::stats.erase(14);fixture::releasedActionFixtures.erase(14);}
    a=std::make_unique<nimby::mod_host::Worker>(exe,directory/L"host_fixture_14.dll",options);
    CHECK(fixture::waitAction(14,1));const auto fresh=fixture::get(14);
    CHECK(fresh.owner!=first.owner&&fresh.actionChecks==31);
    const auto replacement=reinterpret_cast<HANDLE>(fresh.actionWake);
    CHECK(SetEvent(oldEvent.value));CHECK(WaitForSingleObject(replacement,0)==WAIT_TIMEOUT);
    CHECK(SetEvent(replacement));fixture::releaseActionFixtures();CHECK(fixture::waitAction(14,2,1));
    a->requestStop();CHECK(waitFinished(*a));a->join();CHECK(a->outcome()=="stopped");
    CHECK(fixture::get(14).actionStage==3&&fixture::get(14).remoteWakes==1&&fixture::get(14).actionChecks==63);
    a.reset();CloseHandle(oldEvent.value);oldEvent.value=nullptr;
    CHECK(channelMappings()==mappings);
    DWORD handlesAfter{};CHECK(GetProcessHandleCount(GetCurrentProcess(),&handlesAfter)&&handlesAfter<=handlesBefore+2);
    {std::lock_guard lock(fixture::mutex);CHECK(fixture::cleaned==cleanedBefore+3);}
    std::cout<<"PASS real action events: wait-only child rights, independent remote/local events, no cross-worker wake, stop/restart and bounded handle lifetime\n";
}
void watchdogResume(const std::filesystem::path& exe,const std::filesystem::path& directory) {
    using namespace nimby::mod_host;
    CHECK(watchdogStamp(0)==1&&watchdogStamp(9999)==1&&watchdogStamp(10000)==2);
    CHECK(watchdogStamp(UINT64_MAX)<uint64_t(std::numeric_limits<LONG64>::max()));
    const auto began=watchdogStamp(1000ull*10000);
    CHECK(!watchdogExpired(0,UINT64_MAX,5000));
    CHECK(!watchdogExpired(began,began-1,5000));
    CHECK(!watchdogExpired(began,began+5000,5000));
    CHECK(watchdogExpired(began,began+5001,5000));
    LaunchOptions options;options.targetPid=GetCurrentProcessId();options.memoryLimit=128*1024*1024;
    Worker healthy(exe,directory/L"host_fixture_0.dll",options);
    CHECK(fixture::wait(0,16));const auto baseline=fixture::get(0);
    const auto started=watchdogNow();
    Worker blocked(exe,directory/L"host_fixture_16.dll",options);
    CHECK(fixture::wait(16,2)); // Child entered its real outermost callback.
    advanceHostTestWall(20*60*1000);
    // Simulate twenty minutes of sleep without suspending this PC or modifying
    // its clock. No awake budget was consumed; the healthy sibling still runs.
    std::this_thread::sleep_for(150ms);
    CHECK(!blocked.finished());CHECK(!healthy.finished());
    CHECK(fixture::wait(0,baseline.calls+16));
    CHECK(waitFinished(blocked,8s));blocked.join();checkExited(blocked.pid());
    CHECK(blocked.outcome()=="callback timeout; quarantined");
    const auto elapsed=watchdogNow()-started;
    CHECK(elapsed>=5000&&elapsed<8000);
    CHECK(!healthy.finished());const auto after=fixture::get(0);CHECK(fixture::wait(0,after.calls+16));
    healthy.requestStop();CHECK(waitFinished(healthy));healthy.join();CHECK(healthy.outcome()=="stopped");
    std::cout<<"PASS twenty-minute uptime jump preserves callback; real 5000ms awake hang quarantined after "<<elapsed<<"ms; sibling remains healthy\n";
    // An old parent uses the same mailbox layout but a different clock epoch.
    // Reject it before assigning any child state, while public mod V1 remains.
    CHECK(protocol==1&&channelProtocol==2);
    struct Handles {
        std::array<HANDLE,5> values{};
        ~Handles(){for(auto value:values)if(value)CloseHandle(value);}
    } handles;
    handles.values[0]=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),nullptr);
    for(size_t i=1;i<handles.values.size();++i)handles.values[i]=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    for(auto value:handles.values)CHECK(value);
    auto* mailbox=static_cast<Shared*>(MapViewOfFile(handles.values[0],FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    CHECK(mailbox);mailbox->version=1;mailbox->size=sizeof(Shared);mailbox->targetPid=GetCurrentProcessId();
    const auto legacy=attach(handles.values[0],handles.values[1],handles.values[2],handles.values[3],handles.values[4],GetCurrentProcessId());
    UnmapViewOfFile(mailbox);CHECK(legacy==NIMBY_INVALID_BINARY);
    std::cout<<"PASS watchdog zero sentinel, future marker, exact deadline and old mailbox rejection; mod V1 retained\n";
}
}
int wmain(int argc,wchar_t** argv) {
    // Worker supplies exactly seven arguments; parent CTest supplies only the
    // fixture directory. Faulty DLLs can never accidentally start in the parent.
    if(argc==8)return nimby::mod_host::runChild(argc,argv);
    try {
        CHECK(argc==2||argc==3);IsolatedDiagnostics diagnostics;resourcePlans();rejectedBatch();const std::filesystem::path directory=argv[1];const auto exe=executable();
        if(argc==3&&std::wstring_view(argv[2])==L"--watchdog-resume") {watchdogResume(exe,directory);return 0;}
        nimby::mod_host::LaunchOptions options;options.targetPid=GetCurrentProcessId();
        SYSTEM_INFO machine{};GetSystemInfo(&machine);
        CHECK(nimby::mod_host::planResources(4,machine.dwNumberOfProcessors,32ull*1024*1024*1024,8ull*1024*1024*1024,options));
        options.startupTimeoutMs=1200;options.callbackTimeoutMs=350;options.shutdownTimeoutMs=300;
        options.memoryLimit=128*1024*1024;
        const auto originalMappings=channelMappings();
        // An optional retained pre-change DLL exercises real legacy modules
        // against this candidate host without teaching the module a new ABI.
        const auto healthyLibrary=argc==3?std::filesystem::path(argv[2]):directory/L"host_fixture_0.dll";
        nimby::mod_host::Worker healthy(exe,healthyLibrary,options);
        CHECK(healthy.pid()!=GetCurrentProcessId());CHECK(fixture::wait(0,32));fixture::report("healthy_process_baseline",0);
        CHECK(fixture::get(0).cpuLimit==options.cpuRate&&fixture::get(0).memoryLimit==options.memoryLimit);
        CHECK(fixture::get(0).priority==BELOW_NORMAL_PRIORITY_CLASS);
        CHECK(channelMappings()==originalMappings+1);
        const std::array<const char*,9> labels{"healthy","native_crash","startup_hang","callback_busy_loop","shutdown_hang","uncaught_exception","rpc_flood","concurrent_callbacks","nested_callbacks"};
        for(unsigned mode=1;mode<labels.size();++mode) {
            const auto before=fixture::get(0);const auto started=std::chrono::steady_clock::now();
            nimby::mod_host::Worker offender(exe,directory/(L"host_fixture_"+std::to_wstring(mode)+L".dll"),options);
            CHECK(offender.pid()!=GetCurrentProcessId()&&offender.pid()!=healthy.pid());
            CHECK(fixture::wait(mode,1));
            if(mode==4){CHECK(fixture::wait(mode,3));offender.requestStop();}
            if(mode==6){CHECK(fixture::wait(0,before.calls+32));CHECK(fixture::get(6).calls>=32);offender.requestStop();}
            CHECK(waitFinished(offender));offender.join();checkExited(offender.pid());
            // The failed Worker is deliberately still alive here: its outcome
            // must not retain a large shared-memory view until destruction.
            CHECK(channelMappings()==originalMappings+1);
            CHECK(!healthy.finished());CHECK(fixture::wait(0,before.calls+32));fixture::report(labels[mode],before.latency.size());
            {
                std::lock_guard lock(fixture::mutex);
                CHECK(fixture::active.contains(fixture::stats[0].owner));
                CHECK(!fixture::active.contains(fixture::stats[mode].owner));
            }
            const auto outcome=offender.outcome();
            if(mode==2)CHECK(outcome.find("startup timeout")!=std::string::npos);
            if(mode==3||mode==7||mode==8)CHECK(outcome.find("callback timeout")!=std::string::npos);
            if(mode==4)CHECK(outcome.find("stop timeout")!=std::string::npos);
            if(mode==1||mode==5)CHECK(outcome.find("terminated, exit=")!=std::string::npos);
            if(mode==6){
                const auto flood=fixture::get(6);CHECK(outcome=="stopped"&&flood.throttled>0);
                std::cout<<"{\"flood_dispatched\":"<<flood.calls<<",\"flood_resource_limited\":"<<flood.throttled<<"}\n";
            }
            std::cout<<"{\"fault\":\""<<labels[mode]<<"\",\"outcome\":\""<<outcome<<"\",\"elapsed_ms\":"
                <<std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count()<<"}\n";
        }
        const auto before=fixture::get(0);
        std::vector<std::unique_ptr<nimby::mod_host::Worker>> flooders;
        const auto floodStarted=std::chrono::steady_clock::now();
        for(unsigned mode=9;mode<=11;++mode){
            flooders.push_back(std::make_unique<nimby::mod_host::Worker>(exe,directory/(L"host_fixture_"+std::to_wstring(mode)+L".dll"),options));
            CHECK(fixture::wait(mode,3));
        }
        CHECK(fixture::wait(0,before.calls+128));
        for(auto& offender:flooders)offender->requestStop();
        for(auto& offender:flooders){CHECK(waitFinished(*offender));offender->join();CHECK(offender->outcome()=="stopped");checkExited(offender->pid());}
        const auto seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-floodStarted).count();
        for(unsigned mode=9;mode<=11;++mode){
            const auto value=fixture::get(mode);
            CHECK(value.throttled>0&&value.calls>3);
            CHECK(value.calls<=options.rpcCallBurst+seconds*options.rpcCallsPerSecond+1);
            CHECK(value.cpuLimit==options.cpuRate&&value.memoryLimit==options.memoryLimit&&value.priority==BELOW_NORMAL_PRIORITY_CLASS);
            std::cout<<"{\"concurrent_flooder\":"<<mode<<",\"calls\":"<<value.calls<<",\"resource_limited\":"<<value.throttled<<"}\n";
        }
        CHECK(!healthy.finished());CHECK(channelMappings()==originalMappings+1);
        fixture::report("three_expensive_broker_flooders",before.latency.size());
        {
            const auto prior=fixture::get(0);
            nimby::mod_host::Worker offender(exe,directory/L"host_fixture_12.dll",options);
            CHECK(fixture::wait(12,1));CHECK(fixture::wait(0,prior.calls+8));
            const auto started=std::chrono::steady_clock::now();offender.requestStop();
            CHECK(waitFinished(offender));offender.join();CHECK(offender.outcome().find("stop timeout")!=std::string::npos);
            CHECK(std::chrono::steady_clock::now()-started<3s);CHECK(!healthy.finished());
            std::cout<<"PASS continuously signaled request event cannot starve the shutdown deadline\n";
        }
        {
            const auto prior=fixture::get(0);
            nimby::mod_host::Worker offender(exe,directory/L"host_fixture_13.dll",options);
            CHECK(fixture::wait(13,1));CHECK(fixture::wait(0,prior.calls+32));
            offender.requestStop();CHECK(waitFinished(offender));offender.join();
            CHECK(offender.outcome()=="stopped"&&fixture::get(13).throttled>10);CHECK(!healthy.finished());
            std::cout<<"PASS oversized malformed requests consume admission quota before validation\n";
        }
        actionWakeProcesses(exe,directory,options);
        healthy.requestStop();CHECK(waitFinished(healthy));healthy.join();checkExited(healthy.pid());
        CHECK(channelMappings()==originalMappings);
        CHECK(healthy.outcome()=="stopped");
        {std::lock_guard lock(fixture::mutex);CHECK(fixture::active.empty()&&fixture::cleaned==17);}
        diagnostics.verifyRejections();
        std::cout<<"PASS real child processes: native crash, startup/callback/shutdown hangs, uncaught exception, RPC flood, parent/healthy progress and scoped cleanup\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
    return 0;
}
