#include "../fixture.h"
#include <windows.h>
#include <psapi.h>
#include <bit>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>

#define CHECK(condition) do {if(!(condition))throw std::runtime_error("Check failed: " #condition);}while(false)
namespace {
template<class Function> Function symbol(HMODULE module,const char* name) {
    const auto value=GetProcAddress(module,name);CHECK(value);return std::bit_cast<Function>(value);
}
using namespace policy_host_fixture;
Snapshot snapshot{};
Stats read(){Stats value;CHECK(snapshot(&value)==0);return value;}
PROCESS_MEMORY_COUNTERS_EX memory() {
    PROCESS_MEMORY_COUNTERS_EX value{};value.cb=sizeof value;
    CHECK(GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&value),sizeof value));
    return value;
}
template<class Condition> Stats await(Condition&& condition) {
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds{3};
    for(;;){const auto value=read();if(condition(value))return value;
        CHECK(std::chrono::steady_clock::now()<deadline);std::this_thread::sleep_for(std::chrono::milliseconds{5});}
}
}
int main(int argc,char** argv) {
    try {
        CHECK(argc==3);
        const auto job=CreateJobObjectW(nullptr,nullptr);CHECK(job);
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
        limits.BasicLimitInformation.LimitFlags=JOB_OBJECT_LIMIT_PROCESS_MEMORY;
        limits.ProcessMemoryLimit=192ull*1024*1024;
        CHECK(SetInformationJobObject(job,JobObjectExtendedLimitInformation,&limits,sizeof limits));
        CHECK(AssignProcessToJobObject(job,GetCurrentProcess()));
        BOOL inJob{};CHECK(IsProcessInJob(GetCurrentProcess(),job,&inJob)&&inJob);
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION applied{};
        CHECK(QueryInformationJobObject(job,JobObjectExtendedLimitInformation,&applied,sizeof applied,nullptr));
        CHECK(applied.ProcessMemoryLimit==192ull*1024*1024);
        CHECK(applied.BasicLimitInformation.LimitFlags&JOB_OBJECT_LIMIT_PROCESS_MEMORY);
        const auto baseline=memory();
        const auto facade=LoadLibraryExW(std::filesystem::absolute(argv[1]).c_str(),nullptr,
            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);CHECK(facade);
        const auto configure=symbol<Configure>(facade,"Test_Configure");
        const auto change=symbol<Change>(facade,"Test_Change");
        const auto busy=symbol<Busy>(facade,"Test_BusyUpdates");
        snapshot=symbol<Snapshot>(facade,"Test_Snapshot");
        CHECK(configure(1200,Normal)==0);
        const auto mod=LoadLibraryExW(std::filesystem::absolute(argv[2]).c_str(),nullptr,
            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);CHECK(mod);
        using Lifecycle=std::uint32_t(WINAPI*)(void*);
        const auto start=symbol<Lifecycle>(mod,"NRFMod_StartV1");
        const auto stop=symbol<Lifecycle>(mod,"NRFMod_StopV1");
        const auto initialized=symbol<Lifecycle>(mod,"NRFMod_IsInitializedV1");
        CHECK(initialized(nullptr)==0);CHECK(start(nullptr)==0);CHECK(initialized(nullptr)==1);
        auto state=read();CHECK(state.optionAdds==1&&state.optionReads==1&&state.limitAdds==1);
        const auto companion=std::filesystem::path(argv[2]).stem().wstring()+L"Kotlin.dll";
        const auto kotlin=GetModuleHandleW(companion.c_str());CHECK(kotlin);
        const auto abi=symbol<int(*)()>(kotlin,"NRFKotlin_Version")();CHECK(abi==10||abi==11);
        CHECK(state.editorAdds==(abi==11?1u:0u));
        if(abi==11)CHECK(state.declarationBytes>0&&state.resolvedMessages==6);
        CHECK(state.registeredMeters==1200&&state.publishedMeters==1200&&state.limitOwner);
        CHECK(state.readSequence<state.registerSequence&&state.lastKnownRevision==0);
        const auto initialReads=state.optionReads;
        // Exceed the ordinary tool observation cadence. The pure declaration
        // must sleep indefinitely, with no preference polling or game capture.
        std::this_thread::sleep_for(std::chrono::milliseconds{600});state=read();
        CHECK(state.optionReads==initialReads&&!state.limitUpdates&&!state.openProcessCalls&&!state.captureCalls);

        CHECK(change(1500,2)==0);state=await([](const Stats& s){return s.publishedMeters==1500;});
        CHECK(state.limitAdds==1&&state.limitUpdates==1&&state.optionReads==initialReads+1&&state.lastKnownRevision==1);
        CHECK(change(1600,1)==0);state=await([](const Stats& s){return s.publishedMeters==1600;});
        CHECK(state.limitUpdates==2&&state.lastKnownRevision==2);
        const auto updates=state.limitUpdates,reads=state.optionReads;
        CHECK(change(1600,1)==0);state=await([&](const Stats& s){return s.optionReads>reads;});
        std::this_thread::sleep_for(std::chrono::milliseconds{50});CHECK(read().limitUpdates==updates);

        // Exhaust one bounded publication attempt. The same saved revision is
        // retried, and the previously published capability remains owned.
        CHECK(busy(3)==0);CHECK(change(1700,2)==0);
        state=await([](const Stats& s){return s.limitUpdates>=5;});
        CHECK(state.publishedMeters==1600&&state.limitOwner&&!state.limitRemoves);
        state=await([](const Stats& s){return s.publishedMeters==1700;});
        CHECK(state.limitUpdates==6&&state.lastKnownRevision==4);
        const auto settledReads=state.optionReads;
        std::this_thread::sleep_for(std::chrono::milliseconds{300});state=read();
        CHECK(state.optionReads==settledReads&&!state.openProcessCalls&&!state.captureCalls);
        CHECK(state.editorAdds==(abi==11?1u:0u)); // Messages/catalogue are never resent by updates.
        CHECK(stop(nullptr)==0&&initialized(nullptr)==0);state=read();
        CHECK(state.limitRemoves==1&&state.optionRemoves==1&&!state.limitOwner&&!state.optionOwner);
        CHECK(start(nullptr)==0);state=read();CHECK(state.registeredMeters==1700&&state.limitAdds==2);
        CHECK(state.editorAdds==(abi==11?2u:0u));
        CHECK(stop(nullptr)==0);

        for(const auto failure:{FailPreferenceRead,FailRegistration,MissingActionWait}){
            CHECK(configure(1300,failure)==0);CHECK(start(nullptr)!=0&&initialized(nullptr)==0);
            state=read();CHECK(!state.limitOwner&&!state.optionOwner&&!state.openProcessCalls&&!state.captureCalls);
            if(failure==MissingActionWait)CHECK(!state.optionAdds&&!state.limitAdds);
            else CHECK(state.optionAdds==1&&state.optionRemoves==1);
            CHECK(configure(1400,Normal)==0);CHECK(start(nullptr)==0);CHECK(read().registeredMeters==1400);
            CHECK(stop(nullptr)==0);
        }
        // Mod stop must join its only worker before these modules could unload.
        // Kotlin retains its own runtime code until process exit.
        const auto finalMemory=memory();
        CHECK(QueryInformationJobObject(job,JobObjectExtendedLimitInformation,&applied,sizeof applied,nullptr));
        CHECK(finalMemory.PrivateUsage<applied.ProcessMemoryLimit);
        CHECK(finalMemory.PeakPagefileUsage<applied.ProcessMemoryLimit);
        CHECK(applied.PeakProcessMemoryUsed<applied.ProcessMemoryLimit);
        std::cout<<"PASS: real policy-only adapter, saved preference before Register, remote/local event Update, "
                    "unchanged value, contention retry, idle no capture/polling, stop/restart and startup rollback; ABI="<<abi<<'\n';
        if(abi==11)std::cout<<"PASS: immutable mod-owned message declaration, three messages resolved in FR/EN, no declaration resend on Update\n";
        std::cout<<"Memory quota bytes="<<applied.ProcessMemoryLimit<<", baseline PrivateUsage="<<baseline.PrivateUsage
                 <<", final PrivateUsage="<<finalMemory.PrivateUsage<<", PeakPagefileUsage="<<finalMemory.PeakPagefileUsage
                 <<", job peak process bytes="<<applied.PeakProcessMemoryUsed<<'\n';
        return 0;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
