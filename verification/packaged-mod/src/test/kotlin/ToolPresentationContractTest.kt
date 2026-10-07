package nimby.contract

import kotlin.test.*
import nimby.*
import nimby.internal.ToolAccess

class ToolPresentationContractTest {
    private val request = SignalActionRequest(1, 0x8000000000001L, "preview", "repeat.v1", "world", 1, 10, "repeat")

    @Test fun panelCodecPreservesEmptyAndMaximumPayloadLayouts() {
        for (buttonCount in listOf(0, 12)) for (inputCount in listOf(0, 4)) {
            val buttons = List(buttonCount) { ToolButton("button$it", "Étiquette 🚆 $it", it % 2 == 0) }
            val inputs = List(inputCount) { ToolNumberInput("number$it", "Distance $it", it, -100, 100, it % 2 == 0) }
            val message = "é".repeat(128) // 256 UTF-8 bytes, not 256 characters.
            var calls = 0
            ToolAccess.withContext("world", 1, { op, integers, numbers, text ->
                calls++
                assertEquals(if (inputCount == 0) 6 else 8, op)
                val expected = mutableListOf(10L, request.signalId, buttonCount.toLong())
                if (inputCount != 0) expected += inputCount.toLong()
                expected += buttons.map { if (it.enabled) 1L else 0L }
                expected += inputs.flatMap { listOf(it.value.toLong(), it.minimum.toLong(), it.maximum.toLong(), if (it.enabled) 1L else 0L) }
                assertContentEquals(expected.toLongArray(), integers)
                assertTrue(numbers.isEmpty())
                val fields = listOf("repeat.v1", "repeat", message) + buttons.flatMap { listOf(it.id, it.label) } + inputs.flatMap { listOf(it.id, it.label) }
                assertContentEquals((fields.joinToString("\u0000") + "\u0000").encodeToByteArray(), text)
                0
            }) { it.showPanel(request, message, buttons, inputs) }
            assertEquals(1, calls)
        }
    }

    @Test fun invalidPanelFieldsNeverReachTheTransport() {
        ToolAccess.withContext("world", 1, { _, _, _, _ -> error("Malformed panel reached transport") }) { context ->
            val button = ToolButton("apply", "Confirmer")
            val input = ToolNumberInput("spacing", "Espacement", 1000, 3, 100_000)
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "é".repeat(129), listOf(button)) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "bad\u0000text", listOf(button)) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "", List(13) { button.copy(id="b$it") }) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "", listOf(button, button)) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "", listOf(button), listOf(input.copy(id="apply"))) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "", emptyList(), List(5) { input.copy(id="n$it") }) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "", emptyList(), listOf(input.copy(value=2))) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request, "", listOf(button.copy(id="bad id"))) }
            assertFailsWith<IllegalArgumentException> { context.showPanel(request.copy(generation=2), "", listOf(button)) }
        }
    }

    @Test fun previewCodecCopiesMaximumPayloadAndValidatesBeforePublishing() {
        val positions = List(64) { SignalPosition(0x1000000000001L + it * 0x10000L, (it + 1.0) / 65, if (it % 2 == 0) -1 else 1) }
        var calls = 0
        lateinit var retained: ToolContext
        ToolAccess.withContext("world", 1, { op, integers, numbers, text ->
            calls++
            assertEquals(9, op)
            if (calls == 1) {
                val expected = longArrayOf(10, request.signalId, 64) + positions.flatMap { listOf(it.trackId, it.direction.toLong()) }
                assertContentEquals(expected, integers)
                assertContentEquals(positions.map { it.fraction }.toDoubleArray(), numbers)
                assertContentEquals("repeat.v1\u0000repeat\u0000".encodeToByteArray(), text)
            } else {
                assertContentEquals(longArrayOf(0, 0, 0), integers)
                assertTrue(numbers.isEmpty() && text.isEmpty())
            }
            0
        }) { context ->
            retained = context
            context.showSignalPreview(request, positions)
            for (invalid in listOf(positions.last().copy(direction=0), positions.last().copy(fraction=Double.NaN),
                positions.last().copy(fraction=0.0), positions.last().copy(trackId=request.signalId))) {
                assertFailsWith<IllegalArgumentException> { context.showSignalPreview(request, positions.dropLast(1) + invalid) }
            }
            assertFailsWith<IllegalArgumentException> { context.showSignalPreview(request, positions + positions.last()) }
            context.showSignalPreview(request, emptyList())
        }
        assertEquals(2, calls)
        assertFailsWith<IllegalStateException> { retained.showSignalPreview(request, positions) }
        assertFailsWith<IllegalStateException> { retained.showPanel(request, "", emptyList()) }
    }
}
