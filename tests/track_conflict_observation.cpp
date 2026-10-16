// Production registry/capture/copy/epoch with a synthetic OS memory boundary.
// No game is started, modified or read by this regression.
#include <nimby/detail/observation_session.hpp>
#include <cstdio>
#include <stdexcept>
#include "observation_process_fixture.hpp"
extern "C" uint32_t __cdecl NimbyInternal_GetVersion(NimbySdkVersion* out) noexcept{
    if(!out||out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={sizeof *out,NIMBY_OBSERVATION_ABI_VERSION,0,9,0};return NIMBY_OK;
}
namespace {
constexpr uint64_t a=0x1000000000001,b=0x1000000010001,missing=0x1000000020001;
constexpr uint64_t table=0x60000000,block=0x60010000,rows=0x61000000;
void setup(uint32_t pid){
    const auto world=fixture::world(pid);
    world->put(fixture::db,4,uint32_t(2));world->put(fixture::db,8,uint32_t(4));world->put(fixture::db,16,uint32_t(3));
    world->put(fixture::db,24,table);world->put(fixture::db,32,table+8);world->put(fixture::db,40,table+8);world->put(table,0,block);
    world->regions[block].resize(0x110);world->regions[block+0x4e8].resize(0x110);
    world->put(block,0,a);world->put(block+0x4e8,0,b);
    world->put(block,0xf8,rows);world->put(block,0x100,rows+0x30);world->put(block,0x108,rows+0x30);
    world->regions[rows].resize(0x30);world->put(rows,0,b);world->put(rows,0x18,.2f);world->put(rows,0x1c,.6f);world->put(rows,0x20,.3f);world->put(rows,0x24,.8f);
}
}
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
int main(){try{
    NimbySession session{};CHECK(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,420,&session)==NIMBY_OK);setup(420);
    const uint64_t ids[]{a,b,missing};NimbySnapshot snapshot=99;uint32_t stage=99,count{};
    CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,3,&snapshot,&stage)==NIMBY_OK&&snapshot&&stage==0);
    CHECK(NimbyInternal_CopyTrackConflictStates(snapshot,nullptr,0,&count)==NIMBY_OK&&count==3);
    NimbyTrackConflictState states[3]{};CHECK(NimbyInternal_CopyTrackConflictStates(snapshot,states,2,&count)==NIMBY_BUFFER_TOO_SMALL&&count==3);
    CHECK(NimbyInternal_CopyTrackConflictStates(snapshot,states,3,&count)==NIMBY_OK&&states[0].flags==1&&states[1].flags==1&&states[2].flags==0);
    NimbyTrackConflict observed{};CHECK(NimbyInternal_CopyTrackConflicts(snapshot,&observed,1,&count)==NIMBY_OK&&count==1&&observed.other_track_id==b);
    CHECK(NimbyInternal_CopyTrackConflicts(snapshot,nullptr,1,&count)==NIMBY_INVALID_ARGUMENT&&count==0);
    CHECK(NimbyInternal_CopyTrackConflicts(snapshot,&observed,1,nullptr)==NIMBY_INVALID_ARGUMENT);
    NimbyGameSession oldWorld{};oldWorld.struct_size=sizeof oldWorld;CHECK(NimbyInternal_GetGameSession(snapshot,&oldWorld)==NIMBY_OK&&oldWorld.generation);
    fixture::world(420)->put(rows,0x18,.4f);fixture::world(420)->identity=43;
    NimbySnapshot fresh{};CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,3,&fresh,&stage)==NIMBY_OK);
    NimbyGameSession freshWorld{};freshWorld.struct_size=sizeof freshWorld;CHECK(NimbyInternal_GetGameSession(fresh,&freshWorld)==NIMBY_OK&&freshWorld.generation==oldWorld.generation+1);
    CHECK(NimbyInternal_CopyTrackConflicts(snapshot,&observed,1,&count)==NIMBY_OK&&observed.begin_fraction==double(.2f));
    CHECK(NimbyInternal_CopyTrackConflicts(fresh,&observed,1,&count)==NIMBY_OK&&observed.begin_fraction==double(.4f));
    CHECK(NimbyInternal_ReleaseSnapshot(fresh)==NIMBY_OK);
    NimbySnapshot otherScope{};CHECK(NimbyInternal_CaptureSessionSnapshot(session,&otherScope,&stage)==NIMBY_OK);
    CHECK(NimbyInternal_CopyTrackConflicts(otherScope,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(otherScope)==NIMBY_OK);
    {nimby::detail::ObservationSession highLevel(420);const auto values=highLevel.captureTrackConflicts(ids);
     CHECK(values->getConflictsForTrack(a)&&values->getConflictsForTrack(a)->size()==1);
     CHECK(values->getConflictsForTrack(b)&&values->getConflictsForTrack(b)->empty());
     CHECK(!values->getConflictsForTrack(missing)&&values->getUnavailableReason(missing)==NIMBY_TRACK_CONFLICTS_TRACK_UNAVAILABLE);
     CHECK(!values->getConflictsForTrack(a+1)&&!values->getUnavailableReason(a+1));
     CHECK(values->getConflictsForTrack(a)->data()==values->getConflictsForTrack(a)->data());
     CHECK(values->getGameSession().generation==freshWorld.generation&&values->getProcessId()==420);
    }
    // Only the retained captures count. A peer connection has its own allowance.
    std::vector<NimbySnapshot> retained{snapshot};
    for(unsigned i=1;i<16;++i){NimbySnapshot value{};CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,3,&value,&stage)==NIMBY_OK);retained.push_back(value);}
    NimbySnapshot limited=99;CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,3,&limited,&stage)==NIMBY_RESOURCE_LIMIT&&limited==0);
    NimbySession peer{};CHECK(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,420,&peer)==NIMBY_OK);
    CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(peer,ids,3,&limited,&stage)==NIMBY_OK);
    CHECK(NimbyInternal_ReleaseSnapshot(limited)==NIMBY_OK&&NimbyInternal_CloseSession(peer)==NIMBY_OK);
    for(auto value:retained)CHECK(NimbyInternal_ReleaseSnapshot(value)==NIMBY_OK);
    CHECK(NimbyInternal_CopyTrackConflicts(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE);
    limited=99;stage=99;CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,4097,&limited,&stage)==NIMBY_INVALID_ARGUMENT&&limited==0&&stage==0);
    const uint64_t wrongTag=0x5000000000001;CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,&wrongTag,1,&limited,&stage)==NIMBY_INVALID_ARGUMENT);
    fixture::world(420)->refusedAddress=fixture::db;
    CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,3,&limited,&stage)==NIMBY_DATA_UNAVAILABLE&&limited==0);
    fixture::world(420)->refusedAddress=0;
    CHECK(NimbyInternal_CaptureTrackConflictsSnapshot(session,ids,3,&limited,&stage)==NIMBY_OK);
    CHECK(NimbyInternal_CloseSession(session)==NIMBY_OK);
    CHECK(NimbyInternal_CopyTrackConflicts(limited,&observed,1,&count)==NIMBY_OK);
    CHECK(NimbyInternal_ReleaseSnapshot(limited)==NIMBY_OK);
    std::puts("PASS conflict capture ABI, immutable shared views, scope availability, generation, quotas and lifecycle.");
}catch(const std::exception& error){std::fprintf(stderr,"%s\n",error.what());return 1;}}
