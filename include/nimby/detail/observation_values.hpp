#pragma once
// Private native observation values used by the Kotlin adapter and SDK tests.
// Consumer APIs are Kotlin. These native types have no source compatibility contract.
#include <nimby/detail/observation.h>
#include <nimby/signal_settings.hpp>
#include <nimby/detail/platform/host.hpp>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <set>
#include <tuple>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nimby {
using Id = std::uint64_t;
using Milliseconds = std::chrono::milliseconds;
enum class SnapshotScope { Complete, Signalling, Session, TrainData, NetworkTopology };

enum class ErrorCode : std::uint32_t {
    Ok = 0, InvalidArgument = 1, IoError = 2, InvalidBinary = 3,
    AlreadyInitialized = 4, HooksUnavailable = 5, InternalError = 6,
    UnsupportedGame = 7, DataUnavailable = 8, InvalidHandle = 9,
    BufferTooSmall = 10, ProcessExited = 11, ResourceLimit = 12, ClockWriteFailed = 13
};

struct Error {
    std::uint32_t code;
    std::string operation;
    std::string message;
};
class Exception : public std::runtime_error {
public:
    explicit Exception(Error error)
        : std::runtime_error(error.operation + ": " + error.message), error_(std::move(error)) {}
    const Error& error() const noexcept { return error_; }
    ErrorCode code() const noexcept { return static_cast<ErrorCode>(error_.code); }
private:
    Error error_;
};
namespace detail {
inline void check(std::uint32_t code, const char* operation) {
    if (code != NIMBY_OK) throw Exception({code, operation, NimbyInternal_StatusString(code)});
}
inline std::optional<Id> reference(Id id) {
    return id ? std::optional<Id>{id} : std::nullopt;
}
}
struct Version {
    std::uint32_t major, minor, patch, abi;
};
// Checks the installed runtime without opening a game process.
inline Version getVersion() {
    NimbySdkVersion version{};
    version.struct_size = sizeof version;
    detail::check(NimbyInternal_GetVersion(&version), "GetVersion");
    if (version.abi_version != NIMBY_OBSERVATION_ABI_VERSION ||
        version.major != 0 || version.minor != 8)
        detail::check(NIMBY_INVALID_ARGUMENT, "NimbyRailsFranceSDK 0.8.x required");
    return {version.major, version.minor, version.patch, version.abi_version};
}
struct Coordinates { double x, y; };
struct SpecificState { std::string system, state; };

class SimulationClock {
public:
    explicit SimulationClock(const NimbySimulationClock& data) : data_(data) {}
    std::chrono::sys_time<Milliseconds> getDateTimeUtc() const {
        return std::chrono::sys_time<Milliseconds>{Milliseconds{data_.epoch_seconds*1000+data_.ticks*10}};
    }
    Milliseconds getElapsedTime() const { return Milliseconds{data_.ticks*10}; }
    int64_t getEpochSeconds() const { return data_.epoch_seconds; }
    std::string getDateTimeUtcString() const {
        const auto time=getDateTimeUtc();
        const auto day=std::chrono::floor<std::chrono::days>(time);
        const std::chrono::year_month_day date{day};
        const std::chrono::hh_mm_ss tod{time-day};
        char text[40]{};
        std::snprintf(text,sizeof text,"%04d-%02u-%02uT%02lld:%02lld:%02lld.%03lldZ",
            int(date.year()),unsigned(date.month()),unsigned(date.day()),
            static_cast<long long>(tod.hours().count()),static_cast<long long>(tod.minutes().count()),
            static_cast<long long>(tod.seconds().count()),static_cast<long long>(tod.subseconds().count()));
        return text;
    }
private:
    NimbySimulationClock data_;
};

struct SimulationTimeChange {
    SimulationClock clock;
    std::uint32_t interventions;
};

class Position {
public:
    Position(Id track, double fraction, std::int32_t direction)
        : track_(track), fraction_(fraction), direction_(direction) {}
    Id getTrackId() const noexcept { return track_; }
    double getFraction() const noexcept { return fraction_; }
    std::int32_t getDirection() const noexcept { return direction_; }
private:
    Id track_;
    double fraction_;
    std::int32_t direction_;
};

class Train {
public:
    explicit Train(const NimbyTrain& data) : data_(data) {}
    Id getId() const { return data_.id; }
    std::string getName() const { return data_.name_utf8; }
    // Speed is available only when explicitly validated.
    std::optional<double> getSpeedMps() const { return (data_.flags & NIMBY_TRAIN_SPEED_VALID) ? std::optional<double>{data_.speed_mps} : std::nullopt; }
    std::optional<double> getSpeedKmh() const { auto speed = getSpeedMps(); return speed ? std::optional<double>{*speed * 3.6} : std::nullopt; }
    bool isSpeedDefaulted() const { return (data_.flags & (NIMBY_TRAIN_SPEED_VALID | NIMBY_TRAIN_SPEED_DEFAULTED)) == (NIMBY_TRAIN_SPEED_VALID | NIMBY_TRAIN_SPEED_DEFAULTED); }
    std::optional<Position> getPosition() const { return (data_.flags & NIMBY_TRAIN_POSITION_VALID) ? std::optional<Position>{Position{data_.track_id, data_.track_fraction, data_.direction}} : std::nullopt; }
private:
    NimbyTrain data_;
};

} // namespace nimby
#include <nimby/detail/driving_types.hpp>
namespace nimby {

class TrainDetails {
public:
    explicit TrainDetails(const NimbyTrainDetails& data) : data_(data) {}
    Id getTrainId() const { return data_.train_id; }
    std::optional<int32_t> getPassengerCount() const { return data_.flags&NIMBY_TRAIN_PASSENGERS_VALID?std::optional<int32_t>{data_.passenger_count}:std::nullopt; }
    std::optional<Id> getScheduleId() const { return data_.flags&NIMBY_TRAIN_ASSIGNMENT_VALID?std::optional<Id>{data_.schedule_id}:std::nullopt; }
    std::optional<Id> getShiftId() const { return data_.flags&NIMBY_TRAIN_ASSIGNMENT_VALID?std::optional<Id>{data_.shift_id}:std::nullopt; }
    std::optional<int32_t> getOrderIndex() const { return data_.flags&NIMBY_TRAIN_ASSIGNMENT_VALID?std::optional<int32_t>{data_.order_index}:std::nullopt; }
    int32_t getOrderMode() const { return data_.order_mode; }
private:
    NimbyTrainDetails data_;
};
class TrainMetadata {
public:
    explicit TrainMetadata(const NimbyTrainMetadata& data):data_(data){}
    Id getTrainId() const noexcept{return data_.train_id;}
    const NimbyTrainCharacteristics& getConfiguredCharacteristics() const noexcept{return data_.configured;}
    const NimbyTrainCharacteristics& getCurrentCharacteristics() const noexcept{return data_.current;}
    std::optional<int64_t> getPredictedArrivalDelayUs() const noexcept {
        return data_.flags&NIMBY_TRAIN_PREDICTED_DELAY_VALID?std::optional<int64_t>{data_.predicted_arrival_delay_us}:std::nullopt;
    }
    std::optional<double> getPredictedArrivalDelaySeconds() const noexcept {
        const auto value=getPredictedArrivalDelayUs();return value?std::optional<double>{double(*value)/1000000}:std::nullopt;
    }
private:
    NimbyTrainMetadata data_;
};
class Line {
public:
    explicit Line(const NimbyLineMetadata& data):data_(data){}
    Id getId() const noexcept{return data_.line_id;}
    bool hasParentInformation() const noexcept{return data_.flags&NIMBY_LINE_PARENT_VALID;}
    std::optional<Id> getParentId() const noexcept{return hasParentInformation()?detail::reference(data_.parent_line_id):std::nullopt;}
    std::optional<std::string> getName() const{return data_.flags&NIMBY_LINE_NAME_VALID?std::optional<std::string>{data_.name_utf8}:std::nullopt;}
    std::optional<int32_t> getKind() const noexcept{return data_.flags&NIMBY_LINE_KIND_VALID?std::optional<int32_t>{data_.kind}:std::nullopt;}
private:
    NimbyLineMetadata data_;
};
class TrainVehicle {
public:
    explicit TrainVehicle(const NimbyTrainVehicle& data):data_(data){}
    Id getTrainId() const noexcept{return data_.train_id;}
    Id getModelId() const noexcept{return data_.model_id;}
    uint32_t getIndex() const noexcept{return data_.index;}
    uint32_t getComposition() const noexcept{return data_.composition;}
private:
    NimbyTrainVehicle data_;
};
class VehicleModel {
public:
    explicit VehicleModel(const NimbyVehicleModel& data):data_(data){}
    Id getId() const noexcept{return data_.model_id;}
    std::string_view getCode() const noexcept{return data_.code_utf8;}
    std::string_view getNameEnglish() const noexcept{return data_.name_en_utf8;}
    std::string_view getSourceName() const noexcept{return data_.source_name_utf8;}
private:
    NimbyVehicleModel data_;
};
class Tag {
public:
    explicit Tag(const NimbyTag& data):data_(data){}
    Id getId() const noexcept{return data_.tag_id;}
    std::string getName() const{return data_.name_utf8;}
private:
    NimbyTag data_;
};
class LineStop {
public:
    explicit LineStop(const NimbyLineStop& data) : data_(data) {}
    Id getLineId() const { return data_.line_id; }
    Id getTrackId() const { return data_.track_id; }
    Id getStationId() const { return data_.station_id; }
    uint32_t getIndex() const { return data_.index; }
    std::optional<int32_t> getArrivalOffsetSeconds() const { return data_.flags&NIMBY_LINE_STOP_TIMES_VALID?std::optional<int32_t>{data_.arrival_offset_seconds}:std::nullopt; }
    std::optional<int32_t> getDepartureOffsetSeconds() const { return data_.flags&NIMBY_LINE_STOP_TIMES_VALID?std::optional<int32_t>{data_.departure_offset_seconds}:std::nullopt; }
    std::optional<int32_t> getPlannedDwellSeconds() const { return data_.flags&NIMBY_LINE_STOP_TIMES_VALID?std::optional<int32_t>{data_.departure_offset_seconds-data_.arrival_offset_seconds}:std::nullopt; }
private:
    NimbyLineStop data_;
};

class TrainService {
public:
    explicit TrainService(const NimbyTrainService& data) : data_(data) {}
    Id getTrainId() const { return data_.train_id; }
    std::optional<uint32_t> getStatus() const { return valid(NIMBY_SERVICE_STATE_VALID)?std::optional<uint32_t>{data_.status}:std::nullopt; }
    const char* getStatusName() const {
        if(!getStatus())return "unknown";
        switch(data_.status){
            case NIMBY_SERVICE_DRIVING:return "driving";
            case NIMBY_SERVICE_STATION_STOP:return "stopped at station";
            case NIMBY_SERVICE_TIMED_STOP:return "timed stop";
            case NIMBY_SERVICE_DEPOT:return "at depot (hidden)";
            case NIMBY_SERVICE_DISPATCH_WAIT:return "waiting for dispatch";
            case NIMBY_SERVICE_SIGNAL_WAIT:return "waiting at signal";
            case NIMBY_SERVICE_MOTHBALLED:return "mothballed";
            case NIMBY_SERVICE_NOT_PRESENT:return "not present on tracks";
            default:return "other";
        }
    }
    std::optional<uint32_t> getMotionFlags() const { return valid(NIMBY_SERVICE_STATE_VALID)?std::optional<uint32_t>{data_.motion_flags}:std::nullopt; }
    std::optional<bool> isHidden() const {
        return (data_.flags&(NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_PRESENCE_VALID)) ? std::optional<bool>{(data_.motion_flags & NIMBY_MOTION_HIDDEN) != 0} : std::nullopt;
    }
    std::optional<bool> isOnNetwork() const {
        return (data_.flags&(NIMBY_SERVICE_STATE_VALID|NIMBY_SERVICE_PRESENCE_VALID)) ? std::optional<bool>{(data_.motion_flags & (NIMBY_MOTION_PRESENCE | NIMBY_MOTION_DRIVE | NIMBY_MOTION_HIDDEN)) != 0} : std::nullopt;
    }
    std::optional<uint32_t> getAlert() const { return valid(NIMBY_SERVICE_STATE_VALID)?std::optional<uint32_t>{data_.alert}:std::nullopt; }
    std::optional<Id> getLocationTrackId() const { return valid(NIMBY_SERVICE_LOCATION_VALID)?detail::reference(data_.location_track_id):std::nullopt; }
    std::optional<Id> getLocationStationId() const { return valid(NIMBY_SERVICE_LOCATION_VALID)?detail::reference(data_.location_station_id):std::nullopt; }
    std::optional<Id> getLineId() const { return valid(NIMBY_SERVICE_RUN_VALID)?detail::reference(data_.line_id):std::nullopt; }
    std::optional<std::string> getLineName() const { return valid(NIMBY_SERVICE_LINE_VALID)&&data_.line_name_utf8[0]?std::optional<std::string>{data_.line_name_utf8}:std::nullopt; }
    std::optional<int32_t> getLineKind() const { return valid(NIMBY_SERVICE_LINE_VALID)?std::optional<int32_t>{data_.line_kind}:std::nullopt; }
    std::optional<Id> getStopStationId() const { return valid(NIMBY_SERVICE_STOP_VALID)?detail::reference(data_.stop_station_id):std::nullopt; }
    std::optional<Id> getStopTrackId() const { return valid(NIMBY_SERVICE_STOP_VALID)?detail::reference(data_.stop_track_id):std::nullopt; }
    std::optional<int32_t> getStopIndex() const { return valid(NIMBY_SERVICE_RUN_VALID)?std::optional<int32_t>{data_.stop_index}:std::nullopt; }
    std::optional<int64_t> getGameTimeUs() const { return valid(NIMBY_SERVICE_CLOCK_VALID)?std::optional<int64_t>{data_.game_time_us}:std::nullopt; }
    std::optional<int64_t> getDepartureTimeUs() const { return valid(NIMBY_SERVICE_DEPARTURE_VALID)?std::optional<int64_t>{data_.departure_time_us}:std::nullopt; }
    std::optional<int64_t> getArrivalTimeUs() const { return valid(NIMBY_SERVICE_ARRIVAL_VALID)?std::optional<int64_t>{data_.arrival_time_us}:std::nullopt; }
    std::optional<int64_t> getDispatchTimeUs() const { return valid(NIMBY_SERVICE_COOLDOWN_VALID)?std::optional<int64_t>{data_.dispatch_time_us}:std::nullopt; }
    std::optional<int64_t> getGameEpochSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID)?std::optional<int64_t>{data_.game_epoch_seconds}:std::nullopt; }
    std::optional<int64_t> getGameCalendarSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_CLOCK_VALID)?calendarSeconds(data_.game_time_us):std::nullopt; }
    std::optional<int64_t> getDepartureCalendarSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_DEPARTURE_VALID)?calendarSeconds(data_.departure_time_us):std::nullopt; }
    std::optional<int64_t> getArrivalCalendarSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_ARRIVAL_VALID)?calendarSeconds(data_.arrival_time_us):std::nullopt; }
    std::optional<double> getDepartureRemainingSeconds() const { return valid(NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_DEPARTURE_VALID)?std::optional<double>{data_.departure_remaining_seconds}:std::nullopt; }
    std::optional<double> getArrivalRemainingSeconds() const { return valid(NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_ARRIVAL_VALID)?std::optional<double>{data_.arrival_remaining_seconds}:std::nullopt; }
    std::optional<double> getDispatchRemainingSeconds() const { return valid(NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_COOLDOWN_VALID)?std::optional<double>{data_.dispatch_remaining_seconds}:std::nullopt; }
private:
    std::optional<int64_t> calendarSeconds(int64_t microseconds) const {
        const auto seconds=std::chrono::floor<std::chrono::seconds>(std::chrono::microseconds{microseconds}).count();
        const auto epoch=data_.game_epoch_seconds;
        if((seconds>0&&epoch>INT64_MAX-seconds)||(seconds<0&&epoch<INT64_MIN-seconds))return std::nullopt;
        return epoch+seconds;
    }
    bool valid(uint32_t flags) const { return (data_.flags&flags)==flags; }
    NimbyTrainService data_;
};

class Track {
public:
    explicit Track(const NimbyTrack& data) : data_(data) {}
    Id getId() const { return data_.id; }
    std::optional<Id> getStationId() const { return detail::reference(data_.station_id); }
    double getSpeedLimitMps() const { return data_.speed_limit_mps; }
    double getSpeedLimitKmh() const { return data_.speed_limit_mps * 3.6; }
private:
    NimbyTrack data_;
};

class Station {
public:
    explicit Station(const NimbyStation& data) : data_(data) {}
    Id getId() const { return data_.id; }
    std::optional<std::string> getName() const { return data_.name_utf8[0] ? std::optional<std::string>{data_.name_utf8} : std::nullopt; }
private:
    NimbyStation data_;
};

class Signal {
public:
    explicit Signal(const NimbySignal& data) : data_(data) {}
    Id getId() const { return data_.id; }
    Id getTrackId() const { return data_.track_id; }
    double getFraction() const { return data_.track_fraction; }
    std::int32_t getDirection() const { return data_.direction; }
    std::int32_t getKind() const { return data_.kind; }
    // Native Signal_M_forward reverses the stored position for Path signals.
    // Keep getDirection() raw for existing geometric/inspection consumers.
    std::int32_t getForwardDirection() const {
        return data_.kind == NIMBY_SIGNAL_PATH ? -data_.direction : data_.direction;
    }
private:
    NimbySignal data_;
};

class SignalState {
public:
    explicit SignalState(const NimbySignalState& data) : data_(data) {}
    std::optional<std::uint32_t> getExceptionCount() const { return (data_.flags & NIMBY_SIGNAL_FILTER_VALID) ? std::optional<std::uint32_t>{data_.exception_count} : std::nullopt; }
    std::optional<bool> isIgnoredByDefault() const { return (data_.flags & NIMBY_SIGNAL_FILTER_VALID) ? std::optional<bool>{(data_.flags & NIMBY_SIGNAL_FILTER_DEFAULT_IGNORED)!=0} : std::nullopt; }
    Id getSignalId() const { return data_.signal_id; }
    std::optional<std::uint32_t> getAspect() const { return (data_.flags & NIMBY_SIGNAL_ASPECT_VALID) ? std::optional<std::uint32_t>{data_.aspect} : std::nullopt; }
    std::optional<SpecificState> getSpecificState() const { return (data_.flags & NIMBY_SIGNAL_SPECIFIC_STATE_VALID) ? std::optional<SpecificState>{SpecificState{data_.system_utf8, data_.specific_state_utf8}} : std::nullopt; }
    std::optional<std::int32_t> getTextureSelector() const { return (data_.flags & NIMBY_SIGNAL_TEXTURE_STATE_VALID) ? std::optional<std::int32_t>{data_.texture_state} : std::nullopt; }
    bool usesDefaultTextureSelector() const noexcept { return (data_.flags & (NIMBY_SIGNAL_TEXTURE_STATE_VALID | NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT)) == (NIMBY_SIGNAL_TEXTURE_STATE_VALID | NIMBY_SIGNAL_TEXTURE_STATE_DEFAULT); }
private:
    NimbySignalState data_;
};

class TextureReference {
public:
    explicit TextureReference(const NimbySignalTexture& data) : data_(data) {}
    std::string getTexturesId() const { return data_.textures_id_utf8; }
    Id getTexturesHash() const { return data_.textures_hash; }
    Id getFileHash() const { return data_.file_hash; }
    std::int32_t getSelectedIndex() const { return data_.selected_index; }
    std::uint32_t getStateCount() const { return data_.state_count; }
    std::uint32_t getSource() const { return data_.source; }
    std::string getModId() const { return data_.mod_id_utf8; }
    std::string getRelativePath() const { return data_.relative_path_utf8; }
    bool isDefaultSet() const { return (data_.flags & NIMBY_SIGNAL_TEXTURE_DEFAULT_SET) != 0; }
    bool isClamped() const { return (data_.flags & NIMBY_SIGNAL_TEXTURE_CLAMPED) != 0; }
private:
    NimbySignalTexture data_;
};

class SignalTexture {
public:
    explicit SignalTexture(const NimbySignalTexture& data) : data_(data) {}
    Id getSignalId() const { return data_.signal_id; }
    // Vue sans copie de la description complete, valide tant que cet objet vit.
    // Une reference absente ou un nom non termine reste indisponible.
    std::optional<std::string_view> getTexturesIdView() const & noexcept {
        if (!(data_.flags & NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID)) return std::nullopt;
        const auto& name = data_.textures_id_utf8;
        for (std::size_t length = 0; length < sizeof(name); ++length)
            if (!name[length]) return std::string_view{name, length};
        return std::nullopt;
    }
    std::optional<std::string_view> getTexturesIdView() const && = delete;
    std::optional<TextureReference> getReference() const { return (data_.flags & NIMBY_SIGNAL_TEXTURE_REFERENCE_VALID) ? std::optional<TextureReference>{TextureReference{data_}} : std::nullopt; }
    std::optional<std::string> getFilePath() const { return (data_.flags & NIMBY_SIGNAL_TEXTURE_FILE_VALID) ? std::optional<std::string>{data_.file_path_utf8} : std::nullopt; }
private:
    NimbySignalTexture data_;
};

class TrackNode {
public:
    explicit TrackNode(const NimbyTrackNode& data) : data_(data) {}
    Id getId() const { return data_.id; }
    Coordinates getCoordinates() const { return Coordinates{data_.x, data_.y}; }
    std::optional<Id> getLinkAId() const { return detail::reference(data_.link_a); }
    std::optional<Id> getLinkBId() const { return detail::reference(data_.link_b); }
private:
    NimbyTrackNode data_;
};

class TrackJunction {
public:
    explicit TrackJunction(const NimbyTrackJunction& data) : data_(data) {}
    Id getId() const { return data_.branch_track_id; }
    Id getBranchTrackId() const { return data_.branch_track_id; }
    Id getMainTrackId() const { return data_.main_track_id; }
    double getMainFraction() const { return data_.main_fraction; }
    std::int32_t getMainDirection() const { return data_.main_direction; }
    std::int32_t getBranchDirection() const { return data_.branch_direction; }
private:
    NimbyTrackJunction data_;
};

// Ordering is geometric, not a route reservation or a signal aspect decision.
enum class SignalTraceStop { UnobservedConnection, UnknownTrack, InconsistentConnection,
                             AmbiguousConnection, Cycle, TrackLimit, Junction, SignalReached };
struct OrderedSignalSection {
    Id trackId;
    std::int32_t direction;
    std::vector<Signal> orderedSignals;
    double fromFraction = 0, toFraction = 1;
};
struct SignalTrace {
    std::vector<OrderedSignalSection> sections;
    SignalTraceStop stop = SignalTraceStop::TrackLimit;
    Id stoppedAtTrack = 0;
    std::vector<Position> continuations;
};
struct NextSignals {
    std::vector<Signal> nextSignals;
    bool truncated = false;
    bool incomplete = false; // At least one path ended at an unobserved connection.
    std::size_t exploredSections = 0;
};
enum class SignalDirectionConvention { Stored, Forward };
class SignalTopology {
public:
    SignalTopology(std::span<const Signal> signalRows, std::span<const TrackNode> nodes,
                   std::span<const TrackJunction> junctions = {},
                   SignalDirectionConvention directionConvention = SignalDirectionConvention::Stored)
        : directionConvention_(directionConvention) {
        nodes_.reserve(nodes.size());
        signals_.reserve(signalRows.size());
        branches_.reserve(junctions.size());
        attachments_.reserve(junctions.size());
        for (const auto& node : nodes) nodes_.emplace(node.getId(), node);
        for (const auto& signal : signalRows) signals_[signal.getTrackId()].push_back(signal);
        for (const auto& j : junctions) {
            const auto main=nodes_.find(j.getMainTrackId()), branch=nodes_.find(j.getBranchTrackId());
            if(main==nodes_.end()||branch==nodes_.end()||main==branch||
               !std::isfinite(j.getMainFraction())||j.getMainFraction()<0||j.getMainFraction()>1||
               (j.getMainDirection()!=1&&j.getMainDirection()!=-1)||
               (j.getBranchDirection()!=1&&j.getBranchDirection()!=-1))continue;
            const auto& b=branch->second;
            if(j.getBranchDirection()==1 ? (b.getLinkAId()||!b.getLinkBId()) : (b.getLinkBId()||!b.getLinkAId()))continue;
            branches_[j.getMainTrackId()].push_back(j);
            attachments_.emplace(j.getBranchTrackId(),j);
        }
        for(auto& [id, rows]:branches_)std::sort(rows.begin(),rows.end(),[](const auto& a,const auto& b){return a.getId()<b.getId();});
        for (auto& [id, rows] : signals_) std::sort(rows.begin(), rows.end(), [](const Signal& a, const Signal& b) {
            return a.getFraction() != b.getFraction() ? a.getFraction() < b.getFraction() : a.getId() < b.getId();
        });
    }
    // All kinds and both facing directions remain present by default.
    std::vector<Signal> getSignalsForTrack(Id track, std::int32_t direction = 1, bool facingOnly = false) const {
        checkDirection(direction);
        auto it = signals_.find(track);
        if (it == signals_.end()) return {};
        auto rows = it->second;
        if (direction == -1) std::reverse(rows.begin(), rows.end());
        if (facingOnly) std::erase_if(rows, [this,direction](const Signal& s) {
            return (directionConvention_==SignalDirectionConvention::Forward?s.getForwardDirection():s.getDirection()) != direction;
        });
        return rows;
    }
    // Approximate A -> B axis from observed node coordinates; not a curve tangent.
    std::optional<Coordinates> getTrackAxis(Id track) const {
        const auto it = nodes_.find(track); if (it == nodes_.end()) return std::nullopt;
        const auto& node = it->second;
        auto a = nodes_.find(node.getLinkAId().value_or(0)), b = nodes_.find(node.getLinkBId().value_or(0));
        if (a == nodes_.end() && b == nodes_.end()) return std::nullopt;
        const auto from = a == nodes_.end() ? node.getCoordinates() : a->second.getCoordinates();
        const auto to = b == nodes_.end() ? node.getCoordinates() : b->second.getCoordinates();
        const double x = to.x-from.x, y = to.y-from.y, length = std::hypot(x,y);
        if (!std::isfinite(length) || length <= 0) return std::nullopt;
        return Coordinates{x/length,y/length};
    }
    // A single trace stops at a facing junction rather than choosing a route.
    SignalTrace traceFrom(Position start, std::size_t maxTracks = 256, bool facingOnly = false) const {
        return trace(start,maxTracks,facingOnly,std::nullopt);
    }
    // Blocks need only the first facing boundary, not the geometry beyond it.
    // Keep every signal in that section so coincident boundaries stay ambiguous.
    SignalTrace traceToNextSignal(Position start, Id excludeSignal, std::size_t maxTracks = 256) const {
        return trace(start,maxTracks,true,excludeSignal);
    }
private:
    SignalTrace trace(Position start, std::size_t maxTracks, bool facingOnly, std::optional<Id> stopAtSignal) const {
        checkPosition(start);
        SignalTrace result; Cursor cursor{start,false}; std::set<Key> visited;
        for(std::size_t step=0;step<maxTracks;++step){
            result.stoppedAtTrack=cursor.position.getTrackId();
            if(!visited.insert(key(cursor)).second){result.stop=SignalTraceStop::Cycle;return result;}
            auto segment=advance(cursor,facingOnly);
            if(segment.section)result.sections.push_back(std::move(*segment.section));
            if(stopAtSignal && !result.sections.empty() &&
               std::any_of(result.sections.back().orderedSignals.begin(),result.sections.back().orderedSignals.end(),
                   [&](const auto& signal){return signal.getId()!=*stopAtSignal;})){
                result.stop=SignalTraceStop::SignalReached;return result;
            }
            result.stop=segment.stop;
            if(segment.next.size()!=1){
                for(const auto& next:segment.next)result.continuations.push_back(next.position);
                return result;
            }
            cursor=segment.next.front();
        }
        result.stop=SignalTraceStop::TrackLimit;result.stoppedAtTrack=cursor.position.getTrackId();return result;
    }
public:
    // First signals reached on every observed branch, without selecting a train route.
    // Pass the starting signal ID as excludeSignal to look beyond that signal.
    NextSignals findNextSignals(Position start, std::size_t maxSections = 4096,
                                bool facingOnly = false, Id excludeSignal = 0) const {
        checkPosition(start);
        NextSignals result;std::vector<Cursor> pending{{start,false}};std::set<Key> visited;
        std::set<Id> found;
        while(!pending.empty()){
            auto cursor=pending.back();pending.pop_back();
            if(!visited.insert(key(cursor)).second)continue;
            if(result.exploredSections==maxSections){result.truncated=true;break;}
            ++result.exploredSections;
            auto segment=advance(cursor,facingOnly);
            bool hit=false;double fraction=0;
            if(segment.section)for(const auto& signal:segment.section->orderedSignals){
                if(signal.getId()==excludeSignal)continue;
                if(hit&&signal.getFraction()!=fraction)break;
                hit=true;fraction=signal.getFraction();
                if(found.insert(signal.getId()).second)result.nextSignals.push_back(signal);
            }
            if(hit)continue;
            if(segment.next.empty())result.incomplete=true;
            for(const auto& next:segment.next)pending.push_back(next);
        }
        std::sort(result.nextSignals.begin(),result.nextSignals.end(),[](const auto& a,const auto& b){return a.getId()<b.getId();});
        return result;
    }
private:
    struct Cursor { Position position; bool skipStartJunction; };
    using Key=std::tuple<Id,std::int32_t,double,bool>;
    struct Segment {
        std::optional<OrderedSignalSection> section;
        std::vector<Cursor> next;
        SignalTraceStop stop=SignalTraceStop::UnobservedConnection;
    };
    static Key key(const Cursor& c){return {c.position.getTrackId(),c.position.getDirection(),c.position.getFraction(),c.skipStartJunction};}
    static void checkPosition(Position p){
        checkDirection(p.getDirection());
        if(!std::isfinite(p.getFraction())||p.getFraction()<0||p.getFraction()>1)
            throw std::invalid_argument("Signal trace fraction must be in [0,1]");
    }
    Segment advance(const Cursor& cursor,bool facingOnly) const {
        Segment result;
        const auto track=cursor.position.getTrackId();const auto direction=cursor.position.getDirection();
        const auto start=cursor.position.getFraction();
        const auto node=nodes_.find(track);
        if(node==nodes_.end()){result.stop=SignalTraceStop::UnknownTrack;return result;}
        double end=direction==1?1:0;std::vector<TrackJunction> forks;
        const auto branchRows=branches_.find(track);
        if(branchRows!=branches_.end())for(const auto& j:branchRows->second){
            const auto f=j.getMainFraction();
            if(j.getMainDirection()!=direction||(direction==1?f<start:f>start)||
               (cursor.skipStartJunction&&f==start))continue;
            if(direction==1?f<end:f>end){end=f;forks.clear();}
            if(f==end)forks.push_back(j);
        }
        auto rows=getSignalsForTrack(track,direction,facingOnly);
        std::erase_if(rows,[&](const auto& s){const auto f=s.getFraction();return direction==1?(f<start||f>end):(f>start||f<end);});
        result.section=OrderedSignalSection{track,direction,std::move(rows),start,end};
        if(!forks.empty()){
            result.stop=SignalTraceStop::Junction;
            result.next.push_back({Position{track,end,direction},true});
            for(const auto& j:forks)result.next.push_back({Position{j.getBranchTrackId(),j.getBranchDirection()==1?0.:1.,j.getBranchDirection()},false});
            return result;
        }
        const auto nextId=direction==1?node->second.getLinkBId():node->second.getLinkAId();
        if(!nextId){
            const auto a=attachments_.find(track);
            if(a!=attachments_.end()&&direction==-a->second.getBranchDirection()){
                const auto& j=a->second;
                result.next.push_back({Position{j.getMainTrackId(),j.getMainFraction(),-j.getMainDirection()},false});
            }
            return result;
        }
        const auto next=nodes_.find(*nextId);
        if(next==nodes_.end()){result.stop=SignalTraceStop::InconsistentConnection;return result;}
        const bool a=next->second.getLinkAId()==track,b=next->second.getLinkBId()==track;
        if(a==b){result.stop=a?SignalTraceStop::AmbiguousConnection:SignalTraceStop::InconsistentConnection;return result;}
        result.next.push_back({Position{*nextId,a?0.:1.,a?1:-1},false});return result;
    }
    static void checkDirection(std::int32_t direction) {
        if (direction!=1 && direction!=-1) throw std::invalid_argument("Signal order direction must be +1 or -1");
    }
    std::unordered_map<Id,TrackNode> nodes_;
    SignalDirectionConvention directionConvention_;
    std::unordered_map<Id,std::vector<TrackJunction>> branches_;
    std::unordered_map<Id,TrackJunction> attachments_;
    std::unordered_map<Id,std::vector<Signal>> signals_;
};

class Platform {
public:
    explicit Platform(const NimbyPlatform& data) : data_(data),track_ids_{data.track_id} {}
    Platform(const Platform& first,std::vector<Id> tracks) : data_(first.data_),track_ids_(std::move(tracks)) {}
    // Representative section only. Use getTrackIds()/containsTrack() for a whole platform.
    Id getTrackId() const { return data_.track_id; }
    const std::vector<Id>& getTrackIds() const noexcept { return track_ids_; }
    bool containsTrack(Id id) const { return std::find(track_ids_.begin(),track_ids_.end(),id)!=track_ids_.end(); }
    Id getStationId() const { return data_.station_id; }
    std::optional<std::string> getName() const { return data_.flags&NIMBY_PLATFORM_NAME_VALID?std::optional<std::string>{data_.name_utf8}:std::nullopt; }
private:
    NimbyPlatform data_;
    std::vector<Id> track_ids_;
};
struct PlatformOccupation {
    Platform platform;
    // nullopt means unknown; an available empty vector means no trains observed.
    std::optional<std::vector<Train>> occupying_trains, reserving_trains;
    std::optional<bool> isOccupied() const {
        return occupying_trains?std::optional<bool>{!occupying_trains->empty()}:std::nullopt;
    }
};

class TrackUsage {
public:
    explicit TrackUsage(const NimbyTrackUsage& data) : data_(data) {}
    Id getTrainId() const { return data_.train_id; }
    Id getTrackId() const { return data_.track_id; }
    double getBeginFraction() const { return data_.fraction_begin; }
    double getEndFraction() const { return data_.fraction_end; }
private:
    NimbyTrackUsage data_;
};

namespace detail {
// Immutable snapshots build relation indexes only when a caller requests the
// relation. Repeated per-track/per-train queries then visit matching rows, not
// the whole world. Concurrent readers share the completed index safely.
class RelationIndex {
    mutable std::once_flag once_;
    mutable std::unordered_map<Id,std::vector<size_t>> groups_;
public:
    template<class T,class Key>
    std::vector<T> select(const std::vector<T>& rows,Id id,Key key) const {
        std::call_once(once_,[&]{
            std::unordered_map<Id,std::vector<size_t>> groups;
            for(size_t i=0;i<rows.size();++i)if(const auto group=key(rows[i]))groups[group].push_back(i);
            groups_=std::move(groups);
        });
        std::vector<T> result;
        const auto found=groups_.find(id);
        if(found!=groups_.end()){
            result.reserve(found->second.size());
            for(auto i:found->second)result.push_back(rows[i]);
        }
        return result;
    }
};
template<class T> struct Table {
    std::vector<T> rows;
    std::unordered_map<Id, std::size_t> index;
    std::optional<T> find(Id id) const {
        const auto it = index.find(id);
        return it == index.end() ? std::nullopt : std::optional<T>{rows[it->second]};
    }
};
template<class Raw, class Copy>
std::optional<std::vector<Raw>> copyRecords(Copy copy, const char* operation) {
    std::uint32_t count{};
    auto status = copy(static_cast<Raw*>(nullptr), 0, &count);
    if (status == NIMBY_DATA_UNAVAILABLE) return std::nullopt;
    check(status, operation);
    std::vector<Raw> rows(count);
    if (count) check(copy(rows.data(), count, &count), operation);
    return rows;
}
template<class T, class Raw, class Copy, class Key>
Table<T> table(Copy copy, Key key, const char* operation) {
    auto rows = copyRecords<Raw>(copy, operation);
    if (!rows) check(NIMBY_DATA_UNAVAILABLE, operation);
    Table<T> result;
    result.rows.reserve(rows->size());
    // Counts are known: avoid repeated rehashing during the freshness budget.
    result.index.reserve(rows->size());
    for (const auto& row : *rows) {
        result.index.emplace(key(row), result.rows.size());
        result.rows.emplace_back(row);
    }
    return result;
}
struct NativeSnapshot {
    NimbySnapshot value{};
    NativeSnapshot() = default;
    NativeSnapshot(const NativeSnapshot&) = delete;
    ~NativeSnapshot() { if (value) NimbyInternal_ReleaseSnapshot(value); }
};
}

namespace detail { class ObservationSession; }
struct GameSession {
    uint64_t generation=0;
    std::string worldId; // World lineage; Save As may preserve it.
    bool operator==(const GameSession&) const = default;
};
class Snapshot {
public:
    using Ptr = std::shared_ptr<const Snapshot>;
    const std::optional<GameSession>& getGameSession() const noexcept {return gameSession_;}
    std::uint32_t getProcessId() const noexcept { return info_.process_id; }
    std::chrono::system_clock::time_point getCapturedAt() const {
        return std::chrono::system_clock::time_point{Milliseconds{static_cast<std::int64_t>(info_.captured_unix_ms)}};
    }
    Milliseconds getAge() const {
        return std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - captured_);
    }
    bool isOlderThan(Milliseconds age) const { return getAge() > age; }
    std::string getGameSha256() const { return info_.game_sha256; }
    std::optional<SimulationClock> getSimulationClock() const { return clock_; }
    std::span<const Train> getAllTrains() const noexcept { return trains_.rows; }
    std::optional<Train> getTrainById(Id id) const { return trains_.find(id); }
    std::optional<TrainDetails> getTrainDetailsById(Id id) const { return details_.find(id); }
    std::optional<TrainMetadata> getTrainMetadataById(Id id) const {return trainMetadata_.find(id);}
    std::optional<std::span<const TrainVehicle>> getAllTrainVehicles() const noexcept {
        return vehicles_?std::optional<std::span<const TrainVehicle>>{*vehicles_}:std::nullopt;
    }
    std::optional<std::span<const VehicleModel>> getAllVehicleModels() const noexcept {
        return modelsAvailable_?std::optional<std::span<const VehicleModel>>{models_.rows}:std::nullopt;
    }
    std::optional<VehicleModel> getVehicleModelById(Id id) const {return models_.find(id);}
    std::optional<std::span<const Line>> getAllLines() const noexcept {
        return linesAvailable_?std::optional<std::span<const Line>>{lines_.rows}:std::nullopt;
    }
    std::optional<Line> getLineById(Id id) const {return lines_.find(id);}
    std::optional<std::span<const Tag>> getAllTags() const noexcept {
        return tagsAvailable_?std::optional<std::span<const Tag>>{tags_.rows}:std::nullopt;
    }
    std::optional<Tag> getTagById(Id id) const {return tags_.find(id);}
    std::optional<std::span<const Id>> getTagIdsForObject(Id id) const noexcept {
        const auto found=objectTags_.find(id);
        return found==objectTags_.end()?std::nullopt:std::optional<std::span<const Id>>{found->second};
    }
    std::optional<std::span<const LineStop>> getLineStopsForTrain(Id id) const {
        const auto it=line_stops_.find(id);
        return it==line_stops_.end()?std::nullopt:std::optional<std::span<const LineStop>>{it->second};
    }
    std::span<const TrainService> getAllTrainServices() const noexcept { return services_.rows; }
    std::optional<TrainService> getTrainServiceById(Id id) const { return services_.find(id); }
    std::span<const Track> getAllTracks() const noexcept { return tracks_.rows; }
    std::optional<Track> getTrackById(Id id) const { return tracks_.find(id); }
    std::span<const Station> getAllStations() const noexcept { return stations_.rows; }
    std::optional<Station> getStationById(Id id) const { return stations_.find(id); }
    std::span<const Signal> getAllSignals() const noexcept { return signals_.rows; }
    std::optional<Signal> getSignalById(Id id) const { return signals_.find(id); }
    std::span<const TrackJunction> getAllTrackJunctions() const noexcept { return junctions_.rows; }
    std::span<const TrackNode> getAllTrackNodes() const noexcept { return nodes_.rows; }
    const std::optional<std::vector<NimbyTrackMetric>>& getTrackMetrics()const noexcept{return trackMetrics_;}
    std::optional<TrackNode> getTrackNodeById(Id id) const { return nodes_.find(id); }
    std::span<const SignalState> getAllSignalStates() const noexcept { return states_.rows; }
    std::optional<SignalState> getSignalStateById(Id id) const { return states_.find(id); }
    std::span<const SignalTexture> getAllSignalTextures() const noexcept { return textures_.rows; }
    std::optional<SignalTexture> getSignalTextureById(Id id) const { return textures_.find(id); }
    // Experimental native extension settings, owned by this immutable snapshot.
    // Duplicate type names from distinct scripts are ambiguous, never first-wins.
    SignalSettings getSignalSettings(Id id,std::string_view typeName) const {
        if(typeName.empty())return {};
        const auto state=extension_available_.find(id);
        if(state==extension_available_.end()||!state->second)return {};
        SignalSettings result;result.status=SettingsStatus::Absent;
        const auto records=extension_fields_.find(id);
        if(records==extension_fields_.end())return result;
        size_t matches=0;
        for(const auto& row:records->second)if(typeName==row.type_name && !row.field_name[0])++matches;
        if(matches>1)return {};
        if(!matches)return result;
        result.status=SettingsStatus::Present;
        for(const auto& row:records->second)if(typeName==row.type_name && row.field_name[0]){
            const auto value=row.boolean_valid?std::optional<bool>(row.boolean_value!=0):std::nullopt;
            if(!result.booleans.emplace(row.field_name,value).second)return {};
        }
        return result;
    }

    std::optional<Track> getTrackForTrain(Id id) const {
        auto train = getTrainById(id);
        auto position = train ? train->getPosition() : std::nullopt;
        return position ? getTrackById(position->getTrackId()) : std::nullopt;
    }
    std::optional<Station> getStationForTrack(Id id) const {
        auto track = getTrackById(id);
        auto station = track ? track->getStationId() : std::nullopt;
        return station ? getStationById(*station) : std::nullopt;
    }
    std::vector<Signal> getSignalsForTrack(Id id) const {
        auto rows=signalsByTrack_.select(signals_.rows,id,[](const Signal& v) { return v.getTrackId(); });
        std::sort(rows.begin(),rows.end(),[](const Signal& a,const Signal& b) {
            return a.getFraction()!=b.getFraction() ? a.getFraction()<b.getFraction() : a.getId()<b.getId();
        });
        return rows;
    }
    SignalTopology getSignalTopology() const { return SignalTopology{signals_.rows,nodes_.rows,junctions_.rows}; }
    std::vector<Train> getTrainsOnTrack(Id id) const {
        return trainsByTrack_.select(trains_.rows,id,[](const Train& v) {
            auto position = v.getPosition(); return position ? position->getTrackId() : Id{};
        });
    }
    // Raw section detail; use getPlatformOccupationsForStation for grouped platforms.
    std::optional<std::vector<PlatformOccupation>> getPlatformSectionOccupationsForStation(Id id) const {
        if(!getStationById(id))return std::nullopt;
        std::vector<PlatformOccupation> result;
        const auto it=station_platforms_.find(id);
        if(it==station_platforms_.end())return result;
        auto trainsFor=[&](const auto& usage,const auto& index,Id track)->std::optional<std::vector<Train>>{
            if(!usage)return std::nullopt;
            std::vector<Train> trains;std::unordered_map<Id,bool> seen;
            const auto found=index.find(track);if(found==index.end())return trains;
            for(const auto trainId:found->second)if(seen.emplace(trainId,true).second){
                auto train=getTrainById(trainId);if(!train)return std::nullopt;
                trains.push_back(*train);
            }
            return trains;
        };
        for(const auto& platform:it->second)result.push_back({platform,
            trainsFor(occupations_,occupant_ids_,platform.getTrackId()),trainsFor(reservations_,reservation_ids_,platform.getTrackId())});
        return result;
    }
    // Same station ID + exact known platform name. Unknown/empty names stay separate.
    std::optional<std::vector<PlatformOccupation>> getPlatformOccupationsForStation(Id id) const {
        auto sections=getPlatformSectionOccupationsForStation(id);
        if(!sections)return std::nullopt;
        std::vector<PlatformOccupation> result;
        std::unordered_map<std::string,size_t> names;
        auto merge=[](auto& into,const auto& from){
            if(!into||!from){into.reset();return;}
            for(const auto& train:*from)if(std::none_of(into->begin(),into->end(),[&](const Train& existing){return existing.getId()==train.getId();}))
                into->push_back(train);
        };
        for(auto& section:*sections){
            const auto name=section.platform.getName();
            if(!name||name->empty()){result.push_back(std::move(section));continue;}
            const auto [found,inserted]=names.emplace(*name,result.size());
            if(inserted){result.push_back(std::move(section));continue;}
            auto& group=result[found->second];
            auto tracks=group.platform.getTrackIds();
            if(std::find(tracks.begin(),tracks.end(),section.platform.getTrackId())==tracks.end())tracks.push_back(section.platform.getTrackId());
            group.platform=Platform{group.platform,std::move(tracks)};
            merge(group.occupying_trains,section.occupying_trains);
            merge(group.reserving_trains,section.reserving_trains);
        }
        return result;
    }
    std::vector<Track> getTracksForStation(Id id) const {
        return tracksByStation_.select(tracks_.rows,id,[](const Track& v) { return v.getStationId().value_or(0); });
    }
    std::optional<std::vector<Id>> getPathTrackIdsForTrain(Id id) const {
        const auto it = paths_.find(id);
        return it == paths_.end() ? std::nullopt : std::optional<std::vector<Id>>{it->second};
    }
    std::optional<std::span<const TrackUsage>> getAllReservations() const {
        return reservations_ ? std::optional<std::span<const TrackUsage>>{*reservations_} : std::nullopt;
    }
    std::optional<std::span<const TrackUsage>> getAllOccupations() const {
        return occupations_ ? std::optional<std::span<const TrackUsage>>{*occupations_} : std::nullopt;
    }
    std::optional<std::vector<TrackUsage>> getReservationsForTrain(Id id) const {
        const auto& rows = reservations_;
        if (!rows) return std::nullopt;
        return reservationsByTrain_.select(*rows,id,[](const TrackUsage& v) { return v.getTrainId(); });
    }
    std::optional<std::vector<TrackUsage>> getReservationsForTrack(Id id) const {
        const auto& rows = reservations_;
        if (!rows) return std::nullopt;
        return reservationsByTrack_.select(*rows,id,[](const TrackUsage& v) { return v.getTrackId(); });
    }
    std::optional<std::vector<TrackUsage>> getOccupationsForTrain(Id id) const {
        const auto& rows = occupations_;
        if (!rows) return std::nullopt;
        return occupationsByTrain_.select(*rows,id,[](const TrackUsage& v) { return v.getTrainId(); });
    }
    std::optional<std::vector<TrackUsage>> getOccupationsForTrack(Id id) const {
        const auto& rows = occupations_;
        if (!rows) return std::nullopt;
        return occupationsByTrack_.select(*rows,id,[](const TrackUsage& v) { return v.getTrackId(); });
    }

private:
    friend class detail::ObservationSession;
    Snapshot() = default;
    std::optional<GameSession> gameSession_;
    NimbySnapshotInfo info_{};
    std::optional<SimulationClock> clock_;
    std::chrono::steady_clock::time_point captured_;
    detail::Table<Train> trains_;
    detail::Table<TrainService> services_;
    detail::Table<TrainDetails> details_;
    detail::Table<TrainMetadata> trainMetadata_;
    std::optional<std::vector<TrainVehicle>> vehicles_;
    detail::Table<VehicleModel> models_;
    bool modelsAvailable_=false;
    detail::Table<Line> lines_;
    detail::Table<Tag> tags_;
    bool linesAvailable_=false,tagsAvailable_=false;
    std::unordered_map<Id,std::vector<Id>> objectTags_;
    std::unordered_map<Id,std::vector<LineStop>> line_stops_;
    detail::Table<Track> tracks_;
    std::unordered_map<Id,std::vector<Platform>> station_platforms_;
    detail::Table<Station> stations_;
    detail::Table<Signal> signals_;
    detail::Table<TrackNode> nodes_;
    std::optional<std::vector<NimbyTrackMetric>> trackMetrics_;
    detail::Table<TrackJunction> junctions_;
    detail::Table<SignalState> states_;
    detail::Table<SignalTexture> textures_;
    std::unordered_map<Id,bool> extension_available_;
    std::unordered_map<Id,std::vector<NimbySignalExtensionField>> extension_fields_;
    std::unordered_map<Id, std::vector<Id>> paths_;
    std::optional<std::vector<TrackUsage>> reservations_, occupations_;
    std::unordered_map<Id,std::vector<Id>> occupant_ids_,reservation_ids_;
    detail::RelationIndex signalsByTrack_,trainsByTrack_,tracksByStation_;
    detail::RelationIndex reservationsByTrain_,reservationsByTrack_,occupationsByTrain_,occupationsByTrack_;
    static Ptr capture(NimbySession session, SnapshotScope scope,const char* textureSet=nullptr,std::span<const NimbySignalCaptureScope> signalScopes={},uint32_t trainFlags=NIMBY_TRAIN_DATA_ALL) {
        // Age includes the native capture, registry contention and copy-out.
        // A slow capture must not be presented as a brand-new observation.
        const auto started=std::chrono::steady_clock::now();
        detail::NativeSnapshot native;
        uint32_t stage{};
        const auto captureStatus=scope==SnapshotScope::Session ? NimbyInternal_CaptureSessionSnapshot(session,&native.value,&stage) :
            scope==SnapshotScope::NetworkTopology ? NimbyInternal_CaptureNetworkSnapshotDiagnostic(session,&native.value,&stage) :
            scope==SnapshotScope::TrainData ? NimbyInternal_CaptureTrainDataSnapshotWithOptions(session,trainFlags,&native.value,&stage) :
            !signalScopes.empty() ? NimbyInternal_CaptureSignallingScope(session,signalScopes.data(),static_cast<uint32_t>(signalScopes.size()),&native.value,&stage) :
            textureSet ? NimbyInternal_CaptureSignallingFor(session,textureSet,&native.value,&stage) : scope==SnapshotScope::Signalling
            ? NimbyInternal_CaptureSignallingSnapshot(session, &native.value, &stage)
            : NimbyInternal_CaptureSnapshotDiagnostic(session, &native.value, &stage);
        constexpr const char* stages[]{"CaptureSnapshot", "CaptureSnapshot/live roots",
            "CaptureSnapshot/trains", "CaptureSnapshot/network", "CaptureSnapshot/roots after network",
            "CaptureSnapshot/roots after occupations", "CaptureSnapshot/train track reference",
            "CaptureSnapshot/roots after textures"};
        detail::check(captureStatus,stages[stage<std::size(stages)?stage:0]);
        auto result = std::shared_ptr<Snapshot>(new Snapshot);
        result->captured_ = started;
        result->info_.struct_size = sizeof result->info_;
        detail::check(NimbyInternal_GetSnapshotInfo(native.value, &result->info_), "GetSnapshotInfo");
        NimbyGameSession game{};game.struct_size=sizeof(game);
        const auto gameStatus=NimbyInternal_GetGameSession(native.value,&game);
        if(gameStatus==NIMBY_OK){
            GameSession value;value.generation=game.generation;
            constexpr char hex[]="0123456789abcdef";
            for(auto byte:game.world_value){value.worldId+=hex[byte>>4];value.worldId+=hex[byte&15];}
            result->gameSession_=std::move(value);
        }else if(gameStatus!=NIMBY_DATA_UNAVAILABLE)detail::check(gameStatus,"GetGameSession");
        NimbySimulationClock clock{};clock.struct_size=sizeof clock;
        const auto clockStatus=NimbyInternal_GetSimulationClock(native.value,&clock);
        if(clockStatus==NIMBY_OK)result->clock_=SimulationClock{clock};
        else if(clockStatus!=NIMBY_DATA_UNAVAILABLE)detail::check(clockStatus,"GetSimulationClock");
        if(scope==SnapshotScope::Session)return result;
        if(scope==SnapshotScope::NetworkTopology){
            // The tool contract contains these four tables only. Do not copy
            // unrelated empty tables or build train/display indexes for it.
            result->signals_=detail::table<Signal,NimbySignal>(
                [&](auto* out,auto cap,auto* count){return NimbyInternal_CopySignals(native.value,out,cap,count);},
                [](const NimbySignal& row){return row.id;},"CopySignals");
            result->nodes_=detail::table<TrackNode,NimbyTrackNode>(
                [&](auto* out,auto cap,auto* count){return NimbyInternal_CopyTrackNodes(native.value,out,cap,count);},
                [](const NimbyTrackNode& row){return row.id;},"CopyTrackNodes");
            result->junctions_=detail::table<TrackJunction,NimbyTrackJunction>(
                [&](auto* out,auto cap,auto* count){return NimbyInternal_CopyTrackJunctions(native.value,out,cap,count);},
                [](const NimbyTrackJunction& row){return row.branch_track_id;},"CopyTrackJunctions");
            result->trackMetrics_=detail::copyRecords<NimbyTrackMetric>(
                [&](auto* out,auto cap,auto* count){return NimbyInternal_CopyTrackMetrics(native.value,out,cap,count);},"CopyTrackMetrics");
            return result;
        }
        result->trains_ = detail::table<Train, NimbyTrain>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrains(native.value, out, capacity, count); },
            [](const NimbyTrain& row) { return row.id; }, "CopyTrains");
        if(scope!=SnapshotScope::TrainData||(trainFlags&(NIMBY_TRAIN_DATA_SERVICE|NIMBY_TRAIN_DATA_TIMETABLES|NIMBY_TRAIN_DATA_LOCATIONS)))
        result->services_ = detail::table<TrainService, NimbyTrainService>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrainServices(native.value, out, capacity, count); },
            [](const NimbyTrainService& row) { return row.train_id; }, "CopyTrainServices");
        if(scope!=SnapshotScope::TrainData||(trainFlags&(NIMBY_TRAIN_DATA_SERVICE|NIMBY_TRAIN_DATA_TIMETABLES|NIMBY_TRAIN_DATA_PASSENGERS)))
        result->details_ = detail::table<TrainDetails, NimbyTrainDetails>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrainDetails(native.value, out, capacity, count); },
            [](const NimbyTrainDetails& row) { return row.train_id; }, "CopyTrainDetails");
        if(scope==SnapshotScope::TrainData){
            if(trainFlags&(NIMBY_TRAIN_DATA_SERVICE|NIMBY_TRAIN_DATA_TIMETABLES|NIMBY_TRAIN_DATA_CHARACTERISTICS|NIMBY_TRAIN_DATA_COMPOSITION))
            result->trainMetadata_=detail::table<TrainMetadata,NimbyTrainMetadata>(
                [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopyTrainMetadata(native.value,out,capacity,count);},
                [](const NimbyTrainMetadata& row){return row.train_id;},"CopyTrainMetadata");
            if(trainFlags&(NIMBY_TRAIN_DATA_LINES|NIMBY_TRAIN_DATA_TAGS)){
            const auto lines=detail::copyRecords<NimbyLineMetadata>(
                [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopyLines(native.value,out,capacity,count);},"CopyLines");
            if(lines){
                result->linesAvailable_=true;result->lines_.rows.reserve(lines->size());result->lines_.index.reserve(lines->size());
                for(const auto& row:*lines){result->lines_.index.emplace(row.line_id,result->lines_.rows.size());result->lines_.rows.emplace_back(row);}
            }
            }
            if(trainFlags&NIMBY_TRAIN_DATA_TAGS){
            const auto tags=detail::copyRecords<NimbyTag>(
                [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopyTags(native.value,out,capacity,count);},"CopyTags");
            if(tags){
                result->tagsAvailable_=true;result->tags_.rows.reserve(tags->size());result->tags_.index.reserve(tags->size());
                for(const auto& row:*tags){result->tags_.index.emplace(row.tag_id,result->tags_.rows.size());result->tags_.rows.emplace_back(row);}
            }
            const auto states=detail::copyRecords<NimbyObjectTagsState>(
                [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopyObjectTagsStates(native.value,out,capacity,count);},"CopyObjectTagsStates");
            const auto links=detail::copyRecords<NimbyObjectTag>(
                [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopyObjectTags(native.value,out,capacity,count);},"CopyObjectTags");
            if(states&&links){
                result->objectTags_.reserve(states->size());
                for(const auto& row:*states)if(row.available==1)result->objectTags_.try_emplace(row.object_id);
                for(const auto& row:*links){const auto found=result->objectTags_.find(row.object_id);if(found!=result->objectTags_.end())found->second.push_back(row.tag_id);}
            }
            }
            if(trainFlags&NIMBY_TRAIN_DATA_COMPOSITION){
                const auto vehicles=detail::copyRecords<NimbyTrainVehicle>(
                    [&](auto* out,auto cap,auto* count){return NimbyInternal_CopyTrainVehicles(native.value,out,cap,count);},"CopyTrainVehicles");
                if(vehicles){
                    result->vehicles_.emplace();result->vehicles_->reserve(vehicles->size());
                    for(const auto& row:*vehicles)result->vehicles_->emplace_back(row);
                }
                const auto models=detail::copyRecords<NimbyVehicleModel>(
                    [&](auto* out,auto cap,auto* count){return NimbyInternal_CopyVehicleModels(native.value,out,cap,count);},"CopyVehicleModels");
                if(models){
                    result->modelsAvailable_=true;result->models_.rows.reserve(models->size());result->models_.index.reserve(models->size());
                    for(const auto& row:*models){result->models_.index.emplace(row.model_id,result->models_.rows.size());result->models_.rows.emplace_back(row);}
                }
            }
        }
        if(scope!=SnapshotScope::TrainData||(trainFlags&(NIMBY_TRAIN_DATA_LOCATIONS|NIMBY_TRAIN_DATA_TIMETABLES))){
        result->tracks_ = detail::table<Track, NimbyTrack>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTracks(native.value, out, capacity, count); },
            [](const NimbyTrack& row) { return row.id; }, "CopyTracks");
        result->stations_ = detail::table<Station, NimbyStation>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyStations(native.value, out, capacity, count); },
            [](const NimbyStation& row) { return row.id; }, "CopyStations");
        }
        if(scope==SnapshotScope::TrainData){
            if(trainFlags&NIMBY_TRAIN_DATA_TIMETABLES)for(const auto& train:result->trains_.rows){
                auto stops=detail::copyRecords<NimbyLineStop>([&](auto* out,auto cap,auto* count){
                    return NimbyInternal_CopyTrainLineStops(native.value,train.getId(),out,cap,count);
                },"CopyTrainLineStops");
                if(stops){auto& rows=result->line_stops_[train.getId()];rows.reserve(stops->size());for(const auto& row:*stops)rows.emplace_back(row);}
            }
            return result;
        }
        auto platforms=detail::copyRecords<NimbyPlatform>([&](auto* out,auto capacity,auto* count){
            return NimbyInternal_CopyPlatforms(native.value,out,capacity,count);
        },"CopyPlatforms");
        if(!platforms)throw Exception({NIMBY_DATA_UNAVAILABLE,"CopyPlatforms","Platform catalog unavailable"});
        for(const auto& row:*platforms)result->station_platforms_[row.station_id].emplace_back(row);
        result->signals_ = detail::table<Signal, NimbySignal>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopySignals(native.value, out, capacity, count); },
            [](const NimbySignal& row) { return row.id; }, "CopySignals");
        result->nodes_ = detail::table<TrackNode, NimbyTrackNode>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrackNodes(native.value, out, capacity, count); },
            [](const NimbyTrackNode& row) { return row.id; }, "CopyTrackNodes");
        if(scope==SnapshotScope::Complete){
            uint32_t count{};
            const auto status=NimbyInternal_CopyTrackMetrics(native.value,nullptr,0,&count);
            if(status==NIMBY_OK){
                result->trackMetrics_=detail::copyRecords<NimbyTrackMetric>(
                    [&](auto* out,auto capacity,auto* copied){return NimbyInternal_CopyTrackMetrics(native.value,out,capacity,copied);},"CopyTrackMetrics");
            }else if(status!=NIMBY_DATA_UNAVAILABLE)detail::check(status,"CopyTrackMetrics");
        }
        result->junctions_ = detail::table<TrackJunction, NimbyTrackJunction>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrackJunctions(native.value, out, capacity, count); },
            [](const NimbyTrackJunction& row) { return row.branch_track_id; }, "CopyTrackJunctions");
        result->states_ = detail::table<SignalState, NimbySignalState>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopySignalStates(native.value, out, capacity, count); },
            [](const NimbySignalState& row) { return row.signal_id; }, "CopySignalStates");
        result->textures_ = detail::table<SignalTexture, NimbySignalTexture>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopySignalTextures(native.value, out, capacity, count); },
            [](const NimbySignalTexture& row) { return row.signal_id; }, "CopySignalTextures");

        const auto extensionStates=detail::copyRecords<NimbySignalExtensionsState>(
            [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopySignalExtensionsStates(native.value,out,capacity,count);},"CopySignalExtensionsStates");
        const auto extensionFields=detail::copyRecords<NimbySignalExtensionField>(
            [&](auto* out,auto capacity,auto* count){return NimbyInternal_CopySignalExtensionFields(native.value,out,capacity,count);},"CopySignalExtensionFields");
        if(extensionStates && extensionFields){
            for(const auto& row:*extensionStates)result->extension_available_.emplace(row.signal_id,row.available==1);
            for(const auto& row:*extensionFields)result->extension_fields_[row.signal_id].push_back(row);
        }
        if(scope==SnapshotScope::Complete)for (const auto& train : result->trains_.rows) {
            auto stops=detail::copyRecords<NimbyLineStop>([&](auto* out,auto capacity,auto* count){
                return NimbyInternal_CopyTrainLineStops(native.value,train.getId(),out,capacity,count);
            },"CopyTrainLineStops");
            if(stops){auto& rows=result->line_stops_[train.getId()];rows.reserve(stops->size());for(const auto& row:*stops)rows.emplace_back(row);}
            auto path = detail::copyRecords<Id>(
                [&](auto* out, auto capacity, auto* count) {
                    return NimbyInternal_CopyTrainPathTracks(native.value, train.getId(), out, capacity, count);
                }, "CopyTrainPathTracks");
            if (path) result->paths_.emplace(train.getId(), std::move(*path));
        }
        {
            auto rows = detail::copyRecords<NimbyTrackUsage>(
                [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrackReservations(native.value, out, capacity, count); }, "CopyTrackReservations");
            if (rows) {
                result->reservations_.emplace();
                result->reservations_->reserve(rows->size());
                for (const auto& row : *rows) result->reservations_->emplace_back(row);
                for (const auto& row : *rows) result->reservation_ids_[row.track_id].push_back(row.train_id);
            }
        }
        {
            auto rows = detail::copyRecords<NimbyTrackUsage>(
                [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrackOccupations(native.value, out, capacity, count); }, "CopyTrackOccupations");
            if (rows) {
                result->occupations_.emplace();
                result->occupations_->reserve(rows->size());
                for (const auto& row : *rows) result->occupations_->emplace_back(row);
                for (const auto& row : *rows) result->occupant_ids_[row.track_id].push_back(row.train_id);
            }
        }
        return result;
    }
};

} // namespace nimby
