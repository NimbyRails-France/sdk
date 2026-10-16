#include <platform/windows/runtime/route_read_budget.h>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace {
using nimby::windows::automatic::RouteReadBudget;
using Reason=RouteReadBudget::Reason;
unsigned assertions=0;
#define CHECK(x) do {++assertions;if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
static_assert(std::is_trivially_copyable_v<RouteReadBudget>);
static_assert(std::is_nothrow_constructible_v<RouteReadBudget,std::int64_t,std::int64_t>);
static_assert(noexcept(std::declval<RouteReadBudget&>().allow(0,0)));
static_assert(sizeof(RouteReadBudget)<=48);

void clocks() {
    RouteReadBudget exact(100,1000000);
    CHECK(exact.available(100)&&exact.allow(16,2099));
    CHECK(exact.allow(16,2100)&&exact.available(2100));
    CHECK(exact.reads()==2&&exact.bytes()==32);
    CHECK(!exact.allow(1,2101)&&exact.reason()==Reason::Time);
    CHECK(!exact.available(100)&&!exact.allow(0,100));
    CHECK(exact.reason()==Reason::Time&&exact.reads()==2&&exact.bytes()==32);

    // A resolution that does not divide 500 permits its last whole tick,
    // rather than rounding the 2 ms deadline up to an unproven extra tick.
    RouteReadBudget coarse(0,501);
    CHECK(coarse.allow(1,1));
    CHECK(!coarse.available(2)&&coarse.reason()==Reason::Time);
    RouteReadBudget lowResolution(0,499);
    CHECK(lowResolution.allow(1,0));
    CHECK(!lowResolution.allow(1,1)&&lowResolution.reason()==Reason::Time);

    for(auto pair:{std::pair<std::int64_t,std::int64_t>{-1,1000},{0,0},{0,-1}}){
        RouteReadBudget invalid(pair.first,pair.second);
        CHECK(invalid.reason()==Reason::InvalidClock);
        CHECK(!invalid.available(0)&&!invalid.allow(8,1));
        CHECK(invalid.reads()==0&&invalid.bytes()==0);
    }
    RouteReadBudget negativeSample(0,1000000);
    CHECK(!negativeSample.available(-1)&&negativeSample.reason()==Reason::InvalidClock);
    CHECK(!negativeSample.available(0));

    RouteReadBudget backwards(100,1000000);
    CHECK(!backwards.allow(1,99)&&backwards.reason()==Reason::ClockRegression);
    CHECK(!backwards.allow(1,101)&&backwards.reads()==0);
    RouteReadBudget intermediate(100,1000000);
    CHECK(intermediate.available(200)&&intermediate.allow(16,200));
    CHECK(!intermediate.available(199)&&intermediate.reason()==Reason::ClockRegression);
    CHECK(!intermediate.available(201)&&intermediate.bytes()==16);

    // Neither clock validation nor the deadline may overflow near INT64_MAX.
    const auto top=std::numeric_limits<std::int64_t>::max();
    RouteReadBudget late(top-2000,1000000);
    CHECK(late.allow(1,top)&&late.available(top));
    RouteReadBudget wide(0,top);
    CHECK(wide.allow(1,top/500));
    CHECK(!wide.available(top/500+1)&&wide.reason()==Reason::Time);
    RouteReadBudget nearTop(top-1,top);
    CHECK(nearTop.allow(1,top));
}

void bytes() {
    RouteReadBudget exact(0,1000000);
    CHECK(exact.allow(RouteReadBudget::maximumBytes,0));
    CHECK(exact.bytes()==RouteReadBudget::maximumBytes&&exact.reads()==1);
    CHECK(exact.available(2000));
    CHECK(exact.allow(0,2000));
    CHECK(!exact.allow(1,2000)&&exact.reason()==Reason::Bytes);
    CHECK(exact.bytes()==RouteReadBudget::maximumBytes&&exact.reads()==2);
    CHECK(!exact.allow(0,0)&&exact.reason()==Reason::Bytes);

    RouteReadBudget oversized(0,1000000);
    CHECK(!oversized.allow(std::numeric_limits<std::size_t>::max(),0));
    CHECK(oversized.reason()==Reason::Bytes&&oversized.bytes()==0&&oversized.reads()==0);
    RouteReadBudget sum(0,1000000);
    CHECK(sum.allow(RouteReadBudget::maximumBytes-8,1));
    CHECK(!sum.allow(9,2)&&sum.reason()==Reason::Bytes);
    CHECK(sum.bytes()==RouteReadBudget::maximumBytes-8&&sum.reads()==1);
}

void cumulative() {
    // The native adapter checks one scope for successive topology/path reads,
    // not a renewed 2 ms / 4 MiB allowance for each individual callback.
    RouteReadBudget chunks(1000,1000000);
    for(std::int64_t i=1;i<=1024;++i)CHECK(chunks.allow(4096,1000+i));
    CHECK(chunks.bytes()==RouteReadBudget::maximumBytes&&chunks.reads()==1024);
    CHECK(!chunks.allow(1,2025)&&chunks.reason()==Reason::Bytes);
    CHECK(!chunks.available(2024)&&chunks.reason()==Reason::Bytes);

    RouteReadBudget calls(0,1000000);
    for(std::uint32_t i=0;i<RouteReadBudget::maximumReads;++i)CHECK(calls.allow(0,0));
    CHECK(calls.reads()==RouteReadBudget::maximumReads&&calls.bytes()==0);
    CHECK(calls.available(2000));
    CHECK(!calls.allow(0,2000)&&calls.reason()==Reason::Calls);
    CHECK(!calls.allow(1,0)&&calls.reason()==Reason::Calls);
    CHECK(calls.reads()==RouteReadBudget::maximumReads&&calls.bytes()==0);

    RouteReadBudget time(0,1000000);
    for(std::int64_t tick=0;tick<=2000;tick+=100)CHECK(time.allow(8,tick));
    CHECK(time.reads()==21&&time.bytes()==168);
    CHECK(!time.allow(8,2001)&&time.reason()==Reason::Time);
    CHECK(time.reads()==21&&time.bytes()==168);
}

void independent() {
    RouteReadBudget refused(0,1000000),healthy(5000,1000000);
    CHECK(!refused.allow(RouteReadBudget::maximumBytes+1,0));
    CHECK(healthy.allow(64,5000)&&healthy.available(7000));
    CHECK(healthy.reason()==Reason::None&&healthy.reads()==1&&healthy.bytes()==64);
    auto copy=healthy;
    CHECK(!copy.available(7001)&&copy.reason()==Reason::Time);
    CHECK(healthy.available(7000)&&healthy.reason()==Reason::None);
}
}
int main(){try{
    clocks();bytes();cumulative();independent();
    std::cout<<assertions<<" route read budget assertions passed\n";
    return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
