#pragma once
#include <runtime/mod_options_registry.h>
#include <nimby/detail/mod_options_bridge.h>
#include <nimby/detail/translations.hpp>
#include <limits>
#include <set>
#include <span>
#include <stdexcept>

namespace nimby::runtime::mod_options {
struct Declaration {
    Definition definition;
    std::shared_ptr<const nimby::detail::Translations> translations;
};
inline constexpr size_t maximumSavedBytes=64*1024,maximumSavedEntries=256;

namespace codec {
using Json=nlohmann::json;
[[noreturn]] inline void invalid(const char* reason){throw std::invalid_argument(std::string("Mod options: ")+reason);}
inline Json parse(std::string_view bytes,size_t limit){
    if(bytes.empty()||bytes.size()>limit)invalid("JSON byte limit exceeded or empty input");
    std::vector<std::set<std::string>> keys;
    try{
        return Json::parse(bytes,[&](int depth,Json::parse_event_t event,Json& value){
            if(depth>8)invalid("JSON nesting limit exceeded");
            if(event==Json::parse_event_t::object_start)keys.emplace_back();
            if(event==Json::parse_event_t::object_end)keys.pop_back();
            if(event==Json::parse_event_t::key){
                if(keys.empty()||!keys.back().insert(value.get<std::string>()).second)invalid("duplicate JSON key");
            }
            return true;
        });
    }catch(const Json::exception& error){throw std::invalid_argument(std::string("Mod options: invalid JSON: ")+error.what());}
}
inline void members(const Json& value,std::initializer_list<std::string_view> allowed){
    if(!value.is_object())invalid("expected JSON object");
    for(const auto& item:value.items())if(std::find(allowed.begin(),allowed.end(),item.key())==allowed.end())invalid("unknown JSON member");
}
inline const Json& required(const Json& object,std::string_view key){
    const auto found=object.find(key);if(found==object.end())invalid("missing JSON member");
    return *found;
}
inline std::string text(const Json& value,size_t limit,bool empty=false){
    if(!value.is_string())invalid("expected JSON string");
    const auto& text=value.get_ref<const std::string&>();
    if(!Registry::validText(text,limit,empty))invalid("invalid UTF-8 string or byte limit exceeded");
    return text;
}
inline int32_t integer(const Json& value){
    if(!value.is_number_integer())invalid("expected integral JSON number");
    if(value.is_number_unsigned()){
        const auto number=value.get<uint64_t>();
        if(number>uint64_t(std::numeric_limits<int32_t>::max()))invalid("integer outside int32 range");
        return static_cast<int32_t>(number);
    }
    const auto number=value.get<int64_t>();
    if(number<std::numeric_limits<int32_t>::min()||number>std::numeric_limits<int32_t>::max())invalid("integer outside int32 range");
    return static_cast<int32_t>(number);
}
inline void checkSaved(const std::map<std::string,std::string>& values){
    if(values.size()>maximumSavedEntries)invalid("too many saved options");
    for(const auto& [id,value]:values)if(!Registry::validId(id)||!Registry::validText(value,256,true))invalid("invalid saved option id or value");
}
}

// A declaration is copied and checked before registration. No callback,
// filesystem path or address from a mod survives decoding. Its embedded
// catalogue has its own duplicate-key, UTF-8, placeholder and depth checks.
inline Declaration decodeDeclaration(std::string_view bytes){
    const auto root=codec::parse(bytes,NIMBY_OPTIONS_SCHEMA_LIMIT);
    codec::members(root,{"id","title","fields","translations"});
    Declaration result;
    auto& definition=result.definition;
    definition.id=codec::text(codec::required(root,"id"),128);
    definition.title=codec::text(codec::required(root,"title"),256);
    const auto& fields=codec::required(root,"fields");
    if(!fields.is_array()||fields.empty()||fields.size()>Registry::maxFields)codec::invalid("expected 1..64 option fields");
    definition.fields.reserve(fields.size());
    for(const auto& item:fields){
        codec::members(item,{"id","label","description","kind","default","minimum","maximum","choices"});
        Field field;
        field.id=codec::text(codec::required(item,"id"),128);
        field.label=codec::text(codec::required(item,"label"),256);
        if(const auto description=item.find("description");description!=item.end())field.description=codec::text(*description,1024,true);
        const auto kind=codec::integer(codec::required(item,"kind"));
        if(kind<0||kind>3)codec::invalid("unknown option kind");
        field.kind=static_cast<Kind>(kind);
        field.defaultValue=codec::text(codec::required(item,"default"),256,true);
        if(field.kind==Kind::Integer&&(!item.contains("minimum")||!item.contains("maximum")))codec::invalid("integer option requires explicit bounds");
        if(const auto minimum=item.find("minimum");minimum!=item.end())field.minimum=codec::integer(*minimum);
        if(const auto maximum=item.find("maximum");maximum!=item.end())field.maximum=codec::integer(*maximum);
        if(const auto choices=item.find("choices");choices!=item.end()){
            if(!choices->is_array()||choices->size()>Registry::maxChoices)codec::invalid("invalid option choices");
            for(const auto& choice:*choices){
                codec::members(choice,{"id","label"});
                field.choices.push_back({codec::text(codec::required(choice,"id"),128),codec::text(codec::required(choice,"label"),256)});
            }
        }
        definition.fields.push_back(std::move(field));
    }
    if(!Registry::validDefinition(definition))codec::invalid("invalid option declaration");
    if(const auto catalogue=root.find("translations");catalogue!=root.end()){
        auto bytes=codec::text(*catalogue,nimby::detail::Translations::maximumBytes,true);
        if(!bytes.empty())result.translations=std::make_shared<const nimby::detail::Translations>(bytes);
    }
    return result;
}

// Saved values are deliberately schema-independent: unknown IDs survive an
// older/newer mod version. Loading does not mutate this map or silently erase
// user choices; savedValues validates only the currently declared fields.
inline std::map<std::string,std::string> parseSaved(std::string_view bytes){
    const auto root=codec::parse(bytes,maximumSavedBytes);
    codec::members(root,{"version","values"});
    if(codec::integer(codec::required(root,"version"))!=1)codec::invalid("unsupported saved options version");
    const auto& values=codec::required(root,"values");
    if(!values.is_object()||values.size()>maximumSavedEntries)codec::invalid("invalid saved options map");
    std::map<std::string,std::string> result;
    for(const auto& [id,value]:values.items()){
        if(!Registry::validId(id))codec::invalid("invalid saved option id");
        result.emplace(id,codec::text(value,256,true));
    }
    return result;
}
inline std::vector<std::string> savedValues(const Definition& definition,const std::map<std::string,std::string>& saved){
    if(!Registry::validDefinition(definition))codec::invalid("invalid declaration for saved options");
    codec::checkSaved(saved);
    std::vector<std::string> result;result.reserve(definition.fields.size());
    for(const auto& field:definition.fields){
        const auto found=saved.find(field.id);
        result.push_back(found!=saved.end()&&Registry::validValue(field,found->second)?found->second:field.defaultValue);
    }
    return result;
}
inline std::string encodeSaved(const std::map<std::string,std::string>& values){
    codec::checkSaved(values);
    auto result=codec::Json({{"version",1},{"values",values}}).dump();
    if(result.size()>maximumSavedBytes)codec::invalid("saved options JSON byte limit exceeded");
    return result;
}
// Merge current values into the version-independent history. Stale fields
// are retained where possible, but cannot prevent saving active preferences
// when either the entry count OR escaped JSON byte budget is exhausted.
inline std::map<std::string,std::string> mergeSaved(std::span<const Field> fields,
        std::span<const std::string> values,const std::map<std::string,std::string>& saved){
    if(fields.empty()||fields.size()>Registry::maxFields||fields.size()!=values.size())codec::invalid("invalid fields for saved options merge");
    codec::checkSaved(saved);
    std::set<std::string_view> current;
    auto result=saved;
    for(size_t i=0;i<fields.size();++i){
        if(!Registry::validId(fields[i].id)||!current.insert(fields[i].id).second||!Registry::validValue(fields[i],values[i]))
            codec::invalid("invalid current option for saved options merge");
        result[fields[i].id]=values[i];
    }
    // Compute exactly the same UTF-8/escaping cost as encodeSaved, once per
    // row. Repeatedly serializing a near-full file for each eviction would
    // make a bounded history unnecessarily quadratic in stored bytes.
    const auto rowBytes=[](const auto& row){return codec::Json(row.first).dump().size()+1+codec::Json(row.second).dump().size();};
    size_t bytes=codec::Json({{"version",1},{"values",codec::Json::object()}}).dump().size();
    for(const auto& row:result)bytes+=rowBytes(row);
    if(!result.empty())bytes+=result.size()-1;
    for(auto it=result.begin();it!=result.end()&&(result.size()>maximumSavedEntries||bytes>maximumSavedBytes);){
        if(current.contains(it->first)){++it;continue;}
        bytes-=rowBytes(*it)+(result.size()>1?1:0);it=result.erase(it);
    }
    if(result.size()>maximumSavedEntries||bytes>maximumSavedBytes)codec::invalid("current options exceed saved JSON budget");
    return result;
}
}
