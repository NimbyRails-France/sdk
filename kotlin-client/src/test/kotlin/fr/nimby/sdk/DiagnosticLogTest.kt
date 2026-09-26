package fr.nimby.sdk

import java.nio.file.Files
import kotlin.io.path.*
import kotlin.test.*

class DiagnosticLogTest {
    @Test fun errorsSurviveRestartWithUnicodeAndRepeatedFailuresAreCounted() {
        val root = Files.createTempDirectory("nrf-jvm-log-")
        val log = DiagnosticLog("fixture", root)
        val error = IllegalStateException("Échec de connexion", IllegalArgumentException("DLL périmée"))
        error.addSuppressed(Exception("Fermeture échouée"))
        repeat(4) { log.write("Connexion", error) }
        log.write("Rétabli")
        DiagnosticLog("fixture", root).write("Nouvelle session")
        val text = log.file.readText()
        listOf("Échec de connexion", "DLL périmée", "Fermeture échouée", "repeated 3 times", "Rétabli", "Nouvelle session").forEach { assertContains(text, it) }
    }
    @Test fun rotationAndLoggingFailureDoNotChangeApplicationOutcome() {
        val root = Files.createTempDirectory("nrf-jvm-rotate-")
        val log = DiagnosticLog("fixture", root, maximumBytes = 150)
        repeat(10) { log.write("Event $it") }
        assertTrue(log.file.resolveSibling("${log.file.name}.3").exists())
        assertFalse(log.file.resolveSibling("${log.file.name}.4").exists())
        assertContains(log.file.readText(), "Event 9")
        val blocked = root.resolve("blocked").apply { writeText("keep") }
        DiagnosticLog("fixture", blocked).write("Unwritable", Exception("original failure"))
        assertEquals("keep", blocked.readText())
        assertFails { DiagnosticLog("../escape", root) }
    }
}
