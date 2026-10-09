#include <platform/windows/train_length_native.h>
#include <platform/windows/bridge_installation.h>
#include <platform/windows/game_language.h>
#include <platform/windows/native_leaf_gateway.h>
#include <platform/windows/train_length_diagnostics.h>
#include <platform/windows/train_length_group.h>
#include <platform/windows/train_length_validation.h>
#include <engine/binary_identity.h>
#include <engine/train_editor_messages.h>
#include <engine/signal_ui.h>
#include <nimby/detail/diagnostics.hpp>
#include <MinHook.h>
#include <windows.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdarg>
#include <string>

namespace nimby::platform::windows::train_length {
namespace {
uint64_t base{};
using EditorMessages=nimby::engine::train_editor::Messages;
using EditorRegistry=policy::Registry<EditorMessages>;
EditorRegistry registry;
std::atomic<uint64_t> nextOwner{1};
std::atomic<uint64_t> deferredRemovalAttempts{};
uint64_t allocateOwner()noexcept {
    auto value=nextOwner.load(std::memory_order_relaxed);
    while(value&&value!=UINT64_MAX){
        if(nextOwner.compare_exchange_weak(value,value+1,std::memory_order_relaxed))return value;
    }
    return 0;
}
SRWLOCK installationLock=SRWLOCK_INIT;
bool installed=false;
bool installationBroken=false;
bool diagnosticsReady=false;
std::atomic<uint64_t> nextRequest{1};
using Level=diagnostics::Level;
unsigned long long hex(uint64_t value)noexcept{return static_cast<unsigned long long>(value);}
const char* decision(policy::Decision value)noexcept {
    switch(value){
        case policy::Decision::Allowed:return "allowed";
        case policy::Decision::LimitExceeded:return "length-limit-exceeded";
        case policy::Decision::UnknownModel:return "unknown-model";
        case policy::Decision::InvalidLength:return "invalid-model-length";
        case policy::Decision::TooManyElements:return "input-safety-bound";
        case policy::Decision::InvalidLimit:return "invalid-policy-limit";
        case policy::Decision::ResolverFailure:return "unreadable-or-unstable-data";
    }return "unknown";
}
const char* fault(ReadFault value)noexcept {
    switch(value){
        case ReadFault::None:return "none";case ReadFault::Address:return "invalid-address";
        case ReadFault::Read:return "memory-read-failed";case ReadFault::VectorLayout:return "invalid-vector-layout";
        case ReadFault::ElementBudget:return "input-safety-bound";case ReadFault::ModelCatalogue:return "invalid-model-catalogue";
        case ReadFault::ModelMissing:return "model-not-found";case ReadFault::ModelChain:return "invalid-model-chain";
        case ReadFault::ModelLength:return "nonpositive-or-nonfinite-model-length";case ReadFault::Selection:return "invalid-selection";
    }return "unknown";
}
const char* editName(Edit value)noexcept {
    switch(value){case Edit::Append:return "append";case Edit::Replace:return "replace";
        case Edit::Paste:return "paste";case Edit::Reset:return "reset";default:return "none";}
}
const char* groupIssue(groups::Issue value)noexcept {
    switch(value){case groups::Issue::None:return "none";case groups::Issue::Unavailable:return "native-data-unavailable";
        case groups::Issue::InvalidTopology:return "invalid-attachment-topology";
        case groups::Issue::NativeAggregationLimit:return "native-aggregation-count-limit";
        case groups::Issue::CurrentCompositionLag:return "native-current-composition-not-refreshed";
    }return "unknown";
}
void log(Level level,const char* format,...)noexcept {
    std::array<char,2048> text{};va_list arguments;va_start(arguments,format);
    std::vsnprintf(text.data(),text.size(),format,arguments);va_end(arguments);
    diagnostics::writer().publish(level,text.data());
}
// Owned request diagnostics. No native pointer is retained by the asynchronous
// writer, and model details reuse the measurements required by the policy.
// The bounded inventory never performs additional ReadProcessMemory calls.
struct Trace {
    struct Model {uint64_t id{};size_t count{};std::optional<double> meters;bool resolved=false;};
    uint64_t sequence=nextRequest.fetch_add(1,std::memory_order_relaxed);
    std::chrono::steady_clock::time_point start=std::chrono::steady_clock::now();
    std::array<char,diagnostics::Writer::maximumMessageBytes+1> text{};
    size_t size{};bool truncated=false;
    std::array<Model,128> models{};size_t modelCount{},omitted{};
    uint64_t fingerprint=14695981039346656037ULL;
    bool orderedModelsCaptured=false;
    void append(const char* format,...)noexcept {
        if(truncated)return;
        va_list arguments;va_start(arguments,format);
        const auto count=std::vsnprintf(text.data()+size,text.size()-size,format,arguments);va_end(arguments);
        if(count<0||static_cast<size_t>(count)>=text.size()-size){truncated=true;return;}
        size+=static_cast<size_t>(count);
    }
    void inventory(std::span<const policy::ModelBlock> entries)noexcept {
        orderedModelsCaptured=true;
        for(const auto& entry:entries){
            fingerprint^=entry.model;fingerprint*=1099511628211ULL;
            fingerprint^=entry.count;fingerprint*=1099511628211ULL;
            size_t i=0;while(i<modelCount&&models[i].id!=entry.model)++i;
            if(i==modelCount){if(modelCount==models.size()){omitted+=entry.count;continue;}models[modelCount++].id=entry.model;}
            models[i].count+=entry.count;
        }
    }
    void observed(uint64_t model,const std::optional<double>& meters)noexcept {
        for(size_t i=0;i<modelCount;++i)if(models[i].id==model){models[i].meters=meters;models[i].resolved=true;break;}
    }
    void group(const groups::Check& check)noexcept {
        modelCount=check.modelCount;omitted=check.omittedElements;
        for(size_t i=0;i<modelCount;++i){const auto& value=check.models[i];models[i]={value.model,value.count,value.length,value.observed};}
        append(" groupIssue=%s laggingMembers=%zu plannedMeters=%.6f",groupIssue(check.issue),check.laggingMembers,check.planned.totalLengthMeters);
    }
    void assessment(const policy::Assessment& value,ReadIssue issue)noexcept {
        append(" decision=%s candidateMeters=%.6f maximumMeters=%.6f measuredCars=%zu problematicModel=%llx readFault=%s address=%llx detail=%llu elapsedUs=%lld",
            decision(value.decision),value.totalLengthMeters,value.maximumMeters,value.evaluatedElements,hex(value.problematicModel),
            fault(issue.fault),hex(issue.address),hex(issue.detail),
            static_cast<long long>(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count()));
    }
    void emit(Level level,const char* phase)noexcept {
        append(" phase=%s",phase);
        if(orderedModelsCaptured)append(" orderedModelFingerprint=%llx",hex(fingerprint));
        uint64_t inventoryFingerprint=14695981039346656037ULL;
        for(size_t i=0;i<modelCount;++i){inventoryFingerprint^=models[i].id;inventoryFingerprint*=1099511628211ULL;
            inventoryFingerprint^=models[i].count;inventoryFingerprint*=1099511628211ULL;}
        append(" boundedModelInventoryFingerprint=%llx models=[",hex(inventoryFingerprint));
        for(size_t i=0;i<modelCount;++i){const auto& model=models[i];
            append("%s%llx:count=%zu,length=",i?";":"",hex(model.id),model.count);
            if(model.resolved&&model.meters)append("%.6f",*model.meters);
            else append(model.resolved?"unavailable":"not-measured");
        }
        append("] omittedCars=%zu inventoryComplete=%u truncated=%u",omitted,unsigned(!omitted),unsigned(truncated));
        if(truncated){constexpr char suffix[]=" [diagnostic detail truncated]";
            size=text.size()-1-sizeof(suffix);std::memcpy(text.data()+size,suffix,sizeof suffix);size+=sizeof(suffix)-1;
        }
        diagnostics::writer().publish(level,std::string_view(text.data(),size));
    }
};
bool read(void*,uint64_t at,void* target,size_t count) {
    SIZE_T copied{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(at),target,count,&copied)&&copied==count;
}
using Dynamics=void(*)(uint64_t,uint64_t);
using Hash=uint64_t(*)(const void*,uint64_t);
using Leaf=uint64_t(*)(const void*,uint64_t,uint64_t,uint64_t,uint64_t);
using SettingsHash=uint64_t(*)(uint8_t,uint32_t,uint64_t,uint8_t,uint8_t,uint8_t,uint8_t);
using Validate=uint64_t(*)(uint64_t,uint64_t,uint64_t,uint8_t);
using Commit=void(*)(uint64_t,uint64_t,uint64_t);
using Panel=void(*)(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint8_t);
using Command=uint64_t(*)(uint64_t,uint64_t,uint64_t,uint64_t);
using SimCommand=void(*)(uint64_t,uint64_t);
Dynamics originalDynamics{};
Hash originalHash{};
Validate originalValidate{};
Commit originalCommit{};
Panel originalPanel{};
Command originalPurchase{},originalRecompose{};
SimCommand originalHitch{},originalSetConfig{};

struct Failure {
    uint64_t editor{};
    policy::Assessment assessment{};
    std::chrono::steady_clock::time_point until{};
    std::shared_ptr<const EditorRegistry::Selection> selection;
    std::string language;
    const std::string* message=nullptr;
    std::array<char,96> measurements{};
};
// Both composition commit and its panel belong to the native UI thread. A
// borrowed editor address is only compared, never dereferenced after the call.
thread_local Failure lastFailure;
thread_local uint64_t currentEditor{};
thread_local uint64_t currentRequest{};
struct ScopedRequest {
    uint64_t previous=currentRequest;
    explicit ScopedRequest(uint64_t value){currentRequest=value;}
    ~ScopedRequest(){currentRequest=previous;}
};
using Approval=validation::RecompositionApproval;
thread_local const Approval* approvedRecomposition{};
struct ScopedApproval {
    const Approval* previous=approvedRecomposition;
    explicit ScopedApproval(const Approval* value){approvedRecomposition=value;}
    ~ScopedApproval(){approvedRecomposition=previous;}
};
void report(uint64_t editor,const policy::Assessment& value)noexcept {
    const auto& previous=lastFailure;
    if(previous.editor==editor&&previous.selection&&previous.selection->revision==registry.selectionRevision()&&
       previous.assessment.decision==value.decision&&previous.assessment.totalLengthMeters==value.totalLengthMeters&&
       previous.assessment.maximumMeters==value.maximumMeters&&previous.assessment.problematicModel==value.problematicModel){
        lastFailure.until=std::chrono::steady_clock::now()+std::chrono::seconds(15);return;
    }
    const auto selection=registry.selected();
    // A preference may change while a candidate is being measured. Never
    // attach the wording of a different limit to this assessment. The copied
    // selection owns its texts independently of the isolated mod's lifetime.
    if(!selection||selection->limit.maximumMeters!=value.maximumMeters||!selection->presentation){lastFailure={};return;}
    lastFailure={editor,value,std::chrono::steady_clock::now()+std::chrono::seconds(15),selection,{},nullptr,{}};
    if(value.decision==policy::Decision::LimitExceeded)
        std::snprintf(lastFailure.measurements.data(),lastFailure.measurements.size(),"%.3f m / %.0f m",
            value.totalLengthMeters,value.maximumMeters);
    log(Level::Warning,"Train editor refusal presentation: owner=%llu revision=%llu editor=%llx decision=%s candidateMeters=%.6f maximumMeters=%.6f",
        hex(selection->owner),hex(selection->revision),hex(editor),decision(value.decision),value.totalLengthMeters,value.maximumMeters);
}
policy::Assessment unavailable(policy::Limit limit)noexcept {
    policy::Assessment value;value.decision=policy::Decision::ResolverFailure;value.maximumMeters=limit.maximumMeters;return value;
}
policy::Assessment composition(const View& view,uint64_t vector,uint64_t rules,policy::Limit limit) {
    std::vector<Car> cars;
    if(!view.vector(vector,cars))return unavailable(limit);
    std::vector<policy::ModelBlock> models;models.reserve(cars.size());
    for(const auto& car:cars)models.push_back({car.model,1});
    return assess(view,rules,limit,models);
}
void commit(uint64_t editor,uint64_t context,uint64_t rules) {
    const auto limit=registry.limit();
    bool nativeEntered=false;
    if(limit.active())try{
        const View view(read);const auto proposal=proposed(view,editor);
        if(proposal.readable&&proposal.edit==Edit::None){
            if(proposal.requested){
                const auto sequence=nextRequest.fetch_add(1,std::memory_order_relaxed);ScopedRequest request(sequence);
                log(Level::Info,"Train length request=%llu action=other-native-editor-operation editor=%llx selectedCars=%zu actionFlags=%x phase=delegate-unchanged",
                    hex(sequence),hex(editor),proposal.selectedElements,unsigned(proposal.actionFlags));
                nativeEntered=true;originalCommit(editor,context,rules);
                log(Level::Info,"Train length request=%llu action=other-native-editor-operation phase=native-commit-returned editor=%llx",hex(sequence),hex(editor));return;
            }
            nativeEntered=true;originalCommit(editor,context,rules);return;
        }
        Trace trace;trace.append("Train length request=%llu action=%s editor=%llx context=%llx rules=%llx beforeCars=%zu requestedCars=%zu selectedCars=%zu actionFlags=%x",
            hex(trace.sequence),editName(proposal.edit),hex(editor),hex(context),hex(rules),proposal.beforeElements,
            proposal.requestedElements,proposal.selectedElements,unsigned(proposal.actionFlags));
        trace.inventory(proposal.models);
        auto value=proposal.readable?assess(view,rules,limit,proposal.models,
            [&](uint64_t model,const std::optional<double>& meters){trace.observed(model,meters);}):unavailable(limit);
        if(value.decision==policy::Decision::LimitExceeded&&!proposal.beforeModels.empty()){
            const auto before=assess(view,rules,limit,proposal.beforeModels);
            groups::detail::preserveReduction(value,before);
            trace.append(" previousMeters=%.6f previousDecision=%s nonIncreasingExistingTrain=%u",before.totalLengthMeters,
                decision(before.decision),unsigned(value.allowed()));
        }
        trace.assessment(value,view.issue());trace.emit(value.allowed()?Level::Info:Level::Warning,"preflight");
        if(!value.allowed()){report(editor,value);return;}
        if(lastFailure.editor==editor)lastFailure={};
        ScopedRequest request(trace.sequence);
        nativeEntered=true;
        originalCommit(editor,context,rules);
        std::array<uint64_t,3> range{};const auto readable=view.get(editor+0xc0,range);
        log(Level::Info,"Train length request=%llu action=%s phase=native-commit-returned editor=%llx vectorReadable=%u begin=%llx end=%llx capacity=%llx",
            hex(trace.sequence),editName(proposal.edit),hex(editor),unsigned(readable),hex(range[0]),hex(range[1]),hex(range[2]));
        return;
    }catch(...){
        if(nativeEntered){log(Level::Error,"Train length action=editor phase=native-exception editor=%llx context=%llx rules=%llx mutationState=not-rolled-back",hex(editor),hex(context),hex(rules));throw;}
        report(editor,unavailable(limit));log(Level::Error,"Train length action=editor phase=preflight-exception editor=%llx context=%llx rules=%llx maximumMeters=%.6f",hex(editor),hex(context),hex(rules),limit.maximumMeters);return;
    }
    originalCommit(editor,context,rules);
}
void panel(uint64_t editor,uint64_t context,uint64_t emitter,uint64_t rules,uint64_t arg5,uint64_t arg6,
           uint64_t arg7,uint64_t arg8,uint64_t arg9,uint8_t arg10) {
    const auto previousEditor=currentEditor;currentEditor=editor;
    struct RestoreEditor{uint64_t previous;~RestoreEditor(){currentEditor=previous;}} restore{previousEditor};
    originalPanel(editor,context,emitter,rules,arg5,arg6,arg7,arg8,arg9,arg10);
    if(lastFailure.editor!=editor||!lastFailure.selection||
       registry.selectionRevision()!=lastFailure.selection->revision||std::chrono::steady_clock::now()>=lastFailure.until)return;
    try{
        auto ui=nimby::engine::SignalUi::bind(read,nullptr,base,emitter);
        if(!ui)return;
        const auto language=nimby::platform::windows::gameLanguage(read,nullptr,base).value_or("");
        if(!lastFailure.message||lastFailure.language!=language){
            const auto decision=lastFailure.assessment.decision;
            const size_t index=decision==policy::Decision::LimitExceeded?0:
                decision==policy::Decision::UnknownModel||decision==policy::Decision::InvalidLength?1:2;
            lastFailure.message=&lastFailure.selection->presentation->select(index,language);
            lastFailure.language=language;
        }
        // No JSON, mod callback, IPC, disk read or measurement in this panel.
        // The translated notice and numerical facts are already owned/cached.
        ui->message(lastFailure.message->c_str());
        if(lastFailure.measurements[0])ui->message(lastFailure.measurements.data());
    }catch(...){log(Level::Error,"Train length action=notification phase=ui-message-exception editor=%llx emitter=%llx",hex(editor),hex(emitter));}
}
uint64_t validate(uint64_t train,uint64_t output,uint64_t rules,uint8_t ignoreCouplers) {
    const auto result=originalValidate(train,output,rules,ignoreCouplers);
    const auto limit=registry.limit();if(!limit.active())return result;
    try{
        const View view(read);auto value=composition(view,train+0xc0,rules,limit);
        if(value.decision==policy::Decision::LimitExceeded&&train==currentEditor){
            const auto baseline=composition(view,train+0x238,rules,limit);
            groups::detail::preserveReduction(value,baseline);
        }
        if(value.decision==policy::Decision::LimitExceeded&&approvedRecomposition&&
           approvedRecomposition->rules==rules&&approvedRecomposition->maximumMeters==limit.maximumMeters){
            // The model command validates a private Train copy internally.
            // Allow only the exact ordered model candidate already checked at
            // the outer boundary; native coupler checks remain authoritative.
            std::vector<Car> cars;
            if(view.vector(train+0xc0,cars)&&validation::matchesApproval(*approvedRecomposition,rules,limit,cars))
                value.decision=policy::Decision::Allowed;
        }
        if(!value.allowed()){
            // Keep the game's owned coupler flags and allocation intact. Code1
            // prevents submitting the train; the borrowed editor shows why.
            if(train==currentEditor){
                const auto& previous=lastFailure;
                if(previous.editor!=train||previous.assessment.decision!=value.decision||previous.assessment.totalLengthMeters!=value.totalLengthMeters||
                   previous.assessment.maximumMeters!=value.maximumMeters||previous.assessment.problematicModel!=value.problematicModel){
                    const auto issue=view.issue();
                    log(Level::Warning,"Train length validation refused: request=%llu editor=%llx rules=%llx decision=%s candidateMeters=%.6f maximumMeters=%.6f measuredCars=%zu problematicModel=%llx readFault=%s address=%llx detail=%llu",
                        hex(currentRequest),hex(train),hex(rules),decision(value.decision),value.totalLengthMeters,value.maximumMeters,value.evaluatedElements,
                        hex(value.problematicModel),fault(issue.fault),hex(issue.address),hex(issue.detail));
                }
                report(train,value);
            }
            const uint32_t code=1;std::memcpy(reinterpret_cast<void*>(output),&code,sizeof code);return result;
        }
        const auto recovery=validation::recoverCountLimit(view,output,value.evaluatedElements);
        if(recovery.recovered){
            std::memcpy(reinterpret_cast<void*>(output),&recovery.code,sizeof recovery.code);
            if(currentRequest)log(recovery.code==0?Level::Info:Level::Warning,
                "Train length native validation restored: request=%llu train=%llx cars=%zu flagBytes=%llu restoredCode=%u couplersAccepted=%u",
                hex(currentRequest),hex(train),value.evaluatedElements,hex(recovery.flagBytes),recovery.code,unsigned(recovery.code==0));
        }else if(currentRequest&&value.evaluatedElements>30&&recovery.code==1){
            log(Level::Warning,"Train length validation count guard retained: request=%llu train=%llx cars=%zu flagBytes=%llu nativeCode=%u readFault=%s address=%llx detail=%llu",
                hex(currentRequest),hex(train),value.evaluatedElements,hex(recovery.flagBytes),recovery.code,fault(recovery.readIssue.fault),hex(recovery.readIssue.address),hex(recovery.readIssue.detail));
        }
    }catch(...){const uint32_t code=1;std::memcpy(reinterpret_cast<void*>(output),&code,sizeof code);}
    return result;
}
uint64_t rejectedCommand(uint64_t output,bool purchase)noexcept {
    // Exactly the game's scalar failure variant; never clear the dispatcher's
    // remaining envelope, correlation fields, or foreign allocations.
    std::array<uint8_t,32> value{};uint32_t code=2;
    if(purchase){std::memcpy(value.data(),&code,4);value[0x10]=1;}
    else{std::memcpy(value.data()+0x18,&code,4);value[0x1c]=1;}
    std::memcpy(reinterpret_cast<void*>(output),value.data(),value.size());
    *reinterpret_cast<uint8_t*>(output+0x1f0)=4;
    return output;
}
uint64_t command(uint64_t input,uint64_t output,uint64_t context,uint64_t simulation,bool purchase) {
    const auto limit=registry.limit();
    bool nativeEntered=false;
    if(limit.active())try{
        const View view(read);uint64_t database{},rules{},train{};Trace trace;
        trace.append("Train length request=%llu action=%s command=%llx context=%llx simulation=%llx",
            hex(trace.sequence),purchase?"purchase":"recompose",hex(input),hex(context),hex(simulation));
        if(!view.get(context+0x890,database)||!database||!view.get(database+0x408,rules)||!rules){
            const auto value=unavailable(limit);trace.assessment(value,view.issue());trace.emit(Level::Warning,"context-unavailable");
            report(0,value);return rejectedCommand(output,purchase);
        }
        trace.append(" database=%llx rules=%llx",hex(database),hex(rules));
        std::vector<Car> cars;
        if(!view.vector(input+(purchase?0xe0:0x28),cars)||(!purchase&&!view.get(input+0x20,train))){
            const auto value=unavailable(limit);trace.assessment(value,view.issue());trace.emit(Level::Warning,"candidate-unavailable");
            report(0,value);return rejectedCommand(output,purchase);
        }
        std::vector<policy::ModelBlock> models;models.reserve(cars.size());for(const auto& car:cars)models.push_back({car.model,1});
        trace.inventory(models);trace.append(" train=%llx proposedCars=%zu",hex(train),cars.size());
        policy::Assessment value;ReadIssue issue;
        if(purchase){value=assess(view,rules,limit,models,
            [&](uint64_t model,const std::optional<double>& meters){trace.observed(model,meters);});issue=view.issue();}
        else{
            const auto check=groups::recompose(read,nullptr,{database,simulation},limit,train,cars);
            value=check.assessment;issue=check.readIssue;
            trace.group(check);
            trace.append(" groupRoot=%llx members=%zu beforeActiveCars=%zu candidateActiveCars=%zu previousMeters=%.6f ownBeforeMeters=%.6f ownCandidateMeters=%.6f groupChecked=%u",
                hex(check.root),check.members,check.previousElements,check.candidateElements,check.before.totalLengthMeters,
                check.ownBefore.totalLengthMeters,check.ownAfter.totalLengthMeters,unsigned(check.checked));
        }
        trace.assessment(value,issue);trace.emit(value.allowed()?Level::Info:Level::Warning,"preflight");
        if(!value.allowed()){report(0,value);return rejectedCommand(output,purchase);}
        ScopedRequest request(trace.sequence);
        const Approval approval{rules,limit.maximumMeters,cars,value.allowed()};ScopedApproval scope(purchase?nullptr:&approval);
        nativeEntered=true;
        const auto result=(purchase?originalPurchase:originalRecompose)(input,output,context,simulation);
        uint8_t tag{};uint32_t nativeCode{};
        const auto codeReadable=view.get(output+(purchase?0:0x18),nativeCode);
        const auto tagReadable=view.get(output+0x1f0,tag);
        log(Level::Info,"Train length request=%llu action=%s phase=native-command-returned result=%llx codeReadable=%u nativeCode=%u tagReadable=%u variantTag=%u",
            hex(trace.sequence),purchase?"purchase":"recompose",hex(result),unsigned(codeReadable),nativeCode,unsigned(tagReadable),unsigned(tag));
        return result;
    }catch(...){
        if(nativeEntered){log(Level::Error,"Train length action=%s phase=native-exception command=%llx context=%llx simulation=%llx mutationState=not-rolled-back",
            purchase?"purchase":"recompose",hex(input),hex(context),hex(simulation));throw;}
        report(0,unavailable(limit));log(Level::Error,"Train length action=%s phase=preflight-exception command=%llx context=%llx simulation=%llx maximumMeters=%.6f",
            purchase?"purchase":"recompose",hex(input),hex(context),hex(simulation),limit.maximumMeters);return rejectedCommand(output,purchase);
    }
    return (purchase?originalPurchase:originalRecompose)(input,output,context,simulation);
}
uint64_t purchase(uint64_t input,uint64_t output,uint64_t context,uint64_t simulation){return command(input,output,context,simulation,true);}
uint64_t recompose(uint64_t input,uint64_t output,uint64_t context,uint64_t simulation){return command(input,output,context,simulation,false);}

void simCommand(uint64_t input,uint64_t context,bool hitch) {
    const auto limit=registry.limit();
    bool nativeEntered=false;
    if(limit.active())try{
        const View view(read);groups::Context captured;uint64_t train{},driver{};int32_t configuration{};Trace trace;
        trace.append("Train length request=%llu action=%s command=%llx context=%llx",hex(trace.sequence),hitch?"hitch":"configuration",hex(input),hex(context));
        if(!view.get(context,captured.database)||!view.get(context+8,captured.simulation)||!view.get(input+(hitch?0x10:8),train)||
           (hitch?!view.get(input+8,driver):!view.get(input+0x10,configuration))){
            const auto value=unavailable(limit);trace.assessment(value,view.issue());trace.emit(Level::Warning,"command-unavailable");report(0,value);return;
        }
        const auto check=hitch?groups::hitch(read,nullptr,captured,limit,driver,train):
            groups::setConfig(read,nullptr,captured,limit,train,configuration);
        trace.group(check);
        trace.append(" database=%llx simulation=%llx train=%llx requestedDriver=%llx configuration=%d groupRoot=%llx members=%zu beforeActiveCars=%zu candidateActiveCars=%zu previousMeters=%.6f groupChecked=%u",
            hex(captured.database),hex(captured.simulation),hex(train),hex(driver),configuration,hex(check.root),check.members,
            check.previousElements,check.candidateElements,check.before.totalLengthMeters,unsigned(check.checked));
        trace.assessment(check.assessment,check.readIssue);trace.emit(check.allowed()?Level::Info:Level::Warning,"preflight");
        if(!check.allowed()){report(0,check.assessment);return;}
        ScopedRequest request(trace.sequence);nativeEntered=true;(hitch?originalHitch:originalSetConfig)(input,context);
        log(Level::Info,"Train length request=%llu action=%s phase=native-command-returned train=%llx",hex(trace.sequence),hitch?"hitch":"configuration",hex(train));return;
    }catch(...){
        if(nativeEntered){log(Level::Error,"Train length action=%s phase=native-exception command=%llx context=%llx mutationState=not-rolled-back",hitch?"hitch":"configuration",hex(input),hex(context));throw;}
        report(0,unavailable(limit));log(Level::Error,"Train length action=%s phase=preflight-exception command=%llx context=%llx",hitch?"hitch":"configuration",hex(input),hex(context));return;
    }
    (hitch?originalHitch:originalSetConfig)(input,context);
}
void hitch(uint64_t input,uint64_t context){simCommand(input,context,true);}
void setConfig(uint64_t input,uint64_t context){simCommand(input,context,false);}

struct HashFrame {uint64_t dynamics,rules;HashFrame* previous;};
thread_local HashFrame* hashFrame{};
void dynamics(uint64_t object,uint64_t rules) {
    // No extra scan for the game's usual <=30-vehicle compositions. This
    // safety hook stays resident after a mod unload, including save loading.
    std::array<uint64_t,2> range{};
    const View view(read);
    if(!view.get(object,range)||range[1]<range[0]||(range[1]-range[0])%32){
        const auto issue=view.issue();
        log(Level::Error,"Train dynamics refused: object=%llx rules=%llx begin=%llx end=%llx readFault=%s address=%llx detail=%llu",
            hex(object),hex(rules),hex(range[0]),hex(range[1]),fault(issue.fault),hex(issue.address),hex(issue.detail));return;
    }
    const auto count=(range[1]-range[0])/32;
    if(count<=30){originalDynamics(object,rules);return;}
    if(count>policy::maximumElements){
        log(Level::Error,"Train dynamics refused: object=%llx rules=%llx cars=%llu qualifiedSafetyBound=%zu",hex(object),hex(rules),hex(count),policy::maximumElements);return;
    }
    HashFrame frame{object,rules,hashFrame};hashFrame=&frame;
    struct Restore {HashFrame* previous;~Restore(){hashFrame=previous;}} restore{frame.previous};
    originalDynamics(object,rules);
}
uint64_t hash(const void* input,uint64_t bytes)noexcept {
    if(!hashFrame||bytes<=30*8)return originalHash(input,bytes);
    const auto* frame=hashFrame;hashFrame=nullptr;
    struct RestoreFrame{HashFrame* previous;~RestoreFrame(){hashFrame=previous;}} restore{const_cast<HashFrame*>(frame)};
    const char* phase="capture-composition";size_t resolved{},captured{};ReadIssue issue{};
    try{
        const View view(read);std::vector<Car> cars;
        if(!view.vector(frame->dynamics,cars)){issue=view.issue();throw std::runtime_error("Unreadable long composition");}
        captured=cars.size();phase="rebuild-car-hashes";
        // The ORIGINAL writes only30 stack hashes, then passes the full count.
        // Rebuild the same ordered hashes in SDK-owned storage BEFORE it reads.
        std::vector<uint64_t> hashes;hashes.reserve(cars.size());
        const auto leaf=reinterpret_cast<Leaf>(base+0x2416b0);
        const auto setup=reinterpret_cast<SettingsHash>(base+0x3f8720);
        constexpr uint64_t seed=0x9c3805fc2c85caccULL;
        const auto word=[&](uint64_t value){return leaf(&value,8,seed,0,8);};
        const auto combine=[&](uint64_t first,uint64_t second){const uint64_t pair[]{first,second};return leaf(pair,16,seed,0,16);};
        for(const auto& car:cars){
            if(!view.modelNode(frame->rules,car.model))continue; // Native skips missing model IDs.
            uint16_t index{};uint32_t paint{},flags{};
            std::memcpy(&index,car.settings.data(),2);std::memcpy(&paint,car.settings.data()+4,4);std::memcpy(&flags,car.settings.data()+8,4);
            auto value=setup(car.settings[2],paint,flags,car.settings[12],car.settings[13],car.settings[14],car.settings[15]);
            value=combine(word(index),value);value=combine(word(car.train),value);value=combine(word(car.model),value);
            hashes.push_back(value);
        }
        resolved=hashes.size();issue=view.issue();phase="verify-hash-count";
        if(hashes.size()*8!=bytes)throw std::runtime_error("Native composition changed during dynamics hashing");
        const auto result=originalHash(hashes.data(),hashes.size()*8);
        struct LastHash {uint64_t object{},result{};std::chrono::steady_clock::time_point when{};uint64_t repeated{};};
        thread_local LastHash previous;
        const auto now=std::chrono::steady_clock::now();
        if(previous.object!=frame->dynamics||previous.result!=result||now-previous.when>=std::chrono::seconds(5)){
            log(Level::Info,"Train dynamics hash rebuilt: request=%llu object=%llx rules=%llx cars=%zu hashedCars=%zu nativeRequestedBytes=%llu hash=%llx repeated=%llu readFault=%s address=%llx",
                hex(currentRequest),hex(frame->dynamics),hex(frame->rules),captured,resolved,hex(bytes),hex(result),hex(previous.repeated),fault(issue.fault),hex(issue.address));
            previous={frame->dynamics,result,now,0};
        }else ++previous.repeated;
        return result;
    }catch(...){
        log(Level::Error,"Train dynamics hash fallback: request=%llu phase=%s object=%llx rules=%llx cars=%zu hashedCars=%zu nativeRequestedBytes=%llu safePrefixBytes=240 readFault=%s address=%llx detail=%llu",
            hex(currentRequest),phase,hex(frame->dynamics),hex(frame->rules),captured,resolved,hex(bytes),fault(issue.fault),hex(issue.address),hex(issue.detail));
        // Allocation/read failure cannot turn into a read beyond the native
        // stack buffer. The deterministic prefix hash also avoids a crash on
        // retirement paths; diagnostics retain the incomplete-hash failure.
        return originalHash(input,30*8);
    }
}

NativeLeafGateway hashGateway;
struct Hook {uint64_t rva;void* detour;void** original;std::array<uint8_t,16> bytes;};
}

uint32_t install(uint64_t module)noexcept {
    if(!TryAcquireSRWLockExclusive(&installationLock))return NIMBY_RESOURCE_LIMIT;
    struct Unlock{~Unlock(){ReleaseSRWLockExclusive(&installationLock);}} unlock;
    if(installed)return NIMBY_OK;
    if(installationBroken)return NIMBY_HOOKS_UNAVAILABLE;
    BridgeInstallation installation;if(!installation)return installation.status();
    try{
        if(!diagnosticsReady){
            if(!diagnostics::writer().initialize()){
                nimby::detail::diagnostics::write("sdk","ERROR","Train length activation refused: asynchronous diagnostics initialization failed");return NIMBY_INTERNAL_ERROR;
            }
            diagnosticsReady=true;
        }
        log(Level::Info,"Train length activation started: module=%llx policyRead=atomic queueCapacity=%zu maximumMessageBytes=%zu writesPerSecond=%zu residentSafety=1",
            hex(module),diagnostics::Writer::capacity,diagnostics::Writer::maximumMessageBytes,diagnostics::Writer::maximumWritesPerSecond);
        std::array<wchar_t,32768> path{};NimbyBinaryInfo binary{};
        if(module!=reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr))||!GetModuleFileNameW(nullptr,path.data(),DWORD(path.size()))){
            log(Level::Error,"Train length activation refused: phase=locate-game module=%llx win32Error=%lu",hex(module),GetLastError());return NIMBY_INVALID_BINARY;
        }
        const auto identification=nimby::engine::identify(path.data(),binary);
        if(identification!=NIMBY_OK||!binary.recognized_research_build){
            log(Level::Error,"Train length activation refused: phase=qualify-game status=%u recognizedResearchBuild=%u",identification,unsigned(binary.recognized_research_build));return NIMBY_INVALID_BINARY;
        }
        base=module;
        // No hook references the gateway until the entire group is queued.
        // Retire its unwind entry and allocation when a transaction rolls back.
        struct GatewayCleanup {
            ~GatewayCleanup(){
                if(installed||installationBroken)return;
                if(!hashGateway.reset())log(Level::Error,"Train length activation rollback: gateway cleanup failed win32Error=%lu",GetLastError());
            }
        } gatewayCleanup;
        if(!hashGateway.build(reinterpret_cast<void*>(&hash))){log(Level::Error,"Train length activation refused: phase=build-hash-gateway win32Error=%lu",GetLastError());return NIMBY_INTERNAL_ERROR;}
        const Hook hooks[]{
            {0x3e0d90,reinterpret_cast<void*>(&dynamics),reinterpret_cast<void**>(&originalDynamics),{0x48,0x8b,0xc4,0x48,0x89,0x50,0x10,0x41,0x56,0x48,0x81,0xec,0xa0,0x01,0,0}},
            {0x2711c0,hashGateway.address(),reinterpret_cast<void**>(&originalHash),{0x40,0x53,0x56,0x57,0x48,0x83,0xec,0x40,0x48,0x8b,0xfa,0x48,0x8b,0xd9,0x48,0x8b}},
            {0x3e33f0,reinterpret_cast<void*>(&validate),reinterpret_cast<void**>(&originalValidate),{0x44,0x88,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x48}},
            {0x7ef7d0,reinterpret_cast<void*>(&commit),reinterpret_cast<void**>(&originalCommit),{0x48,0x89,0x5c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x55}},
            {0x7ed720,reinterpret_cast<void*>(&panel),reinterpret_cast<void**>(&originalPanel),{0x4c,0x89,0x4c,0x24,0x20,0x48,0x89,0x54,0x24,0x10,0x55,0x53,0x56,0x57,0x41,0x54}},
            {0x304b50,reinterpret_cast<void*>(&purchase),reinterpret_cast<void**>(&originalPurchase),{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x18,0x48,0x89,0x70,0x20,0x48}},
            {0x304e00,reinterpret_cast<void*>(&recompose),reinterpret_cast<void**>(&originalRecompose),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x48}},
            {0x441070,reinterpret_cast<void*>(&hitch),reinterpret_cast<void**>(&originalHitch),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x40,0x48}},
            {0x440f50,reinterpret_cast<void*>(&setConfig),reinterpret_cast<void**>(&originalSetConfig),{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0x59,0x10,0x48,0x8b,0xc1,0x48,0x8b,0x4a,0x08}}
        };
        // Validate every entry before creating or enabling any hook.
        for(const auto& hook:hooks){std::array<uint8_t,16> actual{};const auto readable=read(nullptr,module+hook.rva,actual.data(),actual.size());
            if(!readable||actual!=hook.bytes){Trace trace;trace.append("Train length activation refused: phase=verify-entry rva=%llx readable=%u expected=",hex(hook.rva),unsigned(readable));
                for(auto byte:hook.bytes){trace.append("%02x",unsigned(byte));}
                trace.append(" actual=");for(auto byte:actual){trace.append("%02x",unsigned(byte));}
                trace.emit(Level::Error,"signature-mismatch");return NIMBY_INVALID_BINARY;}
        }
        const auto initialization=MH_Initialize();if(initialization!=MH_OK&&initialization!=MH_ERROR_ALREADY_INITIALIZED){log(Level::Error,"Train length activation refused: phase=initialize-hooks hookStatus=%d",int(initialization));return NIMBY_INTERNAL_ERROR;}
        size_t created=0;
        struct HookCleanup {
            const Hook* hooks;size_t& count;
            ~HookCleanup(){
                if(installed)return;
                bool clean=true;
                // QueueDisable also clears a not-yet-applied enable. A later
                // bridge transaction cannot activate half of this group.
                for(size_t i=0;i<count;++i)if(MH_QueueDisableHook(reinterpret_cast<void*>(base+hooks[i].rva))!=MH_OK)clean=false;
                if(count&&MH_ApplyQueued()!=MH_OK)clean=false;
                if(clean)for(size_t i=0;i<count;++i)if(MH_RemoveHook(reinterpret_cast<void*>(base+hooks[i].rva))!=MH_OK)clean=false;
                if(!clean){installationBroken=true;
                    log(Level::Error,"Train length activation rollback incomplete: createdHooks=%zu furtherActivationRefused=1",count);
                }
            }
        } hookCleanup{hooks,created};
        for(const auto& hook:hooks){
            const auto result=MH_CreateHook(reinterpret_cast<void*>(module+hook.rva),hook.detour,hook.original);
            if(result!=MH_OK){log(Level::Error,"Train length activation refused: phase=create-hook rva=%llx hookStatus=%d createdHooks=%zu",hex(hook.rva),int(result),created);break;}
            ++created;
        }
        if(created!=std::size(hooks))return NIMBY_INTERNAL_ERROR;
        // The safe dynamics/hash pair is resident for the rest of this process.
        // Existing long trains remain readable when their policy owner stops.
        HMODULE self{};
        if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,reinterpret_cast<LPCWSTR>(&install),&self)){log(Level::Error,"Train length activation refused: phase=pin-module win32Error=%lu",GetLastError());return NIMBY_INTERNAL_ERROR;}
        for(const auto& hook:hooks){const auto result=MH_QueueEnableHook(reinterpret_cast<void*>(module+hook.rva));if(result!=MH_OK){log(Level::Error,"Train length activation refused: phase=enable-hook rva=%llx hookStatus=%d",hex(hook.rva),int(result));return NIMBY_INTERNAL_ERROR;}}
        const auto applied=MH_ApplyQueued();if(applied!=MH_OK){log(Level::Error,"Train length activation refused: phase=apply-hooks hookStatus=%d",int(applied));return NIMBY_INTERNAL_ERROR;}
        installed=true;
        log(Level::Info,"Train length activation completed: hooks=%zu entrySignaturesVerified=1 longCompositionSafetyResident=1",std::size(hooks));
        return NIMBY_OK;
    }catch(...){log(Level::Error,"Train length activation refused: phase=bootstrap-exception");return NIMBY_INTERNAL_ERROR;}
}
uint32_t status(policy::PublishResult value)noexcept {
    switch(value){
        case policy::PublishResult::Applied:return NIMBY_OK;
        case policy::PublishResult::Busy:case policy::PublishResult::CapacityReached:return NIMBY_RESOURCE_LIMIT;
        case policy::PublishResult::NotFound:case policy::PublishResult::InvalidOwner:return NIMBY_INVALID_HANDLE;
        case policy::PublishResult::InvalidLimit:return NIMBY_INVALID_ARGUMENT;
    }
    return NIMBY_INTERNAL_ERROR;
}
uint32_t registerPolicy(const char* id,uint32_t maximum,std::shared_ptr<const EditorMessages> messages,uint64_t* owner) {
    const auto ready=install(reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr)));if(ready!=NIMBY_OK)return ready;
    const auto token=allocateOwner();if(!token)return NIMBY_RESOURCE_LIMIT;
    const auto result=status(registry.replace(token,maximum,std::move(messages)));if(result==NIMBY_OK)*owner=token;
    log(result==NIMBY_OK?Level::Info:Level::Warning,"Train editor policy registration: project=%s owner=%llu requestedMeters=%u status=%u effectiveMeters=%.6f revision=%llu copiedPresentation=1",
        id,hex(token),maximum,result,registry.limit().maximumMeters,hex(registry.selectionRevision()));return result;
}
}
#define TRAIN_EXPORT extern "C" __declspec(dllexport) uint32_t
TRAIN_EXPORT NimbyTrainLength_Register(const char* id,uint32_t maximum,uint64_t* owner)noexcept {
    using namespace nimby::platform::windows::train_length;
    if(!owner)return NIMBY_INVALID_ARGUMENT;
    *owner=0;
    if(!id||!maximum||!policy::validMaximum(maximum))return NIMBY_INVALID_ARGUMENT;
    size_t count=0;while(count<129&&id[count])++count;if(!count||count>128)return NIMBY_INVALID_ARGUMENT;
    try{return registerPolicy(id,maximum,EditorMessages::legacy(),owner);}
    catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}
    catch(...){log(Level::Error,"Train length policy registration failed: phase=exception project=%s requestedMeters=%u",id,maximum);return NIMBY_INTERNAL_ERROR;}
}
TRAIN_EXPORT NimbyTrainEditor_RegisterV2(const char* id,uint32_t maximum,const char* declaration,uint32_t bytes,uint64_t* owner)noexcept {
    using namespace nimby::platform::windows::train_length;
    if(owner)*owner=0;
    if(!owner||!id||!policy::validMaximum(maximum)||!declaration||!bytes||bytes>EditorMessages::maximumDeclarationBytes)
        return NIMBY_INVALID_ARGUMENT;
    size_t count=0;while(count<129&&id[count])++count;if(!count||count>128)return NIMBY_INVALID_ARGUMENT;
    try{
        auto messages=std::make_shared<const EditorMessages>(std::string_view(declaration,bytes));
        return registerPolicy(id,maximum,std::move(messages),owner);
    }catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}
    catch(const std::invalid_argument&){log(Level::Warning,"Train editor declaration rejected: project=%s requestedMeters=%u bytes=%u phase=validation",id,maximum,bytes);return NIMBY_INVALID_ARGUMENT;}
    catch(...){log(Level::Error,"Train editor registration failed: project=%s requestedMeters=%u phase=exception",id,maximum);return NIMBY_INTERNAL_ERROR;}
}
TRAIN_EXPORT NimbyTrainLength_Update(uint64_t owner,uint32_t maximum)noexcept {
    using namespace nimby::platform::windows::train_length;
    try{const auto before=registry.limit().maximumMeters;const auto result=status(registry.update(owner,maximum));
        log(result==NIMBY_OK?Level::Info:Level::Warning,"Train length policy update: owner=%llu requestedMeters=%u status=%u previousEffectiveMeters=%.6f effectiveMeters=%.6f",
            hex(owner),maximum,result,before,registry.limit().maximumMeters);return result;
    }catch(...){log(Level::Error,"Train length policy update failed: phase=exception owner=%llu requestedMeters=%u",hex(owner),maximum);return NIMBY_INTERNAL_ERROR;}
}
TRAIN_EXPORT NimbyTrainLength_Remove(uint64_t owner)noexcept {
    using namespace nimby::platform::windows::train_length;
    try{const auto before=registry.limit().maximumMeters;const auto published=registry.remove(owner);const auto result=status(published);
        if(published==policy::PublishResult::Busy){deferredRemovalAttempts.fetch_add(1,std::memory_order_relaxed);return result;}
        log(result==NIMBY_OK?Level::Info:Level::Warning,"Train length policy removal: owner=%llu status=%u previousEffectiveMeters=%.6f effectiveMeters=%.6f deferredRemovalAttemptsSincePreviousCompletion=%llu longCompositionSafetyResident=1",
            hex(owner),result,before,registry.limit().maximumMeters,hex(deferredRemovalAttempts.exchange(0,std::memory_order_relaxed)));return result;
    }catch(...){log(Level::Error,"Train length policy removal failed: phase=exception owner=%llu",hex(owner));return NIMBY_INTERNAL_ERROR;}
}
