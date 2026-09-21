pluginManagement { repositories { gradlePluginPortal(); mavenCentral() } }
dependencyResolutionManagement { repositories { mavenCentral() } }
rootProject.name = "nrf-kotlin-observer"
includeBuild(providers.gradleProperty("nrfSdkClientDir").orNull ?: "../../kotlin-client")
