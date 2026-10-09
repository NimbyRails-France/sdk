package nimby

/** Indication typée d'un modèle : deux enums Kotlin, sans code natif à manipuler.
 * Les recettes utilisent leurs ordinaux locaux au modèle. Ajouter les nouvelles
 * valeurs à la fin si des recettes utilisent déjà ces codes. */
data class Indication<A : Enum<A>, R : Enum<R>>(val aspect: A, val reason: R)

@DslMarker
annotation class SignalModDsl

/** Contexte d'une règle. [next] est null lors du premier calcul ; retourner null
 * demande au SDK de résoudre le signal suivant puis de rappeler la règle.
 * Si le suivant est absent ou si le réseau boucle, le SDK utilise le repli
 * `invalidNetwork` choisi par le mod. Aucune règle nationale n'est implicite. */
class SignalContext<A : Enum<A>, R : Enum<R>> internal constructor(
    signal: Signal,
    val next: Indication<A, R>?,
    private val options: List<Checkbox>
) {
    val signal = signal.copy(observation = signal.observation.validatedApproach())
    val observation: Observation get() = signal.observation
    val block: Occupancy get() = observation.block
    val fresh: Boolean get() = observation.fresh
    val routeKnown: Boolean get() = observation.routeKnown
    /** Identifiant validé d'une approche fraîche ; null sinon. */
    val approachingTrain: Long? get() = observation.approachingTrain.takeIf { fresh }
    /** Présence d'une approche fraîche, sans décodage d'identifiant dans le mod. */
    val trainApproaching: Boolean get() = approachingTrain != null
    val settingsStatus: SettingsStatus get() = signal.settingsStatus

    /** Une valeur absente prend le défaut déclaré. Un profil indisponible reste
     * identifiable par [settingsStatus] : ce défaut ne prouve pas sa validité. */
    fun enabled(option: Checkbox): Boolean {
        require(options.any { it === option }) { "Cette case appartient à un autre type de signal" }
        return signal.settings[option.name] ?: option.defaultValue
    }
}

/** Configuration d'un modèle constructible. Les callbacks n'effectuent aucun
 * appel natif : ils se testent avec de simples valeurs Kotlin. */
@SignalModDsl
class SignalDefinition<A : Enum<A>, R : Enum<R>> internal constructor(private val base: SignalType) {
    private val options = base.checkboxes.toMutableList()
    private val actions = base.actions.toMutableList()
    var observeApproach: Boolean = base.observeApproach
    var approachBlocks: Int = base.approachBlocks
    /** Active la détection dans [blocks] cantons en amont, de 1 à 16. */
    fun observeApproach(blocks: Int) {
        require(blocks in 1..16)
        observeApproach = true; approachBlocks = blocks
    }
    internal var rule: (SignalContext<A, R>.() -> Indication<A, R>?)? = null
    internal var isolated: ((Map<String, Boolean>, Observation) -> Indication<A, R>)? = null
    internal var migration: (Map<String, Boolean>) -> Map<String, Boolean> = { it }
    internal var preparation: (Signal) -> Signal = { it }
    internal var forced: (A) -> Indication<A, R>? = { null }

    fun checkbox(name: String, label: String, description: String = "", defaultValue: Boolean = false,
                 onlyWhenEnabled: Boolean = false): Checkbox =
        Checkbox(name, label, description, defaultValue, onlyWhenEnabled).also { options.add(it) }

    fun rules(block: SignalContext<A, R>.() -> Indication<A, R>?) { rule = block }
    /** Affiché seulement si le fournisseur et son service sont disponibles.
     * Aucune dépendance obligatoire et aucun appel de mod depuis le thread UI. */
    fun action(id: String, label: String, whenMod: String, service: String) {
        actions.add(SignalAction(id, label, whenMod, service))
        validateSignalActions(actions)
    }
    /** Facultatif : calcul isolé sans résolution du réseau, pour les diagnostics. */
    fun evaluate(block: (Map<String, Boolean>, Observation) -> Indication<A, R>) { isolated = block }
    fun migrateSettings(block: (Map<String, Boolean>) -> Map<String, Boolean>) { migration = block }
    fun prepareObservation(block: (Signal) -> Signal) { preparation = block }
    /** Sans ce callback, toute demande de forçage d'indication est refusée. */
    fun allowForcedAspect(block: (A) -> Indication<A, R>?) { forced = block }

    internal fun freeze(): CompiledSignal<A, R> = CompiledSignal(
        base.copy(checkboxes = options.toList(), observeApproach = observeApproach, approachBlocks = approachBlocks, actions = actions.toList()),
        requireNotNull(rule) { "Déclarer rules pour ${base.id}" }, isolated, migration, preparation, forced)
}

internal data class CompiledSignal<A : Enum<A>, R : Enum<R>>(
    val type: SignalType,
    val rule: SignalContext<A, R>.() -> Indication<A, R>?,
    val isolated: ((Map<String, Boolean>, Observation) -> Indication<A, R>)?,
    val migration: (Map<String, Boolean>) -> Map<String, Boolean>,
    val preparation: (Signal) -> Signal,
    val forced: (A) -> Indication<A, R>?
)

/** Déclaration du mod. Les replis, images, vitesses et permissions viennent
 * exclusivement du mod. Le SDK prend en charge les exports et le réseau. */
@SignalModDsl
class SignalModBuilder<A : Enum<A>, R : Enum<R>> internal constructor() {
    internal val declaredOptions = mutableListOf<ModOption<*>>()
    /** Préférences globales du joueur ; les cases d'un signal restent séparées. */
    fun options(vararg values: ModOption<*>) {
        val combined = checkedModOptions(declaredOptions + values)
        declaredOptions.clear(); declaredOptions.addAll(combined)
    }
    var maximumLineSpeed: Boolean = false
    var diagnosticFile: String = "nimby-kotlin-faults.jsonl"
    internal val definitions = mutableListOf<CompiledSignal<A, R>>()
    internal var image: ((Indication<A, R>, Long, Long) -> String)? = null
    internal var animation: ((Indication<A, R>) -> SignalAnimation)? = null
    internal var driving: (Indication<A, R>) -> DrivingRule? = { null }
    internal var fault: (Indication<A, R>) -> Boolean = { false }
    internal var active: (Indication<A, R>) -> Boolean = { true }
    internal var aspectLabel: (A) -> String = { it.name }
    internal var reasonLabel: (R) -> String = { it.name }
    internal var planner: (Vehicle, DrivingSettings, DrivingInput, List<Constraint>) -> DrivingPlan = { _, _, _, _ -> DrivingPlan() }

    fun signal(id: String, title: String, textures: String, block: SignalDefinition<A, R>.() -> Unit) =
        signal(SignalType(id, title, textures), block)

    /** Permet de conserver une déclaration partagée avec des tests ou un éditeur. */
    fun signal(type: SignalType, block: SignalDefinition<A, R>.() -> Unit) {
        definitions.add(SignalDefinition<A, R>(type).apply(block).freeze())
    }
    fun images(block: (Indication<A, R>) -> String) { animation = null; image = { decision, _, _ -> block(decision) } }
    /** Temps simulé et demi-période en millisecondes, jamais l'heure du PC. */
    fun animatedImages(block: (Indication<A, R>, Long, Long) -> String) { animation = null; image = block }
    /** Affichage déclaratif avec steady ou blink, synchronisé sur le jeu. */
    fun appearance(block: (Indication<A, R>) -> SignalAnimation) {
        animation = block; image = { value, time, _ -> block(value).frameAt(time) }
    }
    fun driving(block: (Indication<A, R>) -> DrivingRule?) { driving = block }
    fun faults(block: (Indication<A, R>) -> Boolean) { fault = block }
    fun activeWhen(block: (Indication<A, R>) -> Boolean) { active = block }
    fun aspectNames(block: (A) -> String) { aspectLabel = block }
    fun reasonNames(block: (R) -> String) { reasonLabel = block }
    fun drivingPlan(block: (Vehicle, DrivingSettings, DrivingInput, List<Constraint>) -> DrivingPlan) { planner = block }
}

/** Ancienne déclaration à vocabulaire commun. Pour des modèles indépendants,
 * préférer [signalModel] et la surcharge signalMod(id, title) { signal(model) }.
 * [fallback] couvre les observations inconnues ;
 * [invalidNetwork] couvre les liens absents et les cycles sans décision locale. */
inline fun <reified A : Enum<A>, reified R : Enum<R>> signalMod(
    id: String, title: String, fallback: Indication<A, R>,
    invalidNetwork: Indication<A, R> = fallback,
    noinline block: SignalModBuilder<A, R>.() -> Unit
): SignallingMod = buildSignalMod(id, title, fallback, invalidNetwork, enumValues<A>().toList(), enumValues<R>().toList(), block)

@PublishedApi
internal fun <A : Enum<A>, R : Enum<R>> buildSignalMod(
    id: String, title: String, fallback: Indication<A, R>, invalidNetwork: Indication<A, R>,
    aspects: List<A>, reasons: List<R>, block: SignalModBuilder<A, R>.() -> Unit
): SignallingMod {
    val builder = SignalModBuilder<A, R>().apply(block)
    val definitions = builder.definitions.toList()
    validateSignalTypes(definitions.map { it.type })
    val byId = definitions.associateBy { it.type.id }
    val image = requireNotNull(builder.image) { "Déclarer images ou animatedImages" }
    val animation = builder.animation
    val driving = builder.driving; val fault = builder.fault; val active = builder.active
    val aspectLabel = builder.aspectLabel; val reasonLabel = builder.reasonLabel; val planner = builder.planner
    fun encode(value: Indication<A, R>) = Decision(value.aspect.ordinal, value.reason.ordinal)
    fun decode(value: Decision): Indication<A, R> = Indication(
        requireNotNull(aspects.getOrNull(value.aspect)) { "Indication inconnue : ${value.aspect}" },
        requireNotNull(reasons.getOrNull(value.reason)) { "Motif inconnu : ${value.reason}" })
    fun definition(type: String) = if (type.isEmpty()) definitions.first() else byId[type]
    return object : SignallingMod() {
        override val id = id
        override val title = title
        override val options = checkedModOptions(builder.declaredOptions)
        override val maximumLineSpeed = builder.maximumLineSpeed
        override val diagnosticFile = builder.diagnosticFile
        override val signalTypes = definitions.map { it.type }
        override val textureSet = signalTypes.first().textureSet
        override val checkboxes = signalTypes.first().checkboxes
        override val unknownDecision = encode(fallback)
        override val invalidNetworkDecision = encode(invalidNetwork)
        override fun evaluate(settings: Map<String, Boolean>, observation: Observation) = evaluate("", settings, observation)
        override fun evaluate(type: String, settings: Map<String, Boolean>, observation: Observation): Decision {
            val entry = definition(type) ?: return invalidNetworkDecision
            return encode(entry.isolated?.invoke(settings, observation)
                ?: entry.rule(SignalContext(Signal(settings = settings, observation = observation, type = entry.type.id), null, entry.type.checkboxes))
                ?: fallback)
        }
        override fun decide(signal: Signal, next: Decision?): Decision? {
            val entry = definition(signal.type) ?: return invalidNetworkDecision
            return entry.rule(SignalContext(signal, next?.let(::decode), entry.type.checkboxes))?.let(::encode)
        }
        override fun fromLive(signal: Signal) = definition(signal.type)?.preparation?.invoke(signal) ?: signal
        override fun migrateSettings(type: String, saved: Map<String, Boolean>) = definition(type)?.migration?.invoke(saved) ?: saved
        override fun forcedDecision(aspect: Int) = forcedDecision("", aspect)
        override fun forcedDecision(type: String, aspect: Int): Decision? {
            val entry = definition(type) ?: return null
            return aspects.getOrNull(aspect)?.let(entry.forced)?.let(::encode)
        }
        override fun texture(decision: Decision, simulationMs: Long, halfPeriodMs: Long) = image(decode(decision), simulationMs, halfPeriodMs)
        override fun animation(decision: Decision) = animation?.invoke(decode(decision))
        override fun drivingRule(decision: Decision) = driving(decode(decision))
        override fun isFault(decision: Decision) = fault(decode(decision))
        override fun isActive(decision: Decision) = active(decode(decision))
        override fun aspectName(aspect: Int) = aspectLabel(requireNotNull(aspects.getOrNull(aspect)))
        override fun reasonName(reason: Int) = reasonLabel(requireNotNull(reasons.getOrNull(reason)))
        override fun plan(vehicle: Vehicle, settings: DrivingSettings, input: DrivingInput, constraints: List<Constraint>) = planner(vehicle, settings, input, constraints)
    }
}
