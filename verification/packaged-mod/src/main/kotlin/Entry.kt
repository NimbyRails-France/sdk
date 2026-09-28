package nimby.mod

import nimby.*

fun createMod() = toolMod(modInfo) {
    metadata(author = "NRF", description = "SDK packaging contract")
    service("contract.v1") { log("Package service invoked") }
}
