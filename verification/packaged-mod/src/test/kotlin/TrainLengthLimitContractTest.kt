package nimby.contract

import kotlin.test.*
import nimby.*
import nimby.internal.ModOptionsAccess
import nimby.internal.TrainLengthLimitAccess

class TrainLengthLimitContractTest {
    private fun lengthOption(id: String = "maxLengthMeters") = IntegerOption(
        id, "Longueur maximale (m)", 850, minimumTrainLengthMeters, maximumTrainLengthMeters,
    )
    private fun ToolModBuilder.editor(option: IntegerOption) = trainEditor {
        maximumLength(option, "Trop long", "Longueur indisponible", "Vérification impossible")
    }

    @Test fun aDeclarationOnlyModUsesMetersWithoutWindowsOrServices() {
        val option = lengthOption()
        val mod = toolMod("long-trains", "Trains longs") {
            metadata("Example", "Longueur configurable")
            options(option)
            editor(option)
        }
        val limit = assertNotNull(mod.trainEditor).maximumLength
        assertEquals(850, limit.maximumLengthMeters)
        assertEquals("maxLengthMeters", limit.optionId)
        assertSame(option, mod.options.single())
        assertTrue(mod.windows.isEmpty())
        assertTrue(mod.services.isEmpty())
        assertFalse(TrainLengthLimitAccess(mod).hasTickCallback)
    }

    @Test fun declaringATickKeepsTheCallbackEvenWithNoWindowsOrServices() {
        val option = lengthOption()
        val mod = toolMod("ticked", "Ticked") {
            options(option)
            editor(option)
            onTick { }
        }
        assertTrue(TrainLengthLimitAccess(mod).hasTickCallback)
        assertTrue(mod.windows.isEmpty())
        assertTrue(mod.services.isEmpty())
        val legacy = toolMod("legacy", "Legacy") { service("action") { } }
        assertNull(TrainLengthLimitAccess(legacy).declaration)
        for (index in 0..2) assertEquals("", TrainLengthLimitAccess(legacy).message(index))
    }

    @Test fun lengthFollowsOnlyValidatedOptionSnapshots() {
        val option = lengthOption()
        val mod = toolMod("long-trains", "Trains longs") { options(option); editor(option) }
        val limit = assertNotNull(mod.trainEditor).maximumLength
        val access = ModOptionsAccess(mod.options)
        for (meters in listOf(minimumTrainLengthMeters, 1200, maximumTrainLengthMeters)) {
            access.apply("$meters\u0000".encodeToByteArray(), 1)
            assertEquals(meters, limit.maximumLengthMeters)
        }
        for (invalid in listOf("0", "10001", "81.0", "-1", "NaN")) {
            assertFailsWith<IllegalArgumentException> { access.apply("$invalid\u0000".encodeToByteArray(), 1) }
            assertEquals(maximumTrainLengthMeters, limit.maximumLengthMeters)
        }
        assertEquals("850", access.metadata(0, 3))
    }

    @Test fun bindingRequiresTheDeclaredOptionInstanceEvenWhenIdsMatch() {
        val declared = lengthOption()
        val other = lengthOption()
        assertFailsWith<IllegalArgumentException> {
            toolMod("missing", "Missing") { editor(declared) }
        }
        assertFailsWith<IllegalArgumentException> {
            toolMod("mismatch", "Mismatch") { options(declared); editor(other) }
        }
        val mod = toolMod("ordered", "Ordered") { editor(declared); options(declared) }
        assertEquals(850, assertNotNull(mod.trainEditor).maximumLength.maximumLengthMeters)
    }

    @Test fun duplicatePoliciesAndRangesOutsideSdkBoundsAreRejected() {
        val option = lengthOption()
        assertFailsWith<IllegalArgumentException> {
            toolMod("duplicate", "Duplicate") { options(option); editor(option); editor(option) }
        }
        assertFailsWith<IllegalArgumentException> {
            toolMod("duplicateRule", "Duplicate") {
                options(option)
                trainEditor {
                    maximumLength(option, "Long", "Missing", "Unavailable")
                    maximumLength(option, "Long", "Missing", "Unavailable")
                }
            }
        }
        assertFailsWith<IllegalArgumentException> {
            toolMod("missingRule", "Missing") { options(option); trainEditor { } }
        }
        for ((minimum, maximum) in listOf(0 to 10000, 1 to 10001, -1 to 850)) {
            val unsupported = IntegerOption("length", "Length", 850, minimum, maximum)
            assertFailsWith<IllegalArgumentException> {
                toolMod("range", "Range") { options(unsupported); editor(unsupported) }
            }
        }
        assertFailsWith<IllegalArgumentException> { toolMod("empty", "Empty") {} }
    }

    @Test fun independentModsKeepIndependentLengthPreferences() {
        val firstOption = lengthOption("firstLength")
        val secondOption = lengthOption("secondLength")
        val first = toolMod("first", "First") { options(firstOption); editor(firstOption) }
        val second = toolMod("second", "Second") { options(secondOption); editor(secondOption) }
        ModOptionsAccess(first.options).apply("1500\u0000".encodeToByteArray(), 1)
        assertEquals(1500, assertNotNull(first.trainEditor).maximumLength.maximumLengthMeters)
        assertEquals(850, assertNotNull(second.trainEditor).maximumLength.maximumLengthMeters)
    }

    @Test fun customGameModCannotBindAnotherOptionWithTheSameId() {
        val option = lengthOption()
        val donor = toolMod("donor", "Donor") { options(option); editor(option) }
        val mismatched = object : GameMod() {
            override val id = "custom"
            override val title = "Custom"
            override val options = listOf(lengthOption())
            override val trainEditor = donor.trainEditor
        }
        assertFailsWith<IllegalArgumentException> { TrainLengthLimitAccess(mismatched) }
        assertSame(donor.trainEditor!!.maximumLength, TrainLengthLimitAccess(donor).declaration)
        assertNull(TrainLengthLimitAccess(object : GameMod() {
            override val id = "plain"
            override val title = "Plain"
        }).declaration)
    }

    @Test fun translatedMessagesRemainStaticReferencesAfterOptionChanges() {
        val option = lengthOption()
        var builder: TrainEditorBuilder? = null
        val mod = toolMod("translated", "Translated") {
            options(option)
            trainEditor {
                builder = this
                maximumLength(option, tr("length.exceeded"), tr("length.unavailable"), tr("composition.unavailable"))
            }
        }
        val editor = assertNotNull(mod.trainEditor)
        val access = TrainLengthLimitAccess(mod)
        val expected = listOf(tr("length.exceeded"), tr("length.unavailable"), tr("composition.unavailable"))
        assertEquals(expected, (0..2).map(access::message))
        ModOptionsAccess(mod.options).apply("1500\u0000".encodeToByteArray(), 1)
        assertSame(editor, mod.trainEditor)
        assertEquals(expected, (0..2).map(access::message))
        assertFailsWith<IllegalArgumentException> {
            assertNotNull(builder).maximumLength(option, "Replacement", "Replacement", "Replacement")
        }
        assertEquals(expected, (0..2).map(access::message))
        for (index in listOf(-1, 3)) assertFailsWith<IllegalArgumentException> { access.message(index) }
    }

    @Test fun everyRejectionNeedsAValidBoundedStaticMessage() {
        val invalidMessages = listOf(
            "", "   ", "bad\u0000message", "bad\u0001message", "é".repeat(513),
            tr("length.exceeded", "meters" to 850), "\u001eNRF:[\"length.exceeded\",{}]trailing",
        )
        for (invalid in invalidMessages) for (index in 0..2) {
            val option = lengthOption()
            val messages = mutableListOf("Too long", "Missing length", "Cannot verify")
            messages[index] = invalid
            assertFailsWith<IllegalArgumentException> {
                toolMod("invalidMessage", "Invalid") {
                    options(option)
                    trainEditor { maximumLength(option, messages[0], messages[1], messages[2]) }
                }
            }
        }
        val option = lengthOption()
        val longest = "é".repeat(512)
        val valid = toolMod("maxMessage", "Maximum") {
            options(option)
            trainEditor { maximumLength(option, longest, "Missing length", "Cannot verify") }
        }
        assertEquals(longest, TrainLengthLimitAccess(valid).message(0))
    }
}
