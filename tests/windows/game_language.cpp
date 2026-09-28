#include "platform/windows/game_language.h"
#include <iostream>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Game language reader check failed");}while(false)
struct Memory {
    static constexpr uint64_t base=0x140000000,object=0x100000;
    uint64_t pointer=object;bool changed=false;std::array<unsigned char,32> bytes{};
    Memory(){std::memcpy(bytes.data(),"fra",3);uint64_t n=3,c=15;std::memcpy(bytes.data()+16,&n,8);std::memcpy(bytes.data()+24,&c,8);}
    static bool read(void* ctx,uint64_t at,void* out,size_t n){auto& m=*static_cast<Memory*>(ctx);
        if(at==base+0xb7b610&&n==8){std::memcpy(out,&m.pointer,8);if(m.changed)m.pointer+=8;return true;}
        if(at==base+0xa5ebd8&&n==3){std::memcpy(out,"eng",3);return true;}
        if(at==object&&n<=32){std::memcpy(out,m.bytes.data(),n);return true;}return false;
    }
};
void language(){
    using nimby::platform::windows::gameLanguage;Memory m;
    CHECK(gameLanguage(Memory::read,&m,Memory::base)=="fra");
    m.pointer=0;CHECK(gameLanguage(Memory::read,&m,Memory::base)=="eng");
    m.pointer=Memory::object;m.changed=true;CHECK(!gameLanguage(Memory::read,&m,Memory::base));
    m=Memory{};m.bytes[16]=100;CHECK(!gameLanguage(Memory::read,&m,Memory::base));
    m=Memory{};m.bytes[0]=0;CHECK(!gameLanguage(Memory::read,&m,Memory::base));
}
int main(){language();std::cout<<"PASS: Windows game language reader\n";}
