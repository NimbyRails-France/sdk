// Experimental single-player construction endpoint for the fingerprinted game.
// Native allocation, deep copies, commands, result finalization and undo remain
// owned by NIMBY Rails. This module never allocates a game object in an SDK pool.
#include "platform/windows/runtime/construction_bridge.h"
#include "engine/construction.h"
#include "engine/binary_identity.h"
#include "platform/windows/signal_position.h"
#include <MinHook.h>
#include <array>
#include <atomic>
#include <cstring>
#include <nimby/detail/signal_ui_bridge.h>

namespace {
using namespace nimby::construction_bridge;
using Ui=void(*)(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t);
using Dispatch=void(*)(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t);
using Execute=uint64_t(*)(uint64_t,uint64_t,uint64_t);
using HistoryPush=void(*)(uint64_t,uint64_t);
HistoryPush originalHistoryPush{};
using MoveDelta=uint64_t(*)(uint64_t,uint64_t);
MoveDelta originalMoveDelta{};
thread_local uint64_t expectedDelta{},capturedHistory{};
Ui originalUi{};Dispatch originalDispatch{};Execute originalCreate{},originalUndo{};
uint64_t base{};Shared* shared{};HANDLE mapping{};bool enabled{};
SRWLOCK initialization=SRWLOCK_INIT,operationLock=SRWLOCK_INIT;
std::atomic<uint64_t> revision{1};
thread_local bool authorizedExecution=false;
struct Guard {Guard(){AcquireSRWLockExclusive(&operationLock);}~Guard(){ReleaseSRWLockExclusive(&operationLock);}};
struct Identity {uint64_t owner{},stamp{},sequence{};bool operator==(const Identity&) const=default;};
struct Operation {
    uint64_t token{},root{},simulation{},editor{},context{},preparedRevision{},command{},history{};
    Identity identity{};
    NimbyConstructionRequest request{};
    NimbyConstructionResult result{};
    bool waiting{},undo{},executed{},authorized{},singleCommand{};
} operation;
uint64_t nextToken=1;

bool read(uint64_t address,void* out,size_t size) {
    SIZE_T copied{};
    return address&&ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),out,size,&copied)&&copied==size;
}
template<class T> T get(uint64_t address){T out{};read(address,&out,sizeof out);return out;}
template<class T> void put(uint64_t address,const T& value){std::memcpy(reinterpret_cast<void*>(address),&value,sizeof value);}
template<class Fn> Fn native(uint64_t rva){return reinterpret_cast<Fn>(base+rva);}
Identity commandIdentity(uint64_t command){return {get<uint64_t>(command+8),get<uint64_t>(command+0x10),get<uint64_t>(command+0x18)};}
Identity resultIdentity(uint64_t result){return {get<uint64_t>(result+0x7f8),get<uint64_t>(result+0x800),get<uint64_t>(result+0x810)};}
uint64_t root(){return get<uint64_t>(base+0xb81998);}
bool sessionMatches(){const auto r=root();return r&&r==operation.root&&get<uint64_t>(r+0x680)==operation.simulation;}

// Checked identity lookup, without mutable native lookup/cache helpers. Reading
// a pool index alone is insufficient: the full generation-bearing ID must match.
uint64_t object(uint64_t pool,uint64_t id,size_t stride){
    const auto shift=get<uint32_t>(pool+4),size=get<uint32_t>(pool+8);
    if(!shift||shift>16||size!=(1u<<shift)||get<uint32_t>(pool+16)!=size-1)return 0;
    const auto begin=get<uint64_t>(pool+24),end=get<uint64_t>(pool+32),index=(id>>16)&0xffffffffULL;
    if(end<begin||(end-begin)%8||end-begin>8192||(index>>shift)>=(end-begin)/8)return 0;
    const auto block=get<uint64_t>(begin+(index>>shift)*8),address=block+(index&(size-1))*stride;
    return block&&get<uint64_t>(address)==id?address:0;
}
void finish(uint32_t state,uint32_t reason=0){
    operation.result.state=state;operation.result.reason=reason;
    shared->result=operation.result;
    InterlockedExchange(&shared->state,complete);
}
void reject(uint32_t reason){
    shared->result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_REJECTED,0,shared->request.token,reason,0,{}};
    InterlockedExchange(&shared->state,complete);
}

bool validTargets(uint64_t db,const NimbyConstructionRequest& request){
    if(!object(db+0x380,request.source_signal,0xc8))return false;
    for(uint32_t i=0;i<request.count;++i){
        const auto& p=request.positions[i];
        if(!object(db,p.track_id,0x4e8))return false;
    }
    return true;
}

void position(uint64_t command,uint64_t db,const NimbyConstructionPosition& p){
    const auto track=object(db,p.track_id,0x4e8);
    nimby::platform::windows::writeSignalPosition(reinterpret_cast<void*>(command+0x60),
        p.track_id,p.fraction,p.direction,get<uint8_t>(track+0x2c));
}

uint64_t create(uint64_t command,uint64_t output,uint64_t context){
    Guard guard;
    if(!operation.waiting||operation.undo||operation.command!=command)return originalCreate(command,output,context);
    operation.executed=true;
    const auto db=get<uint64_t>(context+0x428);
    // A ticket is consumed on the simulation thread, before the first mutation.
    // Invalid commands become the game's own missing-track no-op result.
    if(!authorizedExecution||!sessionMatches()||!validTargets(db,operation.request)){
        const auto previous=get<uint64_t>(command+0x60);put<uint64_t>(command+0x60,0);
        const auto result=originalCreate(command,output,context);put(command+0x60,previous);
        operation.result.reason=2;return result;
    }
    operation.authorized=true;
    // Freeze NRF settings before the first native mutation. An older loaded UI
    // bridge must refuse this operation instead of silently dropping settings.
    uint64_t settingsToken{};
    NimbyUiSettingsCopyFinishV1 finishSettings{};
    if(const auto bridge=GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll")){
        const auto begin=reinterpret_cast<NimbyUiSettingsCopyBeginV1>(GetProcAddress(bridge,"NimbyUi_SettingsCopyBeginV1"));
        finishSettings=reinterpret_cast<NimbyUiSettingsCopyFinishV1>(GetProcAddress(bridge,"NimbyUi_SettingsCopyFinishV1"));
        if(!begin||!finishSettings||begin(operation.request.source_signal,&settingsToken)!=NIMBY_OK){
            const auto previous=get<uint64_t>(command+0x60);put<uint64_t>(command+0x60,0);
            const auto result=originalCreate(command,output,context);put(command+0x60,previous);
            operation.authorized=false;operation.result.reason=7;return result;
        }
    }
    const auto first=operation.request.positions[0];
    uint64_t result{};
    for(uint32_t i=0;i<operation.request.count;++i){
        if(i)native<void(*)(uint64_t)>(0x2f3da0)(output);
        const auto& p=operation.request.positions[i];
        position(command,db,p);
        result=originalCreate(command,output,context);
        const auto node=get<uint64_t>(result+0xe8),id=get<uint64_t>(node+0x20);
        if(get<uint64_t>(result+0x100)!=1||(id>>48)!=8){operation.result.reason=3;break;}
        operation.result.ids[operation.result.count++]=id;
    }
    position(command,db,first);
    if(finishSettings&&finishSettings(settingsToken,operation.result.ids,operation.result.count)!=NIMBY_OK)
        operation.result.reason=7;
    return result; // Dispatcher finalizes one cumulative delta after this return.
}

uint64_t undo(uint64_t command,uint64_t output,uint64_t context){
    Guard guard;
    if(!operation.waiting||!operation.undo||operation.command!=command)return originalUndo(command,output,context);
    operation.executed=true;
    if(!authorizedExecution||!sessionMatches()){
        // Keep the original history entry when rejecting. Only this newly owned
        // command's copy is cleared, through the matching native destructor/init.
        native<void(*)(uint64_t)>(0x320ec0)(command+0x20);
        native<void(*)(uint64_t)>(0x319110)(command+0x20);
        operation.result.reason=2;
    }else operation.authorized=true;
    return originalUndo(command,output,context);
}

void dispatch(uint64_t context,uint64_t simulation,uint64_t owner,uint64_t commands,uint64_t results){
    const auto begin=get<uint64_t>(commands),end=get<uint64_t>(commands+8);
    const auto previous=revision.load();
    authorizedExecution=false;
    {
        Guard guard;
        if(operation.waiting&&begin&&end>=begin&&(end-begin)%8==0&&get<uint64_t>(begin)==operation.command){
            authorizedExecution=previous==operation.preparedRevision;
            operation.singleCommand=end-begin==8;
        }
    }
    if(begin!=end)revision.fetch_add(1);
    originalDispatch(context,simulation,owner,commands,results);
    authorizedExecution=false;
}

uint64_t received(uint64_t context){
    const auto begin=get<uint64_t>(context+0x208),end=get<uint64_t>(context+0x210);
    if(end<begin||(end-begin)%0x818||end-begin>0x818*4096)return false;
    for(auto at=begin;at<end;at+=0x818)if(resultIdentity(at)==operation.identity)return at;
    return 0;
}

void historyPush(uint64_t list,uint64_t delta){
    originalHistoryPush(list,delta);
    // Match the exact result delta, not merely the most recent history entry.
    if(expectedDelta&&delta==expectedDelta)capturedHistory=get<uint64_t>(list+8);
}
uint64_t moveDelta(uint64_t destination,uint64_t source){
    const auto result=originalMoveDelta(destination,source);
    if(expectedDelta&&source==expectedDelta)expectedDelta=result;
    return result;
}

void ui(uint64_t editor,uint64_t layout,uint64_t context,uint64_t view,uint64_t input){
    uint64_t acknowledged=0;
    {Guard guard;
        if(operation.waiting&&operation.editor==editor&&operation.context==context)acknowledged=received(context);
        if(acknowledged&&operation.undo&&operation.executed&&operation.authorized&&
           get<uint64_t>(editor+0xd78)==operation.history){
            // Native UI pops history when submitting undo. Our submission keeps
            // it until execution is confirmed, then removes it before remapping.
            native<void(*)(uint64_t,uint64_t)>(0x7c16d0)(editor+0xd70,operation.history);
            operation.history=0;
        }
    }
    expectedDelta=acknowledged?acknowledged+0x1f8:0;capturedHistory=0;
    originalUi(editor,layout,context,view,input);
    expectedDelta=0;
    Guard guard;
    if(acknowledged){
        operation.waiting=false;
        if(!operation.executed||!operation.authorized){reject(operation.result.reason?operation.result.reason:2);return;}
        if(operation.undo){
            operation.result.can_undo=0;operation.history=0;
            finish(NIMBY_CONSTRUCTION_UNDONE);return;
        }
        operation.preparedRevision=revision.load();
        operation.history=operation.singleCommand&&capturedHistory==get<uint64_t>(editor+0xd78)?capturedHistory:0;
        if(operation.history==editor+0xd70)operation.history=0;
        operation.result.can_undo=operation.history!=0;
        finish(operation.result.count==operation.request.count&&!operation.result.reason?NIMBY_CONSTRUCTION_APPLIED:NIMBY_CONSTRUCTION_PARTIAL,operation.result.reason);
        return;
    }
    if(InterlockedCompareExchange(&shared->state,executing,pending)!=pending)return;
    const auto request=shared->request;
    if(GetTickCount64()>=shared->expires||!nimby::engine::construction::valid(request)){
        shared->result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_REJECTED,0,request.token,1,0,{}};
        InterlockedExchange(&shared->state,complete);return;
    }
    const auto tn=get<uint64_t>(context+0x258),db=get<uint64_t>(tn+0x428),currentRoot=root();
    const bool noPending=get<uint64_t>(context+0x238)==get<uint64_t>(context+0x240)&&
        !native<uint8_t(*)(uint64_t,int)>(0x73ec90)(context,3);
    if(!currentRoot||!db||!noPending){
        shared->result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_REJECTED,0,request.token,4,0,{}};
        InterlockedExchange(&shared->state,complete);return;
    }
    if(request.action==NIMBY_CONSTRUCTION_PREPARE){
        operation={};operation.token=nextToken++;operation.root=currentRoot;
        operation.simulation=get<uint64_t>(currentRoot+0x680);
        operation.editor=editor;operation.context=context;operation.preparedRevision=revision.load();
        operation.request=request;
        operation.result={sizeof(NimbyConstructionResult),1,NIMBY_CONSTRUCTION_READY,0,operation.token,0,0,{}};
        if(!object(db+0x380,request.source_signal,0xc8)){reject(5);return;}
        finish(NIMBY_CONSTRUCTION_READY);return;
    }
    if(request.token!=operation.token||editor!=operation.editor||context!=operation.context||
       !sessionMatches()||revision.load()!=operation.preparedRevision){reject(2);return;}
    operation.executed=false;operation.authorized=false;operation.result.reason=0;
    if(request.action==NIMBY_CONSTRUCTION_CREATE){
        if(operation.result.state!=NIMBY_CONSTRUCTION_READY||request.source_signal!=operation.request.source_signal||!validTargets(db,request)){reject(5);return;}
        const auto source=object(db+0x380,request.source_signal,0xc8);
        // Enqueue allocates and transfers ownership to the game's queue; copy is
        // the native deep assignment, including names, filters and extensions.
        const auto command=native<uint64_t(*)(uint64_t)>(0x7bc7e0)(context);
        native<uint64_t(*)(uint64_t,uint64_t)>(0x339140)(command+0x20,source);
        const auto& p=request.positions[0];
        position(command,db,p);
        operation.request=request;operation.command=command;operation.undo=false;operation.result.count=0;
    }else{
        if(!operation.result.can_undo||!operation.history||get<uint64_t>(editor+0xd78)!=operation.history){reject(6);return;}
        const auto command=native<uint64_t(*)(uint64_t)>(0x7b57c0)(context);
        constexpr uint64_t copies[]{0x324a00,0x324ab0,0x324b60,0x324c10,0x324ca0,0x324d30,0x324dc0,0x324e50};
        for(uint32_t i=0;i<8;++i)native<void(*)(uint64_t,uint64_t)>(copies[i])(command+0x20+i*0xc0,operation.history+0x10+i*0xc0);
        operation.command=command;operation.undo=true;
    }
    operation.identity=commandIdentity(operation.command);operation.waiting=true;
    operation.result.state=NIMBY_CONSTRUCTION_PENDING;shared->result=operation.result;
}
}

extern "C" __declspec(dllexport) DWORD WINAPI NimbyInternal_Bootstrap(void* argument) noexcept {
    if(argument)return NIMBY_INVALID_ARGUMENT;
    AcquireSRWLockExclusive(&initialization);
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&initialization);}} unlock;
    if(enabled)return NIMBY_ALREADY_INITIALIZED;
    if(GetModuleHandleW(L"NimbyConstructionProbe-v1.dll")||GetModuleHandleW(L"NimbyConstructionBatchProbe-v1.dll"))return NIMBY_HOOKS_UNAVAILABLE;
    std::array<wchar_t,32768> path{};NimbyBinaryInfo identity{};
    if(!GetModuleFileNameW(nullptr,path.data(),static_cast<DWORD>(path.size()))||
       nimby::engine::identify(path.data(),identity)!=NIMBY_OK||!identity.recognized_research_build)return NIMBY_INVALID_BINARY;
    base=reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr));
    struct Hook {uint64_t rva;void* callback;void** original;};
    const Hook hooks[]{
        {0x7afe40,reinterpret_cast<void*>(&ui),reinterpret_cast<void**>(&originalUi)},
        {0x30e020,reinterpret_cast<void*>(&dispatch),reinterpret_cast<void**>(&originalDispatch)},
        {0x2fda60,reinterpret_cast<void*>(&create),reinterpret_cast<void**>(&originalCreate)},
        {0x2f3ab0,reinterpret_cast<void*>(&undo),reinterpret_cast<void**>(&originalUndo)}
        ,{0x762a70,reinterpret_cast<void*>(&historyPush),reinterpret_cast<void**>(&originalHistoryPush)}
        ,{0x3547f0,reinterpret_cast<void*>(&moveDelta),reinterpret_cast<void**>(&originalMoveDelta)}
    };
    // Whole-file fingerprint is mandatory. Refuse patched entries as well;
    // jumps here indicate another live interceptor whose ordering is unknown.
    for(const auto& h:hooks){const auto first=get<uint8_t>(base+h.rva);if(first==0xe9||first==0xeb||first==0xff)return NIMBY_HOOKS_UNAVAILABLE;}
    mapping=CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Shared),name(GetCurrentProcessId()).c_str());
    if(!mapping)return NIMBY_IO_ERROR;
    if(GetLastError()==ERROR_ALREADY_EXISTS){CloseHandle(mapping);return NIMBY_IO_ERROR;}
    shared=static_cast<Shared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
    if(!shared){CloseHandle(mapping);return NIMBY_IO_ERROR;}
    shared->version=protocol;shared->size=sizeof(Shared);
    bool initialized=false;size_t created=0;
    struct Cleanup {bool& initialized;size_t& created;const Hook* hooks;~Cleanup(){if(enabled)return;
        for(size_t i=0;i<created;++i)MH_RemoveHook(reinterpret_cast<void*>(base+hooks[i].rva));
        if(initialized)MH_Uninitialize();
        UnmapViewOfFile(shared);shared=nullptr;CloseHandle(mapping);
    }} cleanup{initialized,created,hooks};
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    initialized=true;
    for(const auto& h:hooks){
        if(MH_CreateHook(reinterpret_cast<void*>(base+h.rva),h.callback,h.original)!=MH_OK)return NIMBY_INTERNAL_ERROR;
        ++created;if(MH_QueueEnableHook(reinterpret_cast<void*>(base+h.rva))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    }
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&ui),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_ApplyQueued()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;return NIMBY_OK;
}
