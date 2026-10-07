// Compile-only maintainer fixture against a COPY of the installed client.
// No game connection, native loading or redistributable starter project.
plugins { kotlin("jvm") version "2.2.20" }
kotlin { jvmToolchain(21) }
dependencies { implementation("fr.nimbyrails:nimby-observation-client:0.9.0-alpha.1") }
