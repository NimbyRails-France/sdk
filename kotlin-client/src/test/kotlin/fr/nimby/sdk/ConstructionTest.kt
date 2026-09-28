package fr.nimby.sdk

import com.sun.jna.Memory
import kotlin.test.*

class ConstructionTest {
    private val signal=0x8000000000001L
    private val track=0x1000000000001L
    @Test fun requestUsesThePrivateAbiLayout() {
        ConstructionCodec.encode(2,17,signal,listOf(Position(track,.25,-1))).use {
            assertEquals(1568,it.getInt(0));assertEquals(1,it.getInt(4))
            assertEquals(2,it.getInt(8));assertEquals(1,it.getInt(12))
            assertEquals(17,it.getLong(16));assertEquals(signal,it.getLong(24))
            assertEquals(track,it.getLong(32));assertEquals(.25,it.getDouble(40))
            assertEquals(-1,it.getInt(48));assertEquals(0,it.getInt(52))
        }
    }
    @Test fun invalidRequestsNeverReachNativeCode() {
        for(fraction in listOf(0.0,1.0,Double.NaN,Double.POSITIVE_INFINITY))
            assertFailsWith<IllegalArgumentException> { ConstructionCodec.encode(2,1,signal,listOf(Position(track,fraction,1))) }
        val p=Position(track,.2,1)
        assertFailsWith<IllegalArgumentException> { ConstructionCodec.encode(2,1,signal,listOf(p,p.copy(direction=-1))) }
        assertFailsWith<IllegalArgumentException> { ConstructionCodec.encode(2,1,signal,List(65) { p }) }
        assertFailsWith<IllegalArgumentException> { ConstructionCodec.encode(3,0,0,emptyList()) }
    }
    @Test fun resultIsOwnedAndRejectsCorruptCountsOrIds() {
        Memory(544).use {
            it.clear();it.setInt(0,544);it.setInt(4,1);it.setInt(8,2)
            it.setInt(12,1);it.setLong(16,17);it.setInt(28,1);it.setLong(32,signal)
            val result=ConstructionCodec.decode(it)
            assertEquals(ConstructionState.APPLIED,result.state);assertTrue(result.canUndo)
            it.setLong(32,track)
            assertEquals(listOf(signal),result.createdIds)
            assertFailsWith<IllegalArgumentException> { ConstructionCodec.decode(it) }
            it.setInt(12,65)
            assertFailsWith<IllegalArgumentException> { ConstructionCodec.decode(it) }
        }
    }
}
