package nimby

/** Géométrie observée pour un outil. Une longueur absente interrompt le parcours. */
data class ToolTrack(val id: Long, val linkA: Long?, val linkB: Long?, val lengthM: Double?)
data class ToolJunction(val branchTrack: Long, val mainTrack: Long, val fraction: Double, val mainDirection: Int, val branchDirection: Int)
data class ToolSignal(val id: Long, val track: Long, val fraction: Double, val direction: Int, val kind: Int)
data class ToolNetwork(val worldId: String, val generation: Long, val tracks: List<ToolTrack>, val junctions: List<ToolJunction>, val signals: List<ToolSignal>)
data class SignalPosition(val trackId: Long, val fraction: Double, val direction: Int)
enum class ConstructionState { Ready, Applied, Undone, Rejected, Partial, Pending }
data class ConstructionResult(val state: ConstructionState, val token: Long, val createdIds: List<Long>, val reason: Int, val canUndo: Boolean)

/** Contrôles temporaires de l'outil dans le panneau du signal sélectionné.
 * Leur retrait ne modifie pas les cases persistantes du mod de signalisation. */
data class ToolButton(val id: String, val label: String, val enabled: Boolean = true)
/** Champ saisissable au clavier. Le mod choisit la valeur et les bornes ;
 * une modification valide arrive dans action=id et [SignalActionRequest.value].
 * Le texte vide ou incomplet reste un brouillon local : aucune valeur de
 * remplacement n'est envoyée au mod et les boutons de commande sont bloqués.
 * Dans une ToolWindow, le clic envoie tous les champs dans ToolWindowEvent.values,
 * sans événement intermédiaire par touche. */
data class ToolNumberInput(val id: String, val label: String, val value: Int,
    val minimum: Int, val maximum: Int, val enabled: Boolean = true)
enum class LogLevel { Info, Warning, Error }

/** Accès valable uniquement pendant un callback de l'outil, sur son worker.
 * Garder les données copiées est permis ; conserver le contexte ne l'est pas.
 * La construction est expérimentale et exige le pont Windows correspondant. */
class ToolContext internal constructor(
    val worldId: String, val generation: Long,
    private val nativeCall: (Int, LongArray, DoubleArray, ByteArray) -> Int,
) {
    private var open = true
    internal fun close() { open = false }
    private fun call(op: Int, integers: LongArray, numbers: DoubleArray = doubleArrayOf(), text: ByteArray = byteArrayOf()) {
        check(open) { "Le contexte d'outil n'est plus actif" }
        val status = nativeCall(op, integers, numbers, text)
        // A signal can be deleted or change catalogue while its tool is open.
        // A rejected presentation grants no action and is safe to retry on the
        // next tick. Do not turn it into observation loss and log a stack four
        // times per second. Malformed payloads and other errors still fail.
        if ((op == 6 || op == 8) && status == 9) return
        if (status != 0) {
            if (op != 7) runCatching {
                val diagnostic="ToolContext operation=$op status=$status world=$worldId generation=$generation"
                nativeCall(7,longArrayOf(LogLevel.Error.ordinal.toLong()),doubleArrayOf(),diagnostic.encodeToByteArray()+byteArrayOf(0))
            }
            val message=if(op==5) when(status) {
                1 -> "Demande de pose invalide. Consulter le journal du mod."
                3,5 -> "Le composant de pose est incompatible avec le jeu. Relancer avec le SDK correspondant."
                8 -> "Service de pose indisponible. Vérifier le kit SDK et garder l'éditeur des voies ouvert."
                11 -> "Le jeu a été fermé. Relancer la partie."
                12 -> "Une autre opération de pose est en cours."
                else -> "La pose n'a pas pu être confirmée. Consulter le journal avant de réessayer."
            } else if(op==9) "Aperçu sur la carte indisponible (statut SDK $status). Vérifier le SDK chargé et la partie."
            else "Opération d'outil $op refusée (statut SDK $status)"
            error(message)
        }
    }
    /** Journal persistant commun aux mods ; ne pas y écrire de secrets. */
    fun log(message: String, level: LogLevel = LogLevel.Info) {
        val bytes=message.encodeToByteArray()
        require(bytes.size in 1..4096 && '\u0000' !in message)
        call(7,longArrayOf(level.ordinal.toLong()),text=bytes+byteArrayOf(0))
    }

    /** Lit seulement l'horloge et l'identité de la partie, sans capturer le réseau. */
    fun clock(): ToolClock {
        val values = LongArray(2); call(10, values)
        return ToolClock(values[0], values[1])
    }
    /** Change la date UTC sur le thread de simulation. Par défaut, les positions
     * et les délais relatifs sont conservés. recalculateTrains déclenche les
     * interventions natives (déplacements et coûts) : obtenir un choix explicite.
     * Les erreurs peuvent survenir après le début de l'opération : relire, jamais
     * réessayer automatiquement. Les secondes sous la seconde sont conservées. */
    fun changeTime(utcSeconds: Long, recalculateTrains: Boolean = false): ToolTimeChange {
        require(utcSeconds in -62135596800L..253402300799L)
        val values = longArrayOf(utcSeconds, if (recalculateTrains) 1 else 0, 0)
        call(11, values)
        return ToolTimeChange(ToolClock(values[0], values[1]), values[2])
    }
    /** Affiche le formulaire de la fenêtre à l'origine de request. message est
     * affiché au-dessus, puis les libellés/champs et les boutons. Jusqu'à huit
     * champs et douze boutons ; valeurs invalides refusées avant tout événement.
     * Les valeurs sont envoyées ensemble au clic, pas à chaque touche saisie. */
    fun showWindow(request: ToolWindowEvent, message: String, buttons: List<ToolButton>, inputs: List<ToolNumberInput> = emptyList()) {
        require(request.worldId == worldId && request.generation == generation && request.sequence > 0)
        require(buttons.size <= 12 && inputs.size <= 8)
        require((buttons.map { it.id } + inputs.map { it.id }).let { it.size == it.distinct().size })
        val strings = listOf(request.window, message) + buttons.flatMap {
            validateServiceName(it.id); listOf(it.id, it.label)
        } + inputs.flatMap {
            validateServiceName(it.id); require(it.minimum <= it.maximum && it.value in it.minimum..it.maximum)
            listOf(it.id, it.label)
        }
        strings.forEach { require('\u0000' !in it && it.encodeToByteArray().size <= 4096) }
        val values = longArrayOf(request.sequence, buttons.size.toLong(), inputs.size.toLong()) +
            buttons.map { if(it.enabled) 1L else 0L } +
            inputs.flatMap { listOf(it.value.toLong(), it.minimum.toLong(), it.maximum.toLong(), if(it.enabled) 1L else 0L) }
        val text = (strings.joinToString("\u0000") + "\u0000").encodeToByteArray()
        require(text.size <= 8192) { "Formulaire trop volumineux (8192 octets maximum)" }
        call(12, values, text = text)
    }

    /** Nouvelle capture après prepareConstruction, jamais une topologie cachée. */
    fun network(): ToolNetwork {
        val counts=LongArray(3);call(1,counts)
        require(counts.all { it in 0..1_000_000 })
        val trackIds=LongArray(counts[0].toInt()*3);val lengths=DoubleArray(counts[0].toInt())
        call(2,trackIds,lengths)
        val junctionIds=LongArray(counts[1].toInt()*4);val fractions=DoubleArray(counts[1].toInt())
        call(3,junctionIds,fractions)
        val signalIds=LongArray(counts[2].toInt()*4);val positions=DoubleArray(counts[2].toInt())
        call(4,signalIds,positions)
        return ToolNetwork(worldId,generation,
            lengths.indices.map { i -> ToolTrack(trackIds[i*3],trackIds[i*3+1].takeIf { it!=0L },trackIds[i*3+2].takeIf { it!=0L },lengths[i].takeIf { it.isFinite()&&it>0 }) },
            fractions.indices.map { i -> ToolJunction(junctionIds[i*4],junctionIds[i*4+1],fractions[i],junctionIds[i*4+2].toInt(),junctionIds[i*4+3].toInt()) },
            positions.indices.map { i -> ToolSignal(signalIds[i*4],signalIds[i*4+1],positions[i],signalIds[i*4+2].toInt(),signalIds[i*4+3].toInt()) })
    }

    fun prepareConstruction(sourceSignal: Long): ConstructionResult {
        require(sourceSignal ushr 48 == 8L)
        return construction(1,0,sourceSignal)
    }
    /** Affiche les emplacements sur la carte avec le modèle du signal source.
     * Aucun signal n'est construit. Jusqu'à 64 positions, choisies par le mod.
     * Renouveler dans onTick tant que l'aperçu est utile : il expire après deux
     * secondes sans publication et disparaît à l'arrêt du mod ou de la partie.
     * Il est visible seulement pendant l'édition du signal source. La saisie
     * d'une nouvelle valeur masque immédiatement l'ancien aperçu.
     * Un seul aperçu est actif : la dernière publication remplace la précédente.
     * Nécessite le rendu Windows correspondant ; son absence lève une exception.
     * Le retour confirme la publication, pas la visibilité de chaque position :
     * le cadrage et les couches visibles du jeu restent appliqués. */
    fun showSignalPreview(request: SignalActionRequest, positions: List<SignalPosition>) {
        require(request.worldId==worldId&&request.generation==generation)
        require(request.signalId ushr 48==8L&&positions.size<=64)
        if(positions.isEmpty()){clearSignalPreview();return}
        positions.forEach { require(it.trackId ushr 48==1L&&it.fraction.isFinite()&&it.fraction>0&&it.fraction<1&&it.direction in listOf(-1,1)) }
        validateServiceName(request.service);validateServiceName(request.originAction)
        val integers=longArrayOf(request.panelToken,request.signalId,positions.size.toLong())+
            positions.flatMap { listOf(it.trackId,it.direction.toLong()) }
        call(9,integers,positions.map { it.fraction }.toDoubleArray(),
            (request.service+"\u0000"+request.originAction+"\u0000").encodeToByteArray())
    }
    /** Retire uniquement l'aperçu de ce mod. Ne touche ni aux signaux construits,
     * ni à l'historique d'annulation, ni aux aperçus d'un autre mod. */
    fun clearSignalPreview() { call(9,longArrayOf(0,0,0)) }
    /** En cas de Pending, appeler pollConstruction avec le même ticket. Ne pas
     * refaire createSignals : une réponse incertaine peut masquer une pose réelle. */
    fun createSignals(ticket: Long, sourceSignal: Long, positions: List<SignalPosition>): ConstructionResult {
        require(ticket!=0L && sourceSignal ushr 48==8L && positions.size in 1..64)
        require(positions.map { it.trackId to it.fraction }.distinct().size==positions.size)
        positions.forEach { require(it.trackId ushr 48==1L&&it.fraction.isFinite()&&it.fraction>0&&it.fraction<1&&it.direction in listOf(-1,1)) }
        return construction(2,ticket,sourceSignal,positions)
    }
    fun undoConstruction(ticket: Long): ConstructionResult { require(ticket!=0L);return construction(3,ticket,0) }
    fun pollConstruction(ticket: Long): ConstructionResult { require(ticket!=0L);return construction(4,ticket,0) }
    private fun construction(action: Int,ticket: Long,source: Long,positions: List<SignalPosition> = emptyList()): ConstructionResult {
        val ints=LongArray(maxOf(69,4+positions.size*2));val numbers=DoubleArray(positions.size)
        ints[0]=action.toLong();ints[1]=ticket;ints[2]=source;ints[3]=positions.size.toLong()
        positions.forEachIndexed { i,p -> ints[4+i*2]=p.trackId;ints[5+i*2]=p.direction.toLong();numbers[i]=p.fraction }
        call(5,ints,numbers)
        require(ints[0] in 1..6 && ints[2] in 0..64 && ints[4] in 0..1)
        val ids=List(ints[2].toInt()) { ints[5+it].also { id -> require(id ushr 48==8L) } }
        return ConstructionResult(ConstructionState.entries[ints[0].toInt()-1],ints[1],ids,ints[3].toInt(),ints[4]!=0L)
    }

    /** Remplace seulement l'action à l'origine de [request] par ces contrôles.
     * Les clics suivants arrivent au même service avec l'id du bouton dans action. */
    fun showPanel(request: SignalActionRequest, message: String, buttons: List<ToolButton>, inputs: List<ToolNumberInput> = emptyList()) {
        require(request.worldId==worldId&&request.generation==generation)
        require(buttons.size<=12&&buttons.map { it.id }.distinct().size==buttons.size)
        require(inputs.size<=4)
        require((buttons.map { it.id }+inputs.map { it.id }).let { it.distinct().size==it.size })
        val values=listOf(request.service,request.originAction,message)+buttons.flatMap {
            validateServiceName(it.id);listOf(it.id,it.label)
        }+inputs.flatMap { validateServiceName(it.id);require(it.minimum<=it.maximum&&it.value in it.minimum..it.maximum);listOf(it.id,it.label) }
        values.forEach { require('\u0000' !in it&&it.encodeToByteArray().size<=256) }
        val text=(values.joinToString("\u0000")+"\u0000").encodeToByteArray()
        if(inputs.isEmpty()){
            val ints=longArrayOf(request.panelToken,request.signalId,buttons.size.toLong())+buttons.map { if(it.enabled) 1L else 0L }
            call(6,ints,text=text)
        }else{
            val ints=longArrayOf(request.panelToken,request.signalId,buttons.size.toLong(),inputs.size.toLong())+
                buttons.map { if(it.enabled) 1L else 0L }+
                inputs.flatMap { listOf(it.value.toLong(),it.minimum.toLong(),it.maximum.toLong(),if(it.enabled) 1L else 0L) }
            call(8,ints,text=text)
        }
    }
}
