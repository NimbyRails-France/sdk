#include <platform/windows/mod_host_protocol.h>
#include <nimby/detail/signal_ui_bridge.h>
#include <nimby/detail/diagnostics.hpp>
#include <windows.h>
#include <bit>
#include <new>

namespace nimby::mod_host {
namespace {
enum Operation : uint32_t {
    Register=100,Remove,ConditionalVisibility,Numbers,Begin,Observe,Suspend,Read,
    Export,BeginSaved,Actions,PanelContext,ProviderAdd,ProviderRemove,
    ProviderObserve,ProviderSuspend,ProviderPoll,Present,ToolPanel,ToolPanelV2,
    ProviderPollV2,Preview,Translations,ReadBatch,SettingsRevision
};
constexpr uint32_t settingsLimit=16*1024*1024,signalLimit=16384,translationLimit=1024*1024;
template<class F> uint32_t boundary(F&& fn) noexcept {
    try{return fn();}catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}
    catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}catch(...){return NIMBY_INTERNAL_ERROR;}
}
template<class T> T native(const char* name) {
    const auto module=GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll");
    return module?std::bit_cast<T>(GetProcAddress(module,name)):nullptr;
}
template<class T,class... Args> uint32_t call(const char* name,Args... args) {
    const auto fn=native<T>(name);return fn?fn(args...):NIMBY_HOOKS_UNAVAILABLE;
}
bool owns(const Owners& owners,uint64_t token,bool provider=false) {
    return provider?owners.providers.contains(token):owners.panels.contains(token);
}
template<class T> bool exact(const Request& request,T& value) {
    return request.data.size()==sizeof(T)&&read(request,0,value);
}
template<class T> bool rows(const Request& request,uint64_t count,uint32_t limit,std::vector<T>& out) {
    if(count>limit||count>payloadLimit/sizeof(T)||request.data.size()!=count*sizeof(T))return false;
    out.resize(static_cast<size_t>(count));
    if(count)std::memcpy(out.data(),request.data.data(),request.data.size());
    return true;
}
template<class T> uint32_t response(const Reply& reply,T* out,uint32_t status) {
    if(!out)return NIMBY_INVALID_ARGUMENT;
    if(reply.data.size()==sizeof(T))std::memcpy(out,reply.data.data(),sizeof(T));
    else if(status==NIMBY_OK)return NIMBY_INVALID_BINARY;
    return status;
}
uint32_t simple(Operation op,uint64_t a=0,uint64_t b=0,uint64_t c=0) noexcept {
    return boundary([&]{Request request{};request.operation=op;request.args={a,b,c};Reply reply;return invoke(request,reply,0);});
}
template<class T> uint32_t add(Operation op,const T* value,uint64_t* token) noexcept {
    if(token)*token=0;
    if(!value||!token)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=op;append(request,value);Reply reply;const auto status=invoke(request,reply,0);
        if(status==NIMBY_OK)*token=reply.args[0];
        return status;});
}
template<class T> uint32_t array(Operation op,uint64_t owner,uint64_t session,const T* values,uint32_t count,uint32_t limit) noexcept {
    if(count>limit||(!values&&count))return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=op;request.args={owner,session,count};append(request,values,count);
        Reply reply;return invoke(request,reply,0);});
}
template<class T> uint32_t publish(Operation op,uint64_t provider,const T* value) noexcept {
    if(!value)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=op;request.args[0]=provider;append(request,value);
        Reply reply;return invoke(request,reply,0);});
}
template<class T> uint32_t receive(Operation op,uint64_t owner,uint64_t id,T* out) noexcept {
    if(!out)return NIMBY_INVALID_ARGUMENT;
    // Only size/version are input. Do not ship previous results or leave them
    // looking fresh after a dead channel or an invalid owner.
    T empty{};std::memcpy(&empty,out,2*sizeof(uint32_t));*out=empty;
    return boundary([&]{Request request{};request.operation=op;request.args={owner,id};append(request,&empty);
        Reply reply;const auto status=invoke(request,reply,sizeof(T));return response(reply,out,status);});
}
uint32_t registerPanel(const NimbyUiPanelV1* p,uint64_t* out) noexcept {return add(Register,p,out);}
uint32_t removePanel(uint64_t t) noexcept{return simple(Remove,t);}
uint32_t visibility(uint64_t t,uint64_t mask) noexcept{return simple(ConditionalVisibility,t,mask);}
uint32_t numbers(uint64_t t,const NimbyUiNumberSettingV1* p,uint32_t n) noexcept{return array(Numbers,t,0,p,n,4);}
uint32_t begin(uint64_t t,const char* id,uint32_t n,uint64_t* out) noexcept {
    if(out)*out=0;
    if(!out||!id||!n||n>512)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=Begin;request.args[0]=t;append(request,id,n);Reply reply;
        const auto status=invoke(request,reply,0);if(status==NIMBY_OK)*out=reply.args[0];return status;});
}
uint32_t observe(uint64_t t,uint64_t session,const NimbyUiSignalV1* p,uint32_t n) noexcept{return array(Observe,t,session,p,n,signalLimit);}
uint32_t suspend(uint64_t t) noexcept{return simple(Suspend,t);}
uint32_t readValues(uint64_t t,uint64_t signal,NimbyUiValuesV1* out) noexcept{return receive(Read,t,signal,out);}
uint32_t settingsRevision(uint64_t t,uint64_t session,uint64_t* out) noexcept {
    if(out)*out=0;
    if(!out||!session)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=SettingsRevision;request.args={t,session};Reply reply;
        const auto status=invoke(request,reply,0);if(status==NIMBY_OK)*out=reply.args[0];return status;});
}
uint32_t readBatch(uint64_t owner,const uint64_t* ids,uint32_t count,NimbyUiReadBatchHeaderV1* header,NimbyUiReadBatchRowV1* rows) noexcept {
    if(!header||header->size!=sizeof(*header)||header->version!=1||count>signalLimit||(!ids&&count)||(!rows&&count))return NIMBY_INVALID_ARGUMENT;
    *header={};header->size=sizeof(*header);header->version=1;
    return boundary([&]{Request request{};request.operation=ReadBatch;request.args={owner,count};append(request,ids,count);
        Reply reply;const auto expected=sizeof(*header)+count*sizeof(*rows);
        const auto status=invoke(request,reply,static_cast<uint32_t>(expected));if(status!=NIMBY_OK)return status;
        if(reply.data.size()!=expected)return uint32_t(NIMBY_INVALID_BINARY);
        std::memcpy(header,reply.data.data(),sizeof(*header));
        if(count)std::memcpy(rows,reply.data.data()+sizeof(*header),count*sizeof(*rows));
        return status;});
}
uint32_t exportSettings(uint64_t t,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written) noexcept {
    if(written)*written=0;
    if(!written||capacity>settingsLimit||(!bytes&&capacity))return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=Export;request.args={t,session,capacity};Reply reply;
        const auto status=invoke(request,reply,capacity);if(reply.args[0]>settingsLimit)return uint32_t(NIMBY_INVALID_BINARY);
        *written=static_cast<uint32_t>(reply.args[0]);
        if(status==NIMBY_OK&&capacity){if(reply.data.size()!=*written||*written>capacity)return uint32_t(NIMBY_INVALID_BINARY);
            if(*written)std::memcpy(bytes,reply.data.data(),*written);}return status;});
}
uint32_t beginSaved(uint64_t t,const char* id,uint32_t idSize,const char* bytes,uint32_t size,uint64_t* out) noexcept {
    if(out)*out=0;
    if(!out||!id||!idSize||idSize>512||!bytes||!size||size>settingsLimit)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=BeginSaved;request.args={t,idSize,size};append(request,id,idSize);append(request,bytes,size);
        Reply reply;const auto status=invoke(request,reply,0);if(status==NIMBY_OK)*out=reply.args[0];return status;});
}
uint32_t actions(uint64_t t,const NimbyUiActionV1* p,uint32_t n) noexcept{return array(Actions,t,0,p,n,16);}
uint32_t panelContext(uint64_t t,uint64_t session,uint64_t generation) noexcept{return simple(PanelContext,t,session,generation);}
uint32_t providerAdd(const NimbyUiProviderV1* p,uint64_t* out) noexcept{return add(ProviderAdd,p,out);}
uint32_t providerRemove(uint64_t t) noexcept{return simple(ProviderRemove,t);}
uint32_t providerObserve(uint64_t t,const char* world,uint32_t size,uint64_t generation) noexcept {
    if(!world||!size||size>512)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=ProviderObserve;request.args={t,generation};append(request,world,size);
        Reply reply;return invoke(request,reply,0);});
}
uint32_t providerSuspend(uint64_t t) noexcept{return simple(ProviderSuspend,t);}
uint32_t poll(uint64_t t,NimbyUiActionEventV1* out) noexcept{return receive(ProviderPoll,t,0,out);}
uint32_t pollV2(uint64_t t,NimbyUiActionEventV2* out) noexcept{return receive(ProviderPollV2,t,0,out);}
uint32_t present(const char* id,uint32_t length,uint32_t* out) noexcept {
    if(out)*out=0;
    if(!out||!id||!length||length>128)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=Present;append(request,id,length);Reply reply;const auto status=invoke(request,reply,0);
        if(status==NIMBY_OK){if(reply.args[0]>1)return uint32_t(NIMBY_INVALID_BINARY);*out=static_cast<uint32_t>(reply.args[0]);}return status;});
}
uint32_t toolPanel(uint64_t t,const NimbyUiToolPanelV1* p) noexcept{return publish(ToolPanel,t,p);}
uint32_t toolPanelV2(uint64_t t,const NimbyUiToolPanelV2* p) noexcept{return publish(ToolPanelV2,t,p);}
uint32_t preview(uint64_t t,const NimbyUiSignalPreviewV1* p) noexcept{return publish(Preview,t,p);}
uint32_t translations(uint32_t kind,uint64_t t,const char* bytes,uint32_t size) noexcept {
    if(kind>1||!bytes||!size||size>translationLimit)return NIMBY_INVALID_ARGUMENT;
    return boundary([&]{Request request{};request.operation=Translations;request.args={t,kind};append(request,bytes,size);
        Reply reply;return invoke(request,reply,0);});
}
}

uint32_t dispatchUi(const Request& request,Reply& reply,Owners& owners) {
    if(request.operation>=150&&request.operation<=153)return dispatchOptions(request,reply,owners);
    return boundary([&]() -> uint32_t {
        reply={};const auto& a=request.args;
        if(request.data.size()>payloadLimit)return NIMBY_RESOURCE_LIMIT;
        const bool provider=(request.operation>=ProviderRemove&&request.operation<=ProviderPoll)||
            request.operation==ToolPanel||request.operation==ToolPanelV2||request.operation==ProviderPollV2||request.operation==Preview;
        const bool panel=(request.operation>=Remove&&request.operation<=PanelContext)||request.operation==ReadBatch||request.operation==SettingsRevision;
        if((panel||provider)&&!owns(owners,a[0],provider)){
            // This guard runs before the resident endpoint. Record it too so
            // an invalid channel owner cannot look like a silent preview failure.
            // Owners are private to this serialized broker channel; no shared
            // action/store lock is held here. The sink bounds repeated reports.
            char message[256]{};
            std::snprintf(message,sizeof message,
                "Signal UI ownership rejected: status=%u reason=channel_%s_not_owned operation=%u token=%llu panels=%zu providers=%zu",
                uint32_t(NIMBY_INVALID_HANDLE),provider?"provider":"panel",request.operation,
                static_cast<unsigned long long>(a[0]),owners.panels.size(),owners.providers.size());
            detail::diagnostics::write("loader","WARN",message);
            return NIMBY_INVALID_HANDLE;
        }
        switch(request.operation) {
        case Register: {
            NimbyUiPanelV1 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            if(owners.panels.size()>=16)return NIMBY_RESOURCE_LIMIT;
            uint64_t token{};const auto status=call<NimbyUiRegisterV1>("NimbyUi_RegisterV1",&value,&token);
            if(status==NIMBY_OK&&token){try{owners.panels.insert(token);}catch(...){call<NimbyUiRemoveV1>("NimbyUi_RemoveV1",token);throw;}reply.args[0]=token;}
            return status;
        }
        case ProviderAdd: {
            NimbyUiProviderV1 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            if(owners.providers.size()>=16)return NIMBY_RESOURCE_LIMIT;
            uint64_t token{};const auto status=call<NimbyUiProviderAddV1>("NimbyUi_ProviderAddV1",&value,&token);
            if(status==NIMBY_OK&&token){
                // Only the broker's canonical channel event may be bound. This
                // entry point is deliberately absent from the child facade.
                using Bind=uint32_t(__cdecl*)(uint64_t,uint64_t);
                if(owners.actionWake){
                    const auto bound=call<Bind>("NimbyUi_ProviderWakeV1",token,owners.actionWake);
                    if(bound!=NIMBY_OK){call<NimbyUiProviderRemoveV1>("NimbyUi_ProviderRemoveV1",token);return bound;}
                }
                try{owners.providers.insert(token);}catch(...){call<NimbyUiProviderRemoveV1>("NimbyUi_ProviderRemoveV1",token);throw;}
                reply.args[0]=token;
            }
            return status;
        }
        case Remove:case ProviderRemove: {
            if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
            const auto status=request.operation==Remove?call<NimbyUiRemoveV1>("NimbyUi_RemoveV1",a[0]):
                call<NimbyUiProviderRemoveV1>("NimbyUi_ProviderRemoveV1",a[0]);
            if(status==NIMBY_OK||status==NIMBY_INVALID_HANDLE)(provider?owners.providers:owners.panels).erase(a[0]);
            return status;
        }
        case ConditionalVisibility:
            if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiConditionalVisibilityV1>("NimbyUi_ConditionalVisibilityV1",a[0],a[1]);
        case Suspend:case ProviderSuspend:
            if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
            return request.operation==Suspend?call<NimbyUiSuspendV1>("NimbyUi_SuspendV1",a[0]):
                call<NimbyUiProviderSuspendV1>("NimbyUi_ProviderSuspendV1",a[0]);
        case PanelContext:
            if(!request.data.empty())return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiPanelContextV1>("NimbyUi_PanelContextV1",a[0],a[1],a[2]);
        case Numbers: {
            std::vector<NimbyUiNumberSettingV1> values;if(!rows(request,a[2],4,values))return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiNumberSettingsV1>("NimbyUi_NumberSettingsV1",a[0],values.data(),static_cast<uint32_t>(values.size()));
        }
        case Actions: {
            std::vector<NimbyUiActionV1> values;if(!rows(request,a[2],16,values))return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiActionsV1>("NimbyUi_ActionsV1",a[0],values.data(),static_cast<uint32_t>(values.size()));
        }
        case Observe: {
            std::vector<NimbyUiSignalV1> values;if(!rows(request,a[2],signalLimit,values))return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiObserveV1>("NimbyUi_ObserveV1",a[0],a[1],values.data(),static_cast<uint32_t>(values.size()));
        }
        case Begin: {
            if(request.data.empty()||request.data.size()>512)return NIMBY_INVALID_ARGUMENT;
            uint64_t token{};const auto status=call<NimbyUiBeginV1>("NimbyUi_BeginV1",a[0],reinterpret_cast<const char*>(request.data.data()),static_cast<uint32_t>(request.data.size()),&token);
            if(status==NIMBY_OK)reply.args[0]=token;
            return status;
        }
        case BeginSaved: {
            if(!a[1]||a[1]>512||!a[2]||a[2]>settingsLimit||request.data.size()!=a[1]+a[2])return NIMBY_INVALID_ARGUMENT;
            const auto* bytes=reinterpret_cast<const char*>(request.data.data());uint64_t token{};
            const auto status=call<NimbyUiBeginSavedV1>("NimbyUi_BeginSavedV1",a[0],bytes,static_cast<uint32_t>(a[1]),bytes+a[1],static_cast<uint32_t>(a[2]),&token);
            if(status==NIMBY_OK)reply.args[0]=token;
            return status;
        }
        case Export: {
            if(!request.data.empty()||a[2]>settingsLimit)return NIMBY_INVALID_ARGUMENT;
            std::vector<char> bytes(static_cast<size_t>(a[2]));uint32_t written{};
            const auto status=call<NimbyUiExportV1>("NimbyUi_ExportV1",a[0],a[1],bytes.empty()?nullptr:bytes.data(),static_cast<uint32_t>(bytes.size()),&written);
            if(written>settingsLimit)return NIMBY_INVALID_BINARY;
            reply.args[0]=written;
            if(status==NIMBY_OK&&!bytes.empty()){if(written>bytes.size())return NIMBY_INVALID_BINARY;output(reply,bytes.data(),written);}return status;
        }
        case Read: {
            NimbyUiValuesV1 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            const auto status=call<NimbyUiReadV1>("NimbyUi_ReadV1",a[0],a[1],&value);output(reply,&value);return status;
        }
        case SettingsRevision: {
            if(!request.data.empty()||!a[1])return NIMBY_INVALID_ARGUMENT;
            uint64_t revision{};const auto status=call<NimbyUiSettingsRevisionV1>("NimbyUi_SettingsRevisionV1",a[0],a[1],&revision);
            if(status==NIMBY_OK)reply.args[0]=revision;
            return status;
        }
        case ReadBatch: {
            std::vector<uint64_t> ids;if(!rows(request,a[1],signalLimit,ids))return NIMBY_INVALID_ARGUMENT;
            if(const auto batch=native<NimbyUiReadBatchV1>("NimbyUi_ReadBatchV1")){
                NimbyUiReadBatchHeaderV1 header{};header.size=sizeof header;header.version=1;
                std::vector<NimbyUiReadBatchRowV1> results(ids.size());
                const auto status=batch(a[0],ids.data(),static_cast<uint32_t>(ids.size()),&header,results.data());
                if(status!=NIMBY_OK)return status;
                if(header.count!=ids.size()||header.size!=sizeof header||header.version!=1||header.field_count>64)return NIMBY_INVALID_BINARY;
                reply.data.resize(sizeof header+results.size()*sizeof(NimbyUiReadBatchRowV1));
                std::memcpy(reply.data.data(),&header,sizeof header);
                if(!results.empty())std::memcpy(reply.data.data()+sizeof header,results.data(),results.size()*sizeof(NimbyUiReadBatchRowV1));
                return NIMBY_OK;
            }
            const auto reader=native<NimbyUiReadV1>("NimbyUi_ReadV1");if(!reader)return NIMBY_HOOKS_UNAVAILABLE;
            NimbyUiReadBatchHeaderV1 header{};header.size=sizeof header;header.version=1;header.count=static_cast<uint32_t>(ids.size());
            bool schema=false;std::vector<NimbyUiReadBatchRowV1> results;results.reserve(ids.size());
            for(const auto id:ids){
                if(id>>48!=8)return NIMBY_INVALID_ARGUMENT;
                NimbyUiValuesV1 value{};value.size=sizeof value;value.version=1;
                const auto status=reader(a[0],id,&value);if(status!=NIMBY_OK)return status;
                if(value.size!=sizeof value||value.version!=1||value.count>64||value.status>2)return NIMBY_INVALID_BINARY;
                NimbyUiReadBatchRowV1 row{};row.signal=id;row.status=value.status;
                if(value.status==2){
                    if(schema&&header.field_count!=value.count)return NIMBY_INVALID_BINARY;
                    if(!schema)header.field_count=value.count;
                    for(uint32_t i=0;i<value.count;++i){
                        const auto& field=value.fields[i];
                        if(!std::memchr(field.name,0,sizeof field.name)||!field.name[0]||field.value>1)return NIMBY_INVALID_BINARY;
                        if(schema&&std::strcmp(header.names[i],field.name))return NIMBY_INVALID_BINARY;
                        if(!schema)std::memcpy(header.names[i],field.name,sizeof field.name);
                        if(field.value)row.values|=uint64_t{1}<<i;
                    }
                    schema=true;
                }
                results.push_back(row);
            }
            reply.data.resize(sizeof header+results.size()*sizeof(NimbyUiReadBatchRowV1));
            std::memcpy(reply.data.data(),&header,sizeof header);
            if(!results.empty())std::memcpy(reply.data.data()+sizeof header,results.data(),results.size()*sizeof(NimbyUiReadBatchRowV1));
            return NIMBY_OK;
        }
        case ProviderObserve:
            if(request.data.empty()||request.data.size()>512)return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiProviderObserveV1>("NimbyUi_ProviderObserveV1",a[0],reinterpret_cast<const char*>(request.data.data()),static_cast<uint32_t>(request.data.size()),a[1]);
        case ProviderPoll: {
            NimbyUiActionEventV1 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            const auto status=call<NimbyUiProviderPollV1>("NimbyUi_ProviderPollV1",a[0],&value);output(reply,&value);return status;
        }
        case ProviderPollV2: {
            NimbyUiActionEventV2 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            const auto status=call<NimbyUiProviderPollV2>("NimbyUi_ProviderPollV2",a[0],&value);output(reply,&value);return status;
        }
        case Present: {
            if(request.data.empty()||request.data.size()>128)return NIMBY_INVALID_ARGUMENT;
            uint32_t present{};
            const auto status=call<NimbyUiModPresentV1>("NimbyUi_ModPresentV1",reinterpret_cast<const char*>(request.data.data()),static_cast<uint32_t>(request.data.size()),&present);
            reply.args[0]=present;return status;
        }
        case ToolPanel: {
            NimbyUiToolPanelV1 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiToolPanelPublishV1>("NimbyUi_ToolPanelPublishV1",a[0],&value);
        }
        case ToolPanelV2: {
            NimbyUiToolPanelV2 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiToolPanelPublishV2>("NimbyUi_ToolPanelPublishV2",a[0],&value);
        }
        case Preview: {
            NimbyUiSignalPreviewV1 value{};if(!exact(request,value))return NIMBY_INVALID_ARGUMENT;
            return call<NimbyUiSignalPreviewPublishV1>("NimbyUi_SignalPreviewPublishV1",a[0],&value);
        }
        case Translations:
            if(a[1]>1||request.data.empty()||request.data.size()>translationLimit)return NIMBY_INVALID_ARGUMENT;
            if(!owns(owners,a[0],a[1]==1))return NIMBY_INVALID_HANDLE;
            return call<NimbyUiTranslationsV1>("NimbyUi_TranslationsV1",static_cast<uint32_t>(a[1]),a[0],reinterpret_cast<const char*>(request.data.data()),static_cast<uint32_t>(request.data.size()));
        default:return NIMBY_HOOKS_UNAVAILABLE;
        }
    });
}
void cleanupUi(Owners& owners) noexcept {
    cleanupOptions(owners);
    for(const auto token:owners.providers)call<NimbyUiProviderRemoveV1>("NimbyUi_ProviderRemoveV1",token);
    for(const auto token:owners.panels)call<NimbyUiRemoveV1>("NimbyUi_RemoveV1",token);
    owners.providers.clear();owners.panels.clear();
}
}
extern "C" NIMBY_API void* __cdecl NimbyInternal_ModHostUiSymbol(const char* name) noexcept {
    if(!name)return nullptr;
    using namespace nimby::mod_host;
#define SYMBOL(exported,stub) if(std::strcmp(name,exported)==0)return std::bit_cast<void*>(&stub)
    SYMBOL("NimbyUi_RegisterV1",registerPanel);SYMBOL("NimbyUi_RemoveV1",removePanel);
    SYMBOL("NimbyUi_ConditionalVisibilityV1",visibility);SYMBOL("NimbyUi_NumberSettingsV1",numbers);
    SYMBOL("NimbyUi_BeginV1",begin);SYMBOL("NimbyUi_ObserveV1",observe);SYMBOL("NimbyUi_SuspendV1",suspend);
    SYMBOL("NimbyUi_ReadV1",readValues);SYMBOL("NimbyUi_ExportV1",exportSettings);SYMBOL("NimbyUi_BeginSavedV1",beginSaved);
    SYMBOL("NimbyUi_ReadBatchV1",readBatch);
    SYMBOL("NimbyUi_SettingsRevisionV1",settingsRevision);
    SYMBOL("NimbyUi_ActionsV1",actions);SYMBOL("NimbyUi_PanelContextV1",panelContext);
    SYMBOL("NimbyUi_ProviderAddV1",providerAdd);SYMBOL("NimbyUi_ProviderRemoveV1",providerRemove);
    SYMBOL("NimbyUi_ProviderObserveV1",providerObserve);SYMBOL("NimbyUi_ProviderSuspendV1",providerSuspend);
    SYMBOL("NimbyUi_ProviderPollV1",poll);SYMBOL("NimbyUi_ProviderPollV2",pollV2);SYMBOL("NimbyUi_ModPresentV1",present);
    SYMBOL("NimbyUi_ToolPanelPublishV1",toolPanel);SYMBOL("NimbyUi_ToolPanelPublishV2",toolPanelV2);
    SYMBOL("NimbyUi_SignalPreviewPublishV1",preview);SYMBOL("NimbyUi_TranslationsV1",translations);
#undef SYMBOL
    // SettingsCopyBegin/Finish are native construction handoffs, never mod APIs.
    if(const auto symbol=optionsSymbol(name))return symbol;
    return trainLengthSymbol(name);
}
