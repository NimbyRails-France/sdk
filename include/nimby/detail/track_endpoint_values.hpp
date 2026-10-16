#pragma once
// Immutable SDK-internal values. Mods consume the Kotlin API; this adapter
// transport deliberately exposes no pointer, layout offset or native call.
#include <nimby/detail/observation_values.hpp>
#include <nimby/detail/track_endpoints.h>

namespace nimby {
enum class TrackEndpointKind : uint32_t { Unknown=0, Connected=1, Terminal=2 };
struct TrackEndpoint {
    TrackEndpointKind kind=TrackEndpointKind::Unknown;
    uint32_t unavailableReason=NIMBY_TRACK_ENDPOINT_TRACK_UNAVAILABLE;
    bool isTerminal()const noexcept{return kind==TrackEndpointKind::Terminal;}
};
struct TrackEndpoints {
    Id trackId{};std::optional<Id> stationId;TrackEndpoint a,b;
    TrackEndpoint at(int direction)const noexcept{
        if(direction==-1)return a;
        if(direction==1)return b;
        return {};
    }
};
class TrackEndpointObservation {
public:
    using Ptr=std::shared_ptr<const TrackEndpointObservation>;
    const GameSession& getGameSession()const noexcept{return gameSession_;}
    Milliseconds getAge()const{return std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now()-started_);}
    bool isOlderThan(Milliseconds age)const{return getAge()>age;}
    const TrackEndpoints* getTrack(Id id)const noexcept{
        const auto found=index_.find(id);return found==index_.end()?nullptr:&rows_[found->second];
    }
    std::span<const TrackEndpoints> getTracks()const noexcept{return rows_;}
private:
    friend class detail::ObservationSession;
    std::chrono::steady_clock::time_point started_;GameSession gameSession_;
    std::vector<TrackEndpoints> rows_;std::unordered_map<Id,size_t> index_;
    static Ptr capture(NimbySession session,std::span<const Id> ids){
        if(ids.empty()||ids.size()>NIMBY_TRACK_ENDPOINTS_MAX_TRACKS)throw std::invalid_argument("Invalid endpoint track scope");
        const auto started=std::chrono::steady_clock::now();detail::NativeSnapshot native;uint32_t stage{};
        detail::check(NimbyInternal_CaptureTrackEndpointsSnapshot(session,ids.data(),static_cast<uint32_t>(ids.size()),&native.value,&stage),
                      stage==1?"CaptureTrackEndpoints/session":stage==2?"CaptureTrackEndpoints/topology":"CaptureTrackEndpoints");
        auto result=std::shared_ptr<TrackEndpointObservation>(new TrackEndpointObservation);result->started_=started;
        NimbyGameSession game{};game.struct_size=sizeof game;
        detail::check(NimbyInternal_GetGameSession(native.value,&game),"GetGameSession");
        result->gameSession_.generation=game.generation;
        constexpr char hex[]="0123456789abcdef";
        for(auto byte:game.world_value){result->gameSession_.worldId+=hex[byte>>4];result->gameSession_.worldId+=hex[byte&15];}
        auto states=detail::copyRecords<NimbyTrackEndpointStates>([&](auto* rows,auto capacity,auto* count){return NimbyInternal_CopyTrackEndpointStates(native.value,rows,capacity,count);},"CopyTrackEndpointStates");
        if(!states)detail::check(NIMBY_DATA_UNAVAILABLE,"CopyTrackEndpointStates");
        result->rows_.reserve(states->size());result->index_.reserve(states->size());
        for(const auto& row:*states){
            result->index_.emplace(row.track_id,result->rows_.size());
            result->rows_.push_back({row.track_id,detail::reference(row.station_id),
                {static_cast<TrackEndpointKind>(row.state_a),row.reason_a},
                {static_cast<TrackEndpointKind>(row.state_b),row.reason_b}});
        }
        return result;
    }
};
}
