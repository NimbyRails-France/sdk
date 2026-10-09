package nimby.internal

import nimby.GameMod
import nimby.TrainLengthLimit
import nimby.ToolMod
import nimby.checkedTrainEditor
import nimby.checkedModOptions

/** Transport privé du SDK, partagé avec l'adaptateur précompilé.
 * Copie la déclaration et vérifie la liaison même pour une sous-classe
 * GameMod qui ne passe pas par le builder. Aucun callback ni accès au jeu. */
class TrainLengthLimitAccess(mod: GameMod) {
    val declaration: TrainLengthLimit? = checkedTrainEditor(
        mod.trainEditor, checkedModOptions(mod.options, mod.windows.size),
    )?.maximumLength
    fun message(index: Int): String {
        require(index in 0..2) { "Indice de message de composition invalide" }
        val limit = declaration ?: return ""
        return when (index) {
            0 -> limit.exceeded
            1 -> limit.lengthUnavailable
            else -> limit.verificationUnavailable
        }
    }
    // Explicit declaration, rather than guessing whether a lambda does work.
    // Custom subclasses keep the conservative existing callback lifecycle.
    val hasTickCallback: Boolean = (mod as? ToolMod)?.hasTickCallback ?: true
}
