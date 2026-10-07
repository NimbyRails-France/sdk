package nimby.mod

import kotlin.test.*
import nimby.*
import nimby.internal.ToolAccess

class TrainObservationContractTest {
    @Test fun compositionsAreOptInPagedAndDoNotMixConfiguredAndCurrentVehicles() {
        var pages = 0
        lateinit var retained: TrainSnapshot
        ToolAccess.withContext("map", 1, { op, values, numbers, text ->
            when(op) {
                20 -> { assertEquals(161L, values[0]); values[0] = 1; values[3] = Long.MIN_VALUE }
                29 -> { values[0] = 4096; values[1] = 1 }
                31 -> { values[2] = 101; "metro".encodeToByteArray().copyInto(text); "Metro vehicle".encodeToByteArray().copyInto(text, 257) }
                30 -> {
                    pages++; val offset = values[0]; val count = values[1].toInt()
                    repeat(count) { row -> val at = 2 + row * 4
                        values[at] = 0x5000000000001L; values[at + 1] = if(row == 0) 101 else 999
                        values[at + 2] = offset + row; values[at + 3] = 0
                    }
                }
                21 -> { values.fill(0, 2); numbers.fill(Double.NaN); values[2] = 0x5000000000001L; values[24] = -1; values[32] = Long.MIN_VALUE }
                28 -> { values.fill(-1, 2); numbers.fill(Double.NaN); values[4] = 1; values[7] = 1 }
                else -> error("Unexpected data call $op")
            }; 0
        }) { retained = it.trains(TrainQuery(includeComposition = true)) }
        val train = retained.trains.single()
        val configured = assertNotNull(train.configured?.composition)
        assertEquals(4096, configured.size); assertEquals(4095, configured.last().index)
        assertEquals("metro", configured.first().model.code); assertEquals("Metro vehicle", configured.first().model.nameEnglish)
        assertEquals(VehicleModelId(999), configured.last().modelId); assertNull(configured.last().model.code)
        assertEquals(emptyList(), train.current?.composition); assertNull(train.configured?.maximumSpeedMps)
        assertEquals(128, pages)
    }

    @Test fun unknownAndMalformedCompositionsNeverBecomeEmptyOrEraseTheOtherProfile() {
        val first = 0x5000000000001L
        fun capture(available: Boolean): TrainSnapshot {
            lateinit var result: TrainSnapshot
            // This contract is compiled against the installed klib. Exercise
            // the public query instead of reaching its internal decoder class.
            ToolAccess.withContext("map", 1, { op, values, numbers, _ ->
                when(op) {
                    20 -> { values[0] = 2; values[3] = Long.MIN_VALUE }
                    29 -> { values[0] = if(available) 3 else -1; values[1] = -1 }
                    30 -> {
                        val rows = arrayOf(longArrayOf(first, 10, 0, 0), longArrayOf(first, 11, 0, 0), longArrayOf(first, 12, 0, 1))
                        rows.forEachIndexed { row, fields -> fields.copyInto(values, 2 + row * 4) }
                    }
                    21 -> {
                        values.fill(0, 2); numbers.fill(Double.NaN)
                        repeat(2) { row -> values[2 + row * 32] = first + row; values[32 + row * 32] = Long.MIN_VALUE }
                    }
                    28 -> {
                        values.fill(-1, 2); numbers.fill(Double.NaN)
                        repeat(2) { row -> values[4 + row * 6] = 1; values[7 + row * 6] = 1 }
                    }
                    else -> error("Unexpected composition call $op")
                }; 0
            }) { result = it.trains(TrainQuery(includeComposition = true)) }
            return result
        }
        val unknown = capture(false)
        assertNull(unknown.trains[0].configured?.composition)
        assertNull(unknown.trains[1].current?.composition)
        val observed = capture(true)
        assertNull(observed.trains[0].configured?.composition) // Duplicate configured index.
        assertEquals(VehicleModelId(12), observed.trains[0].current?.composition?.single()?.modelId)
        assertEquals(emptyList(), observed.trains[1].current?.composition)
    }

    @Test fun largeTrainCollectionsArePagedJoinedAndOwnedAfterCallback() {
        var pages = 0
        lateinit var retained: TrainSnapshot
        lateinit var closed: ToolContext
        val count = 16_385
        ToolAccess.withContext("map", 19, { op, values, numbers, text ->
            when(op) {
                20 -> { values[0] = count.toLong(); values[1] = 1000; values[2] = 12; values[3] = 1234; values[4] = 5000 }
                21 -> {
                    val offset = values[0].toInt(); val size = values[1].toInt()
                    assertTrue(size in 1..32); assertEquals(32 * 5 * 257, text.size)
                    pages++
                    repeat(size) { row ->
                        val at = 2 + row * 32; val number = row * 5
                        values.fill(0, at, at + 32); text.fill(0, row * 1285, (row + 1) * 1285)
                        values[at] = 0x5000000000000L + offset + row + 1
                        values[at + 1] = 21; values[at + 2] = -1; numbers[number] = .25; numbers[number + 1] = 20.0
                        values[at + 4] = 1; values[at + 5] = 6; values[at + 6] = 6
                        values[at + 7] = 0; values[at + 8] = 1; values[at + 9] = 21; values[at + 10] = 31
                        values[at + 11] = 41; values[at + 12] = 22; values[at + 13] = 32; values[at + 14] = 0; values[at + 15] = 1
                        values[at + 16] = 1000; values[at + 17] = 0; values[at + 18] = -1; values[at + 19] = 0; values[at + 20] = 1
                        numbers[number + 2] = -0.000001; numbers[number + 3] = 0.0; numbers[number + 4] = 0.000001
                        values[at + 21] = 1; values[at + 22] = 0; values[at + 23] = 61; values[at + 24] = 71; values[at + 25] = 0; values[at + 28] = 31
                        values[at + 30] = Long.MIN_VALUE
                        listOf("Train ${offset + row}", "Ligne", "Départ", "Arrivée", "Position").forEachIndexed { field, value ->
                            value.encodeToByteArray().copyInto(text, row * 1285 + field * 257)
                        }
                    }
                }
                else -> error("Unexpected operation $op")
            }; 0
        }) { context ->
            closed = context; retained = context.trains()
            repeat(100) { assertSame(retained, context.trains()) }
        }
        assertEquals(513, pages); assertEquals(count, retained.trains.size)
        val train = assertNotNull(retained[0x5000000004001L])
        assertEquals("Train 16384", train.name); assertEquals(72.0, train.speedKmh)
        assertEquals("Position", train.position?.station?.name); assertEquals(0, train.passengers)
        assertEquals(0, train.assignment?.orderIndex); assertEquals(TrainState.SignalWait, train.service?.state)
        assertEquals(TrainAlert.SignalWait, train.service?.alert); assertEquals(false, train.service?.hidden)
        assertEquals(true, train.service?.line?.isDepot); assertEquals("Arrivée", train.service?.stopStation?.name)
        assertEquals(GameInstant(999, 999999), train.service?.times?.arrival)
        assertEquals(GameInstant(1000), train.service?.times?.departure)
        assertNull(retained[0x5000000004002L]); assertEquals(12L, retained.ageMillis)
        assertFailsWith<IllegalStateException> { closed.trains() }
    }

    @Test fun unavailableValuesAndFallbackSpeedNeverBecomeKnownZero() {
        ToolAccess.withContext("map", 1, { op, values, numbers, _ ->
            when(op) {
                20 -> { values[0] = 1; values[3] = Long.MIN_VALUE }
                21 -> {
                    values.fill(0, 2); numbers.fill(Double.NaN)
                    values[2] = 0x5000000000001L; values[5] = 1 // Defaulted speed.
                    numbers[1] = 0.0
                    values[6] = 1 // Service exists, all its observations unknown.
                    for(field in listOf(5,6,7,8,15,22,26,27)) values[2 + field] = -1
                    for(field in listOf(14,16,17,18,19,20,25,30)) values[2 + field] = Long.MIN_VALUE
                }
            }; 0
        }) { context ->
            val result = context.trains(); val train = result.trains.single()
            assertNull(result.clock); assertNull(train.position); assertNull(train.speedMps); assertTrue(train.speedDefaulted)
            assertNull(train.passengers); assertNull(train.assignment); assertNull(train.service?.state)
            assertNull(train.service?.hidden); assertNull(train.service?.times?.arrival); assertNull(train.service?.line)
        }
        val times = TrainServiceTimes(Long.MAX_VALUE, 1_000_000, null, null, null, null, null, null)
        assertNull(times.observedAt)
        assertFailsWith<IllegalArgumentException> { GameInstant(0, -1) }
    }

    @Test fun lineOffsetsStayRelativeAndMissingPlansStayNull() {
        var calls = 0
        lateinit var plan: TrainLinePlan
        ToolAccess.withContext("map", 1, { op, values, _, text ->
            calls++
            when(op) {
                22 -> { values[1] = if(values[0] == 0x5000000000002L) -1 else 2; values[2] = 41; values[3] = -1; values[4] = 1234 }
                23 -> {
                    values[3] = 0; values[4] = 21; values[5] = 31; values[6] = 120; values[7] = 150
                    values[8] = 1; values[9] = 22; values[10] = 0; values[11] = Long.MIN_VALUE; values[12] = Long.MIN_VALUE
                    "Gare".encodeToByteArray().copyInto(text)
                }
                else -> error("Unexpected $op")
            }; 0
        }) { context ->
            plan = assertNotNull(context.linePlan(0x5000000000001L))
            assertNull(context.linePlan(0x5000000000002L))
            assertSame(plan, context.linePlan(0x5000000000001L))
            assertNull(context.linePlan(0x5000000000002L))
            assertFailsWith<IllegalArgumentException> { context.linePlan(1) }
        }
        assertEquals(3, calls); assertEquals(30L, plan.stops[0].plannedDwellSeconds)
        assertEquals(120, plan.stops[0].arrivalOffsetSeconds); assertNull(plan.stops[1].station)
        assertNull(plan.stops[1].arrivalOffsetSeconds); assertNull(plan.line.isDepot)
    }

    @Test fun explicitNetworkRefreshInvalidatesPreviouslyCopiedTrainData() {
        var count = 0
        ToolAccess.withContext("map", 1, { op, values, _, _ ->
            when(op) {
                20 -> { values[0] = 0; values[1] = (++count).toLong(); values[3] = Long.MIN_VALUE }
                1, 2, 3, 4, 11 -> Unit // Empty refreshed network / explicit time change.
                else -> error("Unexpected operation $op")
            }; 0
        }) { context ->
            val first = context.trains()
            assertSame(first, context.trains())
            context.network()
            val second = context.trains()
            assertNotSame(first, second); assertEquals(1L, first.capturedAtMillis); assertEquals(2L, second.capturedAtMillis)
            context.changeTime(100)
            assertEquals(3L, context.trains().capturedAtMillis)
        }
        assertEquals(3, count)
    }

    @Test fun queriesOnlyAcquireRequestedGroupsAndReuseASuperset() {
        val requests = ArrayList<Long>()
        ToolAccess.withContext("map", 1, { op, values, _, _ ->
            when(op) {
                20 -> { requests += values[0]; values[0] = 0; values[3] = Long.MIN_VALUE }
                24 -> values.fill(-1)
                else -> error("Unrequested data call $op")
            }; 0
        }) { context ->
            context.trains()
            context.trains(TrainQuery(includeCharacteristics = true))
            val all = context.trains(TrainQuery(includeTags = true))
            assertSame(all, context.trains())
            assertSame(all, context.trains(TrainQuery(includeCharacteristics = true)))
        }
        assertEquals(listOf(33L, 35L, 107L), requests)
    }

    @Test fun maximumTagsPerObjectStayCompleteAndCharacteristicsRemainIndependent() {
        var associations = 0
        ToolAccess.withContext("map", 1, { op, values, numbers, text ->
            when(op) {
                20 -> { assertEquals(107L, values[0]); values[0] = 32; values[3] = Long.MIN_VALUE }
                24 -> values.fill(0)
                21 -> repeat(32) { row ->
                    val at = 2 + row * 32; values.fill(0, at, at + 32)
                    values[at] = 0x5000000000001L + row; values[at + 22] = -1
                    values[at + 29] = 4096; values[at + 30] = -1_500_000
                    numbers.fill(Double.NaN); text.fill(0)
                }
                27 -> {
                    associations++; assertEquals(1L, values[0]); assertEquals(4098, values.size)
                    repeat(4096) { values[2 + it] = it + 1L }
                }
                28 -> {
                    numbers.fill(Double.NaN); values.fill(-1, 2)
                    repeat(32) { row -> values[2 + row * 6] = 8; values[3 + row * 6] = 500; numbers[row * 12] = 40.0; numbers[row * 12 + 6] = 35.0 }
                }
                else -> error("Unexpected $op")
            }; 0
        }) { context ->
            val snapshot = context.trains(TrainQuery(includeCharacteristics = true, includeTags = true))
            val train = assertNotNull(snapshot[TrainId(0x5000000000001)])
            assertEquals(4096, train.declaredTags?.size); assertEquals(TagId(4096), train.declaredTags?.last()?.id)
            assertNull(train.declaredTags?.first()?.name); assertEquals(-1.5, train.predictedArrivalDelaySeconds)
            assertEquals(144.0, train.configured?.maximumSpeedKmh); assertEquals(126.0, train.current?.maximumSpeedKmh)
            assertEquals(500, train.configured?.passengerCapacity); assertNull(train.current?.passengerCapacity)
            assertNull(train.current?.lengthM); assertNull(train.speedMps)
        }
        assertEquals(32, associations)
    }

    @Test fun allLinesAndTypedTagsPreserveUnknownParentChainsAndCycles() {
        ToolAccess.withContext("map", 1, { op, values, _, text ->
            when(op) {
                20 -> { values[0] = 0; values[3] = Long.MIN_VALUE }
                24 -> { values[0] = 6; values[1] = 2 }
                26 -> { values[2] = 7; values[3] = 8; "Second".encodeToByteArray().copyInto(text, 257) }
                25 -> repeat(6) { row ->
                    val at = 2 + row * 6; values[at] = row + 91L; values[at + 1] = when(row) { 1 -> 91L; 2 -> 9999L; 3 -> 95L; 4 -> 94L; else -> 0L }
                    values[at + 2] = if(row == 5) 0 else 1; values[at + 3] = if(row == 0) 1 else 0
                    values[at + 4] = when(row) { 0 -> 1; 1 -> 2; 5 -> -1; else -> 0 }; values[at + 5] = 1
                }
                27 -> { assertEquals(2L, values[0]); values[3] = 7; values[4] = 8; values[5] = 7 }
                else -> error("Unexpected $op")
            }; 0
        }) { context ->
            val snapshot = context.trains(TrainQuery(includeTags = true))
            assertEquals(6, snapshot.lines?.size); assertEquals(LineType.Depot, snapshot.line(LineId(91))?.type)
            assertEquals("", snapshot.tag(TagId(7))?.name) // Known empty label differs from unavailable catalog name.
            assertEquals(listOf(TagId(8), TagId(7)), snapshot.tagsForLine(LineId(92))?.map { it.id })
            assertNull(snapshot.tagsForLine(LineId(93))); assertNull(snapshot.tagsForLine(LineId(94))); assertNull(snapshot.tagsForLine(LineId(96)))
            assertSame(snapshot.tagsForLine(LineId(92)), snapshot.tagsForLine(LineId(92)))
        }
    }
}
