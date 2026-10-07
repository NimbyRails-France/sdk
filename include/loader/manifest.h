#pragma once
#include <array>
#include <istream>
#include <sstream>
#include <string>
#include <string_view>

namespace nimby::loader {
inline constexpr size_t manifest_byte_limit=64*1024;
enum class ManifestError {none,invalid,too_large,io};
inline const char* manifest_error_message(ManifestError error) noexcept {
    switch(error){
    case ManifestError::too_large:return "manifest exceeds the 64 KiB byte limit";
    case ManifestError::io:return "manifest could not be read";
    default:return "invalid mod library manifest";
    }
}
// Manifest syntax is shared across hosts. Only suffix/case rules come from the
// native module backend. Paths are single ASCII filenames, never directories.
inline std::string trim_manifest(std::string value) {
    const auto first=value.find_first_not_of(" \t\r");
    return first==std::string::npos?std::string{}:value.substr(first,value.find_last_not_of(" \t\r")-first+1);
}
inline bool manifest_equal(std::string_view a,std::string_view b,bool insensitive) {
    if(a.size()!=b.size())return false;
    auto fold=[&](char c){return insensitive&&c>='A'&&c<='Z'?char(c-'A'+'a'):c;};
    for(size_t i=0;i<a.size();++i)if(fold(a[i])!=fold(b[i]))return false;
    return true;
}
inline std::string parse_manifest_lines(std::istream& input,std::string_view suffix,bool insensitive) {
    std::string line,name;bool section=false,seen=false,first=true;
    while(std::getline(input,line)) {
        if(first&&line.starts_with("\xef\xbb\xbf"))line.erase(0,3);
        first=false;line=trim_manifest(std::move(line));
        if(line.empty()||line.front()==';'||line.front()=='#')continue;
        if(line.front()=='['){section=manifest_equal(line,"[NRFMod]",insensitive);continue;}
        const auto equal=line.find('=');
        if(!section||equal==std::string::npos||!manifest_equal(trim_manifest(line.substr(0,equal)),"library",insensitive))continue;
        if(seen)return {}; // Reject ambiguity, including an empty first value.
        seen=true;name=trim_manifest(line.substr(equal+1));
        if(name.size()>=2&&((name.front()=='"'&&name.back()=='"')||(name.front()=='\''&&name.back()=='\'')))
            name=name.substr(1,name.size()-2);
    }
    if(input.bad()||name.size()<=suffix.size()||name.size()>=200||name.front()=='.'||
       name.find("..")!=std::string::npos||
       name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789._-")!=std::string::npos||
       !manifest_equal(std::string_view(name).substr(name.size()-suffix.size()),suffix,insensitive))return {};
    return name;
}
inline std::string manifest_library(std::istream& input,std::string_view suffix,bool insensitive,
                                    ManifestError* error=nullptr) {
    if(error)*error=ManifestError::invalid;
    // Limit bytes from the actual stream, not a racy filesystem size. Even an
    // endless stream or one giant line consumes only this fixed buffer; the
    // extra byte distinguishes an exact-limit file from an oversized file.
    std::array<char,manifest_byte_limit+1> bytes;
    try {input.read(bytes.data(),static_cast<std::streamsize>(bytes.size()));}
    catch(const std::ios_base::failure&){
        if(input.bad()||!input.eof()){if(error)*error=ManifestError::io;return {};}
    }
    const auto count=static_cast<size_t>(input.gcount());
    if(count>manifest_byte_limit){if(error)*error=ManifestError::too_large;return {};}
    if(input.bad()||(input.fail()&&!input.eof())){if(error)*error=ManifestError::io;return {};}
    if(!count)return {};
    const auto first=static_cast<unsigned char>(bytes[0]);
    std::string decoded;
    if(first!=0xff&&first!=0xfe)decoded.assign(bytes.data(),count);
    else {
        // Preserve Windows INI files saved as UTF-16 with a BOM. The grammar and
        // permitted module names are ASCII; non-ASCII code units stay invalid in
        // identifiers, while arbitrary Unicode comments can still be ignored.
        const bool little=first==0xff;
        if(count<2||static_cast<unsigned char>(bytes[1])!=(little?0xfe:0xff)||(count%2))return {};
        decoded.reserve((count-2)/2);
        for(size_t i=2;i<count;i+=2) {
            const auto a=static_cast<unsigned char>(bytes[i]),b=static_cast<unsigned char>(bytes[i+1]);
            const unsigned unit=little?unsigned(a)|(unsigned(b)<<8):unsigned(b)|(unsigned(a)<<8);
            decoded.push_back(unit<128?static_cast<char>(unit):'\x7f');
        }
    }
    std::istringstream lines(std::move(decoded));
    auto library=parse_manifest_lines(lines,suffix,insensitive);
    if(error&&!library.empty())*error=ManifestError::none;
    return library;
}
}
