package nimby.internal

import nimby.Checkbox

/** Compiled once per native declaration. A value is an immutable bit mask,
 * not a map rebuilt (with boxed entries) for each rule invocation. */
class SignalSettingsMask(checkboxes: List<Checkbox>) {
    private val names = checkboxes.map { it.name }
    private val indices = names.withIndex().associate { it.value to it.index }
    val defaults: Map<String, Boolean>

    init {
        require(names.size <= 64 && indices.size == names.size)
        defaults = read(checkboxes.foldIndexed(0L) { i, mask, box -> if(box.defaultValue) mask or (1L shl i) else mask })
    }

    fun read(mask: Long): Map<String, Boolean> = object : AbstractMap<String, Boolean>() {
        override val size: Int get() = names.size
        override fun containsKey(key: String) = key in indices
        override fun get(key: String): Boolean? = indices[key]?.let { mask and (1L shl it) != 0L }
        override val entries: Set<Map.Entry<String, Boolean>> get() = object : AbstractSet<Map.Entry<String, Boolean>>() {
            override val size: Int get() = names.size
            override fun iterator(): Iterator<Map.Entry<String, Boolean>> = object : Iterator<Map.Entry<String, Boolean>> {
                private var index = 0
                override fun hasNext() = index < names.size
                override fun next(): Map.Entry<String, Boolean> {
                    if(!hasNext()) throw NoSuchElementException()
                    val bit = index++
                    return object : Map.Entry<String, Boolean> {
                        override val key = names[bit]
                        override val value = mask and (1L shl bit) != 0L
                        override fun equals(other: Any?) = other is Map.Entry<*, *> && key == other.key && value == other.value
                        override fun hashCode() = key.hashCode() xor value.hashCode()
                        override fun toString() = "$key=$value"
                    }
                }
            }
        }
    }
}
