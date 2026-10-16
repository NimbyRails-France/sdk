#pragma once
#include <engine/native_rear_coverage.h>
#include <platform/windows/runtime/native_consist_inventory.h>
#include <platform/windows/runtime/integration_commit_witness.h>
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <span>

namespace nimby::windows::consist_rear {
namespace rear=nimby::engine::native_rear;
namespace inventory=nimby::windows::consist_inventory;

// PRIVATE Windows119 adapter. Offsets/units are qualified by the pinned binary
// in native-consist-inventory-20261010/evidence.json and the retained-consist
// evidence. 44CF80 copies CarSetup(32B), writes owner +8 / source ordinal +10,
// reverses children and toggles +1C; its child guard can OMIT >30 cars. 3E0D90
// adds model +FC METERS as float (3E0F07..3E0F15), skipping missing models.
// 43F670 / 44B5B0 describe an aggregate, never per-car position identities.
// This provider refuses missing aggregate members rather than guessing their
// position: 44A330's attached branch clears active and frees child history.
// Current-only capture reads a torn-down attached member through DB CarSetup
// and coupling metadata, then proves every tuple in the live root aggregate.
// It never relies on that child's unqualified Dynamics/history/Presence. Progression
// capture retains its stricter original member-data requirements.
struct CarSetup {
    uint64_t model{},owner{};std::array<uint8_t,16> settings{};
    bool operator==(const CarSetup&)const=default;
    uint16_t ordinal()const noexcept {uint16_t out{};std::memcpy(&out,settings.data(),2);return out;}
};
static_assert(sizeof(CarSetup)==32&&std::is_trivially_copyable_v<CarSetup>);
struct PhysicalCar {CarSetup setup;float nativeMeters{};double meters{};};
static_assert(std::is_trivially_copyable_v<PhysicalCar>);
struct NativeHistory {uint64_t track{};int32_t direction{},reserved{};bool operator==(const NativeHistory&)const=default;};
static_assert(sizeof(NativeHistory)==16);

// A Scope is supplied by the NATIVE ownership/lineage provider, never by a mod.
// validate must seal one actually immutable live scope or an owned snapshot of
// ALL DB/Sim/model/track/member data. CurrentPhysicalSnapshot qualifies the
// retained body at this one publication, including its geometric joins; it
// does NOT qualify progression across reversal, relocation, replan or import.
// ProgressionQualified additionally requires that inter-publication proof.
// An ancestry window/equal readbacks/
// content hash alone MUST return false. This adapter cannot mint these proofs.
// Synthetic tests explicitly supply owned memory; production factory is external.
struct Witness {
    uintptr_t database{},simulation{}; // Borrowed input only; never returned.
    rear::Publication at;
    uint64_t train{},consistRevision{},continuityRevision{};
    bool operator==(const Witness&)const=default;
};
enum class Kind : uint8_t { ProgressionQualified,CurrentPhysicalSnapshot };
struct Scope {
    Witness witness;
    bool (*validate)(void*,const Witness&) noexcept{};
    void* context{};
    Kind kind=Kind::ProgressionQualified;
    bool valid()const noexcept {
        return (kind==Kind::ProgressionQualified||kind==Kind::CurrentPhysicalSnapshot)&&
            validate&&rear::detail::publication(witness.at)&&witness.at.phase==rear::Phase::RetainedMovement&&
            witness.consistRevision&&witness.continuityRevision&&validate(context,witness);
    }
};
// One owned roster per publication, reused for every driving root. Its local
// ObservedAllSlots status is NOT an ownership/continuity certificate; the same
// external Scope must seal the roster and every dependent native value.
struct InventoryView {
    std::span<const inventory::Row> rows;
    inventory::Result result;
    Witness witness;
};
enum class Reason : uint8_t {
    Ready,UnqualifiedScope,ScopeChanged,ArithmeticUnqualified,InvalidStorage,OverlappingStorage,
    BudgetRefused,ReadLimit,ByteLimit,WorkLimit,InvalidAddress,Unreadable,ReaderException,
    InventoryUnknown,MissingTrain,AttachedRoot,MemberCapacity,InvalidOrder,AmbiguousOrder,
    InvalidPool,IdentityChanged,InvalidVector,CarCapacity,InvalidConfiguration,EmptyConsist,
    CurrentCompositionLag,MissingAggregateMember,UnexpectedAggregateCar,DuplicateCar,
    InvalidModel,ModelCapacity,MissingModel,ModelChain,InvalidModelLength,NativeLengthMismatch,
    ValuesChanged,InvalidPresence,MirrorMismatch,HistoryCapacity,InvalidHistory,
    TrackCapacity,TopologyCapacity,InvalidTopology,MissingTransition,HistoryDirection,
    RearCoverageUnknown,CurrentBodyAbsent,InactiveRootWithActiveMember,Count
};
inline const char* reasonName(Reason reason)noexcept {
    switch(reason){
#define NRF_REAR_REASON(value,label) case Reason::value:return label
    NRF_REAR_REASON(Ready,"qualified_complete_aggregate");
    NRF_REAR_REASON(UnqualifiedScope,"missing_owned_scope_or_continuity");
    NRF_REAR_REASON(ScopeChanged,"ownership_or_lineage_changed");
    NRF_REAR_REASON(ArithmeticUnqualified,"unqualified_floating_arithmetic");
    NRF_REAR_REASON(InvalidStorage,"invalid_caller_storage_or_limits");
    NRF_REAR_REASON(OverlappingStorage,"overlapping_caller_storage");
    NRF_REAR_REASON(BudgetRefused,"shared_budget_refused");
    NRF_REAR_REASON(ReadLimit,"local_read_limit");
    NRF_REAR_REASON(ByteLimit,"local_byte_limit");
    NRF_REAR_REASON(WorkLimit,"local_work_limit");
    NRF_REAR_REASON(InvalidAddress,"invalid_native_address");
    NRF_REAR_REASON(Unreadable,"native_read_refused");
    NRF_REAR_REASON(ReaderException,"reader_exception");
    NRF_REAR_REASON(InventoryUnknown,"whole_motion_inventory_unqualified");
    NRF_REAR_REASON(MissingTrain,"train_not_in_whole_pool");
    NRF_REAR_REASON(AttachedRoot,"attached_member_is_not_aggregate_root");
    NRF_REAR_REASON(MemberCapacity,"coupled_member_capacity");
    NRF_REAR_REASON(InvalidOrder,"coupling_order_outside_qualified_range");
    NRF_REAR_REASON(AmbiguousOrder,"coupling_order_tie_unqualified");
    NRF_REAR_REASON(InvalidPool,"invalid_native_pool");
    NRF_REAR_REASON(IdentityChanged,"full_identity_changed");
    NRF_REAR_REASON(InvalidVector,"invalid_or_excessive_native_vector");
    NRF_REAR_REASON(CarCapacity,"physical_car_capacity");
    NRF_REAR_REASON(InvalidConfiguration,"configuration_not_qualified");
    NRF_REAR_REASON(EmptyConsist,"no_selected_physical_cars");
    NRF_REAR_REASON(CurrentCompositionLag,"planned_and_retained_car_tuples_differ");
    NRF_REAR_REASON(MissingAggregateMember,"native_aggregate_omits_expected_cars");
    NRF_REAR_REASON(UnexpectedAggregateCar,"native_aggregate_contains_extra_cars");
    NRF_REAR_REASON(DuplicateCar,"duplicate_physical_car_tuple");
    NRF_REAR_REASON(InvalidModel,"invalid_model_catalogue");
    NRF_REAR_REASON(ModelCapacity,"model_cache_capacity");
    NRF_REAR_REASON(MissingModel,"expected_model_missing");
    NRF_REAR_REASON(ModelChain,"model_bucket_chain_unqualified");
    NRF_REAR_REASON(InvalidModelLength,"model_length_not_finite_positive_normal_meters");
    NRF_REAR_REASON(NativeLengthMismatch,"native_length_differs_from_complete_car_sum");
    NRF_REAR_REASON(ValuesChanged,"native_values_changed_during_copy");
    NRF_REAR_REASON(InvalidPresence,"root_current_presence_unqualified");
    NRF_REAR_REASON(MirrorMismatch,"root_presence_and_mirror_differ");
    NRF_REAR_REASON(HistoryCapacity,"rear_history_or_output_capacity");
    NRF_REAR_REASON(InvalidHistory,"invalid_retained_history");
    NRF_REAR_REASON(TrackCapacity,"track_cache_capacity");
    NRF_REAR_REASON(TopologyCapacity,"topology_children_capacity");
    NRF_REAR_REASON(InvalidTopology,"unqualified_track_topology");
    NRF_REAR_REASON(MissingTransition,"retained_transition_not_reciprocal");
    NRF_REAR_REASON(HistoryDirection,"retained_direction_unqualified");
    NRF_REAR_REASON(RearCoverageUnknown,"whole_body_envelope_unqualified");
    NRF_REAR_REASON(CurrentBodyAbsent,"qualified_current_body_absent");
    NRF_REAR_REASON(InactiveRootWithActiveMember,"inactive_root_has_active_attached_member");
    case Reason::Count:break; // Sentinel is not a provider outcome.
#undef NRF_REAR_REASON
    }
    return "unknown";
}
enum class Status : uint8_t {Unknown,Ready};
struct PublicationProof {
    rear::Publication at;
    uint64_t train{},consistRevision{};
    Kind kind=Kind::ProgressionQualified;
    bool currentPhysicalQualified=false,progressionQualified=false;
    // Explicit physical absence at this owned publication, never historical
    // release. Inactive attached members still belong to their root's body.
    bool physicallyAbsent=false;
};
static_assert(std::is_trivially_copyable_v<PublicationProof>);
struct Result {
    Status status=Status::Unknown;Reason reason=Reason::UnqualifiedScope;
    inventory::Reason inventoryReason=inventory::Reason::UnqualifiedProfile;
    rear::Reason coverageReason=rear::Reason::UnqualifiedHistory;
    uint64_t train{},member{},model{};size_t cars{},expectedCars{},members{},history{},portions{},reads{},bytes{},work{};
    rear::Observation observation;
    PublicationProof publication;
    bool ready()const noexcept{return status==Status::Ready;}
};
struct Limits {
    inventory::Limits inventory;
    size_t members=64,cars=4096,history=4096,models=512,modelNodes=4096,
        tracks=512,children=16384,reads=8192,bytes=4u*1024u*1024u,work=262144;
};
namespace detail {
using inventory::detail::address;using inventory::detail::field;using inventory::detail::index;
inline bool identity(uint64_t id,uint64_t tag)noexcept{return id>>48==tag&&(id&0x0000ffffffffffffULL);}
struct Vector {uintptr_t begin{},end{},cap{};bool operator==(const Vector&)const=default;};
inline bool vectorCount(Vector vector,size_t stride,size_t maximum,size_t& count)noexcept {
    if(vector.end<vector.begin||vector.cap<vector.end||
        (vector.end-vector.begin)%stride||(vector.cap-vector.begin)%stride||
        (vector.end-vector.begin)/stride>maximum||
        (vector.cap&& !address(vector.begin,vector.cap-vector.begin,true)))return false;
    count=(vector.end-vector.begin)/stride;return true;
}
struct Member {
    inventory::Row inventory;
    uintptr_t motion{},plannedTrain{};
    Vector retained,planned,history;
    float nativeLength{};size_t plannedCount{},retainedCount{},expectedBegin{},expectedCount{};
    std::array<unsigned char,24> mirror{};
    nimby::windows::automatic::CommitPresence presence{};
    bool operator==(const Member& other)const noexcept {
        return inventory==other.inventory&&motion==other.motion&&plannedTrain==other.plannedTrain&&
            retained==other.retained&&planned==other.planned&&history==other.history&&nativeLength==other.nativeLength&&
            plannedCount==other.plannedCount&&retainedCount==other.retainedCount&&expectedBegin==other.expectedBegin&&
            expectedCount==other.expectedCount&&mirror==other.mirror&&
            nimby::windows::automatic::sameCommitPresence(presence,other.presence);
    }
};
struct Model {uint64_t id{};uintptr_t node{},next{};float meters{};};
struct Track {
    uint64_t id{},a{},b{},parent{};uintptr_t address{};double metric{},attachmentFraction{};
    int32_t attachmentDirection{};int8_t orientation{};Vector children;
    size_t childrenBegin{},childrenCount{};
};
struct Pool {
    uintptr_t at{};std::array<unsigned char,48> header{};size_t stride{};
};
}
// All storage is owned by the caller and reused. Native addresses exist ONLY in
// temporary Member/Model/Track workspace and must never be exported/retained as
// permissions. Every span is bounded below; initialize only captured prefixes.
struct Workspace {
    std::span<inventory::Row> inventoryRows,inventoryScratch;
    std::span<uint64_t> inventoryIds;
    std::span<detail::Member> members;
    std::span<CarSetup> expected,actual,temporary;
    std::span<PhysicalCar> cars;
    std::span<NativeHistory> history,historyAgain;
    std::span<detail::Model> models;
    std::span<detail::Track> tracks;
    std::span<uint64_t> children,childrenAgain;
    std::span<rear::HistorySection> sections;
    std::span<rear::Portion> portions;
};
struct Output {
    std::span<PhysicalCar> cars;std::span<rear::HistorySection> sections;std::span<rear::Portion> portions;
    PublicationProof* publication{}; // Optional owned POD metadata, committed with the body.
};

template<class Read> Result captureImpl(uintptr_t database,uintptr_t simulation,uint64_t train,
    bool profileQualified,const Scope& scope,Read&& reader,inventory::Budget& budget,
    Workspace scratch,Output output,Limits limits,const InventoryView* supplied)noexcept {
    using namespace detail;
    Result result;result.train=train;
    const auto fail=[&](Reason reason){result.reason=reason;result.cars=result.history=result.portions=0;result.observation={};result.publication={};return result;};
    if(!budget.available())return fail(Reason::BudgetRefused);
    if(!address(database,0x410,true)||!address(simulation,0xa0+48,true))return fail(Reason::InvalidAddress);
    if(!profileQualified||!identity(train,5)||scope.witness.database!=database||scope.witness.simulation!=simulation||
        scope.witness.train!=train||!scope.valid())return fail(Reason::UnqualifiedScope);
    if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
    if(!limits.members||limits.members>64||!limits.cars||limits.cars>4096||!limits.history||limits.history>4096||
       !limits.models||limits.models>512||!limits.modelNodes||limits.modelNodes>4096||!limits.tracks||limits.tracks>512||
       !limits.children||limits.children>16384||!limits.reads||limits.reads>8192||!limits.bytes||limits.bytes>4u*1024u*1024u||
       !limits.work||limits.work>262144)return fail(Reason::InvalidStorage);
    // Check every caller-owned range, including scratch-to-scratch aliasing.
    // The 21-span metadata walk is fixed and does not inspect unused elements.
    std::array<std::pair<uintptr_t,uintptr_t>,21> ranges{};size_t rangeCount=0;
    const auto storage=[&](auto values,size_t maximum,bool required=true){
        if((required&&values.empty())||values.size()>maximum)return false;
        auto& [begin,end]=ranges[rangeCount++];
        return inventory::detail::range(values.data(),values.size(),sizeof(typename decltype(values)::element_type),begin,end);
    };
    if(!storage(scratch.inventoryRows,4096)||!storage(scratch.inventoryScratch,4096)||!storage(scratch.inventoryIds,65536)||
       !storage(scratch.members,64)||!storage(scratch.expected,4096)||!storage(scratch.actual,4096)||!storage(scratch.temporary,4096)||
       !storage(scratch.cars,4096)||!storage(scratch.history,4096)||!storage(scratch.historyAgain,4096)||
       !storage(scratch.models,512)||!storage(scratch.tracks,512)||!storage(scratch.children,16384)||!storage(scratch.childrenAgain,16384)||
       !storage(scratch.sections,4096)||!storage(scratch.portions,4096)||!storage(output.cars,4096)||
       !storage(output.sections,4096)||!storage(output.portions,4096))return fail(Reason::InvalidStorage);
    if(output.publication&&!storage(std::span<PublicationProof>{output.publication,1},1))return fail(Reason::InvalidStorage);
    for(size_t i=0;i<rangeCount;++i)for(size_t j=0;j<i;++j)
        if(inventory::detail::overlaps(ranges[i].first,ranges[i].second,ranges[j].first,ranges[j].second))return fail(Reason::OverlappingStorage);
    if(supplied){
        if(!supplied->result.observedAllSlots()||!supplied->result.relationshipsIndexed||supplied->result.rows!=supplied->rows.size()||
           supplied->rows.size()>4096||supplied->witness.database!=database||supplied->witness.simulation!=simulation||
           supplied->witness.at!=scope.witness.at||supplied->witness.consistRevision!=scope.witness.consistRevision||
           supplied->witness.continuityRevision!=scope.witness.continuityRevision)
            return fail(Reason::InventoryUnknown);
        uintptr_t begin{},end{};
        if(!inventory::detail::range(supplied->rows.data(),supplied->rows.size(),sizeof(inventory::Row),begin,end))
            return fail(Reason::InvalidStorage);
        for(size_t i=0;i<rangeCount;++i)
            if(inventory::detail::overlaps(begin,end,ranges[i].first,ranges[i].second))return fail(Reason::OverlappingStorage);
    }
    Reason refusal=Reason::BudgetRefused;
    const auto available=[&]{if(!budget.available()){refusal=Reason::BudgetRefused;return false;}return true;};
    const auto work=[&]{
        if(result.work==limits.work){budget.exhausted=true;refusal=Reason::WorkLimit;return false;}
        return result.work++%32!=0||available();
    };
    const auto read=[&](uintptr_t at,void* target,size_t bytes){
        if(result.reads==limits.reads){budget.exhausted=true;refusal=Reason::ReadLimit;return false;}
        if(bytes>limits.bytes-result.bytes){budget.exhausted=true;refusal=Reason::ByteLimit;return false;}
        if(!budget.available(bytes)){refusal=Reason::BudgetRefused;return false;}
        ++result.reads;result.bytes+=bytes;
        if(!address(at,bytes)){refusal=Reason::InvalidAddress;return false;}
        if(!reader(at,target,bytes)){refusal=Reason::Unreadable;return false;}return true;
    };
    const auto get=[&](uintptr_t at,auto& value){return read(at,&value,sizeof value);};
    const auto scopeValid=[&]{return available()&&scope.valid();};
    try {
        if(!available())return fail(refusal);
        // Inventory applies the same terminal budget. Its own accounting is
        // already done before our reader, so avoid charging the shared budget
        // twice; still retain the provider's aggregate local quotas.
        const auto inventoryReader=[&](uintptr_t at,void* target,size_t bytes){
            if(result.reads==limits.reads){budget.exhausted=true;refusal=Reason::ReadLimit;return false;}
            if(bytes>limits.bytes-result.bytes){budget.exhausted=true;refusal=Reason::ByteLimit;return false;}
            ++result.reads;result.bytes+=bytes;return reader(at,target,bytes);
        };
        auto inventoryLimits=limits.inventory;
        inventoryLimits.work=std::min(inventoryLimits.work,limits.work);
        auto roster=supplied?supplied->result:inventory::captureMotionInventory(simulation,true,inventoryReader,budget,scratch.inventoryRows,
            {scratch.inventoryScratch,scratch.inventoryIds},inventoryLimits);
        const auto rosterRows=supplied?supplied->rows:std::span<const inventory::Row>{scratch.inventoryRows.data(),roster.rows};
        result.inventoryReason=roster.reason;
        if(!supplied)result.work=roster.work;
        if(!roster.observedAllSlots()){
            if(roster.reason==inventory::Reason::ReadLimit)return fail(Reason::ReadLimit);
            if(roster.reason==inventory::Reason::ByteLimit)return fail(Reason::ByteLimit);
            if(roster.reason==inventory::Reason::WorkLimit)return fail(Reason::WorkLimit);
            return fail(budget.exhausted?refusal:Reason::InventoryUnknown);
        }
        if(!supplied){
            if(roster.work>limits.work){budget.exhausted=true;return fail(Reason::WorkLimit);}
            result.work=roster.work;
        }
        if(!roster.relationshipsIndexed)return fail(Reason::InventoryUnknown);
        // The inventory producer checks strict native ordinal order and the
        // complete inverse-parent index once per owned publication. Reuse that
        // index instead of two O(N) rescans for every driving root.
        size_t low=0,high=roster.rows;const auto trainIndex=index(train);
        while(low<high){
            if(!work())return fail(refusal);
            const auto mid=low+(high-low)/2;
            if(index(rosterRows[mid].train)<trainIndex)low=mid+1;else high=mid;
        }
        const auto root=low;
        if(root==roster.rows||index(rosterRows[root].train)!=trainIndex)return fail(Reason::MissingTrain);
        if(rosterRows[root].train!=train)return fail(Reason::IdentityChanged);
        if(rosterRows[root].attached)return fail(Reason::AttachedRoot);
        if(scope.kind==Kind::CurrentPhysicalSnapshot&&!rosterRows[root].active){
            // Every attached row is represented through its aggregate root.
            // An absent root alone cannot make a live child disappear from a
            // whole-publication occupancy query. Inspect the already owned
            // inverse index without touching torn-down optional histories.
            size_t childrenSeen=0,previous=roster.rows;
            for(auto n=rosterRows[root].firstChild;n!=inventory::noRow;n=rosterRows[n].nextSibling){
                if(!work())return fail(refusal);
                if(n>=roster.rows||n>=previous||n==root||!rosterRows[n].attached||
                   rosterRows[n].parentRow!=root||rosterRows[n].parent!=train||
                   !identity(rosterRows[n].train,5)||index(rosterRows[n].train)==trainIndex)
                    return fail(Reason::InventoryUnknown);
                previous=n;
                if(++childrenSeen>rosterRows[root].childCount)return fail(Reason::InventoryUnknown);
                if(rosterRows[n].active){result.member=rosterRows[n].train;return fail(Reason::InactiveRootWithActiveMember);}
            }
            if(childrenSeen!=rosterRows[root].childCount)return fail(Reason::InventoryUnknown);
            // Motion+1D0 discriminates the constructed Dynamics/history value:
            // 445FE0->45B2D0 sets it after construction; 43E300/44A330 free
            // history before clearing it. 44B5B0 excludes inactive roots from
            // occupation, independently of speed/running. Do not even copy the
            // history descriptor: its pointer can remain stale after teardown.
            // The supplied scope, not these defensive readbacks, owns all data.
            std::array<unsigned char,48> header{},confirmedHeader{};
            if(!read(simulation+0xa0,header.data(),header.size()))return fail(refusal);
            const auto begin=field<uintptr_t>(header,24),end=field<uintptr_t>(header,32),cap=field<uintptr_t>(header,40);
            if(field<uint32_t>(header,0)!=0x100000||field<uint32_t>(header,4)!=9||field<uint32_t>(header,8)!=512||
                field<uint32_t>(header,12)!=512*0x638||field<uint32_t>(header,16)!=511||
                end<begin||cap<end||(end-begin)%8||(cap-begin)%8||(end-begin)/8>128||
                (cap&&!address(begin,cap-begin,true)))return fail(Reason::InvalidPool);
            const auto block=trainIndex>>9;
            if(block>=(end-begin)/8)return fail(Reason::IdentityChanged);
            uintptr_t nativeBlock{};
            if(!get(begin+block*8,nativeBlock))return fail(refusal);
            if(!address(nativeBlock,0x100000,true))return fail(Reason::InvalidAddress);
            const auto motion=nativeBlock+(trainIndex&511)*0x638;
            const auto confirmRoot=[&]{
                uint64_t id{};std::array<unsigned char,0x21> coupling{},activity{};int32_t mode{};
                if(!get(motion,id)||!read(motion+0x1d0,coupling.data(),coupling.size())||
                    !get(motion+0x80,mode)||!read(motion+0x5d0,activity.data(),activity.size()))return false;
                if(id!=train){refusal=Reason::IdentityChanged;return false;}
                auto row=rosterRows[root];row.nativeMode=mode;
                row.parent=field<uint64_t>(coupling,8);row.couplingOrder=field<int64_t>(coupling,16);
                row.active=coupling[0];row.reversed=coupling[0x18];row.attached=coupling[0x20];
                row.running=activity[0];row.scheduled=activity[0x20];
                if(row!=rosterRows[root]){refusal=Reason::ValuesChanged;return false;}return true;
            };
            if(!confirmRoot()||!read(simulation+0xa0,confirmedHeader.data(),confirmedHeader.size()))return fail(refusal);
            if(confirmedHeader!=header)return fail(Reason::ValuesChanged);
            uintptr_t confirmedBlock{};
            if(!get(begin+block*8,confirmedBlock))return fail(refusal);
            if(confirmedBlock!=nativeBlock)return fail(Reason::ValuesChanged);
            if(!confirmRoot())return fail(refusal);
            if(!scopeValid())return fail(budget.exhausted?refusal:Reason::ScopeChanged);
            result.publication={scope.witness.at,train,scope.witness.consistRevision,scope.kind,true,false,true};
            if(output.publication)*output.publication=result.publication;
            result.reason=Reason::CurrentBodyAbsent;result.status=Status::Ready;
            return result; // No car/history/portion output or generic observation.
        }
        const auto appendMember=[&](size_t row){
            if(result.members==limits.members||result.members==scratch.members.size()){refusal=Reason::MemberCapacity;return false;}
            auto& member=scratch.members[result.members++];member={};member.inventory=rosterRows[row];return true;
        };
        if(!appendMember(root))return fail(refusal);
        size_t childrenSeen=0,previous=roster.rows;
        for(auto n=rosterRows[root].firstChild;n!=inventory::noRow;n=rosterRows[n].nextSibling){
            if(!work())return fail(refusal);
            // Producer links children in strictly decreasing row order. This
            // rejects cycles, duplicate references and a mismatched/stale link
            // without an unbounded traversal or a per-root N-sized visited set.
            if(n>=roster.rows||n>=previous||n==root||!rosterRows[n].attached||
               rosterRows[n].parentRow!=root||rosterRows[n].parent!=train||
               !identity(rosterRows[n].train,5)||index(rosterRows[n].train)==trainIndex)
                return fail(Reason::InventoryUnknown);
            previous=n;
            if(++childrenSeen>rosterRows[root].childCount)return fail(Reason::InventoryUnknown);
            if(!appendMember(n))return fail(refusal);
        }
        if(childrenSeen!=rosterRows[root].childCount)return fail(Reason::InventoryUnknown);
        // Native child keys are order*1000000; root key is zero. Reject ties
        // instead of presenting an unproved stable tie order as physical order.
        for(size_t n=0;n<result.members;++n){
            auto& row=scratch.members[n].inventory;
            if(row.nativeMode<0||row.nativeMode>8)return fail(Reason::InvalidConfiguration);
            if(row.attached&&(row.couplingOrder < -1000||row.couplingOrder>1000))return fail(Reason::InvalidOrder);
        }
        for(size_t i=1;i<result.members;++i){
            auto value=scratch.members[i];size_t at=i;
            const auto key=value.inventory.attached?value.inventory.couplingOrder*1000000:0;
            while(at){
                if(!work())return fail(refusal);
                const auto& previous=scratch.members[at-1].inventory;
                const auto previousKey=previous.attached?previous.couplingOrder*1000000:0;
                if(previousKey==key)return fail(Reason::AmbiguousOrder);
                if(previousKey<key)break;
                scratch.members[at]=scratch.members[at-1];--at;
            }
            scratch.members[at]=value;
        }
        const auto pool=[&](uintptr_t at,size_t stride,Pool& out){
            out.at=at;out.stride=stride;
            if(!read(at,out.header.data(),out.header.size()))return false;
            const auto shift=field<uint32_t>(out.header,4),count=field<uint32_t>(out.header,8),mask=field<uint32_t>(out.header,16);
            const auto begin=field<uintptr_t>(out.header,24),end=field<uintptr_t>(out.header,32),cap=field<uintptr_t>(out.header,40);
            if(shift>20||count!=(uint32_t(1)<<shift)||mask!=count-1||count>0x100000/stride||
               field<uint32_t>(out.header,12)!=count*stride||field<uint32_t>(out.header,0)!=0x100000||
               end<begin||cap<end||(end-begin)%8||(cap-begin)%8||
               (end-begin)/8>128||(cap&&!address(begin,cap-begin,true))){refusal=Reason::InvalidPool;return false;}
            return true;
        };
        const auto lookup=[&](const Pool& pool,uint64_t id,uintptr_t& out){
            const auto shift=field<uint32_t>(pool.header,4),mask=field<uint32_t>(pool.header,16);
            const auto begin=field<uintptr_t>(pool.header,24),end=field<uintptr_t>(pool.header,32);
            const auto ordinal=index(id),block=ordinal>>shift;uintptr_t addressBlock{};
            if(block>=(end-begin)/8){refusal=Reason::IdentityChanged;return false;}
            if(!get(begin+block*8,addressBlock))return false;
            if(!address(addressBlock,field<uint32_t>(pool.header,0),true)){refusal=Reason::InvalidAddress;return false;}
            out=addressBlock+(ordinal&mask)*pool.stride;uint64_t actual{};
            if(!get(out,actual))return false;
            if(actual!=id){refusal=Reason::IdentityChanged;return false;}return true;
        };
        Pool motions,trains,tracks;
        if(!pool(simulation+0xa0,0x638,motions)||!pool(database+0x200,0x178,trains)||!pool(database,0x4e8,tracks))return fail(refusal);
        const auto absentAttached=[&](const Member& member){
            return scope.kind==Kind::CurrentPhysicalSnapshot&&member.inventory.train!=train&&
                member.inventory.active==0&&member.inventory.attached==1&&member.inventory.parent==train;
        };
        const auto memberHeader=[&](Member& member){
            uint64_t id{},plannedId{};std::array<unsigned char,0x21> coupling,activity;
            if(!get(member.motion,id)||!get(member.plannedTrain,plannedId))return false;
            if(id!=member.inventory.train||plannedId!=id){refusal=Reason::IdentityChanged;return false;}
            auto& row=member.inventory;
            const auto before=row;
            if(!get(member.plannedTrain+0xc0,member.planned)||
               !read(member.motion+0x1d0,coupling.data(),coupling.size())||!get(member.motion+0x80,row.nativeMode)||
               !read(member.motion+0x5d0,activity.data(),activity.size()))return false;
            row.parent=field<uint64_t>(coupling,8);row.couplingOrder=field<int64_t>(coupling,16);
            row.active=coupling[0];row.reversed=coupling[0x18];row.attached=coupling[0x20];row.running=activity[0];row.scheduled=activity[0x20];
            // Revalidate the exact owned roster before deciding whether an
            // optional native body exists. A changed parent/active flag must
            // not make an unqualified optional readable or an omitted child safe.
            if(row!=before){refusal=Reason::ValuesChanged;return false;}
            if(absentAttached(member))return true;
            if(!get(member.motion+8,member.retained)||!get(member.motion+0x40,member.nativeLength)||
               !get(member.motion+0xe0,member.history)||!read(member.motion+0xb8,member.mirror.data(),member.mirror.size())||
               !get(member.motion+0x3a0,member.presence))return false;
            return true;
        };
        size_t rootMember=result.members,expected=0;
        for(size_t n=0;n<result.members;++n){
            if(!work())return fail(refusal);
            auto& member=scratch.members[n];result.member=member.inventory.train;
            const auto before=member.inventory;
            if(!lookup(motions,before.train,member.motion)||!lookup(trains,before.train,member.plannedTrain)||!memberHeader(member))return fail(refusal);
            if(member.inventory!=before)return fail(Reason::ValuesChanged);
            if(!vectorCount(member.planned,32,limits.cars,member.plannedCount)||
               (!absentAttached(member)&&!vectorCount(member.retained,32,limits.cars,member.retainedCount)))return fail(Reason::InvalidVector);
            if(member.plannedCount>scratch.temporary.size()||member.retainedCount>scratch.actual.size())return fail(Reason::CarCapacity);
            if(member.plannedCount&&!read(member.planned.begin,scratch.temporary.data(),member.plannedCount*32))return fail(refusal);
            member.expectedBegin=expected;
            for(size_t i=0;i<member.plannedCount;++i){
                if(!work())return fail(refusal);
                auto setup=scratch.temporary[i];
                if(member.inventory.nativeMode&&(setup.settings[15]&(uint8_t(1)<<(member.inventory.nativeMode-1)))==0)continue;
                if(!setup.model)return fail(Reason::InvalidModel);
                if(expected==limits.cars||expected==scratch.expected.size())return fail(Reason::CarCapacity);
                setup.owner=member.inventory.train;const auto ordinal=static_cast<uint16_t>(i);
                std::memcpy(setup.settings.data(),&ordinal,2);scratch.expected[expected++]=setup;
            }
            member.expectedCount=expected-member.expectedBegin;
            if(member.inventory.train==train)rootMember=n;
            else {
                // A LIVE child's own retained tuples must still match exactly.
                // A current-only inactive child's DB tuples are instead proved
                // below against the complete root-owned aggregate, including
                // full owner identities, selection, ordinal and reversal.
                if(!absentAttached(member)){
                    if(member.retainedCount!=member.expectedCount)return fail(Reason::CurrentCompositionLag);
                    if(member.retainedCount&&!read(member.retained.begin,scratch.actual.data(),member.retainedCount*32))return fail(refusal);
                    for(size_t i=0;i<member.retainedCount;++i){if(!work())return fail(refusal);
                        if(scratch.actual[i]!=scratch.expected[member.expectedBegin+i])return fail(Reason::CurrentCompositionLag);}
                }
                if(member.inventory.reversed){
                    for(size_t i=0;i<member.expectedCount;++i){if(!work())return fail(refusal);
                        auto& car=scratch.expected[member.expectedBegin+i];car.settings[12]=uint8_t(!car.settings[12]);}
                    for(size_t i=0;i<member.expectedCount/2;++i){
                        if(!work())return fail(refusal);
                        std::swap(scratch.expected[member.expectedBegin+i],scratch.expected[expected-1-i]);
                    }
                }
            }
        }
        result.expectedCars=expected;
        if(!expected)return fail(Reason::EmptyConsist);
        if(expected>scratch.cars.size()||expected>output.cars.size())return fail(Reason::CarCapacity);
        if(rootMember==result.members)return fail(Reason::MissingTrain);
        auto& rootNode=scratch.members[rootMember];
        if(rootNode.retainedCount<expected)return fail(Reason::MissingAggregateMember);
        if(rootNode.retainedCount>expected)return fail(Reason::UnexpectedAggregateCar);
        if(!read(rootNode.retained.begin,scratch.actual.data(),expected*32))return fail(refusal);
        // Exact tuple ORDER includes owner generation, ordinal, all settings,
        // configuration selection and the native child reversal transformation.
        for(size_t i=0;i<expected;++i){if(!work())return fail(refusal);
            if(scratch.actual[i]!=scratch.expected[i])return fail(Reason::CurrentCompositionLag);}
        uintptr_t rules{},modelTable{},sentinel{};uint32_t buckets{};
        if(!get(database+0x408,rules))return fail(refusal);
        if(!address(rules,0x48,true))return fail(Reason::InvalidAddress);
        if(!get(rules+0x38,modelTable)||!get(rules+0x40,buckets))return fail(refusal);
        if(!buckets||buckets>1048576||!address(modelTable,(size_t(buckets)+1)*8,true))return fail(Reason::InvalidModel);
        if(!get(modelTable+size_t(buckets)*8,sentinel))return fail(refusal);
        size_t modelCount=0,nodeCount=0;double preciseUpper=0;float nativeTotal=0;
        for(size_t i=0;i<expected;++i){
            if(!work())return fail(refusal);
            const auto id=scratch.actual[i].model;result.model=id;
            size_t found=0;for(;found<modelCount;++found){if(!work())return fail(refusal);if(scratch.models[found].id==id)break;}
            if(found==modelCount){
                if(modelCount==limits.models||modelCount==scratch.models.size())return fail(Reason::ModelCapacity);
                uintptr_t node{};if(!get(modelTable+(id%buckets)*8,node))return fail(refusal);
                for(;;){
                    if(!work())return fail(refusal);
                    if(!node||node==sentinel)return fail(Reason::MissingModel);
                    if(nodeCount++==limits.modelNodes)return fail(Reason::ModelChain);
                    if(!address(node,0x2a8,true))return fail(Reason::InvalidAddress);
                    uint64_t actual{};uintptr_t next{};
                    if(!get(node,actual)||!get(node+0x2a0,next))return fail(refusal);
                    if(actual==id){float meters{};if(!get(node+0xfc,meters))return fail(refusal);
                        if(!std::isfinite(meters)||meters<=0||std::fpclassify(meters)==FP_SUBNORMAL)return fail(Reason::InvalidModelLength);
                        scratch.models[modelCount++]={id,node,next,meters};break;}
                    if(next==node)return fail(Reason::ModelChain);
                    node=next;
                }
            }
            const auto meters=scratch.models[found].meters;
            if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
            const double rounded=preciseUpper+double(meters),virtualB=rounded-preciseUpper;
            const double residual=(preciseUpper-(rounded-virtualB))+(double(meters)-virtualB);
            preciseUpper=residual>0?std::nextafter(rounded,std::numeric_limits<double>::infinity()):rounded;
            nativeTotal=float(nativeTotal+meters);
            if(!std::isfinite(nativeTotal)||!std::isfinite(preciseUpper))return fail(Reason::InvalidModelLength);
            scratch.cars[i]={scratch.actual[i],meters,double(meters)};
        }
        if(nativeTotal!=rootNode.nativeLength)return fail(Reason::NativeLengthMismatch);
        // Only the driving ROOT can supply the aggregate head/history. Attached
        // member Presence can be stale after 44A330 and is never substituted.
        const auto& presence=rootNode.presence;
        if(!nimby::windows::automatic::validCommitPresence(presence)||presence.orientation<0||presence.orientation>2||!rootNode.inventory.active)
            return fail(Reason::InvalidPresence);
        if(field<uint64_t>(rootNode.mirror,0)!=presence.track||field<double>(rootNode.mirror,8)!=presence.fraction||
           field<int8_t>(rootNode.mirror,16)!=presence.direction||field<int8_t>(rootNode.mirror,17)!=presence.orientation)return fail(Reason::MirrorMismatch);
        size_t historyCount{};
        if(!vectorCount(rootNode.history,16,limits.history,historyCount))return fail(Reason::InvalidHistory);
        if(historyCount>scratch.history.size()||historyCount>scratch.historyAgain.size())return fail(Reason::HistoryCapacity);
        if(historyCount&&!read(rootNode.history.begin,scratch.history.data(),historyCount*16))return fail(refusal);
        for(size_t i=0;i<historyCount;++i){if(!work())return fail(refusal);
            if(!identity(scratch.history[i].track,1)||scratch.history[i].direction < -1||scratch.history[i].direction>1)return fail(Reason::InvalidHistory);}

        size_t trackCount=0,childCount=0;
        const auto loadTrack=[&](uint64_t id,size_t& row){
            for(row=0;row<trackCount;++row){if(!work())return false;if(scratch.tracks[row].id==id)return true;}
            if(trackCount==limits.tracks||trackCount==scratch.tracks.size()){refusal=Reason::TrackCapacity;return false;}
            auto& track=scratch.tracks[trackCount];track={};track.id=id;
            if(!identity(id,1)||!lookup(tracks,id,track.address))return false;
            std::array<unsigned char,24> links;std::array<unsigned char,48> attachment;
            if(!read(track.address,links.data(),links.size())||!get(track.address+0x88,track.metric)||
               !get(track.address+0x2c,track.orientation)||!read(track.address+0x3f0,attachment.data(),attachment.size()))return false;
            track.a=field<uint64_t>(links,8);track.b=field<uint64_t>(links,16);
            track.parent=field<uint64_t>(attachment,0);track.attachmentFraction=field<double>(attachment,8);
            track.attachmentDirection=field<int32_t>(attachment,16);track.children=field<Vector>(attachment,24);
            if(field<uint64_t>(links,0)!=id||!std::isfinite(track.metric)||track.metric<=0||track.orientation<0||track.orientation>2||
               (track.a&&!identity(track.a,1))||(track.b&&!identity(track.b,1))||
               (track.parent&&(!identity(track.parent,1)||track.parent==id||!rear::detail::fraction(track.attachmentFraction)||
                 (track.attachmentDirection!=1&&track.attachmentDirection!=-1)||(track.a==0)==(track.b==0)))||
               !vectorCount(track.children,8,limits.children,track.childrenCount)){refusal=Reason::InvalidTopology;return false;}
            if(track.childrenCount>limits.children-childCount||track.childrenCount>scratch.children.size()-childCount||
               track.childrenCount>scratch.childrenAgain.size()-childCount){refusal=Reason::TopologyCapacity;return false;}
            track.childrenBegin=childCount;
            if(track.childrenCount&&!read(track.children.begin,scratch.children.data()+childCount,track.childrenCount*8))return false;
            for(size_t i=0;i<track.childrenCount;++i){if(!work())return false;const auto child=scratch.children[childCount+i];
                if(!identity(child,1)||child==id||(i&&scratch.children[childCount+i-1]>=child)){refusal=Reason::InvalidTopology;return false;}}
            childCount+=track.childrenCount;++trackCount;return true;
        };
        size_t currentRow{};if(!loadTrack(presence.track,currentRow))return fail(refusal);
        double from=presence.fraction;int rearDirection=-int(presence.direction);
        if(presence.orientation&&presence.orientation!=scratch.tracks[currentRow].orientation){
            const auto transformed=1-from;
            // A rounded orientation transform cannot serve as an exact head
            // endpoint. Refuse instead of dropping an arbitrarily tiny piece
            // of occupied track; a future interval cursor can widen it safely.
            if(rear::detail::differenceResidual(1,from,transformed)!=0)return fail(Reason::ArithmeticUnqualified);
            from=transformed;rearDirection=-rearDirection;
        }
        // Canonical path is reconstructed from the retained historical branch
        // choices, never from a new route or head-minus-length shortcut.
        size_t sectionCount=0,historyAt=0;double remaining=std::max(preciseUpper,double(nativeTotal));
        for(;;){
            if(!work())return fail(refusal);
            if(sectionCount==limits.history||sectionCount==scratch.sections.size())return fail(Reason::HistoryCapacity);
            const auto current=scratch.tracks[currentRow];
            const bool hasHistory=historyAt<historyCount;
            const auto next=hasHistory?scratch.history[historyAt].track:uint64_t(0);
            double to=rearDirection>0?1.:0.,nextFrom=0.;size_t nextRow{};bool connected=false;
            if(hasHistory){
                if(next==current.id){
                    const auto direction=scratch.history[historyAt].direction;
                    if(!direction)return fail(Reason::HistoryDirection);
                    to=direction>0?1.:0.;connected=true;nextRow=currentRow;nextFrom=to;
                }else{
                    if(!loadTrack(next,nextRow))return fail(refusal);
                    const auto target=scratch.tracks[nextRow];
                    if(next==current.a){to=0;nextFrom=1;connected=target.b==current.id;}
                    else if(next==current.b){to=1;nextFrom=0;connected=target.a==current.id;}
                    else if(next==current.parent){to=current.a?1.:0.;nextFrom=current.attachmentFraction;
                        connected=std::binary_search(scratch.children.begin()+target.childrenBegin,
                            scratch.children.begin()+target.childrenBegin+target.childrenCount,current.id);}
                    else if(target.parent==current.id){
                        to=target.attachmentFraction;nextFrom=target.b?0.:1.;
                        connected=std::binary_search(scratch.children.begin()+current.childrenBegin,
                            scratch.children.begin()+current.childrenBegin+current.childrenCount,next);}
                }
                if(!connected)return fail(Reason::MissingTransition);
            }
            const auto direction=rear::detail::direction(from,to);
            if(direction&&sectionCount==0&&direction!=rearDirection)return fail(Reason::HistoryDirection);
            auto& section=scratch.sections[sectionCount];section={current.id,sectionCount+1,from,to,current.metric,
                static_cast<int8_t>(direction?direction:rearDirection),static_cast<uint8_t>(current.orientation),
                hasHistory?rear::HistoryEnd::Continues:rear::HistoryEnd::Unknown,false,0,0,0};
            if(sectionCount){const auto& previous=scratch.sections[sectionCount-1];section.joinQualified=true;
                section.previousTrack=previous.track;section.previousFraction=previous.to;section.previousOrientation=previous.orientation;}
            if(!hasHistory){
                const auto adjacent=rearDirection>0?current.b:current.a;
                if(!adjacent&&!current.parent)section.end=rear::HistoryEnd::QualifiedEnd;
            }
            ++sectionCount;
            double distance{};
            if(!rear::detail::distanceLower(from,to,current.metric,distance))return fail(Reason::ArithmeticUnqualified);
            // Select the necessary historical prefix in O(N), then run the
            // full pure evaluator ONCE. Never re-assess every growing prefix.
            if(remaining==0||remaining<distance)break;
            remaining=rear::detail::remainingUpper(remaining,distance);
            if(!hasHistory){
                if(remaining==0&&section.end==rear::HistoryEnd::QualifiedEnd)break;
                result.coverageReason=remaining==0?rear::Reason::EndpointUnqualified:rear::Reason::HistoryTooShort;
                return fail(Reason::RearCoverageUnknown);
            }
            if(next==current.id){
                // Same-track historical endpoint markers need a subsequent
                // retained choice. Do not fabricate a zero-progress route.
                ++historyAt;if(historyAt==historyCount)return fail(Reason::MissingTransition);
                from=to;continue;
            }
            // 43F670 associates the selected row's direction with the NEXT
            // track (param10=local158 after param7=selected ID). Its sign is
            // the forward direction; this trace walks towards the rear. Do
            // not use the following row or reuse an unrelated current sign.
            const auto nextForwardDirection=scratch.history[historyAt].direction;
            if(!nextForwardDirection)return fail(Reason::HistoryDirection);
            from=nextFrom;currentRow=nextRow;rearDirection=-nextForwardDirection;++historyAt;
        }
        rear::Observation candidate{{train,scope.witness.consistRevision,preciseUpper,nativeTotal,scope.witness.at,true,true,true},
            {train,scope.witness.consistRevision,scope.witness.at,true,true,{scratch.sections.data(),sectionCount}},
            {train,presence.track,0,scratch.sections[0].from,scratch.sections[0].rearDirection,scratch.sections[0].orientation,scope.witness.at,true},true};
        if(!available())return fail(refusal);
        const auto coverage=rear::assess(candidate,scope.witness.at,scratch.portions);
        result.coverageReason=coverage.reason;
        if(!available())return fail(refusal);
        if(!coverage.complete())return fail(Reason::RearCoverageUnknown);
        result.observation=candidate;result.portions=coverage.portions;
        // All dependent native storage is compared again before sealing. The
        // external scope still owns the common immutable publication; equality
        // itself does not confer that ownership, lineage or continuity.
        for(size_t n=0;n<result.members;++n){
            if(!work())return fail(refusal);
            auto current=scratch.members[n];
            if(!memberHeader(current))return fail(refusal);
            if(current!=scratch.members[n])return fail(Reason::ValuesChanged);
            if(current.plannedCount&&!read(current.planned.begin,scratch.temporary.data(),current.plannedCount*32))return fail(refusal);
            // Planned settings were transformed into expected; regenerate the
            // exact selected own tuple prefix to check in-place mutations.
            size_t matched=0;
            for(size_t i=0;i<current.plannedCount;++i){if(!work())return fail(refusal);auto car=scratch.temporary[i];
                if(current.inventory.nativeMode&&(car.settings[15]&(uint8_t(1)<<(current.inventory.nativeMode-1)))==0)continue;
                car.owner=current.inventory.train;const auto ordinal=static_cast<uint16_t>(i);std::memcpy(car.settings.data(),&ordinal,2);
                const auto reversed=current.inventory.attached&&current.inventory.reversed;
                if(reversed)car.settings[12]=uint8_t(!car.settings[12]);
                if(matched>=current.expectedCount)return fail(Reason::ValuesChanged);
                const auto at=current.expectedBegin+(reversed?current.expectedCount-1-matched:matched);
                if(car!=scratch.expected[at])return fail(Reason::ValuesChanged);
                ++matched;
            }
            if(matched!=current.expectedCount)return fail(Reason::ValuesChanged);
            if(current.inventory.attached&&!absentAttached(current)){
                if(current.retainedCount&&!read(current.retained.begin,scratch.temporary.data(),current.retainedCount*32))return fail(refusal);
                for(size_t i=0;i<current.retainedCount;++i){if(!work())return fail(refusal);auto car=scratch.temporary[i];
                    const auto reversed=current.inventory.reversed!=0;if(reversed)car.settings[12]=uint8_t(!car.settings[12]);
                    const auto at=current.expectedBegin+(reversed?current.expectedCount-1-i:i);
                    if(car!=scratch.expected[at])return fail(Reason::ValuesChanged);}
            }
        }
        if(!read(rootNode.retained.begin,scratch.temporary.data(),expected*32))return fail(refusal);
        for(size_t i=0;i<expected;++i){if(!work())return fail(refusal);if(scratch.temporary[i]!=scratch.actual[i])return fail(Reason::ValuesChanged);}
        if(historyCount&&!read(rootNode.history.begin,scratch.historyAgain.data(),historyCount*16))return fail(refusal);
        for(size_t i=0;i<historyCount;++i){if(!work())return fail(refusal);if(scratch.history[i]!=scratch.historyAgain[i])return fail(Reason::ValuesChanged);}
        for(size_t i=0;i<modelCount;++i){if(!work())return fail(refusal);const auto& model=scratch.models[i];uint64_t id{};uintptr_t next{};float meters{};
            if(!get(model.node,id)||!get(model.node+0x2a0,next)||!get(model.node+0xfc,meters))return fail(refusal);
            if(id!=model.id||next!=model.next||meters!=model.meters)return fail(Reason::ValuesChanged);}
        for(size_t i=0;i<trackCount;++i){
            if(!work())return fail(refusal);
            const auto& track=scratch.tracks[i];
            std::array<unsigned char,24> links;std::array<unsigned char,48> attachment;double metric{};int8_t orientation{};
            if(!read(track.address,links.data(),links.size())||!get(track.address+0x88,metric)||!get(track.address+0x2c,orientation)||
               !read(track.address+0x3f0,attachment.data(),attachment.size()))return fail(refusal);
            if(field<uint64_t>(links,0)!=track.id||field<uint64_t>(links,8)!=track.a||field<uint64_t>(links,16)!=track.b||
               metric!=track.metric||orientation!=track.orientation||field<uint64_t>(attachment,0)!=track.parent||
               field<double>(attachment,8)!=track.attachmentFraction||field<int32_t>(attachment,16)!=track.attachmentDirection||
               field<Vector>(attachment,24)!=track.children)return fail(Reason::ValuesChanged);
            if(track.childrenCount&&!read(track.children.begin,scratch.childrenAgain.data()+track.childrenBegin,track.childrenCount*8))return fail(refusal);
            for(size_t j=0;j<track.childrenCount;++j){if(!work())return fail(refusal);
                if(scratch.children[track.childrenBegin+j]!=scratch.childrenAgain[track.childrenBegin+j])return fail(Reason::ValuesChanged);}
        }
        if(result.observation.history.sections.size()>output.sections.size()||result.portions>output.portions.size())return fail(Reason::HistoryCapacity);
        // Final pool/catalogue header validation catches replacement. The scope
        // validator handles universal writer exclusion and content lifetimes.
        for(const auto* value:{&motions,&trains,&tracks}){std::array<unsigned char,48> confirmed;
            if(!read(value->at,confirmed.data(),confirmed.size()))return fail(refusal);
            if(confirmed!=value->header)return fail(Reason::ValuesChanged);}
        uintptr_t finalRules{},finalTable{},finalSentinel{};uint32_t finalBuckets{};
        if(!get(database+0x408,finalRules))return fail(refusal);
        if(finalRules!=rules)return fail(Reason::ValuesChanged);
        if(!get(rules+0x38,finalTable)||!get(rules+0x40,finalBuckets)||
           !get(modelTable+size_t(buckets)*8,finalSentinel))return fail(refusal);
        if(finalTable!=modelTable||finalBuckets!=buckets||finalSentinel!=sentinel)return fail(Reason::ValuesChanged);
        if(!scopeValid())return fail(budget.exhausted?refusal:Reason::ScopeChanged);
        // No refusable check occurs after the first bounded POD commit.
        std::memcpy(output.cars.data(),scratch.cars.data(),expected*sizeof(PhysicalCar));
        std::memcpy(output.sections.data(),scratch.sections.data(),result.observation.history.sections.size()*sizeof(rear::HistorySection));
        std::memcpy(output.portions.data(),scratch.portions.data(),result.portions*sizeof(rear::Portion));
        result.observation.history.sections={output.sections.data(),result.observation.history.sections.size()};
        result.publication={scope.witness.at,train,scope.witness.consistRevision,scope.kind,true,
            scope.kind==Kind::ProgressionQualified};
        if(output.publication)*output.publication=result.publication;
        result.cars=expected;result.history=result.observation.history.sections.size();result.reason=Reason::Ready;result.status=Status::Ready;
        // The internal geometric-history proof describes this retained body,
        // not progression between publications. A current-only caller receives
        // the owned fragments/metadata, never a generic lease-release input.
        if(scope.kind==Kind::CurrentPhysicalSnapshot)result.observation={};
        result.member=result.model=0;return result;
    }catch(...){return fail(Reason::ReaderException);}
}
template<class Read> Result capture(uintptr_t database,uintptr_t simulation,uint64_t train,
    bool profileQualified,const Scope& scope,Read&& reader,inventory::Budget& budget,
    Workspace scratch,Output output,Limits limits={})noexcept {
    return captureImpl(database,simulation,train,profileQualified,scope,std::forward<Read>(reader),budget,scratch,output,limits,nullptr);
}
template<class Read> Result captureWithInventory(uintptr_t database,uintptr_t simulation,uint64_t train,
    bool profileQualified,const Scope& scope,const InventoryView& roster,Read&& reader,inventory::Budget& budget,
    Workspace scratch,Output output,Limits limits={})noexcept {
    return captureImpl(database,simulation,train,profileQualified,scope,std::forward<Read>(reader),budget,scratch,output,limits,&roster);
}
} // namespace nimby::windows::consist_rear
