// Read-only comparison of texture-command membership validation costs.
#include "engine/binary_identity.h"
#include "engine/network.h"
#include <windows.h>
#include <tlhelp32.h>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <string_view>

struct Process {
    HANDLE handle{};
    uint64_t reads{},bytes{};
    ~Process(){if(handle)CloseHandle(handle);}
};
static bool read(void* context,uint64_t address,void* out,size_t size){
    auto& process=*static_cast<Process*>(context);
    ++process.reads;process.bytes+=size;
    SIZE_T received{};
    return ReadProcessMemory(process.handle,reinterpret_cast<void*>(address),out,size,&received)&&received==size;
}
template<class T> bool parse(const char* text,T& value){
    const std::string_view input=text;
    const auto [end,error]=std::from_chars(input.data(),input.data()+input.size(),value);
    return error==std::errc{}&&end==input.data()+input.size()&&value;
}
int main(int argc,char** argv){try{
    uint32_t pid{};uint64_t signal{};
    if(argc!=3||!parse(argv[1],pid)||!parse(argv[2],signal)){
        std::puts("Usage: nimby_signal_membership_probe PID FULL_SIGNAL_ID");return 1;
    }
    Process process{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid)};
    if(!process.handle)return 2;
    wchar_t path[32768]{};DWORD length=32768;NimbyBinaryInfo binary{};
    if(!QueryFullProcessImageNameW(process.handle,0,path,&length)||
       nimby::engine::identify(path,binary)!=NIMBY_OK||!binary.recognized_research_build)return 3;
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
    if(modules==INVALID_HANDLE_VALUE)return 4;
    MODULEENTRY32W module{};module.dwSize=sizeof module;
    const bool moduleFound=Module32FirstW(modules,&module)!=0;CloseHandle(modules);
    if(!moduleFound)return 4;
    nimby::engine::LiveState state{};
    if(!nimby::engine::resolve_live_state(read,&process,reinterpret_cast<uint64_t>(module.modBaseAddr),true,state))return 5;
    auto measure=[&](const char* label,auto operation){
        process.reads=0;process.bytes=0;
        const auto start=std::chrono::steady_clock::now();
        const bool ok=operation();
        const auto ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
        std::printf("%s ok=%d elapsed_ms=%.3f reads=%llu bytes=%llu\n",label,ok,ms,
            static_cast<unsigned long long>(process.reads),static_cast<unsigned long long>(process.bytes));
        return ok;
    };
    const bool targeted=measure("targeted",[&]{bool found=false;
        return nimby::engine::read_signal_membership(read,&process,state,true,signal,found)&&found;});
    const bool whole=measure("whole_network",[&]{nimby::engine::Network network;
        if(!nimby::engine::read_network(read,&process,state,true,network))return false;
        for(const auto& row:network.signals)if(row.id==signal)return true;
        return false;});
    return targeted&&whole?0:6;
}catch(...){return 7;}}
