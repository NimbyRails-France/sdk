package fr.nimby.sdk

import com.sun.jna.Memory
import fr.nimby.sdk.internal.RecordTableCache
import fr.nimby.sdk.internal.RecordList
import fr.nimby.sdk.internal.indexRecords
import kotlin.test.*

class RecordTableCacheTest {
    @Test fun aSharedRecordListBuildsItsPrimaryIndexOnlyOnce() {
        val rows = RecordList(List(100_000) { Track(it.toLong(), null, 80.0) })
        var visited = 0
        val first = rows.indexRecords { visited++; it.id }
        assertSame(first, rows.indexRecords { visited++; it.id })
        assertEquals(100_000, visited); assertEquals(rows.last(), first[99_999])
        assertFailsWith<UnsupportedOperationException> { (first as MutableMap).clear() }
        assertNotSame(first, RecordList(rows.toList()).indexRecords { it.id })
    }
    @Test fun exactBytesCountAndAvailabilityControlReuseWithoutLeakingScratchMemory() {
        Memory(32).use { memory -> RecordTableCache(64).use { cache ->
            memory.clear(); var decoded = 0
            fun read(bytes: Long = 16) = cache.read("nodes", memory, bytes) { decoded++; listOf(memory.getLong(0), bytes) }
            val first = read(); assertSame(first, read()); assertEquals(1, decoded)
            memory.setByte(15, 1); val changed = read(); assertNotSame(first, changed)
            assertEquals(first, changed) // Reserved/unused bytes still invalidate safely.
            val longer = read(24); assertNotSame(changed, longer)
            memory.setLong(0, 19); assertEquals(0L, first[0]); assertEquals(19L, read(24)[0])
            cache.invalidate("nodes"); assertNotSame(longer, read(24))
        } }
    }

    @Test fun retentionBudgetEvictsAndOversizeValuesAreNeverRetained() {
        Memory(32).use { memory -> RecordTableCache(24).use { cache ->
            memory.clear()
            fun read(key: String, bytes: Long = 16) = cache.read(key, memory, bytes) { Any() }
            val first = read("a"); assertSame(first, read("a"))
            read("b"); assertNotSame(first, read("a"))
            val large = read("large", 32); assertNotSame(large, read("large", 32))
            val empty = read("empty", 0); assertNotSame(empty, read("empty", 0))
        } }
    }
}
