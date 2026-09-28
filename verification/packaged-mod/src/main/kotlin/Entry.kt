package nimby.mod

import nimby.*

fun createMod() = toolMod("sdk-contract", "SDK package contract") {
    service("contract.v1") { log("Package service invoked") }
}
