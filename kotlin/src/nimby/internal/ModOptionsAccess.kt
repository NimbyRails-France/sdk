package nimby.internal

import nimby.*

/** SDK-private schema and value transport. The adapter owns this instance and
 * calls [apply] only on the mod's serialized worker, before invoking mod code. */
class ModOptionsAccess(options: List<ModOption<*>>, windowCount: Int = 0) {
    private val options = checkedModOptions(options, windowCount)
    val count: Int get() = options.size

    fun info(index: Int): IntArray {
        val option = options[index]
        return intArrayOf(option.kind, (option as? IntegerOption)?.minimum ?: 0,
            (option as? IntegerOption)?.maximum ?: 0, (option as? ChoiceOption)?.choices?.size ?: 0)
    }

    fun metadata(index: Int, field: Int): String = options[index].let { option ->
        when (field) {
            0 -> option.id
            1 -> option.label
            2 -> option.description
            3 -> option.encodedDefault
            else -> throw IllegalArgumentException("Unknown mod option metadata field")
        }
    }

    fun choiceMetadata(index: Int, choice: Int, field: Int): String {
        val option = options[index] as? ChoiceOption ?: throw IllegalArgumentException("Option is not a choice")
        val entry = option.choices[choice]
        return when (field) {
            0 -> entry.id
            1 -> entry.label
            else -> throw IllegalArgumentException("Unknown mod option choice field")
        }
    }

    /** A complete ordered UTF-8 snapshot, with a trailing NUL for each value.
     * Malformed snapshots leave every previous value unchanged. No callbacks. */
    fun apply(packed: ByteArray, count: Int) {
        require(count == options.size && packed.size <= 64 * 257) { "Invalid mod option snapshot size" }
        if (count == 0) { require(packed.isEmpty()); return }
        require(packed.isNotEmpty() && packed.last() == 0.toByte()) { "Missing option value terminator" }
        val fields = packed.decodeToString(throwOnInvalidSequence = true).dropLast(1).split('\u0000')
        require(fields.size == count && fields.all { it.encodeToByteArray().size <= 256 }) { "Invalid option value count or size" }
        val prepared = options.indices.map { options[it].prepareUpdate(fields[it]) }
        prepared.forEach { it() }
    }
}
