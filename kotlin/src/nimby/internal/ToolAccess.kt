package nimby.internal

import nimby.ToolContext

/** Transport privé partagé avec le pont précompilé. Pas une API de mod. */
object ToolAccess {
    fun <T> withContext(world: String,generation: Long,call: (Int,LongArray,DoubleArray,ByteArray)->Int,
                        block: (ToolContext)->T): T {
        val context=ToolContext(world,generation,call)
        return try { block(context) } finally { context.close() }
    }
}
