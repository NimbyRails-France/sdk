package fr.nimby.sdk

import com.sun.jna.Memory
import com.sun.jna.Pointer

/** Development-only native construction. Requires the separately built experimental bridge.
 * Tickets belong to one running game/editor and expire on intervening native commands.
 * Never retry creation after an uncertain response: poll its token instead.
 */
data class ConstructionResult(
    val state: ConstructionState, val token: Long, val createdIds: List<Long>,
    val reason: Int, val canUndo: Boolean,
)
enum class ConstructionState { READY, APPLIED, UNDONE, REJECTED, PARTIAL, PENDING }

internal object ConstructionCodec {
    const val REQUEST_SIZE=1568
    const val RESULT_SIZE=544
    fun encode(action: Int, token: Long, source: Long, positions: List<Position>): Memory {
        require(positions.size <= 64)
        when(action) {
            1 -> require(token==0L && source ushr 48==8L && positions.isEmpty())
            2 -> {
                require(token!=0L && source ushr 48==8L && positions.isNotEmpty())
                require(positions.map { it.trackId to it.fraction }.distinct().size==positions.size)
                positions.forEach { require(it.trackId ushr 48==1L && it.fraction.isFinite() &&
                    it.fraction>0 && it.fraction<1 && it.direction in listOf(-1,1)) }
            }
            3 -> require(token!=0L && source==0L && positions.isEmpty())
            else -> error("Action de construction inconnue")
        }
        return Memory(REQUEST_SIZE.toLong()).apply {
            clear();setInt(0,REQUEST_SIZE);setInt(4,1);setInt(8,action);setInt(12,positions.size)
            setLong(16,token);setLong(24,source)
            positions.forEachIndexed { index,p ->
                val offset=32L+index*24
                setLong(offset,p.trackId);setDouble(offset+8,p.fraction);setInt(offset+16,p.direction)
            }
        }
    }
    fun decode(p: Pointer): ConstructionResult {
        require(p.getInt(0)==RESULT_SIZE && p.getInt(4)==1) { "Version du résultat incompatible" }
        val state=p.getInt(8);val count=p.getInt(12);val token=p.getLong(16)
        require(state in 1..6 && count in 0..64 && p.getInt(28) in 0..1) { "Résultat de construction invalide" }
        require(token!=0L || state==4 || state==6)
        val ids=List(count) { p.getLong(32L+it*8).also { id -> require(id ushr 48==8L) } }
        require(ids.distinct().size==ids.size)
        return ConstructionResult(ConstructionState.entries[state-1],token,ids,p.getInt(24),p.getInt(28)!=0)
    }
}
