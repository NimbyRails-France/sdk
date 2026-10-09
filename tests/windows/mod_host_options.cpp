#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/mod_option_windows.hpp>
#include <windows.h>
#include <algorithm>
#include <bit>
#include <cstdio>
#include <map>
#include <string>
#include <string_view>

namespace {
HMODULE WINAPI optionsModule(LPCWSTR);
FARPROC WINAPI optionsProcedure(HMODULE,LPCSTR);
BOOL WINAPI optionsExisting(DWORD,LPCSTR,HMODULE*);
BOOL WINAPI optionsUnload(HMODULE);
}

// Replace only module lookup, retaining the actual production facade, broker
// and adapter client. This makes unavailable/malformed bridge responses
// deterministic without loading the game or depending on DLL search paths.
#define GetModuleHandleW optionsModule
#define GetProcAddress optionsProcedure
#define GetModuleHandleExA optionsExisting
#define FreeLibrary optionsUnload
#include "../../src/platform/windows/runtime/mod_host_options.cpp"
#include <nimby/detail/mod_options_client.hpp>
#undef FreeLibrary
#undef GetModuleHandleExA
#undef GetProcAddress
#undef GetModuleHandleW

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
std::string payload=R"({"values":["true"],"events":[]})";
uint64_t responseRevision=7;
struct Record {std::string declaration;uint64_t wake;};
std::map<uint64_t,Record> records;
HMODULE module(){return reinterpret_cast<HMODULE>(uintptr_t{1});}

uint32_t registerBridge(const char* bytes,uint32_t size,uint64_t* owner){
    ++registrations;*owner=0;
    if(failedRegistration)return NIMBY_INVALID_ARGUMENT;
    if(zeroToken)return NIMBY_OK;
    *owner=++serial;records.emplace(*owner,Record{std::string(bytes,size),0});return NIMBY_OK;
}
uint32_t removeBridge(uint64_t owner){
    ++removals;
    if(failedRemoval)return NIMBY_RESOURCE_LIMIT;
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
template<class T> T symbol(const char* name){return std::bit_cast<T>(nimby::mod_host::optionsSymbol(name));}
template<class F> bool rejects(F&& operation){try{operation();return false;}catch(const std::exception&){return true;}}
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
    CHECK(CloseHandle(event1)&&CloseHandle(event2));
    std::puts("PASS mod options: RPC ownership, bounded buffers, rollback, cleanup, immutable client snapshots and malformed replies");
}
