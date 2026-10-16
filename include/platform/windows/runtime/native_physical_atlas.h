#pragma once
#include <platform/windows/runtime/native_owned_rear_snapshot.h>
#include <bit>
#include <span>

// PRIVATE PROTOTYPE. No native call, lock acquisition, permission, continuity
// certification or active bridge integration. Inputs use the unchanged SDK
// owned inventory/current-body producers, never a caller-supplied list of bodies.
// Current-only qualification lives only while the SAME native ownership is
// held; never retain it after Root Exclusive is released. Clear means physical
// interval absence only, not a valid/existing track, matching aliases, a route
// permission or historical rear release. Construct Storage at bootstrap on
// static/owned heap storage, never on a worker stack.
namespace nimby::windows::physical_atlas {
namespace owned=nimby::windows::owned_rear;
namespace inv=nimby::windows::consist_inventory;
namespace provider=nimby::windows::consist_rear;
namespace world=nimby::windows::native_world;
namespace rear=nimby::engine::native_rear;

enum class Reason : uint8_t {
    Complete,UnqualifiedOwnership,Busy,InvalidLimits,InventoryUnknown,
    BodyUnknown,InvalidBody,IntervalCapacity,OwnershipChanged,BudgetRefused,
    StaleProof,InvalidQuery,InvalidRequester,Clear,Occupied,ArithmeticUnqualified
};
struct Interval {uint64_t track{},owner{};double from{},to{};};
struct Limits {
    inv::Limits inventory;
    provider::Limits body;
    size_t reads=16384,bytes=4u*1024u*1024u,preflights=262144;
};
template<size_t Rows,size_t Slots,size_t Intervals>class Storage;

// Only this collector can create the proof. A trusted-caller bool, repeated
// snapshot, elapsed time or a list of body fragments cannot construct it.
class CurrentProof {
    template<size_t,size_t,size_t>friend class Storage;
    world::Capture capture_{};
    uint64_t serial_{};
    const void* producer_{};
    CurrentProof(world::Capture capture,uint64_t serial,const void* producer)noexcept:
        capture_(capture),serial_(serial),producer_(producer){}
public:
    CurrentProof()=default;
    world::Capture capture()const noexcept{return capture_;}
    explicit operator bool()const noexcept{return capture_&&serial_&&producer_;}
    static constexpr bool historicalContinuityQualified=false;
    static constexpr bool leaseReleaseQualified=false;
    static constexpr bool permissionGranted=false;
};
struct Result {
    Reason reason=Reason::UnqualifiedOwnership;
    inv::Reason inventoryReason=inv::Reason::UnqualifiedOwnership;
    owned::Reason bodyReason=owned::Reason::UnqualifiedOwnership;
    provider::Reason providerReason=provider::Reason::UnqualifiedScope;
    uint64_t refusedRoot{},refusedMember{};
    size_t rows{},roots{},absentRoots{},cars{},intervals{},reads{},bytes{},preflights{};
    CurrentProof proof;
    bool complete()const noexcept{return reason==Reason::Complete&&bool(proof);}
};
struct Query {
    Reason reason=Reason::StaleProof;
    uint64_t conflictingRoot{};
    Interval overlap;
    bool known()const noexcept{return reason==Reason::Clear||reason==Reason::Occupied;}
    static constexpr bool permissionGranted=false;
    static constexpr bool leaseReleaseQualified=false;
};
// Caller-selected canonical CLOSED ranges, including any separately qualified
// crossing aliases. This type proves neither the existence of a track nor the
// completeness/freshness of a route or its aliases. Input must remain immutable
// during the copy; no native/mod callback is made by the collector.
struct ProtectedSpan {uint64_t track{};double begin{},end{};};
inline constexpr size_t maximumProtectedSpans=512;
struct QueryLimits {size_t spans=maximumProtectedSpans,operations=65536;};
struct ProtectedQuery {
    Reason reason=Reason::StaleProof;
    world::Capture capture;
    rear::Publication at;
    uint64_t conflictingRoot{};
    Interval overlap;
    size_t requestedSpans{},validatedSpans{},examinedSpans{},work{};
    size_t spanIndex=std::numeric_limits<size_t>::max();
    bool known()const noexcept{return reason==Reason::Clear||reason==Reason::Occupied;}
    static constexpr bool routeQualified=false,aliasesQualified=false;
    static constexpr bool permissionGranted=false,leaseReleaseQualified=false;
    static constexpr bool historicalContinuityQualified=false;
};

// Bootstrap storage only: construct once outside native workers. A capture
// invalidates the previous publication before any read. The gate prevents a
// reader from seeing a staged prefix; failures leave the atlas entirely Unknown.
// Default capacities are structural ceilings, NOT a 5,000-train performance
// promise. Actual pool scans/body captures must also fit local/global budgets.
template<size_t Rows=1024,size_t Slots=8192,size_t Intervals=16384>
class Storage {
    static_assert(Rows&&Rows<=4096&&Slots&&Slots<=65536&&Intervals&&Intervals<=65536);
    std::atomic_flag busy_=ATOMIC_FLAG_INIT;
    owned::Storage bodies_;
    std::array<inv::Row,Rows> rows_{},scratch_{};
    std::array<uint64_t,Slots> ids_{};
    std::array<Interval,Intervals> intervals_{};
    std::array<double,Intervals> prefixMaximumTo_{};
    // Bootstrap-owned packet scratch. A grouped query never allocates, places
    // a large packet on the worker stack or rereads an already validated span.
    std::array<ProtectedSpan,maximumProtectedSpans> protected_{};
    owned::Ownership owner_{};
    inv::Budget* budgetIdentity_{};
    world::Capture publication_{};
    size_t rowCount_{},intervalCount_{},reads_{},bytes_{},preflights_{};
    uint64_t serial_{};
    bool ready_=false;
    Limits limits_{};
    struct Release {std::atomic_flag& gate;~Release(){gate.clear(std::memory_order_release);}};
    struct Shared {
        Storage& storage;inv::Budget& upstream;
        static bool check(void* opaque,size_t bytes)noexcept{
            auto& x=*static_cast<Shared*>(opaque);auto& s=x.storage;
            // Refusal is terminal across inventory, EVERY body and queries.
            if(x.upstream.exhausted)return false;
            if(!rear::detail::arithmeticQualified()){x.upstream.exhausted=true;return false;}
            if(s.preflights_==s.limits_.preflights||
               (bytes&&(s.reads_==s.limits_.reads||bytes>s.limits_.bytes-s.bytes_))){
                x.upstream.exhausted=true;return false;
            }
            if(!x.upstream.available(bytes))return false;
            if(!rear::detail::arithmeticQualified()){x.upstream.exhausted=true;return false;}
            ++s.preflights_;if(bytes){++s.reads_;s.bytes_+=bytes;}return true;
        }
    };
    static bool validate(void* opaque)noexcept{return static_cast<const owned::Ownership*>(opaque)->valid();}
    static bool fraction(double value)noexcept{
        // Integer classification cannot raise a denormal/sNaN FP exception.
        // Subnormals are deliberately unqualified, even with DAZ currently off.
        const auto bits=std::bit_cast<uint64_t>(value);
        const auto exponent=bits&UINT64_C(0x7ff0000000000000);
        const auto mantissa=bits&UINT64_C(0x000fffffffffffff);
        return exponent!=UINT64_C(0x7ff0000000000000)&&!(exponent==0&&mantissa)&&
            rear::detail::fraction(value);
    }
    static bool earlier(const Interval& a,const Interval& b)noexcept{
        if(a.track!=b.track)return a.track<b.track;
        if(a.from!=b.from)return a.from<b.from;
        if(a.to!=b.to)return a.to<b.to;
        return a.owner<b.owner;
    }
    Reason qualifyQuery(const CurrentProof& proof,inv::Budget& shared)noexcept{
        if(!ready_||!proof||proof.producer_!=this||proof.serial_!=serial_||proof.capture_!=publication_||
           &shared!=budgetIdentity_||owner_.capture!=publication_)return Reason::StaleProof;
        if(!owner_.valid()){ready_=false;return Reason::OwnershipChanged;}
        if(!rear::detail::arithmeticQualified()){ready_=false;return Reason::ArithmeticUnqualified;}
        return Reason::Complete;
    }
    Reason requester(uint64_t root,inv::Budget& budget)const noexcept{
        size_t low=0,high=rowCount_;const auto index=inv::detail::index(root);
        while(low<high){
            if(!budget.available())return Reason::BudgetRefused;
            const auto mid=low+(high-low)/2;
            if(inv::detail::index(rows_[mid].train)<index)low=mid+1;else high=mid;
        }
        return low==rowCount_||rows_[low].train!=root||rows_[low].attached?
            Reason::InvalidRequester:Reason::Complete;
    }
    // Called only under the gate after input/requester qualification. Budget
    // and final ownership checks belong to the caller, including on Occupied.
    Query overlap(uint64_t root,uint64_t track,double begin,double end,inv::Budget& budget)const noexcept{
        Query out;size_t low=0,high=intervalCount_;
        while(low<high){
            if(!budget.available()){out.reason=Reason::BudgetRefused;return out;}
            const auto mid=low+(high-low)/2;if(intervals_[mid].track<track)low=mid+1;else high=mid;
        }
        // Prefix maxima preserve closed endpoints and earlier long bodies
        // across an empty gap, without turning independent bodies into a hull.
        const auto first=low;high=intervalCount_;
        while(low<high){
            if(!budget.available()){out.reason=Reason::BudgetRefused;return out;}
            const auto mid=low+(high-low)/2;
            if(intervals_[mid].track==track&&intervals_[mid].from<=end)low=mid+1;else high=mid;
        }
        out.reason=Reason::Clear;
        for(size_t n=low;n>first;){
            if(!budget.available()){out.reason=Reason::BudgetRefused;return out;}
            --n;if(prefixMaximumTo_[n]<begin)break;
            const auto& part=intervals_[n];
            if(part.owner!=root&&part.to>=begin){
                out.reason=Reason::Occupied;out.conflictingRoot=part.owner;out.overlap=part;break;
            }
        }
        return out;
    }
    bool sort(inv::Budget& budget)noexcept{
        // Bounded in-place heapsort; no comparator allocation or global lock.
        const auto sift=[&](size_t start,size_t count){
            size_t n=start;
            while(n<count/2){
                if(!budget.available())return false;
                size_t child=n*2+1;
                if(child+1<count&&earlier(intervals_[child],intervals_[child+1]))++child;
                if(!earlier(intervals_[n],intervals_[child]))break;
                std::swap(intervals_[n],intervals_[child]);n=child;
            }
            return true;
        };
        for(size_t n=intervalCount_/2;n;){if(!sift(--n,intervalCount_))return false;}
        for(size_t count=intervalCount_;count>1;){
            if(!budget.available())return false;
            std::swap(intervals_[0],intervals_[--count]);if(!sift(0,count))return false;
        }
        for(size_t n=0;n<intervalCount_;++n){
            if(!budget.available())return false;
            prefixMaximumTo_[n]=!n||intervals_[n-1].track!=intervals_[n].track?
                intervals_[n].to:std::max(prefixMaximumTo_[n-1],intervals_[n].to);
        }
        return true;
    }
    struct Visit {
        Storage& storage;inv::Budget& budget;uint64_t root{};size_t rootRow{};
        bool called=false,absent=false;Reason reason=Reason::InvalidBody;
        size_t cars{};uint64_t refusedMember{};
        static void copy(void* opaque,const owned::Body& body)noexcept{
            auto& v=*static_cast<Visit*>(opaque);auto& s=v.storage;v.called=true;
            const auto& p=body.publication;
            const rear::Publication expected{s.owner_.capture.world,s.owner_.capture.phase,
                s.owner_.capture.phase,rear::Phase::RetainedMovement};
            if(!s.owner_.valid()||!rear::detail::arithmeticQualified()||p.at!=expected||p.train!=v.root||
               p.consistRevision!=s.owner_.capture.phase||
               p.kind!=provider::Kind::CurrentPhysicalSnapshot||!p.currentPhysicalQualified||p.progressionQualified)return;
            if(p.physicallyAbsent){
                if(!body.cars.empty()||!body.sections.empty()||!body.portions.empty())return;
                // Positive root absence is insufficient if any attached
                // member still has its own native body. Never omit that row
                // just because its parent is inactive. Use the actual owned
                // inverse index, not a guessed parent-to-child native list.
                if(v.rootRow>=s.rowCount_)return;
                const auto& root=s.rows_[v.rootRow];
                if(root.train!=v.root||root.attached||root.active)return;
                size_t previous=s.rowCount_,seen=0;
                for(auto n=root.firstChild;n!=inv::noRow;n=s.rows_[n].nextSibling){
                    if(!v.budget.available()){v.reason=Reason::BudgetRefused;return;}
                    if(n>=s.rowCount_||n>=previous||n==v.rootRow)return;
                    const auto& child=s.rows_[n];previous=n;
                    if(!child.attached||child.parentRow!=v.rootRow||child.parent!=v.root||
                       !provider::detail::identity(child.train,5)||++seen>root.childCount)return;
                    if(child.active){v.refusedMember=child.train;return;}
                }
                if(seen!=root.childCount)return;
                v.absent=true;v.reason=Reason::Complete;return;
            }
            // A generic empty body is never positive absence/clearance.
            if(body.cars.empty()||body.sections.empty()||body.portions.empty())return;
            for(const auto& part:body.portions){
                if(!v.budget.available()){v.reason=Reason::BudgetRefused;return;}
                if(part.train!=v.root||!provider::detail::identity(part.track,1)||
                   !fraction(part.from)||!fraction(part.to)||
                   !std::isfinite(part.lengthMeters)||part.lengthMeters<0||part.orientation>2)return;
                if(s.intervalCount_==Intervals){v.reason=Reason::IntervalCapacity;return;}
                // Canonical fractions retain CLOSED endpoints, including a
                // zero-length point. Direction does not alter conflict tests.
                s.intervals_[s.intervalCount_++]={part.track,v.root,
                    std::min(part.from,part.to),std::max(part.from,part.to)};
            }
            v.cars=body.cars.size();v.reason=Reason::Complete;
        }
    };
public:
    Storage()=default;Storage(const Storage&)=delete;Storage& operator=(const Storage&)=delete;
    template<class Read>Result capture(const owned::Ownership& ownership,bool profileQualified,
        Read&& reader,inv::Budget& shared,Limits limits={})noexcept{
    Result out;
        if(busy_.test_and_set(std::memory_order_acquire)){out.reason=Reason::Busy;return out;}
        Release release{busy_};ready_=false;publication_={};budgetIdentity_=nullptr;
        rowCount_=intervalCount_=reads_=bytes_=preflights_=0;owner_=ownership;limits_=limits;
        const auto fail=[&](Reason why){out.reason=why;out.proof={};out.reads=reads_;out.bytes=bytes_;out.preflights=preflights_;return out;};
        if(!profileQualified||!owner_.valid())return fail(Reason::UnqualifiedOwnership);
        if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
        if(!limits.reads||!limits.bytes||!limits.preflights||!shared.preflight||serial_==UINT64_MAX)
            return fail(Reason::InvalidLimits);
        Shared accounting{*this,shared};inv::Budget budget{Shared::check,&accounting};
        const inv::OwnedScope scope{validate,&owner_};
        const auto inventory=inv::captureMotionInventoryOwned(owner_.capture.simulation,true,scope,
            reader,budget,rows_,{scratch_,ids_},limits.inventory);
        if(budget.exhausted)shared.exhausted=true;
        out.inventoryReason=inventory.reason;
        if(!inventory.observedAllSlots()||!inventory.relationshipsIndexed||!inventory.ownedPublication)
            return fail(shared.exhausted?Reason::BudgetRefused:Reason::InventoryUnknown);
        rowCount_=inventory.rows;out.rows=rowCount_;
        if(!owner_.valid())return fail(Reason::OwnershipChanged);
        const owned::Roster roster{owner_.capture,inventory,{rows_.data(),rowCount_}};
        for(size_t n=0;n<rowCount_;++n){
            if(!budget.available())return fail(Reason::BudgetRefused);
            const auto& row=rows_[n];if(row.attached)continue;
            // ALL roots, including vanilla/unscheduled/stopped/inactive ones.
            // The actual provider decides positive absence, never this loop.
            out.refusedRoot=row.train;Visit visit{*this,budget,row.train,n};
            const auto body=bodies_.capture(owner_,roster,row.train,true,reader,budget,Visit::copy,&visit,limits.body);
            if(budget.exhausted)shared.exhausted=true;
            out.bodyReason=body.reason;out.providerReason=body.providerReason;
            out.refusedMember=body.member?body.member:visit.refusedMember;
            if(!body.ready())return fail(shared.exhausted?Reason::BudgetRefused:Reason::BodyUnknown);
            if(!visit.called||visit.reason!=Reason::Complete)return fail(visit.reason);
            if(!owner_.valid())return fail(Reason::OwnershipChanged);
            ++out.roots;if(visit.absent)++out.absentRoots;out.cars+=visit.cars;
        }
        if(!sort(budget)||!budget.available())return fail(Reason::BudgetRefused);
        if(!owner_.valid())return fail(Reason::OwnershipChanged);
        if(!rear::detail::arithmeticQualified())return fail(Reason::ArithmeticUnqualified);
        // Single final POD publication, no read/check after ready becomes true.
        publication_=owner_.capture;budgetIdentity_=&shared;ready_=true;++serial_;
        out.reason=Reason::Complete;out.refusedRoot=out.refusedMember=0;out.intervals=intervalCount_;
        out.proof=CurrentProof(publication_,serial_,this);
        out.reads=reads_;out.bytes=bytes_;out.preflights=preflights_;return out;
    }
    Query query(const CurrentProof& proof,uint64_t requesterRoot,uint64_t track,
        double from,double to,inv::Budget& shared)noexcept{
        Query out;
        if(busy_.test_and_set(std::memory_order_acquire)){out.reason=Reason::Busy;return out;}
        Release release{busy_};
        if(const auto why=qualifyQuery(proof,shared);why!=Reason::Complete){out.reason=why;return out;}
        if(!provider::detail::identity(track,1)||!provider::detail::identity(requesterRoot,5)||
           !fraction(from)||!fraction(to)){out.reason=Reason::InvalidQuery;return out;}
        Shared accounting{*this,shared};inv::Budget budget{Shared::check,&accounting};
        const auto denied=[&]{ready_=false;out={};out.reason=Reason::BudgetRefused;return out;};
        if(const auto why=requester(requesterRoot,budget);why!=Reason::Complete){
            if(why==Reason::BudgetRefused)return denied();
            out.reason=why;return out;
        }
        const auto begin=std::min(from,to),end=std::max(from,to);
        out=overlap(requesterRoot,track,begin,end,budget);
        if(out.reason==Reason::BudgetRefused)return denied();
        if(!budget.available())return denied();
        if(!owner_.valid()){out={};out.reason=Reason::OwnershipChanged;ready_=false;}
        else if(!rear::detail::arithmeticQualified()){out={};out.reason=Reason::ArithmeticUnqualified;ready_=false;}
        return out;
    }
    // Evaluate the ENTIRE caller-selected packet against ONE current atlas.
    // Validate/copy every range before testing overlaps: an early conflict
    // must not hide an invalid suffix. Same gate/owner/budget stays held for
    // the packet, and even Occupied is revalidated before returning.
    // Clear describes only these ranges, never complete route/alias coverage.
    ProtectedQuery queryProtected(const CurrentProof& proof,uint64_t requesterRoot,
        std::span<const ProtectedSpan> spans,inv::Budget& shared,QueryLimits limits={})noexcept{
        ProtectedQuery out;out.requestedSpans=spans.size();
        if(busy_.test_and_set(std::memory_order_acquire)){out.reason=Reason::Busy;return out;}
        Release release{busy_};
        if(const auto why=qualifyQuery(proof,shared);why!=Reason::Complete){out.reason=why;return out;}
        if(!limits.spans||limits.spans>maximumProtectedSpans||!limits.operations||limits.operations>262144){
            out.reason=Reason::InvalidLimits;return out;
        }
        if(!provider::detail::identity(requesterRoot,5)||spans.empty()||!spans.data()||spans.size()>limits.spans){
            out.reason=Reason::InvalidQuery;return out;
        }
        Shared accounting{*this,shared};inv::Budget aggregate{Shared::check,&accounting};
        struct Work {
            inv::Budget& aggregate;inv::Budget& shared;size_t limit,used{};
            static bool check(void* opaque,size_t bytes)noexcept{
                auto& w=*static_cast<Work*>(opaque);
                if(w.shared.exhausted||w.aggregate.exhausted)return false;
                if(w.used==w.limit){w.shared.exhausted=w.aggregate.exhausted=true;return false;}
                ++w.used;return w.aggregate.available(bytes);
            }
        } work{aggregate,shared,limits.operations};
        inv::Budget budget{Work::check,&work};
        const auto fail=[&](Reason why){
            out.reason=why;out.work=work.used;out.capture={};out.at={};out.conflictingRoot=0;out.overlap={};
            if(why==Reason::BudgetRefused){shared.exhausted=true;ready_=false;}
            return out;
        };
        if(const auto why=requester(requesterRoot,budget);why!=Reason::Complete)return fail(why);
        for(size_t n=0;n<spans.size();++n){
            if(!budget.available())return fail(Reason::BudgetRefused);
            const auto span=spans[n];
            if(!provider::detail::identity(span.track,1)||!fraction(span.begin)||!fraction(span.end)||span.begin>span.end){
                out.spanIndex=n;return fail(Reason::InvalidQuery);
            }
            protected_[n]=span;++out.validatedSpans;
        }
        out.reason=Reason::Clear;
        for(size_t n=0;n<spans.size();++n){
            if(!budget.available())return fail(Reason::BudgetRefused);
            const auto& span=protected_[n];
            const auto q=overlap(requesterRoot,span.track,span.begin,span.end,budget);++out.examinedSpans;
            if(!q.known())return fail(q.reason);
            if(q.reason==Reason::Occupied){
                out.reason=Reason::Occupied;out.spanIndex=n;
                out.conflictingRoot=q.conflictingRoot;out.overlap=q.overlap;break;
            }
        }
        if(!budget.available())return fail(Reason::BudgetRefused);
        if(!owner_.valid()){ready_=false;return fail(Reason::OwnershipChanged);}
        if(!rear::detail::arithmeticQualified()){ready_=false;return fail(Reason::ArithmeticUnqualified);}
        out.work=work.used;out.capture=publication_;
        // Never relabel this post-movement/current-only result BeforeMovement.
        out.at={publication_.world,publication_.phase,publication_.phase,rear::Phase::RetainedMovement};
        return out;
    }
};
} // namespace nimby::windows::physical_atlas
