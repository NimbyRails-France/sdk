#pragma once
#include <platform/windows/runtime/native_consist_rear_provider.h>
#include <platform/windows/runtime/native_world_lineage.h>
#include <atomic>
#include <utility>

namespace nimby::windows::owned_rear {
namespace provider=nimby::windows::consist_rear;
namespace inventory=nimby::windows::consist_inventory;
namespace world=nimby::windows::native_world;
namespace rear=nimby::engine::native_rear;

// PRIVATE production adapter. The caller owns the REAL native exclusive lock
// after 349F9E ReleaseShared, supplies its current-thread accessor/lineage
// validator and holds that lock throughout capture AND the POD visitor.
// This type never acquires/releases a lock or calls native game functions.
struct Ownership {
    world::Capture capture;
    bool (*current)(void*,const world::Capture&) noexcept{};
    uint64_t (*currentThread)(void*) noexcept{};
    void* context{};
    bool valid()const noexcept {
        return capture&&capture.boundary==world::Boundary::PostSharedExclusive&&
            capture.controller>=0x10000&&capture.controller%8==0&&
            capture.controller<=std::numeric_limits<uintptr_t>::max()-0x5d0&&
            capture.context==capture.controller+0x5d0&&
            capture.database&&capture.simulation&&capture.thread&&current&&currentThread&&
            currentThread(context)==capture.thread&&current(context,capture);
    }
};
// Owned externally once per exclusive publication. No pool rescan per train.
// The caller must not reuse this memory or its token from another publication.
struct Roster {
    world::Capture capture;
    inventory::Result result;
    std::span<const inventory::Row> rows;
};
enum class Reason : uint8_t {
    Ready,UnqualifiedOwnership,StorageBusy,RosterUnknown,WrongPublication,
    BudgetRefused,CatalogueNotOwned,Unreadable,ReaderException,ProviderUnknown,
    OwnershipChanged,InvalidVisitor,InvalidLimits
};
struct Result {
    Reason reason=Reason::UnqualifiedOwnership;
    provider::Reason providerReason=provider::Reason::UnqualifiedScope;
    inventory::Reason inventoryReason=inventory::Reason::UnqualifiedProfile;
    rear::Reason coverageReason=rear::Reason::UnqualifiedHistory;
    provider::PublicationProof publication;
    uint64_t train{},member{},model{};
    size_t cars{},members{},sections{},portions{},reads{},bytes{},work{};
    bool ready()const noexcept {return reason==Reason::Ready&&publication.currentPhysicalQualified;}
    // Current body may be used for occupancy/conflict checks. It is deliberately
    // not a movement/progression witness and cannot authorize lease release.
    static constexpr bool progressionQualified=false;
    static constexpr bool leaseReleaseQualified=false;
};
static_assert(std::is_trivially_copyable_v<Result>);
struct Body {
    provider::PublicationProof publication;
    std::span<const provider::PhysicalCar> cars;
    std::span<const rear::HistorySection> sections;
    std::span<const rear::Portion> portions;
};
// Borrowed OWNED-POD spans during the visitor only; do not retain these spans.
// A visitor copies/processes POD only, no game/native callbacks, waits, logging
// formatting or mod callbacks while the native exclusive lock is held.
using Visitor=void (*)(void*,const Body&) noexcept;
struct Output {
    std::array<provider::PhysicalCar,4096> cars{};
    std::array<rear::HistorySection,4096> sections{};
    std::array<rear::Portion,4096> portions{};
    provider::PublicationProof publication;
};

// Allocate once at bridge bootstrap (static or bootstrap-owned allocation),
// never on the worker's stack. Workspace/output are disjoint fixed storage.
// Unknown never invokes the visitor. Failed attempts may alter private scratch
// or staged output, but cannot publish old/partial values as a current body.
class Storage {
    std::atomic_flag busy_=ATOMIC_FLAG_INIT;
    std::array<inventory::Row,1> unusedRows_{},unusedScratch_{};
    std::array<uint64_t,1> unusedIds_{};
    std::array<provider::detail::Member,64> members_{};
    std::array<provider::CarSetup,4096> expected_{},actual_{},temporary_{};
    std::array<provider::PhysicalCar,4096> cars_{};
    std::array<provider::NativeHistory,4096> history_{},historyAgain_{};
    std::array<provider::detail::Model,512> models_{};
    std::array<provider::detail::Track,512> tracks_{};
    std::array<uint64_t,16384> children_{},childrenAgain_{};
    std::array<rear::HistorySection,4096> sections_{};
    std::array<rear::Portion,4096> portions_{};
    Output output_;
    provider::Workspace workspace()noexcept {
        return {unusedRows_,unusedScratch_,unusedIds_,members_,expected_,actual_,temporary_,cars_,
            history_,historyAgain_,models_,tracks_,children_,childrenAgain_,sections_,portions_};
    }
    struct Binding {
        const Ownership& ownership;
        provider::Witness witness;
        static bool validate(void* opaque,const provider::Witness& witness)noexcept {
            const auto& binding=*static_cast<Binding*>(opaque);
            return witness==binding.witness&&binding.ownership.valid();
        }
    };
public:
    Storage()=default;
    Storage(const Storage&)=delete;Storage& operator=(const Storage&)=delete;
    template<class Read> Result capture(const Ownership& ownership,const Roster& roster,uint64_t train,
        bool profileQualified,Read&& reader,inventory::Budget& budget,Visitor visit,void* visitorContext,
        provider::Limits limits={})noexcept {
        Result out;out.train=train;out.inventoryReason=roster.result.reason;
        if(!profileQualified||!ownership.valid())return out;
        if(!visit){out.reason=Reason::InvalidVisitor;return out;}
        // The catalogue ownership read belongs to the SAME aggregate quotas
        // as the provider, in addition to the shared terminal preflight.
        if(limits.reads<2||limits.reads>8192||limits.bytes<=sizeof(uintptr_t)||
            limits.bytes>4u*1024u*1024u){out.reason=Reason::InvalidLimits;return out;}
        if(busy_.test_and_set(std::memory_order_acquire)){out.reason=Reason::StorageBusy;return out;}
        struct Release {std::atomic_flag& busy;~Release(){busy.clear(std::memory_order_release);}} release{busy_};
        output_.publication={};
        if(roster.capture!=ownership.capture){out.reason=Reason::WrongPublication;return out;}
        if(!roster.result.observedAllSlots()||roster.result.rows!=roster.rows.size()||roster.rows.size()>4096){
            out.reason=Reason::RosterUnknown;return out;
        }
        // Never invent a native tick from Sim time. These monotone labels come
        // from the real lifecycle Clock and the newly owned exclusive phase.
        const auto& token=ownership.capture;
        const rear::Publication publication{token.world,token.phase,token.phase,rear::Phase::RetainedMovement};
        const provider::Witness witness{token.database,token.simulation,publication,train,token.phase,token.phase};
        Binding binding{ownership,witness};
        const provider::Scope scope{witness,Binding::validate,&binding,provider::Kind::CurrentPhysicalSnapshot};
        const provider::InventoryView view{roster.rows,roster.result,witness};
        // The pinned constructor owns the catalogue inline at DB+A80. A
        // different pointer is not qualified by this controller's lock.
        if(!budget.available(sizeof(uintptr_t))){out.reason=Reason::BudgetRefused;return out;}
        uintptr_t catalogue{};++out.reads;out.bytes+=sizeof catalogue;
        try {
            if(!provider::detail::address(token.database,0xa80+0x48,true)||
                !reader(token.database+0x408,&catalogue,sizeof catalogue)){
                out.reason=Reason::Unreadable;return out;
            }
        }catch(...){out.reason=Reason::ReaderException;return out;}
        if(catalogue!=token.database+0xa80){out.reason=Reason::CatalogueNotOwned;return out;}
        --limits.reads;limits.bytes-=sizeof catalogue;
        const auto result=provider::captureWithInventory(token.database,token.simulation,train,true,scope,view,
            std::forward<Read>(reader),budget,workspace(),
            {output_.cars,output_.sections,output_.portions,&output_.publication},limits);
        out.providerReason=result.reason;out.coverageReason=result.coverageReason;
        out.inventoryReason=result.inventoryReason;out.member=result.member;out.model=result.model;
        out.reads+=result.reads;out.bytes+=result.bytes;out.work=result.work;out.members=result.members;
        if(!result.ready()){out.reason=Reason::ProviderUnknown;return out;}
        if(!ownership.valid()) {out.reason=Reason::OwnershipChanged;output_.publication={};return out;}
        // Result strips the generic historical Observation intentionally. Only
        // this current-body visitor exposes the owned fragments to the caller.
        if(!result.publication.currentPhysicalQualified||result.publication.progressionQualified||
            result.publication.kind!=provider::Kind::CurrentPhysicalSnapshot){out.reason=Reason::ProviderUnknown;return out;}
        out.publication=result.publication;out.cars=result.cars;out.sections=result.history;out.portions=result.portions;
        visit(visitorContext,{out.publication,{output_.cars.data(),out.cars},
            {output_.sections.data(),out.sections},{output_.portions.data(),out.portions}});
        out.reason=Reason::Ready;return out;
    }
};
} // namespace nimby::windows::owned_rear
