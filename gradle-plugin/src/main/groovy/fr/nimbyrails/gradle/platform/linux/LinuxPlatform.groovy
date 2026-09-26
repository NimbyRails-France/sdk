package fr.nimbyrails.gradle.platform.linux

import fr.nimbyrails.gradle.NativePlatform
import org.gradle.api.GradleException

/** Linux kit contract; this does not claim Windows-equivalent game capabilities. */
final class LinuxPlatform extends NativePlatform {
    LinuxPlatform() {
        super('linux_x64', 'linux', 'linux-x64', '.so', 'kotlin_loader_test',
            'build/gradle-linux', ['NimbyRailsFranceSDK.so'])
    }
    Object createTarget(Object kotlin) { kotlin.linuxX64('linux') }
    boolean executableOnHost() {
        System.getProperty('os.arch') in ['amd64', 'x86_64'] && System.getProperty('os.name') == 'Linux'
    }
    void configureVerification(Object task, File directory) {
        String inherited = System.getenv('LD_LIBRARY_PATH')
        task.environment('LD_LIBRARY_PATH', directory.absolutePath + (inherited ? ':' + inherited : ''))
    }
    void validateVerification(File directory) {
        if (!new File(directory, loaderTest).canExecute())
            throw new GradleException('The SDK loader test is not executable; extract the Linux SDK with executable permissions preserved.')
    }
}
