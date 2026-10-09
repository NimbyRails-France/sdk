#include "runtime/signal_ui_endpoint.h"
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <future>
#include <iostream>
#include <new>
#include <thread>

namespace {
// Stall a real allocation inside a foreign panel's export. This exercises the
// production locks without timing assumptions about how large a save must be.
thread_local bool stallAllocation=false;
std::mutex gateMutex;
std::condition_variable gate;
bool entered=false,released=false;
void blockAllocation(){
    if(!stallAllocation)return;
    stallAllocation=false;
    std::unique_lock lock(gateMutex);entered=true;gate.notify_all();
    gate.wait(lock,[]{return released;});
}
}
#if defined(__GNUC__)
#define FIXTURE_NOINLINE __attribute__((noinline))
#else
#define FIXTURE_NOINLINE __declspec(noinline)
#endif
FIXTURE_NOINLINE void* operator new(std::size_t size){blockAllocation();if(auto* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
FIXTURE_NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
FIXTURE_NOINLINE void operator delete(void* p) noexcept{std::free(p);}
FIXTURE_NOINLINE void operator delete[](void* p) noexcept{std::free(p);}
FIXTURE_NOINLINE void operator delete(void* p,std::size_t) noexcept{std::free(p);}
FIXTURE_NOINLINE void operator delete[](void* p,std::size_t) noexcept{std::free(p);}
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__)+": " #x);}while(false)
using namespace std::chrono_literals;
namespace {
struct BlockedOperation {
    std::thread worker;
    bool completed=false;
    template<class Operation> explicit BlockedOperation(Operation operation){
        {std::lock_guard lock(gateMutex);entered=false;released=false;}
        worker=std::thread([this,operation=std::move(operation)]{stallAllocation=true;try{operation();completed=true;}catch(...) {}});
    }
    bool wait(){std::unique_lock lock(gateMutex);return gate.wait_for(lock,2s,[]{return entered;});}
    void finish(){if(!worker.joinable())return;{std::lock_guard lock(gateMutex);released=true;}gate.notify_all();worker.join();}
    ~BlockedOperation(){finish();}
};
struct BlockedSave:BlockedOperation {
    BlockedSave(const std::shared_ptr<nimby::SignalSettingsStore>& store,uint64_t session)
        :BlockedOperation([store,session]{(void)store->save(session);}){}
};
struct WakeGate {
    std::mutex mutex;std::condition_variable changed;
    bool entered=false,released=false;
    std::atomic<unsigned> destroyed{0};
    void block()noexcept {
        std::unique_lock lock(mutex);entered=true;changed.notify_all();
        changed.wait(lock,[this]{return released;});
    }
    bool wait(){std::unique_lock lock(mutex);return changed.wait_for(lock,2s,[this]{return entered;});}
    void release(){std::lock_guard lock(mutex);released=true;changed.notify_all();}
};
struct GatedWake final:nimby::runtime::SignalActions::ActionWake {
    std::shared_ptr<WakeGate> gate;bool gateNotification;
    GatedWake(std::shared_ptr<WakeGate> value,bool notification):gate(std::move(value)),gateNotification(notification){}
    ~GatedWake()override{if(!gateNotification)gate->block();++gate->destroyed;}
    void notify()const noexcept override{if(gateNotification)gate->block();}
};
struct CountedWake final:nimby::runtime::SignalActions::ActionWake {
    mutable std::atomic<unsigned> calls{0};
    void notify()const noexcept override{++calls;}
};
struct WakeFixture {
    nimby::runtime::SignalUiEndpoint endpoint;
    uint64_t first{},other{};
    nimby::runtime::SignalActions::Selection firstSelection,otherSelection;
    static constexpr uint64_t signal=0x8000000000059;
    WakeFixture(){
        const auto owner=endpoint.host.add({"wake","Wake","atlas",{}});
        const auto store=endpoint.host.store(owner);const auto session=store->beginSession("world");
        const nimby::SignalSettingsStore::Signal row{signal,"atlas"};CHECK(store->observeSignals(session,{&row,1}));
        const auto actions=endpoint.host.actions;
        CHECK(actions->configure(owner,{{"first","First","first","service"},{"other","Other","other","service"}}));
        CHECK(actions->panelContext(owner,{"world",1}));
        first=actions->addProvider("first",{"service"});other=actions->addProvider("other",{"service"});
        CHECK(actions->observeProvider(first,{"world",1})&&actions->observeProvider(other,{"world",1}));
        const auto frame=endpoint.host.prepare(1,signal);CHECK(frame.actions.size()==2);
        firstSelection=frame.actions[0];otherSelection=frame.actions[1];
    }
};
void actionWakeIsOutsideRegistryAndSurvivesRetirement(){
    using namespace nimby::runtime;
    WakeFixture fixture;const auto actions=fixture.endpoint.host.actions;
    const auto gate=std::make_shared<WakeGate>();
    auto wake=std::make_shared<GatedWake>(gate,true);const std::weak_ptr<const SignalActions::ActionWake> retained=wake;
    CHECK(fixture.endpoint.providerWake(fixture.first,wake)==NIMBY_OK);wake.reset();
    auto click=std::async(std::launch::async,[&]{return actions->click(fixture.firstSelection);});
    const bool entered=gate->wait();
    const auto replacementWake=std::make_shared<CountedWake>(),peerWake=std::make_shared<CountedWake>();
    auto retire=std::async(std::launch::async,[&]{
        if(fixture.endpoint.removeProvider(fixture.first)!=NIMBY_OK||actions->poll(fixture.first))return false;
        const auto replacement=actions->addProvider("first",{"service"});
        if(replacement==fixture.first||!actions->observeProvider(replacement,{"world",1})||
           fixture.endpoint.providerWake(replacement,replacementWake)!=NIMBY_OK||
           fixture.endpoint.providerWake(fixture.other,peerWake)!=NIMBY_OK)return false;
        const auto frame=fixture.endpoint.host.prepare(2,WakeFixture::signal);
        const auto current=std::find_if(frame.actions.begin(),frame.actions.end(),[&](const auto& s){return s.provider==replacement;});
        return current!=frame.actions.end()&&actions->click(*current)&&actions->poll(replacement).has_value()&&
            actions->click(fixture.otherSelection)&&actions->poll(fixture.other).has_value()&&
            !actions->click(fixture.firstSelection);
    });
    const bool responsive=retire.wait_for(200ms)==std::future_status::ready;
    const bool stillOwned=!retained.expired()&&gate->destroyed==0;
    gate->release(); // Always release before checking, including a regression.
    const bool accepted=click.get(),retired=retire.get();
    CHECK(entered&&responsive&&stillOwned&&accepted&&retired);
    CHECK(retained.expired()&&gate->destroyed==1);
    CHECK(replacementWake->calls==1&&peerWake->calls==1);
}
void actionWakeDestructionIsOutsideRegistry(){
    for(const bool remove:{false,true}){
        WakeFixture fixture;const auto actions=fixture.endpoint.host.actions;
        const auto gate=std::make_shared<WakeGate>();
        auto wake=std::make_shared<GatedWake>(gate,false);
        CHECK(fixture.endpoint.providerWake(fixture.first,wake)==NIMBY_OK);wake.reset();
        const auto next=std::make_shared<CountedWake>();
        auto retirement=std::async(std::launch::async,[&]{return remove?fixture.endpoint.removeProvider(fixture.first):fixture.endpoint.providerWake(fixture.first,next);});
        const bool entered=gate->wait();
        auto healthy=std::async(std::launch::async,[&]{return actions->click(fixture.otherSelection)&&actions->poll(fixture.other).has_value();});
        const bool responsive=healthy.wait_for(200ms)==std::future_status::ready;
        gate->release();
        const auto status=retirement.get();const bool advanced=healthy.get();
        CHECK(entered&&responsive&&advanced&&status==NIMBY_OK&&gate->destroyed==1);
        if(!remove)CHECK(actions->click(fixture.firstSelection)&&next->calls==1);
    }
}
void actionWakeCannotReviveExpiredIntent(){
    using namespace nimby::runtime;
    WakeFixture fixture;const auto actions=fixture.endpoint.host.actions;
    const auto gate=std::make_shared<WakeGate>();
    const auto wake=std::make_shared<GatedWake>(gate,true);
    const auto peerCount=std::make_shared<CountedWake>();
    CHECK(fixture.endpoint.providerWake(fixture.first,wake)==NIMBY_OK);
    CHECK(fixture.endpoint.providerWake(fixture.other,peerCount)==NIMBY_OK);
    auto click=std::async(std::launch::async,[&]{return actions->click(fixture.firstSelection);});
    const bool entered=gate->wait();
    auto expire=std::async(std::launch::async,[&]{
        const auto later=SignalActions::Clock::now()+SignalActions::workerLease;
        return actions->observeProvider(fixture.first,{"world",1},later)&&!actions->poll(fixture.first,later)&&
            !actions->click(fixture.firstSelection,later)&&actions->hasProvider(fixture.other);
    });
    const bool responsive=expire.wait_for(200ms)==std::future_status::ready;
    gate->release();
    const bool accepted=click.get(),expired=expire.get();
    CHECK(entered&&responsive&&accepted&&expired&&!actions->poll(fixture.first)&&peerCount->calls==0);
}
struct ScrollUi {
    std::vector<std::string> groups;
    size_t checkboxes=0,numbers=0,buttons=0;
    bool clicked=false,enabled=true;
    void checkbox(const char*,const char*,uint32_t& value){++checkboxes;if(clicked)value=!value;}
    bool button(const char*,bool on){++buttons;enabled=on;return clicked;}
    void separator(){}
    nimby::detail::NumberInputResult numberField(const char*,nimby::detail::NumberInputDraft&,int value,int,int,bool on){
        ++numbers;enabled=on;return {false,value};
    }
    template<class Draw> void scroll(const char* key,float,Draw draw){groups.emplace_back(key);draw(*this);}
};
void sameStoreReadContention(){
    using namespace nimby::runtime;
    constexpr uint64_t signal=0x8000000000061;
    constexpr nimby::SignalCheckbox fields[]{{"active","Active","",false}};
    SignalUiEndpoint endpoint;
    const auto owner=endpoint.host.add({"reader","Reader","atlas",fields});
    const auto store=endpoint.host.store(owner);const auto session=store->beginSession("world");
    const nimby::SignalSettingsStore::Signal signals[]{{signal,"atlas"}};
    CHECK(store->observeSignals(session,signals));
    const auto previous=endpoint.host.prepare(1,signal);
    CHECK(previous.panels.size()==1&&previous.panels[0].controls.available);
    NimbyUiValuesV1 values{};values.size=sizeof values;values.version=1;
    uint32_t status=NIMBY_INTERNAL_ERROR;
    // read() allocates its owned settings under the same store mutex used by
    // selectFrame(). Gate that real allocation to make the overlap certain.
    BlockedOperation blocked([&]{status=endpoint.read(owner,signal,&values);});CHECK(blocked.wait());
    auto ui=std::async(std::launch::async,[&]{
        const auto cold=endpoint.host.prepare(2,signal);
        const auto retained=endpoint.host.prepare(2,signal,{},&previous);
        return cold.panels.empty()&&retained.panels.size()==1&&
            retained.panels[0].owner==owner&&!retained.panels[0].controls.available;
    });
    const bool responsive=ui.wait_for(200ms)==std::future_status::ready;
    blocked.finish();CHECK(responsive);CHECK(ui.get());CHECK(blocked.completed);
    CHECK(status==NIMBY_OK&&values.status==2);
    const auto recovered=endpoint.host.prepare(3,signal);
    CHECK(recovered.panels.size()==1&&recovered.panels[0].owner==owner&&recovered.panels[0].controls.available);
}
void previewContention(){
    using namespace nimby::runtime;
    using State=SignalActions::PreviewStatus;
    using Reason=SignalActions::PreviewReason;
    constexpr uint64_t source=0x8000000000041,track=0x1000000000021;
    constexpr nimby::SignalCheckbox fields[]{{"work","Work","",false}};
    SignalUiEndpoint endpoint;
    const auto owner=endpoint.host.add({"preview","Preview","atlas",fields});
    auto store=endpoint.host.store(owner);auto session=store->beginSession("world");
    const nimby::SignalSettingsStore::Signal signals[]{{source,"atlas"}};
    CHECK(store->observeSignals(session,signals));
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    auto& actions=*endpoint.host.actions;
    const auto provider=actions.addProvider("tool",{"repeat"});
    CHECK(actions.configure(owner,{{"repeat","Repeat","tool","repeat"}}));
    CHECK(actions.panelContext(owner,{"world",1}));CHECK(actions.observeProvider(provider,{"world",1}));
    NimbyUiSignalPreviewV1 wire{};wire.size=sizeof wire;wire.version=1;
    wire.panel=owner;wire.signal=source;wire.count=1;wire.positions[0]={track,.5,1,0};
    std::strcpy(wire.origin,"repeat");std::strcpy(wire.service,"repeat");
    NimbyUiToolPanelV1 panel{};panel.size=sizeof panel;panel.version=1;panel.panel=owner;panel.signal=source;panel.count=1;
    std::strcpy(panel.origin,"repeat");std::strcpy(panel.service,"repeat");
    std::strcpy(panel.buttons[0].id,"apply");std::strcpy(panel.buttons[0].label,"Before");panel.buttons[0].enabled=1;
    CHECK(endpoint.publishToolPanel(provider,&panel)==NIMBY_OK);
    const auto editor=store->selectSignal(session,source);
    const auto panelBefore=actions.prepare(owner,editor);CHECK(panelBefore.size()==1);
    uint64_t serial{};CHECK(endpoint.publishPreview(provider,&wire,&serial)==NIMBY_OK&&serial);
    const auto before=*actions.preview(source);
    {
        // The copied preview allocates under the real registry mutex. Without
        // an admitted owner ticket, publication must defer immediately rather
        // than crossing a cancellation that it has not yet observed.
        BlockedOperation blocked([&]{(void)actions.preview(source);});CHECK(blocked.wait());
        unsigned retries=0;
        const auto answer=actions.publishPreview(provider,owner,source,"repeat","repeat",{{track,.6,1,0}},SignalActions::Clock::now(),[&]{++retries;return false;});
        CHECK(answer.status==State::Busy&&retries==0);CHECK(!actions.preview(source));
        CHECK(answer.diagnostic.reason==Reason::RegistryBusy&&answer.publication==0);
        CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_RESOURCE_LIMIT);
        blocked.finish();CHECK(blocked.completed);
    }
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        unsigned retries=0;
        const auto busy=actions.publishPreview(provider,owner,source,"repeat","repeat",{{track,.6,1,0}},SignalActions::Clock::now(),[&]{++retries;return false;});
        CHECK(busy.status==State::Busy&&busy.diagnostic.reason==Reason::StoreBusy&&busy.publication==0&&retries==1);
        auto call=std::async(std::launch::async,[&]{return endpoint.publishPreview(provider,&wire);});
        auto render=std::async(std::launch::async,[&]{return actions.preview(source);});
        auto peer=std::async(std::launch::async,[&]{return actions.observeProvider(provider,{"world",1});});
        const auto ready=call.wait_for(200ms)==std::future_status::ready;
        const auto rendered=render.wait_for(200ms)==std::future_status::ready;
        const auto independent=peer.wait_for(200ms)==std::future_status::ready;
        std::strcpy(panel.buttons[0].label,"After");
        CHECK(endpoint.publishToolPanel(provider,&panel)==NIMBY_INVALID_HANDLE);
        const auto busyPanel=actions.prepare(owner,editor);
        CHECK(busyPanel.size()==1&&!busyPanel[0].enabled&&busyPanel[0].revision==panelBefore[0].revision);
        blocked.finish();CHECK(ready&&rendered&&independent);
        CHECK(call.get()==NIMBY_RESOURCE_LIMIT);CHECK(!render.get());CHECK(peer.get());
    }
    // A refused panel publication does not erase or rebuild the old model.
    // ToolContext op6/op8 deliberately treats this presentation-only refusal
    // as deferred, and the next worker tick can publish the acknowledged model.
    CHECK(actions.prepare(owner,editor)[0].action.label=="Before");
    CHECK(endpoint.publishToolPanel(provider,&panel)==NIMBY_OK);
    CHECK(actions.prepare(owner,editor)[0].action.label=="After");
    // A failed renewal keeps the previous lease exactly; it cannot extend it.
    CHECK(actions.preview(source)->expires==before.expires);
    CHECK(actions.preview(source)->publication==before.publication);
    CHECK(!actions.preview(source,before.expires));
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        unsigned retries=0;const auto begin=SignalActions::Clock::now();
        const auto answer=actions.publishPreview(provider,owner,source,"repeat","repeat",{{track,.7,1,0}},begin,[&]{
            ++retries;blocked.finish();return true;
        });
        CHECK(answer.status==State::Published&&retries==1&&answer.publication>serial);
        CHECK(answer.diagnostic.reason==Reason::None);
        const auto drawn=actions.preview(source);CHECK(drawn&&drawn->positions[0].fraction==.7);
        CHECK(drawn->expires==begin+2s); // Retry does not start another lease.
    }
    auto pendingThen=[&](Reason reason,auto change){
        BlockedSave blocked(store,session);CHECK(blocked.wait());unsigned retries=0;
        const auto answer=actions.publishPreview(provider,owner,source,"repeat","repeat",{{track,.8,1,0}},SignalActions::Clock::now(),[&]{
            ++retries;blocked.finish();change();return true;
        });
        CHECK(answer.status==State::Invalid&&retries==1);CHECK(!actions.preview(source));
        CHECK(answer.diagnostic.reason==reason&&answer.publication==0);
        CHECK(std::string(answer.diagnostic.providerId)=="tool");
        return answer.diagnostic;
    };
    // A stop/clear wins against a renewal already waiting on the real store.
    const auto cancelled=pendingThen(Reason::RequestCancelled,[&]{CHECK(actions.publishPreview(provider,0,0,{},{},{}));});
    CHECK(cancelled.expectedRevision<cancelled.revision);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    const auto resumed=pendingThen(Reason::ObservationEpochChanged,[&]{store->suspendObservations();CHECK(store->observeSignals(session,signals));});
    CHECK(resumed.expectedObservationEpoch<resumed.observationEpoch);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    pendingThen(Reason::ObservationEpochChanged,[&]{CHECK(store->observeSignals(session,{}));CHECK(store->observeSignals(session,signals));});
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    pendingThen(Reason::ObservationEpochChanged,[&]{session=store->beginSession("world");CHECK(store->observeSignals(session,signals));});
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    const auto restarted=pendingThen(Reason::ProviderEpochChanged,[&]{CHECK(actions.suspendProvider(provider));CHECK(actions.observeProvider(provider,{"world",1}));});
    CHECK(restarted.expectedProviderEpoch<restarted.providerEpoch);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    // The source can remain continuously observed while a different signal
    // disappears. The admitted catalogue epoch still cannot be reused.
    const nimby::SignalSettingsStore::Signal withNeighbour[]{{source,"atlas"},{source+1,"atlas"}};
    CHECK(store->observeSignals(session,withNeighbour));
    const auto changedCatalogue=pendingThen(Reason::ObservationEpochChanged,[&]{CHECK(store->observeSignals(session,signals));});
    CHECK(changedCatalogue.expectedObservationEpoch<changedCatalogue.observationEpoch);
    CHECK(store->signalState(source,true,"world")==nimby::SignalSettingsStore::SignalState::Known);
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    // Panel availability is independent of store observations. A new request
    // works once the panel returns; an already waiting request is refused.
    pendingThen(Reason::PanelInactive,[&]{CHECK(actions.panelContext(owner,{}));});
    CHECK(store->signalState(source,true,"world")==nimby::SignalSettingsStore::SignalState::Known);
    CHECK(actions.panelContext(owner,{"world",1}));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    pendingThen(Reason::PanelWorldChanged,[&]{CHECK(actions.panelContext(owner,{"another",1}));});
    CHECK(actions.panelContext(owner,{"world",1}));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    const auto panelReloaded=pendingThen(Reason::PanelGenerationChanged,[&]{CHECK(actions.panelContext(owner,{"world",2}));});
    CHECK(panelReloaded.generation==1&&panelReloaded.providerGeneration==1&&panelReloaded.panelGeneration==2);
    CHECK(actions.panelContext(owner,{"world",1}));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    // Provider context changes revoke the admitted epoch first, even when
    // the replacement world or generation itself would also be incompatible.
    const auto providerWorld=pendingThen(Reason::ProviderEpochChanged,[&]{CHECK(actions.observeProvider(provider,{"another",1}));});
    CHECK(providerWorld.expectedProviderEpoch<providerWorld.providerEpoch);
    CHECK(actions.observeProvider(provider,{"world",1}));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    const auto providerReloaded=pendingThen(Reason::ProviderEpochChanged,[&]{CHECK(actions.observeProvider(provider,{"world",2}));});
    CHECK(providerReloaded.generation==1&&providerReloaded.providerGeneration==2&&providerReloaded.panelGeneration==1);
    CHECK(actions.observeProvider(provider,{"world",1}));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        auto time=SignalActions::Clock::now();const auto begin=time;
        const auto answer=actions.publishPreview(provider,owner,source,"repeat","repeat",{{track,.9,1,0}},begin,[&]{
            blocked.finish();time+=3s;return true;
        },[&]{return time;});
        CHECK(answer.status==State::Invalid&&answer.diagnostic.reason==Reason::PreviewExpired&&answer.publication==0);
        CHECK(!actions.preview(source,time));
    }
    auto rejectedNow=[&](Reason reason){
        const auto answer=actions.publishPreview(provider,owner,source,"repeat","repeat",{{track,.5,1,0}});
        CHECK(answer.status==State::Invalid&&answer.diagnostic.reason==reason&&answer.publication==0);
    };
    store->suspendObservations();CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_INVALID_HANDLE);
    rejectedNow(Reason::ObservationsSuspended);
    CHECK(store->observeSignals(session,{}));CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_INVALID_HANDLE);
    rejectedNow(Reason::SignalMissing);
    CHECK(store->observeSignals(session,signals));
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_OK);
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    pendingThen(Reason::PanelMissing,[&]{CHECK(endpoint.host.remove(owner));});
    CHECK(endpoint.publishPreview(provider,&wire)==NIMBY_INVALID_HANDLE);
    rejectedNow(Reason::PanelMissing);
    CHECK(endpoint.removeProvider(provider)==NIMBY_OK);
    rejectedNow(Reason::ProviderMissing);
}
void retainedBusyPanel(){
    using namespace nimby::runtime;
    constexpr uint64_t source=0x8000000000021,other=source+1,invocation=17;
    constexpr nimby::SignalCheckbox fields[]{
        {"work","Work","Description",true},{"nrf.number.blocks.0","Blocks","",false},
        {"nrf.number.blocks.1","Blocks","",false}};
    constexpr nimby::SignalNumber numbers[]{{"blocks","Blocks","work",2,3}};
    SignalUiHost host;const auto owner=host.add({"test","Test","atlas",fields,nullptr,{},numbers});
    auto store=host.store(owner);auto session=store->beginSession("world");
    const nimby::SignalSettingsStore::Signal signals[]{{source,"atlas"},{other,"atlas"}};
    CHECK(store->observeSignals(session,signals));
    const auto provider=host.actions->addProvider("tool",{"repeat"});
    CHECK(host.actions->configure(owner,{{"repeat","Repeat","tool","repeat"}}));
    CHECK(host.actions->panelContext(owner,{"world",1}));
    CHECK(host.actions->observeProvider(provider,{"world",1}));
    SignalUiPresentation presentation;ScrollUi ui;
    CHECK(presentation.layout(host,invocation,source,ui));CHECK(presentation.interactive(invocation,source,ui));
    CHECK(ui.groups.size()==2&&ui.groups[0]==ui.groups[1]);
    const auto key=ui.groups[0];const auto copied=host.prepare(invocation,source);
    CHECK(copied.panels.size()==1&&copied.panels[0].controls.numbers.size()==1&&copied.actions.size()==1);
    const auto editor=copied.panels[0].controls.editor;
    CHECK(store->setBoolean(editor,"work",true)); // Ensure save allocates under the real store lock.
    // Re-capture after the persistent value's revision changed.
    CHECK(presentation.layout(host,invocation,source,ui));CHECK(presentation.interactive(invocation,source,ui));
    const auto stable=host.prepare(invocation,source);
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        auto future=std::async(std::launch::async,[&]{
            const auto frame=host.prepare(invocation,source,{},&stable);
            CHECK(frame.panels.size()==1&&!frame.panels[0].controls.available);
            CHECK(frame.panels[0].controls.numbers.size()==1&&frame.actions.size()==1&&!frame.actions[0].enabled);
            // Repeated busy frames must still emit the same native scroll key
            // and all rows, even when an unfinished layout gets replaced.
            for(int i=0;i<8;++i)CHECK(presentation.layout(host,invocation,source,ui));
        });
        const auto ready=future.wait_for(200ms)==std::future_status::ready;
        blocked.finish();CHECK(ready);future.get();CHECK(blocked.completed);
        for(const auto& group:ui.groups)CHECK(group==key);
    }
    // Releasing the mutex before interaction must not activate a busy frame.
    ui.clicked=true;CHECK(presentation.interactive(invocation,source,ui));
    CHECK(store->read(source).getBoolean("work")==true);CHECK(!host.actions->poll(provider));CHECK(!ui.enabled);
    ui.clicked=false;CHECK(presentation.layout(host,invocation,source,ui));
    ui.clicked=true;CHECK(presentation.interactive(invocation,source,ui));
    CHECK(store->read(source).getBoolean("work")==false);CHECK(host.actions->poll(provider));
    ui.clicked=false;
    // A real control mutation cannot borrow an earlier shape while busy.
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        CHECK(host.prepare(invocation,source,{},&stable).panels.empty());
    }
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    CHECK(presentation.layout(host,invocation,source,ui));CHECK(presentation.interactive(invocation,source,ui));
    const auto beforeWorld=host.prepare(invocation,source);
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        CHECK(!presentation.layout(host,invocation,other,ui)); // No cross-signal fallback.
        CHECK(!presentation.interactive(invocation,other,ui));
    }
    session=store->beginSession("new-world");CHECK(store->observeSignals(session,signals));
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        CHECK(host.prepare(invocation,source,{},&beforeWorld).panels.empty());
    }
    const auto beforeReload=host.prepare(invocation,source);
    session=store->beginSession("new-world"); // Same world, new observed generation/session.
    CHECK(store->observeSignals(session,signals));
    CHECK(store->setBoolean(store->selectSignal(session,source),"work",true));
    {
        BlockedSave blocked(store,session);CHECK(blocked.wait());
        CHECK(host.prepare(invocation,source,{},&beforeReload).panels.empty());
    }
    CHECK(presentation.layout(host,invocation,source,ui));CHECK(presentation.interactive(invocation,source,ui));
    CHECK(ui.groups.back()!=key); // A new world/session intentionally owns new scroll state.
    CHECK(store->observeSignals(session,{}));
    CHECK(!presentation.layout(host,invocation,source,ui)); // Actual deletion stays absent.
    CHECK(store->observeSignals(session,signals));
    CHECK(presentation.layout(host,invocation,source,ui));CHECK(presentation.interactive(invocation,source,ui));
    CHECK(host.remove(owner));CHECK(!presentation.layout(host,invocation,source,ui));
    CHECK(!presentation.interactive(invocation,source,ui));
    const auto replacement=host.add({"test","Replacement","atlas",fields,nullptr,{},numbers});
    CHECK(replacement!=owner);
    auto replacementStore=host.store(replacement);const auto replacementSession=replacementStore->beginSession("new-world");
    CHECK(replacementStore->observeSignals(replacementSession,signals));
    CHECK(replacementStore->setBoolean(replacementStore->selectSignal(replacementSession,source),"work",true));
    {
        BlockedSave blocked(replacementStore,replacementSession);CHECK(blocked.wait());
        CHECK(host.prepare(invocation,source,{},&beforeReload).panels.empty());
    }
    // Suspension has already cleared editor_, and default-only signals have
    // no saved values to prune. Membership removal must still revoke fallback.
    SignalUiHost deletionHost;
    const auto deletionOwner=deletionHost.add({"deletion","Deletion","atlas",fields});
    auto deletionStore=deletionHost.store(deletionOwner);const auto deletionSession=deletionStore->beginSession("world");
    CHECK(deletionStore->observeSignals(deletionSession,signals));
    CHECK(deletionStore->setBoolean(deletionStore->selectSignal(deletionSession,other),"work",true));
    const auto defaultOnly=deletionHost.prepare(invocation,source);
    CHECK(defaultOnly.panels.size()==1);
    deletionStore->suspendObservations();
    CHECK(deletionStore->observeSignals(deletionSession,std::span(signals).subspan(1)));
    {
        BlockedSave blocked(deletionStore,deletionSession);CHECK(blocked.wait());
        CHECK(deletionHost.prepare(invocation,source,{},&defaultOnly).panels.empty());
    }
}
}
int main(){try{
    nimby::runtime::SignalUiEndpoint endpoint;
    NimbyUiPanelV1 panel{};panel.size=sizeof panel;panel.version=1;panel.count=1;
    std::strcpy(panel.title,"Test");std::strcpy(panel.checkboxes[0].name,"active");std::strcpy(panel.checkboxes[0].label,"Active");
    std::strcpy(panel.id,"healthy");std::strcpy(panel.texture_set,"healthy.atlas");uint64_t healthy{},healthySession{};
    CHECK(endpoint.add(&panel,&healthy)==NIMBY_OK);CHECK(endpoint.begin(healthy,"world",5,&healthySession)==NIMBY_OK);
    std::strcpy(panel.id,"offender");std::strcpy(panel.texture_set,"offender.atlas");uint64_t offender{},offenderSession{};
    CHECK(endpoint.add(&panel,&offender)==NIMBY_OK);CHECK(endpoint.begin(offender,"world",5,&offenderSession)==NIMBY_OK);
    NimbyUiSignalV1 healthySignal{},offenderSignal{};healthySignal.id=0x8000000000001;offenderSignal.id=healthySignal.id+1;
    std::strcpy(healthySignal.texture_set,"healthy.atlas");std::strcpy(offenderSignal.texture_set,"offender.atlas");
    CHECK(endpoint.observe(healthy,healthySession,&healthySignal,1)==NIMBY_OK);
    CHECK(endpoint.observe(offender,offenderSession,&offenderSignal,1)==NIMBY_OK);
    auto retained=endpoint.host.store(offender);const auto oldEditor=retained->selectSignal(offenderSession,offenderSignal.id);
    CHECK(retained->setBoolean(oldEditor,"active",true));
    uint32_t exportStatus{},needed{};
    std::thread slow([&]{stallAllocation=true;exportStatus=endpoint.exportSettings(offender,offenderSession,nullptr,0,&needed);});
    bool stalled{};
    {std::unique_lock lock(gateMutex);stalled=gate.wait_for(lock,2s,[]{return entered;});}
    if(!stalled){{std::lock_guard lock(gateMutex);released=true;}gate.notify_all();slow.join();CHECK(stalled);}
    auto reader=std::async(std::launch::async,[&]{NimbyUiValuesV1 value{};value.size=sizeof value;value.version=1;
        return endpoint.read(healthy,healthySignal.id,&value)==NIMBY_OK&&value.status==2;});
    // Both calls must finish while the foreign export is still blocked. Run
    // them in order: their own store is allowed to return Busy when read()
    // overlaps its first UI frame (covered deterministically above).
    const bool independent=reader.wait_for(200ms)==std::future_status::ready;
    auto ui=std::async(std::launch::async,[&]{const auto frame=endpoint.host.prepare(1,healthySignal.id);
        return frame.panels.size()==1&&frame.panels[0].owner==healthy;});
    const bool responsive=ui.wait_for(200ms)==std::future_status::ready;
    {std::lock_guard lock(gateMutex);released=true;}gate.notify_all();slow.join();
    CHECK(reader.get());CHECK(ui.get());CHECK(exportStatus==NIMBY_OK&&needed);
    CHECK(independent);CHECK(responsive);
    CHECK(endpoint.remove(offender)==NIMBY_OK);
    CHECK(!retained->accepts(oldEditor));CHECK(!retained->observeSignals(offenderSession,{}));
    bool resurrected=true;
    try{retained->beginSession("world");}catch(const std::logic_error&){resurrected=false;}
    CHECK(!resurrected);CHECK(retained->read(offenderSignal.id).status==nimby::SettingsStatus::Unavailable);
    CHECK(!retained->setBoolean(oldEditor,"active",false));
    CHECK(endpoint.host.prepare(2,healthySignal.id).panels.size()==1);
    CHECK(endpoint.remove(healthy)==NIMBY_OK);
    sameStoreReadContention();
    retainedBusyPanel();
    previewContention();
    actionWakeIsOutsideRegistryAndSurvivesRetirement();
    actionWakeDestructionIsOutsideRegistry();
    actionWakeCannotReviveExpiredIntent();
    std::cout<<"PASS UI isolation: independent panels, stable disabled scroll frames, bounded preview retries, nonblocking rendering, unchanged leases and lifecycle invalidation\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
