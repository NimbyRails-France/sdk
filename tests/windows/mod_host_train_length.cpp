#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/train_length.h>
#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/translations.hpp>
#include <platform/windows/mod_host_releases.h>
#include <runtime/mod_options_registry.h>
#include <engine/train_length_policy.h>
#include <windows.h>
#include <algorithm>
#include <atomic>
#include <bit>
#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>

namespace {
HMODULE WINAPI lengthModule(LPCWSTR);
FARPROC WINAPI lengthProcedure(HMODULE,LPCSTR);
}
namespace nimby::detail::diagnostics {
void captureTrainLengthWrite(const char*,const char*,const char*) noexcept;
}
// Replace module lookup only: production wire validation, capability mapping,
// bounded retirement reservation and SDK timer remain exercised unchanged.
#define GetModuleHandleW lengthModule
#define GetProcAddress lengthProcedure
#define write captureTrainLengthWrite
#include "../../src/platform/windows/runtime/mod_host_train_length.cpp"
#undef write
#undef GetProcAddress
#undef GetModuleHandleW

namespace {
using namespace nimby::mod_host;
Owners first,second;
Owners* channel=&first;
Request last;
std::mutex fixtureMutex;
struct Record {std::string id;uint32_t maximum;std::string declaration;};
std::map<uint64_t,Record> records;
uint64_t serial=10000,lastNativeOwner=0;
uint32_t ensureStatus=NIMBY_OK,calls=0,registrations=0,editorRegistrations=0,updates=0;
std::atomic<uint32_t> removals=0;
std::atomic<bool> busyRemove=false;
bool modulePresent=true,missingSymbol=false,zeroOwner=false;
struct CapturedLogs {std::mutex mutex;std::vector<std::string> values;};
CapturedLogs& capturedLogs(){static auto* value=new CapturedLogs;return *value;}
size_t logCount(){auto& logs=capturedLogs();std::lock_guard lock(logs.mutex);return logs.values.size();}
bool hasLog(const std::string& text){auto& logs=capturedLogs();std::lock_guard lock(logs.mutex);
    return std::any_of(logs.values.begin(),logs.values.end(),[&](const auto& entry){return entry.find(text)!=std::string::npos;});}
uint32_t registerStatus=NIMBY_OK,updateStatus=NIMBY_OK;

uint32_t nativeRegister(const char* id,uint32_t maximum,uint64_t* owner){
    std::lock_guard lock(fixtureMutex);++registrations;*owner=0;
    if(zeroOwner)return registerStatus;
    *owner=++serial;lastNativeOwner=*owner;records.emplace(*owner,Record{id,maximum,{}});return registerStatus;
}
uint32_t nativeEditorRegister(const char* id,uint32_t maximum,const char* declaration,uint32_t bytes,uint64_t* owner){
    ++editorRegistrations;
    const auto status=nativeRegister(id,maximum,owner);
    if(*owner){std::lock_guard lock(fixtureMutex);records.at(*owner).declaration.assign(declaration,bytes);}
    return status;
}
uint32_t nativeUpdate(uint64_t owner,uint32_t maximum){
    std::lock_guard lock(fixtureMutex);++updates;lastNativeOwner=owner;
    if(!records.contains(owner))return NIMBY_INVALID_HANDLE;
    if(updateStatus!=NIMBY_OK)return updateStatus;
    records.at(owner).maximum=maximum;return NIMBY_OK;
}
uint32_t nativeRemove(uint64_t owner){
    ++removals;if(busyRemove.load())return NIMBY_RESOURCE_LIMIT;
    std::lock_guard lock(fixtureMutex);return records.erase(owner)?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
HMODULE WINAPI lengthModule(LPCWSTR name){
    assert(std::wstring_view(name)==L"NimbySignalUiBridge-experimental-v1.dll");
    return modulePresent?reinterpret_cast<HMODULE>(uintptr_t{1}):nullptr;
}
FARPROC WINAPI lengthProcedure(HMODULE module,LPCSTR name){
    assert(module==reinterpret_cast<HMODULE>(uintptr_t{1}));if(missingSymbol)return nullptr;
    if(std::strcmp(name,"NimbyTrainLength_Register")==0)return std::bit_cast<FARPROC>(&nativeRegister);
    if(std::strcmp(name,"NimbyTrainEditor_RegisterV2")==0)return std::bit_cast<FARPROC>(&nativeEditorRegister);
    if(std::strcmp(name,"NimbyTrainLength_Update")==0)return std::bit_cast<FARPROC>(&nativeUpdate);
    if(std::strcmp(name,"NimbyTrainLength_Remove")==0)return std::bit_cast<FARPROC>(&nativeRemove);
    return nullptr;
}
size_t active(){std::lock_guard lock(fixtureMutex);return records.size();}
Record record(uint64_t owner){std::lock_guard lock(fixtureMutex);return records.at(owner);}
template<class Predicate> bool wait(Predicate predicate){
    const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(3);
    while(std::chrono::steady_clock::now()<deadline){
        if(predicate())return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return predicate();
}
uint32_t registerPeer(Owners& owners){
    Request request;request.operation=400;request.args[0]=850;const std::string id="BC-Train-super-Long";
    append(request,id.data(),id.size());Reply reply;return dispatchTrainLength(request,reply,owners);
}
std::string editorDeclaration(){
    using Json=nlohmann::json;
    const auto translations=Json({{"fallback","en"},{"languages",{
        {"en",{{"exceeded","Train too long"},{"lengthUnavailable","Train length unavailable"},{"verificationUnavailable","Train cannot be verified"}}},
        {"fr",{{"exceeded","Train trop long"},{"lengthUnavailable","Longueur du train indisponible"},{"verificationUnavailable","Impossible de vérifier le train"}}}}}}).dump();
    auto messages=Json::array();
    for(const auto* key:{"exceeded","lengthUnavailable","verificationUnavailable"})
        messages.push_back(std::string(nimby::detail::Translations::prefix)+Json::array({key,Json::object()}).dump());
    return Json({{"messages",messages},{"translations",translations}}).dump();
}
Request editorRequest(std::string_view id,std::string_view declaration){
    Request request;request.operation=403;request.args={850,id.size(),declaration.size()};
    append(request,id.data(),id.size());append(request,declaration.data(),declaration.size());return request;
}
}
namespace nimby::detail::diagnostics {
void captureTrainLengthWrite(const char* component,const char* level,const char* message)noexcept {
    assert(std::string_view(component)=="sdk"&&std::string_view(level)=="INFO");
    auto& logs=capturedLogs();std::lock_guard lock(logs.mutex);assert(logs.values.size()<512);logs.values.emplace_back(message);
}
}
extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* args,
    const void* input,uint32_t size,void* output,uint32_t capacity,uint32_t* written)noexcept {
    try {
        ++calls;*written=0;last={};last.operation=operation;std::copy_n(args,8,last.args.begin());
        if(size)last.data.assign(static_cast<const uint8_t*>(input),static_cast<const uint8_t*>(input)+size);
        if(operation==1){assert(!size);return ensureStatus;}
        Reply reply;const auto status=dispatchTrainLength(last,reply,*channel);
        if(reply.data.size()>capacity)return NIMBY_RESOURCE_LIMIT;
        std::copy(reply.args.begin(),reply.args.end(),args);*written=static_cast<uint32_t>(reply.data.size());
        if(*written)std::memcpy(output,reply.data.data(),*written);
        return status;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}

int main(){
    using namespace nimby::mod_host;
    assert(!trainLengthSymbol(nullptr)&&!trainLengthSymbol("missing"));
    assert(trainLengthSymbol("NimbyTrainLength_Register")&&trainLengthSymbol("NimbyTrainLength_Update")&&trainLengthSymbol("NimbyTrainLength_Remove"));
    assert(trainLengthSymbol("NimbyTrainEditor_RegisterV2"));
    uint64_t owner=999;
    assert(NimbyInternal_TrainLengthRegister(nullptr,850,&owner)==NIMBY_INVALID_ARGUMENT&&owner==0&&calls==0);
    for(const auto* id:{"","../other","9invalid","invalid/name"})assert(NimbyInternal_TrainLengthRegister(id,850,&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainLengthRegister(std::string(129,'a').c_str(),850,&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainLengthRegister("BC-Train-super-Long",0,&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainLengthRegister("BC-Train-super-Long",10001,&owner)==NIMBY_INVALID_ARGUMENT);
    ensureStatus=NIMBY_HOOKS_UNAVAILABLE;
    assert(NimbyInternal_TrainLengthRegister("BC-Train-super-Long",850,&owner)==NIMBY_HOOKS_UNAVAILABLE&&owner==0&&registrations==0);
    ensureStatus=NIMBY_OK;
    assert(NimbyInternal_TrainLengthRegister("BC-Train-super-Long",850,&owner)==NIMBY_OK&&owner&&owner==first.trainLength);
    const auto nativeFirst=lastNativeOwner;assert(nativeFirst!=owner&&record(nativeFirst).maximum==850);
    assert(record(nativeFirst).id.starts_with("NRF.TrainLength.")&&last.operation==400&&last.args[0]==850);
    assert(std::string(last.data.begin(),last.data.end())=="BC-Train-super-Long");
    uint64_t duplicate{};assert(NimbyInternal_TrainLengthRegister("other",850,&duplicate)==NIMBY_INVALID_ARGUMENT&&duplicate==0&&active()==1);
    channel=&second;uint64_t secondOwner{};
    assert(NimbyInternal_TrainLengthRegister("BC-Train-super-Long",950,&secondOwner)==NIMBY_OK&&secondOwner!=owner);
    const auto nativeSecond=lastNativeOwner;assert(record(nativeFirst).id!=record(nativeSecond).id);
    const auto beforeUpdates=updates;
    assert(NimbyInternal_TrainLengthUpdate(owner,1000)==NIMBY_INVALID_HANDLE&&updates==beforeUpdates);
    assert(NimbyInternal_TrainLengthRemove(owner)==NIMBY_INVALID_HANDLE&&active()==2);
    assert(NimbyInternal_TrainLengthUpdate(secondOwner,1000)==NIMBY_OK&&record(nativeSecond).maximum==1000&&last.operation==401&&last.data.empty());
    updateStatus=NIMBY_RESOURCE_LIMIT;
    assert(NimbyInternal_TrainLengthUpdate(secondOwner,600)==NIMBY_RESOURCE_LIMIT&&record(nativeSecond).maximum==1000&&second.trainLength==secondOwner);
    updateStatus=NIMBY_OK;
    Request malformed;Reply reply;malformed.operation=401;malformed.args={secondOwner,850,1};
    assert(dispatchTrainLength(malformed,reply,second)==NIMBY_INVALID_ARGUMENT);
    malformed.args={secondOwner,UINT64_MAX};assert(dispatchTrainLength(malformed,reply,second)==NIMBY_INVALID_ARGUMENT);
    malformed.args={secondOwner,850};malformed.data={0};assert(dispatchTrainLength(malformed,reply,second)==NIMBY_INVALID_ARGUMENT);
    malformed={};malformed.operation=400;malformed.args[0]=850;malformed.data={'a',0,'b'};
    Owners invalidOwner;assert(dispatchTrainLength(malformed,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    malformed.data={'a'};malformed.args[1]=1;assert(dispatchTrainLength(malformed,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainLengthUpdate(0,850)==NIMBY_INVALID_ARGUMENT&&NimbyInternal_TrainLengthRemove(0)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainLengthRemove(secondOwner)==NIMBY_OK&&second.trainLength==0);
    channel=&first;assert(NimbyInternal_TrainLengthRemove(owner)==NIMBY_OK&&first.trainLength==0&&active()==0);

    // ABI 11 transports one copied declaration and leaves existing ABI 10
    // channels usable. Neither the descriptor nor updates cross ownership.
    auto declaration=editorDeclaration();const auto originalDeclaration=declaration;
    const auto beforeEditorCalls=calls;
    assert(NimbyInternal_TrainEditorRegister(nullptr,850,declaration.data(),uint32_t(declaration.size()),&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainEditorRegister("mod",850,nullptr,1,&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainEditorRegister("mod",850,declaration.data(),0,&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainEditorRegister("mod",850,declaration.data(),NIMBY_OPTIONS_SCHEMA_LIMIT+1,&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainEditorRegister("../mod",850,declaration.data(),uint32_t(declaration.size()),&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainEditorRegister("mod",0,declaration.data(),uint32_t(declaration.size()),&owner)==NIMBY_INVALID_ARGUMENT);
    assert(NimbyInternal_TrainEditorRegister("mod",850,declaration.data(),uint32_t(declaration.size()),nullptr)==NIMBY_INVALID_ARGUMENT);
    assert(calls==beforeEditorCalls&&!owner);
    assert(NimbyInternal_TrainEditorRegister("mod",850,declaration.data(),uint32_t(declaration.size()),&owner)==NIMBY_OK);
    const auto editorNative=lastNativeOwner;
    assert(owner==first.trainLength&&last.operation==403&&last.args[1]==3&&last.args[2]==declaration.size());
    assert(record(editorNative).declaration==originalDeclaration&&editorRegistrations==1);
    assert(hasLog("messageOwner=mod declarationBytes="+std::to_string(declaration.size())));
    std::fill(declaration.begin(),declaration.end(),'x');
    assert(record(editorNative).declaration==originalDeclaration); // Native owner holds copied text.
    assert(NimbyInternal_TrainLengthUpdate(owner,1800)==NIMBY_OK&&record(editorNative).maximum==1800);
    assert(record(editorNative).declaration==originalDeclaration&&last.data.empty()&&editorRegistrations==1);
    channel=&second;
    assert(NimbyInternal_TrainLengthUpdate(owner,1900)==NIMBY_INVALID_HANDLE);
    assert(NimbyInternal_TrainLengthRemove(owner)==NIMBY_INVALID_HANDLE);
    assert(NimbyInternal_TrainEditorRegister("mod",900,originalDeclaration.data(),uint32_t(originalDeclaration.size()),&secondOwner)==NIMBY_OK);
    assert(secondOwner!=owner&&record(lastNativeOwner).id!=record(editorNative).id);
    assert(NimbyInternal_TrainEditorRegister("mod",900,originalDeclaration.data(),uint32_t(originalDeclaration.size()),&duplicate)==NIMBY_INVALID_ARGUMENT&&!duplicate);
    assert(NimbyInternal_TrainLengthRemove(secondOwner)==NIMBY_OK);
    channel=&first;assert(NimbyInternal_TrainLengthRemove(owner)==NIMBY_OK&&!first.trainLength&&active()==0);

    // Rejected descriptors cannot consume retirement/native capacity. The
    // parent parses strict JSON even if an untrusted child bypasses its facade.
    const auto beforeRejectedRegistrations=registrations;
    const std::string plain=R"({"messages":["Too long","Length unavailable","Verification unavailable"],"translations":""})";
    std::vector<std::string> rejected={"", "[]", "{", "null",
        R"({"messages":["a","b"],"translations":""})",
        R"({"messages":["a","b","c","d"],"translations":""})",
        R"({"messages":[1,"b","c"],"translations":""})",
        R"({"messages":["","b","c"],"translations":""})",
        R"({"messages":["a","b","c"],"translations":{},"extra":0})",
        R"({"messages":["a","b","c"],"translations":"","extra":0})",
        R"({"messages":["a","b","c"],"messages":["d","e","f"],"translations":""})",
        R"({"messages":["a","b","c"],"translations":"","translations":""})",
        R"({"messages":["a\u0000","b","c"],"translations":""})"};
    using Json=nlohmann::json;
    auto invalid=Json::parse(plain);invalid["messages"][0]=std::string(1025,'x');rejected.push_back(invalid.dump());
    invalid=Json::parse(plain);invalid["messages"][0]=std::string(nimby::detail::Translations::prefix)+R"(["missing",{}])";rejected.push_back(invalid.dump());
    invalid=Json::parse(originalDeclaration);
    invalid["messages"][0]=std::string(nimby::detail::Translations::prefix)+R"(["exceeded",{"maximum":"850"}])";rejected.push_back(invalid.dump());
    invalid=Json::parse(originalDeclaration);invalid["translations"]=R"({"fallback":"en","languages":{"en":{"exceeded":"a","exceeded":"b"}}})";rejected.push_back(invalid.dump());
    invalid=Json::parse(plain);auto nested=Json::array();
    for(int depth=0;depth<10;++depth)nested=Json::array({nested});
    invalid["messages"][0]=nested;rejected.push_back(invalid.dump());
    auto invalidUtf8=plain;invalidUtf8.insert(invalidUtf8.find("Too"),1,char(0xff));rejected.push_back(invalidUtf8);
    auto embeddedNull=plain;embeddedNull.insert(embeddedNull.find("Too"),1,'\0');rejected.push_back(embeddedNull);
    for(const auto& bad:rejected){auto request=editorRequest("mod",bad);Owners rejectedOwner;
        assert(dispatchTrainLength(request,reply,rejectedOwner)==NIMBY_INVALID_ARGUMENT&&!rejectedOwner.trainLength);}
    auto badRequest=editorRequest("mod",plain);badRequest.args[3]=1;
    assert(dispatchTrainLength(badRequest,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    badRequest=editorRequest("mod",plain);badRequest.args[0]=UINT64_MAX;
    assert(dispatchTrainLength(badRequest,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    badRequest=editorRequest("mod",plain);badRequest.args[1]=129;
    assert(dispatchTrainLength(badRequest,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    badRequest=editorRequest("mod",plain);badRequest.args[2]=NIMBY_OPTIONS_SCHEMA_LIMIT+1;
    assert(dispatchTrainLength(badRequest,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    badRequest=editorRequest("mod",plain);badRequest.data.pop_back();
    assert(dispatchTrainLength(badRequest,reply,invalidOwner)==NIMBY_INVALID_ARGUMENT);
    assert(registrations==beforeRejectedRegistrations&&active()==0);
    assert(NimbyInternal_TrainEditorRegister("mod",850,plain.data(),uint32_t(plain.size()),&owner)==NIMBY_OK);
    assert(record(lastNativeOwner).declaration==plain);assert(NimbyInternal_TrainLengthRemove(owner)==NIMBY_OK);

    // Both declared limits are inclusive. A 1 MiB valid catalogue is embedded
    // in a descriptor padded to exactly 2 MiB without broadening either bound.
    auto boundedCatalogue=std::string(R"({"fallback":"en","languages":{"en":{"label":"Text"}}})");
    boundedCatalogue.append(nimby::detail::Translations::maximumBytes-boundedCatalogue.size(),' ');
    auto boundedJson=Json::parse(plain);boundedJson["translations"]=boundedCatalogue;
    boundedJson["messages"][0]=std::string(1024,'x');
    auto boundedDescriptor=boundedJson.dump();assert(boundedDescriptor.size()<NIMBY_OPTIONS_SCHEMA_LIMIT);
    boundedDescriptor.append(NIMBY_OPTIONS_SCHEMA_LIMIT-boundedDescriptor.size(),' ');
    assert(NimbyInternal_TrainEditorRegister("mod",850,boundedDescriptor.data(),uint32_t(boundedDescriptor.size()),&owner)==NIMBY_OK);
    assert(record(lastNativeOwner).declaration==boundedDescriptor);assert(NimbyInternal_TrainLengthRemove(owner)==NIMBY_OK);
    boundedDescriptor+=' ';const auto callsAtBound=calls;
    assert(NimbyInternal_TrainEditorRegister("mod",850,boundedDescriptor.data(),uint32_t(boundedDescriptor.size()),&owner)==NIMBY_INVALID_ARGUMENT&&!owner&&calls==callsAtBound);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // Missing bridge exports and malformed success never keep an IPC claim.
    modulePresent=false;assert(NimbyInternal_TrainLengthRegister("mod",850,&owner)==NIMBY_HOOKS_UNAVAILABLE&&owner==0);
    modulePresent=true;missingSymbol=true;assert(NimbyInternal_TrainLengthRegister("mod",850,&owner)==NIMBY_HOOKS_UNAVAILABLE&&owner==0);
    missingSymbol=false;zeroOwner=true;assert(NimbyInternal_TrainLengthRegister("mod",850,&owner)==NIMBY_INVALID_BINARY&&owner==0);zeroOwner=false;
    registerStatus=NIMBY_DATA_UNAVAILABLE;
    assert(NimbyInternal_TrainLengthRegister("mod",850,&owner)==NIMBY_DATA_UNAVAILABLE&&owner==0);
    registerStatus=NIMBY_OK;assert(wait([]{return active()==0;}));
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // A quarantined channel gives up its own IPC capability immediately, but
    // its native retirement stays reserved while the native registry is busy.
    assert(registerPeer(first)==NIMBY_OK);busyRemove=true;
    const auto retiredToken=first.trainLength,retiredNative=lastNativeOwner;
    const auto began=std::chrono::steady_clock::now();cleanupTrainLength(first);
    assert(std::chrono::steady_clock::now()-began<std::chrono::milliseconds(50)&&!first.trainLength);
    assert(hasLog("phase=owner-channel-retired ipcOwner="+std::to_string(retiredToken)+" nativeOwner="+std::to_string(retiredNative)));
    assert(hasLog("status=0 statusScope=retirement-queued retryDeferred=1"));
    const auto attempts=removals.load();assert(wait([&]{return removals.load()>attempts;}));
    const auto deferredLogs=logCount();std::this_thread::sleep_for(std::chrono::milliseconds(40));
    assert(logCount()==deferredLogs); // Busy attempts never write retry floods.
    std::array<Owners,64> peers;
    for(size_t i=0;i<63;++i)assert(registerPeer(peers[i])==NIMBY_OK);
    const auto registrationsBefore=registrations;assert(registerPeer(peers[63])==NIMBY_RESOURCE_LIMIT&&registrations==registrationsBefore&&active()==64);
    busyRemove=false;assert(wait([]{return active()==63;}));
    assert(wait([&]{return hasLog("phase=owner-policy-released ipcOwner="+std::to_string(retiredToken)+" nativeOwner="+std::to_string(retiredNative));}));
    assert(hasLog("status=0 statusScope=native-remove retryDeferred=0"));
    assert(wait([&]{return registerPeer(peers[63])==NIMBY_OK;}));
    for(auto& peer:peers)cleanupTrainLength(peer);
    assert(wait([]{return active()==0;}));
    std::cout<<"PASS: train-length RPC validation, separate parent capabilities, isolated updates and reserved retirement under contention\n";
}
