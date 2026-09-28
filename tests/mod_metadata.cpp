#include <engine/mod_metadata.h>
#include <cassert>
#include <iostream>

using namespace nimby::engine;
int main(){
    const std::string bytes=R"({"format":1,"fallback":"fr","languages":{"fr":{"name":"Horloge","description":"Changer\nla date"},"en":{"name":"Clock","description":"Change\nthe date"},"pt-BR":{"name":"Relógio","description":"Data"}}})";
    auto catalog=std::make_shared<ModMetadataCatalog>(bytes);
    assert(catalog->resolve("eng").name=="Clock");
    assert(catalog->resolve("en-US").name=="Clock");
    assert(catalog->resolve("PT_br").name=="Relógio");
    assert(catalog->resolve("de")==catalog->fallback());
    assert(catalog->resolve("").description=="Changer\nla date");
    ModMetadataRegistry registry;
    registry.replace(1,"id",catalog);
    assert(!registry.find(2,"id")&&!registry.find(1,"other"));
    auto retained=registry.find(1,"id");
    registry.replace(1,"id",{});
    assert(!registry.find(1,"id")&&retained->resolve("eng").name=="Clock");
    const std::string nativeKey="nrf.sdk."+std::string(64,'a');
    const auto resource=std::string(R"({"kind":"textures","id":"bal_images","key":")")+nativeKey+
        R"(","default":"Semaphore","languages":{"fr":"Sémaphore","en":"Semaphore"}})";
    const auto resourceJson=std::string(R"({"format":2,"fallback":"fr","languages":{"fr":{"name":"Mod","description":"Desc"},"en":{"name":"Mod","description":"Desc"}},"resources":[)")+resource+"]}";
    const auto resources=std::make_shared<ModMetadataCatalog>(resourceJson);
    assert(resources->resources().size()==1);
    registry.replace(1,"signals",resources);
    const char* borrowed=registry.resolveNative(nativeKey,"Semaphore","fra");
    assert(borrowed&&std::string(borrowed)=="Sémaphore");
    assert(std::string(registry.resolveNative(nativeKey,"Semaphore","en-US"))=="Semaphore");
    assert(std::string(registry.resolveNative(nativeKey,"Semaphore","ja"))=="Sémaphore");
    assert(!registry.resolveNative(nativeKey,"Different resource","fr"));
    assert(!registry.resolveNative("native.game.key","Semaphore","fr"));
    registry.replace(2,"duplicate",resources);
    assert(!registry.resolveNative(nativeKey,"Semaphore","fr")); // A key cannot hijack another package.
    registry.replace(2,"duplicate",{});registry.replace(1,"signals",{});
    assert(!registry.resolveNative(nativeKey,"Semaphore","fr"));
    assert(std::string(borrowed)=="Sémaphore"); // The game may still hold its previous borrowed pointer.
    for(const auto bad:{
        R"({"format":2,"fallback":"fr","languages":{}})",
        R"({"format":1,"fallback":"fr","languages":{"en":{"name":"Clock","description":"Date"}}})",
        R"({"format":1,"fallback":"en","languages":{"en":{"name":"A","description":"D"},"eng":{"name":"B","description":"D"}}})",
        R"({"format":1,"fallback":"en","languages":{"en":{"name":"Bad\nname","description":"Date"}}})",
        R"({"format":1,"fallback":"en","languages":{"en":{"name":"A","description":"\u001eNRF:[bad]"}}})",
        R"({"format":1,"fallback":"en","languages":{"en":{"name":"A","name":"B","description":"Date"}}})"
    }){
        bool failed=false;try{ModMetadataCatalog value(bad);}catch(...){failed=true;}assert(failed);
    }
    std::cout<<"PASS: metadata locale aliases, fallback, multiline text, isolated identity and stale catalogue removal\n";
}
