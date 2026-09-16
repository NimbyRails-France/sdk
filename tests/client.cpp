#include <nimby/client.hpp>
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
std::set<NimbySnapshot> handles;
std::mutex handlesMutex;
template<class T>
uint32_t copy(NimbySnapshot snapshot, T* out, uint32_t capacity, uint32_t* count,
              const std::vector<T>& rows) {
    std::lock_guard lock(handlesMutex);
    if (!handles.contains(snapshot)) return NIMBY_INVALID_HANDLE;
    *count = static_cast<uint32_t>(rows.size());
    if (!out && !capacity) return NIMBY_OK;
    if (capacity < rows.size()) return NIMBY_BUFFER_TOO_SMALL;
    std::copy(rows.begin(), rows.end(), out);
    return NIMBY_OK;
}
}
uint32_t __cdecl NimbySdk_GetVersion(NimbySdkVersion* out) noexcept {
    *out = {sizeof *out, 1, 0, 6, 0}; return NIMBY_OK;
}
const char* __cdecl NimbySdk_StatusString(uint32_t code) noexcept {
    return code == NIMBY_PROCESS_EXITED ? "Process exited" : "Test status";
}
uint32_t __cdecl NimbySdk_OpenProcess(uint32_t, uint32_t pid, NimbySession* out) noexcept {
    *out = pid; return NIMBY_OK;
}
uint32_t __cdecl NimbySdk_CloseSession(NimbySession) noexcept { ++closes; return NIMBY_OK; }
uint32_t __cdecl NimbySdk_CaptureSnapshot(NimbySession, NimbySnapshot* out) noexcept {
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
uint32_t __cdecl NimbySdk_ReleaseSnapshot(NimbySnapshot s) noexcept {
    std::lock_guard lock(handlesMutex);
    if (handles.erase(s) != 1) return NIMBY_INVALID_HANDLE;
    ++releases; return NIMBY_OK;
}
uint32_t __cdecl NimbySdk_GetSnapshotInfo(NimbySnapshot, NimbySnapshotInfo* out) noexcept {
    *out = {}; out->struct_size = sizeof *out; out->process_id = 42;
    out->captured_unix_ms = 1000;
    std::strcpy(out->game_sha256, "fixture");
    return NIMBY_OK;
}
uint32_t __cdecl NimbySdk_CopyTrains(NimbySnapshot s, NimbyTrain* out, uint32_t cap, uint32_t* n) noexcept {
    NimbyTrain train{}; train.id = 0x10000000001ULL; train.track_id = 20;
    train.flags = NIMBY_TRAIN_PRESENT | NIMBY_TRAIN_POSITION_VALID;
    train.speed_mps = double(s); train.track_fraction = .25; train.direction = -1;
    std::strcpy(train.name_utf8, "Test train");
    NimbyTrain unknown{}; unknown.id = 2;
    // Nonzero raw data with no flags must never be exposed as valid.
    unknown.speed_mps = 99; unknown.track_id = 20;
    return copy(s, out, cap, n, std::vector{train, unknown});
}
uint32_t __cdecl NimbySdk_CopyTrainServices(NimbySnapshot s,NimbyTrainService* out,uint32_t cap,uint32_t* n) noexcept {
    NimbyTrainService service{};service.train_id=0x5000000000001;
    service.flags=NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_DEPARTURE_VALID;
    service.status=NIMBY_SERVICE_STATION_STOP;service.departure_remaining_seconds=42;
    NimbyTrainService unknown{};unknown.train_id=2;
    return copy(s,out,cap,n,std::vector{service,unknown});
}
uint32_t __cdecl NimbySdk_CopyTrainDetails(NimbySnapshot s,NimbyTrainDetails* out,uint32_t cap,uint32_t* n) noexcept {
    NimbyTrainDetails row{};row.train_id=0x10000000001ULL;row.flags=NIMBY_TRAIN_PASSENGERS_VALID;row.passenger_count=123;
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbySdk_CopyTrainLineStops(NimbySnapshot s,uint64_t id,NimbyLineStop* out,uint32_t cap,uint32_t* n) noexcept {
    if(id==2){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    NimbyLineStop row{};row.line_id=4;row.track_id=20;row.station_id=30;row.flags=NIMBY_LINE_STOP_TIMES_VALID;
    row.arrival_offset_seconds=120;row.departure_offset_seconds=150;
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbySdk_CopyTracks(NimbySnapshot s, NimbyTrack* out, uint32_t cap, uint32_t* n) noexcept {
    if (trackStatus != NIMBY_OK) return trackStatus;
    return copy(s, out, cap, n, std::vector<NimbyTrack>{{20, 30, 40}, {21, 0, 20}});
}
uint32_t __cdecl NimbySdk_CopyPlatforms(NimbySnapshot s,NimbyPlatform* out,uint32_t cap,uint32_t* n) noexcept {
    NimbyPlatform row{};row.track_id=20;row.station_id=30;row.flags=NIMBY_PLATFORM_NAME_VALID;std::strcpy(row.name_utf8,"A");
    return copy(s,out,cap,n,std::vector{row});
}
uint32_t __cdecl NimbySdk_CopyStations(NimbySnapshot s, NimbyStation* out, uint32_t cap, uint32_t* n) noexcept {
    NimbyStation station{}; station.id = 30; // Unresolved automatic name.
    return copy(s, out, cap, n, std::vector{station});
}
uint32_t __cdecl NimbySdk_CopySignals(NimbySnapshot s, NimbySignal* out, uint32_t cap, uint32_t* n) noexcept {
    return copy(s, out, cap, n, std::vector<NimbySignal>{{40, 20, .5, 1, NIMBY_SIGNAL_PATH}});
}
uint32_t __cdecl NimbySdk_CopySignalStates(NimbySnapshot s, NimbySignalState* out, uint32_t cap, uint32_t* n) noexcept {
    NimbySignalState state{}; state.signal_id = 40; state.flags = NIMBY_SIGNAL_TEXTURE_STATE_VALID;
    return copy(s, out, cap, n, std::vector{state});
}
uint32_t __cdecl NimbySdk_CopySignalTextures(NimbySnapshot s, NimbySignalTexture* out, uint32_t cap, uint32_t* n) noexcept {
    NimbySignalTexture texture{}; texture.signal_id = 40;
    return copy(s, out, cap, n, std::vector{texture});
}
uint32_t __cdecl NimbySdk_CopyTrackNodes(NimbySnapshot s, NimbyTrackNode* out, uint32_t cap, uint32_t* n) noexcept {
    return copy(s, out, cap, n, std::vector<NimbyTrackNode>{{20, 0, 21, 10, 15}});
}
uint32_t __cdecl NimbySdk_CopyTrackReservations(NimbySnapshot s, NimbyTrackUsage* out, uint32_t cap, uint32_t* n) noexcept {
    if (!reservationsAvailable) { *n = 0; return NIMBY_DATA_UNAVAILABLE; }
    const auto rows = emptyReservations ? std::vector<NimbyTrackUsage>{}
        : std::vector<NimbyTrackUsage>{{0x10000000001ULL, 20, .1, .6}};
    return copy(s, out, cap, n, rows);
}
uint32_t __cdecl NimbySdk_CopyTrackOccupations(NimbySnapshot s, NimbyTrackUsage* out, uint32_t cap, uint32_t* n) noexcept {
    if(!occupationsAvailable){*n=0;return NIMBY_DATA_UNAVAILABLE;}
    const auto rows=occupiedPlatform?std::vector<NimbyTrackUsage>{{0x10000000001ULL,20,0,.5},{0x10000000001ULL,20,.5,1},{2,20,.1,.2}}:std::vector<NimbyTrackUsage>{};
    return copy(s, out, cap, n, rows);
}
uint32_t __cdecl NimbySdk_CopyTrainPathTracks(NimbySnapshot s, uint64_t id, uint64_t* out, uint32_t cap, uint32_t* n) noexcept {
    if (id == 2) { *n = 0; return NIMBY_DATA_UNAVAILABLE; }
    return copy(s, out, cap, n, std::vector<uint64_t>{20, 21});
}
int main() {
    using namespace std::chrono_literals;
    constexpr nimby::Id trainId = 0x10000000001ULL;
    try {
        // Both legacy 0.6.0 Drive values and 0.6.1 explicit validity are supported.
        NimbyTrain speedRecord{};
        speedRecord.speed_mps=12.5;
        speedRecord.flags=NIMBY_TRAIN_PRESENT;
        REQUIRE(nimby::Train{speedRecord}.getSpeedKmh()==45.0);
        REQUIRE(!nimby::Train{speedRecord}.isSpeedDefaulted());
        NimbyTrainService unknownService{};unknownService.departure_remaining_seconds=99;
        REQUIRE(!nimby::TrainService{unknownService}.getStatus());
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
        static_assert(!std::is_copy_constructible_v<nimby::Client>);
        nimby::Snapshot::Ptr retained;
        {
            auto client = nimby::Client::connect(42);
            REQUIRE(client.getRefreshInterval() == 250ms);
            REQUIRE(!client.latest());
            REQUIRE(!client.waitForSnapshot(1ms));
            for (auto interval : {0ms, -1ms, std::chrono::duration_cast<nimby::Milliseconds>(25h)}) {
                bool rejected = false;
                try { client.setRefreshInterval(interval); }
                catch (const nimby::Exception& e) { rejected = e.error().code == NIMBY_INVALID_ARGUMENT; }
                REQUIRE(rejected);
            }
            retained = client.capture();
            REQUIRE(captures == releases);
            REQUIRE(retained->getAllTrains().size() == 2);
            REQUIRE(retained->getAllTrainServices().size()==2);
            REQUIRE(retained->getTrainServiceById(0x5000000000001)->getDepartureRemainingSeconds()==42);
            REQUIRE(!retained->getTrainServiceById(2)->getStatus());
            REQUIRE(!retained->getTrainServiceById(999));
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
            REQUIRE(retained->getPathTrackIdsForTrain(trainId)->size() == 2);
            REQUIRE(!retained->getPathTrackIdsForTrain(2));
            REQUIRE(!retained->getPathTrackIdsForTrain(999));
            REQUIRE(retained->getReservationsForTrain(trainId)->size() == 1);
            REQUIRE(retained->getReservationsForTrack(999)->empty());
            REQUIRE(retained->getAllOccupations()->empty());
            REQUIRE(!retained->getSignalStateById(40)->getAspect());
            REQUIRE(retained->getSignalStateById(40)->getTextureSelector() == 0);
            REQUIRE(!retained->getSignalStateById(40)->getSpecificState());
            REQUIRE(!retained->getSignalTextureById(40)->getReference());
            REQUIRE(!retained->getSignalTextureById(40)->getFilePath());
            REQUIRE(!retained->getTrackNodeById(20)->getLinkAId());
            REQUIRE(retained->getTrackNodeById(20)->getLinkBId() == 21);
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
            auto before = client.latest();
            trackStatus = NIMBY_IO_ERROR;
            bool failed = false;
            try { client.capture(); } catch (const nimby::Exception&) { failed = true; }
            REQUIRE(failed && client.latest() == before && captures == releases);
            REQUIRE(client.getLastError()->code == NIMBY_IO_ERROR);
            trackStatus = NIMBY_OK;
            client.capture();
            REQUIRE(!client.getLastError());

            occupiedPlatform=true;
            auto occupied=client.capture()->getPlatformOccupationsForStation(30);
            REQUIRE(occupied->front().isOccupied()==true&&occupied->front().occupying_trains->size()==2);
            occupationsAvailable=false;
            auto unknown=client.capture()->getPlatformOccupationsForStation(30);
            REQUIRE(!unknown->front().isOccupied()&&!unknown->front().occupying_trains);
            REQUIRE(unknown->front().reserving_trains.has_value());
            occupationsAvailable=true;occupiedPlatform=false;
            // Auto/manual captures share a gate even when a capture overruns its period.
            delayMs = 35;
            client.startAutoRefresh(10ms);
            auto manual = std::async(std::launch::async, [&] { return client.capture(); });
            REQUIRE(manual.get());
            std::this_thread::sleep_for(130ms);
            client.stopAutoRefresh();
            REQUIRE(peak == 1);
            REQUIRE(!client.isAutoRefreshRunning());
            const auto stopped = captures.load();
            std::this_thread::sleep_for(40ms);
            REQUIRE(captures == stopped);
            delayMs = 0;
            client.setRefreshInterval(24h);
            const auto began = std::chrono::steady_clock::now();
            client.startAutoRefresh();
            // Interval changes must wake a worker sleeping on a long deadline.
            client.setRefreshInterval(10ms);
            const auto beforeChange = captures.load();
            const auto deadline = std::chrono::steady_clock::now() + 1s;
            while (captures == beforeChange && std::chrono::steady_clock::now() < deadline)
                std::this_thread::sleep_for(1ms);
            REQUIRE(captures > beforeChange);
            client.stopAutoRefresh();
            REQUIRE(std::chrono::steady_clock::now() - began < 1s);
        }
        REQUIRE(retained->getAllTrains().size() == 2); // Client destruction is harmless.
        REQUIRE(closes == 1 && captures == releases);
        {
            auto client = nimby::Client::connect(42);
            captureStatus = NIMBY_DATA_UNAVAILABLE;
            client.startAutoRefresh(10ms);
            REQUIRE(!client.waitForSnapshot(40ms));
            REQUIRE(client.getLastError()->code == NIMBY_DATA_UNAVAILABLE);
            captureStatus = NIMBY_OK;
            REQUIRE(client.waitForSnapshot(1s));
            client.stopAutoRefresh();
            REQUIRE(!client.getLastError());
            auto before = client.latest();
            captureStatus = NIMBY_PROCESS_EXITED;
            try { client.capture(); } catch (const nimby::Exception&) {}
            REQUIRE(client.getConnectionState() == nimby::ConnectionState::ProcessExited);
            REQUIRE(client.latest() == before);
        }
        REQUIRE(closes == 2 && captures == releases);
        std::puts("C++ helpers: ownership, validity, joins, errors, auto refresh, stop/restart and serialization passed.");
    } catch (const std::exception& e) { std::fprintf(stderr, "%s\n", e.what()); return 1; }
}
