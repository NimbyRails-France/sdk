#include <runtime/mod_options_codec.h>
#include <cassert>
#include <functional>
#include <iostream>

namespace options=nimby::runtime::mod_options;
using Json=nlohmann::json;
namespace {
void invalid(const std::function<void()>& action){
    bool rejected=false;try{action();}catch(const std::invalid_argument&){rejected=true;}
    assert(rejected);
}
Json schema(){
    return {{"id","clock"},{"title","Clock"},{"translations",""},{"fields",Json::array({
        {{"id","active"},{"label","Active"},{"description","Enable"},{"kind",0},{"default","true"},{"minimum",0},{"maximum",0},{"choices",Json::array()}},
        {{"id","count"},{"label","Count"},{"kind",1},{"default","4"},{"minimum",-5},{"maximum",64}},
        {{"id","mode"},{"label","Mode"},{"kind",2},{"default","a"},{"choices",Json::array({{{"id","a"},{"label","A"}},{{"id","b"},{"label","B"}}})}},
        {{"id","window.clock"},{"label","Clock"},{"kind",3},{"default","Ctrl+Shift+T"}}
    })}};
}
void declarations(){
    const auto declaration=options::decodeDeclaration(schema().dump());
    assert(declaration.definition.fields.size()==4&&!declaration.translations);
    assert(declaration.definition.fields[1].minimum==-5&&declaration.definition.fields[3].defaultValue=="Ctrl+Shift+T");
    auto input=schema();input["fields"][1]["minimum"]=INT32_MIN;input["fields"][1]["maximum"]=INT32_MAX;
    input["fields"][1]["default"]="-2147483648";assert(options::decodeDeclaration(input.dump()).definition.fields[1].minimum==INT32_MIN);
    input["fields"][1]["default"]="2147483647";assert(options::decodeDeclaration(input.dump()).definition.fields[1].maximum==INT32_MAX);
    for(const auto& bad:{Json(true),Json(1.0),Json("1"),Json(nullptr),Json(uint64_t(INT32_MAX)+1),Json(int64_t(INT32_MIN)-1)}){
        input=schema();input["fields"][1]["minimum"]=bad;invalid([&]{options::decodeDeclaration(input.dump());});
        input=schema();input["fields"][0]["kind"]=bad;invalid([&]{options::decodeDeclaration(input.dump());});
    }
    for(const auto bad:{-1,4}){input=schema();input["fields"][0]["kind"]=bad;invalid([&]{options::decodeDeclaration(input.dump());});}
    for(const auto member:{"id","title","fields"}){input=schema();input.erase(member);invalid([&]{options::decodeDeclaration(input.dump());});}
    for(const auto member:{"id","label","kind","default"}){input=schema();input["fields"][0].erase(member);invalid([&]{options::decodeDeclaration(input.dump());});}
    for(const auto bound:{"minimum","maximum"}){input=schema();input["fields"][1].erase(bound);invalid([&]{options::decodeDeclaration(input.dump());});}
    input=schema();input["extra"]=false;invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][0]["extra"]=false;invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][2]["choices"][0]["extra"]=false;invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][0]["default"]=true;invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][1]["default"]=4;invalid([&]{options::decodeDeclaration(input.dump());});
    for(const auto value:{"04","+4","-0","4.0","65"}){input=schema();input["fields"][1]["default"]=value;invalid([&]{options::decodeDeclaration(input.dump());});}
    input=schema();input["fields"][1]["maximum"]=-6;invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][2]["choices"].erase(1);invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][2]["choices"][1]["id"]="a";invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][2]["default"]="absent";invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][0]["choices"]=schema()["fields"][2]["choices"];invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][1]["id"]="active";invalid([&]{options::decodeDeclaration(input.dump());});
    for(const auto bad:{".","..","../clock","0clock","clock/name","C:\\bad"}){
        input=schema();input["id"]=bad;invalid([&]{options::decodeDeclaration(input.dump());});
    }
    input=schema();input["fields"][3]["default"]="Shift+Ctrl+T";invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][3]["default"]="";assert(options::decodeDeclaration(input.dump()).definition.fields[3].defaultValue.empty());
    invalid([]{options::decodeDeclaration("null");});invalid([]{options::decodeDeclaration("[]");});
    invalid([]{options::decodeDeclaration("");});invalid([]{options::decodeDeclaration("{");});
}
void boundsAndUtf8(){
    auto input=schema();input["id"]=std::string(128,'a');input["title"]=std::string(256,'a');
    input["fields"][0]["description"]=std::string(1024,'a');assert(options::decodeDeclaration(input.dump()).definition.id.size()==128);
    input["title"]=std::string(257,'a');invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"][0]["description"]=std::string(1025,'a');invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["title"]=std::string(129,'\xc3'); // JSON dump itself would reject invalid UTF-8.
    invalid([]{options::decodeDeclaration("{\"id\":\"clock\",\"title\":\"\xc0\xaf\",\"fields\":[]}");});
    input=schema();input["title"]="日本語 Français";assert(options::decodeDeclaration(input.dump()).definition.title=="日本語 Français");
    input["title"]=std::string("bad\0title",9);invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();input["fields"]=Json::array();invalid([&]{options::decodeDeclaration(input.dump());});
    const auto field=schema()["fields"][0];
    for(unsigned i=0;i<64;++i){auto next=field;next["id"]="field"+std::to_string(i);input["fields"].push_back(next);}
    assert(options::decodeDeclaration(input.dump()).definition.fields.size()==64);
    auto extra=field;extra["id"]="field64";input["fields"].push_back(extra);invalid([&]{options::decodeDeclaration(input.dump());});
    input=schema();auto& choices=input["fields"][2]["choices"];choices=Json::array();
    for(unsigned i=0;i<16;++i)choices.push_back({{"id","choice"+std::to_string(i)},{"label","Choice"}});
    input["fields"][2]["default"]="choice0";assert(options::decodeDeclaration(input.dump()).definition.fields[2].choices.size()==16);
    choices.push_back({{"id","choice16"},{"label","Extra"}});invalid([&]{options::decodeDeclaration(input.dump());});
    auto padded=schema().dump();padded.append(NIMBY_OPTIONS_SCHEMA_LIMIT-padded.size(),' ');assert(options::decodeDeclaration(padded).definition.id=="clock");
    padded+=' ';invalid([&]{options::decodeDeclaration(padded);});
    std::string nested(16,'[');nested+="0";nested+=std::string(16,']');invalid([&]{options::decodeDeclaration(nested);});
    for(const auto text:{
        R"({"id":"a","id":"b","title":"T","fields":[]})",
        R"({"id":"a","title":"T","fields":[{"id":"active","label":"A","kind":0,"default":"true","default":"false"}]})",
        R"({"id":"a","title":"T","fields":[{"id":"mode","label":"M","kind":2,"default":"a","choices":[{"id":"a","id":"b","label":"A"}]}]})"
    })invalid([&]{options::decodeDeclaration(text);});
}
void translations(){
    auto input=schema();const std::string catalogue=R"({"fallback":"fr","languages":{"fr":{"title":"Horloge"},"en":{"title":"Clock"}}})";
    input["translations"]=catalogue;input["title"]="\x1eNRF:[\"title\",{}]";
    const auto value=options::decodeDeclaration(input.dump());assert(value.translations);
    assert(nimby::detail::Translations::resolve(value.translations.get(),value.definition.title,"en")=="Clock");
    input["translations"]=Json::object();invalid([&]{options::decodeDeclaration(input.dump());});
    input["translations"]=R"({"fallback":"en","fallback":"fr","languages":{"fr":{"title":"Horloge"}}})";invalid([&]{options::decodeDeclaration(input.dump());});
    input["translations"]=R"({"fallback":"en","languages":{"en":{"title":"A","title":"B"}}})";invalid([&]{options::decodeDeclaration(input.dump());});
    input["translations"]=R"({"fallback":"en","languages":{"en":{"title":"Hello {name}"},"fr":{"title":"Bonjour"}}})";invalid([&]{options::decodeDeclaration(input.dump());});
    auto bounded=catalogue;bounded.append(nimby::detail::Translations::maximumBytes-bounded.size(),' ');
    input["translations"]=bounded;assert(options::decodeDeclaration(input.dump()).translations);
    bounded+=' ';input["translations"]=bounded;invalid([&]{options::decodeDeclaration(input.dump());});
}
void persistence(){
    const auto definition=options::decodeDeclaration(schema().dump()).definition;
    const std::map<std::string,std::string> saved{{"active","false"},{"count","12"},{"mode","b"},{"window.clock",""},{"removed.option","old"}};
    assert(options::parseSaved(options::encodeSaved(saved))==saved);
    assert(options::savedValues(definition,saved)==std::vector<std::string>({"false","12","b",""}));
    const auto defaults=options::savedValues(definition,{});assert(defaults==std::vector<std::string>({"true","4","a","Ctrl+Shift+T"}));
    const std::map<std::string,std::string> old{{"active","yes"},{"count","65"},{"mode","oldMode"},{"window.clock","Win+T"},{"removed.option",""}};
    assert(options::savedValues(definition,old)==defaults);
    assert(old.at("mode")=="oldMode"); // A downgrade must not erase unknown/newer choices.
    for(const auto text:{"", "null", "[]", R"({"values":{}})",R"({"version":1})",
        R"({"version":2,"values":{}})",R"({"version":true,"values":{}})",R"({"version":1.0,"values":{}})",
        R"({"version":"1","values":{}})",R"({"version":1,"values":[],"extra":0})",
        R"({"version":1,"values":{"active":true}})",R"({"version":1,"values":{"count":12}})",
        R"({"version":1,"values":{".":"x"}})",R"({"version":1,"values":{"active":"true","active":"false"}})",
        R"({"version":1,"version":1,"values":{}})",R"({"version":1,"values":{},"extra":{}})"})invalid([&]{options::parseSaved(text);});
    std::map<std::string,std::string> many;
    for(unsigned i=0;i<256;++i)many.emplace("option"+std::to_string(i),"value");
    assert(options::parseSaved(options::encodeSaved(many)).size()==256);
    many.emplace("extra","value");invalid([&]{options::encodeSaved(many);});
    invalid([&]{options::parseSaved(Json({{"version",1},{"values",many}}).dump());});
    many.erase("extra");for(auto& [id,value]:many)value=std::string(256,'a');invalid([&]{options::encodeSaved(many);});
    std::map<std::string,std::string> longValue{{"value",std::string(256,'a')}};assert(options::parseSaved(options::encodeSaved(longValue))==longValue);
    longValue["value"]+='a';invalid([&]{options::encodeSaved(longValue);});
    invalid([&]{options::parseSaved(Json({{"version",1},{"values",longValue}}).dump());});
    auto padded=options::encodeSaved({});padded.append(options::maximumSavedBytes-padded.size(),' ');assert(options::parseSaved(padded).empty());
    padded+=' ';invalid([&]{options::parseSaved(padded);});
    invalid([]{options::encodeSaved({{"value",std::string("bad\0value",9)}});});
    invalid([]{options::encodeSaved({{"bad/id","value"}});});
}
void historyBudget(){
    const auto definition=options::decodeDeclaration(schema().dump()).definition;
    const auto values=options::savedValues(definition,{});
    const std::map<std::string,std::string> previous{{"removed","kept"},{"count","7"}};
    const auto merged=options::mergeSaved(definition.fields,values,previous);
    assert(merged.at("removed")=="kept"&&merged.at("count")=="4"&&previous.at("count")=="7");
    std::map<std::string,std::string> countFull;
    for(unsigned i=0;i<256;++i)countFull.emplace("old"+std::to_string(i),"value");
    auto result=options::mergeSaved(definition.fields,values,countFull);
    assert(result.size()==256&&result.contains("active")&&result.contains("count")&&result.contains("window.clock"));
    assert(options::parseSaved(options::encodeSaved(result))==result);
    // Escaping makes a file large with fewer than 256 historical rows. The
    // original file is valid, but introducing a new 64-field schema would
    // overflow its byte budget without retiring stale rows.
    std::map<std::string,std::string> bytesFull;
    for(unsigned i=0;i<42;++i)bytesFull.emplace("old"+std::to_string(i),std::string(256,'\x01'));
    assert(options::encodeSaved(bytesFull).size()<=options::maximumSavedBytes);
    options::Definition large{"large","Large",{}};std::vector<std::string> current;
    for(unsigned i=0;i<64;++i){
        const auto id=std::string(124,'a')+std::to_string(i);
        large.fields.push_back({id,"Enabled","",options::Kind::Boolean,"true"});current.push_back("true");
    }
    result=options::mergeSaved(large.fields,current,bytesFull);
    assert(result.size()<bytesFull.size()+large.fields.size());
    for(const auto& field:large.fields)assert(result.at(field.id)=="true");
    assert(options::encodeSaved(result).size()<=options::maximumSavedBytes);
    assert(options::parseSaved(options::encodeSaved(result))==result);
    invalid([&]{options::mergeSaved(large.fields,{},bytesFull);});
    auto invalidValues=values;invalidValues[0]="notBoolean";
    invalid([&]{options::mergeSaved(definition.fields,invalidValues,previous);});
}
}
int main(){declarations();boundsAndUtf8();translations();persistence();historyBudget();
    std::cout<<"PASS: mod options JSON types, limits, depth, duplicate keys, translations and saved-value compatibility\n";}
