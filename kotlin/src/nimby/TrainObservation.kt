package nimby

/** Instant du calendrier UTC du jeu, précis à la microseconde. Ce n'est pas
 * l'heure de l'ordinateur. [microsecond] est toujours compris entre 0 et 999999. */
data class GameInstant(val utcSeconds: Long, val microsecond: Int = 0) {
    init { require(utcSeconds in -62135596800L..253402300799L && microsecond in 0..999999) }
    fun dateTime(): GameDateTime = GameDateTime.fromUtcSeconds(utcSeconds)
}

enum class TrainState { Unknown, Driving, StationStop, TimedStop, Depot, DispatchWait, SignalWait, Mothballed, NotPresent, Other }
enum class TrainAlert { None, LineClosed, NoPath, InvalidOrders, Collision, SignalWait, ScheduleClosed, DispatchTracksOccupied, NoServices, ServicesAlreadyAssigned, Other }

/** Référence jointe dans la même capture. Un nom absent ne signifie pas une
 * gare inexistante. Les identifiants sont opaques et incluent leur génération. */
data class Station(val id: Long, val name: String?) {
    val stationId: StationId get() = StationId(id)
}
data class Line(val id: Long, val name: String?, val isDepot: Boolean?, val parentLineId: LineId? = null,
                val parentInformationAvailable: Boolean = false, val declaredTags: List<Tag>? = null) {
    val lineId: LineId get() = LineId(id)
    val type: LineType? get() = isDepot?.let { if(it) LineType.Depot else LineType.Other }
}
/** Sens +1 de A vers B, -1 de B vers A ; null si le sens n'est pas connu. */
data class TrainPosition(val trackId: Long, val fraction: Double, val direction: Int?, val station: Station?) {
    val track: TrackId get() = TrackId(trackId)
}

/** Affectation observée, sans résolution de nom d'horaire ni d'heures futures.
 * [orderIndex] est l'index natif, à partir de zéro ; null s'il est absent. */
data class TrainAssignment(val scheduleId: Long?, val shiftId: Long?, val orderIndex: Int?) {
    val timetable: Timetable? get() = scheduleId?.let { Timetable(TimetableId(it)) }
    val shift: TimetableShiftId? get() = scheduleId?.let { schedule -> shiftId?.let { TimetableShiftId(TimetableId(schedule), it) } }
}

/** Échéances actives observées. arrival/departure ne sont pas nécessairement
 * les horaires commerciaux ; dispatchRetry est une nouvelle tentative de
 * dispatch. Le temps restant avant arrivée peut être négatif ; départ et
 * dispatch sont bornés à zéro. Aucun n'est un retard prévisionnel commercial.
 * Un compteur peut être disponible sans
 * calendrier ; sa date sera alors null. Les compteurs sont en microsecondes
 * depuis l'origine de simulation, jamais depuis 1970. */
data class TrainServiceTimes(
    val gameEpochSeconds: Long?, val gameTimeUs: Long?,
    val arrivalTimeUs: Long?, val departureTimeUs: Long?, val dispatchRetryTimeUs: Long?,
    val arrivalRemainingSeconds: Double?, val departureRemainingSeconds: Double?, val dispatchRetryRemainingSeconds: Double?,
) {
    private fun instant(time: Long?): GameInstant? {
        val epoch = gameEpochSeconds ?: return null
        if(time == null) return null
        val seconds = time.floorDiv(1_000_000L)
        if((seconds > 0 && epoch > Long.MAX_VALUE - seconds) || (seconds < 0 && epoch < Long.MIN_VALUE - seconds)) return null
        val utc = epoch + seconds
        return if(utc in -62135596800L..253402300799L) GameInstant(utc, time.mod(1_000_000L).toInt()) else null
    }
    val observedAt: GameInstant? get() = instant(gameTimeUs)
    val arrival: GameInstant? get() = instant(arrivalTimeUs)
    val departure: GameInstant? get() = instant(departureTimeUs)
    val dispatchRetry: GameInstant? get() = instant(dispatchRetryTimeUs)
}

/** Une propriété nulle est indisponible, pas un état libre ou un zéro.
 * [stopIndex] est l'index de l'arrêt courant de la ligne, à partir de zéro. */
data class TrainService(
    val state: TrainState?, val alert: TrainAlert?, val hidden: Boolean?, val onNetwork: Boolean?,
    val locationTrackId: Long?, val locationStation: TrainStation?, val line: TrainLine?,
    val stopTrackId: Long?, val stopStation: TrainStation?, val stopIndex: Int?, val times: TrainServiceTimes,
)

/** Vitesse mesurée uniquement, en m/s ; une vitesse de secours est null.
 * Les passagers sont les occupants, pas la capacité du matériel. */
data class Train(
    val id: Long, val name: String, val position: TrainPosition?, val speedMps: Double?,
    val speedDefaulted: Boolean, val passengers: Int?, val assignment: TrainAssignment?, val service: TrainService?,
    val declaredTags: List<Tag>? = null, val predictedArrivalDelayUs: Long? = null,
    val configured: TrainCharacteristics? = null, val current: TrainCharacteristics? = null,
) {
    val trainId: TrainId get() = TrainId(id)
    val speedKmh: Double? get() = speedMps?.times(3.6)
    /** Estimation native signée : négative si l'arrivée est prévue en avance.
     * Ce n'est pas le temps écoulé depuis une échéance, ni une priorité. */
    val predictedArrivalDelaySeconds: Double? get() = predictedArrivalDelayUs?.div(1_000_000.0)
}

/** Copie conservable après le callback, sans lecture native cachée. Les recherches
 * par identifiant sont indexées. [capturedAtMillis] est l'heure UTC de
 * l'ordinateur ; [ageMillis] son âge monotone au moment de la copie. La capture
 * valide les enregistrements séparément, elle n'arrête pas la simulation. */
class TrainSnapshot internal constructor(
    val worldId: String, val generation: Long, val capturedAtMillis: Long, val ageMillis: Long,
    val clock: ToolClock?, val trains: List<TrainObservation>,
    private val catalogue: TrainCatalogue = TrainCatalogue(null, null),
    val vehicleModels: List<VehicleModel>? = null,
) {
    private val byId = trains.associateBy { it.id }
    operator fun get(id: Long): TrainObservation? = byId[id]
    operator fun get(id: TrainId): Train? = byId[id.value]
    /** Toutes les lignes du catalogue, même sans train affecté. Null : inconnu. */
    val lines: List<Line>? get() = catalogue.lines
    val tags: List<Tag>? get() = catalogue.tags
    fun line(id: LineId): Line? = catalogue.line(id)
    fun tag(id: TagId): Tag? = catalogue.tag(id)
    /** Tags de cette ligne et de ses parents ; null si la chaîne est inconnue,
     * cyclique ou dépasse les limites. Aucun choix de priorité n'est effectué. */
    fun tagsForLine(id: LineId): List<Tag>? = catalogue.tagsForLine(id)
}

/** Un arrêt peut être un waypoint hors gare. Les offsets validés sont relatifs
 * au plan de ligne. Ne pas les convertir en dates absolues :
 * les courses partielles et les boucles nécessitent d'autres informations. */
data class Stop(val index: Int, val trackId: Long, val station: Station?,
                         val arrivalOffsetSeconds: Int?, val departureOffsetSeconds: Int?) {
    val track: TrackId get() = TrackId(trackId)
    val plannedDwellSeconds: Long? get() = arrivalOffsetSeconds?.let { arrival -> departureOffsetSeconds?.toLong()?.minus(arrival) }
}
/** Plan complet de la ligne associée au train dans cette capture. Il peut
 * inclure des arrêts hors de sa course partielle. Aucun horaire futur déduit. */
data class LinePlan(val trainId: Long, val line: Line, val stops: List<Stop>,
                    val worldId: String, val generation: Long, val capturedAtMillis: Long) {
    val train: TrainId get() = TrainId(trainId)
}

internal typealias TrainDataCall = (Int, LongArray, DoubleArray, ByteArray) -> Unit
private const val PAGE = 32
private const val INTEGER_FIELDS = 32
private const val NUMBER_FIELDS = 5
private const val NAME_BYTES = 257
private const val TEXT_FIELDS = 5
private const val UNKNOWN = Long.MIN_VALUE
private fun Long.reference(): Long? = takeIf { it != 0L }
private fun Long.available(): Long? = takeIf { it != UNKNOWN }
private fun Long.availableIndex(): Int? = takeIf { it in 0..Int.MAX_VALUE.toLong() }?.toInt()
private fun Long.truth(): Boolean? = when(this) { 0L -> false; 1L -> true; else -> null }
private fun Double.available(): Double? = takeIf { it.isFinite() }
private fun ByteArray.name(offset: Int): String {
    var end = offset
    while(end < offset + NAME_BYTES && this[end] != 0.toByte()) end++
    require(end < offset + NAME_BYTES) { "Texte d'observation invalide" }
    return decodeToString(offset, end)
}
internal fun readToolTrains(world: String, generation: Long, flags: Int, call: TrainDataCall): TrainSnapshot {
    val header = LongArray(5); header[0] = flags.toLong(); call(20, header, doubleArrayOf(), byteArrayOf())
    require(header[0] in 0..1_000_000 && header[2] >= 0)
    val catalogue = if(flags and 64 != 0) readToolCatalogue(call) else TrainCatalogue(null, null)
    val compositions = if(flags and 128 != 0) readToolCompositions(call) else null
    val count = header[0].toInt()
    val clock = header[3].available()?.let { ToolClock(it, header[4]) }
    if(count == 0) return TrainSnapshot(world, generation, header[1], header[2], clock, emptyList(), catalogue, compositions?.models)
    val result = ArrayList<TrainObservation>(count)
    val integers = LongArray(2 + PAGE * INTEGER_FIELDS)
    val numbers = DoubleArray(PAGE * NUMBER_FIELDS)
    val text = ByteArray(PAGE * NAME_BYTES * TEXT_FIELDS)
    val characteristicIntegers = if(flags and 130 != 0) LongArray(2 + PAGE * 6) else null
    val characteristicNumbers = if(flags and 130 != 0) DoubleArray(PAGE * 12) else null
    while(result.size < count) {
        val rows = minOf(PAGE, count - result.size)
        integers[0] = result.size.toLong(); integers[1] = rows.toLong()
        call(21, integers, numbers, text)
        if(characteristicIntegers != null && characteristicNumbers != null) {
            characteristicIntegers[0] = result.size.toLong(); characteristicIntegers[1] = rows.toLong()
            call(28, characteristicIntegers, characteristicNumbers, byteArrayOf())
        }
        val declaredTags = if(flags and 8 != 0) readToolTags(List(rows) { row -> integers[2 + row * INTEGER_FIELDS] to integers[2 + row * INTEGER_FIELDS + 29] }, catalogue.tagsById, call) else emptyMap()
        repeat(rows) { row ->
            val at = 2 + row * INTEGER_FIELDS; val number = row * NUMBER_FIELDS; val names = row * NAME_BYTES * TEXT_FIELDS
            fun name(index: Int) = text.name(names + index * NAME_BYTES)
            fun station(id: Long, nameIndex: Int) = id.reference()?.let { TrainStation(it, name(nameIndex).takeIf(String::isNotEmpty)) }
            fun characteristics(profile: Int): TrainCharacteristics? {
                if(characteristicIntegers == null || characteristicNumbers == null) return null
                val values = row * 12 + profile * 6; val counts = 2 + row * 6 + profile * 3
                val cars = characteristicIntegers[counts].availableIndex(); val capacity = characteristicIntegers[counts + 1].availableIndex()
                val composition = if(characteristicIntegers[counts + 2] == 1L) compositions?.forTrain(integers[at], profile) else null
                if(cars == null && capacity == null && composition == null && (0..5).all { !characteristicNumbers[values + it].isFinite() }) return null
                return TrainCharacteristics(characteristicNumbers[values].available(), characteristicNumbers[values + 1].available(), characteristicNumbers[values + 2].available(),
                    capacity, cars, characteristicNumbers[values + 3].available(), characteristicNumbers[values + 4].available(), characteristicNumbers[values + 5].available(), composition)
            }
            val service = if(integers[at + 4] == 0L) null else TrainService(
                integers[at + 5].takeIf { it >= 0 }?.let { TrainState.entries.getOrElse(it.toInt()) { TrainState.Other } },
                when(integers[at + 6]) { -1L -> null; 0L -> TrainAlert.None; 1L -> TrainAlert.LineClosed; 3L -> TrainAlert.NoPath; 4L -> TrainAlert.InvalidOrders; 5L -> TrainAlert.Collision; 6L -> TrainAlert.SignalWait; 7L -> TrainAlert.ScheduleClosed; 8L -> TrainAlert.DispatchTracksOccupied; 9L -> TrainAlert.NoServices; 10L -> TrainAlert.ServicesAlreadyAssigned; else -> TrainAlert.Other },
                integers[at + 7].truth(), integers[at + 8].truth(), integers[at + 9].reference(), station(integers[at + 10], 2),
                integers[at + 11].reference()?.let { catalogue.line(LineId(it)) ?: Line(it, name(1).takeIf(String::isNotEmpty), integers[at + 15].takeIf { kind -> kind in 0..2 }?.let { kind -> kind == 1L }) },
                integers[at + 12].reference(), station(integers[at + 13], 3), integers[at + 14].availableIndex(),
                TrainServiceTimes(integers[at + 16].available(), integers[at + 17].available(), integers[at + 18].available(), integers[at + 19].available(), integers[at + 20].available(),
                    numbers[number + 2].available(), numbers[number + 3].available(), numbers[number + 4].available()))
            val assignment = if(integers[at + 21] == 0L) null else TrainAssignment(integers[at + 23].reference(), integers[at + 24].reference(), integers[at + 25].availableIndex())
            result += TrainObservation(integers[at], name(0), integers[at + 1].reference()?.let {
                TrainPosition(it, numbers[number], integers[at + 2].toInt().takeIf { direction -> direction == -1 || direction == 1 }, station(integers[at + 28], 4)) },
                if(integers[at + 3] != 0L) null else numbers[number + 1].available(), integers[at + 3] != 0L,
                integers[at + 22].availableIndex(), assignment, service, declaredTags[integers[at]], integers[at + 30].available(), characteristics(0), characteristics(1))
        }
    }
    return TrainSnapshot(world, generation, header[1], header[2], clock, result, catalogue, compositions?.models)
}

internal fun readToolLinePlan(trainId: Long, world: String, generation: Long, call: TrainDataCall): TrainLinePlan? {
    val header = longArrayOf(trainId, 0, 0, 0, 0)
    val name = ByteArray(NAME_BYTES)
    call(22, header, doubleArrayOf(), name)
    if(header[1] == -1L) return null
    require(header[1] in 0..16_384)
    val line = TrainLine(header[2], name.name(0).takeIf(String::isNotEmpty), header[3].takeIf { it in 0..2 }?.let { it == 1L })
    val count = header[1].toInt(); val stops = ArrayList<TrainLineStop>(count)
    val values = LongArray(3 + PAGE * 5); val text = ByteArray(PAGE * NAME_BYTES)
    while(stops.size < count) {
        val rows = minOf(PAGE, count - stops.size)
        values[0] = trainId; values[1] = stops.size.toLong(); values[2] = rows.toLong()
        call(23, values, doubleArrayOf(), text)
        repeat(rows) { row ->
            val at = 3 + row * 5
            stops += TrainLineStop(values[at].toInt(), values[at + 1], values[at + 2].reference()?.let { TrainStation(it, text.name(row * NAME_BYTES).takeIf(String::isNotEmpty)) },
                values[at + 3].available()?.toInt(), values[at + 4].available()?.toInt())
        }
    }
    return TrainLinePlan(trainId, line, stops, world, generation, header[4])
}
