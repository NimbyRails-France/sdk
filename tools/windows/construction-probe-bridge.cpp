#include <platform/windows/bridge_installation.h>
// Observes commands issued by the normal editor. Never creates a command,
// changes a payload or invokes an executor independently of its original call.
#include "construction-probe.hpp"
#include "engine/binary_identity.h"
#include <MinHook.h>
#include <array>
#include <cstring>

namespace {
using namespace nimby::construction_probe;
using Enqueue=uint64_t(*)(uint64_t);
using Execute=uint64_t(*)(uint64_t,uint64_t,uint64_t);
using History=void(*)(uint64_t,uint64_t);
Enqueue originalEnqueue{};Execute originalCreate{},originalUndo{};History originalHistory{};
Shared* shared{};HANDLE mapping{};uint64_t base{};bool enabled{};
SRWLOCK initialization=SRWLOCK_INIT,writing=SRWLOCK_INIT;
bool read(uint64_t at,void* out,size_t bytes) noexcept {
    SIZE_T got{};
    return at&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(at),out,bytes,&got)&&got==bytes;
}
void record(Kind kind,uint64_t command,uint64_t context,uint64_t result=0,uint64_t delta=0) noexcept {
    if(GetTickCount64()>=static_cast<uint64_t>(InterlockedCompareExchange64(&shared->expires,0,0)))return;
    Event event{};event.tick=GetTickCount64();event.kind=kind;event.thread=GetCurrentThreadId();
    event.command=command;event.context=context;event.result=result;
    // The editor fills the payload only AFTER enqueue returns. Only its
    // initialized command envelope may be observed at that interception point.
    const size_t commandBytes=kind==Enqueued?0x20:sizeof event.header;
    if(command&&read(command,event.header,commandBytes))event.valid|=1;
    // Execute returns a temporary variant, not the finalized 0x818-byte
    // dispatcher record. Observe its initialized discriminant only: unused
    // variant storage may contain unrelated stack bytes. Keep other words zero.
    uint8_t resultTag{};
    if(result&&read(result+0x1f0,&resultTag,sizeof resultTag)){
        event.resultWords[0x1f0/8]=resultTag;event.valid|=2;
    }
    if(delta&&read(delta,event.deltaWords,sizeof event.deltaWords))event.valid|=4;
    if(kind==CreateBefore||kind==CreateAfter||kind==UndoBefore||kind==UndoAfter) {
        if(read(context+0x428,&event.db,sizeof event.db))event.valid|=8;
    }
    if(kind==HistoryBefore||kind==HistoryAfter) {
        if(read(context+0x10,&event.historyCount,sizeof event.historyCount))event.valid|=16;
    }
    void* stack[16]{};
    event.stackCount=CaptureStackBackTrace(1,16,stack,nullptr);
    for(uint32_t i=0;i<event.stackCount;++i)event.stack[i]=reinterpret_cast<uint64_t>(stack[i]);
    AcquireSRWLockExclusive(&writing);
    InterlockedIncrement(&shared->sequence);
    shared->events[shared->count%capacity]=event;++shared->count;
    InterlockedIncrement(&shared->sequence);
    ReleaseSRWLockExclusive(&writing);
}
uint64_t enqueue(uint64_t context) {
    const auto command=originalEnqueue(context);record(Enqueued,command,context);return command;
}
uint64_t create(uint64_t command,uint64_t result,uint64_t context) {
    record(CreateBefore,command,context);
    const auto returned=originalCreate(command,result,context);
    // The dispatcher constructs the delta later. HistoryBefore observes the
    // completed delta on the UI thread; returned+0x1f8 is NOT a delta here.
    record(CreateAfter,command,context,returned);return returned;
}
uint64_t undo(uint64_t command,uint64_t result,uint64_t context) {
    record(UndoBefore,command,context,0,command+0x20);
    const auto returned=originalUndo(command,result,context);
    record(UndoAfter,command,context,returned);return returned;
}
void history(uint64_t list,uint64_t delta) {
    record(HistoryBefore,0,list,0,delta);
    originalHistory(list,delta); // Consumes the temporary delta; do not read it afterwards.
    record(HistoryAfter,0,list);
}
struct Hook {uint64_t rva;void* callback;void** original;std::array<unsigned char,16> expected;};
}
extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    nimby::platform::windows::BridgeInstallation installation;
    if(!installation)return installation.status();
    AcquireSRWLockExclusive(&initialization);
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    std::array<wchar_t,32768> path{};NimbyBinaryInfo binary{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
       nimby::engine::identify(path.data(),binary)!=NIMBY_OK||!binary.recognized_research_build)return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr));
    const Hook hooks[]{
        {0x7bc7e0,reinterpret_cast<void*>(&enqueue),reinterpret_cast<void**>(&originalEnqueue),{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x6c,0x24,0x18,0x56,0x57,0x41,0x54,0x41,0x56}},
        {0x2fda60,reinterpret_cast<void*>(&create),reinterpret_cast<void**>(&originalCreate),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x7c,0x24,0x20,0x48}},
        {0x2f3ab0,reinterpret_cast<void*>(&undo),reinterpret_cast<void**>(&originalUndo),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x7c,0x24,0x10,0x55,0x48,0x8d,0xac,0x24,0x10}},
        {0x762a70,reinterpret_cast<void*>(&history),reinterpret_cast<void**>(&originalHistory),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x57}},
    };
    for(const auto& hook:hooks){
        std::array<unsigned char,16> bytes{};
        if(!read(base+hook.rva,bytes.data(),bytes.size())||bytes!=hook.expected)return NIMBY_INVALID_BINARY;
    }
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared){CloseHandle(mapping);mapping=nullptr;return NIMBY_IO_ERROR;}
    // The mapping is zero-filled by Windows; avoid a large temporary on the game stack.
    shared->version=1;shared->size=sizeof(Shared);shared->base=base;
    bool initialized=false;size_t created=0;
    struct Cleanup{bool& initialized;size_t& created;const Hook* hooks;~Cleanup(){if(enabled)return;
        for(size_t i=0;i<created;++i)MH_RemoveHook(reinterpret_cast<void*>(base+hooks[i].rva));
        if(initialized)MH_Uninitialize();
        UnmapViewOfFile(shared);shared=nullptr;CloseHandle(mapping);mapping=nullptr;
    }} cleanup{initialized,created,hooks};
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    initialized=true;
    for(const auto& hook:hooks){
        if(MH_CreateHook(reinterpret_cast<void*>(base+hook.rva),hook.callback,hook.original)!=MH_OK)return NIMBY_INTERNAL_ERROR;
        ++created;
        if(MH_QueueEnableHook(reinterpret_cast<void*>(base+hook.rva))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    }
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&record),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_ApplyQueued()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;return NIMBY_OK;
}
