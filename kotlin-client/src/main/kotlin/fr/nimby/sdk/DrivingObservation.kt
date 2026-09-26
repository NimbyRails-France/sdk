package fr.nimby.sdk

/** Material parameters reported by the game, in SI units; not measured performance. */
data class TrainDynamics(
    val maxSpeedMps: Double, val maxAccelerationMps2: Double,
    val serviceBrakingMps2: Double, val emergencyBrakingMps2: Double,
    val tractiveEffortN: Double, val powerW: Double, val emptyMassKg: Double, val lengthM: Double,
)

/**
 * Owned result of one targeted read. Null properties mean unavailable data.
 * Purchased and current dynamics are independent: neither substitutes for the other.
 * sessionGeneration belongs to this connection, not to a globally unique save.
 * Reset any derived state on reconnect as well as on generation changes.
 * elapsedBeginMillis/elapsedEndMillis bound the read in simulation time;
 * capturedAtMillis is the system timestamp. This is not an atomic game tick.
 */
data class DrivingObservation(
    val trainId: Long, val sessionGeneration: Long, val capturedAtMillis: Long,
    val elapsedBeginMillis: Long, val elapsedEndMillis: Long,
    val purchasedDynamics: TrainDynamics?, val currentDynamics: TrainDynamics?,
    val position: Position?, val speedMps: Double?, val speedDefaulted: Boolean,
    val motionAvailable: Boolean,
)
