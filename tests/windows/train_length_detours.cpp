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

// Exercise the production detours without calling install(), loading a game or
// creating any hook. Only the memory transport is replaced; outputs are real,
// test-owned buffers so production result writes also remain under test.
static auto realReadProcessMemory=&::ReadProcessMemory;
static BOOL WINAPI fixtureReadProcessMemory(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
#define ReadProcessMemory fixtureReadProcessMemory
#include "../../src/platform/windows/runtime/train_length_native.cpp"
#undef ReadProcessMemory

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
    size_t reads{},modelCount{};uint64_t failAt{};
    static uint64_t id(size_t index){return (uint64_t{5}<<48)|(index<<16)|7;}
    static uint64_t train(size_t index){return trains+index*0x178;}
    static uint64_t motion(size_t index){return motions+index*0x638;}
    template<class T> void put(uint64_t at,const T& value){assert(at>=base&&at-base<=data.size()-sizeof value);std::memcpy(data.data()+at-base,&value,sizeof value);}
    template<class T> T get(uint64_t at)const{T value{};std::memcpy(&value,data.data()+at-base,sizeof value);return value;}
    bool read(uint64_t at,void* target,size_t count){
        ++reads;
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
    editor_checks();purchase_checks();recompose_checks();native_exceptions_remain_native();
    assert(native::registry.remove(1)==policy::PublishResult::Applied);
    Memory m;memory=&m;m.edit({{5},{5}},99,100);const auto previousCalls=commitCalls;
    const auto reads=m.reads;native::commit(Memory::editor,0,0);
    assert(commitCalls==previousCalls+1&&m.reads==reads);
    logs.idle();assert(native::diagnostics::writer().dropped()==0);
    assert(!native::installed&&!native::installationBroken);
    native::diagnostics::writer().shutdown();
    std::cout<<"Production train length detours passed with fake native bodies; no hooks installed\n";
}
