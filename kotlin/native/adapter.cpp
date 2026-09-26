#include <nimby/detail/diagnostics.hpp>
#include <atomic>
#include <nimby/kotlin_mod.hpp>
#include <nimby/detail/native_library.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>

#ifndef NIMBY_KOTLIN_LIBRARY
#error The SDK build tool must specify the Kotlin library name.
#endif
namespace nimby::kotlin {
namespace {
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
    int (*texture)(int,int,std::int64_t,std::int64_t,char*,int)=nullptr;
    int (*force)(int,int*)=nullptr;
    int (*fault)(int,int)=nullptr;
    int (*driving)(int,int,double*,int*)=nullptr;
    int (*plan)(const double*,const int*,int,const std::int64_t*,const double*,double*,std::int64_t*)=nullptr;
    std::array<int,6> properties{};
    std::string id,title,catalogue,diagnostic;
    std::vector<std::array<std::string,3>> labels;
    std::vector<SignalCheckbox> boxes;
    template<class T> T symbol(const char* name) {
        auto address=detail::native::symbol(module,name);if(!address)throw std::runtime_error(std::string("Missing Kotlin SDK export: ")+name);
        return reinterpret_cast<T>(address);
    }
    std::string text(int field,int index=0) {
        std::array<char,2048> buffer{};const auto count=metadata(field,index,buffer.data(),int(buffer.size()));check(count);
        if(count>=int(buffer.size())||buffer[count]!=0)throw std::runtime_error("Invalid Kotlin text length");
        return {buffer.data(),static_cast<std::size_t>(count)};
    }
    Api() {
        const auto ownPath=detail::native::modulePath(reinterpret_cast<const void*>(&check));
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
        const auto version=symbol<int(*)()>("NRFKotlin_Version");if(version()!=1)throw std::runtime_error("Unsupported Kotlin ABI");
#define LOAD(member, name) member=symbol<decltype(member)>(name)
        LOAD(metadata,"NRFKotlin_Metadata");LOAD(info,"NRFKotlin_Info");LOAD(defaults,"NRFKotlin_Defaults");
        LOAD(decide,"NRFKotlin_Decide");LOAD(texture,"NRFKotlin_Texture");LOAD(fault,"NRFKotlin_Fault");
        LOAD(driving,"NRFKotlin_Driving");LOAD(plan,"NRFKotlin_Plan");
        force=reinterpret_cast<decltype(force)>(detail::native::symbol(module,"NRFKotlin_Force"));
#undef LOAD
        check(info(properties.data()));if(properties[0]<0||properties[0]>64)throw std::runtime_error("Invalid checkbox count");
        id=text(0);title=text(1);catalogue=text(2);diagnostic=text(3);
        detail::diagnostics::write("mods","INFO",("Kotlin mod loaded: "+id+" / SDK 0.8.0-alpha.1 / adapter ABI 1").c_str());
        std::int64_t values=0;check(defaults(&values));
        labels.reserve(properties[0]);boxes.reserve(properties[0]);
        for(int i=0;i<properties[0];++i)labels.push_back({text(4,i),text(5,i),text(6,i)});
        for(int i=0;i<properties[0];++i)boxes.push_back({labels[i][0],labels[i][1],labels[i][2],(std::uint64_t(values)&(std::uint64_t{1}<<i))!=0});
        detail::diagnostics::write("mods","INFO",("Mod ready: id="+id+" title="+title+" textures="+catalogue+" checkboxes="+std::to_string(boxes.size())+" defaultMask="+std::to_string(values)+" force="+(force?"yes":"no")+" detailedErrors="+(lastError.load()?"yes":"no")).c_str());
        for(const auto& label:labels)detail::diagnostics::write("mods","INFO",("Registered setting: "+label[0]+" / "+label[1]).c_str());
    }
};
Api& api() { static Api value;return value; }
std::array<int,7> observation(const Observation& o) {return {int(o.block),o.fresh,o.routeKnown,o.forcedStop,o.lampFailed,o.redFlashCondition,o.next};}
}
Decision Rules::evaluate(const Settings& s,const Observation& o) {
    auto input=observation(o);std::array<int,2> output{};
    const auto status=api().decide(0,std::int64_t(s.mask),int(SettingsStatus::Present),0,0,input.data(),-1,0,output.data());
    check(status);if(status!=0)throw std::runtime_error("Missing Kotlin decision");return {output[0],output[1]};
}
std::optional<Decision> Rules::decide(const Signal& s,const std::optional<Decision>& next) {
    auto input=observation(s.observation);std::array<int,2> output{};
    const auto status=api().decide(s.live?2:1,std::int64_t(s.settings.mask),int(s.settingsStatus),std::int64_t(s.id),std::int64_t(s.nextSignal),
        input.data(),next?next->aspect:-1,next?next->reason:0,output.data());
    check(status);if(status==1)return std::nullopt;if(status!=0)throw std::runtime_error("Invalid Kotlin decision status");
    return Decision{output[0],output[1]};
}
Signal Rules::fromLive(const LiveSignalState& state) {
    Signal result;result.id=state.id;result.nextSignal=state.nextSignal;result.live=true;
    result.observation={state.occupation,state.fresh,state.boundaryKnown};result.settingsStatus=state.settings.status;
    if(state.settings.status==SettingsStatus::Present) {
        const auto& boxes=api().boxes;
        for(std::size_t i=0;i<boxes.size();++i) {
            const auto value=state.settings.getBoolean(boxes[i].name);
            if(!value){result.settingsStatus=SettingsStatus::Unavailable;break;}
            if(*value)result.settings.mask|=std::uint64_t{1}<<i;
        }
    }
    return result;
}
Decision Rules::unknownDecision(){return {api().properties[2],api().properties[3]};}
Decision Rules::invalidNetworkDecision(){return {api().properties[4],api().properties[5]};}
std::string Rules::texture(const Decision& d,std::int64_t time,std::int64_t half) {
    std::array<char,96> buffer{};const auto count=api().texture(d.aspect,d.reason,time,half,buffer.data(),int(buffer.size()));check(count);
    if(count>=int(buffer.size())||buffer[count]!=0)throw std::runtime_error("Invalid Kotlin texture length");
    return {buffer.data(),static_cast<std::size_t>(count)};
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
    unsigned faults=0;
    for(const auto& d:decisions)if(isFault(d))++faults;
    const bool fault=faults!=0;
    const auto now=std::chrono::steady_clock::now();
    if(now>=nextSummary) {
        nextSummary=now+std::chrono::seconds{30};
        detail::diagnostics::write("mods","INFO",("Mod signal heartbeat: id="+api().id+
            " signals="+std::to_string(states.size())+" faults="+std::to_string(faults)+
            " snapshotAgeMs="+std::to_string(snapshot.getAge().count())+
            " occupationsAvailable="+(snapshot.getAllOccupations().has_value()?"yes":"no")).c_str());
    }
    if(!fault){recorded=0;return;}if(recorded>=128)return;
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
std::optional<Decision> Rules::forcedDecision(int aspect){
    if(!api().force)return {};std::array<int,2> out{};const auto status=api().force(aspect,out.data());
    check(status);if(status!=0)return {};return Decision{out[0],out[1]};
}
std::string Rules::settingsId(){return api().id;}
std::string Rules::diagnosticFile(){return api().diagnostic;}
std::string Rules::aspectName(int aspect){return api().text(8,aspect);}
std::string Rules::reasonName(int reason){return api().text(7,reason);}
bool Rules::isFault(const Decision& d){const int status=api().fault(d.aspect,d.reason);check(status);return (status&1)!=0;}
bool Rules::isActive(const Decision& d){const int status=api().fault(d.aspect,d.reason);check(status);return (status&2)!=0;}
}
nimby::Mod nimby::createMod() {
    auto& api=kotlin::api();kotlin::Rules::controlId=api.id;kotlin::Rules::textureSet=api.catalogue;kotlin::Rules::maximumLineSpeed=api.properties[1]!=0;
    auto mod=kotlin::Runtime::createMod();mod.signalSettings={api.id,api.title,api.catalogue,api.boxes};return mod;
}
