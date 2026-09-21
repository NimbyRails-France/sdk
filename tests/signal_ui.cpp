#include "engine/signal_ui.h"
#include "runtime/signal_settings_panel.h"
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>

#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
struct Memory {
    std::map<uint64_t,uint64_t> words;
    static bool read(void* context,uint64_t address,void* out,size_t size){
        const auto& words=static_cast<Memory*>(context)->words;
        const auto it=words.find(address);
        if(it==words.end()||size!=sizeof(uint64_t))return false;
        std::memcpy(out,&it->second,size);return true;
    }
};
int main(){try{
    using Ui=nimby::engine::SignalUi;
    constexpr uint64_t base=0x140000000,object=0x20000000;
    Memory memory;
    CHECK(!Ui::bind(Memory::read,&memory,base,object));
    memory.words={{object,base+0xa83818},{base+0xa83818+0xf0,base+0x55cd20}};
    auto layout=Ui::bind(Memory::read,&memory,base,object);
    CHECK(layout && layout->pass()==Ui::Pass::Layout);
    memory.words={{object,base+0xa83470},{base+0xa83470+0xf0,base+0x560870}};
    auto interactive=Ui::bind(Memory::read,&memory,base,object);
    CHECK(interactive && interactive->pass()==Ui::Pass::Interactive);
    // Recognizing an object is insufficient if its function table was changed.
    memory.words[base+0xa83470+0xf0]=base+0x55cd20;
    CHECK(!Ui::bind(Memory::read,&memory,base,object));
    memory.words[object]=base+0xa83628; // Abstract Declare is not a renderer.
    CHECK(!Ui::bind(Memory::read,&memory,base,object));
    CHECK(!Ui::bind(nullptr,&memory,base,object));
    CHECK(!Ui::bind(Memory::read,&memory,UINT64_MAX,object));
    using Profile=nimby::engine::LiveStateProfile;
    memory.words={{object,base+0x1088938},{base+0x1088938+0xf8,base+0x7a1280}};
    auto linuxLayout=Ui::bind(Memory::read,&memory,base,object,Profile::Linux119);
    CHECK(linuxLayout && linuxLayout->pass()==Ui::Pass::Layout);
    CHECK(!Ui::bind(Memory::read,&memory,base,object,Profile::Windows119));
    memory.words={{object,base+0x1088b00},{base+0x1088b00+0xf8,base+0x7a2490}};
    auto linuxInteractive=Ui::bind(Memory::read,&memory,base,object,Profile::Linux119);
    CHECK(linuxInteractive && linuxInteractive->pass()==Ui::Pass::Interactive);
    memory.words[base+0x1088b00+0xf8]=base+0x7a1280;
    CHECK(!Ui::bind(Memory::read,&memory,base,object,Profile::Linux119));
    memory.words={{object,base+0x1088b00},{base+0x1088b00+0xf0,base+0x7a2490}};
    CHECK(!Ui::bind(Memory::read,&memory,base,object,Profile::Linux119));
    CHECK(!Ui::bind(Memory::read,&memory,base,object,static_cast<Profile>(99)));
    CHECK(!Ui::bind(Memory::read,&memory,UINT64_MAX,object,Profile::Linux119));
    constexpr uint64_t capture=0x30000000,signal=0x40000000,id=0x8000000000001;
    memory.words={{capture+0x38,signal},{signal,id}};
    CHECK(Ui::editorSignal(Memory::read,&memory,capture)==id);
    CHECK(!Ui::editorSignal(Memory::read,&memory,capture,Profile::Linux119));
    memory.words[signal]=0x7000000000001; // A Script ID is not a Signal ID.
    CHECK(!Ui::editorSignal(Memory::read,&memory,capture));
    memory.words[signal]=id;memory.words.erase(capture+0x38);
    CHECK(!Ui::editorSignal(Memory::read,&memory,capture));
    CHECK(!Ui::editorSignal(Memory::read,&memory,UINT64_MAX));
    // Simulate the native two-pass editor contract without invoking game code.
    nimby::SignalSettingsStore store;
    constexpr nimby::SignalCheckbox fields[]{{"active","Active","",false},{"green","Green","",true}};
    store.configure({"panel","Panel","atlas",fields});
    auto session=store.beginSession("save-A");
    const nimby::SignalSettingsStore::Signal signals[]{{id,"atlas"}};
    CHECK(store.observeSignals(session,signals));
    nimby::detail::SignalSettingsPresentation presentation;
    struct Controls {
        size_t count=0;
        void checkbox(const char*,const char*,uint32_t& value){++count;value=!value;}
    } controls;
    CHECK(!presentation.interactive(store,capture,session,id,controls));
    CHECK(presentation.layout(store,capture,session,id,controls));
    CHECK(controls.count==2 && store.read(id).getBoolean("active")==false);
    CHECK(presentation.interactive(store,capture,session,id,controls));
    CHECK(controls.count==4 && store.read(id).getBoolean("active")==true);
    CHECK(!presentation.interactive(store,capture,session,id,controls));
    CHECK(presentation.layout(store,capture,session,id,controls));
    // A hidden window skips rendering; its following layout replaces the frame.
    CHECK(presentation.layout(store,capture+8,session,id,controls));
    CHECK(!presentation.interactive(store,capture,session,id,controls));
    store.suspendObservations();
    auto count=controls.count;
    CHECK(presentation.interactive(store,capture+8,session,id,controls));
    CHECK(controls.count==count+2 && store.read(id).status==nimby::SettingsStatus::Unavailable);
    CHECK(store.observeSignals(session,signals));
    CHECK(store.read(id).getBoolean("active")==true);
    CHECK(presentation.layout(store,capture,session,id,controls));
    session=store.beginSession("save-B");
    CHECK(store.observeSignals(session,signals));
    count=controls.count;
    CHECK(presentation.interactive(store,capture,session,id,controls));
    CHECK(controls.count==count+2 && store.read(id).getBoolean("active")==false);
    CHECK(presentation.layout(store,capture,session,id,controls));
    CHECK(!presentation.layout(store,capture,session,0,controls));
    CHECK(!presentation.interactive(store,capture,session,id,controls));
    // Removing the first mod between passes must not shift the second mod's
    // controls into the first one's reserved native layout positions.
    nimby::runtime::SignalUiHost host;
    auto first=host.add({"first","First","atlas",fields});
    auto second=host.add({"second","Second","atlas",fields});
    auto firstStore=host.store(first),secondStore=host.store(second);
    CHECK(firstStore->observeSignals(firstStore->beginSession("save-A"),signals));
    CHECK(secondStore->observeSignals(secondStore->beginSession("save-A"),signals));
    nimby::runtime::SignalUiPresentation panels;
    CHECK(panels.layout(host,capture,id,controls));
    CHECK(host.remove(first));
    CHECK(!host.store(first));
    count=controls.count;
    CHECK(panels.interactive(capture,id,controls));
    CHECK(controls.count==count+4);
    CHECK(firstStore->read(id).status==nimby::SettingsStatus::Unavailable);
    CHECK(secondStore->read(id).getBoolean("active")==true);
    CHECK(!panels.interactive(capture,id,controls));
    auto replacement=host.add({"first","First","atlas",fields});
    CHECK(replacement!=first);
    CHECK(host.store(replacement)->read(id).status==nimby::SettingsStatus::Unavailable);
    CHECK(panels.layout(host,capture,id,controls));
    CHECK(!panels.interactive(capture+8,id,controls));
    count=controls.count;
    CHECK(panels.interactive(capture,0,controls));
    CHECK(controls.count==count+2);
    CHECK(secondStore->read(id).getBoolean("active")==true);
    // Metadata is copied before the mod's declaration storage disappears.
    {
        std::string label="Owned label";
        const nimby::SignalCheckbox local[]{{"local",label,"",false}};
        auto token=host.add({"temporary","Temporary","atlas",local});
        label.assign("Changed");
        CHECK(host.store(token)->panel().checkboxes[0].label=="Owned label");
    }
    // Exhausted storage must reject edits without skipping native layout slots.
    nimby::SignalSettingsStore full;
    full.configure({"full","Full","atlas",fields});
    const auto fullSession=full.beginSession("save-full");
    std::vector<nimby::SignalSettingsStore::Signal> many;
    for(uint64_t i=0;i<16385;++i)many.push_back({id+i,"atlas"});
    CHECK(full.observeSignals(fullSession,many));
    for(size_t i=0;i<16384;++i)CHECK(full.setBoolean(full.selectSignal(fullSession,many[i].id),"active",true));
    auto last=full.selectFrame(many.back().id);CHECK(last);
    count=controls.count;
    CHECK(nimby::detail::drawSignalSettings(full,*last,controls,true)==2);
    CHECK(controls.count==count+2);
    CHECK(full.read(many.back().id).getBoolean("active")==false);
    std::cout<<"PASS native UI binding and resident panel lifetime (no native calls executed)\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
