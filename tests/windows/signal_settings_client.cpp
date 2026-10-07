#include <nimby/detail/signal_settings_client.hpp>
#include <cstdio>
namespace nimby::detail {
struct SignalSettingsClientTest {
    inline static NimbyUiExportV1 nativeExport{};
    inline static uint32_t exports{};
    static uint32_t countedExport(uint64_t owner,uint64_t session,char* bytes,uint32_t capacity,uint32_t* written){
        ++exports;return nativeExport(owner,session,bytes,capacity,written);
    }
    static bool measure(SignalSettingsClient& client){
        nativeExport=client.export_;client.export_=countedExport;exports=0;return client.revision_!=nullptr;
    }
    static void checkpoint(SignalSettingsClient& client){client.nextSave_={};client.checkpoint(false);}
    inline static uint32_t batchMode{};
    static uint32_t changingBatch(uint64_t,const uint64_t* ids,uint32_t count,NimbyUiReadBatchHeaderV1* header,NimbyUiReadBatchRowV1* rows){
        *header={};header->size=sizeof(*header);header->version=1;header->count=count;header->field_count=1;
        std::strcpy(header->names[0],batchMode==2?"changed":"active");
        for(uint32_t i=0;i<count;++i){rows[i]={};rows[i].signal=ids[i];rows[i].status=batchMode==3?1:batchMode==4?0:2;rows[i].values=batchMode==1||batchMode==2?1:0;}
        if(batchMode==5&&count)rows[0].signal=0;
        return NIMBY_OK;
    }
    static bool changingValues(SignalSettingsClient& client,std::span<const SignalSettingsStore::Signal> signals){
        const auto previous=client.readBatch_;client.readBatch_=changingBatch;
        struct Restore{SignalSettingsClient& client;NimbyUiReadBatchV1 previous;~Restore(){client.readBatch_=previous;client.cached_=false;client.cache_.clear();}} restore{client,previous};
        for(batchMode=0;batchMode<5;++batchMode){
            if(!client.refresh(signals))return false;
            const auto values=client.read(signals[0].id);
            if(batchMode<3){if(values.getBoolean(batchMode==2?"changed":"active")!=std::optional<bool>(batchMode!=0))return false;
                if(batchMode==2&&values.getBoolean("active"))return false;
            }else if(values.status!=(batchMode==3?SettingsStatus::Absent:SettingsStatus::Unavailable)||!values.booleans.empty())return false;
        }
        batchMode=5;return !client.refresh(signals)&&!client.cached_;
    }
};
}
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
    CHECK(nimby::detail::SignalSettingsClientTest::changingValues(client,signals));
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
    using Probe=nimby::detail::SignalSettingsClientTest;CHECK(Probe::measure(client));
    CHECK(client.synchronize(world,signals));
    const auto firstExports=Probe::exports;CHECK(firstExports==2);
    for(int i=0;i<10;++i){CHECK(client.synchronize(world,signals));Probe::checkpoint(client);}
    CHECK(Probe::exports==firstExports); // No repeated serialization or export RPC.
    CHECK(client.read(id).getBoolean("active")==false);
    client.close();CHECK(Probe::exports==firstExports);CHECK(std::filesystem::last_write_time(path)==before);
    CHECK(client.connectExisting(panel));CHECK(Probe::measure(client));CHECK(client.synchronize(world,signals));
    const auto beforePrune=Probe::exports;CHECK(client.synchronize(world,{}));CHECK(Probe::exports==beforePrune);
    client.close();CHECK(Probe::exports==beforePrune+2); // Forced close saves a change inside the 250 ms cadence.
    CHECK(nimby::detail::SignalSettingsFile::load(path)->signals.empty());
    nimby::detail::SignalSettingsFile::save(path,profile);
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
