#include <nimby/detail/signal_settings_client.hpp>
#include <cstdio>
#include <thread>
#include <future>
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
    inline static uint32_t batchCalls{};
    static uint32_t changingBatch(uint64_t,const uint64_t* ids,uint32_t count,NimbyUiReadBatchHeaderV1* header,NimbyUiReadBatchRowV1* rows){
        ++batchCalls;
        *header={};header->size=sizeof(*header);header->version=1;header->count=count;header->field_count=1;
        std::strcpy(header->names[0],batchMode==2?"changed":"active");
        for(uint32_t i=0;i<count;++i){rows[i]={};rows[i].signal=ids[i];rows[i].status=batchMode==3?1:batchMode==4?0:2;rows[i].values=batchMode==1||batchMode==2?1:0;}
        if(batchMode==5&&count)rows[0].signal=0;
        if(batchMode==6)return NIMBY_RESOURCE_LIMIT;
        if(batchMode==7)header->size=0;
        if(batchMode==8)header->version=2;
        if(batchMode==9)++header->count;
        if(batchMode==10)header->field_count=65;
        if(batchMode==11)std::memset(header->names[0],'x',sizeof header->names[0]);
        if(batchMode==12)header->names[0][0]=0;
        if(batchMode==13){header->field_count=2;std::strcpy(header->names[1],"active");}
        if(count){
            if(batchMode==14)rows[0].status=3;
            if(batchMode==15)rows[0].reserved=1;
            if(batchMode==16)rows[0].values=2;
            if(batchMode==17){rows[0].status=1;rows[0].values=1;}
        }
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
    static bool batchDiagnostics(SignalSettingsClient& client,std::span<const SignalSettingsStore::Signal> signals){
        using R=SignalSettingsClient::SyncReason;
        const auto previous=client.readBatch_;client.readBatch_=changingBatch;
        struct Restore{SignalSettingsClient& client;NimbyUiReadBatchV1 previous;
            ~Restore(){client.readBatch_=previous;client.cached_=false;client.cache_.clear();}} restore{client,previous};
        struct Failure{uint32_t mode;R reason;uint32_t status;uint64_t detail;};
        const Failure failures[]{
            {5,R::RowSignal,NIMBY_OK,0},{6,R::BatchRejected,NIMBY_RESOURCE_LIMIT,0},
            {7,R::BatchSize,NIMBY_OK,0},{8,R::BatchVersion,NIMBY_OK,2},
            {9,R::BatchCount,NIMBY_OK,signals.size()+1},{10,R::BatchFieldCount,NIMBY_OK,65},
            {11,R::NameUnterminated,NIMBY_OK,0},{12,R::NameEmpty,NIMBY_OK,0},
            {13,R::NameDuplicate,NIMBY_OK,1},{14,R::RowStatus,NIMBY_OK,0},
            {15,R::RowReserved,NIMBY_OK,0},{16,R::RowMask,NIMBY_OK,0},
            {17,R::RowUnexpectedValues,NIMBY_OK,0},
        };
        for(const auto& failure:failures){
            batchMode=1;if(!client.refresh(signals)||!client.cached_)return false;
            batchMode=failure.mode;const auto result=client.refresh(signals);
            if(result||client.cached_||result.reason!=failure.reason||result.status!=failure.status||
                result.count!=signals.size()||result.detail!=failure.detail)return false;
        }
        batchMode=0;
        const SignalSettingsStore::Signal duplicates[]{signals[0],signals[0]};
        const auto duplicate=client.refresh(duplicates);
        if(duplicate||duplicate.reason!=R::RowDuplicate||duplicate.detail!=1||duplicate.count!=2)return false;
        // A limit failure must not call the transport with an oversized batch.
        std::vector<SignalSettingsStore::Signal> tooMany(16385,signals[0]);
        const auto before=batchCalls;const auto limit=client.refresh(tooMany);
        if(limit||limit.reason!=R::BatchLimit||limit.count!=16385||batchCalls!=before)return false;
        client.readBatch_=nullptr;
        return bool(client.refresh(signals)); // The old-bridge fallback stays supported.
    }
    using DiagnosticSink=void(*)(const char*,const char*,const char*)noexcept;
    inline static std::vector<std::string> diagnosticMessages;
    inline static SignalSettingsClient* loggingClient{};
    inline static bool sinkUnlocked=true;
    static void captureDiagnostic(const char* channel,const char* level,const char* message)noexcept {
        try {
            if(loggingClient){
                // Check from another thread: calling try_lock on a mutex owned
                // by this thread itself is not a portable lock-ownership test.
                bool unlocked=false;
                std::thread probe([&]{if(loggingClient->mutex_.try_lock()){
                    unlocked=true;loggingClient->mutex_.unlock();}});
                probe.join();sinkUnlocked=sinkUnlocked&&unlocked;
            }
            diagnosticMessages.emplace_back(std::string(channel)+" "+level+" "+message);
        }catch(...){sinkUnlocked=false;}
    }
    struct CaptureDiagnostics {
        SignalSettingsClient& client;
        DiagnosticSink previous;
        explicit CaptureDiagnostics(SignalSettingsClient& value):client(value),previous(value.syncDiagnosticSink_){
            diagnosticMessages.clear();loggingClient=&client;sinkUnlocked=true;
            client.syncFailure_=false;client.syncDiagnosticSink_=captureDiagnostic;
        }
        ~CaptureDiagnostics(){client.syncDiagnosticSink_=previous;client.syncFailure_=false;loggingClient=nullptr;}
    };
    static bool hasDiagnostic(std::string_view reason,uint32_t status){
        if(diagnosticMessages.empty())return false;
        const auto& message=diagnosticMessages.back();
        return message.find("reason="+std::string(reason)+" ")!=std::string::npos&&
            message.find("status="+std::to_string(status)+" ")!=std::string::npos&&message.size()<1100;
    }
    static bool diagnosticTransitions(SignalSettingsClient& client){
        using R=SignalSettingsClient::SyncReason;
        using Clock=std::chrono::steady_clock;
        const auto start=Clock::now();
        GameSession game{17,std::string(64,'a')};
        CaptureDiagnostics capture(client);
        // A healthy report must complete even while a reader owns the client
        // mutex. Release before joining so a regression fails, not deadlocks.
        std::unique_lock held(client.mutex_);
        std::promise<void> completed;auto completion=completed.get_future();
        std::thread healthy([&]{client.reportSynchronization({},&game);completed.set_value();});
        const bool fast=completion.wait_for(std::chrono::seconds(1))==std::future_status::ready;
        held.unlock();healthy.join();
        if(!fast)return false;
        if(!diagnosticMessages.empty())return false;
        client.reportSynchronization({R::BatchRejected,NIMBY_RESOURCE_LIMIT,9},&game,start);
        if(diagnosticMessages.size()!=1||!hasDiagnostic("read_batch_rejected",NIMBY_RESOURCE_LIMIT))return false;
        if(diagnosticMessages.back().find("panel=client ")==std::string::npos||
            diagnosticMessages.back().find("generation=17 ")==std::string::npos||
            diagnosticMessages.back().find("textures=atlas ")==std::string::npos||
            diagnosticMessages.back().find("count=9 ")==std::string::npos)return false;
        // New counts/details do not create one log per changing observation.
        for(unsigned tick=1;tick<50;++tick)
            client.reportSynchronization({R::BatchRejected,NIMBY_RESOURCE_LIMIT,tick,tick},&game,start+std::chrono::milliseconds(tick*100));
        if(diagnosticMessages.size()!=1)return false;
        client.reportSynchronization({R::BatchRejected,NIMBY_RESOURCE_LIMIT,59},&game,start+std::chrono::seconds(5));
        if(diagnosticMessages.size()!=2||diagnosticMessages.back().find("count=59 ")==std::string::npos)return false;
        // Both reason and raw transport-status changes are immediately useful.
        client.reportSynchronization({R::ContextRejected,NIMBY_RESOURCE_LIMIT,1},&game,start+std::chrono::seconds(5));
        if(diagnosticMessages.size()!=3||!hasDiagnostic("context_rejected",NIMBY_RESOURCE_LIMIT))return false;
        client.reportSynchronization({R::ContextRejected,NIMBY_HOOKS_UNAVAILABLE,1},&game,start+std::chrono::seconds(5));
        if(diagnosticMessages.size()!=4)return false;
        game.generation=18;
        client.reportSynchronization({R::ContextRejected,NIMBY_HOOKS_UNAVAILABLE,1},&game,start+std::chrono::seconds(5));
        if(diagnosticMessages.size()!=5)return false;
        game.worldId=std::string(64,'b');
        client.reportSynchronization({R::ContextRejected,NIMBY_HOOKS_UNAVAILABLE,1},&game,start+std::chrono::seconds(5));
        if(diagnosticMessages.size()!=6)return false;
        client.reportSynchronization({},&game,start+std::chrono::seconds(6));
        if(diagnosticMessages.size()!=7||!hasDiagnostic("ready",NIMBY_OK)||
            diagnosticMessages.back().find("mods INFO Signal settings synchronization recovered")==std::string::npos)return false;
        for(unsigned tick=0;tick<50;++tick)client.reportSynchronization({},&game,start+std::chrono::seconds(7+tick));
        if(diagnosticMessages.size()!=7)return false;
        client.suspend();
        if(diagnosticMessages.size()!=8||!hasDiagnostic("external_suspension",NIMBY_OK))return false;
        client.suspend();
        return diagnosticMessages.size()==8&&sinkUnlocked;
    }
    inline static NimbyUiBeginV1 originalBegin{};
    inline static NimbyUiObserveV1 originalObserve{};
    inline static NimbyUiPanelContextV1 originalContext{};
    inline static uint32_t beginStatus=NIMBY_OK,observeStatus=NIMBY_OK,contextStatus=NIMBY_OK;
    inline static bool emptySession=false;
    static uint32_t beginProbe(uint64_t owner,const char* identity,uint32_t length,uint64_t* session){
        if(beginStatus!=NIMBY_OK)return beginStatus;
        if(emptySession){*session=0;return NIMBY_OK;}
        return originalBegin(owner,identity,length,session);
    }
    static uint32_t observeProbe(uint64_t owner,uint64_t session,const NimbyUiSignalV1* signals,uint32_t count){
        return observeStatus!=NIMBY_OK?observeStatus:originalObserve(owner,session,signals,count);
    }
    static uint32_t contextProbe(uint64_t owner,uint64_t session,uint64_t generation){
        return contextStatus!=NIMBY_OK?contextStatus:originalContext(owner,session,generation);
    }
    static bool synchronizationDiagnostics(const GameSession& game,std::span<const SignalSettingsStore::Signal> signals){
        constexpr SignalCheckbox fields[]{{"active","Active","",true}};
        SignalSettingsClient client;
        if(!client.connectExisting({"diagnostic-client","Diagnostic client","atlas",fields}))return false;
        CaptureDiagnostics capture(client);
        originalBegin=client.begin_;originalObserve=client.observe_;originalContext=client.context_;
        if(!originalContext)return false;
        const auto previousBatch=client.readBatch_;
        struct Restore{SignalSettingsClient& client;NimbyUiReadBatchV1 batch;
            ~Restore(){client.begin_=originalBegin;client.observe_=originalObserve;client.context_=originalContext;client.readBatch_=batch;}} restore{client,previousBatch};
        client.begin_=beginProbe;client.observe_=observeProbe;client.context_=contextProbe;
        beginStatus=observeStatus=contextStatus=NIMBY_OK;emptySession=false;
        auto invalid=game;invalid.generation=0;
        if(client.synchronize(invalid,signals)||!hasDiagnostic("generation_absent",NIMBY_OK))return false;
        beginStatus=NIMBY_HOOKS_UNAVAILABLE;
        if(client.synchronize(game,signals)||!hasDiagnostic("begin_session_rejected",beginStatus))return false;
        beginStatus=NIMBY_OK;emptySession=true;
        if(client.synchronize(game,signals)||!hasDiagnostic("begin_session_empty",NIMBY_OK))return false;
        emptySession=false;observeStatus=NIMBY_RESOURCE_LIMIT;
        if(client.synchronize(game,signals)||!hasDiagnostic("observe_rejected",observeStatus))return false;
        observeStatus=NIMBY_OK;contextStatus=NIMBY_HOOKS_UNAVAILABLE;
        if(client.synchronize(game,signals)||!hasDiagnostic("context_rejected",contextStatus))return false;
        contextStatus=NIMBY_OK;client.readBatch_=changingBatch;batchMode=6;
        if(client.synchronize(game,signals)||!hasDiagnostic("read_batch_rejected",NIMBY_RESOURCE_LIMIT)||client.cached_)return false;
        client.readBatch_=previousBatch;
        if(!client.synchronize(game,signals)||!hasDiagnostic("ready",NIMBY_OK)||!client.cached_)return false;
        const auto recovered=diagnosticMessages.size();
        if(!client.synchronize(game,signals)||diagnosticMessages.size()!=recovered)return false;
        return sinkUnlocked;
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
    CHECK(nimby::detail::SignalSettingsClientTest::batchDiagnostics(client,signals));
    CHECK(nimby::detail::SignalSettingsClientTest::diagnosticTransitions(client));
    CHECK(client.observe(session,signals)); // Restore the deliberately suspended fixture.
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
    CHECK(nimby::detail::SignalSettingsClientTest::synchronizationDiagnostics(world,signals));
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
    std::puts("PASS settings client lifetime, session invalidation, DLL reference and throttled synchronization diagnostics");
}
