package fr.nimby.sdk

import java.nio.file.Path
import kotlin.test.*

class GameTest {
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
