#include <platform/windows/mod_options_host.h>
#include <platform/windows/runtime/driving_preferences.h>
#include <runtime/mod_options_codec.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/observation.h>
#include <windows.h>
#include <cassert>
#include <fstream>
#include <thread>
#include <iostream>
#include "allocation_fault_fixture.h"

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
    {
        // Keep UI publications changing while the real storage worker saves an
        // older immutable snapshot. Flush must retain that snapshot's owner.
        const auto concurrentRoot=root/"concurrent-flush";
        {
            service::Host host(concurrentRoot);const auto added=host.add(declaration);assert(added);
            assert(host.change(added.token,"enabled","false"));
            std::atomic<unsigned> accepted{};std::atomic<bool> invalid{};
            std::jthread editor([&](std::stop_token stop){
                unsigned value=0;
                while(!stop.stop_requested()){
                    const auto changed=host.change(added.token,"count",std::to_string(1+(value++%8)));
                    if(changed)accepted.fetch_add(1,std::memory_order_relaxed);
                    else if(changed.status!=options::Status::Busy)invalid.store(true,std::memory_order_relaxed);
                    std::this_thread::yield();
                }
            });
            bool stored=false;
            for(unsigned attempt=0;attempt<1000&&!stored;++attempt){
                const auto snapshot=host.registry.snapshot();assert(snapshot->mods.size()==1);
                assert(snapshot->mods.front().values[0]=="false");
                const auto file=std::filesystem::exists(concurrentRoot)?savedFile(concurrentRoot):std::filesystem::path{};
                if(!file.empty())try{
                    std::ifstream input(file);std::string data((std::istreambuf_iterator<char>(input)),{});
                    const auto saved=options::parseSaved(data);
                    stored=saved.at("enabled")=="false"&&saved.at("window.main")=="F8"&&
                        std::stoi(saved.at("count"))>=1&&std::stoi(saved.at("count"))<=8&&accepted.load(std::memory_order_relaxed)>=128;
                }catch(...){}
                if(!stored)std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            editor.request_stop();editor.join();assert(stored&&!invalid&&accepted>=128);
            assert(host.change(added.token,"count","8"));assert(host.remove(added.token));
        }
        service::Host restored(concurrentRoot);assert(restored.add(declaration));
        assert(restored.registry.snapshot()->mods.front().values==std::vector<std::string>({"false","8","F8"}));
    }
    {
        using Preferences=nimby::windows::automatic::DrivingPreferences;
        Preferences detached;
        assert(!detached.ready()&&detached.get()==3&&!detached.set(8));
        std::array<std::jthread,8> bootstraps;
        for(auto& bootstrap:bootstraps)bootstrap=std::jthread([]{
            Preferences view;(void)view.open();assert(view.get()==3);
        });
        for(auto& bootstrap:bootstraps)bootstrap.join();
        // Native integration opens its cached view at bootstrap. The resident
        // Host may arrive afterwards; both views see the same atomic value.
        Preferences reader;assert(reader.open()&&reader.ready()&&reader.get()==3);
        uint64_t sdkToken{};
        {
            service::Host host(root/"sdk-driving");
            assert(host.registry.snapshot()->mods.empty());
            auto forged=Json::parse(declaration);forged["id"]="nrf-sdk";
            assert(!host.add(forged.dump())); // No public mod claims SDK preferences.
            const auto initialized=host.initializeDrivingOptions();assert(initialized);sdkToken=initialized.token;
            const auto repeat=host.initializeDrivingOptions();assert(repeat&&repeat.token==sdkToken);
            const auto state=host.registry.snapshot();assert(state->mods.size()==1);
            const auto& sdk=state->mods.front();assert(sdk.id=="nrf-sdk"&&sdk.fields.size()==1);
            const auto& field=sdk.fields.front();
            assert(field.id=="stopBeforeSignalMeters"&&field.kind==options::Kind::Integer);
            assert(field.defaultValue=="3"&&field.minimum==0&&field.maximum==100&&sdk.values.front()=="3");
            assert(host.translate(sdkToken,sdk.title,"fr")=="NRF SDK");
            assert(host.translate(sdkToken,field.label,"fr").find("Distance d’arrêt")!=std::string::npos);
            assert(host.translate(sdkToken,field.label,"en").find("Stopping distance")!=std::string::npos);
            assert(host.translate(sdkToken,field.description,"fr").find("3 mètres")!=std::string::npos);
            assert(host.translate(sdkToken,field.description,"en").find("3 metres")!=std::string::npos);
            assert(host.change(sdkToken,field.id,"0")&&reader.get()==0);
            assert(host.change(sdkToken,field.id,"100")&&reader.get()==100);
            for(const auto value:{"-1","101","03","NaN"}){
                assert(!host.change(sdkToken,field.id,value)&&reader.get()==100);
            }
            assert(!reader.set(-1)&&!reader.set(101)&&reader.get()==100);
            assert(!host.change(sdkToken,"unknown","7")&&reader.get()==100);
            assert(!host.reset(sdkToken,"unknown")&&reader.get()==100);
            assert(host.reset(sdkToken,field.id)&&reader.get()==3);
            assert(host.change(sdkToken,field.id,"8")&&reader.get()==8);
            assert(host.reset(sdkToken)&&reader.get()==3);
            assert(!host.remove(sdkToken)&&host.registry.snapshot()->mods.size()==1);
            assert(!host.add(forged.dump()));
            // Only this resident option updates the mapping; an unrelated mod
            // retains the ordinary registry/storage lifecycle and isolation.
            const auto other=host.add(declaration);assert(other);
            assert(host.change(other.token,"count","7")&&reader.get()==3);
            assert(host.remove(other.token)&&reader.get()==3);
            assert(host.change(sdkToken,field.id,"9")&&reader.get()==9);
            std::array<char,NIMBY_OPTIONS_VALUES_LIMIT> buffer{};uint32_t bytes{};uint64_t revision{};
            assert(host.read(sdkToken,0,buffer.data(),buffer.size(),&bytes,&revision)==NIMBY_OK&&bytes);
            assert(Json::parse(buffer.data(),buffer.data()+bytes)["values"]==Json::array({"9"}));
            // Competing accepted edits cannot publish out of order after the
            // registry's own try-lock was released. Contention stays bounded.
            std::array<std::jthread,2> editors;
            for(size_t index=0;index<editors.size();++index)editors[index]=std::jthread([&,index]{
                for(unsigned attempt=0;attempt<100;++attempt){
                    const auto result=host.change(sdkToken,field.id,index?"17":"5");
                    assert(result||result.status==options::Status::Busy);
                    const auto visible=reader.get();assert(visible==9||visible==5||visible==17);
                }
            });
            for(auto& editor:editors)editor.join();
            const auto accepted=host.registry.snapshot()->mods.front().values.front();
            assert(reader.get()==std::stoi(accepted));
            assert(host.change(sdkToken,field.id,"9")&&reader.get()==9);
        }
        {
            service::Host reloaded(root/"sdk-driving");const auto restored=reloaded.initializeDrivingOptions();assert(restored);
            assert(reader.get()==9&&reloaded.registry.snapshot()->mods.front().values.front()=="9");
            assert(!reloaded.remove(restored.token));
            assert(reloaded.reset(restored.token)&&reader.get()==3);
        }
    }
    uint64_t first{};
    {
        // Loader feedback uses the existing immutable catalogue; no preference
        // file, registry row, game pointer or user-provided text is involved.
        service::Host host(root/"loader-status");
        assert(host.loaderMessage(true).empty()&&host.loaderMessage(false).empty());
        const auto initial=host.catalogueRevision();
        const service::LoaderStatus noMods{NIMBY_OPTIONS_LOADER_NO_MODS};
        assert(host.reportLoaderStatus(noMods)==NIMBY_OK&&host.catalogueRevision()>initial);
        assert(host.loaderMessage(true).find("NRF Hub")!=std::string::npos);
        assert(host.loaderMessage(false).find("No mods are enabled")!=std::string::npos);
        const auto unchanged=host.catalogueRevision();
        assert(host.reportLoaderStatus(noMods)==NIMBY_OK&&host.catalogueRevision()==unchanged);
        constexpr uint64_t mib=1024*1024;
        service::LoaderStatus refused{NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED,4,0,2305*mib,2048*mib,1024*mib,1216*mib};
        assert(host.reportLoaderStatus(refused)==NIMBY_OK);
        assert(host.loaderMessage(true).find("1024 Mio")!=std::string::npos);
        assert(host.loaderMessage(false).find("1216 MiB")!=std::string::npos);
        assert(host.loaderMessage(false).find("2305 MiB")!=std::string::npos);
        const auto beforeInvalid=host.catalogueRevision();
        const auto accepted=host.loaderMessage(false);
        for(const auto invalid:std::array<service::LoaderStatus,5>{{
            {99}, {NIMBY_OPTIONS_LOADER_READY,34}, {NIMBY_OPTIONS_LOADER_READY,4,5},
            {NIMBY_OPTIONS_LOADER_NO_MODS,1},
            {NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED,4,0,UINT64_MAX}}})
            assert(host.reportLoaderStatus(invalid)==NIMBY_INVALID_ARGUMENT);
        assert(host.catalogueRevision()==beforeInvalid&&host.loaderMessage(false)==accepted);
        assert(host.registry.snapshot()->mods.empty());
        assert(host.reportLoaderStatus({NIMBY_OPTIONS_LOADER_START_FAILED,4})==NIMBY_OK);
        assert(host.loaderMessage(false).find("logs in NRF Hub")!=std::string::npos);
        assert(host.reportLoaderStatus({NIMBY_OPTIONS_LOADER_START_FAILED,4,3})==NIMBY_OK);
        assert(host.loaderMessage(false).find("3/4 active")!=std::string::npos);
        constexpr uint64_t maximumMemory=uint64_t{1}<<60;
        const auto largest=service::loaderDiagnostic({NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED,32,0,
            maximumMemory,maximumMemory,maximumMemory,maximumMemory});
        assert(largest.french.size()<768&&largest.english.size()<768);
        const auto rounding=service::loaderDiagnostic({NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED,1,0,1,1,1,1});
        assert(rounding.english.find("1 MiB; minimum required: 1 MiB")!=std::string::npos);
        const auto unknown=service::loaderDiagnostic({NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED,4});
        assert(unknown.english.find("logs in NRF Hub")!=std::string::npos);
        assert(unknown.english.find("insufficient")==std::string::npos&&unknown.english.find("0 MiB")==std::string::npos);
        const auto countLimit=service::loaderDiagnostic({NIMBY_OPTIONS_LOADER_RESOURCE_REFUSED,33});
        assert(countLimit.english.find("32 active mods")!=std::string::npos&&countLimit.english.find("memory")==std::string::npos);
        assert(host.reportLoaderStatus({NIMBY_OPTIONS_LOADER_READY,4,4})==NIMBY_OK);
        assert(host.loaderMessage(true).empty()&&host.loaderMessage(false).empty());
        // Readers see one complete publication while a producer replaces the
        // status. Numbers from two reports cannot be combined in the message.
        refused.physical=1111*mib;refused.budget=111*mib;
        auto other=refused;other.physical=2222*mib;other.budget=222*mib;
        assert(host.reportLoaderStatus(refused)==NIMBY_OK);
        std::jthread producer([&]{for(unsigned i=0;i<200;++i)assert(host.reportLoaderStatus(i%2?refused:other)==NIMBY_OK);});
        for(unsigned i=0;i<500;++i){
            const auto text=host.loaderMessage(false);
            if(text.find("111 MiB")!=std::string::npos)assert(text.find("1111 MiB")!=std::string::npos&&text.find("2222 MiB")==std::string::npos);
            else assert(text.find("222 MiB")!=std::string::npos&&text.find("2222 MiB")!=std::string::npos&&text.find("1111 MiB")==std::string::npos);
        }
        producer.join();
        assert(!std::filesystem::exists(root/"loader-status"));
    }
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
        assert(host.reportLoaderStatus({NIMBY_OPTIONS_LOADER_NO_MODS})==NIMBY_OK);
        assert(!host.loaderMessage(true).empty());
        const auto published=host.catalogueRevision();assert(published>initial);
        assert(host.translate(a.token,title,"fr")=="Premier"&&host.translate(a.token,title,"en")=="First");
        localized["id"]="other-catalogue";
        localized["translations"]=R"({"fallback":"fr","languages":{"fr":{"title":"Second"},"en":{"title":"Second EN"}}})";
        const auto b=host.add(localized.dump());assert(b&&host.catalogueRevision()>published);
        assert(!host.loaderMessage(true).empty());
        assert(host.translate(a.token,title,"fr")=="Premier"&&host.translate(b.token,title,"fr")=="Second");
        const auto beforeRemoval=host.catalogueRevision();
        assert(host.remove(b.token));assert(host.catalogueRevision()>beforeRemoval);
        assert(!host.loaderMessage(true).empty());
        assert(host.translate(a.token,title,"en")=="First");
        assert(host.translate(b.token,title,"en")!="Second EN");
    }
    {
        service::Host host(root/"registration-oom");
        auto other=Json::parse(declaration);other["id"]="other-mod";
        const auto existing=host.add(other.dump());assert(existing);
        allocation_fault_fixture::arm(std::numeric_limits<size_t>::max(),0);
        const auto baseline=host.add(declaration);
        const auto allocations=allocation_fault_fixture::stop();assert(baseline&&allocations.allocations);
        assert(host.remove(baseline.token));
        size_t rejected=0;
        // Target the final publication allocations, after declaration parsing.
        // The old post-registration profile/catalogue failure followed by
        // Registry::remove's allocation may not orphan a token. Parsing's
        // third-party JSON destruction is outside this transaction fixture.
        const auto first=allocations.allocations>16?allocations.allocations-16:0;
        for(size_t at=first;at<allocations.allocations;++at){
            options::Result added;bool failed=false;
            allocation_fault_fixture::arm(at,2);
            try{added=host.add(declaration);}catch(const std::bad_alloc&){failed=true;}
            const auto fault=allocation_fault_fixture::stop();assert(fault.failures);
            if(failed){
                ++rejected;
                const auto snapshot=host.registry.snapshot();
                if(snapshot->mods.size()!=1||snapshot->mods.front().token!=existing.token){
                    std::cerr<<"Orphaned registration after double allocation fault at "<<at<<'\n';return 1;
                }
                // Retry the same ID, without unloading the healthy owner.
                added=host.add(declaration);assert(added);
            }else assert(added);
            assert(host.registry.snapshot()->mods.size()==2);
            assert(host.remove(added.token));
            const auto snapshot=host.registry.snapshot();
            assert(snapshot->mods.size()==1&&snapshot->mods.front().token==existing.token);
        }
        assert(rejected&&host.remove(existing.token));
        std::cout<<"PASS registration double OOM: "<<allocations.allocations-first<<" final publication allocation boundaries; same-ID retry and healthy owner retained\n";
    }
    std::cout<<"Mod option persistence, isolation, translated catalogues and broker snapshots: PASS\n";
}
