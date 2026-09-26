package fr.nimbyrails.gradle.platform.windows

import fr.nimbyrails.gradle.NativePlatform

/** Windows x64 kit naming and execution. Consumers do not need CMake or C++. */
final class WindowsPlatform extends NativePlatform {
    WindowsPlatform() {
        super('mingw_x64', 'windows', 'windows-x64', '.dll', 'kotlin_loader_test.exe',
            'build/gradle', ['NimbyRailsFranceSDK.dll', 'libwinpthread-1.dll'])
    }
    Object createTarget(Object kotlin) { kotlin.mingwX64('windows') }
    boolean executableOnHost() {
        System.getProperty('os.arch') in ['amd64', 'x86_64'] &&
            System.getProperty('os.name').startsWith('Windows')
    }
}
