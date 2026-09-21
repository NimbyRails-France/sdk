package fr.nimby.sdk

import java.nio.file.Path
import kotlin.test.*

class NativeClientTest {
    private fun fixture() = Path.of(requireNotNull(System.getProperty("nrf.fixture")) { "Native fixture was not built" })
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
}
