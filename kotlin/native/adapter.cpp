#include <nimby/detail/diagnostics.hpp>
#include <atomic>
#include <nimby/kotlin_mod.hpp>
#include <nimby/signal_settings_store.hpp>
#include <nimby/detail/signal_settings_runtime.hpp>
#include <nimby/detail/native_library.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <set>
#include <nimby/detail/translations.hpp>
#include <nimby/detail/mod_options_client.hpp>
#include <nimby/detail/mod_option_windows.hpp>
#include <engine/mod_shortcuts.h>
#include <chrono>
#include <sstream>
#include "tool_context.hpp"

#ifndef NIMBY_KOTLIN_LIBRARY
#error The SDK build tool must specify the Kotlin library name.
#endif
namespace nimby::kotlin {
namespace {
void migrateSettings(std::string_view panelId,std::map<std::string,bool,std::less<>>& values);
using LastError=int(*)(char*,int);
std::atomic<LastError> lastError{nullptr};
void check(int status) {
    if(status>=0)return;
    std::array<char,32768> message{};
    const auto read=lastError.load();
    if(read && read(message.data(),int(message.size()))>0) {
        nimby::detail::diagnostics::write("mods","ERROR",message.data());
        throw std::runtime_error(message.data());
    }
    throw std::runtime_error("Kotlin mod rejected a request; no detailed exception export available");
}
struct Api {
    detail::native::Module module=nullptr;
    int (*metadata)(int,int,char*,int)=nullptr;
    int (*info)(int*)=nullptr;
    int (*defaults)(std::int64_t*)=nullptr;
    int (*decide)(int,std::int64_t,int,std::int64_t,std::int64_t,const int*,int,int,int*)=nullptr;
    int (*decideType)(int,int,std::int64_t,int,std::int64_t,std::int64_t,std::int64_t,const int*,int,int,int*)=nullptr;
    int (*prepareNetwork)(int,const int64_t*,const int*,int64_t*,int*)=nullptr;
    size_t networkLimit=Rules::maxSignals;
    int (*typeMetadata)(int,int,int,char*,int)=nullptr;
    int (*migrate)(int,const char*,const int*,int,std::int64_t*)=nullptr;
    int (*texture)(int,int,std::int64_t,std::int64_t,char*,int)=nullptr;
    int (*animation)(int,int,char*,char*,int,std::int64_t*)=nullptr;
    int (*force)(int,int*)=nullptr;
    int (*forceType)(int,int,int*)=nullptr;
    int (*fallbackType)(int,int,int*)=nullptr;
    int (*localDecision)(int,int,int*)=nullptr;
    int (*fault)(int,int)=nullptr;
    int (*driving)(int,int,double*,int*)=nullptr;
    int (*plan)(const double*,const int*,int,const std::int64_t*,const double*,double*,std::int64_t*)=nullptr;
    std::array<int,6> properties{};
    std::string id,title,catalogue,diagnostic,translationsJson;
    std::vector<std::array<std::string,3>> labels;
    std::vector<SignalCheckbox> boxes;
    struct Type {
        std::string id,title,catalogue;
        size_t approachBlocks=0;
        std::vector<std::array<std::string,3>> labels;
        std::vector<SignalCheckbox> boxes;
        std::vector<std::array<std::string,4>> actionLabels;
        std::vector<SignalAction> actions;
        std::vector<std::array<std::string,3>> numberLabels;
        std::vector<SignalNumber> numbers;
    };
    std::vector<Type> types;
    std::vector<SignalSettingsPanel> panels;
    bool tool=false;
    std::vector<std::string> services;
    int (*serviceEvent)(int,int64_t,int64_t,int64_t,int64_t,const char*,const char*,const char*,ToolCall)=nullptr;
    int (*serviceEventV2)(int,int64_t,int64_t,int64_t,int64_t,const char*,const char*,const char*,ToolCall,int,int)=nullptr;
    int (*toolTick)(const char*,int64_t,ToolCall)=nullptr;
    int (*toolStop)()=nullptr;
    std::vector<platform::ToolWindows::Definition> windows;
    detail::ModOptionsClient optionsClient;
    std::string optionsDeclaration;
    int optionCount{};
    int (*optionsApply)(const char*,int,int)=nullptr;
    void (*originalStop)()=nullptr;
    void (*originalObservationLost)()=nullptr;
    detail::ModOptionWindows optionWindows;
    std::optional<GameSession> optionsGame;
    std::vector<std::string> pendingWindows;
    std::chrono::steady_clock::time_point nextOptionsConnection{};
    int (*windowEvent)(int,int64_t,const char*,const char*,const int32_t*,int,const char*,int64_t,ToolCall)=nullptr;
    template<class T> T symbol(const char* name) {
        auto address=detail::native::symbol(module,name);if(!address)throw std::runtime_error(std::string("Missing Kotlin SDK export: ")+name);
        return reinterpret_cast<T>(address);
    }
    std::string text(int field,int index=0) {
        std::array<char,2048> buffer{};const auto count=metadata(field,index,buffer.data(),int(buffer.size()));check(count);
        if(count>=int(buffer.size())||buffer[count]!=0)throw std::runtime_error("Invalid Kotlin text length");
        return {buffer.data(),static_cast<std::size_t>(count)};
    }
    std::string typeText(int type,int field,int index=0) {
        std::array<char,2048> buffer{};
        const auto count=typeMetadata(type,field,index,buffer.data(),int(buffer.size()));check(count);
        if(count>=int(buffer.size())||buffer[count]!=0)throw std::runtime_error("Invalid Kotlin type text length");
        return {buffer.data(),static_cast<std::size_t>(count)};
    }
    void prepareOptions(int abi) {
        using Json=nlohmann::json;
        auto fields=Json::array();
        if(abi>=9){
            optionCount=symbol<int(*)()>("NRFKotlin_OptionCount")();check(optionCount);
            if(optionCount>64)throw std::runtime_error("Too many Kotlin mod options");
            const auto info=symbol<int(*)(int,int*)>("NRFKotlin_OptionInfo");
            const auto metadata=symbol<int(*)(int,int,char*,int)>("NRFKotlin_OptionMetadata");
            const auto choice=symbol<int(*)(int,int,int,char*,int)>("NRFKotlin_OptionChoiceMetadata");
            optionsApply=symbol<decltype(optionsApply)>("NRFKotlin_OptionsApply");
            auto text=[&](int index,int field){std::array<char,1025> buffer{};
                const auto n=metadata(index,field,buffer.data(),int(buffer.size()));check(n);
                if(n>=int(buffer.size())||buffer[n])throw std::runtime_error("Invalid mod option text");
                return std::string(buffer.data(),n);};
            for(int i=0;i<optionCount;++i){
                std::array<int,4> details{};check(info(i,details.data()));
                if(details[0]<0||details[0]>3||details[3]<0||details[3]>16)throw std::runtime_error("Invalid mod option type");
                Json field={{"id",text(i,0)},{"label",text(i,1)},{"description",text(i,2)},
                    {"kind",details[0]},{"default",text(i,3)},{"minimum",details[1]},{"maximum",details[2]}};
                field["choices"]=Json::array();
                for(int j=0;j<details[3];++j){std::array<std::string,2> parts;
                    for(int k=0;k<2;++k){std::array<char,257> buffer{};const auto n=choice(i,j,k,buffer.data(),int(buffer.size()));check(n);
                        if(n<1||n>=int(buffer.size())||buffer[n])throw std::runtime_error("Invalid mod option choice");parts[k].assign(buffer.data(),n);}
                    field["choices"].push_back({{"id",parts[0]},{"label",parts[1]}});
                }
                fields.push_back(std::move(field));
            }
        }
        for(const auto& window:windows)fields.push_back({{"id",optionWindows.add(window.id)},{"label",window.title},
            {"description",""},{"kind",3},{"default",window.shortcut}});
        if(fields.empty())return;
        if(fields.size()>64)throw std::runtime_error("Too many mod options and windows");
        optionsDeclaration=Json({{"id",id},{"title",title},{"fields",std::move(fields)},
            {"translations",translationsJson}}).dump();
        if(optionsDeclaration.size()>NIMBY_OPTIONS_SCHEMA_LIMIT)throw std::runtime_error("Mod options declaration too large");
    }
    void refreshOptions(){
        if(optionsDeclaration.empty())return;
        if(!optionsClient.connected()){
            const auto now=std::chrono::steady_clock::now();if(now<nextOptionsConnection)return;
            nextOptionsConnection=now+std::chrono::seconds(1);
            if(NimbyInternal_EnsureSignalUiBridge()!=NIMBY_OK||!optionsClient.connect(optionsDeclaration))return;
        }
        const auto changes=optionsClient.refresh([&](const detail::ModOptionsClient::Changes& changes){
            if(!changes.changed)return;
            if(changes.values.size()!=size_t(optionCount)+windows.size())throw std::runtime_error("Invalid mod options snapshot size");
            for(size_t i=size_t(optionCount);i<changes.values.size();++i)
                if(!engine::mod_shortcuts::parse(changes.values[i]))throw std::runtime_error("Invalid window shortcut value");
            if(optionCount){
                std::string packed;for(int i=0;i<optionCount;++i){packed+=changes.values[size_t(i)];packed+='\0';}
                check(optionsApply(packed.data(),int(packed.size()),optionCount));
            }
        });
        // These are opening requests only. The tool UI rechecks its own fresh
        // game context and foreground window before showing anything.
        pendingWindows.clear();
        for(const auto& event:changes.events)if(const auto window=optionWindows.resolve(event);!window.empty())pendingWindows.emplace_back(window);
    }
    void observeOptionsGame(const GameSession& game){
        if(optionsGame&&*optionsGame==game)return;
        // refreshOptions ran before this capture. None of its input events is
        // allowed to cross a session boundary, including the first capture.
        pendingWindows.clear();optionsClient.discardEvents();optionsGame=game;
    }
    void loseOptionsGame(){
        optionsGame.reset();pendingWindows.clear();optionsClient.discardEvents();
    }
    Api() {
        const auto ownPath=detail::native::modulePath(reinterpret_cast<const void*>(&check));
        std::optional<detail::Translations> translations;
        const auto catalogPath=ownPath.parent_path()/"translations.json";
        if(std::filesystem::exists(catalogPath)){
            const auto bytes=std::filesystem::file_size(catalogPath);
            if(!bytes||bytes>detail::Translations::maximumBytes)throw std::runtime_error("translations.json: invalid file size");
            std::ifstream input(catalogPath,std::ios::binary);translationsJson.resize(bytes);
            if(!input.read(translationsJson.data(),bytes))throw std::runtime_error("Cannot read translations.json");
            translations.emplace(translationsJson);
            detail::diagnostics::write("mods","INFO",("Loaded translations.json: "+std::to_string(translations->languageCount())+
                " languages, fallback="+translations->fallback()).c_str());
        }
        // Logs use the declared fallback, so diagnostic text stays readable
        // and stable even when the player changes language. UI declarations
        // retain their references and resolve later in the resident renderer.
        const auto diagnosticText=[&](std::string_view value){
            return detail::Translations::resolve(translations?&*translations:nullptr,value,"");
        };
        // The prebuilt adapter is reusable: rename it to MyMod.dll and place
        // MyModKotlin.dll beside it. Mod builds never compile this C++ source.
        const auto library=ownPath.parent_path()/(ownPath.extension()==".dll"||ownPath.extension()==".so"
            ? ownPath.stem().string()+"Kotlin"+ownPath.extension().string() : std::string(NIMBY_KOTLIN_LIBRARY));
        // Kotlin/Native owns GC threads. Keep its code mapped until process exit,
        // including after a loader stop/restart. No work is performed in DllMain.
        const auto libraryUtf8=library.u8string();
        detail::diagnostics::write("mods","INFO",("Loading Kotlin library: "+std::string(libraryUtf8.begin(),libraryUtf8.end())).c_str());
        module=detail::native::load(library,true);
        lastError.store(reinterpret_cast<LastError>(detail::native::symbol(module,"NRFKotlin_LastError")));
        if(const auto available=reinterpret_cast<int(*)(int)>(detail::native::symbol(module,"NRFKotlin_TranslationsAvailable")))
            check(available(translationsJson.empty()?0:1));
        const auto version=symbol<int(*)()>("NRFKotlin_Version");const auto abi=version();check(abi);
        if(abi<1||abi>9)throw std::runtime_error("Unsupported Kotlin ABI");
        metadata=symbol<decltype(metadata)>("NRFKotlin_Metadata");
        if(abi>=3){
            const auto kind=symbol<int(*)()>("NRFKotlin_ModKind")();check(kind);
            if(kind>1)throw std::runtime_error("Invalid Kotlin mod kind");tool=kind==1;
            const auto count=symbol<int(*)()>("NRFKotlin_ServiceCount")();check(count);
            if(count>32)throw std::runtime_error("Too many Kotlin services");
            auto name=symbol<int(*)(int,char*,int)>("NRFKotlin_ServiceName");
            serviceEvent=symbol<decltype(serviceEvent)>("NRFKotlin_ServiceEvent");
            serviceEventV2=reinterpret_cast<decltype(serviceEventV2)>(detail::native::symbol(module,"NRFKotlin_ServiceEventV2"));
            toolTick=symbol<decltype(toolTick)>("NRFKotlin_ToolTick");
            toolStop=symbol<decltype(toolStop)>("NRFKotlin_ToolStop");
            std::set<std::string> unique;
            for(int i=0;i<count;++i){std::array<char,129> buffer{};const auto n=name(i,buffer.data(),int(buffer.size()));check(n);
                if(n<1||n>=int(buffer.size())||buffer[n]||!unique.emplace(buffer.data(),n).second)throw std::runtime_error("Invalid service name");
                services.emplace_back(buffer.data(),n);}
        }
        if(abi>=7){
            const auto count=symbol<int(*)()>("NRFKotlin_WindowCount")();check(count);
            if(count>8||(count&&!tool))throw std::runtime_error("Invalid tool window declaration");
            windowEvent=symbol<decltype(windowEvent)>("NRFKotlin_WindowEvent");
            auto meta=symbol<int(*)(int,int,char*,int)>("NRFKotlin_WindowMetadata");
            for(int i=0;i<count;++i){std::array<std::string,3> values;
                for(int field=0;field<3;++field){std::array<char,2048> buffer{};const auto n=meta(i,field,buffer.data(),int(buffer.size()));check(n);if((n==0&&field!=2)||n>=int(buffer.size())||buffer[n])throw std::runtime_error("Window metadata");values[field].assign(buffer.data(),n);}
                windows.push_back({values[0],values[1],values[2]});
            }
        }
        id=text(0);title=text(1);prepareOptions(abi);
        if(tool){diagnostic=text(3);return;}
        if(abi>=8)prepareNetwork=symbol<decltype(prepareNetwork)>("NRFKotlin_PrepareNetwork");
        if(const auto limit=reinterpret_cast<int(*)()>(detail::native::symbol(module,"NRFKotlin_NetworkLimit"))){
            const auto count=limit();check(count);
            if(count<int(Rules::maxSignals)||count>int(Rules::maxLiveSignals))throw std::runtime_error("Invalid Kotlin network limit");
            networkLimit=size_t(count);
        }
        if(abi>=4){
            fallbackType=symbol<decltype(fallbackType)>("NRFKotlin_FallbackType");
            localDecision=symbol<decltype(localDecision)>("NRFKotlin_LocalDecision");
        }
#define LOAD(member, name) member=symbol<decltype(member)>(name)
        LOAD(metadata,"NRFKotlin_Metadata");LOAD(info,"NRFKotlin_Info");LOAD(defaults,"NRFKotlin_Defaults");
        LOAD(decide,"NRFKotlin_Decide");LOAD(texture,"NRFKotlin_Texture");LOAD(fault,"NRFKotlin_Fault");
        LOAD(driving,"NRFKotlin_Driving");LOAD(plan,"NRFKotlin_Plan");
        if(abi>=6)LOAD(animation,"NRFKotlin_TextureAnimation");
        force=reinterpret_cast<decltype(force)>(detail::native::symbol(module,"NRFKotlin_Force"));
#undef LOAD
        check(info(properties.data()));if(properties[0]<0||properties[0]>64)throw std::runtime_error("Invalid checkbox count");
        id=text(0);title=text(1);catalogue=text(2);diagnostic=text(3);
        detail::diagnostics::write("mods","INFO",("Kotlin mod loaded: "+id+" / Kotlin ABI "+std::to_string(abi)).c_str());
        std::int64_t values=0;check(defaults(&values));
        labels.reserve(properties[0]);boxes.reserve(properties[0]);
        for(int i=0;i<properties[0];++i)labels.push_back({text(4,i),text(5,i),text(6,i)});
        for(int i=0;i<properties[0];++i)boxes.push_back({labels[i][0],labels[i][1],labels[i][2],(std::uint64_t(values)&(std::uint64_t{1}<<i))!=0});
        detail::diagnostics::write("mods","INFO",("Mod ready: id="+id+" title="+diagnosticText(title)+" textures="+catalogue+" checkboxes="+std::to_string(boxes.size())+" defaultMask="+std::to_string(values)+" force="+(force?"yes":"no")+" detailedErrors="+(lastError.load()?"yes":"no")).c_str());
        for(const auto& label:labels)detail::diagnostics::write("mods","INFO",("Registered setting: "+label[0]+" / "+diagnosticText(label[1])).c_str());
        const auto countTypes=reinterpret_cast<int(*)()>(detail::native::symbol(module,"NRFKotlin_TypeCount"));
        if(abi>=2&&!countTypes)throw std::runtime_error("Missing multi-type Kotlin exports");
        if(countTypes) {
            const auto count=countTypes();check(count);
            if(count<1||count>16)throw std::runtime_error("Invalid Kotlin signal type count");
            typeMetadata=symbol<decltype(typeMetadata)>("NRFKotlin_TypeMetadata");
            decideType=symbol<decltype(decideType)>("NRFKotlin_DecideType");
            forceType=symbol<decltype(forceType)>("NRFKotlin_ForceType");
            migrate=reinterpret_cast<decltype(migrate)>(detail::native::symbol(module,"NRFKotlin_MigrateSettings"));
            const auto typeInfo=symbol<int(*)(int,std::int64_t*)>("NRFKotlin_TypeInfo");
            types.reserve(count);
            std::set<std::string> ids,catalogues;
            for(int n=0;n<count;++n) {
                auto& type=types.emplace_back();
                type.id=typeText(n,0);type.title=typeText(n,1);type.catalogue=typeText(n,2);
                if(!ids.insert(type.id).second||!catalogues.insert(type.catalogue).second)
                    throw std::runtime_error("Duplicate Kotlin signal type or catalogue");
                std::array<std::int64_t,4> description{};check(typeInfo(n,description.data()));
                if(description[3]<0||description[3]>(abi>=5?16:1))throw std::runtime_error("Invalid approach observation range");
                type.approachBlocks=size_t(description[3]);
                if(description[0]<0||description[0]>64)throw std::runtime_error("Invalid type checkbox count");
                type.labels.reserve(description[0]);type.boxes.reserve(description[0]);
                for(int i=0;i<description[0];++i)type.labels.push_back({typeText(n,4,i),typeText(n,5,i),typeText(n,6,i)});
                for(std::size_t i=0;i<type.labels.size();++i)type.boxes.push_back({type.labels[i][0],type.labels[i][1],type.labels[i][2],
                    (std::uint64_t(description[1])&(std::uint64_t{1}<<i))!=0,
                    (std::uint64_t(description[2])&(std::uint64_t{1}<<i))!=0});
                // Validate before any UI registration. Declaration strings are
                // owned for the DLL lifetime, including loader stop/restart.
                if(abi>=8){
                    const auto count=symbol<int(*)(int)>("NRFKotlin_NumberCount")(n);check(count);
                    if(count>4)throw std::runtime_error("Too many numeric settings");
                    auto meta=symbol<int(*)(int,int,int,char*,int)>("NRFKotlin_NumberMetadata");
                    auto info=symbol<int(*)(int,int,int*)>("NRFKotlin_NumberInfo");
                    type.numberLabels.resize(count);
                    for(int i=0;i<count;++i){
                        for(int field=0;field<3;++field){std::array<char,257> buffer{};const auto bytes=meta(n,i,field,buffer.data(),int(buffer.size()));check(bytes);
                            if(bytes>=int(buffer.size())||buffer[bytes])throw std::runtime_error("Invalid number metadata");
                            type.numberLabels[i][field].assign(buffer.data(),bytes);}
                        std::array<int,2> values{};check(info(n,i,values.data()));
                        const auto& labels=type.numberLabels[i];
                        type.numbers.push_back({labels[0],labels[1],labels[2],uint32_t(values[0]),uint32_t(values[1])});
                    }
                }
                SignalSettingsStore validator;validator.configure({type.id,type.title,type.catalogue,type.boxes,nullptr,{},type.numbers});
                if(abi>=3){
                    const auto count=symbol<int(*)(int)>("NRFKotlin_ActionCount")(n);check(count);
                    if(count>16)throw std::runtime_error("Too many signal actions");
                    auto metadata=symbol<int(*)(int,int,int,char*,int)>("NRFKotlin_ActionMetadata");
                    type.actionLabels.resize(count);type.actions.reserve(count);
                    for(int i=0;i<count;++i)for(int field=0;field<4;++field){
                        std::array<char,257> buffer{};const auto bytes=metadata(n,i,field,buffer.data(),int(buffer.size()));check(bytes);
                        if(bytes<1||bytes>=int(buffer.size())||buffer[bytes])throw std::runtime_error("Invalid action metadata");
                        type.actionLabels[i][field]=std::string(buffer.data(),bytes);
                    }
                    for(const auto& a:type.actionLabels)type.actions.push_back({a[0],a[1],a[2],a[3]});
                }
                detail::diagnostics::write("mods","INFO",("Registered signal type: "+type.id+" textures="+type.catalogue+" settings="+std::to_string(type.boxes.size())).c_str());
            }
            for(const auto& type:types)panels.push_back({type.id,type.title,type.catalogue,type.boxes,migrate?migrateSettings:nullptr,type.actions,type.numbers});
        } else {
            // Optional exports preserve compatibility with older one-type mods.
            panels.push_back({id,title,catalogue,boxes});
        }
    }
};
Api& api() { static Api value;return value; }
void migrateSettings(std::string_view panelId,std::map<std::string,bool,std::less<>>& values) {
    if(values.size()>64)throw std::invalid_argument("Too many saved settings");
    auto& a=api();
    const auto panel=std::find_if(a.panels.begin(),a.panels.end(),[&](const auto& p){return p.id==panelId;});
    if(panel==a.panels.end()||!a.migrate)throw std::invalid_argument("Unknown settings migration");
    std::array<char,64*129> names{};std::array<int,64> fields{};
    std::size_t index=0;
    for(const auto& [name,value]:values){
        if(name.empty()||name.size()>128||name.find('\0')!=std::string::npos)throw std::invalid_argument("Invalid saved setting name");
        std::copy(name.begin(),name.end(),names.begin()+index*129);fields[index++]=value?1:0;
    }
    std::int64_t mask{};check(a.migrate(int(panel-a.panels.begin()),names.data(),fields.data(),int(index),&mask));
    std::map<std::string,bool,std::less<>> migrated;
    for(std::size_t i=0;i<panel->checkboxes.size();++i)
        migrated.emplace(std::string(panel->checkboxes[i].name),(std::uint64_t(mask)&(std::uint64_t{1}<<i))!=0);
    values=std::move(migrated);
}
std::array<int,7> observation(const Observation& o) {return {int(o.block),o.fresh,o.routeKnown,o.forcedStop,o.lampFailed,o.redFlashCondition,o.next};}
}
Decision Rules::evaluate(const Settings& s,const Observation& o) {
    auto input=observation(o);std::array<int,2> output{};
    const auto status=api().decide(0,std::int64_t(s.mask),int(SettingsStatus::Present),0,0,input.data(),-1,0,output.data());
    check(status);if(status!=0)throw std::runtime_error("Missing Kotlin decision");return {output[0],output[1]};
}
std::optional<Decision> Rules::decide(const Signal& s,const std::optional<Decision>& next) {
    auto input=observation(s.observation);std::array<int,2> output{};
    auto& a=api();
    if(s.typeIndex>=a.panels.size())throw std::invalid_argument("Unknown signal type index");
    const auto status=a.decideType?
        a.decideType(int(s.typeIndex),s.live?2:1,std::int64_t(s.settings.mask),int(s.settingsStatus),std::int64_t(s.id),std::int64_t(s.nextSignal),
            std::int64_t(s.approachingTrain),input.data(),next?next->aspect:-1,next?next->reason:0,output.data()):
        a.decide(s.live?2:1,std::int64_t(s.settings.mask),int(s.settingsStatus),std::int64_t(s.id),std::int64_t(s.nextSignal),
            input.data(),next?next->aspect:-1,next?next->reason:0,output.data());
    check(status);if(status==1)return std::nullopt;if(status!=0)throw std::runtime_error("Invalid Kotlin decision status");
    return Decision{output[0],output[1]};
}
Signal Rules::fromLive(const LiveSignalState& state) {
    Signal result;result.id=state.id;result.nextSignal=state.nextSignal;result.live=true;
    result.approachingTrain=state.approachingTrain;
    result.observation={state.occupation,state.fresh,state.boundaryKnown};result.settingsStatus=state.settings.status;
    const auto& panels=api().panels;
    const auto type=std::find_if(panels.begin(),panels.end(),[&](const auto& panel){return panel.textureSet==state.textureSet;});
    if(type==panels.end())throw std::invalid_argument("Unregistered live signal catalogue");
    result.typeIndex=static_cast<std::uint32_t>(type-panels.begin());
    if(state.settings.status==SettingsStatus::Present) {
        const auto& boxes=type->checkboxes;
        for(std::size_t i=0;i<boxes.size();++i) {
            const auto value=state.settings.getBoolean(boxes[i].name);
            if(!value){result.settingsStatus=SettingsStatus::Unavailable;break;}
            if(*value)result.settings.mask|=std::uint64_t{1}<<i;
        }
    }
    return result;
}
std::vector<Signal> Rules::prepareNetwork(std::span<const Signal> input) {
    std::vector<Signal> result(input.begin(),input.end());
    const auto prepare=api().prepareNetwork;
    if(!prepare||input.empty())return result;
    if(input.size()>api().networkLimit)throw std::invalid_argument("Too many signals for this Kotlin mod");
    std::vector<int64_t> identities(input.size()*4),masks(input.size());
    std::vector<int> fields(input.size()*9),statuses(input.size());
    for(size_t i=0;i<input.size();++i){const auto& signal=input[i];
        identities[i*4]=signal.id;identities[i*4+1]=signal.nextSignal;identities[i*4+2]=signal.settings.mask;identities[i*4+3]=signal.approachingTrain;
        fields[i*9]=int(signal.typeIndex);fields[i*9+1]=int(signal.settingsStatus);
        const auto values=observation(signal.observation);std::copy(values.begin(),values.end(),fields.begin()+i*9+2);
    }
    check(prepare(int(input.size()),identities.data(),fields.data(),masks.data(),statuses.data()));
    for(size_t i=0;i<input.size();++i){
        if(statuses[i]!=int(input[i].settingsStatus)&&!(input[i].settingsStatus==SettingsStatus::Absent&&statuses[i]==int(SettingsStatus::Present)))
            throw std::runtime_error("Invalid prepared settings status");
        result[i].settings.mask=uint64_t(masks[i]);result[i].settingsStatus=SettingsStatus(statuses[i]);
    }
    return result;
}
bool Rules::multipleTypes(){return api().panels.size()>1;}
std::vector<LiveSignalState> Rules::observe(const Snapshot& snapshot) {
    std::vector<std::string_view> catalogues;
    for(const auto& panel:api().panels)catalogues.push_back(panel.textureSet);
    std::vector<SignalApproachScope> approaches;
    for(const auto& type:api().types)if(type.approachBlocks)approaches.push_back({type.catalogue,type.approachBlocks});
    return observeSignals(snapshot,catalogues,api().networkLimit,Milliseconds{1000},{},false,approaches);
}
Decision Rules::unknownDecision(){return {api().properties[2],api().properties[3]};}
Decision Rules::invalidNetworkDecision(){return {api().properties[4],api().properties[5]};}
Decision Rules::diagnosticDecision(const Decision& decision){
    auto& a=api();if(!a.localDecision)return decision;
    std::array<int,2> result{};check(a.localDecision(decision.aspect,decision.reason,result.data()));
    return {result[0],result[1]};
}
Decision Rules::invalidNetworkDecision(const Signal& signal){
    auto& a=api();
    if(!a.fallbackType)return invalidNetworkDecision();
    if(signal.typeIndex>=a.types.size())throw std::invalid_argument("Unknown Kotlin signal type");
    std::array<int,2> result{};check(a.fallbackType(int(signal.typeIndex),1,result.data()));
    return {result[0],result[1]};
}
std::string Rules::texture(const Decision& d,std::int64_t time,std::int64_t half) {
    std::array<char,96> buffer{};const auto count=api().texture(d.aspect,d.reason,time,half,buffer.data(),int(buffer.size()));check(count);
    if(count>=int(buffer.size())||buffer[count]!=0)throw std::runtime_error("Invalid Kotlin texture length");
    return {buffer.data(),static_cast<std::size_t>(count)};
}
std::optional<detail::SignalAnimation> Rules::animation(const Decision& d) {
    if(!api().animation)return std::nullopt; // Existing Kotlin ABI 1..5 mods.
    std::array<char,96> first{},alternate{};std::int64_t everyMs{};
    const auto status=api().animation(d.aspect,d.reason,first.data(),alternate.data(),int(first.size()),&everyMs);
    check(status);if(status==1)return std::nullopt;
    const auto firstEnd=std::find(first.begin(),first.end(),'\0'),alternateEnd=std::find(alternate.begin(),alternate.end(),'\0');
    if(status!=0||firstEnd==first.end()||alternateEnd==alternate.end())throw std::runtime_error("Invalid Kotlin animation response");
    return detail::checkedAnimation({std::string(first.begin(),firstEnd),std::string(alternate.begin(),alternateEnd),everyMs});
}
std::optional<SignalDrivingRule> Rules::drivingRule(Id id,const Decision& d) {
    std::array<double,2> numbers{};std::array<int,2> flags{};const auto status=api().driving(d.aspect,d.reason,numbers.data(),flags.data());
    check(status);if(status==1)return std::nullopt;if(status!=0)throw std::runtime_error("Invalid Kotlin driving status");
    return SignalDrivingRule{id,numbers[0],numbers[1],std::uint32_t(flags[0]),std::uint32_t(flags[1])};
}
Plan Rules::plan(const Vehicle& v,const DrivingSettings& s,const DrivingInput& i,std::span<const Constraint> constraints) {
    if(constraints.size()>4096)throw std::invalid_argument("Too many constraints");
    const std::array<double,15> values{v.maxSpeedMps,v.maxAccelerationMps2,v.serviceBrakingMps2,v.tractiveEffortN,v.powerW,v.emptyMassKg,v.extraMassKg,v.lengthM,
        s.brakeUse,s.responseSeconds,s.marginM,i.headM,i.speedMps,i.lineSpeedMps,i.visibleClearM.value_or(0)};
    const std::array<int,4> flags{i.fresh,i.routeKnown,i.onSight,i.visibleClearM.has_value()};
    std::vector<std::int64_t> sources;std::vector<double> restrictions;sources.reserve(constraints.size());restrictions.reserve(constraints.size()*4);
    for(const auto& r:constraints){sources.push_back(std::int64_t(r.source));restrictions.insert(restrictions.end(),{r.beginM,r.endM,r.speedMps,double(r.releaseByRear)});}
    std::array<double,3> out{};std::array<std::int64_t,3> result{};
    check(api().plan(values.data(),flags.data(),int(constraints.size()),sources.data(),restrictions.data(),out.data(),result.data()));
    return {result[0]!=0,out[0],out[1],out[2],result[1]!=0,Id(result[2])};
}
void Rules::diagnose(const Snapshot& snapshot,const std::vector<LiveSignalState>& states,std::span<const Decision> decisions) {
    static unsigned recorded=0;
    static auto nextSummary=std::chrono::steady_clock::time_point{};
    static auto nextFault=std::chrono::steady_clock::time_point{};
    unsigned faults=0;
    for(const auto& d:decisions)if(isFault(d))++faults;
    const bool fault=faults!=0;
    const auto now=std::chrono::steady_clock::now();
    if(now>=nextSummary) {
        nextSummary=now+std::chrono::seconds{30};
        detail::diagnostics::write("mods","INFO",("Mod signal heartbeat: id="+api().id+
            " signals="+std::to_string(states.size())+" faults="+std::to_string(faults)+
            " snapshotAgeMs="+std::to_string(snapshot.getAge().count())+
            " tracks="+std::to_string(snapshot.getAllTracks().size())+
            " boundaries="+std::to_string(snapshot.getAllSignals().size())+
            " occupationsAvailable="+(snapshot.getAllOccupations().has_value()?"yes":"no")).c_str());
    }
    if(!fault){recorded=0;return;}if(recorded>=128||now<nextFault)return;
    nextFault=now+std::chrono::seconds{1};
    try {
        const auto coverage=observeBlockCoverage(snapshot);
        std::ostringstream out;
        out<<"{\"fault\":"<<recorded++<<",\"age_ms\":"<<snapshot.getAge().count()
           <<",\"occupations_available\":"<<snapshot.getAllOccupations().has_value()<<",\"coverage_verified\":"<<coverage.verified
           <<",\"unknown_presence\":"<<coverage.unknownPresence<<",\"missing_footprints\":"<<coverage.missingFootprints
           <<",\"unexpected_footprints\":"<<coverage.unexpectedFootprints<<",\"signals\":[";
        for(std::size_t i=0;i<states.size();++i){if(i)out<<',';const auto& s=states[i];
            out<<"{\"id\":\""<<s.id<<"\",\"fresh\":"<<s.fresh<<",\"boundary_known\":"<<s.boundaryKnown
               <<",\"settings_status\":"<<int(s.settings.status)<<",\"occupation\":"<<int(s.occupation)
               <<",\"reason_code\":"<<decisions[i].reason<<'}';}
        out<<"]}";
        detail::diagnostics::write("mods","WARN",out.str().c_str());
    }catch(...){detail::diagnostics::exception("mods","signal fault diagnostic");}
}
std::span<const SignalCheckbox> Rules::checkboxes(){return api().boxes;}
std::span<const SignalCheckbox> Rules::checkboxes(std::string_view catalogue){
    for(const auto& panel:api().panels)if(panel.textureSet==catalogue)return panel.checkboxes;
    return {};
}
std::optional<Decision> Rules::forcedDecision(int aspect){
    if(!api().force)return {};std::array<int,2> out{};const auto status=api().force(aspect,out.data());
    check(status);if(status!=0)return {};return Decision{out[0],out[1]};
}
std::optional<Decision> Rules::forcedDecision(std::string_view catalogue,int aspect){
    if(!api().forceType)return forcedDecision(aspect);
    for(std::size_t i=0;i<api().panels.size();++i)if(api().panels[i].textureSet==catalogue){
        std::array<int,2> out{};const auto status=api().forceType(int(i),aspect,out.data());
        check(status);if(status==1)return {};if(status!=0)throw std::runtime_error("Invalid forced decision status");
        return Decision{out[0],out[1]};
    }
    return {};
}
std::string Rules::settingsId(){return std::string(api().panels.front().id);}
std::string Rules::diagnosticFile(){return api().diagnostic;}
std::string Rules::aspectName(int aspect){return api().text(8,aspect);}
std::string Rules::reasonName(int reason){return api().text(7,reason);}
bool Rules::isFault(const Decision& d){const int status=api().fault(d.aspect,d.reason);check(status);return (status&1)!=0;}
bool Rules::isActive(const Decision& d){const int status=api().fault(d.aspect,d.reason);check(status);return (status&2)!=0;}
}
nimby::Mod nimby::createMod() {
    auto& api=kotlin::api();
    nimby::Mod mod;
    if(api.tool){
        mod.observe=+[](const Snapshot& snapshot){
            if(const auto game=snapshot.getGameSession()){
                kotlin::ToolScope scope(*game);
                auto& api=kotlin::api();
                api.observeOptionsGame(*game);
                if(!api.windows.empty()){
                    if(!kotlin::toolWindows)kotlin::toolWindows=std::make_unique<platform::ToolWindows>(api.windows,api.translationsJson);
                    kotlin::toolWindows->observe(*game);
                    for(const auto& window:api.pendingWindows)kotlin::toolWindows->requestOpen(window);
                    api.pendingWindows.clear();
                    while(const auto event=kotlin::toolWindows->poll()){
                        if(event->game!=*game)continue;
                        std::string names;std::vector<int32_t> values;
                        for(const auto& [name,value]:event->values){names+=name;names+='\0';values.push_back(value);}
                        kotlin::check(api.windowEvent(int(event->index),int64_t(event->sequence),event->action.c_str(),names.c_str(),values.data(),int(values.size()),game->worldId.c_str(),int64_t(game->generation),kotlin::toolCall));
                    }
                }
                kotlin::check(kotlin::api().toolTick(game->worldId.c_str(),int64_t(game->generation),kotlin::toolCall));
            }else{
                if(kotlin::toolWindows)kotlin::toolWindows->invalidate();
                kotlin::toolReader().reset();auto& api=kotlin::api();
                if(api.optionsGame||!api.pendingWindows.empty())api.loseOptionsGame();
            }
        };
        mod.stop=+[]{
            kotlin::toolWindows.reset();
            kotlin::toolReader().reset();kotlin::check(kotlin::api().toolStop());};
        // A temporary observation failure must not forget an uncertain CREATE.
        // Kotlin keeps its ticket and resumes polling after observations recover.
        mod.observationLost=+[]{
            if(kotlin::toolWindows)kotlin::toolWindows->invalidate();
            kotlin::toolReader().reset();};
        // Tool ticks and service events only need session identity. Network
        // operations already obtain their own fresh capture through toolCall.
        mod.observationScope=SnapshotScope::Session;mod.observationIntervalMs=250;
    }else{
        kotlin::Rules::controlId=api.id;kotlin::Rules::textureSet=api.catalogue;kotlin::Rules::maximumLineSpeed=api.properties[1]!=0;
        mod=kotlin::Runtime::createMod();mod.signalSettings=api.panels.front();
        mod.additionalSignalSettings=std::span<const SignalSettingsPanel>(api.panels).subspan(1);
        for(const auto& panel:api.panels){
            NimbySignalCaptureScope scope{};
            if(panel.textureSet.size()>256)throw std::invalid_argument("Signal catalogue too long");
            std::memcpy(scope.texture_set,panel.textureSet.data(),panel.textureSet.size());
            for(const auto& type:api.types)if(type.catalogue==panel.textureSet)scope.approach_blocks=static_cast<uint32_t>(type.approachBlocks);
            mod.observationSignals.push_back(scope);
        }
    }
    mod.id=api.id;mod.services=api.services;mod.translationsJson=api.translationsJson;
    if(!api.optionsDeclaration.empty()){
        mod.refreshOptions=+[]{kotlin::api().refreshOptions();};
        api.originalStop=mod.stop;
        api.originalObservationLost=mod.observationLost;
        mod.observationLost=+[]{auto& api=kotlin::api();
            std::exception_ptr discardError;
            try{api.loseOptionsGame();}catch(...){discardError=std::current_exception();}
            if(api.originalObservationLost)api.originalObservationLost();
            if(discardError)std::rethrow_exception(discardError);};
        mod.stop=+[]{auto& api=kotlin::api();
            if(!api.optionsClient.close())detail::diagnostics::write("mods","WARN","Mod option cleanup deferred to the owning host");
            api.optionsGame.reset();api.pendingWindows.clear();if(api.originalStop)api.originalStop();};
    }
    if(!api.services.empty())mod.signalActionV2=+[](const NimbyUiActionEventV2& input,const Snapshot& snapshot){
        const auto& event=input.base;
        auto& a=kotlin::api();const auto it=std::find(a.services.begin(),a.services.end(),event.service);
        if(it==a.services.end())return;
        const auto game=snapshot.getGameSession();if(!game)return;
        kotlin::ToolScope scope(*game);
        if(a.serviceEventV2)kotlin::check(a.serviceEventV2(int(it-a.services.begin()),int64_t(event.sequence),int64_t(event.signal),int64_t(event.generation),
            int64_t(event.panel),event.action,event.world,event.origin,kotlin::toolCall,int(input.has_value),input.value));
        else if(!input.has_value)kotlin::check(a.serviceEvent(int(it-a.services.begin()),int64_t(event.sequence),int64_t(event.signal),int64_t(event.generation),
            int64_t(event.panel),event.action,event.world,event.origin,kotlin::toolCall));
    };
    return mod;
}
