#include <platform/windows/mod_options_ui.h>
#include <platform/windows/mod_options_host.h>
#include <platform/windows/game_keybindings.h>
#include <platform/windows/mod_options_capture.h>
#include <platform/windows/mod_options_layout.h>
#include <platform/windows/game_language.h>
#include <engine/signal_ui.h>
#include <nimby/detail/diagnostics.hpp>
#include <MinHook.h>
#include <windows.h>
#include <array>
#include <bit>
#include <chrono>
#include <map>
#include <utility>
#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace nimby::platform::windows::mod_options {
namespace {
namespace model=runtime::mod_options;
namespace shortcuts=engine::mod_shortcuts;
using Ui=engine::SignalUi;
using Body=void(*)(uint64_t,uint64_t);
using Pump=uint16_t(*)(uint64_t);
using Frame=uint64_t(*)(uint64_t,uint64_t);
using Poll=bool(*)(void*);
using WindowId=uint32_t(*)(void*);
using TextInput=bool(*)(void*);
using KeyboardState=const bool*(*)(int*);
using Options=uint8_t(*)(uint64_t,uint64_t,uint64_t,uint8_t);
using Button=uint8_t(*)(uint64_t,const char*,uint32_t);
Body originalBody{};
Pump originalPump{};
Frame originalFrame{};
Poll originalPoll{};
WindowId windowId{};
TextInput textInput{};
KeyboardState keyboardState{};
Options originalOptions{};
Button originalLayoutButton{},originalInteractiveButton{};
uint64_t module{};
std::array<void*,7> targets{};
size_t created{};

bool read(void*,uint64_t at,void* out,size_t size){
    SIZE_T got{};return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(at),out,size,&got)&&got==size;
}
template<class T>bool get(uint64_t at,T& out){return read(nullptr,at,&out,sizeof out);}
template<class T>void put(uint64_t at,const T& value){std::memcpy(reinterpret_cast<void*>(at),&value,sizeof value);}
int64_t textClock(){LARGE_INTEGER tick{};if(!QueryPerformanceCounter(&tick))throw std::runtime_error("Native options text clock");return tick.QuadPart;}

// Copy only events delivered by the game's own event pump. A fixed array
// prevents key-repeat floods from allocating or retaining game/SDL pointers.
struct KeyboardEvent {uint32_t window,key;uint16_t modifiers;bool repeat,captured;};
struct InputFrame {
    std::array<KeyboardEvent,256> events{};
    size_t count=0,suppressed=0;uint32_t primaryWindow=0;
    bool collecting=false,overflow=false,captureAllowed=false,focusLost=false,drained=false;
};
thread_local InputFrame input;
thread_local CapturedKeys capturedKeys;
struct Capture {uint64_t token=0,started=0;std::string field;explicit operator bool()const{return token!=0;}};
struct DrawField {model::Field field;std::string value,error,conflict;};
struct DrawMod {uint64_t token;std::string title;std::vector<DrawField> fields;std::string error;};
struct Presentation {
    uint64_t serial=0,drawn=0,captureObject=0,lastLayout=0;
    bool french=false,nativeKnown=false,textFocused=true;
    OptionsCategories categories;
    std::vector<DrawMod> mods;
    Capture capture,frozenCapture;
    std::map<std::pair<uint64_t,std::string>,std::string> errors;
    std::map<std::pair<uint64_t,std::string>,detail::NumberInputDraft> numbers;
    uint64_t registryRevision=~uint64_t{};
    uint64_t catalogueRevision=~uint64_t{};
    std::string language;
    std::string loaderMessage;
};
thread_local Presentation presentation;
// The SDK selection never enters the game's persisted 0..6 tab enum. Owner
// identity is compared only; the pointer is dereferenced solely in its native
// dispatcher callback, while the game guarantees that object is alive.
struct Navigation {
    uint64_t owner=0;
    int nativeTab=1;
    bool selected=false;
    OptionsPage page=OptionsPage::Interface;
};
thread_local Navigation navigation;
struct OptionsFrame {
    bool nrf=false,requested=false,failed=false;
    OptionsBodyLayout body;
    OptionsCategories categories;
    int nativeClicked=-1;
    OptionsPage page=OptionsPage::Interface,pendingPage=OptionsPage::Interface;
    std::array<uint64_t,2> emitters{};
    std::array<size_t,2> menus{};
    std::array<bool,2> injected{};
};
thread_local OptionsFrame* optionsFrame=nullptr;
struct NativeCache {
    std::optional<std::vector<GameKeyBinding>> values;
    std::string language;
    uint64_t languageObject=0;
    bool accepted=false,unknownPublished=false;
    std::chrono::steady_clock::time_point lastRead{};
};
thread_local NativeCache nativeCache;
const char* tr(bool french,const char* fr,const char* en){return french?fr:en;}

// Labels are the very keys/fallbacks used by the native Options > Keybinds
// body. The localization function resolves the CURRENT game language. The
// last three keys remain in native configuration from older game revisions.
struct ActionName {std::string_view id;const char* fallback;};
constexpr ActionName names[]{
    {"speed_up","Speed up"},{"speed_down","Speed down"},{"speed_toggle","Pause"},
    {"map_up","Map: scroll up"},{"map_down","Map: scroll down"},{"map_left","Map: scroll left"},{"map_right","Map: scroll right"},
    {"map_zoom_out","Map: zoom out"},{"map_zoom_in","Map: zoom in"},{"map_toggle_layer","Map: toggle layers"},{"map_toggle_tracks","Map: track mode"},
    {"mode_info","Mode: info"},{"mode_tracks","Mode: tracks"},{"mode_control","Mode: control"},{"mode_stations","Mode: stations"},
    {"mode_lines","Mode: lines"},{"mode_trains","Mode: trains"},{"mode_schedules","Mode: schedules"},{"tracks_unbp","Tracks: reblueprint"},
    {"track_move","Tracks: selection"},{"track_new","Tracks: new tracks"},{"track_branch","Tracks: allow branching"},
    {"track_sat_split","Tracks: split"},{"track_sat_track_tape","Tracks: track tape"},{"track_sat_building_tape","Tracks: building tape"},
    {"track_sat_parallel_tape","Tracks: parallel tape"},{"track_station","Tracks: new station"},{"track_building","Tracks: new building"},
    {"track_poi","Tracks: new POI"},{"track_build_selection","Tracks: build selection"},{"track_sel_signal","Tracks: select signal"},
    {"track_new_signal","Tracks: new signal"},{"track_2x","Tracks: double tracks"},{"track_flip_signal","Tracks: flip track side"},
    {"track_layer_up","Tracks: incr. new track layer"},{"track_layer_down","Tracks: decr. new track layer"},
    {"track_pick_parent","Tracks: pick par. parent"},{"track_plat_promote","Tracks: promote platform"},
    {"track_walk_link","Tracks: walking connection"},{"track_sat_platform_tape","Tracks: platform tape"},{"track_plat_extension","Tracks: extend platform"},
};
std::string nativeText(const std::string& id,const std::string& fallback){
    using Localize=const char*(*)(const char*,const char*);
    const auto source=reinterpret_cast<Localize>(module+0x2d82e0)(id.c_str(),fallback.c_str());
    // Copy borrowed translated bytes now. No foreign string ownership crosses
    // into the registry; a language replacement cannot invalidate its labels.
    std::string result;result.reserve(80);
    // Read bounded chunks without crossing a page boundary, rather than one
    // ReadProcessMemory system call per translated character.
    if(source)while(result.size()<256){
        const auto at=reinterpret_cast<uint64_t>(source)+result.size();
        const auto count=std::min({size_t(64),size_t(256-result.size()),size_t(4096-(at&4095))});
        std::array<char,64> bytes{};
        if(!read(nullptr,at,bytes.data(),count))return fallback;
        const auto end=std::find(bytes.begin(),bytes.begin()+count,char{});
        result.append(bytes.begin(),end);
        if(end!=bytes.begin()+count)return result.empty()?fallback:result;
    }
    return fallback;
}
std::string nativeLabel(std::string_view action){
    const auto found=std::find_if(std::begin(names),std::end(names),[&](const auto& item){return item.id==action;});
    const auto fallback=found==std::end(names)?std::string(action):std::string(found->fallback);
    return nativeText("kb_"+std::string(action),fallback);
}
bool refreshBindings(bool force=true){
    auto& cache=nativeCache;
    const auto now=std::chrono::steady_clock::now();
    // Drawing an open options page polls at most four times per second.
    // Assignment and dispatch always bypass this display-only throttle.
    if(!force&&cache.lastRead!=std::chrono::steady_clock::time_point{}&&now-cache.lastRead<std::chrono::milliseconds(250))return cache.accepted;
    cache.lastRead=now;
    auto values=readGameKeyBindings(read,nullptr,module);
    if(!values){
        cache.accepted=false;
        if(!cache.unknownPublished)cache.unknownPublished=bool(host().registry.listNativeBindings({},false));
        return false;
    }
    const auto language=gameLanguage(read,nullptr,module).value_or("");
    uint64_t languageObject{};
    if(!get(module+0xb7b610,languageObject)){cache.accepted=false;(void)host().registry.listNativeBindings({},false);return false;}
    // The table is still freshly read for assignment/dispatch. Translation,
    // conflict rebuilding and registry publication occur only after a change.
    if(cache.accepted&&cache.values==values&&cache.language==language&&cache.languageObject==languageObject)return true;
    std::vector<model::NativeBinding> bindings;bindings.reserve(values->size()*8+44);
    for(const auto& value:*values){
        const auto key=gameShortcutKey(value.key);if(!key)continue; // cannot be assigned by this SDK's grammar
        const auto label=nativeLabel(value.action);
        // Native FUN_726750 compares keycode only, including when invoked by
        // the gameplay dispatcher. Ctrl/Alt/Shift do not free a native key.
        for(uint8_t modifiers=0;modifiers<8;++modifiers)bindings.push_back({{*key,modifiers},label});
    }
    // FUN_6830d0 handles Escape before the configurable matcher, then F11/G
    // on key release. G is conditional (the native speed/status panel), but
    // can become active later in the session: it is not a free mod key.
    const bool french=language.starts_with("fr");
    for(uint8_t modifiers=0;modifiers<8;++modifiers){
        bindings.push_back({{shortcuts::Key::Escape,modifiers},french?"Fermer / menu du jeu":"Close / game menu"});
        bindings.push_back({{shortcuts::Key::F11,modifiers},french?"Diagnostics du jeu":"Game diagnostics"});
        bindings.push_back({{shortcuts::Key::G,modifiers},french?"Contrôle de la vitesse du jeu":"Game speed controls"});
    }
    // Editor FUN_786320 separately dispatches these keys whenever Ctrl is
    // held; extra Shift/Alt do not free them. Use the native translated tips
    // where present in FUN_7af120, and an SDK-localized label for cut.
    const std::array<std::pair<shortcuts::Key,std::string>,5> editor{{
        {shortcuts::Key::Z,nativeText("editor_tip_undo","Ctrl-Z to undo the last edit")},
        {shortcuts::Key::C,nativeText("editor_tip_copy","Ctrl-C to copy selected objects into clipboard")},
        {shortcuts::Key::V,nativeText("editor_tip_paste","Ctrl-V to paste clipboard objects")},
        {shortcuts::Key::B,nativeText("editor_tip_dupe","Ctrl-B to duplicate selected objects in-place")},
        {shortcuts::Key::X,french?"Couper les objets sélectionnés":"Cut selected objects"}
    }};
    for(const auto& [key,label]:editor)for(uint8_t modifiers=1;modifiers<8;modifiers+=2)
        bindings.push_back({{key,modifiers},label});
    const bool accepted=bool(host().registry.listNativeBindings(std::move(bindings)));
    if(accepted){cache.values=std::move(values);cache.language=language;cache.languageObject=languageObject;cache.accepted=true;cache.unknownPublished=false;}
    else cache.accepted=false;
    return accepted;
}
std::string conflictLabel(const model::Snapshot& snapshot,uint64_t owner,std::string_view field,
        std::string_view fallback,std::string_view language){
    if(!owner)return std::string(fallback);
    const auto entry=std::find_if(snapshot.mods.begin(),snapshot.mods.end(),[&](const auto& item){return item.token==owner;});
    if(entry==snapshot.mods.end())return std::string(tr(language.starts_with("fr"),"Un autre mod","Another mod"));
    auto label=host().translate(owner,entry->title,language);
    const auto setting=std::find_if(entry->fields.begin(),entry->fields.end(),[&](const auto& item){return item.id==field;});
    if(setting!=entry->fields.end())label+=" — "+host().translate(owner,setting->label,language);
    return label;
}
void recordResult(uint64_t token,std::string_view field,const model::Result& result){
    auto key=std::make_pair(token,std::string(field));
    if(result){presentation.errors.erase(key);return;}
    std::string message;
    const bool fr=presentation.french;
    switch(result.status){
        case model::Status::Conflict:{
            message=tr(fr,"Déjà utilisé par : ","Already used by: ");
            const auto language=gameLanguage(read,nullptr,module).value_or("");
            message+=conflictLabel(*host().registry.snapshot(),result.conflictOwner,result.conflictField,result.conflict,language);
            break;
        }
        case model::Status::NativeUnavailable:message=tr(fr,"Les raccourcis du jeu ne sont pas encore vérifiés. Réessayez.","Game shortcuts are not verified yet. Please try again.");break;
        case model::Status::Busy:message=tr(fr,"Modification en attente. Réessayez.","Settings are busy. Please try again.");break;
        case model::Status::NotFound:message=tr(fr,"Ce mod n’est plus chargé.","This mod is no longer loaded.");break;
        default:message=tr(fr,"Cette valeur n’a pas été appliquée.","This value could not be applied.");break;
    }
    presentation.errors[key]=std::move(message);
}
model::Result change(uint64_t token,std::string_view field,std::string value,bool shortcut){
    // Clearing cannot introduce a collision and must remain available even
    // while native bindings are unavailable (for example during a reload).
    if(shortcut&&!value.empty()&&!refreshBindings()){model::Result result{model::Status::NativeUnavailable,token};recordResult(token,field,result);return result;}
    auto result=host().change(token,field,std::move(value));recordResult(token,field,result);return result;
}
void freeze(uint64_t capture){
    auto& view=presentation;view.captureObject=capture;
    const auto language=gameLanguage(read,nullptr,module).value_or("");
    view.french=language.starts_with("fr");
    view.nativeKnown=refreshBindings(!view.lastLayout||view.serial-view.lastLayout>1);
    view.lastLayout=view.serial;
    const auto snapshot=host().registry.snapshot();
    if(view.registryRevision!=snapshot->revision)view.categories=OptionsCategories::from(*snapshot);
    if(optionsFrame){
        optionsFrame->categories=view.categories;
        optionsFrame->page=optionsFrame->categories.select(optionsFrame->page);
        optionsFrame->pendingPage=optionsFrame->page;
    }
    if(view.capture&&!captureAvailable(*snapshot,view.capture.token,view.capture.field,
            optionsFrame&&optionsFrame->nrf&&optionsFrame->page==OptionsPage::Shortcuts))view.capture={};
    view.frozenCapture=view.capture;
    // Metadata can be published just after registration. A separate generation
    // prevents caching fallback labels indefinitely during that brief race.
    const auto catalogueRevision=host().catalogueRevision();
    const bool rebuild=view.registryRevision!=snapshot->revision||view.catalogueRevision!=catalogueRevision||view.language!=language;
    if(rebuild){
      view.loaderMessage=host().loaderMessage(view.french);
      view.mods.clear();
      for(const auto& entry:snapshot->mods){
        DrawMod mod{entry.token,host().translate(entry.token,entry.title,language),{},{}};
        for(size_t i=0;i<entry.fields.size();++i){
            DrawField field{entry.fields[i],entry.values[i],{},{}};
            field.field.label=host().translate(entry.token,field.field.label,language);
            field.field.description=host().translate(entry.token,field.field.description,language);
            for(auto& choice:field.field.choices)choice.label=host().translate(entry.token,choice.label,language);
            if(const auto conflict=entry.conflicts.find(field.field.id);conflict!=entry.conflicts.end()){
                if(conflict->second==model::Registry::nativeUnavailable)
                    field.conflict=tr(view.french,"Les raccourcis du jeu ne sont pas encore vérifiés.","Game shortcuts are not verified yet.");
                else{
                    const auto source=entry.conflictSources.find(field.field.id);
                    const auto label=source==entry.conflictSources.end()?conflict->second:
                        conflictLabel(*snapshot,source->second.first,source->second.second,conflict->second,language);
                    field.conflict=std::string(tr(view.french,"Déjà utilisé par : ","Already used by: "))+label;
                }
            }
            mod.fields.push_back(std::move(field));
        }
        view.mods.push_back(std::move(mod));
      }
      view.registryRevision=snapshot->revision;view.catalogueRevision=catalogueRevision;view.language=language;
    }
    // Mutable feedback is frozen each layout, but potentially thousands of
    // field/choice translations are rebuilt only for a new snapshot/language.
    for(auto& mod:view.mods){
        const auto error=view.errors.find({mod.token,""});
        mod.error=error==view.errors.end()?std::string{}:error->second;
        if(const auto storage=host().storageError(mod.token);!storage.empty()){
            if(!mod.error.empty())mod.error+="\n";
            mod.error+=view.french?storage:
                storage.starts_with("Options sauvegardées illisibles")?"Saved options could not be read; default values are in use.":
                "Options could not be saved. The SDK will try again.";
        }
        for(auto& field:mod.fields){
            const auto fieldError=view.errors.find({mod.token,field.field.id});
            field.error=fieldError==view.errors.end()?field.conflict:fieldError->second;
        }
    }
    if(!rebuild)return;
    // Bound transient UI state by the currently registered fields after changes.
    const auto live=[&](const auto& key){return std::any_of(view.mods.begin(),view.mods.end(),[&](const auto& mod){return mod.token==key.first&&
        (key.second.empty()||std::any_of(mod.fields.begin(),mod.fields.end(),[&](const auto& field){return field.field.id==key.second;}));});};
    std::erase_if(view.numbers,[&](const auto& item){return !live(item.first);});
    std::erase_if(view.errors,[&](const auto& item){return !live(item.first);});
}
void drawMods(const Ui& ui,OptionsPage page){
    auto& view=presentation;const bool interactive=ui.pass()==Ui::Pass::Interactive,fr=view.french;
    const bool shortcutsPage=page==OptionsPage::Shortcuts;
    const auto visible=[&](const auto& field){return (field.field.kind==model::Kind::Shortcut)==shortcutsPage;};
    if(!view.loaderMessage.empty())ui.message(view.loaderMessage.c_str());
    if(view.loaderMessage.empty()&&std::none_of(view.mods.begin(),view.mods.end(),[&](const auto& mod){return std::any_of(mod.fields.begin(),mod.fields.end(),visible);}))
        ui.message(shortcutsPage?tr(fr,"Les raccourcis des mods chargés apparaissent ici.","Shortcuts from loaded mods appear here."):
            tr(fr,"Les options des mods chargés apparaissent ici.","Options from loaded mods appear here."));
    if(shortcutsPage&&!view.nativeKnown)ui.message(tr(fr,"Vérification des raccourcis du jeu en cours. Leur attribution reste désactivée.","Game shortcut verification is pending. Shortcut assignment is disabled."));
    for(const auto& mod:view.mods){
        if(std::none_of(mod.fields.begin(),mod.fields.end(),visible))continue;
        ui.heading(mod.title.c_str());
        for(const auto& item:mod.fields){
            if(!visible(item))continue;
            const auto& field=item.field;
            if(field.kind==model::Kind::Boolean){
                uint32_t value=item.value=="true";const auto before=value;
                ui.checkbox(field.label.c_str(),field.description.c_str(),value);
                if(interactive&&value!=before)(void)change(mod.token,field.id,value?"true":"false",false);
            }else if(field.kind==model::Kind::Integer){
                int32_t value{};std::from_chars(item.value.data(),item.value.data()+item.value.size(),value);
                auto& draft=view.numbers[{mod.token,field.id}];
                ui.numberField(field.label.c_str(),draft,value,field.minimum,field.maximum,true);
                if(!field.description.empty())ui.message(field.description.c_str());
                const auto parsed=draft.value(field.minimum,field.maximum);
                if(ui.button(tr(fr,"Appliquer cette valeur","Apply this value"),draft.modified&&parsed.has_value())&&interactive&&parsed){
                    if(change(mod.token,field.id,std::to_string(*parsed),false))draft.modified=false;
                }
            }else if(field.kind==model::Kind::Choice){
                ui.heading(field.label.c_str());if(!field.description.empty())ui.message(field.description.c_str());
                for(const auto& choice:field.choices){
                    const bool selected=choice.id==item.value;
                    const auto label=selected?choice.label+tr(fr," (sélectionné)"," (selected)"):choice.label;
                    if(ui.button(label.c_str(),!selected)&&interactive)(void)change(mod.token,field.id,choice.id,false);
                }
            }else{
                ui.heading(field.label.c_str());if(!field.description.empty())ui.message(field.description.c_str());
                const auto label=item.value.empty()?tr(fr,"Attribuer un raccourci","Assign a shortcut"):item.value.c_str();
                if(ui.button(label,view.nativeKnown)&&interactive)view.capture={mod.token,view.serial,field.id};
                if(ui.button(tr(fr,"Effacer le raccourci","Clear shortcut"),!item.value.empty())&&interactive){
                    if(change(mod.token,field.id,"",true))view.capture={};
                }
                if(view.frozenCapture.token==mod.token&&view.frozenCapture.field==field.id)
                    ui.message(tr(fr,"Appuyez sur la combinaison souhaitée. Échap annule ; Retour arrière efface.","Press the desired combination. Escape cancels; Backspace clears."));
            }
            if(!item.error.empty())ui.message(item.error.c_str());
        }
        if(ui.button(tr(fr,"Rétablir les valeurs par défaut de ce mod","Restore this mod’s defaults"),true)&&interactive){
            const bool hasShortcut=std::any_of(mod.fields.begin(),mod.fields.end(),[](const auto& field){return field.field.kind==model::Kind::Shortcut;});
            auto result=hasShortcut&&!refreshBindings()?model::Result{model::Status::NativeUnavailable,mod.token}:host().reset(mod.token);
            recordResult(mod.token,"",result);
            if(result){
                std::erase_if(view.numbers,[&](const auto& item){return item.first.first==mod.token;});
                std::erase_if(view.errors,[&](const auto& item){return item.first.first==mod.token;});
                view.capture={};
            }
        }
        if(!mod.error.empty())ui.message(mod.error.c_str());
        ui.separator();
    }
}
// Native sidebar rows use align=0xa0, height=30 and four-pixel bottom
// spacing. These are declaration options, not persistent widget/game state.
void navigationRow(uint64_t declaration){
    const auto& layout=engine::gameLayout(engine::LiveStateProfile::Windows119);
    put(declaration+layout.ui_align_enabled,uint8_t{1});put(declaration+layout.ui_align_value,uint32_t{0xa0});
    put(declaration+layout.ui_height_enabled,uint8_t{1});put(declaration+layout.ui_height_value,30.f);
    put(declaration+layout.ui_margin_enabled,uint8_t{1});put(declaration+layout.ui_margin_value,std::array<float,4>{0,0,0,4});
}
bool pageButton(const Ui& ui,const char* label,bool selected){
    navigationRow(ui.nativeObject());
    const auto function=ui.pass()==Ui::Pass::Layout?originalLayoutButton:originalInteractiveButton;
    return function(ui.nativeObject(),label,selected?2u:0u)!=0;
}
void render(uint64_t capture,uint64_t declaration){
    if(!optionsFrame||!optionsFrame->nrf){originalBody(capture,declaration);return;}
    auto& current=*optionsFrame;
    // The first body pass chooses the rendering tree for this entire cycle.
    // A successful later bind may not reverse an earlier native fallback.
    if(current.body.native()){originalBody(capture,declaration);return;}
    bool nativeEmitted=false;
    const auto nativeFallback=[&]{nativeEmitted=true;originalBody(capture,declaration);};
    const auto failBeforeEmission=[&]{
        current.failed=true;presentation.capture={};
        if(current.body.fail()==OptionsBodyLayout::Action::Native&&!nativeEmitted)nativeFallback();
        // Once SDK Layout has begun, never emit the unrelated native body
        // against its layout nodes, even when Interactive cannot be bound.
    };
    try {
        auto ui=Ui::bind(read,nullptr,module,declaration,engine::LiveStateProfile::Windows119,textClock);
        if(!ui){failBeforeEmission();return;}
        const size_t pass=ui->pass()==Ui::Pass::Layout?0:1;
        const auto action=current.body.prepare(pass==0,!current.failed&&current.emitters[pass]==declaration&&current.injected[pass]);
        if(action!=OptionsBodyLayout::Action::Sdk){
            current.failed=true;presentation.capture={};
            if(action==OptionsBodyLayout::Action::Native)nativeFallback();
            return;
        }
        if(ui->pass()==Ui::Pass::Layout)freeze(capture);
        if(presentation.captureObject!=capture){failBeforeEmission();return;}
        current.body.beginSdk();
        const auto page=current.page;
        if(page==OptionsPage::Shortcuts)presentation.drawn=presentation.serial;
        const bool interactive=ui->pass()==Ui::Pass::Interactive;
        const auto& layout=engine::gameLayout(engine::LiveStateProfile::Windows119);
        using Geometry=OptionsPanelGeometry;
        put(declaration+layout.ui_width_enabled,uint8_t{1});put(declaration+layout.ui_width_value,Geometry::width);
        put(declaration+layout.ui_height_enabled,uint8_t{1});put(declaration+layout.ui_height_value,Geometry::height);
        put(declaration+layout.ui_flow_enabled,uint8_t{1});put(declaration+layout.ui_flow_value,Geometry::columnFlow);
        put(declaration+layout.ui_align_enabled,uint8_t{1});put(declaration+layout.ui_align_value,Geometry::columnAlign);
        reinterpret_cast<void(*)(uint64_t)>(module+(interactive?layout.ui_interactive_box:layout.ui_layout_box))(declaration);
        struct Column {uint64_t object,function;~Column(){reinterpret_cast<void(*)(uint64_t)>(function)(object);}}
            column{declaration,module+(interactive?layout.ui_interactive_box_end:layout.ui_layout_box_end)};
        // The native label's default width is zero outside a scroll child;
        // declare HFILL explicitly instead of relying on fillRows_ there.
        put(declaration+layout.ui_align_enabled,uint8_t{1});put(declaration+layout.ui_align_value,Geometry::rowAlign);
        const bool navigationVisible=current.categories.navigation();
        ui->heading(navigationVisible||!presentation.loaderMessage.empty()?"NRF Hub":page==OptionsPage::Shortcuts?tr(presentation.french,"Raccourcis","Shortcuts"):"Interface");
        if(navigationVisible){
            if(pageButton(*ui,"Interface",page==OptionsPage::Interface)&&interactive)current.pendingPage=OptionsPage::Interface;
            if(pageButton(*ui,tr(presentation.french,"Raccourcis","Shortcuts"),page==OptionsPage::Shortcuts)&&interactive)current.pendingPage=OptionsPage::Shortcuts;
        }
        // The original native Interface body is not emitted here. It remains
        // unchanged in the game's Interface tab. Both passes use the frozen
        // SDK page/rows and a separate scroll key for each category.
        ui->scroll(page==OptionsPage::Shortcuts?"##nrf_shortcuts":"##nrf_interface",Geometry::viewport(navigationVisible),
            [&](const Ui& child){drawMods(child,page);});
    }catch(...){
        detail::diagnostics::exception("sdk","native mod options render");
        failBeforeEmission();
        // Once a child layout was emitted, drawing the original again on the
        // parent would desynchronize native passes. Failure leaves this frame
        // blank; the next normal native callback can retry with owned data.
        OutputDebugStringA("NIMBY SDK: native options frame was not completed\n");
    }
}
// Return addresses, not translated strings or a global widget counter, qualify
// the seven native menu buttons. The first button identifies this dispatcher's
// root emitter separately in layout and interaction; nested emitters are ignored.
constexpr std::array<std::array<uint64_t,7>,7> menuCalls{{
    {{0x5ef8ba,0x5ef9bc,0x5efa6c,0x5efb2d,0x5efbec,0x5efcd5,0x5efd90}},
    {{0x5f02ba,0x5f03bc,0x5f046c,0x5f052d,0x5f05ec,0x5f06d5,0x5f0790}},
    {{0x5f0cba,0x5f0dbc,0x5f0e6c,0x5f0f2d,0x5f0fec,0x5f10d5,0x5f1190}},
    {{0x5f16ba,0x5f17bc,0x5f186c,0x5f192d,0x5f19ec,0x5f1ad5,0x5f1b90}},
    {{0x5f20ba,0x5f21bc,0x5f226c,0x5f232d,0x5f23ec,0x5f24d5,0x5f2590}},
    {{0x5f2baa,0x5f2cac,0x5f2d5c,0x5f2e1d,0x5f2edc,0x5f2fc5,0x5f3080}},
    {{0x5f35a6,0x5f36b2,0x5f3766,0x5f3825,0x5f38e2,0x5f39c9,0x5f3a81}}
}};
uint8_t menuButton(uint64_t declaration,const char* label,uint32_t flags,uint64_t caller,size_t pass){
    const auto original=pass==0?originalLayoutButton:originalInteractiveButton;
    auto* context=optionsFrame;
    if(!context||caller<module)return original(declaration,label,flags);
    size_t menu=menuCalls.size(),item=7;
    for(size_t i=0;i<menuCalls.size()&&menu==menuCalls.size();++i)
        for(size_t j=0;j<7;++j)if(caller-module==menuCalls[i][j]){menu=i;item=j;break;}
    if(menu==menuCalls.size())return original(declaration,label,flags);
    if(item==0&&!context->emitters[pass]){context->emitters[pass]=declaration;context->menus[pass]=menu+1;}
    if(context->emitters[pass]!=declaration||context->menus[pass]!=menu+1)return original(declaration,label,flags);
    const auto clicked=original(declaration,label,context->nrf?flags&~uint32_t{2}:flags);
    if(clicked&&pass==1)context->nativeClicked=int(item);
    if(item==6&&!context->injected[pass]){
        // Emit once in each native pass, directly through the trampoline to
        // avoid recursion. The native function resets these options itself.
        context->injected[pass]=true;
        navigationRow(declaration);
        if(original(declaration,"NRF Hub",context->nrf?2u:0u)&&pass==1)context->requested=true;
    }
    return clicked;
}
uint8_t layoutButton(uint64_t declaration,const char* label,uint32_t flags){
#ifdef _MSC_VER
    const auto caller=reinterpret_cast<uint64_t>(_ReturnAddress());
#else
    const auto caller=reinterpret_cast<uint64_t>(__builtin_return_address(0));
#endif
    return menuButton(declaration,label,flags,caller,0);
}
uint8_t interactiveButton(uint64_t declaration,const char* label,uint32_t flags){
#ifdef _MSC_VER
    const auto caller=reinterpret_cast<uint64_t>(_ReturnAddress());
#else
    const auto caller=reinterpret_cast<uint64_t>(__builtin_return_address(0));
#endif
    return menuButton(declaration,label,flags,caller,1);
}
uint8_t options(uint64_t owner,uint64_t nativeFrame,uint64_t context,uint8_t flag){
    // A nested native dispatch belongs to its caller. Never steal its emitter,
    // pending tab or close result, even if it uses the same button primitives.
    if(optionsFrame){
        struct Nested {OptionsFrame* previous;~Nested(){optionsFrame=previous;}} nested{std::exchange(optionsFrame,nullptr)};
        return originalOptions(owner,nativeFrame,context,flag);
    }
    int before{};
    if(!get(owner+0x10c,before)||before<0||before>6)return originalOptions(owner,nativeFrame,context,flag);
    if(navigation.owner!=owner||navigation.nativeTab!=before){
        navigation={owner,before,false,OptionsPage::Interface};
        presentation.capture={};presentation.frozenCapture={};
    }
    OptionsFrame current;current.nrf=navigation.selected;
    current.page=navigation.page;current.pendingPage=current.page;
    uint8_t closed{};int pending=before;
    {
        // Only valid native Interface index1 is borrowed to route this call.
        // The original dispatcher still initializes/returns Close and handles
        // the uploader tail. Even an exception restores the owner's native tab
        // before leaving this callback; cleanup never holds/dereferences owner.
        struct Scope {
            uint64_t owner;int previous;bool routed;
            ~Scope(){if(routed)put(owner+0x10c,previous);optionsFrame=nullptr;}
        } scope{owner,before,current.nrf};
        optionsFrame=&current;
        if(current.nrf)put(owner+0x10c,int{1});
        closed=originalOptions(owner,nativeFrame,context,flag);
        if(!get(owner+0x10c,pending)||pending<0||pending>6)current.failed=true;
    }
    if(current.nrf)pending=current.nativeClicked>=0?current.nativeClicked:before;
    if(current.failed)pending=before;
    // Native wrappers commit their pendingTab only after BOTH passes. SDK
    // navigation follows that same boundary, keeping widget counts identical.
    if(pending>=0&&pending<=6){put(owner+0x10c,pending);navigation.nativeTab=pending;}
    navigation.selected=!closed&&!current.failed&&
        (current.requested||(current.nrf&&current.nativeClicked<0));
    navigation.page=current.pendingPage;
    if(!navigation.selected||navigation.page!=OptionsPage::Shortcuts||navigation.page!=current.page){
        presentation.capture={};presentation.frozenCapture={};
    }
    return closed;
}
bool poll(void* event){
    if(!event||!input.collecting)return originalPoll(event);
    for(;;){
        // Malicious/repeated captured events cannot monopolize this frame.
        // Unprocessed SDL events remain queued; held ownership is retained.
        if(input.suppressed==input.events.size())return false;
        if(!originalPoll(event)){input.drained=true;return false;}
        std::array<unsigned char,40> bytes{};std::memcpy(bytes.data(),event,bytes.size());
        const auto type=engine::memory::field<uint32_t>(bytes.data(),0);
        const auto window=engine::memory::field<uint32_t>(bytes.data(),16);
        if(type==0x20f&&window==input.primaryWindow){ // SDL_EVENT_WINDOW_FOCUS_LOST
            input.focusLost=true;input.captureAllowed=false;
        }
        if(type!=0x300&&type!=0x301)return true;
        const auto code=engine::memory::field<uint32_t>(bytes.data(),28);
        const auto key=gameShortcutKey(code);
        const auto scancode=engine::memory::field<uint32_t>(bytes.data(),24);
        const bool down=type==0x300,repeat=bytes[37]!=0;
        const auto route=capturedKeys.route(window,scancode,key?uint16_t(*key):0,down,repeat,input.captureAllowed);
        if(down&&route!=CapturedKeys::Route::Consumed){
            if(input.count==input.events.size())input.overflow=true;
            else input.events[input.count++]={window,code,engine::memory::field<uint16_t>(bytes.data(),32),repeat,
                route==CapturedKeys::Route::Capture};
        }
        if(route==CapturedKeys::Route::Native)return true;
        ++input.suppressed;
    }
}
bool foreground(){DWORD pid{};const auto window=GetForegroundWindow();return window&&GetWindowThreadProcessId(window,&pid)&&pid==GetCurrentProcessId();}
uint16_t pump(uint64_t frame){
    input.count=0;input.suppressed=0;input.primaryWindow=0;
    input.overflow=false;input.captureAllowed=false;input.focusLost=false;input.drained=false;input.collecting=true;
    uint64_t primary{};
    const bool focused=foreground();
    if(presentation.capture&&!captureAvailable(*host().registry.snapshot(),presentation.capture.token,presentation.capture.field,
            navigation.selected&&navigation.page==OptionsPage::Shortcuts&&presentation.drawn==presentation.serial)){
        presentation.capture={};presentation.frozenCapture={};
    }
    if(get(frame+0x78,primary)&&primary){
        input.primaryWindow=windowId(reinterpret_cast<void*>(primary));
        capturedKeys.window(input.primaryWindow);
        input.captureAllowed=presentation.capture&&presentation.drawn==presentation.serial&&focused&&
            !presentation.textFocused&&!textInput(reinterpret_cast<void*>(primary));
    }
    if(!focused){presentation.capture={};input.focusLost=true;}
    struct Stop {~Stop(){input.collecting=false;}} stop;
    const auto result=originalPump(frame);
    if(!capturedKeys.empty()){
        int count{};const bool* states=input.drained?keyboardState(&count):nullptr;
        capturedKeys.finish(states,count>0?size_t(count):0,!input.focusLost&&foreground());
    }
    if(input.focusLost)presentation.capture={};
    return result;
}
bool textFocused(uint64_t shell,uint64_t rawFrame)noexcept{
    uint64_t primary{};uint8_t nativeText{};
    if(!get(rawFrame+0x78,primary)||!primary||!get(shell+0x64f0,nativeText))return true;
    return nativeText!=0||textInput(reinterpret_cast<void*>(primary));
}
void handleInput(uint64_t rawFrame,bool typing){
    auto& view=presentation;
    if(view.capture&&!captureAvailable(*host().registry.snapshot(),view.capture.token,view.capture.field,
            navigation.selected&&navigation.page==OptionsPage::Shortcuts&&view.drawn==view.serial)){
        view.capture={};view.frozenCapture={};
    }
    if(!input.count||input.overflow||!foreground()){input.count=0;return;}
    uint64_t primary{};
    if(!get(rawFrame+0x78,primary)||!primary){input.count=0;return;}
    const auto id=windowId(reinterpret_cast<void*>(primary));
    if(typing&&!view.capture){input.count=0;return;}
    if(host().registry.snapshot()->mods.empty()){input.count=0;view.capture={};return;}
    const auto count=std::exchange(input.count,0);
    bool verified=false;
    for(size_t i=0;i<count;++i){
        const auto& event=input.events[i];
        if(event.repeat||event.window!=id)continue;
        const auto key=gameShortcutKey(event.key);const auto modifiers=gameShortcutModifiers(event.modifiers);
        if(!key||!modifiers)continue;
        if(view.capture){
            if(view.capture.started>=view.serial)continue; // keys already queued before clicking Assign
            if(*key==shortcuts::Key::Escape){view.capture={};break;}
            if(typing)continue;
            const auto token=view.capture.token;const auto field=view.capture.field;
            auto value=*key==shortcuts::Key::Backspace?std::string{}:shortcuts::format({*key,*modifiers});
            if(change(token,field,std::move(value),true))view.capture={};
            // A capture consumes this SDK input batch even after success.
            break;
        }
        if(event.captured)continue; // capture was cancelled by navigation during the native frame
        // Native changes during originalFrame are read before the first real
        // dispatch. Repeats, unsupported chords and foreign windows cost no scan.
        if(!verified){if(!refreshBindings())return;verified=true;}
        host().registry.dispatch({*key,*modifiers,false},{true,typing,false});
    }
}
uint64_t frame(uint64_t shell,uint64_t rawFrame){
    ++presentation.serial;
    const bool before=textFocused(shell,rawFrame);
    const auto result=originalFrame(shell,rawFrame);
    presentation.textFocused=before||textFocused(shell,rawFrame);
    try{handleInput(rawFrame,presentation.textFocused);}catch(...){input.count=0;presentation.capture={};detail::diagnostics::exception("sdk","native mod shortcuts");}
    return result;
}
}

void cleanupHooks()noexcept {
    for(size_t i=created;i>0;--i)(void)MH_RemoveHook(targets[i-1]);
    created=0;targets={};originalBody=nullptr;originalPump=nullptr;originalFrame=nullptr;originalPoll=nullptr;
    originalOptions=nullptr;originalLayoutButton=nullptr;originalInteractiveButton=nullptr;
    windowId=nullptr;textInput=nullptr;keyboardState=nullptr;module=0;
}
bool installUi(uint64_t base)noexcept {
    if(created)return module==base;
    try{
        module=base;
        constexpr unsigned char bodyEntry[]{0x48,0x89,0x5c,0x24,0x20,0x48,0x89,0x54,0x24,0x10,0x55,0x56,0x57,0x41,0x54,0x41};
        constexpr unsigned char pumpEntry[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56};
        constexpr unsigned char frameEntry[]{0x48,0x89,0x5c,0x24,0x08,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
        constexpr unsigned char optionsEntry[]{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x55,0x57,0x41,0x56,0x48,0x8d};
        constexpr unsigned char layoutButtonEntry[]{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x48,0x8d,0x15,0x90,0x18,0x63,0x00};
        constexpr unsigned char interactiveButtonEntry[]{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x48};
        const auto matches=[&](uint64_t rva,const auto& expected){std::array<unsigned char,16> bytes{};return read(nullptr,module+rva,bytes.data(),bytes.size())&&!std::memcmp(bytes.data(),expected,bytes.size());};
        if(!matches(0x5d0d70,bodyEntry)||!matches(0x2d4040,pumpEntry)||!matches(0x7272d0,frameEntry)||
           !matches(0x5dc920,optionsEntry)||!matches(0x55c9c0,layoutButtonEntry)||!matches(0x55ef00,interactiveButtonEntry)){cleanupHooks();return false;}
        uint64_t pollAddress{};HMODULE sdl{};
        if(!get(module+0x9ab8f0,pollAddress)||!pollAddress||
           !GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(pollAddress),&sdl)||
           reinterpret_cast<uint64_t>(GetProcAddress(sdl,"SDL_PollEvent"))!=pollAddress){cleanupHooks();return false;}
        windowId=std::bit_cast<WindowId>(GetProcAddress(sdl,"SDL_GetWindowID"));
        textInput=std::bit_cast<TextInput>(GetProcAddress(sdl,"SDL_TextInputActive"));
        keyboardState=std::bit_cast<KeyboardState>(GetProcAddress(sdl,"SDL_GetKeyboardState"));
        if(!windowId||!textInput||!keyboardState){cleanupHooks();return false;}
        targets={reinterpret_cast<void*>(module+0x5d0d70),reinterpret_cast<void*>(module+0x2d4040),
            reinterpret_cast<void*>(module+0x7272d0),reinterpret_cast<void*>(pollAddress),reinterpret_cast<void*>(module+0x5dc920),
            reinterpret_cast<void*>(module+0x55c9c0),reinterpret_cast<void*>(module+0x55ef00)};
        const std::array<void*,7> detours{reinterpret_cast<void*>(render),reinterpret_cast<void*>(pump),reinterpret_cast<void*>(frame),reinterpret_cast<void*>(poll),
            reinterpret_cast<void*>(options),reinterpret_cast<void*>(layoutButton),reinterpret_cast<void*>(interactiveButton)};
        const std::array<void**,7> originals{reinterpret_cast<void**>(&originalBody),reinterpret_cast<void**>(&originalPump),reinterpret_cast<void**>(&originalFrame),reinterpret_cast<void**>(&originalPoll),
            reinterpret_cast<void**>(&originalOptions),reinterpret_cast<void**>(&originalLayoutButton),reinterpret_cast<void**>(&originalInteractiveButton)};
        for(size_t i=0;i<targets.size();++i){
            if(MH_CreateHook(targets[i],detours[i],originals[i])!=MH_OK){cleanupHooks();return false;}++created;
        }
        for(auto target:targets)if(MH_QueueEnableHook(target)!=MH_OK){cleanupHooks();return false;}
        return true;
    }catch(...){cleanupHooks();return false;}
}
}
