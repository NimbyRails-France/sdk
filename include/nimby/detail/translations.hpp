#pragma once
#include <nimby/detail/vendor/json.hpp>
#include <map>
#include <set>
#include <string>
#include <string_view>

namespace nimby::detail {
// Immutable, owned catalogue. No mod callbacks, file reads or process pointers
// survive registration. It may safely outlive the mod in a frozen UI frame.
class Translations {
    using Json=nlohmann::json;
    using Texts=std::map<std::string,std::string,std::less<>>;
    std::map<std::string,Texts,std::less<>> languages_;
    std::string fallback_="en";
public:
    static constexpr size_t maximumBytes=1024*1024;
    static constexpr std::string_view prefix="\x1eNRF:";
    static std::string locale(std::string_view raw) {
        std::string out;
        for(char c:raw){
            if(c>='A'&&c<='Z')c=char(c-'A'+'a');
            if(c=='_')c='-';
            if(!((c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='-')||out.size()>=32)return {};
            out+=c;
        }
        const auto end=out.find('-');const auto base=out.substr(0,end);
        // NIMBY uses ISO 639-2; JSON authors may use common ISO 639-1 codes.
        static const std::map<std::string,std::string> aliases{
            {"eng","en"},{"fra","fr"},{"fre","fr"},{"deu","de"},{"ger","de"},
            {"spa","es"},{"ita","it"},{"por","pt"},{"nld","nl"},{"dut","nl"},
            {"pol","pl"},{"rus","ru"},{"ukr","uk"},{"ces","cs"},{"cze","cs"},
            {"jpn","ja"},{"kor","ko"},{"zho","zh"},{"chi","zh"},{"tur","tr"},
            {"swe","sv"},{"dan","da"},{"fin","fi"},{"nor","no"},{"hun","hu"},
            {"ron","ro"},{"rum","ro"},{"ell","el"},{"gre","el"},{"ara","ar"}};
        if(const auto it=aliases.find(base);it!=aliases.end())out.replace(0,base.size(),it->second);
        if(out.empty()||out.front()=='-'||out.back()=='-'||out.find("--")!=std::string::npos)return {};
        return out;
    }
    static bool identifier(std::string_view s) {
        return !s.empty()&&s.size()<=96&&std::all_of(s.begin(),s.end(),[](unsigned char c){
            return (c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='.'||c=='_'||c=='-';});
    }
    static Json parse(std::string_view bytes) {
        if(bytes.empty()||bytes.size()>maximumBytes)throw std::invalid_argument("translations.json: expected 1..1048576 UTF-8 bytes");
        std::vector<std::set<std::string>> keys;
        try {
            return Json::parse(bytes, [&](int depth,Json::parse_event_t event,Json& value){
                if(depth>8)throw std::invalid_argument("translations.json: nesting limit exceeded");
                if(event==Json::parse_event_t::object_start)keys.emplace_back();
                if(event==Json::parse_event_t::object_end)keys.pop_back();
                if(event==Json::parse_event_t::key&&!keys.back().insert(value.get<std::string>()).second)
                    throw std::invalid_argument("translations.json: duplicate key");
                return true;
            });
        }catch(const Json::exception& e){throw std::invalid_argument(std::string("translations.json: ")+e.what());}
    }
    static std::set<std::string> placeholders(std::string_view text) {
        std::set<std::string> names;
        for(size_t i=0;i<text.size();++i){
            if(text[i]!='{'&&text[i]!='}')continue;
            if(i+1<text.size()&&text[i+1]==text[i]){++i;continue;}
            if(text[i]=='}')throw std::invalid_argument("translations.json: unmatched closing brace");
            const auto end=text.find('}',i+1);
            if(end==text.npos||!identifier(text.substr(i+1,end-i-1)))throw std::invalid_argument("translations.json: invalid placeholder");
            names.emplace(text.substr(i+1,end-i-1));i=end;
        }
        return names;
    }
    explicit Translations(std::string_view bytes) {
        const auto root=parse(bytes);
        if(!root.is_object()||!root.contains("languages")||!root["languages"].is_object())bad();
        for(const auto& [key,value]:root.items())if(key!="fallback"&&key!="languages")bad();
        if(root.contains("fallback")){if(!root["fallback"].is_string())bad();fallback_=locale(root["fallback"].get<std::string>());}
        const auto& languages=root["languages"];
        if(fallback_.empty()||languages.empty()||languages.size()>64)bad();
        for(const auto& [language,items]:languages.items()){
            const auto code=locale(language);
            if(code.empty()||languages_.contains(code)||!items.is_object()||items.size()>4096)bad();
            Texts texts;
            for(const auto& [key,value]:items.items()){
                if(!identifier(key)||!value.is_string())bad();
                auto text=value.get<std::string>();
                if(text.empty()||text.size()>4096||text.find('\0')!=text.npos||text.find('\x1e')!=text.npos)bad();
                placeholders(text);texts.emplace(key,std::move(text));
            }
            languages_.emplace(code,std::move(texts));
        }
        if(!languages_.contains(fallback_)||languages_.at(fallback_).empty())bad();
        // The fallback is the complete source catalogue. Other locales may be
        // partial, but may not invent keys or change the required arguments.
        const auto& fallback=languages_.at(fallback_);
        for(const auto& [code,texts]:languages_)for(const auto& [key,text]:texts){
            const auto original=fallback.find(key);
            if(original==fallback.end()||placeholders(text)!=placeholders(original->second))
                throw std::invalid_argument("translations.json: missing fallback key or inconsistent placeholders: "+key);
        }
    }
    size_t languageCount()const{return languages_.size();}
    const std::string& fallback()const{return fallback_;}
    static bool reference(std::string_view value){return value.starts_with(prefix);}
    // Plain text is unchanged. Missing keys stay visible instead of silently
    // hiding a control. Bad references/arguments are reported as [key].
    static std::string resolve(const Translations* catalog,std::string_view value,std::string_view language) {
        if(!reference(value))return std::string(value);
        std::string key="translation";
        try {
            if(value.size()>256)throw std::invalid_argument("reference too long");
            const auto ref=parse(value.substr(prefix.size()));
            if(!ref.is_array()||ref.size()!=2||!ref[0].is_string()||!ref[1].is_object()||ref[1].size()>8)bad();
            key=ref[0].get<std::string>();if(!identifier(key))bad();
            const std::string* found=nullptr;
            if(catalog){
                const auto exact=locale(language);const auto base=exact.substr(0,exact.find('-'));
                for(const auto& code:{exact,base,catalog->fallback_}){
                    const auto lang=catalog->languages_.find(code);if(lang==catalog->languages_.end())continue;
                    const auto text=lang->second.find(key);if(text!=lang->second.end()){found=&text->second;break;}
                }
            }
            if(!found)return "["+key+"]";
            const auto names=placeholders(*found);
            if(names.size()!=ref[1].size())bad();
            for(const auto& name:names)if(!ref[1].contains(name)||!ref[1][name].is_string())bad();
            std::string out;
            for(size_t i=0;i<found->size();++i){
                const auto c=(*found)[i];
                if((c=='{'||c=='}')&&i+1<found->size()&&(*found)[i+1]==c){out+=c;++i;}
                else if(c=='{'){const auto end=found->find('}',i+1);out+=ref[1][found->substr(i+1,end-i-1)].get<std::string>();i=end;}
                else out+=c;
                if(out.size()>8192)bad();
            }
            if(out.find('\0')!=out.npos||out.find('\x1e')!=out.npos)bad();
            return out;
        }catch(...){return "["+key+"]";}
    }
private:
    [[noreturn]] static void bad(){throw std::invalid_argument("translations.json: invalid catalogue or translation reference");}
};
}
