package nimby

/** Bouton optionnel sur le panneau d'un signal. [whenMod] est l'identifiant
 * du fournisseur, pas une dépendance d'installation. Sans ce mod et son service,
 * le bouton est absent et les règles de signalisation restent inchangées. */
data class SignalAction(val id: String, val label: String, val whenMod: String, val service: String)

/** Clic confirmé par le SDK : signal complet et génération de partie observée.
 * Un clic n'est ni une permission de construire ni une réservation d'itinéraire. */
data class SignalActionRequest(
    val sequence: Long, val signalId: Long, val action: String, val service: String,
    val worldId: String, val generation: Long,
    val panelToken: Long = 0, val originAction: String = action,
    /** Nouvelle valeur d'un champ numérique ; null pour un clic de bouton. */
    val value: Int? = null,
)

/** Base commune : un outil ne doit pas inventer un modèle de signalisation. */
abstract class GameMod {
    abstract val id: String
    abstract val title: String
    open val services: List<String> = emptyList()
    open fun onSignalAction(request: SignalActionRequest, context: ToolContext) {}
    open fun onTick(context: ToolContext) {}
    open fun onStop() {}
}

/** Un mod outil reçoit des actions sans définir d'indication ni de vitesse. */
class ToolMod internal constructor(
    override val id: String, override val title: String,
    private val handlers: Map<String, ToolContext.(SignalActionRequest) -> Unit>,
    private val tick: ToolContext.() -> Unit,
    private val stop: () -> Unit,
) : GameMod() {
    override val services: List<String> = handlers.keys.toList()
    override fun onSignalAction(request: SignalActionRequest, context: ToolContext) {
        handlers[request.service]?.invoke(context, request)
    }
    override fun onTick(context: ToolContext) = tick(context)
    override fun onStop() = stop()
}

class ToolModBuilder internal constructor() {
    internal val handlers = linkedMapOf<String, ToolContext.(SignalActionRequest) -> Unit>()
    internal var tick: ToolContext.() -> Unit = {}
    internal var stop: () -> Unit = {}
    /** Callback sérialisé sur le worker du mod, jamais sur le thread de l'UI. */
    fun service(id: String, handler: ToolContext.(SignalActionRequest) -> Unit) {
        validateServiceName(id)
        require(id !in handlers) { "Service déjà déclaré : $id" }
        require(handlers.size < 32) { "Au maximum 32 services par mod" }
        handlers[id] = handler
    }
    fun onTick(block: ToolContext.() -> Unit) { tick=block }
    fun onStop(block: () -> Unit) { stop=block }
}

fun toolMod(id: String, title: String, block: ToolModBuilder.() -> Unit): ToolMod {
    validateServiceName(id)
    require(title.isNotBlank() && title.length <= 256 && '\u0000' !in title)
    val builder = ToolModBuilder().apply(block)
    require(builder.handlers.isNotEmpty()) { "Déclarer au moins un service" }
    return ToolMod(id, title, builder.handlers.toMap(), builder.tick, builder.stop)
}

internal fun validateServiceName(value: String) {
    require(value.matches(Regex("[a-zA-Z0-9_.-]{1,128}"))) { "Identifiant de service/mod invalide : $value" }
}

internal fun validateSignalActions(actions: List<SignalAction>) {
    require(actions.size <= 16 && actions.map { it.id }.distinct().size == actions.size)
    actions.forEach {
        validateServiceName(it.id); validateServiceName(it.whenMod); validateServiceName(it.service)
        require(it.label.isNotBlank() && it.label.encodeToByteArray().size <= 256 && '\u0000' !in it.label)
    }
}
