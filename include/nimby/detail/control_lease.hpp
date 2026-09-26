#pragma once
#include <nimby/detail/control.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <utility>

namespace nimby::control {
// Common state machine. The host supplies its monotonic clock, loaded-world
// generation and catalogue. No OS calls, national aspects or speed defaults.
struct SignalDecision {int32_t aspect,reason;};
struct TrainCommand {
    uint64_t train=0,exitSignal=0,revision=0;
    double speed=0;
    uint32_t mode=0,flags=0;
};
class Lease {
    uint64_t owner_=0,expires_=0,generation_=0,revision_=0;
public:
    std::map<uint64_t,SignalDecision> signals;
    std::map<uint64_t,TrainCommand> trains;
    std::map<std::pair<uint64_t,int32_t>,bool> settings;
    std::set<uint64_t> knownSignals,knownTrains;
    uint64_t generation() const {return generation_;}
    void clear() {signals.clear();trains.clear();settings.clear();}
    void release() {clear();owner_=expires_=0;}
    void observe(uint64_t generation,std::set<uint64_t> signalIds,std::set<uint64_t> trainIds) {
        if(generation!=generation_){release();generation_=generation;}
        knownSignals=std::move(signalIds);knownTrains=std::move(trainIds);
        std::erase_if(signals,[&](const auto& row){return !knownSignals.contains(row.first);});
        std::erase_if(trains,[&](const auto& row){return !knownTrains.contains(row.first)||
            (row.second.exitSignal&&!knownSignals.contains(row.second.exitSignal));});
        std::erase_if(settings,[&](const auto& row){return !knownSignals.contains(row.first.first);});
    }
    bool active(uint64_t now) {if(owner_&&now>=expires_)release();return owner_!=0;}
    uint64_t remaining(uint64_t now) {return active(now)?expires_-now:0;}
    uint32_t authorize(const NimbyControlRequest& r,uint64_t now) {
        if(r.size!=sizeof r||r.version!=NIMBY_CONTROL_VERSION)return NIMBY_INVALID_ARGUMENT;
        active(now);
        if(r.operation==NIMBY_CONTROL_STATUS)return NIMBY_OK;
        if(!generation_||r.generation!=generation_)return NIMBY_DATA_UNAVAILABLE;
        if(r.operation==NIMBY_CONTROL_READ_SIGNAL||r.operation==NIMBY_CONTROL_READ_TRAIN)return NIMBY_OK;
        if(!r.owner)return NIMBY_INVALID_ARGUMENT;
        if(r.operation==NIMBY_CONTROL_ACQUIRE||r.operation==NIMBY_CONTROL_RENEW) {
            if(r.lease_ms<1000||r.lease_ms>60000||UINT64_MAX-now<r.lease_ms)return NIMBY_INVALID_ARGUMENT;
            if(owner_&&owner_!=r.owner)return NIMBY_RESOURCE_LIMIT;
            if(r.operation==NIMBY_CONTROL_RENEW&&owner_!=r.owner)return NIMBY_INVALID_HANDLE;
            owner_=r.owner;expires_=now+r.lease_ms;return NIMBY_OK;
        }
        return owner_==r.owner?NIMBY_OK:NIMBY_INVALID_HANDLE;
    }
    uint32_t command(const NimbyControlRequest& r) {
        switch(r.operation) {
        case NIMBY_CONTROL_STATUS:case NIMBY_CONTROL_ACQUIRE:case NIMBY_CONTROL_RENEW:return NIMBY_OK;
        case NIMBY_CONTROL_RELEASE:release();return NIMBY_OK;
        case NIMBY_CONTROL_CLEAR:clear();return NIMBY_OK;
        case NIMBY_CONTROL_RESTORE_SIGNAL:signals.erase(r.object);return NIMBY_OK;
        case NIMBY_CONTROL_RESTORE_TRAIN:trains.erase(r.object);return NIMBY_OK;
        case NIMBY_CONTROL_RESTORE_SETTING:settings.erase({r.object,r.index});return NIMBY_OK;
        case NIMBY_CONTROL_TRAIN:
            if(!knownTrains.contains(r.object)||(r.exit_signal&&!knownSignals.contains(r.exit_signal)))return NIMBY_DATA_UNAVAILABLE;
            if(!std::isfinite(r.speed_mps)||r.speed_mps<0||r.mode>NIMBY_CONTROL_STOP||
               (r.flags&~NIMBY_CONTROL_RELEASE_BY_REAR)||(r.mode==NIMBY_CONTROL_STOP&&r.speed_mps!=0)||
               (r.mode!=NIMBY_CONTROL_STOP&&r.speed_mps<=0))return NIMBY_INVALID_ARGUMENT;
            trains[r.object]={r.object,r.exit_signal,++revision_,r.speed_mps,r.mode,r.flags};return NIMBY_OK;
        default:return NIMBY_HOOKS_UNAVAILABLE; // Mod decisions/settings require its schema.
        }
    }
};
}
