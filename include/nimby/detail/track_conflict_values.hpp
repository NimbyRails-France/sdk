#pragma once
// Immutable native values for the SDK's central controller and adapter. The
// consumer contract remains Kotlin; this is not a second public client API.
#include <nimby/detail/observation_values.hpp>
#include <nimby/detail/track_conflicts.h>
namespace nimby {
struct TrackConflict {
    Id trackId{},otherTrackId{};
    double beginFraction{},endFraction{},otherBeginFraction{},otherEndFraction{};
};
class TrackConflictObservation {
public:
    using Ptr=std::shared_ptr<const TrackConflictObservation>;
    const GameSession& getGameSession()const noexcept{return gameSession_;}
    uint32_t getProcessId()const noexcept{return info_.process_id;}
    Milliseconds getAge()const{return std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now()-started_);}
    bool isOlderThan(Milliseconds age)const{return getAge()>age;}
    std::span<const NimbyTrackConflictState> getStates()const noexcept{return states_;}
    // nullopt means unrequested or unavailable; an empty span is a verified
    // complete list without conflicts. Neither is a native admission decision.
    // Views remain valid while this immutable observation is retained.
    std::optional<std::span<const TrackConflict>> getConflictsForTrack(Id id)const noexcept{
        const auto found=index_.find(id);if(found==index_.end()||!found->second.available)return std::nullopt;
        return std::span<const TrackConflict>(rows_).subspan(found->second.begin,found->second.count);
    }
    std::optional<uint32_t> getUnavailableReason(Id id)const noexcept{
        const auto found=index_.find(id);
        if(found==index_.end()||found->second.available)return std::nullopt;
        return states_[found->second.state].reason;
    }
private:
    friend class detail::ObservationSession;
    struct Range {size_t state{},begin{},count{};bool available{};};
    std::chrono::steady_clock::time_point started_;
    NimbySnapshotInfo info_{};GameSession gameSession_;
    std::vector<NimbyTrackConflictState> states_;
    std::vector<TrackConflict> rows_;
    std::unordered_map<Id,Range> index_;
    static Ptr capture(NimbySession session,std::span<const Id> ids){
        if(ids.empty()||ids.size()>NIMBY_TRACK_CONFLICTS_MAX_TRACKS)throw std::invalid_argument("Invalid conflict track scope");
        const auto started=std::chrono::steady_clock::now();detail::NativeSnapshot native;uint32_t stage{};
        const auto status=NimbyInternal_CaptureTrackConflictsSnapshot(session,ids.data(),static_cast<uint32_t>(ids.size()),&native.value,&stage);
        detail::check(status,
                      stage==1?"CaptureTrackConflicts/session":stage==2?"CaptureTrackConflicts/lists":"CaptureTrackConflicts");
        auto result=std::shared_ptr<TrackConflictObservation>(new TrackConflictObservation);result->started_=started;
        result->info_.struct_size=sizeof result->info_;
        detail::check(NimbyInternal_GetSnapshotInfo(native.value,&result->info_),"GetSnapshotInfo");
        NimbyGameSession game{};game.struct_size=sizeof game;
        detail::check(NimbyInternal_GetGameSession(native.value,&game),"GetGameSession");
        result->gameSession_.generation=game.generation;
        constexpr char hex[]="0123456789abcdef";
        for(auto byte:game.world_value){result->gameSession_.worldId+=hex[byte>>4];result->gameSession_.worldId+=hex[byte&15];}
        auto states=detail::copyRecords<NimbyTrackConflictState>([&](auto* rows,auto capacity,auto* count){return NimbyInternal_CopyTrackConflictStates(native.value,rows,capacity,count);},"CopyTrackConflictStates");
        auto rows=detail::copyRecords<NimbyTrackConflict>([&](auto* values,auto capacity,auto* count){return NimbyInternal_CopyTrackConflicts(native.value,values,capacity,count);},"CopyTrackConflicts");
        if(!states||!rows)detail::check(NIMBY_DATA_UNAVAILABLE,"CopyTrackConflicts");
        result->states_=std::move(*states);result->index_.reserve(result->states_.size());result->rows_.reserve(rows->size());
        for(size_t i=0;i<result->states_.size();++i){const auto& state=result->states_[i];
            result->index_.emplace(state.track_id,Range{i,0,0,bool(state.flags&NIMBY_TRACK_CONFLICTS_VALID)});
        }
        for(const auto& row:*rows){
            auto& range=result->index_.at(row.track_id);if(!range.count)range.begin=result->rows_.size();++range.count;
            result->rows_.push_back({row.track_id,row.other_track_id,row.begin_fraction,row.end_fraction,row.other_begin_fraction,row.other_end_fraction});
        }
        return result;
    }
};
}
