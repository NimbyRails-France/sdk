#pragma once
// C++20 convenience layer over the public C ABI. No game-memory internals.
#include "observation.h"
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
private:
    Error error_;
};
namespace detail {
inline void check(std::uint32_t code, const char* operation) {
    if (code != NIMBY_OK) throw Exception({code, operation, NimbySdk_StatusString(code)});
}
inline std::optional<Id> reference(Id id) {
    return id ? std::optional<Id>{id} : std::nullopt;
}
}
struct Version {
    std::uint32_t major, minor, patch, abi;
};
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
    std::optional<double> getSpeedMps() const { return (data_.flags & NIMBY_TRAIN_PRESENT) ? std::optional<double>{data_.speed_mps} : std::nullopt; }
    std::optional<double> getSpeedKmh() const { return (data_.flags & NIMBY_TRAIN_PRESENT) ? std::optional<double>{data_.speed_mps * 3.6} : std::nullopt; }
    std::optional<Position> getPosition() const { return (data_.flags & NIMBY_TRAIN_POSITION_VALID) ? std::optional<Position>{Position{data_.track_id, data_.track_fraction, data_.direction}} : std::nullopt; }
private:
    NimbyTrain data_;
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
    ~NativeSnapshot() { if (value) NimbySdk_ReleaseSnapshot(value); }
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
    detail::Table<Track> tracks_;
    detail::Table<Station> stations_;
    detail::Table<Signal> signals_;
    detail::Table<TrackNode> nodes_;
    detail::Table<SignalState> states_;
    detail::Table<SignalTexture> textures_;
    std::unordered_map<Id, std::vector<Id>> paths_;
    std::optional<std::vector<TrackUsage>> reservations_, occupations_;
    template<class T, class Predicate>
    static std::vector<T> filter(const std::vector<T>& rows, Predicate predicate) {
        std::vector<T> result;
        for (const auto& row : rows) if (predicate(row)) result.push_back(row);
        return result;
    }
    static Ptr capture(NimbySession session) {
        detail::NativeSnapshot native;
        detail::check(NimbySdk_CaptureSnapshot(session, &native.value), "CaptureSnapshot");
        auto result = std::shared_ptr<Snapshot>(new Snapshot);
        result->captured_ = std::chrono::steady_clock::now();
        result->info_.struct_size = sizeof result->info_;
        detail::check(NimbySdk_GetSnapshotInfo(native.value, &result->info_), "GetSnapshotInfo");
        result->trains_ = detail::table<Train, NimbyTrain>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopyTrains(native.value, out, capacity, count); },
            [](const NimbyTrain& row) { return row.id; }, "CopyTrains");
        result->tracks_ = detail::table<Track, NimbyTrack>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopyTracks(native.value, out, capacity, count); },
            [](const NimbyTrack& row) { return row.id; }, "CopyTracks");
        result->stations_ = detail::table<Station, NimbyStation>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopyStations(native.value, out, capacity, count); },
            [](const NimbyStation& row) { return row.id; }, "CopyStations");
        result->signals_ = detail::table<Signal, NimbySignal>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopySignals(native.value, out, capacity, count); },
            [](const NimbySignal& row) { return row.id; }, "CopySignals");
        result->nodes_ = detail::table<TrackNode, NimbyTrackNode>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopyTrackNodes(native.value, out, capacity, count); },
            [](const NimbyTrackNode& row) { return row.id; }, "CopyTrackNodes");
        result->states_ = detail::table<SignalState, NimbySignalState>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopySignalStates(native.value, out, capacity, count); },
            [](const NimbySignalState& row) { return row.signal_id; }, "CopySignalStates");
        result->textures_ = detail::table<SignalTexture, NimbySignalTexture>(
            [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopySignalTextures(native.value, out, capacity, count); },
            [](const NimbySignalTexture& row) { return row.signal_id; }, "CopySignalTextures");

        for (const auto& train : result->trains_.rows) {
            auto path = detail::copyRecords<Id>(
                [&](auto* out, auto capacity, auto* count) {
                    return NimbySdk_CopyTrainPathTracks(native.value, train.getId(), out, capacity, count);
                }, "CopyTrainPathTracks");
            if (path) result->paths_.emplace(train.getId(), std::move(*path));
        }
        {
            auto rows = detail::copyRecords<NimbyTrackUsage>(
                [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopyTrackReservations(native.value, out, capacity, count); }, "CopyTrackReservations");
            if (rows) {
                result->reservations_.emplace();
                for (const auto& row : *rows) result->reservations_->emplace_back(row);
            }
        }
        {
            auto rows = detail::copyRecords<NimbyTrackUsage>(
                [&](auto* out, auto capacity, auto* count) { return NimbySdk_CopyTrackOccupations(native.value, out, capacity, count); }, "CopyTrackOccupations");
            if (rows) {
                result->occupations_.emplace();
                for (const auto& row : *rows) result->occupations_->emplace_back(row);
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
        if (session_) NimbySdk_CloseSession(session_);
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
        NimbySdkVersion version{};
        version.struct_size = sizeof version;
        detail::check(NimbySdk_GetVersion(&version), "GetVersion");
        if (version.abi_version != NIMBY_OBSERVATION_ABI_VERSION ||
            version.major != 0 || version.minor != 6)
            detail::check(NIMBY_INVALID_ARGUMENT, "SDK 0.6.x / ABI 1 required");
        version_ = {version.major, version.minor, version.patch, version.abi_version};
        detail::check(NimbySdk_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION, pid, &session_), "OpenProcess");
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
