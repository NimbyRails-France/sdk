#include <platform/windows/mod_options_host.h>
#include <runtime/mod_options_codec.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/observation.h>
#include <windows.h>
#include <cassert>
#include <fstream>
#include <thread>
#include <iostream>

namespace service=nimby::platform::windows::mod_options;
namespace options=nimby::runtime::mod_options;
using Json=nlohmann::json;
const std::string declaration=R"({"id":"test-mod","title":"Test","fields":[
    {"id":"enabled","label":"Enabled","description":"","kind":0,"default":"true"},
    {"id":"count","label":"Count","kind":1,"default":"4","minimum":1,"maximum":8},
    {"id":"window.main","label":"Window","kind":3,"default":"F8"}]})";
std::filesystem::path savedFile(const std::filesystem::path& folder){
    for(const auto& file:std::filesystem::directory_iterator(folder))if(file.path().extension()==".json")return file.path();
    return {};
}
int main(){
    // Canonicalize the existing parent once. Containment of this generated
    // child is lexical; Wine need not supply native directory file identities.
    const auto temporary=std::filesystem::canonical(std::filesystem::temp_directory_path());
    const auto root=temporary/("nrf-options-test-"+std::to_string(GetCurrentProcessId())+"-"+std::to_string(GetTickCount64()));
    assert(root.is_absolute()&&root.parent_path()==temporary&&root.filename().string().starts_with("nrf-options-test-"));
    // Cleanup owns only a directory this test actually created.
    assert(std::filesystem::create_directory(root));
    struct Cleanup{std::filesystem::path path;~Cleanup(){std::error_code error;std::filesystem::remove_all(path,error);}} cleanup{root};
    uint64_t first{};
    {
        service::Host host(root);auto added=host.add(declaration);assert(added);first=added.token;
        assert(!host.add(declaration));
        std::array<char,NIMBY_OPTIONS_VALUES_LIMIT> buffer{};uint32_t bytes{};uint64_t revision{};
        assert(host.read(first,0,buffer.data(),buffer.size(),&bytes,&revision)==NIMBY_OK&&bytes&&revision);
        auto value=Json::parse(buffer.data(),buffer.data()+bytes);assert(value["values"]==Json::array({"true","4","F8"}));
        assert(host.read(first,revision,buffer.data(),buffer.size(),&bytes,&revision)==NIMBY_OK&&!bytes);
        assert(host.registry.listNativeBindings({}));
        assert(host.change(first,"count","7"));assert(host.change(first,"enabled","false"));
        assert(!host.change(first,"count","9"));
        assert(host.registry.dispatch({options::shortcuts::Key::F8,0,false},{true,false,false})==1);
        assert(host.read(first,0,buffer.data(),1,&bytes,&revision)==NIMBY_INVALID_ARGUMENT);
        assert(host.read(first,0,buffer.data(),buffer.size(),&bytes,&revision)==NIMBY_OK);
        value=Json::parse(buffer.data(),buffer.data()+bytes);assert(value["events"]==Json::array({"window.main"}));
        assert(value["values"]==Json::array({"false","7","F8"}));
        // Removal must persist even an edit not yet seen by the async writer.
        assert(host.change(first,"count","8"));assert(host.remove(first));
        assert(host.read(first,0,buffer.data(),buffer.size(),&bytes,&revision)==NIMBY_INVALID_HANDLE);
    }
    auto file=savedFile(root);assert(!file.empty());
    {
        service::Host host(root);const auto added=host.add(declaration);assert(added);
        assert(host.registry.snapshot()->mods[0].values==std::vector<std::string>({"false","8","F8"}));
        assert(host.change(added.token,"count","2"));
        // A preference reaches disk while the mod remains loaded.
        bool stored=false;
        for(int i=0;i<100&&!stored;++i){
            try{std::ifstream input(file);std::string data((std::istreambuf_iterator<char>(input)),{});stored=options::parseSaved(data).at("count")=="2";}catch(...){}
            if(!stored)std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(stored);assert(host.storageError(added.token).empty());
    }
    // Removed/unknown fields survive an upgrade; invalid current values fall
    // back independently without discarding the other valid preferences.
    {std::ofstream output(file);output<<R"({"version":1,"values":{"count":"999","enabled":"false","oldField":"kept"}})";}
    {
        service::Host host(root);const auto added=host.add(declaration);assert(added);
        assert(host.registry.snapshot()->mods[0].values[1]=="4");
        assert(host.change(added.token,"count","3"));assert(host.remove(added.token));
    }
    {std::ifstream input(file);std::string data((std::istreambuf_iterator<char>(input)),{});assert(options::parseSaved(data).at("oldField")=="kept");}
    // Corrupt originals are preserved before replacement, never silently lost.
    {std::ofstream output(file);output<<"{ broken original";}
    {
        service::Host host(root);const auto added=host.add(declaration);assert(added);
        assert(!host.storageError(added.token).empty());assert(host.change(added.token,"count","6"));assert(host.remove(added.token));
    }
    bool preserved=false;
    for(const auto& entry:std::filesystem::directory_iterator(root))if(entry.path().string().find(".invalid-")!=std::string::npos){
        std::ifstream input(entry.path());std::string text((std::istreambuf_iterator<char>(input)),{});assert(text=="{ broken original");preserved=true;
    }
    assert(preserved);
    // Disk sharing failure during unload must neither lose the final edit nor
    // restore old disk values if this same mod is reloaded before recovery.
    {
        service::Host host(root);auto added=host.add(declaration);assert(added);
        const HANDLE locked=CreateFileW(file.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
        assert(locked!=INVALID_HANDLE_VALUE);
        assert(host.change(added.token,"count","7"));assert(host.remove(added.token));
        added=host.add(declaration);assert(added);
        assert(host.registry.snapshot()->mods[0].values[1]=="7");
        assert(host.change(added.token,"count","8"));assert(host.remove(added.token));
        CloseHandle(locked);
        bool recovered=false;
        for(int i=0;i<150&&!recovered;++i){
            try{std::ifstream input(file);std::string data((std::istreambuf_iterator<char>(input)),{});recovered=options::parseSaved(data).at("count")=="8";}catch(...){}
            if(!recovered)std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(recovered);
    }
    // Two differently cased identifiers must not share a Windows filename.
    {
        service::Host host(root);auto upper=Json::parse(declaration);upper["id"]="Test-Mod";
        auto a=host.add(declaration),b=host.add(upper.dump());assert(a&&b);
        assert(host.change(a.token,"count","1"));assert(host.change(b.token,"count","5"));
        assert(host.remove(a.token));assert(host.remove(b.token));
        a=host.add(declaration);b=host.add(upper.dump());assert(a&&b);
        const auto state=host.registry.snapshot();
        for(const auto& entry:state->mods)assert(entry.values[1]==(entry.id=="test-mod"?"1":"5"));
    }
    // Catalogue publication has its own revision: a registry entry may be
    // observed before its translations arrive, and the UI must then redraw.
    {
        service::Host host(root/"catalogues");
        const auto initial=host.catalogueRevision();
        const std::string title="\x1eNRF:[\"title\",{}]";
        auto localized=Json::parse(declaration);localized["title"]=title;
        localized["translations"]=R"({"fallback":"fr","languages":{"fr":{"title":"Premier"},"en":{"title":"First"}}})";
        const auto a=host.add(localized.dump());assert(a);
        const auto published=host.catalogueRevision();assert(published>initial);
        assert(host.translate(a.token,title,"fr")=="Premier"&&host.translate(a.token,title,"en")=="First");
        localized["id"]="other-catalogue";
        localized["translations"]=R"({"fallback":"fr","languages":{"fr":{"title":"Second"},"en":{"title":"Second EN"}}})";
        const auto b=host.add(localized.dump());assert(b&&host.catalogueRevision()>published);
        assert(host.translate(a.token,title,"fr")=="Premier"&&host.translate(b.token,title,"fr")=="Second");
        const auto beforeRemoval=host.catalogueRevision();
        assert(host.remove(b.token));assert(host.catalogueRevision()>beforeRemoval);
        assert(host.translate(a.token,title,"en")=="First");
        assert(host.translate(b.token,title,"en")!="Second EN");
    }
    std::cout<<"Mod option persistence, isolation, translated catalogues and broker snapshots: PASS\n";
}
