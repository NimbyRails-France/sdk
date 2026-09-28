package nimby

/** Images d'une indication, indépendantes de ses règles de conduite.
 * Construire avec [steady] ou [blink] ; le SDK transmet les images et la durée
 * au jeu. Aucun minuteur ni thread n'est nécessaire dans le mod. */
class SignalAnimation internal constructor(
    val first: String, val alternate: String, val everyMs: Long
) {
    /** Image à un instant de simulation, en millisecondes. À zéro, [first].
     * Un même instant donne toujours la même image, y compris pendant une pause.
     * Un temps négatif est refusé. Utile aussi pour les tests du mod. */
    fun frameAt(simulationMs: Long): String {
        require(simulationMs >= 0) { "Le temps simulé doit être positif ou nul" }
        return if (everyMs > 0 && (simulationMs / everyMs) % 2L != 0L) alternate else first
    }
}

private fun imagePath(path: String): String {
    require(path.isNotBlank() && '\u0000' !in path && path.encodeToByteArray().size < 96) {
        "Le chemin d'image doit contenir de 1 à 95 octets UTF-8, sans caractère nul"
    }
    return path
}

/** Image fixe, déclarée dans le catalogue de ressources du modèle. */
fun steady(path: String): SignalAnimation = imagePath(path).let { SignalAnimation(it, it, 0) }

/** Alterne deux images du catalogue toutes les [everyMs] millisecondes simulées.
 * De 100 à 10 000 ms par image ; 500 signifie 500 ms allumé puis 500 ms éteint.
 * La phase suit l'horloge du jeu : pause et accélération sont respectées.
 * Tous les signaux utilisant la même durée sont synchronisés. Le mod choisit
 * les images et la durée ; cette animation ne change aucune consigne de conduite. */
fun blink(on: String, off: String, everyMs: Long): SignalAnimation {
    require(everyMs in 100..10000) { "La durée de chaque image doit être comprise entre 100 et 10000 ms" }
    return SignalAnimation(imagePath(on), imagePath(off), everyMs)
}
