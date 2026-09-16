#include <nimby/client.hpp>
#include <charconv>
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std::chrono_literals;
int runObserverWindow(std::uint32_t pid=0);

// Location comes from native state, never from a future stop or a reservation.
void printLocation(const nimby::Snapshot& snapshot, const nimby::Train& train,
                   const std::optional<nimby::TrainService>& service) {
    auto trackId=service?service->getLocationTrackId():std::nullopt;
    if(!trackId)if(auto position=train.getPosition())trackId=position->getTrackId();
    if(trackId){
        std::cout << " | lieu=";
        if(auto station=snapshot.getStationForTrack(*trackId)){
            std::cout << station->getName().value_or("gare de nom inconnu");
            if(auto platforms=snapshot.getPlatformOccupationsForStation(station->getId())){
                for(const auto& row:*platforms)if(row.platform.containsTrack(*trackId)){
                    auto name=row.platform.getName();
                    std::cout << " / quai " << (name&&!name->empty()?*name:"nom inconnu");
                    break;
                }
            }
        }else std::cout << "hors gare";
        std::cout << " / voie=" << *trackId;
        if(service&&service->isHidden().value_or(false))std::cout << " (position masquee dans le jeu)";
    }else if(service&&service->isOnNetwork()==false){
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
        if(auto clock=snapshot->getSimulationClock())
            std::cout << "Simulation date (UTC): " << clock->getDateTimeUtcString() << '\n';
        else std::cout << "Simulation date unavailable\n";

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
                    const auto day=(*time%86400+86400)%86400;
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
                std::cout << "  Quai " << state.platform.getName().value_or("nom inconnu") << " | voies=";
                for(auto id:state.platform.getTrackIds())std::cout<<id<<' ';
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
        if(argc==3 && (std::strcmp(argv[1],"--set-date-utc")==0 || std::strcmp(argv[1],"--set-date-and-recalculate-utc")==0)) {
            const std::string_view input{argv[2]};
            if(input.size()!=20 || input[4]!='-' || input[7]!='-' || input[10]!='T' || input[13]!=':' || input[16]!=':' || input[19]!='Z')
                throw std::invalid_argument("Expected YYYY-MM-DDTHH:MM:SSZ (UTC)");
            auto number=[&](size_t offset,size_t count){
                int value{};const char* first=input.data()+offset;const char* last=first+count;
                for(auto p=first;p!=last;++p)if(*p<'0'||*p>'9')throw std::invalid_argument("Invalid date digit");
                const auto parsed=std::from_chars(first,last,value);
                if(parsed.ec!=std::errc{}||parsed.ptr!=last)throw std::invalid_argument("Invalid date number");
                return value;
            };
            const int year=number(0,4),month=number(5,2),day=number(8,2),hour=number(11,2),minute=number(14,2),second=number(17,2);
            if(year<1 || month<1 || month>12 || day<1 || day>31 || hour>23 || minute>59 || second>59)
                throw std::invalid_argument("Expected YYYY-MM-DDTHH:MM:SSZ (UTC)");
            const std::chrono::year_month_day date{std::chrono::year{year},std::chrono::month{static_cast<unsigned>(month)},std::chrono::day{static_cast<unsigned>(day)}};
            if(!date.ok())throw std::invalid_argument("Invalid calendar date");
            auto client=nimby::Client::connect();
            const auto utc=std::chrono::sys_days{date}+std::chrono::hours{hour}+std::chrono::minutes{minute}+std::chrono::seconds{second};
            if(std::strcmp(argv[1],"--set-date-and-recalculate-utc")==0) {
                const auto result=client.setSimulationDateTimeAndRecalculateTrains(utc);
                std::cout<<"Simulation date (UTC): "<<result.clock.getDateTimeUtcString()<<" | native interventions="<<result.interventions<<'\n';
            }else {
                const auto result=client.setSimulationDateTime(utc);
                std::cout << "Simulation calendar changed (UTC): " << result.getDateTimeUtcString() << '\n';
            }
            return 0;
        }
        if(argc==2 && std::strcmp(argv[1],"--clock")==0){
            auto client=nimby::Client::connect();
            const auto snapshot=client.capture();
            const auto clock=snapshot->getSimulationClock();
            if(!clock)throw std::runtime_error("Simulation clock unavailable");
            std::cout << clock->getDateTimeUtcString() << " | epoch=" << clock->getEpochSeconds()
                      << " | elapsed_ms=" << clock->getElapsedTime().count() << '\n';
            return 0;
        }
        if (argc == 2 && std::strcmp(argv[1], "--check-sdk") == 0) {
            const auto version = nimby::getVersion();
            std::cout << "C++ helpers ready | SDK " << version.major << '.' << version.minor
                      << '.' << version.patch << " | internal ABI " << version.abi << '\n';
            return 0; // This check does not need a running game.
        }
        if(argc==2 && std::strcmp(argv[1],"--console")==0){
            auto client=nimby::Client::connect();return observe(client);
        }
        if (argc == 1) {
            return runObserverWindow();
        }
        if (argc == 2) {
            std::uint32_t gamePid{};
            const auto end = argv[1] + std::strlen(argv[1]);
            const auto parsed = std::from_chars(argv[1], end, gamePid);
            if (parsed.ec == std::errc{} && parsed.ptr == end && gamePid != 0) {
                return runObserverWindow(gamePid);
            }
        }
        std::cerr << "Usage: MyNimbyClient [game-pid | --console | --check-sdk | --clock | --set-date-utc YYYY-MM-DDTHH:MM:SSZ | --set-date-and-recalculate-utc YYYY-MM-DDTHH:MM:SSZ]\n";
        return 2;
    } catch (const nimby::Exception& error) {
        std::cerr << "SDK error " << error.error().code << ": " << error.what() << '\n';
        return 1;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
