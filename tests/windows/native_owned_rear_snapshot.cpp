#include <platform/windows/runtime/native_owned_rear_snapshot.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

// Synthetic, sealed owned-memory fixture. No native function, game process,
// authority or production ownership/continuity factory is exercised here.
namespace fixture {bool watching=false;size_t allocations=0;}
#if defined(__GNUC__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE __declspec(noinline)
#endif
NOINLINE void* operator new(size_t size){
    if(fixture::watching)++fixture::allocations;
    if(auto* value=std::malloc(size?size:1))return value;
    throw std::bad_alloc();
}
NOINLINE void* operator new[](size_t size){return ::operator new(size);}
NOINLINE void operator delete(void* value)noexcept{std::free(value);}
NOINLINE void operator delete[](void* value)noexcept{std::free(value);}
NOINLINE void operator delete(void* value,size_t)noexcept{std::free(value);}
NOINLINE void operator delete[](void* value,size_t)noexcept{std::free(value);}
#undef NOINLINE
namespace fixture {
namespace provider=nimby::windows::consist_rear;
namespace rear=nimby::engine::native_rear;
namespace inventory=nimby::windows::consist_inventory;
using provider::CarSetup;using provider::Reason;using provider::Result;
constexpr uintptr_t db=0x10000000,sim=0x11000000,motionBlock=0x20000000,
    trainBlock=0x21000000,trackBlock=0x22000000,motionPointers=0x30000000,
    trainPointers=0x30000100,trackPointers=0x30000200,planned=0x40000000,
    retained=0x41000000,history=0x42000000,children=0x43000000,
    rules=db+0xa80,table=0x50000100,models=0x51000000;
constexpr uint64_t train(size_t ordinal,uint16_t generation=1){return (uint64_t(5)<<48)|(ordinal<<16)|generation;}
constexpr uint64_t track(size_t ordinal,uint16_t generation=1){return (uint64_t(1)<<48)|(ordinal<<16)|generation;}
constexpr size_t carCapacity=128;
size_t checks=0;
void require(bool yes,const char* expression,int line){
    ++checks;if(!yes){std::fprintf(stderr,"line %d: %s\n",line,expression);std::abort();}
}
#define CHECK(...) require(bool(__VA_ARGS__),#__VA_ARGS__,__LINE__)
template<class T>void put(auto& bytes,size_t at,const T& value){std::memcpy(bytes.data()+at,&value,sizeof value);}
struct Region {uintptr_t address{};std::vector<unsigned char> bytes;};
struct Memory {
    std::array<Region,15> regions{{{db,std::vector<unsigned char>(0xb00)},
        {sim,std::vector<unsigned char>(0x200)},
        {motionBlock,std::vector<unsigned char>(0x100000)},
        {trainBlock,std::vector<unsigned char>(0x100000)},
        {trackBlock,std::vector<unsigned char>(0x100000)},
        {motionPointers,std::vector<unsigned char>(0x300)},
        {planned,std::vector<unsigned char>(8*carCapacity*32)},
        {retained,std::vector<unsigned char>(8*carCapacity*32)},
        {history,std::vector<unsigned char>(128*16)},
        {children,std::vector<unsigned char>(128*8)},
        {rules,std::vector<unsigned char>(0x80)},
        {table,std::vector<unsigned char>(3*8)},
        {models,std::vector<unsigned char>(16*0x300)},
        {0x60000000,std::vector<unsigned char>(0x100)},
        {0x61000000,std::vector<unsigned char>(0x100)}}};
    size_t reads=0,bytesRead=0,denyRead=0,throwRead=0,scopeCalls=0,denyScope=0;
    uintptr_t mutationRead=0,mutationAddress=0;
    size_t mutationAtOccurrence=2,mutationSeen=0;
    std::array<unsigned char,32> mutationValue{};size_t mutationBytes=0;
    bool gateRefused=false,readAfterRefusal=false,readRoster=false;
    provider::Witness witness{db,sim,{1,2,3,rear::Phase::RetainedMovement},train(0),4,5};
    template<class T>void write(uintptr_t at,const T& value){
        for(auto& region:regions)if(at>=region.address&&at-region.address<=region.bytes.size()&&sizeof value<=region.bytes.size()-(at-region.address)){
            std::memcpy(region.bytes.data()+at-region.address,&value,sizeof value);return;
        }
        CHECK(false);
    }
    void raw(uintptr_t at,const void* value,size_t bytes){
        for(auto& region:regions)if(at>=region.address&&at-region.address<=region.bytes.size()&&bytes<=region.bytes.size()-(at-region.address)){
            std::memcpy(region.bytes.data()+at-region.address,value,bytes);return;
        }
        CHECK(false);
    }
    template<class T>T value(uintptr_t at){T out{};for(const auto& region:regions){
        if(at>=region.address&&at-region.address<=region.bytes.size()&&sizeof out<=region.bytes.size()-(at-region.address)){
            std::memcpy(&out,region.bytes.data()+at-region.address,sizeof out);return out;
        }
        }
        CHECK(false);return out;
    }
    void vector(uintptr_t at,uintptr_t address,size_t count,size_t stride){
        write(at,provider::detail::Vector{address,address+count*stride,address+count*stride});
    }
    void pool(uintptr_t at,uintptr_t pointers,uintptr_t block,size_t stride){
        write(at,uint32_t(0x100000));write(at+4,uint32_t(9));write(at+8,uint32_t(512));
        write(at+12,uint32_t(512*stride));write(at+16,uint32_t(511));
        vector(at+24,pointers,1,8);write(pointers,block);
    }
    void car(size_t member,size_t ordinal,uint64_t model=100,uint8_t mask=255){
        CarSetup setup{model,0,{}};setup.settings[15]=mask;setup.settings[2]=17;
        write(planned+(member*carCapacity+ordinal)*32,setup);
        setup.owner=train(member);const auto index=uint16_t(ordinal);
        std::memcpy(setup.settings.data(),&index,2);
        write(retained+(member*carCapacity+ordinal)*32,setup);
    }
    void member(size_t ordinal,size_t count,uint64_t parent=0,int64_t order=1,bool reverse=false){
        const auto motion=motionBlock+ordinal*0x638,dbTrain=trainBlock+ordinal*0x178;
        write(motion,train(ordinal));write(dbTrain,train(ordinal));write(motion+0x80,int32_t(0));
        write(motion+0x1d0,uint8_t(parent?0:1));write(motion+0x1d8,parent);write(motion+0x1e0,order);
        write(motion+0x1e8,uint8_t(reverse));write(motion+0x1f0,uint8_t(parent!=0));
        vector(dbTrain+0xc0,planned+ordinal*carCapacity*32,count,32);
        vector(motion+8,retained+ordinal*carCapacity*32,count,32);vector(motion+0xe0,history,0,16);
        for(size_t i=0;i<count;++i)car(ordinal,i);
        write(motion+0x40,float(count*20));
    }
    void model(size_t ordinal,uint64_t id,float meters){
        const auto at=models+ordinal*0x300;write(at,id);write(at+0xfc,meters);write(at+0x2a0,uintptr_t(0));
        write(table+(id%2)*8,at);
    }
    void trackData(size_t ordinal,uint64_t a=0,uint64_t b=0,double metric=100,int8_t orientation=0){
        const auto at=trackBlock+ordinal*0x4e8;
        write(at,track(ordinal));write(at+8,a);write(at+16,b);write(at+0x88,metric);write(at+0x2c,orientation);
    }
    void head(double fraction=.75,int8_t direction=1,int8_t orientation=0){
        nimby::windows::automatic::CommitPresence presence{0,0,track(0),fraction,direction,orientation,{},fraction*100};
        write(motionBlock+0x3a0,presence);write(motionBlock+0xb8,track(0));write(motionBlock+0xc0,fraction);
        write(motionBlock+0xc8,direction);write(motionBlock+0xc9,orientation);
    }
    void reset(){
        for(auto& region:regions)std::fill(region.bytes.begin(),region.bytes.end(),0);
        reads=bytesRead=denyRead=throwRead=scopeCalls=denyScope=mutationRead=mutationAddress=mutationSeen=mutationBytes=0;
        mutationAtOccurrence=2;gateRefused=readAfterRefusal=readRoster=false;
        witness={db,sim,{1,2,3,rear::Phase::RetainedMovement},train(0),4,5};
        pool(sim+0xa0,motionPointers,motionBlock,0x638);pool(db+0x200,trainPointers,trainBlock,0x178);
        pool(db,trackPointers,trackBlock,0x4e8);write(db+0x408,rules);write(rules+0x38,table);write(rules+0x40,uint32_t(2));
        write(table+16,uintptr_t(0x60000000));model(0,100,20);model(1,101,10);
        member(0,1);trackData(0);head();
    }
    Memory(){reset();}
    template<class T>void mutate(uintptr_t readAt,uintptr_t changedAt,const T& value,size_t occurrence=2){
        CHECK(sizeof value<=mutationValue.size());mutationRead=readAt;mutationAddress=changedAt;
        mutationBytes=sizeof value;mutationSeen=0;std::memcpy(mutationValue.data(),&value,sizeof value);mutationAtOccurrence=occurrence;
    }
    bool read(uintptr_t at,void* out,size_t bytes){
        if(gateRefused)readAfterRefusal=true;
        ++reads;bytesRead+=bytes;
        if(reads==throwRead)throw 7;
        if(reads==denyRead)return false;
        if(at==mutationRead&&++mutationSeen==mutationAtOccurrence)raw(mutationAddress,mutationValue.data(),mutationBytes);
        if(at>=motionBlock&&at<motionBlock+0x100000&&(at-motionBlock)%0x638==0x88)readRoster=true;
        for(const auto& region:regions)if(at>=region.address&&at-region.address<=region.bytes.size()&&bytes<=region.bytes.size()-(at-region.address)){
            std::memcpy(out,region.bytes.data()+at-region.address,bytes);return true;
        }
        return false;
    }
    static bool seal(void* context,const provider::Witness& requested)noexcept{
        auto& memory=*static_cast<Memory*>(context);++memory.scopeCalls;
        return memory.scopeCalls!=memory.denyScope&&requested==memory.witness;
    }
    provider::Scope scope(){return {witness,seal,this};}
    void aggregate(size_t rootCars,size_t childCars,bool reverse=false,int64_t order=1){
        member(0,rootCars);member(1,childCars,train(0),order,reverse);
        std::array<CarSetup,carCapacity> full{};size_t at=0;
        const auto append=[&](size_t member,size_t count,bool reversed){
            for(size_t i=0;i<count;++i){auto setup=value<CarSetup>(retained+(member*carCapacity+(reversed?count-1-i:i))*32);
                if(reversed)setup.settings[12]=uint8_t(!setup.settings[12]);
                full[at++]=setup;}
        };
        if(order<0)append(1,childCars,reverse);
        append(0,rootCars,false);
        if(order>=0)append(1,childCars,reverse);
        raw(retained,full.data(),at*32);vector(motionBlock+8,retained,at,32);write(motionBlock+0x40,float(at*20));
    }
};
struct Gate {
    Memory& memory;size_t calls=0,deny=0,afterRefusal=0;
    bool refused=false;
    static bool preflight(void* context,size_t)noexcept{
        auto& gate=*static_cast<Gate*>(context);if(gate.refused)++gate.afterRefusal;
        if(++gate.calls==gate.deny){gate.refused=true;gate.memory.gateRefused=true;return false;}return true;
    }
};
namespace owned=nimby::windows::owned_rear;
namespace world=nimby::windows::native_world;
struct Owner {
    world::Clock clock;
    world::Lineage line;
    world::Capture token;
    uint64_t thread=7;
    bool allowed=true;
    size_t calls=0,deny=0;
    static bool current(void* opaque,const world::Capture& requested)noexcept {
        auto& owner=*static_cast<Owner*>(opaque);
        return ++owner.calls!=owner.deny&&owner.allowed&&owner.line.current(requested);
    }
    static uint64_t currentThread(void* opaque)noexcept{return static_cast<Owner*>(opaque)->thread;}
    void begin(){
        constexpr uintptr_t controller=0x100000,context=controller+0x5d0;
        if(line.state()==world::State::Capturing)CHECK(line.endCapture(token));
        if(line.state()!=world::State::Running){
            const auto stop=line.beginStop(clock,controller,context);CHECK(stop);CHECK(line.finishStop(stop));
            const auto start=line.beginStart(clock,controller,context);CHECK(start);CHECK(line.finishStart(start));
            CHECK(line.runnerEnter(controller,context,7));
        }
        world::Scope scope{controller,context,controller+0x4c0,db,sim,7};
        scope.boundary=world::Boundary::PostSharedExclusive;scope.releaseReturn=0x349fa3;
        scope.originalReleaseCompleted=true;scope.exclusiveTrySucceeded=true;
        const auto captured=line.capturePhase(clock,scope);CHECK(captured.reason==world::Reason::Ready);
        token=captured.capture;thread=7;allowed=true;calls=deny=0;
    }
    owned::Ownership scope(){return {token,current,currentThread,this};}
};
struct RosterStorage {
    std::array<inventory::Row,16> rows{},scratch{};
    std::array<uint64_t,512> ids{};
    owned::Roster capture(Memory& memory,const world::Capture& token){
        Gate gate{memory};inventory::Budget budget{Gate::preflight,&gate};
        const auto before=allocations;watching=true;
        auto result=inventory::captureMotionInventory(sim,true,
            [&](uintptr_t at,void* out,size_t bytes){return memory.read(at,out,bytes);},
            budget,rows,{scratch,ids});
        watching=false;CHECK(allocations==before);CHECK(result.observedAllSlots());
        return {token,result,{rows.data(),result.rows}};
    }
};
struct Seen {
    size_t visits=0,cars=0,sections=0,portions=0;
    provider::PublicationProof publication;
    provider::PhysicalCar firstCar,lastCar;
    rear::Portion firstPortion;
    owned::Storage* reentrant{};
    const owned::Ownership* ownership{};
    const owned::Roster* roster{};
    Memory* memory{};
    owned::Reason nested=owned::Reason::Ready;
    size_t nestedReads=999;
    static void visit(void* opaque,const owned::Body& body)noexcept{
        auto& seen=*static_cast<Seen*>(opaque);++seen.visits;
        seen.cars=body.cars.size();seen.sections=body.sections.size();seen.portions=body.portions.size();
        seen.publication=body.publication;seen.firstCar=body.cars.front();seen.lastCar=body.cars.back();
        seen.firstPortion=body.portions.front();
        if(seen.reentrant){
            Gate gate{*seen.memory};inventory::Budget budget{Gate::preflight,&gate};
            const auto nested=seen.reentrant->capture(*seen.ownership,*seen.roster,train(0),true,
                [&](uintptr_t at,void* out,size_t bytes){return seen.memory->read(at,out,bytes);},
                budget,visit,&seen);
            seen.nested=nested.reason;seen.nestedReads=nested.reads;
        }
    }
};
owned::Result capture(Memory& memory,owned::Storage& storage,Owner& owner,const owned::Roster& roster,
    Seen& seen,Gate& gate,provider::Limits limits={},bool profile=true,owned::Visitor visit=Seen::visit){
    inventory::Budget budget{Gate::preflight,&gate};const auto before=allocations;
    const auto startReads=memory.reads,startBytes=memory.bytesRead;watching=true;
    const auto result=storage.capture(owner.scope(),roster,train(0),profile,
        [&](uintptr_t at,void* out,size_t bytes){return memory.read(at,out,bytes);},budget,visit,&seen,limits);
    watching=false;CHECK(allocations==before);CHECK(!memory.readRoster);CHECK(!memory.readAfterRefusal);
    CHECK(!gate.afterRefusal);CHECK(result.reads==memory.reads-startReads);CHECK(result.bytes==memory.bytesRead-startBytes);
    CHECK(result.reads<=limits.reads&&result.bytes<=limits.bytes);
    if(!result.ready()){CHECK(!seen.visits);CHECK(!result.publication.currentPhysicalQualified);
        CHECK(!result.cars&&!result.sections&&!result.portions);}
    return result;
}
owned::Result run(Memory& memory,owned::Storage& storage,Owner& owner,const owned::Roster& roster,Seen& seen,provider::Limits limits={}){
    Gate gate{memory};return capture(memory,storage,owner,roster,seen,gate,limits);
}
struct SmallStorage {
    std::array<inventory::Row,16> inventoryRows{},inventoryScratch{};
    std::array<uint64_t,512> ids{};
    std::array<provider::detail::Member,8> members{};
    std::array<CarSetup,carCapacity> expected{},actual{},temporary{};
    std::array<provider::PhysicalCar,carCapacity> cars{},carsOut{};
    std::array<provider::NativeHistory,128> history{},historyAgain{};
    std::array<provider::detail::Model,16> models{};
    std::array<provider::detail::Track,128> tracks{};
    std::array<uint64_t,128> children{},childrenAgain{};
    std::array<rear::HistorySection,128> sections{},sectionsOut{};
    std::array<rear::Portion,128> portions{},portionsOut{};
    std::array<unsigned char,sizeof carsOut+sizeof sectionsOut+sizeof portionsOut> before{};
    provider::Workspace scratch(){return {inventoryRows,inventoryScratch,ids,members,expected,actual,temporary,cars,
        history,historyAgain,models,tracks,children,childrenAgain,sections,portions};}
    provider::Output output(){return {carsOut,sectionsOut,portionsOut};}
    void sentinel(){
        before.fill(0xa5);std::memcpy(carsOut.data(),before.data(),sizeof carsOut);
        std::memcpy(sectionsOut.data(),before.data()+sizeof carsOut,sizeof sectionsOut);
        std::memcpy(portionsOut.data(),before.data()+sizeof carsOut+sizeof sectionsOut,sizeof portionsOut);
    }
    bool untouched()const{return std::memcmp(carsOut.data(),before.data(),sizeof carsOut)==0&&
        std::memcmp(sectionsOut.data(),before.data()+sizeof carsOut,sizeof sectionsOut)==0&&
        std::memcmp(portionsOut.data(),before.data()+sizeof carsOut+sizeof sectionsOut,sizeof portionsOut)==0;}
};
void allTests(){
    static Memory memory;static owned::Storage storage;static RosterStorage rosters;Owner owner;
    const auto setup=[&]{memory.reset();owner.begin();auto roster=rosters.capture(memory,owner.token);
        memory.reads=memory.bytesRead=0;return roster;};
    auto roster=setup();Seen seen;auto result=run(memory,storage,owner,roster,seen);
    CHECK(result.ready());CHECK(seen.visits==1&&seen.cars==1&&seen.sections==1&&seen.portions==1);
    CHECK(result.publication.currentPhysicalQualified);CHECK(!result.publication.progressionQualified);
    CHECK(result.publication.kind==provider::Kind::CurrentPhysicalSnapshot);
    CHECK(!owned::Result::progressionQualified&&!owned::Result::leaseReleaseQualified);
    CHECK(result.publication.at.session==owner.token.world);
    CHECK(result.publication.at.epoch==owner.token.phase&&result.publication.at.tick==owner.token.phase);
    CHECK(result.publication.at.phase==rear::Phase::RetainedMovement);
    CHECK(seen.firstCar.setup.model==100&&seen.firstCar.setup.owner==train(0)&&seen.firstCar.meters==20);
    CHECK(seen.firstPortion.track==track(0)&&seen.firstPortion.from==.75&&seen.firstPortion.to<.55);
    const auto copied=seen.firstCar;memory.write(retained+8,train(0,2));CHECK(seen.firstCar.setup==copied.setup);
    // Reuse the same bootstrap storage without rescanning the externally owned roster.
    roster=setup();seen={};auto ownership=owner.scope();
    seen.reentrant=&storage;seen.ownership=&ownership;seen.roster=&roster;seen.memory=&memory;
    CHECK(run(memory,storage,owner,roster,seen).ready());
    CHECK(seen.nested==owned::Reason::StorageBusy&&seen.nestedReads==0);CHECK(seen.visits==1);
    // Attached/reversed cars are copied in the native root aggregate order.
    memory.reset();memory.aggregate(1,2,true,-1);memory.trackData(0,0,0,1000);owner.begin();
    roster=rosters.capture(memory,owner.token);memory.reads=memory.bytesRead=0;seen={};
    result=run(memory,storage,owner,roster,seen);CHECK(result.ready());CHECK(seen.cars==3&&result.members==2);
    CHECK(seen.firstCar.setup.owner==train(1)&&seen.firstCar.setup.ordinal()==1&&seen.firstCar.setup.settings[12]==1);
    CHECK(seen.lastCar.setup.owner==train(0));
    // No current token, legacy Shared scope or wrong native thread can read.
    for(unsigned variation=0;variation<4;++variation){
        roster=setup();seen={};Gate gate{memory};
        const auto realToken=owner.token;
        if(variation==0)owner.allowed=false;
        if(variation==1)owner.thread=9;
        if(variation==2)owner.token.boundary=world::Boundary::PostTrainShared;
        if(variation==3)CHECK(owner.line.endCapture(owner.token));
        CHECK(capture(memory,storage,owner,roster,seen,gate).reason==owned::Reason::UnqualifiedOwnership);
        CHECK(!memory.reads);
        owner.token=realToken;
    }
    roster=setup();seen={};++roster.capture.phase;
    CHECK(run(memory,storage,owner,roster,seen).reason==owned::Reason::WrongPublication);CHECK(!memory.reads);
    roster=setup();seen={};roster.result.status=inventory::Status::Unknown;
    CHECK(run(memory,storage,owner,roster,seen).reason==owned::Reason::RosterUnknown);CHECK(!memory.reads);
    roster=setup();seen={};roster.result.rows=2;
    CHECK(run(memory,storage,owner,roster,seen).reason==owned::Reason::RosterUnknown);CHECK(!memory.reads);
    roster=setup();seen={};memory.write(db+0x408,uintptr_t(0x60000000));
    CHECK(run(memory,storage,owner,roster,seen).reason==owned::Reason::CatalogueNotOwned);CHECK(memory.reads==1);
    roster=setup();seen={};memory.write(table,uintptr_t(0));
    result=run(memory,storage,owner,roster,seen);CHECK(result.reason==owned::Reason::ProviderUnknown);
    CHECK(result.providerReason==provider::Reason::MissingModel);
    roster=setup();seen={};memory.write(motionBlock,train(0,2));
    result=run(memory,storage,owner,roster,seen);CHECK(result.providerReason==provider::Reason::IdentityChanged);
    roster=setup();seen={};memory.write(retained+8,train(0,2));
    result=run(memory,storage,owner,roster,seen);CHECK(result.providerReason==provider::Reason::CurrentCompositionLag);
    roster=setup();seen={};memory.write(models+0xfc,float(140));memory.write(motionBlock+0x40,float(140));
    result=run(memory,storage,owner,roster,seen);CHECK(result.providerReason==provider::Reason::RearCoverageUnknown);
    // Budget refusal is terminal, and callbacks never see staged/old bodies.
    for(size_t denied:{size_t(1),size_t(8),size_t(20)}){
        roster=setup();seen={};Gate gate{memory};gate.deny=denied;
        result=capture(memory,storage,owner,roster,seen,gate);
        CHECK(!result.ready());CHECK(gate.refused);CHECK(!seen.visits);
        if(denied==1)CHECK(!memory.reads&&result.reason==owned::Reason::BudgetRefused);
    }
    roster=setup();seen={};provider::Limits tiny;tiny.reads=2;
    result=run(memory,storage,owner,roster,seen,tiny);CHECK(result.providerReason==provider::Reason::ReadLimit);
    roster=setup();seen={};tiny={};tiny.bytes=9;
    result=run(memory,storage,owner,roster,seen,tiny);CHECK(result.providerReason==provider::Reason::ByteLimit);
    for(unsigned variation=0;variation<4;++variation){
        roster=setup();seen={};tiny={};
        if(variation==0)tiny.reads=0;
        if(variation==1)tiny.reads=1;
        if(variation==2)tiny.bytes=0;
        if(variation==3)tiny.bytes=8;
        CHECK(run(memory,storage,owner,roster,seen,tiny).reason==owned::Reason::InvalidLimits);CHECK(!memory.reads);
    }
    roster=setup();seen={};memory.denyRead=1;
    CHECK(run(memory,storage,owner,roster,seen).reason==owned::Reason::Unreadable);
    for(size_t denied:{size_t(1),size_t(3)}){
        roster=setup();seen={};memory.throwRead=denied;
        result=run(memory,storage,owner,roster,seen);CHECK(!result.ready());
        CHECK(result.reason==(denied==1?owned::Reason::ReaderException:owned::Reason::ProviderUnknown));
        if(denied>1)CHECK(result.providerReason==provider::Reason::ReaderException);
    }
    roster=setup();seen={};owner.deny=3;
    result=run(memory,storage,owner,roster,seen);CHECK(!result.ready());CHECK(!seen.visits);
    roster=setup();seen={};Gate gate{memory};
    CHECK(capture(memory,storage,owner,roster,seen,gate,{},false).reason==owned::Reason::UnqualifiedOwnership);
    CHECK(!memory.reads);
    roster=setup();seen={};Gate noVisitor{memory};
    CHECK(capture(memory,storage,owner,roster,seen,noVisitor,{},true,nullptr).reason==owned::Reason::InvalidVisitor);
    CHECK(!memory.reads);
    // The lower-level provider cannot expose generic progression/clearance
    // booleans through its current-only branch. Legacy scopes remain compatible.
    static SmallStorage small;provider::PublicationProof publication;
    for(auto kind:{provider::Kind::ProgressionQualified,provider::Kind::CurrentPhysicalSnapshot}){
        memory.reset();Gate directGate{memory};inventory::Budget budget{Gate::preflight,&directGate};
        auto scope=memory.scope();scope.kind=kind;auto output=small.output();output.publication=&publication;
        const auto before=allocations;watching=true;
        const auto direct=provider::capture(db,sim,train(0),true,scope,
            [&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);},
            budget,small.scratch(),output);
        watching=false;CHECK(allocations==before);CHECK(direct.ready());
        CHECK(publication.currentPhysicalQualified&&publication.kind==kind);
        CHECK(publication.progressionQualified==(kind==provider::Kind::ProgressionQualified));
        CHECK(direct.observation.stablePublicationQualified==(kind==provider::Kind::ProgressionQualified));
        CHECK(direct.observation.history.qualified==(kind==provider::Kind::ProgressionQualified));
        CHECK(direct.observation.history.continuityQualified==(kind==provider::Kind::ProgressionQualified));
        CHECK(direct.observation.history.sections.empty()==(kind==provider::Kind::CurrentPhysicalSnapshot));
        CHECK(small.carsOut[0].meters==20&&small.portionsOut[0].to<.55);
    }
    memory.reset();small.sentinel();Gate aliasGate{memory};inventory::Budget aliasBudget{Gate::preflight,&aliasGate};
    auto aliased=small.output();aliased.publication=reinterpret_cast<provider::PublicationProof*>(small.carsOut.data());
    const auto alias=provider::capture(db,sim,train(0),true,memory.scope(),
        [&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);},aliasBudget,small.scratch(),aliased);
    CHECK(alias.reason==provider::Reason::OverlappingStorage);CHECK(!memory.reads);CHECK(small.untouched());
    // Failed provider attempts leave explicit metadata and all outputs untouched.
    memory.reset();small.sentinel();memory.write(models+0xfc,float(-20));
    publication={{91,92,93,rear::Phase::BeforeMovement},94,95,provider::Kind::CurrentPhysicalSnapshot,true,false};
    const auto sentinel=publication;Gate missingGate{memory};inventory::Budget missingBudget{Gate::preflight,&missingGate};
    auto metadataOutput=small.output();metadataOutput.publication=&publication;
    const auto missing=provider::capture(db,sim,train(0),true,memory.scope(),
        [&](uintptr_t at,void* target,size_t bytes){return memory.read(at,target,bytes);},missingBudget,small.scratch(),metadataOutput);
    CHECK(missing.reason==provider::Reason::InvalidModelLength);CHECK(small.untouched());
    CHECK(publication.at==sentinel.at&&publication.train==sentinel.train&&publication.kind==sentinel.kind);
    CHECK(publication.currentPhysicalQualified==sentinel.currentPhysicalQualified);
    std::printf("native_owned_rear_snapshot: %zu checks PASS; storage=%zu bytes\n",checks,sizeof storage);
}
} // namespace fixture
int main(){fixture::allTests();return 0;}
