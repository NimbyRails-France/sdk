#include "runtime/signal_ui_endpoint.h"
#include <cassert>
#include <iostream>
#include <functional>

int main(){
    using namespace nimby::runtime;
    SignalUiEndpoint endpoint;
    constexpr uint64_t signal=0x8000000000001;
    NimbyUiPanelV1 panel{};panel.size=sizeof panel;panel.version=1;
    std::strcpy(panel.id,"sfr.bal");std::strcpy(panel.title,"BAL");std::strcpy(panel.texture_set,"bal");
    uint64_t owner{},session{};
    assert(endpoint.add(&panel,&owner)==NIMBY_OK);
    NimbyUiActionV1 repeat{};
    std::strcpy(repeat.id,"repeat");std::strcpy(repeat.label,"Répéter ce signal");
    std::strcpy(repeat.provider,"signal-placement");std::strcpy(repeat.service,"repeat.v1");
    assert(endpoint.actions(owner,&repeat,1)==NIMBY_OK);
    assert(endpoint.begin(owner,"world",5,&session)==NIMBY_OK);
    assert(endpoint.panelContext(owner,session,1)==NIMBY_OK);
    NimbyUiSignalV1 row{};row.id=signal;std::strcpy(row.texture_set,"bal");
    assert(endpoint.observe(owner,session,&row,1)==NIMBY_OK);
    assert(endpoint.host.prepare(1,signal).actions.empty()); // SFR works on its own.
    NimbyUiProviderV1 provider{};provider.size=sizeof provider;provider.version=1;provider.count=1;
    std::strcpy(provider.id,"signal-placement");std::strcpy(provider.services[0],"repeat.v1");
    uint64_t token{},duplicate{};
    assert(endpoint.addProvider(&provider,&token)==NIMBY_OK);
    assert(endpoint.addProvider(&provider,&duplicate)==NIMBY_INVALID_ARGUMENT&&duplicate==0);
    uint32_t loaded=0;
    assert(endpoint.present("signal-placement",16,&loaded)==NIMBY_OK&&loaded==1);
    assert(endpoint.host.prepare(2,signal).actions.empty()); // Loaded != observed.
    assert(endpoint.observeProvider(token,"different",9,1)==NIMBY_OK);
    assert(endpoint.host.prepare(3,signal).actions.empty());
    assert(endpoint.observeProvider(token,"world",5,1)==NIMBY_OK);
    auto frame=endpoint.host.prepare(4,signal);assert(frame.actions.size()==1);
    struct Ui {
        size_t buttons=0;bool clicked=false,enabled=true;
        size_t separators=0;std::vector<float> heights;std::vector<std::string> groups;
        void checkbox(const char*,const char*,uint32_t&){}
        bool button(const char*,bool on){++buttons;enabled=on;return clicked;}
        void separator(){++separators;}
        void scroll(const char* key,float height,std::function<void(Ui&)> draw){
            groups.push_back(key);heights.push_back(height);draw(*this);groups.push_back("end");
        }
    } ui;
    SignalUiHost::layout(frame,ui);
    assert(ui.buttons==1&&ui.separators==1);
    ui.clicked=true;
    SignalUiHost::interactive(frame,signal,ui);
    assert(ui.buttons==2&&ui.enabled);
    assert(!endpoint.host.actions->click(frame.actions.front())); // Deduplicate pending click.
    NimbyUiActionEventV1 event{};event.size=sizeof event;event.version=1;
    assert(endpoint.pollProvider(token,&event)==NIMBY_OK);
    assert(event.signal==signal&&event.generation==1&&event.sequence&&std::string(event.service)=="repeat.v1");
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE&&event.sequence==0);
    // A provider publishes copied controls. Layout rows remain fixed, but a
    // new revision invalidates previously rendered buttons and queued clicks.
    NimbyUiToolPanelV1 tool{};tool.size=sizeof tool;tool.version=1;tool.panel=owner;tool.signal=signal;tool.count=1;
    std::strcpy(tool.origin,"repeat");std::strcpy(tool.service,"repeat.v1");std::strcpy(tool.message,"3 emplacements");
    std::strcpy(tool.buttons[0].id,"apply");std::strcpy(tool.buttons[0].label,"Confirmer");tool.buttons[0].enabled=1;
    assert(endpoint.publishToolPanel(token,&tool)==NIMBY_OK);
    assert(!endpoint.host.actions->click(frame.actions.front()));
    auto expanded=endpoint.host.prepare(40,signal);assert(expanded.actions.size()==2);
    assert(!endpoint.host.actions->click(expanded.actions[0])); // Message, not a command.
    assert(endpoint.host.actions->click(expanded.actions[1]));
    assert(endpoint.publishToolPanel(token,&tool)==NIMBY_OK); // Identical refresh keeps the queued click.
    const auto slowCapture=SignalActions::Clock::now()+std::chrono::milliseconds(2750);
    assert(endpoint.host.actions->observeProvider(token,{"world",1},slowCapture));
    assert(endpoint.host.actions->poll(token,slowCapture));
    assert(endpoint.host.actions->click(expanded.actions[1]));
    tool.buttons[0].enabled=0;
    assert(endpoint.publishToolPanel(token,&tool)==NIMBY_OK);
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE);
    assert(!endpoint.host.actions->click(expanded.actions[1]));
    expanded=endpoint.host.prepare(41,signal);assert(!endpoint.host.actions->available(expanded.actions[1]));
    // A long tool panel gets a bounded native scroll group. Its complete rows,
    // separator and height survive provider removal between the two UI passes.
    tool.count=12;
    for(unsigned i=0;i<tool.count;++i){
        const auto id="button"+std::to_string(i);
        std::strcpy(tool.buttons[i].id,id.c_str());std::strcpy(tool.buttons[i].label,id.c_str());tool.buttons[i].enabled=1;
    }
    assert(endpoint.publishToolPanel(token,&tool)==NIMBY_OK);
    SignalUiPresentation scroll;Ui scrolling;
    assert(scroll.layout(endpoint.host,42,signal,scrolling));
    assert(scrolling.buttons==13&&scrolling.separators==1&&scrolling.heights==std::vector<float>{320.f});
    // Removed provider between native layout/render: consume the row, disable
    // the click, never retain a callable mod address. Restart gets a new token.
    assert(endpoint.removeProvider(token)==NIMBY_OK);
    scrolling.clicked=true;
    assert(scroll.interactive(42,signal,scrolling));
    assert(scrolling.buttons==26&&scrolling.separators==2&&!scrolling.enabled);
    assert(scrolling.heights==std::vector<float>({320.f,320.f}));
    assert(scrolling.groups.size()==4&&scrolling.groups[0].starts_with("##nrf_signal_extensions_")&&scrolling.groups[0]==scrolling.groups[2]&&scrolling.groups[1]=="end"&&scrolling.groups[3]=="end");
    SignalUiHost::interactive(frame,signal,ui);
    assert(ui.buttons==3&&!ui.enabled);
    assert(endpoint.host.prepare(5,signal).actions.empty());
    const auto old=token;
    assert(endpoint.addProvider(&provider,&token)==NIMBY_OK&&token!=old);
    assert(endpoint.observeProvider(token,"world",5,1)==NIMBY_OK);
    assert(!endpoint.host.actions->click(frame.actions.front()));
    frame=endpoint.host.prepare(6,signal);
    assert(endpoint.host.actions->click(frame.actions.front()));
    // A capture failure invalidates requests even if observations later recover.
    assert(endpoint.suspendProvider(token)==NIMBY_OK);
    assert(endpoint.observeProvider(token,"world",5,1)==NIMBY_OK);
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE);
    assert(!endpoint.host.actions->click(frame.actions.front()));
    frame=endpoint.host.prepare(7,signal);
    assert(endpoint.host.actions->click(frame.actions.front()));
    assert(endpoint.suspend(owner)==NIMBY_OK);
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE);
    assert(endpoint.panelContext(owner,session,1)==NIMBY_OK);
    assert(endpoint.observe(owner,session,&row,1)==NIMBY_OK);
    frame=endpoint.host.prepare(8,signal);
    assert(endpoint.host.actions->click(frame.actions.front()));
    // Reopening the same world is a different runtime generation.
    assert(endpoint.begin(owner,"world",5,&session)==NIMBY_OK);
    assert(endpoint.panelContext(owner,session,2)==NIMBY_OK);
    assert(endpoint.observe(owner,session,&row,1)==NIMBY_OK);
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE);
    assert(endpoint.host.prepare(9,signal).actions.empty());
    assert(endpoint.observeProvider(token,"world",5,2)==NIMBY_OK);
    // Numeric edits round-trip through an additive ABI. They invalidate Apply
    // immediately, before the worker has had time to rebuild its preview.
    NimbyUiToolPanelV2 numeric{};numeric.base=tool;numeric.base.size=sizeof numeric;numeric.base.version=2;numeric.base.count=1;
    std::strcpy(numeric.base.buttons[0].id,"apply");numeric.base.buttons[0].enabled=1;
    numeric.input_count=1;auto& number=numeric.inputs[0];std::strcpy(number.id,"spacing");std::strcpy(number.label,"Espacement (m)");
    number.value=1000;number.minimum=3;number.maximum=100000;number.enabled=1;
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    auto numbers=endpoint.host.prepare(50,signal);assert(numbers.actions.size()==3&&numbers.actions[0].input);
    const auto input=numbers.actions[0],apply=numbers.actions[2];
    assert(!endpoint.host.actions->click(input));
    assert(!endpoint.host.actions->editNumber(input,2));
    assert(endpoint.host.actions->click(apply));
    assert(endpoint.host.actions->beginNumberEdit(input)); // Erase the whole draft: no integer can be sent yet.
    assert(!endpoint.host.actions->available(apply));
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE); // Earlier Apply was removed.
    numeric.inputs[0].value=1000;
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    numbers=endpoint.host.prepare(50,signal);
    // Continue with the fresh revision after the worker republishes the model.
    const auto freshInput=numbers.actions[0],freshApply=numbers.actions[2];
    assert(endpoint.host.actions->click(freshApply));
    assert(endpoint.host.actions->editNumber(freshInput,725));
    assert(!endpoint.host.actions->available(freshApply)&&!endpoint.host.actions->click(freshApply));
    assert(endpoint.host.actions->editNumber(freshInput,800)); // Latest value wins before worker consumption.
    NimbyUiActionEventV2 valueEvent{};valueEvent.base.size=sizeof valueEvent;valueEvent.base.version=2;
    assert(endpoint.pollProviderV2(token,&valueEvent)==NIMBY_OK); // Queued old Apply was discarded.
    assert(valueEvent.has_value==1&&valueEvent.value==800&&std::string(valueEvent.base.action)=="spacing");
    assert(endpoint.pollProviderV2(token,&valueEvent)==NIMBY_DATA_UNAVAILABLE);
    numeric.inputs[0].value=800;numeric.base.buttons[0].enabled=0;
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    assert(!endpoint.host.actions->editNumber(input,900)); // Old revision cannot edit.
    numeric.inputs[0].minimum=801;assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_INVALID_ARGUMENT);
    numeric.inputs[0].minimum=3;std::strcpy(numeric.inputs[0].id,"apply");
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_INVALID_ARGUMENT);
    std::strcpy(numeric.inputs[0].id,"spacing");numeric.input_count=2;numeric.inputs[1]=numeric.inputs[0];std::strcpy(numeric.inputs[1].id,"other");
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    auto two=endpoint.host.prepare(51,signal);
    assert(endpoint.host.actions->editNumber(two.actions[0],850));
    assert(endpoint.host.actions->editNumber(two.actions[1],900));
    assert(endpoint.pollProviderV2(token,&valueEvent)==NIMBY_OK&&valueEvent.value==850);
    numeric.inputs[0].value=850; // Worker acknowledges A while B is still queued.
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    assert(endpoint.host.prepare(52,signal).actions[1].input->value==900);
    assert(endpoint.pollProviderV2(token,&valueEvent)==NIMBY_OK&&valueEvent.value==900&&std::string(valueEvent.base.action)=="other");
    // A publication between drawing the textbox and queuing its edit must
    // not lose that edit; an empty draft must survive a normal UI frame too.
    numeric.input_count=1;numeric.inputs[0].value=850;
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    struct TextUi {
        bool interactive=false;std::optional<std::string> replacement;std::function<void()> race;
        std::string shown;
        void checkbox(const char*,const char*,uint32_t&){}
        bool button(const char*,bool){return false;}
        nimby::detail::NumberInputResult numberField(const char*,nimby::detail::NumberInputDraft& draft,int32_t value,int32_t min,int32_t max,bool enabled){
            draft.synchronize(value);bool changed=false;
            if(interactive&&enabled&&replacement){
                changed=std::string(draft.text.data(),draft.length)!=*replacement;
                draft.text={};std::copy(replacement->begin(),replacement->end(),draft.text.begin());draft.length=int(replacement->size());
                draft.modified|=changed;
            }
            shown.assign(draft.text.data(),draft.length);
            if(interactive&&race){auto invoke=std::move(race);race={};invoke();}
            return {changed,draft.value(min,max)};
        }
    } textUi;
    SignalUiPresentation textPresentation;
    auto drawText=[&](uint64_t invocation){textUi.interactive=false;assert(textPresentation.layout(endpoint.host,invocation,signal,textUi));
        textUi.interactive=true;assert(textPresentation.interactive(invocation,signal,textUi));};
    textUi.replacement="";drawText(60);assert(textUi.shown.empty());
    textUi.replacement.reset();drawText(61);assert(textUi.shown.empty());
    assert(endpoint.suspendProvider(token)==NIMBY_OK);
    assert(endpoint.suspend(owner)==NIMBY_OK);
    drawText(611);assert(textUi.shown.empty()); // A disabled panel retains the empty draft.
    assert(endpoint.host.prepare(612,signal).actions.size()==3);
    assert(endpoint.observe(owner,session,&row,1)==NIMBY_OK);
    assert(endpoint.panelContext(owner,session,2)==NIMBY_OK);
    assert(endpoint.observeProvider(token,"world",5,2)==NIMBY_OK);
    assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);
    drawText(613);assert(textUi.shown.empty());
    textUi.replacement="725";textUi.race=[&]{std::strcpy(numeric.base.message,"New status");assert(endpoint.publishToolPanelV2(token,&numeric)==NIMBY_OK);};
    drawText(62);assert(endpoint.pollProviderV2(token,&valueEvent)==NIMBY_DATA_UNAVAILABLE);
    textUi.replacement.reset();drawText(63);
    assert(endpoint.pollProviderV2(token,&valueEvent)==NIMBY_OK&&valueEvent.value==725);
    // A closed menu is also a presentation: suspension must not reopen it.
    assert(endpoint.host.actions->publish(token,owner,signal,"repeat","repeat.v1","",{{"repeat","Repeat",true}}));
    assert(endpoint.suspendProvider(token)==NIMBY_OK);
    frame=endpoint.host.prepare(9,signal);assert(frame.actions.size()==1&&!frame.actions.front().enabled);
    assert(endpoint.observeProvider(token,"world",5,2)==NIMBY_OK);
    frame=endpoint.host.prepare(10,signal);assert(frame.actions.size()==1);
    // Expiry disables controls without removing them. No real sleeps.
    const auto later=SignalActions::Clock::now()+SignalActions::workerLease+std::chrono::seconds(1);
    const auto expired=endpoint.host.actions->prepare(owner,frame.actions.front().editor,later);
    assert(expired.size()==1&&!expired.front().enabled);
    assert(!endpoint.host.actions->available(frame.actions.front(),later));
    assert(!endpoint.host.actions->click(frame.actions.front(),later));
    assert(endpoint.host.actions->click(frame.actions.front()));
    assert(endpoint.remove(owner)==NIMBY_OK);
    assert(endpoint.pollProvider(token,&event)==NIMBY_DATA_UNAVAILABLE);
    assert(endpoint.removeProvider(token)==NIMBY_OK);
    std::cout<<"PASS optional mod presence, UI passes, unload/reload, session epochs and stale clicks\n";
}
