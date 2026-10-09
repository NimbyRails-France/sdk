plugins {
    kotlin("jvm") version "2.2.20"
    `maven-publish`
}
group = "fr.nimbyrails"
version = "0.9.0-alpha.3"
layout.buildDirectory = layout.projectDirectory.dir("build/${System.getProperty("os.name").lowercase().replace(' ', '-')}")
kotlin { jvmToolchain(21) }
dependencies {
    implementation("net.java.dev.jna:jna:5.17.0")
    testImplementation(kotlin("test"))
}
publishing { publications { create<MavenPublication>("client") { from(components["java"]) } } }
run {
    val windows = System.getProperty("os.name").startsWith("Windows")
    val mac = System.getProperty("os.name").startsWith("Mac")
    val fixture = layout.buildDirectory.file("native/" + if (windows) "observation_fixture.dll" else if (mac) "libobservation_fixture.dylib" else "libobservation_fixture.so")
    val nativeFixture by tasks.registering(Exec::class) {
        inputs.file("src/test/native/observation_fixture.c")
        inputs.dir("../include/nimby/detail")
        outputs.file(fixture)
        doFirst { fixture.get().asFile.parentFile.mkdirs() }
        val compiler = providers.gradleProperty("nrfTestCc").orElse(if (windows) "gcc" else "cc")
        commandLine(listOf(compiler.get(), if (mac) "-dynamiclib" else "-shared") +
            (if (windows) emptyList() else listOf("-fPIC")) +
            listOf("-Wall", "-Wextra", "-Werror", "-I../include", "src/test/native/observation_fixture.c", "-o", fixture.get().asFile.absolutePath))
    }
    tasks.test { dependsOn(nativeFixture); systemProperty("nrf.fixture", fixture.get().asFile.absolutePath) }
    // A second library really lacks the additive export: absence is not the same
    // as an existing function returning DATA_UNAVAILABLE.
    val legacy = layout.buildDirectory.file("native/" + if (windows) "legacy_fixture.dll" else if (mac) "liblegacy_fixture.dylib" else "liblegacy_fixture.so")
    val legacyFixture by tasks.registering(Exec::class) {
        inputs.file("src/test/native/observation_fixture.c")
        inputs.dir("../include/nimby/detail")
        outputs.file(legacy)
        doFirst { legacy.get().asFile.parentFile.mkdirs() }
        val compiler = providers.gradleProperty("nrfTestCc").orElse(if (windows) "gcc" else "cc")
        commandLine(listOf(compiler.get(), if (mac) "-dynamiclib" else "-shared") +
            (if (windows) emptyList() else listOf("-fPIC")) +
            listOf("-Wall", "-Wextra", "-Werror", "-DNRF_FIXTURE_LEGACY_METRICS", "-I../include", "src/test/native/observation_fixture.c", "-o", legacy.get().asFile.absolutePath))
    }
    tasks.test { dependsOn(legacyFixture); systemProperty("nrf.fixture.legacy", legacy.get().asFile.absolutePath) }
}
