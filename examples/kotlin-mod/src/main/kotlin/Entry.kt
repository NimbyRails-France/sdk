package nimby.mod

import nimby.*

// Public entry point discovered by the Gradle plugin. The SDK supplies the
// precompiled native adapter; this project does not create a C++ client or DLL glue.
fun createMod(): SignallingMod = ExampleSignals()

/** Minimal API example, not a national signalling or train-driving system. */
internal class ExampleSignals : SignallingMod() {
    override val id = "example.signals"
    override val title = "Example signals"
    override val textureSet = "example_signals"
    override val checkboxes = listOf(Checkbox("active", "Enable", "Enable this example signal."))
    override val unknownDecision = Decision(0, 0)
    override val invalidNetworkDecision = Decision(0, 1)
    override fun evaluate(settings: Map<String, Boolean>, observation: Observation): Decision =
        if (settings["active"] != true || !observation.fresh || !observation.routeKnown)
            unknownDecision else Decision(if (observation.block == Occupancy.Clear) 1 else 0, 0)
    // The SDK delivers fresh observations here. Keep decisions in Kotlin and
    // treat missing/uncertain observations explicitly in evaluate().
    override fun decide(signal: Signal, next: Decision?): Decision = evaluate(signal.settings, signal.observation)
    override fun texture(decision: Decision, simulationMs: Long, halfPeriodMs: Long) = "signal.svg"
    override fun drivingRule(decision: Decision): DrivingRule? = null
    override fun isFault(decision: Decision) = decision.aspect == 0
    override fun reasonName(reason: Int) = if (reason == 1) "Invalid network" else "Example decision"
}
