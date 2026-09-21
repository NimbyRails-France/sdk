@file:OptIn(kotlin.experimental.ExperimentalNativeApi::class, kotlinx.cinterop.ExperimentalForeignApi::class)
package nimby.internal

import kotlinx.cinterop.*
import kotlin.native.CName
import nimby.*
import nimby.mod.createMod

// ABI privée v1 du SDK. Aucun pointeur ni annotation native dans le code du mod.
private val mod: SignallingMod by lazy {
    createMod().also {
        require(it.checkboxes.size <= 64 && it.checkboxes.map { box -> box.name }.toSet().size == it.checkboxes.size)
        require(it.id.isNotBlank() && it.textureSet.isNotBlank())
        require(it.diagnosticFile.matches(Regex("[a-zA-Z0-9_.-]+")) && it.diagnosticFile !in setOf(".", ".."))
    }
}
private inline fun guarded(block: () -> Int): Int = try { block() } catch (_: Throwable) { -1 }
private fun text(value: String, out: CPointer<ByteVar>?, capacity: Int): Int {
    val bytes = value.encodeToByteArray()
    require(out != null && capacity > bytes.size && !value.contains('\u0000'))
    bytes.forEachIndexed { i, byte -> out[i] = byte }; out[bytes.size] = 0
    return bytes.size
}
@CName("NRFKotlin_Version") fun version(): Int = 1
@CName("NRFKotlin_Metadata") fun metadata(field: Int, index: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded {
    text(when (field) {
        0 -> mod.id; 1 -> mod.title; 2 -> mod.textureSet; 3 -> mod.diagnosticFile
        4 -> mod.checkboxes[index].name; 5 -> mod.checkboxes[index].label; 6 -> mod.checkboxes[index].description
        7 -> mod.reasonName(index)
        8 -> mod.aspectName(index)
        else -> error("Unknown metadata")
    }, out, capacity)
}
@CName("NRFKotlin_Info") fun info(out: CPointer<IntVar>?): Int = guarded {
    require(out != null)
    out[0] = mod.checkboxes.size; out[1] = if (mod.maximumLineSpeed) 1 else 0
    out[2] = mod.unknownDecision.aspect; out[3] = mod.unknownDecision.reason
    out[4] = mod.invalidNetworkDecision.aspect; out[5] = mod.invalidNetworkDecision.reason
    0
}
@CName("NRFKotlin_Defaults") fun defaults(out: CPointer<LongVar>?): Int = guarded {
    require(out != null)
    out[0] = mod.checkboxes.foldIndexed(0L) { i, mask, box -> if (box.defaultValue) mask or (1L shl i) else mask }; 0
}
@CName("NRFKotlin_Decide") fun decide(mode: Int, mask: Long, status: Int, id: Long, nextId: Long,
    observation: CPointer<IntVar>?, nextAspect: Int, nextReason: Int, out: CPointer<IntVar>?): Int = guarded {
    require(observation != null && out != null && mode in 0..2)
    val settings = mod.checkboxes.mapIndexed { i, box -> box.name to (mask and (1L shl i) != 0L) }.toMap()
    val o = Observation(Occupancy.entries[observation[0]], observation[1] != 0, observation[2] != 0,
        observation[3] != 0, observation[4] != 0, observation[5] != 0, observation[6])
    val next = if (nextAspect < 0) null else Decision(nextAspect, nextReason)
    val signal = Signal(id, nextId, settings, o, SettingsStatus.entries[status])
    val result = if (mode == 0) mod.evaluate(settings, o) else mod.decide(if (mode == 2) mod.fromLive(signal) else signal, next)
    if (result == null) 1 else { out[0] = result.aspect; out[1] = result.reason; 0 }
}
@CName("NRFKotlin_Texture") fun texture(aspect: Int, reason: Int, time: Long, half: Long, out: CPointer<ByteVar>?, capacity: Int): Int = guarded {
    text(mod.texture(Decision(aspect, reason), time, half), out, capacity)
}
@CName("NRFKotlin_Fault") fun fault(aspect: Int, reason: Int): Int = guarded {
    val decision = Decision(aspect, reason)
    (if (mod.isFault(decision)) 1 else 0) or (if (mod.isActive(decision)) 2 else 0)
}
@CName("NRFKotlin_Driving") fun driving(aspect: Int, reason: Int, numbers: CPointer<DoubleVar>?, flags: CPointer<IntVar>?): Int = guarded {
    require(numbers != null && flags != null)
    val rule = mod.drivingRule(Decision(aspect, reason))
    if (rule == null) 1 else {
        numbers[0] = rule.speedMps; numbers[1] = rule.reopenedSpeedMps
        flags[0] = rule.signalsAhead; flags[1] = rule.flags.fold(0) { bits, flag -> bits or flag.bit }; 0
    }
}
@CName("NRFKotlin_Plan") fun plan(values: CPointer<DoubleVar>?, flags: CPointer<IntVar>?, count: Int,
    sources: CPointer<LongVar>?, restrictions: CPointer<DoubleVar>?, out: CPointer<DoubleVar>?, result: CPointer<LongVar>?): Int = guarded {
    require(values != null && flags != null && out != null && result != null && count in 0..4096)
    require(count == 0 || (sources != null && restrictions != null))
    val v = Vehicle(values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7])
    val s = DrivingSettings(values[8], values[9], values[10])
    val i = DrivingInput(values[11], values[12], values[13], flags[0] != 0, flags[1] != 0, flags[2] != 0, if (flags[3] != 0) values[14] else null)
    val constraints = List(count) { n -> Constraint(sources!![n], restrictions!![n*4], restrictions[n*4+1], restrictions[n*4+2], restrictions[n*4+3] != 0.0) }
    val p = mod.plan(v, s, i, constraints)
    out[0] = p.speedCeilingMps; out[1] = p.serviceDecelerationMps2; out[2] = p.accelerationMps2
    result[0] = if (p.available) 1 else 0; result[1] = if (p.brakingRequired) 1 else 0; result[2] = p.limitingSource
    0
}
