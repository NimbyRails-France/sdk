package nimby

/** Présence d'un train dans le canton observé, jamais une permission de passage. */
enum class TrainPresence { Unknown, Present, Absent }

/**
 * Trains détectés dans le canton, y compris lorsqu'il reste seulement leur queue.
 * Une liste incomplète peut prouver une présence, mais jamais une absence.
 * Utiliser [Observation.blockTrains] pour respecter la fraîcheur de l'observation.
 * Un canton vide à cet instant ne prouve ni entrée antérieure ni libération d'itinéraire.
 */
class BlockTrains private constructor(private val ids: LongArray, val complete: Boolean) {
    /** Vue en lecture seule, indépendante de la collection fournie au constructeur. */
    val trainIds: List<Long> = object : AbstractList<Long>() {
        override val size: Int get() = ids.size
        override fun get(index: Int): Long = ids[index]
    }

    /** Recherche sans allocation. Un identifiant invalide reste inconnu. */
    fun presenceOf(trainId: Long): TrainPresence = when {
        !validTrain(trainId) -> TrainPresence.Unknown
        known(trainId) -> TrainPresence.Present
        complete -> TrainPresence.Absent
        else -> TrainPresence.Unknown
    }

    private fun known(trainId: Long): Boolean {
        var first = 0
        var last = ids.size
        while (first < last) {
            val middle = first + (last - first) / 2
            if (ids[middle] < trainId) first = middle + 1 else last = middle
        }
        return first < ids.size && ids[first] == trainId
    }

    internal fun incomplete(): BlockTrains =
        if (!complete) this else if (ids.isEmpty()) Unknown else BlockTrains(ids, false)

    companion object {
        /** Aucune liste qualifiée disponible, notamment avec un ancien adaptateur. */
        val Unknown = BlockTrains(LongArray(0), false)
        private val empty = BlockTrains(LongArray(0), true)
        internal const val maximumTrains = 4096

        /** Construit une observation ou une donnée de test, sans lecture du jeu. */
        fun observed(trainIds: Collection<Long>, complete: Boolean): BlockTrains {
            require(trainIds.size <= maximumTrains) { "Too many observed block trains" }
            if (trainIds.isEmpty()) return if (complete) empty else Unknown
            val copied = trainIds.toLongArray()
            require(copied.all(::validTrain)) { "Invalid observed train identity" }
            // The native reader already orders IDs. Only synthetic/unordered
            // callers need sorting; repeated membership queries use binary search.
            if ((1 until copied.size).any { copied[it] < copied[it - 1] }) copied.sort()
            var count = 0
            for (id in copied) if (count == 0 || copied[count - 1] != id) copied[count++] = id
            return BlockTrains(if (count == copied.size) copied else copied.copyOf(count), complete)
        }

        private fun validTrain(id: Long): Boolean =
            id ushr 48 == 5L && (id and 0x0000ffffffffffffL) != 0L
    }
}
