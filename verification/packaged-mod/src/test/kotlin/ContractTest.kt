package nimby.mod

import kotlin.test.*

class ContractTest {
    @Test fun installedApiBuildsAService() {
        assertEquals(listOf("contract.v1"), createMod().services)
    }
}
