package nimby.mod

import kotlin.test.*
import nimby.*
import nimby.internal.ToolAccess

private enum class Aspect { Closed, Open }
private enum class Reason { Unknown, Approach }

class ContractTest {
    @Test fun stalePanelDoesNotInvalidateObservationButConstructionStillFails() {
        val calls = mutableListOf<Int>()
        val request = SignalActionRequest(1, 0x8000000000001L, "preview", "repeat.v1", "world", 1, 10, "repeat")
        ToolAccess.withContext("world", 1, { op, _, _, _ -> calls.add(op); if (op == 7) 0 else 9 }) { context ->
            context.showPanel(request, "Preview", emptyList())
            context.showPanel(request, "Preview", emptyList(), listOf(ToolNumberInput("spacing", "Spacing", 100, 10, 1000)))
            assertEquals(listOf(6, 8), calls)
            assertFailsWith<IllegalStateException> { context.prepareConstruction(request.signalId) }
            assertEquals(listOf(6, 8, 5, 7), calls)
        }
        ToolAccess.withContext("world", 1, { _, _, _, _ -> 1 }) { context ->
            assertFailsWith<IllegalStateException> { context.showPanel(request, "Preview", emptyList()) }
        }
    }

    @Test fun installedApiBuildsAService() {
        assertEquals(listOf("contract.v1"), createMod().services)
    }

    @Test fun blinkingUsesTheDeclaredSimulationDuration() {
        val flash = blink("on.svg", "off.svg", everyMs = 250)
        assertEquals(listOf("on.svg", "on.svg", "off.svg", "off.svg", "on.svg"),
            listOf(0L, 249L, 250L, 499L, 500L).map(flash::frameAt))
        repeat(3) { assertEquals("off.svg", flash.frameAt(250)) } // Paused clock.
        assertEquals("off.svg", flash.frameAt(9_000_000_000_250L))
        assertEquals("fixed.svg", steady("fixed.svg").frameAt(Long.MAX_VALUE))
        for (duration in listOf(0L, 99L, 10001L, Long.MAX_VALUE))
            assertFailsWith<IllegalArgumentException> { blink("on.svg", "off.svg", duration) }
        assertFailsWith<IllegalArgumentException> { flash.frameAt(-1) }
        for (path in listOf("", " ", "bad\u0000.svg", "x".repeat(96)))
            assertFailsWith<IllegalArgumentException> { steady(path) }
    }

    @Test fun modelsExposeValidatedApproachAndDeclarativeImages() {
        val flash = blink("on.svg", "off.svg", everyMs = 250)
        val closed = steady("closed.svg")
        val model = signalModel("contract.approach", "Approach", "contract_images",
            Indication(Aspect.Closed, Reason.Unknown)) {
            observeApproach(blocks = 2)
            rules {
                if (trainApproaching) Indication(Aspect.Open, Reason.Approach)
                else Indication(Aspect.Closed, Reason.Unknown)
            }
            appearance { if (it.aspect == Aspect.Open) flash else closed }
        }
        val mod = signalMod("contract-signal", "Contract") { signal(model) }
        assertTrue(mod.signalTypes.single().observeApproach)
        assertEquals(2, mod.signalTypes.single().approachBlocks)
        fun decide(id: Long?, fresh: Boolean = true) = assertNotNull(mod.decide(Signal(
            type = model.type.id, observation = Observation(fresh = fresh, approachingTrain = id)), null))
        val train = 0x5000000010001L
        val open = decide(train)
        assertEquals(Aspect.Open, mod.indication(open)?.aspect)
        assertSame(flash, mod.animation(open))
        assertEquals("off.svg", mod.texture(open, 250, 500)) // Runtime default must not replace 250.
        assertEquals("on.svg", mod.texture(open, 500, 500))
        for (id in listOf(null, 0L, 1L, -1L, 0x8000000010001L))
            assertEquals(Aspect.Closed, mod.indication(decide(id))?.aspect)
        assertEquals(Aspect.Closed, mod.indication(decide(train, fresh = false))?.aspect)
        assertFailsWith<IllegalArgumentException> {
            signalModel("contract.bad", "Bad", "bad", Indication(Aspect.Closed, Reason.Unknown)) {
                observeApproach(blocks = 17)
                rules { Indication(Aspect.Closed, Reason.Unknown) }; images { "closed.svg" }
            }
        }
    }
}
