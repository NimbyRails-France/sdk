#include <nimby/observation.h>
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
    CHECK(NimbySdk_OpenProcess(99,0,&session)==NIMBY_INVALID_ARGUMENT&&session==0);
    CHECK(NimbySdk_OpenProcess(1,0,&session)==NIMBY_UNSUPPORTED_GAME&&session==0);
    CHECK(NimbySdk_CaptureSnapshot(0,&snapshot)==NIMBY_INVALID_HANDLE&&snapshot==0);
    CHECK(NimbySdk_CopyTrains(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopyTrains(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbySdk_CopySignalStates(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopySignalStates(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbySdk_CopySignalStates(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbySdk_CopySignalTextures(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopySignalTextures(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbySdk_CopySignalTextures(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbySdk_CopyTrackNodes(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopyTrackReservations(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopyTrackOccupations(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopyTrackReservations(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbySdk_CopyTrackOccupations(0,nullptr,0,nullptr)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbySdk_CopyTrainPathTracks(0,0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CloseSession(0)==NIMBY_INVALID_HANDLE);
    CHECK(NimbySdk_ReleaseSnapshot(0)==NIMBY_INVALID_HANDLE);
    NimbySnapshotInfo info{};
    CHECK(NimbySdk_GetSnapshotInfo(0,&info)==NIMBY_INVALID_ARGUMENT);
    if(argc==2) {
        CHECK(NimbySdk_OpenProcess(1,static_cast<uint32_t>(std::strtoul(argv[1],nullptr,10)),&session)==NIMBY_OK);
        CHECK(NimbySdk_CaptureSnapshot(session,&snapshot)==NIMBY_OK);
        info.struct_size=sizeof info;
        CHECK(NimbySdk_GetSnapshotInfo(snapshot,&info)==NIMBY_OK);
        CHECK(info.abi_version==1&&info.flags==(NIMBY_SNAPSHOT_EXPERIMENTAL|NIMBY_SNAPSHOT_NON_ATOMIC));
        CHECK(NimbySdk_CopyTrains(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.train_count&&count>0);
        NimbyTrain sentinel{};std::memset(&sentinel,0x5a,sizeof sentinel);auto previous=sentinel;
        CHECK(NimbySdk_CopyTrains(snapshot,&sentinel,0,&count)==NIMBY_BUFFER_TOO_SMALL);
        CHECK(std::memcmp(&sentinel,&previous,sizeof sentinel)==0);
        std::vector<NimbyTrain> before(count),after(count);
        CHECK(NimbySdk_CopyTrains(snapshot,before.data(),count,&count)==NIMBY_OK);
        uint32_t nodeCount=0;CHECK(NimbySdk_CopyTrackNodes(snapshot,nullptr,0,&nodeCount)==NIMBY_OK);
        std::vector<NimbyTrackNode> nodes(nodeCount);CHECK(NimbySdk_CopyTrackNodes(snapshot,nodes.data(),nodeCount,&nodeCount)==NIMBY_OK);
        std::unordered_set<uint64_t> nodeIds;for(auto& n:nodes)nodeIds.insert(n.id);
        for(auto& n:nodes)CHECK((!n.link_a||nodeIds.contains(n.link_a))&&(!n.link_b||nodeIds.contains(n.link_b)));
        unsigned pathCount=0;
        for(auto& t:before){uint32_t n=0;auto status=NimbySdk_CopyTrainPathTracks(snapshot,t.id,nullptr,0,&n);
            CHECK(status==NIMBY_OK||status==NIMBY_DATA_UNAVAILABLE);
            if(status==NIMBY_OK){std::vector<uint64_t> ids(n);CHECK(NimbySdk_CopyTrainPathTracks(snapshot,t.id,ids.data(),n,&n)==NIMBY_OK);
                for(auto id:ids)CHECK(nodeIds.contains(id));
                ++pathCount;}}
        std::printf("Experimental graph: %u nodes, %u available train Paths.\n",nodeCount,pathCount);
        std::unordered_set<uint64_t> trainIds;for(const auto& t:before)trainIds.insert(t.id);
        for(auto function:{NimbySdk_CopyTrackReservations,NimbySdk_CopyTrackOccupations}){
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
        CHECK(NimbySdk_CloseSession(session)==NIMBY_OK);
        NimbySnapshot rejected=99;CHECK(NimbySdk_CaptureSnapshot(session,&rejected)==NIMBY_INVALID_HANDLE&&rejected==0);
        CHECK(NimbySdk_CopyTrains(snapshot,after.data(),count,&count)==NIMBY_OK);
        CHECK(std::memcmp(before.data(),after.data(),count*sizeof(NimbyTrain))==0);
        CHECK(NimbySdk_CopyTracks(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.track_count);
        CHECK(NimbySdk_CopyStations(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.station_count);
        CHECK(NimbySdk_CopySignals(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        std::vector<NimbySignal> signals(count);
        CHECK(NimbySdk_CopySignals(snapshot,signals.data(),count,&count)==NIMBY_OK);
        CHECK(NimbySdk_CopySignalStates(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        if(count){
            NimbySignalState guard{};std::memset(&guard,0x5a,sizeof guard);auto old=guard;
            CHECK(NimbySdk_CopySignalStates(snapshot,&guard,0,&count)==NIMBY_BUFFER_TOO_SMALL);
            CHECK(std::memcmp(&guard,&old,sizeof guard)==0);
        }
        std::vector<NimbySignalState> states(count),statesAgain(count);
        CHECK(NimbySdk_CopySignalStates(snapshot,states.data(),count,&count)==NIMBY_OK);
        CHECK(NimbySdk_CopySignalStates(snapshot,statesAgain.data(),count,&count)==NIMBY_OK);
        CHECK(!count||std::memcmp(states.data(),statesAgain.data(),count*sizeof(NimbySignalState))==0);
        for(size_t i=0;i<states.size();++i){
            CHECK(states[i].signal_id==signals[i].id);
            CHECK(states[i].aspect==NIMBY_SIGNAL_ASPECT_UNKNOWN);
            CHECK(states[i].flags==0||states[i].flags==(NIMBY_SIGNAL_TEXTURE_STATE_VALID|NIMBY_SIGNAL_SPECIFIC_STATE_VALID));
            if(states[i].flags){CHECK(states[i].system_utf8[0]!=0&&states[i].specific_state_utf8[0]!=0);
                std::printf("Signal %llx: %s:%s selector=%d\n",static_cast<unsigned long long>(states[i].signal_id),states[i].system_utf8,states[i].specific_state_utf8,states[i].texture_state);}
        }
        CHECK(NimbySdk_CopySignalTextures(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        std::vector<NimbySignalTexture> textures(count);
        CHECK(NimbySdk_CopySignalTextures(snapshot,textures.data(),count,&count)==NIMBY_OK);
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
        CHECK(NimbySdk_ReleaseSnapshot(snapshot)==NIMBY_OK);
        CHECK(NimbySdk_ReleaseSnapshot(snapshot)==NIMBY_INVALID_HANDLE);
        CHECK(NimbySdk_CopyTrains(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE);
        CHECK(NimbySdk_CopySignalStates(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
        CHECK(NimbySdk_CopySignalTextures(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
        std::printf("Real SDK snapshot: %u trains, %u tracks, %u stations, %u signals; ownership and buffer guards passed.\n",info.train_count,info.track_count,info.station_count,info.signal_count);
    }
    std::puts("Public observation ABI guards passed.");
}
