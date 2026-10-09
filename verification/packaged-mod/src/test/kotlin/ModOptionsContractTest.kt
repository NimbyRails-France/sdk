package nimby.contract

import kotlin.test.*
import nimby.*
import nimby.internal.ModOptionsAccess

private enum class OptionAspect { Stop, Clear }
private enum class OptionReason { Unknown }

class ModOptionsContractTest {
    private fun packed(vararg values: String): ByteArray = (values.joinToString("\u0000") + "\u0000").encodeToByteArray()

    @Test fun playerValuesAreTypedAndUpdatesDoNotChangeTheDeclaration() {
        val enabled = BooleanOption("enabled", "Activer", description = "Cette option")
        val distance = IntegerOption("distance", "Distance", 10, -20, 200)
        val mode = ChoiceOption("mode", "Mode", listOf(OptionChoice("normal", "Normal"), OptionChoice("slow", "Lent")), "normal")
        val api = ModOptionsAccess(listOf(enabled, distance, mode))
        val bool: Boolean = enabled.value
        val number: Int = distance.value
        val selected: String = mode.value
        assertFalse(bool); assertEquals(10, number); assertEquals("normal", selected)
        assertEquals(3, api.count)
        assertContentEquals(intArrayOf(0, 0, 0, 0), api.info(0))
        assertContentEquals(intArrayOf(1, -20, 200, 0), api.info(1))
        assertContentEquals(intArrayOf(2, 0, 0, 2), api.info(2))
        assertEquals("slow", api.choiceMetadata(2, 1, 0))
        assertEquals("Lent", api.choiceMetadata(2, 1, 1))
        assertEquals("Cette option", api.metadata(0, 2))
        val translated = BooleanOption("translated", tr("options.enabled"), description = tr("options.explanation"))
        val translatedApi = ModOptionsAccess(listOf(translated))
        assertEquals(tr("options.enabled"), translatedApi.metadata(0, 1))
        assertEquals(tr("options.explanation"), translatedApi.metadata(0, 2))

        api.apply(packed("true", "-20", "slow"), 3)
        assertTrue(enabled.value); assertEquals(-20, distance.value)
        assertEquals("slow", mode.value)
        assertEquals("false", api.metadata(0, 3)); assertEquals("10", api.metadata(1, 3))
        assertEquals("normal", api.metadata(2, 3))
        api.apply(packed("false", "200", "normal"), 3)
        assertFalse(enabled.value); assertEquals(200, distance.value)
    }

    @Test fun invalidSnapshotsNeverPartiallyChangeAnyOption() {
        val enabled = BooleanOption("enabled", "Enabled")
        val count = IntegerOption("count", "Count", 3, 0, 10)
        val mode = ChoiceOption("mode", "Mode", listOf(OptionChoice("a", "A"), OptionChoice("b", "B")), "a")
        val api = ModOptionsAccess(listOf(enabled, count, mode))
        val invalid = listOf(
            packed("true", "11", "b"), packed("true", "4", "unknown"),
            packed("True", "4", "b"), packed("true", "+4", "b"), packed("true", "04", "b"),
            packed("true", "4", "b").dropLast(1).toByteArray(),
            packed("true", "4"), packed("true", "4", "b", "extra"),
            packed("true", "4", "x".repeat(257)),
            byteArrayOf(0xc3.toByte(), 0x28, 0),
        )
        for (bytes in invalid) {
            assertFails { api.apply(bytes, 3) }
            assertFalse(enabled.value); assertEquals(3, count.value)
            assertEquals("a", mode.value)
        }
        for (wrongCount in listOf(-1, 0, 2, 4, 65)) assertFailsWith<IllegalArgumentException> {
            api.apply(packed("true", "4", "b"), wrongCount)
        }
        assertFalse(enabled.value)
    }

    @Test fun optionSchemasRejectAmbiguousIdentityAndUnsafeDefaults() {
        for (id in listOf("", "9number", "a b", "a/b", "étiquette", "a".repeat(129), "window.main"))
            assertFailsWith<IllegalArgumentException> { BooleanOption(id, "Name") }
        for (label in listOf("", " ", "bad\u0000label", "é".repeat(129)))
            assertFailsWith<IllegalArgumentException> { BooleanOption("enabled", label) }
        assertFailsWith<IllegalArgumentException> { BooleanOption("a", "Name", description = "é".repeat(513)) }
        assertFailsWith<IllegalArgumentException> { IntegerOption("n", "Number", 0, 1, 3) }
        assertFailsWith<IllegalArgumentException> { IntegerOption("n", "Number", 0, 3, 1) }
        val choices = listOf(OptionChoice("a", "A"), OptionChoice("b", "B"))
        assertFailsWith<IllegalArgumentException> { ChoiceOption("c", "Choice", choices, "missing") }
        assertFailsWith<IllegalArgumentException> { ChoiceOption("c", "Choice", listOf(choices.first(), choices.first()), "a") }
        assertFailsWith<IllegalArgumentException> { ChoiceOption("c", "Choice", listOf(choices.first()), "a") }
        assertFailsWith<IllegalArgumentException> { ChoiceOption("c", "Choice", List(17) { OptionChoice("c$it", "$it") }, "c0") }
        assertFailsWith<IllegalArgumentException> { ModOptionsAccess(List(65) { BooleanOption("o$it", "Option") }) }
        assertFailsWith<IllegalArgumentException> { ModOptionsAccess(listOf(BooleanOption("same", "A"), IntegerOption("same", "B", 1, 0, 2))) }
        assertEquals(64, ModOptionsAccess(List(64) { BooleanOption("o$it", "Option") }).count)
        assertFailsWith<IllegalArgumentException> { ModOptionsAccess(List(64) { BooleanOption("o$it", "Option") }, windowCount = 1) }
        assertEquals(56, ModOptionsAccess(List(56) { BooleanOption("o$it", "Option") }, windowCount = 8).count)
    }

    @Test fun choiceAndSchemaListsAreCopiedBeforeRuntimeUpdates() {
        val choices = mutableListOf(OptionChoice("a", "A"), OptionChoice("b", "B"))
        val mode = ChoiceOption("mode", "Mode", choices, "a")
        val schema = mutableListOf<ModOption<*>>(mode)
        val api = ModOptionsAccess(schema)
        choices.clear(); schema.clear()
        assertEquals(2, mode.choices.size); assertEquals(1, api.count)
        api.apply(packed("b"), 1)
        assertEquals("b", mode.value)
    }

    @Test fun allShortcutKeysUseTheSameCanonicalModifierOrder() {
        val keys = ('A'..'Z').map(Char::toString) + ('0'..'9').map(Char::toString) + (1..24).map { "F$it" } +
            listOf("Backspace", "Tab", "Enter", "Escape", "Space", "Left", "Right", "Up", "Down", "Home", "End", "PageUp", "PageDown", "Insert", "Delete")
        val prefixes = listOf("", "Ctrl+", "Alt+", "Shift+", "Ctrl+Alt+", "Ctrl+Shift+", "Alt+Shift+", "Ctrl+Alt+Shift+")
        for (key in keys) for (prefix in prefixes) {
            val mod = toolMod("keys", "Keys") { window("main", "Main", shortcut = prefix + key) {} }
            assertEquals(prefix + key, mod.windows.single().shortcut)
        }
        for (key in listOf("f8", "F0", "F25", "F01", "Ctrl", "Ctrl+", "Ctrl+Ctrl+T", "Shift+Ctrl+T", "Alt+Ctrl+T", "CTRL+T", "Ctrl +T", "A+B", "Win+T", "Return"))
            assertFailsWith<IllegalArgumentException> { toolMod("keys", "Keys") { window("main", "Main", shortcut = key) {} } }
        assertEquals("", toolMod("keys", "Keys") { window("main", "Main", shortcut = "") {} }.windows.single().shortcut)
    }

    @Test fun everyBuilderKeepsOptionsAndRejectsDuplicateRegistration() {
        val enabled = BooleanOption("enabled", "Enabled")
        val tool = toolMod("tool", "Tool") { options(enabled); service("test") {} }
        assertSame(enabled, tool.options.single())
        val fallback = Indication(OptionAspect.Stop, OptionReason.Unknown)
        val legacy = signalMod("signal-old", "Signal", fallback) {
            options(enabled)
            signal("stop", "Stop", "stop_images") { rules { fallback } }
            images { "stop.svg" }
        }
        val model = signalModel("stop", "Stop", "stop_images", fallback) { rules { fallback }; images { "stop.svg" } }
        val modern = signalMod("signal-new", "Signal") { options(enabled); signal(model) }
        assertSame(enabled, legacy.options.single()); assertSame(enabled, modern.options.single())
        assertFailsWith<IllegalArgumentException> { toolMod("dup", "Duplicate") { options(enabled); options(enabled); service("test") {} } }
        assertFailsWith<IllegalArgumentException> { signalMod("dup", "Duplicate", fallback) { options(enabled, enabled) } }
        assertFailsWith<IllegalArgumentException> { signalMod("dup", "Duplicate") { options(enabled, enabled) } }
        val windows = toolMod("windows", "Windows") {
            window("first", "First", shortcut = "Ctrl+Alt+Shift+F24") {}
            window("second", "Second", shortcut = "") {}
            window("third", "Third", shortcut = "") {}
        }.windows
        assertEquals(listOf("Ctrl+Alt+Shift+F24", "", ""), windows.map { it.shortcut })
        assertFailsWith<IllegalArgumentException> {
            toolMod("windows", "Windows") { window("bad", "Bad", shortcut = "Shift+Ctrl+T") {} }
        }
        assertFailsWith<IllegalArgumentException> {
            toolMod("windows", "Windows") {
                options(*Array(64) { BooleanOption("o$it", "Option") })
                window("main", "Main") {}
            }
        }
        assertFailsWith<IllegalArgumentException> {
            toolMod("windows", "Windows") {
                window("main", "Main") {}
                options(*Array(64) { BooleanOption("o$it", "Option") })
            }
        }
    }

    @Test fun valuesBecomeVisibleToSignalRulesWithoutAnyToolContextOrRpc() {
        val enabled = BooleanOption("enabled", "Enabled")
        val fallback = Indication(OptionAspect.Stop, OptionReason.Unknown)
        val model = signalModel("signal", "Signal", "signal_images", fallback) {
            rules { Indication(if (enabled.value) OptionAspect.Clear else OptionAspect.Stop, OptionReason.Unknown) }
            images { "signal.svg" }
        }
        val mod = signalMod("example", "Example") { options(enabled); signal(model) }
        fun aspect() = mod.indication(assertNotNull(mod.decide(Signal(type = "signal"), null)))?.aspect
        assertEquals(OptionAspect.Stop, aspect())
        ModOptionsAccess(mod.options).apply(packed("true"), 1)
        assertEquals(OptionAspect.Clear, aspect())
    }

    @Test fun integerExtremesAndEmptySchemasAreSupported() {
        val value = IntegerOption("number", "Number", 0, Int.MIN_VALUE, Int.MAX_VALUE)
        val api = ModOptionsAccess(listOf(value))
        for (number in listOf(Int.MIN_VALUE, Int.MAX_VALUE, 0)) { api.apply(packed(number.toString()), 1); assertEquals(number, value.value) }
        for (text in listOf("2147483648", "-2147483649", "-0", "1.0", "NaN"))
            assertFailsWith<IllegalArgumentException> { api.apply(packed(text), 1) }
        ModOptionsAccess(emptyList()).apply(byteArrayOf(), 0)
        assertFailsWith<IllegalArgumentException> { ModOptionsAccess(emptyList()).apply(byteArrayOf(0), 0) }
        assertTrue(toolMod("empty", "Empty") { service("test") {} }.options.isEmpty())
    }
}
