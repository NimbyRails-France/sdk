#pragma once
#include <windows.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <initializer_list>

namespace nimby::platform::windows {
// Native callers can know that a particular integer-returning leaf preserves
// volatile XMM0..5. A general C++ helper cannot make that promise. This gateway
// preserves them around the helper, forwarding the Windows register arguments
// unchanged and retaining its integer result in RAX. Stack arguments and a
// floating-point result are not supported by this leaf-specific gateway.
//
// The owner must remove every hook and join every caller before reset(). There
// is intentionally no destructor cleanup: freeing active generated code or its
// unwind entry during DLL detach would be unsafe. The function table is stored
// in this immovable object and must outlive every call and unwind.
class NativeLeafGateway {
public:
    NativeLeafGateway()=default;
    NativeLeafGateway(const NativeLeafGateway&)=delete;
    NativeLeafGateway& operator=(const NativeLeafGateway&)=delete;
    NativeLeafGateway(NativeLeafGateway&&)=delete;
    NativeLeafGateway& operator=(NativeLeafGateway&&)=delete;

    bool build(void* helper)noexcept {
        static_assert(sizeof(void*)==8,"NativeLeafGateway requires Windows x64");
        if(!helper)return false;
        if(memory_)return registered_&&helper_==helper;
        std::array<uint8_t,128> code{};
        size_t count=0;
        const auto emit=[&](std::initializer_list<uint8_t> bytes){
            for(const auto byte:bytes)code[count++]=byte;
        };
        // At entry RSP is 8 mod 16. Reserve 32-byte shadow space, six 16-byte
        // vectors and 8 bytes of alignment; RSP is 16-byte aligned at CALL.
        emit({0x48,0x81,0xec,0x88,0,0,0}); // sub rsp,136
        for(uint8_t i=0;i<6;++i)
            emit({0xf3,0x0f,0x7f,uint8_t(0x44+(i<<3)),0x24,uint8_t(0x20+i*16)});
        emit({0x48,0xb8}); // mov rax,helper
        const auto target=reinterpret_cast<uint64_t>(helper);
        for(unsigned i=0;i<8;++i)emit({uint8_t(target>>(i*8))});
        emit({0xff,0xd0}); // call rax
        for(uint8_t i=0;i<6;++i)
            emit({0xf3,0x0f,0x6f,uint8_t(0x44+(i<<3)),0x24,uint8_t(0x20+i*16)});
        emit({0x48,0x81,0xc4,0x88,0,0,0,0xc3}); // add rsp,136; ret
        if(count!=99)return false;

        auto* memory=static_cast<uint8_t*>(VirtualAlloc(nullptr,pageBytes,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));
        if(!memory)return false;
        std::memcpy(memory,code.data(),count);
        // Version 1, 7-byte prologue, two slots. UWOP_ALLOC_LARGE, OpInfo 0,
        // stores the allocation in units of eight bytes: 136/8 = 17.
        constexpr uint8_t unwind[]{1,7,2,0,7,1,17,0};
        std::memcpy(memory+unwindOffset,unwind,sizeof unwind);
        table_={0,static_cast<DWORD>(count),unwindOffset};
        DWORD previous{};
        if(!VirtualProtect(memory,pageBytes,PAGE_EXECUTE_READ,&previous)||
           !FlushInstructionCache(GetCurrentProcess(),memory,count)||
           !RtlAddFunctionTable(&table_,1,reinterpret_cast<DWORD64>(memory))){
            VirtualFree(memory,0,MEM_RELEASE);table_={};return false;
        }
        memory_=memory;helper_=helper;registered_=true;
        return true;
    }

    void* address()const noexcept{return registered_?memory_:nullptr;}

    bool reset()noexcept {
        if(!memory_)return true;
        // Never release code while its unwind entry is still registered.
        if(registered_){
            if(!RtlDeleteFunctionTable(&table_))return false;
            registered_=false;
        }
        if(!VirtualFree(memory_,0,MEM_RELEASE))return false;
        memory_=nullptr;helper_=nullptr;table_={};
        return true;
    }
private:
    static constexpr size_t pageBytes=4096;
    static constexpr DWORD unwindOffset=256;
    uint8_t* memory_{};
    void* helper_{};
    RUNTIME_FUNCTION table_{};
    bool registered_=false;
};
}
