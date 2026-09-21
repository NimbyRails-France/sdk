// Native integration experiment. Replaces function inputs, never train speed.
// Commands are bounded to one full train ID and expire on simulated and wall time.
#include "runtime/driving_bridge.h"
#include "engine/binary_identity.h"
#include <MinHook.h>
#include <array>
#include <cmath>
#include <algorithm>
#include <cstring>
namespace {
using Integrate=uintptr_t(__fastcall*)(uintptr_t,uintptr_t,uintptr_t,uintptr_t,uintptr_t,double,double,double,double,double,double,int64_t,uintptr_t);
Integrate original{};
nimby::driving_bridge::Shared* shared{};
SRWLOCK initialization=SRWLOCK_INIT,commandLock=SRWLOCK_INIT;
struct Command {uint64_t train=0,expiry=0;double ceiling=0,brakeUse=0.8;int64_t remaining=0,sampleBudget=0;};
Command command;
template<class T> bool read(uintptr_t address,T& out){SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),&out,sizeof out,&got)&&got==sizeof out;}
uintptr_t __fastcall integrate(uintptr_t result,uintptr_t network,uintptr_t path,uintptr_t presence,uintptr_t dynamics,
 double extraMass,double ceiling,double acceleration,double braking,double distance,double target,int64_t budget,uintptr_t check){
 uint64_t id{};
 if(!shared || shared->state==0 || shared->state==3 || dynamics<8 || presence!=dynamics+0x398 || !read(dynamics-8,id)||id>>48!=5)
  return original(result,network,path,presence,dynamics,extraMass,ceiling,acceleration,braking,distance,target,budget,check);
 // Only the selected train takes the lock. Includes the generation bits.
 if(id!=shared->train)return original(result,network,path,presence,dynamics,extraMass,ceiling,acceleration,braking,distance,target,budget,check);
 AcquireSRWLockExclusive(&commandLock);
 struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&commandLock);}} unlock;
 const auto now=GetTickCount64();
 if(shared->state==1){
  const auto cap=shared->ceilingMps,ratio=shared->brakeUse;
  if(!std::isfinite(cap)||cap<=0||cap>166.667||!std::isfinite(ratio)||ratio<=0||ratio>1||
     shared->durationMs<=0||shared->durationMs>120000||shared->expiresWallMs<=now||shared->expiresWallMs-now>30000){
   InterlockedExchange(&shared->state,3);
  }else{command={id,shared->expiresWallMs,cap,ratio,(shared->durationMs+9)/10,0};InterlockedExchange(&shared->state,2);}
 }
 if(shared->state!=2||command.train!=id||now>=command.expiry||command.remaining<=0||budget<=0||budget>120000){
  InterlockedExchange(&shared->state,3);
  return original(result,network,path,presence,dynamics,extraMass,ceiling,acceleration,braking,distance,target,budget,check);
 }
 float emptyMass{},service{};
 read(dynamics+0x34,emptyMass);read(dynamics+0x24,service);
 if(!std::isfinite(extraMass)||extraMass<0||!std::isfinite(emptyMass)||emptyMass<=0||!std::isfinite(service)||service<=0)
  return original(result,network,path,presence,dynamics,extraMass,ceiling,acceleration,braking,distance,target,budget,check);
 // Keep the native ceiling and any pre-existing braking target more restrictive.
 const double effectiveCeiling=std::min(ceiling,command.ceiling);
 const double effectiveBraking=std::min(braking,service*command.brakeUse*emptyMass/(emptyMass+extraMass));
 nimby::driving_bridge::Sample* sample=nullptr;
 if(command.sampleBudget<=0&&shared->count<1024){
  sample=&shared->samples[shared->count];sample->train=id;sample->wallMs=now;sample->budget=budget;
  sample->ceiling=effectiveCeiling;sample->braking=effectiveBraking;sample->extraMass=extraMass;
  read(presence+0x28,sample->speedBefore);read(presence+0x20,sample->headBefore);
  read(presence+8,sample->trackBefore);read(presence+0x10,sample->fractionBefore);
  command.sampleBudget=10; // Native integration quantum: 0.01 simulated second.
 }
 const auto returned=original(result,network,path,presence,dynamics,extraMass,effectiveCeiling,acceleration,effectiveBraking,distance,target,budget,check);
 int64_t used{};read(result+0x30,used);
 if(used<0||used>budget){InterlockedExchange(&shared->state,3);return returned;}
 command.remaining-=used;command.sampleBudget-=used;
 InterlockedIncrement64(&shared->applied);
 if(sample){sample->used=used;read(presence+0x28,sample->speedAfter);read(presence+0x20,sample->headAfter);
  read(presence+8,sample->trackAfter);read(presence+0x10,sample->fractionAfter);
  InterlockedExchange(&sample->ready,1);InterlockedIncrement(&shared->count);}
 if(command.remaining<=0)InterlockedExchange(&shared->state,3);
 return returned;
}
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
 if(argument)return NIMBY_INVALID_ARGUMENT;
 AcquireSRWLockExclusive(&initialization);struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
 if(shared)return NIMBY_ALREADY_INITIALIZED;
 std::array<wchar_t,32768> executable{};NimbyBinaryInfo identity{};
 if(!GetModuleFileNameW(nullptr,executable.data(),static_cast<DWORD>(executable.size()))||
    nimby::engine::identify(executable.data(),identity)!=NIMBY_OK||!identity.recognized_research_build)return NIMBY_INVALID_BINARY;
 const auto base=reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
 constexpr unsigned char bytes[]={0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x4c,0x24,0x08,0x55,0x56,0x57,0x41,0x55,0x41};
 auto* entry=reinterpret_cast<void*>(base+0x378600);
 if(std::memcmp(entry,bytes,sizeof bytes))return NIMBY_INVALID_BINARY;
 HANDLE mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(nimby::driving_bridge::Shared),nimby::driving_bridge::name(GetCurrentProcessId()).c_str());
 if(!mapping)return NIMBY_IO_ERROR;
 if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);return NIMBY_IO_ERROR;}
 auto* view=static_cast<nimby::driving_bridge::Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,0));
 if(!view){CloseHandle(mapping);return NIMBY_IO_ERROR;}
 *view=nimby::driving_bridge::Shared{};
 bool init=false,created=false,success=false;
 struct Cleanup{HANDLE mapping;void* view;void* entry;bool& init;bool& created;bool& success;
 ~Cleanup(){if(success)return;if(created)MH_RemoveHook(entry);if(init)MH_Uninitialize();UnmapViewOfFile(view);CloseHandle(mapping);shared=nullptr;}
 }cleanup{mapping,view,entry,init,created,success};
 if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
 init=true;
 if(MH_CreateHook(entry,reinterpret_cast<void*>(&integrate),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
 created=true;
 HMODULE self{};
 if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&integrate),&self))return NIMBY_INTERNAL_ERROR;
 shared=view;if(MH_EnableHook(entry)!=MH_OK)return NIMBY_INTERNAL_ERROR;success=true;return NIMBY_OK;
}
