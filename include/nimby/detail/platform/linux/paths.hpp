#pragma once
#include <filesystem>
#include <string>
#include <cstring>

namespace nimby::detail::platform {
inline std::filesystem::path fromUtf8(const std::string& text) {
    return std::filesystem::path(std::u8string_view(reinterpret_cast<const char8_t*>(text.data()),text.size()));
}
inline bool localAssetRoot(const std::filesystem::path& path) {
    return path.is_absolute();
}
// Output is caller-owned and already zero-filled; preserve room for its NUL.
inline bool pathToUtf8(const std::filesystem::path& path,char* output,size_t capacity) {
    const auto text=path.u8string();
    if(text.empty()||text.size()>=capacity)return false;
    std::memcpy(output,text.data(),text.size());
    return true;
}
}
