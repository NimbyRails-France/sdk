package fr.nimby.sdk

import kotlin.math.abs

/**
 * Metric used by the native route scanner, in metres per unit fraction.
 * Available only for a qualified game binary and a stable observed header.
 * Does not certify the topology, construction state or permission to edit.
 */
data class TrackMetric(val trackId: Long, val lengthM: Double) {
    init {
        require(trackId != 0L && lengthM.isFinite() && lengthM > 0)
    }
    fun offsetM(fraction: Double): Double {
        require(fraction.isFinite() && fraction in 0.0..1.0)
        return fraction * lengthM
    }
    fun fraction(offsetM: Double): Double {
        require(offsetM.isFinite() && offsetM in 0.0..lengthM)
        return offsetM / lengthM
    }
    fun distanceM(fromFraction: Double, toFraction: Double): Double = abs(offsetM(toFraction) - offsetM(fromFraction))

    /** Position depuis l'origine de cette voie, en mètres. Le sens ne change
     * pas cette origine. Une position n'est pas une autorisation de construire. */
    fun positionAtMetres(offsetM: Double, direction: TrackDirection): Position =
        Position(trackId, fraction(offsetM), direction.nativeValue)
}

/** Sens dans la représentation de la voie : croissant ou décroissant. */
enum class TrackDirection(val nativeValue: Int) { Forward(1), Backward(-1) }
