#include <nimby/detail/control_lease.hpp>
#include <engine/train_constraint.h>
#include <cassert>
#include <limits>
int main(){
    using namespace nimby::control;
    constexpr uint64_t t=0x5000000000001,s=0x8000000000001;
    Lease lease;lease.observe(41,{s},{t});
    NimbyControlRequest r{};r.size=sizeof r;r.version=1;r.operation=NIMBY_CONTROL_ACQUIRE;r.lease_ms=1000;r.owner=9;r.generation=41;
    assert(lease.authorize(r,100)==NIMBY_OK);
    auto other=r;other.owner=10;assert(lease.authorize(other,101)==NIMBY_RESOURCE_LIMIT);
    r.operation=NIMBY_CONTROL_TRAIN;r.object=t;r.exit_signal=s;r.speed_mps=7.2;
    assert(lease.authorize(r,200)==NIMBY_OK&&lease.command(r)==NIMBY_OK);
    const auto revision=lease.trains.at(t).revision;
    r.speed_mps=std::numeric_limits<double>::quiet_NaN();assert(lease.command(r)==NIMBY_INVALID_ARGUMENT);
    assert(lease.trains.at(t).revision==revision);
    r.operation=NIMBY_CONTROL_RENEW;assert(lease.authorize(r,1000)==NIMBY_OK);
    assert(lease.active(1999)&&!lease.active(2000)&&lease.trains.empty());
    assert(lease.authorize(r,2000)==NIMBY_INVALID_HANDLE);
    r.operation=NIMBY_CONTROL_ACQUIRE;assert(lease.authorize(r,2000)==NIMBY_OK);
    lease.observe(42,{s},{t});assert(!lease.active(2001)&&lease.authorize(r,2001)==NIMBY_DATA_UNAVAILABLE);
    r.generation=42;assert(lease.authorize(r,2002)==NIMBY_OK);
    r.operation=NIMBY_CONTROL_TRAIN;r.speed_mps=4.5;assert(lease.command(r)==NIMBY_OK);
    assert(lease.trains.at(t).revision>revision);
    lease.observe(42,{}, {t});assert(lease.trains.empty()); // deleted exit

    using namespace nimby::engine::automatic;
    NimbyTrainConstraint c{t,s,1,7.2,0,0};assert(validConstraint(c));
    ControlledMotion m;m.accept(c,0);std::vector<Ahead> route{{s,100}};
    assert(m.ceiling(0,25,route,.5,{},40)==7.2&&m.state()==2);
    assert(m.ceiling(100,25,route,.5,{},40)==7.2); // exact boundary is not passage
    assert(m.ceiling(101,25,route,.5,{},40)==40&&m.state()==3);
    m.accept(c,102);assert(m.completed); // renewal never re-arms
    c.revision=2;c.flags=1;m.accept(c,0);m.observe(0,25,route);
    assert(m.ceiling(101,25,route,.5,{},40)==7.2);
    assert(m.ceiling(126,25,route,.5,{},40)==40);
    c.revision=3;c.mode=1;c.flags=0;m.accept(c,0);
    assert(m.ceiling(1,25,route,.5,{},40)==0); // no physical evidence
    assert(m.ceiling(1,25,route,.5,{true,200,0},40)==7.2);
    m.observe(-1,25,route);assert(m.completed&&m.state()==4); // cancellation is not proof of passage
    c.revision=4;c.exit_signal=0;m.accept(c,0);
    assert(m.ceiling(0,25,{},.5,{true,200,0},40)==0&&m.state()==1);
    assert(m.ceiling(1,25,route,.5,{true,200,0},40)==7.2&&m.exitSignal==s);
    c.revision=5;c.mode=2;c.speed_mps=0;assert(validConstraint(c));m.accept(c,0);
    assert(m.ceiling(0,25,route,.5,{},40)==0);
    c.speed_mps=1;assert(!validConstraint(c));c.mode=0;c.flags=4;assert(!validConstraint(c));
}
