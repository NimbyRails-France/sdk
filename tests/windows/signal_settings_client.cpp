#include <nimby/detail/signal_settings_client.hpp>
#include <cstdio>
#define CHECK(x) do {if(!(x)){std::fprintf(stderr,"Failed line %d\n",__LINE__);return 1;}}while(false)
int main(int argc,char** argv){
    CHECK(argc==2);
    constexpr nimby::SignalCheckbox fields[]{{"active","Active","",true}};
    const nimby::SignalSettingsPanel panel{"client","Client","atlas",fields};
    nimby::detail::SignalSettingsClient client;
    CHECK(!client.connectExisting(panel)); // No implicit DLL search/load.
    auto dll=LoadLibraryA(argv[1]);CHECK(dll);
    CHECK(client.connectExisting(panel));
    CHECK(client.connected());
    CHECK(FreeLibrary(dll)); // Client owns a reference for every following call.
    constexpr uint64_t id=0x8000000000001;
    CHECK(client.read(id).status==nimby::SettingsStatus::Unavailable);
    const auto session=client.beginSession("test-save");CHECK(session);
    const nimby::SignalSettingsStore::Signal signals[]{{id,"atlas"}};
    CHECK(client.observe(session,signals));
    CHECK(client.read(id).getBoolean("active")==true);
    auto saved=client.exportSettings(session);CHECK(saved&&saved->sessionId=="test-save"&&saved->panelId=="client");
    saved->signals.push_back({id,{{"active",false}}});
    CHECK(!client.beginSession("another-save",*saved));
    CHECK(client.read(id).getBoolean("active")==true);
    client.suspend();CHECK(client.read(id).status==nimby::SettingsStatus::Unavailable);
    CHECK(client.observe(session,signals));
    CHECK(client.read(id).getBoolean("active")==true);
    const auto replacement=client.beginSession("test-save",*saved);CHECK(replacement&&replacement!=session);
    CHECK(!client.exportSettings(session));
    CHECK(!client.observe(session,signals));
    CHECK(client.observe(replacement,signals));
    CHECK(client.read(id).getBoolean("active")==false);
    saved=client.exportSettings(replacement);CHECK(saved&&saved->signals.size()==1&&!saved->signals[0].values.at("active"));
    client.close();CHECK(!client.connected());
    CHECK(client.read(id).status==nimby::SettingsStatus::Unavailable);
    CHECK(!GetModuleHandleW(L"NimbySignalUiBridge-experimental-v1.dll"));
    dll=LoadLibraryA(argv[1]);CHECK(dll);
    CHECK(client.connectExisting(panel));
    CHECK(client.read(id).status==nimby::SettingsStatus::Unavailable);
    client.close();CHECK(FreeLibrary(dll));
    // Automatic worker persistence uses a temporary LOCALAPPDATA and the real
    // resident DLL. Reconnect/reload must restore edits, isolate worlds, and
    // discard settings for IDs absent from a complete catalog.
    wchar_t originalRoot[32768]{};CHECK(GetEnvironmentVariableW(L"LOCALAPPDATA",originalRoot,32768));
    const auto temporary=std::filesystem::temp_directory_path()/("nimby-profile-test-"+std::to_string(GetCurrentProcessId()));
    CHECK(SetEnvironmentVariableW(L"LOCALAPPDATA",temporary.c_str()));
    const nimby::GameSession world{1,"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"};
    const auto path=client.profilePath(world.worldId,panel.id);
    nimby::SignalSettingsStore::SavedSettings profile{world.worldId,"client",{{id,{{"active",false}}}}};
    nimby::detail::SignalSettingsFile::save(path,profile);
    const auto before=std::filesystem::last_write_time(path);
    dll=LoadLibraryA(argv[1]);CHECK(dll);CHECK(client.connectExisting(panel));
    CHECK(client.synchronize(world,signals));
    CHECK(client.read(id).getBoolean("active")==false);
    client.close();CHECK(std::filesystem::last_write_time(path)==before);
    CHECK(client.connectExisting(panel));CHECK(client.synchronize(world,signals));
    CHECK(client.read(id).getBoolean("active")==false);
    CHECK(client.synchronize(nimby::GameSession{2,"1123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"},signals));
    CHECK(client.read(id).getBoolean("active")==true);
    CHECK(client.synchronize(nimby::GameSession{3,world.worldId},{}));
    CHECK(client.read(id).status==nimby::SettingsStatus::Absent);
    client.close();CHECK(nimby::detail::SignalSettingsFile::load(path)->signals.empty());
    {std::ofstream corrupt(path,std::ios::binary);corrupt<<"broken";}
    CHECK(client.connectExisting(panel));
    bool rejected=false;try{client.synchronize(world,signals);}catch(const std::invalid_argument&){rejected=true;}
    CHECK(rejected);client.close();CHECK(std::filesystem::file_size(path)==6);
    CHECK(FreeLibrary(dll));CHECK(SetEnvironmentVariableW(L"LOCALAPPDATA",originalRoot));
    std::filesystem::remove_all(temporary);
    std::puts("PASS settings client lifetime, session invalidation and DLL reference");
}
