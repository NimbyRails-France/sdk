#include <nimby/detail/observation.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <cmath>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(int argc,char** argv) {
    static_assert(sizeof(NimbyTrackUsage)==32);
    NimbySession session=123;NimbySnapshot snapshot=123;uint32_t count=99;
    NimbySimulationClock clock{};clock.struct_size=sizeof clock;
    CHECK(NimbyInternal_GetSimulationClock(0,&clock)==NIMBY_INVALID_HANDLE);
    CHECK(NimbyInternal_SetSimulationDateTime(0,-946771200,&clock)==NIMBY_INVALID_HANDLE);
    CHECK(NimbyInternal_SetSimulationDateTime(0,INT64_MAX,&clock)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_SetSimulationDateTime(0,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(0,0,&clock,&count)==NIMBY_INVALID_HANDLE);
    CHECK(count==0);
    CHECK(NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(0,INT64_MAX,&clock,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(0,0,&clock,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_OpenProcess(99,0,&session)==NIMBY_INVALID_ARGUMENT&&session==0);
    CHECK(NimbyInternal_OpenProcess(1,0,&session)==NIMBY_INVALID_ARGUMENT&&session==0);
    CHECK(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,0,&session)==NIMBY_UNSUPPORTED_GAME&&session==0);
    CHECK(NimbyInternal_CaptureSnapshot(0,&snapshot)==NIMBY_INVALID_HANDLE&&snapshot==0);
    CHECK(NimbyInternal_CopyTrains(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrains(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyTrainServices(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrainServices(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbyInternal_CopyTrainServices(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopySignalStates(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopySignalStates(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbyInternal_CopySignalStates(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopySignalTextures(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopySignalTextures(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbyInternal_CopySignalTextures(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyTrackNodes(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrackReservations(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrackOccupations(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrackReservations(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbyInternal_CopyTrackOccupations(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyTrainPathTracks(0,0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CloseSession(0)==NIMBY_INVALID_HANDLE);
    CHECK(NimbyInternal_ReleaseSnapshot(0)==NIMBY_INVALID_HANDLE);
    static_assert(sizeof(NimbyLineStop)==40&&sizeof(NimbyTrainDetails)==40);
    CHECK(NimbyInternal_CopyTrainDetails(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrainDetails(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyTrainDetails(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyTrainLineStops(0,0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyTrainLineStops(0,0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyTrainLineStops(0,0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    static_assert(sizeof(NimbyPlatform)==288);
    CHECK(NimbyInternal_CopyPlatforms(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbyInternal_CopyPlatforms(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbyInternal_CopyPlatforms(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    NimbySnapshotInfo info{};
    CHECK(NimbyInternal_GetSnapshotInfo(0,&info)==NIMBY_INVALID_ARGUMENT);
    if(argc==2) {
        CHECK(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,static_cast<uint32_t>(std::strtoul(argv[1],nullptr,10)),&session)==NIMBY_OK);
        CHECK(NimbyInternal_CaptureSnapshot(session,&snapshot)==NIMBY_OK);
        info.struct_size=sizeof info;
        CHECK(NimbyInternal_GetSnapshotInfo(snapshot,&info)==NIMBY_OK);
        CHECK(info.abi_version==NIMBY_OBSERVATION_ABI_VERSION&&info.flags==(NIMBY_SNAPSHOT_EXPERIMENTAL|NIMBY_SNAPSHOT_NON_ATOMIC));
        CHECK(NimbyInternal_CopyTrains(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.train_count&&count>0);
        NimbyTrain sentinel{};std::memset(&sentinel,0x5a,sizeof sentinel);auto previous=sentinel;
        CHECK(NimbyInternal_CopyTrains(snapshot,&sentinel,0,&count)==NIMBY_BUFFER_TOO_SMALL);
        CHECK(std::memcmp(&sentinel,&previous,sizeof sentinel)==0);
        std::vector<NimbyTrain> before(count),after(count);
        CHECK(NimbyInternal_CopyTrains(snapshot,before.data(),count,&count)==NIMBY_OK);
        uint32_t serviceCount{};
        CHECK(NimbyInternal_CopyTrainServices(snapshot,nullptr,0,&serviceCount)==NIMBY_OK&&serviceCount==count);
        std::vector<NimbyTrainService> services(serviceCount);
        NimbyTrainService serviceSentinel{};std::memset(&serviceSentinel,0x5a,sizeof serviceSentinel);
        const auto servicePrevious=serviceSentinel;
        CHECK(NimbyInternal_CopyTrainServices(snapshot,&serviceSentinel,0,&serviceCount)==NIMBY_BUFFER_TOO_SMALL);
        CHECK(std::memcmp(&servicePrevious,&serviceSentinel,sizeof serviceSentinel)==0);
        CHECK(NimbyInternal_CopyTrainServices(snapshot,services.data(),serviceCount,&serviceCount)==NIMBY_OK);
        unsigned servicesResolved=0;
        for(size_t i=0;i<services.size();++i){
            const auto& s=services[i];CHECK(s.train_id==before[i].id);
            if(s.flags&NIMBY_SERVICE_STATE_VALID)++servicesResolved;
            if(s.status==NIMBY_SERVICE_DEPOT)CHECK(s.line_kind==1&&(s.motion_flags&NIMBY_MOTION_HIDDEN)&&s.location_station_id==s.stop_station_id&&s.stop_station_id);
            if(s.status==NIMBY_SERVICE_STATION_STOP)CHECK((s.motion_flags&NIMBY_MOTION_TIMED_STOP)&&s.location_station_id==s.stop_station_id&&s.stop_station_id);
            if(s.flags&NIMBY_SERVICE_DEPARTURE_VALID)CHECK(s.motion_flags&NIMBY_MOTION_TIMED_STOP);
            if(s.flags&NIMBY_SERVICE_CLOCK_VALID)CHECK(s.departure_remaining_seconds>=0&&s.dispatch_remaining_seconds>=0);
        }
        std::printf("Train services resolved: %u/%u\n",servicesResolved,serviceCount);
        uint32_t detailCount{};CHECK(NimbyInternal_CopyTrainDetails(snapshot,nullptr,0,&detailCount)==NIMBY_OK&&detailCount==before.size());
        std::vector<NimbyTrainDetails> details(detailCount);
        CHECK(NimbyInternal_CopyTrainDetails(snapshot,details.data(),detailCount,&detailCount)==NIMBY_OK);
        uint32_t platformCount{};CHECK(NimbyInternal_CopyPlatforms(snapshot,nullptr,0,&platformCount)==NIMBY_OK);
        std::vector<NimbyPlatform> platforms(platformCount);
        CHECK(NimbyInternal_CopyPlatforms(snapshot,platforms.data(),platformCount,&platformCount)==NIMBY_OK);
        unsigned named=0;std::unordered_set<uint64_t> platformTracks;
        for(const auto& p:platforms){CHECK(p.track_id>>48==1&&p.station_id>>48==2&&platformTracks.insert(p.track_id).second);if(p.flags&NIMBY_PLATFORM_NAME_VALID)++named;}
        std::printf("Platforms: %u; named: %u\n",platformCount,named);
        unsigned plans=0,passengers=0,assignments=0;
        for(size_t i=0;i<details.size();++i){
            CHECK(details[i].train_id==before[i].id);
            if(details[i].flags&NIMBY_TRAIN_PASSENGERS_VALID){CHECK(details[i].passenger_count>=0);++passengers;}
            if(details[i].flags&NIMBY_TRAIN_ASSIGNMENT_VALID){CHECK(details[i].schedule_id>>48==6&&details[i].shift_id&&details[i].order_index>=0);++assignments;}
            uint32_t n{};auto status=NimbyInternal_CopyTrainLineStops(snapshot,details[i].train_id,nullptr,0,&n);
            CHECK(status==NIMBY_OK||status==NIMBY_DATA_UNAVAILABLE);
            if(status==NIMBY_OK){
                ++plans;std::vector<NimbyLineStop> stops(n);
                CHECK(NimbyInternal_CopyTrainLineStops(snapshot,details[i].train_id,stops.data(),n,&n)==NIMBY_OK);
                for(size_t j=0;j<stops.size();++j){CHECK(stops[j].index==j&&stops[j].line_id==services[i].line_id);}
                if(n){auto sentinel=stops[0],old=sentinel;CHECK(NimbyInternal_CopyTrainLineStops(snapshot,details[i].train_id,&sentinel,0,&n)==NIMBY_BUFFER_TOO_SMALL);CHECK(!std::memcmp(&old,&sentinel,sizeof old));}
            }
        }
        std::printf("Line plans: %u; passenger counts: %u; assignments: %u\n",plans,passengers,assignments);
        for(const auto& t:before){
            if(t.flags&NIMBY_TRAIN_ACTIVE_DRIVE)CHECK(t.flags&NIMBY_TRAIN_SPEED_VALID);
            if(t.flags&NIMBY_TRAIN_SPEED_VALID)CHECK(std::isfinite(t.speed_mps));
            if(t.flags&NIMBY_TRAIN_SPEED_DEFAULTED){
                CHECK(t.flags&NIMBY_TRAIN_SPEED_VALID);
                CHECK(!(t.flags&NIMBY_TRAIN_ACTIVE_DRIVE)&&t.speed_mps==0);
            }
        }
        uint32_t nodeCount=0;CHECK(NimbyInternal_CopyTrackNodes(snapshot,nullptr,0,&nodeCount)==NIMBY_OK);
        std::vector<NimbyTrackNode> nodes(nodeCount);CHECK(NimbyInternal_CopyTrackNodes(snapshot,nodes.data(),nodeCount,&nodeCount)==NIMBY_OK);
        std::unordered_set<uint64_t> nodeIds;for(auto& n:nodes)nodeIds.insert(n.id);
        for(auto& n:nodes)CHECK((!n.link_a||nodeIds.contains(n.link_a))&&(!n.link_b||nodeIds.contains(n.link_b)));
        unsigned pathCount=0;
        for(auto& t:before){uint32_t n=0;auto status=NimbyInternal_CopyTrainPathTracks(snapshot,t.id,nullptr,0,&n);
            CHECK(status==NIMBY_OK||status==NIMBY_DATA_UNAVAILABLE);
            if(status==NIMBY_OK){std::vector<uint64_t> ids(n);CHECK(NimbyInternal_CopyTrainPathTracks(snapshot,t.id,ids.data(),n,&n)==NIMBY_OK);
                for(auto id:ids)CHECK(nodeIds.contains(id));
                ++pathCount;}}
        std::printf("Experimental graph: %u nodes, %u available train Paths.\n",nodeCount,pathCount);
        std::unordered_set<uint64_t> trainIds;for(const auto& t:before)trainIds.insert(t.id);
        for(auto function:{NimbyInternal_CopyTrackReservations,NimbyInternal_CopyTrackOccupations}){
            uint32_t n=0;const auto status=function(snapshot,nullptr,0,&n);
            CHECK(status==NIMBY_OK||status==NIMBY_DATA_UNAVAILABLE);
            if(status==NIMBY_DATA_UNAVAILABLE){CHECK(n==0);continue;}
            std::vector<NimbyTrackUsage> a(n),b(n);
            if(n){NimbyTrackUsage guard{};std::memset(&guard,0x5a,sizeof guard);auto old=guard;
                CHECK(function(snapshot,&guard,0,&n)==NIMBY_BUFFER_TOO_SMALL);
                CHECK(std::memcmp(&guard,&old,sizeof guard)==0);}
            CHECK(function(snapshot,a.data(),n,&n)==NIMBY_OK);
            CHECK(function(snapshot,b.data(),n,&n)==NIMBY_OK);
            CHECK(a.empty()||std::memcmp(a.data(),b.data(),n*sizeof(NimbyTrackUsage))==0);
            for(const auto& u:a)CHECK(trainIds.contains(u.train_id)&&nodeIds.contains(u.track_id)&&
                std::isfinite(u.fraction_begin)&&std::isfinite(u.fraction_end)&&u.fraction_begin>=0&&u.fraction_end<=1&&u.fraction_begin<=u.fraction_end);
            std::printf("Track usage: %u records; IDs, ranges, buffers and immutability passed.\n",n);
        }
        CHECK(NimbyInternal_CloseSession(session)==NIMBY_OK);
        NimbySnapshot rejected=99;CHECK(NimbyInternal_CaptureSnapshot(session,&rejected)==NIMBY_INVALID_HANDLE&&rejected==0);
        CHECK(NimbyInternal_CopyTrains(snapshot,after.data(),count,&count)==NIMBY_OK);
        CHECK(std::memcmp(before.data(),after.data(),count*sizeof(NimbyTrain))==0);
        CHECK(NimbyInternal_CopyTracks(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.track_count);
        CHECK(NimbyInternal_CopyStations(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.station_count);
        std::vector<NimbyStation> stations(count);
        CHECK(NimbyInternal_CopyStations(snapshot,stations.data(),count,&count)==NIMBY_OK);
        unsigned namedStations=0;
        for(const auto& station:stations){
            CHECK(std::memchr(station.name_utf8,0,sizeof station.name_utf8));
            if(station.name_utf8[0])++namedStations;
        }
        std::printf("Station names resolved: %u/%u\n",namedStations,count);
        CHECK(NimbyInternal_CopySignals(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        std::vector<NimbySignal> signals(count);
        CHECK(NimbyInternal_CopySignals(snapshot,signals.data(),count,&count)==NIMBY_OK);
        CHECK(NimbyInternal_CopySignalStates(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        if(count){
            NimbySignalState guard{};std::memset(&guard,0x5a,sizeof guard);auto old=guard;
            CHECK(NimbyInternal_CopySignalStates(snapshot,&guard,0,&count)==NIMBY_BUFFER_TOO_SMALL);
            CHECK(std::memcmp(&guard,&old,sizeof guard)==0);
        }
        std::vector<NimbySignalState> states(count),statesAgain(count);
        CHECK(NimbyInternal_CopySignalStates(snapshot,states.data(),count,&count)==NIMBY_OK);
        CHECK(NimbyInternal_CopySignalStates(snapshot,statesAgain.data(),count,&count)==NIMBY_OK);
        CHECK(!count||std::memcmp(states.data(),statesAgain.data(),count*sizeof(NimbySignalState))==0);
        for(size_t i=0;i<states.size();++i){
            CHECK(states[i].signal_id==signals[i].id);
            CHECK(states[i].aspect==NIMBY_SIGNAL_ASPECT_UNKNOWN);
            CHECK(states[i].flags==0||states[i].flags==(NIMBY_SIGNAL_TEXTURE_STATE_VALID|NIMBY_SIGNAL_SPECIFIC_STATE_VALID));
            if(states[i].flags){CHECK(states[i].system_utf8[0]!=0&&states[i].specific_state_utf8[0]!=0);
                std::printf("Signal %llx: %s:%s selector=%d\n",static_cast<unsigned long long>(states[i].signal_id),states[i].system_utf8,states[i].specific_state_utf8,states[i].texture_state);}
        }
        CHECK(NimbyInternal_CopySignalTextures(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        std::vector<NimbySignalTexture> textures(count);
        CHECK(NimbyInternal_CopySignalTextures(snapshot,textures.data(),count,&count)==NIMBY_OK);
        unsigned resolved=0;
        for(size_t i=0;i<textures.size();++i){const auto& texture=textures[i];CHECK(texture.signal_id==signals[i].id);
            if(texture.flags&NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID){
                CHECK(texture.selected_index>=0&&static_cast<uint32_t>(texture.selected_index)<texture.state_count);
                std::printf("Texture %llx: %s [%d/%u] %s file=%s\n",static_cast<unsigned long long>(texture.signal_id),
                    texture.textures_id_utf8,texture.selected_index,texture.state_count,texture.relative_path_utf8,texture.file_path_utf8);
            }
            if(texture.flags&NIMBY_SIGNAL_TEXTURE_FILE_VALID){CHECK(texture.file_path_utf8[0]!=0);++resolved;}
        }
        std::printf("Signal texture files resolved: %u/%u\n",resolved,count);
        CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
        CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_INVALID_HANDLE);
        CHECK(NimbyInternal_CopyTrains(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE);
        CHECK(NimbyInternal_CopySignalStates(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
        CHECK(NimbyInternal_CopySignalTextures(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
        std::printf("Real SDK snapshot: %u trains, %u tracks, %u stations, %u signals; ownership and buffer guards passed.\n",info.train_count,info.track_count,info.station_count,info.signal_count);
    }
    std::puts("Public observation ABI guards passed.");
}
