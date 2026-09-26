#include <nimby/signalling_control.hpp>
#include <cassert>
struct Rules {
    struct Decision {int aspect,reason;};
    static std::optional<Decision> forcedDecision(int aspect){if(aspect<0||aspect>3)return {};return Decision{aspect,99};}
    static std::span<const nimby::SignalCheckbox> checkboxes(){
        static const nimby::SignalCheckbox boxes[]{{"enabled","Enabled","",false},{"extra","Extra","",true}};return boxes;
    }
};
int main(){
    constexpr uint64_t signal=0x8000000000001,train=0x5000000000001;
    nimby::SignallingControl<Rules> control;
    control.observedAt=control.now();control.lease.observe(123,{signal},{train});
    NimbyControlRequest r{};r.size=sizeof r;r.version=1;r.operation=NIMBY_CONTROL_ACQUIRE;r.lease_ms=1000;r.owner=7;r.generation=123;
    NimbyControlResponse out{};out.size=sizeof out;
    assert(control.handle(r,out)==NIMBY_OK);
    r.operation=NIMBY_CONTROL_FORCE_SIGNAL;r.object=signal;r.value=2;
    assert(control.handle(r,out)==NIMBY_OK&&control.forced(signal)->aspect==2);
    r.value=100;assert(control.handle(r,out)==NIMBY_INVALID_ARGUMENT&&control.forced(signal)->aspect==2);
    r.operation=NIMBY_CONTROL_READ_SIGNAL;
    assert(control.handle(r,out)==NIMBY_DATA_UNAVAILABLE); // accepted is not evaluated
    control.decisions[signal]={1,0};assert(control.handle(r,out)==NIMBY_OK&&out.active==1);
    control.decisions[signal]={2,99};assert(control.handle(r,out)==NIMBY_OK&&out.active==2);
    r.operation=NIMBY_CONTROL_SETTING;r.index=0;r.value=1;assert(control.handle(r,out)==NIMBY_OK);
    nimby::SignalSettings underlying;underlying.status=nimby::SettingsStatus::Absent;
    auto effective=underlying;control.overlay(signal,effective);
    assert(effective.getBoolean("enabled")==true&&effective.getBoolean("extra")==true);
    assert(underlying.status==nimby::SettingsStatus::Absent&&underlying.booleans.empty());
    r.index=2;assert(control.handle(r,out)==NIMBY_INVALID_ARGUMENT);
    r.operation=NIMBY_CONTROL_RESTORE_SETTING;r.index=0;assert(control.handle(r,out)==NIMBY_OK);
    effective=underlying;control.overlay(signal,effective);assert(effective.status==nimby::SettingsStatus::Absent);
    r.operation=NIMBY_CONTROL_RELEASE;assert(control.handle(r,out)==NIMBY_OK&&!control.forced(signal));
    r.operation=NIMBY_CONTROL_ACQUIRE;assert(control.handle(r,out)==NIMBY_OK);
    r.operation=NIMBY_CONTROL_FORCE_SIGNAL;r.value=3;assert(control.handle(r,out)==NIMBY_OK);
    control.observedAt=control.now()-2000;r.operation=NIMBY_CONTROL_STATUS;
    assert(control.handle(r,out)==NIMBY_OK&&out.generation==0&&!control.forced(signal));
}
