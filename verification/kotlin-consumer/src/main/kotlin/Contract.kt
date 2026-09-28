package verification

import fr.nimby.sdk.Nimby
import java.nio.file.Path

// Compilation verifies that the installed composite exposes the public facade
// and brings in its transitive JNA dependency. This function is never executed.
fun checkConsumerTypes(library: Path) {
    Nimby.connect(library).use { game ->
        game.clock.read()
        game.snapshot()
    }
}
