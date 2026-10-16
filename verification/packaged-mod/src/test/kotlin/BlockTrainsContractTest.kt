package nimby.mod

import kotlin.test.*
import nimby.*

private enum class PresenceAspect { Stop, Go }
private enum class PresenceReason { Unknown, Ready }

class BlockTrainsContractTest {
    private val train = 0x5000000010001L
    private val other = 0x5000000020001L // Same slot, another complete identity.
    private val stop = Indication(PresenceAspect.Stop, PresenceReason.Unknown)

    @Test fun occupantTransportIsAnExplicitPerModelRequest() {
        val default = signalModel("presence.default", "Default", "default_images", stop) {
            rules { stop }; images { "stop.svg" }
        }
        val enabled = signalModel("presence.enabled", "Enabled", "enabled_images", stop) {
            observeBlockTrains()
            rules { stop }; images { "stop.svg" }
        }
        assertFalse(default.type.observeBlockTrains)
        assertTrue(enabled.type.observeBlockTrains)
        val legacy = signalMod("presence-legacy-declaration", "Presence", stop) {
            signal("presence.legacy", "Legacy", "legacy_images") {
                observeBlockTrains(); rules { stop }
            }
            images { "stop.svg" }
        }
        assertTrue(legacy.signalTypes.single().observeBlockTrains)
    }

    @Test fun aPositivePresenceSurvivesAnIncompleteListButAbsenceDoesNot() {
        val partial = BlockTrains.observed(listOf(train), complete = false)
        assertEquals(TrainPresence.Present, partial.presenceOf(train))
        assertEquals(TrainPresence.Unknown, partial.presenceOf(other))
        assertEquals(TrainPresence.Unknown, BlockTrains.Unknown.presenceOf(train))
        val complete = BlockTrains.observed(listOf(train), complete = true)
        assertEquals(TrainPresence.Present, complete.presenceOf(train))
        assertEquals(TrainPresence.Absent, complete.presenceOf(other))
        assertEquals(TrainPresence.Absent, BlockTrains.observed(emptyList(), true).presenceOf(train))
    }

    @Test fun anUnknownBoundaryOrStaleObservationCannotProveAbsence() {
        val current = Observation(block = Occupancy.Occupied, fresh = true, routeKnown = true,
            observedBlockTrains = BlockTrains.observed(listOf(train), true))
        assertTrue(current.blockTrains.complete)
        assertEquals(TrainPresence.Present, current.blockTrains.presenceOf(train))
        val prefix = current.copy(routeKnown = false)
        assertFalse(prefix.blockTrains.complete)
        assertEquals(TrainPresence.Present, prefix.blockTrains.presenceOf(train))
        assertEquals(TrainPresence.Unknown, prefix.blockTrains.presenceOf(other))
        assertSame(BlockTrains.Unknown, current.copy(fresh = false).blockTrains)
        assertSame(BlockTrains.Unknown, Observation(fresh = true, routeKnown = true).blockTrains)
        // A vanished head approach must not discard a still observed rear.
        assertEquals(TrainPresence.Present, current.copy(approachingTrain = null).blockTrains.presenceOf(train))
    }

    @Test fun observationsOwnTheirListsAndUseFullTrainIdentities() {
        val input = mutableListOf(other, train, other)
        val observed = BlockTrains.observed(input, true)
        input.clear()
        assertEquals(listOf(train, other), observed.trainIds)
        assertEquals(TrainPresence.Present, observed.presenceOf(other))
        assertEquals(TrainPresence.Absent, observed.presenceOf(0x5000000030001L))
        for (invalid in listOf(0L, -1L, 1L, 0x5000000000000L, 0x8000000010001L)) {
            assertEquals(TrainPresence.Unknown, observed.presenceOf(invalid))
            assertFailsWith<IllegalArgumentException> { BlockTrains.observed(listOf(train, invalid), true) }
        }
        assertFailsWith<IllegalArgumentException> { BlockTrains.observed(List(4097) { train }, true) }
        assertEquals(4096, BlockTrains.observed(List(4096) { train + it }, true).trainIds.size)
    }

    @Test fun modelContextsMaskStaleAndInvalidApproachesWithoutLosingCurrentRearPresence() {
        var seen: BlockTrains? = null
        val model = signalModel("presence.model", "Presence", "presence_images", stop) {
            rules {
                seen = blockTrains
                if (blockTrains.presenceOf(train) == TrainPresence.Absent)
                    Indication(PresenceAspect.Go, PresenceReason.Ready) else stop
            }
            images { "stop.svg" }
        }
        val mod = signalMod("presence-contract", "Presence") { signal(model) }
        fun decide(observation: Observation, settings: SettingsStatus = SettingsStatus.Present) =
            mod.decide(Signal(type = model.type.id, observation = observation, settingsStatus = settings), null)
        val occupied = Observation(fresh = true, routeKnown = true, observedBlockTrains = BlockTrains.observed(listOf(train), true))
        decide(occupied)
        assertEquals(TrainPresence.Present, assertNotNull(seen).presenceOf(train))
        decide(occupied.copy(fresh = false))
        assertSame(BlockTrains.Unknown, seen)
        decide(occupied, SettingsStatus.Unavailable)
        assertSame(BlockTrains.Unknown, seen)
        decide(occupied.copy(approachingTrain = 1L))
        assertSame(BlockTrains.Unknown, seen)
        decide(occupied.copy(approachingTrain = null))
        assertEquals(TrainPresence.Present, assertNotNull(seen).presenceOf(train))
        val empty = occupied.copy(block = Occupancy.Clear, observedBlockTrains = BlockTrains.observed(emptyList(), true))
        decide(empty)
        assertEquals(TrainPresence.Absent, assertNotNull(seen).presenceOf(train))
        decide(empty.copy(routeKnown = false))
        assertEquals(TrainPresence.Unknown, assertNotNull(seen).presenceOf(train))
    }

    @Test fun legacySignalContextsReceiveTheSameFacts() {
        var seen: BlockTrains? = null
        val mod = signalMod("presence-legacy", "Presence", stop) {
            signal("presence.legacy", "Presence", "presence_images") {
                rules { seen = blockTrains; stop }
            }
            images { "stop.svg" }
        }
        val current = Observation(fresh = true, routeKnown = true,
            observedBlockTrains = BlockTrains.observed(listOf(train, other), true))
        mod.decide(Signal(type = "presence.legacy", observation = current), null)
        assertEquals(listOf(train, other), assertNotNull(seen).trainIds)
        mod.decide(Signal(type = "presence.legacy", observation = current.copy(fresh = false)), null)
        assertSame(BlockTrains.Unknown, seen)
    }
}
