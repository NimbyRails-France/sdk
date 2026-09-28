package fr.nimby.sdk

/** IDs are opaque. Unknown observations are null, never a manufactured clear state. */
data class Train(val id: Long, val name: String, val position: Position?, val speedKmh: Double?, val speedDefaulted: Boolean)
data class Position(val trackId: Long, val fraction: Double, val direction: Int)
data class Track(val id: Long, val stationId: Long?, val speedLimitKmh: Double)
data class Station(val id: Long, val name: String)
data class TrackNode(val id: Long, val linkA: Long?, val linkB: Long?, val x: Double, val y: Double)
data class TrackJunction(val branchTrackId: Long, val mainTrackId: Long, val fraction: Double, val mainDirection: Int, val branchDirection: Int)
data class TrackUsage(val trainId: Long, val trackId: Long, val from: Double, val to: Double)
data class Signal(val id: Long, val position: Position, val kind: Int, val textureState: Int?, val specificState: String?, val texturePath: String?)
data class Service(
    val trainId: Long, val lineName: String?, val status: Int?, val stationId: Long?, val flags: Int,
    val lineId: Long? = null, val stopStationId: Long? = null, val stopIndex: Int? = null,
    val gameEpochSeconds: Long? = null, val gameTimeUs: Long? = null,
    val arrivalTimeUs: Long? = null, val departureTimeUs: Long? = null, val dispatchTimeUs: Long? = null,
)
data class TrainDetails(val trainId: Long, val passengers: Int?, val scheduleId: Long?, val shiftId: Long?)
data class Platform(val trackId: Long, val stationId: Long?, val name: String?)
data class LineStop(val lineId: Long, val trackId: Long, val stationId: Long?, val index: Int, val arrivalOffsetSeconds: Int?, val departureOffsetSeconds: Int?)
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
)

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
