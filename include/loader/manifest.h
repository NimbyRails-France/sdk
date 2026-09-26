#pragma once
#include <istream>
#include <sstream>
#include <string>
#include <string_view>

namespace nimby::loader {
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
inline std::string manifest_library(std::istream& input,std::string_view suffix,bool insensitive) {
    const auto first=input.peek();
    if(first!=0xff&&first!=0xfe)return parse_manifest_lines(input,suffix,insensitive);
    // Preserve Windows INI files saved as UTF-16 with a BOM. The grammar and
    // permitted module names are ASCII; non-ASCII code units stay invalid in
    // identifiers, while arbitrary Unicode comments can still be ignored.
    const bool little=input.get()==0xff;
    if(input.get()!=(little?0xfe:0xff))return {};
    std::string decoded;
    for(int a=input.get();a!=std::char_traits<char>::eof();a=input.get()) {
        const int b=input.get();
        if(b==std::char_traits<char>::eof())return {};
        const unsigned unit=little?unsigned(a)|(unsigned(b)<<8):unsigned(b)|(unsigned(a)<<8);
        decoded.push_back(unit<128?static_cast<char>(unit):'\x7f');
    }
    if(input.bad())return {};
    std::istringstream lines(decoded);
    return parse_manifest_lines(lines,suffix,insensitive);
}
}
