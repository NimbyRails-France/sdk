#include <platform/windows/native_leaf_gateway.h>
#include <array>
#include <bit>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#define CHECK(value) do{if(!(value))throw std::runtime_error("Check failed: " #value);}while(false)
namespace {
using Gateway=nimby::platform::windows::NativeLeafGateway;
struct Code {
    void* memory{};
    explicit Code(const std::vector<uint8_t>& bytes){
        memory=VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);CHECK(memory);
        std::memcpy(memory,bytes.data(),bytes.size());DWORD previous{};
        CHECK(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&previous));
        CHECK(FlushInstructionCache(GetCurrentProcess(),memory,bytes.size()));
    }
    ~Code(){if(memory)VirtualFree(memory,0,MEM_RELEASE);}
    Code(const Code&)=delete;
};
void emit(std::vector<uint8_t>& code,std::initializer_list<uint8_t> bytes){code.insert(code.end(),bytes);}
void immediate(std::vector<uint8_t>& code,uint64_t value,unsigned bytes=8){
    for(unsigned i=0;i<bytes;++i)code.push_back(uint8_t(value>>(i*8)));
}
struct Capture {
    std::array<std::array<uint8_t,16>,6> input{},output{};
    uint64_t result{};
};
struct HelperCapture {uint64_t stackPointer{},first{},second{};};
constexpr uint64_t firstArgument=0x1122334455667788,secondArgument=0x8877665544332211,
    helperResult=0x1847362518273645;

std::vector<uint8_t> clobberingHelper(HelperCapture& captured){
    std::vector<uint8_t> code;
    emit(code,{0x48,0xb8});immediate(code,reinterpret_cast<uint64_t>(&captured));
    emit(code,{0x48,0x89,0x20,0x48,0x89,0x48,0x08,0x48,0x89,0x50,0x10}); // capture RSP,RCX,RDX
    // Exercise all four shadow slots without touching the saved XMM vectors.
    for(uint8_t i=0;i<4;++i){emit(code,{0x48,0xc7,0x44,0x24,uint8_t(8+i*8)});immediate(code,0x5a5a5a5a,4);}
    for(uint8_t i=0;i<6;++i)emit(code,{0x66,0x0f,0xef,uint8_t(0xc0+i*9)}); // pxor XMMi,XMMi
    emit(code,{0x48,0xb8});immediate(code,helperResult);emit(code,{0xc3});
    return code;
}

std::vector<uint8_t> captureCaller(void* gateway){
    std::vector<uint8_t> code;
    emit(code,{0x48,0x83,0xec,0x28,0x48,0x89,0x4c,0x24,0x20}); // shadow+context, save RCX
    for(uint8_t i=0;i<6;++i){
        emit(code,{0xf3,0x0f,0x6f,uint8_t(0x81+i*8)});immediate(code,offsetof(Capture,input)+i*16,4);
    }
    emit(code,{0x48,0xb9});immediate(code,firstArgument);
    emit(code,{0x48,0xba});immediate(code,secondArgument);
    emit(code,{0x48,0xb8});immediate(code,reinterpret_cast<uint64_t>(gateway));emit(code,{0xff,0xd0});
    emit(code,{0x48,0x8b,0x4c,0x24,0x20,0x48,0x89,0x81});immediate(code,offsetof(Capture,result),4);
    for(uint8_t i=0;i<6;++i){
        emit(code,{0xf3,0x0f,0x7f,uint8_t(0x81+i*8)});immediate(code,offsetof(Capture,output)+i*16,4);
    }
    emit(code,{0x48,0x83,0xc4,0x28,0xc3});
    return code;
}

void checkUnwind(void* address){
    const auto start=reinterpret_cast<DWORD64>(address);
    DWORD64 imageBase{};
    const auto function=RtlLookupFunctionEntry(start+43,&imageBase,nullptr);
    CHECK(function&&imageBase==start&&function->BeginAddress==0&&function->EndAddress==99);
    const auto info=static_cast<const uint8_t*>(address)+function->UnwindData;
    CHECK(info[0]==1&&info[1]==7&&info[2]==2&&info[4]==7&&info[5]==1&&info[6]==17);
    alignas(16) std::array<uint64_t,64> stack{};
    auto* entry=stack.data()+41; // Entry RSP is 8 mod 16.
    constexpr uint64_t returnAddress=0x123456789abcdef0;
    *entry=returnAddress;
    for(const auto codeOffset:{DWORD64{0},DWORD64{43},DWORD64{91}}){
        CONTEXT context{};context.ContextFlags=CONTEXT_FULL;
        context.Rip=start+codeOffset;
        context.Rsp=reinterpret_cast<DWORD64>(codeOffset?entry-17:entry);
        PVOID handlerData{};DWORD64 establisher{};
        RtlVirtualUnwind(UNW_FLAG_NHANDLER,imageBase,context.Rip,function,&context,
            &handlerData,&establisher,nullptr);
        CHECK(context.Rip==returnAddress);
        CHECK(context.Rsp==reinterpret_cast<DWORD64>(entry+1));
    }
}
}

int main(){
    try{
        Gateway gateway;CHECK(!gateway.address());CHECK(!gateway.build(nullptr));CHECK(gateway.reset());
        HelperCapture recorded;
        Code helper(clobberingHelper(recorded));
        CHECK(gateway.build(helper.memory));const auto address=gateway.address();CHECK(address);
        CHECK(gateway.build(helper.memory));CHECK(gateway.address()==address);
        CHECK(!gateway.build(reinterpret_cast<void*>(uintptr_t{1})));CHECK(gateway.address()==address);
        MEMORY_BASIC_INFORMATION protection{};
        CHECK(VirtualQuery(address,&protection,sizeof protection)==sizeof protection);
        CHECK(protection.Protect==PAGE_EXECUTE_READ);
        Code caller(captureCaller(address));
        const auto capture=std::bit_cast<void(*)(Capture*)>(caller.memory);
        for(unsigned sample=0;sample<64;++sample){
            Capture values;
            for(size_t i=0;i<6;++i)for(size_t j=0;j<16;++j)
                values.input[i][j]=uint8_t(1+((sample*17+i*31+j*11)%254));
            capture(&values);
            CHECK(values.output==values.input);
            CHECK(values.result==helperResult);
            CHECK((recorded.stackPointer&15)==8);
            CHECK(recorded.first==firstArgument&&recorded.second==secondArgument);
        }
        checkUnwind(address);
        CHECK(gateway.reset());CHECK(!gateway.address());CHECK(gateway.reset());
        DWORD64 base{};CHECK(!RtlLookupFunctionEntry(reinterpret_cast<DWORD64>(address)+43,&base,nullptr));
        CHECK(gateway.build(helper.memory));CHECK(gateway.address());CHECK(gateway.reset());
        std::cout<<"PASS: native leaf preserves XMM0..5/RAX, arguments, shadow space, alignment, RX protection and unwind lifecycle\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
