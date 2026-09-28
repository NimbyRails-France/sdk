package nimby.internal

/** SDK bridge state. Public bytecode visibility is required because Exports.kt
 * is compiled in the consuming mod while this class belongs to the SDK klib.
 * Not part of the supported mod-author API. Set before createMod() is evaluated. */
object TranslationEnvironment { var catalogAvailable = true }
