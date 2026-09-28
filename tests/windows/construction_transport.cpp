#include "platform/windows/runtime/construction_bridge.h"
#include "nimby/detail/observation.h"
#include <stdexcept>
#include <cstdio>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
int main(){try{
    using nimby::construction_bridge::exchange;
    NimbyConstructionRequest request{};request.size=sizeof request;request.version=1;
    request.action=NIMBY_CONSTRUCTION_PREPARE;request.source_signal=0x8000000000001;
    NimbyConstructionResult result{};result.size=sizeof result;result.version=1;
    NimbyBinaryInfo binary{};
    // Current-process bootstrap must work without external injection. The fake
    // endpoint lives beside this test executable in an isolated fixture folder.
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    CHECK(result.token==123&&result.state==NIMBY_CONSTRUCTION_READY);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,123,result)==NIMBY_OK);
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,nullptr,124,result)==NIMBY_INVALID_HANDLE);
    // Repeated preparation accepts ALREADY_INITIALIZED from the resident DLL.
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_OK);
    request.action=999;
    CHECK(exchange(GetCurrentProcess(),GetCurrentProcessId(),binary,&request,0,result)==NIMBY_INVALID_ARGUMENT);
    std::puts("PASS: in-process construction bootstrap, exchange, polling and resident reuse");
}catch(const std::exception& e){std::fprintf(stderr,"%s\n",e.what());return 1;}}
