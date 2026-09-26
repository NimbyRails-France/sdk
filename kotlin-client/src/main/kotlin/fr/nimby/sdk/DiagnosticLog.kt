package fr.nimby.sdk

import java.nio.file.*
import java.time.Instant
import kotlin.io.path.*

/** Production diagnostics, separate from observations and game saves. Never
 * record a successful polling tick. Repeated failures are coalesced, with a
 * count on recovery/change or after 30 seconds. Each write closes the file.
 */
class DiagnosticLog(val component: String, root: Path = defaultRoot(),
                    private val maximumBytes: Long = 2 * 1024 * 1024) {
    companion object {
        fun defaultRoot(): Path = System.getenv("NRF_LOG_DIR")?.takeIf { it.isNotBlank() }?.let(Path::of)
            ?: Path.of(System.getenv("LOCALAPPDATA") ?: System.getProperty("user.home"), "NimbyRailsFrance", "logs")
        private val instances = mutableMapOf<String, DiagnosticLog>()
        @Synchronized fun forComponent(name: String): DiagnosticLog = instances.getOrPut(name) { DiagnosticLog(name) }
    }
    init { require(component.matches(Regex("[a-zA-Z0-9_.-]{1,80}")) && component !in setOf(".", "..")) }
    val directory: Path = root.resolve(component)
    val file: Path = directory.resolve("$component-${ProcessHandle.current().pid()}.log")
    private var last = ""
    private var repeated = 0L
    private var writtenAt = 0L
    private var prepared = false

    @Synchronized fun write(message: String, failure: Throwable? = null, level: String = if (failure == null) "INFO" else "ERROR") {
        val text = "$level $message" + (failure?.let { "\n${it.stackTraceToString()}" } ?: "")
        val now = System.nanoTime()
        if (text == last && now - writtenAt < 30_000_000_000L) { repeated++; return }
        try {
            directory.createDirectories()
            if (!prepared) {
                // Only our own files, no recursive deletion or symlink traversal.
                Files.list(directory).use { paths -> paths.filter { Files.isRegularFile(it, LinkOption.NOFOLLOW_LINKS) && it.name.matches(Regex("${Regex.escape(component)}-[0-9]+\\.log(?:\\.[1-3])?")) }
                    .sorted(compareByDescending { Files.getLastModifiedTime(it).toMillis() }).skip(16).forEach(Files::deleteIfExists) }
                prepared = true
            }
            val entry = buildString {
                if (repeated > 0) append("${Instant.now()} [$component] Previous event repeated $repeated times\n")
                append("${Instant.now()} [$component] [pid=${ProcessHandle.current().pid()} thread=${Thread.currentThread().name}] $text\n")
            }.toByteArray(Charsets.UTF_8)
            if (file.exists() && file.fileSize() + entry.size > maximumBytes) {
                file.resolveSibling("${file.name}.3").deleteIfExists()
                for (index in 2 downTo 1) {
                    val old = file.resolveSibling("${file.name}.$index")
                    if (old.exists()) Files.move(old, file.resolveSibling("${file.name}.${index + 1}"), StandardCopyOption.REPLACE_EXISTING)
                }
                Files.move(file, file.resolveSibling("${file.name}.1"), StandardCopyOption.REPLACE_EXISTING)
            }
            Files.write(file, entry, StandardOpenOption.CREATE, StandardOpenOption.APPEND)
            last = text; repeated = 0; writtenAt = now
        } catch (error: Exception) { System.err.println("Cannot write diagnostic log $file: ${error.message}\n$text") }
    }

    /** Install once at the application entry point, never inside a library
     * loaded into someone else's process. Fatal native crashes remain outside
     * JVM exception handling; a log is not a native crash dump. */
    fun startApplication(version: String) {
        write("Starting $component version=$version Java=${System.getProperty("java.version")} OS=${System.getProperty("os.name")} ${System.getProperty("os.arch")} log=$file")
        val previous = Thread.getDefaultUncaughtExceptionHandler()
        Thread.setDefaultUncaughtExceptionHandler { thread, failure ->
            write("Uncaught exception on ${thread.name}", failure)
            previous?.uncaughtException(thread, failure)
        }
        Runtime.getRuntime().addShutdownHook(Thread({ write("Stopping $component") }, "$component-diagnostics"))
    }
}
