package fr.nimby.sdk

import kotlin.math.abs

/**
 * Longueur longitudinale observée d'une voie, en mètres, pour convertir les
 * positions entre mètres et fractions (0 = origine, 1 = extrémité).
 * Ces conversions sont locales : elles ne relisent pas le jeu.
 * Une métrique absente reste inconnue ; elle ne se déduit pas du dessin de la
 * voie et ne suffit pas à autoriser une construction.
 */
data class TrackMetric(val trackId: Long, val lengthM: Double) {
    init {
        require(trackId != 0L && lengthM.isFinite() && lengthM > 0)
    }
    /** Distance depuis l'origine de la voie ; [fraction] doit être dans 0..1. */
    fun offsetM(fraction: Double): Double {
        require(fraction.isFinite() && fraction in 0.0..1.0)
        return fraction * lengthM
    }
    /** Fraction correspondant à une distance dans 0..[lengthM], sans inversion du sens. */
    fun fraction(offsetM: Double): Double {
        require(offsetM.isFinite() && offsetM in 0.0..lengthM)
        return offsetM / lengthM
    }
    /** Distance positive entre deux positions de cette même voie. */
    fun distanceM(fromFraction: Double, toFraction: Double): Double = abs(offsetM(toFraction) - offsetM(fromFraction))

    /** Position depuis l'origine de cette voie, en mètres. Le sens ne change
     * pas cette origine. Une position n'est pas une autorisation de construire. */
    fun positionAtMetres(offsetM: Double, direction: TrackDirection): Position =
        Position(trackId, fraction(offsetM), direction.nativeValue)
}

/** Sens dans la représentation de la voie : croissant ou décroissant. */
enum class TrackDirection(val nativeValue: Int) { Forward(1), Backward(-1) }
