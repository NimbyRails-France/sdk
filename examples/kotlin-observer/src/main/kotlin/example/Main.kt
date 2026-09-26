package example

import fr.nimby.sdk.NimbyClient
import java.nio.file.Path

fun main(args: Array<String>) {
    if (args.contentEquals(arrayOf("--help"))) {
        println("Usage: kotlin-observer <NimbyRailsFranceSDK.dll> <game PID>")
        return
    }
    require(args.size == 2) { "Provide the SDK library path and the game PID; use --help." }
    val pid = args[1].toIntOrNull()
    require(pid != null && pid > 0) { "The game PID must be a positive integer." }
    NimbyClient.open(Path.of(args[0]), pid).use { client ->
        val snapshot = client.capture()
        println("PID=${snapshot.processId} trains=${snapshot.trains.size} tracks=${snapshot.tracks.size} signals=${snapshot.signals.size}")
        snapshot.trains.take(10).forEach { train -> println("${train.id}: ${train.name}") }
        // This second read queries one train only. It can disappear between
        // reads; null remains unavailable data, never an assumed stopped train.
        snapshot.trains.firstOrNull()?.let { train ->
            val driving = client.readTrain(train.id)
            println("Targeted speed (m/s)=${driving?.speedMps}, length (m)=${driving?.currentDynamics?.lengthM}")
        }
    }
}
