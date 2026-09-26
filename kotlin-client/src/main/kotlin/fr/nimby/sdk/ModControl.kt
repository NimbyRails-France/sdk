package fr.nimby.sdk

/** Protocol values only; aspect codes and setting indices belong to the mod. */
enum class ControlOperation(val code: Int) {
    Status(0), Acquire(1), Renew(2), Release(3), ForceSignal(4), RestoreSignal(5),
    Train(6), RestoreTrain(7), Setting(8), RestoreSetting(9), Clear(10), ReadSignal(11), ReadTrain(12)
}
enum class TrainControlMode(val code: Int) { SpeedLimit(0), PhysicalClearance(1), Stop(2) }
enum class TrainControlState { Absent, AwaitingExit, Active, Completed, Cancelled }
data class ControlRequest(
    val operation: ControlOperation, val owner: Long = 0, val generation: Long = 0,
    val leaseMillis: Int = 0, val objectId: Long = 0, val exitSignal: Long = 0,
    val speedMps: Double = 0.0, val mode: TrainControlMode = TrainControlMode.SpeedLimit,
    val releaseByRear: Boolean = false, val value: Int = 0, val settingIndex: Int = 0
)
/** Counts describe requested overlays. ReadSignal reads the last evaluated
 * decision; ReadTrain.active is [TrainControlState.ordinal], not a permission. */
data class ControlResponse(
    val capabilities: Int, val generation: Long, val remainingMillis: Long,
    val signalCount: Int, val trainCount: Int, val settingCount: Int,
    val active: Int, val aspect: Int, val reason: Int,
    val speedMps: Double, val exitSignal: Long, val detail: String
)

/** Explicit recipe lease. No background renewals and no automatic retries.
 * Expiration relinquishes temporary overrides. After a transport failure,
 * inspect status and observations before deciding on another mutation. */
class ModControlSession internal constructor(
    private val client: NimbyClient, val modId: String, val owner: Long, val generation: Long
) : AutoCloseable {
    private var closed = false
    private fun send(operation: ControlOperation, configure: (ControlRequest) -> ControlRequest = { it }): ControlResponse {
        check(!closed) { "Session de recette fermee" }
        return client.modControl(modId, configure(ControlRequest(operation, owner, generation)))
    }
    @Synchronized fun renew(leaseMillis: Int = 5000) = send(ControlOperation.Renew) { it.copy(leaseMillis = leaseMillis) }
    @Synchronized fun forceSignal(signal: Long, aspect: Int) = send(ControlOperation.ForceSignal) { it.copy(objectId = signal, value = aspect) }
    @Synchronized fun restoreSignal(signal: Long) = send(ControlOperation.RestoreSignal) { it.copy(objectId = signal) }
    @Synchronized fun setSetting(signal: Long, index: Int, value: Boolean) = send(ControlOperation.Setting) {
        it.copy(objectId = signal, settingIndex = index, value = if (value) 1 else 0)
    }
    @Synchronized fun restoreSetting(signal: Long, index: Int) = send(ControlOperation.RestoreSetting) { it.copy(objectId = signal, settingIndex = index) }
    @Synchronized fun constrainTrain(train: Long, speedMps: Double, mode: TrainControlMode,
        exitSignal: Long = 0, releaseByRear: Boolean = false) = send(ControlOperation.Train) {
        it.copy(objectId = train, speedMps = speedMps, mode = mode, exitSignal = exitSignal, releaseByRear = releaseByRear)
    }
    @Synchronized fun restoreTrain(train: Long) = send(ControlOperation.RestoreTrain) { it.copy(objectId = train) }
    @Synchronized fun readSignal(signal: Long) = send(ControlOperation.ReadSignal) { it.copy(objectId = signal) }
    @Synchronized fun readTrain(train: Long) = send(ControlOperation.ReadTrain) { it.copy(objectId = train) }
    @Synchronized fun clear() = send(ControlOperation.Clear)
    @Synchronized override fun close() {
        if (!closed) try { send(ControlOperation.Release) } finally { closed = true }
    }
}
