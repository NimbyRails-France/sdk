// Mesures ciblees de plusieurs trains : aucune commande ni pointeur natif.
#include <nimby/client.hpp>
#include <algorithm>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
int main(int argc, char** argv) {
    try {
        if (argc < 4 || argc > 68) throw std::invalid_argument("Usage: nimby_signalling_driving_live COUNT INTERVAL_MS TRAIN_ID ... [--tracks TRACK_ID ...] (max 32 each)");
        auto number = [](const char* value) {
            std::string_view text(value); uint64_t result{};
            const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size()) throw std::invalid_argument("Invalid number");
            return result;
        };
        const auto count = number(argv[1]), interval = number(argv[2]);
        if (!count || count > 20000 || interval < 20 || interval > 10000) throw std::invalid_argument("Unbounded capture");
        std::vector<nimby::Id> ids, tracks;
        bool trackArguments=false;
        for (int i = 3; i < argc; ++i) {
            if (std::string_view(argv[i])=="--tracks") {
                if (trackArguments) throw std::invalid_argument("Repeated track filter");
                trackArguments=true; continue;
            }
            const auto id = number(argv[i]);
            if (id >> 48 != (trackArguments?1u:5u)) throw std::invalid_argument("Wrong ID type");
            (trackArguments?tracks:ids).push_back(id);
        }
        if (ids.empty() || ids.size()>32 || tracks.size()>32 || (trackArguments&&tracks.empty())) throw std::invalid_argument("Invalid capture scope");
        auto client = nimby::Client::connect();
        std::cout << std::setprecision(17);
        for (uint64_t i = 0; i < count; ++i) {
            for (const auto id : ids) {
                const auto observed = client.readTrain(id);
                if (!observed) throw std::runtime_error("Train reading unavailable: " + std::to_string(id));
                // Identical position/speed schema to sdk-study for crossing analysis.
                const auto& train = *observed;
                const auto position = train.getPosition();
                // Filtering output preserves the read clock and avoids recording
                // the rest of a train's journey when only BAL approaches matter.
                if (!tracks.empty() && (!position || std::find(tracks.begin(),tracks.end(),position->getTrackId())==tracks.end())) continue;
                std::cout << "{\"type\":\"driving\",\"train_id\":\"" << id << "\",\"sample\":" << i;
                std::cout << ",\"elapsed_begin_ms\":" << train.getElapsedBegin().count()
                          << ",\"elapsed_end_ms\":" << train.getElapsedEnd().count();
                std::cout << ",\"speed_mps\":";
                if (const auto speed = train.getSpeedMps()) std::cout << *speed; else std::cout << "null";
                std::cout << ",\"speed_defaulted\":" << (train.isSpeedDefaulted() ? "true" : "false");
                if (position)
                    std::cout << ",\"track_id\":\"" << position->getTrackId() << "\",\"fraction\":" << position->getFraction()
                              << ",\"direction\":" << position->getDirection();
                std::cout << "}\n";
            }
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
