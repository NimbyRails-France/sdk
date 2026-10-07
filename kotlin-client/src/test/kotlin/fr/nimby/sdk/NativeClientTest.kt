package fr.nimby.sdk

import java.nio.file.Path
import kotlin.test.*

class NativeClientTest {
    @Test fun exactNetworkReuseNeverHidesDynamicChangesOrUnavailableGeometry() {
        val library = com.sun.jna.NativeLibrary.getInstance(fixture().toString())
        val change = library.getFunction("Fixture_NetworkRevision")
        fun revision(value: Int) = change.invokeVoid(arrayOf(value))
        NimbyClient.open(fixture(), 44).use { client -> try {
            revision(0); val original = client.capture(0x5000000000001L)
            revision(1); val dynamic = client.capture(0x5000000000001L)
            assertSame(original.nodes, dynamic.nodes)
            assertEquals(1, dynamic.signals.first().textureState); assertEquals("textures/changed.svg", dynamic.signals.first().texturePath)
            assertEquals(1L, dynamic.occupations?.single()?.trackId); assertEquals(1L, dynamic.reservations?.single()?.trackId)
            assertEquals(listOf(1L), dynamic.selectedPath)
            revision(2); val changed = client.capture(0x5000000000001L)
            assertNotSame(original.nodes, changed.nodes); assertEquals(777.0, changed.nodes.last().y)
            assertEquals(199998.0, original.nodes.last().y); assertEquals(0, original.signals.first().textureState)
            revision(3); assertTrue(client.capture().nodes.isEmpty())
            revision(0); val recovered = client.capture()
            assertNotSame(original.nodes, recovered.nodes); assertEquals(original.nodes, recovered.nodes)
        } finally { revision(0) } }
    }
    @Test fun richQueriesAreOptInTypedAndKeepMaterialSeparateFromMovement() {
        val library = com.sun.jna.NativeLibrary.getInstance(fixture().toString())
        fun counter(name: String) = library.getFunction(name).invokeInt(emptyArray())
        NimbyClient.open(fixture(), 42).use { client ->
            val before = counter("Fixture_RichReads"); val captures = counter("Fixture_TrainDataCaptures")
            val ordinary = client.capture(); assertNull(ordinary.lines); assertNull(ordinary.trainMetadata)
            assertEquals(before, counter("Fixture_RichReads")); assertEquals(captures, counter("Fixture_TrainDataCaptures"))
            val base = client.captureTrainData()
            assertEquals(33, counter("Fixture_LastTrainFlags")); assertNull(base.lines); assertNull(base.metadata(TrainId(-1))?.configured)
            val material = client.captureTrainData(query = TrainQuery(includeService = false, includeLocations = false, includeCharacteristics = true))
            assertEquals(2, counter("Fixture_LastTrainFlags")); assertTrue(material.services.isEmpty()); assertTrue(material.tracks.isEmpty())
            val observed = assertNotNull(material.metadata(TrainId(-1)))
            assertEquals(144.0, observed.configured?.maximumSpeedKmh); assertEquals(126.0, observed.current?.maximumSpeedKmh)
            assertEquals(200.0, observed.configured?.lengthM); assertNull(observed.current?.lengthM)
            assertEquals(500, observed.configured?.passengerCapacity); assertNull(observed.current?.passengerCapacity)
            assertNull(material.trains.single().speedKmh) // A material maximum never fills missing movement speed.
            assertTrue(material.nodes.isEmpty()); assertTrue(material.signals.isEmpty()); assertNull(material.occupations)
        }
    }
    @Test fun catalogsIncludeUnassignedLinesAndInheritanceFailsClosedWithoutDroppingDeclaredTags() {
        val data = NimbyClient.open(fixture(), 42).use { it.captureTrainData(query = TrainQuery(includeTags = true)) }
        assertEquals(7, data.lines?.size); assertEquals(2, data.tags?.size)
        assertEquals(LineType.Depot, data.line(LineId(91))?.type)
        assertEquals(listOf(TagId(8), TagId(7)), data.tagsForLine(LineId(92))?.map { it.id })
        assertNull(data.tagsForLine(LineId(93))); assertNull(data.tagsForLine(LineId(94))); assertNull(data.tagsForLine(LineId(96)))
        assertEquals(listOf(Tag(TagId(999), null)), data.tagsForLine(LineId(97)))
        val train = assertNotNull(data.train(TrainId(-1)))
        assertEquals(-1.5, train.metadata?.predictedArrivalDelaySeconds)
        assertEquals(listOf(TagId(7), TagId(999)), train.metadata?.declaredTags?.map { it.id })
        assertEquals(TimetableId(61), train.details?.timetable?.id); assertEquals(TimetableShiftId(TimetableId(61),71), train.details?.shift)
        val deep = data.copy(lines = List(258) { Line(LineId(it + 1L), null, null, if(it == 257) null else LineId(it + 2L), true, emptyList()) })
        assertNull(deep.tagsForLine(LineId(1))); assertEquals(emptyList(), deep.tagsForLine(LineId(3)))
    }
    @Test fun legacyLibraryKeepsNewCatalogsAndCharacteristicsUnknown() {
        val legacy = Path.of(requireNotNull(System.getProperty("nrf.fixture.legacy")))
        val data = NimbyClient.open(legacy,42).use { it.captureTrainData(query = TrainQuery(includeTags = true, includeCharacteristics = true)) }
        assertEquals("Train test", data.trains.single().name); assertNull(data.lines); assertNull(data.tags); assertNull(data.trainMetadata)
    }
    @Test fun richServiceAndDetailsKeepValidityAndUseSnapshotIndexes() {
        val copied = NimbyClient.open(fixture(), 42).use { it.capture() }
        val row = assertNotNull(copied.train(-1))
        assertEquals("Train test", row.train.name)
        val service = assertNotNull(row.service)
        assertEquals(TrainState.SignalWait, service.state); assertEquals(TrainAlert.SignalWait, service.alertState)
        assertEquals(false, service.hidden); assertEquals(true, service.onNetwork)
        assertEquals(9L, service.locationTrackId); assertEquals(94L, service.stopTrackId)
        assertEquals(true, service.isDepotLine); assertEquals(-6.0, service.arrivalRemainingSeconds)
        assertEquals(0.0, service.departureRemainingSeconds)
        assertEquals(java.time.Instant.ofEpochSecond(1233), service.arrival)
        assertEquals(java.time.Instant.ofEpochSecond(1234), service.departure)
        assertEquals(java.time.Instant.ofEpochSecond(1235), service.dispatchRetry)
        val details = assertNotNull(row.details)
        assertEquals(0, details.passengers); assertEquals(61L, details.scheduleId); assertEquals(71L, details.shiftId)
        assertEquals(0, details.orderIndex); assertEquals(true, details.isMothballed)
        val unknown = assertNotNull(copied.service(1))
        assertNull(unknown.state); assertNull(unknown.hidden); assertNull(unknown.alertState); assertNull(unknown.locationTrackId)
        assertNull(unknown.lineKind); assertNull(unknown.arrivalRemainingSeconds); assertNull(unknown.dispatchRetry)
        assertNull(copied.details(1)?.orderIndex); assertNull(copied.details(1)?.orderMode)
        assertNull(copied.train(0x5000000000001L)); assertNull(row.stopStation) // ID known, station row unavailable.
        assertEquals(92L, service.stopStationId)
    }
    @Test fun serviceDatesNormalizeSignedFractionalMicrosecondsWithoutInventingCalendars() {
        val service = Service(1, null, null, null, 0, gameEpochSeconds = 1000, arrivalTimeUs = -1, departureTimeUs = 0)
        assertEquals(java.time.Instant.ofEpochSecond(999, 999999000), service.arrival)
        assertEquals(java.time.Instant.ofEpochSecond(1000), service.departure)
        assertNull(service.copy(gameEpochSeconds = null).arrival)
        assertNull(service.copy(gameEpochSeconds = Long.MAX_VALUE, arrivalTimeUs = 1_000_000).arrival)
        assertEquals(4_294_967_295L, LineStop(1, 2, null, 0, Int.MIN_VALUE, Int.MAX_VALUE).plannedDwellSeconds)
    }
    @Test fun largeMapBuffersCanBeReusedWithoutChangingEarlierObservations() {
        val first = NimbyClient.open(fixture(), 44).use { client ->
            val before = client.capture()
            repeat(3) {
                val next = client.capture()
                assertSame(before.nodes, next.nodes) // Fresh bytes match exactly; no 100k-row rebuild.
                assertEquals(100000, next.nodes.size)
                assertEquals(16384, next.trains.size)
                assertEquals(8192, next.signals.size)
                assertEquals(199998.0, next.nodes.last().y)
                assertEquals(0x8000000002000L, next.signals.last().id)
                assertEquals("textures/large-map.svg", next.signals.last().texturePath)
                assertEquals("System:Restricted", next.signals.first().specificState)
                assertEquals("Train test", before.trains.first().name)
                assertEquals(before, next)
            }
            before
        }
        assertEquals(100000, first.nodes.size)
        assertEquals("textures/large-map.svg", first.signals.first().texturePath)
        assertFails { (first.nodes as MutableList).clear() }
    }
    @Test fun absurdTableSizeIsRejectedAndItsSnapshotReleased() {
        val library = com.sun.jna.NativeLibrary.getInstance(fixture().toString())
        NimbyClient.open(fixture(), 45).use { client ->
            val released = library.getFunction("Fixture_Released")
            val before = released.invokeInt(emptyArray())
            assertFailsWith<IllegalArgumentException> { client.capture() }
            assertEquals(before + 1, released.invokeInt(emptyArray()))
        }
    }
    @Test fun nativeTrackMetricsRemainOwnedAfterSessionClose() {
        val metric = NimbyClient.open(fixture(),42).use { it.capture().trackMetrics!!.single() }
        assertEquals(TrackMetric(9,1234.5),metric)
        assertEquals(617.25,metric.distanceM(.75,.25))
        assertEquals(.5,metric.fraction(metric.offsetM(.5)))
    }
    @Test fun oldDllWithoutMetricExportStillCapturesOtherTables() {
        val legacy = Path.of(requireNotNull(System.getProperty("nrf.fixture.legacy")))
        val observation = NimbyClient.open(legacy,42).use { it.capture() }
        assertNull(observation.trackMetrics)
        assertEquals("Train test",observation.trains.single().name)
    }
    @Test fun trackMetricRejectsInvalidValuesAndOutOfTrackPositions() {
        for (value in listOf(0.0,-1.0,Double.NaN,Double.POSITIVE_INFINITY))
            assertFailsWith<IllegalArgumentException> { TrackMetric(9,value) }
        val metric = TrackMetric(9,1234.5)
        assertFailsWith<IllegalArgumentException> { metric.offsetM(1.01) }
        assertFailsWith<IllegalArgumentException> { metric.offsetM(Double.NaN) }
        assertFailsWith<IllegalArgumentException> { metric.fraction(-1.0) }
        assertFailsWith<IllegalArgumentException> { metric.fraction(1235.0) }
    }
    private fun fixture() = Path.of(requireNotNull(System.getProperty("nrf.fixture")) { "Native fixture was not built" })
    @Test fun compositionQueryCopiesReferencedModelsAndPreservesIndependentEmptyProfile() {
        val snapshot = NimbyClient.open(fixture(), 42).use { client ->
            assertNull(client.captureTrainData().train(-1)?.metadata?.configured?.composition)
            client.captureTrainData(query = TrainQuery(includeService = false, includeLocations = false, includeComposition = true))
        }
        val metadata = assertNotNull(snapshot.train(-1)?.metadata)
        val vehicles = assertNotNull(metadata.configured?.composition)
        assertEquals(listOf(0, 1), vehicles.map { it.index })
        assertEquals("metro", vehicles.first().model.code)
        assertEquals("Metro vehicle", vehicles.first().model.nameEnglish)
        assertEquals("Base game", vehicles.first().model.sourceName)
        assertEquals(VehicleModelId(999), vehicles.last().modelId); assertNull(vehicles.last().model.code)
        assertEquals(emptyList(), metadata.current?.composition)
        assertNull(metadata.configured?.maximumSpeedMps)
        assertEquals(listOf(VehicleModelId(101)), snapshot.vehicleModels?.map { it.id })
    }
    @Test fun malformedCompositionDoesNotEraseIndependentCurrentVehicles() {
        val rows = TrainCompositions(null, true)
        rows.add(1, 10, 0, 0); rows.add(1, 11, 0, 0); rows.add(1, 12, 0, 1)
        rows.finish(); assertNull(rows.forTrain(1, 0))
        assertEquals(VehicleModelId(12), rows.forTrain(1, 1)?.single()?.modelId)
        assertEquals(emptyList(), rows.forTrain(2, 1))
        assertNull(TrainCompositions(null, false).also { it.finish() }.forTrain(1, 0))
    }
    @Test fun targetedDrivingOwnsValuesAndKeepsIndependentValidity() {
        val observed = NimbyClient.open(fixture(), 42).use { client ->
            val value = assertNotNull(client.readTrain(0x5000000000001))
            assertEquals(19L, value.sessionGeneration)
            assertEquals(987654L, value.capturedAtMillis)
            assertEquals(1200L, value.elapsedBeginMillis)
            assertEquals(1207L, value.elapsedEndMillis)
            assertEquals(Position(9, .25, -1), value.position)
            assertEquals(0.0, value.speedMps) // A measured stop remains available.
            assertFalse(value.speedDefaulted); assertTrue(value.motionAvailable)
            assertNull(value.purchasedDynamics)
            assertEquals(TrainDynamics(41.0, .7, .8, 1.2, 90000.0, 700000.0, 32000.0, 87.0), value.currentDynamics)
            val purchased = assertNotNull(client.readTrain(0x5000000000006))
            assertNotNull(purchased.purchasedDynamics); assertNull(purchased.currentDynamics)
            assertEquals(87.0, value.currentDynamics?.lengthM) // Another read cannot overwrite it.
            value
        }
        assertEquals(87.0, observed.currentDynamics?.lengthM) // Nor can closing the native library.
    }
    @Test fun targetedDrivingNeverTurnsMissingOrDefaultedDataIntoAStop() {
        val client = NimbyClient.open(fixture(), 42)
        client.use {
            assertNull(it.readTrain(0x5000000000002))
            assertEquals(11, assertFailsWith<SdkException> { it.readTrain(0x5000000000003) }.status)
            val defaulted = assertNotNull(it.readTrain(0x5000000000004))
            assertTrue(defaulted.speedDefaulted); assertNull(defaulted.speedMps)
            assertNull(defaulted.currentDynamics); assertFalse(defaulted.motionAvailable)
            val invalid = assertNotNull(it.readTrain(0x5000000000005))
            assertNull(invalid.speedMps); assertNull(invalid.position); assertNull(invalid.currentDynamics)
            assertFailsWith<IllegalArgumentException> { it.readTrain(0x8000000000001) }
            assertFailsWith<IllegalStateException> { it.readTrain(0x5000000000007) }
        }
        assertFailsWith<IllegalStateException> { client.readTrain(0x5000000000001) }
    }
    @Test fun nativeAbiPreservesUnknownAndOwnedCopies() {
        val observation = NimbyClient.open(fixture(), 42).use { client -> client.capture(-1) }
        val train = observation.trains.single()
        assertEquals(-1L, train.id)
        assertEquals("Train test", train.name)
        assertEquals(Position(9, .25, 0), train.position)
        assertNull(train.speedKmh)
        assertTrue(train.speedDefaulted)
        assertNull(observation.reservations)
        assertEquals(emptyList(), observation.occupations)
        assertNull(observation.clock)
        assertNull(observation.selectedPath)
        assertEquals(42, observation.processId)
        assertEquals(123456L, observation.capturedAtMillis)
    }
    @Test fun rejectedGameFailsWithoutReturningSession() {
        val error = assertFailsWith<SdkException> { NimbyClient.open(fixture(), 43) }
        assertEquals(7, error.status)
    }
    @Test fun serviceCalendarUsesValidityBitsAndPreservesZeroAndNegativeTimes() {
        val services = NimbyClient.open(fixture(), 42).use { it.capture().services }
        val known = services.first()
        assertEquals(91L, known.lineId)
        assertEquals(92L, known.stopStationId)
        assertEquals(0, known.stopIndex)
        assertEquals(1234L, known.gameEpochSeconds)
        assertEquals(-1_000_000L, known.arrivalTimeUs)
        assertEquals(0L, known.departureTimeUs)
        assertEquals(1_000_000L, known.dispatchTimeUs)
        val unknown = services.last()
        assertNull(unknown.lineId); assertNull(unknown.stopStationId); assertNull(unknown.stopIndex)
        assertNull(unknown.gameEpochSeconds); assertNull(unknown.gameTimeUs)
        assertNull(unknown.arrivalTimeUs); assertNull(unknown.departureTimeUs); assertNull(unknown.dispatchTimeUs)
    }
    @Test fun closedClientCannotReadAgain() {
        val client = NimbyClient.open(fixture(), 42)
        client.close(); client.close()
        assertFailsWith<IllegalStateException> { client.capture() }
    }
    @Test fun closingOneSessionKeepsOtherSessionUsable() {
        NimbyClient.open(fixture(), 42).use { first ->
            val second = NimbyClient.open(fixture().parent.resolve(".").resolve(fixture().fileName), 42)
            second.close()
            assertEquals("Train test", first.capture().trains.single().name)
        }
    }
    @Test fun failedOpenDoesNotUnloadExistingSession() {
        NimbyClient.open(fixture(), 42).use { first ->
            assertFailsWith<SdkException> { NimbyClient.open(fixture(), 43) }
            assertEquals(42, first.capture().processId)
        }
    }

    @Test fun clockWritesPreserveSignedSecondsAndNativePhase() {
        NimbyClient.open(fixture(), 42).use { client ->
            val result = client.setSimulationDateTime(java.time.Instant.ofEpochSecond(-5))
            assertEquals(java.time.Instant.ofEpochSecond(-5, 170_000_000), result.clock.toInstant())
            assertEquals(0L, result.interventions)
            val recalculated = client.setSimulationDateTime(java.time.Instant.ofEpochSecond(500), true)
            assertEquals(4_294_967_295L, recalculated.interventions)
            assertFailsWith<IllegalArgumentException> { client.setSimulationDateTime(java.time.Instant.ofEpochSecond(1, 1)) }
            val library = com.sun.jna.NativeLibrary.getInstance(fixture().toString())
            val count = library.getFunction("Fixture_MutationCalls")
            val before = count.invokeInt(emptyArray())
            assertEquals(13, assertFailsWith<SdkException> { client.setSimulationDateTime(java.time.Instant.ofEpochSecond(777)) }.status)
            assertEquals(before + 1, count.invokeInt(emptyArray())) // Never retry a failed mutation.
        }
        assertFailsWith<IllegalArgumentException> { SimulationClock(0, -1).toInstant() }
    }
    @Test fun visualCommandsUseExplicitPidAndBoundedLease() {
        val client = NimbyClient.open(fixture(), 42)
        client.showSignalTextureFor(0x8000000000001, "catalogue", "test.svg", 1500)
        client.restoreSignalTexture(0x8000000000001)
        assertFailsWith<IllegalArgumentException> { client.showSignalTextureFor(0x8000000000001, "catalogue", "test.svg", 0) }
        client.close()
        assertFailsWith<IllegalStateException> { client.restoreSignalTexture(0x8000000000001) }
        assertFailsWith<IllegalStateException> { client.setSimulationDateTime(java.time.Instant.EPOCH) }
    }
    @Test fun allModCommandsUseVersionedAbiAndSameOwnerGeneration() {
        NimbyClient.open(fixture(), 42).use { client ->
            val status = client.modControl("test.mod", ControlRequest(ControlOperation.Status))
            assertEquals(99L, status.generation); assertEquals(8191, status.capabilities)
            client.acquireModControl("test.mod").use { control ->
                assertEquals(1500L, control.renew(1500).remainingMillis)
                assertEquals(123, control.forceSignal(0x8000000000001, 123).aspect)
                control.restoreSignal(0x8000000000001)
                assertEquals(4, control.setSetting(0x8000000000001, 4, true).reason)
                control.restoreSetting(0x8000000000001, 4)
                val accepted = control.constrainTrain(0x5000000000001, 6.75, TrainControlMode.PhysicalClearance, 0x8000000000001, true)
                assertEquals(6.75, accepted.speedMps); assertEquals(0x8000000000001, accepted.exitSignal)
                assertEquals(2, control.readTrain(0x5000000000001).active)
                assertEquals("fixture", control.readSignal(0x8000000000001).detail)
                control.restoreTrain(0x5000000000001);control.clear()
                assertFailsWith<IllegalArgumentException> { control.constrainTrain(0x5000000000001, Double.NaN, TrainControlMode.SpeedLimit) }
            }
            assertFailsWith<IllegalArgumentException> { client.modControl("bad/id", ControlRequest(ControlOperation.Status)) }
        }
    }
}
