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
static unsigned full_captures=0,session_captures=0;
static unsigned network_revision=0;
NIMBY_API void Fixture_NetworkRevision(uint32_t value){network_revision=value;}
NIMBY_API uint32_t Fixture_Released(void){return released;}
NIMBY_API uint32_t Fixture_Closed(void){return closed;}
NIMBY_API uint32_t Fixture_FullCaptures(void){return full_captures;}
NIMBY_API uint32_t Fixture_SessionCaptures(void){return session_captures;}
uint32_t NimbyInternal_GetVersion(NimbySdkVersion* v){if(!v||v->struct_size!=sizeof *v)return 1;*v=(NimbySdkVersion){sizeof *v,2,0,8,0};return 0;}
uint32_t NimbyInternal_OpenProcess(uint32_t abi,uint32_t pid,NimbySession* out){if(abi!=2||(pid!=42&&pid!=44&&pid!=45))return 7;*out=pid==42?123:pid==44?124:125;return 0;}
uint32_t NimbyInternal_CloseSession(NimbySession s){if(s<123||s>125)return 9;closed++;return 0;}
uint32_t NimbyInternal_CaptureSnapshot(NimbySession s,NimbySnapshot* out){if(s<123||s>125)return 9;++full_captures;*out=s+333;return 0;}
#ifndef NRF_FIXTURE_LEGACY_METRICS
static uint32_t rich_captures=0, rich_reads=0, last_train_flags=0;
NIMBY_API uint32_t Fixture_TrainDataCaptures(void){return rich_captures;}
NIMBY_API uint32_t Fixture_RichReads(void){return rich_reads;}
NIMBY_API uint32_t Fixture_LastTrainFlags(void){return last_train_flags;}
uint32_t NimbyInternal_CaptureTrainDataSnapshotWithOptions(NimbySession s,uint32_t flags,NimbySnapshot* out,uint32_t* stage){
 if(s<123||s>125||flags>255)return 1;
 ++rich_captures;last_train_flags=flags;*stage=0;*out=s+333;return 0;
}
uint32_t NimbyInternal_CaptureSessionSnapshot(NimbySession s,NimbySnapshot* out,uint32_t* stage){
 if(s<123||s>125)return 9;
 ++session_captures;*stage=0;*out=s+333;return 0;
}
uint32_t NimbyInternal_CopyTrainMetadata(NimbySnapshot s,NimbyTrainMetadata* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=1;if(!out&&!cap)return 0;if(cap<1)return 10;
    memset(out,0,sizeof *out);out->train_id=0xffffffffffffffffULL;
    out->flags=NIMBY_TRAIN_PREDICTED_DELAY_VALID;out->predicted_arrival_delay_us=-1500000;
    if(last_train_flags&NIMBY_TRAIN_DATA_CHARACTERISTICS){
      out->configured=(NimbyTrainCharacteristics){255,8,500,0,40,200,600000,1,8000000,200000};
      out->current.flags=NIMBY_CHARACTERISTICS_MAX_SPEED_VALID;out->current.maximum_speed_mps=35;out->current.length_m=999;
    }
    if(last_train_flags&NIMBY_TRAIN_DATA_COMPOSITION){out->configured.flags|=256;out->current.flags|=256;}
    return 0;
}
uint32_t NimbyInternal_CopyTrainVehicles(NimbySnapshot s,NimbyTrainVehicle* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=2;if(!out&&!cap)return 0;if(cap<2)return 10;
    out[0]=(NimbyTrainVehicle){0xffffffffffffffffULL,101,0,0};out[1]=(NimbyTrainVehicle){0xffffffffffffffffULL,999,1,0};return 0;
}
uint32_t NimbyInternal_CopyVehicleModels(NimbySnapshot s,NimbyVehicleModel* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=1;if(!out&&!cap)return 0;if(cap<1)return 10;
    memset(out,0,sizeof *out);out->model_id=101;strcpy(out->code_utf8,"metro");strcpy(out->name_en_utf8,"Metro vehicle");strcpy(out->source_name_utf8,"Base game");return 0;
}
uint32_t NimbyInternal_CopyTags(NimbySnapshot s,NimbyTag* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=2;if(!out&&!cap)return 0;if(cap<2)return 10;
    memset(out,0,2*sizeof *out);out[0].tag_id=7;strcpy(out[0].name_utf8,"Regional");out[1].tag_id=8;strcpy(out[1].name_utf8,"Maintenance");return 0;
}
uint32_t NimbyInternal_CopyLines(NimbySnapshot s,NimbyLineMetadata* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=7;if(!out&&!cap)return 0;if(cap<7)return 10;
    memset(out,0,7*sizeof *out);
    for(uint32_t i=0;i<7;++i){out[i].line_id=91+i;out[i].flags=7;strcpy(out[i].name_utf8,"Unassigned line");}
    out[0].kind=1;strcpy(out[0].name_utf8,"Depot line");out[1].parent_line_id=91;out[2].parent_line_id=9999;
    out[3].parent_line_id=95;out[4].parent_line_id=94;out[5].flags=6;return 0;
}
uint32_t NimbyInternal_CopyObjectTagsStates(NimbySnapshot s,NimbyObjectTagsState* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=8;if(!out&&!cap)return 0;if(cap<8)return 10;
    out[0]=(NimbyObjectTagsState){0xffffffffffffffffULL,1,0};
    for(uint32_t i=1;i<8;++i){out[i]=(NimbyObjectTagsState){90+i,i==6?0:1,0};}
    return 0;
}
uint32_t NimbyInternal_CopyObjectTags(NimbySnapshot s,NimbyObjectTag* out,uint32_t cap,uint32_t* n){
    (void)s;++rich_reads;*n=6;if(!out&&!cap)return 0;if(cap<6)return 10;
    const NimbyObjectTag rows[]={{0xffffffffffffffffULL,7},{0xffffffffffffffffULL,999},{91,7},{92,8},{92,7},{97,999}};
    memcpy(out,rows,sizeof rows);return 0;
}
#endif
uint32_t NimbyInternal_ReleaseSnapshot(NimbySnapshot s){if(s<456||s>458)return 9;released++;return 0;}
uint32_t NimbyInternal_GetSnapshotInfo(NimbySnapshot s,NimbySnapshotInfo* out){(void)s;if(out->struct_size!=sizeof *out)return 1;memset(out,0,sizeof *out);out->process_id=42;out->captured_unix_ms=123456;return 0;}
uint32_t NimbyInternal_CopyTrains(NimbySnapshot s,NimbyTrain* out,uint32_t capacity,uint32_t* count){
 *count=s==457?16384:s==458?0x7fffffff:1;if(!out&&!capacity)return 0;if(capacity<*count)return 10;
 for(uint32_t i=0;i<*count;++i){memset(out+i,0,sizeof *out);out[i].id=s==457?0x5000000000000ULL+i+1:0xffffffffffffffffULL;
 out[i].track_id=9;out[i].track_fraction=.25;out[i].flags=NIMBY_TRAIN_POSITION_VALID|NIMBY_TRAIN_SPEED_VALID|NIMBY_TRAIN_SPEED_DEFAULTED;
 strcpy(out[i].name_utf8,"Train test");}return 0;
}
#define EMPTY(name,type) uint32_t name(NimbySnapshot s,type* out,uint32_t cap,uint32_t* count){(void)s;(void)out;(void)cap;*count=0;return 0;}
EMPTY(NimbyInternal_CopyTracks,NimbyTrack)
EMPTY(NimbyInternal_CopyStations,NimbyStation)
uint32_t NimbyInternal_CopySignals(NimbySnapshot s,NimbySignal* out,uint32_t cap,uint32_t* count){
 *count=s==457?8192:0;if(!out&&!cap)return 0;if(cap<*count)return 10;
 for(uint32_t i=0;i<*count;++i){out[i]=(NimbySignal){0x8000000000000ULL+i+1,9,.5,1,2};}return 0;
}
uint32_t NimbyInternal_CopySignalStates(NimbySnapshot s,NimbySignalState* out,uint32_t cap,uint32_t* count){
 *count=s==457?8192:0;if(!out&&!cap)return 0;if(cap<*count)return 10;
 for(uint32_t i=0;i<*count;++i){memset(out+i,0,sizeof *out);out[i].signal_id=0x8000000000000ULL+i+1;out[i].flags=6;out[i].texture_state=(int32_t)(i%7+network_revision);
 strcpy(out[i].system_utf8,"System");strcpy(out[i].specific_state_utf8,"Restricted");}return 0;
}
uint32_t NimbyInternal_CopySignalTextures(NimbySnapshot s,NimbySignalTexture* out,uint32_t cap,uint32_t* count){
 *count=s==457?8192:0;if(!out&&!cap)return 0;if(cap<*count)return 10;
 for(uint32_t i=0;i<*count;++i){memset(out+i,0,sizeof *out);out[i].signal_id=0x8000000000000ULL+i+1;out[i].flags=2;
 strcpy(out[i].file_path_utf8,network_revision?"textures/changed.svg":"textures/large-map.svg");}return 0;
}
uint32_t NimbyInternal_CopyTrackNodes(NimbySnapshot s,NimbyTrackNode* out,uint32_t cap,uint32_t* count){
 *count=s==457?100000:0;if(network_revision==3){*count=0;return 8;}if(!out&&!cap)return 0;if(cap<*count)return 10;
 for(uint32_t i=0;i<*count;++i){out[i]=(NimbyTrackNode){(uint64_t)i+1,0,0,(double)i,(double)i*2};}
 if(*count&&network_revision==2)out[*count-1].y=777;
 return 0;
}
EMPTY(NimbyInternal_CopyTrackJunctions,NimbyTrackJunction)
#ifndef NRF_FIXTURE_LEGACY_METRICS
_Static_assert(sizeof(NimbyTrackMetric)==16,"Kotlin track metric ABI size");
uint32_t NimbyInternal_CopyTrackMetrics(NimbySnapshot s,NimbyTrackMetric* out,uint32_t cap,uint32_t* count){
    (void)s;*count=1;if(!out&&!cap)return 0;if(cap<1)return 10;
    *out=(NimbyTrackMetric){9,1234.5};return 0;
}
#endif
uint32_t NimbyInternal_CopyTrainServices(NimbySnapshot s,NimbyTrainService* out,uint32_t cap,uint32_t* n){
    (void)s;*n=2;if(!out&&!cap)return 0;if(cap<2)return 10;
    memset(out,0,2*sizeof *out);
    out[0].train_id=0xffffffffffffffffULL;
    out[0].flags=NIMBY_SERVICE_RUN_VALID|NIMBY_SERVICE_STOP_VALID|NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_ARRIVAL_VALID|NIMBY_SERVICE_DEPARTURE_VALID|NIMBY_SERVICE_COOLDOWN_VALID|NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_LOCATION_VALID|NIMBY_SERVICE_LINE_VALID;
    out[0].line_id=91;out[0].stop_station_id=92;out[0].stop_index=0;
    out[0].game_epoch_seconds=1234;out[0].game_time_us=5000000;
    out[0].arrival_time_us=-1000000;out[0].departure_time_us=0;out[0].dispatch_time_us=1000000;
    out[0].motion_flags=NIMBY_MOTION_PRESENCE|NIMBY_MOTION_DRIVE;out[0].status=NIMBY_SERVICE_SIGNAL_WAIT;out[0].alert=6;
    out[0].location_track_id=9;out[0].location_station_id=93;out[0].stop_track_id=94;out[0].line_kind=1;
    strcpy(out[0].line_name_utf8,"Depot line");out[0].arrival_remaining_seconds=-6;out[0].departure_remaining_seconds=0;out[0].dispatch_remaining_seconds=0;
    out[1]=out[0];out[1].train_id=1;out[1].flags=0;
    return 0;
}
uint32_t NimbyInternal_CopyTrainDetails(NimbySnapshot s,NimbyTrainDetails* out,uint32_t cap,uint32_t* n){
    (void)s;*n=2;if(!out&&!cap)return 0;if(cap<2)return 10;
    memset(out,0,2*sizeof *out);out[0].train_id=0xffffffffffffffffULL;
    out[0].flags=NIMBY_TRAIN_PASSENGERS_VALID|NIMBY_TRAIN_ASSIGNMENT_VALID;
    out[0].schedule_id=61;out[0].shift_id=71;out[0].order_index=0;out[0].order_mode=2;
    out[1]=out[0];out[1].train_id=1;out[1].flags=0;out[1].order_mode=99;return 0;
}
EMPTY(NimbyInternal_CopyPlatforms,NimbyPlatform)
uint32_t NimbyInternal_CopyTrackOccupations(NimbySnapshot s,NimbyTrackUsage* out,uint32_t cap,uint32_t* n){
 *n=s==457&&network_revision?1:0;if(!out&&!cap)return 0;if(cap<*n)return 10;
 if(*n)*out=(NimbyTrackUsage){0x5000000000001ULL,network_revision,.25,.75};
 return 0;
}
uint32_t NimbyInternal_CopyTrackReservations(NimbySnapshot s,NimbyTrackUsage* out,uint32_t cap,uint32_t* n){
 if(s!=457||!network_revision){*n=0;return 8;}return NimbyInternal_CopyTrackOccupations(s,out,cap,n);
}
uint32_t NimbyInternal_CopyTrainPathTracks(NimbySnapshot s,uint64_t t,uint64_t* out,uint32_t cap,uint32_t* n){
 (void)t;if(s!=457||!network_revision){*n=0;return 8;}*n=1;if(!out&&!cap)return 0;if(cap<1)return 10;*out=network_revision;return 0;
}
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
