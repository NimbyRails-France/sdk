package example

import kotlin.test.*
import nimby.*
import nimby.mod.createMod

class ExampleTest {
    @Test fun unavailableObservationsRemainUnknown() {
        val mod = createMod()
        assertEquals(mod.unknownDecision, mod.evaluate(mapOf("active" to true), Observation()))
    }
    @Test fun onlyKnownClearTrackProducesClearDecision() {
        val mod = createMod()
        val observation = Observation(Occupancy.Clear, fresh = true, routeKnown = true)
        assertEquals(1, mod.evaluate(mapOf("active" to true), observation).aspect)
        assertEquals(0, mod.evaluate(mapOf("active" to true), observation.copy(block = Occupancy.Unknown)).aspect)
    }
}
