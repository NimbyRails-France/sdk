#include <platform/windows/train_length_validation.h>
#include <platform/windows/train_length_group.h>
#include <cassert>
#include <iostream>

namespace native=nimby::platform::windows::train_length;
namespace validation=native::validation;
namespace {
struct Memory {
    static constexpr uint64_t base=0x10000,output=0x11000,flags=0x12000;
    std::array<uint8_t,0x5000> data{};
    uint64_t failAt{};
    size_t reads{};
    template<class T> void put(uint64_t at,const T& value){
        assert(at>=base&&at-base<=data.size()-sizeof value);
        std::memcpy(data.data()+at-base,&value,sizeof value);
    }
    static bool read(void* context,uint64_t at,void* target,size_t count){
        auto& memory=*static_cast<Memory*>(context);++memory.reads;
        if(at<base||count>memory.data.size()||at-base>memory.data.size()-count||
           (memory.failAt&&at<=memory.failAt&&memory.failAt-at<count))return false;
        std::memcpy(target,memory.data.data()+at-base,count);return true;
    }
    void vector(uint64_t begin,uint64_t end,uint64_t capacity){
        put(output+8,std::array<uint64_t,3>{begin,end,capacity});
    }
    Memory(){
        data.fill(1);put(output,uint32_t{1});vector(flags,flags+62,flags+64);
    }
    validation::Recovery recover(size_t cars=31){
        const auto before=data;
        auto result=validation::recoverCountLimit(native::View(read,this),output,cars);
        assert(data==before);return result;
    }
};

struct RecomposeShim {
    static constexpr uint64_t rules=0x16000;
    std::vector<native::Car> candidate=std::vector<native::Car>(31,native::Car{1,7});
    Memory memory;
    validation::RecompositionApproval approval;
    size_t validatorCalls{},mutatorCalls{};
    RecomposeShim(){
        candidate[1].model=2;
        for(size_t i=0;i<candidate.size();++i)candidate[i].settings[15]=uint8_t{1};
        const native::policy::ModelBlock previous{3,1};
        auto current=native::policy::evaluateAppend({850},{},std::span(&previous,1),length);
        std::vector<native::policy::ModelBlock> models;
        for(const auto& car:candidate)models.push_back({car.model,1});
        auto proposed=native::policy::evaluateAppend({850},{},models,length);
        assert(current.totalLengthMeters==1200&&proposed.totalLengthMeters==1086);
        assert(proposed.decision==native::policy::Decision::LimitExceeded);
        native::groups::detail::preserveReduction(proposed,current);
        assert(proposed.allowed());
        approval={rules,850,candidate,proposed.allowed()};
    }
    static std::optional<double> length(uint64_t model){
        if(model==1)return 35.;
        if(model==2)return 36.;
        if(model==3)return 1200.;
        return {};
    }
    // This deliberately validates a different object: the native implementation
    // copies the command's vector into a stack Train before submitting mutations.
    uint32_t originalRecompose(std::span<const native::Car> temporary,uint64_t nativeRules=rules,
                               native::policy::Limit limit={850}){
        ++validatorCalls;
        std::vector<native::policy::ModelBlock> models;
        for(const auto& car:temporary)models.push_back({car.model,1});
        auto value=native::policy::evaluateAppend(limit,{},models,length);
        if(value.decision==native::policy::Decision::LimitExceeded&&
           validation::matchesApproval(approval,nativeRules,limit,temporary))
            value.decision=native::policy::Decision::Allowed;
        uint32_t code=1;
        if(value.allowed()){
            const auto recovery=memory.recover(temporary.size());
            if(recovery.recovered)code=recovery.code;
        }
        if(code==0)++mutatorCalls;
        return code;
    }
};
void recompose_contract(){
    {
        RecomposeShim shim;const auto copy=shim.candidate;
        assert(shim.originalRecompose(copy)==0);
        assert(shim.validatorCalls==1&&shim.mutatorCalls==1);
        // An approved reduction cannot erase a native incompatible-coupler flag.
        shim.memory.put(Memory::flags+61,uint8_t{});
        assert(shim.originalRecompose(copy)==4&&shim.mutatorCalls==1);
    }
    {
        RecomposeShim shim;
        auto changed=shim.candidate;changed.pop_back();
        assert(shim.originalRecompose(changed)==1&&shim.mutatorCalls==0);
        changed=shim.candidate;std::swap(changed[0],changed[1]);
        assert(shim.originalRecompose(changed)==1&&shim.mutatorCalls==0);
        changed=shim.candidate;changed.back().model=2;
        assert(shim.originalRecompose(changed)==1&&shim.mutatorCalls==0);
        changed=shim.candidate;changed.back().settings[15]=2;
        assert(shim.originalRecompose(changed)==1&&shim.mutatorCalls==0);
        changed=shim.candidate;changed.back().train=8;
        assert(shim.originalRecompose(changed)==1&&shim.mutatorCalls==0);
        assert(shim.originalRecompose(shim.candidate,RecomposeShim::rules+8)==1&&shim.mutatorCalls==0);
        assert(shim.originalRecompose(shim.candidate,RecomposeShim::rules,{849})==1&&shim.mutatorCalls==0);
        shim.approval.preflightAccepted=false;
        assert(shim.originalRecompose(shim.candidate)==1&&shim.mutatorCalls==0);
        assert(shim.validatorCalls==8);
    }
    {
        RecomposeShim shim;auto copy=shim.candidate;
        const auto before=shim.memory.data;
        assert(!validation::matchesApproval(shim.approval,0,{850},copy));
        assert(!validation::matchesApproval(shim.approval,RecomposeShim::rules,{},copy));
        assert(!validation::matchesApproval(shim.approval,RecomposeShim::rules,{10001},copy));
        shim.approval.cars={};
        assert(!validation::matchesApproval(shim.approval,RecomposeShim::rules,{850},copy));
        const auto large=std::vector<native::Car>(native::policy::maximumElements+1,native::Car{1});
        shim.approval.cars=large;
        assert(!validation::matchesApproval(shim.approval,RecomposeShim::rules,{850},large));
        assert(shim.memory.data==before);
    }
}
}
int main(){
    recompose_contract();
    {
        Memory memory;auto result=memory.recover();
        assert(result.recovered&&result.code==0&&result.flagBytes==62);
        assert(result.readIssue.fault==native::ReadFault::None&&memory.reads==3);
        for(size_t i=0;i<62;++i){
            memory.put(Memory::flags+i,uint8_t{});result=memory.recover();
            assert(result.recovered&&result.code==4&&result.flagBytes==62);
            memory.put(Memory::flags+i,uint8_t{1});
        }
    }
    {
        // The old count result is the only result this reader may reinterpret.
        for(const auto code:{uint32_t{0},uint32_t{2},uint32_t{4},uint32_t{99}}){
            Memory memory;memory.put(Memory::output,code);const auto result=memory.recover();
            assert(!result.recovered&&result.code==code&&memory.reads==1);
        }
        for(const auto cars:{size_t{0},size_t{1},size_t{30}}){
            Memory memory;const auto result=memory.recover(cars);
            assert(!result.recovered&&result.code==1&&memory.reads==1);
        }
        Memory memory;auto result=memory.recover(native::policy::maximumElements+1);
        assert(!result.recovered&&result.code==1&&memory.reads==1);
        assert(result.readIssue.fault==native::ReadFault::ElementBudget);
    }
    {
        // Truncation, an extra byte/vehicle, reversed ranges and invalid capacity
        // must never convert an unproven result into an accepted composition.
        for(const auto count:{uint64_t{0},uint64_t{60},uint64_t{61},uint64_t{63},uint64_t{64}}){
            Memory memory;memory.vector(Memory::flags,Memory::flags+count,Memory::flags+64);
            const auto result=memory.recover();
            assert(!result.recovered&&result.code==1&&memory.reads==2);
            assert(result.readIssue.fault==native::ReadFault::VectorLayout);
        }
        const std::array<std::array<uint64_t,3>,7> malformed{{
            {Memory::flags,Memory::flags-2,Memory::flags+64},
            {Memory::flags,Memory::flags+62,Memory::flags+60},
            {Memory::flags,Memory::flags+62,Memory::flags+63},
            {0,62,64},
            {0xffff,0xffff+62,0xffff+64},
            {Memory::flags,Memory::flags+62,0x7fffffff0001ULL},
            {UINT64_MAX-62,UINT64_MAX,UINT64_MAX}
        }};
        for(const auto& range:malformed){
            Memory memory;memory.vector(range[0],range[1],range[2]);const auto result=memory.recover();
            assert(!result.recovered&&result.code==1&&memory.reads==2);
            assert(result.readIssue.fault==native::ReadFault::VectorLayout);
        }
    }
    {
        for(const auto fail:{Memory::output,Memory::output+8,Memory::flags,Memory::flags+61}){
            Memory memory;memory.failAt=fail;const auto result=memory.recover();
            assert(!result.recovered&&result.code==1&&result.flagBytes==0);
            assert(result.readIssue.fault==native::ReadFault::Read);
        }
        Memory memory;memory.vector(0x18000,0x18000+62,0x18000+64);
        const auto result=memory.recover();
        assert(!result.recovered&&result.code==1&&result.readIssue.fault==native::ReadFault::Read);
    }
    {
        // The qualified upper budget is accepted without allocating a vector.
        Memory memory;const auto bytes=native::policy::maximumElements*2;
        memory.vector(Memory::flags,Memory::flags+bytes,Memory::flags+bytes);
        auto result=memory.recover(native::policy::maximumElements);
        assert(result.recovered&&result.code==0&&result.flagBytes==bytes&&memory.reads==3);
        memory.put(Memory::flags+bytes-1,uint8_t{});
        result=memory.recover(native::policy::maximumElements);
        assert(result.recovered&&result.code==4);
    }
    std::cout<<"PASS: complete native coupler flags, approved temporary recomposition, bounds and read failures\n";
}
