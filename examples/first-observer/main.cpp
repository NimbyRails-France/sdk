#include <nimby/observation.h>

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

    std::printf("\nSnapshot %d | pid=%u | captured_unix_ms=%llu | %zu trains\n",
                number, info.process_id,
                static_cast<unsigned long long>(info.captured_unix_ms), trains.size());
    for (const auto& train : trains) {
        std::printf("Train %s | id=%llx | ", train.name_utf8,
                    static_cast<unsigned long long>(train.id));
        if (train.flags & NIMBY_TRAIN_PRESENT) {
            std::printf("speed=%.1f km/h | ", train.speed_mps * 3.6);
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
