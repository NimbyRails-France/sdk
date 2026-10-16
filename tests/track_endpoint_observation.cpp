// ABI, owned immutable snapshots and typed adapter, using synthetic memory.
#include <nimby/detail/observation_session.hpp>
#include <cstdio>
#include <stdexcept>
#include "observation_process_fixture.hpp"
extern "C" uint32_t __cdecl NimbyInternal_GetVersion(NimbySdkVersion* out) noexcept{
    if(!out||out->struct_size!=sizeof *out)return NIMBY_INVALID_ARGUMENT;
    *out={sizeof *out,NIMBY_OBSERVATION_ABI_VERSION,0,9,0};return NIMBY_OK;
}
#define CHECK(x) do{if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
int main(){try{
    constexpr uint64_t a=0x1000000000001,b=0x1000000010001,missing=0x1000000020001;
    constexpr uint64_t table=0x60000000,block=0x60010000;
    NimbySession session{};CHECK(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,421,&session)==NIMBY_OK);
    const auto world=fixture::world(421);
    world->put(fixture::db,4,uint32_t(2));world->put(fixture::db,8,uint32_t(4));world->put(fixture::db,16,uint32_t(3));
    world->put(fixture::db,24,table);world->put(fixture::db,32,table+8);world->put(fixture::db,40,table+8);world->put(table,0,block);
    world->regions[block].resize(0x420);world->regions[block+0x4e8].resize(0x420);
    world->put(block,0,a);world->put(block+0x4e8,0,b);
    const uint64_t ids[]{a,b,missing};NimbySnapshot snapshot{};uint32_t stage{},count{};
    CHECK(NimbyInternal_CaptureTrackEndpointsSnapshot(session,ids,3,&snapshot,&stage)==NIMBY_OK&&snapshot&&stage==0);
    CHECK(NimbyInternal_CopyTrackEndpointStates(snapshot,nullptr,0,&count)==NIMBY_OK&&count==3);
    NimbyTrackEndpointStates states[3]{};states[0].track_id=99;
    CHECK(NimbyInternal_CopyTrackEndpointStates(snapshot,states,2,&count)==NIMBY_BUFFER_TOO_SMALL&&count==3&&states[0].track_id==99);
    CHECK(NimbyInternal_CopyTrackEndpointStates(snapshot,states,3,&count)==NIMBY_OK&&states[0].state_b==2&&states[2].state_b==0);
    NimbySnapshot other{};CHECK(NimbyInternal_CaptureSessionSnapshot(session,&other,&stage)==NIMBY_OK);
    CHECK(NimbyInternal_CopyTrackEndpointStates(other,nullptr,0,&count)==NIMBY_DATA_UNAVAILABLE&&count==0);
    CHECK(NimbyInternal_ReleaseSnapshot(other)==NIMBY_OK);
    NimbyGameSession prior{};prior.struct_size=sizeof prior;CHECK(NimbyInternal_GetGameSession(snapshot,&prior)==NIMBY_OK&&prior.generation);
    world->put(block,16,b);world->identity=43;
    {nimby::detail::ObservationSession typed(421);const auto values=typed.captureTrackEndpoints(ids);
     CHECK(values->getTrack(a)&&values->getTrack(a)->at(-1).isTerminal()&&values->getTrack(a)->at(1).kind==nimby::TrackEndpointKind::Connected);
     CHECK(values->getTrack(missing)&&values->getTrack(missing)->at(1).kind==nimby::TrackEndpointKind::Unknown);
     CHECK(!values->getTrack(a+1)&&values->getTrack(a)->at(0).kind==nimby::TrackEndpointKind::Unknown);
     CHECK(values->getGameSession().generation==prior.generation+1&&values->getTracks().size()==3);
    }
    CHECK(NimbyInternal_CopyTrackEndpointStates(snapshot,states,3,&count)==NIMBY_OK&&states[0].state_b==2);
    NimbySnapshot invalid=99;stage=99;CHECK(NimbyInternal_CaptureTrackEndpointsSnapshot(session,ids,4097,&invalid,&stage)==NIMBY_INVALID_ARGUMENT&&invalid==0&&stage==0);
    world->refusedAddress=fixture::db;CHECK(NimbyInternal_CaptureTrackEndpointsSnapshot(session,ids,3,&invalid,&stage)==NIMBY_DATA_UNAVAILABLE&&invalid==0);
    world->refusedAddress=0;CHECK(NimbyInternal_CloseSession(session)==NIMBY_OK);
    CHECK(NimbyInternal_CopyTrackEndpointStates(snapshot,states,3,&count)==NIMBY_OK);
    CHECK(NimbyInternal_ReleaseSnapshot(snapshot)==NIMBY_OK);
    CHECK(NimbyInternal_CopyTrackEndpointStates(snapshot,nullptr,0,&count)==NIMBY_INVALID_HANDLE);
    std::puts("PASS endpoint snapshot ABI, availability, typed directions, immutable views, session generation and lifetime.");
}catch(const std::exception& error){std::fprintf(stderr,"%s\n",error.what());return 1;}}
