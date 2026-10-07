package nimby

/** Lecture groupée selon le besoin. Par défaut : état/service et localisation.
 * Les caractéristiques, occupants, horaires et catalogues restent à la demande.
 * Les horaires incluent le service ; les tags incluent le catalogue des lignes. */
data class TrainQuery(val includeService: Boolean = true, val includeLocations: Boolean = true,
    val includeCharacteristics: Boolean = false, val includeTimetables: Boolean = false,
    val includeTags: Boolean = false, val includePassengers: Boolean = false, val includeLines: Boolean = false,
    val includeComposition: Boolean = false) {
    internal val flags: Int get() = (if(includeService || includeTimetables) 1 else 0) or
        (if(includeCharacteristics) 2 else 0) or (if(includeTimetables) 4 else 0) or
        (if(includeTags) 8 else 0) or (if(includePassengers) 16 else 0) or
        (if(includeLocations) 32 else 0) or (if(includeLines || includeTags) 64 else 0) or (if(includeComposition) 128 else 0)
}

/** Identifiants opaques copiés du jeu. Les types empêchent de confondre train,
 * ligne, gare et voie. Ne pas découper leur valeur ni déduire une catégorie. */
data class TrainId(val value: Long)
data class LineId(val value: Long)
data class StationId(val value: Long)
data class TrackId(val value: Long)
data class TimetableId(val value: Long)
/** Une clé de service n'est unique que dans son horaire. */
data class TimetableShiftId(val timetableId: TimetableId, val value: Long)
/** Identifiant du catalogue de tags ; ce n'est pas un identifiant de train. */
data class TagId(val value: Long)
data class VehicleModelId(val value: Long)

/** Seule la classification dépôt est actuellement prouvée. Other ne signifie
 * ni voyageurs, ni fret, ni catégorie commerciale particulière. */
enum class LineType { Depot, Other }

/** Horaire identifié par une affectation observée. Son nom n'est pas encore
 * résolu ; null ne signifie pas qu'il est vide. Aucun arrêt futur n'est déduit. */
data class Timetable(val id: TimetableId, val name: String? = null)
/** Libellé de classement observé. Un tag ne confère aucune priorité. */
data class Tag(val id: TagId, val name: String?)

/** Caractéristiques du matériel, distinctes de sa vitesse actuelle et de la
 * limite de la voie. Configuré/acheté et composition actuelle restent séparés.
 * Toute mesure non prouvée reste null, sans remplacement par l'autre profil. */
data class TrainCharacteristics(val maximumSpeedMps: Double?, val lengthM: Double?, val emptyMassKg: Double?,
    val passengerCapacity: Int?, val carCount: Int?, val maximumAccelerationMps2: Double?, val powerW: Double?, val tractiveForceN: Double?,
    val composition: List<TrainVehicle>? = null) {
    val maximumSpeedKmh: Double? get() = maximumSpeedMps?.times(3.6)
}
/** Modèle référencé par une composition observée. Le nom est explicitement
 * celui du catalogue anglais, pas une catégorie voyageurs/fret inférée. */
data class VehicleModel(val id: VehicleModelId, val code: String?, val nameEnglish: String?, val sourceName: String?)
data class TrainVehicle(val index: Int, val model: VehicleModel) {
    val modelId: VehicleModelId get() = model.id
}

// The detailed names remain aliases for callers using the initial observation API.
typealias TrainObservation = Train
typealias TrainLine = Line
typealias TrainStation = Station
typealias TrainLineStop = Stop
typealias TrainLinePlan = LinePlan
