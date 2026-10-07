package fr.nimby.sdk

internal class LineTagInheritance(private val lines: Map<LineId, Line>) {
    private val results = HashMap<LineId, List<Tag>?>()
    @Synchronized fun read(id: LineId): List<Tag>? {
        if(results.containsKey(id)) return results[id]
        val visited = HashSet<LineId>(); val tags = LinkedHashMap<TagId, Tag>()
        var current: LineId? = id
        while(current != null) {
            if(visited.size >= 256 || !visited.add(current)) return null.also { results[id] = null }
            val line = lines[current] ?: return null.also { results[id] = null }
            val declared = line.declaredTags ?: return null.also { results[id] = null }
            if(!line.parentInformationAvailable) return null.also { results[id] = null }
            for(tag in declared) {
                if(tags.size >= 65_536 && tag.id !in tags) return null.also { results[id] = null }
                tags.putIfAbsent(tag.id, tag)
            }
            current = line.parentId
        }
        return tags.values.toList().also { results[id] = it }
    }
}
