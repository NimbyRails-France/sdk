package nimby.mod

import kotlin.test.*
import nimby.*

private enum class VisibilityAspect { Stop }
private enum class VisibilityReason { Test }

class CheckboxVisibilityContractTest {
    private fun type(boxes: List<Checkbox>) = SignalType("visibility", "Visibility", "visibility_images", boxes)
    private val mode = Checkbox("mode", "Semaphore mode", "")
    private val force = Checkbox("force", "Force square", "", visibleWhen = "mode")

    @Test fun positiveParentDependencyHasAnEmptyDefaultAndIsDistinctFromOnlyWhenEnabled() {
        assertEquals("", mode.visibleWhen)
        assertFalse(mode.defaultValue)
        assertEquals("mode", force.visibleWhen)
        assertFalse(force.onlyWhenEnabled)
        validateSignalTypes(listOf(type(listOf(mode, force))))
    }

    @Test fun bothBuildersPreserveTheDependency() {
        val stop = Indication(VisibilityAspect.Stop, VisibilityReason.Test)
        val model = signalModel("visibility", "Visibility", "visibility_images", stop) {
            checkbox("mode", "Mode")
            checkbox("force", "Force", visibleWhen = "mode")
            rules { stop }; images { "stop.svg" }
        }
        assertEquals("mode", model.type.checkboxes.last().visibleWhen)
        val mod = signalMod("visibility-mod", "Visibility", stop) {
            signal("visibility", "Visibility", "visibility_images") {
                checkbox("mode", "Mode")
                checkbox("force", "Force", visibleWhen = "mode")
                rules { stop }
            }
            images { "stop.svg" }
        }
        assertEquals("mode", mod.signalTypes.single().checkboxes.last().visibleWhen)
    }

    @Test fun unknownSelfAndCyclicParentsAreRejected() {
        for (boxes in listOf(
            listOf(mode, force.copy(visibleWhen = "missing")),
            listOf(mode, force.copy(visibleWhen = "force")),
            listOf(mode.copy(visibleWhen = "force"), force),
            listOf(mode, force.copy(visibleWhen = "é".repeat(65))),
            listOf(mode, force.copy(visibleWhen = "mode\u0000")),
        )) assertFailsWith<IllegalArgumentException> { validateSignalTypes(listOf(type(boxes))) }
    }

    @Test fun theLargestAcceptedAcyclicDependencyChainIsBounded() {
        val boxes = List(64) { index ->
            Checkbox("box$index", "Box $index", "", visibleWhen = if(index == 0) "" else "box${index - 1}")
        }
        validateSignalTypes(listOf(type(boxes)))
        assertFailsWith<IllegalArgumentException> {
            validateSignalTypes(listOf(type(boxes + Checkbox("extra", "Extra", ""))))
        }
        assertFailsWith<IllegalArgumentException> {
            validateSignalTypes(listOf(type(boxes.mapIndexed { index, box -> if(index == 0) box.copy(visibleWhen = "box63") else box })))
        }
    }
}
