#include "engine/native_string.h"
#include <cassert>
#include <map>
#include <vector>
#include <iostream>
using namespace nimby::engine;
struct Memory {
    std::map<uint64_t,std::vector<unsigned char>> bytes;
    bool changed=false;
};
static bool read(void* context,uint64_t address,void* output,size_t size) {
    auto& memory=*static_cast<Memory*>(context);
    auto entry=memory.bytes.find(address);
    if(entry==memory.bytes.end() || size>entry->second.size())return false;
    std::memcpy(output,entry->second.data(),size);
    if(memory.changed && size==32)static_cast<unsigned char*>(output)[0]^=1;
    return true;
}
int main() {
    for(auto profile:{LiveStateProfile::Windows119,LiveStateProfile::Linux119}) {
        for(const std::string& text:{std::string("TER"),std::string("Une ligne avec un nom long")}) {
            Memory memory;
            auto& object=memory.bytes[0x20000];object.resize(32);
            auto word=[&](size_t at,uint64_t value){std::memcpy(object.data()+at,&value,8);};
            const bool small=text.size()<=15;
            if(profile==LiveStateProfile::Windows119) {
                word(16,text.size());word(24,small?15:64);
                if(small)std::memcpy(object.data(),text.c_str(),text.size()+1);else word(0,0x30000);
            }else {
                word(0,small?0x20010:0x30000);word(8,text.size());
                if(small)std::memcpy(object.data()+16,text.c_str(),text.size()+1);else word(16,64);
            }
            memory.bytes[0x30000]={text.begin(),text.end()};memory.bytes[0x30000].push_back(0);
            std::string out;
            assert(read_native_string(read,&memory,0x20000,object.data(),profile,out)&&out==text);
            memory.changed=true;
            assert(!read_native_string(read,&memory,0x20000,object.data(),profile,out)&&out.empty());
            memory.changed=false;
            word(profile==LiveStateProfile::Windows119?16:8,257);
            assert(!read_native_string(read,&memory,0x20000,object.data(),profile,out));
        }
    }
    std::cout<<"MSVC and GNU string layouts, heap/inline storage, bounds and replacement passed\n";
}
