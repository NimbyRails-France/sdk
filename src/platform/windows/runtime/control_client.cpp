#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/platform/windows/control_pipe.hpp>
extern "C" NIMBY_API uint32_t __cdecl NimbyInternal_ModControl(uint32_t pid,const char* id,
    const NimbyControlRequest* request,NimbyControlResponse* response) noexcept {
    if(!request||!response||request->size!=sizeof *request||response->size!=sizeof *response||
       request->version!=NIMBY_CONTROL_VERSION)return NIMBY_INVALID_ARGUMENT;
    *response={};response->size=sizeof *response;response->version=NIMBY_CONTROL_VERSION;
    try{
        using namespace nimby::detail::control;
        const auto name=pipeName(pid,id);
        auto connect=[&]{return CreateFileW(name.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,nullptr);};
        auto connection=connect();
        // Wait only BEFORE sending anything. The previous caller may have just
        // acknowledged its response while the server is disconnecting the pipe.
        if(connection==INVALID_HANDLE_VALUE&&GetLastError()==ERROR_PIPE_BUSY&&WaitNamedPipeW(name.c_str(),2000))connection=connect();
        Handle pipe(connection);
        if(pipe.value==INVALID_HANDLE_VALUE)return NIMBY_DATA_UNAVAILABLE;
        ULONG server=0;DWORD mode=PIPE_READMODE_MESSAGE;
        if(!GetNamedPipeServerProcessId(pipe.value,&server)||server!=pid||!SetNamedPipeHandleState(pipe.value,&mode,nullptr,nullptr))return NIMBY_IO_ERROR;
        auto copy=*request;
        if(!transfer(pipe.value,&copy,sizeof copy,true)||!transfer(pipe.value,response,sizeof *response,false))return NIMBY_IO_ERROR;
        uint32_t ack=1;transfer(pipe.value,&ack,sizeof ack,true);
        if(response->size!=sizeof *response||response->version!=NIMBY_CONTROL_VERSION)return NIMBY_INVALID_BINARY;
        return response->result;
    }catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
