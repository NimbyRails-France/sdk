#pragma once
// Public C++20 API for NimbyRailsFranceSDK. detail/ is implementation-only.
#include "detail/observation.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nimby {
using Id = std::uint64_t;
using Milliseconds = std::chrono::milliseconds;

enum class ErrorCode : std::uint32_t {
    Ok = 0, InvalidArgument = 1, IoError = 2, InvalidBinary = 3,
    AlreadyInitialized = 4, HooksUnavailable = 5, InternalError = 6,
    UnsupportedGame = 7, DataUnavailable = 8, InvalidHandle = 9,
    BufferTooSmall = 10, ProcessExited = 11, ResourceLimit = 12
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
        version.major != 0 || version.minor != 7)
        detail::check(NIMBY_INVALID_ARGUMENT, "NimbyRailsFranceSDK 0.7.x required");
    return {version.major, version.minor, version.patch, version.abi_version};
}
struct Coordinates { double x, y; };
struct SpecificState { std::string system, state; };

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
        return valid(NIMBY_SERVICE_STATE_VALID) ? std::optional<bool>{(data_.motion_flags & NIMBY_MOTION_HIDDEN) != 0} : std::nullopt;
    }
    std::optional<bool> isOnNetwork() const {
        return valid(NIMBY_SERVICE_STATE_VALID) ? std::optional<bool>{(data_.motion_flags & (NIMBY_MOTION_PRESENCE | NIMBY_MOTION_DRIVE | NIMBY_MOTION_HIDDEN)) != 0} : std::nullopt;
    }
    std::optional<uint32_t> getAlert() const { return valid(NIMBY_SERVICE_STATE_VALID)?std::optional<uint32_t>{data_.alert}:std::nullopt; }
    std::optional<Id> getLocationTrackId() const { return valid(NIMBY_SERVICE_LOCATION_VALID)?detail::reference(data_.location_track_id):std::nullopt; }
    std::optional<Id> getLocationStationId() const { return valid(NIMBY_SERVICE_LOCATION_VALID)?detail::reference(data_.location_station_id):std::nullopt; }
    std::optional<Id> getLineId() const { return valid(NIMBY_SERVICE_RUN_VALID)?detail::reference(data_.line_id):std::nullopt; }
    std::optional<std::string> getLineName() const { return valid(NIMBY_SERVICE_LINE_VALID)&&data_.line_name_utf8[0]?std::optional<std::string>{data_.line_name_utf8}:std::nullopt; }
    std::optional<Id> getStopStationId() const { return valid(NIMBY_SERVICE_STOP_VALID)?detail::reference(data_.stop_station_id):std::nullopt; }
    std::optional<Id> getStopTrackId() const { return valid(NIMBY_SERVICE_STOP_VALID)?detail::reference(data_.stop_track_id):std::nullopt; }
    std::optional<int32_t> getStopIndex() const { return valid(NIMBY_SERVICE_RUN_VALID)?std::optional<int32_t>{data_.stop_index}:std::nullopt; }
    std::optional<int64_t> getGameTimeUs() const { return valid(NIMBY_SERVICE_CLOCK_VALID)?std::optional<int64_t>{data_.game_time_us}:std::nullopt; }
    std::optional<int64_t> getDepartureTimeUs() const { return valid(NIMBY_SERVICE_DEPARTURE_VALID)?std::optional<int64_t>{data_.departure_time_us}:std::nullopt; }
    std::optional<int64_t> getArrivalTimeUs() const { return valid(NIMBY_SERVICE_ARRIVAL_VALID)?std::optional<int64_t>{data_.arrival_time_us}:std::nullopt; }
    std::optional<int64_t> getGameCalendarSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_CLOCK_VALID)?std::optional<int64_t>{data_.game_epoch_seconds+data_.game_time_us/1000000}:std::nullopt; }
    std::optional<int64_t> getDepartureCalendarSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_DEPARTURE_VALID)?std::optional<int64_t>{data_.game_epoch_seconds+data_.departure_time_us/1000000}:std::nullopt; }
    std::optional<int64_t> getArrivalCalendarSeconds() const { return valid(NIMBY_SERVICE_CALENDAR_VALID|NIMBY_SERVICE_ARRIVAL_VALID)?std::optional<int64_t>{data_.game_epoch_seconds+data_.arrival_time_us/1000000}:std::nullopt; }
    std::optional<double> getDepartureRemainingSeconds() const { return valid(NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_DEPARTURE_VALID)?std::optional<double>{data_.departure_remaining_seconds}:std::nullopt; }
    std::optional<double> getArrivalRemainingSeconds() const { return valid(NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_ARRIVAL_VALID)?std::optional<double>{data_.arrival_remaining_seconds}:std::nullopt; }
    std::optional<double> getDispatchRemainingSeconds() const { return valid(NIMBY_SERVICE_CLOCK_VALID|NIMBY_SERVICE_COOLDOWN_VALID)?std::optional<double>{data_.dispatch_remaining_seconds}:std::nullopt; }
private:
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
private:
    NimbySignal data_;
};

class SignalState {
public:
    explicit SignalState(const NimbySignalState& data) : data_(data) {}
    Id getSignalId() const { return data_.signal_id; }
    std::optional<std::uint32_t> getAspect() const { return (data_.flags & NIMBY_SIGNAL_ASPECT_VALID) ? std::optional<std::uint32_t>{data_.aspect} : std::nullopt; }
    std::optional<SpecificState> getSpecificState() const { return (data_.flags & NIMBY_SIGNAL_SPECIFIC_STATE_VALID) ? std::optional<SpecificState>{SpecificState{data_.system_utf8, data_.specific_state_utf8}} : std::nullopt; }
    std::optional<std::int32_t> getTextureSelector() const { return (data_.flags & NIMBY_SIGNAL_TEXTURE_STATE_VALID) ? std::optional<std::int32_t>{data_.texture_state} : std::nullopt; }
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

class Platform {
public:
    explicit Platform(const NimbyPlatform& data) : data_(data) {}
    Id getTrackId() const { return data_.track_id; }
    Id getStationId() const { return data_.station_id; }
    std::optional<std::string> getName() const { return data_.flags&NIMBY_PLATFORM_NAME_VALID?std::optional<std::string>{data_.name_utf8}:std::nullopt; }
private:
    NimbyPlatform data_;
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

class Client;
class Snapshot {
public:
    using Ptr = std::shared_ptr<const Snapshot>;
    std::uint32_t getProcessId() const noexcept { return info_.process_id; }
    std::chrono::system_clock::time_point getCapturedAt() const {
        return std::chrono::system_clock::time_point{Milliseconds{static_cast<std::int64_t>(info_.captured_unix_ms)}};
    }
    Milliseconds getAge() const {
        return std::chrono::duration_cast<Milliseconds>(std::chrono::steady_clock::now() - captured_);
    }
    bool isOlderThan(Milliseconds age) const { return getAge() > age; }
    std::string getGameSha256() const { return info_.game_sha256; }
    std::span<const Train> getAllTrains() const noexcept { return trains_.rows; }
    std::optional<Train> getTrainById(Id id) const { return trains_.find(id); }
    std::optional<TrainDetails> getTrainDetailsById(Id id) const { return details_.find(id); }
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
    std::span<const TrackNode> getAllTrackNodes() const noexcept { return nodes_.rows; }
    std::optional<TrackNode> getTrackNodeById(Id id) const { return nodes_.find(id); }
    std::span<const SignalState> getAllSignalStates() const noexcept { return states_.rows; }
    std::optional<SignalState> getSignalStateById(Id id) const { return states_.find(id); }
    std::span<const SignalTexture> getAllSignalTextures() const noexcept { return textures_.rows; }
    std::optional<SignalTexture> getSignalTextureById(Id id) const { return textures_.find(id); }

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
        return filter(signals_.rows, [id](const Signal& v) { return v.getTrackId() == id; });
    }
    std::vector<Train> getTrainsOnTrack(Id id) const {
        return filter(trains_.rows, [id](const Train& v) {
            auto position = v.getPosition(); return position && position->getTrackId() == id;
        });
    }
    // One result per station track section. Labels can repeat; track IDs cannot.
    std::optional<std::vector<PlatformOccupation>> getPlatformOccupationsForStation(Id id) const {
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
    std::vector<Track> getTracksForStation(Id id) const {
        return filter(tracks_.rows, [id](const Track& v) { return v.getStationId() == id; });
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
        return filter(*rows, [id](const TrackUsage& v) { return v.getTrainId() == id; });
    }
    std::optional<std::vector<TrackUsage>> getReservationsForTrack(Id id) const {
        const auto& rows = reservations_;
        if (!rows) return std::nullopt;
        return filter(*rows, [id](const TrackUsage& v) { return v.getTrackId() == id; });
    }
    std::optional<std::vector<TrackUsage>> getOccupationsForTrain(Id id) const {
        const auto& rows = occupations_;
        if (!rows) return std::nullopt;
        return filter(*rows, [id](const TrackUsage& v) { return v.getTrainId() == id; });
    }
    std::optional<std::vector<TrackUsage>> getOccupationsForTrack(Id id) const {
        const auto& rows = occupations_;
        if (!rows) return std::nullopt;
        return filter(*rows, [id](const TrackUsage& v) { return v.getTrackId() == id; });
    }

private:
    friend class Client;
    Snapshot() = default;
    NimbySnapshotInfo info_{};
    std::chrono::steady_clock::time_point captured_;
    detail::Table<Train> trains_;
    detail::Table<TrainService> services_;
    detail::Table<TrainDetails> details_;
    std::unordered_map<Id,std::vector<LineStop>> line_stops_;
    detail::Table<Track> tracks_;
    std::unordered_map<Id,std::vector<Platform>> station_platforms_;
    detail::Table<Station> stations_;
    detail::Table<Signal> signals_;
    detail::Table<TrackNode> nodes_;
    detail::Table<SignalState> states_;
    detail::Table<SignalTexture> textures_;
    std::unordered_map<Id, std::vector<Id>> paths_;
    std::optional<std::vector<TrackUsage>> reservations_, occupations_;
    std::unordered_map<Id,std::vector<Id>> occupant_ids_,reservation_ids_;
    template<class T, class Predicate>
    static std::vector<T> filter(const std::vector<T>& rows, Predicate predicate) {
        std::vector<T> result;
        for (const auto& row : rows) if (predicate(row)) result.push_back(row);
        return result;
    }
    static Ptr capture(NimbySession session) {
        detail::NativeSnapshot native;
        detail::check(NimbyInternal_CaptureSnapshot(session, &native.value), "CaptureSnapshot");
        auto result = std::shared_ptr<Snapshot>(new Snapshot);
        result->captured_ = std::chrono::steady_clock::now();
        result->info_.struct_size = sizeof result->info_;
        detail::check(NimbyInternal_GetSnapshotInfo(native.value, &result->info_), "GetSnapshotInfo");
        result->trains_ = detail::table<Train, NimbyTrain>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrains(native.value, out, capacity, count); },
            [](const NimbyTrain& row) { return row.id; }, "CopyTrains");
        result->services_ = detail::table<TrainService, NimbyTrainService>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrainServices(native.value, out, capacity, count); },
            [](const NimbyTrainService& row) { return row.train_id; }, "CopyTrainServices");
        result->details_ = detail::table<TrainDetails, NimbyTrainDetails>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrainDetails(native.value, out, capacity, count); },
            [](const NimbyTrainDetails& row) { return row.train_id; }, "CopyTrainDetails");
        result->tracks_ = detail::table<Track, NimbyTrack>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTracks(native.value, out, capacity, count); },
            [](const NimbyTrack& row) { return row.id; }, "CopyTracks");
        result->stations_ = detail::table<Station, NimbyStation>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyStations(native.value, out, capacity, count); },
            [](const NimbyStation& row) { return row.id; }, "CopyStations");
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
        result->states_ = detail::table<SignalState, NimbySignalState>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopySignalStates(native.value, out, capacity, count); },
            [](const NimbySignalState& row) { return row.signal_id; }, "CopySignalStates");
        result->textures_ = detail::table<SignalTexture, NimbySignalTexture>(
            [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopySignalTextures(native.value, out, capacity, count); },
            [](const NimbySignalTexture& row) { return row.signal_id; }, "CopySignalTextures");

        for (const auto& train : result->trains_.rows) {
            auto stops=detail::copyRecords<NimbyLineStop>([&](auto* out,auto capacity,auto* count){
                return NimbyInternal_CopyTrainLineStops(native.value,train.getId(),out,capacity,count);
            },"CopyTrainLineStops");
            if(stops){auto& rows=result->line_stops_[train.getId()];for(const auto& row:*stops)rows.emplace_back(row);}
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
                for (const auto& row : *rows) result->reservations_->emplace_back(row);
                for (const auto& row : *rows) result->reservation_ids_[row.track_id].push_back(row.train_id);
            }
        }
        {
            auto rows = detail::copyRecords<NimbyTrackUsage>(
                [&](auto* out, auto capacity, auto* count) { return NimbyInternal_CopyTrackOccupations(native.value, out, capacity, count); }, "CopyTrackOccupations");
            if (rows) {
                result->occupations_.emplace();
                for (const auto& row : *rows) result->occupations_->emplace_back(row);
                for (const auto& row : *rows) result->occupant_ids_[row.track_id].push_back(row.train_id);
            }
        }
        return result;
    }
};

enum class ConnectionState { Connected, ProcessExited };

namespace detail {
inline std::uint32_t discoverProcess() {
    const HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (handle == INVALID_HANDLE_VALUE) check(NIMBY_IO_ERROR, "FindProcess");
    struct Close { HANDLE value; ~Close() { CloseHandle(value); } } owner{handle};
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof entry;
    std::uint32_t found{};
    if (!Process32FirstW(handle, &entry)) check(NIMBY_IO_ERROR, "FindProcess");
    do {
        if (_wcsicmp(entry.szExeFile, L"NIMBYRails.exe") != 0) continue;
        if (found) throw Exception({NIMBY_INVALID_ARGUMENT, "FindProcess",
                                   "Several NIMBY Rails processes found; use connect(pid)"});
        found = entry.th32ProcessID;
    } while (Process32NextW(handle, &entry));
    if (GetLastError() != ERROR_NO_MORE_FILES) check(NIMBY_IO_ERROR, "FindProcess");
    if (!found) throw Exception({NIMBY_IO_ERROR, "FindProcess", "NIMBY Rails is not running"});
    return found;
}
}

// Non-copyable/non-movable: one session and one worker owned by this client.
// Getters/capture/control methods may be called concurrently; destruction requires
// all external callers to have stopped. No user callback runs on the worker.
class Client {
public:
    static Client connect() { return Client(detail::discoverProcess()); }
    static Client connect(std::uint32_t pid) {
        if (!pid) detail::check(NIMBY_INVALID_ARGUMENT, "connect(pid)");
        return Client(pid);
    }
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    Client(Client&&) = delete;
    Client& operator=(Client&&) = delete;
    ~Client() {
        stopAutoRefresh();
        if (session_) NimbyInternal_CloseSession(session_);
    }
    Version getSdkVersion() const noexcept { return version_; }
    std::uint32_t getProcessId() const noexcept { return pid_; }
    ConnectionState getConnectionState() const {
        std::lock_guard lock(mutex_); return connection_;
    }
    std::optional<Error> getLastError() const {
        std::lock_guard lock(mutex_); return error_;
    }
    Milliseconds getRefreshInterval() const {
        std::lock_guard lock(mutex_); return interval_;
    }
    bool isAutoRefreshRunning() const {
        std::lock_guard lock(mutex_); return running_;
    }
    Snapshot::Ptr latest() const {
        std::lock_guard lock(mutex_); return latest_;
    }
    bool waitForSnapshot(Milliseconds timeout) const {
        if (timeout < Milliseconds::zero()) detail::check(NIMBY_INVALID_ARGUMENT, "waitForSnapshot");
        std::unique_lock lock(mutex_);
        condition_.wait_for(lock, timeout, [this] {
            return bool(latest_) || connection_ == ConnectionState::ProcessExited;
        });
        return bool(latest_);
    }
    Snapshot::Ptr capture() {
        std::lock_guard captureLock(captureMutex_);
        try {
            auto snapshot = Snapshot::capture(session_);
            {
                std::lock_guard lock(mutex_);
                latest_ = snapshot;
                error_.reset();
                connection_ = ConnectionState::Connected;
            }
            condition_.notify_all();
            return snapshot;
        } catch (const Exception& e) {
            recordError(e.error());
            throw;
        } catch (const std::exception& e) {
            recordError({NIMBY_INTERNAL_ERROR, "capture", e.what()});
            throw;
        }
    }
    void setRefreshInterval(Milliseconds interval) {
        if (interval <= Milliseconds::zero() || interval > std::chrono::hours{24})
            detail::check(NIMBY_INVALID_ARGUMENT, "setRefreshInterval");
        {
            std::lock_guard lock(mutex_);
            interval_ = interval;
            ++revision_;
        }
        condition_.notify_all();
    }
    void startAutoRefresh() {
        std::lock_guard lifecycle(lifecycleMutex_);
        std::lock_guard lock(mutex_);
        if (running_) return;
        stop_ = false;
        running_ = true;
        try { worker_ = std::thread([this] { run(); }); }
        catch (...) { running_ = false; throw; }
    }
    void startAutoRefresh(Milliseconds interval) {
        setRefreshInterval(interval);
        startAutoRefresh();
    }
    void stopAutoRefresh() {
        std::lock_guard lifecycle(lifecycleMutex_);
        {
            std::lock_guard lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        if (worker_.joinable()) worker_.join();
        std::lock_guard lock(mutex_);
        running_ = false;
    }
private:
    explicit Client(std::uint32_t pid) : pid_(pid) {
        version_ = nimby::getVersion();
        detail::check(NimbyInternal_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION, pid, &session_), "OpenProcess");
    }
    void recordError(Error error) {
        {
            std::lock_guard lock(mutex_);
            if (error.code == NIMBY_PROCESS_EXITED) connection_ = ConnectionState::ProcessExited;
            error_ = std::move(error);
        }
        condition_.notify_all();
    }
    void run() noexcept {
        using Clock = std::chrono::steady_clock;
        std::unique_lock lock(mutex_);
        auto revision = revision_;
        auto next = Clock::now(); // First capture starts immediately.
        while (!stop_) {
            condition_.wait_until(lock, next, [&] { return stop_ || revision_ != revision; });
            if (stop_) break;
            if (revision_ != revision) {
                revision = revision_;
                next = Clock::now() + interval_;
                continue;
            }
            const auto started = Clock::now();
            lock.unlock();
            try { capture(); }
            catch (...) {
                // capture() records the normal SDK/std::exception failures.
                // Never let an exception escape a background thread.
            }
            lock.lock();
            revision = revision_;
            next = started + interval_;
            const auto now = Clock::now();
            if (next <= now) {
                // Skip missed deadlines; never enqueue catch-up captures.
                next += interval_ * ((now - next) / interval_ + 1);
            }
        }
    }
    NimbySession session_{};
    std::uint32_t pid_;
    Version version_{};
    mutable std::mutex mutex_;
    std::mutex captureMutex_, lifecycleMutex_;
    mutable std::condition_variable condition_;
    std::thread worker_;
    Snapshot::Ptr latest_;
    std::optional<Error> error_;
    ConnectionState connection_ = ConnectionState::Connected;
    Milliseconds interval_{250};
    std::uint64_t revision_{};
    bool running_ = false, stop_ = false;
};
} // namespace nimby
