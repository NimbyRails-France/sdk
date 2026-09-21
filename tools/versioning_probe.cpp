// Read-only research probe. The candidate DB offset is NOT a production API.
#include "engine/binary_identity.h"
#include "engine/versioning.h"
#include <windows.h>
#include <tlhelp32.h>
#include <array>
#include <charconv>
#include <cstdio>
#include <string_view>

struct Process { HANDLE handle{};~Process(){if(handle)CloseHandle(handle);} };
static bool read(void* context,uint64_t address,void* out,size_t size){
    SIZE_T received{};
    return address>=0x10000 && address<0x7fffffff0000ULL && size<0x7fffffff0000ULL-address &&
        ReadProcessMemory(static_cast<Process*>(context)->handle,reinterpret_cast<void*>(address),out,size,&received)&&received==size;
}
int main(int argc,char** argv){
    if(argc!=2){std::puts("Usage: nimby_versioning_probe PID");return 1;}
    uint32_t pid{};const std::string_view input=argv[1];
    const auto [end,error]=std::from_chars(input.data(),input.data()+input.size(),pid);
    if(error!=std::errc{}||end!=input.data()+input.size()||!pid)return 1;
    Process process{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid)};
    if(!process.handle)return 2;
    wchar_t path[32768]{};DWORD length=32768;NimbyBinaryInfo binary{};
    if(!QueryFullProcessImageNameW(process.handle,0,path,&length)||
       nimby::engine::identify(path,binary)!=NIMBY_OK||!binary.recognized_research_build)return 3;
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
    if(modules==INVALID_HANDLE_VALUE)return 4;
    MODULEENTRY32W module{};module.dwSize=sizeof module;
    const bool found=Module32FirstW(modules,&module)!=0;CloseHandle(modules);if(!found)return 4;
    nimby::engine::VersioningObservation observation;
    if(!nimby::engine::read_versioning_observation(read,&process,reinterpret_cast<uint64_t>(module.modBaseAddr),true,observation))return 6;
    std::printf("candidate_db_offset=0xa48 value=");
    for(auto byte:observation.value)std::printf("%02x",byte);
    std::printf(" history_count=%zu\n",observation.history.size());
}
