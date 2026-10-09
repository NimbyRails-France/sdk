@file:OptIn(kotlin.experimental.ExperimentalNativeApi::class, kotlinx.cinterop.ExperimentalForeignApi::class)
package nimby.internal

import kotlinx.cinterop.*
import kotlin.native.CName
import nimby.*
import nimby.mod.createMod

// ABI privée v1 du SDK. Aucun pointeur ni annotation native dans le code du mod.
private val gameMod: GameMod by lazy {
    createMod().also {
        require(it.id.isNotBlank())
        if(it is SignallingMod) require(it.diagnosticFile.matches(Regex("[a-zA-Z0-9_.-]+")) && it.diagnosticFile !in setOf(".", ".."))
    }
}
private val mod: SignallingMod get() = gameMod as SignallingMod
private val options by lazy { ModOptionsAccess(gameMod.options, gameMod.windows.size) }
// Copy the declarations once. A mod cannot change the mask layout after the
// native adapter has copied its panels. No mutable current-type global exists.
private val types by lazy {
    if(gameMod is SignallingMod) mod.signalTypes.map { it.copy(checkboxes = it.checkboxes.toList(), actions = it.actions.toList(), numbers = it.numbers.toList()) }.also(::validateSignalTypes)
    else emptyList()
}
private val networkSchema by lazy { SignalNetworkSchema(types) }
private val settingsMasks by lazy { types.map { SignalSettingsMask(it.checkboxes) } }
private val services by lazy { gameMod.services.toList().also { entries ->
    require(entries.size <= 32 && entries.distinct().size == entries.size)
    entries.forEach { require(it.matches(Regex("[a-zA-Z0-9_.-]{1,128}"))) }
} }
@kotlin.native.concurrent.ThreadLocal
private var lastFailure = ""
private inline fun guarded(operation: String, block: () -> Int): Int = try {
    lastFailure = ""; block()
} catch (error: Throwable) {
    lastFailure = try { "$operation: ${error.stackTraceToString()}" } catch (_: Throwable) { "Kotlin failure: stack unavailable" }; -1
}
/** Private, optional export: the adapter reads the failure on the same thread.
 * Older adapters can ignore it; no exception or pointer crosses the ABI. */
@CName("NRFKotlin_LastError") fun lastError(out: CPointer<ByteVar>?, capacity: Int): Int = try {
    require(out != null && capacity > 0)
    val bytes = lastFailure.encodeToByteArray()
    var count = minOf(bytes.size, capacity - 1)
    if (count < bytes.size) while (count > 0 && (bytes[count].toInt() and 0xc0) == 0x80) count--
    for (i in 0 until count) out[i] = bytes[i]
    out[count] = 0
    count
} catch (_: Throwable) { -1 }
private fun text(value: String, out: CPointer<ByteVar>?, capacity: Int): Int {
    val bytes = value.encodeToByteArray()
    require(out != null && capacity > bytes.size && !value.contains('\u0000'))
    bytes.forEachIndexed { i, byte -> out[i] = byte }; out[bytes.size] = 0
    return bytes.size
}
// An older adapter must reject schemas it cannot represent, rather than
// silently operate only the first model or persist under a different ID.
@CName("NRFKotlin_Version") fun version(): Int = guarded("Version") {
    // Newly compiled signals require an animation-aware adapter. Otherwise an
    // old adapter could silently sample a custom cadence as a static image.
    val extendedShortcuts = gameMod.windows.any { !it.shortcut.matches(Regex("Ctrl\\+Shift\\+[A-Z]|F([1-9]|1[0-2])")) }
    if(options.count != 0 || extendedShortcuts) 9 else if(gameMod is SignallingMod) 8 else if(gameMod.windows.isNotEmpty()) 7 else 3
}
// Additive ABI 9. Options are copied once and updates are all-or-nothing. These
// exports never call mod code; the adapter invokes OptionsApply on its worker.
@CName("NRFKotlin_OptionCount") fun optionCount(): Int = guarded("OptionCount") { options.count }
@CName("NRFKotlin_OptionInfo") fun optionInfo(index: Int, out: CPointer<IntVar>?): Int = guarded("OptionInfo") {
    require(out != null)
    options.info(index).forEachIndexed { i, value -> out[i] = value }; 0
}
@CName("NRFKotlin_OptionMetadata") fun optionMetadata(index: Int, field: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("OptionMetadata") {
    text(options.metadata(index, field), out, capacity)
}
@CName("NRFKotlin_OptionChoiceMetadata") fun optionChoiceMetadata(index: Int, choice: Int, field: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("OptionChoiceMetadata") {
    text(options.choiceMetadata(index, choice, field), out, capacity)
}
@CName("NRFKotlin_OptionsApply") fun optionsApply(packed: CPointer<ByteVar>?, bytes: Int, count: Int): Int = guarded("OptionsApply") {
    require(bytes in 0..64 * 257 && count in 0..64 && (bytes == 0 || packed != null))
    options.apply(if (bytes == 0) byteArrayOf() else packed!!.readBytes(bytes), count); 0
}
// Called before version()/createMod(). Tests using the Kotlin API directly
// keep the default; the actual loader checks the file next to the mod DLL.
@CName("NRFKotlin_TranslationsAvailable") fun translationsAvailable(available: Int): Int = guarded("TranslationsAvailable") {
    require(available == 0 || available == 1)
    TranslationEnvironment.catalogAvailable = available == 1
    0
}
@CName("NRFKotlin_ModKind") fun modKind(): Int = guarded("ModKind") { if(gameMod is ToolMod) 1 else 0 }
@CName("NRFKotlin_ServiceCount") fun serviceCount(): Int = guarded("ServiceCount") { services.size }
@CName("NRFKotlin_ServiceName") fun serviceName(index: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("ServiceName") { text(services[index],out,capacity) }
typealias ToolCall = CPointer<CFunction<(Int, CPointer<LongVar>?, Int, CPointer<DoubleVar>?, Int, CPointer<ByteVar>?, Int) -> Int>>
private fun inTool(world: String, generation: Long, call: ToolCall?, block: (ToolContext) -> Unit) {
    require(call != null && world.isNotBlank() && generation != 0L)
    ToolAccess.withContext(world, generation, { op, integers, numbers, bytes ->
        integers.usePinned { a -> numbers.usePinned { b -> bytes.usePinned { c ->
            call(op, if(integers.isEmpty()) null else a.addressOf(0), integers.size,
                if(numbers.isEmpty()) null else b.addressOf(0), numbers.size,
                if(bytes.isEmpty()) null else c.addressOf(0), bytes.size)
        } } }
    }, block)
}
@CName("NRFKotlin_ServiceEvent") fun serviceEvent(index: Int, sequence: Long, signal: Long, generation: Long,
    panel: Long, action: CPointer<ByteVar>?, world: CPointer<ByteVar>?, origin: CPointer<ByteVar>?, call: ToolCall?): Int = guarded("ServiceEvent") {
    require(sequence != 0L && signal ushr 48 == 8L && panel != 0L && action != null && world != null && origin != null)
    val request=SignalActionRequest(sequence,signal,action.toKString(),services[index],world.toKString(),generation,panel,origin.toKString())
    inTool(request.worldId,generation,call) { gameMod.onSignalAction(request,it) }; 0
}
@CName("NRFKotlin_ToolTick") fun toolTick(world: CPointer<ByteVar>?, generation: Long, call: ToolCall?): Int = guarded("ToolTick") {
    require(world != null); inTool(world.toKString(),generation,call) { gameMod.onTick(it) }; 0
}
@CName("NRFKotlin_ServiceEventV2") fun serviceEventV2(index: Int, sequence: Long, signal: Long, generation: Long,
    panel: Long, action: CPointer<ByteVar>?, world: CPointer<ByteVar>?, origin: CPointer<ByteVar>?, call: ToolCall?,
    hasValue: Int, value: Int): Int = guarded("ServiceEventV2") {
    require(sequence!=0L&&signal ushr 48==8L&&panel!=0L&&action!=null&&world!=null&&origin!=null&&hasValue in 0..1)
    val request=SignalActionRequest(sequence,signal,action.toKString(),services[index],world.toKString(),generation,panel,origin.toKString(),value.takeIf { hasValue==1 })
    inTool(request.worldId,generation,call) { gameMod.onSignalAction(request,it) };0
}
@CName("NRFKotlin_ToolStop") fun toolStop(): Int = guarded("ToolStop") { gameMod.onStop(); 0 }
@CName("NRFKotlin_WindowCount") fun windowCount(): Int = guarded("WindowCount") { gameMod.windows.size }
@CName("NRFKotlin_WindowMetadata") fun windowMetadata(index: Int, field: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("WindowMetadata") {
    val window = gameMod.windows[index]
    text(when(field) { 0 -> window.id; 1 -> window.title; 2 -> window.shortcut; else -> error("Unknown window field") }, out, capacity)
}
@CName("NRFKotlin_WindowEvent") fun windowEvent(index: Int, sequence: Long, action: CPointer<ByteVar>?, names: CPointer<ByteVar>?, values: CPointer<IntVar>?, count: Int,
    world: CPointer<ByteVar>?, generation: Long, call: ToolCall?): Int = guarded("WindowEvent") {
    require(sequence > 0 && action != null && world != null && count in 0..8)
    require(count == 0 || (names != null && values != null))
    val fields = linkedMapOf<String, Int>(); var offset = 0
    repeat(count) { i ->
        val name = (names!! + offset)!!.toKString(); offset += name.encodeToByteArray().size + 1
        require(name.matches(Regex("[a-zA-Z0-9_.-]{1,128}")) && name !in fields); fields[name] = values!![i]
    }
    val request = ToolWindowEvent(gameMod.windows[index].id, action.toKString(), fields, sequence, world.toKString(), generation)
    inTool(request.worldId, generation, call) { gameMod.onWindowEvent(request, it) }; 0
}
@CName("NRFKotlin_ActionCount") fun actionCount(type: Int): Int = guarded("ActionCount") { types[type].actions.size }
@CName("NRFKotlin_ActionMetadata") fun actionMetadata(type: Int,index: Int,field: Int,out: CPointer<ByteVar>?,capacity: Int): Int = guarded("ActionMetadata") {
    val action=types[type].actions[index]
    text(when(field){0->action.id;1->action.label;2->action.whenMod;3->action.service;else->error("Unknown action field")},out,capacity)
}
// Optional extension of ABI 1. Old single-type DLLs remain supported. New
// adapters use these exports together, never mix layouts from two versions.
@CName("NRFKotlin_TypeCount") fun typeCount(): Int = guarded("TypeCount") { types.size }
@CName("NRFKotlin_NumberCount") fun numberCount(type: Int): Int = guarded("NumberCount") { types[type].numbers.size }
@CName("NRFKotlin_NumberMetadata") fun numberMetadata(type: Int, index: Int, field: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("NumberMetadata") {
    val number = types[type].numbers[index]
    text(when(field) { 0 -> number.name; 1 -> number.label; 2 -> number.visibleWhen; else -> error("Unknown number field") }, out, capacity)
}
@CName("NRFKotlin_NumberInfo") fun numberInfo(type: Int, index: Int, out: CPointer<IntVar>?): Int = guarded("NumberInfo") {
    require(out != null)
    val number = types[type].numbers[index]; out[0] = (1..16).first { number.maximum < (1 shl it) }; out[1] = number.maximum; 0
}
/** ABI 4 : un échec réseau garde le modèle du signal concerné. */
@CName("NRFKotlin_FallbackType") fun fallbackType(type: Int, invalid: Int, out: CPointer<IntVar>?): Int = guarded("FallbackType") {
    require(out != null && invalid in 0..1)
    val decision = if (invalid == 1) mod.invalidNetworkDecision(types[type].id) else mod.unknownDecision(types[type].id)
    out[0] = decision.aspect; out[1] = decision.reason; 0
}
/** Diagnostic d'un signal déjà identifié : exposer ses ordinaux locaux au banc,
 * jamais les tags privés utilisés entre les callbacks du pont. */
@CName("NRFKotlin_LocalDecision") fun localDecision(aspect: Int, reason: Int, out: CPointer<IntVar>?): Int = guarded("LocalDecision") {
    require(out != null)
    if (mod.modelLocalIndications) {
        val value = requireNotNull(mod.indication(Decision(aspect, reason)))
        out[0] = value.aspect.ordinal; out[1] = value.reason.ordinal
    } else { out[0] = aspect; out[1] = reason }
    0
}
@CName("NRFKotlin_TypeMetadata") fun typeMetadata(type: Int, field: Int, index: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("TypeMetadata") {
    val declaration = types[type]
    text(when (field) {
        0 -> declaration.id; 1 -> declaration.title; 2 -> declaration.textureSet
        4 -> declaration.checkboxes[index].name; 5 -> declaration.checkboxes[index].label
        6 -> declaration.checkboxes[index].description
        else -> error("Unknown type metadata")
    }, out, capacity)
}
@CName("NRFKotlin_TypeInfo") fun typeInfo(type: Int, out: CPointer<LongVar>?): Int = guarded("TypeInfo") {
    require(out != null)
    val boxes = types[type].checkboxes
    out[0] = boxes.size.toLong()
    out[1] = boxes.foldIndexed(0L) { i, mask, box -> if (box.defaultValue) mask or (1L shl i) else mask }
    out[2] = boxes.foldIndexed(0L) { i, mask, box -> if (box.onlyWhenEnabled) mask or (1L shl i) else mask }
    out[3] = if (types[type].observeApproach) types[type].approachBlocks.toLong() else 0
    0
}
@CName("NRFKotlin_MigrateSettings") fun migrateSettings(type: Int, names: CPointer<ByteVar>?, values: CPointer<IntVar>?, count: Int,
    out: CPointer<LongVar>?): Int = guarded("MigrateSettings") {
    require(count in 0..64 && out != null && (count == 0 || (names != null && values != null)))
    val declaration = types[type]
    val saved = mutableMapOf<String, Boolean>()
    for (i in 0 until count) {
        val bytes = ByteArray(129) { names!![i * 129 + it] }
        val length = bytes.indexOf(0)
        require(length in 1..128 && values!![i] in 0..1)
        val key = bytes.copyOf(length).decodeToString(throwOnInvalidSequence = true)
        require(key !in saved)
        saved[key] = values!![i] != 0
    }
    val migrated = mod.migrateSettings(declaration.id, saved.toMap())
    out[0] = declaration.checkboxes.foldIndexed(0L) { i, mask, box ->
        if (migrated[box.name] ?: box.defaultValue) mask or (1L shl i) else mask
    }
    0
}
@CName("NRFKotlin_Force") fun force(aspect: Int, out: CPointer<IntVar>?): Int = guarded("Force") {
    require(out != null)
    val decision = mod.forcedDecision(aspect)
    if (decision == null) 1 else { out[0] = decision.aspect; out[1] = decision.reason; 0 }
}
@CName("NRFKotlin_ForceType") fun forceType(type: Int, aspect: Int, out: CPointer<IntVar>?): Int = guarded("ForceType") {
    require(out != null)
    val decision = mod.forcedDecision(types[type].id, aspect)
    if (decision == null) 1 else { out[0] = decision.aspect; out[1] = decision.reason; 0 }
}
@CName("NRFKotlin_Metadata") fun metadata(field: Int, index: Int, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("Metadata") {
    text(when (field) {
        0 -> gameMod.id; 1 -> gameMod.title; 2 -> types.firstOrNull()?.textureSet ?: ""; 3 -> if(gameMod is SignallingMod) mod.diagnosticFile else "nimby-tool-faults.jsonl"
        4 -> types.first().checkboxes[index].name; 5 -> types.first().checkboxes[index].label; 6 -> types.first().checkboxes[index].description
        7 -> mod.reasonName(index)
        8 -> mod.aspectName(index)
        else -> error("Unknown metadata")
    }, out, capacity)
}
@CName("NRFKotlin_Info") fun info(out: CPointer<IntVar>?): Int = guarded("Info") {
    require(out != null)
    out[0] = types.first().checkboxes.size; out[1] = if (mod.maximumLineSpeed) 1 else 0
    out[2] = mod.unknownDecision.aspect; out[3] = mod.unknownDecision.reason
    out[4] = mod.invalidNetworkDecision.aspect; out[5] = mod.invalidNetworkDecision.reason
    0
}
@CName("NRFKotlin_Defaults") fun defaults(out: CPointer<LongVar>?): Int = guarded("Defaults") {
    require(out != null)
    out[0] = types.first().checkboxes.foldIndexed(0L) { i, mask, box -> if (box.defaultValue) mask or (1L shl i) else mask }; 0
}
@CName("NRFKotlin_Decide") fun decide(mode: Int, mask: Long, status: Int, id: Long, nextId: Long,
    observation: CPointer<IntVar>?, nextAspect: Int, nextReason: Int, out: CPointer<IntVar>?): Int =
    decideType(0, mode, mask, status, id, nextId, 0, observation, nextAspect, nextReason, out)

// Each row: id, next, mask, approaching train; type, status and seven observation integers.
@CName("NRFKotlin_PrepareNetwork") fun prepareNetwork(count: Int, identities: CPointer<LongVar>?, fields: CPointer<IntVar>?, masks: CPointer<LongVar>?, statuses: CPointer<IntVar>?): Int = guarded("PrepareNetwork") {
    require(count in 0..maximumSignalNetworkSize)
    if(count == 0) return@guarded 0
    require(identities != null && fields != null && masks != null && statuses != null)
    val source = List(count) { i ->
        val declaration = types[fields[i*9]]
        val mask = identities[i*4+2]
        val settings = settingsMasks[fields[i*9]].read(mask)
        val observation = Observation(Occupancy.entries[fields[i*9+2]], fields[i*9+3]!=0, fields[i*9+4]!=0,
            fields[i*9+5]!=0, fields[i*9+6]!=0, fields[i*9+7]!=0, fields[i*9+8], identities[i*4+3].takeIf { it!=0L })
        Signal(identities[i*4], identities[i*4+1], settings, observation, SettingsStatus.entries[fields[i*9+1]], declaration.id)
    }
    val prepared = networkSchema.prepare(mod, source)
    prepared.forEachIndexed { i, signal ->
        masks[i] = types[fields[i*9]].checkboxes.foldIndexed(0L) { bit, mask, box ->
            if(signal.settings[box.name] ?: box.defaultValue) mask or (1L shl bit) else mask }
        statuses[i] = signal.settingsStatus.ordinal
    }
    0
}

// Optional capability: older ABI 8 DLLs retain their original 512-signal
// bound. A new adapter must not send a larger request to those DLLs.
@CName("NRFKotlin_NetworkLimit") fun networkLimit(): Int = maximumSignalNetworkSize

@CName("NRFKotlin_DecideType") fun decideType(type: Int, mode: Int, mask: Long, status: Int, id: Long, nextId: Long, approachingTrain: Long,
    observation: CPointer<IntVar>?, nextAspect: Int, nextReason: Int, out: CPointer<IntVar>?): Int = guarded("Decide") {
    require(observation != null && out != null && mode in 0..2)
    val declaration = types[type]
    val settings = settingsMasks[type].read(mask)
    val o = Observation(Occupancy.entries[observation[0]], observation[1] != 0, observation[2] != 0,
        observation[3] != 0, observation[4] != 0, observation[5] != 0, observation[6], approachingTrain.takeIf { it != 0L })
    val next = if (nextAspect < 0) null else Decision(nextAspect, nextReason)
    val signal = Signal(id, nextId, settings, o, SettingsStatus.entries[status], declaration.id)
    val result = if (mode == 0) mod.evaluate(declaration.id, settings, o) else mod.decide(if (mode == 2) mod.fromLive(signal) else signal, next)
    if (result == null) 1 else { out[0] = result.aspect; out[1] = result.reason; 0 }
}
@CName("NRFKotlin_Texture") fun texture(aspect: Int, reason: Int, time: Long, half: Long, out: CPointer<ByteVar>?, capacity: Int): Int = guarded("Texture") {
    text(mod.texture(Decision(aspect, reason), time, half), out, capacity)
}
@CName("NRFKotlin_TextureAnimation") fun textureAnimation(aspect: Int, reason: Int,
    first: CPointer<ByteVar>?, alternate: CPointer<ByteVar>?, capacity: Int, everyMs: CPointer<LongVar>?): Int = guarded("TextureAnimation") {
    require(first != null && alternate != null && everyMs != null)
    val animation = mod.animation(Decision(aspect, reason))
    if (animation == null) 1 else {
        text(animation.first, first, capacity); text(animation.alternate, alternate, capacity)
        everyMs[0] = animation.everyMs
        0
    }
}
@CName("NRFKotlin_Fault") fun fault(aspect: Int, reason: Int): Int = guarded("Fault") {
    val decision = Decision(aspect, reason)
    (if (mod.isFault(decision)) 1 else 0) or (if (mod.isActive(decision)) 2 else 0)
}
@CName("NRFKotlin_Driving") fun driving(aspect: Int, reason: Int, numbers: CPointer<DoubleVar>?, flags: CPointer<IntVar>?): Int = guarded("Driving") {
    require(numbers != null && flags != null)
    val rule = mod.drivingRule(Decision(aspect, reason))
    if (rule == null) 1 else {
        numbers[0] = rule.speedMps; numbers[1] = rule.reopenedSpeedMps
        flags[0] = rule.signalsAhead; flags[1] = rule.flags.fold(0) { bits, flag -> bits or flag.bit }; 0
    }
}
@CName("NRFKotlin_Plan") fun plan(values: CPointer<DoubleVar>?, flags: CPointer<IntVar>?, count: Int,
    sources: CPointer<LongVar>?, restrictions: CPointer<DoubleVar>?, out: CPointer<DoubleVar>?, result: CPointer<LongVar>?): Int = guarded("Plan") {
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
