#pragma once
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/control.h>
#include <windows.h>
#include <string>
#include <thread>
#include <stdexcept>
#include <nimby/detail/platform/windows/native_library.hpp>
#include <nimby/detail/platform/host.hpp>

namespace nimby::detail::control {
inline std::wstring pipeName(uint32_t pid,const char* id) {
    if(!pid||!id)throw std::invalid_argument("Missing control endpoint");
    std::wstring name=L"\\\\.\\pipe\\NRF.ModControl.v1."+std::to_wstring(pid)+L".";
    size_t n=0;
    for(;id[n]&&n<96;++n){const auto c=id[n];
        if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='.'||c=='_'||c=='-'))
            throw std::invalid_argument("Invalid mod control ID");
        name+=wchar_t(c);
    }
    if(!n||n==96)throw std::invalid_argument("Invalid mod control ID length");
    return name;
}
struct Handle {
    HANDLE value=INVALID_HANDLE_VALUE;
    explicit Handle(HANDLE h):value(h){}
    ~Handle(){if(value&&value!=INVALID_HANDLE_VALUE)CloseHandle(value);}
    Handle(const Handle&)=delete;Handle& operator=(const Handle&)=delete;
};
// Every pending operation is cancelled AND drained before its stack buffer or
// event is destroyed. The stop event also interrupts a client that never writes.
inline bool finish(HANDLE pipe,OVERLAPPED& io,BOOL started,DWORD error,HANDLE stop,DWORD timeout,DWORD& bytes) {
    if(!started&&error!=ERROR_IO_PENDING)return false;
    if(!started){HANDLE events[]{io.hEvent,stop};
        const auto wait=WaitForMultipleObjects(stop?2:1,events,FALSE,timeout);
        if(wait!=WAIT_OBJECT_0){CancelIoEx(pipe,&io);GetOverlappedResult(pipe,&io,&bytes,TRUE);return false;}
    }
    return GetOverlappedResult(pipe,&io,&bytes,FALSE)!=0;
}
inline bool transfer(HANDLE pipe,void* data,DWORD size,bool write,HANDLE stop=nullptr) {
    Handle event(CreateEventW(nullptr,TRUE,FALSE,nullptr));if(!event.value)return false;
    OVERLAPPED io{};io.hEvent=event.value;DWORD bytes=0;
    const auto started=write?WriteFile(pipe,data,size,nullptr,&io):ReadFile(pipe,data,size,nullptr,&io);
    const auto error=GetLastError();
    return finish(pipe,io,started,error,stop,2000,bytes)&&bytes==size;
}
class Server {
    HANDLE stop_=nullptr;
    std::thread thread_;
public:
    void start(const char* id,NimbyControlHandler handler) {
        if(thread_.joinable()||!handler)throw std::invalid_argument("Control server already started or missing handler");
        const auto target=native::modHostTarget();
        const auto name=pipeName(target?target:GetCurrentProcessId(),id);
        // Single owning endpoint: a second instance must fail instead of
        // accepting requests unpredictably on the same mod name.
        HANDLE pipe=CreateNamedPipeW(name.c_str(),PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
            PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,4096,4096,0,nullptr);
        if(pipe==INVALID_HANDLE_VALUE)throw std::runtime_error("Cannot create mod control endpoint");
        stop_=CreateEventW(nullptr,TRUE,FALSE,nullptr);
        if(!stop_){CloseHandle(pipe);throw std::runtime_error("Cannot create control stop event");}
        try{thread_=std::thread([this,pipe,handler]{
            Handle connection(pipe),event(CreateEventW(nullptr,TRUE,FALSE,nullptr));
            if(!event.value)return;
            while(WaitForSingleObject(stop_,0)==WAIT_TIMEOUT){
                ResetEvent(event.value);OVERLAPPED io{};io.hEvent=event.value;DWORD bytes=0;
                const auto started=ConnectNamedPipe(pipe,&io);const auto error=GetLastError();
                const bool connected=(!started&&error==ERROR_PIPE_CONNECTED)||finish(pipe,io,started,error,stop_,INFINITE,bytes);
                if(!connected){DisconnectNamedPipe(pipe);continue;}
                NimbyControlRequest request{};NimbyControlResponse response{};
                response.size=sizeof response;response.version=NIMBY_CONTROL_VERSION;
                if(transfer(pipe,&request,sizeof request,false,stop_)){
                    try{platform::ModWork work;response.result=handler(&request,&response);}catch(...){ nimby::detail::diagnostics::exception("mods", __func__); response.result=NIMBY_INTERNAL_ERROR;}
                    if(transfer(pipe,&response,sizeof response,true,stop_)){
                        // DisconnectNamedPipe can discard unread messages.
                        uint32_t ack=0;transfer(pipe,&ack,sizeof ack,false,stop_);
                    }
                }
                DisconnectNamedPipe(pipe);
            }
        });}catch(...){ nimby::detail::diagnostics::exception("mods", __func__); CloseHandle(pipe);CloseHandle(stop_);stop_=nullptr;throw;}
    }
    void stop(){if(stop_)SetEvent(stop_);if(thread_.joinable())thread_.join();if(stop_)CloseHandle(stop_);stop_=nullptr;}
};
}
