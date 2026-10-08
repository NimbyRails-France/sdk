#include <nimby/detail/observation_session.hpp>
#include "../kotlin/native/train_observation.hpp"
#include <atomic>
#include <cstdio>
#include <cstring>
#include <future>
#include <set>
#include <type_traits>

#define REQUIRE(x) do { if (!(x)) throw std::runtime_error("Failed line " + std::to_string(__LINE__) + ": " #x); } while (false)
namespace {
std::atomic<int> captures{}, releases{}, closes{}, active{}, peak{}, delayMs{};
std::atomic<uint32_t> captureStatus{NIMBY_OK}, trackStatus{NIMBY_OK};
std::atomic<bool> reservationsAvailable{true};
std::atomic<bool> emptyReservations{false}, occupationsAvailable{true}, occupiedPlatform{false};
std::atomic<bool> groupedPlatforms{false};
std::atomic<bool> duplicateExtension{false};
std::atomic<bool> gameSessionAvailable{false};
std::atomic<bool> richTrainRows{false};
std::atomic<uint32_t> lastTrainDataFlags{}, richTableCopies{};
std::atomic<unsigned> topologyCaptures{};
std::atomic<bool> topologyMetrics{false};
std::atomic<double> topologySignalFraction{.5};
std::atomic<uint32_t> nodeStatus{NIMBY_OK};
std::mutex copiesMutex;
std::set<std::string> copiedTables;
void recorded(const char* table){std::lock_guard lock(copiesMutex);copiedTables.insert(table);}
std::set<NimbySnapshot> handles;
std::mutex handlesMutex;
template<class T>
uint32_t copy(NimbySnapshot snapshot, T* out, uint32_t capacity, uint32_t* count,
              const std::vector<T>& rows) {
    std::lock_guard lock(handlesMutex);
    if (!handles.contains(snapshot)) return NIMBY_INVALID_HANDLE;
    *count = static_cast<uint32_t>(rows.size());
    if (!out && !capacity) return NIMBY_OK;
    if (!out) return NIMBY_INVALID_ARGUMENT;
    if (capacity < rows.size()) return NIMBY_BUFFER_TOO_SMALL;
    for(size_t i=0;i<rows.size();++i)out[i]=rows[i];
    return NIMBY_OK;
}
}
static uint32_t runtimeMajor = 0, runtimeMinor = 9, runtimeAbi = 2, runtimePatch = 0;
static std::atomic<int64_t> clockEpoch{1781123300};
static std::atomic<bool> clockAvailable{true};
uint32_t __cdecl NimbyInternal_GetVersion(NimbySdkVersion* out) noexcept {
    *out = {sizeof *out, runtimeAbi, runtimeMajor, runtimeMinor, runtimePatch}; return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_GetSimulationClock(NimbySnapshot,NimbySimulationClock* out) noexcept {
    if(!clockAvailable)return NIMBY_DATA_UNAVAILABLE;
    *out={sizeof *out,0,clockEpoch,542534653};return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_SetSimulationDateTime(NimbySession,int64_t seconds,NimbySimulationClock* out) noexcept {
    clockEpoch=seconds-5425346;*out={sizeof *out,0,clockEpoch,542534653};return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_SetSimulationDateTimeAndRecalculateTrains(NimbySession session,int64_t seconds,NimbySimulationClock* out,uint32_t* count) noexcept {
    *count=345;return NimbyInternal_SetSimulationDateTime(session,seconds,out);
}
const char* __cdecl NimbyInternal_StatusString(uint32_t code) noexcept {
    return code == NIMBY_PROCESS_EXITED ? "Process exited" : "Test status";
}
uint32_t __cdecl NimbyInternal_OpenProcess(uint32_t, uint32_t pid, NimbySession* out) noexcept {
    *out = pid; return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_CloseSession(NimbySession) noexcept { ++closes; return NIMBY_OK; }
uint32_t __cdecl NimbyInternal_CaptureSnapshot(NimbySession, NimbySnapshot* out) noexcept {
    const int simultaneous = ++active;
    if (simultaneous > peak) peak = simultaneous;
    std::this_thread::sleep_for(std::chrono::milliseconds{delayMs.load()});
    --active;
    if (captureStatus != NIMBY_OK) { *out = 0; return captureStatus; }
    *out = static_cast<NimbySnapshot>(++captures);
    std::lock_guard lock(handlesMutex);
    handles.insert(*out);
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_ReleaseSnapshot(NimbySnapshot s) noexcept {
    std::lock_guard lock(handlesMutex);
    if (handles.erase(s) != 1) return NIMBY_INVALID_HANDLE;
    ++releases; return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_CaptureSnapshotDiagnostic(NimbySession s,NimbySnapshot* out,uint32_t* stage) noexcept {
    *stage=0;
    const auto status=NimbyInternal_CaptureSnapshot(s,out);
    if(status==NIMBY_DATA_UNAVAILABLE)*stage=2;
    return status;
}
uint32_t __cdecl NimbyInternal_CaptureSignallingSnapshot(NimbySession s,NimbySnapshot* out,uint32_t* stage) noexcept {
    return NimbyInternal_CaptureSnapshotDiagnostic(s,out,stage);
}
uint32_t __cdecl NimbyInternal_CaptureSignallingFor(NimbySession s,const char*,NimbySnapshot* out,uint32_t* stage) noexcept {
    return NimbyInternal_CaptureSnapshotDiagnostic(s,out,stage);
}
uint32_t __cdecl NimbyInternal_CaptureSignallingScope(NimbySession s,const NimbySignalCaptureScope*,uint32_t,NimbySnapshot* out,uint32_t* stage) noexcept {
    return NimbyInternal_CaptureSnapshotDiagnostic(s,out,stage);
}
uint32_t __cdecl NimbyInternal_CaptureSessionSnapshot(NimbySession s,NimbySnapshot* out,uint32_t* stage) noexcept {
    return NimbyInternal_CaptureSnapshotDiagnostic(s,out,stage);
}
uint32_t __cdecl NimbyInternal_CaptureNetworkSnapshotDiagnostic(NimbySession s,NimbySnapshot* out,uint32_t* stage) noexcept {
    ++topologyCaptures;
    return NimbyInternal_CaptureSnapshotDiagnostic(s,out,stage);
}
uint32_t __cdecl NimbyInternal_CaptureTrainDataSnapshotWithOptions(NimbySession s,uint32_t flags,NimbySnapshot* out,uint32_t* stage) noexcept {
    lastTrainDataFlags=flags;return NimbyInternal_CaptureSnapshotDiagnostic(s,out,stage);
}
uint32_t __cdecl NimbyInternal_CaptureTrainDataSnapshotDiagnostic(NimbySession s,NimbySnapshot* out,uint32_t* stage) noexcept {
    return NimbyInternal_CaptureTrainDataSnapshotWithOptions(s,NIMBY_TRAIN_DATA_ALL,out,stage);
}
uint32_t __cdecl NimbyInternal_GetGameSession(NimbySnapshot, NimbyGameSession* out) noexcept {
    *out={};out->struct_size=sizeof(*out);
    if(!gameSessionAvailable)return NIMBY_DATA_UNAVAILABLE;
    out->generation=7;out->world_value[0]=0xaf;return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_GetSnapshotInfo(NimbySnapshot, NimbySnapshotInfo* out) noexcept {
    *out = {}; out->struct_size = sizeof *out; out->process_id = 42;
    out->captured_unix_ms = 1000;
    std::strcpy(out->game_sha256, "fixture");
    return NIMBY_OK;
}
uint32_t __cdecl NimbyInternal_CopyTrackMetrics(NimbySnapshot s, NimbyTrackMetric* out, uint32_t capacity, uint32_t* count) noexcept {
    recorded("TrackMetrics");
    if(topologyMetrics)return copy(s,out,capacity,count,std::vector<NimbyTrackMetric>{{20,1234.5}});
    *count=0;return NIMBY_DATA_UNAVAILABLE;
}
uint32_t __cdecl NimbyInternal_CopyTrains(NimbySnapshot s, NimbyTrain* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("Trains");
    NimbyTrain train{}; train.id = 0x10000000001ULL; train.track_id = 20;
    if(richTrainRows)train.id=0x5000000000001;
    train.flags = NIMBY_TRAIN_ACTIVE_DRIVE | NIMBY_TRAIN_SPEED_VALID | NIMBY_TRAIN_POSITION_VALID;
    train.speed_mps = double(s); train.track_fraction = .25; train.direction = -1;
    std::strcpy(train.name_utf8, "Test train");
    NimbyTrain unknown{}; unknown.id = 2;
    // Nonzero raw data with no flags must never be exposed as valid.
    unknown.speed_mps = 99; unknown.track_id = 20;
    return copy(s, out, cap, n, std::vector{train, unknown});
}
uint32_t __cdecl NimbyInternal_CopyTrainServices(NimbySnapshot s,NimbyTrainService* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("TrainServices");
    NimbyTrainService service{};service.train_id=0x5000000000001;
    service.flags=NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_DEPARTURE_VALID;
    service.status=NIMBY_SERVICE_STATION_STOP;service.departure_remaining_seconds=42;
    if(richTrainRows){
        service.flags|=NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_ARRIVAL_VALID|NIMBY_SERVICE_COOLDOWN_VALID|NIMBY_SERVICE_LOCATION_VALID|NIMBY_SERVICE_RUN_VALID|NIMBY_SERVICE_STOP_VALID|NIMBY_SERVICE_LINE_VALID;
        service.game_epoch_seconds=1000;service.arrival_time_us=-1;service.dispatch_time_us=1;
        service.motion_flags=NIMBY_MOTION_PRESENCE;service.line_id=0x4000000000001;service.line_kind=1;
        service.location_track_id=service.stop_track_id=20;service.location_station_id=service.stop_station_id=30;
        std::strcpy(service.line_name_utf8,"Depot line");
    }
    NimbyTrainService unknown{};unknown.train_id=2;
    return copy(s,out,cap,n,std::vector{service,unknown});
}
uint32_t __cdecl NimbyInternal_CopyTrainDetails(NimbySnapshot s,NimbyTrainDetails* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("TrainDetails");
    NimbyTrainDetails row{};row.train_id=0x10000000001ULL;row.flags=NIMBY_TRAIN_PASSENGERS_VALID;row.passenger_count=123;
    if(richTrainRows){row.train_id=0x5000000000001;row.flags|=NIMBY_TRAIN_ASSIGNMENT_VALID;row.schedule_id=0x6000000000001;row.shift_id=7;row.order_index=0;}
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbyInternal_CopyTrainMetadata(NimbySnapshot s,NimbyTrainMetadata* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("TrainMetadata");
    ++richTableCopies;
    NimbyTrainMetadata row{};row.train_id=0x5000000000001;
    row.flags=NIMBY_TRAIN_PREDICTED_DELAY_VALID;row.predicted_arrival_delay_us=-1500000;
    row.configured={511,8,500,0,40,200,600000,1,8000000,200000};
    row.current.flags=NIMBY_CHARACTERISTICS_MAX_SPEED_VALID|NIMBY_CHARACTERISTICS_COMPOSITION_VALID;row.current.maximum_speed_mps=35;row.current.length_m=999;
    return copy(s,out,cap,n,richTrainRows?std::vector{row}:std::vector<NimbyTrainMetadata>{});
}
uint32_t __cdecl NimbyInternal_CopyLines(NimbySnapshot s,NimbyLineMetadata* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("Lines");
    if(!richTrainRows){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    NimbyLineMetadata row{};row.line_id=0x4000000000001;
    row.flags=NIMBY_LINE_PARENT_VALID|NIMBY_LINE_KIND_VALID|NIMBY_LINE_NAME_VALID;row.kind=1;std::strcpy(row.name_utf8,"Depot line");
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbyInternal_CopyTags(NimbySnapshot s,NimbyTag* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("Tags");
    if(!richTrainRows){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    NimbyTag row{};row.tag_id=109;std::strcpy(row.name_utf8,"Regional");
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbyInternal_CopyObjectTagsStates(NimbySnapshot s,NimbyObjectTagsState* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("ObjectTagsStates");
    return copy(s,out,cap,n,richTrainRows?std::vector<NimbyObjectTagsState>{{0x5000000000001,1,0},{0x4000000000001,1,0},{2,0,0}}:std::vector<NimbyObjectTagsState>{});
}
uint32_t __cdecl NimbyInternal_CopyObjectTags(NimbySnapshot s,NimbyObjectTag* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("ObjectTags");
    return copy(s,out,cap,n,richTrainRows?std::vector<NimbyObjectTag>{{0x5000000000001,109},{0x5000000000001,999}}:std::vector<NimbyObjectTag>{});
}
uint32_t __cdecl NimbyInternal_CopyTrainLineStops(NimbySnapshot s,uint64_t id,NimbyLineStop* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("TrainLineStops");
    if(id==2){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    NimbyLineStop row{};row.line_id=4;row.track_id=20;row.station_id=30;row.flags=NIMBY_LINE_STOP_TIMES_VALID;
    row.arrival_offset_seconds=120;row.departure_offset_seconds=150;
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbyInternal_CopyTrainVehicles(NimbySnapshot s,NimbyTrainVehicle* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("TrainVehicles");
    return copy(s,out,cap,n,richTrainRows?std::vector<NimbyTrainVehicle>{{0x5000000000001,101,0,0},{0x5000000000001,999,1,0}}:std::vector<NimbyTrainVehicle>{});
}
uint32_t __cdecl NimbyInternal_CopyVehicleModels(NimbySnapshot s,NimbyVehicleModel* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("VehicleModels");
    if(!richTrainRows){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    NimbyVehicleModel model{};model.model_id=101;std::strcpy(model.code_utf8,"metro");std::strcpy(model.name_en_utf8,"Metro vehicle");std::strcpy(model.source_name_utf8,"Base game");
    return copy(s,out,cap,n,std::vector{model});
}
uint32_t __cdecl NimbyInternal_CopyTracks(NimbySnapshot s, NimbyTrack* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("Tracks");
    if (trackStatus != NIMBY_OK) return trackStatus;
    return copy(s, out, cap, n, std::vector<NimbyTrack>{{20, 30, 40}, {21, 0, 20}});
}
uint32_t __cdecl NimbyInternal_CopyPlatforms(NimbySnapshot s,NimbyPlatform* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("Platforms");
    NimbyPlatform row{};row.track_id=20;row.station_id=30;row.flags=NIMBY_PLATFORM_NAME_VALID;std::strcpy(row.name_utf8,"A");
    if(groupedPlatforms){
        auto second=row;second.track_id=21;
        auto other=row;other.track_id=22;std::strcpy(other.name_utf8,"B");
        auto unknown=row;unknown.track_id=23;unknown.flags=0;
        auto unknown2=unknown;unknown2.track_id=24;
        auto empty=row;empty.track_id=25;empty.name_utf8[0]=0;
        auto empty2=empty;empty2.track_id=26;
        auto elsewhere=row;elsewhere.station_id=31;elsewhere.track_id=29;
        return copy(s,out,cap,n,std::vector{row,second,other,unknown,unknown2,empty,empty2,elsewhere});
    }
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbyInternal_CopyStations(NimbySnapshot s, NimbyStation* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("Stations");
    NimbyStation station{}; station.id = 30; // Unresolved automatic name.
    if(richTrainRows)std::strcpy(station.name_utf8,"Joined station");
    if(groupedPlatforms){auto other=station;other.id=31;return copy(s,out,cap,n,std::vector{station,other});}
    return copy(s, out, cap, n, std::vector{station});
}
uint32_t __cdecl NimbyInternal_CopySignals(NimbySnapshot s, NimbySignal* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("Signals");
    return copy(s, out, cap, n, std::vector<NimbySignal>{{40, 20, topologySignalFraction.load(), 1, NIMBY_SIGNAL_PATH}});
}
uint32_t __cdecl NimbyInternal_CopySignalStates(NimbySnapshot s, NimbySignalState* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("SignalStates");
    NimbySignalState state{}; state.signal_id = 40; state.flags = NIMBY_SIGNAL_TEXTURE_STATE_VALID;
    return copy(s, out, cap, n, std::vector{state});
}
uint32_t __cdecl NimbyInternal_CopySignalTextures(NimbySnapshot s, NimbySignalTexture* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("SignalTextures");
    NimbySignalTexture texture{}; texture.signal_id = 40;
    return copy(s, out, cap, n, std::vector{texture});
}
uint32_t __cdecl NimbyInternal_CopySignalExtensionsStates(NimbySnapshot s,NimbySignalExtensionsState* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("SignalExtensionsStates");
    return copy(s,out,cap,n,std::vector<NimbySignalExtensionsState>{{40,1,0},{41,1,0},{42,0,0}});
}
uint32_t __cdecl NimbyInternal_CopySignalExtensionFields(NimbySnapshot s,NimbySignalExtensionField* out,uint32_t cap,uint32_t* n) noexcept {
    recorded("SignalExtensionFields");
    NimbySignalExtensionField header{};header.signal_id=40;header.script_id=7;std::strcpy(header.type_name,"TestSettings");
    auto value=header;std::strcpy(value.field_name,"active");value.boolean_valid=1;value.boolean_value=0;
    auto unknown=header;std::strcpy(unknown.field_name,"nonBoolean");
    std::vector<NimbySignalExtensionField> rows{header,value,unknown};
    if(duplicateExtension){header.script_id=8;rows.push_back(header);}
    return copy(s,out,cap,n,rows);
}
uint32_t __cdecl NimbyInternal_CopyTrackNodes(NimbySnapshot s, NimbyTrackNode* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("TrackNodes");
    if(nodeStatus!=NIMBY_OK)return nodeStatus;
    return copy(s, out, cap, n, std::vector<NimbyTrackNode>{{20, 0, 21, 10, 15}});
}
uint32_t __cdecl NimbyInternal_CopyTrackJunctions(NimbySnapshot s, NimbyTrackJunction* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("TrackJunctions");
    return copy(s, out, cap, n, std::vector<NimbyTrackJunction>{{21,20,.5,1,-1}});
}
uint32_t __cdecl NimbyInternal_CopyTrackReservations(NimbySnapshot s, NimbyTrackUsage* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("TrackReservations");
    if (!reservationsAvailable) { *n = 0; return NIMBY_DATA_UNAVAILABLE; }
    if(groupedPlatforms)return copy(s,out,cap,n,std::vector<NimbyTrackUsage>{{0x10000000001ULL,20,0,1},{0x10000000001ULL,21,0,1},{2,21,0,1}});
    const auto rows = emptyReservations ? std::vector<NimbyTrackUsage>{}
        : std::vector<NimbyTrackUsage>{{0x10000000001ULL, 20, .1, .6}};
    return copy(s, out, cap, n, rows);
}
uint32_t __cdecl NimbyInternal_CopyTrackOccupations(NimbySnapshot s, NimbyTrackUsage* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("TrackOccupations");
    if(!occupationsAvailable){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    if(groupedPlatforms)return copy(s,out,cap,n,std::vector<NimbyTrackUsage>{{0x10000000001ULL,21,0,.5},{0x10000000001ULL,21,.5,1},{2,21,0,1}});
    const auto rows=occupiedPlatform?std::vector<NimbyTrackUsage>{{0x10000000001ULL,20,0,.5},{0x10000000001ULL,20,.5,1},{2,20,.1,.2}}:std::vector<NimbyTrackUsage>{};
    return copy(s, out, cap, n, rows);
}
uint32_t __cdecl NimbyInternal_CopyTrainPathTracks(NimbySnapshot s, uint64_t id, uint64_t* out, uint32_t cap, uint32_t* n) noexcept {
    recorded("TrainPathTracks");
    if (id == 2) { *n = 0; return NIMBY_DATA_UNAVAILABLE; }
    return copy(s, out, cap, n, std::vector<uint64_t>{20, 21});
}
int main() {
    using namespace std::chrono_literals;
    constexpr nimby::Id trainId = 0x10000000001ULL;
    try {
        {
            // Concurrent relation reads build once; subsequent lookups must
            // not rescan a large immutable snapshot for every track.
            struct Row {nimby::Id group,value;};
            std::vector<Row> rows;rows.reserve(100000);
            for(unsigned i=0;i<100000;++i)rows.push_back({i%1000+1,i});
            nimby::detail::RelationIndex index;std::atomic<unsigned> visited=0;
            auto key=[&](const Row& row){++visited;return row.group;};
            std::vector<std::future<void>> readers;
            for(unsigned thread=0;thread<4;++thread)readers.push_back(std::async(std::launch::async,[&]{
                for(unsigned id=1;id<=1000;++id){
                    const auto found=index.select(rows,id,key);REQUIRE(found.size()==100);
                    for(const auto& row:found)REQUIRE(row.group==id&&row.value%1000+1==id);
                }
            }));
            for(auto& reader:readers)reader.get();
            REQUIRE(visited==rows.size());
            REQUIRE(index.select(rows,1001,key).empty());
            REQUIRE(visited==rows.size());
        }
        REQUIRE(nimby::getVersion().major == 0 && nimby::getVersion().minor == 9 && nimby::getVersion().abi == 2);
        for(const auto version:std::array{nimby::Version{0,7,0,2},nimby::Version{0,8,0,2},
                nimby::Version{0,10,0,2},nimby::Version{1,9,0,2},nimby::Version{0,9,0,1},nimby::Version{0,9,0,3}}) {
            runtimeMajor=version.major;runtimeMinor=version.minor;runtimeAbi=version.abi;
            bool rejected = false;
            try { (void)nimby::getVersion(); }
            catch (const nimby::Exception& e) { rejected = e.code() == nimby::ErrorCode::InvalidArgument; }
            REQUIRE(rejected);
        }
        runtimeMajor = 0; runtimeMinor = 9; runtimeAbi = 2;
        // The new minor begins at patch zero; capabilities must not depend on
        // the patch threshold from the previous 0.7 development series.
        REQUIRE(nimby::getVersion().patch == 0);
        runtimePatch = 1;
        REQUIRE(nimby::getVersion().patch == 1);
        runtimePatch = 0;
        NimbySimulationClock historical{sizeof(NimbySimulationClock),0,-946771200,53};
        REQUIRE(nimby::SimulationClock{historical}.getDateTimeUtcString()=="1940-01-01T00:00:00.530Z");
        // Active Drive no longer substitutes for explicit speed validity.
        NimbyTrain speedRecord{};
        speedRecord.speed_mps=12.5;
        speedRecord.flags=NIMBY_TRAIN_ACTIVE_DRIVE;
        REQUIRE(!nimby::Train{speedRecord}.getSpeedKmh());
        REQUIRE(!nimby::Train{speedRecord}.isSpeedDefaulted());
        NimbyTrainService unknownService{};unknownService.departure_remaining_seconds=99;
        REQUIRE(!nimby::TrainService{unknownService}.getStatus());
        REQUIRE(!nimby::TrainService{unknownService}.isHidden());
        REQUIRE(!nimby::TrainService{unknownService}.isOnNetwork());
        NimbyTrainService locatedService{};
        locatedService.flags = NIMBY_SERVICE_STATE_VALID;
        REQUIRE(nimby::TrainService{locatedService}.isOnNetwork() == false);
        locatedService.motion_flags = NIMBY_MOTION_HIDDEN;
        REQUIRE(nimby::TrainService{locatedService}.isOnNetwork() == true);
        REQUIRE(nimby::TrainService{locatedService}.isHidden() == true);
        locatedService.flags=NIMBY_SERVICE_PRESENCE_VALID;
        REQUIRE(nimby::TrainService{locatedService}.isHidden()==true);
        REQUIRE(nimby::TrainService{locatedService}.isOnNetwork()==true);
        REQUIRE(!nimby::TrainService{locatedService}.getStatus());
        REQUIRE(!nimby::TrainService{locatedService}.getMotionFlags());
        locatedService.motion_flags=NIMBY_MOTION_DRIVE;
        REQUIRE(nimby::TrainService{locatedService}.isOnNetwork()==true);
        REQUIRE(nimby::TrainService{locatedService}.isHidden()==false);
        REQUIRE(!nimby::TrainService{unknownService}.getDepartureRemainingSeconds());
        unknownService.flags=NIMBY_SERVICE_DEPARTURE_VALID;
        REQUIRE(!nimby::TrainService{unknownService}.getDepartureRemainingSeconds()); // Needs game clock too.
        unknownService.flags|=NIMBY_SERVICE_CALENDAR_VALID;
        unknownService.game_epoch_seconds=100;unknownService.departure_time_us=86402000000;
        REQUIRE(nimby::TrainService{unknownService}.getDepartureCalendarSeconds()==86502);
        speedRecord.flags=NIMBY_TRAIN_SPEED_VALID;
        REQUIRE(nimby::Train{speedRecord}.getSpeedMps()==12.5);
        speedRecord.speed_mps=0;
        speedRecord.flags=NIMBY_TRAIN_SPEED_VALID|NIMBY_TRAIN_SPEED_DEFAULTED;
        REQUIRE(nimby::Train{speedRecord}.getSpeedKmh()==0.0);
        REQUIRE(nimby::Train{speedRecord}.isSpeedDefaulted());
        speedRecord.flags=NIMBY_TRAIN_POSITION_VALID;
        REQUIRE(!nimby::Train{speedRecord}.getSpeedKmh());
        REQUIRE(!nimby::Train{speedRecord}.isSpeedDefaulted());
        static_assert(!std::is_copy_constructible_v<nimby::detail::ObservationSession>);
        nimby::Snapshot::Ptr retained;
        {
            auto client = nimby::detail::ObservationSession(42);
            retained = client.capture();
            REQUIRE(!retained->getGameSession());
            gameSessionAvailable=true;
            const auto identified=client.capture();
            REQUIRE(identified->getGameSession()&&identified->getGameSession()->generation==7);
            REQUIRE(identified->getGameSession()->worldId=="af"+std::string(62,'0'));
            gameSessionAvailable=false;
            {
                // A topology request is a new native capture, not a cached Full
                // snapshot. Only the four tables consumed by ToolContext cross
                // the C++ boundary; unrelated train/texture failures cannot enter.
                topologyMetrics=true;
                const auto before=topologyCaptures.load();
                copiedTables.clear();
                const auto first=client.capture(nimby::SnapshotScope::NetworkTopology);
                REQUIRE((copiedTables==std::set<std::string>{"TrackNodes","TrackJunctions","TrackMetrics","Signals"}));
                REQUIRE(first->getAllTrackNodes().size()==1&&first->getAllTrackJunctions().size()==1&&first->getAllSignals().size()==1);
                REQUIRE(first->getTrackMetrics()&&first->getTrackMetrics()->front().length_m==1234.5);
                REQUIRE(first->getAllTrains().empty()&&first->getAllTrainServices().empty()&&first->getAllStations().empty());
                topologySignalFraction=.75;
                const auto second=client.capture(nimby::SnapshotScope::NetworkTopology);
                REQUIRE(topologyCaptures==before+2);
                REQUIRE(first->getAllSignals().front().getFraction()==.5&&second->getAllSignals().front().getFraction()==.75);
                topologySignalFraction=.5;topologyMetrics=false;
                REQUIRE(!client.capture(nimby::SnapshotScope::NetworkTopology)->getTrackMetrics());
                nodeStatus=NIMBY_IO_ERROR;
                bool rejected=false;
                try{client.capture(nimby::SnapshotScope::NetworkTopology);}catch(const nimby::Exception& e){rejected=e.code()==nimby::ErrorCode::IoError;}
                nodeStatus=NIMBY_OK;REQUIRE(rejected&&captures==releases);
                captureStatus=NIMBY_DATA_UNAVAILABLE;rejected=false;
                try{client.capture(nimby::SnapshotScope::NetworkTopology);}catch(const nimby::Exception& e){rejected=e.code()==nimby::ErrorCode::DataUnavailable;}
                captureStatus=NIMBY_OK;REQUIRE(rejected&&captures==releases);
            }
            REQUIRE(retained->getSignalSettings(40,"TestSettings").status==nimby::SettingsStatus::Present);
            REQUIRE(retained->getSignalSettings(40,"TestSettings").getBoolean("active")==false);
            REQUIRE(!retained->getSignalSettings(40,"TestSettings").getBoolean("nonBoolean"));
            REQUIRE(!retained->getSignalSettings(40,"TestSettings").getBoolean("missing"));
            REQUIRE(retained->getSignalSettings(41,"TestSettings").status==nimby::SettingsStatus::Absent);
            REQUIRE(retained->getSignalSettings(42,"TestSettings").status==nimby::SettingsStatus::Unavailable);
            REQUIRE(retained->getSignalSettings(43,"TestSettings").status==nimby::SettingsStatus::Unavailable);
            duplicateExtension=true;
            REQUIRE(client.capture()->getSignalSettings(40,"TestSettings").status==nimby::SettingsStatus::Unavailable);
            duplicateExtension=false;
            REQUIRE(retained->getSignalSettings(40,"TestSettings").status==nimby::SettingsStatus::Present);
            REQUIRE(retained->getSimulationClock().has_value());
            const auto originalClock=retained->getSimulationClock()->getDateTimeUtc();
            const auto changedClock=client.setSimulationDateTime(std::chrono::sys_seconds{std::chrono::seconds{-946771200}});
            REQUIRE(changedClock.getDateTimeUtcString()=="1940-01-01T00:00:00.530Z");
            REQUIRE(retained->getSimulationClock()->getDateTimeUtc()==originalClock);
            REQUIRE(client.capture()->getSimulationClock()->getDateTimeUtc()==changedClock.getDateTimeUtc());
            const auto reset=client.setSimulationDateTimeAndRecalculateTrains(std::chrono::sys_seconds{std::chrono::seconds{-946767600}});
            REQUIRE(reset.interventions==345);
            REQUIRE(reset.clock.getDateTimeUtcString()=="1940-01-01T01:00:00.530Z");
            REQUIRE(retained->getSimulationClock()->getDateTimeUtc()==originalClock);
            clockAvailable=false;
            REQUIRE(!client.capture()->getSimulationClock());
            clockAvailable=true;
            REQUIRE(captures == releases);
            REQUIRE(retained->getAllTrains().size() == 2);
            REQUIRE(retained->getAllTrainServices().size()==2);
            REQUIRE(retained->getTrainServiceById(0x5000000000001)->getDepartureRemainingSeconds()==42);
            REQUIRE(!retained->getTrainServiceById(2)->getStatus());
            REQUIRE(!retained->getTrainServiceById(999));
            {
                const auto copies=richTableCopies.load();client.capture();REQUIRE(richTableCopies==copies);
                richTrainRows=true;const auto rich=client.captureTrainData();richTrainRows=false;
                REQUIRE(lastTrainDataFlags==NIMBY_TRAIN_DATA_ALL);
                namespace wire=nimby::kotlin::trainData;
                std::vector<int64_t> integers(2+wire::pageSize*wire::integerFields);
                std::vector<double> numbers(wire::pageSize*wire::numberFields);
                std::vector<char> names(wire::pageSize*wire::textFields*wire::nameBytes);
                auto read=[&](){return wire::trains(*rich,integers.data(),int(integers.size()),numbers.data(),int(numbers.size()),names.data(),int(names.size()));};
                integers[1]=2;REQUIRE(read()==NIMBY_OK);
                REQUIRE(integers[2]==0x5000000000001&&integers[2+22]==123);
                REQUIRE(integers[2+21]==1&&integers[2+25]==0);
                REQUIRE(integers[2+5]==NIMBY_SERVICE_STATION_STOP&&integers[2+8]==1);
                REQUIRE(integers[2+16]==1000&&integers[2+18]==-1&&integers[2+19]==0&&integers[2+20]==1);
                REQUIRE(integers[2+29]==2&&integers[2+30]==-1500000);
                REQUIRE(!std::strcmp(names.data()+2*wire::nameBytes,"Joined station"));
                REQUIRE(!std::strcmp(names.data()+4*wire::nameBytes,"Joined station"));
                REQUIRE(integers[2+32+5]==-1&&integers[2+32+16]==INT64_MIN&&std::isnan(numbers[5+1]));
                const auto before=integers;integers[0]=INT64_MAX;REQUIRE(read()==NIMBY_INVALID_ARGUMENT);REQUIRE(integers[2]==before[2]);
                integers[0]=0;integers[1]=33;REQUIRE(read()==NIMBY_INVALID_ARGUMENT);
                std::array<int64_t,5> header{0x5000000000001,0,0,0,0};std::array<char,257> name{};
                REQUIRE(wire::planHeader(*rich,header.data(),5,0,name.data(),257)==NIMBY_OK);
                REQUIRE(header[1]==1&&header[2]==0x4000000000001&&header[3]==1&&header[4]==1000);
                std::vector<int64_t> stops(3+wire::pageSize*5);std::vector<char> stations(wire::pageSize*wire::nameBytes);
                stops[0]=header[0];stops[2]=1;
                REQUIRE(wire::stops(*rich,stops.data(),int(stops.size()),0,stations.data(),int(stations.size()))==NIMBY_OK);
                REQUIRE(stops[4]==20&&stops[5]==30&&stops[6]==120&&stops[7]==150);
                REQUIRE(!std::strcmp(stations.data(),"Joined station"));
                header[0]=0x5000000000002;REQUIRE(wire::planHeader(*rich,header.data(),5,0,name.data(),257)==NIMBY_OK&&header[1]==-1);
                std::array<int64_t,2> counts{};REQUIRE(wire::catalogue(*rich,24,counts.data(),2,0,nullptr,0)==NIMBY_OK&&counts[0]==1&&counts[1]==1);
                std::vector<int64_t> lineValues(2+wire::pageSize*6);lineValues[1]=1;
                REQUIRE(wire::catalogue(*rich,25,lineValues.data(),int(lineValues.size()),0,stations.data(),int(stations.size()))==NIMBY_OK);
                REQUIRE(lineValues[2]==0x4000000000001&&lineValues[3]==0&&lineValues[4]==1&&lineValues[6]==0&&lineValues[7]==1);
                std::array<int64_t,5> links{2,0x5000000000001,0x4000000000001,0,0};
                REQUIRE(wire::catalogue(*rich,27,links.data(),5,0,nullptr,0)==NIMBY_OK&&links[3]==109&&links[4]==999);
                links[0]=33;REQUIRE(wire::catalogue(*rich,27,links.data(),5,0,nullptr,0)==NIMBY_INVALID_ARGUMENT);
                std::vector<int64_t> traitInts(2+wire::pageSize*6);std::vector<double> traitNumbers(wire::pageSize*12);traitInts[1]=2;
                REQUIRE(wire::characteristics(*rich,traitInts.data(),int(traitInts.size()),traitNumbers.data(),int(traitNumbers.size()),0)==NIMBY_OK);
                REQUIRE(traitInts[2]==8&&traitInts[3]==500&&traitInts[4]==1&&traitInts[5]==-1&&traitInts[6]==-1&&traitInts[7]==1);
                REQUIRE(traitNumbers[0]==40&&traitNumbers[1]==200&&traitNumbers[6]==35&&std::isnan(traitNumbers[7]));
                REQUIRE(std::isnan(traitNumbers[12]));
                REQUIRE(wire::compositions(*rich,29,counts.data(),2,0,nullptr,0)==NIMBY_OK&&counts[0]==2&&counts[1]==1);
                std::vector<int64_t> vehicles(2+wire::pageSize*4);vehicles[1]=2;
                REQUIRE(wire::compositions(*rich,30,vehicles.data(),int(vehicles.size()),0,nullptr,0)==NIMBY_OK);
                REQUIRE(vehicles[2]==0x5000000000001&&vehicles[3]==101&&vehicles[4]==0&&vehicles[5]==0&&vehicles[7]==999&&vehicles[8]==1);
                std::vector<int64_t> models(2+wire::pageSize);models[1]=1;std::vector<char> modelNames(wire::pageSize*3*wire::nameBytes);
                REQUIRE(wire::compositions(*rich,31,models.data(),int(models.size()),0,modelNames.data(),int(modelNames.size()))==NIMBY_OK);
                REQUIRE(models[2]==101&&!std::strcmp(modelNames.data(),"metro")&&!std::strcmp(modelNames.data()+257,"Metro vehicle"));
                models[0]=1;REQUIRE(wire::compositions(*rich,31,models.data(),int(models.size()),0,modelNames.data(),int(modelNames.size()))==NIMBY_INVALID_ARGUMENT);
                NimbyTrainService signedTimes{};signedTimes.flags=NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_ARRIVAL_VALID|NIMBY_SERVICE_DEPARTURE_VALID;
                signedTimes.game_epoch_seconds=1000;signedTimes.game_time_us=signedTimes.arrival_time_us=signedTimes.departure_time_us=-1;
                REQUIRE(nimby::TrainService(signedTimes).getGameCalendarSeconds()==999);
                REQUIRE(nimby::TrainService(signedTimes).getArrivalCalendarSeconds()==999);
                REQUIRE(nimby::TrainService(signedTimes).getDepartureCalendarSeconds()==999);
                signedTimes.game_epoch_seconds=INT64_MAX;signedTimes.game_time_us=1000000;
                REQUIRE(!nimby::TrainService(signedTimes).getGameCalendarSeconds());
                copiedTables.clear();client.captureTrainData(0);
                REQUIRE(copiedTables==std::set<std::string>{"Trains"});
                copiedTables.clear();client.captureTrainData(NIMBY_TRAIN_DATA_PASSENGERS);
                REQUIRE((copiedTables==std::set<std::string>{"Trains","TrainDetails"}));
                copiedTables.clear();client.captureTrainData(NIMBY_TRAIN_DATA_COMPOSITION);
                REQUIRE((copiedTables==std::set<std::string>{"Trains","TrainMetadata","TrainVehicles","VehicleModels"}));
                copiedTables.clear();client.captureTrainData(NIMBY_TRAIN_DATA_ALL);
                for(const auto* forbidden:{"Signals","SignalStates","SignalTextures","SignalExtensionsStates","SignalExtensionFields","TrackNodes","TrackJunctions","TrackMetrics","Platforms","TrainPathTracks","TrackOccupations","TrackReservations"})REQUIRE(!copiedTables.contains(forbidden));
            }
            const auto platforms=retained->getPlatformOccupationsForStation(30);
            REQUIRE(platforms&&platforms->size()==1&&platforms->front().platform.getName()=="A");
            REQUIRE(platforms->front().isOccupied()==false); // Reservation does not occupy a platform.
            REQUIRE(platforms->front().reserving_trains->size()==1);
            REQUIRE(!retained->getPlatformOccupationsForStation(999));
            REQUIRE(retained->getTrainDetailsById(0x10000000001ULL)->getPassengerCount()==123);
            REQUIRE(!retained->getTrainDetailsById(0x10000000001ULL)->getScheduleId());
            REQUIRE(retained->getLineStopsForTrain(0x10000000001ULL)->front().getPlannedDwellSeconds()==30);
            REQUIRE(!retained->getLineStopsForTrain(2));
            REQUIRE(!retained->getLineStopsForTrain(999));
            NimbyLineStop unknownStop{};unknownStop.arrival_offset_seconds=12;
            REQUIRE(!nimby::LineStop(unknownStop).getArrivalOffsetSeconds());
            REQUIRE(retained->getTrainById(trainId)->getName() == "Test train");
            REQUIRE(!retained->getTrainById(1)); // Preserve the full 64-bit ID.
            REQUIRE(!retained->getTrainById(2)->getSpeedMps());
            REQUIRE(!retained->getTrainById(2)->getPosition());
            REQUIRE(retained->getTrackForTrain(trainId)->getId() == 20);
            REQUIRE(!retained->getTrackForTrain(2));
            REQUIRE(retained->getStationForTrack(20)->getId() == 30);
            REQUIRE(!retained->getStationForTrack(21));
            REQUIRE(!retained->getStationById(30)->getName());
            REQUIRE(retained->getTracksForStation(30).size() == 1);
            REQUIRE(retained->getTrainsOnTrack(20).size() == 1);
            REQUIRE(retained->getSignalsForTrack(20).size() == 1);
            REQUIRE(retained->getSignalTopology().getSignalsForTrack(20)[0].getId() == 40);
            REQUIRE(retained->getPathTrackIdsForTrain(trainId)->size() == 2);
            REQUIRE(!retained->getPathTrackIdsForTrain(2));
            REQUIRE(!retained->getPathTrackIdsForTrain(999));
            const auto signalling=client.captureSignalling();
            REQUIRE(client.captureSignalling("test")->getAllTrains().size()==2);
            bool invalidScope=false;
            try{client.captureSignalling("");}catch(const std::invalid_argument&){invalidScope=true;}
            REQUIRE(invalidScope);
            REQUIRE(!signalling->getPathTrackIdsForTrain(trainId));
            REQUIRE(!signalling->getLineStopsForTrain(trainId));
            REQUIRE(signalling->getAllTrains().size()==retained->getAllTrains().size());
            REQUIRE(retained->getReservationsForTrain(trainId)->size() == 1);
            REQUIRE(retained->getReservationsForTrack(999)->empty());
            REQUIRE(retained->getAllOccupations()->empty());
            REQUIRE(!retained->getSignalStateById(40)->getAspect());
            REQUIRE(retained->getSignalStateById(40)->getTextureSelector() == 0);
            REQUIRE(!retained->getSignalStateById(40)->usesDefaultTextureSelector());
            NimbySignalState defaultSelector{};
            REQUIRE(!nimby::SignalState{defaultSelector}.getExceptionCount());
            defaultSelector.flags=NIMBY_SIGNAL_FILTER_VALID|NIMBY_SIGNAL_FILTER_DEFAULT_IGNORED;
            defaultSelector.exception_count=2;
            REQUIRE(nimby::SignalState{defaultSelector}.getExceptionCount()==2);
            REQUIRE(nimby::SignalState{defaultSelector}.isIgnoredByDefault()==true);
            defaultSelector.exception_count=0;
            REQUIRE(nimby::SignalState{defaultSelector}.getExceptionCount()==0);
            defaultSelector.flags=NIMBY_SIGNAL_TEXTURE_STATE_VALID|NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT;
            REQUIRE(nimby::SignalState{defaultSelector}.usesDefaultTextureSelector());
            REQUIRE(nimby::SignalState{defaultSelector}.getTextureSelector()==0);
            defaultSelector.flags=NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT;
            REQUIRE(!nimby::SignalState{defaultSelector}.usesDefaultTextureSelector());
            REQUIRE(!retained->getSignalStateById(40)->getSpecificState());
            REQUIRE(!retained->getSignalTextureById(40)->getReference());
            REQUIRE(!retained->getSignalTextureById(40)->getFilePath());
            REQUIRE(!retained->getTrackNodeById(20)->getLinkAId());
            REQUIRE(retained->getTrackNodeById(20)->getLinkBId() == 21);
            REQUIRE(retained->getAllTrackJunctions().size() == 1);
            REQUIRE(retained->getAllTrackJunctions()[0].getMainTrackId() == 20);
            REQUIRE(retained->getAllTrackJunctions()[0].getBranchDirection() == -1);
            const auto speed = retained->getTrainById(trainId)->getSpeedKmh();
            reservationsAvailable = false;
            auto unavailable = client.capture();
            REQUIRE(!unavailable->getAllReservations());
            REQUIRE(!unavailable->getReservationsForTrain(trainId));
            reservationsAvailable = true; emptyReservations = true;
            REQUIRE(client.capture()->getAllReservations()->empty());
            REQUIRE(retained->getTrainById(trainId)->getSpeedKmh() == speed);
            REQUIRE(retained->getAllReservations()->size() == 1);
            // A failed copy must release its native snapshot and keep the last good one.
            auto before = client.capture();
            trackStatus = NIMBY_IO_ERROR;
            bool failed = false;
            try { client.capture(); } catch (const nimby::Exception&) { failed = true; }
            REQUIRE(failed && !before->getAllTrains().empty() && captures == releases);
            trackStatus = NIMBY_OK;
            client.capture();

            occupiedPlatform=true;
            auto occupied=client.capture()->getPlatformOccupationsForStation(30);
            REQUIRE(occupied->front().isOccupied()==true&&occupied->front().occupying_trains->size()==2);
            occupationsAvailable=false;
            auto unknown=client.capture()->getPlatformOccupationsForStation(30);
            REQUIRE(!unknown->front().isOccupied()&&!unknown->front().occupying_trains);
            REQUIRE(unknown->front().reserving_trains.has_value());
            occupationsAvailable=true;occupiedPlatform=false;
            groupedPlatforms=true;
            auto groupedSnapshot=client.capture();
            auto grouped=groupedSnapshot->getPlatformOccupationsForStation(30);
            REQUIRE(grouped->size()==6); // A, B, two unknown names, two empty names.
            REQUIRE(grouped->front().platform.getTrackIds()==std::vector<nimby::Id>({20,21}));
            REQUIRE(grouped->front().platform.containsTrack(21));
            REQUIRE(!grouped->front().platform.containsTrack(29));
            REQUIRE(grouped->front().isOccupied()==true);
            REQUIRE(grouped->front().occupying_trains->size()==2);
            REQUIRE(grouped->front().reserving_trains->size()==2);
            REQUIRE((*grouped)[1].isOccupied()==false);
            const auto sections=groupedSnapshot->getPlatformSectionOccupationsForStation(30);
            REQUIRE(sections->size()==7&&sections->front().isOccupied()==false&&(*sections)[1].isOccupied()==true);
            REQUIRE(groupedSnapshot->getPlatformOccupationsForStation(31)->front().platform.getTrackIds()==std::vector<nimby::Id>{29});
            occupationsAvailable=false;
            REQUIRE(!client.capture()->getPlatformOccupationsForStation(30)->front().isOccupied());
            occupationsAvailable=true;reservationsAvailable=false;
            REQUIRE(!client.capture()->getPlatformOccupationsForStation(30)->front().reserving_trains);
            reservationsAvailable=true;groupedPlatforms=false;
            // Adapter and diagnostic calls on the same native session serialize.
            delayMs = 35;
            auto first = std::async(std::launch::async, [&] { return client.capture(); });
            auto second = std::async(std::launch::async, [&] { return client.capture(); });
            REQUIRE(first.get() && second.get());
            REQUIRE(peak == 1);
            REQUIRE(client.capture()->getAge()>=nimby::Milliseconds{35});
            delayMs = 0;
            // A tool heartbeat must not request any network tables. Deliberately
            // failing track copy proves this is independent of network capture.
            trackStatus=NIMBY_IO_ERROR;gameSessionAvailable=true;
            const auto metadata=client.capture(nimby::SnapshotScope::Session);
            REQUIRE(metadata->getGameSession()&&metadata->getSimulationClock());
            REQUIRE(metadata->getAllTracks().empty()&&metadata->getAllTrains().empty());
            trackStatus=NIMBY_OK;
        }
        REQUIRE(retained->getAllTrains().size() == 2); // Native session destruction is harmless.
        REQUIRE(closes == 1 && captures == releases);
        {
            auto session = nimby::detail::ObservationSession(42);
            for (const auto status : {NIMBY_DATA_UNAVAILABLE, NIMBY_PROCESS_EXITED}) {
                captureStatus = status;
                bool rejected = false;
                try { session.capture(); }
                catch (const nimby::Exception& e) { rejected = e.error().code == status; }
                REQUIRE(rejected); // Never substitute a cached capture after failure.
            }
            captureStatus = NIMBY_OK;
            REQUIRE(session.capture()->getAllTrains().size() == 2);
        }
        REQUIRE(closes == 2 && captures == releases);
        std::puts("Native adapter observations: ownership, validity, joins, errors and serialization passed.");
    } catch (const std::exception& e) { std::fprintf(stderr, "%s\n", e.what()); return 1; }
}
