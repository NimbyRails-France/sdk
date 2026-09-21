#include "engine/binary_identity.h"
#include "platform/linux/process.h"
#include <cstring>
#include <filesystem>

namespace nimby::engine {
bool is_research_build(const NimbyBinaryInfo& info) noexcept {
    return info.file_size==20787376 && std::strcmp(info.sha256,
        "2581d0e8157f43acb137b2bd9d52e2a7c82bd8af8b62fab5d87f00cc27eefde6")==0;
}
uint32_t identify(const wchar_t* path,NimbyBinaryInfo& out) noexcept {
    out={};out.struct_size=sizeof out;
    if(!path||!*path)return NIMBY_INVALID_ARGUMENT;
    try {
        std::string utf8;
        for(;*path;++path) {
            const auto cp=static_cast<uint32_t>(*path);
            if(cp>0x10ffff||(cp>=0xd800&&cp<=0xdfff))return NIMBY_INVALID_ARGUMENT;
            if(cp<0x80)utf8.push_back(static_cast<char>(cp));
            else {
                if(cp<0x800)utf8.push_back(static_cast<char>(0xc0|(cp>>6)));
                else {
                    if(cp<0x10000)utf8.push_back(static_cast<char>(0xe0|(cp>>12)));
                    else {utf8.push_back(static_cast<char>(0xf0|(cp>>18)));utf8.push_back(static_cast<char>(0x80|((cp>>12)&63)));}
                    utf8.push_back(static_cast<char>(0x80|((cp>>6)&63)));
                }
                utf8.push_back(static_cast<char>(0x80|(cp&63)));
            }
        }
        if(!std::filesystem::is_regular_file(utf8))return NIMBY_IO_ERROR;
        const auto identity=nimby::platform::linux_os::identify(utf8);
        out.file_size=identity.size;std::memcpy(out.sha256,identity.sha256.c_str(),65);
        out.recognized_research_build=is_research_build(out);
        return NIMBY_OK;
    }catch(...){return NIMBY_INVALID_BINARY;}
}
}
