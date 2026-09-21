plugins { kotlin("multiplatform") version "2.2.20" }

// Maintainer verification against the SDK sources, independently of a packaged
// SDK or game hooks. This does not produce an installable mod distribution.
val mod = file(providers.gradleProperty("modProject").get())
require(mod.resolve("mod.json").isFile) { "modProject must contain mod.json" }
val sdk = rootDir.resolve("../..").canonicalFile
val host = when {
    System.getProperty("os.name").startsWith("Windows") -> "windows"
    System.getProperty("os.name") == "Linux" -> "linux"
    else -> error("Native mod verification currently supports Windows and Linux x64")
}
require(System.getProperty("os.arch") in setOf("amd64", "x86_64")) { "An x64 host is required" }
layout.buildDirectory = sdk.resolve("build/verification-kotlin-$host").let { layout.projectDirectory.dir(it.path) }
kotlin {
    val target = if (host == "windows") mingwX64("host") else linuxX64("host")
    target.binaries.sharedLib { baseName = "VerifiedModKotlin" }
    sourceSets {
        val hostMain by getting {
            kotlin.srcDirs(sdk.resolve("kotlin/src"), sdk.resolve("kotlin/native"), mod.resolve("src/main/kotlin"))
        }
        val hostTest by getting {
            kotlin.srcDir(mod.resolve("src/test/kotlin"))
            dependencies { implementation(kotlin("test")) }
        }
    }
}
val prepareTestAssets by tasks.registering(Sync::class) {
    from(mod.resolve("assets"))
    from(mod.resolve("imgs")) { into("imgs") }
    from(mod.resolve("config")) { into("config") }
    into(layout.buildDirectory.dir("test-assets"))
}
tasks.withType<org.jetbrains.kotlin.gradle.targets.native.tasks.KotlinNativeTest>().configureEach {
    dependsOn(prepareTestAssets)
    workingDir = layout.buildDirectory.dir("test-assets").get().asFile.absolutePath
}
