#include <platform/windows/train_length_native.h>
#include <cassert>
#include <iostream>
#include <limits>

namespace native=nimby::platform::windows::train_length;
namespace policy=nimby::engine::train_length;
namespace {
struct Memory {
    static constexpr uint64_t base=0x10000,editor=0x20000,rules=0x40000,table=0x50000,
        cars=0x60000,resetCars=0x70000,clipboard=0x80000,selection=0x90000,modelBase=0x100000;
    std::vector<uint8_t> data=std::vector<uint8_t>(0x200000);
    size_t reads=0;uint64_t failAt=0;size_t nodes=0;
    template<class T> void put(uint64_t at,const T& value){
        assert(at>=base&&at-base<=data.size()-sizeof(T));std::memcpy(data.data()+at-base,&value,sizeof value);
    }
    template<class T> T get(uint64_t at)const {T result{};std::memcpy(&result,data.data()+at-base,sizeof result);return result;}
    static bool read(void* context,uint64_t at,void* target,size_t count){
        auto& memory=*static_cast<Memory*>(context);++memory.reads;
        if(at<base||count>memory.data.size()||at-base>memory.data.size()-count||
            (memory.failAt&&at<=memory.failAt&&memory.failAt-at<count))return false;
        std::memcpy(target,memory.data.data()+at-base,count);return true;
    }
    native::View view(){return {&read,this};}
    Memory(){
        put(rules+0x38,table);put(rules+0x40,uint32_t{8});put(table+64,uint64_t{0x50100});
        model(1,25);model(2,400);model(3,425);model(4,.01f);model(5,500);model(6,300);
        model(7,10);model(8,2.5f);model(9,0);model(10,std::numeric_limits<float>::quiet_NaN());
        vector(editor+0xc0,cars,{});vector(editor+0x238,resetCars,{});vector(editor+0x1608,clipboard,{});
        flags(4);put(editor+0x15e0,native::Car{1});put(editor+0x1600,int32_t{1});
    }
    uint64_t model(uint64_t id,float meters){
        const auto node=modelBase+nodes++*0x400;const auto bucket=table+(id%8)*8;
        put(node,id);put(node+0xfc,meters);put(node+0x2a0,get<uint64_t>(bucket));put(bucket,node);return node;
    }
    void vector(uint64_t descriptor,uint64_t storage,const std::vector<native::Car>& values){
        const std::array<uint64_t,3> pointers{storage,storage+values.size()*sizeof(native::Car),storage+values.size()*sizeof(native::Car)};
        put(descriptor,pointers);if(!values.empty())std::memcpy(data.data()+storage-base,values.data(),values.size()*sizeof(native::Car));
    }
    void carsOf(std::initializer_list<uint64_t> ids){
        std::vector<native::Car> values;for(auto id:ids)values.push_back({id});vector(editor+0xc0,cars,values);
    }
    void flags(size_t action){
        std::array<uint8_t,16> value{};value[action]=1;put(editor+0x15c0,value);put(editor+0x15d0,uint8_t{1});
        put(editor+0x1598,std::array<uint8_t,7>{});put(editor+0x1138,uint64_t{});put(editor+0x1128,uint64_t{});
    }
    void selected(std::initializer_list<int32_t> indices){
        put(editor+0x1138,static_cast<uint64_t>(indices.size()));put(editor+0x1128,indices.size()?selection:uint64_t{});
        size_t i=0;for(auto index:indices){const auto node=selection+i*0x40;
            put(node,std::array<uint64_t,3>{++i<indices.size()?node+0x40:uint64_t{},0,0});put(node+0x20,index);}
    }
    native::Proposal proposal(){return native::proposed(view(),editor);}
    policy::Assessment assess(const native::Proposal& proposal){return native::assess(view(),rules,{850},proposal.models);}
};
void diagnostic_contracts(){
    Memory m;
    m.carsOf({2,3});m.put(Memory::editor+0x1600,int32_t{19});
    auto proposal=m.proposal();
    assert(proposal.beforeElements==2&&proposal.requestedElements==19&&proposal.selectedElements==0);
    assert(proposal.actionFlags==(1u<<4)&&proposal.beforeModels.empty());
    m.flags(5);m.carsOf({2,3,1});m.selected({0,1});
    proposal=m.proposal();
    assert(proposal.beforeElements==3&&proposal.requestedElements==2&&proposal.selectedElements==2);
    assert(proposal.actionFlags==(1u<<5)&&proposal.beforeModels.size()==3);
    assert(proposal.beforeModels[0].model==2&&proposal.beforeModels[1].model==3);
    assert(proposal.models[0].model==1&&proposal.models[1].model==1);
    assert(native::assess(m.view(),Memory::rules,{850},proposal.beforeModels).totalLengthMeters==850);
    m.flags(9);m.vector(Memory::editor+0x1608,Memory::clipboard,{{5},{6}});
    proposal=m.proposal();assert(proposal.beforeElements==3&&proposal.requestedElements==2&&proposal.beforeModels.empty());
    m.flags(11);m.vector(Memory::editor+0x238,Memory::resetCars,{{5},{6}});
    proposal=m.proposal();
    assert(proposal.edit==native::Edit::Reset&&proposal.beforeElements==3&&proposal.requestedElements==2);
    assert(proposal.models.size()==2&&proposal.beforeModels.size()==3);
    assert(native::assess(m.view(),Memory::rules,{850},proposal.beforeModels).totalLengthMeters==850);
    assert(m.assess(proposal).totalLengthMeters==800);

    // A snapshot exposes the first failing read, even if later diagnostic
    // enrichment also encounters a bad field or an unknown model.
    m.flags(4);m.carsOf({2});m.put(Memory::editor+0x1600,int32_t{1});
    auto view=m.view();assert(view.issue().fault==native::ReadFault::None);
    m.failAt=Memory::cars;
    proposal=native::proposed(view,Memory::editor);assert(!proposal.readable);
    const auto first=view.issue();
    assert(first.fault==native::ReadFault::Read&&first.address==Memory::cars&&first.detail==sizeof(native::Car));
    uint64_t value{};assert(!view.get(0xffff,value));
    assert(view.issue().fault==first.fault&&view.issue().address==first.address&&view.issue().detail==first.detail);
    m.failAt=0;
    auto missing=m.view();assert(!missing.length(Memory::rules,77));
    assert(missing.issue().fault==native::ReadFault::ModelMissing&&missing.issue().address==Memory::table&&missing.issue().detail==77);
    auto invalid=m.view();const auto zeroModel=invalid.modelNode(Memory::rules,9);assert(zeroModel);
    assert(invalid.length(Memory::rules,9)==0);
    assert(invalid.issue().fault==native::ReadFault::ModelLength&&invalid.issue().address==*zeroModel+0xfc&&invalid.issue().detail==9);
    m.flags(5);m.selected({-1});
    auto selection=m.view();assert(!native::proposed(selection,Memory::editor).readable);
    assert(selection.issue().fault==native::ReadFault::Selection&&selection.issue().address==Memory::selection+0x20);
    m.flags(4);m.put(Memory::editor+0x1600,int32_t{-1});
    auto count=m.view();assert(!native::proposed(count,Memory::editor).readable);
    assert(count.issue().fault==native::ReadFault::ElementBudget&&count.issue().address==Memory::editor+0x1600);

    // Logging each resolved descriptor must share the existing request-local
    // model cache and must perform no extra memory reads of its own.
    m.flags(4);m.put(Memory::editor+0x1600,int32_t{1});
    m.vector(Memory::editor+0xc0,Memory::cars,std::vector<native::Car>(100,{8}));
    proposal=m.proposal();const auto before=m.reads;
    std::vector<std::pair<uint64_t,double>> observed;
    const auto assessment=native::assess(m.view(),Memory::rules,{850},proposal.models,
        [&](uint64_t model,const std::optional<double>& meters){assert(meters);observed.push_back({model,*meters});});
    assert(assessment.allowed()&&assessment.totalLengthMeters==275&&m.reads-before<=16);
    assert(observed.size()==101&&observed.back()==std::make_pair(uint64_t{1},25.0));
    for(size_t i=0;i<100;++i)assert(observed[i]==std::make_pair(uint64_t{8},2.5));
    const auto inactiveReads=m.reads;size_t inactiveObserved=0;
    assert(native::assess(m.view(),Memory::rules,{},proposal.models,[&](auto,auto){++inactiveObserved;}).allowed());
    assert(m.reads==inactiveReads&&inactiveObserved==0);
}
}

int main(){
    diagnostic_contracts();
    Memory m;
    // The former car-count ceiling is not a business rule: 30 -> 31 and
    // longer compositions remain valid when their metres fit.
    m.vector(Memory::editor+0xc0,Memory::cars,std::vector<native::Car>(30,{1}));
    auto proposal=m.proposal();assert(proposal.readable&&proposal.edit==native::Edit::Append&&proposal.models.size()==31);
    assert(m.assess(proposal).allowed()&&m.assess(proposal).totalLengthMeters==775);
    m.vector(Memory::editor+0xc0,Memory::cars,std::vector<native::Car>(100,{8}));
    proposal=m.proposal();const auto beforeCachedRead=m.reads;const auto repeatedAssessment=m.assess(proposal);
    assert(repeatedAssessment.allowed()&&repeatedAssessment.totalLengthMeters==275&&m.reads-beforeCachedRead<=16);

    m.carsOf({2,3});proposal=m.proposal();assert(proposal.readable&&m.assess(proposal).totalLengthMeters==850&&m.assess(proposal).allowed());
    m.carsOf({2,3,1});m.put(Memory::editor+0x15e0,native::Car{4});proposal=m.proposal();
    assert(m.assess(proposal).decision==policy::Decision::LimitExceeded&&m.assess(proposal).totalLengthMeters>850.009);
    m.carsOf({2});m.put(Memory::editor+0x15e0,native::Car{1});m.put(Memory::editor+0x1600,int32_t{19});
    const auto before=m.get<native::Car>(Memory::cars);proposal=m.proposal();
    assert(proposal.models.back().count==19&&m.assess(proposal).decision==policy::Decision::LimitExceeded);
    assert(m.assess(proposal).totalLengthMeters==875&&m.get<native::Car>(Memory::cars).model==before.model);
    m.put(Memory::editor+0x1600,int32_t{-1});assert(!m.proposal().readable);
    m.put(Memory::editor+0x1600,int32_t{5000});assert(m.assess(m.proposal()).decision==policy::Decision::TooManyElements);

    m.flags(5);m.carsOf({2,3,1});m.selected({0,1});m.put(Memory::editor+0x15e0,native::Car{1});proposal=m.proposal();
    assert(proposal.readable&&proposal.edit==native::Edit::Replace&&m.assess(proposal).allowed()&&m.assess(proposal).totalLengthMeters==75);
    m.put(Memory::editor+0x15e0,native::Car{5});proposal=m.proposal();
    assert(proposal.readable&&m.assess(proposal).decision==policy::Decision::LimitExceeded&&m.assess(proposal).totalLengthMeters==1025);
    for(const auto bad:{-1,3}){m.selected({bad});assert(!m.proposal().readable);}
    m.selected({0,0});assert(!m.proposal().readable);
    m.selected({0});m.put(Memory::selection,std::array<uint64_t,3>{Memory::selection,0,0});assert(!m.proposal().readable);
    m.selected({0});m.put(Memory::editor+0x1138,uint64_t{2});assert(!m.proposal().readable);
    m.selected({0});m.put(Memory::editor+0x1128,uint64_t{0});assert(!m.proposal().readable);
    m.selected({0});m.put(Memory::editor+0x1138,uint64_t{policy::maximumElements+1});assert(!m.proposal().readable);

    for(const auto action:{size_t{0},size_t{12},size_t{13}}){
        m.flags(action);auto flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[4]=1;m.put(Memory::editor+0x15c0,flags);
        m.failAt=Memory::editor+0x1600;proposal=m.proposal();assert(proposal.readable&&proposal.edit==native::Edit::None);m.failAt=0;
    }
    for(const auto action:{size_t{1},size_t{2}}){
        m.flags(action);m.selected({0});auto flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[4]=1;m.put(Memory::editor+0x15c0,flags);
        assert(m.proposal().edit==native::Edit::None);
    }
    for(size_t cosmetic=0;cosmetic<7;++cosmetic){
        m.flags(4);std::array<uint8_t,7> value{};value[cosmetic]=1;m.put(Memory::editor+0x1598,value);
        assert(m.proposal().edit==native::Edit::None);
    }
    // No-op flags never reject an old, oversized composition or attempt to
    // resolve/read its cars. Native operations with priority stay available.
    m.flags(4);m.carsOf({5,5});m.put(Memory::editor+0x1600,int32_t{0});m.failAt=Memory::editor+0xc0;
    proposal=m.proposal();assert(proposal.readable&&proposal.edit==native::Edit::None);m.failAt=0;
    m.flags(5);m.failAt=Memory::editor+0xc0;proposal=m.proposal();assert(proposal.readable&&proposal.edit==native::Edit::None);m.failAt=0;
    m.flags(4);m.put(Memory::editor+0x15d0,uint8_t{0});m.failAt=Memory::editor+0x15c0;
    assert(m.proposal().readable&&m.proposal().edit==native::Edit::None);m.failAt=0;

    m.flags(9);m.vector(Memory::editor+0xc0,Memory::cars,std::vector<native::Car>(30,{1}));
    m.vector(Memory::editor+0x1608,Memory::clipboard,std::vector<native::Car>(4,{1}));proposal=m.proposal();
    assert(proposal.readable&&proposal.edit==native::Edit::Paste&&m.assess(proposal).allowed()&&m.assess(proposal).totalLengthMeters==850);
    m.vector(Memory::editor+0x1608,Memory::clipboard,std::vector<native::Car>(5,{1}));proposal=m.proposal();
    assert(m.assess(proposal).decision==policy::Decision::LimitExceeded&&m.assess(proposal).totalLengthMeters==875);
    auto flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[11]=1;m.put(Memory::editor+0x15c0,flags);
    m.vector(Memory::editor+0x238,Memory::resetCars,{{5},{6}});
    assert(m.proposal().edit==native::Edit::Paste); // Nonempty paste wins over reset.
    m.vector(Memory::editor+0x1608,Memory::clipboard,{});proposal=m.proposal();
    assert(proposal.readable&&proposal.edit==native::Edit::Reset&&m.assess(proposal).allowed()&&m.assess(proposal).totalLengthMeters==800);
    m.vector(Memory::editor+0x238,Memory::resetCars,{{5},{5}});assert(m.assess(m.proposal()).decision==policy::Decision::LimitExceeded);
    for(const auto action:{size_t{6},size_t{7},size_t{8}}){m.flags(action);flags=m.get<std::array<uint8_t,16>>(Memory::editor+0x15c0);flags[11]=1;m.put(Memory::editor+0x15c0,flags);assert(m.proposal().edit==native::Edit::None);}

    m.flags(4);m.carsOf({2});m.put(Memory::editor+0x1600,int32_t{1});m.put(Memory::editor+0x15e0,native::Car{99});
    assert(m.assess(m.proposal()).decision==policy::Decision::UnknownModel);
    for(const auto model:{uint64_t{9},uint64_t{10}}){m.put(Memory::editor+0x15e0,native::Car{model});assert(m.assess(m.proposal()).decision==policy::Decision::InvalidLength);}

    const auto view=m.view();uint64_t output{};const auto reads=m.reads;
    assert(!view.get(0xffff,output)&&!view.get(0x7fffffff0000ULL,output));
    assert(!view.bytes(0xffff,&output,8)&&!view.bytes(UINT64_MAX,&output,8)&&m.reads==reads);
    std::vector<native::Car> values;
    for(const auto pointers:{std::array<uint64_t,3>{Memory::cars+32,Memory::cars,Memory::cars+32},
            std::array<uint64_t,3>{Memory::cars,Memory::cars+1,Memory::cars+32},
            std::array<uint64_t,3>{Memory::cars,Memory::cars+32,Memory::cars},
            std::array<uint64_t,3>{Memory::cars,Memory::cars+(policy::maximumElements+1)*32,Memory::cars+(policy::maximumElements+1)*32},
            std::array<uint64_t,3>{UINT64_MAX-31,UINT64_MAX,UINT64_MAX}}){
        m.put(Memory::editor+0xc0,pointers);assert(!view.vector(Memory::editor+0xc0,values));
    }
    m.carsOf({2});m.failAt=Memory::cars;assert(!m.proposal().readable);m.failAt=0;
    assert(!view.modelNode(Memory::rules,123456));
    m.put(Memory::rules+0x40,uint32_t{0});assert(!view.modelNode(Memory::rules,1));m.put(Memory::rules+0x40,uint32_t{8});
    const auto cycleA=m.model(80,20),cycleB=m.model(88,20);m.put(cycleA+0x2a0,cycleB);m.put(cycleB+0x2a0,cycleA);
    const auto began=m.reads;assert(!view.modelNode(Memory::rules,96));assert(m.reads-began<=4+policy::maximumElements*2);
    m.put(cycleA+0x2a0,cycleA);m.put(Memory::table,cycleA);assert(!view.modelNode(Memory::rules,96));
    m.put(Memory::table,uint64_t{0x50100});assert(!view.modelNode(Memory::rules,96));
    std::cout<<"PASS: native composition proposal priority, replacement, batches, paste/reset and bounded memory/model reads\n";
}
