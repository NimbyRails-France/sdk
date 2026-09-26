// Diagnostic en lecture seule : distinguer une limite de voie d'une consigne BAL.
#include <nimby/detail/observation_session.hpp>
#include <charconv>
#include <iomanip>
#include <iostream>
#include <vector>
int main(int argc, char** argv) {
    try {
        if (argc < 2 || argc > 33) throw std::invalid_argument("Usage: nimby_signalling_track_study TRACK_ID [TRACK_ID ...] (maximum 32)");
        std::vector<nimby::Id> ids;
        for (int i = 1; i < argc; ++i) {
            const std::string_view text(argv[i]);
            nimby::Id id{};
            const auto parsed = std::from_chars(text.data(), text.data() + text.size(), id);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size() || !id)
                throw std::invalid_argument("Invalid track ID");
            ids.push_back(id);
        }
        auto client = nimby::detail::ObservationSession(nimby::detail::discoverProcess());
        const auto snapshot = client.capture();
        std::cout << std::setprecision(17);
        for (const auto id : ids) {
            const auto track = snapshot->getTrackById(id);
            if (!track) throw std::runtime_error("Track absent: " + std::to_string(id));
            std::cout << "{\"track_id\":\"" << id << "\",\"limit_mps\":"
                      << track->getSpeedLimitMps();
            // Coordonnees projetees du noeud : reperage relatif sur la carte,
            // sans les presenter comme latitude/longitude ou longueur de voie.
            for (const auto& node : snapshot->getAllTrackNodes()) {
                if (node.getId() != id) continue;
                const auto point = node.getCoordinates();
                std::cout << ",\"node_x\":" << point.x << ",\"node_y\":" << point.y;
                break;
            }
            std::cout << "}\n";
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
