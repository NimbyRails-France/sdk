#include <nimby/detail/platform/windows/control_pipe.hpp>
#include <cassert>
#include <chrono>
using namespace nimby::detail::control;
uint32_t handle(const NimbyControlRequest* r,NimbyControlResponse* out){
    if(r->size!=sizeof *r||r->version!=1)return NIMBY_INVALID_ARGUMENT;
    out->generation=r->generation;out->aspect=r->value;out->speed_mps=r->speed_mps;return NIMBY_OK;
}
int main(){
    Server server;server.start("sdk-test",handle);
    Server duplicate;bool rejected=false;
    try{duplicate.start("sdk-test",handle);}catch(...){rejected=true;}assert(rejected);
    NimbyControlRequest request{};request.size=sizeof request;request.version=1;request.operation=NIMBY_CONTROL_STATUS;
    request.generation=123456789;request.value=77;request.speed_mps=6.75;
    NimbyControlResponse response{};response.size=sizeof response;
    assert(NimbyInternal_ModControl(GetCurrentProcessId(),"sdk-test",&request,&response)==NIMBY_OK);
    assert(response.generation==123456789&&response.aspect==77&&response.speed_mps==6.75);
    for(int i=0;i<100;++i)assert(NimbyInternal_ModControl(GetCurrentProcessId(),"sdk-test",&request,&response)==NIMBY_OK);
    assert(NimbyInternal_ModControl(GetCurrentProcessId(),"bad/id",&request,&response)==NIMBY_INVALID_ARGUMENT);
    server.stop();server.stop();
    assert(NimbyInternal_ModControl(GetCurrentProcessId(),"sdk-test",&request,&response)==NIMBY_DATA_UNAVAILABLE);
    // A client that connects but sends nothing cannot block module shutdown.
    server.start("sdk-test",handle);
    Handle idle(CreateFileW(pipeName(GetCurrentProcessId(),"sdk-test").c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,nullptr));
    assert(idle.value!=INVALID_HANDLE_VALUE);
    const auto before=std::chrono::steady_clock::now();server.stop();
    assert(std::chrono::steady_clock::now()-before<std::chrono::seconds(1));
}
