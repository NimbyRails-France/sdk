#include <nimby/client.hpp>
#include <charconv>
#include <cstring>
#include <iostream>

using namespace std::chrono_literals;

int observe(nimby::Client& client) {
    client.startAutoRefresh(); // First capture immediately, then target 4 Hz.
    if (!client.waitForSnapshot(2s)) {
        if (auto error = client.getLastError())
            std::cerr << error->operation << ": " << error->message << '\n';
        std::cerr << "No snapshot yet. Load a game and retry.\n";
        return 3;
    }
    nimby::Snapshot::Ptr previous;
    for (int frame = 0; frame < 5; ++frame) {
        auto snapshot = client.latest(); // Keep ownership while using collection views.
        const bool fresh = snapshot != previous;
        std::cout << "\nPID " << snapshot->getProcessId()
                  << " | age=" << snapshot->getAge().count() << " ms"
                  << " | " << (fresh ? "new snapshot" : "same snapshot") << '\n';
        if (snapshot->isOlderThan(1s)) std::cout << "Stale data\n";

        for (const auto& train : snapshot->getAllTrains()) {
            std::cout << train.getName() << " | id=" << train.getId();
            if (auto speed = train.getSpeedKmh()) std::cout << " | " << *speed << " km/h";
            else std::cout << " | speed unavailable";

            // Constant-average-time lookup by the full 64-bit ID.
            auto found = snapshot->getTrainById(train.getId());
            if (!found) throw std::runtime_error("Snapshot train index mismatch");
            if (auto track = snapshot->getTrackForTrain(train.getId())) {
                std::cout << " | track=" << track->getId();
                if (auto station = snapshot->getStationForTrack(track->getId()))
                    std::cout << " | track station=" << station->getName().value_or("(unknown name)");
            }
            if (auto portions = snapshot->getReservationsForTrain(train.getId()))
                std::cout << " | reservations=" << portions->size();
            else std::cout << " | reservations unknown";
            std::cout << '\n';
        }
        if (auto error = client.getLastError())
            std::cerr << "Latest refresh failed: " << error->message << '\n';
        previous = std::move(snapshot);
        std::this_thread::sleep_for(250ms); // UI pacing is independent of SDK capture.
    }
    client.stopAutoRefresh();
    // Copied snapshots remain valid even when refresh stops or the client is destroyed.
    std::cout << "Stopped; retained trains=" << previous->getAllTrains().size() << '\n';
    return 0;
}
int main(int argc, char** argv) {
    try {
        if (argc == 2 && std::strcmp(argv[1], "--check-sdk") == 0) {
            NimbySdkVersion version{};
            version.struct_size = sizeof version;
            const auto status = NimbySdk_GetVersion(&version);
            if (status != NIMBY_OK || version.major != 0 || version.minor != 6 || version.abi_version != 1)
                throw std::runtime_error("SDK 0.6.x / ABI 1 required");
            std::cout << "C++ helpers ready | SDK " << version.major << '.' << version.minor
                      << '.' << version.patch << " | ABI " << version.abi_version << '\n';
            return 0; // This check does not need a running game.
        }
        if (argc == 1) {
            auto client = nimby::Client::connect();
            return observe(client);
        }
        if (argc == 2) {
            std::uint32_t gamePid{};
            const auto end = argv[1] + std::strlen(argv[1]);
            const auto parsed = std::from_chars(argv[1], end, gamePid);
            if (parsed.ec == std::errc{} && parsed.ptr == end && gamePid != 0) {
                auto client = nimby::Client::connect(gamePid);
                return observe(client);
            }
        }
        std::cerr << "Usage: MyNimbyClient [game-pid | --check-sdk]\n";
        return 2;
    } catch (const nimby::Exception& error) {
        std::cerr << "SDK error " << error.error().code << ": " << error.what() << '\n';
        return 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
