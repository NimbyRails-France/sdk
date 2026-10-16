#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/mod_option_windows.hpp>
#include <nimby/detail/diagnostics.hpp>
#include <runtime/mod_options_registry.h>
#include <windows.h>
#include <algorithm>
#include <bit>
#include <cstdio>
#include <map>
#include <string>
#include <string_view>
#include <condition_variable>
#include <cstdlib>
#include <new>
#include <thread>

namespace nimby::detail::diagnostics {
void captureOptionsDiagnostic(const char*,const char*,const char*)noexcept;
}

namespace {
HMODULE WINAPI optionsModule(LPCWSTR);
FARPROC WINAPI optionsProcedure(HMODULE,LPCSTR);
BOOL WINAPI optionsExisting(DWORD,LPCSTR,HMODULE*);
BOOL WINAPI optionsUnload(HMODULE);
BOOL WINAPI optionsPin(DWORD,LPCWSTR,HMODULE*);
PTP_TIMER WINAPI optionsTimer(PTP_TIMER_CALLBACK,void*,PTP_CALLBACK_ENVIRON);
void WINAPI optionsSetTimer(PTP_TIMER,PFILETIME,DWORD,DWORD);
bool failNextAllocation=false;
}

// A single targeted allocation failure, not a synthetic bridge return code.
// Registry::remove must keep its transaction intact while the broker retains
// the token for a later independent timer tick.
#ifdef __GNUC__
#define FIXTURE_NOINLINE __attribute__((noinline))
#else
#define FIXTURE_NOINLINE __declspec(noinline)
#endif
FIXTURE_NOINLINE void* operator new(std::size_t bytes){
    if(failNextAllocation){failNextAllocation=false;throw std::bad_alloc();}
    if(auto* p=std::malloc(bytes?bytes:1))return p;
    throw std::bad_alloc();
}
FIXTURE_NOINLINE void* operator new[](std::size_t bytes){return ::operator new(bytes);}
FIXTURE_NOINLINE void operator delete(void* p) noexcept {std::free(p);}
FIXTURE_NOINLINE void operator delete[](void* p) noexcept {std::free(p);}
FIXTURE_NOINLINE void operator delete(void* p,std::size_t) noexcept {std::free(p);}
FIXTURE_NOINLINE void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
#undef FIXTURE_NOINLINE

// Replace only module lookup, retaining the actual production facade, broker
// and adapter client. This makes unavailable/malformed bridge responses
// deterministic without loading the game or depending on DLL search paths.
#define GetModuleHandleW optionsModule
#define GetProcAddress optionsProcedure
#define GetModuleHandleExA optionsExisting
#define FreeLibrary optionsUnload
#define GetModuleHandleExW optionsPin
#define CreateThreadpoolTimer optionsTimer
#define SetThreadpoolTimer optionsSetTimer
#define write captureOptionsDiagnostic
#include "../../src/platform/windows/runtime/mod_host_options.cpp"
#undef write
#include <nimby/detail/mod_options_client.hpp>
#undef FreeLibrary
#undef GetModuleHandleExA
#undef GetProcAddress
#undef GetModuleHandleW
#undef GetModuleHandleExW
#undef CreateThreadpoolTimer
#undef SetThreadpoolTimer

namespace {
using nimby::mod_host::Owners;
using nimby::mod_host::Request;
using nimby::mod_host::Reply;
Owners first,second;
Owners* channel=&first;
Request last;
uint32_t calls=0,registrations=0,removals=0,wakes=0,reads=0,discards=0,moduleRefs=0;
uint64_t serial=100,lastWake=0,lastKnown=0;
bool modulePresent=true,missingRead=false,failedRegistration=false,zeroToken=false;
bool oversizedBridgeWrite=false,oversizedRpcWrite=false,failedRemoval=false;
bool missingDiscard=false,failedDiscard=false;
PTP_TIMER_CALLBACK cleanupTick{};
void* cleanupContext{};
bool cleanupScheduled=false;
unsigned timerCreations=0,pins=0;
std::atomic<unsigned> deferredLogs{},recoveredLogs{},saturationLogs{};
nimby::runtime::mod_options::Registry* actualRegistry=nullptr;
uint32_t actualRemovalStatus=NIMBY_OK;
std::mutex blockedMutex;
std::condition_variable blockedWake;
bool blockRemoval=false,removalEntered=false,removalContinue=false;
std::string payload=R"({"values":["true"],"events":[]})";
uint64_t responseRevision=7;
struct Record {std::string declaration;uint64_t wake;};
std::map<uint64_t,Record> records;
HMODULE module(){return reinterpret_cast<HMODULE>(uintptr_t{1});}

uint32_t registerBridge(const char* bytes,uint32_t size,uint64_t* owner){
    ++registrations;*owner=0;
    if(failedRegistration)return NIMBY_INVALID_ARGUMENT;
    if(zeroToken)return NIMBY_OK;
    if(actualRegistry){
        using namespace nimby::runtime::mod_options;
        Definition definition{"bc-train-super-long",std::string(160,'T'),{}};
        Field field;field.id="maximumLength";field.label=std::string(160,'L');
        field.kind=Kind::Integer;field.defaultValue="850";field.minimum=1;field.maximum=9999;
        definition.fields.push_back(std::move(field));
        const auto result=actualRegistry->add(++serial,std::move(definition));
        if(!result)return result.status==Status::Duplicate?NIMBY_INVALID_ARGUMENT:NIMBY_RESOURCE_LIMIT;
        *owner=result.token;
    }else *owner=++serial;
    records.emplace(*owner,Record{std::string(bytes,size),0});return NIMBY_OK;
}
uint32_t removeBridge(uint64_t owner){
    ++removals;
    {std::unique_lock lock(blockedMutex);
        if(blockRemoval){removalEntered=true;blockedWake.notify_all();blockedWake.wait(lock,[]{return removalContinue;});}
    }
    if(failedRemoval)return NIMBY_RESOURCE_LIMIT;
    if(actualRegistry){
        try {
            nimby::runtime::mod_options::Entry final;
            const auto result=actualRegistry->remove(owner,&final);
            actualRemovalStatus=result?NIMBY_OK:NIMBY_INVALID_HANDLE;
        }catch(const std::bad_alloc&){actualRemovalStatus=NIMBY_RESOURCE_LIMIT;}
        if(actualRemovalStatus!=NIMBY_OK)return actualRemovalStatus;
    }
    return records.erase(owner)?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
uint32_t discardBridge(uint64_t owner){
    ++discards;
    if(!records.contains(owner))return NIMBY_INVALID_HANDLE;
    if(failedDiscard)return NIMBY_RESOURCE_LIMIT;
    return NIMBY_OK;
}
uint32_t wakeBridge(uint64_t owner,uint64_t event){
    ++wakes;lastWake=event;
    const auto found=records.find(owner);if(found==records.end())return NIMBY_INVALID_HANDLE;
    DWORD flags{};
    if(!event||event==UINT64_MAX||!GetHandleInformation(reinterpret_cast<HANDLE>(uintptr_t(event)),&flags))return NIMBY_INVALID_HANDLE;
    found->second.wake=event;return NIMBY_OK;
}
uint32_t readBridge(uint64_t owner,uint64_t known,char* output,uint32_t capacity,uint32_t* written,uint64_t* revision){
    ++reads;lastKnown=known;*written=0;*revision=0;
    if(!records.contains(owner))return NIMBY_INVALID_HANDLE;
    if(oversizedBridgeWrite){*written=capacity+1;*revision=responseRevision;return NIMBY_OK;}
    *revision=responseRevision;
    if(known==responseRevision&&payload==R"({"values":["true"],"events":[]})")return NIMBY_OK;
    if(payload.size()>capacity)return NIMBY_RESOURCE_LIMIT;
    *written=static_cast<uint32_t>(payload.size());
    if(*written)std::memcpy(output,payload.data(),*written);
    return NIMBY_OK;
}
HMODULE WINAPI optionsModule(LPCWSTR name){
    return modulePresent&&name&&std::wstring_view(name)==L"NimbySignalUiBridge-experimental-v1.dll"?module():nullptr;
}
FARPROC WINAPI optionsProcedure(HMODULE handle,LPCSTR name){
    if(handle!=module()||!name)return nullptr;
    if(std::strcmp(name,"NimbyOptions_RegisterV1")==0)return std::bit_cast<FARPROC>(&registerBridge);
    if(std::strcmp(name,"NimbyOptions_RemoveV1")==0)return std::bit_cast<FARPROC>(&removeBridge);
    if(std::strcmp(name,"NimbyOptions_WakeV1")==0)return std::bit_cast<FARPROC>(&wakeBridge);
    if(std::strcmp(name,"NimbyOptions_ReadV1")==0&&!missingRead)return std::bit_cast<FARPROC>(&readBridge);
    if(std::strcmp(name,"NimbyOptions_DiscardV1")==0&&!missingDiscard)return std::bit_cast<FARPROC>(&discardBridge);
    return nullptr;
}
BOOL WINAPI optionsExisting(DWORD,LPCSTR name,HMODULE* output){
    if(modulePresent&&name&&std::string_view(name)=="NimbySignalUiBridge-experimental-v1.dll"){
        *output=module();++moduleRefs;return TRUE;
    }
    *output=nullptr;return FALSE;
}
BOOL WINAPI optionsUnload(HMODULE handle){
    if(handle!=module()||!moduleRefs)return FALSE;
    --moduleRefs;return TRUE;
}
BOOL WINAPI optionsPin(DWORD flags,LPCWSTR address,HMODULE* output){
    if(!address||!output||flags!=(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN))return FALSE;
    ++pins;*output=module();return TRUE;
}
PTP_TIMER WINAPI optionsTimer(PTP_TIMER_CALLBACK callback,void* context,PTP_CALLBACK_ENVIRON){
    ++timerCreations;cleanupTick=callback;cleanupContext=context;
    return reinterpret_cast<PTP_TIMER>(uintptr_t{1});
}
void WINAPI optionsSetTimer(PTP_TIMER timer,PFILETIME due,DWORD period,DWORD){
    if(timer!=reinterpret_cast<PTP_TIMER>(uintptr_t{1}))std::abort();
    if(due&&period!=10)std::abort();
    cleanupScheduled=due!=nullptr;
}
void drainOptions(){cleanupTick(nullptr,cleanupContext,reinterpret_cast<PTP_TIMER>(uintptr_t{1}));}
template<class T> T symbol(const char* name){return std::bit_cast<T>(nimby::mod_host::optionsSymbol(name));}
template<class F> bool rejects(F&& operation){try{operation();return false;}catch(const std::exception&){return true;}}
}

namespace nimby::detail::diagnostics {
void captureOptionsDiagnostic(const char*,const char*,const char* message)noexcept{
    if(std::strstr(message,"Options cleanup deferred:"))++deferredLogs;
    if(std::strstr(message,"Options cleanup recovered:"))++recoveredLogs;
    if(std::strstr(message,"Options cleanup capacity reached:"))++saturationLogs;
}
}

extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* args,
    const void* input,uint32_t size,void* output,uint32_t capacity,uint32_t* written) noexcept {
    try {
        ++calls;*written=0;last={};last.operation=operation;std::copy_n(args,8,last.args.begin());
        if(size)last.data.assign(static_cast<const uint8_t*>(input),static_cast<const uint8_t*>(input)+size);
        if(oversizedRpcWrite){*written=capacity+1;return NIMBY_OK;}
        Reply reply;const auto status=nimby::mod_host::dispatchOptions(last,reply,*channel);
        if(reply.data.size()>capacity)return NIMBY_RESOURCE_LIMIT;
        std::copy(reply.args.begin(),reply.args.end(),args);*written=static_cast<uint32_t>(reply.data.size());
        if(*written)std::memcpy(output,reply.data.data(),*written);
        return status;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}

#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(){
    const auto add=symbol<NimbyOptionsRegisterV1>("NimbyOptions_RegisterV1");
    const auto remove=symbol<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1");
    const auto read=symbol<NimbyOptionsReadV1>("NimbyOptions_ReadV1");
    const auto discard=symbol<NimbyOptionsDiscardV1>("NimbyOptions_DiscardV1");
    CHECK(add&&remove&&read&&discard);
    CHECK(!nimby::mod_host::optionsSymbol(nullptr)&&!nimby::mod_host::optionsSymbol("missing"));
    CHECK(!nimby::mod_host::optionsSymbol("NimbyOptions_WakeV1"));
    const auto event1=CreateEventW(nullptr,FALSE,FALSE,nullptr),event2=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    CHECK(event1&&event2);first.actionWake=reinterpret_cast<uintptr_t>(event1);second.actionWake=reinterpret_cast<uintptr_t>(event2);
    const std::string schema1=R"({"id":"first"})",schema2=R"({"id":"second"})";
    uint64_t owner=999,other=0;
    const auto before=calls;
    CHECK(add(nullptr,1,&owner)==NIMBY_INVALID_ARGUMENT&&owner==0&&calls==before);
    CHECK(add(schema1.data(),NIMBY_OPTIONS_SCHEMA_LIMIT+1,&owner)==NIMBY_INVALID_ARGUMENT&&calls==before);
    CHECK(add(schema1.data(),uint32_t(schema1.size()),nullptr)==NIMBY_INVALID_ARGUMENT&&calls==before);
    modulePresent=false;
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_HOOKS_UNAVAILABLE&&!owner&&!first.options);
    modulePresent=true;failedRegistration=true;
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_INVALID_ARGUMENT&&!owner&&records.empty());
    failedRegistration=false;zeroToken=true;
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_INVALID_BINARY&&!owner&&!first.options);
    zeroToken=false;first.actionWake=UINT64_MAX;
    const auto beforeRollback=removals;
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_INVALID_HANDLE&&!owner&&!first.options);
    CHECK(records.empty()&&removals==beforeRollback+1&&lastWake==UINT64_MAX);
    first.actionWake=reinterpret_cast<uintptr_t>(event1);
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_OK&&owner&&first.options==owner);
    CHECK(records.at(owner).declaration==schema1&&last.operation==150&&lastWake==first.actionWake);
    const auto registrationsBeforeDuplicate=registrations;
    CHECK(add(schema2.data(),uint32_t(schema2.size()),&other)==NIMBY_INVALID_ARGUMENT&&!other&&registrations==registrationsBeforeDuplicate);

    std::array<char,256> output;output.fill('x');uint32_t written=99;uint64_t revision=99;
    CHECK(read(owner,0,output.data(),1,&written,&revision)==NIMBY_RESOURCE_LIMIT&&!written&&!revision&&output.front()=='x');
    CHECK(read(owner,0,output.data(),uint32_t(output.size()),&written,&revision)==NIMBY_OK&&revision==7&&written==payload.size());
    CHECK(std::string_view(output.data(),written)==payload&&last.operation==152&&last.args[1]==0);
    CHECK(read(owner,revision,output.data(),uint32_t(output.size()),&written,&revision)==NIMBY_OK&&written==0&&revision==7);
    const auto validRead=last;
    CHECK(discard(owner)==NIMBY_OK&&first.options==owner&&last.operation==153&&records.contains(owner));
    const auto validDiscard=last;const auto discardsBeforeForgery=discards;
    const auto callsBeforeInvalid=calls;
    CHECK(read(owner,0,nullptr,1,&written,&revision)==NIMBY_INVALID_ARGUMENT&&calls==callsBeforeInvalid);
    CHECK(read(owner,0,output.data(),NIMBY_OPTIONS_VALUES_LIMIT+1,&written,&revision)==NIMBY_INVALID_ARGUMENT&&calls==callsBeforeInvalid);
    CHECK(read(owner,0,output.data(),256,nullptr,&revision)==NIMBY_INVALID_ARGUMENT&&calls==callsBeforeInvalid);
    oversizedBridgeWrite=true;
    CHECK(read(owner,0,output.data(),256,&written,&revision)==NIMBY_INVALID_BINARY&&!written&&!revision);
    oversizedBridgeWrite=false;oversizedRpcWrite=true;
    CHECK(read(owner,0,output.data(),256,&written,&revision)==NIMBY_INVALID_BINARY&&!written&&!revision);
    oversizedRpcWrite=false;

    channel=&second;const auto readsBeforeForgery=reads;
    CHECK(read(owner,0,output.data(),256,&written,&revision)==NIMBY_INVALID_HANDLE&&!written&&!revision&&reads==readsBeforeForgery);
    CHECK(remove(owner)==NIMBY_INVALID_HANDLE&&records.contains(owner));
    CHECK(discard(owner)==NIMBY_INVALID_HANDLE&&discards==discardsBeforeForgery);
    CHECK(add(schema2.data(),uint32_t(schema2.size()),&other)==NIMBY_OK&&other!=owner&&lastWake==second.actionWake);
    Reply reply;
    CHECK(nimby::mod_host::dispatchOptions(validDiscard,reply,second)==NIMBY_INVALID_HANDLE&&discards==discardsBeforeForgery);
    auto invalidDiscard=validDiscard;invalidDiscard.args[0]=other;invalidDiscard.data={1};
    CHECK(nimby::mod_host::dispatchOptions(invalidDiscard,reply,second)==NIMBY_INVALID_ARGUMENT&&discards==discardsBeforeForgery);
    failedDiscard=true;CHECK(discard(other)==NIMBY_RESOURCE_LIMIT&&second.options==other);failedDiscard=false;
    CHECK(discard(other)==NIMBY_OK&&records.contains(owner)&&records.contains(other));
    CHECK(nimby::mod_host::dispatchOptions(validRead,reply,second)==NIMBY_INVALID_HANDLE);
    auto malformed=validRead;malformed.args[0]=other;malformed.data={1};
    CHECK(nimby::mod_host::dispatchOptions(malformed,reply,second)==NIMBY_INVALID_ARGUMENT);
    malformed.data.clear();malformed.args[2]=UINT64_MAX;
    CHECK(nimby::mod_host::dispatchOptions(malformed,reply,second)==NIMBY_INVALID_ARGUMENT);
    malformed.args[2]=0;malformed.operation=159;
    CHECK(nimby::mod_host::dispatchOptions(malformed,reply,second)==NIMBY_INVALID_ARGUMENT);
    failedRemoval=true;CHECK(remove(other)==NIMBY_RESOURCE_LIMIT&&second.options==other);failedRemoval=false;
    nimby::mod_host::cleanupOptions(second);CHECK(!second.options&&!records.contains(other)&&records.contains(owner));
    channel=&first;CHECK(read(owner,0,output.data(),256,&written,&revision)==NIMBY_OK);
    CHECK(remove(owner)==NIMBY_OK&&!first.options&&!records.contains(owner));
    CHECK(remove(owner)==NIMBY_INVALID_HANDLE);
    nimby::mod_host::cleanupOptions(first);CHECK(records.empty());

    // The client must not consume its revision when a response is rejected.
    nimby::detail::ModOptionsClient client;
    missingRead=true;CHECK(!client.connect(schema1)&&!moduleRefs&&records.empty());missingRead=false;
    missingDiscard=true;CHECK(!client.connect(schema1)&&!moduleRefs&&records.empty());missingDiscard=false;
    CHECK(client.connect(schema1)&&client.connected()&&moduleRefs==1);
    auto changes=client.refresh();CHECK(changes.changed&&changes.values==std::vector<std::string>{"true"}&&lastKnown==0);
    CHECK(!client.refresh().changed&&lastKnown==7);
    client.discardEvents();CHECK(!client.refresh().changed&&lastKnown==7);
    failedDiscard=true;CHECK(rejects([&]{client.discardEvents();})&&client.connected());failedDiscard=false;
    responseRevision=8;payload=R"({"values":["false"],"events":["window.main"]})";
    changes=client.refresh();CHECK(changes.changed&&changes.values==std::vector<std::string>{"false"}&&changes.events==std::vector<std::string>{"window.main"});
    responseRevision=9;
    for(const auto& invalid:std::vector<std::string>{"", "{", "{}", "[]", R"({"events":[]})", R"({"values":[42]})",R"({"values":["bad\u0000value"]})",
        R"({"events":["bad\u0000event"]})",R"({"values":[")"+std::string(257,'x')+R"("]})"}){
        payload=invalid;CHECK(rejects([&]{client.refresh();})&&lastKnown==8);
    }
    payload=R"({"values":["true"],"events":[]})";
    responseRevision=0;CHECK(rejects([&]{client.refresh();})&&lastKnown==8);
    responseRevision=7;CHECK(rejects([&]{client.refresh();})&&lastKnown==8);
    responseRevision=9;oversizedBridgeWrite=true;CHECK(rejects([&]{client.refresh();})&&lastKnown==8);oversizedBridgeWrite=false;
    CHECK(rejects([&]{client.refresh([](const auto&){throw std::invalid_argument("Schema mismatch");});})&&lastKnown==8);
    changes=client.refresh();CHECK(changes.changed&&lastKnown==8);
    client.close();CHECK(!client.connected()&&!moduleRefs&&records.empty());
    client.discardEvents();
    nimby::detail::ModOptionWindows mapping,reordered;
    CHECK(mapping.add("main")=="window.main"&&mapping.resolve("window.main")=="main");
    CHECK(mapping.resolve("window.forged").empty());
    CHECK(mapping.add(std::string(121,'x'))=="window."+std::string(121,'x'));
    const std::string longWindow(128,'x');const auto longKey=mapping.add(longWindow);
    CHECK(longKey=="window.h49c1db537e970e2d89b531efc0159b8d"&&mapping.resolve(longKey)==longWindow);
    CHECK(reordered.add(longWindow)==longKey&&reordered.add("main")=="window.main");
    CHECK(rejects([&]{mapping.add("main");}));
    // A literal legacy ID that aliases a generated digest is rejected, never
    // silently associated with somebody else's saved shortcut preference.
    CHECK(rejects([&]{mapping.add(longKey.substr(7));}));
    CHECK(rejects([&]{mapping.add(std::string(129,'x'));}));
    CHECK(rejects([&]{mapping.add(std::string(127,'x')+" ");}));

    // The actual transactional Registry throws before revocation under OOM.
    // The last worker may disappear: only the independently scheduled timer
    // retries, preserving another live worker's options and wake handle.
    Owners active,retired,restarted;
    active.actionWake=event1?reinterpret_cast<uintptr_t>(event1):0;
    retired.actionWake=restarted.actionWake=active.actionWake;
    channel=&active;uint64_t activeToken{};
    CHECK(add(schema2.data(),uint32_t(schema2.size()),&activeToken)==NIMBY_OK);
    nimby::runtime::mod_options::Registry registry;actualRegistry=&registry;
    using OptionsStatus=nimby::runtime::mod_options::Status;
    nimby::runtime::mod_options::Field liveField;
    liveField.id="window.clock";liveField.label="Live clock";
    liveField.kind=nimby::runtime::mod_options::Kind::Shortcut;liveField.defaultValue="F9";
    CHECK(registry.listNativeBindings({}));
    const auto live=registry.add(10000,{"live-worker","Live worker",{liveField}});CHECK(live);
    const nimby::engine::mod_shortcuts::Event liveKey{nimby::engine::mod_shortcuts::Key::F9,0,false};
    const nimby::engine::mod_shortcuts::Context inGame{true,false,false};
    channel=&retired;uint64_t retiredToken{};
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&retiredToken)==NIMBY_OK);
    CHECK(registry.dispatch(liveKey,inGame)==1);
    failNextAllocation=true;
    const auto beforeDeferred=deferredLogs.load(),beforeRecovered=recoveredLogs.load();
    nimby::mod_host::cleanupOptions(retired);
    CHECK(!failNextAllocation&&actualRemovalStatus==NIMBY_RESOURCE_LIMIT&&!retired.options&&cleanupScheduled);
    CHECK(deferredLogs==beforeDeferred+1&&recoveredLogs==beforeRecovered);
    CHECK(registry.snapshot()->mods.size()==2&&records.contains(retiredToken)&&records.contains(activeToken));
    CHECK(registry.poll(live.token).has_value()); // Failed removal did not discard another worker's event.
    channel=&restarted;uint64_t restartToken{};
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&restartToken)==NIMBY_INVALID_ARGUMENT&&!restarted.options&&!restartToken);
    failedRemoval=true;const auto beforeRetry=removals;drainOptions();
    CHECK(removals==beforeRetry+4&&cleanupScheduled&&registry.snapshot()->mods.size()==2);
    CHECK(deferredLogs==beforeDeferred+1&&recoveredLogs==beforeRecovered);
    channel=&active;CHECK(read(activeToken,0,output.data(),256,&written,&revision)==NIMBY_OK);
    DWORD eventFlags{};CHECK(GetHandleInformation(event1,&eventFlags));
    failedRemoval=false;drainOptions();
    CHECK(!cleanupScheduled&&registry.snapshot()->mods.size()==1&&!records.contains(retiredToken)&&records.contains(activeToken));
    CHECK(recoveredLogs==beforeRecovered+1);
    CHECK(registry.dispatch(liveKey,inGame)==1&&registry.poll(live.token).has_value());
    CHECK(registry.poll(retiredToken)==std::nullopt);
    channel=&restarted;
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&restartToken)==NIMBY_OK&&restartToken!=retiredToken);
    CHECK(remove(restartToken)==NIMBY_OK&&!restarted.options);
    actualRegistry=nullptr;nimby::mod_host::cleanupOptions(active);CHECK(records.empty());
    CHECK(registry.remove(live.token).status==OptionsStatus::Ok&&registry.snapshot()->mods.empty());

    // A failed Wake must retain a failed rollback even though no token was
    // exposed to the child and the registration never became its owner.
    Owners failedWake;failedWake.actionWake=UINT64_MAX;channel=&failedWake;
    failedRemoval=true;
    CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_INVALID_HANDLE&&!owner&&!failedWake.options);
    CHECK(records.size()==1&&cleanupScheduled);
    failedRemoval=false;drainOptions();CHECK(records.empty()&&!cleanupScheduled);

    // Capacity is reserved before Register. Repeated dead/failed workers cannot
    // overflow the retry queue or create one untracked registry entry at #65.
    std::array<Owners,64> stopped;
    failedRemoval=true;
    for(auto& stoppedOwner:stopped){
        stoppedOwner.actionWake=reinterpret_cast<uintptr_t>(event1);channel=&stoppedOwner;
        CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_OK&&owner);
        nimby::mod_host::cleanupOptions(stoppedOwner);CHECK(!stoppedOwner.options);
    }
    CHECK(records.size()==64&&cleanupScheduled);
    Owners overflow;overflow.actionWake=reinterpret_cast<uintptr_t>(event2);channel=&overflow;
    const auto beforeCapacity=registrations;
    const auto beforeSaturation=saturationLogs.load();
    CHECK(add(schema2.data(),uint32_t(schema2.size()),&owner)==NIMBY_RESOURCE_LIMIT&&!owner&&!overflow.options);
    CHECK(saturationLogs==beforeSaturation+1);
    CHECK(add(schema2.data(),uint32_t(schema2.size()),&owner)==NIMBY_RESOURCE_LIMIT&&!owner&&!overflow.options);
    CHECK(saturationLogs==beforeSaturation+1); // Refusal spam is one transition, not one disk log per request.
    CHECK(registrations==beforeCapacity&&records.size()==64);
    const auto beforeFullTick=removals;drainOptions();CHECK(removals==beforeFullTick+4&&records.size()==64);
    failedRemoval=false;
    for(unsigned tick=0;tick<16;++tick){const auto beforeTick=removals;drainOptions();CHECK(removals==beforeTick+4);}
    CHECK(records.empty()&&!cleanupScheduled);

    // Native removal must not hold the queue's capacity mutex. While the timer
    // is inside one call, a different worker can register; an overlapping tick
    // must not invoke a second removal for the same pending token.
    Owners paused,peer;paused.actionWake=peer.actionWake=reinterpret_cast<uintptr_t>(event1);
    channel=&paused;CHECK(add(schema1.data(),uint32_t(schema1.size()),&owner)==NIMBY_OK);
    failedRemoval=true;nimby::mod_host::cleanupOptions(paused);failedRemoval=false;
    {std::lock_guard lock(blockedMutex);blockRemoval=true;removalEntered=false;removalContinue=false;}
    std::thread draining([]{drainOptions();});
    {std::unique_lock lock(blockedMutex);CHECK(blockedWake.wait_for(lock,std::chrono::seconds(2),[]{return removalEntered;}));}
    const auto inFlight=removals;drainOptions();CHECK(removals==inFlight);
    channel=&peer;CHECK(add(schema2.data(),uint32_t(schema2.size()),&other)==NIMBY_OK&&records.contains(other));
    CHECK(GetHandleInformation(event1,&eventFlags));
    {std::lock_guard lock(blockedMutex);removalContinue=true;blockedWake.notify_all();}
    draining.join();
    {std::lock_guard lock(blockedMutex);blockRemoval=false;}
    CHECK(records.size()==1&&records.contains(other)&&!cleanupScheduled);
    nimby::mod_host::cleanupOptions(peer);CHECK(records.empty());
    CHECK(timerCreations==1&&pins==1);
    CHECK(CloseHandle(event1)&&CloseHandle(event2));
    std::puts("PASS mod options: RPC ownership, bounded buffers, retained OOM/Wake cleanup, 64-slot retirement, independent timer, concurrent owners and immutable snapshots");
}
