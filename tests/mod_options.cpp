#include <runtime/mod_options_registry.h>
#include <cassert>
#include <iostream>
#include <thread>

namespace options=nimby::runtime::mod_options;
namespace keys=nimby::engine::mod_shortcuts;

namespace {
options::Field shortcut(std::string id,std::string chord){
    options::Field result;result.id=std::move(id);result.label=result.id;result.kind=options::Kind::Shortcut;result.defaultValue=std::move(chord);return result;
}
options::Definition definition(std::string id,std::string chord){return {id,id,{shortcut("window.clock",std::move(chord))}};}
keys::Event key(std::string_view value){const auto chord=keys::parse(value);assert(chord);return {chord->key,chord->modifiers,false};}
const keys::Context game{true,false,false};
struct Wake final:options::Wake {
    options::Registry* registry=nullptr;uint64_t token=0;mutable size_t calls=0;
    void notify()const noexcept override{
        ++calls;
        // These methods acquire the registry mutex: notification must happen
        // after dispatch/change release it, including unregistering oneself.
        if(registry&&token)assert(registry->discardEvents(token));
    }
};
void defaultsAndValidation(){
    options::Registry registry;
    assert(registry.snapshot()->mods.empty()&&!registry.snapshot()->nativeBindingsKnown);
    auto mod=definition("clock","Ctrl+Shift+T");
    mod.fields.push_back({"enabled","Enabled","Description",options::Kind::Boolean,"true"});
    mod.fields.push_back({"count","Count","",options::Kind::Integer,"4",-12,64});
    mod.fields.push_back({"mode","Mode","",options::Kind::Choice,"a",0,0,{{"a","A"},{"b","B"}}});
    const auto added=registry.add(1,mod);assert(added&&added.token);
    const auto old=registry.snapshot();assert(old->mods.size()==1&&old->mods[0].values[1]=="true");
    assert(old->mods[0].conflicts.at("window.clock")==options::Registry::nativeUnavailable);
    assert(!registry.dispatch(key("Ctrl+Shift+T"),game));
    assert(registry.change(added.token,"window.clock","F8").status==options::Status::NativeUnavailable);
    assert(registry.change(added.token,"enabled","false")); // Unknown native keys do not block other option types.
    assert(old->mods[0].values[1]=="true"&&registry.snapshot()->mods[0].values[1]=="false");
    for(const auto value:{"+1","01","-0","-01","2147483648","-2147483649","4 ","-13","65","NaN",""})assert(registry.change(added.token,"count",value).status==options::Status::Invalid);
    assert(registry.change(added.token,"count","-12"));assert(registry.change(added.token,"count","64"));
    assert(registry.change(added.token,"enabled","1").status==options::Status::Invalid);
    assert(registry.change(added.token,"mode","other").status==options::Status::Invalid);
    assert(registry.change(added.token,"mode","b"));
    assert(registry.change(added.token,"unknown","x").status==options::Status::NotFound);
    assert(registry.add(1,definition("different","F8")).status==options::Status::Duplicate);
    assert(registry.add(2,mod).status==options::Status::Duplicate);
    assert(registry.add(0,mod).status==options::Status::Invalid);
    assert(registry.add(2,definition("bad/id","F8")).status==options::Status::Invalid);
    for(const auto id:{".","..","0mod","-mod","_mod"})assert(!options::Registry::validId(id));
    auto bad=definition("bad","F8");bad.fields.push_back(bad.fields.front());assert(registry.add(2,bad).status==options::Status::Invalid);
    bad=definition("bad","F8");bad.title=std::string("bad\0title",9);assert(!options::Registry::validDefinition(bad));
    bad.title="\xc0\xaf";assert(!options::Registry::validDefinition(bad));
    bad.title="\xed\xa0\x80";assert(!options::Registry::validDefinition(bad));
    bad.title="Français 日本語";assert(options::Registry::validDefinition(bad));
    assert(registry.listNativeBindings({}));assert(registry.reset(added.token));
    const auto reset=registry.snapshot();assert(reset->mods[0].values[1]=="true"&&reset->mods[0].values[2]=="4"&&reset->mods[0].values[3]=="a");
    assert(registry.reset(added.token).status==options::Status::Unchanged);
}
void nativeAndModConflicts(){
    options::Registry registry;assert(registry.listNativeBindings({{*keys::parse("F8"),"Native action"}}));
    const auto a=registry.add(1,definition("first","F8"));assert(a);
    assert(registry.snapshot()->mods[0].conflicts.at("window.clock")=="Native action");
    assert(registry.snapshot()->mods[0].conflictSources.empty());
    assert(!registry.dispatch(key("F8"),game));
    assert(registry.change(a.token,"window.clock","Ctrl+Shift+T"));
    auto denied=registry.change(a.token,"window.clock","F8");assert(denied.status==options::Status::Conflict&&denied.conflict=="Native action");
    assert(denied.conflictOwner==0&&denied.conflictField.empty());
    assert(registry.snapshot()->mods[0].values[0]=="Ctrl+Shift+T");
    const auto b=registry.add(2,definition("second","F9"));assert(b);
    denied=registry.change(a.token,"window.clock","F9");assert(denied.status==options::Status::Conflict&&denied.conflict.find("second")!=std::string::npos);
    assert(denied.conflictOwner==b.token&&denied.conflictField=="window.clock");
    assert(registry.dispatch(key("Ctrl+Shift+T"),game)==1);
    assert(registry.dispatch(key("F9"),game)==1);
    const auto eventA=registry.poll(a.token),eventB=registry.poll(b.token);assert(eventA&&eventB&&eventA->sequence!=eventB->sequence);
    // User changes a native shortcut after the mod page was drawn: current
    // native state wins both assignment validation and input dispatch.
    assert(registry.dispatch(key("F9"),game));
    assert(registry.listNativeBindings({{*keys::parse("F9"),"Renamed native action"}}));
    assert(!registry.poll(b.token)&&!registry.dispatch(key("F9"),game));
    assert(registry.snapshot()->mods[1].conflicts.at("window.clock")=="Renamed native action");
    assert(registry.change(b.token,"window.clock","F10"));
    assert(registry.listNativeBindings({},false));
    assert(!registry.dispatch(key("F10"),game));
    assert(registry.change(b.token,"window.clock", "")); // Unassigning remains possible when verification is unavailable.
    assert(registry.listNativeBindings({}));
    assert(registry.change(b.token,"window.clock","Ctrl+Shift+T").status==options::Status::Conflict);
    // Default collisions do not crash either mod; both bindings are disabled
    // until the player chooses an unambiguous assignment.
    const auto c=registry.add(3,definition("third","Ctrl+Shift+T"));assert(c);
    assert(!registry.dispatch(key("Ctrl+Shift+T"),game));
    assert(registry.snapshot()->mods[0].conflicts.contains("window.clock"));
    assert(registry.snapshot()->mods[2].conflicts.contains("window.clock"));
    const auto collision=registry.snapshot();
    assert(collision->mods[0].conflictSources.at("window.clock")==std::make_pair(c.token,std::string("window.clock")));
    assert(collision->mods[2].conflictSources.at("window.clock")==std::make_pair(a.token,std::string("window.clock")));
    assert(registry.change(c.token,"window.clock","F11"));
    assert(registry.snapshot()->mods[0].conflictSources.empty()&&registry.snapshot()->mods[2].conflictSources.empty());
    assert(!collision->mods[0].conflictSources.empty()); // Published collision identities stay immutable.
    assert(registry.dispatch(key("Ctrl+Shift+T"),game));
    options::Entry final;
    assert(registry.remove(a.token,&final));assert(!registry.poll(a.token));
    assert(final.token==a.token&&final.values[0]=="Ctrl+Shift+T");
    assert(registry.dispatch(key("F11"),game)&&registry.poll(c.token));
    assert(registry.remove(c.token));assert(!registry.dispatch(key("F11"),game));
    const auto again=registry.add(4,definition("third","F11"));assert(again&&again.token!=c.token);
    assert(!registry.poll(again.token));
}
void sameModAndAtomicReset(){
    options::Registry registry;assert(registry.listNativeBindings({}));
    auto mod=definition("clock","F8");mod.fields.push_back(shortcut("window.other","F9"));
    const auto added=registry.add(1,mod);assert(added);
    const auto duplicate=registry.change(added.token,"window.other","F8");
    assert(duplicate.status==options::Status::Conflict&&duplicate.conflictOwner==added.token&&duplicate.conflictField=="window.clock");
    assert(registry.change(added.token,"window.clock","F10"));
    assert(registry.change(added.token,"window.other","F11"));
    assert(registry.listNativeBindings({{*keys::parse("F9"),"Native F9"}}));
    assert(registry.reset(added.token).status==options::Status::Conflict);
    assert(registry.snapshot()->mods[0].values==std::vector<std::string>({"F10","F11"}));
    assert(registry.reset(added.token,"window.clock"));
    assert(registry.snapshot()->mods[0].values==std::vector<std::string>({"F8","F11"}));
}
void queuesGuardsAndWake(){
    options::Registry registry;assert(registry.listNativeBindings({}));
    auto mod=definition("clock","F8");mod.fields.push_back(shortcut("window.other","F9"));
    const auto added=registry.add(1,mod);assert(added);
    const auto wake=std::make_shared<Wake>();assert(registry.setWake(added.token,wake));
    const auto now=options::Registry::Clock::now();
    for(unsigned i=0;i<1000;++i)assert(registry.dispatch(key("F8"),game,now)==1);
    assert(wake->calls==1000);assert(registry.poll(added.token,now)&&!registry.poll(added.token,now));
    assert(registry.dispatch(key("F8"),game,now));
    assert(!registry.poll(added.token,now+options::Registry::eventLifetime+std::chrono::milliseconds(1)));
    assert(registry.dispatch(key("F8"),game,now));assert(registry.discardEvents(added.token));assert(!registry.poll(added.token,now));
    assert(!registry.dispatch(key("F8"),{false,false,false}));
    assert(!registry.dispatch(key("F8"),{true,true,false}));
    assert(!registry.dispatch(key("F8"),{true,false,true}));
    auto repeated=key("F8");repeated.repeat=true;assert(!registry.dispatch(repeated,game));
    assert(registry.dispatch(key("F8"),game));assert(registry.change(added.token,"window.clock","F10"));assert(!registry.poll(added.token));
    const auto oldRevision=registry.snapshot()->mods[0].revision;
    assert(registry.change(added.token,"window.clock","F12"));
    assert(registry.dispatch(key("F12"),game,now));
    assert(!registry.poll(added.token,now,oldRevision));
    const auto currentRevision=registry.snapshot()->mods[0].revision;
    assert(registry.poll(added.token,now,currentRevision)); // Old broker view did not eat the new event.
    wake->registry=&registry;wake->token=added.token;
    assert(registry.dispatch(key("F9"),game));assert(!registry.poll(added.token)); // Reentrant wake drained outside lock.
    assert(registry.change(added.token,"window.clock","F11"));
    wake->registry=nullptr;
    // A full queue affects only its own registration, not another mod.
    options::Definition large{"large","Large",{}};
    for(unsigned i=0;i<33;++i)large.fields.push_back(shortcut("key"+std::to_string(i),std::string("Ctrl+")+char('A'+i%26)));
    // Use distinct canonical bindings: 26 Ctrl+letters then 7 Alt+letters.
    for(unsigned i=26;i<33;++i)large.fields[i].defaultValue=std::string("Alt+")+char('A'+i-26);
    const auto second=registry.add(2,large);assert(second);
    for(unsigned i=0;i<32;++i)assert(registry.dispatch(key(large.fields[i].defaultValue),game));
    assert(!registry.dispatch(key(large.fields[32].defaultValue),game));
    assert(registry.dispatch(key("F9"),game));assert(registry.poll(added.token));
    size_t count=0;while(registry.poll(second.token))++count;assert(count==32);
}
void concurrentSnapshots(){
    options::Registry registry;assert(registry.listNativeBindings({}));
    const auto added=registry.add(1,{"bool","Boolean",{{"enabled","Enabled","",options::Kind::Boolean,"false"}}});assert(added);
    std::atomic<bool> stop=false;
    std::thread reader([&]{uint64_t previous=0;while(!stop){const auto snapshot=registry.snapshot();assert(snapshot->revision>=previous&&snapshot->mods.size()==1);previous=snapshot->revision;assert(snapshot->mods[0].values[0]=="true"||snapshot->mods[0].values[0]=="false");}});
    for(unsigned i=0;i<1000;++i)assert(registry.change(added.token,"enabled",i%2?"true":"false"));
    stop=true;reader.join();
}
}
int main(){defaultsAndValidation();nativeAndModConflicts();sameModAndAtomicReset();queuesGuardsAndWake();concurrentSnapshots();
    std::cout<<"PASS: typed mod options, native/mod conflicts, atomic reset, immutable snapshots and isolated bounded shortcut queues\n";}
