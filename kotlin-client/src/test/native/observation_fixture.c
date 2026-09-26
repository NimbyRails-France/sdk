#define NIMBY_SDK_BUILD
#include <nimby/detail/observation.h>
#include <string.h>
#include <nimby/detail/control.h>
#include <nimby/detail/driving.h>
#include <stddef.h>
#include <math.h>
_Static_assert(sizeof(NimbyDrivingObservation)==480, "Kotlin driving ABI size");
_Static_assert(offsetof(NimbyDrivingObservation,purchased)==48, "purchased offset");
_Static_assert(offsetof(NimbyDrivingObservation,current)==112, "current offset");
_Static_assert(offsetof(NimbyDrivingObservation,train)==176, "train offset");

uint32_t NimbyInternal_ReadTrainDriving(NimbySession session,uint64_t id,NimbyDrivingObservation* out){
 if(session!=123)return NIMBY_INVALID_HANDLE;
 if(!out||out->struct_size!=sizeof *out||(id>>48)!=5)return NIMBY_INVALID_ARGUMENT;
 if((id&0xffff)==2)return NIMBY_DATA_UNAVAILABLE;
 if((id&0xffff)==3)return NIMBY_PROCESS_EXITED;
 memset(out,0,sizeof *out);out->struct_size=sizeof *out;
 out->train_id=id;out->train.id=id;out->session_generation=19;
 out->captured_unix_ms=987654;out->elapsed_begin_ms=1200;out->elapsed_end_ms=1207;
 out->flags=NIMBY_DRIVING_CURRENT_VALID|NIMBY_DRIVING_MOTION_VALID;
 out->current=(NimbyTrainDynamics){41,0.7,0.8,1.2,90000,700000,32000,87};
 out->train.track_id=9;out->train.track_fraction=.25;out->train.direction=-1;
 out->train.flags=NIMBY_TRAIN_POSITION_VALID|NIMBY_TRAIN_SPEED_VALID;
 if((id&0xffff)==4){out->train.flags|=NIMBY_TRAIN_SPEED_DEFAULTED;out->flags=0;}
 if((id&0xffff)==5){out->train.speed_mps=NAN;out->train.track_fraction=NAN;out->current.length_m=NAN;}
 if((id&0xffff)==6){out->flags=NIMBY_DRIVING_PURCHASED_VALID;out->purchased=out->current;}
 if((id&0xffff)==7)out->train_id++;
 return NIMBY_OK;
}
static unsigned released=0,closed=0;
NIMBY_API uint32_t Fixture_Released(void){return released;}
NIMBY_API uint32_t Fixture_Closed(void){return closed;}
uint32_t NimbyInternal_GetVersion(NimbySdkVersion* v){if(!v||v->struct_size!=sizeof *v)return 1;*v=(NimbySdkVersion){sizeof *v,2,0,8,0};return 0;}
uint32_t NimbyInternal_OpenProcess(uint32_t abi,uint32_t pid,NimbySession* out){if(abi!=2||pid!=42)return 7;*out=123;return 0;}
uint32_t NimbyInternal_CloseSession(NimbySession s){if(s!=123)return 9;closed++;return 0;}
uint32_t NimbyInternal_CaptureSnapshot(NimbySession s,NimbySnapshot* out){if(s!=123)return 9;*out=456;return 0;}
uint32_t NimbyInternal_ReleaseSnapshot(NimbySnapshot s){if(s!=456)return 9;released++;return 0;}
uint32_t NimbyInternal_GetSnapshotInfo(NimbySnapshot s,NimbySnapshotInfo* out){(void)s;if(out->struct_size!=sizeof *out)return 1;memset(out,0,sizeof *out);out->process_id=42;out->captured_unix_ms=123456;return 0;}
uint32_t NimbyInternal_CopyTrains(NimbySnapshot s,NimbyTrain* out,uint32_t capacity,uint32_t* count){(void)s;*count=1;if(!out&&!capacity)return 0;if(capacity<1)return 10;memset(out,0,sizeof *out);out->id=0xffffffffffffffffULL;out->track_id=9;out->track_fraction=.25;out->flags=NIMBY_TRAIN_POSITION_VALID|NIMBY_TRAIN_SPEED_VALID|NIMBY_TRAIN_SPEED_DEFAULTED;strcpy(out->name_utf8,"Train test");return 0;}
#define EMPTY(name,type) uint32_t name(NimbySnapshot s,type* out,uint32_t cap,uint32_t* count){(void)s;(void)out;(void)cap;*count=0;return 0;}
EMPTY(NimbyInternal_CopyTracks,NimbyTrack)
EMPTY(NimbyInternal_CopyStations,NimbyStation)
EMPTY(NimbyInternal_CopySignals,NimbySignal)
EMPTY(NimbyInternal_CopySignalStates,NimbySignalState)
EMPTY(NimbyInternal_CopySignalTextures,NimbySignalTexture)
EMPTY(NimbyInternal_CopyTrackNodes,NimbyTrackNode)
EMPTY(NimbyInternal_CopyTrackJunctions,NimbyTrackJunction)
uint32_t NimbyInternal_CopyTrainServices(NimbySnapshot s,NimbyTrainService* out,uint32_t cap,uint32_t* n){
    (void)s;*n=2;if(!out&&!cap)return 0;if(cap<2)return 10;
    memset(out,0,2*sizeof *out);
    out[0].train_id=0xffffffffffffffffULL;
    out[0].flags=NIMBY_SERVICE_RUN_VALID|NIMBY_SERVICE_STOP_VALID|NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_ARRIVAL_VALID|NIMBY_SERVICE_DEPARTURE_VALID|NIMBY_SERVICE_COOLDOWN_VALID;
    out[0].line_id=91;out[0].stop_station_id=92;out[0].stop_index=0;
    out[0].game_epoch_seconds=1234;out[0].game_time_us=5000000;
    out[0].arrival_time_us=-1000000;out[0].departure_time_us=0;out[0].dispatch_time_us=1000000;
    out[1]=out[0];out[1].train_id=1;out[1].flags=0;
    return 0;
}
EMPTY(NimbyInternal_CopyTrainDetails,NimbyTrainDetails)
EMPTY(NimbyInternal_CopyPlatforms,NimbyPlatform)
EMPTY(NimbyInternal_CopyTrackOccupations,NimbyTrackUsage)
uint32_t NimbyInternal_CopyTrackReservations(NimbySnapshot s,NimbyTrackUsage* out,uint32_t cap,uint32_t* n){(void)s;(void)out;(void)cap;*n=0;return 8;}
uint32_t NimbyInternal_CopyTrainPathTracks(NimbySnapshot s,uint64_t t,uint64_t* out,uint32_t cap,uint32_t* n){(void)s;(void)t;(void)out;(void)cap;*n=0;return 8;}
uint32_t NimbyInternal_CopyTrainLineStops(NimbySnapshot s,uint64_t t,NimbyLineStop* out,uint32_t cap,uint32_t* n){(void)s;(void)t;(void)out;(void)cap;*n=0;return 8;}
uint32_t NimbyInternal_GetSimulationClock(NimbySnapshot s,NimbySimulationClock* out){(void)s;(void)out;return 8;}

static uint32_t mutation_calls=0;
NIMBY_API uint32_t Fixture_MutationCalls(void){return mutation_calls;}
uint32_t NimbyInternal_SetSimulationDateTime(NimbySession s,int64_t utc,NimbySimulationClock* out){
    ++mutation_calls;
    if(s!=123||!out||out->struct_size!=sizeof *out)return 1;
    if(utc==777)return NIMBY_CLOCK_WRITE_FAILED;
    *out=(NimbySimulationClock){sizeof *out,0,utc,17};return 0;
}
uint32_t NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(NimbySession s,int64_t utc,NimbySimulationClock* out,uint32_t* count){
    uint32_t status=NimbyInternal_SetSimulationDateTime(s,utc,out);*count=0xffffffffu;return status;
}
NIMBY_API uint32_t NimbyInternal_ShowSignalTextureFor(uint32_t pid,uint64_t signal,const char* set,const char* path,uint32_t duration){
    ++mutation_calls;return pid==42&&signal==0x8000000000001ULL&&!strcmp(set,"catalogue")&&!strcmp(path,"test.svg")&&duration==1500?0:1;
}
NIMBY_API uint32_t NimbyInternal_RestoreSignalTexture(uint32_t pid,uint64_t signal){++mutation_calls;return pid==42&&signal==0x8000000000001ULL?0:1;}
uint32_t NimbyInternal_ModControl(uint32_t pid,const char* id,const NimbyControlRequest* r,NimbyControlResponse* out){
    ++mutation_calls;
    if(pid!=42||strcmp(id,"test.mod")||r->size!=72||r->version!=1||out->size!=328)return 1;
    memset(out,0,sizeof *out);out->size=sizeof *out;out->version=1;out->generation=99;out->capabilities=8191;
    out->remaining_ms=r->lease_ms;out->signal_count=1;out->train_count=2;out->setting_count=3;
    out->aspect=r->value;out->reason=r->index;out->speed_mps=r->speed_mps;out->exit_signal=r->exit_signal;
    out->active=r->operation==NIMBY_CONTROL_READ_TRAIN?2:r->operation;
    strcpy(out->detail,"fixture");
    if(r->operation==NIMBY_CONTROL_STATUS)return 0;
    if(!r->owner||r->generation!=99)return 9;
    return 0;
}
