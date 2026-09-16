#include <nimby/detail/observation.h>
#include "engine/binary_identity.h"
#include "engine/network.h"
#include "engine/track_usage.h"
#include "engine/signal_textures.h"
#include <windows.h>
#include <tlhelp32.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>
#include <unordered_set>

namespace {
struct Session {
    HANDLE process{};
    uint32_t pid{};
    uint64_t base{};
    NimbyBinaryInfo binary{};
    std::filesystem::path game_directory;
    ~Session() { if(process) CloseHandle(process); }
};
struct Snapshot {
    NimbySnapshotInfo info{};
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
    std::vector<NimbyTrackNode> nodes;
    std::map<uint64_t,std::vector<uint64_t>> paths;
    std::vector<NimbyTrackUsage> reservations,occupations;
    bool reservations_available=false,occupations_available=false;
};
struct Registry {
    uint64_t next=1;
    std::map<uint64_t,std::unique_ptr<Session>> sessions;
    std::map<uint64_t,Snapshot> snapshots;
};
SRWLOCK registry_lock=SRWLOCK_INIT;
struct Guard { Guard(){AcquireSRWLockExclusive(&registry_lock);} ~Guard(){ReleaseSRWLockExclusive(&registry_lock);} };
Registry& registry() { static Registry value;return value; }
std::wstring from_utf8(const std::string& text) {
    const int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),nullptr,0);
    if(!n)return {};
    std::wstring result(n,L'\0');
    MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),result.data(),n);return result;
}
bool relative_asset(const std::filesystem::path& path) {
    if(path.empty()||path.has_root_path())return false;
    for(const auto& part:path)if(part==L".."||part==L"."||part.native().find(L':')!=std::wstring::npos)return false;
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
    if(!base.is_absolute()||!base.has_root_name()||base.native().starts_with(L"\\\\"))return;
    std::error_code ec;const auto root=std::filesystem::weakly_canonical(base,ec);if(ec)return;
    const auto path=std::filesystem::weakly_canonical(root/mod/relative,ec);if(ec)return;
    // Reject traversal through symlinks/junctions as well as textual '..'.
    const auto within=path.lexically_relative(root);
    if(!relative_asset(within)||!std::filesystem::is_regular_file(path,ec)||ec)return;
    const auto text=path.wstring();
    const int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),nullptr,0,nullptr,nullptr);
    if(n<=0||static_cast<size_t>(n)>=sizeof out.file_path_utf8)return;
    WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),static_cast<int>(text.size()),out.file_path_utf8,n,nullptr,nullptr);
    out.flags|=NIMBY_SIGNAL_TEXTURE_FILE_VALID;
}
bool read(void* context,uint64_t address,void* out,size_t size) {
    const auto& s=*static_cast<Session*>(context);
    SIZE_T got{};
    return size<=0x7fffffffffffULL&&address>=0x10000&&address<=0x7fffffffffffULL-size&&
        ReadProcessMemory(s.process,reinterpret_cast<void*>(address),out,size,&got)&&got==size;
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
        auto s=std::make_unique<Session>();s->pid=pid?pid:GetCurrentProcessId();
        s->process=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|SYNCHRONIZE,FALSE,s->pid);
        if(!s->process)return NIMBY_IO_ERROR;
        std::array<wchar_t,32768> path{};DWORD length=static_cast<DWORD>(path.size());
        if(!QueryFullProcessImageNameW(s->process,0,path.data(),&length))return NIMBY_IO_ERROR;
        s->game_directory=std::filesystem::path(path.data()).parent_path();
        auto result=nimby::engine::identify(path.data(),s->binary);
        if(result!=NIMBY_OK)return result;
        if(!s->binary.recognized_research_build)return NIMBY_UNSUPPORTED_GAME;
        const HANDLE modules=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,s->pid);
        if(modules==INVALID_HANDLE_VALUE)return NIMBY_IO_ERROR;
        MODULEENTRY32W module{};module.dwSize=sizeof module;
        const bool found=Module32FirstW(modules,&module)!=FALSE;
        CloseHandle(modules);
        if(!found)return NIMBY_IO_ERROR;
        s->base=reinterpret_cast<uint64_t>(module.modBaseAddr);
        if(WaitForSingleObject(s->process,0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
        const auto id=r.next++;
        r.sessions.emplace(id,std::move(s));*out=id;return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_CloseSession(NimbySession handle) noexcept {
    Guard guard;return registry().sessions.erase(handle)?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
uint32_t __cdecl NimbyInternal_ReleaseSnapshot(NimbySnapshot handle) noexcept {
    Guard guard;return registry().snapshots.erase(handle)?NIMBY_OK:NIMBY_INVALID_HANDLE;
}
uint32_t __cdecl NimbyInternal_CaptureSnapshot(NimbySession handle,NimbySnapshot* out) noexcept {
    if(!out)return NIMBY_INVALID_ARGUMENT;
    *out=0;
    try {
        Guard guard;auto& r=registry();auto found=r.sessions.find(handle);
        if(found==r.sessions.end())return NIMBY_INVALID_HANDLE;
        auto& session=*found->second;
        if(WaitForSingleObject(session.process,0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
        if(r.snapshots.size()>=16||!r.next)return NIMBY_RESOURCE_LIMIT;
        nimby::engine::LiveState state{},after{};
        nimby::engine::Network network;
        std::vector<nimby::engine::Train> trains;
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,state)||
           !nimby::engine::read_trains(read,&session,state,true,trains)||
           !nimby::engine::read_network(read,&session,state,true,network)||
           !nimby::engine::resolve_live_state(read,&session,session.base,true,after)||state!=after)return NIMBY_DATA_UNAVAILABLE;
        Snapshot snapshot;
        std::unordered_set<uint64_t> track_ids;
        for(const auto& t:network.tracks)track_ids.insert(t.id);
        std::map<uint64_t,uint64_t> track_stations;
        for(const auto& t:network.tracks)track_stations.emplace(t.id,t.station_id);
        std::unordered_set<uint64_t> station_ids;
        for(const auto& s:network.stations)station_ids.insert(s.id);
        std::unordered_set<uint64_t> train_ids;for(const auto& t:trains)train_ids.insert(t.id);
        auto usage=[&](auto reader,std::vector<NimbyTrackUsage>& dest){
            std::vector<nimby::engine::TrackUsage> values;
            if(!reader(read,&session,state,true,values))return false;
            for(const auto& v:values)if(!train_ids.contains(v.train_id)||!track_ids.contains(v.track_id))return false;
            for(const auto& v:values)dest.push_back({v.train_id,v.track_id,v.begin,v.end});
            return true;
        };
        snapshot.reservations_available=usage(nimby::engine::read_reservations,snapshot.reservations);
        snapshot.occupations_available=usage(nimby::engine::read_occupations,snapshot.occupations);
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,after)||state!=after)return NIMBY_DATA_UNAVAILABLE;
        for(const auto& t:trains) {
            NimbyTrain value{};value.id=t.id;
            std::memcpy(value.name_utf8,t.name.data(),t.name.size());
            if(t.present)value.flags|=NIMBY_TRAIN_ACTIVE_DRIVE;
            if(t.speed_available){
                value.flags|=NIMBY_TRAIN_SPEED_VALID;value.speed_mps=t.speed_mps;
                if(!t.present)value.flags|=NIMBY_TRAIN_SPEED_DEFAULTED;
            }
            if(t.positioned){
                if(!track_ids.contains(t.position.track_id))return NIMBY_DATA_UNAVAILABLE;
                value.flags|=NIMBY_TRAIN_POSITION_VALID;value.track_id=t.position.track_id;
                value.track_fraction=t.position.fraction;value.direction=t.position.direction;
            }
            snapshot.trains.push_back(value);
            auto service=t.service;service.train_id=t.id;
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
            auto stops=t.line_stops;
            for(auto& stop:stops)if(!stop.station_id&&track_stations.contains(stop.track_id))stop.station_id=track_stations.at(stop.track_id);
            if(t.line_stops_available&&std::all_of(stops.begin(),stops.end(),[&](const auto& stop){
                return track_stations.contains(stop.track_id)&&(!stop.station_id||station_ids.contains(stop.station_id))&&track_stations.at(stop.track_id)==stop.station_id;
            }))snapshot.line_stops.emplace(t.id,std::move(stops));
            if(t.path_available&&std::all_of(t.path.begin(),t.path.end(),[&](uint64_t id){return track_ids.contains(id);}))snapshot.paths.emplace(t.id,t.path);
        }
        snapshot.platforms=std::move(network.platforms);
        for(const auto& t:network.tracks)snapshot.tracks.push_back({t.id,t.station_id,t.limit_mps});
        for(const auto& s:network.stations){NimbyStation value{};value.id=s.id;std::memcpy(value.name_utf8,s.name.data(),s.name.size());snapshot.stations.push_back(value);}
        std::vector<nimby::engine::SignalTextureState> native_signal_states;
        std::map<uint64_t,int32_t> texture_states;
        nimby::engine::SignalTextureCatalog texture_catalog;
        const bool catalog_available=nimby::engine::read_signal_texture_catalog(read,&session,state,true,texture_catalog);
        if(nimby::engine::read_signal_texture_states(read,&session,state,true,native_signal_states))
            for(const auto& value:native_signal_states)texture_states.emplace(value.id,value.state);
        for(const auto& s:network.signals){
            snapshot.signals.push_back({s.id,s.track_id,s.fraction,s.direction,s.kind});
            NimbySignalState signal_state{};signal_state.signal_id=s.id;
            if(const auto it=texture_states.find(s.id);it!=texture_states.end()){
                signal_state.texture_state=it->second;
                signal_state.flags=NIMBY_SIGNAL_TEXTURE_STATE_VALID|NIMBY_SIGNAL_SPECIFIC_STATE_VALID;
                // An atlas-scoped native selector, not a guessed railway aspect.
                std::snprintf(signal_state.system_utf8,sizeof signal_state.system_utf8,"nimby:%016llx",static_cast<unsigned long long>(s.textures_hash));
                std::snprintf(signal_state.specific_state_utf8,sizeof signal_state.specific_state_utf8,"kind.%d.state.%d",s.kind,it->second);
            }
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
                    texture_file_path(session,texture_catalog,file,texture);
                }
            }
            snapshot.signal_textures.push_back(texture);
        }
        if(!nimby::engine::resolve_live_state(read,&session,session.base,true,after)||state!=after)return NIMBY_DATA_UNAVAILABLE;
        std::map<uint64_t,const nimby::engine::Track*> geometry;
        for(const auto& t:network.tracks)if(t.geometry)geometry.emplace(t.id,&t);
        for(const auto& [id,t]:geometry){
            // Branches can point into a main track without the reverse primary link.
            auto link=[&](uint64_t target){return target!=id&&geometry.contains(target)?target:uint64_t(0);};
            snapshot.nodes.push_back({id,link(t->links[0]),link(t->links[1]),t->x,t->y});
        }
        auto& info=snapshot.info;
        info.struct_size=sizeof info;info.abi_version=NIMBY_OBSERVATION_ABI_VERSION;info.process_id=session.pid;
        info.flags=NIMBY_SNAPSHOT_EXPERIMENTAL|NIMBY_SNAPSHOT_NON_ATOMIC;
        info.captured_unix_ms=static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        info.train_count=static_cast<uint32_t>(snapshot.trains.size());info.track_count=static_cast<uint32_t>(snapshot.tracks.size());
        info.station_count=static_cast<uint32_t>(snapshot.stations.size());info.signal_count=static_cast<uint32_t>(snapshot.signals.size());
        std::memcpy(info.game_sha256,session.binary.sha256,sizeof info.game_sha256);
        if(WaitForSingleObject(session.process,0)!=WAIT_TIMEOUT)return NIMBY_PROCESS_EXITED;
        auto id=r.next++;r.snapshots.emplace(id,std::move(snapshot));*out=id;return NIMBY_OK;
    }catch(...){return NIMBY_INTERNAL_ERROR;}
}
uint32_t __cdecl NimbyInternal_GetSnapshotInfo(NimbySnapshot handle,NimbySnapshotInfo* out) noexcept {
    if(!out||out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={};out->struct_size=sizeof *out;
    Guard guard;auto& r=registry();auto found=r.snapshots.find(handle);
    if(found==r.snapshots.end())return NIMBY_INVALID_HANDLE;
    *out=found->second.info;return NIMBY_OK;
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
uint32_t __cdecl NimbyInternal_CopyTrackNodes(NimbySnapshot s,NimbyTrackNode* out,uint32_t cap,uint32_t* count) noexcept {return copy(s,out,cap,count,&Snapshot::nodes);}
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
    default:return "Unknown status";}
}
