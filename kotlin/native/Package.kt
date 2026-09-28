package nimby.packaging

import nimby.*
import nimby.mod.createMod

// Build-time entry point, shipped by the kit. It only reads declarations:
// no onTick, onStop, service, rule or rendering callback is invoked.
// Keep createMod and object initializers free from game/IO side effects.
private fun quote(value: String): String = buildString {
    append('"')
    value.forEach { c -> when (c) {
        '"' -> append("\\\""); '\\' -> append("\\\\")
        else -> if (c < ' ') append("\\u" + c.code.toString(16).padStart(4, '0')) else append(c)
    } }
    append('"')
}
private fun optional(value: String?) = value?.let(::quote) ?: "null"

fun main() {
    val mod = createMod()
    val meta = requireNotNull(mod.metadata) { "Déclarer metadata(author, description) dans le mod" }
    val types = if (mod is SignallingMod) mod.signalTypes.also(::validateSignalTypes) else emptyList()
    val models = types.joinToString(",") { type ->
        val resource = requireNotNull(type.construction) { "Déclarer construction(states) pour ${type.id}" }
        "{\"id\":${quote(type.id)},\"textures\":${quote(type.textureSet)}," +
            "\"name\":${quote(resource.name)},\"kind\":${quote(resource.kind)}," +
            "\"catalogueName\":${quote(resource.catalogueName)},\"nameKey\":${optional(resource.nameKey)}," +
            "\"catalogueNameKey\":${optional(resource.catalogueNameKey)}," +
            "\"states\":[${resource.states.joinToString(",", transform = ::quote)}]}"
    }
    println("{\"format\":1,\"id\":${quote(mod.id)},\"name\":${quote(mod.title)}," +
        "\"displayName\":${optional(meta.name)},\"author\":${quote(meta.author)},\"description\":${quote(meta.description)},\"signals\":[$models]}")
}
