@file:OptIn(kotlinx.cinterop.ExperimentalForeignApi::class)
package nimby

import kotlinx.cinterop.*
import platform.posix.*

/** Lecture UTF-8 bornée pour les outils et tests Kotlin/Native. */
fun readTextFile(path: String): String {
    val file = fopen(path, "rb") ?: error("Fichier inaccessible : $path")
    try {
        require(fseek(file, 0, SEEK_END) == 0)
        val length = ftell(file)
        require(length in 0..4_194_304) { "Fichier trop volumineux : $path" }
        require(fseek(file, 0, SEEK_SET) == 0)
        if (length.toLong() == 0L) return ""
        val bytes = ByteArray(length.toInt())
        bytes.usePinned { require(fread(it.addressOf(0), 1u, bytes.size.toULong(), file) == bytes.size.toULong()) }
        return bytes.decodeToString()
    } finally {
        fclose(file)
    }
}
