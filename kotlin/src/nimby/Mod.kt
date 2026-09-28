package nimby

/** API de mod Kotlin. Les unités sont mètres, secondes, m/s, kg, N et W. */
/** Case du panneau d'extensions du signal sélectionné.
 * name est la clé technique enregistrée, jamais affichée ; label apparaît à côté
 * de la case et description sous la case, avec retour à la ligne automatique.
 * Une description vide n'ajoute aucune ligne. label et description acceptent tr.
 * defaultValue est utilisé quand aucun réglage n'a encore été enregistré.
 * La case ne change pas une règle seule : consulter enabled dans rules. */
data class Checkbox(val name: String, val label: String, val description: String, val defaultValue: Boolean = false,
    /** Pour un avertissement importé : visible tant qu'il reste à acquitter. */
    val onlyWhenEnabled: Boolean = false)

/** Un modèle constructible du mod. Les identifiants restent stables entre versions
 * pour retrouver les textures et les réglages des parties existantes.
 * Les noms de cases sont locaux au type : deux types peuvent avoir une case
 * « active » avec des valeurs par défaut différentes. */
data class SignalType(
    val id: String,
    val title: String,
    val textureSet: String,
    val checkboxes: List<Checkbox> = emptyList(),
    /** Demande l'observation d'un train orienté vers ce signal, dans [approachBlocks] cantons en amont.
     * Ne constitue pas une réservation ni une autorisation de mouvement. */
    val observeApproach: Boolean = false,
    val actions: List<SignalAction> = emptyList(),
    /** Portée lorsque [observeApproach] est activé : 1 = canton immédiatement
     * derrière le signal ; 2 = les deux cantons précédents. De 1 à 16.
     * Le parcours suit la tête du train, sans choisir de branche à une aiguille.
     * Le SDK fournit une observation ; le mod décide si elle autorise l'ouverture. */
    val approachBlocks: Int = 1,
    /** Ressources et entrée constructible, utilisées pour générer mod.txt. */
    val construction: SignalConstruction? = null,
    val numbers: List<NumberSetting> = emptyList()
)
/** Occupation physique : l'entrée de la tête suffit à occuper un canton et
 * l'arrière doit le dégager pour le libérer. Une portion aval observée peut
 * prouver [Occupied] même sans limite connue ; elle ne prouve jamais [Clear].
 * Le modèle de signal reste responsable de l'indication à afficher. */
enum class Occupancy { Unknown, Clear, Occupied }
enum class SettingsStatus { Unavailable, Absent, Present }
data class Observation(
    val block: Occupancy = Occupancy.Unknown, val fresh: Boolean = false,
    val routeKnown: Boolean = false, val forcedStop: Boolean = false,
    val lampFailed: Boolean = false, val redFlashCondition: Boolean = false, val next: Int = 0,
    val approachingTrain: Long? = null
)

// The native object tag is an SDK detail. Invalid input cannot become evidence
// of an approaching train, including in locally constructed test observations.
internal fun Observation.validatedApproach(): Observation =
    if (approachingTrain != null && approachingTrain ushr 48 != 5L)
        copy(fresh = false, approachingTrain = null) else this
/** Transport opaque, propre à la déclaration du mod. Ne pas persister ces codes
 * ni les utiliser comme indices d'enum : SignallingMod.indication les décode.
 * Les recettes du banc utilisent séparément les ordinaux locaux du modèle. */
data class Decision(val aspect: Int, val reason: Int)
data class Signal(
    val id: Long = 0, val nextSignal: Long = 0, val settings: Map<String, Boolean> = emptyMap(),
    val observation: Observation = Observation(), val settingsStatus: SettingsStatus = SettingsStatus.Present,
    /** Identifiant de SignalType, fourni par le SDK à partir du catalogue observé. */
    val type: String = ""
)
data class Vehicle(
    val maxSpeedMps: Double = 0.0, val maxAccelerationMps2: Double = 0.0,
    val serviceBrakingMps2: Double = 0.0, val tractiveEffortN: Double = 0.0,
    val powerW: Double = 0.0, val emptyMassKg: Double = 0.0,
    val extraMassKg: Double = 0.0, val lengthM: Double = 0.0
)
data class DrivingSettings(val brakeUse: Double = 0.8, val responseSeconds: Double = 2.0, val marginM: Double = 10.0)
data class Constraint(val source: Long, val beginM: Double, val endM: Double, val speedMps: Double, val releaseByRear: Boolean = true)
data class DrivingInput(
    val headM: Double = 0.0, val speedMps: Double = 0.0, val lineSpeedMps: Double = 0.0,
    val fresh: Boolean = false, val routeKnown: Boolean = false,
    val onSight: Boolean = false, val visibleClearM: Double? = null
)
data class DrivingPlan(
    val available: Boolean = false, val speedCeilingMps: Double = 0.0,
    val serviceDecelerationMps2: Double = 0.0, val accelerationMps2: Double = 0.0,
    val brakingRequired: Boolean = false, val limitingSource: Long = 0
)
enum class DrivingFlag(val bit: Int) {
    Clear(1), HoldToClear(2), Stop(4), FollowTarget(8), OnSight(16), StopThenProceed(32), CancelAtNextClear(64),
    /** Passage permis selon la vitesse memorisee ; ne libere aucune autre restriction. */
    ApproachPassable(128)
}
data class DrivingRule(
    val speedMps: Double = -1.0, val reopenedSpeedMps: Double = 0.0,
    val signalsAhead: Int = 0, val flags: Set<DrivingFlag> = emptySet()
)

/** Une seule implémentation Kotlin suffit. Le SDK fournit DLL, exports et boucle de lecture. */
abstract class SignallingMod : GameMod() {
    /** Pure transformation of effective settings in this observed network.
     * Never persists derived values or changes topology/observations. */
    open fun prepareNetwork(signals: List<Signal>): List<Signal> = signals
    // Compatibilité des mods à un type. Les nouveaux mods déclarent signalTypes.
    open val textureSet: String = ""
    open val checkboxes: List<Checkbox> = emptyList()
    open val signalTypes: List<SignalType> get() = listOf(SignalType(id, title, textureSet, checkboxes))
    /** Convertit les anciennes cases lors du chargement d'un profil sauvegardé.
     * Ne reçoit que les valeurs enregistrées, sans inventer de valeurs absentes.
     * Retourner les nouvelles clés ; le SDK complète leurs valeurs par défaut. */
    open fun migrateSettings(type: String, saved: Map<String, Boolean>): Map<String, Boolean> = saved
    open val maximumLineSpeed = false
    open val diagnosticFile: String = "nimby-kotlin-faults.jsonl"
    abstract val unknownDecision: Decision
    abstract val invalidNetworkDecision: Decision
    /** Le pont natif conserve alors le propriétaire de chaque indication. */
    open val modelLocalIndications: Boolean = false
    open fun unknownDecision(type: String): Decision = unknownDecision
    open fun invalidNetworkDecision(type: String): Decision = invalidNetworkDecision
    /** Lecture typée des décisions produites par signalModel ; null pour un code inconnu. */
    open fun indication(decision: Decision): SignalIndication? = null
    abstract fun evaluate(settings: Map<String, Boolean>, observation: Observation): Decision
    /** Calcul isolé d'un type. Le réseau utilise decide et Signal.type.
     * Les décisions brutes sont opaques ; indication permet leur lecture typée. */
    open fun evaluate(type: String, settings: Map<String, Boolean>, observation: Observation): Decision =
        evaluate(settings, observation)
    abstract fun decide(signal: Signal, next: Decision?): Decision?
    open fun fromLive(signal: Signal): Signal = signal
    abstract fun texture(decision: Decision, simulationMs: Long, halfPeriodMs: Long): String
    /** Description d'affichage fournie par appearance. Null conserve le callback
     * historique texture ; les modèles déclaratifs la fournissent automatiquement. */
    open fun animation(decision: Decision): SignalAnimation? = null
    /** Indication de recette autorisee par le mod. Null refuse le code.
     * Le motif determine aussi la conduite : arret absolu ou permissif.
     * Aucun aspect, motif ni vitesse ne sont interpretes par le SDK. */
    open fun forcedDecision(aspect: Int): Decision? = null
    open fun forcedDecision(type: String, aspect: Int): Decision? = forcedDecision(aspect)
    abstract fun drivingRule(decision: Decision): DrivingRule?
    abstract fun isFault(decision: Decision): Boolean
    open fun isActive(decision: Decision): Boolean = true
    open fun aspectName(aspect: Int): String = aspect.toString()
    abstract fun reasonName(reason: Int): String
    open fun plan(vehicle: Vehicle, settings: DrivingSettings, input: DrivingInput, constraints: List<Constraint>) = DrivingPlan()
}

/** Résolution des liens, commune aux mods et aux tests, sans récursion profonde. */
/** Validated preparation shared by the native adapter and the offline resolver. */
fun SignallingMod.prepareObservedNetwork(input: List<Signal>): List<Signal> {
    require(input.size <= 512)
    val types = signalTypes
    validateSignalTypes(types)
    val knownTypes = types.map { it.id }.toSet()
    val normalized = input.map { signal ->
        if (signal.type.isEmpty()) signal.copy(type = types.first().id)
        else signal.also { require(it.type in knownTypes) { "Type de signal inconnu : ${it.type}" } }
    }
    require(normalized.map { it.id }.toSet().size == normalized.size && normalized.none { it.id == 0L })
    val signals = prepareNetwork(normalized)
    require(signals.size == normalized.size && signals.indices.all { i ->
        signals[i].id == normalized[i].id && signals[i].nextSignal == normalized[i].nextSignal &&
            signals[i].type == normalized[i].type && signals[i].observation == normalized[i].observation &&
            (signals[i].settingsStatus == normalized[i].settingsStatus ||
                (normalized[i].settingsStatus == SettingsStatus.Absent && signals[i].settingsStatus == SettingsStatus.Present))
    }) { "Network preparation may only change effective settings" }
    return signals
}

fun SignallingMod.evaluateNetwork(input: List<Signal>): List<Decision> {
    val signals = prepareObservedNetwork(input)
    val index = signals.mapIndexed { i, signal -> signal.id to i }.toMap()
    require(index.size == signals.size && signals.none { it.id == 0L })
    val results = arrayOfNulls<Decision>(signals.size)
    val state = IntArray(signals.size)
    for (start in signals.indices) {
        if (state[start] == 2) continue
        val pending = mutableListOf<Int>()
        var current = start
        while (state[current] != 2) {
            if (state[current] == 1) {
                pending.forEach { results[it] = invalidNetworkDecision(signals[it].type); state[it] = 2 }
                break
            }
            val local = decide(signals[current], null)
            if (local != null) { results[current] = local; state[current] = 2; break }
            val next = index[signals[current].nextSignal]
            if (next == null) { results[current] = invalidNetworkDecision(signals[current].type); state[current] = 2; break }
            state[current] = 1; pending.add(current); current = next
        }
        for (i in pending.asReversed()) {
            if (state[i] == 2) continue
            results[i] = decide(signals[i], results[index.getValue(signals[i].nextSignal)]) ?: invalidNetworkDecision(signals[i].type)
            state[i] = 2
        }
    }
    return results.map { requireNotNull(it) }
}
