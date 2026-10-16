// Compile the real host with a test-only uptime seam. A parent-side jump models
// the supervisor running first after resume, before its child gets CPU time.
// The watchdog's awake clock is deliberately the real Windows API.
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <platform/windows/mod_host.h>
#include <platform/windows/mod_host_epoch.h>
#include <platform/windows/mod_host_watchdog.h>
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/native_library.hpp>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/signal_settings_runtime.hpp>
#include <engine/binary_identity.h>
#include <loader/manifest.h>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
void armHostTestBatchAllocationFailure()noexcept;
namespace {
std::atomic<uint64_t> wallAdvanceMs{};
std::atomic<bool> rejectCpuControl{};
std::atomic<unsigned> batchCpuConfigurations{};
// TEST ONLY. The disk sink deliberately drops contention/flooding. Capture
// emissions independently so parallel CTests cannot turn that documented
// diagnostic policy into a flaky assertion about worker lifecycle behavior.
// No production sink/worker is made blocking, and capture itself never allocates.
struct MemoryDiagnostic {uint32_t child{};std::array<char,1024> text{};};
SRWLOCK memoryDiagnosticLock=SRWLOCK_INIT;
std::array<MemoryDiagnostic,64> memoryDiagnostics;
size_t memoryDiagnosticCount=0;
uint32_t memoryDiagnosticDropped=0;
struct ResourceDiagnostic {uint32_t child{};std::array<char,1024> text{};};
SRWLOCK resourceDiagnosticLock=SRWLOCK_INIT;
std::array<ResourceDiagnostic,64> resourceDiagnostics;
size_t resourceDiagnosticCount=0;
uint32_t resourceDiagnosticDropped=0;
void captureResourceDiagnostic(const char* message) noexcept {
    constexpr char marker[]="Mod RPC resource limit:";
    if(!message||std::strncmp(message,marker,sizeof marker-1)!=0)return;
    const auto length=strnlen(message,1024);
    const auto childAt=length<1024?std::strstr(message,"child_pid="):nullptr;
    uint64_t child=0;
    if(childAt)for(const auto* at=childAt+10;*at>='0'&&*at<='9';++at){
        child=child*10+uint64_t(*at-'0');
        if(child>UINT32_MAX)break;
    }
    AcquireSRWLockExclusive(&resourceDiagnosticLock);
    if(length==1024||!child||child>UINT32_MAX||resourceDiagnosticCount==resourceDiagnostics.size())++resourceDiagnosticDropped;
    else {
        auto& sample=resourceDiagnostics[resourceDiagnosticCount++];sample.child=uint32_t(child);
        std::memcpy(sample.text.data(),message,length+1);
    }
    ReleaseSRWLockExclusive(&resourceDiagnosticLock);
}
void captureMemoryDiagnostic(const char* message) noexcept {
    constexpr char marker[]="Mod memory sample:";
    if(!message||std::strncmp(message,marker,sizeof marker-1)!=0)return;
    const auto length=strnlen(message,1024);
    const auto childAt=length<1024?std::strstr(message,"child_pid="):nullptr;
    uint64_t child=0;
    if(childAt)for(const auto* at=childAt+10;*at>='0'&&*at<='9';++at){
        child=child*10+uint64_t(*at-'0');
        if(child>UINT32_MAX)break;
    }
    AcquireSRWLockExclusive(&memoryDiagnosticLock);
    if(length==1024||!child||child>UINT32_MAX||memoryDiagnosticCount==memoryDiagnostics.size())++memoryDiagnosticDropped;
    else {
        auto& sample=memoryDiagnostics[memoryDiagnosticCount++];sample.child=uint32_t(child);
        std::memcpy(sample.text.data(),message,length+1);
    }
    ReleaseSRWLockExclusive(&memoryDiagnosticLock);
}
ULONGLONG WINAPI advancedTickCount64() {
    return GetTickCount64()+wallAdvanceMs.load(std::memory_order_relaxed);
}
BOOL WINAPI testSetInformationJobObject(HANDLE job,JOBOBJECTINFOCLASS kind,LPVOID info,DWORD size) {
    if(kind==JobObjectCpuRateControlInformation&&rejectCpuControl.load(std::memory_order_relaxed)) {
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    const auto status=SetInformationJobObject(job,kind,info,size);
    if(status&&kind==JobObjectCpuRateControlInformation){
        const auto remaining=batchCpuConfigurations.load();
        if(remaining&&batchCpuConfigurations.fetch_sub(1)==1)armHostTestBatchAllocationFailure();
    }
    return status;
}
}
void armHostTestBatchLaunchFault()noexcept{batchCpuConfigurations=2;}
void advanceHostTestWall(uint64_t milliseconds) noexcept {wallAdvanceMs=milliseconds;}
void rejectHostTestCpuControl(bool reject) noexcept {rejectCpuControl=reject;}
void resetHostTestMemoryDiagnostics() noexcept {
    // The parent resets only before starting workers, after the recorder's own
    // finite concurrency fixture has joined. Child processes have separate state.
    AcquireSRWLockExclusive(&memoryDiagnosticLock);
    memoryDiagnosticCount=0;memoryDiagnosticDropped=0;
    ReleaseSRWLockExclusive(&memoryDiagnosticLock);
}
uint32_t droppedHostTestMemoryDiagnostics() noexcept {
    AcquireSRWLockShared(&memoryDiagnosticLock);const auto count=memoryDiagnosticDropped;
    ReleaseSRWLockShared(&memoryDiagnosticLock);return count;
}
std::vector<std::string> hostTestMemoryDiagnostics(uint32_t child) {
    std::array<MemoryDiagnostic,64> copied;size_t count;
    AcquireSRWLockShared(&memoryDiagnosticLock);count=memoryDiagnosticCount;
    std::memcpy(copied.data(),memoryDiagnostics.data(),count*sizeof(MemoryDiagnostic));
    ReleaseSRWLockShared(&memoryDiagnosticLock);
    std::vector<std::string> result;
    for(size_t n=0;n<count;++n)if(copied[n].child==child)result.emplace_back(copied[n].text.data());
    return result;
}
void resetHostTestResourceDiagnostics() noexcept {
    AcquireSRWLockExclusive(&resourceDiagnosticLock);
    resourceDiagnosticCount=0;resourceDiagnosticDropped=0;
    ReleaseSRWLockExclusive(&resourceDiagnosticLock);
}
uint32_t droppedHostTestResourceDiagnostics() noexcept {
    AcquireSRWLockShared(&resourceDiagnosticLock);const auto count=resourceDiagnosticDropped;
    ReleaseSRWLockShared(&resourceDiagnosticLock);return count;
}
std::vector<std::string> hostTestResourceDiagnostics(uint32_t child) {
    std::array<ResourceDiagnostic,64> copied;size_t count;
    AcquireSRWLockShared(&resourceDiagnosticLock);count=resourceDiagnosticCount;
    std::memcpy(copied.data(),resourceDiagnostics.data(),count*sizeof(ResourceDiagnostic));
    ReleaseSRWLockShared(&resourceDiagnosticLock);
    std::vector<std::string> result;
    for(size_t n=0;n<count;++n)if(!child||copied[n].child==child)result.emplace_back(copied[n].text.data());
    return result;
}
bool hostTestMemoryDiagnosticContract() {
    resetHostTestMemoryDiagnostics();
    const auto emit=[](const char* message){for(unsigned n=0;n<16;++n)captureMemoryDiagnostic(message);};
    std::thread first(emit,"Mod memory sample: event=startup_ready child_pid=71");
    std::thread second(emit,"Mod memory sample: event=process_exit child_pid=72");
    first.join();second.join();
    const auto concurrent=hostTestMemoryDiagnostics(71).size()==16&&hostTestMemoryDiagnostics(72).size()==16&&
        droppedHostTestMemoryDiagnostics()==0;
    for(unsigned n=0;n<33;++n)captureMemoryDiagnostic("Mod memory sample: event=process_exit child_pid=73");
    const auto bounded=hostTestMemoryDiagnostics(73).size()==32&&droppedHostTestMemoryDiagnostics()==1;
    resetHostTestMemoryDiagnostics();
    captureMemoryDiagnostic("Other event child_pid=74");
    const auto reset=hostTestMemoryDiagnostics(71).empty()&&hostTestMemoryDiagnostics(72).empty()&&
        hostTestMemoryDiagnostics(73).empty()&&droppedHostTestMemoryDiagnostics()==0;
    return concurrent&&bounded&&reset;
}
namespace nimby::detail::host_fixture_diagnostics {
inline void write(const char* component,const char* level,const char* message) noexcept {
    captureMemoryDiagnostic(message);
    captureResourceDiagnostic(message);
    nimby::detail::diagnostics::write(component,level,message);
}
inline void exception(const char* component,const char* operation) noexcept {
    nimby::detail::diagnostics::exception(component,operation);
}
}
#define GetTickCount64 advancedTickCount64
#define SetInformationJobObject testSetInformationJobObject
// All headers above have already been parsed with their production namespace.
// Redirect only the included host's call sites; the real disk sink is unchanged.
#define diagnostics host_fixture_diagnostics
#ifndef NIMBY_MOD_HOST_RUNTIME_SOURCE
#define NIMBY_MOD_HOST_RUNTIME_SOURCE "../../src/platform/windows/runtime/mod_host.cpp"
#endif
#include NIMBY_MOD_HOST_RUNTIME_SOURCE
#undef diagnostics
#undef SetInformationJobObject
#undef GetTickCount64
