package fr.nimby.sdk.internal

import com.sun.jna.Pointer
import fr.nimby.sdk.*

/** Private pack(8) ABI from nimby/detail/driving.h, verified by the native test fixture. */
internal object NativeDriving {
    const val SIZE = 480
    private const val PURCHASED = 48L
    private const val CURRENT = 112L
    private const val TRAIN = 176L

    private fun dynamics(p: Pointer, offset: Long): TrainDynamics? {
        val values = List(8) { p.getDouble(offset + it * 8L) }
        if (values.any { !it.isFinite() || it < 0.0 }) return null
        return TrainDynamics(values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7])
    }

    fun decode(p: Pointer, requestedTrain: Long): DrivingObservation {
        check(p.getInt(0) == SIZE && p.getLong(8) == requestedTrain) { "Invalid targeted observation identity or size" }
        val flags = p.getInt(4)
        val train = NimbyTrain(p.share(TRAIN, NimbyTrain.SIZE.toLong()))
        check(train.id == requestedTrain) { "Invalid train record identity" }
        val position = if (train.flags and 2 != 0 && train.trackFraction.isFinite() && train.trackFraction in 0.0..1.0)
            Position(train.trackId, train.trackFraction, train.direction) else null
        val defaulted = train.flags and 8 != 0
        val speed = if (train.flags and 4 != 0 && !defaulted && train.speedMps.isFinite() && train.speedMps >= 0.0)
            train.speedMps else null
        return DrivingObservation(requestedTrain, p.getLong(16), p.getLong(24), p.getLong(32), p.getLong(40),
            if (flags and 1 != 0) dynamics(p, PURCHASED) else null,
            if (flags and 2 != 0) dynamics(p, CURRENT) else null,
            position, speed, defaulted, flags and 4 != 0)
    }
}
