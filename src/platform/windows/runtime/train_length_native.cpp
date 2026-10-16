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
#include <winternl.h>
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
std::atomic<bool> hashSafetyResident{false};
std::atomic<bool> mechanicsReady{false};
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
using Aggregate=void(*)(uint64_t,uint64_t,uint64_t);
using Hash=uint64_t(*)(const void*,uint64_t);
using Leaf=uint64_t(*)(const void*,uint64_t,uint64_t,uint64_t,uint64_t);
using SettingsHash=uint64_t(*)(uint8_t,uint32_t,uint64_t,uint8_t,uint8_t,uint8_t,uint8_t);
using Validate=uint64_t(*)(uint64_t,uint64_t,uint64_t,uint8_t);
using Commit=void(*)(uint64_t,uint64_t,uint64_t);
using Panel=void(*)(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint8_t);
using Command=uint64_t(*)(uint64_t,uint64_t,uint64_t,uint64_t);
using SimCommand=void(*)(uint64_t,uint64_t);
Dynamics originalDynamics{};
Aggregate originalAggregate{};
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
    std::array<char,EditorMessages::maximumMessageBytes+128> measurements{};
};
// Both composition commit and its panel belong to the native UI thread. A
// borrowed editor address is only compared, never dereferenced after the call.
thread_local Failure lastFailure;
thread_local uint64_t currentEditor{};
thread_local uint64_t currentRequest{};
// Reuse validation of the real editor composition within this UI pass. A
// proposed (possibly rejected) addition is never a current-length sample.
struct PanelMeasurement {
    uint64_t editor{},rules{};
    PanelMeasurement* previous{};
    std::optional<policy::Assessment> value;
};
thread_local PanelMeasurement* panelMeasurement{};
struct ScopedPanelMeasurement {
    PanelMeasurement frame;
    ScopedPanelMeasurement(uint64_t editor,uint64_t rules):frame{editor,rules,panelMeasurement,{}}{panelMeasurement=&frame;}
    ~ScopedPanelMeasurement(){panelMeasurement=frame.previous;}
};
void invalidatePanelMeasurement(uint64_t editor)noexcept {
    if(panelMeasurement&&panelMeasurement->editor==editor)panelMeasurement->value.reset();
}
struct CurrentLengthDisplay {
    uint64_t editor{},rules{},revision{};
    policy::Assessment value;
    bool initialized=false,layoutReady=false;
    std::string language;
    std::array<char,EditorMessages::maximumMessageBytes+128> line{};
};
thread_local CurrentLengthDisplay currentLengthDisplay;
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
bool prepareCurrentLength(uint64_t editor,uint64_t rules,const std::shared_ptr<const EditorRegistry::Selection>& selection,
                          const PanelMeasurement& frame,bool layout,std::string_view language) {
    auto& display=currentLengthDisplay;
    if(!selection||!selection->presentation||!selection->presentation->showsLength()){
        display={};return false;
    }
    // The interactive pass paints the exact row measured by layout. Edits
    // made while handling a click are reflected by the next layout/paint,
    // without another scan or a second allocation during interaction.
    if(!layout&&display.layoutReady&&display.editor==editor&&display.rules==rules&&
       display.revision==selection->revision&&display.language==language){
        display.layoutReady=false;return true;
    }
    auto value=unavailable(selection->limit);
    if(frame.value&&frame.value->maximumMeters==selection->limit.maximumMeters)value=*frame.value;
    else if(layout)value=composition(View(read),editor+0xc0,rules,selection->limit);
    // An incomplete measurement never masquerades as a current length. If
    // there was no matching layout or validation, display the mod's unknown
    // state; do not reuse facts from an older editor or composition.
    const bool changed=!display.initialized||display.editor!=editor||display.rules!=rules||
        display.revision!=selection->revision||display.language!=language||
        display.value.decision!=value.decision||display.value.totalLengthMeters!=value.totalLengthMeters||
        display.value.maximumMeters!=value.maximumMeters;
    if(changed){
        const bool complete=value.decision==policy::Decision::Allowed||value.decision==policy::Decision::LimitExceeded;
        const auto& label=selection->presentation->select(complete?3:5,language);
        if(complete)std::snprintf(display.line.data(),display.line.size(),"%s: %.3f m / %.0f m",
            label.c_str(),value.totalLengthMeters,selection->limit.maximumMeters);
        else std::snprintf(display.line.data(),display.line.size(),"%s",label.c_str());
        log(complete?Level::Info:Level::Warning,
            "Train length display updated: editor=%llx rules=%llx revision=%llu currentKnown=%u measuredMeters=%.6f maximumMeters=%.6f decision=%s problematicModel=%llx",
            hex(editor),hex(rules),hex(selection->revision),unsigned(complete),value.totalLengthMeters,
            selection->limit.maximumMeters,decision(value.decision),hex(value.problematicModel));
    }
    display.editor=editor;display.rules=rules;display.revision=selection->revision;display.value=value;
    display.initialized=true;display.layoutReady=layout;
    if(display.language!=language)display.language=language;
    return true;
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
                if(proposal.removalRequested&&lastFailure.editor==editor){
                    log(Level::Info,"Train length request=%llu action=remove-requested phase=previous-refusal-retired editor=%llx previousDecision=%s previousCandidateMeters=%.6f maximumMeters=%.6f",
                        hex(sequence),hex(editor),decision(lastFailure.assessment.decision),lastFailure.assessment.totalLengthMeters,lastFailure.assessment.maximumMeters);
                    // Retire only this editor's old candidate BEFORE the native
                    // operation. Its validation may report a new failure, which
                    // must survive. Ordinary validation of the unchanged train
                    // keeps rejected-addition notices visible.
                    lastFailure={};
                }
                invalidatePanelMeasurement(editor);
                nativeEntered=true;originalCommit(editor,context,rules);
                // Validation inside the native operation may precede its
                // final mutation. Re-measure only once, at the end of layout.
                invalidatePanelMeasurement(editor);
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
        invalidatePanelMeasurement(editor);
        nativeEntered=true;
        originalCommit(editor,context,rules);
        invalidatePanelMeasurement(editor);
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
    ScopedPanelMeasurement measurement(editor,rules);
    originalPanel(editor,context,emitter,rules,arg5,arg6,arg7,arg8,arg9,arg10);
    const auto selection=registry.selected();
    const bool notice=lastFailure.editor==editor&&lastFailure.selection&&selection&&
        selection->revision==lastFailure.selection->revision&&std::chrono::steady_clock::now()<lastFailure.until;
    if(!notice&&(!selection||!selection->presentation||!selection->presentation->showsLength()))return;
    try{
        auto ui=nimby::engine::SignalUi::bind(read,nullptr,base,emitter);
        if(!ui)return;
        const auto language=nimby::platform::windows::gameLanguage(read,nullptr,base).value_or("");
        const bool current=prepareCurrentLength(editor,rules,selection,measurement.frame,
            ui->pass()==nimby::engine::SignalUi::Pass::Layout,language);
        // Preference changes or mod retirement can invalidate the captured
        // wording while the original body runs. Never paint an old limit.
        if(!selection||registry.selectionRevision()!=selection->revision)return;
        if(current)ui->message(currentLengthDisplay.line.data());
        if(!notice)return;
        if(!lastFailure.message||lastFailure.language!=language){
            const auto decision=lastFailure.assessment.decision;
            const size_t index=decision==policy::Decision::LimitExceeded?0:
                decision==policy::Decision::UnknownModel||decision==policy::Decision::InvalidLength?1:2;
            lastFailure.message=&lastFailure.selection->presentation->select(index,language);
            lastFailure.language=language;
            if(decision==policy::Decision::LimitExceeded&&lastFailure.selection->presentation->showsLength()){
                const auto& label=lastFailure.selection->presentation->select(4,language);
                std::snprintf(lastFailure.measurements.data(),lastFailure.measurements.size(),"%s: %.3f m / %.0f m",
                    label.c_str(),lastFailure.assessment.totalLengthMeters,lastFailure.assessment.maximumMeters);
            }
        }
        // No JSON, mod callback, IPC or disk read in this panel. Validation
        // supplies current facts; layout alone performs a bounded fallback.
        ui->message(lastFailure.message->c_str());
        if(lastFailure.measurements[0])ui->message(lastFailure.measurements.data());
    }catch(...){log(Level::Error,"Train length action=notification phase=ui-message-exception editor=%llx emitter=%llx",hex(editor),hex(emitter));}
}
uint64_t validate(uint64_t train,uint64_t output,uint64_t rules,uint8_t ignoreCouplers) {
    const auto result=originalValidate(train,output,rules,ignoreCouplers);
    const auto limit=registry.limit();if(!limit.active())return result;
    try{
        const View view(read);auto value=composition(view,train+0xc0,rules,limit);
        if(panelMeasurement&&panelMeasurement->editor==train&&panelMeasurement->rules==rules)
            panelMeasurement->value=value; // Capture BEFORE legacy reduction exceptions.
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
            const auto check=groups::recompose(read,nullptr,{database,simulation,runtimeMechanicsReady()},limit,train,cars);
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
        captured.extendedAggregation=runtimeMechanicsReady();
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

// Per-thread, call-scoped admission. The Windows inline TLS slot is reserved
// before hooks activate, avoiding emulated C++ TLS's first-use allocation.
struct AggregateFrame {
    uint64_t object{},count{};
    std::array<aggregation::Prefix,aggregation::maximumMembers-1> prefixes{};
};
static_assert(offsetof(AggregateFrame,prefixes)==16&&sizeof(aggregation::Prefix)==16);
static_assert(offsetof(TEB,TlsSlots)==0x1480);
DWORD aggregateTls=TLS_OUT_OF_INDEXES;

// Replacement of the interior CMP+JA, with no CALL, allocation, native-vector
// access or XMM mutation. An old/foreign native invocation has no matching TLS
// admission and keeps CMP30. Preserve RAX/RDX in the native outgoing shadow
// area (44CF80 has no locals in RSP..RSP+31 and no CALL runs in this gateway);
// produce the exact unsigned CMP
// flags, including the root-before-child reserved count, then native continues.
// Because this is an INTERIOR JMP, unwind chains back to the native 44CF80
// frame (not a fictitious leaf return address). RSP never changes, so the
// zero-code noncontiguous CHAININFO has the same fixed allocation as native.
class AggregateCountGateway {
    uint8_t* memory_{};uint64_t accepted_{},rejected_{},module_{};DWORD slot_{};
    std::array<RUNTIME_FUNCTION,1> table_{};bool registered_=false;
    size_t restoreEnd_{},codeBytes_{};
public:
    bool build(uint64_t accepted,uint64_t rejected,DWORD slot,uint64_t module)noexcept {
        if(!accepted||!rejected||slot>=64||!module)return false;
        if(memory_)return registered_&&accepted_==accepted&&rejected_==rejected&&slot_==slot&&module_==module;
        DWORD64 parentBase{},rejectBase{};
        auto* parent=RtlLookupFunctionEntry(accepted,&parentBase,nullptr);
        auto* rejectParent=RtlLookupFunctionEntry(rejected,&rejectBase,nullptr);
        if(!parent||!rejectParent||parentBase!=module||rejectBase!=module||
            std::memcmp(parent,rejectParent,sizeof(RUNTIME_FUNCTION)))return false;
        std::array<uint8_t,4> parentHeader{};
        if(!read(nullptr,module+parent->UnwindData,parentHeader.data(),parentHeader.size())||
            (parentHeader[0]&7)!=1||parentHeader[3]!=0)return false;
        const RUNTIME_FUNCTION parentCopy=*parent;
        std::array<uint8_t,256> code{};size_t n=0;
        const auto emit=[&](std::initializer_list<uint8_t> bytes){for(auto byte:bytes)code[n++]=byte;};
        const auto imm=[&](uint64_t value,unsigned bytes){for(unsigned i=0;i<bytes;++i)code[n++]=uint8_t(value>>(i*8));};
        const auto branch=[&](uint8_t op){emit({0x0f,op});const size_t at=n;imm(0,4);return at;};
        const auto relative=[&](size_t at,size_t target){const int32_t delta=static_cast<int32_t>(target)-static_cast<int32_t>(at+4);std::memcpy(code.data()+at,&delta,4);};
        emit({0x48,0x89,0x04,0x24,0x48,0x89,0x54,0x24,0x08});
        emit({0x48,0xb8});imm(rejected,8);emit({0x48,0x89,0x44,0x24,0x18});
        emit({0x48,0xb8});imm(accepted,8);emit({0x48,0x89,0x44,0x24,0x10});
        emit({0x65,0x48,0x8b,0x04,0x25,0x30,0,0,0}); // NtCurrentTeb
        emit({0x48,0x8b,0x80});imm(offsetof(TEB,TlsSlots)+uint64_t(slot)*8,4);
        emit({0x48,0x85,0xc0});const auto noFrame=branch(0x84);
        emit({0x48,0x8d,0x53,0xf8,0x48,0x39,0x10});const auto otherRoot=branch(0x85);
        emit({0x48,0x8b,0x50,0x08,0x48,0x85,0xd2});const auto noChildren=branch(0x84);
        emit({0x48,0x8d,0x40,0x10});const auto loop=n;
        emit({0x4c,0x39,0x28});const auto found=branch(0x84);
        emit({0x48,0x8d,0x40,0x10,0x48,0xff,0xca});const auto next=branch(0x85);relative(next,loop);
        const auto fallback=n;emit({0x48,0x83,0xf9,0x1e,0xe9});const auto toRestore=n;imm(0,4);
        const auto qualified=n;emit({0x48,0x3b,0x48,0x08});
        const auto restore=n;
        emit({0x48,0x8b,0x04,0x24,0x48,0x8b,0x54,0x24,0x08});
        restoreEnd_=n;
        // Disp8 stack-indirect JMP (ModRM.mod=01) is deliberately not an x64
        // recognized tailcall epilogue. A RIP-indirect JMP (mod=00) would make
        // RtlVirtualUnwind pop a fictitious leaf return instead of following
        // CHAININFO and restoring the native frame. All four native shadow
        // slots are free here; the branch destinations cannot outlive the JMP.
        emit({0x76,0x04,0xff,0x64,0x24,0x18,0xff,0x64,0x24,0x10});
        relative(noFrame,fallback);relative(otherRoot,fallback);relative(noChildren,fallback);relative(found,qualified);relative(toRestore,restore);
        codeBytes_=n;
        // The table shares the original native image base so CHAININFO RVAs
        // keep their meaning. Search a bounded set of nearby allocation hints.
        uint8_t* memory=nullptr;
        for(size_t i=0;i<128&&!memory;++i){
            const uint64_t hint=module+0x2000000+i*0x10000;
            if(hint<module||hint>0x7fffffff0000ULL-4096)return false;
            memory=static_cast<uint8_t*>(VirtualAlloc(reinterpret_cast<void*>(hint),4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
        }
        if(!memory)return false;
        const uint64_t rva=reinterpret_cast<uint64_t>(memory)-module;
        if(rva>UINT32_MAX-4096){VirtualFree(memory,0,MEM_RELEASE);return false;}
        std::memcpy(memory,code.data(),n);
        const std::array<uint8_t,4> chained{0x21,0,0,0};
        std::memcpy(memory+256,chained.data(),chained.size());std::memcpy(memory+260,&parentCopy,sizeof parentCopy);
        table_={RUNTIME_FUNCTION{DWORD(rva),DWORD(rva+n),DWORD(rva+256)}};
        DWORD previous{};
        if(!VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&previous)||!FlushInstructionCache(GetCurrentProcess(),memory,n)||
            !RtlAddFunctionTable(table_.data(),DWORD(table_.size()),module)){
            VirtualFree(memory,0,MEM_RELEASE);table_={};return false;
        }
        memory_=memory;accepted_=accepted;rejected_=rejected;module_=module;slot_=slot;registered_=true;return true;
    }
    void* address()const noexcept{return registered_?memory_:nullptr;}
    size_t restoreEnd()const noexcept{return restoreEnd_;}
    size_t codeBytes()const noexcept{return codeBytes_;}
    bool reset()noexcept {
        if(registered_){if(!RtlDeleteFunctionTable(table_.data()))return false;registered_=false;}
        if(memory_&&!VirtualFree(memory_,0,MEM_RELEASE))return false;
        memory_=nullptr;accepted_=rejected_=module_=0;table_={};restoreEnd_=codeBytes_=0;return true;
    }
};
void aggregate(uint64_t pool,uint64_t database,uint64_t object) {
    // No mod callback/policy/UI and no SDK-owned native-vector replacement.
    // Refused input leaves the native composition untouched. The final rear
    // provider must independently qualify the published aggregate and scope.
    if(!mechanicsReady.load(std::memory_order_acquire)||!hashSafetyResident.load(std::memory_order_acquire)){
        log(Level::Error,"Native composition refused: phase=mechanics-not-ready object=%llx",hex(object));return;
    }
    const auto input=aggregation::admit(View(read),pool,database,object);
    if(!input.allowed){
        log(Level::Error,"Native composition refused: phase=aggregate-preflight train=%llx object=%llx fault=%u address=%llx rootCars=%zu totalCars=%zu members=%zu safetyBound=%zu mutation=none",
            hex(input.train),hex(object),unsigned(input.fault),hex(input.address),input.rootElements,input.totalElements,input.members,policy::maximumElements);return;
    }
    if(aggregateTls>=64){log(Level::Error,"Native composition refused: phase=tls-slot-unavailable object=%llx",hex(object));return;}
    AggregateFrame frame;frame.object=object;frame.count=input.members-1;frame.prefixes=input.prefixes;
    auto& tls=NtCurrentTeb()->TlsSlots[aggregateTls];
    auto* previous=tls;tls=input.extended?&frame:nullptr;
    struct RestoreAdmission {void*& slot;void* previous;~RestoreAdmission(){slot=previous;}} restore{tls,previous};
    // Invoke exactly once. A native exception after mutation is not retried or
    // represented as a successful/rolled-back composition by the SDK.
    originalAggregate(pool,database,object);
}

NativeLeafGateway hashGateway;
AggregateCountGateway aggregateGateway;
void* originalAggregateCompare{};
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
                if(installed||installationBroken||hashSafetyResident.load(std::memory_order_acquire))return;
                if(!hashGateway.reset())log(Level::Error,"Train length activation rollback: gateway cleanup failed win32Error=%lu",GetLastError());
                if(!aggregateGateway.reset())log(Level::Error,"Train length activation rollback: aggregate gateway cleanup failed win32Error=%lu",GetLastError());
                if(aggregateTls!=TLS_OUT_OF_INDEXES){TlsFree(aggregateTls);aggregateTls=TLS_OUT_OF_INDEXES;}
            }
        } gatewayCleanup;
        if(!hashGateway.build(reinterpret_cast<void*>(&hash))){log(Level::Error,"Train length activation refused: phase=build-hash-gateway win32Error=%lu",GetLastError());return NIMBY_INTERNAL_ERROR;}
        if(aggregateTls==TLS_OUT_OF_INDEXES){
            aggregateTls=TlsAlloc();
            if(aggregateTls>=64){if(aggregateTls!=TLS_OUT_OF_INDEXES)TlsFree(aggregateTls);aggregateTls=TLS_OUT_OF_INDEXES;
                log(Level::Error,"Train length activation refused: phase=reserve-inline-tls first64SlotsRequired=1");return NIMBY_RESOURCE_LIMIT;}
            void* marker=&aggregateTls;
            if(!TlsSetValue(aggregateTls,marker)||NtCurrentTeb()->TlsSlots[aggregateTls]!=marker||TlsGetValue(aggregateTls)!=marker||!TlsSetValue(aggregateTls,nullptr)){
                log(Level::Error,"Train length activation refused: phase=qualify-inline-tls");return NIMBY_INTERNAL_ERROR;
            }
        }
        if(!aggregateGateway.build(module+0x44d3f8,module+0x44d62a,aggregateTls,module)){
            log(Level::Error,"Train length activation refused: phase=build-aggregate-gateway win32Error=%lu",GetLastError());return NIMBY_INTERNAL_ERROR;
        }
        const Hook hooks[]{
            {0x3e0d90,reinterpret_cast<void*>(&dynamics),reinterpret_cast<void**>(&originalDynamics),{0x48,0x8b,0xc4,0x48,0x89,0x50,0x10,0x41,0x56,0x48,0x81,0xec,0xa0,0x01,0,0}},
            {0x2711c0,hashGateway.address(),reinterpret_cast<void**>(&originalHash),{0x40,0x53,0x56,0x57,0x48,0x83,0xec,0x40,0x48,0x8b,0xfa,0x48,0x8b,0xd9,0x48,0x8b}},
            {0x3e33f0,reinterpret_cast<void*>(&validate),reinterpret_cast<void**>(&originalValidate),{0x44,0x88,0x4c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x48}},
            {0x7ef7d0,reinterpret_cast<void*>(&commit),reinterpret_cast<void**>(&originalCommit),{0x48,0x89,0x5c,0x24,0x20,0x4c,0x89,0x44,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x55}},
            {0x7ed720,reinterpret_cast<void*>(&panel),reinterpret_cast<void**>(&originalPanel),{0x4c,0x89,0x4c,0x24,0x20,0x48,0x89,0x54,0x24,0x10,0x55,0x53,0x56,0x57,0x41,0x54}},
            {0x304b50,reinterpret_cast<void*>(&purchase),reinterpret_cast<void**>(&originalPurchase),{0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x18,0x48,0x89,0x70,0x20,0x48}},
            {0x304e00,reinterpret_cast<void*>(&recompose),reinterpret_cast<void**>(&originalRecompose),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x6c,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x48}},
            {0x441070,reinterpret_cast<void*>(&hitch),reinterpret_cast<void**>(&originalHitch),{0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x40,0x48}},
            {0x440f50,reinterpret_cast<void*>(&setConfig),reinterpret_cast<void**>(&originalSetConfig),{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0x59,0x10,0x48,0x8b,0xc1,0x48,0x8b,0x4a,0x08}},
            {0x44cf80,reinterpret_cast<void*>(&aggregate),reinterpret_cast<void**>(&originalAggregate),{0x48,0x8b,0xc4,0x4c,0x89,0x40,0x18,0x48,0x89,0x50,0x10,0x48,0x89,0x48,0x08,0x55}},
            {0x44d3ee,aggregateGateway.address(),&originalAggregateCompare,{0x48,0x83,0xf9,0x1e,0x0f,0x87,0x32,0x02,0,0,0x41,0x80,0xbd,0xf0,0x01,0}}
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
                const size_t first=hashSafetyResident.load(std::memory_order_acquire)?2:0;
                for(size_t i=first;i<count;++i)if(MH_QueueDisableHook(reinterpret_cast<void*>(base+hooks[i].rva))!=MH_OK)clean=false;
                if(count>first&&MH_ApplyQueued()!=MH_OK)clean=false;
                if(clean)for(size_t i=first;i<count;++i)if(MH_RemoveHook(reinterpret_cast<void*>(base+hooks[i].rva))!=MH_OK)clean=false;
                // Once native long-train safety has been published it must
                // remain resident even when the later hooks cannot activate.
                if(first)installationBroken=true;
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
        // Phase one actually activates the safe dynamics/hash pair before the
        // aggregation count predicate can admit a longer child composition.
        for(size_t i=0;i<2;++i){const auto& hook=hooks[i];const auto result=MH_QueueEnableHook(reinterpret_cast<void*>(module+hook.rva));
            if(result!=MH_OK){log(Level::Error,"Train length activation refused: phase=enable-safety rva=%llx hookStatus=%d",hex(hook.rva),int(result));return NIMBY_INTERNAL_ERROR;}}
        auto applied=MH_ApplyQueued();if(applied!=MH_OK){log(Level::Error,"Train length activation refused: phase=apply-safety hookStatus=%d",int(applied));return NIMBY_INTERNAL_ERROR;}
        hashSafetyResident.store(true,std::memory_order_release);
        for(size_t i=2;i<std::size(hooks);++i){const auto& hook=hooks[i];const auto result=MH_QueueEnableHook(reinterpret_cast<void*>(module+hook.rva));if(result!=MH_OK){log(Level::Error,"Train length activation refused: phase=enable-hook rva=%llx hookStatus=%d",hex(hook.rva),int(result));return NIMBY_INTERNAL_ERROR;}}
        applied=MH_ApplyQueued();if(applied!=MH_OK){log(Level::Error,"Train length activation refused: phase=apply-hooks hookStatus=%d",int(applied));return NIMBY_INTERNAL_ERROR;}
        installed=true;
        mechanicsReady.store(true,std::memory_order_release);
        log(Level::Info,"Train length activation completed: hooks=%zu entrySignaturesVerified=1 longCompositionSafetyResident=1 aggregateWholeBound=%zu aggregateMemberBound=%zu policyRequired=0",std::size(hooks),policy::maximumElements,aggregation::maximumMembers);
        return NIMBY_OK;
    }catch(...){log(Level::Error,"Train length activation refused: phase=bootstrap-exception");return NIMBY_INTERNAL_ERROR;}
}
uint32_t ensureRuntimeMechanics(uint64_t module)noexcept {return install(module);}
bool runtimeMechanicsReady()noexcept {return mechanicsReady.load(std::memory_order_acquire)&&hashSafetyResident.load(std::memory_order_acquire);}
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
TRAIN_EXPORT NimbyTrainRuntime_Ensure()noexcept {
    return nimby::platform::windows::train_length::ensureRuntimeMechanics(
        reinterpret_cast<uint64_t>(GetModuleHandleW(nullptr)));
}
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
