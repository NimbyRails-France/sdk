#pragma once
#include <filesystem>
#include <string>
#include <cstring>
#include <nimby/detail/platform/windows/system.hpp>
namespace nimby::detail::platform {
inline std::filesystem::path fromUtf8(const std::string& text) {
    const int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),nullptr,0);
    if(!n)return {};
    std::wstring result(n,L'\0');
    MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),result.data(),n);return result;
}
inline bool localAssetRoot(const std::filesystem::path& path) {
    return path.is_absolute() && path.has_root_name() && !path.native().starts_with(L"\\\\");
}
// Output is caller-owned and already zero-filled; preserve room for its NUL.
inline bool pathToUtf8(const std::filesystem::path& path,char* output,size_t capacity) {
    const auto text=path.wstring();
    const int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),nullptr,0,nullptr,nullptr);
    if(n<=0||static_cast<size_t>(n)>=capacity)return false;
    WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),output,n,nullptr,nullptr);
    return true;
}
}
