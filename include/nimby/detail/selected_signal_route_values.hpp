#pragma once
#include <nimby/selected_signal_route.hpp>
#include <nimby/detail/signal_route_values.hpp>
#include <unordered_map>
#include <unordered_set>

namespace nimby::detail {
// Index once for this copied snapshot. No game call, native address, national
// marker convention or persistent history is involved in these joins.
class SelectedRouteIndex {
    struct Object {
        Signal signal;
        bool unconditional=false;
        bool filterKnown=false;
        std::optional<SignalRouteMarker> marker{};
    };
    std::unordered_map<Id,Object> objects_;
    std::unordered_map<Id,std::vector<Id>> markers_;
    static bool identity(Id id,unsigned kind){return id>>48==kind&&(id&0x0000ffffffffffffULL);}
public:
    SelectedRouteIndex(std::span<const Signal> signals,std::span<const SignalState> states,
        std::span<const SignalTexture> textures) {
        std::unordered_map<Id,const SignalState*> byState;
        std::unordered_map<Id,const SignalTexture*> byTexture;
        for(const auto& state:states)byState.emplace(state.getSignalId(),&state);
        for(const auto& texture:textures)byTexture.emplace(texture.getSignalId(),&texture);
        for(const auto& signal:signals){
            const auto kind=signal.getKind();
            if(kind!=NIMBY_SIGNAL_PATH&&kind!=NIMBY_SIGNAL_BALISE&&kind!=NIMBY_SIGNAL_MARKER)continue;
            if(!identity(signal.getId(),8)||!identity(signal.getTrackId(),1)||!std::isfinite(signal.getFraction())||
               signal.getFraction()<0||signal.getFraction()>1)continue;
            Object object{signal};
            if(const auto state=byState.find(signal.getId());state!=byState.end()){
                object.filterKnown=state->second->isIgnoredByDefault().has_value()&&state->second->getExceptionCount().has_value();
                object.unconditional=state->second->isIgnoredByDefault()==std::optional<bool>{false}&&
                    state->second->getExceptionCount()==std::optional<uint32_t>{0};
            }
            if(kind==NIMBY_SIGNAL_MARKER){
                if(const auto texture=byTexture.find(signal.getId());texture!=byTexture.end()){
                    if(const auto ref=texture->second->getReference();ref&&!ref->isClamped()&&
                       ref->getSelectedIndex()>=0&&ref->getSelectedIndex()<256&&uint32_t(ref->getSelectedIndex())<ref->getStateCount()){
                        const auto name=ref->getTexturesId();
                        if(!name.empty()&&name.size()<=256&&std::all_of(name.begin(),name.end(),[](unsigned char c){return c>=32&&c<=126;}))
                            object.marker=SignalRouteMarker{signal.getId(),name,ref->getSelectedIndex()};
                    }
                }
                markers_[signal.getTrackId()].push_back(signal.getId());
            }
            if(!objects_.emplace(signal.getId(),std::move(object)).second)
                throw std::invalid_argument("Duplicate selected-route object");
        }
        for(auto& [track,ids]:markers_)std::sort(ids.begin(),ids.end(),[&](Id a,Id b){
            return std::pair{objects_.at(a).signal.getFraction(),a}<std::pair{objects_.at(b).signal.getFraction(),b};
        });
    }

    SelectedSignalRoute read(const NimbySignalRouteObservation& row,const Signal& source,
        std::span<const NimbySignalRouteSection> parts,std::span<const NimbySignalRouteAnnotation> annotations,
        const BlockReader& reader)const {
        if(!SignalRoutes::sourceMatches(row,source)||parts.empty()||parts.size()>4096||annotations.size()>4096)return {};
        const NimbySignalRouteAnnotation* boundary=nullptr;
        for(const auto& annotation:annotations){
            if(annotation.train_id!=row.train_id||annotation.signal_id!=row.signal_id||
               !identity(annotation.object_id,8)||!identity(annotation.track_id,1)||
               !std::isfinite(annotation.track_fraction)||annotation.track_fraction<0||annotation.track_fraction>1||
               !std::isfinite(annotation.distance_m)||annotation.distance_m<0||
               (annotation.direction!=1&&annotation.direction!=-1)||(annotation.flags&~15u))return {};
            if(annotation.flags&NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY){if(boundary)return {};boundary=&annotation;}
        }
        // A distance limit, last copied range or missing next PATH is not an end.
        if(!boundary||boundary->object_id==source.getId())return {};
        const auto end=objects_.find(boundary->object_id);
        if(end!=objects_.end()&&(end->second.signal.getKind()!=boundary->native_kind||
           end->second.signal.getTrackId()!=boundary->track_id||end->second.signal.getFraction()!=boundary->track_fraction))return {};
        RouteDestination destination;
        if(boundary->native_kind==NIMBY_SIGNAL_PATH&&boundary->flags==
            (NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY|NIMBY_SELECTED_ROUTE_PROTECTING_SIGNAL)&&
            end!=objects_.end()&&end->second.signal.getForwardDirection()==boundary->direction)destination=RouteDestination::ProtectingSignal;
        // Flag 8 qualifies an unconditional balise by revalidated native filter
        // facts. A scoped geometric snapshot may stop before this chosen branch.
        // Mere absence in that snapshot neither supplies nor cancels this fact.
        else if(boundary->native_kind==NIMBY_SIGNAL_BALISE&&
            boundary->flags==(NIMBY_SELECTED_ROUTE_FIRST_BOUNDARY|NIMBY_SELECTED_ROUTE_NATIVE_BOUNDARY|NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE)&&
            !(end!=objects_.end()&&end->second.filterKnown&&!end->second.unconditional)&&
            (boundary->flags&NIMBY_SELECTED_ROUTE_UNCONDITIONAL_BALISE))
            destination=RouteDestination::NativeBoundary;
        else return {};

        // Positive native marker facts before the qualified endpoint must be
        // reconcilable with the copied metadata, including a marker created or
        // moved into this route since the snapshot. Checking only snapshot
        // points already inside the route would silently miss those changes.
        for(const auto& annotation:annotations){
            if(annotation.native_kind!=NIMBY_SIGNAL_MARKER||annotation.distance_m>boundary->distance_m)continue;
            const auto marker=objects_.find(annotation.object_id);
            if(marker==objects_.end()||marker->second.signal.getKind()!=NIMBY_SIGNAL_MARKER||
               marker->second.signal.getTrackId()!=annotation.track_id||
               marker->second.signal.getFraction()!=annotation.track_fraction||!marker->second.marker)return {};
        }

        std::vector<BlockSection> extent;
        std::vector<SignalRouteMarker> markers;
        std::unordered_set<Id> markerIds,visited;
        Id previousTrack=0;double previousTo=0;int previousDirection=0;
        bool reached=false;
        for(const auto& part:parts){
            if(part.signal_id!=row.signal_id||part.train_id!=row.train_id||!identity(part.track_id,1)||
               !std::isfinite(part.begin_fraction)||!std::isfinite(part.end_fraction)||
               part.begin_fraction<0||part.begin_fraction>1||part.end_fraction<0||part.end_fraction>1||
               part.begin_fraction==part.end_fraction)return {};
            const int direction=part.end_fraction>part.begin_fraction?1:-1;
            if(extent.empty()){
                if(part.track_id==source.getTrackId()){
                    if(part.begin_fraction!=source.getFraction()||direction!=source.getForwardDirection())return {};
                }else if(source.getFraction()!=0&&source.getFraction()!=1)return {};
            }
            if(previousTrack==part.track_id){
                if(previousTo!=part.begin_fraction||previousDirection!=direction)return {};
            }else if(!visited.insert(part.track_id).second)return {}; // Re-entry/loop needs a stronger witness.
            double to=part.end_fraction;
            if(part.track_id==boundary->track_id&&direction==boundary->direction&&
               (boundary->track_fraction-part.begin_fraction)*direction>=0&&
               (to-boundary->track_fraction)*direction>=0){to=boundary->track_fraction;reached=true;}
            if(to!=part.begin_fraction){
                extent.push_back({part.track_id,std::min(part.begin_fraction,to),std::max(part.begin_fraction,to)});
                if(const auto group=markers_.find(part.track_id);group!=markers_.end()){
                    const auto add=[&](Id id)->bool{
                        const auto& object=objects_.at(id);const auto fraction=object.signal.getFraction();
                        if((fraction-part.begin_fraction)*direction<0||(to-fraction)*direction<0)return true;
                        if(part.track_id==source.getTrackId()&&fraction==source.getFraction())return true;
                        if(!markerIds.insert(id).second)return true; // Contact shared by two adjacent ranges.
                        const auto witnessed=std::find_if(annotations.begin(),annotations.end(),[&](const auto& point){
                            return point.object_id==id&&point.native_kind==NIMBY_SIGNAL_MARKER&&point.track_id==part.track_id&&
                                point.track_fraction==fraction&&point.direction==direction;
                        });
                        // Some passive markers do not emit a traversal record.
                        // Their position belongs to the fresh copied snapshot,
                        // which is not an atomic simulation tick. If the native
                        // scan did see this ID, contradictory positions must
                        // never be joined into a made-up route.
                        const bool nativeSaw=std::any_of(annotations.begin(),annotations.end(),[&](const auto& point){return point.object_id==id;});
                        if(nativeSaw&&witnessed==annotations.end())return false;
                        if(!object.marker||markers.size()==128)return false;
                        markers.push_back(*object.marker);return true;
                    };
                    if(direction>0){for(const auto id:group->second)if(!add(id))return {};}
                    else for(auto at=group->second.rbegin();at!=group->second.rend();++at)if(!add(*at))return {};
                }
            }
            if(reached)break;
            previousTrack=part.track_id;previousTo=to;previousDirection=direction;
        }
        if(!reached||extent.empty())return {};
        const auto occupation=reader.read(extent,BlockEntry{source.getTrackId(),source.getFraction(),source.getForwardDirection()});
        return {row.train_id,boundary->object_id,destination,occupation,std::move(markers),true};
    }
};
}
