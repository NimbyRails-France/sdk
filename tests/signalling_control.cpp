#include <nimby/signalling_control.hpp>
#include <nimby/automatic_driving.hpp>
#include <nimby/detail/live_texture_tracking.hpp>
#include <cassert>
#include <iostream>

namespace {
uint32_t trainReleaseStatus=NIMBY_OK,textureReleaseStatus=NIMBY_OK,drivingReleaseStatus=NIMBY_OK;
unsigned trainReleases=0,textureReleases=0,drivingReleases=0;
uint64_t trainPublisher=0,lastTrainReleasePublisher=0;
}
// Exercise the real control/texture/driving wrappers with deterministic failures.
uint32_t __cdecl NimbyInternal_PublishTrainConstraints(const NimbyTrainConstraint*,uint32_t count,uint32_t,uint64_t publisher) noexcept {
    if(count){trainPublisher=publisher;return NIMBY_OK;}
    ++trainReleases;lastTrainReleasePublisher=publisher;return trainReleaseStatus;
}
uint32_t __cdecl NimbyInternal_ClearOwnedTextures(uint32_t,uint64_t,const uint64_t*,uint32_t) noexcept {
    ++textureReleases;return textureReleaseStatus;
}
uint32_t __cdecl NimbyInternal_PublishDrivingRulesV3(const NimbySignalDrivingRule*,uint32_t,uint32_t,uint32_t,uint64_t) noexcept {
    ++drivingReleases;return drivingReleaseStatus;
}
struct Rules {
    struct Decision {int aspect,reason;};
    static std::optional<Decision> forcedDecision(int aspect){if(aspect<0||aspect>3)return {};return Decision{aspect,99};}
    static std::span<const nimby::SignalCheckbox> checkboxes(){
        static const nimby::SignalCheckbox boxes[]{{"enabled","Enabled","",false},{"extra","Extra","",true}};return boxes;
    }
};
struct TypedRules : Rules {
    static Decision diagnosticDecision(const Decision& value){return {value.aspect-100,value.reason-200};}
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
    // Compare the opaque stored decision before exposing model-local codes.
    nimby::SignallingControl<TypedRules> typed;
    typed.observedAt=typed.now();typed.lease.observe(123,{signal},{train});
    r.operation=NIMBY_CONTROL_ACQUIRE;assert(typed.handle(r,out)==NIMBY_OK);
    typed.decisions[signal]={102,299};typed.lease.signals[signal]={102,299};
    r.operation=NIMBY_CONTROL_READ_SIGNAL;
    assert(typed.handle(r,out)==NIMBY_OK&&out.aspect==2&&out.reason==99&&out.active==2);

    // A failed train release cannot prevent texture cleanup. Its ownership
    // remains pending until a later successful release, without reviving leases.
    control.lease.trains[train]={train,0,1,10,0,0};
    control.publishTrains();assert(control.publishedTrains&&trainPublisher==control.publisher);
    std::vector<uint64_t> owned{signal};
    const auto restore=[&] {
        std::unique_lock unlocked(control.mutex,std::try_to_lock);
        assert(unlocked.owns_lock()); // Independent output RPCs run outside it.
        assert(control.lease.trains.empty()&&!control.game&&control.observedAt==0);
        nimby::detail::restoreTrackedTextures(owned,[](auto){return true;},
            [](auto batch){nimby::SignalTextures::inGame().restore(batch);},[](auto){});
    };
    trainReleaseStatus=NIMBY_RESOURCE_LIMIT;
    bool rejected=false;
    try {control.invalidateAndCleanup(restore);}catch(const nimby::Exception& error){rejected=error.code()==nimby::ErrorCode::ResourceLimit;}
    assert(rejected&&textureReleases==1&&owned.empty()&&control.publishedTrains);
    assert(lastTrainReleasePublisher==trainPublisher&&trainReleases==1);

    // The reverse failure preserves pending texture IDs while the successful
    // train release stays complete; its next cleanup must not resend that RPC.
    trainReleaseStatus=NIMBY_OK;textureReleaseStatus=NIMBY_RESOURCE_LIMIT;owned={signal};
    control.invalidateAndCleanup(restore);
    assert(!control.publishedTrains&&trainReleases==2&&textureReleases==2&&owned.size()==1);
    textureReleaseStatus=NIMBY_OK;control.invalidateAndCleanup(restore);
    assert(trainReleases==2&&textureReleases==3&&owned.empty());

    // Stop uses the same independent cleanup but releases automatic signal
    // rules. Preserve the first failure if both channels are unavailable.
    control.lease.trains[train]={train,0,2,10,0,0};control.publishTrains();
    trainReleaseStatus=NIMBY_RESOURCE_LIMIT;drivingReleaseStatus=NIMBY_IO_ERROR;
    rejected=false;
    try {control.invalidateAndCleanup([]{nimby::AutomaticDriving::release();});}
    catch(const nimby::Exception& error){rejected=error.code()==nimby::ErrorCode::ResourceLimit;}
    assert(rejected&&drivingReleases==1&&control.publishedTrains&&trainReleases==3);
    trainReleaseStatus=NIMBY_OK;drivingReleaseStatus=NIMBY_OK;
    control.invalidateAndCleanup([]{nimby::AutomaticDriving::release();});
    assert(!control.publishedTrains&&drivingReleases==2&&trainReleases==4);
    std::cout<<"PASS control overlays and independent cleanup with transient failures\n";
}
