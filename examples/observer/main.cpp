#include <nimby/client.hpp>
#include <charconv>
#include <cstring>
#include <iostream>
using namespace std::chrono_literals;

void printSnapshot(const nimby::Snapshot& snapshot) {
    std::cout << snapshot.getAllTrains().size() << " trains | " << snapshot.getAllTracks().size()
              << " tracks | " << snapshot.getAllStations().size() << " stations | " << snapshot.getAllSignals().size() << " signals\n";
    for (const auto& service : snapshot.getAllTrainServices()) {
        std::cout << "service train=" << service.getTrainId() << " state=" << service.getStatusName();
        if (auto seconds = service.getDepartureRemainingSeconds()) std::cout << " departs_in=" << *seconds << " game s";
        if (auto station = service.getLocationStationId()) std::cout << " station=" << *station;
        std::cout << '\n';
    }
    for (const auto& signal : snapshot.getAllSignals()) {
        std::cout << "signal=" << signal.getId() << " track=" << signal.getTrackId();
        if (auto state = snapshot.getSignalStateById(signal.getId())) {
            if (auto aspect = state->getAspect()) std::cout << " aspect=" << *aspect;
            else std::cout << " aspect=unknown";
            if (auto specific = state->getSpecificState()) std::cout << " specific=" << specific->system << ':' << specific->state;
            if (auto texture = state->getTextureSelector()) std::cout << " texture=" << *texture;
        }
        if (auto texture = snapshot.getSignalTextureById(signal.getId()))
            if (auto path = texture->getFilePath()) std::cout << " file=" << *path;
        std::cout << '\n';
    }
    auto printUsage = [](const char* label, const auto& usage) {
        std::cout << label << ": ";
        if (usage) std::cout << usage->size() << " native intervals\n";
        else std::cout << "unavailable\n";
    };
    printUsage("Reservations", snapshot.getAllReservations());
    printUsage("Occupations", snapshot.getAllOccupations());
    for (const auto& train : snapshot.getAllTrains()) {
        std::cout << train.getName() << " id=" << train.getId();
        if (train.isSpeedDefaulted()) std::cout << " speed not measured";
        else if (auto speed = train.getSpeedKmh()) std::cout << ' ' << *speed << " km/h";
        else std::cout << " speed unavailable";
        if (auto position = train.getPosition()) std::cout << " track=" << position->getTrackId() << " fraction=" << position->getFraction();
        if (auto path = snapshot.getPathTrackIdsForTrain(train.getId())) std::cout << " path tracks=" << path->size();
        std::cout << '\n';
    }
}

int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "Usage: MyNimbyObserver <game-pid> | --check-sdk\n"; return 2; }
    try {
        const auto version = nimby::getVersion();
        std::cout << "NimbyRailsFranceSDK " << version.major << '.' << version.minor << '.' << version.patch << '\n';
        if (std::strcmp(argv[1], "--check-sdk") == 0) return 0;
        std::uint32_t pid{};
        const auto end = argv[1] + std::strlen(argv[1]);
        const auto parsed = std::from_chars(argv[1], end, pid);
        if (parsed.ec != std::errc{} || parsed.ptr != end || !pid) return 2;
        auto client = nimby::Client::connect(pid);
        int captured = 0;
        for (int tick = 0; tick < 5; ++tick) {
            try { printSnapshot(*client.capture()); ++captured; }
            catch (const nimby::Exception& e) {
                if (e.code() != nimby::ErrorCode::DataUnavailable) throw;
                std::cerr << "Snapshot unavailable; load a game and retry.\n";
            }
            if (tick < 4) std::this_thread::sleep_for(250ms);
        }
        return captured ? 0 : 3;
    } catch (const std::exception& e) { std::cerr << "SDK: " << e.what() << '\n'; return 1; }
}
