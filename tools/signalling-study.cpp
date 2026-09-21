#include <nimby/signal_textures.hpp>
#include <nimby/block_topology.hpp>
#include <nimby/block_observation.hpp>
#include <nimby/signal_observation.hpp>
#include <nimby/detail/signal_settings_catalog.hpp>
#include <nimby/texture_preview.hpp>
#include <charconv>
#include <cstdlib>
#include <bit>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string_view>

namespace {
const std::string& textureSet() {
    static const std::string value=[] {
        const char* name=std::getenv("NRF_TEXTURE_SET");
        if(!name||!*name)throw std::invalid_argument("Set NRF_TEXTURE_SET to the mod texture catalogue");
        return std::string(name);
    }();
    return value;
}
using namespace std::chrono_literals;
std::string json(std::string_view s);
// Explique le blocage observé, sans inventer une indication BAL ni une branche.
std::string_view blockDiagnostic(const nimby::SignalBlock& block,
    const nimby::BlockObservation& observation, bool fresh) {
    if (block.issue == nimby::BlockBoundaryIssue::MissingSignal) return "signal_absent";
    if (block.issue == nimby::BlockBoundaryIssue::AmbiguousSignal) return "limite_de_canton_ambigue";
    if (!block.hasBoundary()) {
        if (block.traceStop == nimby::SignalTraceStop::Junction) return "itineraire_non_resolu_a_une_bifurcation";
        return "limite_aval_non_resolue";
    }
    if (!fresh) return "observation_trop_ancienne";
    if (!observation.complete) return "couverture_des_trains_incomplete";
    return "canton_observe_indication_aval_a_verifier";
}
// Read-only audit of actual block boundaries. No native aspect is guessed and
// no texture is written; unresolved topology/coverage remains visible in output.
void auditBlocks(nimby::Client& client,std::optional<nimby::Id> requested={},bool signalling=false) {
    const auto snapshot = signalling?client.captureSignalling(textureSet()):client.capture();
    const auto deliveredAge=snapshot->getAge().count();
    const auto topologyStart=std::chrono::steady_clock::now();
    std::vector<nimby::Signal> boundaries;
    for (const auto& signal : snapshot->getAllSignals()) {
        if (signal.getKind() == NIMBY_SIGNAL_PATH) boundaries.push_back(signal);
    }
    const nimby::BlockTopology topology(boundaries, snapshot->getAllTrackNodes(), snapshot->getAllTrackJunctions());
    const auto topologyMs=std::chrono::duration_cast<nimby::Milliseconds>(std::chrono::steady_clock::now()-topologyStart).count();
    const auto coverage=nimby::observeBlockCoverage(*snapshot);
    const auto readerAge=snapshot->getAge().count();
    const auto reader = nimby::observeBlocks(*snapshot);
    std::size_t resolved = 0, occupied = 0, clear = 0, unknown = 0, matchedTextures = 0;
    std::size_t occupiedExamples=0;
    for (const auto& signal : boundaries) {
        if(requested&&signal.getId()!=*requested)continue;
        const auto block = topology.read(signal.getId());
        const auto texture = snapshot->getSignalTextureById(signal.getId());
        const auto reference = texture ? texture->getReference() : std::nullopt;
        const bool belongsToMod = reference && reference->getTexturesId() == textureSet();
        matchedTextures += belongsToMod;
        const auto occupation = block.occupation(reader);
        if (block.hasBoundary()) {
            ++resolved;
            occupied += occupation == nimby::BlockOccupancy::Occupied;
            clear += occupation == nimby::BlockOccupancy::Clear;
            unknown += occupation == nimby::BlockOccupancy::Unknown;
        }
        const bool example=!requested&&!belongsToMod&&occupation==nimby::BlockOccupancy::Occupied&&occupiedExamples<3;
        if(example)++occupiedExamples;
        if (belongsToMod||requested||example) {
            const auto observation=block.observe(reader);
            std::cout << "{\"type\":\""<<(example?"occupied_block_example":"mod_block")<<"\",\"signal\":\"" << signal.getId()
                << "\",\"next_signal\":\"" << block.nextSignal << "\",\"sections\":" << block.sections.size()
                << ",\"track_id\":\"" << signal.getTrackId() << "\",\"fraction\":" << signal.getFraction()
                << ",\"stored_direction\":" << signal.getDirection()
                << ",\"forward_direction\":" << signal.getForwardDirection()
                << ",\"boundary_known\":" << (block.hasBoundary() ? "true" : "false")
                << ",\"issue\":" << static_cast<int>(block.issue)
                << ",\"trace_stop\":" << static_cast<int>(block.traceStop)
                << ",\"diagnostic\":" << json(blockDiagnostic(block, observation, readerAge <= 1000))
                << ",\"occupation\":" << static_cast<int>(observation.occupation)
                << ",\"reader_age_ms\":"<<readerAge
                << ",\"delivered_age_ms\":"<<deliveredAge<<",\"topology_ms\":"<<topologyMs
                << ",\"coverage_verified\":"<<(coverage.verified?"true":"false")
                << ",\"unknown_presence\":"<<coverage.unknownPresence
                << ",\"missing_footprints\":"<<coverage.missingFootprints
                << ",\"unexpected_footprints\":"<<coverage.unexpectedFootprints
                << ",\"train_list_complete\":" << (observation.complete?"true":"false") << ",\"trains\":[";
            bool first=true;
            for(const auto trainId:observation.trains){
                if(!first)std::cout<<',';first=false;
                const auto train=snapshot->getTrainById(trainId);
                std::cout<<"{\"id\":\""<<trainId<<"\",\"name\":"<<(train?json(train->getName()):"null")<<'}';
            }
            std::cout<<"]}\n";
            if(requested)for(const auto& section:block.sections){
                std::cout<<"{\"type\":\"block_section\",\"track\":\""<<section.track
                    <<"\",\"begin\":"<<section.begin<<",\"end\":"<<section.end<<"}\n";
                if(const auto footprints=snapshot->getAllOccupations())for(const auto& row:*footprints){
                    if(row.getTrackId()!=section.track)continue;
                    std::cout<<"{\"type\":\"track_footprint\",\"train\":\""<<row.getTrainId()
                        <<"\",\"track\":\""<<row.getTrackId()<<"\",\"begin\":"<<row.getBeginFraction()
                        <<",\"end\":"<<row.getEndFraction()<<"}\n";
                }
            }
        }
    }
    if(requested){
        if(!snapshot->getSignalById(*requested))throw std::invalid_argument("Signal absent from snapshot");
        if(snapshot->getSignalById(*requested)->getKind()!=NIMBY_SIGNAL_PATH)throw std::invalid_argument("Signal does not delimit a path block");
        return;
    }
    std::cout << "{\"type\":\"block_audit\",\"path_signals\":" << boundaries.size()
        << ",\"mod_signals\":" << matchedTextures << ",\"resolved_boundaries\":" << resolved
        << ",\"unresolved_boundaries\":" << boundaries.size()-resolved
        << ",\"occupied\":" << occupied << ",\"clear\":" << clear << ",\"unknown_occupation\":" << unknown << "}\n";
}
std::string json(std::string_view s) {
    std::string out = "\"";
    constexpr char hex[] = "0123456789abcdef";
    for (unsigned char c : s) {
        if (c == '"' || c == '\\') { out += '\\'; out += char(c); }
        else if (c < 32) { out += "\\u00"; out += hex[c >> 4]; out += hex[c & 15]; }
        else out += char(c);
    }
    return out + '"';
}
// Cross-check the native occupation collection against positioned trains. This
// diagnoses coverage; matching reference points alone does NOT prove every tail.
void auditCoverage(nimby::Client& client) {
    const auto snapshot=client.capture();
    const auto occupations=snapshot->getAllOccupations();
    if(!occupations) throw std::runtime_error("Occupation table unavailable");
    std::unordered_map<nimby::Id,std::vector<nimby::TrackUsage>> rowsByTrain;
    for(const auto& row:*occupations) rowsByTrain[row.getTrainId()].push_back(row);
    std::size_t positioned=0, covered=0, mirrored=0, noRows=0, unpositionedWithRows=0;
    std::size_t roundoffCovered=0;
    double maximumPointGap=0;
    for(const auto& train:snapshot->getAllTrains()) {
        const auto position=train.getPosition();
        const auto found=rowsByTrain.find(train.getId());
        if(!position) { unpositionedWithRows += found!=rowsByTrain.end(); continue; }
        ++positioned;
        bool pointCovered=false, inverseCovered=false;
        double nearestGap=std::numeric_limits<double>::infinity();
        if(found!=rowsByTrain.end()) for(const auto& row:found->second) {
            if(row.getTrackId()==position->getTrackId()) nearestGap=std::min(nearestGap,
                std::max({0.0,row.getBeginFraction()-position->getFraction(),position->getFraction()-row.getEndFraction()}));
            if(row.getTrackId()==position->getTrackId()
                && row.getBeginFraction()<=position->getFraction()
                && row.getEndFraction()>=position->getFraction()) pointCovered=true;
            if(row.getTrackId()==position->getTrackId()
                && row.getBeginFraction()<=1-position->getFraction()
                && row.getEndFraction()>=1-position->getFraction()) inverseCovered=true;
        }
        covered+=pointCovered;
        mirrored+=inverseCovered;
        roundoffCovered+=nearestGap<=1e-12;
        maximumPointGap=std::max(maximumPointGap,nearestGap);
        noRows+=found==rowsByTrain.end();
        if(!pointCovered) {
            const auto service=snapshot->getTrainServiceById(train.getId());
            std::cout << "{\"type\":\"coverage_gap\",\"train\":\"" << train.getId()
                << "\",\"name\":" << json(train.getName()) << ",\"track\":\"" << position->getTrackId()
                << "\",\"fraction\":" << position->getFraction()
                << ",\"direction\":" << position->getDirection()
                << ",\"nearest_gap\":" << (std::isfinite(nearestGap)?std::to_string(nearestGap):"null")
                << ",\"inverse_covered\":" << (inverseCovered?"true":"false")
                << ",\"rows\":" << (found==rowsByTrain.end()?0:found->second.size())
                << ",\"speed_defaulted\":" << (train.isSpeedDefaulted()?"true":"false")
                << ",\"service\":" << (service?json(service->getStatusName()):"null") << "}\n";
            if(train.getId()==1407374883553285ULL && found!=rowsByTrain.end())
                for(const auto& row:found->second) std::cout << "{\"type\":\"sample_interval\",\"track\":\""
                    << row.getTrackId() << "\",\"begin\":" << row.getBeginFraction() << ",\"end\":" << row.getEndFraction() << "}\n";
        }
    }
    const auto coverage=nimby::observeBlockCoverage(*snapshot);
    std::cout << "{\"type\":\"coverage_check\",\"verified\":" << (coverage.verified?"true":"false")
        << ",\"unknown_presence\":" << coverage.unknownPresence << ",\"missing_footprints\":" << coverage.missingFootprints
        << ",\"unexpected_footprints\":" << coverage.unexpectedFootprints << "}\n";
    std::cout << "{\"type\":\"coverage_audit\",\"trains\":" << snapshot->getAllTrains().size()
        << ",\"positioned\":" << positioned << ",\"point_covered\":" << covered << ",\"no_rows\":" << noRows
        << ",\"inverse_covered\":" << mirrored
        << ",\"roundoff_covered\":" << roundoffCovered << ",\"maximum_point_gap\":";
    if(std::isfinite(maximumPointGap)) std::cout << maximumPointGap; else std::cout << "null";
    std::cout
        << ",\"unpositioned_with_rows\":" << unpositionedWithRows << ",\"occupation_owners\":" << rowsByTrain.size()
        << ",\"occupation_rows\":" << occupations->size() << "}\n";
}
std::uint64_t number(std::string_view text) {
    std::uint64_t value{};
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc{} || end != text.data() + text.size())
        throw std::invalid_argument("Expected an unsigned decimal integer");
    return value;
}
// Diagnostic host only; the loader ABI is shared by native and Kotlin mods.
class ModObserver {
    HMODULE module_{};
    using Lifecycle=DWORD(WINAPI*)(void*);
    using Read=DWORD(WINAPI*)(uint64_t,NimbyDrivingObservation*);
    Lifecycle stop_{};
    Read read_{};
public:
    ModObserver() {
        const char* library=std::getenv("NRF_MOD_LIBRARY");
        if(!library||!*library)throw std::invalid_argument("Set NRF_MOD_LIBRARY to the mod DLL path");
        const auto path=std::filesystem::absolute(library);
        module_=LoadLibraryExW(path.c_str(),nullptr,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_SYSTEM32);
        if(!module_)throw std::runtime_error("Cannot load NRF_MOD_LIBRARY");
        auto start=std::bit_cast<Lifecycle>(GetProcAddress(module_,"NRFMod_StartV1"));
        stop_=std::bit_cast<Lifecycle>(GetProcAddress(module_,"NRFMod_StopV1"));
        read_=std::bit_cast<Read>(GetProcAddress(module_,"NRFMod_ReadTrainV1"));
        try {
            if(!start||!stop_||!read_)throw std::runtime_error("Mod observation adapter unavailable");
            nimby::detail::check(start(nullptr),"StartStudyMod");
        } catch(...) { FreeLibrary(module_);module_=nullptr;throw; }
    }
    ~ModObserver(){if(module_&&stop_(nullptr)==NIMBY_OK)FreeLibrary(module_);}
    ModObserver(const ModObserver&)=delete;
    ModObserver& operator=(const ModObserver&)=delete;
    std::optional<nimby::DrivingObservation> readTrain(nimby::Id id) {
        NimbyDrivingObservation out{};out.struct_size=sizeof out;
        const auto status=read_(id,&out);
        if(status==NIMBY_DATA_UNAVAILABLE)return std::nullopt;
        nimby::detail::check(status,"ModReadTrain");return nimby::DrivingObservation{out};
    }
};
template<class T> void optional(const std::optional<T>& value) {
    if (value) std::cout << *value; else std::cout << "null";
}
void printDriving(const std::optional<nimby::DrivingObservation>& sample, long long elapsedUs) {
    if(!sample) { std::cout << "{\"type\":\"driving_unavailable\",\"read_us\":" << elapsedUs << "}\n";return; }
    const auto& v=*sample;
    std::cout << "{\"type\":\"driving\",\"train_id\":" << json(std::to_string(v.getTrainId()))
              << ",\"generation\":" << v.getSessionGeneration() << ",\"read_us\":" << elapsedUs
              << ",\"elapsed_begin_ms\":" << v.getElapsedBegin().count()
              << ",\"elapsed_end_ms\":" << v.getElapsedEnd().count() << ",\"speed_mps\":";
    optional(v.getSpeedMps());
    std::cout << ",\"speed_defaulted\":" << (v.isSpeedDefaulted()?"true":"false") << ",\"track_id\":";
    const auto position=v.getPosition();
    if(position)std::cout << json(std::to_string(position->getTrackId()));else std::cout << "null";
    // Position precise pour relier la vitesse mesuree au franchissement du panneau.
    if(position)std::cout << ",\"fraction\":" << position->getFraction() << ",\"direction\":" << position->getDirection();
    else std::cout << ",\"fraction\":null,\"direction\":null";
    auto emit=[](const char* name,const std::optional<nimby::TrainDynamics>& d) {
        std::cout << ",\"" << name << "\":";
        if(!d){std::cout << "null";return;}
        std::cout << "{\"max_speed_mps\":" << d->maxSpeedMps << ",\"max_acceleration_mps2\":" << d->maxAccelerationMps2
            << ",\"service_braking_mps2\":" << d->serviceBrakingMps2 << ",\"emergency_braking_mps2\":" << d->emergencyBrakingMps2
            << ",\"tractive_effort_n\":" << d->tractiveEffortN << ",\"power_w\":" << d->powerW
            << ",\"empty_mass_kg\":" << d->emptyMassKg << ",\"length_m\":" << d->lengthM << '}';
    };
    emit("purchased",v.getPurchasedDynamics());emit("current",v.getCurrentDynamics());
    std::cout << "}\n";std::cout.flush();
}
void capture(nimby::Client& client, std::optional<nimby::Id> selected) {
    const auto started = std::chrono::steady_clock::now();
    const auto snapshot = client.capture();
    const auto captureMs = std::chrono::duration_cast<nimby::Milliseconds>(
        std::chrono::steady_clock::now() - started).count();
    const auto trainCount = snapshot->getAllTrains().size();
    std::size_t positioned = 0, speeds = 0, defaulted = 0, paths = 0, passengers = 0;
    for (const auto& train : snapshot->getAllTrains()) {
        positioned += train.getPosition().has_value();
        speeds += train.getSpeedMps().has_value();
        defaulted += train.isSpeedDefaulted();
        paths += snapshot->getPathTrackIdsForTrain(train.getId()).has_value();
        const auto detail = snapshot->getTrainDetailsById(train.getId());
        passengers += detail && detail->getPassengerCount().has_value();
    }
    std::cout << "{\"type\":\"snapshot\",\"pid\":" << snapshot->getProcessId()
              << ",\"sha256\":" << json(snapshot->getGameSha256())
              << ",\"capture_ms\":" << captureMs << ",\"captured_unix_ms\":"
              << std::chrono::duration_cast<nimby::Milliseconds>(snapshot->getCapturedAt().time_since_epoch()).count()
              << ",\"sim_elapsed_ms\":";
    const auto clock = snapshot->getSimulationClock();
    if (clock) std::cout << clock->getElapsedTime().count(); else std::cout << "null";
    std::cout << ",\"trains\":" << trainCount << ",\"positions\":" << positioned
              << ",\"speeds\":" << speeds << ",\"defaulted_speeds\":" << defaulted
              << ",\"paths\":" << paths << ",\"passenger_counts\":" << passengers
              << ",\"tracks\":" << snapshot->getAllTracks().size()
              << ",\"signals\":" << snapshot->getAllSignals().size()
              << ",\"junctions\":" << snapshot->getAllTrackJunctions().size()
              << ",\"occupation_rows\":";
    const auto occupations = snapshot->getAllOccupations();
    if (occupations) std::cout << occupations->size(); else std::cout << "null";
    std::cout << ",\"reservation_rows\":";
    const auto reservations = snapshot->getAllReservations();
    if (reservations) std::cout << reservations->size(); else std::cout << "null";
    std::cout << "}\n";
    if (selected && !snapshot->getTrainById(*selected))
        throw std::runtime_error("Selected train is absent from this snapshot");
    for (const auto& train : snapshot->getAllTrains()) {
        if (selected && *selected != train.getId()) continue;
        std::cout << "{\"type\":\"train\",\"id\":" << json(std::to_string(train.getId()))
                  << ",\"name\":" << json(train.getName()) << ",\"speed_mps\":";
        optional(train.getSpeedMps());
        std::cout << ",\"speed_defaulted\":" << (train.isSpeedDefaulted() ? "true" : "false");
        if (auto p = train.getPosition())
            std::cout << ",\"track_id\":" << json(std::to_string(p->getTrackId()))
                      << ",\"fraction\":" << p->getFraction() << ",\"direction\":" << p->getDirection();
        else std::cout << ",\"track_id\":null,\"fraction\":null,\"direction\":null";
        std::cout << ",\"track_limit_mps\":";
        if (auto t = snapshot->getTrackForTrain(train.getId())) std::cout << t->getSpeedLimitMps();
        else std::cout << "null";
        const auto service = snapshot->getTrainServiceById(train.getId());
        // L'audit doit identifier la ligne reelle avant de modifier le parc d'essai.
        const auto lineName = service ? service->getLineName() : std::nullopt;
        std::cout << ",\"line\":" << (lineName ? json(*lineName) : "null");
        std::cout << ",\"service\":" << (service ? json(service->getStatusName()) : "null")
                  << ",\"passengers\":";
        const auto detail = snapshot->getTrainDetailsById(train.getId());
        optional(detail ? detail->getPassengerCount() : std::nullopt);
        // Signaux exactement au nez : diagnostic d'attente, pas une resolution
        // de l'itineraire ni une autorisation de franchissement.
        std::cout << ",\"signals_at_head\":[";
        bool firstSignal = true;
        if (const auto position = train.getPosition())
            for (const auto& signal : snapshot->getSignalsForTrack(position->getTrackId())) {
                if (signal.getKind() != NIMBY_SIGNAL_PATH ||
                    signal.getForwardDirection() != position->getDirection() ||
                    std::abs(signal.getFraction() - position->getFraction()) > 1e-8) continue;
                if (!firstSignal) std::cout << ',';
                firstSignal = false;
                std::cout << json(std::to_string(signal.getId()));
            }
        std::cout << ']';
        std::cout << "}\n";
    }
    std::cout.flush();
}
}

int main(int argc, char** argv) {
    try {
        std::cout << std::setprecision(17);
        const std::string_view command = argc > 1 ? argv[1] : "help";
        if (command == "help") {
            std::cout << "SDK signalling study (read-only unless texture-* is explicitly used)\n"
                         "  capabilities\n  capture\n  blocks\n  coverage\n  benchmark-signals\n  benchmark-signalling\n  watch TRAIN_ID COUNT INTERVAL_MS\n"
                         "  read-train TRAIN_ID\n  watch-train TRAIN_ID COUNT INTERVAL_MS\n"
                         "  mod-watch-train TRAIN_ID COUNT INTERVAL_MS\n"
                         "  texture-show SIGNAL_ID RELATIVE_SVG_PATH\n  texture-restore SIGNAL_ID\n"
                         "  texture-animate SIGNAL_ID FIRST_SVG SECOND_SVG HALF_MS DURATION_MS\n"
                         "  texture-status SIGNAL_ID\n  block SIGNAL_ID (trains physically occupying the block)\n"
                         "  watch-block SIGNAL_ID COUNT INTERVAL_MS\n"
                         "  watch-block-signalling SIGNAL_ID COUNT INTERVAL_MS\n"
                         "IDs: full unsigned decimal. watch: 1..1000 captures, 100..60000 ms between captures.\n"
                         "watch-train: targeted reads, up to 20000 captures, minimum 20 ms.\n";
            return 0;
        }
        if (command == "capabilities" && argc == 2) {
            const auto v = nimby::getVersion();
            std::cout << "SDK " << v.major << '.' << v.minor << '.' << v.patch << " ABI " << v.abi << '\n'
                      << "Available: snapshot, simulation clock, train position/speed/service/passengers, "
                         "track limits, topology, path membership, occupation/reservation intervals, textures.\n"
                         "Commands available: texture-show, texture-restore (visual only).\n"
                         "SDK 0.7.3: read-train/watch-train provide targeted motion and train dynamics.\n"
                         "Not exposed: route distances, traction/brake commands, "
                         "speed target, native UI extension fields.\n"
                         "Native script capabilities are separate; see docs/sdk-conduite-etude.md.\n";
            return 0;
        }
        std::optional<nimby::Id> id;
        std::uint64_t count = 1, interval = 1000;
        if ((command == "watch" || command == "watch-train" || command == "mod-watch-train" || command == "watch-block" || command == "watch-block-signalling") && argc == 5) {
            id = number(argv[2]); count = number(argv[3]); interval = number(argv[4]);
            // La lecture ciblee permet de situer le franchissement du panneau
            // sans refaire une capture du monde a chaque mesure de vitesse.
            const bool targeted = command == "watch-train";
            if (!*id || count < 1 || count > (targeted ? 20000 : 1000)
                || interval < (targeted ? 20 : 100) || interval > 60000)
                throw std::invalid_argument("Invalid watch bounds");
            if((command=="watch-block"||command=="watch-block-signalling")&&(*id>>48)!=8)throw std::invalid_argument("Expected full signal ID");
        } else if(command=="block"&&argc==3){
            id=number(argv[2]);
            if((*id>>48)!=8)throw std::invalid_argument("Expected full signal ID");
        } else if (command == "read-train" && argc == 3) {
            id=number(argv[2]);
            if((*id>>48)!=5)throw std::invalid_argument("Expected full train ID");
        } else if ((command == "texture-show" && argc == 4) || (command == "texture-restore" && argc == 3)
            || (command == "texture-status" && argc == 3) || (command == "texture-animate" && argc == 7)) {
            id = number(argv[2]);
            if (!*id) throw std::invalid_argument("Signal ID must be nonzero");
            if(command == "texture-animate") {
                interval=number(argv[5]); count=number(argv[6]);
                if(interval<100 || interval>10000 || count<1000 || count>60000)
                    throw std::invalid_argument("Invalid animation half-period or lease");
            }
        } else if (!((command == "capture" || command == "blocks" || command == "coverage" || command == "benchmark-signals" || command == "benchmark-signalling") && argc == 2)) {
            throw std::invalid_argument("Unknown command or arguments; use help");
        }
        if(command=="mod-watch-train") {
            ModObserver mod;bool observed=false;
            for(std::uint64_t i=0;i<count;++i) {
                const auto start=std::chrono::steady_clock::now();
                auto sample=mod.readTrain(*id);
                const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
                printDriving(sample,us);observed=observed||sample.has_value();
                if(i+1<count)std::this_thread::sleep_for(nimby::Milliseconds(interval));
            }
            if(!observed)throw std::runtime_error("No successful mod observation");
            return 0;
        }
        auto client = nimby::Client::connect();
        if(command=="benchmark-signals"||command=="benchmark-signalling") {
            // Lecture seule : separer le cout de capture du traitement du meme snapshot.
            const auto start=std::chrono::steady_clock::now();
            const auto snapshot=command=="benchmark-signalling"?client.captureSignalling(textureSet()):client.capture();
            const auto captured=std::chrono::steady_clock::now();
            const auto catalog=nimby::detail::signalSettingsCatalog(*snapshot);
            const auto catalogued=std::chrono::steady_clock::now();
            const auto signals=nimby::observeSignals(*snapshot,textureSet());
            const auto observed=std::chrono::steady_clock::now();
            const auto us=[](auto a,auto b){return std::chrono::duration_cast<std::chrono::microseconds>(b-a).count();};
            std::cout<<"{\"capture_us\":"<<us(start,captured)
                <<",\"catalog_us\":"<<us(captured,catalogued)
                <<",\"observe_signals_us\":"<<us(catalogued,observed)
                <<",\"catalog_valid\":"<<(catalog?"true":"false")
                <<",\"signals\":"<<signals.size()<<",\"fresh\":"
                <<(!snapshot->isOlderThan(nimby::Milliseconds{1000})?"true":"false")<<"}\n";
            return catalog?0:1;
        }
        if(command=="watch-block"||command=="watch-block-signalling"){
            const auto start=std::chrono::steady_clock::now();
            std::uint64_t successes=0;
            for(std::uint64_t attempt=0;attempt<count;++attempt){
                const auto now=std::chrono::steady_clock::now();
                std::cout<<"{\"type\":\"block_sample\",\"attempt\":"<<attempt+1
                    <<",\"elapsed_ms\":"<<std::chrono::duration_cast<nimby::Milliseconds>(now-start).count()<<"}\n";
                try{auditBlocks(client,id,command=="watch-block-signalling");++successes;}
                catch(const nimby::Exception& error){
                    if(error.code()!=nimby::ErrorCode::DataUnavailable)throw;
                    std::cout<<"{\"type\":\"capture_error\",\"message\":"<<json(error.what())<<"}\n";
                }
                std::cout.flush();
                if(attempt+1<count)std::this_thread::sleep_for(nimby::Milliseconds{interval});
            }
            if(!successes)throw std::runtime_error("No successful block capture");
            return 0;
        }
        if(command=="block"){auditBlocks(client,id);return 0;}
        if (command == "coverage") { auditCoverage(client); return 0; }
        if (command == "blocks") {
            auditBlocks(client);
            return 0;
        }
        if(command=="read-train"||command=="watch-train") {
            bool observed=false;
            for(std::uint64_t i=0;i<count;++i) {
                const auto start=std::chrono::steady_clock::now();
                auto sample=client.readTrain(*id);
                const auto us=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count();
                printDriving(sample,us);observed=observed||sample.has_value();
                if(i+1<count)std::this_thread::sleep_for(nimby::Milliseconds(interval));
            }
            if(!observed)throw std::runtime_error("No successful targeted observation");
        } else if (command == "texture-status") {
            const auto state=nimby::getSignalTextureOverrideStatus(client.getProcessId(),*id);
            const auto renderer=nimby::getTexturePreviewStatus(client.getProcessId());
            std::cout << "{\"signal\":\"" << *id << "\",\"active\":" << state.active
                << ",\"texture_index\":" << state.index << ",\"active_overrides\":" << state.active_count
                << ",\"expires_wall_ms\":" << state.expires_at_ms
                << ",\"render_callbacks\":" << renderer.callbacks << ",\"applied\":" << renderer.applied << "}\n";
        } else if (command == "texture-show" || command == "texture-restore" || command == "texture-animate") {
            const auto snapshot = client.capture();
            if (!snapshot->getSignalById(*id)) throw std::invalid_argument("Signal absent from snapshot");
            auto textures = nimby::SignalTextures::connect(client.getProcessId());
            if (command == "texture-show") textures.show(*id, {textureSet(), argv[3]});
            else if(command == "texture-animate") textures.animateFor(*id,{textureSet(),argv[3]},argv[4],
                nimby::Milliseconds{interval},nimby::Milliseconds{count});
            else textures.restore(*id);
            std::cout << "SDK accepted visual command; rendering and permissions not verified.\n";
        } else {
            bool captured = false;
            for (std::uint64_t i = 0; i < count; ++i) {
                try {
                    capture(client, id);
                    captured = true;
                } catch (const nimby::Exception& e) {
                    if (command != "watch" || e.code() != nimby::ErrorCode::DataUnavailable) throw;
                    std::cout << "{\"type\":\"capture_error\",\"attempt\":" << i + 1
                              << ",\"message\":" << json(e.what()) << "}\n";
                    std::cout.flush();
                }
                if (i + 1 < count) std::this_thread::sleep_for(nimby::Milliseconds(interval));
            }
            if (!captured) throw std::runtime_error("No successful capture; no previous data reused");
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "SDK study: " << e.what() << '\n';
        return 1;
    }
}
