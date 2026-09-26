// Read-only probe. Explicit PID and full signal ID, no injection or native calls.
#include "engine/signal_extensions.h"
#include "engine/binary_identity.h"
#include <windows.h>
#include <tlhelp32.h>
#include <charconv>
#include <cstdio>
#include <string_view>
struct Process { HANDLE handle{};~Process(){if(handle)CloseHandle(handle);} };
static bool read(void* context,uint64_t address,void* out,size_t size) {
    SIZE_T received{};
    return address>=0x10000 && address<0x7fffffff0000ULL && size<0x7fffffff0000ULL-address &&
        ReadProcessMemory(static_cast<Process*>(context)->handle,reinterpret_cast<void*>(address),out,size,&received)&&received==size;
}
template<class T> static T field(const void* p,size_t offset){T value{};std::memcpy(&value,static_cast<const unsigned char*>(p)+offset,sizeof value);return value;}
int main(int argc,char** argv) {
    uint64_t args[2]{};
    if(argc!=3){std::puts("Usage: nimby_signal_extension_probe PID SIGNAL_ID");return 1;}
    for(int i=0;i<2;++i){std::string_view input=argv[i+1];auto [end,error]=std::from_chars(input.data(),input.data()+input.size(),args[i]);if(error!=std::errc{}||end!=input.data()+input.size())return 1;}
    if(!args[0]||args[0]>MAXDWORD||args[1]>>48!=8)return 1;
    Process process{OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,static_cast<DWORD>(args[0]))};if(!process.handle)return 2;
    wchar_t path[32768]{};DWORD length=32768;NimbyBinaryInfo binary{};
    if(!QueryFullProcessImageNameW(process.handle,0,path,&length)||nimby::engine::identify(path,binary)!=NIMBY_OK||!binary.recognized_research_build)return 3;
    HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,static_cast<DWORD>(args[0]));if(modules==INVALID_HANDLE_VALUE)return 4;
    MODULEENTRY32W module{};module.dwSize=sizeof module;bool found=Module32FirstW(modules,&module)!=0;CloseHandle(modules);if(!found)return 4;
    nimby::engine::LiveState state{},after{};
    if(!nimby::engine::resolve_live_state(read,&process,reinterpret_cast<uint64_t>(module.modBaseAddr),true,state))return 5;
    std::array<unsigned char,48> header{},headerAfter{};
    if(!read(&process,state.database+0x380,header.data(),header.size()))return 6;
    const auto shift=field<uint32_t>(header.data(),4),slots=field<uint32_t>(header.data(),8),mask=field<uint32_t>(header.data(),16);
    const auto begin=field<uint64_t>(header.data(),24),end=field<uint64_t>(header.data(),32),cap=field<uint64_t>(header.data(),40);
    if(!shift||shift>16||slots!=(1u<<shift)||mask!=slots-1||end<begin||cap<end||cap-begin>8192||(end-begin)%8)return 6;
    const auto index=(args[1]>>16)&0xffffffffULL,blockIndex=index>>shift;
    if(blockIndex>=(end-begin)/8)return 7;
    uint64_t block{},blockAfter{};if(!read(&process,begin+blockIndex*8,&block,8)||block>=0x7fffffff0000ULL-slots*0xc8ULL)return 7;
    std::vector<nimby::engine::SignalExtension> extensions;
    if(!nimby::engine::read_signal_extensions(read,&process,block+(index&mask)*0xc8,args[1],true,extensions))return 8;
    if(!read(&process,state.database+0x380,headerAfter.data(),headerAfter.size())||header!=headerAfter||
       !read(&process,begin+blockIndex*8,&blockAfter,8)||block!=blockAfter||
       !nimby::engine::resolve_live_state(read,&process,state.module_base,true,after)||state!=after)return 9;
    std::printf("signal=%llu extensions=%zu\n",static_cast<unsigned long long>(args[1]),extensions.size());
    for(const auto& extension:extensions){
        std::printf("script=%llu type=%llu values=%zu metadata=%zu\n",static_cast<unsigned long long>(extension.script),static_cast<unsigned long long>(extension.type),extension.values.size(),extension.metadata.size());
        for(size_t i=0;i<extension.values.size();++i){const auto boolean=extension.values[i].boolean();std::printf("  field_index=%zu variant=%u boolean=%s\n",i,extension.values[i].bytes[8],boolean?(*boolean?"true":"false"):"unknown");}
    }
}
