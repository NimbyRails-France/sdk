#include <nimby/detail/signal_settings_file.hpp>
#include <iostream>
#define CHECK(x) do{if(!(x))throw std::runtime_error("Failed line "+std::to_string(__LINE__));}while(false)
#include "settings_file_platform.hpp"
template<class Action> void rejects(Action action){bool rejected=false;try{action();}catch(const std::exception&){rejected=true;}CHECK(rejected);}
int main(){try{
    using File=nimby::detail::SignalSettingsFile;
    using Store=nimby::SignalSettingsStore;
    Store::SavedSettings data{"save \"A\"","sfr.bal-a",{{0x8000000000001,{{"active",true},{"green",false}}}}};
    const auto encoded=File::encode(data);
    const auto restored=File::decode(encoded);
    CHECK(restored.sessionId==data.sessionId&&restored.panelId==data.panelId);
    CHECK(restored.signals.size()==1&&restored.signals[0].values==data.signals[0].values);
    for(size_t cut:{size_t{0},size_t{10},encoded.size()-1})rejects([&]{File::decode(encoded.substr(0,cut));});
    auto damaged=encoded;damaged.back()='x';rejects([&]{File::decode(damaged);});
    damaged=encoded;damaged[22]='9';rejects([&]{File::decode(damaged);});
    Temporary target;
    rejects([&]{File::load(target.path);}); // An empty file is corrupt, not absent.
    File::save(target.path,data);
    CHECK(File::load(target.path)->signals[0].values.at("active"));
    auto invalid=data;invalid.signals.push_back(invalid.signals[0]);
    rejects([&]{File::save(target.path,invalid);});
    CHECK(File::load(target.path)->signals.size()==1);
    auto changed=data;changed.signals[0].values["active"]=false;
    verifyReplacement(target,data,changed);
    File::save(target.path,changed);CHECK(!File::load(target.path)->signals[0].values.at("active"));
    CHECK(std::filesystem::remove(target.path));CHECK(!File::load(target.path));
    std::cout<<"PASS settings file roundtrip, corruption and failed replacement\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
