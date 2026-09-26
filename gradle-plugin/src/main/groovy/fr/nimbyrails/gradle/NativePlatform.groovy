package fr.nimbyrails.gradle

import org.gradle.api.GradleException
import fr.nimbyrails.gradle.platform.windows.WindowsPlatform
import fr.nimbyrails.gradle.platform.linux.LinuxPlatform

/** Names in a precompiled SDK kit, shared by staging and native verification. */
abstract class NativePlatform {
    final String target, sourceSet, id, extension, loaderTest, buildDirectory
    final List<String> libraries

    protected NativePlatform(String target, String sourceSet, String id, String extension,
                           String loaderTest, String buildDirectory, List<String> libraries) {
        this.target = target; this.sourceSet = sourceSet; this.id = id
        this.extension = extension; this.loaderTest = loaderTest
        this.buildDirectory = buildDirectory; this.libraries = libraries.asImmutable()
    }

    static NativePlatform forTarget(Object target) {
        switch (target) {
            case 'mingw_x64': return new WindowsPlatform()
            case 'linux_x64': return new LinuxPlatform()
            default: throw new GradleException("Unsupported Kotlin SDK target: ${target}")
        }
    }

    String getAdapter() { "NimbyKotlinMod${extension}" }
    List<String> requiredFiles() {
        ['sdk.json', 'klib/nimby-mod-api.klib', 'bridge/Exports.kt', "bin/${adapter}", "bin/${loaderTest}"] +
            libraries.collect { "bin/${it}" }
    }
    /** Creates only the kit's declared target; never guesses from the host. */
    abstract Object createTarget(Object kotlin)
    abstract boolean executableOnHost()
    /** Configure native loader lookup without modifying the parent environment. */
    void configureVerification(Object task, File directory) {}
    /** Called at execution time, after verification files have been staged. */
    void validateVerification(File directory) {}
}
