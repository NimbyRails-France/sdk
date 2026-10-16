#include <platform/windows/runtime/native_crossing_observation.h>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
using namespace nimby::windows::automatic;
namespace {
constexpr CrossingIdentity identity{0x10000,(5ULL<<48)|1,(8ULL<<48)|1,4,true};
constexpr CrossingRange range{0x20000,0x30000,0x40000,(1ULL<<48)|1,.2,.8,true};
constexpr uint64_t other=(1ULL<<48)|0x10001;
void call(CrossingObservation& value,CrossingPredicate kind,uintptr_t caller,uint8_t result,
    uint64_t track=other,double from=.3,double to=.7,CrossingIdentity who=identity){
    value.predicate(kind,caller,kind==CrossingPredicate::Occupation?range.occupationContext:range.reservationContext,
        who,track,from,to,result);
}
void followed(CrossingObservation& value,uint8_t occupiedClear=1,uint8_t reservedClear=1){
    call(value,CrossingPredicate::Occupation,value.followedOccupationReturnRva,occupiedClear,range.ownTrack,range.from,range.to);
    if(occupiedClear)call(value,CrossingPredicate::Reservation,value.followedReservationReturnRva,reservedClear,range.ownTrack,range.from,range.to);
}
CrossingObservation started(){CrossingObservation value(identity);CHECK(value.source(identity));CHECK(value.beginRange(range));return value;}
void run(){
    static_assert(std::is_trivially_copyable_v<CrossingObservation>);
    for(const bool reservation:{false,true}){
        auto value=started();followed(value);
        call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,reservation?1:0);
        if(reservation)call(value,CrossingPredicate::Reservation,value.crossingReservationReturnRva,0);
        value.endRange(0);
        CHECK(value.finish(identity,0)==(reservation?CrossingRefusal::CrossingReserved:CrossingRefusal::CrossingOccupied));
        CHECK(!value.finish(identity,1)); // Composite clear is not a crossed refusal.
        auto changed=identity;changed.routeRevision++;CHECK(!value.finish(changed,0));
        changed=identity;changed.source++;CHECK(!value.finish(changed,0));
        changed=identity;changed.train++;CHECK(!value.finish(changed,0));
        changed=identity;changed.motion++;CHECK(!value.finish(changed,0));
    }
    for(const bool physical:{false,true}){
        auto value=started();followed(value,physical?0:1,0);value.endRange(0);
        CHECK(!value.finish(identity,0)); // Both longitudinal refusals preserve permissive policy.
    }
    {auto value=started();followed(value);value.endRange(0);CHECK(!value.finish(identity,0));} // Controller/script/termination unknown.
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,0,0);value.endRange(0);CHECK(!value.finish(identity,0));}
    {auto value=started();call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0);value.endRange(0);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);call(value,CrossingPredicate::Reservation,value.crossingReservationReturnRva,0);value.endRange(0);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,1);
     call(value,CrossingPredicate::Reservation,value.crossingReservationReturnRva,0,other,.3,.71);value.endRange(0);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0);value.endRange(1);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0);
     value.endRange(0);CHECK(!value.source(identity));CHECK(!value.finish(identity,0));}
    {CrossingObservation value(identity);CHECK(!value.beginRange(range));followed(value);value.endRange(0);CHECK(!value.finish(identity,0));}
    // An alias can repeat exactly the same longitudinal arguments. Only the
    // pinned caller, not track/range inequality, identifies its crossed denial.
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0,range.ownTrack,range.from,range.to);
     value.endRange(0);CHECK(value.finish(identity,0)==CrossingRefusal::CrossingOccupied);}
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,1);
     call(value,CrossingPredicate::Reservation,value.crossingReservationReturnRva,1);value.endRange(1);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0,1);value.endRange(0);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);auto wrong=identity;wrong.qualified=false;
     call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0,other,.3,.7,wrong);value.endRange(0);CHECK(!value.finish(identity,0));}
    {auto value=started();followed(value);for(int i=0;i<4096;++i){call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,1);
     call(value,CrossingPredicate::Reservation,value.crossingReservationReturnRva,1);}
     call(value,CrossingPredicate::Occupation,value.crossingOccupationReturnRva,0);value.endRange(0);CHECK(!value.finish(identity,0));}
}
}
int main(){try{run();std::cout<<"qualified native crossing refusal observation: PASS\n";return 0;}
catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
