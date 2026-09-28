package fr.nimby.sdk

import java.nio.file.Path
import java.time.Instant

/** Point d'entrée d'une application Kotlin. Un mod chargé par le loader utilise
 * le module nimby et n'a pas besoin d'ouvrir cette connexion externe. */
object Nimby {
    /** Liste les jeux accessibles sans choisir silencieusement un processus. */
    fun runningGames(): List<GameProcess> = GameProcesses.discover()

    /** Ouvre le SDK indiqué. Sans PID, exige exactement un jeu accessible.
     * La connexion se ferme avec use { game -> … }. Aucun essai de mutation
     * ni injection n'est déclenché par cette ouverture. */
    fun connect(sdk: Path, processId: Int? = null): Game {
        val selected = processId ?: runningGames().let { games ->
            require(games.size == 1) { "Un seul jeu doit être ouvert, ou fournissez processId (${games.size} trouvés)." }
            games.single().pid
        }
        return Game(NimbyClient.open(sdk, selected))
    }
}

/** Une connexion organisée par usage. Les observations sont des copies Kotlin.
 * [advanced] conserve l'accès à toutes les opérations détaillées du transport,
 * sur la même session et avec le même verrou. Ne pas le fermer séparément. */
class Game internal constructor(val advanced: NimbyClient) : AutoCloseable {
    val trains: Trains = Trains(advanced)
    val clock: Clock = Clock(advanced)
    val signals: Signals = Signals(advanced)
    val mods: Mods = Mods(advanced)
    val construction: Construction = Construction(advanced)

    /** Capture complète ; demander seulement le train voulu avec trains.read
     * quand les autres tables ne sont pas nécessaires. */
    fun snapshot(selectedTrain: Long? = null): Observation = advanced.capture(selectedTrain)
    override fun close(): Unit = advanced.close()

    class Trains internal constructor(private val client: NimbyClient) {
        /** Null signifie absent ou instable, jamais arrêté par défaut. */
        fun read(id: Long): DrivingObservation? = client.readTrain(id)
    }

    class Clock internal constructor(private val client: NimbyClient) {
        /** Lit via une capture complète. Réutiliser snapshot.clock lorsqu'une
         * capture a déjà été réalisée, pour ne pas doubler le coût de lecture. */
        fun read(): SimulationClock? = client.capture().clock
        /** UTC en secondes entières. Écriture explicite, sans nouvelle tentative. */
        fun set(utc: Instant, recalculateTrains: Boolean = false): SimulationTimeChange =
            client.setSimulationDateTime(utc, recalculateTrains)
    }

    class Signals internal constructor(private val client: NimbyClient) {
        /** Affichage seul, de 1 à 60 secondes. Ne change pas une permission. */
        fun showTexture(id: Long, catalogue: String, image: String, durationMillis: Int): Unit =
            client.showSignalTextureFor(id, catalogue, image, durationMillis)
        fun restoreTexture(id: Long): Unit = client.restoreSignalTexture(id)
    }

    class Mods internal constructor(private val client: NimbyClient) {
        /** Interroge le mod chargé. Une erreur de transport n'est pas une preuve
         * d'absence : l'exception reste visible au programme appelant. */
        fun status(id: String): ControlResponse = client.modControl(id, ControlRequest(ControlOperation.Status))
        /** Bail explicite pour une recette. Le mod décide des forçages acceptés. */
        fun control(id: String, leaseMillis: Int = 5000): ModControlSession = client.acquireModControl(id, leaseMillis)
    }

    /** Expérimental : nécessite le pont de construction séparé. */
    class Construction internal constructor(private val client: NimbyClient) {
        fun prepare(sourceSignal: Long): ConstructionResult = client.prepareConstruction(sourceSignal)
        fun create(token: Long, sourceSignal: Long, positions: List<Position>): ConstructionResult =
            client.createSignals(token, sourceSignal, positions)
        fun poll(token: Long): ConstructionResult = client.pollConstruction(token)
        fun undo(token: Long): ConstructionResult = client.undoConstruction(token)
    }
}
