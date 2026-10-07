package fr.nimby.sdk

import com.sun.jna.Memory
import com.sun.jna.NativeLibrary
import com.sun.jna.Pointer
import com.sun.jna.ptr.IntByReference
import com.sun.jna.ptr.LongByReference
import fr.nimby.sdk.internal.*
import java.nio.file.Path

/** Owns one session. All returned observations are immutable JVM copies. */
class NimbyClient private constructor(private val lease: LibraryLease, private var session: Long, private val processId: Int) : ObservationClient {
    private val library get() = lease.library
    private var recordBuffer: Memory? = null
    private val networkValues = RecordTableCache()
    // Only context-free rows belong here. Signals are joined with freshly read
    // states/textures, so identical signal positions alone cannot reuse them.
    private val reusableTables = setOf("CopyTracks", "CopyStations", "CopyTrackNodes", "CopyTrackJunctions", "CopyTrackMetrics", "CopyPlatforms")
    private val optionalExports = HashMap<String, Boolean>()
    private fun available(name: String): Boolean = optionalExports.getOrPut(name) {
        try { library.getFunction("NimbyInternal_$name"); true } catch (_: UnsatisfiedLinkError) { false }
    }
    private val trackMetricsAvailable by lazy {
        try { library.getFunction("NimbyInternal_CopyTrackMetrics"); true } catch (_: UnsatisfiedLinkError) { false }
    }
    private val sessionCaptureAvailable by lazy {
        try { library.getFunction("NimbyInternal_CaptureSessionSnapshot"); true } catch (_: UnsatisfiedLinkError) { false }
    }
    companion object {
        fun open(libraryPath: Path, processId: Int): NimbyClient {
            DiagnosticLog.forComponent("sdk-client").write("Open SDK 0.8.x ABI 2: library=$libraryPath gamePid=$processId")
            require(processId > 0) { "Un PID de jeu explicite est nécessaire" }
            val lease = Libraries.acquire(libraryPath)
            val client = NimbyClient(lease, 0, processId)
            try {
                Memory(NimbySdkVersion.SIZE.toLong()).use { memory ->
                    memory.clear(); memory.setInt(0, NimbySdkVersion.SIZE)
                    client.call("GetVersion", memory)
                    val version = NimbySdkVersion(memory)
                    DiagnosticLog.forComponent("sdk-client").write("Loaded SDK version=${version.major}.${version.minor}.${version.patch} ABI=${version.abiVersion} library=${libraryPath.toAbsolutePath()}")
                    require(version.abiVersion == 2 && version.major == 0 && version.minor == 8) { "SDK 0.8.x, ABI 2 requis" }
                }
                val result = LongByReference()
                client.call("OpenProcess", 2, processId, result)
                client.session = result.value
                check(client.session != 0L) { "Le SDK a renvoyé une session vide" }
                DiagnosticLog.forComponent("sdk-client").write("Connected gamePid=$processId session=${client.session}")
                return client
            } catch (failure: Throwable) { DiagnosticLog.forComponent("sdk-client").write("Open SDK failed", failure); lease.close(); throw failure }
        }
    }

    private fun status(name: String, vararg args: Any?): Int = try {
        library.getFunction("NimbyInternal_$name").invokeInt(args).also { result ->
            if (result != 0 && result != 8) DiagnosticLog.forComponent("sdk-client").write("$name failed: status=$result gamePid=$processId", level = "ERROR")
        }
    } catch (failure: Throwable) {
        DiagnosticLog.forComponent("sdk-client").write("Native call $name gamePid=$processId", failure)
        throw failure
    }
    private fun call(name: String, vararg args: Any?) {
        val result = status(name, *args)
        if (result != 0) throw SdkException(result, name)
    }

    /**
     * Reads only this train. No network capture, hook installation or game write.
     * Null means absent/unstable data; other native failures throw SdkException.
     * Serialized with capture(), commands and close() on this connection.
     * A result owns JVM values and remains usable after the client is closed.
     */
    @Synchronized fun readTrain(trainId: Long): DrivingObservation? {
        check(session != 0L) { "Session fermée" }
        require(trainId ushr 48 == 5L) { "Identifiant de train requis" }
        return Memory(NativeDriving.SIZE.toLong()).use { memory ->
            memory.clear(); memory.setInt(0, NativeDriving.SIZE)
            when (val result = status("ReadTrainDriving", session, trainId, memory)) {
                0 -> NativeDriving.decode(memory, trainId)
                8 -> null
                else -> throw SdkException(result, "ReadTrainDriving")
            }
        }
    }

    /** Explicitly loads the experimental construction bridge. Capture alone never does. */
    @Synchronized fun prepareConstruction(sourceSignal: Long): ConstructionResult =
        construction(1,0,sourceSignal,emptyList())

    @Synchronized fun createSignals(token: Long, sourceSignal: Long, positions: List<Position>): ConstructionResult =
        construction(2,token,sourceSignal,positions)

    /** Refuses if another command replaced the series at the top of native undo history. */
    @Synchronized fun undoConstruction(token: Long): ConstructionResult = construction(3,token,0,emptyList())

    @Synchronized fun pollConstruction(token: Long): ConstructionResult {
        check(session!=0L);require(token!=0L)
        return Memory(ConstructionCodec.RESULT_SIZE.toLong()).use { result ->
            result.clear();result.setInt(0,ConstructionCodec.RESULT_SIZE)
            call("ConstructionPoll",session,token,result)
            ConstructionCodec.decode(result)
        }
    }

    private fun construction(action: Int, token: Long, source: Long, positions: List<Position>): ConstructionResult {
        check(session!=0L) { "Session fermée" }
        return ConstructionCodec.encode(action,token,source,positions).use { request ->
            Memory(ConstructionCodec.RESULT_SIZE.toLong()).use { result ->
                result.clear();result.setInt(0,ConstructionCodec.RESULT_SIZE)
                call("Construction",session,request,result)
                ConstructionCodec.decode(result).also {
                    DiagnosticLog.forComponent("sdk-client").write("Construction action=$action gamePid=$processId token=${it.token} state=${it.state} reason=${it.reason} ids=${it.createdIds} canUndo=${it.canUndo}")
                }
            }
        }
    }

    private fun <T> withRecordBuffer(bytes: Long, decode: (Memory) -> T): T {
        // Captures are serialized per connection. Every decoder creates JVM
        // values before the next table reuses this buffer; no pointer escapes.
        // An unusually large map is accepted without retaining its peak native
        // allocation forever on an otherwise idle connection.
        val retainedLimit = 32L * 1024 * 1024
        if(bytes > retainedLimit) return Memory(bytes).use(decode)
        var memory = recordBuffer
        if(memory == null || memory.size() < bytes) {
            val capacity = maxOf(bytes, minOf(retainedLimit, (memory?.size() ?: 32768L) * 2))
            val replacement = Memory(capacity)
            recordBuffer = replacement
            memory?.close()
            memory = replacement
        }
        return decode(memory)
    }

    private fun <T> readRecords(name: String, snapshot: Long, size: Int, extra: Long? = null, decode: (Memory?, Int) -> T): T? {
        val count = IntByReference()
        fun arguments(pointer: Pointer?, capacity: Int): Array<Any?> = if (extra == null)
            arrayOf(snapshot, pointer, capacity, count) else arrayOf(snapshot, extra, pointer, capacity, count)
        val result = status(name, *arguments(null, 0))
        if (result == 8) return null
        if (result != 0) throw SdkException(result, name)
        val capacity = count.value
        require(capacity >= 0 && capacity.toLong() * size <= 256L * 1024 * 1024) { "Taille d'observation invalide" }
        if (capacity == 0) return decode(null, 0)
        return withRecordBuffer(capacity.toLong() * size) { memory ->
            call(name, *arguments(memory, capacity))
            require(count.value in 0..capacity) { "Le SDK a dépassé le tampon demandé" }
            decode(memory, count.value)
        }
    }
    private fun <T> records(name: String, snapshot: Long, size: Int, extra: Long? = null, decode: (Pointer) -> T): List<T>? {
        val reusable = extra == null && name in reusableTables
        val result = readRecords(name, snapshot, size, extra) { memory, count ->
            fun copy(): List<T> = RecordList(List(count) {
                decode(memory!!.share(it.toLong() * size, size.toLong()))
            })
            if(reusable) networkValues.read(name, memory, count.toLong() * size, ::copy) else copy()
        }
        if(result == null && reusable) networkValues.invalidate(name)
        return result
    }
    private fun <T> recordsById(name: String, snapshot: Long, size: Int, decode: (Pointer) -> T): Map<Long, T>? =
        readRecords(name, snapshot, size) { memory, count ->
            val result = HashMap<Long, T>(count)
            repeat(count) { i ->
                val row = memory!!.share(i.toLong() * size, size.toLong())
                result[row.getLong(0)] = decode(row)
            }
            result
        }

    private fun <T> optionalRecords(name: String, snapshot: Long, size: Int, decode: (Pointer) -> T): List<T>? =
        if(available(name)) records(name, snapshot, size, decode = decode) else null

    private fun characteristics(value: NimbyTrainCharacteristics, composition: List<TrainVehicle>?): TrainCharacteristics? {
        if(value.flags and 255 == 0 && composition == null) return null
        fun number(flag: Int, raw: Double) = if(value.flags and flag != 0) raw.takeIf(Double::isFinite) else null
        return TrainCharacteristics(number(1, value.maximumSpeedMps), number(2, value.lengthM), number(4, value.emptyMassKg),
            value.passengerCapacity.takeIf { value.flags and 8 != 0 && it >= 0 }, value.carCount.takeIf { value.flags and 16 != 0 && it >= 0 },
            number(32, value.maximumAccelerationMps2), number(64, value.powerW), number(128, value.tractiveForceN), composition)
    }

    private data class RichData(val lines: List<Line>?, val tags: List<Tag>?, val trains: List<TrainMetadata>?, val models: List<VehicleModel>?)
    private fun richData(snapshot: Long, trains: List<Train>, flags: Int): RichData {
        val models = if(flags and 128 == 0) null else optionalRecords("CopyVehicleModels", snapshot, NimbyVehicleModel.SIZE) { p -> NimbyVehicleModel(p).let {
            VehicleModel(VehicleModelId(it.modelId), it.codeUtf8, it.nameEnUtf8, it.sourceNameUtf8)
        } }
        var compositions: TrainCompositions? = null
        if(flags and 128 != 0 && available("CopyTrainVehicles")) {
            readRecords("CopyTrainVehicles", snapshot, NimbyTrainVehicle.SIZE) { memory, count ->
                require(count <= 262_144)
                compositions = TrainCompositions(models, true).also { observed ->
                    repeat(count) { i -> NimbyTrainVehicle(memory!!.share(i.toLong() * NimbyTrainVehicle.SIZE, NimbyTrainVehicle.SIZE.toLong())).let {
                        observed.add(it.trainId, it.modelId, it.index, it.composition)
                    } }; observed.finish()
                }
            }
        }
        val tags = if(flags and 8 == 0) null else optionalRecords("CopyTags", snapshot, NimbyTag.SIZE) { p -> NimbyTag(p).let { Tag(TagId(it.tagId), it.nameUtf8) } }
        val tagIndex = tags?.associateBy { it.id.value }.orEmpty()
        val states = if(flags and 8 == 0) null else optionalRecords("CopyObjectTagsStates", snapshot, NimbyObjectTagsState.SIZE) { p -> NimbyObjectTagsState(p).let { it.objectId to (it.available == 1) } }
        val links = if(flags and 8 == 0) null else optionalRecords("CopyObjectTags", snapshot, NimbyObjectTag.SIZE) { p -> NimbyObjectTag(p).let { it.objectId to it.tagId } }
        val declared = if(states == null || links == null) null else HashMap<Long, MutableList<Tag>>().also { groups ->
            for((id, known) in states) if(known) groups[id] = ArrayList()
            for((id, tag) in links) groups[id]?.add(tagIndex[tag] ?: Tag(TagId(tag), null))
        }
        val lines = if(flags and 64 == 0) null else optionalRecords("CopyLines", snapshot, NimbyLineMetadata.SIZE) { p -> NimbyLineMetadata(p).let {
            Line(LineId(it.lineId), if(it.flags and 4 != 0) it.nameUtf8 else null,
                if(it.flags and 2 != 0) when(it.kind) { 1 -> LineType.Depot; 0, 2 -> LineType.Other; else -> null } else null,
                if(it.flags and 1 != 0) it.parentLineId.nonzero()?.let(::LineId) else null, it.flags and 1 != 0, declared?.get(it.lineId))
        } }
        val metadata = if(flags and 131 == 0) null else optionalRecords("CopyTrainMetadata", snapshot, NimbyTrainMetadata.SIZE) { p -> NimbyTrainMetadata(p).let {
            TrainMetadata(TrainId(it.trainId), if(it.flags and 1 != 0) it.predictedArrivalDelayUs else null,
                declared?.get(it.trainId), characteristics(it.configured, if(it.configured.flags and 256 != 0) compositions?.forTrain(it.trainId, 0) else null),
                characteristics(it.current, if(it.current.flags and 256 != 0) compositions?.forTrain(it.trainId, 1) else null))
        } }
        val byId = metadata?.associateBy { it.trainId.value }.orEmpty()
        val trainData = if(metadata == null && declared == null) null else trains.map { train ->
            byId[train.id] ?: TrainMetadata(train.trainId, null, declared?.get(train.id))
        }
        return RichData(lines, tags, trainData, models)
    }

    private fun snapshotClock(snapshot: Long): SimulationClock? = Memory(NimbySimulationClock.SIZE.toLong()).use { memory ->
        memory.clear(); memory.setInt(0, NimbySimulationClock.SIZE)
        when(val result = status("GetSimulationClock", snapshot, memory)) {
            0 -> NimbySimulationClock(memory).let { SimulationClock(it.epochSeconds, it.ticks) }
            8 -> null
            else -> throw SdkException(result, "GetSimulationClock")
        }
    }

    /** Reads a fresh session clock without decoding trains, track geometry or
     * signal textures. Older DLLs can supply it through their full snapshot. */
    @Synchronized fun readSimulationClock(): SimulationClock? {
        check(session != 0L) { "Session fermée" }
        val handle = LongByReference()
        if(sessionCaptureAvailable) call("CaptureSessionSnapshot", session, handle, IntByReference())
        else call("CaptureSnapshot", session, handle)
        val snapshot = handle.value
        check(snapshot != 0L)
        var failure: Throwable? = null
        try { return snapshotClock(snapshot) }
        catch(error: Throwable) { failure = error; throw error }
        finally {
            try { call("ReleaseSnapshot", snapshot) }
            catch(error: Throwable) { if(failure != null) failure.addSuppressed(error) else throw error }
        }
    }

    @Synchronized override fun capture(selectedTrainId: Long?): Observation = capture(selectedTrainId, null)

    /** Explicit train-service/catalog request. Ordinary capture and BAL reads
     * never request these tables. An older DLL leaves new data unavailable. */
    @Synchronized fun captureTrainData(selectedTrainId: Long? = null, query: TrainQuery = TrainQuery()): Observation = capture(selectedTrainId, query)

    private fun capture(selectedTrainId: Long?, query: TrainQuery?): Observation {
        check(session != 0L) { "Session fermée" }
        val handle = LongByReference()
        val trainFlags = query?.let { it.flags or if(selectedTrainId != null) 5 else 0 }
        if(trainFlags != null && available("CaptureTrainDataSnapshotWithOptions")) call("CaptureTrainDataSnapshotWithOptions", session, trainFlags, handle, IntByReference())
        else call("CaptureSnapshot", session, handle)
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
            val locations = trainFlags == null || trainFlags and 36 != 0
            val tracks = if(!locations) emptyList() else records("CopyTracks", snapshot, NimbyTrack.SIZE) { p -> NimbyTrack(p).let { Track(it.id, it.stationId.nonzero(), it.speedLimitMps * 3.6) } } ?: error("Voies indisponibles")
            val stations = if(!locations) emptyList() else records("CopyStations", snapshot, NimbyStation.SIZE) { p -> NimbyStation(p).let { Station(it.id, it.nameUtf8) } } ?: error("Gares indisponibles")
            val nodes = if(query != null) emptyList() else records("CopyTrackNodes", snapshot, NimbyTrackNode.SIZE) { p -> NimbyTrackNode(p).let { TrackNode(it.id, it.linkA.nonzero(), it.linkB.nonzero(), it.x, it.y) } }.orEmpty()
            val junctions = if(query != null) emptyList() else records("CopyTrackJunctions", snapshot, NimbyTrackJunction.SIZE) { p -> NimbyTrackJunction(p).let { TrackJunction(it.branchTrackId, it.mainTrackId, it.mainFraction, it.mainDirection, it.branchDirection) } }.orEmpty()
            // Additive private ABI: older 0.8.x DLLs remain usable. Catch only
            // symbol lookup failure, never a failure while invoking/decoding it.
            val metrics = if (query != null || !trackMetricsAvailable) null else records("CopyTrackMetrics", snapshot, NimbyTrackMetric.SIZE) { p ->
                NimbyTrackMetric(p).let { TrackMetric(it.trackId, it.lengthM) }
            }
            val states = if(query != null) emptyMap() else recordsById("CopySignalStates", snapshot, NimbySignalState.SIZE) { p -> NimbySignalState(p).let {
                Pair(if (it.flags and 4 != 0) it.textureState else null,
                    if (it.flags and 2 != 0) "${it.systemUtf8}:${it.specificStateUtf8}" else null)
            } }.orEmpty()
            val textures = if(query != null) emptyMap() else recordsById("CopySignalTextures", snapshot, NimbySignalTexture.SIZE) { p -> NimbySignalTexture(p).let {
                if (it.flags and 2 != 0) it.filePathUtf8.takeIf(String::isNotEmpty) else null
            } }.orEmpty()
            val signals = if(query != null) emptyList() else records("CopySignals", snapshot, NimbySignal.SIZE) { p -> NimbySignal(p).let {
                Signal(it.id, Position(it.trackId, it.trackFraction, it.direction), it.kind, states[it.id]?.first, states[it.id]?.second, textures[it.id])
            } }.orEmpty()
            val services = if(trainFlags != null && trainFlags and 1 == 0) emptyList() else records("CopyTrainServices", snapshot, NimbyTrainService.SIZE) { p -> NimbyTrainService(p).let {
                Service(it.trainId, if (it.flags and 256 != 0) it.lineNameUtf8 else null, if (it.flags and 1 != 0) it.status else null,
                    if (it.flags and 4 != 0) it.locationStationId.nonzero() else null, it.flags,
                    if (it.flags and 8 != 0) it.lineId.nonzero() else null,
                    if (it.flags and 16 != 0) it.stopStationId.nonzero() else null,
                    if (it.flags and 8 != 0) it.stopIndex else null,
                    if (it.flags and 512 != 0) it.gameEpochSeconds else null,
                    if (it.flags and 2 != 0) it.gameTimeUs else null,
                    if (it.flags and 32 != 0) it.arrivalTimeUs else null,
                    if (it.flags and 64 != 0) it.departureTimeUs else null,
                    if (it.flags and 128 != 0) it.dispatchTimeUs else null,
                    if (it.flags and 1025 != 0) it.motionFlags else null,
                    if (it.flags and 1 != 0) it.alert else null,
                    if (it.flags and 4 != 0) it.locationTrackId.nonzero() else null,
                    if (it.flags and 16 != 0) it.stopTrackId.nonzero() else null,
                    if (it.flags and 256 != 0) it.lineKind else null,
                    if (it.flags and 34 == 34) it.arrivalRemainingSeconds.takeIf(Double::isFinite) else null,
                    if (it.flags and 66 == 66) it.departureRemainingSeconds.takeIf(Double::isFinite) else null,
                    if (it.flags and 130 == 130) it.dispatchRemainingSeconds.takeIf(Double::isFinite) else null)
            } }.orEmpty()
            val details = if(trainFlags != null && trainFlags and 21 == 0) emptyList() else records("CopyTrainDetails", snapshot, NimbyTrainDetails.SIZE) { p -> NimbyTrainDetails(p).let {
                TrainDetails(it.trainId, if (it.flags and 1 != 0) it.passengerCount else null,
                    if (it.flags and 2 != 0) it.scheduleId.nonzero() else null, if (it.flags and 2 != 0) it.shiftId.nonzero() else null,
                    if (it.flags and 2 != 0) it.orderIndex else null, it.orderMode.takeIf { mode -> mode in 0..2 })
            } }.orEmpty()
            val platforms = if(query != null) emptyList() else records("CopyPlatforms", snapshot, NimbyPlatform.SIZE) { p -> NimbyPlatform(p).let {
                Platform(it.trackId, it.stationId.nonzero(), if (it.flags and 1 != 0) it.nameUtf8 else null)
            } }.orEmpty()
            fun usage(name: String) = if(query != null) null else records(name, snapshot, NimbyTrackUsage.SIZE) { p -> NimbyTrackUsage(p).let { TrackUsage(it.trainId, it.trackId, it.fractionBegin, it.fractionEnd) } }
            val path = if(query != null) null else selectedTrainId?.let { records("CopyTrainPathTracks", snapshot, 8, it) { p -> p.getLong(0) } }
            val stops = selectedTrainId?.let { train -> records("CopyTrainLineStops", snapshot, NimbyLineStop.SIZE, train) { p -> NimbyLineStop(p).let {
                LineStop(it.lineId, it.trackId, it.stationId.nonzero(), it.index, if (it.flags and 1 != 0) it.arrivalOffsetSeconds else null,
                    if (it.flags and 1 != 0) it.departureOffsetSeconds else null)
            } } }
            val clock = snapshotClock(snapshot)
            val rich = trainFlags?.let { richData(snapshot, trains, it) }
            return Observation(info.first, info.second, info.third, trains, tracks, stations, nodes, junctions, signals, services, details, platforms,
                usage("CopyTrackReservations"), usage("CopyTrackOccupations"), path, stops, clock, metrics, rich?.lines, rich?.tags, rich?.trains, rich?.models)
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

    /**
     * Explicit mutation, never retried. A failure can follow partial native work;
     * capture again before deciding on another action. Input uses whole UTC seconds;
     * the native subsecond timer phase is preserved in the returned clock.
     */
    @Synchronized fun setSimulationDateTime(utc: java.time.Instant, recalculateTrains: Boolean = false): SimulationTimeChange {
        check(session != 0L) { "Session fermée" }
        require(utc.nano == 0) { "La date doit être exprimée en secondes UTC entières" }
        return Memory(NimbySimulationClock.SIZE.toLong()).use { memory ->
            memory.clear(); memory.setInt(0, NimbySimulationClock.SIZE)
            val interventions = IntByReference()
            if (recalculateTrains) call("SetSimulationDateTimeAndRecalculateTrains", session, utc.epochSecond, memory, interventions)
            else call("SetSimulationDateTime", session, utc.epochSecond, memory)
            val clock = NimbySimulationClock(memory).let { SimulationClock(it.epochSeconds, it.ticks) }
            SimulationTimeChange(clock, Integer.toUnsignedLong(interventions.value))
        }
    }

    /** Visual override only. Does not change signalling permissions or mod decisions. */
    @Synchronized fun showSignalTextureFor(signal: Long, catalogue: String, path: String, durationMillis: Int) {
        check(session != 0L) { "Session fermée" }
        require(signal ushr 48 == 8L && catalogue.isNotBlank() && path.isNotBlank())
        require('\u0000' !in catalogue && '\u0000' !in path && durationMillis in 1000..60000)
        call("ShowSignalTextureFor", processId, signal, catalogue, path, durationMillis)
    }

    @Synchronized fun restoreSignalTexture(signal: Long) {
        check(session != 0L) { "Session fermée" }
        require(signal ushr 48 == 8L)
        call("RestoreSignalTexture", processId, signal)
    }

    /** Sends exactly once to the mod already hosted in this explicit game PID. */
    @Synchronized fun modControl(modId: String, request: ControlRequest): ControlResponse {
        check(session != 0L) { "Session fermee" }
        require(modId.matches(Regex("[a-zA-Z0-9_.-]{1,95}")))
        require(request.speedMps.isFinite() && request.speedMps >= 0)
        if (request.operation in setOf(ControlOperation.Acquire, ControlOperation.Renew))
            require(request.owner != 0L && request.leaseMillis in 1000..60000)
        if (request.operation == ControlOperation.Train) {
            require(request.objectId ushr 48 == 5L && (request.exitSignal == 0L || request.exitSignal ushr 48 == 8L))
            require(if (request.mode == TrainControlMode.Stop) request.speedMps == 0.0 else request.speedMps > 0.0)
        }
        return Memory(72).use { input -> Memory(328).use { output ->
            input.clear(); output.clear(); input.setInt(0, 72); input.setInt(4, 1)
            input.setInt(8, request.operation.code); input.setInt(12, request.leaseMillis)
            input.setLong(16, request.owner); input.setLong(24, request.generation)
            input.setLong(32, request.objectId); input.setLong(40, request.exitSignal)
            input.setDouble(48, request.speedMps); input.setInt(56, request.mode.code)
            input.setInt(60, if (request.releaseByRear) 1 else 0)
            input.setInt(64, request.value); input.setInt(68, request.settingIndex); output.setInt(0, 328)
            call("ModControl", processId, modId, input, output)
            check(output.getInt(0) == 328 && output.getInt(4) == 1)
            ControlResponse(output.getInt(12), output.getLong(16), output.getLong(24),
                output.getInt(32), output.getInt(36), output.getInt(40), output.getInt(44),
                output.getInt(48), output.getInt(52), output.getDouble(56), output.getLong(64),
                output.getByteArray(72, 256).takeWhile { it != 0.toByte() }.toByteArray().toString(Charsets.UTF_8))
        } }
    }

    @Synchronized fun acquireModControl(modId: String, leaseMillis: Int = 5000): ModControlSession {
        val observed = modControl(modId, ControlRequest(ControlOperation.Status))
        check(observed.generation != 0L) { "Aucun monde observe par le mod" }
        var owner: Long
        do { owner = java.util.concurrent.ThreadLocalRandom.current().nextLong() } while (owner == 0L)
        modControl(modId, ControlRequest(ControlOperation.Acquire, owner, observed.generation, leaseMillis))
        return ModControlSession(this, modId, owner, observed.generation)
    }

    @Synchronized override fun close() {
        DiagnosticLog.forComponent("sdk-client").write("Close session gamePid=$processId")
        if (session != 0L) { try { call("CloseSession", session) } finally {
            session = 0
            try { networkValues.close(); recordBuffer?.close(); recordBuffer = null } finally { lease.close() }
        } }
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
        val canonical = try { path.toRealPath() } catch (failure: Exception) { DiagnosticLog.forComponent("sdk-client").write("SDK path unavailable: $path", failure); throw failure }
        val entry = entries.getOrPut(canonical) { Entry(try { NativeLibrary.getInstance(canonical.toString()) } catch (failure: Throwable) { DiagnosticLog.forComponent("sdk-client").write("Cannot load SDK: $canonical", failure); throw failure }, 0) }
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
