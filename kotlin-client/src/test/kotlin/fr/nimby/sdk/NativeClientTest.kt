package fr.nimby.sdk

import java.nio.file.Path
import kotlin.test.*

class NativeClientTest {
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
