package nimby

/** Décision d'un voisin, accompagnée du modèle qui l'a produite.
 * Utiliser [of] pour la lire avec les enums de ce modèle. Un code numérique
 * identique chez deux modèles ne rend jamais leurs indications interchangeables. */
class SignalIndication internal constructor(
    private val model: SignalModel<*, *>,
    val aspect: Enum<*>,
    val reason: Enum<*>
) {
    val type: SignalType get() = model.type
    /** Consigne explicitement déclarée par le modèle, sans déduction de couleur. */
    val drivingRule: DrivingRule? get() = model.driving(Decision(aspect.ordinal, reason.ordinal))
    val active: Boolean get() = model.active(Decision(aspect.ordinal, reason.ordinal))

    /** Null si le voisin n'est pas exactement ce modèle. L'identité de la
     * déclaration est vérifiée avant la conversion, pas seulement son nom. */
    fun <A : Enum<A>, R : Enum<R>> of(model: SignalModel<A, R>): Indication<A, R>? {
        if (this.model !== model) return null
        @Suppress("UNCHECKED_CAST")
        return Indication(aspect as A, reason as R)
    }
}

/** Voisin aval résolu sur le lien fourni par le réseau du mod.
 * Aucun parcours géométrique supplémentaire : ce n'est pas une recherche de
 * tous les signaux proches. Une donnée d'un mod absent n'est pas inventée. */
class SignalNeighbour internal constructor(val id: Long, val indication: SignalIndication) {
    val type: SignalType get() = indication.type
    val drivingRule: DrivingRule? get() = indication.drivingRule
    val active: Boolean get() = indication.active
    fun <A : Enum<A>, R : Enum<R>> of(model: SignalModel<A, R>): Indication<A, R>? = indication.of(model)
}

/** Entrée d'une règle de modèle. [next] conserve le type du voisin.
 * Retourner null demande sa résolution ; un lien absent ou cyclique utilise
 * le repli du modèle courant. Les interprétations entre modèles restent dans le mod. */
class SignalRuleContext internal constructor(
    source: Signal, indication: SignalIndication?, private val options: List<Checkbox>, defaults: Map<String, Boolean>
) {
    val next: SignalNeighbour? = indication?.let { SignalNeighbour(source.nextSignal, it) }
    /** Normalisation commune : un profil absent utilise les valeurs déclarées,
     * un profil indisponible rend l'observation non fraîche. Aucun état de feu
     * ni consigne de conduite n'est choisi ici. Le statut reste consultable. */
    val signal: Signal = run {
        val settings = if(source.settingsStatus == SettingsStatus.Absent) defaults else source.settings
        val observation = if(source.settingsStatus == SettingsStatus.Unavailable && source.observation.fresh)
            source.observation.copy(fresh = false).validatedApproach() else source.observation.validatedApproach()
        if(settings === source.settings && observation === source.observation) source
        else source.copy(settings = settings, observation = observation)
    }
    val settings: Map<String, Boolean> get() = signal.settings
    val observation: Observation get() = signal.observation
    val block: Occupancy get() = observation.block
    val fresh: Boolean get() = observation.fresh
    val routeKnown: Boolean get() = observation.routeKnown
    /** Identifiant validé du train en approche, ou null si absent ou non frais.
     * Aucun décodage de l'identifiant n'est nécessaire dans le mod. */
    val approachingTrain: Long? get() = observation.approachingTrain.takeIf { fresh }
    /** Vrai si une tête de train est observée en approche avec des données fraîches.
     * Ne prouve ni voie libre ni autorisation de mouvement. */
    val trainApproaching: Boolean get() = approachingTrain != null
    val settingsStatus: SettingsStatus get() = signal.settingsStatus
    fun enabled(option: Checkbox): Boolean {
        require(options.any { it === option }) { "Cette case appartient à un autre modèle" }
        return signal.settings[option.name] ?: option.defaultValue
    }
}

/** Tous les rôles d'un modèle, avec ses propres types d'indication et de motif.
 * Les fonctions peuvent être déclarées dans des fichiers séparés du mod. */
@SignalModDsl
class SignalModelBuilder<A : Enum<A>, R : Enum<R>> internal constructor(private val base: SignalType) {
    private val options = base.checkboxes.toMutableList()
    private val numbers = base.numbers.toMutableList()
    fun number(option: NumberSetting) { numbers.add(option) }
    private val actions = base.actions.toMutableList()
    private var construction = base.construction
    /** Déclare les images dans leur ordre de catalogue et le signal constructible.
     * name apparaît dans le menu de construction ; par défaut, le titre du modèle.
     * Les chemins sont relatifs au paquet : assets/a.svg devient a.svg.
     * Le SDK génère mod.txt au build, sans exécuter les règles ni ouvrir le jeu. */
    fun construction(states: List<String>, name: String = base.title, kind: String = "path",
                     catalogueName: String = name, nameKey: String? = null, catalogueNameKey: String? = null,
                     size: Int = 0, left: Boolean = false) {
        construction = SignalConstruction(states, name, kind, catalogueName, nameKey, catalogueNameKey, size, left)
    }
    var observeApproach = base.observeApproach
    var approachBlocks = base.approachBlocks
    /** Active la détection dans [blocks] cantons en amont (de 1 à 16).
     * Le mod consulte ensuite trainApproaching dans rules et décide de l'ouverture. */
    fun observeApproach(blocks: Int) {
        require(blocks in 1..16) { "La portée d'approche doit être comprise entre 1 et 16 cantons" }
        observeApproach = true; approachBlocks = blocks
    }
    internal var rule: (SignalRuleContext.() -> Indication<A, R>?)? = null
    internal var isolated: ((Map<String, Boolean>, Observation) -> Indication<A, R>)? = null
    internal var typedIsolated: ((Map<String, Boolean>, Observation, A) -> Indication<A, R>)? = null
    internal var migration: (Map<String, Boolean>) -> Map<String, Boolean> = { it }
    internal var preparation: (Signal) -> Signal = { it }
    internal var forced: (A) -> Indication<A, R>? = { null }
    internal var image: ((Indication<A, R>, Long, Long) -> String)? = null
    internal var animation: ((Indication<A, R>) -> SignalAnimation)? = null
    internal var driving: (Indication<A, R>) -> DrivingRule? = { null }
    internal var fault: (Indication<A, R>) -> Boolean = { false }
    internal var active: (Indication<A, R>) -> Boolean = { true }
    internal var aspectLabel: (A) -> String = { it.name }
    internal var reasonLabel: (R) -> String = { it.name }

    fun checkbox(name: String, label: String, description: String = "", defaultValue: Boolean = false,
                 onlyWhenEnabled: Boolean = false): Checkbox =
        Checkbox(name, label, description, defaultValue, onlyWhenEnabled).also { options.add(it) }
    fun action(id: String, label: String, whenMod: String, service: String) {
        actions.add(SignalAction(id, label, whenMod, service)); validateSignalActions(actions)
    }
    fun rules(block: SignalRuleContext.() -> Indication<A, R>?) { rule = block }
    fun evaluate(block: (Map<String, Boolean>, Observation) -> Indication<A, R>) { isolated = block }
    /** Facultatif : compatibilité avec les diagnostics isolés qui fournissent
     * Observation.next comme ordinal de CE modèle. Le SDK le valide et le décode.
     * En réseau, utiliser rules et son voisin portant un modèle explicite. */
    fun evaluate(block: (Map<String, Boolean>, Observation, A) -> Indication<A, R>) { typedIsolated = block }
    fun migrateSettings(block: (Map<String, Boolean>) -> Map<String, Boolean>) { migration = block }
    fun prepareObservation(block: (Signal) -> Signal) { preparation = block }
    /** Recettes : le code reçu est l'ordinal local à ce modèle. */
    fun allowForcedAspect(block: (A) -> Indication<A, R>?) { forced = block }
    fun images(block: (Indication<A, R>) -> String) { animation = null; image = { value, _, _ -> block(value) } }
    /** Temps de simulation, en millisecondes. */
    fun animatedImages(block: (Indication<A, R>, Long, Long) -> String) { animation = null; image = block }
    /** Décrit l'affichage avec steady ou blink. Les durées sont transmises au
     * moteur de rendu ; le mod ne calcule pas la phase du clignotement. */
    fun appearance(block: (Indication<A, R>) -> SignalAnimation) {
        animation = block; image = { value, time, _ -> block(value).frameAt(time) }
    }
    fun driving(block: (Indication<A, R>) -> DrivingRule?) { driving = block }
    fun faults(block: (Indication<A, R>) -> Boolean) { fault = block }
    fun activeWhen(block: (Indication<A, R>) -> Boolean) { active = block }
    fun aspectNames(block: (A) -> String) { aspectLabel = block }
    fun reasonNames(block: (R) -> String) { reasonLabel = block }
    internal fun type(): SignalType {
        require(numbers.map { it.name }.distinct().size == numbers.size && numbers.size <= 4)
        require(numbers.all { n -> n.visibleWhen.isEmpty() || options.any { it.name == n.visibleWhen } })
        return base.copy(checkboxes = options.toList() + numbers.flatMap { it.storage() }, numbers = numbers.toList(),
            actions = actions.toList(), observeApproach = observeApproach, approachBlocks = approachBlocks, construction = construction)
    }
}

/** Modèle réutilisable et typé. Sa déclaration ne charge ni DLL ni partie.
 * Ajouter ce modèle à [signalMod] pour le rendre constructible. */
class SignalModel<A : Enum<A>, R : Enum<R>> @PublishedApi internal constructor(
    builder: SignalModelBuilder<A, R>, private val fallback: Indication<A, R>,
    private val invalidNetwork: Indication<A, R>, private val aspects: List<A>, private val reasons: List<R>
) {
    val type: SignalType = builder.type()
    private val defaultSettings = nimby.internal.SignalSettingsMask(type.checkboxes).defaults
    private val rule = requireNotNull(builder.rule) { "Déclarer rules pour ${type.id}" }
    private val image = requireNotNull(builder.image) { "Déclarer images pour ${type.id}" }
    private val animation = builder.animation
    private val isolated = builder.isolated
    private val typedIsolated = builder.typedIsolated
    private val migrate = builder.migration
    private val prepare = builder.preparation
    private val force = builder.forced
    private val driving = builder.driving
    private val fault = builder.fault
    private val active = builder.active
    private val aspectLabel = builder.aspectLabel
    private val reasonLabel = builder.reasonLabel

    init { require(aspects.size <= 65536 && reasons.size <= 65536) }
    // Codes privés du pont natif : le tag garde le propriétaire même dans les
    // callbacks de rendu/conduite qui ne reçoivent pas le signal d'origine.
    internal fun encode(value: Indication<A, R>, tag: Int) =
        Decision(tag or value.aspect.ordinal, tag or value.reason.ordinal)
    private fun decode(raw: Decision) = Indication(aspects[raw.aspect and 65535], reasons[raw.reason and 65535])
    internal fun read(raw: Decision): SignalIndication? {
        val aspect = aspects.getOrNull(raw.aspect and 65535) ?: return null
        val reason = reasons.getOrNull(raw.reason and 65535) ?: return null
        return SignalIndication(this, aspect, reason)
    }
    internal fun valid(raw: Decision) = (raw.aspect and 65535) < aspects.size && (raw.reason and 65535) < reasons.size
    internal fun fallback(tag: Int, invalid: Boolean) = encode(if (invalid) invalidNetwork else fallback, tag)
    internal fun evaluate(settings: Map<String, Boolean>, observation: Observation, tag: Int): Decision = encode(
        typedIsolated?.invoke(settings, observation,
            requireNotNull(aspects.getOrNull(observation.next)) { "Indication de diagnostic inconnue : ${observation.next}" })
            ?: isolated?.invoke(settings, observation) ?: rule(SignalRuleContext(
                Signal(settings = settings, observation = observation, type = type.id), null, type.checkboxes, defaultSettings)) ?: fallback, tag)
    internal fun decide(signal: Signal, next: SignalIndication?, tag: Int) =
        rule(SignalRuleContext(signal, next, type.checkboxes, defaultSettings))?.let { encode(it, tag) }
    internal fun forced(aspect: Int, tag: Int) = aspects.getOrNull(aspect)?.let(force)?.let { encode(it, tag) }
    internal fun migrate(saved: Map<String, Boolean>) = migrate.invoke(saved)
    internal fun prepare(signal: Signal) = prepare.invoke(signal)
    internal fun texture(raw: Decision, simulationMs: Long, halfPeriodMs: Long) = image(decode(raw), simulationMs, halfPeriodMs)
    internal fun animation(raw: Decision) = animation?.invoke(decode(raw))
    internal fun driving(raw: Decision) = driving.invoke(decode(raw))
    internal fun fault(raw: Decision) = fault.invoke(decode(raw))
    internal fun active(raw: Decision) = active.invoke(decode(raw))
    internal fun aspectName(code: Int) = aspectLabel(aspects[code and 65535])
    internal fun reasonName(code: Int) = reasonLabel(reasons[code and 65535])
}

inline fun <reified A : Enum<A>, reified R : Enum<R>> signalModel(
    id: String, title: String, textures: String, fallback: Indication<A, R>,
    invalidNetwork: Indication<A, R> = fallback, noinline block: SignalModelBuilder<A, R>.() -> Unit
): SignalModel<A, R> = signalModel(SignalType(id, title, textures), fallback, invalidNetwork, block)

inline fun <reified A : Enum<A>, reified R : Enum<R>> signalModel(
    type: SignalType, fallback: Indication<A, R>, invalidNetwork: Indication<A, R> = fallback,
    noinline block: SignalModelBuilder<A, R>.() -> Unit
): SignalModel<A, R> = buildSignalModel(type, fallback, invalidNetwork, enumValues<A>().toList(), enumValues<R>().toList(), block)

@PublishedApi internal fun <A : Enum<A>, R : Enum<R>> buildSignalModel(
    type: SignalType, fallback: Indication<A, R>, invalidNetwork: Indication<A, R>,
    aspects: List<A>, reasons: List<R>, block: SignalModelBuilder<A, R>.() -> Unit
) = SignalModel(SignalModelBuilder<A, R>(type).apply(block), fallback, invalidNetwork, aspects, reasons)

/** Composition du paquet : chaque modèle conserve ses enums et ses callbacks. */
@SignalModDsl
class SignalModelsBuilder internal constructor() {
    internal val declaredOptions = mutableListOf<ModOption<*>>()
    /** Préférences globales du joueur ; les cases d'un signal restent séparées. */
    fun options(vararg values: ModOption<*>) {
        val combined = checkedModOptions(declaredOptions + values)
        declaredOptions.clear(); declaredOptions.addAll(combined)
    }
    internal var preparation: (List<Signal>) -> List<Signal> = { it }
    fun prepareNetwork(block: (List<Signal>) -> List<Signal>) { preparation = block }
    internal var metadata: ModMetadata? = null
    /** Auteur et description affichés dans la liste des mods du jeu. */
    fun metadata(author: String, description: String, name: String? = null) { metadata = ModMetadata(author, description, name) }
    var maximumLineSpeed = false
    var diagnosticFile = "nimby-kotlin-faults.jsonl"
    internal val models = mutableListOf<SignalModel<*, *>>()
    internal var planner: (Vehicle, DrivingSettings, DrivingInput, List<Constraint>) -> DrivingPlan = { _, _, _, _ -> DrivingPlan() }
    fun signal(model: SignalModel<*, *>) { models.add(model) }
    fun drivingPlan(block: (Vehicle, DrivingSettings, DrivingInput, List<Constraint>) -> DrivingPlan) { planner = block }
}

/** Assemble des modèles indépendants. Le SDK transporte leur identité, mais
 * n'invente aucune correspondance entre leurs états et aucune règle de conduite. */
fun signalMod(id: String, title: String, block: SignalModelsBuilder.() -> Unit): SignallingMod {
    val builder = SignalModelsBuilder().apply(block)
    val models = builder.models.toList()
    validateSignalTypes(models.map { it.type })
    val byId = models.withIndex().associate { it.value.type.id to it.index }
    fun index(type: String) = if (type.isEmpty()) 0 else requireNotNull(byId[type]) { "Modèle inconnu : $type" }
    fun tag(index: Int) = (index + 1) shl 16
    fun owner(code: Int): SignalModel<*, *>? = models.getOrNull((code ushr 16) - 1)
    fun checked(raw: Decision): SignalModel<*, *> {
        require(raw.aspect ushr 16 == raw.reason ushr 16) { "Indication et motif de modèles différents" }
        return requireNotNull(owner(raw.aspect)?.takeIf { it.valid(raw) }) { "Décision inconnue" }
    }
    val planner = builder.planner
    return object : SignallingMod() {
        override fun prepareNetwork(signals: List<Signal>) = builder.preparation(signals)
        override val id = id
        override val title = title
        override val options = checkedModOptions(builder.declaredOptions)
        override val metadata = builder.metadata
        override val modelLocalIndications = true
        override val maximumLineSpeed = builder.maximumLineSpeed
        override val diagnosticFile = builder.diagnosticFile
        override val signalTypes = models.map { it.type }
        override val textureSet = signalTypes.first().textureSet
        override val checkboxes = signalTypes.first().checkboxes
        override val unknownDecision = unknownDecision("")
        override val invalidNetworkDecision = invalidNetworkDecision("")
        override fun unknownDecision(type: String) = index(type).let { models[it].fallback(tag(it), false) }
        override fun invalidNetworkDecision(type: String) = index(type).let { models[it].fallback(tag(it), true) }
        override fun indication(decision: Decision): SignalIndication? {
            if (decision.aspect ushr 16 != decision.reason ushr 16) return null
            return owner(decision.aspect)?.read(decision)
        }
        override fun evaluate(settings: Map<String, Boolean>, observation: Observation) = evaluate("", settings, observation)
        override fun evaluate(type: String, settings: Map<String, Boolean>, observation: Observation) =
            index(type).let { models[it].evaluate(settings, observation, tag(it)) }
        override fun decide(signal: Signal, next: Decision?): Decision? = index(signal.type).let {
            models[it].decide(signal, next?.let { raw -> requireNotNull(indication(raw)) { "Décision voisine inconnue" } }, tag(it))
        }
        override fun fromLive(signal: Signal) = models[index(signal.type)].prepare(signal)
        override fun migrateSettings(type: String, saved: Map<String, Boolean>) = models[index(type)].migrate(saved)
        override fun forcedDecision(aspect: Int) = forcedDecision("", aspect)
        override fun forcedDecision(type: String, aspect: Int) = index(type).let { models[it].forced(aspect, tag(it)) }
        override fun texture(decision: Decision, simulationMs: Long, halfPeriodMs: Long) = checked(decision).texture(decision, simulationMs, halfPeriodMs)
        override fun animation(decision: Decision) = checked(decision).animation(decision)
        override fun drivingRule(decision: Decision) = checked(decision).driving(decision)
        override fun isFault(decision: Decision) = checked(decision).fault(decision)
        override fun isActive(decision: Decision) = checked(decision).active(decision)
        override fun aspectName(aspect: Int) = requireNotNull(owner(aspect)).aspectName(aspect)
        override fun reasonName(reason: Int) = requireNotNull(owner(reason)).reasonName(reason)
        override fun plan(vehicle: Vehicle, settings: DrivingSettings, input: DrivingInput, constraints: List<Constraint>) = planner(vehicle, settings, input, constraints)
    }
}
