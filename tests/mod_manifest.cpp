#include <loader/manifest.h>
#include <cassert>
#include <sstream>
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
        std::string utf16=little?"\xff\xfe":"\xfe\xff";
        for(char c:std::string("[NRFMod]\r\nlibrary=Mod.dll\r\n")){
            utf16.push_back(little?c:'\0');utf16.push_back(little?'\0':c);
        }
        std::istringstream input(utf16);
        assert(nimby::loader::manifest_library(input,".dll",true)=="Mod.dll");
        utf16.pop_back();std::istringstream truncated(utf16);
        assert(nimby::loader::manifest_library(truncated,".dll",true).empty());
    }
}
