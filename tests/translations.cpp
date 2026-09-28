#include "runtime/signal_ui_endpoint.h"
#include <iostream>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Line "+std::to_string(__LINE__)+": " #x);}while(false)
using nimby::detail::Translations;
const std::string catalog=R"({"fallback":"en","languages":{"en":{"title":"Options","repeat":"Repeat","count":"{count} signals","brace":"{{OK}}"},"fr":{"title":"Réglages","repeat":"Répéter","count":"{count} signaux"},"fr-CA":{"title":"Options CA"}}})";
std::string ref(std::string key){return std::string(Translations::prefix)+"[\""+key+"\",{}]";}
void catalogs(){
    Translations t(catalog);
    CHECK(Translations::resolve(&t,ref("title"),"fra")=="Réglages");
    CHECK(Translations::resolve(&t,ref("title"),"FR_ca")=="Options CA");
    CHECK(Translations::resolve(&t,ref("repeat"),"fr-CA")=="Répéter");
    CHECK(Translations::resolve(&t,ref("brace"),"fr")=="{OK}");
    CHECK(Translations::resolve(&t,ref("title"),"jpn")=="Options");
    CHECK(Translations::resolve(&t,ref("title"),"")=="Options");
    CHECK(Translations::resolve(&t,ref("unknown"),"fr")=="[unknown]");
    CHECK(Translations::resolve(nullptr,ref("title"),"fr")=="[title]");
    CHECK(Translations::resolve(&t,"literal", "fr")=="literal");
    const auto count=std::string(Translations::prefix)+R"(["count",{"count":"3"}])";
    CHECK(Translations::resolve(&t,count,"fra")=="3 signaux");
    CHECK(Translations::resolve(&t,ref("count"),"fra")=="[count]");
    for(const auto& bad:{std::string("{}"), std::string(R"({"languages":{"en":{"x":"{broken"}}})"),
        std::string(R"({"languages":{"en":{"x":"A","x":"B"}}})"),
        std::string(R"({"languages":{"en":{"x":"{n}"},"fr":{"x":"{other}"}}})"),
        std::string(R"({"languages":{"en":{"x":"ok"},"fr":{"extra":"oops"}}})"),
        std::string(R"({"fallback":"fr","languages":{"en":{"x":"ok"}}})"),
        std::string(R"({"languages":{"en":{"x":"\u0000"}}})"),
        std::string("{\"languages\":{\"en\":{\"x\":\"")+char(0xff)+"\"}}}"}){
        bool rejected=false;try{Translations invalid(bad);}catch(const std::invalid_argument&){rejected=true;}CHECK(rejected);
    }
}
void frames(){
    using namespace nimby::runtime;
    SignalUiEndpoint e;constexpr uint64_t signal=0x8000000000001;
    NimbyUiPanelV1 panel{};panel.size=sizeof panel;panel.version=1;panel.count=1;
    std::strcpy(panel.id,"panel");std::strcpy(panel.texture_set,"atlas");std::strcpy(panel.title,ref("title").c_str());
    std::strcpy(panel.checkboxes[0].name,"flag");std::strcpy(panel.checkboxes[0].label,ref("repeat").c_str());
    uint64_t owner{},session{};CHECK(e.add(&panel,&owner)==NIMBY_OK);
    CHECK(e.translations(0,owner,catalog.data(),uint32_t(catalog.size()))==NIMBY_OK);
    CHECK(e.translations(1,owner,catalog.data(),uint32_t(catalog.size()))==NIMBY_INVALID_HANDLE);
    NimbyUiActionV1 action{};std::strcpy(action.id,"repeat");std::strcpy(action.label,ref("repeat").c_str());
    std::strcpy(action.provider,"tool");std::strcpy(action.service,"repeat.v1");CHECK(e.actions(owner,&action,1)==NIMBY_OK);
    CHECK(e.begin(owner,"world",5,&session)==NIMBY_OK);CHECK(e.panelContext(owner,session,1)==NIMBY_OK);
    NimbyUiSignalV1 row{};row.id=signal;std::strcpy(row.texture_set,"atlas");CHECK(e.observe(owner,session,&row,1)==NIMBY_OK);
    auto fr=e.host.prepare(1,signal,"fra"),en=e.host.prepare(2,signal,"eng");
    CHECK(fr.panels[0].controls.title=="Réglages"&&en.panels[0].controls.title=="Options");
    CHECK(fr.panels[0].controls.editor.session==en.panels[0].controls.editor.session);
    CHECK(fr.panels[0].controls.controls[0].checkbox.name=="flag");
    CHECK(e.translations(0,owner,"{}",2)==NIMBY_INVALID_ARGUMENT);
    CHECK(e.host.prepare(3,signal,"fr").panels[0].controls.title=="Réglages"); // transactional rejection
    NimbyUiProviderV1 provider{};provider.size=sizeof provider;provider.version=1;provider.count=1;
    std::strcpy(provider.id,"tool");std::strcpy(provider.services[0],"repeat.v1");uint64_t token{};
    CHECK(e.addProvider(&provider,&token)==NIMBY_OK);
    const std::string different=R"({"languages":{"en":{"repeat":"Tool repeat"}}})";
    CHECK(e.translations(1,token,different.data(),uint32_t(different.size()))==NIMBY_OK);
    CHECK(e.observeProvider(token,"world",5,1)==NIMBY_OK);
    auto initial=e.host.prepare(4,signal,"fr");CHECK(initial.actions[0].action.label=="Répéter");
    NimbyUiToolPanelV1 tool{};tool.size=sizeof tool;tool.version=1;tool.panel=owner;tool.signal=signal;tool.count=1;
    std::strcpy(tool.origin,"repeat");std::strcpy(tool.service,"repeat.v1");std::strcpy(tool.buttons[0].id,"repeat");
    std::strcpy(tool.buttons[0].label,ref("repeat").c_str());tool.buttons[0].enabled=1;
    CHECK(e.publishToolPanel(token,&tool)==NIMBY_OK);
    auto expanded=e.host.prepare(5,signal,"fr");CHECK(expanded.actions[0].action.label=="Tool repeat");
    CHECK(e.host.actions->click(expanded.actions[0])); // translated label cannot alter dispatch
    CHECK(e.removeProvider(token)==NIMBY_OK);CHECK(e.remove(owner)==NIMBY_OK);
    CHECK(fr.panels[0].controls.title=="Réglages"&&expanded.actions[0].action.label=="Tool repeat");
    CHECK(!e.host.actions->click(expanded.actions[0])); // owned frozen text; no stale callbacks
}
int main(){try{catalogs();frames();std::cout<<"PASS: catalogues, fallback, placeholders, ownership, language changes and Windows reader\n";}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
