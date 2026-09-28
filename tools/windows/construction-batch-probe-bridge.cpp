// Qualification of native delta grouping, restricted to an explicit track and
// the next manual signal placement. It does not issue commands from a new thread.
#include "construction-batch-protocol.hpp"
#include "engine/binary_identity.h"
#include "engine/detail/memory_reader.h"
#include <MinHook.h>
#include <array>
#include <cmath>
#include <cstring>

namespace {
using namespace nimby::construction_batch_probe;
using Execute=uint64_t(*)(uint64_t,uint64_t,uint64_t);
Execute original{};
uint64_t base{};
Shared* shared{};
HANDLE mapping{};
bool enabled{};
SRWLOCK initialization=SRWLOCK_INIT;
bool read(void*,uint64_t address,void* out,size_t size) {
    SIZE_T copied{};
    return address&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&copied)&&copied==size;
}
template<class T> T value(uint64_t address) {T out{};read(nullptr,address,&out,sizeof out);return out;}

uint64_t execute(uint64_t command,uint64_t output,uint64_t context) {
    if(InterlockedCompareExchange(&shared->state,armed,armed)!=armed||
       GetTickCount64()>=shared->expires||value<uint64_t>(command+0x60)!=shared->track)
        return original(command,output,context);
    if(InterlockedCompareExchange(&shared->state,executing,armed)!=armed)
        return original(command,output,context);

    // Copy external input once, before touching native state. The client cannot
    // rearm this mapping while executing. Checked reads fail back to the normal
    // single manual placement; they do not suppress the user's command.
    const auto count=shared->count;
    std::array<double,capacity> fractions{};
    std::memcpy(fractions.data(),shared->fractions,sizeof fractions);
    const auto track=shared->track;
    const auto db=value<uint64_t>(context+0x428);
    const auto originalFraction=value<double>(command+0x68);
    bool valid=count>=2&&count<=capacity&&std::isfinite(originalFraction);
    for(uint32_t i=0;valid&&i<count;++i){
        valid=std::isfinite(fractions[i])&&fractions[i]>0&&fractions[i]<1;
        for(uint32_t j=0;j<i;++j)valid=valid&&std::abs(fractions[i]-fractions[j])>1e-6;
    }
    // First placement must be the actual manual click (allow only pixel
    // precision); subsequent positions are explicit diagnostic inputs.
    valid=valid&&std::abs(fractions[0]-originalFraction)<.01;
    try { if(valid){
        bool found=false;
        valid=nimby::engine::memory::collect(read,nullptr,db,1,0x4e8,[&](const unsigned char* bytes,uint64_t){
            if(nimby::engine::memory::field<uint64_t>(bytes,0)==track){
                found=true;
                // Refuse any attachment/branch on this experimental track.
                if(nimby::engine::memory::field<uint64_t>(bytes,0x3f0)||
                   nimby::engine::memory::field<uint64_t>(bytes,0x408)!=nimby::engine::memory::field<uint64_t>(bytes,0x410))return false;
            }
            return true;
        })&&found;
    }
    if(valid)valid=nimby::engine::memory::collect(read,nullptr,db+0x380,8,0xc8,[&](const unsigned char* bytes,uint64_t){
        if(nimby::engine::memory::field<uint64_t>(bytes,0x40)!=track)return true;
        const auto fraction=nimby::engine::memory::field<double>(bytes,0x48);
        for(uint32_t i=0;i<count;++i)if(std::abs(fraction-fractions[i])<.01)return false;
        return true;
    }); } catch(...) {valid=false;}
    if(!valid){
        shared->error=1;
        const auto result=original(command,output,context);
        InterlockedExchange(&shared->state,complete);return result;
    }
    shared->thread=GetCurrentThreadId();shared->created=0;
    uint64_t result{};
    for(uint32_t i=0;i<count;++i){
        if(i){
            // Destroy the previous initialized variant using the game's own
            // destructor before reusing the caller's result storage.
            reinterpret_cast<void(*)(uint64_t)>(base+0x2f3da0)(output);
        }
        std::memcpy(reinterpret_cast<void*>(command+0x68),&fractions[i],sizeof(double));
        result=original(command,output,context);
        // CreateSignal returns a single ID in its ordered set, even on failure
        // (then ID=0). Never infer success from a non-null return pointer.
        const auto node=value<uint64_t>(result+0xe8);
        const auto id=value<uint64_t>(node+0x20);
        if(value<uint64_t>(result+0x100)!=1||(id>>48)!=8){shared->error=2;break;}
        shared->ids[shared->created++]=id;
    }
    std::memcpy(reinterpret_cast<void*>(command+0x68),&originalFraction,sizeof(double));
    // The native dispatcher now finalizes ONE delta containing every creation.
    // Only the last signal is selected by the normal editor result, intentionally.
    InterlockedExchange(&shared->state,complete);
    return result;
}
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    AcquireSRWLockExclusive(&initialization);
    struct Unlock {~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    // Both observers patch CreateSignal. Require a fresh process rather than
    // chaining unknown trampolines from a previous qualification session.
    if(GetModuleHandleW(L"NimbyConstructionProbe-v1.dll"))return NIMBY_HOOKS_UNAVAILABLE;
    std::array<wchar_t,32768> path{};NimbyBinaryInfo binary{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
       nimby::engine::identify(path.data(),binary)!=NIMBY_OK||!binary.recognized_research_build)return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr));
    constexpr std::array<unsigned char,16> expected{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7c,0x24,0x20,0x48};
    std::array<unsigned char,16> actual{};
    if(!read(nullptr,base+0x2fda60,actual.data(),actual.size())||actual!=expected)return NIMBY_INVALID_BINARY;
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    shared->version=protocol;shared->size=sizeof(Shared);
    bool initialized=false,created=false;
    struct Cleanup {bool& initialized;bool& created;~Cleanup(){if(enabled)return;
        if(created)MH_RemoveHook(reinterpret_cast<void*>(base+0x2fda60));
        if(initialized)MH_Uninitialize();
        UnmapViewOfFile(shared);shared=nullptr;CloseHandle(mapping);
    }} cleanup{initialized,created};
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    initialized=true;
    if(MH_CreateHook(reinterpret_cast<void*>(base+0x2fda60),reinterpret_cast<void*>(&execute),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    created=true;
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&execute),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_EnableHook(reinterpret_cast<void*>(base+0x2fda60))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;return NIMBY_OK;
}
