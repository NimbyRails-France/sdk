package nimby

/** Extrémités géométriques d'une voie ; le sens de circulation est indépendant. */
enum class ToolTrackEnd { A, B }

/** Raccordement observé, sans choisir une branche d'aiguille.
 * Unknown ne signifie jamais une fin de voie : la capture peut être incomplète. */
sealed interface ToolTrackConnection {
    data object Unknown : ToolTrackConnection
    data object Junction : ToolTrackConnection
    data class Join(val trackId: Long, val entry: ToolTrackEnd) : ToolTrackConnection
}

/** Voie exploitable pour un calcul de distance. Les aiguilles sont exprimées
 * en mètres depuis A, triées, y compris les aiguilles abordées en talon. */
data class ToolRouteTrack(
    val id: Long,
    val lengthM: Double,
    val a: ToolTrackConnection,
    val b: ToolTrackConnection,
    val junctionOffsetsM: List<Double>,
) {
    fun connection(end: ToolTrackEnd): ToolTrackConnection = if (end == ToolTrackEnd.A) a else b
}

/** Copie indexée d'une capture pour les outils de parcours.
 * Les voies sans longueur connue et leurs signaux sont omis ; les raccords
 * vers ces voies restent Unknown. Aucune branche n'est choisie implicitement.
 * Conserver cette copie ne dispense pas d'une nouvelle capture avant une pose. */
class ToolTopology internal constructor(network: ToolNetwork) {
    val worldId: String = network.worldId
    val generation: Long = network.generation
    val tracks: List<ToolRouteTrack>
    val signals: List<ToolSignal>
    private val tracksById: Map<Long, ToolTrack>
    private val junctionsByTrack: Map<Long, List<Double>>
    private val branchEnds: Map<Long, Int>
    private val signalsById: Map<Long, ToolSignal>

    init {
        val observed = HashMap<Long, ToolTrack>(network.tracks.size)
        val usableTracks = ArrayList<ToolTrack>(network.tracks.size)
        for (track in network.tracks) {
            require(track.id != 0L && observed.put(track.id, track) == null) { "Voies dupliquées ou sans identité" }
            require(track.lengthM == null || (track.lengthM.isFinite() && track.lengthM > 0)) { "Longueur de voie invalide" }
            if (track.lengthM != null) usableTracks.add(track)
        }
        val junctions = HashMap<Long, MutableList<Double>>()
        val branchEnds = HashMap<Long, Int>()
        for (junction in network.junctions) {
            require(junction.mainTrack != junction.branchTrack && junction.fraction.isFinite() && junction.fraction in 0.0..1.0 &&
                (junction.mainDirection == -1 || junction.mainDirection == 1) &&
                (junction.branchDirection == -1 || junction.branchDirection == 1)) { "Raccordement d'aiguille invalide" }
            observed[junction.mainTrack]?.lengthM?.let { length ->
                junctions.getOrPut(junction.mainTrack) { mutableListOf() }.add(length * junction.fraction)
            }
            val bit = if (junction.branchDirection == 1) 1 else 2
            branchEnds[junction.branchTrack] = (branchEnds[junction.branchTrack] ?: 0) or bit
        }
        junctionsByTrack = junctions.mapValues { (_, values) ->
            values.sort()
            // Keep the backing array private: a route may share these offsets,
            // but a caller must not mutate the next lookup through a List cast.
            val copy = values.toDoubleArray()
            object : AbstractList<Double>() {
                override val size: Int get() = copy.size
                override fun get(index: Int): Double = copy[index]
            }
        }
        val seenSignals = HashSet<Long>(network.signals.size)
        val indexedSignals = LinkedHashMap<Long, ToolSignal>(network.signals.size)
        for (signal in network.signals) {
            require(signal.id != 0L && seenSignals.add(signal.id)) { "Signaux dupliqués ou sans identité" }
            if (observed[signal.track]?.lengthM == null) continue
            require(signal.fraction.isFinite() && signal.fraction in 0.0..1.0) { "Position de signal invalide" }
            signal.travelDirection // Validate the normalized direction at the SDK boundary.
            indexedSignals[signal.id] = signal
        }
        tracksById = observed
        this.branchEnds = branchEnds
        signalsById = indexedSignals
        // Validation and ownership are eager; route values are pure lookups.
        // A short route must not allocate connections for the entire map, and
        // asking only for the list size must not materialize those values.
        tracks = object : AbstractList<ToolRouteTrack>() {
            // A consumer iterating the whole map pays once. A single
            // synchronized lazy publishes an owned read-only list; no cache
            // entry or lock is needed for each individual route lookup.
            private val materialized = lazy(LazyThreadSafetyMode.SYNCHRONIZED) {
                val copy = Array(usableTracks.size) { routeTrack(usableTracks[it]) }
                object : AbstractList<ToolRouteTrack>() {
                    override val size: Int get() = copy.size
                    override fun get(index: Int): ToolRouteTrack = copy[index]
                }
            }
            override val size: Int get() = usableTracks.size
            override fun get(index: Int): ToolRouteTrack = if(materialized.isInitialized()) materialized.value[index]
                else routeTrack(usableTracks[index])
            override fun iterator(): Iterator<ToolRouteTrack> = materialized.value.iterator()
            override fun listIterator(): ListIterator<ToolRouteTrack> = materialized.value.listIterator()
            override fun listIterator(index: Int): ListIterator<ToolRouteTrack> = materialized.value.listIterator(index)
        }
        signals = indexedSignals.values.toList()
    }

    private fun connection(track: ToolTrack, end: ToolTrackEnd): ToolTrackConnection {
        val bit = if (end == ToolTrackEnd.A) 1 else 2
        if ((branchEnds[track.id] ?: 0) and bit != 0) return ToolTrackConnection.Junction
        val offsets = junctionsByTrack[track.id]
        if (end == ToolTrackEnd.A && offsets?.firstOrNull() == 0.0 ||
            end == ToolTrackEnd.B && offsets?.lastOrNull() == track.lengthM) return ToolTrackConnection.Junction
        val other = tracksById[if (end == ToolTrackEnd.A) track.linkA else track.linkB] ?: return ToolTrackConnection.Unknown
        if (other.id == track.id || other.lengthM == null || (other.linkA == track.id) == (other.linkB == track.id)) return ToolTrackConnection.Unknown
        return ToolTrackConnection.Join(other.id, if (other.linkA == track.id) ToolTrackEnd.A else ToolTrackEnd.B)
    }

    private fun routeTrack(track: ToolTrack): ToolRouteTrack = ToolRouteTrack(track.id, requireNotNull(track.lengthM),
        connection(track, ToolTrackEnd.A), connection(track, ToolTrackEnd.B), junctionsByTrack[track.id].orEmpty())

    fun track(id: Long): ToolRouteTrack? = tracksById[id]?.takeIf { it.lengthM != null }?.let { routeTrack(it) }
    fun signal(id: Long): ToolSignal? = signalsById[id]
}

/** Prépare les distances, aiguilles et connexions typées de cette capture.
 * Les liens du moteur et le sens natif des signaux restent internes au SDK. */
fun ToolNetwork.topology(): ToolTopology = ToolTopology(this)
