package fr.nimby.sdk

import com.sun.jna.Memory
import com.sun.jna.NativeLibrary
import com.sun.jna.Pointer
import com.sun.jna.ptr.IntByReference
import com.sun.jna.ptr.LongByReference
import fr.nimby.sdk.internal.*
import java.nio.file.Path

/** Owns one session. All returned observations are immutable JVM copies. */
class NimbyClient private constructor(private val lease: LibraryLease, private var session: Long) : ObservationClient {
    private val library get() = lease.library
    companion object {
        fun open(libraryPath: Path, processId: Int): NimbyClient {
            require(processId > 0) { "Un PID de jeu explicite est nécessaire" }
            val lease = Libraries.acquire(libraryPath)
            val client = NimbyClient(lease, 0)
            try {
                Memory(NimbySdkVersion.SIZE.toLong()).use { memory ->
                    memory.clear(); memory.setInt(0, NimbySdkVersion.SIZE)
                    client.call("GetVersion", memory)
                    val version = NimbySdkVersion(memory)
                    require(version.abiVersion == 2 && version.major == 0 && version.minor == 7 && version.patch >= 1) { "SDK 0.7.1+ (0.7.x), ABI 2 requis" }
                }
                val result = LongByReference()
                client.call("OpenProcess", 2, processId, result)
                client.session = result.value
                check(client.session != 0L) { "Le SDK a renvoyé une session vide" }
                return client
            } catch (failure: Throwable) { lease.close(); throw failure }
        }
    }

    private fun status(name: String, vararg args: Any?): Int = library.getFunction("NimbyInternal_$name").invokeInt(args)
    private fun call(name: String, vararg args: Any?) {
        val result = status(name, *args)
        if (result != 0) throw SdkException(result, name)
    }

    private fun <T> records(name: String, snapshot: Long, size: Int, extra: Long? = null, decode: (Pointer) -> T): List<T>? {
        val count = IntByReference()
        fun arguments(pointer: Pointer?, capacity: Int): Array<Any?> = if (extra == null)
            arrayOf(snapshot, pointer, capacity, count) else arrayOf(snapshot, extra, pointer, capacity, count)
        val result = status(name, *arguments(null, 0))
        if (result == 8) return null
        if (result != 0) throw SdkException(result, name)
        val capacity = count.value
        require(capacity >= 0 && capacity.toLong() * size <= 256L * 1024 * 1024) { "Taille d'observation invalide" }
        if (capacity == 0) return emptyList()
        return Memory(capacity.toLong() * size).use { memory ->
            call(name, *arguments(memory, capacity))
            require(count.value in 0..capacity) { "Le SDK a dépassé le tampon demandé" }
            List(count.value) { decode(memory.share(it.toLong() * size, size.toLong())) }
        }
    }

    @Synchronized override fun capture(selectedTrainId: Long?): Observation {
        check(session != 0L) { "Session fermée" }
        val handle = LongByReference()
        call("CaptureSnapshot", session, handle)
        val snapshot = handle.value
        check(snapshot != 0L)
        var captureFailure: Throwable? = null
        try {
            val info = Memory(NimbySnapshotInfo.SIZE.toLong()).use { memory ->
                memory.clear(); memory.setInt(0, NimbySnapshotInfo.SIZE); call("GetSnapshotInfo", snapshot, memory)
                NimbySnapshotInfo(memory).let { Triple(it.capturedUnixMs, it.processId, it.gameSha256) }
            }
            val trains = records("CopyTrains", snapshot, NimbyTrain.SIZE) { p -> NimbyTrain(p).let {
                Train(it.id, it.nameUtf8,
                    if (it.flags and 2 != 0 && it.trackFraction.isFinite() && it.trackFraction in 0.0..1.0) Position(it.trackId, it.trackFraction, it.direction) else null,
                    if (it.flags and 4 != 0 && it.flags and 8 == 0 && it.speedMps.isFinite()) it.speedMps * 3.6 else null, it.flags and 8 != 0)
            } } ?: error("Trains indisponibles")
            val tracks = records("CopyTracks", snapshot, NimbyTrack.SIZE) { p -> NimbyTrack(p).let { Track(it.id, it.stationId.nonzero(), it.speedLimitMps * 3.6) } } ?: error("Voies indisponibles")
            val stations = records("CopyStations", snapshot, NimbyStation.SIZE) { p -> NimbyStation(p).let { Station(it.id, it.nameUtf8) } } ?: error("Gares indisponibles")
            val nodes = records("CopyTrackNodes", snapshot, NimbyTrackNode.SIZE) { p -> NimbyTrackNode(p).let { TrackNode(it.id, it.linkA.nonzero(), it.linkB.nonzero(), it.x, it.y) } }.orEmpty()
            val junctions = records("CopyTrackJunctions", snapshot, NimbyTrackJunction.SIZE) { p -> NimbyTrackJunction(p).let { TrackJunction(it.branchTrackId, it.mainTrackId, it.mainFraction, it.mainDirection, it.branchDirection) } }.orEmpty()
            val states = records("CopySignalStates", snapshot, NimbySignalState.SIZE) { p -> NimbySignalState(p).let {
                it.signalId to Pair(if (it.flags and 4 != 0) it.textureState else null,
                    if (it.flags and 2 != 0) "${it.systemUtf8}:${it.specificStateUtf8}" else null)
            } }.orEmpty().toMap()
            val textures = records("CopySignalTextures", snapshot, NimbySignalTexture.SIZE) { p -> NimbySignalTexture(p).let {
                it.signalId to if (it.flags and 2 != 0) it.filePathUtf8.takeIf(String::isNotEmpty) else null
            } }.orEmpty().toMap()
            val signals = records("CopySignals", snapshot, NimbySignal.SIZE) { p -> NimbySignal(p).let {
                Signal(it.id, Position(it.trackId, it.trackFraction, it.direction), it.kind, states[it.id]?.first, states[it.id]?.second, textures[it.id])
            } }.orEmpty()
            val services = records("CopyTrainServices", snapshot, NimbyTrainService.SIZE) { p -> NimbyTrainService(p).let {
                Service(it.trainId, if (it.flags and 256 != 0) it.lineNameUtf8 else null, if (it.flags and 1 != 0) it.status else null,
                    if (it.flags and 4 != 0) it.locationStationId.nonzero() else null, it.flags,
                    if (it.flags and 8 != 0) it.lineId.nonzero() else null,
                    if (it.flags and 16 != 0) it.stopStationId.nonzero() else null,
                    if (it.flags and 16 != 0) it.stopIndex else null,
                    if (it.flags and 512 != 0) it.gameEpochSeconds else null,
                    if (it.flags and 2 != 0) it.gameTimeUs else null,
                    if (it.flags and 32 != 0) it.arrivalTimeUs else null,
                    if (it.flags and 64 != 0) it.departureTimeUs else null,
                    if (it.flags and 128 != 0) it.dispatchTimeUs else null)
            } }.orEmpty()
            val details = records("CopyTrainDetails", snapshot, NimbyTrainDetails.SIZE) { p -> NimbyTrainDetails(p).let {
                TrainDetails(it.trainId, if (it.flags and 1 != 0) it.passengerCount else null,
                    if (it.flags and 2 != 0) it.scheduleId.nonzero() else null, if (it.flags and 2 != 0) it.shiftId.nonzero() else null)
            } }.orEmpty()
            val platforms = records("CopyPlatforms", snapshot, NimbyPlatform.SIZE) { p -> NimbyPlatform(p).let {
                Platform(it.trackId, it.stationId.nonzero(), if (it.flags and 1 != 0) it.nameUtf8 else null)
            } }.orEmpty()
            fun usage(name: String) = records(name, snapshot, NimbyTrackUsage.SIZE) { p -> NimbyTrackUsage(p).let { TrackUsage(it.trainId, it.trackId, it.fractionBegin, it.fractionEnd) } }
            val path = selectedTrainId?.let { records("CopyTrainPathTracks", snapshot, 8, it) { p -> p.getLong(0) } }
            val stops = selectedTrainId?.let { train -> records("CopyTrainLineStops", snapshot, NimbyLineStop.SIZE, train) { p -> NimbyLineStop(p).let {
                LineStop(it.lineId, it.trackId, it.stationId.nonzero(), it.index, if (it.flags and 1 != 0) it.arrivalOffsetSeconds else null,
                    if (it.flags and 1 != 0) it.departureOffsetSeconds else null)
            } } }
            val clock = Memory(NimbySimulationClock.SIZE.toLong()).use { memory ->
                memory.clear(); memory.setInt(0, NimbySimulationClock.SIZE)
                val result = status("GetSimulationClock", snapshot, memory)
                when (result) { 0 -> NimbySimulationClock(memory).let { SimulationClock(it.epochSeconds, it.ticks) }; 8 -> null; else -> throw SdkException(result, "GetSimulationClock") }
            }
            return Observation(info.first, info.second, info.third, trains, tracks, stations, nodes, junctions, signals, services, details, platforms,
                usage("CopyTrackReservations"), usage("CopyTrackOccupations"), path, stops, clock)
        } catch (failure: Throwable) {
            captureFailure = failure
            throw failure
        } finally {
            try { call("ReleaseSnapshot", snapshot) }
            catch (releaseFailure: Throwable) {
                if (captureFailure != null) captureFailure.addSuppressed(releaseFailure) else throw releaseFailure
            }
        }
    }

    @Synchronized override fun close() {
        if (session != 0L) { try { call("CloseSession", session) } finally { session = 0; lease.close() } }
    }
}
private fun Long.nonzero(): Long? = takeIf { it != 0L }

/** JNA caches library instances; one session must not unload another's code. */
private class LibraryLease(val library: NativeLibrary, private val release: () -> Unit) : AutoCloseable {
    override fun close() = release()
}
private object Libraries {
    private data class Entry(val library: NativeLibrary, var users: Int)
    private val entries = mutableMapOf<Path, Entry>()
    @Synchronized fun acquire(path: Path): LibraryLease {
        val canonical = path.toRealPath()
        val entry = entries.getOrPut(canonical) { Entry(NativeLibrary.getInstance(canonical.toString()), 0) }
        entry.users++
        return LibraryLease(entry.library) { release(canonical) }
    }
    @Synchronized private fun release(path: Path) {
        val entry = entries.getValue(path)
        if (--entry.users == 0) {
            entries.remove(path)
            entry.library.close()
        }
    }
}
