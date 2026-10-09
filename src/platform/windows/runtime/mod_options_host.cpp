#include <platform/windows/mod_options_host.h>
#include <runtime/mod_options_codec.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/observation.h>
#include <nimby/detail/diagnostics.hpp>
#include <windows.h>
#include <bcrypt.h>
#include <condition_variable>
#include <fstream>
#include <thread>

namespace nimby::platform::windows::mod_options {
namespace options=runtime::mod_options;
namespace {
using Json=nlohmann::json;
uint32_t status(options::Result value){
    switch(value.status){
        case options::Status::Ok:case options::Status::Unchanged:return NIMBY_OK;
        case options::Status::NotFound:return NIMBY_INVALID_HANDLE;
        case options::Status::Limit:case options::Status::Busy:return NIMBY_RESOURCE_LIMIT;
        default:return NIMBY_INVALID_ARGUMENT;
    }
}
template<class F> uint32_t boundary(F&& f)noexcept{
    try{return f();}catch(const std::bad_alloc&){return NIMBY_RESOURCE_LIMIT;}
    catch(const std::invalid_argument&){return NIMBY_INVALID_ARGUMENT;}
    catch(...){nimby::detail::diagnostics::exception("sdk","mod options");return NIMBY_INTERNAL_ERROR;}
}
std::filesystem::path userDirectory(){
    wchar_t root[32768]{};const auto count=GetEnvironmentVariableW(L"LOCALAPPDATA",root,32768);
    if(!count||count>=32768)throw std::runtime_error("Mod options: LOCALAPPDATA unavailable");
    return std::filesystem::path(root)/L"NimbyRailsFrance"/L"mod-options";
}
// IDs are case sensitive, whereas Windows filenames are not. Hashing also
// avoids reserved device names, traversal and path-length growth from IDs.
std::string storageName(std::string_view id){
    unsigned char hash[32]{};
    struct Algorithm{BCRYPT_ALG_HANDLE value{};~Algorithm(){if(value)BCryptCloseAlgorithmProvider(value,0);}} algorithm;
    struct Hash{BCRYPT_HASH_HANDLE value{};~Hash(){if(value)BCryptDestroyHash(value);}} digest;
    if(BCryptOpenAlgorithmProvider(&algorithm.value,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0||
        BCryptCreateHash(algorithm.value,&digest.value,nullptr,0,nullptr,0,0)<0||
        BCryptHashData(digest.value,reinterpret_cast<PUCHAR>(const_cast<char*>(id.data())),static_cast<ULONG>(id.size()),0)<0||
        BCryptFinishHash(digest.value,hash,sizeof hash,0)<0)throw std::runtime_error("Mod options: hash failed");
    std::string name;name.reserve(69);constexpr char hex[]="0123456789abcdef";
    for(auto byte:hash){name+=hex[byte>>4];name+=hex[byte&15];}return name+".json";
}
void writeAtomic(const std::filesystem::path& path,std::string_view bytes){
    std::filesystem::create_directories(path.parent_path());
    // Unique sibling, exclusive creation, flush before atomic replacement.
    // No random temp directory or shell command participates in persistence.
    static std::atomic<uint64_t> serial{};
    auto temporary=path;temporary+=L".tmp-"+std::to_wstring(GetCurrentProcessId())+L"-"+std::to_wstring(++serial);
    HANDLE file=CreateFileW(temporary.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file==INVALID_HANDLE_VALUE)throw std::runtime_error("Mod options: cannot create temporary file");
    try{
        DWORD written{};
        if(!WriteFile(file,bytes.data(),static_cast<DWORD>(bytes.size()),&written,nullptr)||written!=bytes.size()||!FlushFileBuffers(file))
            throw std::runtime_error("Mod options: cannot write preferences");
        CloseHandle(file);file=INVALID_HANDLE_VALUE;
        if(!MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
            throw std::runtime_error("Mod options: cannot replace preferences");
    }catch(...){if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);DeleteFileW(temporary.c_str());throw;}
}
}
struct Host::State {
    struct Profile{
        std::filesystem::path path;
        std::map<std::string,std::string> saved;
        std::vector<std::string> written;
        std::shared_ptr<const nimby::detail::Translations> translations;
        std::string error;
        bool invalidOriginal=false;
        std::optional<options::Entry> retired;
    };
    struct Metadata{std::shared_ptr<const nimby::detail::Translations> translations;std::string error;};
    using Catalogues=std::map<uint64_t,Metadata>;
    Host& owner;
    std::filesystem::path directory;
    // Only broker/storage threads acquire lifecycle. UI reads an immutable
    // catalogue and registry snapshot, so slow disk cannot hold up the game.
    std::mutex lifecycle,waitMutex;
    std::condition_variable wake;
    std::map<uint64_t,Profile> profiles;
    std::atomic<std::shared_ptr<const Catalogues>> catalogues{std::make_shared<const Catalogues>()};
    std::atomic<uint64_t> catalogueRevision{0};
    uint64_t nextOwner=0;
    std::jthread writer;
    State(Host& host,std::filesystem::path path):owner(host),directory(path.empty()?userDirectory():std::move(path)),
        writer([this](std::stop_token stop){run(stop);}){}
    ~State(){writer.request_stop();wake.notify_one();if(writer.joinable())writer.join();}
    void publish(){
        auto next=std::make_shared<Catalogues>();
        for(const auto& [token,p]:profiles)if(!p.retired)next->emplace(token,Metadata{p.translations,p.error});
        catalogues.store(std::move(next),std::memory_order_release);
        catalogueRevision.fetch_add(1,std::memory_order_release);
    }
    void save(const options::Entry& entry,Profile& profile){
        if(profile.written==entry.values)return;
        try{
            auto saved=options::mergeSaved(entry.fields,entry.values,profile.saved);
            const auto bytes=options::encodeSaved(saved);
            if(profile.invalidOriginal&&std::filesystem::exists(profile.path)){
                auto backup=profile.path;backup+=L".invalid-"+std::to_wstring(GetTickCount64());
                // No REPLACE_EXISTING: an earlier diagnostic backup is retained.
                if(!MoveFileExW(profile.path.c_str(),backup.c_str(),MOVEFILE_WRITE_THROUGH))
                    throw std::runtime_error("Mod options: cannot preserve invalid preferences");
                profile.invalidOriginal=false;
            }
            writeAtomic(profile.path,bytes);profile.saved=std::move(saved);profile.written=entry.values;
            if(!profile.error.empty()){profile.error.clear();publish();}
        }catch(...){
            if(profile.error!="Impossible d'enregistrer les options. Nouvelle tentative en cours."){
                profile.error="Impossible d'enregistrer les options. Nouvelle tentative en cours.";publish();
                nimby::detail::diagnostics::exception("sdk","persist mod options");
            }
        }
    }
    void flush(){
        std::lock_guard lock(lifecycle);
        // Capture after locking: removal/re-registration cannot be overwritten
        // later by a stale snapshot taken before the lifecycle operation.
        for(const auto& entry:owner.registry.snapshot()->mods){
            const auto it=profiles.find(entry.token);if(it!=profiles.end())save(entry,it->second);
        }
        // A transient write failure during unload retains the final values.
        // Retry even though the mod is no longer registered or running.
        for(auto it=profiles.begin();it!=profiles.end();){
            auto& profile=it->second;
            if(profile.retired){
                save(*profile.retired,profile);
                if(profile.written==profile.retired->values){it=profiles.erase(it);continue;}
            }
            ++it;
        }
    }
    void run(std::stop_token stop)noexcept{
        while(!stop.stop_requested()){
            {std::unique_lock lock(waitMutex);wake.wait_for(lock,std::chrono::milliseconds(250));}
            try{flush();}catch(...){nimby::detail::diagnostics::exception("sdk","mod options storage worker");}
        }
    }
};
Host::Host(std::filesystem::path directory):state_(std::make_unique<State>(*this,std::move(directory))){}
Host::~Host(){
    state_->writer.request_stop();state_->wake.notify_one();if(state_->writer.joinable())state_->writer.join();
    try{state_->flush();}catch(...){nimby::detail::diagnostics::exception("sdk","final mod options flush");}
}
options::Result Host::add(std::string_view bytes){
    auto declaration=options::decodeDeclaration(bytes);
    std::lock_guard lock(state_->lifecycle);
    State::Profile profile;profile.path=state_->directory/storageName(declaration.definition.id);profile.translations=std::move(declaration.translations);
    const auto pending=std::find_if(state_->profiles.begin(),state_->profiles.end(),[&](const auto& item){
        return item.second.retired&&item.second.path==profile.path;
    });
    // A reload replaces an existing retained write instead of consuming space.
    if(state_->profiles.size()>=128&&pending==state_->profiles.end())return {options::Status::Limit};
    if(pending!=state_->profiles.end()){
        // Reload before the failed write recovered: use the last accepted
        // values rather than stale disk, and transfer ownership of the retry.
        const auto& previous=pending->second;
        profile.saved=options::mergeSaved(previous.retired->fields,previous.retired->values,previous.saved);
        profile.invalidOriginal=previous.invalidOriginal;profile.error=previous.error;
    }else{
    try{
        if(std::filesystem::exists(profile.path)){
            const auto size=std::filesystem::file_size(profile.path);
            if(size>65536)throw std::invalid_argument("Mod preferences too large");
            std::ifstream input(profile.path,std::ios::binary);std::string data(static_cast<size_t>(size),'\0');
            if(!input.read(data.data(),static_cast<std::streamsize>(size)))throw std::runtime_error("Cannot read mod preferences");
            profile.saved=options::parseSaved(data);
        }
    }catch(...){
        // An invalid original is preserved before the first successful write.
        profile.invalidOriginal=true;profile.error="Options sauvegardées illisibles : valeurs par défaut utilisées.";
        nimby::detail::diagnostics::exception("sdk","load mod options");
    }
    }
    auto values=options::savedValues(declaration.definition,profile.saved);
    if(pending==state_->profiles.end())profile.written=values;
    const auto result=registry.add(++state_->nextOwner,std::move(declaration.definition),std::move(values));
    if(!result)return result;
    try{
        state_->profiles.emplace(result.token,std::move(profile));state_->publish();
        if(pending!=state_->profiles.end())state_->profiles.erase(pending);
    }
    catch(...){registry.remove(result.token);state_->profiles.erase(result.token);throw;}
    return result;
}
options::Result Host::remove(uint64_t token){
    std::lock_guard lock(state_->lifecycle);options::Entry final;
    const auto result=registry.remove(token,&final);if(!result)return result;
    const auto found=state_->profiles.find(token);
    if(found!=state_->profiles.end()){
        if(found->second.written==final.values)state_->profiles.erase(found);
        else found->second.retired=std::move(final);
        state_->publish();state_->wake.notify_one();
    }
    return result;
}
options::Result Host::change(uint64_t token,std::string_view field,std::string value){
    auto result=registry.change(token,field,std::move(value));if(result)state_->wake.notify_one();return result;
}
options::Result Host::reset(uint64_t token,std::string_view field){
    auto result=registry.reset(token,field);if(result)state_->wake.notify_one();return result;
}
std::string Host::translate(uint64_t token,std::string_view text,std::string_view language)const{
    const auto catalogs=state_->catalogues.load(std::memory_order_acquire);const auto found=catalogs->find(token);
    return nimby::detail::Translations::resolve(found==catalogs->end()?nullptr:found->second.translations.get(),text,language);
}
std::string Host::storageError(uint64_t token)const{
    const auto catalogs=state_->catalogues.load(std::memory_order_acquire);const auto found=catalogs->find(token);
    return found==catalogs->end()?std::string{}:found->second.error;
}
uint64_t Host::catalogueRevision()const noexcept{return state_->catalogueRevision.load(std::memory_order_acquire);}
uint32_t Host::read(uint64_t token,uint64_t known,char* output,uint32_t capacity,uint32_t* written,uint64_t* revision){
    if(written)*written=0;
    if(revision)*revision=0;
    // Fixed internal transport capacity guarantees the bounded response fits
    // before any event is consumed (no lost click due to a tiny output buffer).
    if(!output||!written||!revision||capacity!=NIMBY_OPTIONS_VALUES_LIMIT)return NIMBY_INVALID_ARGUMENT;
    const auto snapshot=registry.snapshot();
    const auto found=std::find_if(snapshot->mods.begin(),snapshot->mods.end(),[&](const auto& mod){return mod.token==token;});
    if(found==snapshot->mods.end())return NIMBY_INVALID_HANDLE;
    if(known>found->revision)return NIMBY_INVALID_ARGUMENT;
    Json response=Json::object();
    if(known!=found->revision)response["values"]=found->values;
    Json events=Json::array();
    for(size_t i=0;i<options::Registry::maxPending;++i){
        const auto event=registry.poll(token,options::Registry::Clock::now(),found->revision);if(!event)break;
        // Native/UI edits may race this broker read. The registry leaves newer
        // events queued for the next read of their matching immutable values.
        if(event->revision==found->revision)events.push_back(event->field);
    }
    if(!events.empty())response["events"]=std::move(events);
    if(!response.empty()){
        const auto bytes=response.dump();if(bytes.size()>capacity)return NIMBY_RESOURCE_LIMIT;
        std::memcpy(output,bytes.data(),bytes.size());*written=static_cast<uint32_t>(bytes.size());
    }
    *revision=found->revision;return NIMBY_OK;
}
Host& host(){
    // The bridge is pinned. Avoid joining a worker from DLL_PROCESS_DETACH
    // under the loader lock; registration/removal performs orderly flushes.
    static auto* instance=new Host;return *instance;
}
namespace {
class ParentEvent final:public options::Wake{
    HANDLE event_;
public:
    explicit ParentEvent(HANDLE value):event_(value){}
    ~ParentEvent()override{CloseHandle(event_);}
    void notify()const noexcept override{SetEvent(event_);}
};
}
}
#define OPTIONS_EXPORT extern "C" __declspec(dllexport) uint32_t
namespace service=nimby::platform::windows::mod_options;
OPTIONS_EXPORT NimbyOptions_RegisterV1(const char* bytes,uint32_t count,uint64_t* owner)noexcept{
    if(owner)*owner=0;
    if(!owner||!bytes||!count||count>NIMBY_OPTIONS_SCHEMA_LIMIT)return NIMBY_INVALID_ARGUMENT;
    return service::boundary([&]{const auto result=service::host().add({bytes,count});if(result)*owner=result.token;return service::status(result);});
}
OPTIONS_EXPORT NimbyOptions_RemoveV1(uint64_t owner)noexcept{
    return service::boundary([&]{return service::status(service::host().remove(owner));});
}
OPTIONS_EXPORT NimbyOptions_DiscardV1(uint64_t owner)noexcept{
    return service::boundary([&]{return service::status(service::host().registry.discardEvents(owner));});
}
OPTIONS_EXPORT NimbyOptions_ReadV1(uint64_t owner,uint64_t known,char* out,uint32_t capacity,uint32_t* written,uint64_t* revision)noexcept{
    return service::boundary([&]{return service::host().read(owner,known,out,capacity,written,revision);});
}
OPTIONS_EXPORT NimbyOptions_WakeV1(uint64_t owner,uint64_t parentEvent)noexcept{
    if(!owner||!parentEvent||reinterpret_cast<HANDLE>(parentEvent)==INVALID_HANDLE_VALUE)return NIMBY_INVALID_ARGUMENT;
    HANDLE duplicate{};
    if(!DuplicateHandle(GetCurrentProcess(),reinterpret_cast<HANDLE>(parentEvent),GetCurrentProcess(),&duplicate,EVENT_MODIFY_STATE,FALSE,0))return NIMBY_INVALID_HANDLE;
    return service::boundary([&]{
        std::shared_ptr<const nimby::runtime::mod_options::Wake> wake;
        try{wake=std::make_shared<service::ParentEvent>(duplicate);}catch(...){CloseHandle(duplicate);throw;}
        return service::status(service::host().registry.setWake(owner,std::move(wake)));
    });
}
