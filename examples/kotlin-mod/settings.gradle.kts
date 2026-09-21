pluginManagement {
    val sdk = providers.gradleProperty("nrfSdkDir")
        .orElse(providers.environmentVariable("NRF_KOTLIN_SDK"))
        .orNull ?: error("Configurer nrfSdkDir ou NRF_KOTLIN_SDK vers le kit SDK Kotlin.")
    repositories {
        maven { url = uri(file(sdk).resolve("gradle-repository")) }
        gradlePluginPortal()
        mavenCentral()
    }
}

dependencyResolutionManagement { repositories { mavenCentral() } }
rootProject.name = "example-signals"
