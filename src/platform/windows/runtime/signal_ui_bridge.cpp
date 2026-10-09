#include <platform/windows/bridge_installation.h>
#include <nimby/detail/diagnostics.hpp>
// Experimental resident renderer for the verified NIMBY Rails binary.
// Registration does not enable a panel: a real session and a complete observed
// signal catalog must first be supplied by the SDK observation worker.
#include "runtime/signal_ui_endpoint.h"
#include "runtime/signal_preview_diagnostics.h"
#include "runtime/signal_settings_panel.h"
#include "engine/binary_identity.h"
#include "platform/windows/signal_preview.h"
#include "platform/windows/game_language.h"
#include <platform/windows/mod_options_host.h>
#include <platform/windows/mod_options_ui.h>
#include <MinHook.h>
#include <array>

namespace {
using Body=void(*)(uint64_t,uint64_t);
Body original{};
uint64_t base{};
bool enabled=false;
SRWLOCK initialization=SRWLOCK_INIT;
nimby::runtime::SignalUiEndpoint endpoint;
nimby::runtime::SignalPreviewDiagnostics previewDiagnostics;
class ProviderEvent final:public nimby::runtime::SignalActions::ActionWake {
    HANDLE event_{};
public:
    explicit ProviderEvent(HANDLE event):event_(event){}
    ~ProviderEvent()override{CloseHandle(event_);}
    void notify()const noexcept override{SetEvent(event_);}
};
namespace preview=nimby::platform::windows::signal_preview;
preview::ViewportDraw originalViewport{};
// Native pointers never cross into the common registry or into mod code.
SRWLOCK previewLock=SRWLOCK_INIT;
uint64_t previewRoot{},previewDatabase{},previewSimulation{},previewPublication{};
uint64_t selectedSignal{};
std::chrono::steady_clock::time_point selectedUntil{};
bool readMemory(void*,uint64_t at,void* out,size_t bytes) {
    SIZE_T got{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(at),out,bytes,&got)&&got==bytes;
}
int64_t textInputClock(){
    LARGE_INTEGER now{};
    if(!QueryPerformanceCounter(&now))throw std::runtime_error("Windows text-input clock unavailable");
    return now.QuadPart;
}
void render(uint64_t capture,uint64_t declaration) {
    original(capture,declaration);
    thread_local nimby::runtime::SignalUiPresentation presentation;
    try {
        const auto language=nimby::platform::windows::gameLanguage(readMemory,nullptr,base).value_or("");
        thread_local std::optional<std::string> previousLanguage;
        if(!previousLanguage||*previousLanguage!=language){
            nimby::detail::diagnostics::write("sdk","INFO",("Mod UI language="+(language.empty()?std::string("unavailable; per-mod fallback"):language)).c_str());
            previousLanguage=language;
        }
        const auto drawn=nimby::runtime::draw_signal_settings_panels(readMemory,nullptr,base,capture,declaration,endpoint.host,presentation,textInputClock,
            language);
        const auto selected=drawn?nimby::engine::SignalUi::editorSignal(readMemory,nullptr,capture).value_or(0):0;
        if(TryAcquireSRWLockExclusive(&previewLock)){
            selectedSignal=selected;selectedUntil=std::chrono::steady_clock::now()+std::chrono::milliseconds(250);
            ReleaseSRWLockExclusive(&previewLock);
        }
        if(presentation.takeFailedWrites())OutputDebugStringA("NIMBY SDK: signal checkbox write rejected by storage failure\n");
    }catch(...) { nimby::detail::diagnostics::exception("sdk", __func__); 
        // Never unwind a C++ exception into the game's native UI.
        OutputDebugStringA("NIMBY SDK: signal settings rendering failed\n");
    }
}
void renderViewport(uint64_t renderer,uint64_t camera,uint64_t scene,uint64_t context,uint64_t simulationView,
        uint64_t options,uint64_t visibility) {
    originalViewport(renderer,camera,scene,context,simulationView,options,visibility);
    try {
        nimby::platform::windows::signal_preview::BorrowedFrame frame(readMemory,nullptr);
        std::optional<nimby::runtime::SignalActions::Preview> value;
        uint64_t expectedRoot{},expectedDatabase{},expectedSimulation{},expectedPublication{},selected{};
        {
            if(!TryAcquireSRWLockShared(&previewLock))return;
            struct Unlock{~Unlock(){ReleaseSRWLockShared(&previewLock);}} unlock;
            if(std::chrono::steady_clock::now()>=selectedUntil)return;
            selected=selectedSignal;expectedPublication=previewPublication;
            expectedRoot=previewRoot;expectedDatabase=previewDatabase;expectedSimulation=previewSimulation;
        }
        value=endpoint.host.actions->preview(selected);
        struct Trace {uint64_t root{},source{};size_t requested{},submitted{};};
        thread_local Trace last;
        // Publication retries run outside previewLock. A serial binds the
        // borrowed native roots to the exact owned drawing; a racing commit
        // skips this frame instead of mixing worlds or waiting in the renderer.
        if(!value||value->publication!=expectedPublication){last={};return;}
        uint64_t root{},db{},simulation{},drawDb{},rules{},routes{};
        const auto databaseRef=context+0x428,schedule=context+0x890;
        if(!frame.get(base+0xb81998,root)||root!=expectedRoot||!root||!frame.get(root+0x540,db)||db!=expectedDatabase||
            !frame.get(root+0x680,simulation)||simulation!=expectedSimulation||!frame.get(databaseRef,drawDb)||!drawDb||
            !frame.get(context+0x408,rules)||!rules||!frame.get(context+0x420,routes)||!routes)return;
        const auto draw=reinterpret_cast<preview::TemporaryDraw>(base+preview::temporaryDrawRva);
        // The viewport owns its DB view (render context +0x428), which need
        // not be the live root DB. Resolve complete IDs in that same view.
        const auto submitted=frame.draw(drawDb,options,value->signal,value->positions,[&](uint64_t ghost){
            // 0x0a is the native NewSignalEditor drawing mode, confirmed at
            // 0x78361d. Positioning, model texture, zoom and clipping stay native.
            draw(renderer,camera,scene,rules,routes,databaseRef,schedule,0,0x0a,ghost);
        });
        if(last.root!=root||last.source!=value->signal||last.requested!=value->positions.size()||last.submitted!=submitted){
            char message[240]{};
            std::snprintf(message,sizeof message,"Signal preview viewport: source=%llu requested=%zu submitted=%zu (native camera clipping applies)",
                static_cast<unsigned long long>(value->signal),value->positions.size(),submitted);
            nimby::detail::diagnostics::write("sdk","INFO",message);
            last={root,value->signal,value->positions.size(),submitted};
        }
    }catch(...){nimby::detail::diagnostics::exception("sdk","signal preview render");}
}
}
#define UI_EXPORT extern "C" __declspec(dllexport) uint32_t
UI_EXPORT NimbyUi_NumberSettingsV1(uint64_t owner,const NimbyUiNumberSettingV1* fields,uint32_t count) noexcept {return endpoint.numbers(owner,fields,count);}
UI_EXPORT NimbyUi_SettingsCopyBeginV1(uint64_t source,uint64_t* token) noexcept {return endpoint.beginCopy(source,token);}
UI_EXPORT NimbyUi_SettingsCopyFinishV1(uint64_t token,const uint64_t* ids,uint32_t count) noexcept {return endpoint.finishCopy(token,ids,count);}
UI_EXPORT NimbyUi_SettingsRevisionV1(uint64_t owner,uint64_t session,uint64_t* revision) noexcept {return endpoint.settingsRevision(owner,session,revision);}
UI_EXPORT NimbyUi_RegisterV1(const NimbyUiPanelV1* panel,uint64_t* owner) noexcept {return endpoint.add(panel,owner);}
UI_EXPORT NimbyUi_TranslationsV1(uint32_t kind,uint64_t token,const char* json,uint32_t bytes) noexcept {return endpoint.translations(kind,token,json,bytes);}
UI_EXPORT NimbyUi_RemoveV1(uint64_t owner) noexcept {return endpoint.remove(owner);}
UI_EXPORT NimbyUi_ActionsV1(uint64_t owner,const NimbyUiActionV1* actions,uint32_t count) noexcept {return endpoint.actions(owner,actions,count);}
UI_EXPORT NimbyUi_PanelContextV1(uint64_t owner,uint64_t session,uint64_t generation) noexcept {return endpoint.panelContext(owner,session,generation);}
UI_EXPORT NimbyUi_ProviderAddV1(const NimbyUiProviderV1* provider,uint64_t* token) noexcept {return endpoint.addProvider(provider,token);}
// Called only by the resident broker for its freshly-created provider. The
// child facade deliberately exposes no way to bind a handle in this process.
UI_EXPORT NimbyUi_ProviderWakeV1(uint64_t token,uint64_t parentEvent) noexcept {
    if(!token||!parentEvent)return NIMBY_INVALID_ARGUMENT;
    if(reinterpret_cast<HANDLE>(parentEvent)==INVALID_HANDLE_VALUE)return NIMBY_INVALID_HANDLE;
    HANDLE duplicate{};
    if(!DuplicateHandle(GetCurrentProcess(),reinterpret_cast<HANDLE>(parentEvent),GetCurrentProcess(),
            &duplicate,EVENT_MODIFY_STATE,FALSE,0))return NIMBY_INVALID_HANDLE;
    try {
        auto wake=std::make_shared<ProviderEvent>(duplicate);duplicate=nullptr;
        return endpoint.providerWake(token,std::move(wake));
    }catch(...){if(duplicate)CloseHandle(duplicate);return NIMBY_INTERNAL_ERROR;}
}
UI_EXPORT NimbyUi_ProviderRemoveV1(uint64_t token) noexcept {return endpoint.removeProvider(token);}
UI_EXPORT NimbyUi_ProviderObserveV1(uint64_t token,const char* world,uint32_t length,uint64_t generation) noexcept {return endpoint.observeProvider(token,world,length,generation);}
UI_EXPORT NimbyUi_ProviderSuspendV1(uint64_t token) noexcept {return endpoint.suspendProvider(token);}
UI_EXPORT NimbyUi_ProviderPollV1(uint64_t token,NimbyUiActionEventV1* event) noexcept {return endpoint.pollProvider(token,event);}
UI_EXPORT NimbyUi_ProviderPollV2(uint64_t token,NimbyUiActionEventV2* event) noexcept {return endpoint.pollProviderV2(token,event);}
UI_EXPORT NimbyUi_ModPresentV1(const char* id,uint32_t length,uint32_t* present) noexcept {return endpoint.present(id,length,present);}
UI_EXPORT NimbyUi_ToolPanelPublishV1(uint64_t provider,const NimbyUiToolPanelV1* panel) noexcept {return endpoint.publishToolPanel(provider,panel);}
UI_EXPORT NimbyUi_ToolPanelPublishV2(uint64_t provider,const NimbyUiToolPanelV2* panel) noexcept {return endpoint.publishToolPanelV2(provider,panel);}
UI_EXPORT NimbyUi_SignalPreviewPublishV1(uint64_t provider,const NimbyUiSignalPreviewV1* value) noexcept {
    const auto report=[&](uint32_t status,const nimby::runtime::SignalActions::PreviewResult& detail)noexcept {
        // Diagnostics must never replace the original operation status.
        try{previewDiagnostics.report(provider,value,status,detail,[](const char* message){
            nimby::detail::diagnostics::write("sdk","WARN",message);
        });}catch(...){}
        return status;
    };
    if(!enabled||!originalViewport)return report(NIMBY_HOOKS_UNAVAILABLE,{});
    preview::BorrowedFrame frame(readMemory,nullptr);
    uint64_t root{},db{},sim{};
    if(value&&value->count&&(!frame.get(base+0xb81998,root)||!root||!frame.get(root+0x540,db)||!db||!frame.get(root+0x680,sim)||!sim))return report(NIMBY_DATA_UNAVAILABLE,{});
    uint64_t publication{};
    nimby::runtime::SignalActions::PreviewResult detail{};
    const auto result=endpoint.publishPreview(provider,value,&publication,&detail);
    if(result!=NIMBY_OK)return report(result,detail);
    if(result==NIMBY_OK&&value&&value->count){
        AcquireSRWLockExclusive(&previewLock);
        // Only assignments occur under this lock. Out-of-order RPC completion
        // cannot replace the roots of a newer publication with older roots.
        if(publication>previewPublication){previewRoot=root;previewDatabase=db;previewSimulation=sim;previewPublication=publication;}
        ReleaseSRWLockExclusive(&previewLock);
    }
    return result;
}
UI_EXPORT NimbyUi_ConditionalVisibilityV1(uint64_t owner,uint64_t mask) noexcept {return endpoint.conditionalVisibility(owner,mask);}
UI_EXPORT NimbyUi_BeginV1(uint64_t owner,const char* identity,uint32_t length,uint64_t* session) noexcept {return endpoint.begin(owner,identity,length,session);}
UI_EXPORT NimbyUi_ObserveV1(uint64_t owner,uint64_t session,const NimbyUiSignalV1* signals,uint32_t count) noexcept {return endpoint.observe(owner,session,signals,count);}
UI_EXPORT NimbyUi_SuspendV1(uint64_t owner) noexcept {return endpoint.suspend(owner);}
UI_EXPORT NimbyUi_ReadV1(uint64_t owner,uint64_t signal,NimbyUiValuesV1* values) noexcept {return endpoint.read(owner,signal,values);}
UI_EXPORT NimbyUi_ReadBatchV1(uint64_t owner,const uint64_t* signals,uint32_t count,NimbyUiReadBatchHeaderV1* header,NimbyUiReadBatchRowV1* rows) noexcept {return endpoint.readBatch(owner,signals,count,header,rows);}
UI_EXPORT NimbyUi_ExportV1(uint64_t owner,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written) noexcept {return endpoint.exportSettings(owner,session,bytes,capacity,written);}
UI_EXPORT NimbyUi_BeginSavedV1(uint64_t owner,const char* identity,uint32_t identityLength,const char* bytes,uint32_t length,uint64_t* session) noexcept {return endpoint.beginSaved(owner,identity,identityLength,bytes,length,session);}

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
    constexpr unsigned char bytes[]{0x48,0x8b,0xc4,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48};
    std::array<unsigned char,16> actual{};
    if(!readMemory(nullptr,base+0x7a0a40,actual.data(),actual.size())||std::memcmp(bytes,actual.data(),16))return NIMBY_INVALID_BINARY;
    if(std::memcmp(reinterpret_cast<void*>(base+preview::viewportDrawRva),preview::viewportEntry,sizeof preview::viewportEntry))return NIMBY_INVALID_BINARY;
    if(MH_Initialize()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    bool created=false,viewportCreated=false;
    const auto target=reinterpret_cast<void*>(base+0x7a0a40);
    const auto viewportTarget=reinterpret_cast<void*>(base+preview::viewportDrawRva);
    struct Cleanup{bool& created;bool& viewportCreated;void* target;void* viewportTarget;~Cleanup(){if(enabled)return;
        nimby::platform::windows::mod_options::cleanupHooks();
        if(created)MH_RemoveHook(target);
        if(viewportCreated)MH_RemoveHook(viewportTarget);
        MH_Uninitialize();
    }} cleanup{created,viewportCreated,target,viewportTarget};
    if(MH_CreateHook(target,reinterpret_cast<void*>(&render),reinterpret_cast<void**>(&original))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    created=true;
    if(MH_CreateHook(viewportTarget,reinterpret_cast<void*>(&renderViewport),reinterpret_cast<void**>(&originalViewport))!=MH_OK)return NIMBY_INTERNAL_ERROR;
    viewportCreated=true;
    // Initialize storage on the bootstrap worker, before any UI/input hook.
    try{(void)nimby::platform::windows::mod_options::host();}catch(...){return NIMBY_INTERNAL_ERROR;}
    if(!nimby::platform::windows::mod_options::installUi(base))return NIMBY_INTERNAL_ERROR;
    // The trampoline, TLS frames and owned registry must survive mod unloads.
    HMODULE self{};
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&render),&self))return NIMBY_INTERNAL_ERROR;
    if(MH_QueueEnableHook(target)!=MH_OK||MH_QueueEnableHook(viewportTarget)!=MH_OK||MH_ApplyQueued()!=MH_OK)return NIMBY_INTERNAL_ERROR;
    enabled=true;return NIMBY_OK;
}
