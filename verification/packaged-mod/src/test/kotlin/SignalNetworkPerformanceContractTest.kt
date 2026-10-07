package nimby.mod

import kotlin.test.*
import nimby.*
import nimby.internal.SignalSettingsMask

private enum class NetworkAspect { Stop }
private enum class NetworkReason { Restricted }

class SignalNetworkPerformanceContractTest {
    @Test fun maskSettingsPreserveTheMapContractWithoutMutableSharedState() {
        val boxes = List(64) { Checkbox("bit$it", "Bit $it", "", it == 63) }
        val schema = SignalSettingsMask(boxes)
        val first = schema.read(5)
        val last = schema.read(Long.MIN_VALUE)
        assertEquals(64, first.size)
        assertEquals(true, first["bit0"])
        assertEquals(false, first["bit1"])
        assertEquals(true, first["bit2"])
        assertEquals(false, first["bit63"])
        assertEquals(true, last["bit63"])
        assertEquals(false, last["bit0"])
        assertNull(first["foreign"])
        assertFalse(first.containsKey("foreign"))
        val expected = boxes.associate { it.name to (it.name in setOf("bit0", "bit2")) }
        assertEquals(expected, first)
        assertEquals(first, expected)
        assertEquals(expected.hashCode(), first.hashCode())
        assertEquals(expected.entries, first.entries)
        assertEquals(expected, first.toMap())
        assertEquals(last, schema.defaults)
        assertFalse(first is MutableMap<*, *>)
        val changed = first + ("bit1" to true)
        assertEquals(true, changed["bit1"])
        assertEquals(false, first["bit1"])
    }

    private fun mod(prepare: (List<Signal>) -> List<Signal> = { it }): SignallingMod {
        val model = signalModel("network", "Network", "network-images", Indication(NetworkAspect.Stop, NetworkReason.Restricted)) {
            rules { Indication(NetworkAspect.Stop, NetworkReason.Restricted) }; images { "stop.svg" }
        }
        return signalMod("network-contract", "Network") { signal(model); prepareNetwork(prepare) }
    }

    @Test fun preparationValidatesAllEntriesEvenWhenAListIsModifiedInPlace() {
        val source = listOf(Signal(1, 2), Signal(2))
        assertFails { mod { (it as MutableList)[0] = it[0].copy(nextSignal = 123); it }.prepareObservedNetwork(source) }
        assertFailsWith<IllegalArgumentException> { mod { it.reversed() }.prepareObservedNetwork(source) }
        assertFailsWith<IllegalArgumentException> { mod { it.map { s -> s.copy(observation = Observation(fresh = true)) } }.prepareObservedNetwork(source) }
        val accepted = mod { it.map { s -> s.copy(settings = mapOf("derived" to true)) } }.prepareObservedNetwork(source)
        assertTrue(accepted.all { it.settings["derived"] == true })
        assertEquals(2L, source[0].nextSignal)
    }
}
