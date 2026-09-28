package nimby

/** Persistent bounded integer in a signal panel. Zero can mean source only.
 * Its storage fields are SDK details; use [read] in rules. */
class NumberSetting(val name: String, val label: String, val maximum: Int,
                    val defaultValue: Int = 0, val visibleWhen: String = "") {
    init {
        require(name.matches(Regex("[a-zA-Z][a-zA-Z0-9_.-]{0,95}")))
        require(maximum in 1..65535 && defaultValue in 0..maximum)
    }
    internal val bits = (1..16).first { maximum < (1 shl it) }
    internal fun key(bit: Int) = "nrf.number.$name.$bit"
    internal fun storage() = (0 until bits).map {
        Checkbox(key(it), label, "", defaultValue and (1 shl it) != 0)
    }
    fun read(settings: Map<String, Boolean>): Int = (0 until bits).fold(0) { value, bit ->
        if (settings[key(bit)] ?: (defaultValue and (1 shl bit) != 0)) value or (1 shl bit) else value
    }.coerceIn(0, maximum)
    fun withValue(settings: Map<String, Boolean>, value: Int): Map<String, Boolean> {
        require(value in 0..maximum)
        return settings + (0 until bits).associate { key(it) to (value and (1 shl it) != 0) }
    }
}
