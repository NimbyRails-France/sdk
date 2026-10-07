package fr.nimby.sdk

internal class TrainCompositions(val models: List<VehicleModel>?, private val available: Boolean) {
    private val modelIndex = models?.associateBy { it.id.value }.orEmpty()
    private val groups = arrayOf(HashMap<Long, MutableList<TrainVehicle>>(), HashMap<Long, MutableList<TrainVehicle>>())
    private val results = arrayOf(HashMap<Long, List<TrainVehicle>?>(), HashMap<Long, List<TrainVehicle>?>())
    fun add(train: Long, model: Long, index: Int, profile: Int) {
        require(profile in 0..1 && index in 0..4095)
        val group = groups[profile].getOrPut(train) { ArrayList() }; require(group.size < 4096)
        group += TrainVehicle(index, modelIndex[model] ?: VehicleModel(VehicleModelId(model), null, null, null))
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
