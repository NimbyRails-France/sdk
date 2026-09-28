#pragma once
#include <nimby/detail/translations.hpp>
#include <memory>
#include <mutex>

namespace nimby::engine {
struct MetadataText {
    std::string name, description;
    bool operator==(const MetadataText&) const = default;
};
// Generated, fully resolved text only. No code, paths or game addresses occur
// in this format. Immutable catalogues may be shared by simultaneous readers.
class ModMetadataCatalog {
    std::map<std::string,MetadataText,std::less<>> languages_;
    std::string fallback_;
public:
    struct Resource {
        std::string kind,id,key,english;
        std::map<std::string,std::string,std::less<>> languages;
    };
private:
    std::vector<Resource> resources_;
public:
    explicit ModMetadataCatalog(std::string_view bytes) {
        const auto root=detail::Translations::parse(bytes);
        if(!root.is_object())bad();
        const auto format=root.value("format",0);
        if(!((format==1&&root.size()==3)||(format==2&&root.size()==4&&root.contains("resources")))||
           !root.contains("fallback")||!root["fallback"].is_string()||
           !root.contains("languages")||!root["languages"].is_object())bad();
        fallback_=detail::Translations::locale(root["fallback"].get<std::string>());
        if(fallback_.empty()||root["languages"].empty()||root["languages"].size()>64)bad();
        for(const auto& [raw,text]:root["languages"].items()){
            const auto code=detail::Translations::locale(raw);
            if(code.empty()||languages_.contains(code)||!text.is_object()||text.size()!=2||
               !text.contains("name")||!text["name"].is_string()||
               !text.contains("description")||!text["description"].is_string())bad();
            MetadataText value{text["name"].get<std::string>(),text["description"].get<std::string>()};
            validate(value.name,false);validate(value.description,true);
            languages_.emplace(code,std::move(value));
        }
        if(!languages_.contains(fallback_))bad();
        if(format==2){
            if(!root["resources"].is_array()||root["resources"].empty()||root["resources"].size()>32)bad();
            std::set<std::string> keys,identities;
            for(const auto& r:root["resources"]){
                if(!r.is_object()||r.size()!=5)bad();
                for(const auto* key:{"kind","id","key","default"})if(!r.contains(key)||!r[key].is_string())bad();
                if(!r.contains("languages")||!r["languages"].is_object())bad();
                Resource resource{r["kind"].get<std::string>(),r["id"].get<std::string>(),r["key"].get<std::string>(),r["default"].get<std::string>(),{}};
                if((resource.kind!="textures"&&resource.kind!="template")||!detail::Translations::identifier(resource.id)||
                   !nativeKey(resource.key)||!keys.insert(resource.key).second||!identities.insert(resource.kind+":"+resource.id).second)bad();
                validate(resource.english,false);
                for(const auto& [raw,text]:r["languages"].items()){
                    const auto code=detail::Translations::locale(raw);
                    if(!languages_.contains(code)||resource.languages.contains(code)||!text.is_string())bad();
                    auto label=text.get<std::string>();validate(label,false);resource.languages.emplace(code,std::move(label));
                }
                if(!resource.languages.contains(fallback_))bad();
                resources_.push_back(std::move(resource));
            }
        }
    }
    const MetadataText& fallback()const{return languages_.at(fallback_);}
    const MetadataText& resolve(std::string_view language)const {
        const auto exact=detail::Translations::locale(language);
        for(const auto& code:{exact,exact.substr(0,exact.find('-')),fallback_})
            if(const auto it=languages_.find(code);it!=languages_.end())return it->second;
        return fallback();
    }
    static bool nativeKey(std::string_view key){
        return key.size()==72&&key.starts_with("nrf.sdk.")&&std::all_of(key.begin()+8,key.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');});
    }
    const std::vector<Resource>& resources()const{return resources_;}
    const std::string& resolve(const Resource& resource,std::string_view language)const {
        const auto exact=detail::Translations::locale(language);
        for(const auto& code:{exact,exact.substr(0,exact.find('-')),fallback_})
            if(const auto it=resource.languages.find(code);it!=resource.languages.end())return it->second;
        return resource.english;
    }
private:
    static void validate(std::string_view text,bool multiline){
        if(text.empty()||text.size()>16384)bad();
        for(unsigned char c:text)if((c<32&&!(multiline&&c=='\n'))||c==127)bad();
    }
    [[noreturn]]static void bad(){throw std::invalid_argument("Invalid generated nrf-metadata.json");}
};
// The identity is the game's source kind + immutable mod ID, never its name.
// Discovery of a removed/invalid sidecar explicitly forgets the old catalogue.
class ModMetadataRegistry {
    using Key=std::pair<int,std::string>;
    std::mutex mutex_;
    std::map<Key,std::shared_ptr<const ModMetadataCatalog>> catalogues_;
    // Native translation callers receive a borrowed C string. Keep interned
    // buffers stable until shutdown, including after rediscovery/removal.
    // A bounded pool prevents repeated reinstalls from growing it indefinitely.
    std::set<std::string,std::less<>> nativeTexts_;
    size_t nativeBytes_{};
public:
    void replace(int kind,std::string id,std::shared_ptr<const ModMetadataCatalog> catalog){
        std::lock_guard lock(mutex_);Key key{kind,std::move(id)};
        if(catalog)catalogues_.insert_or_assign(std::move(key),std::move(catalog));else catalogues_.erase(key);
    }
    std::shared_ptr<const ModMetadataCatalog> find(int kind,const std::string& id){
        std::lock_guard lock(mutex_);const auto it=catalogues_.find({kind,id});
        return it==catalogues_.end()?nullptr:it->second;
    }
    const char* resolveNative(std::string_view key,std::string_view original,std::string_view language){
        if(!ModMetadataCatalog::nativeKey(key))return nullptr;
        std::lock_guard lock(mutex_);
        const std::string* text=nullptr;
        for(const auto& [id,catalog]:catalogues_)for(const auto& resource:catalog->resources())if(resource.key==key){
            if(text||resource.english!=original)return nullptr; // Unknown/ambiguous ownership keeps the game's fallback.
            text=&catalog->resolve(resource,language);
        }
        if(!text)return nullptr;
        if(const auto it=nativeTexts_.find(*text);it!=nativeTexts_.end())return it->c_str();
        constexpr size_t maximumNativeBytes=32*1024*1024;
        if(text->size()+1>maximumNativeBytes-nativeBytes_)return nullptr;
        const auto [it,inserted]=nativeTexts_.insert(*text);
        if(inserted)nativeBytes_+=it->size()+1;
        return it->c_str();
    }
};
}
