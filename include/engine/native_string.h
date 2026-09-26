#pragma once
#include "engine/live_state.h"
#include <array>
#include <cstring>
#include <string>

namespace nimby::engine {
// Both supported standard libraries use 32-byte std::string objects, but their
// size/capacity and inline storage fields differ. Never infer ABI from contents.
inline bool read_native_string(ReadMemory read, void* context, uint64_t address,
                               const void* object, LiveStateProfile profile, std::string& out, size_t limit = 256) {
    out.clear();
    auto word = [&](size_t offset) { uint64_t value; std::memcpy(&value,
        static_cast<const unsigned char*>(object) + offset, 8); return value; };
    const auto& layout=gameLayout(profile);
    if(!layout.root_rva)return false;
    const auto size=word(layout.string_size), data=word(0);
    const auto local_offset=layout.string_inline;
    const bool local=layout.string_inline_by_pointer
        ? data==address+local_offset : word(layout.string_capacity)==15;
    const auto capacity=local?uint64_t(15):word(layout.string_capacity);
    if (limit > 4096 || size > limit || capacity < 15 || capacity < size || capacity > 1048576) return false;
    std::string text(size+1, '\0'), again(size+1, '\0');
    if (local) std::memcpy(text.data(), static_cast<const unsigned char*>(object) + local_offset, size + 1);
    else if (data < 0x10000 || data >= 0x7fffffff0000ULL ||
             !read(context,data,text.data(),size+1) || !read(context,data,again.data(),size+1) || text != again) return false;
    if (text[size] || std::memchr(text.data(),0,size)) return false;
    std::array<unsigned char,32> after{};
    if (!read(context,address,after.data(),after.size()) || std::memcmp(object,after.data(),after.size())) return false;
    out.assign(text.data(),size); return true;
}
}
