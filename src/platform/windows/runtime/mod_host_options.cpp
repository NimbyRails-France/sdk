#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/mod_options_bridge.h>
#include <windows.h>
#include <bit>

namespace nimby::mod_host {
namespace {
constexpr uint32_t addOperation=150,removeOperation=151,readOperation=152,discardOperation=153;
template<class F> uint32_t boundary(F&& f) noexcept {
    try{return f();}catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}
    catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
template<class T,class... Args> uint32_t call(const char* name,Args... args) {
    const auto module=GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll");
    const auto fn=module?std::bit_cast<T>(GetProcAddress(module,name)):nullptr;
    return fn?fn(args...):NIMBY_HOOKS_UNAVAILABLE;
}
uint32_t add(const char* bytes,uint32_t size,uint64_t* owner) noexcept {
    if(owner)*owner=0;
    if(!owner||!bytes||!size||size>NIMBY_OPTIONS_SCHEMA_LIMIT)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request;request.operation=addOperation;append(request,bytes,size);
        Reply reply;const auto status=invoke(request,reply,0);if(status==NIMBY_OK)*owner=reply.args[0];return status;});
}
uint32_t remove(uint64_t owner) noexcept {
    return boundary([&]{Request request;request.operation=removeOperation;request.args[0]=owner;
        Reply reply;return invoke(request,reply,0);});
}
uint32_t discard(uint64_t owner) noexcept {
    return boundary([&]{Request request;request.operation=discardOperation;request.args[0]=owner;
        Reply reply;return invoke(request,reply,0);});
}
uint32_t readOptions(uint64_t owner,uint64_t known,char* out,uint32_t capacity,uint32_t* written,uint64_t* revision) noexcept {
    if(written)*written=0;
    if(revision)*revision=0;
    if(!written||!revision||capacity>NIMBY_OPTIONS_VALUES_LIMIT||(!out&&capacity))return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request;request.operation=readOperation;request.args={owner,known,capacity};
        Reply reply;const auto status=invoke(request,reply,capacity);
        if(status==NIMBY_OK){if(reply.data.size()>capacity)return uint32_t(NIMBY_INVALID_BINARY);
            if(!reply.data.empty())std::memcpy(out,reply.data.data(),reply.data.size());
            *written=static_cast<uint32_t>(reply.data.size());*revision=reply.args[1];}
        return status;});
}
}
uint32_t dispatchOptions(const Request& request,Reply& reply,Owners& owners) {
    const auto& a=request.args;
    if(request.operation==addOperation){
        // One registry entry per isolated mod, irrespective of how many times
        // a faulty child asks to register. Failed setup never retains a token.
        if(owners.options||request.data.empty()||request.data.size()>NIMBY_OPTIONS_SCHEMA_LIMIT)return NIMBY_INVALID_ARGUMENT;
        uint64_t owner{};
        auto status=call<NimbyOptionsRegisterV1>("NimbyOptions_RegisterV1",reinterpret_cast<const char*>(request.data.data()),
            static_cast<uint32_t>(request.data.size()),&owner);
        if(status!=NIMBY_OK)return status;
        if(!owner)return NIMBY_INVALID_BINARY;
        status=call<NimbyOptionsWakeV1>("NimbyOptions_WakeV1",owner,owners.actionWake);
        if(status!=NIMBY_OK){call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",owner);return status;}
        owners.options=owner;reply.args[0]=owner;return NIMBY_OK;
    }
    if(!a[0]||a[0]!=owners.options)return NIMBY_INVALID_HANDLE;
    if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
    if(request.operation==discardOperation)
        return call<NimbyOptionsDiscardV1>("NimbyOptions_DiscardV1",a[0]);
    if(request.operation==removeOperation){
        const auto status=call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",a[0]);
        if(status==NIMBY_OK)owners.options=0;
        return status;
    }
    if(request.operation==readOperation){
        if(a[2]>NIMBY_OPTIONS_VALUES_LIMIT)return NIMBY_INVALID_ARGUMENT;
        reply.data.resize(static_cast<size_t>(a[2]));uint32_t written{};uint64_t revision{};
        const auto status=call<NimbyOptionsReadV1>("NimbyOptions_ReadV1",a[0],a[1],
            reinterpret_cast<char*>(reply.data.data()),static_cast<uint32_t>(reply.data.size()),&written,&revision);
        if(status!=NIMBY_OK){reply.data.clear();return status;}
        if(written>reply.data.size()){reply.data.clear();return NIMBY_INVALID_BINARY;}
        reply.data.resize(written);reply.args[1]=revision;return NIMBY_OK;
    }
    return NIMBY_INVALID_ARGUMENT;
}
void cleanupOptions(Owners& owners) noexcept {
    if(owners.options)call<NimbyOptionsRemoveV1>("NimbyOptions_RemoveV1",owners.options);
    owners.options=0;
}
void* optionsSymbol(const char* name) noexcept {
    if(!name)return nullptr;
    if(std::strcmp(name,"NimbyOptions_RegisterV1")==0)return std::bit_cast<void*>(&add);
    if(std::strcmp(name,"NimbyOptions_RemoveV1")==0)return std::bit_cast<void*>(&remove);
    if(std::strcmp(name,"NimbyOptions_DiscardV1")==0)return std::bit_cast<void*>(&discard);
    if(std::strcmp(name,"NimbyOptions_ReadV1")==0)return std::bit_cast<void*>(&readOptions);
    return nullptr;
}
}
