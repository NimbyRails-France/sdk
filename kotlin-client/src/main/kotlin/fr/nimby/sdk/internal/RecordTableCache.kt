package fr.nimby.sdk.internal

import com.sun.jna.Memory

/** Per-client value cache, never an observation cache. Every lookup compares
 * the complete table just copied from a fresh native snapshot. No deadline,
 * hash collision or assumed map revision can hide a changed record. */
internal class RecordTableCache(private val limit: Long = 32L * 1024 * 1024) : AutoCloseable {
    private class Entry(val bytes: Memory, val value: Any)
    private val entries = LinkedHashMap<String, Entry>(8, .75f, true)
    private var retained = 0L

    fun invalidate(key: String) {
        entries.remove(key)?.let { retained -= it.bytes.size(); it.bytes.close() }
    }

    @Suppress("UNCHECKED_CAST")
    fun <T : Any> read(key: String, source: Memory?, bytes: Long, decode: () -> T): T {
        if(bytes == 0L || bytes > limit) { invalidate(key); return decode() }
        require(source != null && bytes <= source.size())
        val previous = entries[key]
        if(previous != null && previous.bytes.size() == bytes &&
            previous.bytes.getByteBuffer(0, bytes).mismatch(source.getByteBuffer(0, bytes)) == -1) {
            return previous.value as T
        }
        // Decode first: a malformed row must not install a partially decoded
        // value. The stored bytes own their allocation; the scratch buffer is
        // reused by the very next table and cannot be retained here.
        val value = decode()
        invalidate(key)
        while(retained + bytes > limit) invalidate(entries.keys.first())
        val owned = Memory(bytes)
        try { owned.getByteBuffer(0, bytes).put(source.getByteBuffer(0, bytes)) }
        catch(failure: Throwable) { owned.close(); throw failure }
        entries[key] = Entry(owned, value); retained += bytes
        return value
    }

    override fun close() { entries.values.forEach { it.bytes.close() }; entries.clear(); retained = 0 }
}
