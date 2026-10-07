package fr.nimby.sdk.internal

/** Owned, read-only records. A primary-ID index follows the list's lifetime,
 * so exact table reuse also reuses its index across observation snapshots. */
internal class RecordList<T>(private val rows: List<T>) : AbstractList<T>(), java.util.RandomAccess {
    override val size get() = rows.size
    override fun get(index: Int): T = rows[index]
    @Volatile private var primaryIndex: Map<Long, T>? = null

    fun indexById(id: (T) -> Long): Map<Long, T> {
        primaryIndex?.let { return it }
        return synchronized(this) {
            primaryIndex ?: java.util.Collections.unmodifiableMap(rows.associateBy(id)).also { primaryIndex = it }
        }
    }
}

/** The selector must be the record type's primary ID (train, station, track,
 * or platform track). Caller-provided ordinary lists remain supported. */
internal fun <T> List<T>.indexRecords(id: (T) -> Long): Map<Long, T> =
    if(this is RecordList<T>) indexById(id) else associateBy(id)
