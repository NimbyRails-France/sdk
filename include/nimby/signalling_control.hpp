#pragma once
#include <nimby/mod.hpp>
#include <nimby/signal_observation.hpp>
#include <nimby/detail/automatic_driving.h>
#include <nimby/detail/control_lease.hpp>
#include <mutex>

namespace nimby {
// Serializes remote requests with the mod's observation/evaluation cycle.
// These overlays are deliberately ephemeral: no saved setting is overwritten.
template<class Rules> class SignallingControl {
public:
    std::mutex mutex;
    control::Lease lease;
    std::map<Id,typename Rules::Decision> decisions;
    std::map<Id,SignalSettings> settings;
    std::optional<GameSession> game;
    uint64_t observedAt=0;
    const uint64_t publisher=static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count())|1u;
    bool publishedTrains=false;
    void releaseTrains(){
        if(publishedTrains){detail::check(NimbyInternal_PublishTrainConstraints(nullptr,0,1000,publisher),"ReleaseTrainConstraints");publishedTrains=false;}
    }
    static uint64_t now(){return std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
    void lost(){lease.observe(0,{},{});game.reset();decisions.clear();settings.clear();observedAt=0;}
    void observe(const Snapshot& snapshot,const std::vector<LiveSignalState>& states) {
        if(!snapshot.getGameSession())throw std::runtime_error("Control requires an observed world identity");
        if(game!=snapshot.getGameSession()){
            lost();game=snapshot.getGameSession();
            // Opaque session token, renewed even on reload of the same save.
            lease.observe(static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()),{},{});
        }
        std::set<uint64_t> signals,trains;
        for(const auto& s:states)signals.insert(s.id);
        for(const auto& t:snapshot.getAllTrains())trains.insert(t.getId());
        lease.observe(lease.generation(),std::move(signals),std::move(trains));
        observedAt=now();lease.active(observedAt);settings.clear();decisions.clear();
    }
    void overlay(Id signal,SignalSettings& value) {
        for(const auto& [key,on]:lease.settings)if(key.first==signal){
            const auto boxes=Rules::checkboxes();
            if(value.status!=SettingsStatus::Present) {
                value.status=SettingsStatus::Present;
                for(const auto& box:boxes)value.booleans[std::string(box.name)]=box.defaultValue;
            }
            value.booleans[std::string(boxes[key.second].name)]=on;
        }
        settings[signal]=value;
    }
    std::optional<typename Rules::Decision> forced(Id signal) const {
        const auto at=lease.signals.find(signal);
        if(at==lease.signals.end())return {};
        return typename Rules::Decision{at->second.aspect,at->second.reason};
    }
    void publishTrains() {
        if(lease.trains.empty()){releaseTrains();return;}
        std::vector<NimbyTrainConstraint> commands;
        for(const auto& [id,c]:lease.trains)commands.push_back({id,c.exitSignal,c.revision,c.speed,c.mode,c.flags});
        const auto status=NimbyInternal_PublishTrainConstraints(commands.data(),static_cast<uint32_t>(commands.size()),1000,publisher);
        detail::check(status,"PublishTrainConstraints");publishedTrains=true;
    }
    uint32_t handle(const NimbyControlRequest& request,NimbyControlResponse& out) {
        std::lock_guard guard(mutex);
        const auto time=now();if(observedAt&&time-observedAt>1500)lost();
        auto result=lease.authorize(request,time);
        if(result==NIMBY_OK)switch(request.operation){
        case NIMBY_CONTROL_FORCE_SIGNAL:
            if(!lease.knownSignals.contains(request.object)){result=NIMBY_DATA_UNAVAILABLE;break;}
            if(const auto decision=Rules::forcedDecision(request.value)){
                lease.signals[request.object]={decision->aspect,decision->reason};
            }else result=NIMBY_INVALID_ARGUMENT;
            break;
        case NIMBY_CONTROL_SETTING:
            if(!lease.knownSignals.contains(request.object)){result=NIMBY_DATA_UNAVAILABLE;break;}
            if(request.index<0||static_cast<size_t>(request.index)>=Rules::checkboxes().size()||
               (request.value!=0&&request.value!=1)){result=NIMBY_INVALID_ARGUMENT;break;}
            lease.settings[{request.object,request.index}]=request.value!=0;break;
        case NIMBY_CONTROL_READ_SIGNAL:
            if(const auto at=decisions.find(request.object);at!=decisions.end()){
                out.aspect=at->second.aspect;out.reason=at->second.reason;
                // active reports whether the observed decision is currently forced.
                const auto forced=lease.signals.find(request.object);
                out.active=forced==lease.signals.end()?0u:
                    (forced->second.aspect==out.aspect&&forced->second.reason==out.reason?2u:1u);
            }else result=NIMBY_DATA_UNAVAILABLE;
            break;
        case NIMBY_CONTROL_READ_TRAIN: {
            if(!lease.knownTrains.contains(request.object)){result=NIMBY_DATA_UNAVAILABLE;break;}
            NimbyTrainConstraintStatus state{};state.size=sizeof state;
            result=NimbyInternal_ReadTrainConstraint(request.object,&state);
            if(result==NIMBY_OK){
                if(const auto pending=lease.trains.find(request.object);pending!=lease.trains.end()){
                    const auto& command=pending->second;
                    out.active=state.revision==command.revision?state.state:1u;
                    out.speed_mps=command.speed;out.exit_signal=state.revision==command.revision?state.exit_signal:command.exitSignal;
                }else{out.active=state.state;out.speed_mps=state.speed_mps;out.exit_signal=state.exit_signal;}
            }
            break;
        }
        default:result=lease.command(request);break;
        }
        out.generation=lease.generation();out.remaining_ms=lease.remaining(time);
        out.signal_count=static_cast<uint32_t>(lease.signals.size());out.train_count=static_cast<uint32_t>(lease.trains.size());
        out.setting_count=static_cast<uint32_t>(lease.settings.size());
        out.capabilities=(1u<<13)-1;
        out.result=result;
        return result;
    }
};
}
