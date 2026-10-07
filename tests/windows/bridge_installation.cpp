#include <platform/windows/bridge_installation.h>
#include <filesystem>
#include <future>
#include <chrono>
#include <iostream>
#include <bit>
#include <vector>
#include <algorithm>
#include <stdexcept>

#define CHECK(x) do { if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x); } while(false)
int main(int argc,char** argv){try {
    using Bootstrap=uint32_t(__cdecl*)(HANDLE,HANDLE,uint32_t);
    CHECK(argc==3);
    const auto first=LoadLibraryW(std::filesystem::absolute(argv[1]).c_str());
    const auto second=LoadLibraryW(std::filesystem::absolute(argv[2]).c_str());
    CHECK(first&&second&&first!=second);
    const auto a=std::bit_cast<Bootstrap>(GetProcAddress(first,"Fixture_Bootstrap"));
    const auto b=std::bit_cast<Bootstrap>(GetProcAddress(second,"Fixture_Bootstrap"));
    CHECK(a&&b);
    const auto entered=CreateEventW(nullptr,TRUE,FALSE,nullptr),release=CreateEventW(nullptr,TRUE,FALSE,nullptr);
    CHECK(entered&&release);
    auto slow=std::async(std::launch::async,[&]{return a(entered,release,1);});
    CHECK(WaitForSingleObject(entered,1000)==WAIT_OBJECT_0);
    std::vector<double> samples;
    bool refused=true;
    for(int i=0;i<64;++i){
        const auto started=std::chrono::steady_clock::now();
        refused=refused&&(b(nullptr,nullptr,0)==NIMBY_RESOURCE_LIMIT);
        samples.push_back(std::chrono::duration<double,std::micro>(std::chrono::steady_clock::now()-started).count());
    }
    SetEvent(release);CHECK(slow.get()==NIMBY_OK&&refused);
    std::sort(samples.begin(),samples.end());CHECK(samples.back()<500000);
    CHECK(b(nullptr,nullptr,0)==NIMBY_OK);
    CHECK(a(nullptr,nullptr,2)==NIMBY_INTERNAL_ERROR);CHECK(b(nullptr,nullptr,0)==NIMBY_OK);
    {
        nimby::platform::windows::BridgeInstallation parent;
        CHECK(parent&&a(nullptr,nullptr,0)==NIMBY_OK&&b(nullptr,nullptr,0)==NIMBY_OK);
    }
    CHECK(a(nullptr,nullptr,0)==NIMBY_OK);
    std::cout<<"{\"scenario\":\"peer_bootstrap_during_stalled_installation\",\"samples\":64,\"p50_us\":"<<samples[31]<<",\"p95_us\":"<<samples[60]<<",\"p99_us\":"<<samples[63]<<"}\n";
    std::cout<<"PASS separate DLL bootstrap exclusion, retry, recursive parent gate and exception cleanup\n";
    CloseHandle(entered);CloseHandle(release);FreeLibrary(first);FreeLibrary(second);
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
