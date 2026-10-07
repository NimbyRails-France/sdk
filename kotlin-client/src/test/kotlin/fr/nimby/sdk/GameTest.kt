package fr.nimby.sdk

import java.nio.file.Path
import kotlin.test.*

class GameTest {
    @Test fun clockReadDoesNotCaptureTheWholeNetworkAndKeepsLegacySupport() {
        for(legacy in listOf(false, true)) {
            val path = Path.of(requireNotNull(System.getProperty(if(legacy) "nrf.fixture.legacy" else "nrf.fixture")))
            val library = com.sun.jna.NativeLibrary.getInstance(path.toString())
            Nimby.connect(path, 44).use { game ->
                val full = library.getFunction("Fixture_FullCaptures")
                val session = library.getFunction("Fixture_SessionCaptures")
                val release = library.getFunction("Fixture_Released")
                val before = full.invokeInt(emptyArray())
                val scopedBefore = session.invokeInt(emptyArray())
                val releaseBefore = release.invokeInt(emptyArray())
                assertNull(game.clock.read()) // Unknown remains unknown in either ABI.
                assertEquals(before + if(legacy) 1 else 0, full.invokeInt(emptyArray()))
                assertEquals(scopedBefore + if(legacy) 0 else 1, session.invokeInt(emptyArray()))
                assertEquals(releaseBefore + 1, release.invokeInt(emptyArray()))
            }
        }
    }
    @Test fun groupedApiUsesTheSameSessionAndClosesIt() {
        val game = Nimby.connect(Path.of(requireNotNull(System.getProperty("nrf.fixture"))), 42)
        game.use {
            assertEquals(42, it.snapshot().processId)
            assertEquals(it.advanced.capture().clock, it.clock.read())
            assertEquals(it.advanced.capture().trains, it.snapshot().trains)
        }
        assertFailsWith<IllegalStateException> { game.snapshot() }
        assertFailsWith<IllegalStateException> { game.trains.read(0x5000000000001) }
        game.close() // Fermeture idempotente.
    }

    @Test fun metricPositionKeepsOriginAndExplicitDirection() {
        val metric = TrackMetric(0x1000000000001, 800.0)
        assertEquals(Position(metric.trackId, .25, -1), metric.positionAtMetres(200.0, TrackDirection.Backward))
        assertFailsWith<IllegalArgumentException> { metric.positionAtMetres(801.0, TrackDirection.Forward) }
    }
}
