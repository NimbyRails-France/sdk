// Exercise the real discovery/translation code with owned ABI-shaped fixtures.
// These substitutes check ownership and field selection, not the game ABI.
#include "../../src/platform/windows/runtime/mod_metadata_bridge.cpp"
#include <cassert>
#include <set>
#include <iostream>

namespace fixture {
std::set<void*> allocations;
std::string renderedName,renderedDescription,renderedLabel;
uintptr_t renderedMeta{};uint32_t renderedFlags{};
void construct(void* object,const char* bytes,size_t size){
    auto* words=static_cast<uint64_t*>(object);
    std::memset(words,0,32);words[2]=size;words[3]=size<16?15:size;
    char* destination=reinterpret_cast<char*>(words);
    if(size>=16){destination=new char[size+1];allocations.insert(destination);words[0]=reinterpret_cast<uint64_t>(destination);}
    std::memcpy(destination,bytes,size);destination[size]=0;
}
void destroy(void* object){
    auto* words=static_cast<uint64_t*>(object);
    if(words[3]>=16){auto* buffer=reinterpret_cast<char*>(words[0]);assert(allocations.erase(buffer)==1);delete[] buffer;}
    std::memset(object,0,32);words[3]=15;
}
void* move(void* destination,void* source){destroy(destination);std::memcpy(destination,source,32);std::memset(source,0,32);static_cast<uint64_t*>(source)[3]=15;return destination;}
struct Meta {
    alignas(8) std::array<uint8_t,0x118> data{};
    Meta(){
        *reinterpret_cast<int*>(data.data())=1;data[0x110]=1;
        const std::string fields[]{"fixture-mod","Horloge","Unchanged author","Modifier la date de cette partie","1.2.3-alpha.1","resource-folder","unused"};
        for(size_t i=0;i<7;++i)construct(data.data()+8+i*32,fields[i].data(),fields[i].size());
    }
    ~Meta(){for(size_t i=0;i<7;++i)destroy(data.data()+8+i*32);}
    uintptr_t at(){return reinterpret_cast<uintptr_t>(data.data());}
};
}
int main(){
    constructString=fixture::construct;destroyString=fixture::destroy;moveString=fixture::move;
    originalRead=+[](uintptr_t out,int,uintptr_t){return out;};
    const auto folder=std::filesystem::temp_directory_path()/(L"NRF-MetadataFixture-"+std::to_wstring(GetCurrentProcessId()));
    assert(!std::filesystem::exists(folder));std::filesystem::create_directory(folder);
    const auto file=folder/L"nrf-metadata.json";
    const auto wide=folder.wstring();std::array<uint64_t,4> nativePath{reinterpret_cast<uint64_t>(wide.c_str()),0,wide.size(),wide.size()};
    const auto path=reinterpret_cast<uintptr_t>(nativePath.data());
    const std::string json=R"({"format":1,"fallback":"fr","languages":{"fr":{"name":"Horloge","description":"Modifier la date de cette partie"},"en":{"name":"Clock with a deliberately long name","description":"Edit date"}}})";
    {
        fixture::Meta loaded;
        std::ofstream(file,std::ios::binary)<<json;
        assert(discover(loaded.at(),1,path)==loaded.at());
        assert(registry.find(1,"fixture-mod"));
        const auto unchanged=loaded.data;
        const auto owned=fixture::allocations;
        originalDetails=+[](uintptr_t,uintptr_t,uintptr_t meta){
            fixture::renderedMeta=meta;
            fixture::renderedName=string<char>(meta+0x28,16384);
            fixture::renderedDescription=string<char>(meta+0x68,16384);
            assert(string<char>(meta+8,4096)=="fixture-mod");
            assert(string<char>(meta+0x48,4096)=="Unchanged author");
            assert(string<char>(meta+0x88,4096)=="1.2.3-alpha.1");
        };
        originalLabel=+[](uintptr_t,uintptr_t text,uint32_t flags){
            fixture::renderedLabel=string<char>(text,16384);fixture::renderedFlags=flags;
        };
        // In-game records may later go to save/enable commands. Repeated
        // language switches must leave every source byte and allocation intact.
        drawDetails(0,0,loaded.at(),"eng");
        assert(fixture::renderedMeta!=loaded.at());
        assert(fixture::renderedName=="Clock with a deliberately long name"&&fixture::renderedDescription=="Edit date");
        assert(loaded.data==unchanged&&fixture::allocations==owned);
        drawDetails(0,0,loaded.at(),"fr");assert(fixture::renderedName=="Horloge");
        drawDetails(0,0,loaded.at(),"de");assert(fixture::renderedName=="Horloge");
        drawLabel(0,loaded.at()+0x28,0x11,0x66a56a,"en");
        assert(fixture::renderedLabel=="Clock with a deliberately long name"&&fixture::renderedFlags==0x11);
        drawLabel(0,loaded.at()+0x28,0x22,0x5cc620,"en"); // Other renderers/Steam remain untouched.
        assert(fixture::renderedLabel=="Horloge"&&fixture::renderedFlags==0x22);
        assert(loaded.data==unchanged&&fixture::allocations==owned);
        auto withResources=nimby::detail::Translations::parse(json);
        const std::string key="nrf.sdk."+std::string(64,'b');
        withResources["format"]=2;
        withResources["resources"]={{{"kind","textures"},{"id","bal_images"},{"key",key},{"default","BAL semaphore"},
            {"languages",{{"fr","Sémaphore BAL"},{"en","BAL semaphore"}}}}};
        std::ofstream(file)<<withResources.dump();discover(loaded.at(),1,path);
        drawDetails(0,0,loaded.at(),"fr");
        alignas(8) std::array<uint8_t,0x68> rule{};
        *reinterpret_cast<int*>(rule.data()+0x20)=10;
        fixture::construct(rule.data()+0x28,"bal_images",10);
        fixture::construct(rule.data()+0x48,"BAL semaphore",13);
        const auto labelAddress=reinterpret_cast<uintptr_t>(rule.data()+0x48);
        const auto ruleBefore=rule;
        drawLabel(0,labelAddress,0x11,0x66ad59,"fr");assert(fixture::renderedLabel=="Sémaphore BAL");
        drawLabel(0,labelAddress,0x11,0x66ad59,"en");assert(fixture::renderedLabel=="BAL semaphore");
        assert(rule==ruleBefore);
        *reinterpret_cast<int*>(rule.data()+0x20)=0; // Same name in a train catalogue isn't our signal resource.
        drawLabel(0,labelAddress,0x11,0x66ad59,"fr");assert(fixture::renderedLabel=="BAL semaphore");
        fixture::destroy(rule.data()+0x28);fixture::destroy(rule.data()+0x48);
        originalLocalize=+[](const char*,const char* fallback){return fallback;};
        assert(std::string(localize(key.c_str(),"BAL semaphore"))=="Sémaphore BAL"); // Unavailable language -> declared fallback.
        assert(std::string(localize("native.game.key","Game text"))=="Game text");
        assert(std::string(localize(key.c_str(),"Other label"))=="Other label");
        translate(loaded.at(),"eng");
        assert(string<char>(loaded.at()+0x28,4096)=="Clock with a deliberately long name");
        assert(string<char>(loaded.at()+0x68,4096)=="Edit date");
        assert(string<char>(loaded.at()+8,4096)=="fixture-mod");
        for(size_t i=0;i<loaded.data.size();++i)if(!(i>=0x28&&i<0x48)&&!(i>=0x68&&i<0x88))assert(loaded.data[i]==unchanged[i]);
        fixture::Meta nextCopy;
        translate(nextCopy.at(),"fr");assert(string<char>(nextCopy.at()+0x28,4096)=="Horloge");
        withResources["format"]=3;
        withResources["construction"]={{{"textures","bal_images"},{"side","left"}}};
        hashString=+[](const void* native){assert(string<char>(reinterpret_cast<uintptr_t>(native),96)=="bal_images");return uint64_t(123);};
        std::ofstream(file)<<withResources.dump();discover(nextCopy.at(),1,path);
        assert(constructionDefaults.left(123)&&!constructionDefaults.left(124));
        originalBuildSignal=+[](uintptr_t,uintptr_t out,uintptr_t,uintptr_t,uintptr_t,uintptr_t,uint8_t){return out;};
        std::array<uint8_t,0x150> editor{};
        for(int direction:{-1,1})for(uint8_t orientation:{0,1,2}){
            std::array<uint8_t,0x64> fresh{};
            uint64_t hash=123,track=456;std::memcpy(fresh.data()+0x38,&hash,8);std::memcpy(fresh.data()+0x40,&track,8);
            fresh[0x50]=uint8_t(direction);fresh[0x51]=orientation;fresh[0x5c]=4;
            const auto before=fresh;
            const auto out=reinterpret_cast<uintptr_t>(fresh.data());
            assert(buildSignal(0,out,0,0,0,reinterpret_cast<uintptr_t>(editor.data()),0)==out);
            int32_t side{};std::memcpy(&side,fresh.data()+0x58,4);assert(side==-direction);
            for(size_t i=0;i<fresh.size();++i)if(i<0x58||i>=0x5c)assert(fresh[i]==before[i]);
            // A placed signal, a copied source, an unknown catalogue or an
            // invalid track/direction must retain every byte.
            for(int invalid=0;invalid<5;++invalid){
                fresh=before;editor[0x148]=0;
                if(invalid==0)fresh[0]=8;
                if(invalid==1)editor[0x148]=8;
                if(invalid==2)fresh[0x38]=124;
                if(invalid==3)fresh[0x50]=0;
                if(invalid==4)std::memset(fresh.data()+0x40,0,8);
                const auto unchanged=fresh;
                buildSignal(0,out,0,0,0,reinterpret_cast<uintptr_t>(editor.data()),0);
                assert(fresh==unchanged);
            }
            editor[0x148]=0;
        }
        constructionDefaults.replace(2,"duplicate",{123});assert(!constructionDefaults.left(123));
        constructionDefaults.replace(2,"duplicate",{});assert(constructionDefaults.left(123));
        // Removing a sidecar invalidates the registry. A later invalid or
        // mismatched file must not revive a previous mod's translated text.
        std::filesystem::remove(file);discover(nextCopy.at(),1,path);assert(!registry.find(1,"fixture-mod"));
        assert(!constructionDefaults.left(123));
        drawDetails(0,0,nextCopy.at(),"en");assert(fixture::renderedMeta==nextCopy.at()&&fixture::renderedName=="Horloge");
        std::ofstream(file)<<"invalid";discover(nextCopy.at(),1,path);assert(!registry.find(1,"fixture-mod"));
        std::ofstream(file)<<json;
        fixture::destroy(nextCopy.data.data()+0x28);fixture::construct(nextCopy.data.data()+0x28,"Other mod",9);
        discover(nextCopy.at(),1,path);assert(!registry.find(1,"fixture-mod"));
    }
    assert(fixture::allocations.empty());
    std::filesystem::remove(file);std::filesystem::remove(folder);
    std::cout<<"PASS: real metadata discovery, UTF-16 path, copy-only fields, native allocation ownership and invalidation\n";
}
