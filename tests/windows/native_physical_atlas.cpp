#include "native_physical_atlas_fixture.h"
#include "allocation_fault_fixture.h"

namespace fixture {
namespace atlas=nimby::windows::physical_atlas;
struct SharedBudget {
    Memory& memory;size_t calls{},reads{},bytes{},deny{};bool refused=false;
    void reset()noexcept{calls=reads=bytes=deny=0;refused=false;}
    static bool available(void* opaque,size_t bytes)noexcept{
        auto& x=*static_cast<SharedBudget*>(opaque);
        CHECK(!x.refused);++x.calls;
        if(x.calls==x.deny){x.refused=true;x.memory.gateRefused=true;return false;}
        if(bytes){++x.reads;x.bytes+=bytes;}return true;
    }
};
using Atlas=atlas::Storage<16,512,16>;
static_assert(!std::is_constructible_v<atlas::CurrentProof,world::Capture,uint64_t,const void*>);
static Atlas storage;
static Memory memory;
static Owner owner;
static atlas::Storage<16,512,1> small;

void head(size_t member,size_t rail,double fraction,int8_t direction=1,int8_t orientation=0){
    const auto motion=motionBlock+member*0x638;
    const auto metric=memory.value<double>(trackBlock+rail*0x4e8+0x88);
    const nimby::windows::automatic::CommitPresence value{0,0,track(rail),fraction,direction,orientation,{},fraction*metric};
    memory.write(motion+0x3a0,value);memory.write(motion+0xb8,track(rail));memory.write(motion+0xc0,fraction);
    memory.write(motion+0xc8,direction);memory.write(motion+0xc9,orientation);
}
// 5 native slots: active root0 + TWO inactive attached members, another
// vanilla/unscheduled stationary root3, and positively absent unattached4.
// Inverse child index is genuinely produced as decreasing rows 2 -> 1.
void multiple(){
    memory.reset();memory.member(0,1);memory.member(1,1,train(0),1,false);memory.member(2,1,train(0),2,false);
    std::array<CarSetup,3> full{};
    for(size_t n=0;n<3;++n)full[n]=memory.value<CarSetup>(retained+n*carCapacity*32);
    memory.raw(retained,full.data(),sizeof full);memory.vector(motionBlock+8,retained,3,32);memory.write(motionBlock+0x40,float(60));
    memory.trackData(0,0,0,1000);head(0,0,.75);
    memory.member(3,1);memory.trackData(1);head(3,1,.75,-1);
    memory.member(4,1);memory.write(motionBlock+4*0x638+0x1d0,uint8_t(0));
    // Teardown leaves poisoned history/presence. Positive absence must avoid it.
    memory.write(motionBlock+4*0x638+0xe0,provider::detail::Vector{0xdeadbeef,UINTPTR_MAX,UINTPTR_MAX});
    memory.write(motionBlock+4*0x638+0x3a0,nimby::windows::automatic::CommitPresence{});
    owner.begin();
}
template<class S>atlas::Result collect(S& into,inventory::Budget& budget,atlas::Limits limits={}){
    auto result=into.capture(owner.scope(),true,
        [&](uintptr_t at,void* out,size_t bytes){
            // root4 is inactive+unattached in this fixture. Its stale history
            // and presence must never be read to certify positive absence.
            const auto motion=motionBlock+4*0x638;
            CHECK(!(at>=motion+0xe0&&at<motion+0xf8));
            CHECK(at!=motion+0x3a0);return memory.read(at,out,bytes);
        },budget,limits);
    CHECK(!memory.readAfterRefusal);return result;
}
void checkProof(const atlas::Result& result){
    CHECK(result.complete());CHECK(result.proof.capture()==owner.token);
    CHECK(!atlas::CurrentProof::historicalContinuityQualified);
    CHECK(!atlas::CurrentProof::leaseReleaseQualified);CHECK(!atlas::CurrentProof::permissionGranted);
    CHECK(!atlas::Query::permissionGranted&&!atlas::Query::leaseReleaseQualified);
}
void protectedQueries(){
    using Span=atlas::ProtectedSpan;
    static_assert(!atlas::ProtectedQuery::permissionGranted&&!atlas::ProtectedQuery::leaseReleaseQualified);
    static_assert(!atlas::ProtectedQuery::routeQualified&&!atlas::ProtectedQuery::aliasesQualified);
    static_assert(!atlas::ProtectedQuery::historicalContinuityQualified);
    SharedBudget shared{memory};inventory::Budget budget{SharedBudget::available,&shared};
    const auto fresh=[&]{
        multiple();shared.reset();budget={SharedBudget::available,&shared};
        const auto result=collect(storage,budget);checkProof(result);return result;
    };
    auto result=fresh();
    const std::array<Span,3> spans{{{track(0),0,1},{track(1),0,.1},{track(1),.8,.81}}};
    const auto reads=memory.reads,bytes=memory.bytesRead;
    auto q=storage.queryProtected(result.proof,train(0),spans,budget);
    CHECK(q.reason==atlas::Reason::Occupied&&q.known()&&q.conflictingRoot==train(3));
    CHECK(q.requestedSpans==3&&q.validatedSpans==3&&q.examinedSpans==3&&q.spanIndex==2&&q.work>3);
    CHECK(q.capture==owner.token&&q.at.session==owner.token.world&&q.at.epoch==owner.token.phase);
    CHECK(q.at.tick==owner.token.phase&&q.at.phase==rear::Phase::RetainedMovement);
    CHECK(memory.reads==reads&&memory.bytesRead==bytes);
    const auto overlap=q.overlap;
    // Closed zero-length contact, exact root aggregate exclusion and a missing
    // full track generation retain the scalar API's physical-only semantics.
    std::array<Span,1> point{{{track(1),overlap.to,overlap.to}}};
    q=storage.queryProtected(result.proof,train(0),point,budget);CHECK(q.reason==atlas::Reason::Occupied);
    point[0]={track(0),0,1};q=storage.queryProtected(result.proof,train(0),point,budget);CHECK(q.reason==atlas::Reason::Clear);
    point[0]={track(1,2),0,1};q=storage.queryProtected(result.proof,train(0),point,budget);CHECK(q.reason==atlas::Reason::Clear);
    // These unsorted/overlapping caller aliases are ranges to test, never an
    // atlas-produced claim of a complete route. Do not merge away their gaps.
    const std::array<Span,3> aliases{{{track(1),0,.1},{track(0),.9,1},{track(1),.05,.2}}};
    q=storage.queryProtected(result.proof,train(0),aliases,budget);
    CHECK(q.reason==atlas::Reason::Clear&&q.validatedSpans==3&&q.examinedSpans==3);
    // A conflict in the first range cannot mask ANY malformed later range.
    // The entire packet is validated before the first occupancy lookup.
    const double nan=std::numeric_limits<double>::quiet_NaN();
    const double signallingNan=std::bit_cast<double>(UINT64_C(0x7ff0000000000001));
    const double infinity=std::numeric_limits<double>::infinity();
    const double denormal=std::numeric_limits<double>::denorm_min();
    const std::array<Span,15> invalid{{
        {uint64_t(1)<<48,0,1},{train(0),0,1},{track(1),nan,1},{track(1),0,nan},
        {track(1),infinity,1},{track(1),0,infinity},{track(1),-1,1},{track(1),0,1.01},
        {track(1),.9,.8},{track(1),denormal,1},{track(1),0,denormal},
        {track(1),-denormal,1},{track(1),0,-denormal},
        {track(1),signallingNan,1},{track(1),0,signallingNan}}};
    for(const auto& bad:invalid){
        const std::array<Span,2> packet{{{track(1),.8,.81},bad}};
        q=storage.queryProtected(result.proof,train(0),packet,budget);
        CHECK(!q.known()&&q.reason==atlas::Reason::InvalidQuery&&q.spanIndex==1);
        CHECK(q.requestedSpans==2&&q.validatedSpans==1&&q.examinedSpans==0);
        CHECK(!q.capture&&q.at.phase==rear::Phase::Unknown&&q.conflictingRoot==0&&q.overlap.track==0);
    }
    q=storage.queryProtected(result.proof,train(1),spans,budget);CHECK(q.reason==atlas::Reason::InvalidRequester);
    q=storage.queryProtected(result.proof,train(0,2),spans,budget);CHECK(q.reason==atlas::Reason::InvalidRequester);
    q=storage.queryProtected(result.proof,uint64_t(5)<<48,spans,budget);CHECK(!q.known());
    q=storage.queryProtected(result.proof,train(0),{},budget);CHECK(q.reason==atlas::Reason::InvalidQuery);
    std::array<Span,atlas::maximumProtectedSpans+1> tooMany{};
    q=storage.queryProtected(result.proof,train(0),tooMany,budget);CHECK(q.reason==atlas::Reason::InvalidQuery&&q.validatedSpans==0);
    for(const auto limits:std::array<atlas::QueryLimits,4>{{{0,1},{513,1},{1,0},{1,262145}}}){
        q=storage.queryProtected(result.proof,train(0),spans,budget,limits);CHECK(q.reason==atlas::Reason::InvalidLimits);
    }
    q=storage.queryProtected(result.proof,train(0),spans,budget,{2,65536});CHECK(q.reason==atlas::Reason::InvalidQuery);
    // The hard 512-range ceiling is usable, including repeated ranges; there
    // is no allocation or per-range native reread and no invented route proof.
    std::array<Span,atlas::maximumProtectedSpans> maximum{};
    maximum.fill({track(1),0,.1});
    q=storage.queryProtected(result.proof,train(0),maximum,budget);
    CHECK(q.reason==atlas::Reason::Clear&&q.validatedSpans==maximum.size()&&q.examinedSpans==maximum.size());
    CHECK(memory.reads==reads&&memory.bytesRead==bytes);
    inventory::Budget other{SharedBudget::available,&shared};
    q=storage.queryProtected(result.proof,train(0),spans,other);CHECK(q.reason==atlas::Reason::StaleProof);
    q=small.queryProtected(result.proof,train(0),spans,budget);CHECK(q.reason==atlas::Reason::StaleProof);
    q=storage.queryProtected({},train(0),spans,budget);CHECK(q.reason==atlas::Reason::StaleProof);
    const auto old=result.proof;result=fresh();
    q=storage.queryProtected(old,train(0),spans,budget);CHECK(q.reason==atlas::Reason::StaleProof);
    // A local or aggregate quota refuses the WHOLE result, including the final
    // preflight after an already found conflict. All refusals remain terminal.
    q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(q.reason==atlas::Reason::Occupied);
    const auto operations=q.work;
    for(size_t limit=1;limit<operations;++limit){
        result=fresh();q=storage.queryProtected(result.proof,train(0),spans,budget,{512,limit});
        CHECK(q.reason==atlas::Reason::BudgetRefused&&!q.known()&&budget.exhausted&&q.work==limit);
        CHECK(q.conflictingRoot==0&&q.overlap.track==0&&!q.capture);
        const auto calls=shared.calls;
        q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(!q.known()&&shared.calls==calls);
    }
    result=fresh();q=storage.queryProtected(result.proof,train(0),spans,budget,{512,operations});CHECK(q.reason==atlas::Reason::Occupied);
    for(size_t refusal=1;refusal<=operations;++refusal){
        result=fresh();shared.deny=shared.calls+refusal;
        q=storage.queryProtected(result.proof,train(0),spans,budget);
        CHECK(q.reason==atlas::Reason::BudgetRefused&&!q.known()&&budget.exhausted);
        CHECK(q.conflictingRoot==0&&q.overlap.track==0&&!q.capture);
        const auto calls=shared.calls;shared.deny=0;
        q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(!q.known()&&shared.calls==calls);
    }
    // Ownership is checked after the grouped overlap lookup too. A lost token
    // cannot renew itself, even if a later current() call would return true.
    result=fresh();owner.deny=owner.calls+2;
    q=storage.queryProtected(result.proof,train(0),spans,budget);
    CHECK(q.reason==atlas::Reason::OwnershipChanged&&!q.known()&&q.conflictingRoot==0);
    owner.deny=0;q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(!q.known());
    result=fresh();CHECK(owner.line.endCapture(owner.token));
    q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(q.reason==atlas::Reason::OwnershipChanged);
    result=fresh();owner.thread=9;q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(!q.known());
    // Read-only FP qualification remains mandatory for the entire packet.
    for(unsigned mode:{0x0040u,0x8000u,0x2000u,0x4000u}){
        result=fresh();const auto controls=_mm_getcsr();_mm_setcsr(controls|mode);
        q=storage.queryProtected(result.proof,train(0),spans,budget);
        const auto actual=_mm_getcsr();_mm_setcsr(controls);
        CHECK(q.reason==atlas::Reason::ArithmeticUnqualified&&!q.known()&&actual==(controls|mode));
        q=storage.queryProtected(result.proof,train(0),spans,budget);CHECK(!q.known());
    }
    // Reentrant calls from the trusted preflight cannot access packet scratch
    // or retire the outer publication while its gate is held.
    result=fresh();
    struct Reentrant {
        SharedBudget& shared;const atlas::CurrentProof& proof;inventory::Budget& budget;
        std::span<const Span> spans;bool called=false;atlas::Reason query{},capture{};
        static bool check(void* opaque,size_t bytes)noexcept{
            auto& x=*static_cast<Reentrant*>(opaque);
            if(!x.called){
                x.called=true;x.query=storage.queryProtected(x.proof,train(0),x.spans,x.budget).reason;
                x.capture=collect(storage,x.budget).reason;
            }
            return SharedBudget::available(&x.shared,bytes);
        }
    } reentrant{shared,result.proof,budget,spans};
    budget.preflight=Reentrant::check;budget.context=&reentrant;
    q=storage.queryProtected(result.proof,train(0),spans,budget);
    CHECK(q.reason==atlas::Reason::Occupied&&reentrant.called);
    CHECK(reentrant.query==atlas::Reason::Busy&&reentrant.capture==atlas::Reason::Busy);
    // Zero allocation attempts under persistent failure, including 512 clear
    // spans and a whole invalid suffix, uses only bootstrap-owned packet POD.
    result=fresh();const std::array<Span,2> malformed{{{track(1),.8,.81},{track(1),nan,1}}};
    allocation_fault_fixture::arm(0,UINT_MAX);
    q=storage.queryProtected(result.proof,train(0),maximum,budget);
    const auto bad=storage.queryProtected(result.proof,train(0),malformed,budget);
    const auto oom=allocation_fault_fixture::stop();
    CHECK(q.reason==atlas::Reason::Clear&&bad.reason==atlas::Reason::InvalidQuery);
    CHECK(!oom.allocations&&!oom.failures);
    std::printf("protected-spans: %zu ranges maximum, %zu nominal operations, no permission/rear release; %zu total checks PASS\n",
        maximum.size(),operations,checks);
}
void tests(){
    multiple();SharedBudget shared{memory};inventory::Budget budget{SharedBudget::available,&shared};
    auto result=collect(storage,budget);checkProof(result);
    CHECK(result.rows==5&&result.roots==3&&result.absentRoots==1&&result.cars==4&&result.intervals==2);
    CHECK(result.reads==memory.reads&&result.reads==shared.reads);
    CHECK(result.bytes==memory.bytesRead&&result.bytes==shared.bytes);
    // Exact aggregate excludes all attached cars as root0, no child owner leak.
    auto q=storage.query(result.proof,train(0),track(0),0,1,budget);CHECK(q.reason==atlas::Reason::Clear);
    q=storage.query(result.proof,train(3),track(0),.70,.71,budget);
    CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(0));
    CHECK(q.overlap.owner==train(0)&&q.overlap.track==track(0));
    CHECK(q.overlap.from<.70&&q.overlap.to==.75);
    // Native reverse body canonicalizes .75 -> >.95 without changing endpoints.
    q=storage.query(result.proof,train(0),track(1),.8,.81,budget);
    CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(3));
    const auto begin=q.overlap.from,end=q.overlap.to;
    CHECK(begin==.75&&end>.95);
    q=storage.query(result.proof,train(0),track(1),begin,begin,budget);CHECK(q.reason==atlas::Reason::Occupied);
    q=storage.query(result.proof,train(0),track(1),end,end,budget);CHECK(q.reason==atlas::Reason::Occupied);
    q=storage.query(result.proof,train(0),track(1),1,std::nextafter(end,1.0),budget);CHECK(q.reason==atlas::Reason::Clear);
    q=storage.query(result.proof,train(4),track(1),.8,.81,budget);CHECK(q.reason==atlas::Reason::Occupied);
    q=storage.query(result.proof,train(1),track(0),.7,.71,budget);CHECK(q.reason==atlas::Reason::InvalidRequester);
    q=storage.query(result.proof,train(3,2),track(1),.8,.81,budget);CHECK(q.reason==atlas::Reason::InvalidRequester);
    q=storage.query(result.proof,train(0),track(1,2),.8,.81,budget);CHECK(q.reason==atlas::Reason::Clear);
    q=storage.query({},train(0),track(1),.8,.81,budget);CHECK(q.reason==atlas::Reason::StaleProof);
    q=small.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(q.reason==atlas::Reason::StaleProof);
    inventory::Budget foreign{SharedBudget::available,&shared};
    q=storage.query(result.proof,train(0),track(1),.8,.81,foreign);CHECK(q.reason==atlas::Reason::StaleProof);
    // Unknown late root invalidates ALL preceding known root fragments.
    const auto old=result.proof;multiple();memory.write(motionBlock+3*0x638+8,provider::detail::Vector{});
    shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    CHECK(!result.complete()&&result.reason==atlas::Reason::BodyUnknown&&result.refusedRoot==train(3));
    CHECK(!result.proof);q=storage.query(old,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    // Late capacity refusal cannot publish one root and omit another.
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(small,budget);
    CHECK(result.reason==atlas::Reason::IntervalCapacity&&!result.complete());
    q=small.query(result.proof,train(0),track(1),0,1,budget);CHECK(!q.known());
    // Inactive ATTACHED members remain in the exact root aggregate. Missing
    // native aggregate car or changed model never becomes positive absence.
    multiple();memory.vector(motionBlock+8,retained,2,32);shared.reset();budget={SharedBudget::available,&shared};
    result=collect(storage,budget);CHECK(!result.complete());
    CHECK(result.providerReason==provider::Reason::MissingAggregateMember);
    multiple();memory.write(table,uintptr_t(0));shared.reset();budget={SharedBudget::available,&shared};
    result=collect(storage,budget);CHECK(!result.complete()&&result.providerReason==provider::Reason::MissingModel);
    // Wrong native owner/thread/lifecycle fails before ANY native read.
    for(unsigned n=0;n<3;++n){
        multiple();if(n==0)owner.allowed=false;if(n==1)owner.thread=9;if(n==2)CHECK(owner.line.endCapture(owner.token));
        shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
        CHECK(result.reason==atlas::Reason::UnqualifiedOwnership&&memory.reads==0);
    }
    // Local inventory/body caps are also terminal for the aggregate budget,
    // preventing a new worker/body attempt from continuing after exhaustion.
    for(bool inventoryCap:{false,true}){
        multiple();shared.reset();budget={SharedBudget::available,&shared};atlas::Limits limits;
        if(inventoryCap)limits.inventory.reads=2;else limits.body.reads=2;
        result=collect(storage,budget,limits);CHECK(!result.complete()&&budget.exhausted);
        const auto reads=memory.reads;result=collect(storage,budget);CHECK(!result.complete()&&memory.reads==reads);
    }
    // Publication cannot be assembled from changed phases or readbacks.
    multiple();owner.deny=10;shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    CHECK(!result.complete());
    multiple();memory.mutate(motionBlock+3*0x638, motionBlock+3*0x638,train(3,2),2);
    shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);CHECK(!result.complete());
    // Every global refusal is terminal across the one inventory and all bodies.
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    const auto totalCalls=shared.calls,totalReads=result.reads,totalBytes=result.bytes;
    for(size_t denied=1;denied<=totalCalls;++denied){
        multiple();shared.reset();shared.deny=denied;budget={SharedBudget::available,&shared};result=collect(storage,budget);
        CHECK(!result.complete()&&budget.exhausted&&shared.refused);
        const auto reads=memory.reads;q=storage.query(result.proof,train(0),track(1),0,1,budget);CHECK(!q.known()&&memory.reads==reads);
    }
    for(unsigned n=0;n<3;++n){
        multiple();shared.reset();budget={SharedBudget::available,&shared};atlas::Limits limits;
        if(n==0)limits.reads=totalReads-1;
        if(n==1)limits.bytes=totalBytes-1;
        if(n==2)limits.preflights=totalCalls-1;
        result=collect(storage,budget,limits);CHECK(!result.complete()&&budget.exhausted);
    }
    // Native reader exception/read failure after root0 rejects whole publication.
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    for(bool throws:{false,true}){
        multiple();if(throws)memory.throwRead=totalReads-10;else memory.denyRead=totalReads-10;
        shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);CHECK(!result.complete());
    }
    // Queries revalidate the stored real owner, not a substituted trusted bool.
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);owner.allowed=false;
    q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    owner.allowed=true;q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    CHECK(owner.line.endCapture(owner.token));q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    owner.deny=owner.calls+2;q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    // A second publication cannot renew or mix the previous proof.
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);const auto first=result.proof;
    owner.begin();result=collect(storage,budget);checkProof(result);
    q=storage.query(first,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    // Reentrant reader work cannot publish/query a staged prefix. Neither
    // nested call mutates the outer capture's data or consumes extra reads.
    multiple();shared.reset();budget={SharedBudget::available,&shared};
    bool nested=false;atlas::Reason nestedQuery{},nestedCapture{};
    result=storage.capture(owner.scope(),true,[&](uintptr_t at,void* out,size_t bytes){
        if(!nested){
            nested=true;const auto reads=memory.reads;
            nestedQuery=storage.query(first,train(0),track(1),0,1,budget).reason;
            nestedCapture=collect(storage,budget).reason;CHECK(memory.reads==reads);
        }
        return memory.read(at,out,bytes);
    },budget);
    checkProof(result);CHECK(nested&&nestedQuery==atlas::Reason::Busy&&nestedCapture==atlas::Reason::Busy);
    // Failures/unknown qualification invalidate the previously published proof.
    const auto prior=result.proof;
    result=storage.capture(owner.scope(),false,[&](uintptr_t at,void* out,size_t bytes){return memory.read(at,out,bytes);},budget);
    CHECK(!result.complete());q=storage.query(prior,train(0),track(1),0,1,budget);CHECK(!q.known());
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    // Actual provider accepts all three native canonical orientations.
    multiple();memory.trackData(1,0,0,100,2);head(3,1,.75,-1,2);
    shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);checkProof(result);
    q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(q.reason==atlas::Reason::Occupied);
    // Four unsorted portions on one track preserve gaps rather than merging
    // the min/max body positions of independent trains into a false envelope.
    multiple();memory.member(5,1);memory.member(6,1);memory.member(7,10);
    memory.trackData(1,0,0,1000);head(3,1,.75);head(5,1,.20);head(6,1,.50);head(7,1,.55);
    shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);checkProof(result);
    CHECK(result.roots==6&&result.cars==16&&result.intervals==5);
    q=storage.query(result.proof,train(0),track(1),.30,.31,budget);CHECK(q.reason==atlas::Reason::Clear);
    q=storage.query(result.proof,train(0),track(1),.19,.20,budget);CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(5));
    q=storage.query(result.proof,train(0),track(1),.49,.50,budget);CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(6));
    q=storage.query(result.proof,train(0),track(1),.74,.75,budget);CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(3));
    q=storage.query(result.proof,train(6),track(1),.49,.50,budget);CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(7));
    q=storage.query(result.proof,train(0),track(1),.55,.55,budget);CHECK(q.reason==atlas::Reason::Occupied&&q.conflictingRoot==train(7));
    q=storage.query(result.proof,train(0),track(1),.60,.61,budget);CHECK(q.reason==atlas::Reason::Clear);
    for(double invalid:{-1.0,1.01,std::numeric_limits<double>::quiet_NaN()}){
        q=storage.query(result.proof,train(0),track(1),invalid,.8,budget);CHECK(!q.known());
    }
    q=storage.query(result.proof,train(0),uint64_t(1)<<48,0,1,budget);CHECK(!q.known());
    q=storage.query(result.proof,uint64_t(5)<<48,track(1),0,1,budget);CHECK(!q.known());
    for(double invalid:{std::numeric_limits<double>::denorm_min(),-std::numeric_limits<double>::denorm_min()}){
        q=storage.query(result.proof,train(0),track(1),invalid,invalid,budget);CHECK(q.reason==atlas::Reason::InvalidQuery);
    }
    // Read-only qualification of the caller's current FP controls. The
    // collector never enables/disables DAZ, FTZ or changes rounding itself.
    for(unsigned mode:{0x0040u,0x8000u,0x2000u,0x4000u}){
        multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);checkProof(result);
        const auto controls=_mm_getcsr();_mm_setcsr(controls|mode);
        q=storage.query(result.proof,train(0),track(1),-std::numeric_limits<double>::denorm_min(),.8,budget);
        const auto afterQuery=_mm_getcsr();_mm_setcsr(controls);
        CHECK(q.reason==atlas::Reason::ArithmeticUnqualified&&!q.known());CHECK(afterQuery==(controls|mode));
        q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(!q.known());
        multiple();shared.reset();budget={SharedBudget::available,&shared};_mm_setcsr(controls|mode);
        result=collect(storage,budget);const auto afterCapture=_mm_getcsr();_mm_setcsr(controls);
        CHECK(result.reason==atlas::Reason::ArithmeticUnqualified&&!result.complete()&&memory.reads==0);
        CHECK(afterCapture==(controls|mode));
    }
    // Inactive parent + independently active attached child is NOT a wholly
    // absent consist, even if an older current-body producer says root absent.
    multiple();memory.write(motionBlock+0x1d0,uint8_t(0));
    memory.write(motionBlock+0x638+0x1d0,uint8_t(1));memory.trackData(2);head(1,2,.75);
    shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    CHECK(!result.complete()&&result.refusedRoot==train(0)&&result.refusedMember==train(1));
    q=storage.query(result.proof,train(3),track(2),.6,.7,budget);CHECK(!q.known());
    // An all-inactive attached consist is positively absent, with no child or
    // root history/presence traversal required. The isolated inactive root4
    // remains independently covered by its own positive absence proof.
    multiple();memory.write(motionBlock+0x1d0,uint8_t(0));
    memory.write(motionBlock+0xe0,provider::detail::Vector{0xdeadbeef,UINTPTR_MAX,UINTPTR_MAX});
    shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);checkProof(result);
    CHECK(result.rows==5&&result.roots==3&&result.absentRoots==2&&result.cars==1&&result.intervals==1);
    q=storage.query(result.proof,train(3),track(0),0,1,budget);CHECK(q.reason==atlas::Reason::Clear);
    multiple();shared.reset();budget={SharedBudget::available,&shared};result=collect(storage,budget);
    // Query budget is the same terminal global budget, including after build.
    shared.deny=shared.calls+1;q=storage.query(result.proof,train(0),track(1),.8,.81,budget);
    CHECK(!q.known()&&budget.exhausted);shared.deny=0;
    q=storage.query(result.proof,train(0),track(1),.8,.81,budget);CHECK(!q.known());
    // The whole operation stays allocation-free under persistent failure.
    multiple();shared.reset();budget={SharedBudget::available,&shared};
    allocation_fault_fixture::arm(0,UINT_MAX);
    result=collect(storage,budget);q=storage.query(result.proof,train(0),track(1),.8,.81,budget);
    const auto oom=allocation_fault_fixture::stop();checkProof(result);CHECK(q.reason==atlas::Reason::Occupied);
    CHECK(!oom.allocations&&!oom.failures);
    std::printf("%zu checks PASS; reads=%zu bytes=%zu preflights=%zu small-storage=%zu default-storage=%zu\n",
        checks,result.reads,result.bytes,result.preflights,sizeof storage,sizeof(atlas::Storage<>));
}
}
int main(){fixture::tests();fixture::protectedQueries();}
