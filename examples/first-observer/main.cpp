#include <nimby/client.hpp>
#include <charconv>
#include <cstring>
#include <iostream>
using namespace std::chrono_literals;

void printSnapshot(const nimby::Snapshot& snapshot) {
    std::cout << snapshot.getAllTrains().size() << " trains | " << snapshot.getAllStations().size() << " stations\n";
    for (const auto& train : snapshot.getAllTrains()) {
        std::cout << train.getName() << " | id=" << train.getId();
        if (train.isSpeedDefaulted()) std::cout << " | speed not measured";
        else if (auto speed = train.getSpeedKmh()) std::cout << " | " << *speed << " km/h";
        else std::cout << " | speed unavailable";
        auto service = snapshot.getTrainServiceById(train.getId());
        auto track = service ? service->getLocationTrackId() : std::nullopt;
        if (!track) if (auto position = train.getPosition()) track = position->getTrackId();
        if (track) {
            std::cout << " | track=" << *track;
            if (auto station = snapshot.getStationForTrack(*track)) {
                std::cout << " | station=" << station->getName().value_or("unknown name");
                if (auto platforms = snapshot.getPlatformOccupationsForStation(station->getId()))
                    for (const auto& row : *platforms) if (row.platform.containsTrack(*track))
                        std::cout << " | platform=" << row.platform.getName().value_or("unknown");
            } else std::cout << " | outside station";
        } else if (service && service->isOnNetwork() == false) std::cout << " | not placed on network";
        else std::cout << " | location unavailable";
        if (service) {
            std::cout << " | state=" << service->getStatusName();
            if (auto line = service->getLineName()) std::cout << " | line=" << *line;
            if (auto stop = service->getStopStationId()) if (auto station = snapshot.getStationById(*stop))
                std::cout << " | service stop=" << station->getName().value_or("unknown");
            if (auto seconds = service->getDepartureRemainingSeconds()) std::cout << " | departure in=" << *seconds << " game s";
            if (auto time = service->getDepartureCalendarSeconds()) std::cout << " | departure calendar seconds=" << *time;
            if (auto seconds = service->getArrivalRemainingSeconds()) std::cout << " | arrival delta=" << *seconds << " game s";
            if (auto seconds = service->getDispatchRemainingSeconds()) std::cout << " | dispatch retry in=" << *seconds << " game s";
            if (auto alert = service->getAlert(); alert && *alert) std::cout << " | alert=" << *alert;
        }
        if (auto details = snapshot.getTrainDetailsById(train.getId())) {
            if (auto count = details->getPassengerCount()) std::cout << " | passengers=" << *count;
            if (auto schedule = details->getScheduleId()) std::cout << " | schedule=" << *schedule;
            if (auto shift = details->getShiftId()) std::cout << " | shift=" << *shift;
        }
        std::cout << '\n';
        if (auto stops = snapshot.getLineStopsForTrain(train.getId())) {
            // Complete line plan; partial runs may serve only a subset.
            const auto active = service ? service->getStopIndex().value_or(0) : 0;
            int shown = 0;
            for (const auto& stop : *stops) {
                if (static_cast<int>(stop.getIndex()) < active || shown++ >= 3) continue;
                auto station = snapshot.getStationById(stop.getStationId());
                std::cout << "  plan[" << stop.getIndex() << "] " << (station ? station->getName().value_or("unknown") : "outside station");
                if (auto arrival = stop.getArrivalOffsetSeconds()) std::cout << " | arrival offset=" << *arrival;
                if (auto departure = stop.getDepartureOffsetSeconds()) std::cout << " | departure offset=" << *departure;
                if (auto dwell = stop.getPlannedDwellSeconds()) std::cout << " | planned dwell=" << *dwell << " s";
                std::cout << '\n';
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) { std::cerr << "Usage: MyFirstNimbyTool <game-pid> | --check-sdk\n"; return 2; }
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
