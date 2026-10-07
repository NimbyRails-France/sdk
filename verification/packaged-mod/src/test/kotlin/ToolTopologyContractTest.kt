package nimby.mod

import kotlin.test.*
import nimby.*

class ToolTopologyContractTest {
    private fun network(tracks: List<ToolTrack>, junctions: List<ToolJunction> = emptyList(), signals: List<ToolSignal> = emptyList()) =
        ToolNetwork("world", 7, tracks, junctions, signals)

    @Test fun joinsExposeTheCorrectEntryForAllEndpointOrientations() {
        for (exit in ToolTrackEnd.entries) for (entry in ToolTrackEnd.entries) {
            val first = ToolTrack(1, if(exit == ToolTrackEnd.A) 2 else null, if(exit == ToolTrackEnd.B) 2 else null, 100.0)
            val second = ToolTrack(2, if(entry == ToolTrackEnd.A) 1 else null, if(entry == ToolTrackEnd.B) 1 else null, 200.0)
            val topology = network(listOf(first, second)).topology()
            assertEquals(ToolTrackConnection.Join(2, entry), topology.track(1)!!.connection(exit))
            assertEquals(ToolTrackConnection.Join(1, exit), topology.track(2)!!.connection(entry))
            assertSame(ToolTrackConnection.Unknown, topology.track(1)!!.connection(if(exit == ToolTrackEnd.A) ToolTrackEnd.B else ToolTrackEnd.A))
        }
    }

    @Test fun missingAmbiguousOrIncompleteLinksAreUnknownRatherThanInventedEnds() {
        for (other in listOf<ToolTrack?>(null, ToolTrack(2, null, null, 100.0), ToolTrack(2, 1, 1, 100.0), ToolTrack(2, 1, null, null))) {
            val topology = network(listOfNotNull(ToolTrack(1, null, 2, 100.0), other)).topology()
            assertSame(ToolTrackConnection.Unknown, topology.track(1)!!.connection(ToolTrackEnd.B))
            if(other?.lengthM == null) assertNull(topology.track(2))
        }
        assertSame(ToolTrackConnection.Unknown, network(listOf(ToolTrack(1, 1, null, 100.0))).topology().track(1)!!.a)
    }

    @Test fun switchEndpointsAndTrailingBranchesAreTypedBeforeFollowingAnyLink() {
        for (direction in listOf(-1, 1)) {
            val topology = network(listOf(ToolTrack(1, null, 2, 100.0), ToolTrack(2, 1, null, 200.0)),
                listOf(ToolJunction(2, 1, .75, -direction, direction), ToolJunction(3, 1, .25, direction, 1))).topology()
            assertEquals(listOf(25.0, 75.0), topology.track(1)!!.junctionOffsetsM)
            val branchEnd = if(direction == 1) ToolTrackEnd.A else ToolTrackEnd.B
            assertSame(ToolTrackConnection.Junction, topology.track(2)!!.connection(branchEnd))
        }
        val endpoints = network(listOf(ToolTrack(1, null, null, 100.0)),
            listOf(ToolJunction(2, 1, 0.0, 1, 1), ToolJunction(3, 1, 1.0, 1, -1))).topology().track(1)!!
        assertSame(ToolTrackConnection.Junction, endpoints.a)
        assertSame(ToolTrackConnection.Junction, endpoints.b)
    }

    @Test fun cyclesRemainExplicitAndTopologyOwnsItsSnapshotWithoutCrossCaptureCaching() {
        val tracks = mutableListOf(ToolTrack(1, 3, 2, 100.0), ToolTrack(2, 1, 3, 100.0), ToolTrack(3, 2, 1, 100.0))
        val signals = mutableListOf(ToolSignal(8, 1, .5, -1, 4))
        val source = network(tracks, signals = signals)
        val topology = source.topology()
        tracks.clear(); signals.clear()
        assertEquals(3, topology.tracks.size)
        assertEquals(1, topology.signal(8)!!.travelDirection)
        var trackId = 1L
        repeat(3) { trackId = (topology.track(trackId)!!.b as ToolTrackConnection.Join).trackId }
        assertEquals(1L, trackId)
        assertTrue(source.topology().tracks.isEmpty())
        assertEquals("world", topology.worldId); assertEquals(7L, topology.generation)
    }

    @Test fun invalidGeometryFailsBeforeAnyPlacementCanUseIt() {
        val track = ToolTrack(1, null, null, 100.0)
        assertFailsWith<IllegalArgumentException> { network(listOf(track, track)).topology() }
        for(length in listOf(0.0, -1.0, Double.NaN, Double.POSITIVE_INFINITY))
            assertFailsWith<IllegalArgumentException> { network(listOf(track.copy(lengthM = length))).topology() }
        for(junction in listOf(ToolJunction(1, 1, .5, 1, 1), ToolJunction(2, 1, Double.NaN, 1, 1), ToolJunction(2, 1, .5, 0, 1)))
            assertFailsWith<IllegalArgumentException> { network(listOf(track), listOf(junction)).topology() }
        val signal = ToolSignal(8, 1, .5, 1, 1)
        assertFailsWith<IllegalArgumentException> { network(listOf(track), signals = listOf(signal, signal)).topology() }
        assertFailsWith<IllegalArgumentException> { network(listOf(track), signals = listOf(signal.copy(direction = 0))).topology() }
        assertFailsWith<IllegalArgumentException> { network(listOf(track), signals = listOf(signal.copy(fraction = Double.NaN))).topology() }
        assertTrue(network(listOf(track.copy(lengthM = null)), signals = listOf(signal)).topology().signals.isEmpty())
    }

    @Test fun trackListRetainsInputOrderSizeAndListValueSemantics() {
        val input = listOf(
            ToolTrack(9, null, 3, 900.0),
            ToolTrack(7, null, null, null),
            ToolTrack(3, 9, null, 300.0),
            ToolTrack(12, null, 99, 1200.0),
        )
        val topology = network(input).topology()
        val expected = listOf(
            ToolRouteTrack(9, 900.0, ToolTrackConnection.Unknown, ToolTrackConnection.Join(3, ToolTrackEnd.A), emptyList()),
            ToolRouteTrack(3, 300.0, ToolTrackConnection.Join(9, ToolTrackEnd.B), ToolTrackConnection.Unknown, emptyList()),
            ToolRouteTrack(12, 1200.0, ToolTrackConnection.Unknown, ToolTrackConnection.Unknown, emptyList()),
        )
        assertEquals(3, topology.tracks.size)
        assertEquals(expected, topology.tracks)
        assertEquals(topology.tracks, expected)
        assertEquals(expected.hashCode(), topology.tracks.hashCode())
        assertEquals(expected.subList(1, 3), topology.tracks.subList(1, 3))
        assertEquals(listOf(12L, 3L, 9L), topology.tracks.asReversed().map { it.id })
        assertEquals(expected, topology.tracks.toList())
        for (index in expected.indices) {
            assertEquals(expected[index], topology.tracks[index])
            assertEquals(expected[index], topology.track(expected[index].id))
        }
        assertNull(topology.track(7))
        assertNull(topology.track(99))
        assertFailsWith<IndexOutOfBoundsException> { topology.tracks[-1] }
        assertFailsWith<IndexOutOfBoundsException> { topology.tracks[3] }
    }

    @Test fun inputReplacementsAndExposedOffsetsCannotMutateFutureTrackResolutions() {
        val inputTracks = mutableListOf(ToolTrack(1, null, 2, 100.0), ToolTrack(2, 1, null, 200.0))
        val inputJunctions = mutableListOf(ToolJunction(2, 1, .75, -1, 1), ToolJunction(3, 1, .25, 1, -1))
        val inputSignals = mutableListOf(ToolSignal(8, 1, .5, -1, 4))
        val source = network(inputTracks, inputJunctions, inputSignals)
        val topology = source.topology()
        val original = topology.track(1)!!
        val originalSignal = topology.signal(8)!!
        inputTracks[0] = inputTracks[0].copy(lengthM = 800.0, linkB = null)
        inputTracks.reverse()
        inputJunctions.clear()
        inputSignals[0] = inputSignals[0].copy(fraction = .9)
        assertEquals(original, topology.track(1))
        assertEquals(original, topology.tracks[0])
        assertEquals(originalSignal, topology.signal(8))
        assertEquals(listOf(originalSignal), topology.signals)
        assertEquals(listOf(25.0, 75.0), topology.track(1)!!.junctionOffsetsM)
        // List is a read-only API. Even a caller attempting a mutable cast
        // must not gain access to the resolver's private junction storage.
        val exposed = topology.track(1)!!.junctionOffsetsM
        if (exposed is MutableList<Double>) {
            try { exposed[0] = -999.0 } catch (_: UnsupportedOperationException) { }
        }
        assertEquals(listOf(25.0, 75.0), topology.track(1)!!.junctionOffsetsM)
        assertEquals(listOf(25.0, 75.0), topology.tracks[0].junctionOffsetsM)
        val fresh = source.topology()
        assertEquals(listOf(2L, 1L), fresh.tracks.map { it.id })
        assertEquals(800.0, fresh.track(1)!!.lengthM)
        assertTrue(fresh.track(1)!!.junctionOffsetsM.isEmpty())
        assertEquals(.9, fresh.signal(8)!!.fraction)
    }

    @Test fun remoteInvalidRecordsAreRejectedBeforeAnyTrackIsRequested() {
        val tracks = List(64) { ToolTrack(it.toLong() + 1, null, null, 100.0) }
        assertFailsWith<IllegalArgumentException> { network(tracks + tracks.last()).topology() }
        assertFailsWith<IllegalArgumentException> { network(tracks + ToolTrack(65, null, null, Double.NaN)).topology() }
        assertFailsWith<IllegalArgumentException> {
            network(tracks, junctions = listOf(ToolJunction(63, 64, .5, 1, 0))).topology()
        }
        assertFailsWith<IllegalArgumentException> {
            network(tracks, signals = listOf(ToolSignal(8, 64, .5, 0, 1))).topology()
        }
        assertFailsWith<IllegalArgumentException> {
            network(tracks, signals = listOf(ToolSignal(8, 64, Double.POSITIVE_INFINITY, 1, 1))).topology()
        }
        // Identity validation also applies to signals omitted for an unknown
        // track. Their unusable geometry is still ignored, as before.
        val omitted = ToolSignal(8, 99, Double.NaN, 0, 1)
        assertTrue(network(tracks, signals = listOf(omitted)).topology().signals.isEmpty())
        assertFailsWith<IllegalArgumentException> { network(tracks, signals = listOf(omitted, omitted)).topology() }
    }

    @Test fun repeatedInterleavedQueriesRemainEquivalentWithBothJunctionDirections() {
        val topology = network(
            listOf(ToolTrack(1, null, null, 100.0), ToolTrack(2, null, 1, 200.0), ToolTrack(3, 1, null, 300.0)),
            listOf(ToolJunction(2, 1, .25, 1, 1), ToolJunction(3, 1, .75, -1, -1)),
        ).topology()
        val expected = topology.tracks.toList()
        repeat(100) { pass ->
            for (index in expected.indices.reversed()) {
                assertEquals(expected[index], topology.track(expected[index].id))
                assertEquals(expected[(index + pass) % expected.size], topology.tracks[(index + pass) % expected.size])
            }
            assertNull(topology.track(999))
        }
        assertEquals(listOf(25.0, 75.0), topology.track(1)!!.junctionOffsetsM)
        assertSame(ToolTrackConnection.Junction, topology.track(2)!!.a)
        assertSame(ToolTrackConnection.Junction, topology.track(3)!!.b)
    }

    @Test fun interleavedIteratorsAndIndexedReadsRetainAnOwnedSnapshot() {
        val inputTracks = mutableListOf(
            ToolTrack(1, null, 2, 100.0),
            ToolTrack(9, null, null, null),
            ToolTrack(2, 1, 3, 200.0),
            ToolTrack(3, 2, null, 300.0),
        )
        val inputJunctions = mutableListOf(ToolJunction(4, 2, .5, 1, 1))
        val source = network(inputTracks, inputJunctions)
        val topology = source.topology()
        // Obtain values through point lookups before asking for an iterator.
        val expected = listOf(topology.track(1)!!, topology.tracks[1], topology.track(3)!!)
        assertEquals(3, topology.tracks.size)
        inputTracks[0] = inputTracks[0].copy(lengthM = 900.0, linkB = null)
        inputTracks.reverse()
        inputJunctions.clear()

        val first = topology.tracks.iterator()
        assertEquals(expected[0], first.next())
        // A second traversal and indexed reads cannot advance the first one.
        val second = topology.tracks.iterator()
        assertEquals(expected[0], second.next())
        assertEquals(expected[2], topology.tracks[2])
        assertEquals(expected[1], second.next())
        inputTracks.clear()
        inputTracks.add(ToolTrack(10, null, null, 1000.0))
        inputJunctions.add(ToolJunction(11, 10, .25, -1, -1))
        assertEquals(expected[1], first.next())
        assertEquals(expected[2], second.next())
        assertFalse(second.hasNext())
        assertEquals(expected[2], first.next())
        assertFalse(first.hasNext())
        assertFailsWith<NoSuchElementException> { first.next() }
        assertEquals(expected, topology.tracks.toList())
        for (index in expected.indices) {
            assertEquals(expected[index], topology.tracks[index])
            assertEquals(expected[index], topology.track(expected[index].id))
        }
        val backwards = topology.tracks.listIterator(topology.tracks.size)
        for (value in expected.asReversed()) assertEquals(value, backwards.previous())
        assertFalse(backwards.hasPrevious())
        assertEquals(listOf(100.0), topology.track(2)!!.junctionOffsetsM)
        assertEquals(listOf(10L), source.topology().tracks.map { it.id })
    }

    @Test fun iteratorCannotMutateValuesSharedWithLaterTraversals() {
        val topology = network(listOf(ToolTrack(1, null, 2, 100.0), ToolTrack(2, 1, null, 200.0))).topology()
        val expected = listOf(topology.track(1)!!, topology.track(2)!!)
        val iterator = topology.tracks.iterator()
        assertEquals(expected[0], iterator.next())
        if (iterator is MutableIterator<ToolRouteTrack>) {
            try { iterator.remove() } catch (_: UnsupportedOperationException) { }
        }
        assertEquals(expected, topology.tracks.toList())
        assertEquals(expected[0], topology.tracks[0])
        assertEquals(expected[1], topology.tracks[1])
        assertEquals(2, topology.tracks.size)
    }
}
