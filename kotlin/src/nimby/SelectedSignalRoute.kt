package nimby

/** Borne observée sur le parcours que le jeu analyse pour le train.
 * Ce parcours n'est ni une réservation possédée ni une autorisation de mouvement. */
enum class RouteDestination {
    /** Parcours incomplet, indisponible ou sans borne qualifiée. */
    Unknown,
    /** Un autre signal de protection est atteint sur ce parcours. */
    ProtectingSignal,
    /** Une borne native identifiée termine le parcours observé.
     * Une limite de distance/temps ou la simple présence géométrique d'une balise
     * ne prouvent pas cette fin. */
    NativeBoundary
}

/** Balise copiée dans l'ordre du parcours observé. Identifiant et catalogue
 * restent opaques : le SDK ne leur attribue aucune règle de signalisation. */
data class SignalRouteMarker(val id: Long, val textureSet: String, val textureState: Int)

/** Parcours sélectionné observé pour un train, distinct de sa réservation native.
 * [occupation] concerne les portions effectivement observées ; seule une liste
 * complète jusqu'à une borne qualifiée peut prouver leur absence d'occupation.
 * Consulter [Observation.selectedRoute] pour respecter fraîcheur et identité du train. */
class SelectedSignalRoute private constructor(
    val trainId: Long,
    val boundarySignalId: Long,
    val destination: RouteDestination,
    val occupation: Occupancy,
    markers: List<SignalRouteMarker>,
    val complete: Boolean
) {
    private val copiedMarkers = markers.toTypedArray()
    /** Vue indépendante de la collection d'entrée, dans l'ordre observé. */
    val markers: List<SignalRouteMarker> = object : AbstractList<SignalRouteMarker>() {
        override val size: Int get() = copiedMarkers.size
        override fun get(index: Int): SignalRouteMarker = copiedMarkers[index]
    }

    companion object {
        /** Aucun parcours qualifié disponible. Une liste vide ici ne prouve rien. */
        val Unknown = SelectedSignalRoute(0, 0, RouteDestination.Unknown, Occupancy.Unknown, emptyList(), false)
        internal const val maximumMarkers = 128

        /** Construit des faits copiés ou une donnée de test, sans lire le jeu.
         * Une destination connue exige une observation complète et l'identifiant
         * de sa borne. Un parcours partiel peut prouver Occupied, jamais Clear.
         * Aucune borne n'est déduite de la dernière portion ou d'une liste vide. */
        fun observed(
            trainId: Long,
            boundarySignalId: Long,
            destination: RouteDestination,
            occupation: Occupancy,
            markers: List<SignalRouteMarker>,
            complete: Boolean = true
        ): SelectedSignalRoute {
            require(validIdentity(trainId, 5L)) { "Invalid observed route train identity" }
            require(markers.size <= maximumMarkers) { "Too many observed route markers" }
            if (destination == RouteDestination.Unknown) {
                require(boundarySignalId == 0L && !complete) { "An unknown route has no qualified boundary" }
            } else {
                require(complete && validIdentity(boundarySignalId, 8L)) { "A complete route needs a qualified signal boundary" }
            }
            require(occupation != Occupancy.Clear || complete && destination != RouteDestination.Unknown) {
                "An incomplete route cannot prove absence of occupation"
            }
            val identities = HashSet<Long>(markers.size)
            for (marker in markers) {
                require(validIdentity(marker.id, 8L) && identities.add(marker.id)) { "Invalid or duplicate observed marker identity" }
                require(marker.textureSet.isNotBlank() && marker.textureSet.length <= 256 &&
                    marker.textureSet.all { it.code in 0x20..0x7e }) { "Invalid observed marker catalogue" }
                require(marker.textureState in 0..255) { "Invalid observed marker texture state" }
            }
            return SelectedSignalRoute(trainId, boundarySignalId, destination, occupation, markers, complete)
        }

        private fun validIdentity(id: Long, type: Long): Boolean =
            id ushr 48 == type && (id and 0x0000ffffffffffffL) != 0L
    }
}
