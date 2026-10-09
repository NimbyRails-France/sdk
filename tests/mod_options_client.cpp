#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/native_library.hpp>
#include <nimby/detail/platform/host.hpp>
#include <nimby/detail/vendor/json.hpp>
#include <bit>
#include <cassert>
#include <cstdio>
#include <deque>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace nimby::detail::options_test_library {
struct Library {};
using Module=Library*;
Module existing(const char*);
void* symbol(Module,const char*);
void unload(Module);
}
// Inject only native module lookup. The client, C ABI function signatures,
// response decoding, revision acknowledgement and cleanup are production code.
// The test body has no Win32 API, game process, runtime registry or storage.
#define native options_test_library
#include <nimby/detail/mod_options_client.hpp>
#undef native

namespace {
using Client=nimby::detail::ModOptionsClient;
nimby::detail::options_test_library::Library library;
bool available=true;
std::string missing;
uint32_t addStatus=NIMBY_OK,removeStatus=NIMBY_OK;
bool zeroRegistration=false;
uint64_t serial=0;
unsigned references=0,adds=0,removes=0,reads=0,discards=0;
std::map<uint64_t,std::string> owners;
std::vector<uint64_t> knownRevisions,readOwners;
struct Response {uint64_t revision;std::string bytes;uint32_t status=NIMBY_OK;bool oversized=false;};
std::deque<Response> replies;
uint32_t add(const char* data,uint32_t bytes,uint64_t* owner){
    ++adds;*owner=0;
    if(addStatus!=NIMBY_OK||zeroRegistration)return addStatus;
    *owner=++serial;owners.emplace(*owner,std::string(data,bytes));return NIMBY_OK;
}
uint32_t remove(uint64_t owner){
    ++removes;
    if(removeStatus!=NIMBY_OK)return removeStatus;
    return owners.erase(owner)?NIMBY_OK:NIMBY_INVALID_ARGUMENT;
}
uint32_t discard(uint64_t owner){
    assert(owners.contains(owner));++discards;return NIMBY_OK;
}
uint32_t read(uint64_t owner,uint64_t known,char* output,uint32_t capacity,uint32_t* written,uint64_t* revision){
    assert(owners.contains(owner));assert(!replies.empty());
    ++reads;knownRevisions.push_back(known);readOwners.push_back(owner);
    auto reply=std::move(replies.front());replies.pop_front();
    *written=0;*revision=reply.revision;
    if(reply.status!=NIMBY_OK)return reply.status;
    if(reply.oversized){*written=capacity+1;return NIMBY_OK;}
    assert(reply.bytes.size()<=capacity);
    std::memcpy(output,reply.bytes.data(),reply.bytes.size());*written=uint32_t(reply.bytes.size());return NIMBY_OK;
}
template<class F> bool rejects(F&& f){try{f();return false;}catch(const std::exception&){return true;}}
void response(uint64_t revision,std::string bytes){replies.push_back({revision,std::move(bytes)});}
}

namespace nimby::detail::options_test_library {
Module existing(const char* name){
    assert(std::string_view(name)==platform::signalUiLibrary);
    if(!available)return nullptr;
    ++references;return &library;
}
void* symbol(Module module,const char* name){
    assert(module==&library);
    if(name==missing)return nullptr;
    if(std::string_view(name)=="NimbyOptions_RegisterV1")return std::bit_cast<void*>(NimbyOptionsRegisterV1(&add));
    if(std::string_view(name)=="NimbyOptions_RemoveV1")return std::bit_cast<void*>(NimbyOptionsRemoveV1(&remove));
    if(std::string_view(name)=="NimbyOptions_ReadV1")return std::bit_cast<void*>(NimbyOptionsReadV1(&read));
    if(std::string_view(name)=="NimbyOptions_DiscardV1")return std::bit_cast<void*>(NimbyOptionsDiscardV1(&discard));
    return nullptr;
}
void unload(Module module){assert(module==&library&&references);--references;}
}

int main(){
    static_assert(!std::is_copy_constructible_v<Client> && !std::is_copy_assignable_v<Client>);
    static_assert(!std::is_move_constructible_v<Client> && !std::is_move_assignable_v<Client>);
    const std::string schema=R"({"id":"fixture","fields":[]})";
    Client client;
    assert(!client.connect("")&&!client.connected()&&!references&&!adds);
    assert(!client.connect(std::string(NIMBY_OPTIONS_SCHEMA_LIMIT+1,'x'))&&!references&&!adds);
    available=false;assert(!client.connect(schema)&&!references&&!adds);available=true;
    for(const auto* name:{"NimbyOptions_RegisterV1","NimbyOptions_RemoveV1","NimbyOptions_ReadV1","NimbyOptions_DiscardV1"}){
        missing=name;assert(!client.connect(schema)&&!references&&!adds);
    }
    missing.clear();addStatus=NIMBY_INVALID_ARGUMENT;
    assert(!client.connect(schema)&&!client.connected()&&!references&&owners.empty());
    addStatus=NIMBY_OK;zeroRegistration=true;
    assert(!client.connect(schema)&&!client.connected()&&!references&&owners.empty());zeroRegistration=false;
    assert(client.connect(schema)&&client.connected()&&references==1&&owners.size()==1);
    const auto firstOwner=owners.begin()->first;
    const auto registeredAdds=adds;
    assert(client.connect(schema)&&adds==registeredAdds&&references==1);

    // The first response must contain a complete valid snapshot. None of these
    // failures may acknowledge revision 10 or expose an event to the adapter.
    for(const auto& malformed:std::vector<std::string>{"", "{", "[]", "null", "{}", R"({"events":["window.main"]})",
            R"({"values":[false]})",R"({"values":["bad\u0000value"]})",R"({"values":["true"],"events":[9]})",
            std::string("{\"values\":[\"")+char(0xff)+"\"]}"}){
        response(10,malformed);assert(rejects([&]{client.refresh();})&&knownRevisions.back()==0);
    }
    response(10,R"({"values":["true","F8"],"events":["window.main"]})");
    unsigned validateCalls=0;
    assert(rejects([&]{client.refresh([&](const auto&){++validateCalls;throw std::invalid_argument("Rejected changed values");});}));
    assert(validateCalls==1&&knownRevisions.back()==0);
    response(10,R"({"values":["true","F8"]})");
    auto changes=client.refresh();assert(changes.changed&&changes.values==std::vector<std::string>({"true","F8"})&&changes.events.empty());
    assert(knownRevisions.back()==0&&readOwners.back()==firstOwner);

    // Event-only reads are legal at the same values revision. A second empty
    // response must not replay the event returned by the preceding read.
    response(10,R"({"events":["window.main"]})");
    changes=client.refresh();assert(!changes.changed&&changes.values.empty()&&changes.events==std::vector<std::string>{"window.main"});
    response(10,"");changes=client.refresh();assert(!changes.changed&&changes.events.empty()&&knownRevisions.back()==10);
    client.discardEvents();assert(discards==1);
    response(10,"");client.refresh();assert(knownRevisions.back()==10);

    // A rejected update can be retried, but its already-read input event must
    // never be replayed from a client-side cache when that update succeeds.
    response(11,R"({"values":["false","F9"],"events":["window.main"]})");
    assert(rejects([&]{client.refresh([](const auto&){throw std::invalid_argument("out of range");});}));
    response(11,R"({"values":["false","F9"]})");changes=client.refresh();
    assert(knownRevisions.back()==10&&changes.changed&&changes.events.empty());
    for(const auto& reply:std::vector<Response>{{10,R"({"values":["true"]})"},{0,""},{12,""},{12,R"({"events":[]})"},
            {12,R"({"values":["true"]})",NIMBY_IO_ERROR},{12,"",NIMBY_OK,true}}){
        replies.push_back(reply);assert(rejects([&]{client.refresh();})&&knownRevisions.back()==11);
    }
    response(12,R"({"values":["true"],"events":["window.main"]})");changes=client.refresh();
    assert(changes.changed&&changes.events.size()==1&&knownRevisions.back()==11);

    // A transient remove failure retains the token/reference for a real retry.
    removeStatus=NIMBY_IO_ERROR;assert(!client.close()&&client.connected()&&references==1&&owners.contains(firstOwner));
    assert(client.connect(schema)&&adds==registeredAdds);
    response(12,"");client.refresh();assert(readOwners.back()==firstOwner&&knownRevisions.back()==12);
    removeStatus=NIMBY_OK;assert(client.close()&&!client.connected()&&!references&&owners.empty());
    const auto removedCalls=removes,readCalls=reads,discardCalls=discards;
    assert(client.close()&&removes==removedCalls);client.discardEvents();assert(discards==discardCalls);
    changes=client.refresh();assert(!changes.changed&&changes.events.empty()&&reads==readCalls);
    assert(client.connect(schema)&&owners.size()==1&&owners.begin()->first!=firstOwner);
    response(1,R"({"values":["false"]})");changes=client.refresh();
    assert(knownRevisions.back()==0&&changes.changed&&changes.values==std::vector<std::string>{"false"});
    assert(client.close()&&owners.empty()&&!references);

    {Client automatic;assert(automatic.connect(schema));}
    assert(owners.empty()&&!references);
    {Client retiring;assert(retiring.connect(schema));removeStatus=NIMBY_IO_ERROR;}
    // Parent cleanup owns the last resort after host destruction; even then
    // the client must not leak its loader reference or perform double cleanup.
    assert(owners.size()==1&&!references);owners.clear();removeStatus=NIMBY_OK;
    assert(replies.empty());
    std::puts("PASS options client: handshake, immutable revisions, event-only delivery, retry, reconnect and ownership");
}
