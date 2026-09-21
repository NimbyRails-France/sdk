package nimby

/** API de mod Kotlin. Les unités sont mètres, secondes, m/s, kg, N et W. */
data class Checkbox(val name: String, val label: String, val description: String, val defaultValue: Boolean = false)
enum class Occupancy { Unknown, Clear, Occupied }
enum class SettingsStatus { Unavailable, Absent, Present }
data class Observation(
    val block: Occupancy = Occupancy.Unknown, val fresh: Boolean = false,
    val routeKnown: Boolean = false, val forcedStop: Boolean = false,
    val lampFailed: Boolean = false, val redFlashCondition: Boolean = false, val next: Int = 0
)
data class Decision(val aspect: Int, val reason: Int)
data class Signal(
    val id: Long = 0, val nextSignal: Long = 0, val settings: Map<String, Boolean> = emptyMap(),
    val observation: Observation = Observation(), val settingsStatus: SettingsStatus = SettingsStatus.Present
)
data class Vehicle(
    val maxSpeedMps: Double = 0.0, val maxAccelerationMps2: Double = 0.0,
    val serviceBrakingMps2: Double = 0.0, val tractiveEffortN: Double = 0.0,
    val powerW: Double = 0.0, val emptyMassKg: Double = 0.0,
    val extraMassKg: Double = 0.0, val lengthM: Double = 0.0
)
data class DrivingSettings(val brakeUse: Double = 0.8, val responseSeconds: Double = 2.0, val marginM: Double = 10.0)
data class Constraint(val source: Long, val beginM: Double, val endM: Double, val speedMps: Double, val releaseByRear: Boolean = true)
data class DrivingInput(
    val headM: Double = 0.0, val speedMps: Double = 0.0, val lineSpeedMps: Double = 0.0,
    val fresh: Boolean = false, val routeKnown: Boolean = false,
    val onSight: Boolean = false, val visibleClearM: Double? = null
)
data class DrivingPlan(
    val available: Boolean = false, val speedCeilingMps: Double = 0.0,
    val serviceDecelerationMps2: Double = 0.0, val accelerationMps2: Double = 0.0,
    val brakingRequired: Boolean = false, val limitingSource: Long = 0
)
enum class DrivingFlag(val bit: Int) {
    Clear(1), HoldToClear(2), Stop(4), FollowTarget(8), OnSight(16), StopThenProceed(32), CancelAtNextClear(64)
}
data class DrivingRule(
    val speedMps: Double = -1.0, val reopenedSpeedMps: Double = 30.0 / 3.6,
    val signalsAhead: Int = 0, val flags: Set<DrivingFlag> = emptySet()
)

/** Une seule implémentation Kotlin suffit. Le SDK fournit DLL, exports et boucle de lecture. */
abstract class SignallingMod {
    abstract val id: String
    abstract val title: String
    abstract val textureSet: String
    abstract val checkboxes: List<Checkbox>
    open val maximumLineSpeed = false
    open val diagnosticFile: String = "nimby-kotlin-faults.jsonl"
    abstract val unknownDecision: Decision
    abstract val invalidNetworkDecision: Decision
    abstract fun evaluate(settings: Map<String, Boolean>, observation: Observation): Decision
    abstract fun decide(signal: Signal, next: Decision?): Decision?
    open fun fromLive(signal: Signal): Signal = signal
    abstract fun texture(decision: Decision, simulationMs: Long, halfPeriodMs: Long): String
    abstract fun drivingRule(decision: Decision): DrivingRule?
    abstract fun isFault(decision: Decision): Boolean
    open fun isActive(decision: Decision): Boolean = true
    open fun aspectName(aspect: Int): String = aspect.toString()
    abstract fun reasonName(reason: Int): String
    open fun plan(vehicle: Vehicle, settings: DrivingSettings, input: DrivingInput, constraints: List<Constraint>) = DrivingPlan()
}

/** Résolution des liens, commune aux mods et aux tests, sans récursion profonde. */
fun SignallingMod.evaluateNetwork(signals: List<Signal>): List<Decision> {
    require(signals.size <= 512)
    val index = signals.mapIndexed { i, signal -> signal.id to i }.toMap()
    require(index.size == signals.size && signals.none { it.id == 0L })
    val results = arrayOfNulls<Decision>(signals.size)
    val state = IntArray(signals.size)
    for (start in signals.indices) {
        if (state[start] == 2) continue
        val pending = mutableListOf<Int>()
        var current = start
        while (state[current] != 2) {
            if (state[current] == 1) {
                pending.forEach { results[it] = invalidNetworkDecision; state[it] = 2 }
                break
            }
            val local = decide(signals[current], null)
            if (local != null) { results[current] = local; state[current] = 2; break }
            val next = index[signals[current].nextSignal]
            if (next == null) { results[current] = invalidNetworkDecision; state[current] = 2; break }
            state[current] = 1; pending.add(current); current = next
        }
        for (i in pending.asReversed()) {
            if (state[i] == 2) continue
            results[i] = decide(signals[i], results[index.getValue(signals[i].nextSignal)]) ?: invalidNetworkDecision
            state[i] = 2
        }
    }
    return results.map { requireNotNull(it) }
}
