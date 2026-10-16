package nimby.mod

import kotlin.test.*
import nimby.*

private enum class RouteAspect { Stop }
private enum class RouteReason { Test }

class SelectedSignalRouteContractTest {
    private val train = 0x5000000010001L
    private val otherTrain = 0x5000000020001L
    private val boundary = 0x8000000030001L
    private val marker = SignalRouteMarker(0x8000000040001L, "utility_markers", 32)
    private val stop = Indication(RouteAspect.Stop, RouteReason.Test)

    private fun complete(destination: RouteDestination = RouteDestination.NativeBoundary,
                         occupation: Occupancy = Occupancy.Clear,
                         markers: List<SignalRouteMarker> = listOf(marker)) =
        SelectedSignalRoute.observed(train, boundary, destination, occupation, markers)

    @Test fun unknownNeverClaimsABoundaryOrAnEmptyClearRoute() {
        val unknown = SelectedSignalRoute.Unknown
        assertEquals(RouteDestination.Unknown, unknown.destination)
        assertFalse(unknown.complete)
        assertEquals(Occupancy.Unknown, unknown.occupation)
        assertEquals(0, unknown.boundarySignalId)
        assertTrue(unknown.markers.isEmpty())
        assertSame(unknown, Observation(fresh = true, approachingTrain = train).selectedRoute)
        val partial = SelectedSignalRoute.observed(train, 0, RouteDestination.Unknown,
            Occupancy.Occupied, listOf(marker), complete = false)
        assertEquals(Occupancy.Occupied, partial.occupation)
        assertFalse(partial.complete)
        assertFailsWith<IllegalArgumentException> {
            SelectedSignalRoute.observed(train, 0, RouteDestination.Unknown, Occupancy.Clear, emptyList(), false)
        }
    }

    @Test fun knownDestinationsNeedACompleteQualifiedBoundary() {
        for (destination in listOf(RouteDestination.ProtectingSignal, RouteDestination.NativeBoundary)) {
            val observed = complete(destination)
            assertEquals(destination, observed.destination)
            assertEquals(boundary, observed.boundarySignalId)
            assertTrue(observed.complete)
            assertFailsWith<IllegalArgumentException> {
                SelectedSignalRoute.observed(train, boundary, destination, Occupancy.Occupied, emptyList(), false)
            }
            for (id in listOf(0L, -1L, train, 0x8000000000000L)) {
                assertFailsWith<IllegalArgumentException> {
                    SelectedSignalRoute.observed(train, id, destination, Occupancy.Unknown, emptyList())
                }
            }
        }
        assertFailsWith<IllegalArgumentException> {
            SelectedSignalRoute.observed(train, boundary, RouteDestination.Unknown, Occupancy.Unknown, emptyList(), false)
        }
        assertFailsWith<IllegalArgumentException> {
            SelectedSignalRoute.observed(train, 0, RouteDestination.Unknown, Occupancy.Unknown, emptyList())
        }
    }

    @Test fun staleMissingOrDifferentApproachMasksTheWholeRoute() {
        val route = complete()
        val current = Observation(fresh = true, approachingTrain = train, observedRoute = route)
        assertSame(route, current.selectedRoute)
        // The general block can lack a PATH boundary while this selected route
        // reaches a separately qualified native boundary.
        assertSame(route, current.copy(routeKnown = false).selectedRoute)
        assertSame(SelectedSignalRoute.Unknown, current.copy(fresh = false).selectedRoute)
        assertSame(SelectedSignalRoute.Unknown, current.copy(approachingTrain = null).selectedRoute)
        assertSame(SelectedSignalRoute.Unknown, current.copy(approachingTrain = otherTrain).selectedRoute)
        assertSame(route, current.selectedRoute)
    }

    @Test fun fullIdentitiesAndCopiedMarkerOrderArePreserved() {
        val second = marker.copy(id = 0x8000000050001L, textureState = 33)
        val markers = mutableListOf(second, marker)
        val observed = complete(markers = markers)
        markers.clear()
        assertEquals(listOf(second, marker), observed.markers)
        assertEquals(train, observed.trainId)
        assertNotEquals(otherTrain, observed.trainId)
        for (invalid in listOf(0L, -1L, 1L, boundary, 0x5000000000000L)) {
            assertFailsWith<IllegalArgumentException> {
                SelectedSignalRoute.observed(invalid, boundary, RouteDestination.NativeBoundary,
                    Occupancy.Clear, emptyList())
            }
        }
    }

    @Test fun markerIdentitiesStatesAndResourcesAreBoundedBeforeTransport() {
        for (invalid in listOf(0L, -1L, 1L, train, 0x8000000000000L)) {
            assertFailsWith<IllegalArgumentException> { complete(markers = listOf(marker.copy(id = invalid))) }
        }
        assertFailsWith<IllegalArgumentException> { complete(markers = listOf(marker, marker)) }
        for (state in listOf(-1, 256, Int.MAX_VALUE)) {
            assertFailsWith<IllegalArgumentException> { complete(markers = listOf(marker.copy(textureState = state))) }
        }
        for (catalogue in listOf("", "  ", "a".repeat(257), "é", "a\u0000", "a\n")) {
            assertFailsWith<IllegalArgumentException> { complete(markers = listOf(marker.copy(textureSet = catalogue))) }
        }
        val bounded = List(128) { marker.copy(id = marker.id + it, textureSet = "a".repeat(256), textureState = 255) }
        assertEquals(bounded, complete(markers = bounded).markers)
        assertFailsWith<IllegalArgumentException> { complete(markers = bounded + marker.copy(id = boundary)) }
    }

    @Test fun observationIsOptInAndRequiresApproachInBothDeclarations() {
        val default = signalModel("route.default", "Route", "default_images", stop) {
            rules { stop }; images { "stop.svg" }
        }
        assertFalse(default.type.observeRoute)
        val declared = signalModel("route.enabled", "Route", "enabled_images", stop) {
            observeApproach(2); observeRoute(); rules { stop }; images { "stop.svg" }
        }
        signalMod("route-contract", "Route") { signal(declared) }
        assertTrue(declared.type.observeRoute)
        assertEquals(2, declared.type.approachBlocks)
        val invalid = signalModel("route.invalid", "Route", "invalid_images", stop) {
            observeRoute(); rules { stop }; images { "stop.svg" }
        }
        assertFailsWith<IllegalArgumentException> { signalMod("route-invalid", "Route") { signal(invalid) } }
        val legacy = signalMod("route-legacy", "Route", stop) {
            signal("route.legacy", "Route", "legacy_images") {
                observeRoute(); observeApproach(2); rules { stop }
            }
            images { "stop.svg" }
        }
        assertTrue(legacy.signalTypes.single().observeRoute)
        assertFailsWith<IllegalArgumentException> {
            signalMod("route-invalid-legacy", "Route", stop) {
                signal("route.invalid", "Route", "invalid_images") { observeRoute(); rules { stop } }
                images { "stop.svg" }
            }
        }
    }

    @Test fun ruleContextsAndUnavailableSettingsCannotReuseAnOldRoute() {
        var seen: SelectedSignalRoute? = null
        val model = signalModel("route.context", "Route", "context_images", stop) {
            observeApproach(2); observeRoute()
            rules { seen = selectedRoute; stop }
            images { "stop.svg" }
        }
        val mod = signalMod("route-context", "Route") { signal(model) }
        val route = complete()
        val observation = Observation(fresh = true, approachingTrain = train, observedRoute = route)
        val signal = Signal(type = model.type.id, observation = observation)
        mod.decide(signal, null)
        assertSame(route, seen)
        mod.decide(signal.copy(settingsStatus = SettingsStatus.Unavailable), null)
        assertSame(SelectedSignalRoute.Unknown, seen)
        mod.decide(signal.copy(observation = observation.copy(approachingTrain = 1)), null)
        assertSame(SelectedSignalRoute.Unknown, seen)
        val legacy = signalMod("route-legacy-context", "Route", stop) {
            signal("route.legacy", "Route", "legacy_images") {
                observeApproach(2); observeRoute(); rules { seen = selectedRoute; stop }
            }
            images { "stop.svg" }
        }
        legacy.decide(signal.copy(type = "route.legacy"), null)
        assertSame(route, seen)
        legacy.decide(signal.copy(type = "route.legacy", observation = observation.copy(fresh = false)), null)
        assertSame(SelectedSignalRoute.Unknown, seen)
    }
}
