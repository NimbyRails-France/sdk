#pragma once
#include <nimby/detail/observation_session.hpp>
#include <nimby/detail/track_fraction.hpp>

namespace nimby {
// Ordered facing boundaries ahead of the HEAD. The first bounds the immediate
// upstream block, the second the following block, etc. Geometry alone never
// selects a branch or grants movement. Stop at a junction, cycle, missing link
// or coincident boundary; the caller chooses how many blocks matter to its mod.
inline std::vector<Id> approachedSignals(const SignalTopology& topology,Position head,size_t blocks=1) {
    if(!blocks||blocks>16)throw std::invalid_argument("Approach range must be 1..16 blocks");
    // A train stopped at a signal can be one rounding unit beyond its stored
    // fraction. Keep that contact in the approach, consistently with BlockReader
    // excluding it from downstream occupation. Only normalize the initial head,
    // on its own track and facing direction; never rewind along a previous track
    // or move subsequent traversal points. Real entry still releases the approach.
    if(std::isfinite(head.getFraction())&&head.getFraction()>=0&&head.getFraction()<=1){
        std::optional<double> contact;
        for(const auto& signal:topology.getSignalsForTrack(head.getTrackId(),head.getDirection(),true)){
            if(!(std::abs(signal.getFraction()-head.getFraction())<=detail::trackFractionRounding))continue;
            if(contact)return {}; // Numerically coincident boundaries are ambiguous.
            contact=signal.getFraction();
        }
        if(contact&&(head.getFraction()-*contact)*head.getDirection()>0)
            head={head.getTrackId(),*contact,head.getDirection()};
    }
    std::vector<Id> result;Id exclude=0;size_t remaining=256;
    while(result.size()<blocks&&remaining){
        const auto trace=topology.traceToNextSignal(head,exclude,remaining);
        remaining-=std::min(remaining,trace.sections.size());
        if(trace.stop!=SignalTraceStop::SignalReached)break;
        std::optional<Signal> first;
        for(const auto& section:trace.sections){
            for(const auto& signal:section.orderedSignals){
                if(signal.getId()==exclude)continue;
                if(first){
                    if(signal.getFraction()==first->getFraction())return result;
                    break;
                }
                if(exclude&&signal.getTrackId()==head.getTrackId()&&signal.getFraction()==head.getFraction())return result;
                first=signal;
            }
            if(first)break;
        }
        if(!first||std::find(result.begin(),result.end(),first->getId())!=result.end())break;
        result.push_back(first->getId());exclude=first->getId();
        head={first->getTrackId(),first->getFraction(),first->getForwardDirection()};
    }
    return result;
}
inline std::optional<Id> firstApproachedSignal(const SignalTopology& topology,Position head) {
    const auto signals=approachedSignals(topology,head);return signals.empty()?std::nullopt:std::optional<Id>{signals.front()};
}

struct SignalApproach {Id train=0;size_t blocks=0;};

inline std::map<Id,SignalApproach> observeSignalApproaches(const Snapshot& snapshot,const SignalTopology& topology,
    Milliseconds maxAge=Milliseconds{1000},size_t blocks=1) {
    std::map<Id,SignalApproach> result;
    if(snapshot.isOlderThan(maxAge)||snapshot.getAllTrains().size()>4096)return result;
    for(const auto& train:snapshot.getAllTrains()) {
        const auto service=snapshot.getTrainServiceById(train.getId());
        const auto head=train.getPosition();
        if(!service||!head||service->isOnNetwork()!=std::optional<bool>{true}||service->isHidden()!=std::optional<bool>{false})continue;
        const auto signals=approachedSignals(topology,*head,blocks);
        for(size_t i=0;i<signals.size();++i){
            const SignalApproach candidate{train.getId(),i+1};
            const auto [it,inserted]=result.try_emplace(signals[i],candidate);
            // Prefer the nearer block; stable full ID breaks ties deterministically.
            if(!inserted&&std::tie(candidate.blocks,candidate.train)<std::tie(it->second.blocks,it->second.train))it->second=candidate;
        }
    }
    return result;
}
inline std::map<Id,SignalApproach> observeSignalApproaches(const Snapshot& snapshot,std::span<const Signal> boundaries,
    Milliseconds maxAge=Milliseconds{1000},size_t blocks=1) {
    const SignalTopology topology(boundaries,snapshot.getAllTrackNodes(),snapshot.getAllTrackJunctions(),SignalDirectionConvention::Forward);
    return observeSignalApproaches(snapshot,topology,maxAge,blocks);
}
}
