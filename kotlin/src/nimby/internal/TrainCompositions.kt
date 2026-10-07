package nimby

internal class TrainCompositions(val models: List<VehicleModel>?, private val available: Boolean) {
    private val modelIndex = models?.associateBy { it.id.value }.orEmpty()
    private val groups = arrayOf(HashMap<Long, MutableList<TrainVehicle>>(), HashMap<Long, MutableList<TrainVehicle>>())
    private val results = arrayOf(HashMap<Long, List<TrainVehicle>?>(), HashMap<Long, List<TrainVehicle>?>())
    fun add(train: Long, model: Long, index: Long, profile: Long) {
        require(profile in 0..1 && index in 0..4095)
        val group = groups[profile.toInt()].getOrPut(train) { ArrayList() }
        require(group.size < 4096)
        group += TrainVehicle(index.toInt(), modelIndex[model] ?: VehicleModel(VehicleModelId(model), null, null, null))
    }
    fun finish() {
        for(profile in 0..1) for((train, rows) in groups[profile]) {
            val ordered = rows.sortedBy { it.index }
            results[profile][train] = ordered.takeIf { it.withIndex().all { (index, row) -> row.index == index } }
        }
        groups.forEach { it.clear() }
    }
    fun forTrain(train: Long, profile: Int): List<TrainVehicle>? = if(!available) null else
        if(results[profile].containsKey(train)) results[profile][train] else emptyList()
}

internal fun readToolCompositions(call: TrainDataCall): TrainCompositions {
    val counts = LongArray(2); call(29, counts, doubleArrayOf(), byteArrayOf())
    require(counts[0] in -1..262_144 && counts[1] in -1..16_384)
    val models = if(counts[1] < 0) null else ArrayList<VehicleModel>(counts[1].toInt()).also { output ->
        val values = LongArray(2 + 32); val text = ByteArray(32 * 3 * 257)
        while(output.size < counts[1]) {
            val count = minOf(32, counts[1].toInt() - output.size); values[0] = output.size.toLong(); values[1] = count.toLong()
            call(31, values, doubleArrayOf(), text)
            repeat(count) { row ->
                fun name(field: Int): String {
                    val start = row * 771 + field * 257; var end = start
                    while(end < start + 257 && text[end] != 0.toByte()) end++
                    require(end < start + 257); return text.decodeToString(start, end)
                }
                output += VehicleModel(VehicleModelId(values[2 + row]), name(0), name(1), name(2))
            }
        }
    }
    val result = TrainCompositions(models, counts[0] >= 0)
    val values = LongArray(2 + 32 * 4); var offset = 0
    while(offset < counts[0]) {
        val count = minOf(32, counts[0].toInt() - offset); values[0] = offset.toLong(); values[1] = count.toLong()
        call(30, values, doubleArrayOf(), byteArrayOf())
        repeat(count) { row -> val at = 2 + row * 4; result.add(values[at], values[at + 1], values[at + 2], values[at + 3]) }
        offset += count
    }
    result.finish(); return result
}
