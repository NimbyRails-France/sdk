package nimby

/** Refus d'une opération d'outil, avec le statut SDK conservé pour le diagnostic.
 * [isBusy] signale une contention ou un budget temporairement indisponible.
 * Cela n'autorise pas à rejouer une écriture : une construction ou un changement
 * d'heure peut déjà avoir commencé. Seules les présentations sans effet sur la
 * partie, comme un aperçu, peuvent être retentées au prochain callback. */
class ToolOperationException(val status: Int, message: String) : IllegalStateException(message) {
    val isBusy: Boolean get() = status == 12
}
