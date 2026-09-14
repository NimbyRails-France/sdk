#include <nimby/observation.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_set>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);return 1;}}while(false)
int main(int argc,char** argv) {
    NimbySession session=123;NimbySnapshot snapshot=123;uint32_t count=99;
    CHECK(NimbySdk_OpenProcess(99,0,&session)==NIMBY_INVALID_ARGUMENT&&session==0);
    CHECK(NimbySdk_OpenProcess(1,0,&session)==NIMBY_UNSUPPORTED_GAME&&session==0);
    CHECK(NimbySdk_CaptureSnapshot(0,&snapshot)==NIMBY_INVALID_HANDLE&&snapshot==0);
    CHECK(NimbySdk_CopyTrains(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
    CHECK(NimbySdk_CopyTrains(0,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT);
    CHECK(NimbySdk_CopyTrackNodes(0,nullptr,0,&count)==NIMBY_INVALID_HANDLE&&count==0);
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
        CHECK(NimbySdk_CloseSession(session)==NIMBY_OK);
        NimbySnapshot rejected=99;CHECK(NimbySdk_CaptureSnapshot(session,&rejected)==NIMBY_INVALID_HANDLE&&rejected==0);
        CHECK(NimbySdk_CopyTrains(snapshot,after.data(),count,&count)==NIMBY_OK);
        CHECK(std::memcmp(before.data(),after.data(),count*sizeof(NimbyTrain))==0);
        CHECK(NimbySdk_CopyTracks(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.track_count);
        CHECK(NimbySdk_CopyStations(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.station_count);
        CHECK(NimbySdk_CopySignals(snapshot,nullptr,0,&count)==NIMBY_OK&&count==info.signal_count);
        CHECK(NimbySdk_ReleaseSnapshot(snapshot)==NIMBY_OK);
        CHECK(NimbySdk_ReleaseSnapshot(snapshot)==NIMBY_INVALID_HANDLE);
        CHECK(NimbySdk_CopyTrains(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE);
        std::printf("Real SDK snapshot: %u trains, %u tracks, %u stations, %u signals; ownership and buffer guards passed.\n",info.train_count,info.track_count,info.station_count,info.signal_count);
    }
    std::puts("Public observation ABI guards passed.");
}
