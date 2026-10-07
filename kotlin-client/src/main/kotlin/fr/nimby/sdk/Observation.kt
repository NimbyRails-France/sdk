package fr.nimby.sdk

import fr.nimby.sdk.internal.indexRecords

/** IDs are opaque. Unknown observations are null, never a manufactured clear state. */
data class Train(val id: Long, val name: String, val position: Position?, val speedKmh: Double?, val speedDefaulted: Boolean) {
    val trainId: TrainId get() = TrainId(id)
}
data class Position(val trackId: Long, val fraction: Double, val direction: Int) {
    val track: TrackId get() = TrackId(trackId)
}
data class Track(val id: Long, val stationId: Long?, val speedLimitKmh: Double) {
    val trackId: TrackId get() = TrackId(id)
    val station: StationId? get() = stationId?.let(::StationId)
}
data class Station(val id: Long, val name: String) {
    val stationId: StationId get() = StationId(id)
}
data class TrackNode(val id: Long, val linkA: Long?, val linkB: Long?, val x: Double, val y: Double)
data class TrackJunction(val branchTrackId: Long, val mainTrackId: Long, val fraction: Double, val mainDirection: Int, val branchDirection: Int)
data class TrackUsage(val trainId: Long, val trackId: Long, val from: Double, val to: Double)
data class Signal(val id: Long, val position: Position, val kind: Int, val textureState: Int?, val specificState: String?, val texturePath: String?)
data class Service(
    val trainId: Long, val lineName: String?, val status: Int?, val stationId: Long?, val flags: Int,
    val lineId: Long? = null, val stopStationId: Long? = null, val stopIndex: Int? = null,
    val gameEpochSeconds: Long? = null, val gameTimeUs: Long? = null,
    val arrivalTimeUs: Long? = null, val departureTimeUs: Long? = null, val dispatchTimeUs: Long? = null,
    val motionFlags: Int? = null, val alert: Int? = null, val locationTrackId: Long? = null,
    val stopTrackId: Long? = null, val lineKind: Int? = null,
    val arrivalRemainingSeconds: Double? = null, val departureRemainingSeconds: Double? = null,
    val dispatchRemainingSeconds: Double? = null,
) {
    val train: TrainId get() = TrainId(trainId)
    val line: LineId? get() = lineId?.let(::LineId)
    /** Null is unavailable. Unknown/Other are observed native states. */
    val state: TrainState? get() = status?.let { TrainState.entries.getOrElse(it) { TrainState.Other } }
    val alertState: TrainAlert? get() = alert?.let(TrainAlert::fromNative)
    /** Presence is independently valid even when the full service is unknown. */
    val hidden: Boolean? get() = motionFlags?.takeIf { flags and 1025 != 0 }?.let { it and 2 != 0 }
    val onNetwork: Boolean? get() = motionFlags?.takeIf { flags and 1025 != 0 }?.let { it and 7 != 0 }
    val isDepotLine: Boolean? get() = lineKind?.takeIf { it in 0..2 }?.let { it == 1 }
    /** UTC calendar in the game, never wall-clock time. Signed microseconds
     * are normalized correctly, including a deadline before the epoch. */
    val observedAt: java.time.Instant? get() = serviceInstant(gameEpochSeconds, gameTimeUs)
    val arrival: java.time.Instant? get() = serviceInstant(gameEpochSeconds, arrivalTimeUs)
    val departure: java.time.Instant? get() = serviceInstant(gameEpochSeconds, departureTimeUs)
    /** A retry deadline, not a commercial departure or predicted lateness. */
    val dispatchRetry: java.time.Instant? get() = serviceInstant(gameEpochSeconds, dispatchTimeUs)
}
data class TrainDetails(val trainId: Long, val passengers: Int?, val scheduleId: Long?, val shiftId: Long?,
                        val orderIndex: Int? = null, val orderMode: Int? = null) {
    val train: TrainId get() = TrainId(trainId)
    val timetable: Timetable? get() = scheduleId?.let { Timetable(TimetableId(it)) }
    val shift: TimetableShiftId? get() = scheduleId?.let { schedule -> shiftId?.let { TimetableShiftId(TimetableId(schedule), it) } }
    val isMothballed: Boolean? get() = orderMode?.takeIf { it in 0..2 }?.let { it == 2 }
}
data class Platform(val trackId: Long, val stationId: Long?, val name: String?)
/** Relative line offsets; partial runs and loops prevent inferring absolute
 * train dates. A waypoint outside a station has a null stationId. */
data class LineStop(val lineId: Long, val trackId: Long, val stationId: Long?, val index: Int, val arrivalOffsetSeconds: Int?, val departureOffsetSeconds: Int?) {
    val line: LineId get() = LineId(lineId)
    val track: TrackId get() = TrackId(trackId)
    val station: StationId? get() = stationId?.let(::StationId)
    val plannedDwellSeconds: Long? get() = arrivalOffsetSeconds?.let { arrival -> departureOffsetSeconds?.toLong()?.minus(arrival) }
}
data class SimulationClock(val epochSeconds: Long, val ticks: Long) {
    /** Native ticks are hundredths of a second; the epoch may precede 1970. */
    fun toInstant(): java.time.Instant {
        require(ticks >= 0) { "Negative simulation ticks" }
        return java.time.Instant.ofEpochSecond(Math.addExact(epochSeconds, ticks / 100), (ticks % 100) * 10_000_000)
    }
}
data class SimulationTimeChange(val clock: SimulationClock, val interventions: Long)
data class Observation(
    val capturedAtMillis: Long,
    val processId: Int,
    val gameHash: String,
    val trains: List<Train>,
    val tracks: List<Track>,
    val stations: List<Station>,
    val nodes: List<TrackNode>,
    val junctions: List<TrackJunction>,
    val signals: List<Signal>,
    val services: List<Service>,
    val details: List<TrainDetails>,
    val platforms: List<Platform>,
    val reservations: List<TrackUsage>?,
    val occupations: List<TrackUsage>?,
    val selectedPath: List<Long>?,
    val lineStops: List<LineStop>?,
    val clock: SimulationClock?,
    /** Null: old DLL or unsupported capture/profile. Missing rows remain unknown. */
    val trackMetrics: List<TrackMetric>? = null,
    /** Opt-in train-data capture only. Null means unavailable or not requested. */
    val lines: List<Line>? = null, val tags: List<Tag>? = null, val trainMetadata: List<TrainMetadata>? = null,
    val vehicleModels: List<VehicleModel>? = null,
) {
    private val trainsById by lazy { trains.indexRecords { it.id } }
    private val servicesById by lazy { services.indexRecords { it.trainId } }
    private val detailsById by lazy { details.indexRecords { it.trainId } }
    private val stationsById by lazy { stations.indexRecords { it.id } }
    private val tracksById by lazy { tracks.indexRecords { it.id } }
    private val platformsByTrack by lazy { platforms.indexRecords { it.trackId } }
    private val linesById by lazy { lines?.associateBy { it.id }.orEmpty() }
    private val tagsById by lazy { tags?.associateBy { it.id }.orEmpty() }
    private val metadataById by lazy { trainMetadata?.associateBy { it.trainId }.orEmpty() }
    private val inheritedTags by lazy { LineTagInheritance(linesById) }
    /** Indexed joins within this copied snapshot; no native read or recapture. */
    fun train(id: Long): TrainRecord? = trainsById[id]?.let { train ->
        val service = servicesById[id]
        TrainRecord(train, service, detailsById[id], train.position?.trackId?.let(tracksById::get)?.stationId?.let(stationsById::get),
            service?.stationId?.let(stationsById::get), service?.stopStationId?.let(stationsById::get), metadataById[TrainId(id)], service?.line?.let(linesById::get))
    }
    fun service(trainId: Long): Service? = servicesById[trainId]
    fun train(id: TrainId): TrainRecord? = train(id.value)
    fun service(id: TrainId): Service? = service(id.value)
    fun details(trainId: Long): TrainDetails? = detailsById[trainId]
    fun details(id: TrainId): TrainDetails? = details(id.value)
    fun station(id: Long): Station? = stationsById[id]
    fun station(id: StationId): Station? = station(id.value)
    fun track(id: Long): Track? = tracksById[id]
    fun track(id: TrackId): Track? = track(id.value)
    /** Platform observation for one track, without scanning every platform. */
    fun platform(trackId: Long): Platform? = platformsByTrack[trackId]
    fun platform(trackId: TrackId): Platform? = platform(trackId.value)
    fun line(id: LineId): Line? = linesById[id]
    fun tag(id: TagId): Tag? = tagsById[id]
    fun metadata(id: TrainId): TrainMetadata? = metadataById[id]
    /** Includes declared tags of every parent. Null if any required link or tag
     * list is unknown, cyclic, deeper than256, or would exceed65536 unique tags. */
    fun tagsForLine(id: LineId): List<Tag>? = inheritedTags.read(id)
}

interface ObservationClient : AutoCloseable {
    fun capture(selectedTrainId: Long? = null): Observation
}

data class GameProcess(val pid: Int, val executable: String)
object GameProcesses {
    fun discover(): List<GameProcess> = ProcessHandle.allProcesses().use { processes ->
        processes.map { process -> process.info().command().orElse(null)?.let { command ->
            val name = java.nio.file.Path.of(command).fileName.toString()
            if (name.equals("NIMBYRails.exe", true) || name == "nimbyrails") GameProcess(process.pid().toInt(), command) else null
        } }.filter { it != null }.map { it!! }.toList()
    }
}

class SdkException(val status: Int, operation: String) : IllegalStateException("$operation : ${when (status) {
    2 -> "accès au processus ou au fichier impossible"
    7 -> "version du jeu non prise en charge par ce SDK"
    8 -> "observation indisponible"
    11 -> "jeu fermé"
    else -> "erreur SDK $status"
}}")
