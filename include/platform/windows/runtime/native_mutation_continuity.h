#pragma once
#include <platform/windows/runtime/native_owned_rear_snapshot.h>
#include <bit>
#include <atomic>

namespace nimby::windows::mutation_continuity {
namespace owned=nimby::windows::owned_rear;
namespace inventory=nimby::windows::consist_inventory;
namespace world=nimby::windows::native_world;

// PRIVATE producer of OBSERVED mutation history, never a safety certificate.
// The opt-in bridge observes 30E020 batches and 440840 replans. SimCmd,
// relocation, reversal, normalization and foreign row/import writers require
// BEFORE-write producers. Neither an unchanged tuple nor normal completion of
// a batch proves those unobserved writers absent. No result can release a lease.
struct WorldKey {
    uint64_t world{},continuity{};
    uintptr_t controller{},database{},simulation{};
    uint64_t thread{};
    bool operator==(const WorldKey&)const=default;
};
inline WorldKey key(const world::Capture& value)noexcept {
    return {value.world,value.continuity,value.controller,value.database,value.simulation,value.thread};
}
struct MemberTuple {
    uint64_t train{},parent{};int64_t order{};int32_t mode{};
    uint8_t active{},attached{},reversed{};
    bool operator==(const MemberTuple&)const=default;
};
struct CarTuple {
    owned::provider::CarSetup setup;
    uint32_t nativeLengthBits{};
    bool operator==(const CarTuple&)const=default;
};
struct Stamp {
    WorldKey owner;
    uint64_t train{},consistRevision{},globalMutation{},localMutation{};
    bool operator==(const Stamp&)const=default;
    explicit operator bool()const noexcept{return train&&consistRevision&&globalMutation;}
};
enum class Reason:uint8_t {
    Ready,Unarmed,Busy,OwnershipUnknown,WorldChanged,OldPublication,Lost,
    InvalidRoster,MissingRoot,InvalidConsist,Capacity,Budget,SequenceExhausted,
    MutationInProgress,MutationObserved,TupleChanged,IdentityChanged,
    ForeignWorld,UnqualifiedBatch,Completed,OriginalException,NestedMutation,Count
};
struct Result {
    Reason reason=Reason::Unarmed;
    Stamp stamp;
    bool observedMutationsUnbroken=false;
    static constexpr bool nativeWritersComplete=false;
    static constexpr bool progressionQualified=false;
    static constexpr bool leaseReleaseQualified=false;
};
struct Record {
    uint64_t train{},revision{},localMutation{},phase{};
    size_t memberBegin{},members{},carBegin{},cars{};
};
// Static/bootstrap storage. The journal never allocates, reads native memory,
// calls a game/mod callback, formats a log or waits. Flat storage compacts only
// on changed tuple sizes; unchanged tuples do not copy the world or its cars.
template<size_t Roots=256,size_t Members=1024,size_t Cars=8192> struct Storage {
    std::array<Record,Roots> records{};
    std::array<MemberTuple,Members> members{};
    std::array<CarTuple,Cars> cars{};
};
struct Budget {
    size_t limit=65536,used{};
    inventory::Budget* shared{};
    bool take(size_t count=1)noexcept {
        if(used>limit||count>limit-used||(shared&&!shared->available()))return false;
        used+=count;return true;
    }
};
struct BatchScope {
    WorldKey owner;
    uintptr_t returnRva{};
    // Facts from the pinned caller under its existing native Exclusive lock.
    // currentRootVerified includes Controller/DB/Sim/runner-thread readback.
    bool profileQualified=false,nativeExclusiveSite=false,currentRootVerified=false;
    bool (*currentWorld)(void*,const WorldKey&)noexcept{};
    void* context{};
    bool valid()const noexcept {
        return profileQualified&&nativeExclusiveSite&&currentRootVerified&&currentWorld&&
            (returnRva==0x348f07||returnRva==0x34928b||returnRva==0x3493d5)&&
            owner.world&&owner.continuity&&owner.controller&&owner.database&&owner.simulation&&owner.thread&&
            currentWorld(context,owner);
    }
};
struct Mutation {
    WorldKey owner;
    uint64_t sequence{};
    explicit operator bool()const noexcept{return sequence!=0;}
};
struct Counters {
    uint64_t begun{},completed{},exceptions{},unqualified{},foreign{},prebootstrap{},
        nested{},busy{},worldChanges{},tupleChanges{},compositionObservations{},capacity{},budget{},lost{};
    uint64_t lastMutation{},lastCompleted{},world{},globalMutation{};
    size_t roots{},members{},cars{};
};

template<size_t Roots=256,size_t Members=1024,size_t Cars=8192> class Journal {
    static_assert(Roots&&Members&&Cars);
    Storage<Roots,Members,Cars>& storage_;
    world::Clock clock_;
    std::atomic_flag gate_=ATOMIC_FLAG_INIT;
    std::atomic<bool> pendingLost_{};
    WorldKey world_;
    uint64_t phase_{},globalMutation_{},activeMutation_{};
    size_t roots_{},members_{},cars_{};
    bool lost_{};
    Counters counters_;
    struct Gate {
        Journal& owner;bool held;
        explicit Gate(Journal& value,bool invalidateOnBusy=true)noexcept:owner(value),held(!value.gate_.test_and_set(std::memory_order_acquire)) {
            if(!held&&invalidateOnBusy)value.pendingLost_.store(true,std::memory_order_release);
        }
        ~Gate(){if(held)owner.gate_.clear(std::memory_order_release);}
    };
    uint64_t sequence()noexcept {
        const auto value=clock_.take();if(!value){lost_=true;++counters_.lost;}return value;
    }
    void lose()noexcept {lost_=true;++counters_.lost;}
    void consumePending()noexcept {
        if(pendingLost_.exchange(false,std::memory_order_acq_rel)){lose();++counters_.busy;}
    }
    void invalidate()noexcept {
        const auto value=sequence();if(value)globalMutation_=value;
    }
    size_t find(uint64_t train)const noexcept {
        const auto ordinal=inventory::detail::index(train);
        for(size_t i=0;i<roots_;++i)if(inventory::detail::index(storage_.records[i].train)==ordinal)return i;
        return roots_;
    }
    Stamp stamp(const Record& row)const noexcept {
        return {world_,row.train,row.revision,globalMutation_,row.localMutation};
    }
    static MemberTuple member(const inventory::Row& row)noexcept {
        return {row.train,row.parent,row.couplingOrder,row.nativeMode,row.active,row.attached,row.reversed};
    }
    static CarTuple car(const owned::provider::PhysicalCar& value)noexcept {
        return {value.setup,std::bit_cast<uint32_t>(value.nativeMeters)};
    }
public:
    explicit Journal(Storage<Roots,Members,Cars>& storage,uint64_t firstSequence=1)noexcept:
        storage_(storage),clock_(firstSequence){}
    Journal(const Journal&)=delete;Journal& operator=(const Journal&)=delete;
    // Binding requires the real, current PostSharedExclusive ownership. A new
    // world can establish NEW records; it never recovers old-world history.
    Result bind(const owned::Ownership& ownership)noexcept {
        if(!ownership.valid()){invalidateWorld(true);return {Reason::OwnershipUnknown,{}};}
        Gate gate(*this);if(!gate.held)return {Reason::Busy,{}};consumePending();
        const auto value=key(ownership.capture);
        if(value.world<world_.world)return {Reason::WorldChanged,{}};
        if(value.world!=world_.world){
            if(!ownership.valid()){lose();return {Reason::OwnershipUnknown,{}};}
            world_=value;phase_=ownership.capture.phase;roots_=members_=cars_=0;
            activeMutation_=0;lost_=false;invalidate();++counters_.worldChanges;
        }else{
            if(lost_)return {Reason::Lost,{}};
            if(value.controller!=world_.controller||value.database!=world_.database||
               value.simulation!=world_.simulation||value.thread!=world_.thread){lose();return {Reason::WorldChanged,{}};}
            if(ownership.capture.phase<phase_)return {Reason::OldPublication,{}};
            if(activeMutation_)return {Reason::MutationInProgress,{}};
            if(value.continuity!=world_.continuity){invalidate();world_.continuity=value.continuity;}
            phase_=ownership.capture.phase;
        }
        if(!ownership.valid()){lose();return {Reason::OwnershipUnknown,{}};}
        return {lost_?Reason::SequenceExhausted:Reason::Ready,{}};
    }
    WorldKey owner()noexcept {
        Gate gate(*this,false);if(!gate.held)return {};consumePending();return lost_?WorldKey{}:world_;
    }
    // Feed this only from the owned body visitor. The complete indexed roster
    // seals root+children; the provider has validated every physical CarSetup.
    // Exact arrays, not hashes or train/car counts, define the consist revision.
    Result observe(const owned::Ownership& ownership,const owned::Roster& roster,
                   const owned::Body& body,Budget& budget)noexcept {
        if(!ownership.valid()){invalidateWorld(true);return {Reason::OwnershipUnknown,{}};}
        Gate gate(*this);if(!gate.held)return {Reason::Busy,{}};consumePending();
        if(lost_)return {Reason::Lost,{}};
        if(!world_.world)return {Reason::Unarmed,{}};
        if(key(ownership.capture)!=world_)return {Reason::WorldChanged,{}};
        if(ownership.capture.phase!=phase_)return {Reason::OldPublication,{}};
        if(activeMutation_)return {Reason::MutationInProgress,{}};
        const auto refusal=[&](Reason reason){invalidate();return Result{reason,{}};};
        const auto& proof=body.publication;
        if(roster.capture!=ownership.capture||!roster.result.observedAllSlots()||
           !roster.result.relationshipsIndexed||!roster.result.ownedPublication||
           roster.result.rows!=roster.rows.size()||roster.rows.size()>4096||
           !proof.currentPhysicalQualified||proof.progressionQualified||
           proof.kind!=owned::provider::Kind::CurrentPhysicalSnapshot||
           proof.at.session!=world_.world||proof.at.epoch!=phase_||proof.at.tick!=phase_||
           proof.at.phase!=owned::rear::Phase::RetainedMovement||proof.physicallyAbsent||
           !inventory::detail::fullTrain(proof.train)||body.cars.empty()||body.cars.size()>4096)
            return refusal(Reason::InvalidRoster);
        uintptr_t inputBegin{},inputEnd{};
        const auto storageBegin=reinterpret_cast<uintptr_t>(&storage_);
        const auto storageEnd=storageBegin+sizeof storage_;
        if(!inventory::detail::range(body.cars.data(),body.cars.size(),sizeof(owned::provider::PhysicalCar),inputBegin,inputEnd)||
           (inputBegin<storageEnd&&storageBegin<inputEnd))return refusal(Reason::InvalidConsist);
        if(!budget.take(roster.rows.size()+body.cars.size())){++counters_.budget;return refusal(Reason::Budget);}
        size_t root=roster.rows.size();uint64_t lastOrdinal{};
        for(size_t i=0;i<roster.rows.size();++i){
            const auto& row=roster.rows[i];const auto ordinal=inventory::detail::index(row.train);
            if(!inventory::detail::fullTrain(row.train)||(i&&ordinal<=lastOrdinal))return refusal(Reason::InvalidRoster);
            lastOrdinal=ordinal;if(row.train==proof.train)root=i;
        }
        if(root==roster.rows.size())return refusal(Reason::MissingRoot);
        const auto& rootRow=roster.rows[root];
        if(rootRow.attached||rootRow.parentRow!=inventory::noRow||!rootRow.active||rootRow.childCount>=64)
            return refusal(Reason::InvalidConsist);
        std::array<MemberTuple,64> tuples{};size_t tupleCount=1;
        tuples[0]=member(rootRow);uint32_t previous=inventory::noRow;
        for(auto child=rootRow.firstChild;child!=inventory::noRow;child=roster.rows[child].nextSibling){
            // The inventory prepends children while scanning ascending rows:
            // the sealed inverse index therefore has strictly DESCENDING links.
            // Match the body provider; an increasing link is not producer output.
            if(child>=roster.rows.size()||tupleCount==tuples.size()||
               (previous!=inventory::noRow&&child>=previous))return refusal(Reason::InvalidRoster);
            const auto& row=roster.rows[child];
            if(!row.attached||row.parent!=proof.train||row.parentRow!=root||row.firstChild!=inventory::noRow||row.childCount)
                return refusal(Reason::InvalidRoster);
            tuples[tupleCount++]=member(row);previous=child;
        }
        if(tupleCount!=size_t(rootRow.childCount)+1)return refusal(Reason::InvalidRoster);
        // Preserve the native physical car order, settings, model and exact
        // float length. Parent/child reordering and toggled reversal must differ.
        const auto carChecks=body.cars.size()*tupleCount+body.cars.size()*(body.cars.size()-1)/2;
        if(!budget.take(carChecks)){++counters_.budget;return refusal(Reason::Budget);}
        for(size_t i=0;i<body.cars.size();++i){
            const auto& value=body.cars[i];bool ownerFound=false;
            for(size_t n=0;n<tupleCount;++n)ownerFound=ownerFound||tuples[n].train==value.setup.owner;
            if(!ownerFound||!value.setup.model||!std::isfinite(value.nativeMeters)||value.nativeMeters<=0)
                return refusal(Reason::InvalidConsist);
            for(size_t j=0;j<i;++j)if(body.cars[j].setup.owner==value.setup.owner&&
               body.cars[j].setup.ordinal()==value.setup.ordinal())return refusal(Reason::InvalidConsist);
        }
        const auto at=find(proof.train);const bool fresh=at==roots_;
        if(fresh&&roots_==Roots){++counters_.capacity;return refusal(Reason::Capacity);}
        Record old=fresh?Record{}:storage_.records[at];
        if(!budget.take(old.members+old.cars+roots_)){++counters_.budget;return refusal(Reason::Budget);}
        bool same=!fresh&&old.train==proof.train&&old.members==tupleCount&&old.cars==body.cars.size();
        if(same)for(size_t n=0;n<tupleCount;++n)same=same&&storage_.members[old.memberBegin+n]==tuples[n];
        if(same)for(size_t n=0;n<body.cars.size();++n)same=same&&storage_.cars[old.carBegin+n]==car(body.cars[n]);
        if(!same){
            const size_t nextMembers=members_-old.members+tupleCount,nextCars=cars_-old.cars+body.cars.size();
            if(nextMembers>Members||nextCars>Cars){++counters_.capacity;return refusal(Reason::Capacity);}
            if(!budget.take(members_+cars_+tupleCount+body.cars.size()+roots_)){++counters_.budget;return refusal(Reason::Budget);}
            if(!ownership.valid()){lose();return {Reason::OwnershipUnknown,{}};}
            const auto revision=sequence();if(!revision)return {Reason::SequenceExhausted,{}};
            const auto memberBegin=fresh?members_:old.memberBegin,carBegin=fresh?cars_:old.carBegin;
            const auto memberTail=memberBegin+old.members,carTail=carBegin+old.cars;
            std::memmove(storage_.members.data()+memberBegin+tupleCount,storage_.members.data()+memberTail,
                (members_-memberTail)*sizeof(MemberTuple));
            std::memmove(storage_.cars.data()+carBegin+body.cars.size(),storage_.cars.data()+carTail,
                (cars_-carTail)*sizeof(CarTuple));
            for(size_t n=0;n<roots_;++n)if(n!=at){auto& row=storage_.records[n];
                if(row.memberBegin>=memberTail)row.memberBegin=row.memberBegin-old.members+tupleCount;
                if(row.carBegin>=carTail)row.carBegin=row.carBegin-old.cars+body.cars.size();}
            for(size_t n=0;n<tupleCount;++n)storage_.members[memberBegin+n]=tuples[n];
            for(size_t n=0;n<body.cars.size();++n)storage_.cars[carBegin+n]=car(body.cars[n]);
            storage_.records[at]={proof.train,revision,revision,phase_,memberBegin,tupleCount,carBegin,body.cars.size()};
            members_=nextMembers;cars_=nextCars;if(fresh)++roots_;++counters_.tupleChanges;
        }else storage_.records[at].phase=phase_;
        if(!ownership.valid()||pendingLost_.load(std::memory_order_acquire)){lose();return {Reason::OwnershipUnknown,{}};}
        ++counters_.compositionObservations;return {Reason::Ready,stamp(storage_.records[at]),true};
    }
    Result unchanged(const Stamp& previous)noexcept {
        Gate gate(*this,false);if(!gate.held)return {Reason::Busy,{}};consumePending();
        if(lost_)return {Reason::Lost,{}};
        if(previous.owner!=world_)return {Reason::WorldChanged,{}};
        if(activeMutation_)return {Reason::MutationInProgress,{}};
        const auto at=find(previous.train);
        if(at==roots_||storage_.records[at].train!=previous.train)return {Reason::IdentityChanged,{}};
        const auto now=stamp(storage_.records[at]);
        if(now.consistRevision!=previous.consistRevision)return {Reason::TupleChanged,now};
        if(now.globalMutation!=previous.globalMutation||now.localMutation!=previous.localMutation)
            return {Reason::MutationObserved,now};
        return {Reason::Ready,now,true};
    }
    // BEFORE original 30E020, invalidate globally without decoding an opaque
    // command vector or guessing its transitive train/model/network effects.
    // Even failed native batches remain a break. Completion cannot heal it.
    Mutation beginBatch(const BatchScope& scope)noexcept {
        const bool valid=scope.valid();
        Gate gate(*this);if(!gate.held)return {};consumePending();
        if(!world_.world){++counters_.prebootstrap;return {};}
        if(scope.owner.database!=world_.database||scope.owner.simulation!=world_.simulation){++counters_.foreign;return {};}
        if(!valid||scope.owner!=world_){invalidate();++counters_.unqualified;return {};}
        if(activeMutation_){invalidate();lose();++counters_.nested;return {};}
        if(lost_)return {};
        invalidate();if(lost_)return {};
        activeMutation_=globalMutation_;++counters_.begun;counters_.lastMutation=activeMutation_;
        return {world_,activeMutation_};
    }
    Reason finishBatch(Mutation token,bool originalReturned)noexcept {
        Gate gate(*this);if(!gate.held)return Reason::Busy;consumePending();
        if(!token)return Reason::Unarmed;
        if(token.owner!=world_||activeMutation_!=token.sequence||lost_){lose();return Reason::Lost;}
        activeMutation_=0;
        if(!originalReturned){lose();++counters_.exceptions;return Reason::OriginalException;}
        ++counters_.completed;counters_.lastCompleted=token.sequence;return Reason::Completed;
    }
    // Existing lifecycle/merge/reset wrappers call this BEFORE the native
    // operation. Equal bytes after a merge never heal a pre-merge stamp.
    void invalidateWorld(bool abandoned=false)noexcept {
        Gate gate(*this);if(!gate.held)return;consumePending();
        if(!world_.world)return;
        invalidate();if(abandoned)lose();
    }
    Counters counters()noexcept {
        Gate gate(*this,false);if(!gate.held)return {};consumePending();auto out=counters_;
        out.world=world_.world;out.globalMutation=globalMutation_;out.roots=roots_;out.members=members_;out.cars=cars_;return out;
    }
};
} // namespace nimby::windows::mutation_continuity
