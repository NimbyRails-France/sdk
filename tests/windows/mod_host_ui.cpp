#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/signal_ui_bridge.h>
#include <windows.h>
#include <algorithm>
#include <bit>
#include <cstdio>
#include <string>

namespace {
nimby::mod_host::Owners first,second;
nimby::mod_host::Owners* channel=&first;
nimby::mod_host::Request last;
uint32_t calls{};
template<class T> T symbol(const char* name){return std::bit_cast<T>(NimbyInternal_ModHostUiSymbol(name));}
}
extern "C" uint32_t __cdecl NimbyInternal_ModHostCall(uint32_t operation,uint64_t* args,
    const void* input,uint32_t size,void* output,uint32_t capacity,uint32_t* written) noexcept {
    try {
        ++calls;*written=0;last={};last.operation=operation;std::copy_n(args,8,last.args.begin());
        if(size)last.data.assign(static_cast<const uint8_t*>(input),static_cast<const uint8_t*>(input)+size);
        nimby::mod_host::Reply reply;const auto status=nimby::mod_host::dispatchUi(last,reply,*channel);
        if(reply.data.size()>capacity)return NIMBY_RESOURCE_LIMIT;
        std::copy(reply.args.begin(),reply.args.end(),args);*written=static_cast<uint32_t>(reply.data.size());
        if(*written)std::memcpy(output,reply.data.data(),*written);
        return status;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(int argc,char** argv){
    CHECK(argc==2);const auto library=LoadLibraryA(argv[1]);CHECK(library);
    const auto add=symbol<NimbyUiRegisterV1>("NimbyUi_RegisterV1");
    const auto remove=symbol<NimbyUiRemoveV1>("NimbyUi_RemoveV1");
    const auto begin=symbol<NimbyUiBeginV1>("NimbyUi_BeginV1");
    const auto observe=symbol<NimbyUiObserveV1>("NimbyUi_ObserveV1");
    const auto read=symbol<NimbyUiReadV1>("NimbyUi_ReadV1");
    const auto batch=symbol<NimbyUiReadBatchV1>("NimbyUi_ReadBatchV1");
    const auto revision=symbol<NimbyUiSettingsRevisionV1>("NimbyUi_SettingsRevisionV1");
    const auto save=symbol<NimbyUiExportV1>("NimbyUi_ExportV1");
    const auto load=symbol<NimbyUiBeginSavedV1>("NimbyUi_BeginSavedV1");
    const auto providerAdd=symbol<NimbyUiProviderAddV1>("NimbyUi_ProviderAddV1");
    const auto providerObserve=symbol<NimbyUiProviderObserveV1>("NimbyUi_ProviderObserveV1");
    const auto providerRemove=symbol<NimbyUiProviderRemoveV1>("NimbyUi_ProviderRemoveV1");
    const auto present=symbol<NimbyUiModPresentV1>("NimbyUi_ModPresentV1");
    const auto translations=symbol<NimbyUiTranslationsV1>("NimbyUi_TranslationsV1");
    CHECK(add&&remove&&begin&&observe&&read&&batch&&revision&&save&&load&&providerAdd&&providerObserve&&providerRemove&&present&&translations);
    CHECK(!NimbyInternal_ModHostUiSymbol(nullptr));CHECK(!NimbyInternal_ModHostUiSymbol("NimbyUi_SettingsCopyBeginV1"));
    CHECK(!NimbyInternal_ModHostUiSymbol("NimbyUi_ProviderWakeV1")); // No child-supplied parent handle.
    CHECK(!NimbyInternal_ModHostUiSymbol("missing"));
    NimbyUiPanelV1 panel{};panel.size=sizeof panel;panel.version=1;panel.count=1;
    std::strcpy(panel.id,"isolated-first");std::strcpy(panel.title,"First");std::strcpy(panel.texture_set,"first.atlas");
    std::strcpy(panel.checkboxes[0].name,"active");std::strcpy(panel.checkboxes[0].label,"Active");panel.checkboxes[0].default_value=1;
    uint64_t owner{},session{};CHECK(add(&panel,&owner)==NIMBY_OK&&owner&&first.panels.contains(owner));
    CHECK(begin(owner,"test-world",10,&session)==NIMBY_OK&&session);
    uint64_t beforeRevision{};CHECK(revision(owner,session,&beforeRevision)==NIMBY_OK&&beforeRevision);
    CHECK(last.operation==124&&last.data.empty());
    NimbyUiSignalV1 signal{};signal.id=0x8000000000001ULL;std::strcpy(signal.texture_set,"first.atlas");
    CHECK(observe(owner,session,&signal,1)==NIMBY_OK);
    NimbyUiValuesV1 values{};values.size=sizeof values;values.version=1;
    CHECK(read(owner,signal.id,&values)==NIMBY_OK&&values.status==2&&values.count==1&&values.fields[0].value==1);
    const auto validRead=last;
    std::vector<uint64_t> ids(512,signal.id);
    std::vector<NimbyUiReadBatchRowV1> compact(ids.size());
    NimbyUiReadBatchHeaderV1 header{};header.size=sizeof header;header.version=1;
    const auto beforeBatch=calls;
    CHECK(batch(owner,ids.data(),static_cast<uint32_t>(ids.size()),&header,compact.data())==NIMBY_OK&&calls==beforeBatch+1);
    CHECK(header.count==ids.size()&&header.field_count==1&&std::string(header.names[0])=="active");
    CHECK(std::all_of(compact.begin(),compact.end(),[&](auto row){return row.signal==signal.id&&row.status==2&&row.values==1;}));
    CHECK(sizeof header+compact.size()*sizeof(compact[0])<32*1024);
    uint32_t needed{};CHECK(save(owner,session,nullptr,0,&needed)==NIMBY_OK&&needed);
    std::vector<char> bytes(needed);uint32_t written{};
    CHECK(save(owner,session,bytes.data(),needed,&written)==NIMBY_OK&&written==needed);
    const auto saved=bytes;std::fill(bytes.begin(),bytes.end(),'x');
    CHECK(save(owner,session,bytes.data(),needed-1,&written)==NIMBY_RESOURCE_LIMIT&&written==needed);
    CHECK(std::all_of(bytes.begin(),bytes.end(),[](char value){return value=='x';}));
    CHECK(load(owner,"test-world",10,saved.data(),static_cast<uint32_t>(saved.size()),&session)==NIMBY_OK);
    uint64_t afterRevision{};CHECK(revision(owner,session,&afterRevision)==NIMBY_OK&&afterRevision>beforeRevision);
    CHECK(observe(owner,session,&signal,1)==NIMBY_OK);
    CHECK(read(owner,signal.id,&values)==NIMBY_OK&&values.fields[0].value==1);
    channel=&second;
    CHECK(revision(owner,session,&afterRevision)==NIMBY_INVALID_HANDLE&&afterRevision==0);
    CHECK(read(owner,signal.id,&values)==NIMBY_INVALID_HANDLE&&values.count==0);
    CHECK(batch(owner,ids.data(),static_cast<uint32_t>(ids.size()),&header,compact.data())==NIMBY_INVALID_HANDLE&&header.count==0);
    CHECK(remove(owner)==NIMBY_INVALID_HANDLE&&first.panels.contains(owner));
    const char invalidJson[]="{}";
    CHECK(translations(0,owner,invalidJson,2)==NIMBY_INVALID_HANDLE);
    nimby::mod_host::Reply reply;
    CHECK(nimby::mod_host::dispatchUi(validRead,reply,second)==NIMBY_INVALID_HANDLE);
    std::strcpy(panel.id,"isolated-second");std::strcpy(panel.texture_set,"second.atlas");uint64_t other{};
    CHECK(add(&panel,&other)==NIMBY_OK&&other!=owner&&second.panels.contains(other));
    NimbyUiProviderV1 provider{};provider.size=sizeof provider;provider.version=1;provider.count=1;
    std::strcpy(provider.id,"isolated-tool");std::strcpy(provider.services[0],"repeat.v1");uint64_t token{};
    second.actionWake=UINT64_MAX;
    CHECK(providerAdd(&provider,&token)==NIMBY_INVALID_HANDLE&&!token&&second.providers.empty());
    uint32_t missing{};CHECK(present("isolated-tool",13,&missing)==NIMBY_OK&&!missing);
    const auto parentWake=CreateEventW(nullptr,FALSE,FALSE,nullptr);CHECK(parentWake);
    second.actionWake=reinterpret_cast<uintptr_t>(parentWake);
    CHECK(providerAdd(&provider,&token)==NIMBY_OK&&second.providers.contains(token));
    // The bridge owns its duplicate; closing the channel's original cannot
    // make a delayed notification reuse an unrelated handle.
    CloseHandle(parentWake);second.actionWake=0;
    CHECK(providerObserve(token,"test-world",10,1)==NIMBY_OK);
    uint32_t exists{};CHECK(present("isolated-tool",13,&exists)==NIMBY_OK&&exists==1);
    channel=&first;
    CHECK(providerRemove(token)==NIMBY_INVALID_HANDLE&&second.providers.contains(token));
    CHECK(translations(1,token,invalidJson,2)==NIMBY_INVALID_HANDLE);
    // Public stubs reject counts before allocating or submitting any request.
    const auto before=calls;CHECK(observe(owner,session,&signal,16385)==NIMBY_INVALID_ARGUMENT&&calls==before);
    auto malformed=validRead;malformed.data.pop_back();
    CHECK(nimby::mod_host::dispatchUi(malformed,reply,first)==NIMBY_INVALID_ARGUMENT);
    // The broker independently checks a corrupt child packet and its token.
    CHECK(observe(owner,session,&signal,1)==NIMBY_OK);malformed=last;malformed.args[2]=UINT64_MAX;
    CHECK(nimby::mod_host::dispatchUi(malformed,reply,first)==NIMBY_INVALID_ARGUMENT);
    nimby::mod_host::Request unknown{};unknown.operation=199;
    CHECK(nimby::mod_host::dispatchUi(unknown,reply,first)==NIMBY_HOOKS_UNAVAILABLE);
    // Fault cleanup from one process cannot unregister the other mod's UI.
    nimby::mod_host::cleanupUi(second);CHECK(second.panels.empty()&&second.providers.empty());
    CHECK(present("isolated-tool",13,&exists)==NIMBY_OK&&exists==0);
    CHECK(read(owner,signal.id,&values)==NIMBY_OK&&values.status==2&&values.count==1);
    CHECK(first.panels.contains(owner));
    nimby::mod_host::cleanupUi(first);CHECK(first.panels.empty());
    CHECK(read(owner,signal.id,&values)==NIMBY_INVALID_HANDLE&&values.count==0);
    CHECK(FreeLibrary(library));
    std::puts("PASS isolated UI facade: roundtrip, buffer contracts, per-channel ownership, malformed packets and fault cleanup");
}
