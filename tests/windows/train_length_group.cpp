#include <platform/windows/train_length_group.h>
#include <cassert>
#include <iostream>
#include <limits>

namespace native=nimby::platform::windows::train_length;
namespace groups=native::groups;
namespace policy=nimby::engine::train_length;
namespace {
struct Memory {
    static constexpr uint64_t base=0x10000,database=0x20000,simulation=0x40000,rules=0x60000,
        table=0x61000,sentinel=0x62000,modelBase=0x580000,trainBlocks=0x80000,motionBlocks=0x90000,
        trainBlock=0x100000,motionBlock=0x200000,carStorage=0x300000;
    static constexpr size_t slots=32;
    std::vector<unsigned char> data=std::vector<unsigned char>(0x600000);
    size_t reads{},bulkReads{},models{};
    uint64_t failAt{},changedMotion{};
    bool changeHeader=false;
    static uint64_t id(size_t index,uint16_t generation=7){return (uint64_t{5}<<48)|(index<<16)|generation;}
    static uint64_t train(size_t index){return trainBlock+index*0x178;}
    static uint64_t motion(size_t index){return motionBlock+index*0x638;}
    template<class T> void put(uint64_t at,const T& value){
        assert(at>=base&&at-base<=data.size()-sizeof value);std::memcpy(data.data()+at-base,&value,sizeof value);
    }
    template<class T> T get(uint64_t at)const{T value{};std::memcpy(&value,data.data()+at-base,sizeof value);return value;}
    static bool read(void* context,uint64_t at,void* target,size_t count){
        auto& m=*static_cast<Memory*>(context);++m.reads;
        if(at<base||count>m.data.size()||at-base>m.data.size()-count||
           (m.failAt&&at<=m.failAt&&m.failAt-at<count))return false;
        std::memcpy(target,m.data.data()+at-base,count);
        if(at==motionBlock&&count==slots*0x638){
            ++m.bulkReads;if(m.changeHeader){m.put(simulation+0xa0+8,uint32_t{16});m.changeHeader=false;}
        }
        if(m.changedMotion&&at>=carStorage&&at<carStorage+slots*0x10000){
            m.put(m.changedMotion+0x80,int32_t{2});m.changedMotion=0;
        }
        return true;
    }
    void pool(uint64_t header,uint64_t blocks,uint64_t block){
        std::array<unsigned char,48> bytes{};put(header,bytes);
        put(header+4,uint32_t{5});put(header+8,uint32_t{slots});put(header+0x10,uint32_t{slots-1});
        put(header+0x18,blocks);put(header+0x20,blocks+8);put(header+0x28,blocks+8);put(blocks,block);
    }
    void model(uint64_t key,float meters){
        const auto node=modelBase+(models++)*0x400,bucket=table+(key%8)*8;
        put(node,key);put(node+0xfc,meters);put(node+0x2a0,get<uint64_t>(bucket));put(bucket,node);
    }
    static native::Car car(uint64_t model,uint8_t mask=0xff){native::Car result{model};result.settings[15]=mask;return result;}
    void cars(size_t index,const std::vector<native::Car>& values){
        const auto at=carStorage+index*0x10000;
        assert(values.size()*sizeof(native::Car)<=0x10000);
        put(train(index)+0xc0,std::array<uint64_t,3>{at,at+values.size()*32,at+values.size()*32});
        if(!values.empty())std::memcpy(data.data()+at-base,values.data(),values.size()*sizeof(native::Car));
    }
    void current(size_t index,const std::vector<native::Car>& values){
        const auto at=0x500000+index*0x4000;
        assert(values.size()*sizeof(native::Car)<=0x4000);
        put(motion(index)+8,std::array<uint64_t,3>{at,at+values.size()*32,at+values.size()*32});
        if(!values.empty())std::memcpy(data.data()+at-base,values.data(),values.size()*sizeof(native::Car));
    }
    void record(size_t index,const std::vector<native::Car>& values,uint64_t parent=0,int32_t mode=0){
        put(train(index),id(index));put(motion(index),id(index));
        put(motion(index)+0x1d8,parent);put(motion(index)+0x1f0,uint8_t(parent!=0));put(motion(index)+0x80,mode);
        cars(index,values);
        // Deliberately empty native hitch lists. Pending parent/flag fields
        // must determine the candidate even before native normalization.
        put(motion(index)+0x88,std::array<uint64_t,3>{});
    }
    Memory(){
        pool(database+0x200,trainBlocks,trainBlock);pool(simulation+0xa0,motionBlocks,motionBlock);
        put(database+0x408,rules);put(rules+0x38,table);put(rules+0x40,uint32_t{8});put(table+64,sentinel);
        model(1,500);model(2,400);model(3,450);model(4,100);model(5,200);model(6,300);
        model(7,1);model(8,600);model(9,0);model(10,std::numeric_limits<float>::quiet_NaN());
        record(0,{car(1)});record(1,{car(1)});
    }
    groups::Context context()const{return {database,simulation};}
    groups::Check hitch(size_t driver,size_t child,policy::Limit limit={850}){
        return groups::hitch(read,this,context(),limit,id(driver),id(child));
    }
    groups::Check config(size_t train,int32_t mode,policy::Limit limit={850}){
        return groups::setConfig(read,this,context(),limit,id(train),mode);
    }
    groups::Check recompose(size_t train,const std::vector<native::Car>& values,policy::Limit limit={850}){
        return groups::recompose(read,this,context(),limit,id(train),values);
    }
};
}
int main(){
    {
        Memory m;const auto before=m.data;auto result=m.hitch(0,1);
        assert(!result.allowed()&&result.assessment.decision==policy::Decision::LimitExceeded);
        assert(result.root==Memory::id(0)&&result.changedTrain==Memory::id(1)&&result.members==2);
        assert(result.before.totalLengthMeters==500&&result.assessment.totalLengthMeters==1000&&m.data==before);
        assert(result.previousElements==1&&result.candidateElements==2&&m.bulkReads==1);
        assert(result.modelCount==1&&result.models[0].model==1&&result.models[0].count==2);
        assert(result.models[0].observed&&result.models[0].length==500&&result.omittedElements==0);
        m.cars(0,{Memory::car(2)});m.cars(1,{Memory::car(3)});result=m.hitch(0,1);
        assert(result.allowed()&&result.assessment.totalLengthMeters==850&&m.bulkReads==2);
    }
    {
        Memory m;m.cars(0,{Memory::car(8),Memory::car(4)});m.cars(1,{Memory::car(4)});
        m.current(1,{Memory::car(1),Memory::car(1)});const auto before=m.data;
        auto result=m.hitch(0,1);
        assert(!result.allowed()&&result.issue==groups::Issue::CurrentCompositionLag&&result.laggingMembers==1);
        assert(result.planned.totalLengthMeters==800&&result.assessment.totalLengthMeters==1700&&result.candidateElements==4);
        assert(result.models[2].model==1&&result.models[2].count==2&&result.models[2].length==500&&m.data==before);
        m.current(1,{Memory::car(4)});result=m.hitch(0,1);
        assert(result.allowed()&&result.assessment.totalLengthMeters==800&&result.laggingMembers==0&&m.bulkReads==2);
        m.current(1,std::vector<native::Car>(31,Memory::car(7)));result=m.hitch(0,1);
        assert(!result.allowed()&&result.issue==groups::Issue::CurrentCompositionLag&&result.candidateElements==33);
        assert(result.assessment.decision==policy::Decision::TooManyElements); // Count guard uses larger current count.
    }
    {
        // An earlier child command has not refreshed CurrentDynamics yet.
        // Growing the root must compare real physical bounds on both sides.
        Memory m;m.record(0,{Memory::car(4,1),Memory::car(1,2)},0,1);
        m.record(1,{Memory::car(4)},Memory::id(0));m.current(1,{Memory::car(1),Memory::car(1)});
        const auto before=m.data;auto result=m.config(0,2);
        assert(!result.allowed()&&result.issue==groups::Issue::CurrentCompositionLag);
        assert(result.before.totalLengthMeters==1100&&result.planned.totalLengthMeters==600&&result.assessment.totalLengthMeters==1500);
        assert(result.laggingMembers==1&&m.data==before&&m.bulkReads==1);
        m.record(0,{Memory::car(4)});result=m.recompose(0,{Memory::car(1)});
        assert(!result.allowed()&&result.issue==groups::Issue::CurrentCompositionLag);
        assert(result.before.totalLengthMeters==1100&&result.planned.totalLengthMeters==600&&result.assessment.totalLengthMeters==1500);
        m.record(0,{Memory::car(1)});result=m.recompose(0,{Memory::car(2)});
        assert(result.allowed()&&result.before.totalLengthMeters==1500&&result.assessment.totalLengthMeters==1400);
        assert(result.assessment.maximumMeters==850&&result.laggingMembers==1); // Proven physical reduction stays available.
    }
    {
        Memory m;m.record(0,{Memory::car(8),Memory::car(4)});
        m.record(1,{Memory::car(4,1),Memory::car(1,2),Memory::car(2,2)},Memory::id(0));
        m.current(1,{Memory::car(4),Memory::car(1),Memory::car(2)});
        const auto before=m.data;auto result=m.config(1,1);
        assert(result.allowed()&&result.before.totalLengthMeters==1700&&result.assessment.totalLengthMeters==1700);
        assert(result.planned.totalLengthMeters==800&&result.laggingMembers==1&&m.data==before);
        result=m.recompose(1,{Memory::car(4)});
        assert(result.allowed()&&result.before.totalLengthMeters==1700&&result.assessment.totalLengthMeters==1700);
        assert(result.ownBefore.totalLengthMeters==1000&&result.ownAfter.totalLengthMeters==100);
        m.current(1,{Memory::car(99)});result=m.config(0,1);
        assert(!result.allowed()&&result.assessment.decision==policy::Decision::UnknownModel);
        assert(result.readIssue.fault==native::ReadFault::ModelMissing&&result.readIssue.detail==99);
        assert(result.modelCount==1&&result.models[0].model==99&&result.models[0].observed&&!result.models[0].length);
    }
    {
        Memory m;m.cars(0,{Memory::car(6)});m.cars(1,{Memory::car(6)});
        m.record(2,{Memory::car(6)},Memory::id(0));const auto before=m.data;
        const auto result=m.hitch(0,1); // A previous command is already visible in parent/flag.
        assert(result.members==3&&result.before.totalLengthMeters==600&&result.assessment.totalLengthMeters==900);
        assert(!result.allowed()&&m.data==before&&m.bulkReads==1);
    }
    {
        Memory m;m.cars(0,{Memory::car(2)});m.cars(1,{Memory::car(5)});
        m.record(2,{Memory::car(4)},Memory::id(0));m.record(3,{Memory::car(4)});
        m.put(Memory::motion(1)+0x1d8,Memory::id(3));m.put(Memory::motion(1)+0x1f0,uint8_t{1});
        auto result=m.hitch(0,1); // Reparent from root3 into root0; never count source twice.
        assert(result.allowed()&&result.members==3&&result.assessment.totalLengthMeters==700);
        m.put(Memory::motion(1)+0x1d8,Memory::id(0));result=m.hitch(0,1,{100});
        assert(result.allowed()&&!result.checked); // Same group: orientation/order only after lower limit.
        m.failAt=Memory::motionBlock;
        result=groups::hitch(Memory::read,&m,m.context(),{1},0,Memory::id(1));
        assert(result.allowed()&&!result.checked); // Unhitch uses no snapshot/read.
    }
    {
        Memory m;auto reads=m.reads;
        auto result=groups::hitch(Memory::read,&m,{}, {},Memory::id(0),Memory::id(1));
        assert(result.allowed()&&!result.checked&&m.reads==reads);
        result=groups::setConfig(Memory::read,&m,{}, {},Memory::id(0),1);
        assert(result.allowed()&&!result.checked&&m.reads==reads);
        result=groups::recompose(Memory::read,&m,{}, {},Memory::id(0),{});
        assert(result.allowed()&&!result.checked&&m.reads==reads);
        result=groups::hitch(Memory::read,&m,{Memory::database,0x123},{850},Memory::id(0),Memory::id(1));
        assert(!result.allowed()&&result.issue==groups::Issue::Unavailable&&result.readIssue.fault!=native::ReadFault::None);
        result=groups::hitch(Memory::read,&m,{0x123,Memory::simulation},{850},Memory::id(0),Memory::id(1));
        assert(!result.allowed()&&result.readIssue.fault!=native::ReadFault::None);
        result=groups::hitch(Memory::read,&m,m.context(),{850},Memory::id(7),Memory::id(1));
        assert(result.allowed()&&!result.checked); // Absent driver is native detach.
        result=groups::hitch(Memory::read,&m,m.context(),{850},Memory::id(0),Memory::id(1,8));
        assert(result.allowed()&&!result.checked); // Old generation: absent child native no-op.
    }
    {
        Memory m;
        auto result=m.hitch(0,0);assert(!result.allowed()&&result.issue==groups::Issue::InvalidTopology);
        m.put(Memory::motion(0)+0x1d8,Memory::id(1));m.put(Memory::motion(0)+0x1f0,uint8_t{1});
        result=m.hitch(0,1);assert(!result.allowed()&&result.issue==groups::Issue::InvalidTopology);
        m.put(Memory::motion(1)+0x1d8,Memory::id(0));m.put(Memory::motion(1)+0x1f0,uint8_t{1});
        result=m.config(1,1);assert(!result.allowed()&&result.issue==groups::Issue::InvalidTopology);
    }
    {
        Memory m;m.record(2,{Memory::car(4)},Memory::id(1));
        auto result=m.hitch(0,1);assert(!result.allowed()&&result.issue==groups::Issue::InvalidTopology);
        m.put(Memory::motion(1)+0x1d8,Memory::id(0));m.put(Memory::motion(1)+0x1f0,uint8_t{1});
        result=m.config(0,1);assert(!result.allowed()&&result.issue==groups::Issue::InvalidTopology);
        result=m.recompose(1,{Memory::car(4)});assert(!result.allowed()&&result.issue==groups::Issue::InvalidTopology);
    }
    {
        Memory m;m.cars(0,{Memory::car(2)});m.record(1,{Memory::car(4,1),Memory::car(1,2)},0,1);
        auto result=m.hitch(0,1);assert(result.allowed()&&result.assessment.totalLengthMeters==500);
        m.put(Memory::motion(1)+0x1d8,Memory::id(0));m.put(Memory::motion(1)+0x1f0,uint8_t{1});
        const auto before=m.data;result=m.config(1,0);
        assert(!result.allowed()&&result.before.totalLengthMeters==500&&result.assessment.totalLengthMeters==1000&&m.data==before);
        result=m.config(1,99);assert(result.allowed()&&result.assessment.totalLengthMeters==400); // Mode8 excludes both.
    }
    {
        Memory m;m.cars(0,{Memory::car(8)});m.record(1,{Memory::car(2,1),Memory::car(4,2)},Memory::id(0),0);
        const auto before=m.data;auto result=m.config(1,1);
        assert(result.allowed()&&result.before.totalLengthMeters==1100&&result.assessment.totalLengthMeters==1000);
        assert(result.assessment.maximumMeters==850&&result.candidateElements==2&&result.previousElements==3&&m.data==before);
        result=m.config(1,0);assert(result.allowed()&&!result.checked); // Native unchanged config, no truncation.
    }
    {
        Memory m;m.cars(0,{Memory::car(2)});m.record(1,{Memory::car(2)},Memory::id(0));
        const auto before=m.data;auto result=m.recompose(1,{Memory::car(1)});
        assert(!result.allowed()&&result.assessment.totalLengthMeters==900&&result.ownAfter.totalLengthMeters==500&&m.data==before);
        result=m.recompose(1,{Memory::car(6)});assert(result.allowed()&&result.assessment.totalLengthMeters==700);
        m.cars(0,{Memory::car(8)});m.cars(1,{Memory::car(8)});result=m.recompose(1,{Memory::car(1)});
        assert(result.allowed()&&result.before.totalLengthMeters==1200&&result.assessment.totalLengthMeters==1100);
        m.record(1,{Memory::car(2,1),Memory::car(2,2)},Memory::id(0),1);
        result=m.recompose(1,{Memory::car(2,1),Memory::car(1,2)});
        assert(!result.allowed()&&result.ownAfter.totalLengthMeters==900&&result.ownBefore.totalLengthMeters==800);
    }
    {
        Memory m;m.cars(0,{Memory::car(1),Memory::car(1)});m.put(Memory::motion(0),uint64_t{});
        auto result=m.recompose(0,{Memory::car(3),Memory::car(3)});
        assert(result.allowed()&&result.assessment.totalLengthMeters==900&&result.before.totalLengthMeters==1000&&result.members==1);
        result=m.recompose(0,{Memory::car(8),Memory::car(1)});
        assert(!result.allowed()&&result.assessment.totalLengthMeters==1100);
        result=m.recompose(0,std::vector<native::Car>(9,Memory::car(4)));
        assert(result.allowed()&&result.assessment.totalLengthMeters==900&&result.candidateElements==9);
    }
    {
        Memory m;m.cars(0,{Memory::car(7)});m.cars(1,{Memory::car(99)});
        auto result=m.hitch(0,1);assert(!result.allowed()&&result.assessment.decision==policy::Decision::UnknownModel);
        assert(result.readIssue.fault==native::ReadFault::ModelMissing&&result.readIssue.detail==99);
        assert(result.modelCount==2&&result.models[1].model==99&&result.models[1].observed&&!result.models[1].length);
        for(const auto key:{uint64_t{9},uint64_t{10}}){m.cars(1,{Memory::car(key)});result=m.hitch(0,1);
            assert(!result.allowed()&&result.assessment.decision==policy::Decision::InvalidLength&&result.readIssue.fault==native::ReadFault::ModelLength);}
        m.cars(0,{Memory::car(99)});result=m.recompose(0,{Memory::car(4)});
        assert(result.allowed()&&result.assessment.totalLengthMeters==100); // Repair an absent original model.
    }
    {
        Memory m;m.cars(0,std::vector<native::Car>(30,Memory::car(7)));m.cars(1,{Memory::car(7)});
        const auto result=m.hitch(0,1);
        assert(!result.allowed()&&result.issue==groups::Issue::NativeAggregationLimit&&result.assessment.totalLengthMeters==31);
        assert(result.assessment.decision==policy::Decision::TooManyElements&&result.candidateElements==31);
        // The native cap applies to aggregate expansion, never standalone composition.
        assert(m.recompose(0,std::vector<native::Car>(31,Memory::car(7))).allowed());
    }
    {
        Memory m;std::vector<native::Car> many;
        for(uint64_t i=100;i<229;++i){m.model(i,1);many.push_back(Memory::car(i));}
        const auto result=m.recompose(0,many);
        assert(result.allowed()&&result.modelCount==128&&result.omittedElements==1&&result.assessment.totalLengthMeters==129);
        for(size_t i=0;i<result.modelCount;++i)assert(result.models[i].observed&&result.models[i].length==1&&result.models[i].count==1);
    }
    {
        Memory m;m.changeHeader=true;const auto result=m.hitch(0,1);
        assert(!result.allowed()&&result.issue==groups::Issue::Unavailable&&result.readIssue.fault==native::ReadFault::VectorLayout);
    }
    {
        Memory m;m.changedMotion=Memory::motion(1);const auto result=m.hitch(0,1);
        assert(!result.allowed()&&result.issue==groups::Issue::Unavailable&&result.readIssue.fault==native::ReadFault::VectorLayout);
    }
    {
        Memory m;m.put(Memory::train(1),Memory::id(1,8));auto result=m.hitch(0,1);
        assert(!result.allowed()&&result.issue==groups::Issue::Unavailable&&result.readIssue.detail==Memory::id(1));
        m.failAt=Memory::motionBlock;result=m.config(0,1);
        assert(!result.allowed()&&result.readIssue.fault==native::ReadFault::Read&&result.readIssue.address==Memory::motionBlock);
    }
    std::cout<<"PASS: train group metre gates, pending attachments, active masks, recompose, reduction and read faults\n";
}
