package nimby.mod

import kotlin.test.*
import nimby.*

private enum class ApproachAspect { Closed, Open }
private enum class ApproachReason { Unavailable, Waiting, Ready }
private val approachClosed = Indication(ApproachAspect.Closed, ApproachReason.Unavailable)
private val approachOpen = Indication(ApproachAspect.Open, ApproachReason.Ready)

/** Test data may contain native identities; signal rules only consume the
 * validated high-level presence and freshness supplied by the SDK. */
class SignalApproachNormalizationContractTest {
    private val train = 0x5000000010001L
    private val invalidTrains = listOf(0L, 1L, -1L, 0x5000000000000L, 0x1000000010001L, 0x8000000010001L)

    @Test fun typedIsolatedEvaluationReceivesTheValidatedObservationAndTypedNeighbour() {
        var received: Observation? = null
        var neighbour: ApproachAspect? = null
        val model = signalModel("approach.typed", "Typed approach", "approach_images", approachClosed) {
            observeApproach(blocks = 2)
            rules { approachClosed }
            evaluate { _: Map<String, Boolean>, current: Observation, next: ApproachAspect ->
                received = current
                neighbour = next
                if (current.fresh && current.approachingTrain != null) approachOpen else approachClosed
            }
            images { if (it.aspect == ApproachAspect.Open) "open.svg" else "closed.svg" }
        }
        val mod = signalMod("approach-typed-contract", "Approach") { signal(model) }
        fun decide(current: Observation) = assertNotNull(mod.indication(mod.evaluate(model.type.id, emptyMap(), current)))
        val valid = Observation(block = Occupancy.Clear, fresh = true, routeKnown = true,
            next = ApproachAspect.Open.ordinal, approachingTrain = train)
        assertEquals(ApproachAspect.Open, decide(valid).aspect)
        assertSame(valid, received) // Valid input needs no normalization allocation.
        assertEquals(ApproachAspect.Open, neighbour)
        for (id in invalidTrains) {
            val invalid = valid.copy(approachingTrain = id, forcedStop = true, lampFailed = true)
            assertEquals(ApproachAspect.Closed, decide(invalid).aspect)
            val checked = assertNotNull(received)
            assertFalse(checked.fresh)
            assertNull(checked.approachingTrain)
            assertTrue(checked.forcedStop)
            assertTrue(checked.lampFailed)
            assertEquals(Occupancy.Clear, checked.block)
            assertEquals(ApproachAspect.Open, neighbour)
            assertEquals(id, invalid.approachingTrain) // The caller's value is immutable.
            assertTrue(invalid.fresh)
        }
        assertEquals(ApproachAspect.Closed, decide(valid.copy(fresh = false)).aspect)
        assertEquals(ApproachAspect.Closed, decide(valid.copy(approachingTrain = null)).aspect)
    }

    @Test fun untypedIsolatedEvaluationUsesTheSameApproachValidation() {
        var received: Observation? = null
        val model = signalModel("approach.isolated", "Isolated approach", "approach_images", approachClosed) {
            rules { approachClosed }
            evaluate { _: Map<String, Boolean>, current: Observation ->
                received = current
                if (current.fresh && current.approachingTrain != null) approachOpen else approachClosed
            }
            images { "closed.svg" }
        }
        val mod = signalMod("approach-isolated-contract", "Approach") { signal(model) }
        val valid = Observation(fresh = true, approachingTrain = train)
        assertEquals(ApproachAspect.Open, mod.indication(mod.evaluate(emptyMap(), valid))?.aspect)
        assertSame(valid, received)
        for (id in invalidTrains) {
            assertEquals(ApproachAspect.Closed, mod.indication(mod.evaluate(emptyMap(), valid.copy(approachingTrain = id)))?.aspect)
            assertFalse(assertNotNull(received).fresh)
            assertNull(received?.approachingTrain)
        }
    }

    @Test fun legacyIsolatedEvaluationAlsoReceivesValidatedApproachWithoutMutatingInput() {
        var received: Observation? = null
        val mod = signalMod("approach-legacy-contract", "Legacy approach", approachClosed) {
            signal("approach.legacy", "Approach", "approach_images") {
                observeApproach(blocks = 2)
                rules { if (fresh && trainApproaching) approachOpen else approachClosed }
                evaluate { _: Map<String, Boolean>, current: Observation ->
                    received = current
                    if (current.fresh && current.approachingTrain != null) approachOpen else approachClosed
                }
            }
            images { if (it.aspect == ApproachAspect.Open) "open.svg" else "closed.svg" }
        }
        // The legacy DSL reads its typed decision through the texture callback.
        fun image(current: Observation) = mod.texture(mod.evaluate("approach.legacy", emptyMap(), current), 0, 500)
        val valid = Observation(fresh = true, routeKnown = true, approachingTrain = train)
        assertEquals("open.svg", image(valid))
        assertSame(valid, received)
        for (id in invalidTrains) {
            val invalid = valid.copy(approachingTrain = id, forcedStop = true)
            assertEquals("closed.svg", image(invalid))
            val checked = assertNotNull(received)
            assertFalse(checked.fresh)
            assertNull(checked.approachingTrain)
            assertTrue(checked.forcedStop)
            assertEquals(id, invalid.approachingTrain)
            assertTrue(invalid.fresh)
            assertEquals("closed.svg", mod.texture(assertNotNull(mod.decide(Signal(type = "approach.legacy",
                observation = invalid), null)), 0, 500))
        }
        assertEquals("closed.svg", image(valid.copy(fresh = false)))
        assertEquals("closed.svg", image(valid.copy(approachingTrain = null)))
    }

    @Test fun liveAndIsolatedRulesConsumeFreshApproachAndDeclaredOptionsWithoutDecodingIds() {
        var observedTrain: Long? = null
        var observedFresh = false
        val model = signalModel("approach.rules", "Approach rules", "approach_images", approachClosed) {
            val automatic = checkbox("automatic", "Open on approach", defaultValue = true)
            observeApproach(blocks = 2)
            rules {
                observedTrain = approachingTrain
                observedFresh = fresh
                when {
                    !fresh || !routeKnown || observation.forcedStop -> approachClosed
                    !enabled(automatic) || !trainApproaching -> Indication(ApproachAspect.Closed, ApproachReason.Waiting)
                    else -> approachOpen
                }
            }
            images { "closed.svg" }
        }
        val mod = signalMod("approach-rules-contract", "Approach") { signal(model) }
        fun live(current: Observation, settings: Map<String, Boolean> = emptyMap(), status: SettingsStatus = SettingsStatus.Absent) =
            assertNotNull(mod.indication(assertNotNull(mod.decide(Signal(type = model.type.id,
                settings = settings, settingsStatus = status, observation = current), null))))
        val valid = Observation(fresh = true, routeKnown = true, approachingTrain = train)
        assertEquals(ApproachAspect.Open, live(valid).aspect)
        assertEquals(train, observedTrain)
        assertTrue(observedFresh)
        assertEquals(ApproachAspect.Open, mod.indication(mod.evaluate(model.type.id, emptyMap(), valid))?.aspect)
        for (id in invalidTrains) {
            assertEquals(ApproachAspect.Closed, live(valid.copy(approachingTrain = id)).aspect)
            assertFalse(observedFresh)
            assertNull(observedTrain)
            assertEquals(ApproachAspect.Closed, mod.indication(mod.evaluate(model.type.id, emptyMap(), valid.copy(approachingTrain = id)))?.aspect)
        }
        assertEquals(ApproachAspect.Closed, live(valid.copy(fresh = false)).aspect)
        assertNull(observedTrain)
        assertEquals(ApproachReason.Waiting, live(valid.copy(approachingTrain = null)).reason)
        assertEquals(ApproachReason.Waiting, live(valid, mapOf("automatic" to false), SettingsStatus.Present).reason)
        assertEquals(ApproachAspect.Closed, live(valid, status = SettingsStatus.Unavailable).aspect)
        assertFalse(observedFresh)
        assertNull(observedTrain)
        assertEquals(ApproachAspect.Closed, live(valid.copy(forcedStop = true)).aspect)
        assertEquals(ApproachAspect.Closed, live(valid.copy(routeKnown = false)).aspect)
    }
}
