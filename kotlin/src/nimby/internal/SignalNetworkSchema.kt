package nimby.internal

import nimby.*

/** The declaration is fixed for the lifetime of a loaded mod. Compile its
 * validation once; each observation still validates every identity and link. */
class SignalNetworkSchema(types: List<SignalType>) {
    private val defaultType: String
    private val knownTypes: Set<String>
    init {
        validateSignalTypes(types)
        defaultType = types.first().id
        knownTypes = types.mapTo(HashSet()) { it.id }
    }

    fun prepare(mod: SignallingMod, input: List<Signal>): List<Signal> {
        require(input.size <= maximumSignalNetworkSize)
        val identities = HashSet<Long>(input.size)
        val normalized = ArrayList<Signal>(input.size)
        for(signal in input) {
            require(signal.id != 0L && identities.add(signal.id)) { "Missing or duplicate signal ID" }
            require(signal.type.isEmpty() || signal.type in knownTypes) { "Unknown signal type: ${signal.type}" }
            normalized.add(if(signal.type.isEmpty()) signal.copy(type = defaultType) else signal)
        }
        // A callback cannot replace entries in the validation baseline by
        // casting the supplied List to MutableList and mutating it in place.
        val source = object : AbstractList<Signal>() {
            override val size: Int get() = normalized.size
            override fun get(index: Int) = normalized[index]
        }
        val signals = mod.prepareNetwork(source)
        require(signals.size == normalized.size && signals.indices.all { i ->
            val before = normalized[i]
            val after = signals[i]
            after.id == before.id && after.nextSignal == before.nextSignal &&
                after.type == before.type && after.observation == before.observation &&
                (after.settingsStatus == before.settingsStatus ||
                    (before.settingsStatus == SettingsStatus.Absent && after.settingsStatus == SettingsStatus.Present))
        }) { "Network preparation may only change effective settings" }
        return signals
    }
}
