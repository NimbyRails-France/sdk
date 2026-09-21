package fr.nimbyrails.gradle

import org.gradle.api.GradleException

/** Names in a precompiled SDK kit, shared by staging and native verification. */
final class NativePlatform {
    final String target, sourceSet, id, extension, loaderTest, buildDirectory
    final List<String> libraries

    private NativePlatform(String target, String sourceSet, String id, String extension,
                           String loaderTest, String buildDirectory, List<String> libraries) {
        this.target = target; this.sourceSet = sourceSet; this.id = id
        this.extension = extension; this.loaderTest = loaderTest
        this.buildDirectory = buildDirectory; this.libraries = libraries.asImmutable()
    }

    static NativePlatform forTarget(Object target) {
        switch (target) {
            case 'mingw_x64': return new NativePlatform('mingw_x64', 'windows', 'windows-x64', '.dll',
                'kotlin_loader_test.exe', 'build/gradle', ['NimbyRailsFranceSDK.dll', 'libwinpthread-1.dll'])
            case 'linux_x64': return new NativePlatform('linux_x64', 'linux', 'linux-x64', '.so',
                'kotlin_loader_test', 'build/gradle-linux', ['NimbyRailsFranceSDK.so'])
            default: throw new GradleException("Unsupported Kotlin SDK target: ${target}")
        }
    }

    String getAdapter() { "NimbyKotlinMod${extension}" }
    List<String> requiredFiles() {
        ['sdk.json', 'klib/nimby-mod-api.klib', 'bridge/Exports.kt', "bin/${adapter}", "bin/${loaderTest}"] +
            libraries.collect { "bin/${it}" }
    }
    boolean executableOnHost() {
        String os = System.getProperty('os.name')
        String arch = System.getProperty('os.arch')
        (arch in ['amd64', 'x86_64']) &&
            (sourceSet == 'windows' ? os.startsWith('Windows') : os == 'Linux')
    }
}
