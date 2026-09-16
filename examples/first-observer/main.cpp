#include <nimby/client.hpp>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {
void check(uint32_t status, const char* operation) {
    if (status != NIMBY_OK) {
        throw std::runtime_error(std::string(operation) + ": " +
                                 NimbySdk_StatusString(status));
    }
}

// RAII: leaving the scope releases the handle, including on exceptions.
// A handle has one owner; copying an owner would cause a double release.
struct Session {
    NimbySession value{};
    Session() = default;
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    ~Session() { if (value) NimbySdk_CloseSession(value); }
};

struct Snapshot {
    NimbySnapshot value{};
    Snapshot() = default;
    Snapshot(const Snapshot&) = delete;
    Snapshot& operator=(const Snapshot&) = delete;
    ~Snapshot() { if (value) NimbySdk_ReleaseSnapshot(value); }
};

template<class T, class Copy>
std::vector<T> readRecords(NimbySnapshot snapshot, Copy copy, const char* operation) {
    uint32_t count{};
    check(copy(snapshot, nullptr, 0, &count), operation); // Count records, not bytes.
    std::vector<T> result(count);
    if (count != 0) {
        check(copy(snapshot, result.data(), count, &count), operation);
    }
    return result; // The SDK snapshot is immutable between the two calls.
}

void checkVersion() {
    NimbySdkVersion version{};
    version.struct_size = sizeof version;
    check(NimbySdk_GetVersion(&version), "GetVersion");
    std::printf("SDK %u.%u.%u | ABI %u\n",
                version.major, version.minor, version.patch, version.abi_version);
    if (version.abi_version != NIMBY_OBSERVATION_ABI_VERSION ||
        version.major != 0 || version.minor != 6) {
        throw std::runtime_error("This example requires SDK 0.6.x / ABI 1");
    }
}

void printSnapshot(NimbySnapshot snapshot, int number) {
    NimbySnapshotInfo info{};
    info.struct_size = sizeof info;
    check(NimbySdk_GetSnapshotInfo(snapshot, &info), "GetSnapshotInfo");
    const auto trains = readRecords<NimbyTrain>(snapshot, NimbySdk_CopyTrains, "CopyTrains");
    const auto tracks = readRecords<NimbyTrack>(snapshot, NimbySdk_CopyTracks, "CopyTracks");
    const auto stations = readRecords<NimbyStation>(snapshot, NimbySdk_CopyStations, "CopyStations");
    const auto details = readRecords<NimbyTrainDetails>(snapshot, NimbySdk_CopyTrainDetails, "CopyTrainDetails");
    const auto services = readRecords<NimbyTrainService>(snapshot, NimbySdk_CopyTrainServices, "CopyTrainServices");

    std::printf("\nSnapshot %d | pid=%u | captured_unix_ms=%llu | %zu trains\n",
                number, info.process_id,
                static_cast<unsigned long long>(info.captured_unix_ms), trains.size());
    auto stationName=[&](uint64_t id){
        if(!id)return "(outside station)";
        const auto it=std::find_if(stations.begin(),stations.end(),[&](const auto& v){return v.id==id;});
        return it!=stations.end()&&it->name_utf8[0]?it->name_utf8:"(unavailable)";
    };
    for (const auto& train : trains) {
        std::printf("\nTrain %s | id=%llu\n",train.name_utf8,static_cast<unsigned long long>(train.id));
        const auto detail=std::find_if(details.begin(),details.end(),[&](const auto& d){return d.train_id==train.id;});
        if(detail!=details.end()){
            if(detail->flags&NIMBY_TRAIN_PASSENGERS_VALID)std::printf("Passengers: %d | ",detail->passenger_count);
            else std::printf("Passengers: unavailable | ");
            if(detail->flags&NIMBY_TRAIN_ASSIGNMENT_VALID)std::printf("schedule=%llu | shift=%llu | order index=%d | ",
                static_cast<unsigned long long>(detail->schedule_id),static_cast<unsigned long long>(detail->shift_id),detail->order_index);
            std::printf("order mode=%d\n",detail->order_mode);
        }
        const auto found=std::find_if(services.begin(),services.end(),[&](const auto& s){return s.train_id==train.id;});
        if(found!=services.end()){
            nimby::TrainService service(*found);
            if(found->flags&NIMBY_SERVICE_LINE_VALID)std::printf("Line: %s | ",found->line_name_utf8);
            if(found->flags&NIMBY_SERVICE_STOP_VALID)std::printf("active stop: %s (#%d) | ",stationName(found->stop_station_id),found->stop_index+1);
            std::printf("Service: %s | ",service.getStatusName());
            if(auto seconds=service.getDepartureRemainingSeconds())std::printf("departs in %.1f game s | ",*seconds);
            if(auto time=service.getDepartureCalendarSeconds())std::printf("departure=%02lld:%02lld:%02lld (game) | ",*time/3600%24,*time/60%60,*time%60);
            if(auto seconds=service.getDispatchRemainingSeconds())std::printf("dispatch retry in %.1f game s | ",*seconds);
        }
        std::puts("");
        uint32_t stopCount{};
        const auto stopStatus=NimbySdk_CopyTrainLineStops(snapshot,train.id,nullptr,0,&stopCount);
        if(stopStatus==NIMBY_OK){
            std::vector<NimbyLineStop> stops(stopCount);
            check(NimbySdk_CopyTrainLineStops(snapshot,train.id,stops.data(),stopCount,&stopCount),"CopyTrainLineStops");
            const auto first=found!=services.end()&&found->stop_index>=0?size_t(found->stop_index):0;
            std::printf("Line plan (%u stops; showing up to 3 from active index):\n",stopCount);
            for(size_t i=first;i<stops.size()&&i<first+3;++i){
                const auto& stop=stops[i];
                std::printf("  #%u %s | track=%llu",stop.index+1,stationName(stop.station_id),static_cast<unsigned long long>(stop.track_id));
                if(stop.flags&NIMBY_LINE_STOP_TIMES_VALID)std::printf(" | relative arrival=+%ds departure=+%ds | planned dwell=%ds",
                    stop.arrival_offset_seconds,stop.departure_offset_seconds,stop.departure_offset_seconds-stop.arrival_offset_seconds);
                std::puts("");
            }
        }else if(stopStatus==NIMBY_DATA_UNAVAILABLE)std::puts("Line plan: unavailable");
        else check(stopStatus,"CopyTrainLineStops");
        if (train.flags & (NIMBY_TRAIN_SPEED_VALID | NIMBY_TRAIN_PRESENT)) {
            std::printf("speed=%.1f km/h%s | ", train.speed_mps * 3.6,
                        (train.flags & NIMBY_TRAIN_SPEED_DEFAULTED) ? " (game display default; speed not measured)" : "");
        } else {
            std::printf("speed=unavailable | ");
        }
        if (!(train.flags & NIMBY_TRAIN_POSITION_VALID)) {
            std::puts("position=unavailable");
            continue;
        }
        std::printf("track=%llx | position=%.1f%% | direction=%d | ",
                    static_cast<unsigned long long>(train.track_id),
                    train.track_fraction * 100.0, train.direction);

        // Join full IDs from this snapshot only. A track's station is not
        // necessarily the train's destination, next stop, or current stop.
        const auto track = std::find_if(tracks.begin(), tracks.end(),
            [&](const auto& item) { return item.id == train.track_id; });
        if (track == tracks.end()) {
            std::puts("track station=unavailable");
        } else if (track->station_id == 0) {
            std::puts("track station=none");
        } else {
            const auto station = std::find_if(stations.begin(), stations.end(),
                [&](const auto& item) { return item.id == track->station_id; });
            if (station == stations.end()) {
                std::puts("track station=unavailable");
            } else {
                std::printf("track station=%s\n",
                    station->name_utf8[0] ? station->name_utf8 : "(automatic name unresolved)");
            }
        }
    }
}
} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::puts("Usage: MyFirstNimbyTool <game-pid> | --check-sdk");
        return 2;
    }
    const bool versionOnly = std::strcmp(argv[1], "--check-sdk") == 0;
    uint32_t gamePid{};
    if (!versionOnly) {
        const auto end = argv[1] + std::strlen(argv[1]);
        const auto parsed = std::from_chars(argv[1], end, gamePid);
        if (parsed.ec != std::errc{} || parsed.ptr != end || gamePid == 0) {
            std::fputs("Expected a non-zero decimal game PID, or --check-sdk.\n", stderr);
            return 2;
        }
    }
    try {
        checkVersion();
        if (versionOnly) return 0; // No running game is needed for this check.

        Session session;
        check(NimbySdk_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION, gamePid,
                                  &session.value), "OpenProcess");
        int captured = 0;
        for (int attempt = 1; attempt <= 5; ++attempt) {
            {
                Snapshot snapshot;
                const auto status = NimbySdk_CaptureSnapshot(session.value, &snapshot.value);
                if (status == NIMBY_DATA_UNAVAILABLE) {
                    std::printf("Attempt %d: data unavailable; load a game and retry.\n", attempt);
                } else {
                    check(status, "CaptureSnapshot");
                    printSnapshot(snapshot.value, attempt);
                    ++captured;
                }
            } // Release each snapshot before waiting for the next one.
            if (attempt < 5) std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        if (captured == 0) {
            std::fputs("No snapshot captured. See docs/troubleshooting.md.\n", stderr);
            return 3;
        }
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "SDK: %s\n", error.what());
        return 1;
    }
}
