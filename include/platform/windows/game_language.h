#pragma once
#include "engine/detail/memory_reader.h"
#include <optional>
#include <string>

namespace nimby::platform::windows {
// Windows 1.19, qualified executable only (the bridge validates its SHA first).
// FUN_1402d8a50 copies the first MSVC string of *0x140b7b610; with no active
// language object it returns the three-byte code at 0x140a5ebd8. No game function
// is invoked. A replacement/failed read is unknown, never an OS-locale guess.
inline std::optional<std::string> gameLanguage(engine::ReadMemory read,void* context,uint64_t module) {
    using namespace engine::memory;
    if(!read||!module)return {};
    uint64_t object=0,after=0;
    if(!read(context,module+0xb7b610,&object,8))return {};
    std::string code;
    if(!object){
        char fallback[3]{};
        if(!read(context,module+0xa5ebd8,fallback,3))return {};
        code.assign(fallback,3);
    }else{
        if(!pointer(object))return {};
        std::array<unsigned char,32> before{},again{};
        if(!read(context,object,before.data(),before.size()))return {};
        const auto size=field<uint64_t>(before.data(),16),capacity=field<uint64_t>(before.data(),24);
        if(size<2||size>32||capacity<size||capacity>4096)return {};
        const auto address=capacity<16?object:field<uint64_t>(before.data(),0);
        if(!pointer(address)||(capacity<16&&size>=16))return {};
        code.resize(size);
        if(!read(context,address,code.data(),size)||!read(context,object,again.data(),again.size())||before!=again)return {};
    }
    if(!read(context,module+0xb7b610,&after,8)||after!=object)return {};
    for(char c:code)if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'))return {};
    return code;
}
}
