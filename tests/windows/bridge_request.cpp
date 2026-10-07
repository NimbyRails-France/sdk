#include "platform/windows/runtime/bridge_request.h"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
int main(int argc,char**){try{
    if(argc>1){Sleep(30000);return 0;}
    using namespace nimby::platform::windows;
    auto lease=bridgeRequestLease(10000);CHECK(bridgeRequestAlive(lease));
    auto wrong=lease;++wrong.creation;CHECK(!bridgeRequestAlive(wrong));CHECK(!sameBridgeRequester(lease,wrong));
    wrong=lease;wrong.expires=GetTickCount64();CHECK(!bridgeRequestAlive(wrong));
    CHECK(bridgeRequesterAlive(wrong));CHECK(sameBridgeRequester(lease,wrong));
    std::array<wchar_t,32768> path{};CHECK(GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size())));
    std::wstring command=L"\""+std::wstring(path.data())+L"\" --child";
    STARTUPINFOW startup{};startup.cb=sizeof startup;PROCESS_INFORMATION child{};
    CHECK(CreateProcessW(path.data(),command.data(),nullptr,nullptr,FALSE,CREATE_NO_WINDOW|CREATE_SUSPENDED,nullptr,nullptr,&startup,&child));
    struct Cleanup{PROCESS_INFORMATION& child;~Cleanup(){TerminateProcess(child.hProcess,0);CloseHandle(child.hThread);CloseHandle(child.hProcess);}} cleanup{child};
    lease={GetTickCount64()+10000,processCreation(child.hProcess),child.dwProcessId,0};
    CHECK(bridgeRequestAlive(lease));
    CHECK(TerminateProcess(child.hProcess,7));CHECK(WaitForSingleObject(child.hProcess,1000)==WAIT_OBJECT_0);
    CHECK(!bridgeRequestAlive(lease));CHECK(!bridgeRequesterAlive(lease));
    std::puts("PASS: native mutation lease rejects expired, recycled-identity and dead worker requests");
}catch(const std::exception& error){std::fprintf(stderr,"%s\n",error.what());return 1;}}
