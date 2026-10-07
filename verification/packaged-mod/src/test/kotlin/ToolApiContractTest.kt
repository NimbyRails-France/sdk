package nimby.mod

import kotlin.test.*
import nimby.*
import nimby.internal.ToolAccess

class ToolApiContractTest {
    @Test fun copiedSignalsKeepTheRequestedTravelDirectionForEveryNativeOrientation() {
        val track = 0x1000000000001L
        for(kind in listOf(0, 1, 2, 3, 4)) for(nativeDirection in listOf(-1, 1)) {
            val source = ToolSignal(0x8000000000001L, track, 0.25, nativeDirection, kind)
            assertEquals(if(kind == 4) -nativeDirection else nativeDirection, source.travelDirection)
            val same = source.placementAt(track, 0.5)
            assertEquals(nativeDirection, same.direction)
            for(travel in listOf(-1, 1)) {
                val position = source.placementAt(track, 0.75, travel)
                val copied = ToolSignal(0x8000000000002L, position.trackId, position.fraction, position.direction, kind)
                assertEquals(travel, copied.travelDirection)
            }
            for(fraction in listOf(0.0, 1.0, Double.NaN, Double.POSITIVE_INFINITY))
                assertFailsWith<IllegalArgumentException> { source.placementAt(track, fraction) }
            assertFailsWith<IllegalArgumentException> { source.placementAt(1, 0.5) }
            assertFailsWith<IllegalArgumentException> { source.placementAt(track, 0.5, 0) }
        }
    }

    @Test fun changingADateUsesTheSameValidatedOperationAndExplicitRecalculationFlag() {
        val date = GameDateTime(2024, 2, 29, 23, 59, 58)
        var calls = 0
        ToolAccess.withContext("world", 1, { operation, integers, _, _ ->
            assertEquals(11, operation)
            assertEquals(date.toUtcSeconds(), integers[0])
            assertEquals(calls.toLong(), integers[1])
            integers[1] = 123
            integers[2] = if(calls++ == 0) 0 else 8
            0
        }) { context ->
            val kept = context.changeTime(date)
            assertEquals(date, kept.clock.dateTime())
            assertEquals(123L, kept.clock.elapsedMillis)
            val recalculated = context.changeTime(date, recalculateTrains = true)
            assertEquals(date, recalculated.clock.dateTime())
        }
        assertEquals(2, calls)
    }
}
