pluginManagement {
    val sdk = file(providers.gradleProperty("nrfSdkDir").get())
    val metadata = groovy.json.JsonSlurper().parse(sdk.resolve("sdk.json")) as Map<*, *>
    repositories { maven { url = uri(sdk.resolve("gradle-repository")) }; gradlePluginPortal(); mavenCentral() }
    plugins { id("fr.nimbyrails.mod") version (metadata["gradlePluginVersion"] as String) }
}
dependencyResolutionManagement { repositories { mavenCentral() } }
rootProject.name = "sdk-package-contract"
