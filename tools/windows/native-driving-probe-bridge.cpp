// Observation-only detour. No speed, dynamics, path or signal is modified.
#include "native-driving-probe.hpp"
#include "engine/binary_identity.h"
#include <MinHook.h>
#include <array>
#include <cstring>
namespace {
using Step=uintptr_t(__fastcall*)(uintptr_t,uintptr_t);
Step original{};
driving_probe::Shared* shared{};
SRWLOCK initialization=SRWLOCK_INIT;
template<class T> bool read(uintptr_t address,T& value){
 SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),&value,sizeof value,&got)&&got==sizeof value;
}
uintptr_t __fastcall observe(uintptr_t context,uintptr_t range){
 // Disabled and full-buffer paths only forward the native call.
 uint64_t id{},motion{},motionRef{},signal{},distanceRef{};int kind{};double offset{},part{};
 read(range+0x20,kind);read(range+0x28,signal);
 read(context+0x18,motionRef);read(motionRef,motion);read(motion,id);
 if(!shared || !InterlockedCompareExchange(&shared->enabled,0,0) ||
    shared->count>=2048 || kind!=6 || !signal || id>>48!=5 || (shared->target&&shared->target!=id))
    return original(context,range);
 const auto index=InterlockedIncrement(&shared->count)-1;
 if(index>=2048)return original(context,range);
 auto& sample=shared->samples[index];
 sample.thread=GetCurrentThreadId();sample.train=id;sample.context=context;sample.motion=motion;
 read(context+0x10,sample.simulation);read(sample.simulation+0x18,sample.simulation);read(sample.simulation+0x28,sample.ticks);
 read(signal,sample.signal);read(context,distanceRef);read(distanceRef,offset);read(range+0x18,part);
 sample.distanceM=offset+part;read(motion+0x3c0,sample.headM);
 read(motion+0x3c8,sample.speedBefore);read(motion+0x3a8,sample.trackBefore);read(motion+0x3b0,sample.fractionBefore);
 read(motion+0x24,sample.dynamics);
 const auto result=original(context,range);
 read(motion+0x3c8,sample.speedAfter);read(motion+0x3a8,sample.trackAfter);read(motion+0x3b0,sample.fractionAfter);
 InterlockedExchange(&sample.ready,1);
 return result;
}
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
 if(argument)return NIMBY_INVALID_ARGUMENT;
 AcquireSRWLockExclusive(&initialization);
 struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
 if(shared)return NIMBY_ALREADY_INITIALIZED;
 std::array<wchar_t,32768> path{};NimbyBinaryInfo info{};
 if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
    nimby::engine::identify(path.data(),info)!=NIMBY_OK||!info.recognized_research_build)return NIMBY_INVALID_BINARY;
 const auto base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
 constexpr unsigned char bytes[]={0x48,0x8b,0xc4,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x53,0x55,0x56,0x57,0x41};
 auto* target=reinterpret_cast<void*>(base+0x449700);
 if(std::memcmp(target,bytes,sizeof bytes))return NIMBY_INVALID_BINARY;
 HANDLE mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(driving_probe::Shared),driving_probe::name(GetCurrentProcessId()).c_str());
 if(!mapping)return NIMBY_IO_ERROR;
 if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);return NIMBY_IO_ERROR;}
 auto* view=static_cast<driving_probe::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,0));
 if(!view){CloseHandle(mapping);return NIMBY_IO_ERROR;}
 *view=driving_probe::Shared{};
 bool initialized=false,created=false,success=false;
 struct Cleanup{HANDLE mapping;void* view;void* target;bool& init;bool& created;bool& success;
 ~Cleanup(){if(success)return;if(created)MH_RemoveHook(target);if(init)MH_Uninitialize();UnmapViewOfFile(view);CloseHandle(mapping);shared=nullptr;}
 } cleanup{mapping,view,target,initialized,created,success};
 if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
 initialized=true;
 if(MH_CreateHook(target,reinterpret_cast<void*>(&observe),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
 created=true;
 HMODULE self{};
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&observe),&self))return NIMBY_INTERNAL_ERROR;
 shared=view;
 if(MH_EnableHook(target)!=MH_OK)return NIMBY_INTERNAL_ERROR;
 success=true;return NIMBY_OK;
}
