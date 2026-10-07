#include "runtime/signal_ui_host.h"
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <future>
#include <iostream>
#include <new>
#include <thread>

namespace {
std::mutex gateMutex;
std::condition_variable gate;
std::atomic<void*> selectedAllocation{};
bool entered=false,released=false;
thread_local bool stallDelete=false;
thread_local bool recordEventLabel=false;
void* recordedEventLabel=nullptr;
void beforeDelete(void* allocation) {
    if(!stallDelete||!allocation||selectedAllocation.load()!=allocation)return;
    selectedAllocation.store(nullptr);
    std::unique_lock lock(gateMutex);entered=true;gate.notify_all();
    gate.wait(lock,[]{return released;});
}
}
#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE __declspec(noinline)
#endif
NOINLINE void* operator new(std::size_t size){
    if(auto* p=std::malloc(size?size:1)){
        if(recordEventLabel&&size==257){recordedEventLabel=p;recordEventLabel=false;}
        return p;
    }throw std::bad_alloc();
}
NOINLINE void* operator new[](std::size_t size){return ::operator new(size);}
NOINLINE void operator delete(void* p)noexcept{beforeDelete(p);std::free(p);}
NOINLINE void operator delete[](void* p)noexcept{::operator delete(p);}
NOINLINE void operator delete(void* p,std::size_t)noexcept{::operator delete(p);}
NOINLINE void operator delete[](void* p,std::size_t)noexcept{::operator delete(p);}

#define CHECK(value) do{if(!(value))throw std::runtime_error("Line "+std::to_string(__LINE__)+": " #value);}while(false)
using namespace std::chrono_literals;
using nimby::runtime::SignalUiHost;
using nimby::runtime::SignalActions;
namespace {
constexpr uint64_t source=0x8000000000041;
struct Fixture {
    SignalUiHost host;
    uint64_t owner=0,healthyOwner=0,bad=0,healthy=0;
    SignalActions::Selection healthyButton;
    Fixture(){
        constexpr nimby::SignalCheckbox fields[]{{"work","Work","",false}};
        owner=host.add({"owner","Owner","atlas",fields});
        healthyOwner=host.add({"healthy","Healthy","atlas",fields});
        std::vector<nimby::SignalSettingsStore::Signal> signals;
        for(uint64_t i=0;i<64;++i)signals.push_back({source+i,"atlas"});
        for(const auto token:{owner,healthyOwner}){
            const auto store=host.store(token);const auto session=store->beginSession("world");
            CHECK(store->observeSignals(session,signals));
        }
        bad=host.actions->addProvider("bad",{"svc"});healthy=host.actions->addProvider("healthy",{"svc"});
        // Lease expiry has separate tests. It must not turn a slow CI machine
        // or a debugger stop into an unrelated failure of this lock test.
        const auto lease=SignalActions::Clock::now()+1h;
        CHECK(host.actions->observeProvider(bad,{"world",1},lease));CHECK(host.actions->observeProvider(healthy,{"world",1},lease));
        CHECK(host.actions->configure(owner,{{"open","Open","bad","svc"}}));
        CHECK(host.actions->configure(healthyOwner,{{"open","Open","healthy","svc"}}));
        CHECK(host.actions->panelContext(owner,{"world",1}));CHECK(host.actions->panelContext(healthyOwner,{"world",1}));
        const auto frame=host.prepare(1,source);
        const auto found=std::find_if(frame.actions.begin(),frame.actions.end(),[&](const auto& action){return action.provider==healthy;});
        CHECK(found!=frame.actions.end());healthyButton=*found;CHECK(host.actions->available(healthyButton));
    }
    void* installCatalog(bool provider){
        // Close to the accepted 1 MiB budget. The fixture stalls the catalog's
        // real final deallocation, not a callback inserted into production.
        std::string json="{\"languages\":{\"en\":{";
        for(int i=0;i<200;++i){if(i)json+=',';json+='"'+std::to_string(i)+"\":\""+std::string(4096,'x')+'"';}
        json+="}}}";
        auto* allocation=new nimby::detail::Translations(json);
        std::shared_ptr<const nimby::detail::Translations> catalog(allocation);
        CHECK(host.translations(provider,provider?bad:owner,std::move(catalog)));
        return allocation;
    }
    void* installPresentations(){
        void* firstAllocation=nullptr;
        for(uint64_t i=0;i<64;++i){
            std::vector<SignalActions::Button> buttons{{"apply",std::string(256,'A'),true}};
            if(!i)firstAllocation=buttons.front().label.data();
            // Move ownership through the real API: this exact non-SSO label
            // allocation remains in the provider's first retained presentation.
            CHECK(host.actions->publish(bad,owner,source+i,"open","svc",std::string(256,'M'),std::move(buttons)));
        }
        return firstAllocation;
    }
    void* installQueuedAction(){
        installPresentations();
        const auto frame=host.prepare(3,source);
        const auto found=std::find_if(frame.actions.begin(),frame.actions.end(),[&](const auto& action){return action.provider==bad&&action.action.id=="apply";});
        CHECK(found!=frame.actions.end());CHECK(host.actions->available(*found));
        // click copies this exact 256-byte label into its owned queue event.
        // Other identifiers fit SSO; the list/deque node size is different.
        recordedEventLabel=nullptr;recordEventLabel=true;
        const bool accepted=host.actions->click(*found);recordEventLabel=false;
        CHECK(accepted&&recordedEventLabel);return recordedEventLabel;
    }
};

template<class Remove> bool whileRetiring(Fixture& fixture,void* allocation,Remove remove){
    {std::lock_guard lock(gateMutex);entered=false;released=false;selectedAllocation.store(allocation);}
    auto retiring=std::async(std::launch::async,[&]{stallDelete=true;const auto answer=remove();stallDelete=false;return answer;});
    bool stalled;
    {std::unique_lock lock(gateMutex);stalled=gate.wait_for(lock,2s,[]{return entered;});}
    // The gate establishes ordering. These generous waits only detect that a
    // peer can complete while destruction remains blocked; they are not a
    // microsecond latency assertion or a performance benchmark.
    auto prepare=std::async(std::launch::async,[&]{return !fixture.host.prepare(2,source).panels.empty();});
    auto available=std::async(std::launch::async,[&]{return fixture.host.actions->available(fixture.healthyButton);});
    auto observe=std::async(std::launch::async,[&]{return fixture.host.actions->observeProvider(fixture.healthy,{"world",1},SignalActions::Clock::now()+1h);});
    const auto deadline=std::chrono::steady_clock::now()+2s;
    const bool prepared=prepare.wait_until(deadline)==std::future_status::ready;
    const bool accepted=available.wait_until(deadline)==std::future_status::ready;
    const bool observed=observe.wait_until(deadline)==std::future_status::ready;
    {std::lock_guard lock(gateMutex);released=true;}gate.notify_all();
    const bool removed=retiring.get(),preparation=prepare.get(),availability=available.get(),observation=observe.get();
    CHECK(stalled);CHECK(removed&&preparation&&availability&&observation);
    return prepared&&accepted&&observed;
}
bool translationAdmittedBeforeRemovalIsReleased(){
    Fixture fixture;void* allocation=fixture.installCatalog(true);
    auto replacement=std::make_shared<nimby::detail::Translations>("{\"languages\":{\"en\":{\"0\":\"replacement\"}}}");
    std::weak_ptr<const nimby::detail::Translations> retained=replacement;
    {std::lock_guard lock(gateMutex);entered=false;released=false;selectedAllocation.store(allocation);}
    auto publication=std::async(std::launch::async,[&,catalog=std::move(replacement)]() mutable {
        stallDelete=true;const bool answer=fixture.host.translations(true,fixture.bad,std::move(catalog));stallDelete=false;return answer;
    });
    bool stalled;{std::unique_lock lock(gateMutex);stalled=gate.wait_for(lock,2s,[]{return entered;});}
    auto removal=std::async(std::launch::async,[&]{return fixture.host.removeProvider(fixture.bad);});
    const bool progressed=removal.wait_for(2s)==std::future_status::ready;
    const bool releasedWhileStalled=progressed&&retained.expired();
    {std::lock_guard lock(gateMutex);released=true;}gate.notify_all();
    CHECK(publication.get());CHECK(removal.get());CHECK(stalled);CHECK(retained.expired());
    return progressed&&releasedWhileStalled;
}
bool replacementProviderSurvivesOldRetirement(){
    Fixture fixture;void* allocation=fixture.installPresentations();
    const auto before=fixture.host.prepare(3,source);
    const auto old=std::find_if(before.actions.begin(),before.actions.end(),[&](const auto& action){return action.provider==fixture.bad&&action.action.id=="apply";});
    CHECK(old!=before.actions.end());const auto oldButton=*old;CHECK(fixture.host.actions->available(oldButton));
    {std::lock_guard lock(gateMutex);entered=false;released=false;selectedAllocation.store(allocation);}
    auto removal=std::async(std::launch::async,[&]{stallDelete=true;const bool answer=fixture.host.removeProvider(fixture.bad);stallDelete=false;return answer;});
    bool stalled;{std::unique_lock lock(gateMutex);stalled=gate.wait_for(lock,2s,[]{return entered;});}
    auto replacement=std::async(std::launch::async,[&]{
        const auto token=fixture.host.actions->addProvider("bad",{"svc"});CHECK(token!=fixture.bad);
        CHECK(fixture.host.actions->observeProvider(token,{"world",1},SignalActions::Clock::now()+1h));
        auto catalog=std::make_shared<nimby::detail::Translations>("{\"languages\":{\"en\":{\"0\":\"new owner\"}}}");
        std::weak_ptr<const nimby::detail::Translations> weak=catalog;
        CHECK(fixture.host.translations(true,token,std::move(catalog)));
        return std::make_pair(token,weak);
    });
    const bool progressed=replacement.wait_for(2s)==std::future_status::ready;
    {std::lock_guard lock(gateMutex);released=true;}gate.notify_all();
    CHECK(removal.get());const auto next=replacement.get();CHECK(stalled);
    CHECK(!fixture.host.actions->available(oldButton));CHECK(!next.second.expired());
    CHECK(fixture.host.actions->hasProvider(next.first));CHECK(fixture.host.removeProvider(next.first));CHECK(next.second.expired());
    return progressed;
}
void repeatedCatalogReplacement(){
    Fixture fixture;
    for(unsigned round=0;round<8;++round){
        const auto token=fixture.host.actions->addProvider("repeated",{"svc"});
        std::weak_ptr<const nimby::detail::Translations> previous;
        for(unsigned update=0;update<4;++update){
            auto catalog=std::make_shared<nimby::detail::Translations>("{\"languages\":{\"en\":{\"0\":\"small\"}}}");
            std::weak_ptr<const nimby::detail::Translations> next=catalog;
            CHECK(fixture.host.translations(true,token,std::move(catalog)));CHECK(previous.expired());previous=next;
        }
        CHECK(!previous.expired());CHECK(fixture.host.removeProvider(token));CHECK(previous.expired());
    }
}
void removingOnePanelPreservesOtherQueuedEdits(){
    SignalUiHost host;
    constexpr nimby::SignalCheckbox fields[]{{"work","Work","",false}};
    const auto first=host.add({"first","First","atlas",fields}),second=host.add({"second","Second","atlas",fields});
    const auto provider=host.actions->addProvider("shared",{"svc"});
    CHECK(host.actions->observeProvider(provider,{"world",1},SignalActions::Clock::now()+1h));
    std::map<uint64_t,nimby::SignalSettingsStore::Editor> editors;
    for(const auto owner:{first,second}){
        const auto store=host.store(owner);const auto session=store->beginSession("world");
        const nimby::SignalSettingsStore::Signal signals[]{{source,"atlas"}};
        CHECK(store->observeSignals(session,signals));editors[owner]=store->selectSignal(session,source);
        CHECK(host.actions->configure(owner,{{"open","Open","shared","svc"}}));CHECK(host.actions->panelContext(owner,{"world",1}));
        CHECK(host.actions->publish(provider,owner,source,"open","svc","",{{"run","Run",true}},
            {{"left","Left",1,1,100,true},{"right","Right",1,1,100,true}}));
    }
    const auto a=host.actions->prepare(first,editors[first]),b=host.actions->prepare(second,editors[second]);
    auto action=[](const auto& selections,std::string_view name){
        const auto found=std::find_if(selections.begin(),selections.end(),[&](const auto& value){return value.action.id==name;});
        CHECK(found!=selections.end());return *found;
    };
    CHECK(host.actions->editNumber(action(b,"left"),5));CHECK(host.actions->click(action(a,"run")));
    CHECK(host.actions->editNumber(action(b,"right"),6));CHECK(host.actions->editNumber(action(b,"left"),7));
    CHECK(host.remove(first));
    const auto left=host.actions->poll(provider),right=host.actions->poll(provider);
    CHECK(left&&right&&left->selection.panel==second&&right->selection.panel==second);
    CHECK(left->selection.action.id=="left"&&left->selection.value==7);
    CHECK(right->selection.action.id=="right"&&right->selection.value==6&&left->sequence<right->sequence);
    CHECK(!host.actions->poll(provider));
}
void queueCapacityAndRetirement(){
    SignalUiHost host;
    constexpr nimby::SignalCheckbox fields[]{{"work","Work","",false}};
    const auto provider=host.actions->addProvider("shared",{"svc"});
    CHECK(host.actions->observeProvider(provider,{"world",1},SignalActions::Clock::now()+1h));
    std::vector<uint64_t> owners;
    std::vector<std::vector<SignalActions::Selection>> selections;
    for(unsigned panel=0;panel<9;++panel){
        const auto id="panel"+std::to_string(panel);
        const auto owner=host.add({id,id,"atlas",fields});CHECK(owner);owners.push_back(owner);
        const auto store=host.store(owner);const auto session=store->beginSession("world");
        const nimby::SignalSettingsStore::Signal signals[]{{source,"atlas"}};
        CHECK(store->observeSignals(session,signals));const auto editor=store->selectSignal(session,source);
        CHECK(host.actions->configure(owner,{{"open","Open","shared","svc"}}));
        CHECK(host.actions->panelContext(owner,{"world",1}));
        std::vector<SignalActions::Button> buttons;
        for(unsigned button=0;button<8;++button)buttons.push_back({"run"+std::to_string(button),"Run",true});
        CHECK(host.actions->publish(provider,owner,source,"open","svc","",std::move(buttons)));
        selections.push_back(host.actions->prepare(owner,editor));CHECK(selections.back().size()==8);
    }
    // Fill the public queue, prove refusal at the boundary, then reclaim only
    // the removed panel's eight entries. Other owners retain their FIFO order.
    for(unsigned panel=0;panel<8;++panel)
        for(const auto& selection:selections[panel])CHECK(host.actions->click(selection));
    for(const auto& selection:selections[8])CHECK(!host.actions->click(selection));
    CHECK(host.remove(owners[0]));
    for(const auto& selection:selections[8])CHECK(host.actions->click(selection));
    uint64_t previous=0;
    for(unsigned panel=1;panel<9;++panel){
        for(unsigned button=0;button<8;++button){
            const auto event=host.actions->poll(provider);CHECK(event);
            CHECK(event->selection.panel==owners[panel]);
            CHECK(event->selection.action.id=="run"+std::to_string(button));
            CHECK(event->sequence>previous);previous=event->sequence;
        }
    }
    CHECK(!host.actions->poll(provider));
}
}
int main(){
    try{
        bool passed=true;
        for(const auto mode:{0,1,2,3,4,5,6,7}){
            Fixture fixture;
            void* allocation=mode<3?fixture.installCatalog(mode!=2):mode==7?fixture.installQueuedAction():fixture.installPresentations();
            const bool progress=whileRetiring(fixture,allocation,[&]{
                if(mode==0)return fixture.host.translations(true,fixture.bad,std::make_shared<nimby::detail::Translations>("{\"languages\":{\"en\":{\"0\":\"new\"}}}"));
                if(mode==1||mode==3)return fixture.host.removeProvider(fixture.bad);
                if(mode==2||mode==5)return fixture.host.remove(fixture.owner);
                if(mode==6)return fixture.host.actions->observeProvider(fixture.bad,{"next world",2},SignalActions::Clock::now()+1h);
                if(mode==7)return fixture.host.actions->suspendProvider(fixture.bad);
                return fixture.host.actions->removeProvider(fixture.bad);
            });
            std::cout<<(progress?"PASS ":"FAIL ")<<"peer progress during retirement mode="<<mode<<'\n';passed&=progress;
            if(mode==1||mode==3||mode==4){
                CHECK(!fixture.host.actions->hasProvider(fixture.bad));
                CHECK(!fixture.host.translations(true,fixture.bad,std::make_shared<nimby::detail::Translations>("{\"languages\":{\"en\":{\"0\":\"late\"}}}")));
            }
        }
        const bool translation=translationAdmittedBeforeRemovalIsReleased();
        std::cout<<(translation?"PASS ":"FAIL ")<<"concurrent translation admitted before removal is released\n";passed&=translation;
        const bool replacement=replacementProviderSurvivesOldRetirement();
        std::cout<<(replacement?"PASS ":"FAIL ")<<"new provider generation survives previous retirement\n";passed&=replacement;
        repeatedCatalogReplacement();std::cout<<"PASS repeated catalog replacement/removal releases all references\n";
        removingOnePanelPreservesOtherQueuedEdits();std::cout<<"PASS panel removal preserves peer edit FIFO and coalescence\n";
        queueCapacityAndRetirement();std::cout<<"PASS queue capacity 64 and FIFO survive panel retirement\n";
        return passed?0:1;
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}
}
