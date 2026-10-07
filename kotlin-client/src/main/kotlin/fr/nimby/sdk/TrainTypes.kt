package fr.nimby.sdk

/** Batch train query. Expensive catalogs and material/occupant/plan reads are
 * explicitly requested; default reads service and locations only. */
data class TrainQuery(val includeService: Boolean = true, val includeLocations: Boolean = true,
    val includeCharacteristics: Boolean = false, val includeTimetables: Boolean = false,
    val includeTags: Boolean = false, val includePassengers: Boolean = false, val includeLines: Boolean = false,
    val includeComposition: Boolean = false) {
    internal val flags: Int get() = (if(includeService || includeTimetables) 1 else 0) or
        (if(includeCharacteristics) 2 else 0) or (if(includeTimetables) 4 else 0) or
        (if(includeTags) 8 else 0) or (if(includePassengers) 16 else 0) or
        (if(includeLocations) 32 else 0) or (if(includeLines || includeTags) 64 else 0) or (if(includeComposition) 128 else 0)
}

/** Opaque identities; no train category or memory address is encoded by this API. */
data class TrainId(val value: Long)
data class LineId(val value: Long)
data class StationId(val value: Long)
data class TrackId(val value: Long)
data class TimetableId(val value: Long)
/** Shift keys are scoped to their timetable. */
data class TimetableShiftId(val timetableId: TimetableId, val value: Long)
data class TagId(val value: Long)
data class VehicleModelId(val value: Long)
enum class LineType { Depot, Other }
/** Only the timetable identity is currently proven; no manufactured name or future times. */
data class Timetable(val id: TimetableId, val name: String? = null)
data class Tag(val id: TagId, val name: String?)
data class Line(val id: LineId, val name: String?, val type: LineType?, val parentId: LineId?,
                val parentInformationAvailable: Boolean, val declaredTags: List<Tag>?)
data class TrainMetadata(val trainId: TrainId, val predictedArrivalDelayUs: Long?, val declaredTags: List<Tag>? = null,
                         val configured: TrainCharacteristics? = null, val current: TrainCharacteristics? = null) {
    /** Native estimate, signed: negative means predicted early. Not deadline age. */
    val predictedArrivalDelaySeconds: Double? get() = predictedArrivalDelayUs?.div(1_000_000.0)
}
/** Stop timings remain offsets relative to a line plan. */
typealias Stop = LineStop

/** Material limits, not current speed or track speed limits. Configured and
 * current profiles are independent; neither fills missing values of the other. */
data class TrainCharacteristics(val maximumSpeedMps: Double?, val lengthM: Double?, val emptyMassKg: Double?,
    val passengerCapacity: Int?, val carCount: Int?, val maximumAccelerationMps2: Double?, val powerW: Double?, val tractiveForceN: Double?,
    val composition: List<TrainVehicle>? = null) {
    val maximumSpeedKmh: Double? get() = maximumSpeedMps?.times(3.6)
}
/** Only referenced models. Names are the English catalog labels, never an
 * inferred passenger/freight classification. Missing catalog values stay null. */
data class VehicleModel(val id: VehicleModelId, val code: String?, val nameEnglish: String?, val sourceName: String?)
data class TrainVehicle(val index: Int, val model: VehicleModel) {
    val modelId: VehicleModelId get() = model.id
}
