package fr.nimby.sdk

enum class TrainState { Unknown, Driving, StationStop, TimedStop, Depot, DispatchWait, SignalWait, Mothballed, NotPresent, Other }
enum class TrainAlert {
    None, LineClosed, NoPath, InvalidOrders, Collision, SignalWait, ScheduleClosed, DispatchTracksOccupied, NoServices, ServicesAlreadyAssigned, Other;
    companion object {
        internal fun fromNative(value: Int): TrainAlert = when(value) {
            0 -> None; 1 -> LineClosed; 3 -> NoPath; 4 -> InvalidOrders; 5 -> Collision; 6 -> SignalWait
            7 -> ScheduleClosed; 8 -> DispatchTracksOccupied; 9 -> NoServices; 10 -> ServicesAlreadyAssigned; else -> Other
        }
    }
}

/** Joins preserve unknown fields independently. A missing station record is
 * not evidence that a train is outside stations; consult its observed IDs. */
data class TrainRecord(val train: Train, val service: Service?, val details: TrainDetails?,
                       val positionStation: Station?, val locationStation: Station?, val stopStation: Station?,
                       val metadata: TrainMetadata? = null, val line: Line? = null)

internal fun serviceInstant(epoch: Long?, microseconds: Long?): java.time.Instant? {
    if(epoch == null || microseconds == null) return null
    return try {
        java.time.Instant.ofEpochSecond(Math.addExact(epoch, Math.floorDiv(microseconds, 1_000_000L)),
            Math.floorMod(microseconds, 1_000_000L) * 1000)
    } catch (_: ArithmeticException) { null }
      catch (_: java.time.DateTimeException) { null }
}
