#pragma once
#include <nimby/detail/translations.hpp>
#include <array>
#include <memory>

namespace nimby::engine::train_editor {
// A resident, immutable presentation supplied by the mod. Resolve references
// once at registration, never from the game panel or a mod callback. Text is
// always data: even a literal "%n" is not passed to a printf formatter.
class Messages {
    using Json=nlohmann::json;
    using Texts=std::array<std::string,3>;
    std::map<std::string,Texts,std::less<>> languages_;
    std::string fallback_;
    static Json parse(std::string_view bytes) {
        if(bytes.empty()||bytes.size()>maximumDeclarationBytes)
            throw std::invalid_argument("Train editor declaration exceeds its transport bound");
        std::vector<std::set<std::string>> keys;
        try {
            return Json::parse(bytes,[&](int depth,Json::parse_event_t event,Json& value){
                if(depth>8)throw std::invalid_argument("Train editor declaration nesting limit exceeded");
                if(event==Json::parse_event_t::object_start)keys.emplace_back();
                if(event==Json::parse_event_t::object_end)keys.pop_back();
                if(event==Json::parse_event_t::key&&!keys.back().insert(value.get<std::string>()).second)
                    throw std::invalid_argument("Train editor declaration contains duplicate keys");
                return true;
            });
        }catch(const Json::exception&){throw std::invalid_argument("Invalid train editor declaration JSON");}
    }
    static void validateText(std::string_view text) {
        if(text.empty()||text.size()>maximumMessageBytes||text.find('\0')!=text.npos)
            throw std::invalid_argument("Train editor message must contain 1..1024 UTF-8 bytes");
        for(unsigned char c:text)if(c<32&&c!='\n'&&c!='\r'&&c!='\t')
            throw std::invalid_argument("Train editor message contains a control character");
    }
public:
    static constexpr size_t maximumDeclarationBytes=2*1024*1024;
    static constexpr size_t maximumMessageBytes=1024;
    explicit Messages(std::string_view bytes) {
        const auto declaration=parse(bytes);
        if(!declaration.is_object()||declaration.size()!=2||!declaration.contains("messages")||
           !declaration.contains("translations")||!declaration["translations"].is_string()||
           !declaration["messages"].is_array()||declaration["messages"].size()!=3)
            throw std::invalid_argument("Train editor requires exactly three messages and a copied catalogue");
        const auto catalogueBytes=declaration["translations"].get<std::string>();
        std::unique_ptr<const nimby::detail::Translations> catalogue;
        std::vector<std::string> locales;
        if(!catalogueBytes.empty()){
            catalogue=std::make_unique<const nimby::detail::Translations>(catalogueBytes);
            const auto source=nimby::detail::Translations::parse(catalogueBytes);
            for(const auto& [code,unused]:source.at("languages").items())
                locales.push_back(nimby::detail::Translations::locale(code));
            fallback_=catalogue->fallback();
        }else{fallback_="en";locales.push_back(fallback_);}
        Texts references;
        for(size_t i=0;i<references.size();++i){
            if(!declaration["messages"][i].is_string())throw std::invalid_argument("Invalid train editor message type");
            references[i]=declaration["messages"][i].get<std::string>();
            if(nimby::detail::Translations::reference(references[i])){
                // Dynamic facts are displayed separately by the SDK. Reject
                // accidental fixed substitution of a changing train length.
                if(references[i].size()>256||!catalogue)throw std::invalid_argument("Invalid train editor translation reference");
                const auto ref=nimby::detail::Translations::parse(std::string_view(references[i]).substr(nimby::detail::Translations::prefix.size()));
                if(!ref.is_array()||ref.size()!=2||!ref[0].is_string()||
                   !nimby::detail::Translations::identifier(ref[0].get<std::string>())||
                   !ref[1].is_object()||!ref[1].empty())
                    throw std::invalid_argument("Train editor messages require a static translation reference");
                const auto key=ref[0].get<std::string>();
                for(const auto& language:locales){
                    const auto resolved=nimby::detail::Translations::resolve(catalogue.get(),references[i],language);
                    if(resolved=="["+key+"]")throw std::invalid_argument("Missing or parameterized train editor translation: "+key);
                    validateText(resolved);languages_[language][i]=resolved;
                }
            }else{
                validateText(references[i]);
                for(const auto& language:locales)languages_[language][i]=references[i];
            }
        }
    }
    const std::string& select(size_t index,std::string_view rawLanguage)const {
        if(index>=3)throw std::invalid_argument("Invalid train editor message index");
        const auto exact=nimby::detail::Translations::locale(rawLanguage);
        const auto base=exact.substr(0,exact.find('-'));
        for(const auto& code:{exact,base,fallback_}){
            const auto found=languages_.find(code);
            if(found!=languages_.end())return found->second[index];
        }
        return languages_.at(fallback_)[index];
    }
    const std::string& fallback()const noexcept{return fallback_;}
    size_t languageCount()const noexcept{return languages_.size();}
    // Compatibility only for privately built ABI10 mods. New declarations
    // always supply their own wording and catalogue through RegisterV2.
    static std::shared_ptr<const Messages> legacy() {
        static const auto value=[] {
            Json catalogue={{"fallback","en"},{"languages",{
                {"fr",{{"exceeded","Impossible d’ajouter ce véhicule : la longueur maximale du train serait dépassée."},
                       {"length","Impossible d’ajouter ce véhicule : sa longueur est indisponible."},
                       {"verification","Impossible de modifier cette composition : sa longueur ne peut pas être vérifiée."}}},
                {"en",{{"exceeded","Cannot add this vehicle: the maximum train length would be exceeded."},
                       {"length","Cannot add this vehicle: its length is unavailable."},
                       {"verification","Cannot change this composition: its length could not be verified."}}}}}};
            auto references=Json::array();
            for(const auto* key:{"exceeded","length","verification"})
                references.push_back(std::string(nimby::detail::Translations::prefix)+Json::array({key,Json::object()}).dump());
            return std::make_shared<const Messages>(Json({{"messages",references},{"translations",catalogue.dump()}}).dump());
        }();return value;
    }
};
}
