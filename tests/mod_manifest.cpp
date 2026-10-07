#include <loader/manifest.h>
#include <cassert>
#include <cstring>
#include <sstream>
namespace {
struct Endless final:std::streambuf {
    size_t consumed=0,calls=0;
    std::streamsize xsgetn(char* output,std::streamsize count) override {
        assert(count>0&&count<=static_cast<std::streamsize>(nimby::loader::manifest_byte_limit+1));
        std::memset(output,'x',static_cast<size_t>(count));consumed+=static_cast<size_t>(count);++calls;return count;
    }
};
std::string utf16(std::string_view text,bool little) {
    std::string result=little?"\xff\xfe":"\xfe\xff";
    for(char c:text){result.push_back(little?c:'\0');result.push_back(little?'\0':c);}return result;
}
}
int main(){
    auto parse=[](const char* text,const char* suffix=".dll",bool insensitive=true){
        std::istringstream input(text);return nimby::loader::manifest_library(input,suffix,insensitive);
    };
    assert(parse("[NRFMod]\nlibrary=Mod.dll\n")=="Mod.dll");
    assert(parse("\xef\xbb\xbf; comment\r\n[nrfmod]\r\n Library = \"Mod.DLL\" \r\n")=="Mod.DLL");
    assert(parse("[NRFMod]\nlibrary=Mod.so\n",".so",false)=="Mod.so");
    assert(parse("[nrfmod]\nlibrary=Mod.so\n",".so",false).empty());
    assert(parse("[NRFMod]\nlibrary=Mod.SO\n",".so",false).empty());
    assert(parse("[NRFMod]\nlibrary=Mod.dll\nlibrary=Other.dll").empty());
    assert(parse("[NRFMod]\nlibrary=\nlibrary=Other.dll").empty());
    for(const auto* name:{"../Mod.dll","folder/Mod.dll","folder\\Mod.dll","C:Mod.dll",".Mod.dll","a..dll","Mod.so",".dll"}) {
        std::string text="[NRFMod]\nlibrary=";text+=name;
        assert(parse(text.c_str()).empty());
    }
    assert(parse("[Other]\nlibrary=Mod.dll").empty());
    for(bool little:{true,false}) {
        auto encoded=utf16("[NRFMod]\r\nlibrary=Mod.dll\r\n",little);
        std::istringstream input(encoded);
        assert(nimby::loader::manifest_library(input,".dll",true)=="Mod.dll");
        encoded.pop_back();std::istringstream truncated(encoded);
        assert(nimby::loader::manifest_library(truncated,".dll",true).empty());
    }
    using nimby::loader::ManifestError;
    const auto limit=nimby::loader::manifest_byte_limit;
    auto check=[](const std::string& text,ManifestError expected){
        ManifestError error{};std::istringstream input(text);
        const auto result=nimby::loader::manifest_library(input,".dll",true,&error);
        assert(error==expected);assert((expected==ManifestError::none)==(result=="Mod.dll"));
    };
    // The byte cap includes BOMs and comments. A valid early library entry
    // never makes an oversized trailing line acceptable.
    for(const auto* prefix:{"[NRFMod]\nlibrary=Mod.dll\n#","\xef\xbb\xbf[NRFMod]\nlibrary=Mod.dll\n#"}){
        std::string boundary=prefix;boundary.resize(limit,'x');
        check(boundary,ManifestError::none);boundary.push_back('x');check(boundary,ManifestError::too_large);
    }
    for(bool little:{true,false}){
        std::string text="[NRFMod]\nlibrary=Mod.dll\n#";text.resize((limit-2)/2,'x');
        auto encoded=utf16(text,little);assert(encoded.size()==limit);check(encoded,ManifestError::none);
        encoded.push_back('\0');encoded.push_back('\0');check(encoded,ManifestError::too_large);
    }
    for(const auto& invalid:{std::string{},std::string("\xff",1),std::string("\xff\xff",2),std::string("\xff\xfe\0",3)})
        check(invalid,ManifestError::invalid);
    check(std::string(limit,'x'),ManifestError::invalid);
    Endless source;std::istream endless(&source);ManifestError error{};
    assert(nimby::loader::manifest_library(endless,".dll",true,&error).empty());
    assert(error==ManifestError::too_large&&source.consumed==limit+1&&source.calls==1);
    std::istringstream broken("[NRFMod]\nlibrary=Mod.dll\n");broken.setstate(std::ios::badbit);
    assert(nimby::loader::manifest_library(broken,".dll",true,&error).empty()&&error==ManifestError::io);
    std::istringstream throwing("[NRFMod]\nlibrary=Mod.dll\n");throwing.exceptions(std::ios::failbit|std::ios::badbit);
    assert(nimby::loader::manifest_library(throwing,".dll",true,&error)=="Mod.dll"&&error==ManifestError::none);
    assert(std::string_view(nimby::loader::manifest_error_message(ManifestError::too_large)).find("64 KiB")!=std::string_view::npos);
}
