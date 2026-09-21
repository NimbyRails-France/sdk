# Example Kotlin mod

Copy this directory to your own workspace. Install JDK 21 and extract the NRF
Kotlin SDK. Set `NRF_KOTLIN_SDK` to that SDK directory, or pass
`-PnrfSdkDir=C:/SDK/NimbyKotlin` to Gradle.

Open `build.gradle.kts` in IntelliJ IDEA and run `build`, or use:

```text
gradlew.bat build
```

Edit `mod.json` to set your mod identity and compatibility, and implement
`nimby.mod.createMod()` in `src/main/kotlin`. Tests belong in `src/test/kotlin`.
The SDK plugin provides compilation, native checks and packaging.

The ZIP and the local Hub descriptor are written to `build/gradle/distributions`.
Add this source directory in the Hub's developer profile, select the Kotlin SDK,
then build and prepare the local mod. Installing and switching profiles belong
to the Hub; Gradle never changes the game installation.

This example demonstrates the API and packaging only. It does not implement
real railway signalling, change train driving, or certify a gameplay scenario.
