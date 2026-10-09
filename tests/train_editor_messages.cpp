#include <engine/train_editor_messages.h>
#include <engine/train_length_policy.h>
#include <atomic>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <thread>

namespace editor=nimby::engine::train_editor;
namespace policy=nimby::engine::train_length;
using Json=nlohmann::json;
#define CHECK(value) do {if(!(value))throw std::runtime_error("Train editor message check failed: " #value);} while(false)

std::string reference(std::string key,Json arguments=Json::object()) {
    return std::string(nimby::detail::Translations::prefix)+Json::array({std::move(key),std::move(arguments)}).dump();
}
std::string catalogue() {
    return Json({{"fallback","en"},{"languages",{
        {"en",{{"exceeded","Addition refused."},{"length","Vehicle length unavailable."},{"verify","Cannot verify composition."}}},
        {"fr",{{"exceeded","Ajout refusé."},{"length","Longueur du véhicule indisponible."},{"verify","Composition impossible à vérifier."}}},
        {"fr-CA",{{"exceeded","Ajout refusé au Canada."}}}
    }}}).dump();
}
std::string declaration(Json messages,std::string translations={}) {
    return Json({{"messages",std::move(messages)},{"translations",std::move(translations)}}).dump();
}
std::string translated(std::string translations=catalogue()) {
    return declaration(Json::array({reference("exceeded"),reference("length"),reference("verify")}),std::move(translations));
}
std::shared_ptr<const editor::Messages> plain(std::string text) {
    return std::make_shared<const editor::Messages>(declaration(Json::array({text,text,text})));
}
void rejected(const std::string& bytes) {
    bool refused=false;
    try {editor::Messages messages(bytes);}
    catch(const std::invalid_argument&){refused=true;}
    CHECK(refused);
}

void localeContracts() {
    auto bytes=translated();editor::Messages messages(bytes);
    bytes.assign(bytes.size(),'x'); // All source JSON and text have been copied.
    CHECK(messages.fallback()=="en"&&messages.languageCount()==3);
    for(const auto raw:{"fr","fra","fre","FR","FRA","fr-FR","fra_FR"})
        CHECK(messages.select(0,raw)=="Ajout refusé.");
    CHECK(messages.select(0,"FR_ca")=="Ajout refusé au Canada.");
    CHECK(messages.select(0,"fra-CA")=="Ajout refusé au Canada.");
    CHECK(messages.select(1,"fr-CA")=="Longueur du véhicule indisponible.");
    CHECK(messages.select(2,"fra")=="Composition impossible à vérifier.");
    for(const auto raw:{"eng","en-GB","deu","unknown","","--","fr/FR"})
        CHECK(messages.select(0,raw)=="Addition refused.");
    const auto* frozen=&messages.select(1,"fra");
    for(size_t i=0;i<2000;++i)CHECK(&messages.select(1,"fra")==frozen);
    bool badIndex=false;
    try {messages.select(3,"fr");}catch(const std::invalid_argument&){badIndex=true;}
    CHECK(badIndex);
    const auto frenchFallback=Json({{"fallback","fr"},{"languages",{
        {"fr",{{"exceeded","Refus français."},{"length","Longueur inconnue."},{"verify","Vérification impossible."}}}
    }}}).dump();
    CHECK(editor::Messages(translated(frenchFallback)).select(0,"eng")=="Refus français.");
}

void validationContracts() {
    const std::string data="100% %n %s %.3f {literal}";
    const editor::Messages literal(declaration(Json::array({data,"No catalogue required.","Third message."})));
    CHECK(literal.select(0,"fra")==data&&literal.select(1,"eng")=="No catalogue required.");
    CHECK(literal.select(0,"fr-CA")==data&&literal.languageCount()==1);
    const std::string boundary(editor::Messages::maximumMessageBytes,'a');
    CHECK(editor::Messages(declaration(Json::array({boundary,"b","c"}))).select(0,"en")==boundary);
    // The declared bound counts UTF-8 bytes, not code points.
    std::string unicodeBoundary;
    for(size_t i=0;i<editor::Messages::maximumMessageBytes/2;++i)unicodeBoundary+="é";
    CHECK(unicodeBoundary.size()==editor::Messages::maximumMessageBytes);
    CHECK(editor::Messages(declaration(Json::array({unicodeBoundary,"b","c"}))).select(0,"fra")==unicodeBoundary);
    rejected(declaration(Json::array({boundary+"a","b","c"})));
    rejected(declaration(Json::array({unicodeBoundary+"é","b","c"})));
    rejected(declaration(Json::array({"","b","c"})));
    rejected(declaration(Json::array({std::string("x\0y",3),"b","c"})));
    rejected(declaration(Json::array({std::string("x\x01y",3),"b","c"})));
    rejected(declaration(Json::array({reference("missing"),"b","c"}),catalogue()));
    rejected(declaration(Json::array({reference("exceeded"),"b","c"})));
    rejected(declaration(Json::array({reference("exceeded",Json({{"length","850"}})),"b","c"}),catalogue()));
    auto parameterized=Json::parse(catalogue());
    parameterized["languages"]["en"]["exceeded"]="Maximum {length} exceeded.";
    parameterized["languages"]["fr"]["exceeded"]="Maximum {length} dépassé.";
    parameterized["languages"]["fr-CA"]["exceeded"]="Maximum {length} dépassé au Canada.";
    rejected(translated(parameterized.dump()));
    rejected(declaration(Json::array({"a","b"})));
    rejected(declaration(Json::array({"a","b","c","d"})));
    rejected(declaration(Json::array({1,"b","c"})));
    rejected(R"({"messages":["a","b","c"],"messages":["x","y","z"],"translations":""})");
    rejected(R"({"messages":["a","b","c"]})");
    rejected(std::string(R"({"messages":[")")+char(0xc0)+char(0xaf)+R"(","b","c"],"translations":""})");
    rejected(std::string(editor::Messages::maximumDeclarationBytes+1,'x'));
}

void ownershipContracts() {
    policy::Registry<editor::Messages> registry;
    CHECK(!registry.selected()&&!registry.limit().active());
    auto alpha=plain("Alpha refusal"),beta=plain("Beta refusal");
    CHECK(registry.replace(10,850,alpha)==policy::PublishResult::Applied);
    auto selection=registry.selected();
    CHECK(selection&&selection->owner==10&&selection->limit.maximumMeters==850&&selection->presentation==alpha);
    CHECK(selection->revision==registry.selectionRevision());
    CHECK(registry.replace(20,700,beta)==policy::PublishResult::Applied);
    CHECK(registry.selected()->owner==20&&registry.selected()->presentation->select(0,"fra")=="Beta refusal");
    CHECK(registry.update(10,650)==policy::PublishResult::Applied);
    selection=registry.selected();
    CHECK(selection->owner==10&&selection->limit.maximumMeters==650&&selection->presentation==alpha);
    const auto revision=selection->revision;
    CHECK(registry.update(10,0)==policy::PublishResult::InvalidLimit);
    CHECK(registry.replace(0,700,beta)==policy::PublishResult::InvalidOwner);
    CHECK(registry.remove(30)==policy::PublishResult::NotFound);
    CHECK(registry.update(30,700)==policy::PublishResult::NotFound);
    CHECK(registry.selectionRevision()==revision&&registry.selected()==selection);
    CHECK(registry.remove(10)==policy::PublishResult::Applied);
    CHECK(registry.selected()->owner==20&&registry.selected()->limit.maximumMeters==700&&registry.selected()->presentation==beta);
    CHECK(selection->owner==10&&selection->presentation->select(0,"fr")=="Alpha refusal");
    CHECK(selection->revision!=registry.selectionRevision());
    CHECK(registry.remove(20)==policy::PublishResult::Applied);
    CHECK(!registry.selected()&&!registry.limit().active());
    CHECK(registry.replace(40,850,alpha)==policy::PublishResult::Applied);
    const auto oldNotice=registry.selected();
    CHECK(registry.replace(50,850,beta)==policy::PublishResult::Applied);
    CHECK(registry.selected()->owner==40); // Equal limits select the smaller opaque token.
    CHECK(registry.remove(40)==policy::PublishResult::Applied);
    CHECK(registry.limit().maximumMeters==850&&registry.selected()->owner==50);
    CHECK(registry.selectionRevision()!=oldNotice->revision); // Same value, different owner invalidates the notice.
    CHECK(registry.selected()->presentation->select(0,"en")=="Beta refusal");

    policy::Registry<editor::Messages> lifetime;
    auto detached=plain("Frozen refusal");std::weak_ptr<const editor::Messages> weak=detached;
    CHECK(lifetime.replace(60,850,detached)==policy::PublishResult::Applied);
    auto frozen=lifetime.selected();detached.reset();
    CHECK(lifetime.remove(60)==policy::PublishResult::Applied&&!weak.expired());
    CHECK(frozen->presentation->select(0,"fra")=="Frozen refusal");
    frozen.reset();CHECK(weak.expired());
}

void concurrentContracts() {
    policy::Registry<editor::Messages> registry;
    auto alpha=plain("Alpha"),beta=plain("Beta");
    CHECK(registry.replace(10,850,alpha)==policy::PublishResult::Applied);
    std::atomic<bool> stop=false,started=false,bad=false;
    std::atomic<size_t> samples=0;
    std::thread reader([&]{
        started.store(true,std::memory_order_release);
        do {
            const auto snapshot=registry.selected();
            if(!snapshot||!snapshot->revision)bad=true;
            else if(snapshot->owner==10){
                if(snapshot->presentation!=alpha||(snapshot->limit.maximumMeters!=850&&snapshot->limit.maximumMeters!=650))bad=true;
            }else if(snapshot->owner==20){
                if(snapshot->presentation!=beta||snapshot->limit.maximumMeters!=700)bad=true;
            }else bad=true;
            samples.fetch_add(1,std::memory_order_relaxed);
        }while(!stop.load(std::memory_order_acquire));
    });
    while(!started.load(std::memory_order_acquire))std::this_thread::yield();
    for(size_t i=0;i<2000;++i){
        CHECK(registry.replace(20,700,beta)==policy::PublishResult::Applied);
        CHECK(registry.update(10,650)==policy::PublishResult::Applied);
        CHECK(registry.update(10,850)==policy::PublishResult::Applied);
        CHECK(registry.remove(20)==policy::PublishResult::Applied);
    }
    stop.store(true,std::memory_order_release);reader.join();
    CHECK(!bad&&samples>0&&registry.selected()->owner==10&&registry.selected()->presentation==alpha);
}

int main() {
    try {
        localeContracts();validationContracts();ownershipContracts();concurrentContracts();
        std::cout<<"PASS: resident train editor messages, ISO locales, validation bounds, immutable strictest owner and retirement\n";
    }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
