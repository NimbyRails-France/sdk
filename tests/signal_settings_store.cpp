#include <nimby/signal_settings_store.hpp>
#include <nimby/detail/signal_settings_presenter.hpp>
#include <array>
#include <future>
#include <iostream>
#define CHECK(x) do {if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
int main(){try{
    using Store=nimby::SignalSettingsStore;
    {
        constexpr nimby::SignalCheckbox boxes[]{{"works","Works","",false},
            {"nrf.number.blocks.0","Blocks","",false},{"nrf.number.blocks.1","Blocks","",false},
            {"nrf.number.blocks.2","Blocks","",false}};
        constexpr nimby::SignalNumber numbers[]{{"blocks","Following blocks","works",3,4}};
        const nimby::SignalSettingsPanel panel{"numbers","Numbers","atlas",boxes,nullptr,{},numbers};
        Store store;store.configure(panel);
        const auto epoch=store.beginSession("world");
        constexpr uint64_t a=0x8000000000011,b=0x8000000010011;
        CHECK(store.observeSignals(epoch,std::array<Store::Signal,2>{{{a,"atlas"},{b,"atlas"}}}));
        const auto editor=store.selectSignal(epoch,a);
        CHECK(store.frame(editor)->controls.size()==1&&store.frame(editor)->numbers.empty());
        CHECK(!store.setNumber(editor,"blocks",2));
        CHECK(store.setBoolean(editor,"works",true));
        CHECK(store.setNumber(editor,"blocks",3));
        auto frame=store.frame(editor);CHECK(frame&&frame->numbers.size()==1&&frame->numbers[0].value==3);
        CHECK(!store.setNumber(editor,"blocks",-1)&&!store.setNumber(editor,"blocks",5));
        // Partial text survives observations, but not changing the edited signal.
        frame->numbers[0].draft->length=0;frame->numbers[0].draft->modified=true;
        CHECK(store.observeSignals(epoch,std::array<Store::Signal,2>{{{a,"atlas"},{b,"atlas"}}}));
        CHECK(store.frame(editor)->numbers[0].draft->length==0);
        CHECK(store.queueCopies(*store.copySource(a),std::array{b}));
        CHECK(store.observeSignals(epoch,std::array<Store::Signal,2>{{{a,"atlas"},{b,"atlas"}}}));
        auto target=store.selectSignal(epoch,b);CHECK(store.frame(target)->numbers[0].value==3);
        CHECK(!store.setNumber(editor,"blocks",1));
        const auto saved=store.save(epoch);
        Store restored;restored.configure(panel);const auto restoredEpoch=restored.beginSession("world",&saved);
        CHECK(restored.observeSignals(restoredEpoch,std::array<Store::Signal,2>{{{a,"atlas"},{b,"atlas"}}}));
        auto restoredEditor=restored.selectSignal(restoredEpoch,b);
        CHECK(restored.frame(restoredEditor)->numbers[0].value==3);
        CHECK(restored.setBoolean(restoredEditor,"works",false));CHECK(restored.frame(restoredEditor)->numbers.empty());
        CHECK(restored.setBoolean(restoredEditor,"works",true));CHECK(restored.frame(restoredEditor)->numbers[0].value==3);
        restored.suspendObservations();CHECK(!restored.setNumber(restoredEditor,"blocks",2));
    }
    {
        Store copied;
        constexpr nimby::SignalCheckbox boxes[]{{"active","Active","",false},{"option","Option","",true}};
        copied.configure({"copy","Copy","atlas",boxes});
        const auto epoch=copied.beginSession("world");
        constexpr uint64_t source=0x8000000000011,target=0x8000000010011;
        const std::array<Store::Signal,1> before{{{source,"atlas"}}};
        CHECK(copied.observeSignals(epoch,before));
        const auto editor=copied.selectSignal(epoch,source);
        CHECK(copied.setBoolean(editor,"active",true));
        CHECK(copied.setBoolean(editor,"option",false));
        const auto frozen=copied.copySource(source);CHECK(frozen);
        CHECK(copied.setBoolean(editor,"active",false));
        CHECK(copied.queueCopies(*frozen,std::array{target}));
        CHECK(copied.observeSignals(epoch,before)); // An older capture must not discard the pending copy.
        const std::array<Store::Signal,2> after{{{source,"atlas"},{target,"atlas"}}};
        CHECK(copied.observeSignals(epoch,after));
        CHECK(copied.read(target).getBoolean("active")==true);
        CHECK(copied.read(target).getBoolean("option")==false);
        CHECK(copied.save(epoch).signals.size()==2);
        CHECK(copied.observeSignals(epoch,before)); // Undo removes copied settings on observation.
        CHECK(copied.read(target).status==nimby::SettingsStatus::Absent);
        copied.beginSession("another world");
        CHECK(!copied.queueCopies(*frozen,std::array{target}));
    }
    constexpr uint64_t a=0x8000000000001,b=0x8000000010001,recycled=a+1;
    constexpr nimby::SignalCheckbox fields[]{{"active","Active","",false},{"green","Green","",true}};
    Store store;store.configure({"test.panel","Test","test.atlas",fields});
    CHECK(store.read(a).status==nimby::SettingsStatus::Unavailable);
    const auto session=store.beginSession("save-A");
    CHECK(store.read(a).status==nimby::SettingsStatus::Unavailable);
    const std::array<Store::Signal,2> signals{{{a,"test.atlas"},{b,"test.atlas"}}};
    CHECK(store.observeSignals(session,signals));
    CHECK(store.read(a).getBoolean("active")==false && store.read(a).getBoolean("green")==true);
    const auto editorA=store.selectSignal(session,a);
    const auto initialFrame=store.frame(editorA);
    CHECK(initialFrame && initialFrame->controls.size()==2);
    struct TestUi {
        size_t count=0;
        void checkbox(const char*,const char*,uint32_t& value){++count;value=!value;}
    } layoutUi;
    nimby::detail::drawSignalSettings(store,*initialFrame,layoutUi,false);
    CHECK(layoutUi.count==2 && store.read(a).getBoolean("active")==false);
    TestUi interactiveUi;
    nimby::detail::drawSignalSettings(store,*initialFrame,interactiveUi,true);
    CHECK(interactiveUi.count==2 && store.read(a).getBoolean("active")==true);
    CHECK(store.setBoolean(editorA,"active",true));
    CHECK(store.read(a).getBoolean("active")==true && store.read(b).getBoolean("active")==false);
    CHECK(!store.setBoolean(editorA,"unknown",true));
    const auto editorB=store.selectSignal(session,b);
    CHECK(!store.frame(editorA));
    TestUi staleUi;
    nimby::detail::drawSignalSettings(store,*initialFrame,staleUi,true);
    CHECK(staleUi.count==2 && store.read(b).getBoolean("active")==false);
    CHECK(!store.setBoolean(editorA,"active",false)); // Queued click from old selection.
    CHECK(store.setBoolean(editorB,"green",false));
    store.suspendObservations();
    CHECK(store.read(a).status==nimby::SettingsStatus::Unavailable && !store.setBoolean(editorB,"active",true));
    const auto suspended=store.selectFrame(b);
    CHECK(suspended&&!suspended->available&&suspended->controls.size()==2);
    CHECK(!store.accepts(suspended->editor)&&!store.setBoolean(suspended->editor,"green",true));
    CHECK(!store.frame(suspended->editor)); // Read-only presentation never becomes fresh data.
    CHECK(store.observeSignals(session,signals) && store.read(a).getBoolean("active")==true);
    auto saved=store.save();
    auto wrong=saved;wrong.sessionId="save-B";
    try{store.beginSession("save-A",&wrong);CHECK(false);}catch(const std::invalid_argument&){}
    CHECK(store.read(a).getBoolean("active")==true); // Failed import is transactional.
    store.endSession();CHECK(!store.setBoolean(editorB,"active",true));
    CHECK(store.read(a).status==nimby::SettingsStatus::Unavailable);
    // Reordering UI fields is harmless; adding a new field gets its default.
    constexpr nimby::SignalCheckbox reordered[]{{"green","Green","",true},{"new","New","",false},{"active","Active","",false}};
    store.configure({"test.panel","Test","test.atlas",reordered});
    const auto resumed=store.beginSession("save-A",&saved);
    CHECK(resumed!=session && !store.observeSignals(session,signals));
    CHECK(store.observeSignals(resumed,signals));
    CHECK(store.read(a).getBoolean("active")==true && store.read(a).getBoolean("new")==false);
    CHECK(store.read(b).getBoolean("green")==false);
    const auto beforeDelete=store.selectSignal(resumed,a);
    const std::array<Store::Signal,1> replacement{{{recycled,"test.atlas"}}};
    CHECK(store.observeSignals(resumed,replacement));
    CHECK(!store.setBoolean(beforeDelete,"active",true));
    CHECK(store.read(a).status==nimby::SettingsStatus::Absent);
    CHECK(store.read(recycled).getBoolean("active")==false && store.save().signals.empty());
    const auto editor=store.selectSignal(resumed,recycled);
    auto writer=std::async(std::launch::async,[&]{for(int i=0;i<1000;++i)CHECK(store.setBoolean(editor,"active",i%2));});
    for(int i=0;i<1000;++i){const auto state=store.read(recycled);CHECK(state.getBoolean("active").has_value());CHECK(state.booleans.size()==3);}
    writer.get();
    const auto other=store.beginSession("save-B");CHECK(store.observeSignals(other,replacement));
    CHECK(store.read(recycled).getBoolean("active")==false && !store.setBoolean(editor,"active",true));
    constexpr nimby::SignalCheckbox duplicate[]{{"same","A",""},{"same","B",""}};
    try{store.configure({"bad","Bad","test.atlas",duplicate});CHECK(false);}catch(const std::invalid_argument&){}
    CHECK(store.panel().id=="test.panel");
    const auto changingFrame=store.frame(store.selectSignal(other,recycled));
    CHECK(changingFrame);
    struct ChangingUi {
        Store& store;size_t count=0;
        void checkbox(const char* label,const char*,uint32_t& value){
            CHECK(label && *label);
            if(++count==1)store.configure({}); // Simulate mod/session loss during drawing.
            value=1;
        }
    } changingUi{store};
    nimby::detail::drawSignalSettings(store,*changingFrame,changingUi,true);
    CHECK(changingUi.count==3 && store.read(recycled).status==nimby::SettingsStatus::Unavailable);
    store.configure({});CHECK(store.read(recycled).status==nimby::SettingsStatus::Unavailable);
    std::cout<<"PASS C++ panel settings, session isolation, stale clicks, reload and concurrent access\n";
}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
