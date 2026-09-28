#include <nimby/detail/diagnostics.hpp>
#include <nimby/detail/observation.h>
#include <nimby/block_coverage.hpp>
#include "engine/binary_identity.h"
#include "engine/construction.h"
#include "engine/network.h"
#include "engine/driving.h"
#include "engine/track_usage.h"
#include "engine/signal_textures.h"
#include "engine/simulation_clock.h"
#include "runtime/observation_epoch.h"
#include "platform/observation_process.h"
#include <nimby/detail/platform/paths.hpp>
#include <mutex>
#include <thread>
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>
#include <unordered_set>

namespace {
// The registry owns sessions; a session owns its platform process connection.
// Captures own their records independently and remain valid after session close.
struct Session : nimby::platform::ObservationProcess {
    // Concurrent readers of one live process coordinate mod events using the
    // same epoch. The separate targeted-driving counter remains client-local.
    std::shared_ptr<nimby::runtime::ObservationEpoch> settings_epoch=std::make_shared<nimby::runtime::ObservationEpoch>();
    nimby::engine::LiveState driving_state{};
    uint64_t driving_generation{};
    int64_t driving_last_elapsed=-1;
};
struct Snapshot {
    NimbyGameSession game_session{};
    NimbySnapshotInfo info{};
    nimby::engine::SimulationClock clock{};
    bool clock_available=false;
    std::vector<NimbyTrain> trains;
    std::vector<NimbyTrainService> train_services;
    std::vector<NimbyTrainDetails> train_details;
    std::map<uint64_t,std::vector<NimbyLineStop>> line_stops;
    std::vector<NimbyTrack> tracks;
    std::vector<NimbyPlatform> platforms;
    std::vector<NimbyStation> stations;
    std::vector<NimbySignal> signals;
    std::vector<NimbySignalState> signal_states;
    std::vector<NimbySignalTexture> signal_textures;
    std::vector<NimbySignalExtensionsState> extension_states;
    std::vector<NimbySignalExtensionField> extension_fields;
    std::vector<NimbyTrackNode> nodes;
    std::vector<NimbyTrackJunction> junctions;
    std::vector<NimbyTrackMetric> metrics;
    bool metrics_available=false;
    std::map<uint64_t,std::vector<uint64_t>> paths;
    std::vector<NimbyTrackUsage> reservations,occupations;
    bool reservations_available=false,occupations_available=false;
};
struct Registry {
    // Monotonic IDs prevent a released handle from aliasing a later object.
    // Limits bound retained captures, not total bytes of all possible game saves.
    uint64_t next=1;
    std::map<uint64_t,std::unique_ptr<Session>> sessions;
    std::map<uint64_t,Snapshot> snapshots;
};
std::mutex registry_lock;
struct Guard : std::lock_guard<std::mutex> { Guard():std::lock_guard<std::mutex>(registry_lock){} };
Registry& registry() { static Registry value;return value; }
NimbySignalState signal_render_state(const nimby::engine::Signal& signal,bool table_available,
                                    const std::map<uint64_t,int32_t>& selectors) {
    NimbySignalState result{};result.signal_id=signal.id;
    if(signal.filter_available){
        result.flags|=NIMBY_SIGNAL_FILTER_VALID;result.exception_count=signal.exception_count;
        if(signal.filter_default_ignored)result.flags|=NIMBY_SIGNAL_FILTER_DEFAULT_IGNORED;
    }
    if(!table_available)return result;
    const auto it=selectors.find(signal.id);
    // Native renderer RVA 0x620140 initializes the selector to zero on a missing ID.
    // An unreadable table must not be confused with a successfully observed absence.
    result.texture_state=it==selectors.end()?0:it->second;
    result.flags|=NIMBY_SIGNAL_TEXTURE_STATE_VALID|NIMBY_SIGNAL_SPECIFIC_STATE_VALID;
    if(it==selectors.end())result.flags|=NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT;
    std::snprintf(result.system_utf8,sizeof result.system_utf8,"nimby:%016llx",static_cast<unsigned long long>(signal.textures_hash));
    std::snprintf(result.specific_state_utf8,sizeof result.specific_state_utf8,"kind.%d.state.%d",signal.kind,result.texture_state);
    return result;
}
std::filesystem::path from_utf8(const std::string& text) {
    return nimby::detail::platform::fromUtf8(text);
}
bool relative_asset(const std::filesystem::path& path) {
    if(path.empty()||path.has_root_path())return false;
    for(const auto& part:path)if(part==".."||part=="."||part.generic_string().find(':')!=std::string::npos)return false;
    return true;
}
void texture_file_path(const Session& session,const nimby::engine::SignalTextureCatalog& catalog,
                       const nimby::engine::SignalTextureFile& file,NimbySignalTexture& out) {
    const std::filesystem::path mod=from_utf8(file.mod),relative=from_utf8(file.relative_path);
    if(!relative_asset(mod)||!relative_asset(relative))return;
    std::filesystem::path base;
    if(file.source==0)base=session.game_directory/L"resources";
    else if(file.source==1)base=catalog.local_mod_root;
    else if(file.source==2){
        if(file.mod.empty()||file.mod.find_first_not_of("0123456789")!=std::string::npos)return;
        base=session.game_directory.parent_path().parent_path()/L"workshop"/L"content"/L"1134710";
    }
    if(!nimby::detail::platform::localAssetRoot(base))return;
    std::error_code ec;const auto root=std::filesystem::weakly_canonical(base,ec);if(ec)return;
    const auto path=std::filesystem::weakly_canonical(root/mod/relative,ec);if(ec)return;
    // Reject traversal through symlinks/junctions as well as textual '..'.
    const auto within=path.lexically_relative(root);
    if(!relative_asset(within)||!std::filesystem::is_regular_file(path,ec)||ec)return;
    if(!nimby::detail::platform::pathToUtf8(path,out.file_path_utf8,sizeof out.file_path_utf8))return;
    out.flags|=NIMBY_SIGNAL_TEXTURE_FILE_VALID;
}
bool read(void* context,uint64_t address,void* out,size_t size) {
    const auto& s=*static_cast<Session*>(context);
    return s.read(address,out,size);
}
template<class T> uint32_t copy(NimbySnapshot handle,T* records,uint32_t capacity,uint32_t* required,std::vector<T> Snapshot::*member,bool Snapshot::*available=nullptr) noexcept {
    if(!required)return NIMBY_INVALID_ARGUMENT;
    *required=0;
    if(!records&&capacity)return NIMBY_INVALID_ARGUMENT;
    Guard guard;
    auto& r=registry();auto found=r.snapshots.find(handle);
    if(found==r.snapshots.end())return NIMBY_INVALID_HANDLE;
    if(available&&!(found->second.*available))return NIMBY_DATA_UNAVAILABLE;
    const auto& values=found->second.*member;
    *required=static_cast<uint32_t>(values.size());
    if(!records&&!capacity)return NIMBY_OK;
    if(capacity<values.size())return NIMBY_BUFFER_TOO_SMALL;
    if(!values.empty())std::memcpy(records,values.data(),values.size()*sizeof(T));
    return NIMBY_OK;
}
}
uint32_t __cdecl NimbyInternal_OpenProcess(uint32_t abi,uint32_t pid,NimbySession* out) noexcept {
    if(!out)return NIMBY_INVALID_ARGUMENT;
    *out=0;if(abi!=NIMBY_OBSERVATION_ABI_VERSION)return NIMBY_INVALID_ARGUMENT;
    try {
        Guard guard;auto& r=registry();
        if(r.sessions.size()>=8||!r.next)return NIMBY_RESOURCE_LIMIT;
        auto s=std::make_unique<Session>();
        const auto result=s->open(pid);
        if(result!=NIMBY_OK)return result;
        for(const auto& [handle,existing]:r.sessions)
            if(existing->pid==s->pid&&existing->alive()){s->settings_epoch=existing->settings_epoch;break;}
        const auto id=r.next++;
        r.sessions.emplace(id,std::move(s));*out=id;return NIMBY_OK;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_CloseSession(NimbySession handle) noexcept {
    Guard guard;return registry().sessions.erase(handle)?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
uint32_t __cdecl NimbyInternal_ReleaseSnapshot(NimbySnapshot handle) noexcept {
    Guard guard;return registry().snapshots.erase(handle)?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
uint32_t __cdecl NimbyInternal_GetSimulationClock(NimbySnapshot handle,NimbySimulationClock* out) noexcept {
    if(!out || out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={sizeof *out,0,0,0};
    Guard guard;auto found=registry().snapshots.find(handle);
    if(found==registry().snapshots.end())return NIMBY_INVALID_HANDLE;
    if(!found->second.clock_available)return NIMBY_DATA_UNAVAILABLE;
    out->epoch_seconds=found->second.clock.epoch_seconds;out->ticks=found->second.clock.ticks;
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_SetSimulationDateTime(NimbySession handle,int64_t utc_seconds,NimbySimulationClock* out) noexcept {
    if(!out || out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={sizeof *out,0,0,0};
    if(utc_seconds<nimby::engine::calendar_min || utc_seconds>nimby::engine::calendar_max)return NIMBY_INVALID_ARGUMENT;
    Guard guard;auto found=registry().sessions.find(handle);
    if(found==registry().sessions.end())return NIMBY_INVALID_HANDLE;
    auto& session=*found->second;
    // Never suspend our own process. Normal observation sessions remain read-only.
    return session.setClock(utc_seconds,*out);
}
uint32_t __cdecl NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(
    NimbySession handle,int64_t utc,NimbySimulationClock* out,uint32_t* count) noexcept {
    if(!out || out->struct_size!=sizeof *out || !count)return NIMBY_INVALID_ARGUMENT;
    *out={sizeof *out,0,0,0};*count=0;
    if(utc<nimby::engine::calendar_min || utc>nimby::engine::calendar_max)return NIMBY_INVALID_ARGUMENT;
    Guard guard;auto found=registry().sessions.find(handle);
    if(found==registry().sessions.end())return NIMBY_INVALID_HANDLE;
    auto& session=*found->second;
    return session.setClockAndRecalculate(utc,*out,*count);
}
uint32_t __cdecl NimbyInternal_ReadTrainDriving(NimbySession handle,uint64_t id,NimbyDrivingObservation* out) noexcept {
    if(!out||out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={};out->struct_size=sizeof *out;
    if((id>>48)!=5)return NIMBY_INVALID_ARGUMENT;
    try {
        Guard guard;auto& r=registry();auto found=r.sessions.find(handle);
        if(found==r.sessions.end())return NIMBY_INVALID_HANDLE;
        auto& session=*found->second;
        if(!session.alive())return NIMBY_PROCESS_EXITED;
        nimby::engine::LiveState state{};
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,session.profile(),state)) {
            session.driving_state={};session.driving_last_elapsed=-1;return NIMBY_DATA_UNAVAILABLE;
        }
        if(session.driving_state!=state) {
            session.driving_state=state;
            if(session.driving_generation==UINT64_MAX)return NIMBY_RESOURCE_LIMIT;
            ++session.driving_generation;
            session.driving_last_elapsed=-1;
        }
        if(!nimby::engine::read_train_driving(read,&session,state,id,*out))return NIMBY_DATA_UNAVAILABLE;
        if(out->elapsed_end_ms<session.driving_last_elapsed) {
            if(session.driving_generation==UINT64_MAX){*out={};out->struct_size=sizeof *out;return NIMBY_RESOURCE_LIMIT;}
            ++session.driving_generation;
        }
        session.driving_last_elapsed=out->elapsed_end_ms;
        out->session_generation=session.driving_generation;
        out->captured_unix_ms=static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        return NIMBY_OK;
    } catch (...) { nimby::detail::diagnostics::exception("sdk", __func__);  *out={};out->struct_size=sizeof *out;return NIMBY_INTERNAL_ERROR; }
}
uint32_t __cdecl NimbyInternal_CaptureSnapshot(NimbySession handle,NimbySnapshot* out) noexcept {
    uint32_t stage{};
    return NimbyInternal_CaptureSnapshotDiagnostic(handle,out,&stage);
}
static uint32_t capture_snapshot(NimbySession handle,NimbySnapshot* out,uint32_t* stage,bool signallingOnly,std::span<const NimbySignalCaptureScope> scopes={}) noexcept;
uint32_t __cdecl NimbyInternal_CaptureSnapshotDiagnostic(NimbySession handle,NimbySnapshot* out,uint32_t* stage) noexcept {
    return capture_snapshot(handle,out,stage,false);
}
uint32_t __cdecl NimbyInternal_CaptureSignallingSnapshot(NimbySession handle,NimbySnapshot* out,uint32_t* stage) noexcept {
    return capture_snapshot(handle,out,stage,true);
}
uint32_t __cdecl NimbyInternal_CaptureSignallingFor(NimbySession handle,const char* textureSet,NimbySnapshot* out,uint32_t* stage) noexcept {
    if(!textureSet||!*textureSet||strnlen(textureSet,257)>256){if(out)*out=0;if(stage)*stage=0;return NIMBY_INVALID_ARGUMENT;}
    NimbySignalCaptureScope scope{};std::memcpy(scope.texture_set,textureSet,std::strlen(textureSet)+1);
    return capture_snapshot(handle,out,stage,true,std::span(&scope,1));
}
uint32_t __cdecl NimbyInternal_CaptureSignallingScope(NimbySession handle,const NimbySignalCaptureScope* scopes,uint32_t count,NimbySnapshot* out,uint32_t* stage) noexcept {
    if(out)*out=0;if(stage)*stage=0;
    if(!scopes||!count||count>16)return NIMBY_INVALID_ARGUMENT;
    for(uint32_t i=0;i<count;++i){
        if(!scopes[i].texture_set[0]||!std::memchr(scopes[i].texture_set,0,257)||scopes[i].approach_blocks>16)return NIMBY_INVALID_ARGUMENT;
        for(uint32_t j=0;j<i;++j)if(!std::strcmp(scopes[i].texture_set,scopes[j].texture_set))return NIMBY_INVALID_ARGUMENT;
    }
    return capture_snapshot(handle,out,stage,true,std::span(scopes,count));
}
uint32_t __cdecl NimbyInternal_CaptureSessionSnapshot(NimbySession handle,NimbySnapshot* out,uint32_t* stage) noexcept {
    if(out)*out=0;if(stage)*stage=0;
    if(!out||!stage)return NIMBY_INVALID_ARGUMENT;
    try {
        Guard guard;auto& r=registry();const auto found=r.sessions.find(handle);
        if(found==r.sessions.end())return NIMBY_INVALID_HANDLE;
        auto& session=*found->second;
        if(!session.alive())return NIMBY_PROCESS_EXITED;
        if(r.snapshots.size()>=16||!r.next)return NIMBY_RESOURCE_LIMIT;
        nimby::engine::VersioningObservation before{},after{};Snapshot snapshot;
        *stage=1;
        if(!nimby::engine::read_versioning_observation(read,&session,session.base,true,before,session.profile())||
           !nimby::engine::read_simulation_clock(read,&session,before.state.simulation,snapshot.clock)||
           !nimby::engine::read_versioning_observation(read,&session,session.base,true,after,session.profile())||
           before.state!=after.state||before.value!=after.value||before.history!=after.history){
            session.settings_epoch->unavailable();return NIMBY_DATA_UNAVAILABLE;
        }
        snapshot.clock_available=true;
        snapshot.game_session.struct_size=sizeof snapshot.game_session;
        snapshot.game_session.generation=session.settings_epoch->observe(after,snapshot.clock.ticks);
        std::memcpy(snapshot.game_session.world_value,after.value.data(),32);
        auto& info=snapshot.info;info.struct_size=sizeof info;info.abi_version=NIMBY_OBSERVATION_ABI_VERSION;
        info.process_id=session.pid;info.flags=NIMBY_SNAPSHOT_EXPERIMENTAL|NIMBY_SNAPSHOT_NON_ATOMIC;
        info.captured_unix_ms=static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        std::memcpy(info.game_sha256,session.binary.sha256,sizeof info.game_sha256);
        const auto id=r.next++;r.snapshots.emplace(id,std::move(snapshot));*out=id;*stage=0;return NIMBY_OK;
    }catch(...){nimby::detail::diagnostics::exception("sdk",__func__);return NIMBY_INTERNAL_ERROR;}
}
static uint32_t capture_snapshot(NimbySession handle,NimbySnapshot* out,uint32_t* stage,bool signallingOnly,std::span<const NimbySignalCaptureScope> scopes) noexcept {
    if(stage)*stage=0;
    if(out)*out=0;
    if(!stage)return NIMBY_INVALID_ARGUMENT;
    if(!out)return NIMBY_INVALID_ARGUMENT;
    *out=0;
    try {
        Guard guard;auto& r=registry();auto found=r.sessions.find(handle);
        if(found==r.sessions.end())return NIMBY_INVALID_HANDLE;
        auto& session=*found->second;
        if(!session.alive())return NIMBY_PROCESS_EXITED;
        if(r.snapshots.size()>=16||!r.next)return NIMBY_RESOURCE_LIMIT;
        nimby::engine::LiveState state{},after{};
        nimby::engine::VersioningObservation versionBefore{},versionAfter{};
        const bool versionAvailable=nimby::engine::read_versioning_observation(read,&session,session.base,true,versionBefore,session.profile());
        if(!versionAvailable)session.settings_epoch->unavailable();
        nimby::engine::Network network;
        std::vector<nimby::engine::Train> trains;
        *stage=1;
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,session.profile(),state))return NIMBY_DATA_UNAVAILABLE;
        *stage=3;
        nimby::engine::SignalTextureCatalog texture_catalog;
        bool catalog_available=false;
        const bool scoped=!scopes.empty();
        if(scoped){
            catalog_available=nimby::engine::read_signal_texture_catalog(read,&session,state,true,texture_catalog);
            if(!catalog_available)return NIMBY_DATA_UNAVAILABLE;
            std::vector<nimby::engine::SignallingScope> selected;
            for(const auto& scope:scopes){
                const nimby::engine::SignalTextureSet* match=nullptr;
                for(const auto& [hash,set]:texture_catalog.sets)if(set.name==scope.texture_set){if(match)return NIMBY_DATA_UNAVAILABLE;match=&set;}
                if(!match)return NIMBY_DATA_UNAVAILABLE;
                selected.push_back({match->hash,scope.approach_blocks});
            }
            if(!nimby::engine::read_signalling_network(read,&session,state,selected,network))return NIMBY_DATA_UNAVAILABLE;
        }else if(!nimby::engine::read_network(read,&session,state,true,network,signallingOnly))return NIMBY_DATA_UNAVAILABLE;
        *stage=4;
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,session.profile(),after)||state!=after)return NIMBY_DATA_UNAVAILABLE;
        // Topology traversal can be much slower than a simulation tick. Read
        // presence immediately before occupation, not before topology: otherwise
        // an entering/leaving train makes unrelated blocks lose coverage.
        *stage=2;
        if(!nimby::engine::read_trains(read,&session,state,true,trains,signallingOnly))return NIMBY_DATA_UNAVAILABLE;
        *stage=4;
        Snapshot snapshot;
        // These sizes come from already validated, owned engine records. Reserve
        // once per capture to avoid geometric growth and copying large ABI rows.
        // Capacity dies with the snapshot; no cache retains a previous world.
        snapshot.trains.reserve(trains.size());
        snapshot.train_services.reserve(trains.size());
        if(!signallingOnly)snapshot.train_details.reserve(trains.size());
        snapshot.tracks.reserve(network.tracks.size());
        snapshot.stations.reserve(network.stations.size());
        snapshot.signals.reserve(network.signals.size());
        snapshot.signal_states.reserve(network.signals.size());
        snapshot.signal_textures.reserve(network.signals.size());
        snapshot.extension_states.reserve(network.signals.size());
        snapshot.clock_available=nimby::engine::read_simulation_clock(read,&session,state.simulation,snapshot.clock);
        std::unordered_set<uint64_t> track_ids;
        track_ids.reserve(network.tracks.size());
        for(const auto& t:network.tracks)track_ids.insert(t.id);
        std::map<uint64_t,uint64_t> track_stations;
        for(const auto& t:network.tracks)track_stations.emplace(t.id,t.station_id);
        std::unordered_set<uint64_t> station_ids;
        station_ids.reserve(network.stations.size());
        for(const auto& s:network.stations)station_ids.insert(s.id);
        std::unordered_set<uint64_t> train_ids;
        train_ids.reserve(trains.size());
        for(const auto& t:trains)train_ids.insert(t.id);
        auto usage=[&](auto reader,std::vector<NimbyTrackUsage>& dest){
            std::vector<nimby::engine::TrackUsage> values;
            if(!reader(read,&session,state,true,values))return false;
            for(const auto& v:values)if(!train_ids.contains(v.train_id)||(!scoped&&!track_ids.contains(v.track_id)))return false;
            dest.reserve(values.size());
            for(const auto& v:values)dest.push_back({v.train_id,v.track_id,v.begin,v.end});
            return true;
        };
        if(!signallingOnly)snapshot.reservations_available=usage(nimby::engine::read_reservations,snapshot.reservations);
        snapshot.occupations_available=usage(nimby::engine::read_occupations,snapshot.occupations);
        if(signallingOnly) {
            // Both collections are mutable. Retry the pair, not only occupancy:
            // retaining earlier presence would repeat the same inconsistency.
            // Never combine attempts or accept an old clear observation.
            for(unsigned attempt=0;attempt<3;++attempt) {
                std::vector<nimby::TrainPresence> presence;
                std::vector<nimby::TrainFootprint> footprints;
                presence.reserve(trains.size());
                footprints.reserve(snapshot.occupations.size());
                for(const auto& t:trains) {
                    std::optional<bool> present;
                    if(t.service.flags&(NIMBY_SERVICE_PRESENCE_VALID|NIMBY_SERVICE_STATE_VALID)) {
                        const auto flags=t.service.motion_flags;
                        present=(flags&(NIMBY_MOTION_PRESENCE|NIMBY_MOTION_DRIVE|NIMBY_MOTION_HIDDEN)) && !(flags&NIMBY_MOTION_HIDDEN);
                    }
                    presence.push_back({t.id,present});
                }
                for(const auto& row:snapshot.occupations)
                    footprints.push_back({row.train_id,row.track_id,row.fraction_begin,row.fraction_end});
                if(nimby::checkBlockCoverage(presence,footprints,snapshot.occupations_available).verified || attempt==2)break;
                // Yield past a native update instead of rereading its halfway state.
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                *stage=2;
                if(!nimby::engine::read_trains(read,&session,state,true,trains,true))return NIMBY_DATA_UNAVAILABLE;
                train_ids.clear();for(const auto& t:trains)train_ids.insert(t.id);
                snapshot.occupations.clear();
                snapshot.occupations_available=usage(nimby::engine::read_occupations,snapshot.occupations);
            }
            snapshot.clock_available=nimby::engine::read_simulation_clock(read,&session,state.simulation,snapshot.clock);
        }
        *stage=5;
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,session.profile(),after)||state!=after)return NIMBY_DATA_UNAVAILABLE;
        for(auto& t:trains) {
            NimbyTrain value{};value.id=t.id;
            std::memcpy(value.name_utf8,t.name.data(),t.name.size());
            if(t.present)value.flags|=NIMBY_TRAIN_ACTIVE_DRIVE;
            if(t.speed_available){
                value.flags|=NIMBY_TRAIN_SPEED_VALID;value.speed_mps=t.speed_mps;
                if(!t.present)value.flags|=NIMBY_TRAIN_SPEED_DEFAULTED;
            }
            if(t.positioned){
                if(!track_ids.contains(t.position.track_id)){
                    // A scoped signalling capture may deliberately omit the
                    // track of a train outside the selected signal network.
                    if(!signallingOnly){*stage=6;return NIMBY_DATA_UNAVAILABLE;}
                }else{
                    value.flags|=NIMBY_TRAIN_POSITION_VALID;value.track_id=t.position.track_id;
                    value.track_fraction=t.position.fraction;value.direction=t.position.direction;
                }
            }
            snapshot.trains.push_back(value);
            auto service=t.service;service.train_id=t.id;
            if(signallingOnly){snapshot.train_services.push_back(service);continue;}
            if((service.flags&NIMBY_SERVICE_LOCATION_VALID)&&track_stations.contains(service.location_track_id))
                service.location_station_id=track_stations.at(service.location_track_id);
            else {service.flags&=~NIMBY_SERVICE_LOCATION_VALID;service.location_track_id=0;}
            if((service.flags&NIMBY_SERVICE_STOP_VALID)&&!service.stop_station_id&&track_stations.contains(service.stop_track_id))
                service.stop_station_id=track_stations.at(service.stop_track_id);
            if((service.flags&NIMBY_SERVICE_STOP_VALID)&&
               (!track_ids.contains(service.stop_track_id)||!station_ids.contains(service.stop_station_id)||
                track_stations.at(service.stop_track_id)!=service.stop_station_id)){
                service.flags&=~NIMBY_SERVICE_STOP_VALID;service.stop_track_id=service.stop_station_id=0;
            }
            if((service.status==NIMBY_SERVICE_STATION_STOP||service.status==NIMBY_SERVICE_DEPOT)&&
               (!(service.flags&NIMBY_SERVICE_STOP_VALID)||!service.location_station_id||
                service.location_station_id!=service.stop_station_id))service.status=NIMBY_SERVICE_TIMED_STOP;
            snapshot.train_services.push_back(service);
            snapshot.train_details.push_back(t.details);
            // Transfer capture-local storage after its final validation. No
            // pointer into game memory or another snapshot is retained.
            auto stops=std::move(t.line_stops);
            for(auto& stop:stops)if(!stop.station_id&&track_stations.contains(stop.track_id))stop.station_id=track_stations.at(stop.track_id);
            if(t.line_stops_available&&std::all_of(stops.begin(),stops.end(),[&](const auto& stop){
                return track_stations.contains(stop.track_id)&&(!stop.station_id||station_ids.contains(stop.station_id))&&track_stations.at(stop.track_id)==stop.station_id;
            }))snapshot.line_stops.emplace(t.id,std::move(stops));
            if(t.path_available&&std::all_of(t.path.begin(),t.path.end(),[&](uint64_t id){return track_ids.contains(id);}))snapshot.paths.emplace(t.id,std::move(t.path));
        }
        snapshot.platforms=std::move(network.platforms);
        for(const auto& t:network.tracks)snapshot.tracks.push_back({t.id,t.station_id,t.limit_mps});
        for(const auto& s:network.stations){NimbyStation value{};value.id=s.id;std::memcpy(value.name_utf8,s.name.data(),s.name.size());snapshot.stations.push_back(value);}
        std::vector<nimby::engine::SignalTextureState> native_signal_states;
        std::map<uint64_t,int32_t> texture_states;
        if(!scoped)catalog_available=nimby::engine::read_signal_texture_catalog(read,&session,state,true,texture_catalog);
        const bool states_available=nimby::engine::read_signal_texture_states(read,&session,state,true,native_signal_states);
        if(states_available)
            for(const auto& value:native_signal_states)texture_states.emplace(value.id,value.state);
        // Cache only within this capture: thousands of signals share a few assets.
        // A subsequent snapshot checks the filesystem again, including missing files.
        std::map<const nimby::engine::SignalTextureFile*,std::string> texture_paths;
        for(const auto& s:network.signals){
            size_t extension_records=0;
            for(const auto& extension:s.extensions)extension_records+=1+extension.fields.size();
            const bool extensions_available=s.extensions_available && extension_records<=65536-snapshot.extension_fields.size();
            snapshot.extension_states.push_back({s.id,extensions_available?1u:0u,0});
            if(extensions_available)for(const auto& extension:s.extensions){
                NimbySignalExtensionField record{};record.signal_id=s.id;record.script_id=extension.script;
                std::memcpy(record.type_name,extension.typeName.c_str(),extension.typeName.size()+1);
                snapshot.extension_fields.push_back(record);
                for(const auto& [name,value]:extension.fields){
                    std::memset(record.field_name,0,sizeof record.field_name);
                    std::memcpy(record.field_name,name.c_str(),name.size()+1);
                    const auto boolean=value.boolean();record.boolean_valid=boolean.has_value();record.boolean_value=boolean.value_or(false);
                    snapshot.extension_fields.push_back(record);
                }
            }
            snapshot.signals.push_back({s.id,s.track_id,s.fraction,s.direction,s.kind});
            const auto signal_state=signal_render_state(s,states_available,texture_states);
            snapshot.signal_states.push_back(signal_state);
            NimbySignalTexture texture{};texture.signal_id=s.id;
            if(catalog_available&&(signal_state.flags&NIMBY_SIGNAL_TEXTURE_STATE_VALID)){
                const auto* set=nimby::engine::select_signal_textures(texture_catalog,s.kind,s.textures_hash);
                if(set&&!set->files.empty()){
                    texture.selected_index=std::clamp(signal_state.texture_state,0,static_cast<int>(set->files.size())-1);
                    const auto& file=set->files[texture.selected_index];
                    texture.textures_hash=set->hash;texture.file_hash=file.hash;texture.source=file.source;
                    texture.state_count=static_cast<uint32_t>(set->files.size());texture.flags=NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID;
                    if(set->hash!=s.textures_hash)texture.flags|=NIMBY_SIGNAL_TEXTURE_DEFAULT_SET;
                    if(texture.selected_index!=signal_state.texture_state)texture.flags|=NIMBY_SIGNAL_TEXTURE_CLAMPED;
                    std::memcpy(texture.textures_id_utf8,set->name.c_str(),set->name.size()+1);
                    std::memcpy(texture.mod_id_utf8,file.mod.c_str(),file.mod.size()+1);
                    std::memcpy(texture.relative_path_utf8,file.relative_path.c_str(),file.relative_path.size()+1);
                    const auto [cached,inserted]=texture_paths.try_emplace(&file);
                    if(inserted&&!signallingOnly){
                        texture_file_path(session,texture_catalog,file,texture);
                        if(texture.flags&NIMBY_SIGNAL_TEXTURE_FILE_VALID)cached->second=texture.file_path_utf8;
                    }else if(!cached->second.empty()){
                        std::memcpy(texture.file_path_utf8,cached->second.c_str(),cached->second.size()+1);
                        texture.flags|=NIMBY_SIGNAL_TEXTURE_FILE_VALID;
                    }
                }
            }
            snapshot.signal_textures.push_back(texture);
        }
        *stage=7;
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,session.profile(),after)||state!=after)return NIMBY_DATA_UNAVAILABLE;
        std::map<uint64_t,const nimby::engine::Track*> geometry;
        for(const auto& t:network.tracks)if(t.geometry)geometry.emplace(t.id,&t);
        snapshot.nodes.reserve(geometry.size());
        snapshot.junctions.reserve(network.junctions.size());
        snapshot.metrics_available=!signallingOnly&&nimby::engine::gameLayout(state.profile).track_metric_offset!=0;
        if(snapshot.metrics_available)for(const auto& t:network.tracks)
            if(t.native_length_m>0)snapshot.metrics.push_back({t.id,t.native_length_m});
        for(const auto& [id,t]:geometry){
            // Branches can point into a main track without the reverse primary link.
            auto link=[&](uint64_t target){return target!=id&&(scoped||geometry.contains(target))?target:uint64_t(0);};
            snapshot.nodes.push_back({id,link(t->links[0]),link(t->links[1]),t->x,t->y});
        }
        for(const auto& junction:network.junctions)
            if(geometry.contains(junction.branch_track_id)&&geometry.contains(junction.main_track_id))
                snapshot.junctions.push_back(junction);
        auto& info=snapshot.info;
        info.struct_size=sizeof info;info.abi_version=NIMBY_OBSERVATION_ABI_VERSION;info.process_id=session.pid;
        info.flags=NIMBY_SNAPSHOT_EXPERIMENTAL|NIMBY_SNAPSHOT_NON_ATOMIC;
        info.captured_unix_ms=static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        info.train_count=static_cast<uint32_t>(snapshot.trains.size());info.track_count=static_cast<uint32_t>(snapshot.tracks.size());
        info.station_count=static_cast<uint32_t>(snapshot.stations.size());info.signal_count=static_cast<uint32_t>(snapshot.signals.size());
        std::memcpy(info.game_sha256,session.binary.sha256,sizeof info.game_sha256);
        if(versionAvailable&&versionBefore.state==state&&
           nimby::engine::read_versioning_observation(read,&session,session.base,true,versionAfter,session.profile())&&
           versionAfter.state==state&&versionBefore.value==versionAfter.value&&versionBefore.history==versionAfter.history){
            snapshot.game_session.struct_size=sizeof(snapshot.game_session);
            snapshot.game_session.generation=session.settings_epoch->observe(versionAfter,
                snapshot.clock_available?std::optional<int64_t>{snapshot.clock.ticks}:std::nullopt);
            std::memcpy(snapshot.game_session.world_value,versionAfter.value.data(),32);
        }else session.settings_epoch->unavailable();
        if(!session.alive())return NIMBY_PROCESS_EXITED;
        auto id=r.next++;r.snapshots.emplace(id,std::move(snapshot));*out=id;*stage=0;return NIMBY_OK;
    }catch(...){ nimby::detail::diagnostics::exception("sdk", __func__); return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_GetSnapshotInfo(NimbySnapshot handle,NimbySnapshotInfo* out) noexcept {
    if(!out||out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={};out->struct_size=sizeof *out;
    Guard guard;auto& r=registry();auto found=r.snapshots.find(handle);
    if(found==r.snapshots.end())return NIMBY_INVALID_HANDLE;
    *out=found->second.info;return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_GetGameSession(NimbySnapshot handle,NimbyGameSession* out) noexcept {
    if(!out||out->struct_size!=sizeof(*out))return NIMBY_INVALID_ARGUMENT;
    *out={};out->struct_size=sizeof(*out);
    Guard guard;const auto found=registry().snapshots.find(handle);
    if(found==registry().snapshots.end())return NIMBY_INVALID_HANDLE;
    if(!found->second.game_session.generation)return NIMBY_DATA_UNAVAILABLE;
    *out=found->second.game_session;return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_CopyTrains(NimbySnapshot s,NimbyTrain* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::trains);}
uint32_t __cdecl NimbyInternal_CopyTrainServices(NimbySnapshot s,NimbyTrainService* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::train_services);}
uint32_t __cdecl NimbyInternal_CopyTrackReservations(NimbySnapshot s,NimbyTrackUsage* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::reservations,&Snapshot::reservations_available);}
uint32_t __cdecl NimbyInternal_CopyTrackOccupations(NimbySnapshot s,NimbyTrackUsage* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::occupations,&Snapshot::occupations_available);}
uint32_t __cdecl NimbyInternal_CopyTracks(NimbySnapshot s,NimbyTrack* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::tracks);}
uint32_t __cdecl NimbyInternal_CopyPlatforms(NimbySnapshot s,NimbyPlatform* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::platforms);}
uint32_t __cdecl NimbyInternal_CopyStations(NimbySnapshot s,NimbyStation* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::stations);}
uint32_t __cdecl NimbyInternal_CopySignals(NimbySnapshot s,NimbySignal* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::signals);}
uint32_t __cdecl NimbyInternal_CopySignalStates(NimbySnapshot s,NimbySignalState* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::signal_states);}
uint32_t __cdecl NimbyInternal_CopySignalTextures(NimbySnapshot s,NimbySignalTexture* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::signal_textures);}
uint32_t __cdecl NimbyInternal_CopySignalExtensionsStates(NimbySnapshot s,NimbySignalExtensionsState* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::extension_states);}
uint32_t __cdecl NimbyInternal_CopySignalExtensionFields(NimbySnapshot s,NimbySignalExtensionField* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::extension_fields);}
uint32_t __cdecl NimbyInternal_CopyTrackNodes(NimbySnapshot s,NimbyTrackNode* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::nodes);}
uint32_t __cdecl NimbyInternal_CopyTrackJunctions(NimbySnapshot s,NimbyTrackJunction* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::junctions);}
uint32_t __cdecl NimbyInternal_CopyTrackMetrics(NimbySnapshot s,NimbyTrackMetric* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::metrics,&Snapshot::metrics_available);}
uint32_t __cdecl NimbyInternal_CopyTrainDetails(NimbySnapshot s,NimbyTrainDetails* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::train_details);}
uint32_t __cdecl NimbyInternal_CopyTrainLineStops(NimbySnapshot handle,uint64_t train,NimbyLineStop* out,uint32_t cap,uint32_t* count) noexcept {
    if(!count)return NIMBY_INVALID_ARGUMENT;
    *count=0;
    if(!out&&cap)return NIMBY_INVALID_ARGUMENT;
    Guard guard;auto s=registry().snapshots.find(handle);if(s==registry().snapshots.end())return NIMBY_INVALID_HANDLE;
    auto p=s->second.line_stops.find(train);if(p==s->second.line_stops.end())return NIMBY_DATA_UNAVAILABLE;
    *count=static_cast<uint32_t>(p->second.size());if(!out&&!cap)return NIMBY_OK;
    if(cap<p->second.size())return NIMBY_BUFFER_TOO_SMALL;
    if(!p->second.empty())std::memcpy(out,p->second.data(),p->second.size()*sizeof(NimbyLineStop));
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_CopyTrainPathTracks(NimbySnapshot handle,uint64_t train,uint64_t* out,uint32_t cap,uint32_t* count) noexcept {
    if(!count)return NIMBY_INVALID_ARGUMENT;
    *count=0;
    if(!out&&cap)return NIMBY_INVALID_ARGUMENT;
    Guard guard;auto s=registry().snapshots.find(handle);if(s==registry().snapshots.end())return NIMBY_INVALID_HANDLE;
    auto p=s->second.paths.find(train);if(p==s->second.paths.end())return NIMBY_DATA_UNAVAILABLE;
    *count=static_cast<uint32_t>(p->second.size());if(!out&&!cap)return NIMBY_OK;
    if(cap<p->second.size())return NIMBY_BUFFER_TOO_SMALL;
    if(!p->second.empty())std::memcpy(out,p->second.data(),p->second.size()*8);
    return NIMBY_OK;
}
const char* __cdecl NimbyInternal_StatusString(uint32_t status) noexcept {
    switch(status){
    case NIMBY_OK:return "OK";case NIMBY_INVALID_ARGUMENT:return "Invalid argument or ABI version";
    case NIMBY_IO_ERROR:return "Process or file access failed";case NIMBY_INVALID_BINARY:return "Invalid binary";
    case NIMBY_ALREADY_INITIALIZED:return "Already initialized";case NIMBY_HOOKS_UNAVAILABLE:return "Hooks unavailable";
    case NIMBY_INTERNAL_ERROR:return "Internal error";case NIMBY_UNSUPPORTED_GAME:return "Unsupported game binary";
    case NIMBY_DATA_UNAVAILABLE:return "Game data unavailable or changed; retry later";
    case NIMBY_INVALID_HANDLE:return "Invalid or released handle";case NIMBY_BUFFER_TOO_SMALL:return "Buffer too small";
    case NIMBY_PROCESS_EXITED:return "Process exited; open a new session";case NIMBY_RESOURCE_LIMIT:return "Resource limit; release handles";
    case NIMBY_CLOCK_WRITE_FAILED:return "Calendar write or verification failed; re-read the simulation clock";
    default:return "Unknown status";}
}
uint32_t __cdecl NimbyInternal_Construction(uint64_t session,const NimbyConstructionRequest* request,NimbyConstructionResult* out) noexcept {
    if(!request||!out||out->size!=sizeof(*out)||!nimby::engine::construction::valid(*request))return NIMBY_INVALID_ARGUMENT;
    Guard guard;auto found=registry().sessions.find(session);
    if(found==registry().sessions.end())return NIMBY_INVALID_HANDLE;
    return found->second->construction(request,0,*out);
}
uint32_t __cdecl NimbyInternal_ConstructionPoll(uint64_t session,uint64_t token,NimbyConstructionResult* out) noexcept {
    if(!out||out->size!=sizeof(*out)||!token)return NIMBY_INVALID_ARGUMENT;
    Guard guard;auto found=registry().sessions.find(session);
    if(found==registry().sessions.end())return NIMBY_INVALID_HANDLE;
    return found->second->construction(nullptr,token,*out);
}
