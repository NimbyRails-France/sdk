package nimby

/** Internal copied catalog; memoized inheritance never performs native reads. */
internal class TrainCatalogue(val lines: List<Line>?, val tags: List<Tag>?) {
    private val linesById = lines?.associateBy { it.id }.orEmpty()
    val tagsById = tags?.associateBy { it.id.value }.orEmpty()
    private val inherited by lazy { lines.orEmpty().associate { it.lineId to lazy { collectTags(it.lineId) } } }
    fun line(id: LineId): Line? = linesById[id.value]
    fun tag(id: TagId): Tag? = tagsById[id.value]
    fun tagsForLine(id: LineId): List<Tag>? = inherited[id]?.value
    private fun collectTags(id: LineId): List<Tag>? {
        val visited = HashSet<LineId>()
        val result = LinkedHashMap<TagId, Tag>()
        var current: LineId? = id
        while(current != null) {
            if(visited.size >= 256 || !visited.add(current)) return null
            val line = line(current) ?: return null
            val declared = line.declaredTags ?: return null
            if(!line.parentInformationAvailable) return null
            for(tag in declared) {
                if(result.size >= 65_536 && tag.id !in result) return null
                if(tag.id !in result) result[tag.id] = tag
            }
            current = line.parentLineId
        }
        return result.values.toList()
    }
}

internal fun readToolCatalogue(call: TrainDataCall): TrainCatalogue {
    val counts = LongArray(2); call(24, counts, doubleArrayOf(), byteArrayOf())
    require(counts[0] in -1..1_000_000 && counts[1] in -1..16_384)
    val tags = if(counts[1] < 0) null else ArrayList<Tag>(counts[1].toInt()).also { output ->
        val integers = LongArray(2 + 32); val text = ByteArray(32 * 257)
        while(output.size < counts[1]) {
            val rows = minOf(32, counts[1].toInt() - output.size)
            integers[0] = output.size.toLong(); integers[1] = rows.toLong()
            call(26, integers, doubleArrayOf(), text)
            repeat(rows) { row -> output += Tag(TagId(integers[2 + row]), text.catalogueName(row * 257)) }
        }
    }
    val tagsById = tags?.associateBy { it.id.value }.orEmpty()
    val lines = if(counts[0] < 0) null else ArrayList<Line>(counts[0].toInt()).also { output ->
        val integers = LongArray(2 + 32 * 6); val text = ByteArray(32 * 257)
        while(output.size < counts[0]) {
            val rows = minOf(32, counts[0].toInt() - output.size)
            integers[0] = output.size.toLong(); integers[1] = rows.toLong()
            call(25, integers, doubleArrayOf(), text)
            val objectTags = readToolTags(List(rows) { row -> integers[2 + row * 6] to integers[6 + row * 6] }, tagsById, call)
            repeat(rows) { row ->
                val at = 2 + row * 6; val id = integers[at]
                output += Line(id, if(integers[at + 5] == 1L) text.catalogueName(row * 257) else null, integers[at + 3].takeIf { it in 0..2 }?.let { it == 1L },
                    integers[at + 1].takeIf { it != 0L }?.let(::LineId), integers[at + 2] == 1L, objectTags[id])
            }
        }
    }
    return TrainCatalogue(lines, tags)
}

/** Groups associations into at most 4096 tags per bounded native copy. A full
 * 4096-tag object is supported, never truncated to a partial "known" list. */
internal fun readToolTags(objects: List<Pair<Long, Long>>, tags: Map<Long, Tag>, call: TrainDataCall): Map<Long, List<Tag>?> {
    val result = HashMap<Long, List<Tag>?>(objects.size)
    var index = 0
    while(index < objects.size) {
        val group = ArrayList<Pair<Long, Int>>(32); var count = 0
        while(index < objects.size && group.size < 32) {
            val objectTags = objects[index]
            require(objectTags.second in -1..4096) { "Nombre de tags invalide" }
            val size = objectTags.second.toInt()
            if(size <= 0) { result[objectTags.first] = if(size == 0) emptyList() else null; index++; continue }
            if(count + size > 4096) break
            group += objectTags.first to size; count += size; index++
        }
        if(group.isEmpty()) continue
        val integers = LongArray(1 + group.size + count)
        integers[0] = group.size.toLong(); group.forEachIndexed { i, objectTags -> integers[1 + i] = objectTags.first }
        call(27, integers, doubleArrayOf(), byteArrayOf())
        var offset = 1 + group.size
        for((id, size) in group) {
            result[id] = List(size) { val tagId = integers[offset++]; tags[tagId] ?: Tag(TagId(tagId), null) }
        }
    }
    return result
}

private fun ByteArray.catalogueName(offset: Int): String {
    var end = offset
    while(end < offset + 257 && this[end] != 0.toByte()) end++
    require(end < offset + 257) { "Libellé de catalogue invalide" }
    return decodeToString(offset, end)
}
