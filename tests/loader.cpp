#include "loader/loader.h"
#include "engine/binary_identity.h"
#include <cstdio>
#include <filesystem>
#include <stdexcept>

#define CHECK(x) do { if (!(x)) throw std::runtime_error("FAIL line " + std::to_string(__LINE__) + ": " #x); } while(false)
namespace {
struct Child {
    HANDLE ready{}, stop{}, observed{}, process{};
    DWORD pid{};
    Child(const std::wstring& path,unsigned sequence) {
        const auto prefix=L"Local\\NimbySDK.Test."+std::to_wstring(GetCurrentProcessId())+L"."+std::to_wstring(sequence);
        const auto r=prefix+L".ready", s=prefix+L".stop", o=prefix+L".observed";
        ready=CreateEventW(nullptr,TRUE,FALSE,r.c_str());
        stop=CreateEventW(nullptr,TRUE,FALSE,s.c_str());
        observed=CreateEventW(nullptr,TRUE,FALSE,o.c_str());
        CHECK(ready && stop && observed);
        auto command=L"\""+path+L"\" "+r+L" "+s+L" "+o;
        STARTUPINFOW startup{}; startup.cb=sizeof startup;
        PROCESS_INFORMATION info{};
        CHECK(CreateProcessW(path.c_str(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&info));
        process=info.hProcess; pid=info.dwProcessId; CloseHandle(info.hThread);
        CHECK(WaitForSingleObject(ready,5000)==WAIT_OBJECT_0);
    }
    void finish() {
        SetEvent(stop);
        CHECK(WaitForSingleObject(process,5000)==WAIT_OBJECT_0);
        DWORD code{}; CHECK(GetExitCodeProcess(process,&code) && code==0);
    }
    ~Child() {
        if(stop) SetEvent(stop);
        if(process) {
            if(WaitForSingleObject(process,5000)!=WAIT_OBJECT_0) TerminateProcess(process,99); // Our fixture only.
            CloseHandle(process);
        }
        if(ready) CloseHandle(ready);
        if(stop) CloseHandle(stop);
        if(observed) CloseHandle(observed);
    }
};
}
int wmain(int argc,wchar_t** argv) {
    try {
        CHECK(argc==3);
        const auto target=std::filesystem::absolute(argv[1]).wstring();
        const auto sdk=std::filesystem::absolute(argv[2]).wstring();
        NimbyBinaryInfo identity{};
        CHECK(nimby::engine::identify(target.c_str(),identity)==NIMBY_OK);
        CHECK(!identity.recognized_research_build); // Never accidentally test against the real game.
        {
            nimby::loader::Monitor denied(target,std::string(64,'0'),sdk);
            Child child(target,0);
            const auto events=denied.poll();
            CHECK(events.size()==1 && !events[0].success);
            CHECK(events[0].message.find("SHA-256")!=std::string::npos);
            CHECK(WaitForSingleObject(child.observed,50)==WAIT_TIMEOUT);
            CHECK(denied.poll().empty());
            child.finish();
        }
        nimby::loader::Monitor monitor(target,identity.sha256,sdk);
        CHECK(monitor.poll().empty());
        // Same executable name/hash in a different directory must not match.
        const auto decoy_dir=std::filesystem::path(target).parent_path()/L"loader-test-decoy";
        std::filesystem::create_directories(decoy_dir);
        const auto decoy=decoy_dir/std::filesystem::path(target).filename();
        std::filesystem::copy_file(target,decoy,std::filesystem::copy_options::overwrite_existing);
        {
            Child child(decoy.wstring(),100);
            CHECK(monitor.poll().empty());
            CHECK(WaitForSingleObject(child.observed,50)==WAIT_TIMEOUT);
            child.finish();
        }
        std::filesystem::remove(decoy);
        std::filesystem::remove(decoy_dir);
        for(unsigned cycle=1;cycle<=3;++cycle) {
            Child child(target,cycle);
            const auto events=monitor.poll();
            for (const auto& event:events) std::printf("PID %lu: %s\n",event.pid,event.message.c_str());
            CHECK(events.size()==1 && events[0].pid==child.pid && events[0].success);
            CHECK(WaitForSingleObject(child.observed,5000)==WAIT_OBJECT_0);
            CHECK(monitor.poll().empty()); // No second LoadLibrary/initialization per process instance.
            {
                nimby::loader::Monitor restarted(target,identity.sha256,sdk);
                const auto duplicate=restarted.poll();
                CHECK(duplicate.size()==1 && duplicate[0].success);
                CHECK(duplicate[0].message.find("already initialized")!=std::string::npos);
            }
            child.finish();
            CHECK(monitor.poll().empty());
        }
        std::puts("PASS remote SDK initialization, hash refusal, deduplication, monitor restart, 3 process restarts");
        return 0;
    } catch(const std::exception& error) { std::fprintf(stderr,"%s\n",error.what()); return 1; }
}
