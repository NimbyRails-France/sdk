#define NIMBY_SDK_BUILD
#include <nimby/signal_observation.hpp>
#include <iostream>
#include <stdexcept>

#define CHECK(x) do {if(!(x))throw std::runtime_error("line "+std::to_string(__LINE__)+": " #x);}while(false)
namespace {
constexpr uint64_t a=(1ULL<<48)|1,b=(1ULL<<48)|0x10001,c=(1ULL<<48)|0x20001;
constexpr uint64_t train=(5ULL<<48)|1,geometricTrain=(5ULL<<48)|0x10001;
constexpr uint64_t sourceId=(8ULL<<48)|1,nextId=(8ULL<<48)|0x10001;
NimbySignal source{sourceId,a,.4,-1,NIMBY_SIGNAL_PATH};
NimbySignal next{nextId,a,.8,-1,NIMBY_SIGNAL_PATH};
bool independentGeometry=false;
template<class T,size_t Extent> uint32_t copied(T* out,uint32_t cap,uint32_t* count,std::span<const T,Extent> rows){
    *count=static_cast<uint32_t>(rows.size());
    if(!out&&!cap)return NIMBY_OK;
    if(!out)return NIMBY_INVALID_ARGUMENT;
    if(cap<rows.size())return NIMBY_BUFFER_TOO_SMALL;
    std::copy(rows.begin(),rows.end(),out);return NIMBY_OK;
}
NimbyTrain trainRow(uint64_t id,uint64_t track,double fraction){
    NimbyTrain row{};row.id=id;row.track_id=track;row.track_fraction=fraction;row.direction=1;
    row.flags=NIMBY_TRAIN_POSITION_VALID;return row;
}
NimbyTrainService serviceRow(uint64_t id){
    NimbyTrainService row{};row.train_id=id;row.flags=NIMBY_SERVICE_PRESENCE_VALID;row.motion_flags=NIMBY_MOTION_PRESENCE;return row;
}
NimbySignalRouteObservation routeRow(){
    // A qualified native prefix reaches the source from another incoming track.
    // Its end has not reached the next signal, so it can never prove Clear.
    NimbySignalRouteObservation row{};
    row.train_id=train;row.signal_id=sourceId;row.route_revision=1;row.observed_at_ms=1000;
    row.head_track_id=c;row.head_fraction=.1;row.distance_m=300;row.head_direction=1;
    row.blocks=1;row.flags=NIMBY_SIGNAL_ROUTE_ORDERED;
    row.source_track_id=a;row.source_fraction=.4;row.source_direction=1;row.source_kind=NIMBY_SIGNAL_PATH;
    return row;
}
}
uint32_t __cdecl NimbyInternal_GetVersion(NimbySdkVersion* out)noexcept{*out={sizeof *out,2,0,9,0};return NIMBY_OK;}
const char* __cdecl NimbyInternal_StatusString(uint32_t)noexcept{return "fixture status";}
uint32_t __cdecl NimbyInternal_OpenProcess(uint32_t,uint32_t,NimbySession* out)noexcept{*out=1;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_CloseSession(NimbySession)noexcept{return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_ReleaseSnapshot(NimbySnapshot)noexcept{return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_GetSnapshotInfo(NimbySnapshot,NimbySnapshotInfo* out)noexcept{*out={};out->struct_size=sizeof *out;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_GetGameSession(NimbySnapshot,NimbyGameSession*)noexcept{return NIMBY_DATA_UNAVAILABLE;}
uint32_t __cdecl NimbyInternal_GetSimulationClock(NimbySnapshot,NimbySimulationClock*)noexcept{return NIMBY_DATA_UNAVAILABLE;}
#define CAPTURE(name) uint32_t __cdecl NimbyInternal_##name(NimbySession,NimbySnapshot* out,uint32_t* stage)noexcept{*out=1;*stage=0;return NIMBY_OK;}
CAPTURE(CaptureSnapshotDiagnostic)
CAPTURE(CaptureSignallingSnapshot)
CAPTURE(CaptureSessionSnapshot)
CAPTURE(CaptureNetworkSnapshotDiagnostic)
uint32_t __cdecl NimbyInternal_CaptureSignallingFor(NimbySession,const char*,NimbySnapshot* out,uint32_t* stage)noexcept{*out=1;*stage=0;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_CaptureSignallingScope(NimbySession,const NimbySignalCaptureScope*,uint32_t,NimbySnapshot* out,uint32_t* stage)noexcept{*out=1;*stage=0;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_CaptureRuntimeSignallingScope(NimbySession,const NimbySignalCaptureScope*,uint32_t,NimbySnapshot* out,uint32_t* stage)noexcept{*out=1;*stage=0;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_CaptureTrainDataSnapshotWithOptions(NimbySession,uint32_t,NimbySnapshot* out,uint32_t* stage)noexcept{*out=1;*stage=0;return NIMBY_OK;}
#define EMPTY_COPY(name,type) uint32_t __cdecl NimbyInternal_Copy##name(NimbySnapshot,type*,uint32_t,uint32_t* count)noexcept{*count=0;return NIMBY_OK;}
EMPTY_COPY(TrainDetails,NimbyTrainDetails)
EMPTY_COPY(TrainMetadata,NimbyTrainMetadata)
EMPTY_COPY(Lines,NimbyLineMetadata)
EMPTY_COPY(Tags,NimbyTag)
EMPTY_COPY(ObjectTagsStates,NimbyObjectTagsState)
EMPTY_COPY(ObjectTags,NimbyObjectTag)
EMPTY_COPY(TrainVehicles,NimbyTrainVehicle)
EMPTY_COPY(VehicleModels,NimbyVehicleModel)
EMPTY_COPY(Tracks,NimbyTrack)
EMPTY_COPY(Stations,NimbyStation)
EMPTY_COPY(Platforms,NimbyPlatform)
EMPTY_COPY(TrackJunctions,NimbyTrackJunction)
EMPTY_COPY(TrackMetrics,NimbyTrackMetric)
EMPTY_COPY(SignalStates,NimbySignalState)
EMPTY_COPY(SignalExtensionsStates,NimbySignalExtensionsState)
EMPTY_COPY(SignalExtensionFields,NimbySignalExtensionField)
EMPTY_COPY(TrackReservations,NimbyTrackUsage)
EMPTY_COPY(TrackOccupations,NimbyTrackUsage)
uint32_t __cdecl NimbyInternal_CopyTrainLineStops(NimbySnapshot,uint64_t,NimbyLineStop*,uint32_t,uint32_t* count)noexcept{*count=0;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_CopyTrainPathTracks(NimbySnapshot,uint64_t,uint64_t*,uint32_t,uint32_t* count)noexcept{*count=0;return NIMBY_OK;}
uint32_t __cdecl NimbyInternal_CopyTrains(NimbySnapshot,NimbyTrain* out,uint32_t cap,uint32_t* count)noexcept{
    const std::array rows{trainRow(train,c,.1),trainRow(geometricTrain,a,.1)};
    return copied(out,cap,count,std::span(rows).first(independentGeometry?2:1));
}
uint32_t __cdecl NimbyInternal_CopyTrainServices(NimbySnapshot,NimbyTrainService* out,uint32_t cap,uint32_t* count)noexcept{
    const std::array rows{serviceRow(train),serviceRow(geometricTrain)};
    return copied(out,cap,count,std::span(rows).first(independentGeometry?2:1));
}
uint32_t __cdecl NimbyInternal_CopySignals(NimbySnapshot,NimbySignal* out,uint32_t cap,uint32_t* count)noexcept{
    const std::array rows{source,next};return copied(out,cap,count,std::span(rows));
}
uint32_t __cdecl NimbyInternal_CopyTrackNodes(NimbySnapshot,NimbyTrackNode* out,uint32_t cap,uint32_t* count)noexcept{
    const std::array<NimbyTrackNode,3> rows{{{a,0,0,0,0},{b,0,0,0,0},{c,0,0,0,0}}};
    return copied(out,cap,count,std::span(rows));
}
uint32_t __cdecl NimbyInternal_CopySignalTextures(NimbySnapshot,NimbySignalTexture* out,uint32_t cap,uint32_t* count)noexcept{
    NimbySignalTexture row{};row.signal_id=sourceId;row.flags=NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID;std::strcpy(row.textures_id_utf8,"atlas");
    const std::array rows{row};return copied(out,cap,count,std::span(rows));
}
int main(){try{
    nimby::detail::ObservationSession session(42);
    constexpr std::array<std::string_view,1> catalogues{"atlas"};
    constexpr std::array<nimby::SignalApproachScope,1> scopes{{{"atlas",2}}};
    const auto checkRejected=[&](NimbySignalRouteObservation row){
        const auto snapshot=session.captureSignalling();
        nimby::detail::SignalRoutes routes({row},{{train,sourceId,a,.4,.7}});
        const nimby::SignalTopology topology(snapshot->getAllSignals(),snapshot->getAllTrackNodes(),{},nimby::SignalDirectionConvention::Forward);
        const auto approaches=nimby::observeSignalApproaches(*snapshot,topology,nimby::Milliseconds{1000},2,routes.rows());
        CHECK(!approaches.contains(sourceId));
        CHECK(!routes.block(row,nimby::Signal(source)));
        const auto observed=nimby::observeSignals(*snapshot,catalogues,512,nimby::Milliseconds{1000},{},false,scopes,&routes);
        if(source.kind==NIMBY_SIGNAL_PATH){CHECK(observed.size()==1);CHECK(observed[0].approachingTrain==0);CHECK(!observed[0].routeConflict);CHECK(observed[0].nextSignal==nextId);CHECK(observed[0].boundaryKnown);}
        else CHECK(observed.empty());
    };
    const auto native=routeRow();
    CHECK(nimby::detail::SignalRoutes::sourceMatches(native,nimby::Signal(source)));
    {
        const auto snapshot=session.captureSignalling();
        nimby::detail::SignalRoutes routes({native},{{train,sourceId,a,.4,.7}});
        const auto observed=nimby::observeSignals(*snapshot,catalogues,512,nimby::Milliseconds{1000},{},false,scopes,&routes);
        CHECK(observed.size()==1&&observed[0].approachingTrain==train);
        CHECK(!observed[0].boundaryKnown&&observed[0].nextSignal==0); // Accepted native prefix substituted.
    }
    auto moved=native;moved.source_fraction=.5;checkRejected(moved);
    for(const auto flag:{NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED,NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED}){
        auto crossed=native;crossed.flags|=flag;
        for(const bool copiedSections:{false,true})for(const bool genericBlocked:{false,true}){
            crossed.flags=native.flags|flag|(genericBlocked?NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED:0);
            const auto snapshot=session.captureSignalling();
            const std::vector<NimbySignalRouteSection> sections=copiedSections?
                std::vector<NimbySignalRouteSection>{{train,sourceId,a,.4,.7}}:std::vector<NimbySignalRouteSection>{};
            nimby::detail::SignalRoutes routes({crossed},sections);
            const auto observed=nimby::observeSignals(*snapshot,catalogues,512,nimby::Milliseconds{1000},{},false,scopes,&routes);
            CHECK(observed.size()==1&&observed[0].approachingTrain==train);
            CHECK(observed[0].routeConflict==(flag==NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED?
                nimby::RouteConflict::CrossingOccupied:nimby::RouteConflict::CrossingReserved));
        }
        crossed.source_fraction=.5;checkRejected(crossed);
        crossed=native;crossed.flags|=flag;crossed.train_id+=1;checkRejected(crossed);
    }
    {
        auto blocked=native;blocked.flags|=NIMBY_SIGNAL_ROUTE_NATIVE_BLOCKED;
        const auto snapshot=session.captureSignalling();nimby::detail::SignalRoutes routes({blocked},{});
        const auto observed=nimby::observeSignals(*snapshot,catalogues,512,nimby::Milliseconds{1000},{},false,scopes,&routes);
        CHECK(observed.size()==1&&observed[0].approachingTrain==train&&!observed[0].routeConflict);
        // Generic refusal must not turn a longitudinal semaphore into Carré.
        for(const auto badFlags:{NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_CROSSING_RESERVED,
                                NIMBY_SIGNAL_ROUTE_CROSSING_OCCUPIED|NIMBY_SIGNAL_ROUTE_NATIVE_CLEAR}){
            blocked=native;blocked.flags|=badFlags;checkRejected(blocked);
        }
    }
    moved.source_fraction=std::nextafter(source.track_fraction,1.0);checkRejected(moved); // Exact anchor, no metres tolerance.
    auto otherTrack=native;otherTrack.source_track_id=b;checkRejected(otherTrack);
    auto reversed=native;reversed.source_direction=-1;checkRejected(reversed);
    auto otherKind=native;otherKind.source_kind=NIMBY_SIGNAL_ONE_WAY;checkRejected(otherKind);
    source.kind=NIMBY_SIGNAL_ONE_WAY;checkRejected(native);source.kind=NIMBY_SIGNAL_PATH;
    {
        // Rejecting one native approach must not erase an independently valid
        // geometric approach or replace its geometric downstream block.
        independentGeometry=true;const auto snapshot=session.captureSignalling();
        nimby::detail::SignalRoutes routes({moved},{{train,sourceId,a,.4,.7}});
        const auto observed=nimby::observeSignals(*snapshot,catalogues,512,nimby::Milliseconds{1000},{},false,scopes,&routes);
        CHECK(observed.size()==1&&observed[0].approachingTrain==geometricTrain);
        CHECK(observed[0].boundaryKnown&&observed[0].nextSignal==nextId);
        independentGeometry=false;
    }
    for(const auto fraction:{0.0,1.0}){
        auto endpoint=native;endpoint.source_fraction=fraction;auto boundary=source;boundary.track_fraction=fraction;
        CHECK(nimby::detail::SignalRoutes::sourceMatches(endpoint,nimby::Signal(boundary)));
        source=boundary;const auto snapshot=session.captureSignalling();
        const auto section=fraction==0?NimbySignalRouteSection{train,sourceId,a,0,.7}:NimbySignalRouteSection{train,sourceId,b,0,.3};
        nimby::detail::SignalRoutes routes({endpoint},{section});
        const auto observed=nimby::observeSignals(*snapshot,catalogues,512,nimby::Milliseconds{1000},{},false,scopes,&routes);
        CHECK(observed.size()==1&&observed[0].approachingTrain==train);
        CHECK(!observed[0].boundaryKnown&&observed[0].nextSignal==0);
        endpoint.source_fraction=std::nextafter(fraction,fraction==0?1.0:0.0);
        CHECK(!nimby::detail::SignalRoutes::sourceMatches(endpoint,nimby::Signal(boundary)));
    }
    source.track_fraction=.4;
    std::cout<<"signal approach snapshot/native source agreement: PASS\n";return 0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
