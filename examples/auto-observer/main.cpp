#include <nimby/client.hpp>
#include <charconv>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std::chrono_literals;

// Location comes from native state, never from a future stop or a reservation.
void printLocation(const nimby::Snapshot& snapshot, const nimby::Train& train,
                   const std::optional<nimby::TrainService>& service) {
    auto trackId=service?service->getLocationTrackId():std::nullopt;
    if(!trackId)if(auto position=train.getPosition())trackId=position->getTrackId();
    const auto flags=service?service->getMotionFlags():std::nullopt;
    if(trackId){
        std::cout << " | lieu=";
        if(auto station=snapshot.getStationForTrack(*trackId)){
            std::cout << station->getName().value_or("gare de nom inconnu");
            if(auto platforms=snapshot.getPlatformOccupationsForStation(station->getId())){
                for(const auto& row:*platforms)if(row.platform.getTrackId()==*trackId){
                    auto name=row.platform.getName();
                    std::cout << " / quai " << (name&&!name->empty()?*name:"nom inconnu");
                    break;
                }
            }
        }else std::cout << "hors gare";
        std::cout << " / voie=" << *trackId;
        if(flags&&(*flags&NIMBY_MOTION_HIDDEN))std::cout << " (position masquee dans le jeu)";
    }else if(flags&&!(*flags&(NIMBY_MOTION_PRESENCE|NIMBY_MOTION_DRIVE|NIMBY_MOTION_HIDDEN))){
        std::cout << " | lieu=hors reseau (non place dans la simulation)";
    }else std::cout << " | lieu=inconnu (position indisponible)";
}

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
            const auto service=snapshot->getTrainServiceById(train.getId());
            printLocation(*snapshot,train,service);
            if(train.isSpeedDefaulted())std::cout << " | vitesse non mesuree";
            else if(auto speed=train.getSpeedKmh())std::cout << " | " << *speed << " km/h";
            else std::cout << " | vitesse indisponible";
            if(service){
                std::cout << " | state=" << service->getStatusName();
                if(auto name=service->getLineName())std::cout << " | run=" << *name;
                if(auto id=service->getStopStationId())if(auto station=snapshot->getStationById(*id))
                    std::cout << " | service stop=" << station->getName().value_or("unknown");
                if(auto remaining=service->getDepartureRemainingSeconds())std::cout << " | departs in=" << *remaining << " game s";
                if(auto time=service->getDepartureCalendarSeconds()){
                    const auto day=*time%86400;
                    std::cout << " | departure=" << std::setfill('0') << std::setw(2) << day/3600 << ':' << std::setw(2) << day/60%60 << ':' << std::setw(2) << day%60 << std::setfill(' ') << " (game)";
                }
                if(auto remaining=service->getArrivalRemainingSeconds())std::cout << " | arrival delta=" << *remaining << " game s";
                if(auto remaining=service->getDispatchRemainingSeconds())std::cout << " | prochaine tentative de mise en circulation dans " << *remaining << " s de jeu";
                if(auto alert=service->getAlert();alert&&*alert)std::cout << " | alert=" << *alert;
            }

            if(auto details=snapshot->getTrainDetailsById(train.getId())){
                if(auto count=details->getPassengerCount())std::cout << " | passengers=" << *count;
                if(auto id=details->getScheduleId())std::cout << " | schedule=" << *id << " | shift=" << *details->getShiftId();
            }
            if(auto stops=snapshot->getLineStopsForTrain(train.getId()))std::cout << " | line plan stops=" << stops->size();

            // Constant-average-time lookup by the full 64-bit ID.
            auto found = snapshot->getTrainById(train.getId());
            if (!found) throw std::runtime_error("Snapshot train index mismatch");
            if (auto portions = snapshot->getReservationsForTrain(train.getId()))
                std::cout << " | reservations=" << portions->size();
            else std::cout << " | reservations unknown";
            std::cout << '\n';
        }
        std::cout << "\nGARES / QUAIS (sections de voie)\n";
        for(const auto& station:snapshot->getAllStations()){
            const auto platforms=snapshot->getPlatformOccupationsForStation(station.getId());
            std::cout << "Gare " << station.getName().value_or("nom inconnu") << " | id=" << station.getId() << '\n';
            if(!platforms){std::cout << "  Quais inconnus\n";continue;}
            if(platforms->empty())std::cout << "  Aucune voie de quai dans ce snapshot\n";
            for(const auto& state:*platforms){
                std::cout << "  Quai " << state.platform.getName().value_or("nom inconnu") << " | voie=" << state.platform.getTrackId();
                const auto occupied=state.isOccupied();
                std::cout << " | " << (!occupied?"OCCUPATION INCONNUE":*occupied?"OCCUPE":"LIBRE");
                auto printTrains=[&](const auto& trains){
                    for(size_t i=0;i<trains.size();++i){if(i)std::cout << ", ";std::cout << trains[i].getName() << " (" << trains[i].getId() << ')';}
                };
                if(occupied&&*occupied){std::cout << " par ";printTrains(*state.occupying_trains);}
                if(!state.reserving_trains)std::cout << " | reservations inconnues";
                else if(!state.reserving_trains->empty()){std::cout << " | reserve par ";printTrains(*state.reserving_trains);}
                std::cout << '\n';
            }
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
