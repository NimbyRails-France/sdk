#include <nimby/client.hpp>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <vector>
void check(uint32_t status) {
    if(status!=NIMBY_OK)throw std::runtime_error(NimbySdk_StatusString(status));
}
struct Session {
    NimbySession value{};
    ~Session(){if(value)NimbySdk_CloseSession(value);}
};
struct Snapshot {
    NimbySnapshot value{};
    ~Snapshot(){if(value)NimbySdk_ReleaseSnapshot(value);}
};
template<class T,class F> std::vector<T> records(NimbySnapshot snapshot,F copy) {
    uint32_t count{};check(copy(snapshot,nullptr,0,&count));
    std::vector<T> result(count);check(copy(snapshot,result.data(),count,&count));return result;
}
int main(int argc,char** argv) {
    if(argc!=2){std::puts("Usage: MyNimbyObserver <game-pid> (5 snapshots, read-only)");return 2;}
    uint32_t pid{};const auto end=argv[1]+std::strlen(argv[1]);const auto parsed=std::from_chars(argv[1],end,pid);
    if(parsed.ec!=std::errc{}||parsed.ptr!=end||!pid)return 2;
    try {
        Session session;check(NimbySdk_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,pid,&session.value));
        for(int tick=0;tick<5;++tick) {
            Snapshot snapshot;
            auto status=NimbySdk_CaptureSnapshot(session.value,&snapshot.value);
            if(status==NIMBY_DATA_UNAVAILABLE){std::puts(NimbySdk_StatusString(status));std::this_thread::sleep_for(std::chrono::milliseconds(250));continue;}
            check(status);
            auto trains=records<NimbyTrain>(snapshot.value,NimbySdk_CopyTrains);
            auto tracks=records<NimbyTrack>(snapshot.value,NimbySdk_CopyTracks);
            auto stations=records<NimbyStation>(snapshot.value,NimbySdk_CopyStations);
            auto services=records<NimbyTrainService>(snapshot.value,NimbySdk_CopyTrainServices);
            for(const auto& row:services){
                nimby::TrainService service(row);
                std::printf("service train=%llu state=%s",row.train_id,service.getStatusName());
                if(auto seconds=service.getDepartureRemainingSeconds())std::printf(" departs_in=%.1f_game_s",*seconds);
                if(auto station=service.getLocationStationId())std::printf(" station=%llu",*station);
                std::puts("");
            }
            auto signals=records<NimbySignal>(snapshot.value,NimbySdk_CopySignals);
            auto signalStates=records<NimbySignalState>(snapshot.value,NimbySdk_CopySignalStates);
            for(const auto& state:signalStates){
                std::printf("signal=%llx ",static_cast<unsigned long long>(state.signal_id));
                if(state.flags&NIMBY_SIGNAL_ASPECT_VALID)std::printf("aspect=%u ",state.aspect);
                else std::printf("aspect=unknown ");
                if(state.flags&NIMBY_SIGNAL_SPECIFIC_STATE_VALID)
                    std::printf("specific=%s:%s ",state.system_utf8,state.specific_state_utf8);
                else std::printf("specific=unknown ");
                if(state.flags&NIMBY_SIGNAL_TEXTURE_STATE_VALID)std::printf("texture=%d",state.texture_state);
                else std::printf("texture=unknown");
                std::puts("");
            }
            std::printf("snapshot %d: %zu trains, %zu tracks, %zu stations, %zu signals\n",tick,trains.size(),tracks.size(),stations.size(),signals.size());
            struct UsageCollection {
                decltype(&NimbySdk_CopyTrackReservations) copy;
                const char* label;
            };
            const UsageCollection collections[]{
                {NimbySdk_CopyTrackReservations,"Reserved portions"},
                {NimbySdk_CopyTrackOccupations,"Occupation portions"}
            };
            for(const auto& collection:collections){
                const auto function=collection.copy;
                uint32_t count=0;const auto status=function(snapshot.value,nullptr,0,&count);
                const char* label=collection.label;
                if(status==NIMBY_DATA_UNAVAILABLE){std::printf("%s: unavailable\n",label);continue;}
                check(status);auto usage=records<NimbyTrackUsage>(snapshot.value,function);
                std::printf("%s: %zu (unordered native intervals)\n",label,usage.size());
            }
            for(const auto& train:trains) {
                std::printf("%s id=%llx ",train.name_utf8,static_cast<unsigned long long>(train.id));
                if(train.flags&(NIMBY_TRAIN_SPEED_VALID|NIMBY_TRAIN_PRESENT))std::printf("%.1f km/h%s ",train.speed_mps*3.6,
                    (train.flags&NIMBY_TRAIN_SPEED_DEFAULTED)?" (game display default; speed not measured)":"");
                else std::printf("speed unavailable ");
                if(train.flags&NIMBY_TRAIN_POSITION_VALID)std::printf("track=%llx %.2f%%",static_cast<unsigned long long>(train.track_id),train.track_fraction*100);
                std::puts("");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        return 0;
    }catch(const std::exception& e){std::fprintf(stderr,"SDK: %s\n",e.what());return 1;}
}
