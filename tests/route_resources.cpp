#include <engine/route_resources.h>
#include <array>
#include <cstdio>
#include <limits>
using namespace nimby::engine;
using namespace nimby::engine::interlocking;
namespace {
constexpr uint64_t a=0x1000000000001,b=0x1000000010001,c=0x1000000020001;
constexpr uint64_t trainA=0x5000000000001,trainB=0x5000000000002;
NimbyTrackConflict row(uint64_t source,uint64_t other,double from,double to,double otherFrom,double otherTo){return {source,other,from,to,otherFrom,otherTo};}
Result acquire(RouteInterlocking& engine,const ResourceResult& resources,uint64_t train,uint64_t source=0x8000000000001){
    return engine.acquire({{1,train,source,1},resources.resources,10,resources.complete(),Proof::No},{1,10});
}
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(){
    const ConflictList emptyA{a,TrackConflictReason::Complete,{}},emptyB{b,TrackConflictReason::Complete,{}},emptyC{c,TrackConflictReason::Complete,{}};
    {const RoutePortion forward{a,.2,.8},reverse{a,.8,.2};
     const auto one=buildRouteResources(std::span(&forward,1),std::span(&emptyA,1));
     const auto opposite=buildRouteResources(std::span(&reverse,1),std::span(&emptyA,1));
     CHECK(one.complete()&&one.resources==opposite.resources&&one.resources.size()==1);
     RouteInterlocking engine;CHECK(engine.beginSession(1));CHECK(acquire(engine,one,trainA).granted());
     CHECK(acquire(engine,opposite,trainB).outcome==Outcome::Conflict); // Opposed travel claims same physical range.
    }
    {const RoutePortion one{a,.1,.5},touching{a,.5,.7},disjoint{a,.6,.9};
     const auto first=buildRouteResources(std::span(&one,1),std::span(&emptyA,1));
     const auto contact=buildRouteResources(std::span(&touching,1),std::span(&emptyA,1));
     const auto independent=buildRouteResources(std::span(&disjoint,1),std::span(&emptyA,1));
     RouteInterlocking engine;engine.beginSession(1);CHECK(acquire(engine,first,trainA).granted());
     CHECK(acquire(engine,contact,trainB).outcome==Outcome::Conflict);
     CHECK(acquire(engine,independent,trainB).granted());
    }
    {const std::array relationsA{row(a,b,.2,.6,.3,.8)};const std::array relationsB{row(b,a,.3,.8,.2,.6)};
     const ConflictList lists[]{ {a,TrackConflictReason::Complete,relationsA},{b,TrackConflictReason::Complete,relationsB} };
     const RoutePortion upper{a,.4,.5},lower{b,.5,.4},outside{b,.9,1};
     const auto first=buildRouteResources(std::span(&upper,1),lists),crossing=buildRouteResources(std::span(&lower,1),lists),clear=buildRouteResources(std::span(&outside,1),lists);
     CHECK(first.complete()&&first.resources.size()==2&&first.resources[1]==(Interval{b,.3,.8}));
     CHECK(crossing.complete()&&crossing.resources.size()==2&&clear.complete()&&clear.resources.size()==1);
     RouteInterlocking engine;engine.beginSession(1);CHECK(acquire(engine,first,trainA).granted());
     CHECK(acquire(engine,crossing,trainB).outcome==Outcome::Conflict);
     CHECK(acquire(engine,clear,trainB).granted()); // Non-intersecting F8 ranges do not create aliases.
    }
    {const RoutePortion portions[]{ {a,.4,.2},{c,.2,.1},{a,.3,.5},{a,.9,.8} };
     const std::array relations{row(a,b,.1,.4,.2,.5),row(a,b,.3,.6,.4,.7),row(a,b,.7,.9,.6,.8)};
     const ConflictList lists[]{ {a,TrackConflictReason::Complete,relations},emptyC };
     const auto result=buildRouteResources(portions,lists);CHECK(result.complete()&&result.checkedRelations==3);
     const std::vector<Interval> expected{{a,.2,.5},{a,.8,.9},{b,.2,.8},{c,.1,.2}};
     CHECK(result.resources==expected); // Sorted full IDs; duplicate/overlapping aliases merge once.
    }
    {const RoutePortion portion{a,.5,.5};const std::array relations{row(a,b,.2,.5,0,.1)};
     const ConflictList list{a,TrackConflictReason::Complete,relations};
     const auto result=buildRouteResources(std::span(&portion,1),std::span(&list,1));
     CHECK(result.complete()&&result.resources.size()==2&&result.resources[0]==(Interval{a,.5,.5}));
    }
    {const RoutePortion portions[]{ {a,0,.1},{b,.8,1} };const ConflictList lists[]{emptyA,emptyB};
     const auto result=buildRouteResources(portions,lists);CHECK(result.complete()&&result.resources.size()==2);
     // Distinct track endpoint coordinates are not supplied or inferred here.
     // A known-empty F8 list does not invent a crossing from a shared node.
     const RoutePortion pa{a,1,1},pb{b,0,0};RouteInterlocking engine;engine.beginSession(1);
     CHECK(acquire(engine,buildRouteResources(std::span(&pa,1),lists),trainA).granted());
     CHECK(acquire(engine,buildRouteResources(std::span(&pb,1),lists),trainB).granted());
    }
    {const RoutePortion portions[]{ {a,.1,.2},{b,.3,.4} };const ConflictList lists[]{emptyA};
     const auto result=buildRouteResources(portions,lists);CHECK(result.outcome==ResourceOutcome::MissingTable&&result.track==b&&result.resources.empty());
     const ConflictList unknown[]{emptyA,{b,TrackConflictReason::ReadUnavailable,{}}};
     CHECK(buildRouteResources(portions,unknown).outcome==ResourceOutcome::UnknownTable);
     const ConflictList duplicates[]{emptyA,emptyA,emptyB};CHECK(buildRouteResources(portions,duplicates).outcome==ResourceOutcome::InvalidTable);
    }
    {const RoutePortion recycled{a+1,.1,.2};CHECK(buildRouteResources(std::span(&recycled,1),std::span(&emptyA,1)).outcome==ResourceOutcome::MissingTable);
     const RoutePortion portion{a,.1,.2};const std::array stale{row(a+1,b,.1,.2,.1,.2)};
     const ConflictList list{a,TrackConflictReason::Complete,stale};CHECK(buildRouteResources(std::span(&portion,1),std::span(&list,1)).outcome==ResourceOutcome::InvalidConflict);
    }
    for(double invalid:{-.1,1.1,std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){
        const RoutePortion portion{a,invalid,.2};const auto result=buildRouteResources(std::span(&portion,1),std::span(&emptyA,1));CHECK(result.outcome==ResourceOutcome::InvalidPortion&&result.resources.empty());
        const RoutePortion valid{a,.8,.9};const std::array bad{row(a,b,0,.1,invalid,.5)};const ConflictList list{a,TrackConflictReason::Complete,bad};
        CHECK(buildRouteResources(std::span(&valid,1),std::span(&list,1)).outcome==ResourceOutcome::InvalidConflict); // Even an unused malformed row invalidates the claimed Complete table.
    }
    {const RoutePortion portion{a,.1,.2};const std::array bad{row(a,trainA,0,.3,0,.3)};const ConflictList list{a,TrackConflictReason::Complete,bad};CHECK(buildRouteResources(std::span(&portion,1),std::span(&list,1)).outcome==ResourceOutcome::InvalidConflict);}
    {const RoutePortion portion{a,.1,.2};const std::array relations{row(a,b,0,.5,.1,.2),row(a,c,0,.5,.1,.2)};const ConflictList list{a,TrackConflictReason::Complete,relations};
     ResourceLimits limits;limits.relations=1;auto result=buildRouteResources(std::span(&portion,1),std::span(&list,1),limits);CHECK(result.outcome==ResourceOutcome::BudgetExceeded&&result.resources.empty());
     limits={};limits.candidates=2;CHECK(buildRouteResources(std::span(&portion,1),std::span(&list,1),limits).outcome==ResourceOutcome::BudgetExceeded);
     limits={};limits.resources=2;CHECK(buildRouteResources(std::span(&portion,1),std::span(&list,1),limits).outcome==ResourceOutcome::Capacity);
     const RoutePortion two[]{portion,portion};limits={};limits.portions=1;CHECK(buildRouteResources(two,std::span(&list,1),limits).outcome==ResourceOutcome::BudgetExceeded);
     limits={};limits.tables=1;const ConflictList tooMany[]{list,emptyB};CHECK(buildRouteResources(std::span(&portion,1),tooMany,limits).outcome==ResourceOutcome::BudgetExceeded);
    }
    {const RoutePortion portion{a,.1,.2};const ConflictList unrelatedFaults[]{emptyA,{c,TrackConflictReason::Unstable,{}},{c,TrackConflictReason::Complete,{}}};CHECK(buildRouteResources(std::span(&portion,1),unrelatedFaults).complete());}
    {const std::array ra{row(a,b,0,.3,.2,.6)},rb{row(b,c,0,1,0,1)};
     const ConflictList lists[]{ {a,TrackConflictReason::Complete,ra},{b,TrackConflictReason::Complete,rb} };
     const RoutePortion portion{a,.1,.2};const auto result=buildRouteResources(std::span(&portion,1),lists);
     CHECK(result.complete()&&result.resources.size()==2&&result.checkedRelations==1&&result.resources.back().track==b); // No transitively invented C claim.
     std::vector<RoutePortion> repeated(512,portion);const auto merged=buildRouteResources(repeated,lists);
     CHECK(merged.complete()&&merged.checkedRelations==1&&merged.resources==result.resources); // Repeated portions do not multiply F8 scans.
    }
    {const RoutePortion pa{a,.1,.2},pc{c,.1,.2};const std::array ra{row(a,b,0,.3,.2,.6)},rc{row(c,b,0,.3,.4,.8)};
     const ConflictList lists[]{ {a,TrackConflictReason::Complete,ra},{c,TrackConflictReason::Complete,rc} };
     RouteInterlocking engine;engine.beginSession(1);CHECK(acquire(engine,buildRouteResources(std::span(&pa,1),lists),trainA).granted());
     CHECK(acquire(engine,buildRouteResources(std::span(&pc,1),lists),trainB).outcome==Outcome::Conflict);
     // Conservative alias overlap is documented: this model cannot prove A/C
     // independent solely because the available F8 data only names third B.
    }
    std::puts("PASS route resources: opposed/shared/crossing ranges, inclusive contact, native-only aliases, completeness, full IDs, budgets and conservative exclusions.");
}
