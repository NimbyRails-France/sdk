#include <windows.h>
#include <platform/windows/train_length_native.h>
#include <array>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <cstdlib>
#include <new>
#include <bit>

static thread_local bool failAllocation{},watchAllocation{};
static thread_local size_t allocationCount{};
[[gnu::noinline]] void* operator new(size_t size){if(watchAllocation)++allocationCount;if(failAllocation)throw std::bad_alloc();if(auto* p=std::malloc(size?size:1))return p;throw std::bad_alloc();}
[[gnu::noinline]] void operator delete(void* p)noexcept{std::free(p);}
[[gnu::noinline]] void operator delete(void* p,size_t)noexcept{std::free(p);}

// Exercise the production detours without calling install(), loading a game or
// creating any hook. Only the memory transport is replaced; outputs are real,
// test-owned buffers so production result writes also remain under test.
static auto realReadProcessMemory=&::ReadProcessMemory;
static BOOL WINAPI fixtureReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
static bool failVirtualAllocation{};
static LPVOID WINAPI fixtureVirtualAlloc(LPVOID at,SIZE_T bytes,DWORD type,DWORD protection){return failVirtualAllocation?nullptr : ::VirtualAlloc(at,bytes,type,protection);}
#define ReadProcessMemory fixtureReadProcessMemory
#define VirtualAlloc fixtureVirtualAlloc
#include "../../src/platform/windows/runtime/train_length_native.cpp"
#undef ReadProcessMemory
#undef VirtualAlloc

static size_t checks{};
static void checked(bool success,const char* expression,int line){
    ++checks;if(!success){std::cerr<<"Check failed at "<<line<<": "<<expression<<'\n';std::abort();}
}
#undef assert
#define assert(value) checked(static_cast<bool>(value),#value,__LINE__)

namespace native=nimby::platform::windows::train_length;
namespace policy=nimby::engine::train_length;
namespace {
using namespace std::chrono_literals;
struct Memory {
    static constexpr uint64_t base=0x10000,database=0x20000,simulation=0x40000,rules=0x60000,
        table=0x61000,sentinel=0x62000,models=0x70000,trainBlocks=0x80000,motionBlocks=0x90000,
        editor=0xa0000,context=0xb0000,command=0xc0000,candidate=0xd0000,
        trains=0x100000,motions=0x200000,carStorage=0x300000;
    std::vector<uint8_t> data=std::vector<uint8_t>(0x600000);
    size_t reads{},modelCount{},failRead{};uint64_t failAt{},mutateAt{};size_t mutateRead{};
    static uint64_t id(size_t index){return (uint64_t{5}<<48)|(index<<16)|7;}
    static uint64_t train(size_t index){return trains+index*0x178;}
    static uint64_t motion(size_t index){return motions+index*0x638;}
    template<class T> void put(uint64_t at,const T& value){assert(at>=base&&at-base<=data.size()-sizeof value);std::memcpy(data.data()+at-base,&value,sizeof value);}
    template<class T> T get(uint64_t at)const{T value{};std::memcpy(&value,data.data()+at-base,sizeof value);return value;}
    bool read(uint64_t at,void* target,size_t count){
        ++reads;
        if(mutateAt&&reads==mutateRead){put(mutateAt,get<uint64_t>(mutateAt)^uint64_t{1});}
        if(failRead&&reads==failRead)return false;
        if(at<base||count>data.size()||at-base>data.size()-count||(failAt&&at<=failAt&&failAt-at<count))return false;
        std::memcpy(target,data.data()+at-base,count);return true;
    }
    void pool(uint64_t header,uint64_t blocks,uint64_t block){
        put(header,std::array<uint8_t,48>{});put(header+4,uint32_t{5});put(header+8,uint32_t{32});
        put(header+0x10,uint32_t{31});put(header+0x18,blocks);put(header+0x20,blocks+8);put(header+0x28,blocks+8);put(blocks,block);
    }
    void model(uint64_t id,float meters){
        const auto node=models+modelCount++*0x400,bucket=table+(id%8)*8;
        put(node,id);put(node+0xfc,meters);put(node+0x2a0,get<uint64_t>(bucket));put(bucket,node);
    }
    void vector(uint64_t descriptor,uint64_t storage,const std::vector<native::Car>& cars){
        put(descriptor,std::array<uint64_t,3>{storage,storage+cars.size()*32,storage+cars.size()*32});
        if(!cars.empty())std::memcpy(data.data()+storage-base,cars.data(),cars.size()*32);
    }
    void record(size_t index,const std::vector<native::Car>& cars){
        put(train(index),id(index));put(motion(index),id(index));
        put(motion(index)+0x1d8,uint64_t{});put(motion(index)+0x1f0,uint8_t{});
        vector(train(index)+0xc0,carStorage+index*0x10000,cars);
    }
    void edit(const std::vector<native::Car>& existing,uint64_t pending,int32_t count){
        vector(editor+0xc0,carStorage+0x20000,existing);
        std::array<uint8_t,16> flags{};flags[4]=1;put(editor+0x15c0,flags);
        put(editor+0x15d0,uint8_t{1});put(editor+0x1598,std::array<uint8_t,7>{});put(editor+0x1138,uint64_t{});
        put(editor+0x15e0,native::Car{pending});put(editor+0x1600,count);
    }
    void proposal(const std::vector<native::Car>& cars,bool purchase){
        vector(command+(purchase?0xe0:0x28),candidate,cars);put(command+0x20,id(0));
    }
    Memory(){
        pool(database+0x200,trainBlocks,trains);pool(simulation+0xa0,motionBlocks,motions);
        put(database+0x408,rules);put(rules+0x38,table);put(rules+0x40,uint32_t{8});put(table+64,sentinel);
        put(context+0x890,database);model(1,25);model(2,400);model(3,425);model(4,.01f);model(5,500);model(6,600);
        record(0,{{2}});
    }
};
Memory* memory{};
struct Logs {
    std::mutex mutex;std::condition_variable changed;std::vector<std::string> messages;
    static void sink(void* context,const char*,const char* message)noexcept {
        auto& self=*static_cast<Logs*>(context);std::lock_guard lock(self.mutex);
        self.messages.emplace_back(message);self.changed.notify_all();
    }
    void awaitPreflight(uint64_t request){
        const auto prefix="Train length request="+std::to_string(request)+" ";
        std::unique_lock lock(mutex);
        assert(changed.wait_for(lock,4s,[&]{return std::any_of(messages.begin(),messages.end(),[&](const auto& text){
            return text.starts_with(prefix)&&(text.find("phase=preflight")!=std::string::npos||
                text.find("phase=delegate-unchanged")!=std::string::npos);
        });}));
    }
    bool contains(const char* text){std::lock_guard lock(mutex);return std::any_of(messages.begin(),messages.end(),[&](const auto& line){return line.find(text)!=std::string::npos;});}
    size_t size(){std::lock_guard lock(mutex);return messages.size();}
    void idle(){
        const auto end=std::chrono::steady_clock::now()+4s;
        while(!native::diagnostics::writer().idle()&&std::chrono::steady_clock::now()<end)std::this_thread::sleep_for(1ms);
        assert(native::diagnostics::writer().idle());
    }
};
Logs logs;
size_t commitCalls{},purchaseCalls{},recomposeCalls{},validationCalls{},mutations{};
bool badCoupler{},alterTemporary{},throwNative{};
std::vector<uint8_t> couplerFlags;
std::array<uint8_t,0x40> validationResult{};
uint64_t address(const void* value){return reinterpret_cast<uint64_t>(value);}
void fakeCommit(uint64_t editor,uint64_t,uint64_t){
    ++commitCalls;if(native::currentRequest)logs.awaitPreflight(native::currentRequest);
    memory->put(editor+0x15d0,uint8_t{});++mutations;
    if(throwNative)throw std::runtime_error("Fake original commit failed after mutation");
}
uint64_t fakeValidate(uint64_t train,uint64_t output,uint64_t,uint8_t){
    ++validationCalls;
    const native::View view(native::read);std::vector<native::Car> cars;assert(view.vector(train+0xc0,cars));
    couplerFlags.assign(cars.size()*2,1);if(badCoupler&&!couplerFlags.empty())couplerFlags.back()=0;
    const uint32_t code=cars.size()>30?1u:badCoupler?4u:0u;
    std::memcpy(reinterpret_cast<void*>(output),&code,sizeof code);
    const std::array<uint64_t,3> range{address(couplerFlags.data()),address(couplerFlags.data())+couplerFlags.size(),address(couplerFlags.data())+couplerFlags.size()};
    std::memcpy(reinterpret_cast<void*>(output+8),range.data(),sizeof range);
    return output;
}
uint64_t fakePurchase(uint64_t,uint64_t output,uint64_t,uint64_t){
    ++purchaseCalls;if(native::currentRequest)logs.awaitPreflight(native::currentRequest);
    const uint32_t code=0;std::memcpy(reinterpret_cast<void*>(output),&code,sizeof code);++mutations;
    if(throwNative)throw std::runtime_error("Fake original purchase failed after mutation");
    return output;
}
uint64_t fakeRecompose(uint64_t input,uint64_t output,uint64_t,uint64_t){
    ++recomposeCalls;if(native::currentRequest)logs.awaitPreflight(native::currentRequest);
    std::vector<native::Car> cars;assert(native::View(native::read).vector(input+0x28,cars));
    if(alterTemporary)cars[0].settings[15]^=1;
    std::array<uint8_t,0x300> temporary{};
    const std::array<uint64_t,3> range{address(cars.data()),address(cars.data())+cars.size()*32,address(cars.data())+cars.size()*32};
    std::memcpy(temporary.data()+0xc0,range.data(),sizeof range);
    validationResult.fill(0x5a);
    const auto result=native::validate(address(temporary.data()),address(validationResult.data()),Memory::rules,0);
    assert(result==address(validationResult.data()));uint32_t code{};std::memcpy(&code,validationResult.data(),sizeof code);
    std::memcpy(reinterpret_cast<void*>(output+0x18),&code,sizeof code);
    if(!code){memory->record(0,cars);++mutations;}
    if(throwNative)throw std::runtime_error("Fake original recompose failed after mutation");
    return output;
}
void fakeRemove(uint64_t editor,uint64_t context,uint64_t rules){
    fakeCommit(editor,context,rules);
    std::vector<native::Car> cars;assert(native::View(native::read).vector(editor+0xc0,cars));
    assert(!cars.empty());cars.pop_back();
    memory->vector(editor+0xc0,Memory::carStorage+0x20000,cars);
    validationResult.fill(0x5a);
    native::validate(editor,address(validationResult.data()),rules,0);
}
void fakeRemoveAfterValidation(uint64_t editor,uint64_t context,uint64_t rules){
    native::validate(editor,address(validationResult.data()),rules,0);
    fakeCommit(editor,context,rules);
    std::vector<native::Car> cars;assert(native::View(native::read).vector(editor+0xc0,cars));
    assert(!cars.empty());cars.pop_back();
    memory->vector(editor+0xc0,Memory::carStorage+0x20000,cars);
}
void fakeAppend(uint64_t editor,uint64_t context,uint64_t rules){
    fakeCommit(editor,context,rules);
    std::vector<native::Car> cars;assert(native::View(native::read).vector(editor+0xc0,cars));
    const auto car=memory->get<native::Car>(editor+0x15e0);
    const auto count=memory->get<int32_t>(editor+0x1600);
    for(int32_t i=0;i<count;++i)cars.push_back(car);
    memory->vector(editor+0xc0,Memory::carStorage+0x20000,cars);
}
void editor_checks(){
    Memory m;memory=&m;
    m.edit({{2}},1,19);const auto before=m.data;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==0&&mutations==0&&m.data==before);
    assert(native::lastFailure.assessment.totalLengthMeters==875&&logs.contains("candidateMeters=875.000000"));
    assert(logs.contains("requestedCars=19")&&logs.contains("decision=length-limit-exceeded"));
    m.edit({{2},{3}},1,1); // 850 m exact.
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==1&&mutations==1&&m.get<uint8_t>(Memory::editor+0x15d0)==0);
    assert(native::lastFailure.editor==0&&logs.contains("decision=allowed candidateMeters=850.000000"));
    m.edit({{2},{3},{1}},4,1);const auto over=m.data;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==1&&mutations==1&&m.data==over);
    m.edit({{2}},99,1);const auto unknown=m.data;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==1&&m.data==unknown&&logs.contains("decision=unknown-model")&&logs.contains("readFault=model-not-found"));
    m.edit({{2}},1,1);m.failAt=Memory::editor+0x1600;const auto unreadable=m.data;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==1&&m.data==unreadable&&logs.contains("readFault=memory-read-failed"));
    // Deletion takes priority over a stale append flag and never touches an
    // unreadable/oversized car vector. A dirty-free frame produces no trace.
    m.failAt=0;m.edit({{5},{5}},99,100);
    auto flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[0]=1;m.put(Memory::editor+0x15c0,flags);
    m.failAt=Memory::editor+0xc0;auto reads=m.reads;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==2&&m.reads-reads==4&&logs.contains("phase=delegate-unchanged"));
    m.edit({{5},{5}},99,0);reads=m.reads;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==3&&m.reads-reads==5);
    const auto logCount=logs.size();reads=m.reads;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==4&&m.reads-reads==1&&logs.size()==logCount);
}
void purchase_checks(){
    Memory m;memory=&m;std::array<uint8_t,0x210> output{};output.fill(0xa5);
    m.proposal({{5},{5}},true);const auto before=m.data;const auto prior=mutations;
    assert(native::purchase(Memory::command,address(output.data()),Memory::context,Memory::simulation)==address(output.data()));logs.idle();
    uint32_t code{};std::memcpy(&code,output.data(),4);
    assert(code==2&&output[0x10]==1&&output[0x1f0]==4&&purchaseCalls==0&&mutations==prior&&m.data==before);
    for(size_t i=0x20;i<output.size();++i)if(i!=0x1f0)assert(output[i]==0xa5);
    m.proposal({{2},{3},{1}},true);
    native::purchase(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    assert(purchaseCalls==1&&mutations==prior+1);
}
void recompose_checks(){
    Memory m;memory=&m;std::array<uint8_t,0x210> output{};output.fill(0xa5);
    m.proposal({{5},{5}},false);const auto before=m.data;const auto prior=mutations;
    native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    uint32_t code{};std::memcpy(&code,output.data()+0x18,4);
    assert(code==2&&output[0x1c]==1&&output[0x1f0]==4&&recomposeCalls==0&&mutations==prior&&m.data==before);
    for(size_t i=0x20;i<output.size();++i)if(i!=0x1f0)assert(output[i]==0xa5);
    // Reducing an existing oversized physical train is accepted at both the
    // outer command and its ORIGINAL's private temporary validation boundary.
    m.record(0,{{6},{6}});m.proposal({{5},{5}},false);
    native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    std::memcpy(&code,output.data()+0x18,4);
    assert(code==0&&recomposeCalls==1&&validationCalls==1&&mutations==prior+1&&native::approvedRecomposition==nullptr);
    assert(logs.contains("ownBeforeMeters=1200.000000 ownCandidateMeters=1000.000000"));
    m.record(0,{{6},{6}});badCoupler=true;const auto coupled=m.data;
    native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    std::memcpy(&code,output.data()+0x18,4);
    assert(code==4&&recomposeCalls==2&&validationCalls==2&&mutations==prior+1&&m.data==coupled&&native::approvedRecomposition==nullptr);
    badCoupler=false;alterTemporary=true;const auto altered=m.data;
    native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    std::memcpy(&code,output.data()+0x18,4);
    assert(code==1&&recomposeCalls==3&&validationCalls==3&&mutations==prior+1&&m.data==altered&&native::approvedRecomposition==nullptr);
    alterTemporary=false;
    // The old count-only refusal is recoverable for an accepted 31-car train,
    // including the incompatibility flag that native code1 otherwise masks.
    m.record(0,{{2}});m.proposal(std::vector<native::Car>(31,{1}),false);
    native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    std::memcpy(&code,output.data()+0x18,4);
    assert(code==0&&recomposeCalls==4&&validationCalls==4&&mutations==prior+2);
    std::array<uint64_t,3> retainedFlags{};std::memcpy(retainedFlags.data(),validationResult.data()+8,sizeof retainedFlags);
    assert(retainedFlags[1]-retainedFlags[0]==62);
    for(size_t i=32;i<validationResult.size();++i)assert(validationResult[i]==0x5a);
    m.record(0,{{2}});badCoupler=true;const auto countCoupler=m.data;
    native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);logs.idle();
    std::memcpy(&code,output.data()+0x18,4);
    assert(code==4&&recomposeCalls==5&&validationCalls==5&&mutations==prior+2&&m.data==countCoupler);
    badCoupler=false;
}
void native_exceptions_remain_native(){
    Memory m;memory=&m;throwNative=true;
    m.edit({{2}},1,1);const auto before=mutations;
    bool thrown=false;
    try{native::commit(Memory::editor,Memory::context,Memory::rules);}catch(const std::runtime_error&){thrown=true;}
    logs.idle();assert(thrown&&mutations==before+1&&m.get<uint8_t>(Memory::editor+0x15d0)==0);
    assert(logs.contains("action=editor phase=native-exception")&&native::currentRequest==0);
    std::array<uint8_t,0x210> output{};output.fill(0xa5);m.proposal({{2}},true);thrown=false;
    try{native::purchase(Memory::command,address(output.data()),Memory::context,Memory::simulation);}catch(const std::runtime_error&){thrown=true;}
    logs.idle();uint32_t code{};std::memcpy(&code,output.data(),4);
    assert(thrown&&code==0&&mutations==before+2&&output[0x1f0]==0xa5);
    assert(logs.contains("action=purchase phase=native-exception")&&native::currentRequest==0);
    m.proposal({{2},{1}},false);thrown=false;
    try{native::recompose(Memory::command,address(output.data()),Memory::context,Memory::simulation);}catch(const std::runtime_error&){thrown=true;}
    logs.idle();std::memcpy(&code,output.data()+0x18,4);
    assert(thrown&&code==0&&mutations==before+3&&output[0x1f0]==0xa5);
    assert(logs.contains("action=recompose phase=native-exception")&&native::currentRequest==0&&native::approvedRecomposition==nullptr);
    assert(logs.contains("mutationState=not-rolled-back"));
    throwNative=false;
}
void removal_refreshes_refusal(){
    Memory m;memory=&m;
    const auto previousEditor=native::currentEditor;native::currentEditor=Memory::editor;
    struct RestoreEditor{uint64_t previous;~RestoreEditor(){native::currentEditor=previous;}} restoreEditor{previousEditor};
    const auto previousCommit=native::originalCommit;
    struct RestoreCommit{native::Commit previous;~RestoreCommit(){native::originalCommit=previous;}} restoreCommit{previousCommit};
    m.edit({{2},{3},{1}},1,1); // Existing 850 m, proposed addition 875 m.
    const auto beforeCalls=commitCalls;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==beforeCalls&&native::lastFailure.editor==Memory::editor);
    assert(native::lastFailure.assessment.totalLengthMeters==875);

    // Validating the unchanged 850 m train must NOT erase a rejected addition.
    native::validate(Memory::editor,address(validationResult.data()),Memory::rules,0);
    assert(native::lastFailure.editor==Memory::editor&&native::lastFailure.assessment.totalLengthMeters==875);
    auto flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[0]=1;
    m.put(Memory::editor+0x15c0,flags);m.put(Memory::editor+0x1138,uint64_t{1});
    native::originalCommit=&fakeRemove;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==beforeCalls+1);
    const auto after=native::composition(native::View(native::read),Memory::editor+0xc0,Memory::rules,{850});
    assert(after.allowed()&&after.totalLengthMeters==825);
    uint32_t code{};std::memcpy(&code,validationResult.data(),sizeof code);assert(code==0);
    assert(native::lastFailure.editor==0); // The old 875 m notice must disappear immediately.

    native::originalCommit=&fakeCommit;
    // Re-arm an addition against the vector actually shortened by fakeRemove.
    flags.fill(0);flags[4]=1;m.put(Memory::editor+0x15c0,flags);
    m.put(Memory::editor+0x15d0,uint8_t{1});m.put(Memory::editor+0x1138,uint64_t{});
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(commitCalls==beforeCalls+2&&native::lastFailure.editor==0); // 825 + 25 = 850 m.

    // A new verification failure raised INSIDE the original removal survives.
    m.edit({{99},{1}},1,1);
    flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[0]=1;
    m.put(Memory::editor+0x15c0,flags);m.put(Memory::editor+0x1138,uint64_t{1});
    policy::Assessment old;old.decision=policy::Decision::LimitExceeded;old.totalLengthMeters=875;old.maximumMeters=850;
    native::report(Memory::editor,old);native::originalCommit=&fakeRemove;
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(native::lastFailure.editor==Memory::editor&&native::lastFailure.assessment.decision==policy::Decision::UnknownModel);
    assert(native::lastFailure.assessment.problematicModel==99);

    // Removing here must not erase another editor's refusal.
    m.edit({{2},{1}},1,1);
    flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[0]=1;
    m.put(Memory::editor+0x15c0,flags);m.put(Memory::editor+0x1138,uint64_t{1});
    native::report(Memory::editor+0x8000,old);
    native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
    assert(native::lastFailure.editor==Memory::editor+0x8000);

    // Frames without an action, layout/orientation and cosmetic edits keep
    // the rejected-addition notice, even with a stale deletion flag present.
    native::originalCommit=&fakeCommit;
    const auto retained=[&](uint8_t dirty,int priorityFlag,int cosmetic){
        m.edit({{2},{3},{1}},1,1);native::report(Memory::editor,old);
        m.put(Memory::editor+0x15d0,dirty);m.put(Memory::editor+0x1138,uint64_t{1});
        auto action=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);action[0]=1;
        if(priorityFlag>=0)action[priorityFlag]=1;
        m.put(Memory::editor+0x15c0,action);
        if(cosmetic>=0){std::array<uint8_t,7> appearance{};appearance[cosmetic]=1;m.put(Memory::editor+0x1598,appearance);}
        native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
        assert(native::lastFailure.editor==Memory::editor&&native::lastFailure.assessment.totalLengthMeters==875);
    };
    retained(0,-1,-1);retained(1,12,-1);retained(1,13,-1);
    for(int cosmetic=0;cosmetic<7;++cosmetic)retained(1,-1,cosmetic);
    native::lastFailure={};
}
void live_length_tracks_actual_composition(){
    Memory m;memory=&m;
    const auto previousEditor=native::currentEditor;native::currentEditor=Memory::editor;
    struct RestoreEditor{uint64_t value;~RestoreEditor(){native::currentEditor=value;}} restoreEditor{previousEditor};
    const auto previousCommit=native::originalCommit;
    struct RestoreCommit{native::Commit value;~RestoreCommit(){native::originalCommit=value;}} restoreCommit{previousCommit};
    const auto messages=std::make_shared<const native::EditorMessages>(
        R"({"messages":["Addition refused","Missing vehicle length","Cannot verify"],"translations":"","lengthDisplay":["Current length","Proposed length","Current length unavailable"]})");
    assert(native::registry.replace(1,850,messages)==policy::PublishResult::Applied);
    native::currentLengthDisplay={};native::lastFailure={};
    const auto layout=[&](const native::PanelMeasurement& frame){
        assert(native::prepareCurrentLength(Memory::editor,Memory::rules,native::registry.selected(),frame,true,"en"));
        return std::string(native::currentLengthDisplay.line.data());
    };
    m.edit({{2},{3},{1}},1,1);
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        native::validate(Memory::editor,address(validationResult.data()),Memory::rules,0);
        const auto reads=m.reads;
        assert(layout(frame.frame)=="Current length: 850.000 m / 850 m");
        assert(m.reads==reads); // Reuse production validation, no second model scan.
        const auto calls=commitCalls;
        native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
        assert(commitCalls==calls&&native::lastFailure.assessment.totalLengthMeters==875);
        assert(layout(frame.frame)=="Current length: 850.000 m / 850 m"); // Rejected candidate is separate.
        auto flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[0]=1;
        m.put(Memory::editor+0x15c0,flags);m.put(Memory::editor+0x1138,uint64_t{1});
        native::originalCommit=&fakeRemoveAfterValidation;
        native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
        assert(!frame.frame.value); // A pre-mutation native validation cannot become the new current length.
        assert(layout(frame.frame)=="Current length: 825.000 m / 850 m");
        assert(native::lastFailure.editor==0);
    }
    const auto unchangedHeader=m.get<std::array<uint64_t,3>>(Memory::editor+0xc0);
    m.put(unchangedHeader[0],native::Car{1}); // Replace model in place: same pointer, count and capacity.
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        const auto reads=m.reads;
        assert(native::prepareCurrentLength(Memory::editor,Memory::rules,native::registry.selected(),frame.frame,false,"en"));
        assert(std::string(native::currentLengthDisplay.line.data())=="Current length: 825.000 m / 850 m");
        assert(m.reads==reads); // Paint the layout snapshot; next layout will show the edit.
        assert(layout(frame.frame)=="Current length: 450.000 m / 850 m");
        assert((m.get<std::array<uint64_t,3>>(Memory::editor+0xc0)==unchangedHeader));
    }
    m.edit({{1}},2,1);native::originalCommit=&fakeAppend;
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        native::commit(Memory::editor,Memory::context,Memory::rules);logs.idle();
        assert(layout(frame.frame)=="Current length: 425.000 m / 850 m"); // Accepted addition, actual mutated vector.
        assert(native::registry.update(1,1000)==policy::PublishResult::Applied);
        assert(layout(frame.frame)=="Current length: 425.000 m / 1000 m");
    }
    m.edit({},1,1);
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        assert(layout(frame.frame)=="Current length: 0.000 m / 1000 m");
    }
    m.edit({{1},{99}},1,1);
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        native::validate(Memory::editor,address(validationResult.data()),Memory::rules,0);
        assert(frame.frame.value&&frame.frame.value->totalLengthMeters==25);
        assert(layout(frame.frame)=="Current length unavailable"); // Never present a partial sum as current.
    }
    m.failAt=Memory::editor+0xc0;
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        assert(layout(frame.frame)=="Current length unavailable");
    }
    m.failAt=0;
    assert(native::registry.replace(1,850,native::EditorMessages::legacy())==policy::PublishResult::Applied);
    {
        native::ScopedPanelMeasurement frame(Memory::editor,Memory::rules);
        const auto reads=m.reads;
        assert(!native::prepareCurrentLength(Memory::editor,Memory::rules,native::registry.selected(),frame.frame,true,"en"));
        assert(m.reads==reads&&!native::currentLengthDisplay.initialized); // Legacy opt-out adds no measurement.
    }
    native::lastFailure={};logs.idle();
}
}
namespace {
struct MachineCapture {
    uint64_t count{},root{},child{};
    std::array<std::array<uint8_t,16>,6> input{},output{};
    std::array<uint64_t,11> registers{};
    uint64_t flags{},beforeStack{};
};
struct NativeMachine {
    uint8_t* memory{};RUNTIME_FUNCTION parent{};bool registered=false;
    static constexpr size_t accepted=0x500,rejected=0x520,decision=0x600,unwind=0x700;
    NativeMachine(){
        memory=static_cast<uint8_t*>(::VirtualAlloc(nullptr,0x500000,MEM_RESERVE,PAGE_NOACCESS));assert(memory);
        assert(::VirtualAlloc(memory,4096,MEM_COMMIT,PAGE_READWRITE)==memory);
        constexpr uint8_t info[]{1,42,12,0,42,0x68,8,0,38,1,19,0,27,0xf0,25,0xe0,23,0xd0,21,0xc0,19,0x70,18,0x60,17,0x30,16,0x50};
        std::memcpy(memory+unwind,info,sizeof info);
        parent={0x100,0x540,unwind};assert(RtlAddFunctionTable(&parent,1,reinterpret_cast<DWORD64>(memory)));registered=true;
    }
    ~NativeMachine(){if(registered)assert(RtlDeleteFunctionTable(&parent));if(memory)assert(VirtualFree(memory,0,MEM_RELEASE));}
    uint64_t address(size_t offset=0)const{return reinterpret_cast<uint64_t>(memory)+offset;}
    void bind(size_t offset,const void* function){
        auto* target=memory+offset;auto* page=memory+(offset&~size_t{4095});assert(::VirtualAlloc(page,4096,MEM_COMMIT,PAGE_READWRITE)==page);
        constexpr uint8_t jump[]{0xff,0x25,0,0,0,0};std::memcpy(target,jump,6);
        const auto value=reinterpret_cast<uint64_t>(function);std::memcpy(target+6,&value,8);
        DWORD previous{};assert(VirtualProtect(page,4096,PAGE_EXECUTE_READ,&previous));assert(FlushInstructionCache(GetCurrentProcess(),target,14));
    }
    void caller(void* gate){
        DWORD old{};assert(VirtualProtect(memory,4096,PAGE_READWRITE,&old));
        std::vector<uint8_t> code;
        const auto emit=[&](std::initializer_list<uint8_t> bytes){code.insert(code.end(),bytes);};
        const auto imm=[&](uint64_t value,unsigned bytes=8){for(unsigned i=0;i<bytes;++i)code.push_back(uint8_t(value>>(i*8)));};
        // The same native frame shape as44CF80:8saved nonvolatiles,152bytes,
        // XMM6 atRSP+128. Enter the gateway by JMP, never an artificial CALL.
        emit({0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,
              0x48,0x81,0xec,0x98,0,0,0,0x0f,0x29,0xb4,0x24,0x80,0,0,0,
              0x48,0x89,0x4c,0x24,0x20});
        for(uint8_t i=0;i<6;++i){emit({0xf3,0x0f,0x6f,uint8_t(0x81+i*8)});imm(offsetof(MachineCapture,input)+i*16,4);}
        emit({0x48,0x8b,0x59,0x08,0x48,0x8d,0x5b,0x08,0x4c,0x8b,0x69,0x10,
              0x48,0x89,0xa1});imm(offsetof(MachineCapture,beforeStack),4);
        emit({0x48,0x8b,0x09,0x48,0xb8});imm(0x1122334455667788);
        emit({0x48,0xba});imm(0x8877665544332211);
        for(uint8_t i=0;i<4;++i){emit({0x49,uint8_t(0xb8+i)});imm(0x8192837465000000ULL+i);}
        emit({0x49,0xbc});imm(reinterpret_cast<uint64_t>(gate));emit({0x41,0xff,0xe4});
        const uint64_t continuation=address(0x100+code.size());
        emit({0x4c,0x89,0x64,0x24,0x28,0x4c,0x8b,0x64,0x24,0x20});
        const std::array<std::pair<uint8_t,uint8_t>,10> stores{{{0x49,0x84},{0x49,0x8c},{0x49,0x94},{0x4d,0x84},{0x4d,0x8c},{0x4d,0x94},{0x4d,0x9c},{0x49,0x9c},{0x4d,0xac},{0x49,0xa4}}};
        for(size_t i=0;i<stores.size();++i){emit({stores[i].first,0x89,stores[i].second,0x24});imm(offsetof(MachineCapture,registers)+i*8,4);}
        emit({0x48,0x8b,0x44,0x24,0x28,0x49,0x89,0x84,0x24});imm(offsetof(MachineCapture,registers)+10*8,4);
        emit({0x9c,0x58,0x49,0x89,0x84,0x24});imm(offsetof(MachineCapture,flags),4);
        for(uint8_t i=0;i<6;++i){emit({0xf3,0x41,0x0f,0x7f,uint8_t(0x84+i*8),0x24});imm(offsetof(MachineCapture,output)+i*16,4);}
        emit({0x0f,0x28,0xb4,0x24,0x80,0,0,0,0x48,0x81,0xc4,0x98,0,0,0,
              0x41,0x5f,0x41,0x5e,0x41,0x5d,0x41,0x5c,0x5f,0x5e,0x5b,0x5d,0xc3});
        assert(code.size()<accepted-0x100);std::memcpy(memory+0x100,code.data(),code.size());
        for(size_t i=0;i<2;++i){
            auto* stub=memory+(i?rejected:accepted);
            const int32_t relative=static_cast<int32_t>(decision-(i?rejected:accepted)-10);
            constexpr uint8_t store[]{0xc7,0x05};std::memcpy(stub,store,2);std::memcpy(stub+2,&relative,4);
            const uint32_t branch=uint32_t(i+1);std::memcpy(stub+6,&branch,4);
            constexpr uint8_t jump[]{0xff,0x25,0,0,0,0};std::memcpy(stub+10,jump,6);std::memcpy(stub+16,&continuation,8);
        }
        // Decision is test-owned in this same page; writable data must live
        // in another committed page while machinecode remains RX.
        assert(::VirtualAlloc(memory+4096,4096,MEM_COMMIT,PAGE_READWRITE)==memory+4096);
        for(size_t i=0;i<2;++i){const int32_t relative=static_cast<int32_t>(4096-(i?rejected:accepted)-10);std::memcpy(memory+(i?rejected:accepted)+2,&relative,4);}
        assert(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&old));assert(FlushInstructionCache(GetCurrentProcess(),memory,4096));
    }
    uint32_t run(MachineCapture& capture){reinterpret_cast<void(*)(MachineCapture*)>(memory+0x100)(&capture);uint32_t value{};std::memcpy(&value,memory+4096,4);return value;}
};
uint64_t cmpFlags(uint64_t a,uint64_t b){
    const uint64_t difference=a-b;
    return uint64_t(a<b)|uint64_t(std::popcount(uint8_t(difference))%2==0)*4|
        ((a^b^difference)&16)|uint64_t(!difference)*64|(difference&(uint64_t{1}<<63)?128:0)|
        ((((a^b)&(a^difference))>>63)*0x800);
}
void gateway_unwind(native::AggregateCountGateway& gate,const NativeMachine& machine){
    alignas(16) std::array<uint64_t,96> stack{};auto* rsp=stack.data()+8;
    constexpr uint64_t returnAddress=0x123456789abcdef0;
    for(size_t i=0;i<8;++i)rsp[19+i]=0x100+i;
    rsp[27]=returnAddress;
    constexpr M128A xmm{0x1122334455667788,0x3344556677889900};std::memcpy(reinterpret_cast<uint8_t*>(rsp)+128,&xmm,16);
    constexpr size_t boundaries[]{0,4,9,19,24,34,39,48,55,58,64,68,71,77,81,84,90,94,97,103,107,110,116,120,125,129,133,138,140,144};
    for(const size_t offset:boundaries){
        assert(offset<gate.codeBytes());
        DWORD64 module{};const auto rf=RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(gate.address())+offset,&module,nullptr);assert(rf&&module==machine.address());
        CONTEXT context{};context.ContextFlags=CONTEXT_FULL;context.Rip=reinterpret_cast<DWORD64>(gate.address())+offset;context.Rsp=reinterpret_cast<DWORD64>(rsp);
        PVOID data{};DWORD64 frame{};RtlVirtualUnwind(UNW_FLAG_NHANDLER,module,context.Rip,rf,&context,&data,&frame,nullptr);
        if(context.Rip!=returnAddress||context.Rsp!=reinterpret_cast<DWORD64>(rsp+28))std::cerr<<"unwind offset="<<offset<<" rip="<<std::hex<<context.Rip<<" rspDelta="<<std::dec<<(context.Rsp-reinterpret_cast<DWORD64>(rsp))<<'\n';
        assert(context.Rip==returnAddress&&context.Rsp==reinterpret_cast<DWORD64>(rsp+28));
        assert(context.R15==0x100&&context.R14==0x101&&context.R13==0x102&&context.R12==0x103&&context.Rdi==0x104&&context.Rsi==0x105&&context.Rbx==0x106&&context.Rbp==0x107);
        assert(!std::memcmp(&context.Xmm6,&xmm,sizeof xmm));
    }
}
void aggregate_gateway_checks(){
    memory=nullptr;NativeMachine machine;const DWORD slot=TlsAlloc();assert(slot<64);
    native::AggregateCountGateway gate;assert(!gate.build(machine.address(NativeMachine::accepted),machine.address(NativeMachine::rejected),64,machine.address()));
    failVirtualAllocation=true;assert(!gate.build(machine.address(NativeMachine::accepted),machine.address(NativeMachine::rejected),slot,machine.address()));assert(!gate.address());failVirtualAllocation=false;
    assert(gate.build(machine.address(NativeMachine::accepted),machine.address(NativeMachine::rejected),slot,machine.address()));
    assert(gate.build(machine.address(NativeMachine::accepted),machine.address(NativeMachine::rejected),slot,machine.address()));
    machine.caller(gate.address());gateway_unwind(gate,machine);
    native::AggregateFrame frame{0x200000,2,{}};frame.prefixes[0]={0x200638,4096-100};frame.prefixes[1]={0x200c70,4096};
    for(const int mode:{0,1,2,3})for(const uint64_t count:{uint64_t{0},uint64_t{30},uint64_t{31},uint64_t{80},uint64_t{3996},uint64_t{3997},uint64_t{4096},uint64_t{4097},UINT64_MAX}){
        MachineCapture value;value.count=count;value.root=mode==3?frame.object+1:frame.object;value.child=mode==2?frame.prefixes[1].motion:frame.prefixes[0].motion;
        for(size_t i=0;i<6;++i)for(size_t j=0;j<16;++j)value.input[i][j]=uint8_t(1+i*19+j*7);
        NtCurrentTeb()->TlsSlots[slot]=mode?&frame:nullptr;const auto limit=mode==1?frame.prefixes[0].maximum:mode==2?frame.prefixes[1].maximum:30;
        const auto allocations=allocationCount;watchAllocation=true;const auto branch=machine.run(value);watchAllocation=false;
        assert(allocationCount==allocations&&branch==(count<=limit?1u:2u)&&value.output==value.input);
        assert((value.flags&0x8d5)==cmpFlags(count,limit));
        assert((value.registers==std::array<uint64_t,11>{0x1122334455667788,count,0x8877665544332211,
            0x8192837465000000,0x8192837465000001,0x8192837465000002,0x8192837465000003,value.root+8,value.child,value.beforeStack,reinterpret_cast<uint64_t>(gate.address())}));
    }
    NtCurrentTeb()->TlsSlots[slot]=nullptr;assert(gate.reset());assert(!gate.address());assert(TlsFree(slot));
}
size_t aggregateCalls{},aggregateDepth{};bool aggregateThrows{},aggregateNested{};std::vector<native::Car> aggregated;
void fakeAggregate(uint64_t pool,uint64_t database,uint64_t object){
    ++aggregateCalls;auto* frame=static_cast<native::AggregateFrame*>(NtCurrentTeb()->TlsSlots[native::aggregateTls]);assert(!frame||frame->object==object);
    assert(pool==Memory::simulation+0xa0&&database==Memory::database);
    if(aggregateThrows)throw std::bad_alloc();
    if(aggregateNested&&!aggregateDepth){
        ++aggregateDepth;native::aggregate(pool,database,object);--aggregateDepth;
        assert(NtCurrentTeb()->TlsSlots[native::aggregateTls]==frame);
    }
    const native::View view(native::read);std::vector<native::Car> planned,children;assert(view.vector(Memory::train(0)+0xc0,planned));
    struct Item{int64_t order;uint64_t address,id;};std::vector<Item> order{{0,object,Memory::id(0)}};
    assert(view.vector(object+0x88,children));for(const auto& child:children){const auto ordinal=(child.model>>16)&0xffffffff;const auto at=Memory::motion(ordinal);order.push_back({memory->get<int64_t>(at+0x1e0)*1000000,at,child.model});}
    std::stable_sort(order.begin(),order.end(),[](const Item& a,const Item& b){return a.order<b.order;});aggregated.clear();
    for(const auto& item:order){
        if(item.address==object){int32_t mode=std::clamp(memory->get<int32_t>(object+0x80),0,8);for(size_t i=0;i<planned.size();++i)if(!mode||(planned[i].settings[15]&(1u<<(mode-1)))){
            auto car=planned[i];car.train=Memory::id(0);const uint16_t ordinal=uint16_t(i);std::memcpy(car.settings.data(),&ordinal,2);aggregated.push_back(car);}}
        else{std::vector<native::Car> cars;assert(view.vector(item.address+8,cars));size_t maximum=30;
            if(frame){auto* found=std::find_if(frame->prefixes.begin(),frame->prefixes.begin()+frame->count,[&](auto prefix){return prefix.motion==item.address;});assert(found!=frame->prefixes.begin()+frame->count);maximum=found->maximum;}
            if(aggregated.size()+cars.size()>maximum)continue;
            if(memory->get<uint8_t>(item.address+0x1e8)){std::reverse(cars.begin(),cars.end());for(auto& car:cars)car.settings[12]=!car.settings[12];}
            aggregated.insert(aggregated.end(),cars.begin(),cars.end());}
    }
}
void physicalGroup(Memory& m,size_t rootCars,size_t firstCars,size_t secondCars=0){
    const auto cars=[](size_t count,uint64_t owner){std::vector<native::Car> result(count,native::Car{1,owner});for(size_t i=0;i<count;++i){uint16_t ordinal=uint16_t(i);std::memcpy(result[i].settings.data(),&ordinal,2);}return result;};
    m.record(0,{});m.put(Memory::motion(0)+0x80,int32_t{});m.vector(Memory::train(0)+0xc0,0x400000,cars(rootCars,Memory::id(0)));m.vector(Memory::motion(0)+8,0x500000,{});
    std::vector<native::Car> children;
    for(size_t i=1;i<=2;++i){const auto count=i==1?firstCars:secondCars;if(!count)continue;
        m.record(i,{});m.put(Memory::motion(i)+0x1d8,Memory::id(0));m.put(Memory::motion(i)+0x1f0,uint8_t{1});m.put(Memory::motion(i)+0x1e0,int64_t(i==1?-1:1));m.put(Memory::motion(i)+0x1e8,uint8_t(i==1));
        m.vector(Memory::motion(i)+8,0x520000+i*0x20000,cars(count,Memory::id(i)));children.push_back({Memory::id(i)});
    }
    m.vector(Memory::motion(0)+0x88,0x5a0000,children);
}
void aggregate_admission_checks(){
    Memory m;memory=&m;physicalGroup(m,1,30);native::originalAggregate=&fakeAggregate;
    const auto slot=TlsAlloc();assert(slot<64);native::aggregateTls=slot;
    native::mechanicsReady=false;native::hashSafetyResident=false;
    auto calls=aggregateCalls;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls);
    native::mechanicsReady=true;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls);
    native::hashSafetyResident=true;
    for(const auto count:{size_t{31},size_t{80},size_t{512},size_t{4096}}){
        physicalGroup(m,1,count-2,1);native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==++calls&&aggregated.size()==count);
        assert(aggregated.front().train==Memory::id(1)&&aggregated[count-2].train==Memory::id(0)&&aggregated.back().train==Memory::id(2));
        uint16_t ordinal{};std::memcpy(&ordinal,aggregated.front().settings.data(),2);assert(ordinal==count-3&&aggregated.front().settings[12]==1);
        assert(!NtCurrentTeb()->TlsSlots[slot]);
    }
    physicalGroup(m,4096,1);const auto before=m.data;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls&&m.data==before);
    physicalGroup(m,2,79);m.put(Memory::motion(0)+0x80,int32_t{1}); // selected root mask absent
    native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==++calls&&aggregated.size()==79);
    physicalGroup(m,1,79);auto reads=m.reads;
    const auto allocations=allocationCount;watchAllocation=true;const auto admitted=native::aggregation::admit(native::View(native::read),Memory::simulation+0xa0,Memory::database,Memory::motion(0));watchAllocation=false;
    assert(admitted.allowed&&admitted.totalElements==80&&allocationCount==allocations);const auto readCount=m.reads-reads;
    for(size_t i=1;i<=readCount;++i){m.failRead=m.reads+i;const auto unchanged=m.data;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls&&m.data==unchanged&&!NtCurrentTeb()->TlsSlots[slot]);m.failRead=0;}
    m.failAt=0x540000;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls);m.failAt=0;
    m.put(Memory::motion(1),Memory::id(1)^1);native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls);m.put(Memory::motion(1),Memory::id(1));
    m.put(Memory::motion(1)+0x1d8,Memory::id(2));native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls);m.put(Memory::motion(1)+0x1d8,Memory::id(0));
    // A readback mutation, including a selected-car byte, never reaches native.
    for(const auto at:{Memory::motion(1),Memory::motion(1)+0x1d8,Memory::train(0)+0xc0,uint64_t{0x400018}}){
        physicalGroup(m,1,79);m.mutateAt=at;m.mutateRead=m.reads+20;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls);m.mutateAt=0;
    }
    physicalGroup(m,1,79);native::AggregateFrame outer{0xabcdef,0,{}};NtCurrentTeb()->TlsSlots[slot]=&outer;aggregateThrows=true;
    try{native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(false);}catch(const std::bad_alloc&){}
    assert(aggregateCalls==++calls&&NtCurrentTeb()->TlsSlots[slot]==&outer);aggregateThrows=false;NtCurrentTeb()->TlsSlots[slot]=nullptr;
    aggregateNested=true;native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));calls+=2;
    assert(aggregateCalls==calls&&!NtCurrentTeb()->TlsSlots[slot]);aggregateNested=false;
    physicalGroup(m,80,0);m.put(Memory::motion(0)+0x1d8,Memory::id(2));m.put(Memory::motion(0)+0x1f0,uint8_t{1});
    native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==++calls&&aggregated.size()==80&&!NtCurrentTeb()->TlsSlots[slot]);
    physicalGroup(m,1,79);m.put(Memory::motion(0)+0x1d8,Memory::id(2));m.put(Memory::motion(0)+0x1f0,uint8_t{1});
    native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==++calls&&aggregated.size()==1); // nested append retains CMP30
    physicalGroup(m,4096,1);m.put(Memory::motion(0)+0x1d8,Memory::id(2));m.put(Memory::motion(0)+0x1f0,uint8_t{1});
    native::aggregate(Memory::simulation+0xa0,Memory::database,Memory::motion(0));assert(aggregateCalls==calls); // fallback never bypasses total bound
    assert(TlsFree(slot));native::aggregateTls=TLS_OUT_OF_INDEXES;native::mechanicsReady=false;native::hashSafetyResident=false;
}
size_t dynamicsCalls{},hashCalls{},hashBytes{},hashDepth{};
uint64_t hashFirst{},hashLast{},hashValue{};const void* nativeHashBuffer{};
bool dynamicsThrows{},hashOom{},dynamicsNested{};
uint64_t fakeLeaf(const void* input,uint64_t bytes,uint64_t seed,uint64_t zero,uint64_t width){
    assert(!zero&&bytes==width&&(bytes==8||bytes==16));
    uint64_t value=seed;const auto* data=static_cast<const uint8_t*>(input);
    for(size_t i=0;i<bytes;++i)value=(value^data[i])*0x100000001b3ULL;
    return value;
}
uint64_t fakeSettingsHash(uint8_t first,uint32_t paint,uint64_t flags,uint8_t a,uint8_t b,uint8_t c,uint8_t d){
    return first^uint64_t(paint)<<8^flags<<24^uint64_t(a)<<40^uint64_t(b)<<48^uint64_t(c)<<56^d;
}
uint64_t fakeHash(const void* input,uint64_t bytes){
    ++hashCalls;assert(bytes%8==0);if(input==nativeHashBuffer)assert(bytes<=30*8);
    hashBytes=bytes;hashFirst=hashLast=0;uint64_t result=0;
    for(size_t i=0;i<bytes/8;++i){uint64_t value{};std::memcpy(&value,static_cast<const uint8_t*>(input)+i*8,8);if(!i)hashFirst=value;hashLast=value;result=(result^value)*0x100000001b3ULL;}
    return result;
}
uint64_t carHash(const native::Car& car){
    constexpr uint64_t seed=0x9c3805fc2c85caccULL;
    const auto word=[&](uint64_t value){return fakeLeaf(&value,8,seed,0,8);};
    const auto combine=[&](uint64_t a,uint64_t b){const uint64_t pair[]{a,b};return fakeLeaf(pair,16,seed,0,16);};
    uint16_t index{};uint32_t paint{},flags{};
    std::memcpy(&index,car.settings.data(),2);std::memcpy(&paint,car.settings.data()+4,4);std::memcpy(&flags,car.settings.data()+8,4);
    auto value=fakeSettingsHash(car.settings[2],paint,flags,car.settings[12],car.settings[13],car.settings[14],car.settings[15]);
    value=combine(word(index),value);value=combine(word(car.train),value);return combine(word(car.model),value);
}
void fakeDynamics(uint64_t object,uint64_t rules){
    ++dynamicsCalls;if(dynamicsThrows)throw std::bad_alloc();
    auto* current=native::hashFrame;
    if(dynamicsNested&&!hashDepth){++hashDepth;native::dynamics(object,rules);--hashDepth;assert(native::hashFrame==current);}
    const auto range=memory->get<std::array<uint64_t,3>>(object);const size_t count=(range[1]-range[0])/32;
    std::array<uint64_t,30> prefix{};for(size_t i=0;i<std::min(count,prefix.size());++i)prefix[i]=0x1000+i;
    auto* previous=nativeHashBuffer;nativeHashBuffer=prefix.data();
    failAllocation=hashOom;hashValue=native::hash(prefix.data(),count*8);failAllocation=false;nativeHashBuffer=previous;
}
void dynamics_hash_checks(){
    Memory m;memory=&m;NativeMachine functions;
    functions.bind(0x2416b0,reinterpret_cast<const void*>(&fakeLeaf));functions.bind(0x3f8720,reinterpret_cast<const void*>(&fakeSettingsHash));
    const auto previousBase=native::base;native::base=functions.address();native::originalDynamics=&fakeDynamics;native::originalHash=&fakeHash;
    auto calls=dynamicsCalls;auto hashes=hashCalls;
    for(const size_t count:{size_t{0},size_t{30},size_t{31},size_t{80},size_t{512},size_t{4096}}){
        std::vector<native::Car> cars(count,native::Car{1,Memory::id(0)});for(size_t i=0;i<count;++i){const uint16_t ordinal=uint16_t(i);std::memcpy(cars[i].settings.data(),&ordinal,2);}
        m.vector(Memory::motion(0)+8,0x400000,cars);native::dynamics(Memory::motion(0)+8,Memory::rules);
        assert(dynamicsCalls==++calls&&hashCalls==++hashes&&hashBytes==count*8&&!native::hashFrame);
        if(count>30)assert(hashFirst==carHash(cars.front())&&hashLast==carHash(cars.back()));
        else if(count)assert(hashFirst==0x1000&&hashLast==0x1000+count-1);
    }
    m.vector(Memory::motion(0)+8,0x400000,std::vector<native::Car>(4097,native::Car{1}));native::dynamics(Memory::motion(0)+8,Memory::rules);
    assert(dynamicsCalls==calls&&hashCalls==hashes);
    m.vector(Memory::motion(0)+8,0x400000,std::vector<native::Car>(80,native::Car{1}));
    m.failAt=Memory::motion(0)+8;native::dynamics(Memory::motion(0)+8,Memory::rules);assert(dynamicsCalls==calls);m.failAt=0;
    // Only the native's actually initialized30-hash prefix may be read after
    // allocation, model lookup, unreadable composition or count mismatch.
    for(const int failure:{0,1,2,3}){
        m.vector(Memory::motion(0)+8,0x400000,std::vector<native::Car>(80,native::Car{1}));
        hashOom=failure==0;
        if(failure==1)m.failAt=0x400000;
        if(failure==2)m.put(0x400000,native::Car{99});
        if(failure==3)m.failRead=m.reads+2;
        native::dynamics(Memory::motion(0)+8,Memory::rules);
        assert(dynamicsCalls==++calls&&hashCalls==++hashes&&hashBytes==240&&hashFirst==0x1000&&hashLast==0x101d&&!native::hashFrame);
        hashOom=false;m.failAt=0;m.failRead=0;
    }
    native::HashFrame outer{0xabc,0xdef,nullptr};native::hashFrame=&outer;dynamicsThrows=true;
    try{native::dynamics(Memory::motion(0)+8,Memory::rules);assert(false);}catch(const std::bad_alloc&){}
    assert(dynamicsCalls==++calls&&hashCalls==hashes&&native::hashFrame==&outer);dynamicsThrows=false;native::hashFrame=nullptr;
    dynamicsNested=true;native::dynamics(Memory::motion(0)+8,Memory::rules);calls+=2;hashes+=2;
    assert(dynamicsCalls==calls&&hashCalls==hashes&&!native::hashFrame);dynamicsNested=false;native::base=previousBase;
}
}
static BOOL WINAPI fixtureReadProcessMemory(HANDLE process,LPCVOID at,LPVOID target,SIZE_T count,SIZE_T* copied){
    const auto value=reinterpret_cast<uint64_t>(at);
    if(memory&&value>=Memory::base&&value<Memory::base+memory->data.size()){
        const bool success=memory->read(value,target,count);if(copied)*copied=success?count:0;return success;
    }
    return realReadProcessMemory(process,at,target,count,copied);
}
int main(){
    assert(!native::installed&&!native::installationBroken);
    assert(native::diagnostics::writer().initialize(&Logs::sink,&logs));
    assert(native::registry.replace(1,850,native::EditorMessages::legacy())==policy::PublishResult::Applied);
    native::originalCommit=&fakeCommit;native::originalPurchase=&fakePurchase;
    native::originalRecompose=&fakeRecompose;native::originalValidate=&fakeValidate;
    editor_checks();purchase_checks();recompose_checks();native_exceptions_remain_native();removal_refreshes_refusal();live_length_tracks_actual_composition();
    assert(native::registry.remove(1)==policy::PublishResult::Applied);
    aggregate_gateway_checks();aggregate_admission_checks();
    // This synthetic refusal campaign deliberately fills the asynchronous
    // rate-limited queue; drain it in bounded chunks before the hash campaign.
    for(size_t i=0;i<16&&!native::diagnostics::writer().idle();++i)std::this_thread::sleep_for(500ms);
    logs.idle();dynamics_hash_checks();
    Memory m;memory=&m;m.edit({{5},{5}},99,100);const auto previousCalls=commitCalls;
    const auto reads=m.reads;native::commit(Memory::editor,0,0);
    assert(commitCalls==previousCalls+1&&m.reads==reads);
    logs.idle();
    // The production writer deliberately never blocks producers. Contention
    // with its worker can drop an event, including in this synthetic burst;
    // any such loss must be reported rather than falsely promising delivery.
    if(native::diagnostics::writer().dropped())assert(logs.contains("Train length diagnostics dropped:"));
    assert(!native::installed&&!native::installationBroken);
    native::diagnostics::writer().shutdown();
    std::cout<<"Production train length detours passed: "<<checks<<" checks with fake native bodies; no hooks installed\n";
}
