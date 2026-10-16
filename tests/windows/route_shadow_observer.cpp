#include <engine/route_shadow_batch.h>
#include <engine/track_conflicts.h>
#include <engine/versioning.h>
#include <nimby/detail/diagnostics.hpp>
#include <windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <map>
#include <limits>
#include <new>
#include <sstream>
#include <string>
#include <vector>

// Instrument C++ allocations without involving the game or an external PID.
namespace fixture {
std::atomic<size_t> allocations{};
bool watchBudgetAllocations=false,budgetRefused=false;
size_t allocationsAfterBudget=0;
}
#ifdef __GNUC__
#define FIXTURE_NOINLINE __attribute__((noinline))
#else
#define FIXTURE_NOINLINE __declspec(noinline)
#endif
FIXTURE_NOINLINE void* operator new(size_t size){
    fixture::allocations.fetch_add(1,std::memory_order_relaxed);
    if(fixture::watchBudgetAllocations&&fixture::budgetRefused)++fixture::allocationsAfterBudget;
    if(auto* result=std::malloc(size?size:1))return result;
    throw std::bad_alloc();
}
FIXTURE_NOINLINE void* operator new[](size_t size){return ::operator new(size);}
FIXTURE_NOINLINE void operator delete(void* value)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete(void* value,size_t)noexcept{std::free(value);}
FIXTURE_NOINLINE void operator delete[](void* value,size_t)noexcept{std::free(value);}
#undef FIXTURE_NOINLINE

namespace fixture {
constexpr uint64_t module=0x140000000,root=0x20000000,database=0x30000000;
constexpr uint64_t copy=0x40000000,simulation=0x50000000,table=0x60000000,block=0x60010000,relations=0x61000000;
constexpr uint64_t trackA=0x1000000000001,trackB=0x1000000010001;
constexpr uint64_t trainA=0x5000000000001,trainB=0x5000000010001;
constexpr uint64_t signalA=0x8000000000001,signalB=0x8000000010001,signalC=0x8000000020001;
std::map<uint64_t,std::vector<unsigned char>> memory;
std::vector<std::string> logs;
uint64_t refused=0,now=1000;
unsigned reads=0,moduleCalls=0,environmentCalls=0,clockCalls=0,counterCalls=0;
bool enabled=false,expireBudget=false;
template<class T>void put(uint64_t address,size_t offset,T value){
    auto& bytes=memory[address];if(bytes.size()<offset+sizeof value)bytes.resize(offset+sizeof value);
    std::memcpy(bytes.data()+offset,&value,sizeof value);
}
void prepareMemory(){
    memory.clear();
    put(module+0xb81998,0,root);put(root,0x540,database);put(root,0x5c0,copy);put(root,0x680,simulation);
    memory[database].resize(0xa48+0x38);
    put(database,4,uint32_t(2));put(database,8,uint32_t(4));put(database,16,uint32_t(3));
    put(database,24,table);put(database,32,table+8);put(database,40,table+8);put(table,0,block);
    put(database,0xa48,uint8_t(42)); // Valid versioning value, empty history.
    memory[block].resize(0x110);memory[block+0x4e8].resize(0x110);
    put(block,0,trackA);put(block+0x4e8,0,trackB);
    memory[relations].resize(0x30);
    put(block,0xf8,relations);put(block,0x100,relations+0x30);put(block,0x108,relations+0x30);
    put(relations,0,trackB);put(relations,0x18,.2f);put(relations,0x1c,.6f);
    put(relations,0x20,.8f);put(relations,0x24,.3f);
}
BOOL WINAPI readMemory(HANDLE,LPCVOID address,LPVOID output,SIZE_T size,SIZE_T* copied){
    ++reads;if(copied)*copied=0;const auto at=reinterpret_cast<uint64_t>(address);
    if(at==refused)return FALSE;
    auto found=memory.upper_bound(at);if(found==memory.begin())return FALSE;--found;
    const auto offset=at-found->first;
    if(offset>found->second.size()||size>found->second.size()-offset)return FALSE;
    std::memcpy(output,found->second.data()+offset,size);if(copied)*copied=size;return TRUE;
}
HMODULE WINAPI getModule(LPCWSTR){++moduleCalls;return reinterpret_cast<HMODULE>(module);}
DWORD WINAPI environment(LPCWSTR name,LPWSTR value,DWORD capacity){
    ++environmentCalls;
    if(std::wcscmp(name,L"NRF_INTERLOCKING_SHADOW")!=0||capacity<2)return 0;
    value[0]=enabled?L'1':L'0';value[1]=0;return 1;
}
ULONGLONG WINAPI ticks(){++clockCalls;return now;}
BOOL WINAPI frequency(LARGE_INTEGER* value){value->QuadPart=1000000;return TRUE;}
BOOL WINAPI counter(LARGE_INTEGER* value){
    ++counterCalls;value->QuadPart=expireBudget&&counterCalls%2==0?10000:1000;return TRUE;
}
bool contains(const std::string& part){
    return std::any_of(logs.begin(),logs.end(),[&](const auto& row){return row.find(part)!=std::string::npos;});
}
size_t count(const std::string& part){
    return std::count_if(logs.begin(),logs.end(),[&](const auto& row){return row.find(part)!=std::string::npos;});
}
}
namespace nimby::detail::diagnostics {
void captureShadowDiagnostic(const char* component,const char* level,const char* message)noexcept{
    try{fixture::logs.emplace_back(std::string(component)+" "+level+" "+message);}catch(...){}
}
}

// Replacement names are active ONLY while compiling the observer header. The
// production readers retain their real implementation, with this fake memory
// callback supplied through the same ReadMemory interface as the native adapter.
#define ReadProcessMemory fixture::readMemory
#define GetModuleHandleW fixture::getModule
#define GetEnvironmentVariableW fixture::environment
#define GetTickCount64 fixture::ticks
#define QueryPerformanceFrequency fixture::frequency
#define QueryPerformanceCounter fixture::counter
#define write captureShadowDiagnostic
#include <platform/windows/runtime/route_shadow_observer.h>
#undef write
#undef QueryPerformanceCounter
#undef QueryPerformanceFrequency
#undef GetTickCount64
#undef GetEnvironmentVariableW
#undef GetModuleHandleW
#undef ReadProcessMemory

using namespace nimby::windows::automatic;
namespace {
struct Batch {
    std::array<NimbySignalRouteObservation,2> routes{};
    std::array<NimbySignalRouteSection,2> sections{};
    Batch(){
        auto& first=routes[0];first.train_id=fixture::trainA;first.signal_id=fixture::signalA;
        first.next_signal_id=fixture::signalC;first.route_revision=1;first.observed_at_ms=1000;
        first.head_track_id=fixture::trackA;first.head_fraction=.1;first.distance_m=100;first.head_direction=1;first.blocks=1;
        first.flags=NIMBY_SIGNAL_ROUTE_ORDERED|NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR|NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE;
        first.source_track_id=fixture::trackA;first.source_fraction=.2;first.source_direction=1;first.source_kind=NIMBY_SIGNAL_PATH;
        first.next_track_id=fixture::trackA;first.next_fraction=.8;first.next_direction=1;first.next_kind=NIMBY_SIGNAL_PATH;
        routes[1]=first;auto& second=routes[1];second.train_id=fixture::trainB;second.signal_id=fixture::signalB;
        second.head_fraction=.9;second.head_direction=-1;second.source_fraction=.8;second.source_direction=-1;
        second.next_fraction=.2;second.next_direction=-1;
        second.flags=NIMBY_SIGNAL_ROUTE_ORDERED|NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED|NIMBY_SIGNAL_ROUTE_BLOCK_COMPLETE;
        sections={{{fixture::trainA,fixture::signalA,fixture::trackA,.2,.8},
                   {fixture::trainB,fixture::signalB,fixture::trackA,.2,.8}}};
    }
};
struct PhaseBudget {
    size_t checks=0,refuseAt=std::numeric_limits<size_t>::max();
    static bool available(void* context,size_t){
        auto& budget=*static_cast<PhaseBudget*>(context);
        if(++budget.checks>=budget.refuseAt){fixture::budgetRefused=true;return false;}
        return true;
    }
};
void beginPhaseWatch(){fixture::budgetRefused=false;fixture::allocationsAfterBudget=0;fixture::watchBudgetAllocations=true;}
void endPhaseWatch(){fixture::watchBudgetAllocations=false;fixture::budgetRefused=false;}
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(int argc,char** argv){
    CHECK(argc==2&&(std::strcmp(argv[1],"off")==0||std::strcmp(argv[1],"on")==0||
        std::strcmp(argv[1],"on-identity-gap")==0));
    fixture::enabled=std::strcmp(argv[1],"off")!=0;const bool enabled=fixture::enabled;
    Batch batch;const auto savedRoutes=batch.routes;const auto savedSections=batch.sections;
    fixture::prepareMemory();const auto savedMemory=fixture::memory;
    if(!enabled){
        const auto allocations=fixture::allocations.load();
        observeShadowRoutes(batch.routes,batch.sections);
        observeShadowRoutes({},{});
        CHECK(fixture::reads==0&&fixture::moduleCalls==0&&fixture::clockCalls==0&&fixture::counterCalls==0);
        CHECK(fixture::allocations.load()==allocations&&fixture::logs.empty());
        CHECK(fixture::environmentCalls==1);
        // The environment preference is process-local and deliberately cached.
        fixture::enabled=true;observeShadowRoutes(batch.routes,batch.sections);
        CHECK(!shadowObservationRequested()&&fixture::environmentCalls==1);
        CHECK(fixture::reads==0&&fixture::allocations.load()==allocations&&fixture::logs.empty());
    }else{
        // Owner-side preparation uses the same terminal preflight as reads.
        // Sweep expiry at every checkpoint, including AFTER a full plan was
        // computed: output stays empty and no further allocation is attempted.
        {
            using namespace nimby::engine;using namespace nimby::engine::interlocking;
            TrackConflicts captured;
            captured.states={{fixture::trackA,NIMBY_TRACK_CONFLICTS_VALID,NIMBY_TRACK_CONFLICTS_COMPLETE}};
            captured.rows={{fixture::trackA,fixture::trackB,.2,.6,.3,.8}};
            ShadowBatchPlan plan;PhaseBudget normal;TrackConflictReadBudget normalGate{PhaseBudget::available,&normal};
            CHECK(prepareShadowPlan(captured,batch.routes,batch.sections,normalGate,plan)&&plan.routes.size()==2);
            const auto checkpoints=normal.checks;CHECK(checkpoints>10);
            const auto beforeReads=fixture::reads;
            for(size_t stop=1;stop<=checkpoints;++stop){
                PhaseBudget limited;limited.refuseAt=stop;TrackConflictReadBudget gate{PhaseBudget::available,&limited};
                beginPhaseWatch();const bool prepared=prepareShadowPlan(captured,batch.routes,batch.sections,gate,plan);endPhaseWatch();
                CHECK(!prepared&&gate.exhausted&&plan.routes.empty()&&limited.checks==stop);
                CHECK(fixture::allocationsAfterBudget==0&&fixture::reads==beforeReads);
            }
            // A large all-nonoverlapping F8 table reaches the periodic inner
            // relation checks, then stops with no partial/free resource list.
            std::array<NimbyTrackConflict,2048> relations;
            relations.fill({fixture::trackA,fixture::trackB,.6,.7,.3,.8});
            const RoutePortion portion{fixture::trackA,.1,.2};
            const ConflictList list{fixture::trackA,TrackConflictReason::Complete,relations};
            PhaseBudget full;TrackConflictReadBudget fullGate{PhaseBudget::available,&full};
            CHECK(buildRouteResources(std::span(&portion,1),std::span(&list,1),{},&fullGate).complete());
            PhaseBudget limited;limited.refuseAt=full.checks/2;TrackConflictReadBudget gate{PhaseBudget::available,&limited};
            beginPhaseWatch();auto interrupted=buildRouteResources(std::span(&portion,1),std::span(&list,1),{},&gate);endPhaseWatch();
            CHECK(gate.exhausted&&interrupted.outcome==ResourceOutcome::BudgetExceeded&&interrupted.resources.empty());
            CHECK(interrupted.checkedRelations>0&&interrupted.checkedRelations<relations.size()&&fixture::allocationsAfterBudget==0);
            // Cancellation inside the budgeted sort is allocation-free too.
            std::array<RoutePortion,512> portions;
            for(size_t i=0;i<portions.size();++i)portions[i]={fixture::trackA+(portions.size()-i)*0x10000,.1,.2};
            std::array<ConflictList,512> emptyLists;
            for(size_t i=0;i<emptyLists.size();++i)emptyLists[i]={portions[i].track,TrackConflictReason::Complete,{}};
            PhaseBudget sortFull;TrackConflictReadBudget sortFullGate{PhaseBudget::available,&sortFull};
            const auto expected=buildRouteResources(portions,emptyLists);
            const auto budgeted=buildRouteResources(portions,emptyLists,{},&sortFullGate);
            CHECK(budgeted.complete()&&expected.complete()&&budgeted.resources==expected.resources&&budgeted.resources.size()==512);
            PhaseBudget sorting;sorting.refuseAt=22;TrackConflictReadBudget sortGate{PhaseBudget::available,&sorting};
            beginPhaseWatch();auto sorted=buildRouteResources(portions,{}, {},&sortGate);endPhaseWatch();
            CHECK(sortGate.exhausted&&sorted.outcome==ResourceOutcome::BudgetExceeded&&sorted.resources.empty()&&fixture::allocationsAfterBudget==0);

            // Seed an actually retained synthetic lease, then interrupt a
            // two-route submission after one event was queued. Neither an
            // implicit drain nor a reset may release the old protection.
            ShadowCoordinator coordinator;CHECK(coordinator.beginSession(1));
            const Ticket held{1,fixture::trainA,fixture::signalA,1};
            ShadowRequest seed{held,fixture::now,true,Proof::No,NativeStage::Clear,ResourceOutcome::Complete};
            const Interval occupied{fixture::trackA,.2,.8};
            CHECK(coordinator.trySubmit(seed,std::span(&occupied,1))==ShadowSubmit::Queued);
            coordinator.drain({1,fixture::now});CHECK(coordinator.holds(held));
            ShadowEvidence entered{held,{fixture::now,1,Proof::Yes,Proof::Unknown},NativeStage::Clear};
            CHECK(coordinator.trySubmit(entered)==ShadowSubmit::Queued);coordinator.drain({1,fixture::now});
            PhaseBudget ready;TrackConflictReadBudget readyGate{PhaseBudget::available,&ready};
            CHECK(prepareShadowPlan(captured,batch.routes,batch.sections,readyGate,plan));
            PhaseBudget submission;submission.refuseAt=3;TrackConflictReadBudget submitGate{PhaseBudget::available,&submission};
            std::optional<ShadowReport> report;
            const auto beforeClock=fixture::clockCalls;
            beginPhaseWatch();const bool submitted=submitShadowPlan(coordinator,plan,submitGate,report);endPhaseWatch();
            CHECK(!submitted&&submitGate.exhausted&&!report&&coordinator.queueStats().queued==1);
            CHECK(coordinator.holds(held)&&coordinator.leaseCount()==1&&coordinator.resourceCount()==1);
            CHECK(fixture::allocationsAfterBudget==0&&fixture::clockCalls==beforeClock);
            coordinator.drain({1,fixture::now}); // Qualified future owner activity may process the retained queue.
            CHECK(coordinator.holds(held)&&coordinator.leaseCount()==1&&coordinator.resourceCount()==1);
        }
        // No identity bootstrap is created when the initial metadata fails.
        fixture::refused=fixture::module+0xb81998;
        observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::contains("skipped=1 lineage_lost=0"));
        CHECK(fixture::count("Interlocking shadow: scope=")==0);fixture::refused=0;

        // Exhausted time budgets refuse reads before ReadProcessMemory and
        // cannot bootstrap a world from partial metadata either.
        fixture::now=2000;fixture::counterCalls=0;fixture::expireBudget=true;
        const auto beforeExpired=fixture::reads;observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::reads==beforeExpired&&fixture::contains("skipped=2 lineage_lost=0"));
        CHECK(fixture::count("Interlocking shadow: scope=")==0);fixture::expireBudget=false;

        fixture::now=3000;const auto beforeQualified=fixture::reads;
        observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::reads>beforeQualified&&fixture::reads-beforeQualified<150&&fixture::moduleCalls==3);
        CHECK(fixture::contains("Interlocking shadow: scope=requested_mod_routes"));
        CHECK(fixture::contains("non_atomic=1 native_permissions_changed=0 entry_proof_producer=0 rear_clear_producer=0"));
        CHECK(fixture::contains("processed=2 leases=0 resources=0"));
        CHECK(fixture::contains("native_stage=1 comparison=0 resource_reason=0 entered=0 rear_clear=0"));
        CHECK(fixture::contains("native_stage=2 comparison=0 resource_reason=0 entered=0 rear_clear=0"));
        CHECK(!fixture::contains("outcome=0 ")); // No entered proof, so not even a shadow grant.
        auto reports=fixture::count("Interlocking shadow: scope=");
        CHECK(reports==1&&fixture::environmentCalls==1);
        fixture::enabled=false;CHECK(shadowObservationRequested()&&fixture::environmentCalls==1);

        // An unavailable F8 sample cannot reset retained state or grant SDK
        // movement permission; intact metadata permits later observation.
        fixture::now=4000;fixture::refused=fixture::table;
        observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::contains("Interlocking shadow unavailable:")&&fixture::contains("skipped=3 lineage_lost=0"));
        CHECK(fixture::contains("native_permissions_changed=0 retained_leases_not_released=1"));
        CHECK(fixture::count("Interlocking shadow: scope=")==reports);fixture::refused=0;
        fixture::now=5000;
        observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::count("Interlocking shadow: scope=")==reports+1);++reports;

        // Oversized batches fail before any native sample.
        fixture::now=6000;std::array<NimbySignalRouteObservation,65> tooMany{};
        const auto beforeLimit=fixture::reads;observeShadowRoutes(tooMany,batch.sections);
        CHECK(fixture::reads==beforeLimit&&fixture::contains("skipped=4 lineage_lost=0"));

        // Each enabled scenario runs in its own process. Both changed identity
        // and missing identity AFTER bootstrap latch unavailable, never reset.
        fixture::now=7000;
        if(std::strcmp(argv[1],"on-identity-gap")==0)fixture::refused=fixture::module+0xb81998;
        else fixture::put(fixture::database,0xa48,uint8_t(43));
        observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::contains("skipped=5 lineage_lost=1"));
        fixture::now=8000;fixture::refused=0;fixture::put(fixture::database,0xa48,uint8_t(42));
        const auto beforeLatched=fixture::reads;
        observeShadowRoutes(batch.routes,batch.sections);
        CHECK(fixture::reads==beforeLatched&&fixture::contains("skipped=6 lineage_lost=1"));
        CHECK(fixture::count("Interlocking shadow: scope=")==reports);

        // Direct limits prove refused reads do not leak into the native API.
        std::array<unsigned char,8> output{};const auto beforeDirect=fixture::reads;
        {ShadowReadBudget budget;budget.reads=0;
         CHECK(!ShadowReadBudget::copy(&budget,fixture::root,output.data(),output.size())&&budget.exhausted);}
        {ShadowReadBudget budget;budget.reads=1;budget.bytes=7;
         CHECK(!ShadowReadBudget::copy(&budget,fixture::root,output.data(),output.size())&&budget.exhausted);
         const auto clocks=fixture::counterCalls;budget.bytes=1024;budget.reads=8192;
         CHECK(!ShadowReadBudget::copy(&budget,fixture::root,output.data(),1));
         CHECK(!ShadowReadBudget::available(&budget,0)&&fixture::counterCalls==clocks);}
        {ShadowReadBudget budget;budget.frequency.QuadPart=0;
         CHECK(!ShadowReadBudget::copy(&budget,fixture::root,output.data(),output.size())&&budget.exhausted);}
        {fixture::counterCalls=0;fixture::expireBudget=true;ShadowReadBudget budget;
         CHECK(!ShadowReadBudget::available(&budget,0)&&budget.exhausted);fixture::expireBudget=false;
         const auto clocks=fixture::counterCalls;
         CHECK(!ShadowReadBudget::copy(&budget,fixture::root,output.data(),1)&&fixture::counterCalls==clocks);}
        CHECK(fixture::reads==beforeDirect);
        // A business memory failure consumes its read quota but is not a
        // terminal budget refusal; unrelated readable metadata still works.
        {ShadowReadBudget budget;fixture::refused=fixture::root;
         CHECK(!ShadowReadBudget::copy(&budget,fixture::root,output.data(),1)&&!budget.exhausted);
         fixture::refused=0;
         CHECK(ShadowReadBudget::copy(&budget,fixture::module+0xb81998,output.data(),output.size())&&!budget.exhausted);}
    }
    CHECK(std::memcmp(batch.routes.data(),savedRoutes.data(),sizeof batch.routes)==0);
    CHECK(std::memcmp(batch.sections.data(),savedSections.data(),sizeof batch.sections)==0);
    CHECK(fixture::memory==savedMemory);
    std::printf("PASS Windows route shadow observer %s: synthetic inputs unchanged, cached opt-in, bounded reads, no native authority or reset.\n",argv[1]);
}
