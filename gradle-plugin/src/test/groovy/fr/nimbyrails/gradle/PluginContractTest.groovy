package fr.nimbyrails.gradle

import groovy.json.JsonOutput
import groovy.json.JsonSlurper
import java.security.MessageDigest
import org.gradle.testkit.runner.GradleRunner
import org.junit.*
import org.junit.rules.TemporaryFolder
import static org.junit.Assert.*

class PluginContractTest {
    @Rule public TemporaryFolder temp = new TemporaryFolder()

    private File project(String target = 'mingw_x64') {
        File root = temp.newFolder('independent project')
        new File(root, 'settings.gradle').text = "rootProject.name = 'independent'"
        new File(root, 'build.gradle').text = "plugins { id 'fr.nimbyrails.mod' }"
        new File(root, 'mod.json').text = JsonOutput.toJson([id: 'independent', name: 'Independent', modId: 'Independent',
            module: 'IndependentMod', version: '1.0.0', language: 'kotlin-native', sdkMin: '0.8.0', sdkMaxExclusive: '0.9.0', gameSha256: ['a' * 64]])
        File sdk = new File(root, 'sdk with spaces')
        NativePlatform.forTarget(target).requiredFiles().each {
            File file = new File(sdk, it); file.parentFile.mkdirs(); file.text = 'fixture'
        }
        new File(sdk, 'sdk.json').text = JsonOutput.toJson([format: 1, target: target, kotlinVersion: '2.2.20',
            sdkVersion: '0.8.0', gradlePluginVersion: NrfModPlugin.pluginVersion(), gameSha256: ['a' * 64]])
        root
    }
    private GradleRunner runner(File root, String... tasks) {
        GradleRunner.create().withProjectDir(root).withPluginClasspath()
            .withArguments((tasks as List) + ['-PnrfSdkDir=sdk with spaces', '--stacktrace'])
    }
    @Test void generatesLoaderManifestWithoutNativeBuildOrInstallation() {
        File root = project()
        def result = runner(root, 'generateModManifest').build()
        assertEquals('[NRFMod]\nlibrary=IndependentMod.dll\n', new File(root, 'build/gradle/generated/mod/nrf-mod.ini').text)
        assertFalse(result.tasks.any { it.path.contains('compileKotlin') || it.path.contains('install') })
    }
    @Test void failsEarlyOnIncompatibleSdk() {
        File root = project()
        File metadata = new File(root, 'sdk with spaces/sdk.json')
        metadata.text = metadata.text.replace('"sdkVersion":"0.8.0"', '"sdkVersion":"0.9.0"')
        assertTrue(runner(root, 'tasks').buildAndFail().output.contains('outside [0.8.0, 0.9.0)'))
    }

    @Test void rejectsPluginFromAnotherKit() {
        File root = project()
        File metadata = new File(root, 'sdk with spaces/sdk.json')
        def data = new JsonSlurper().parse(metadata)
        data.gradlePluginVersion = '0.0.1'
        metadata.text = JsonOutput.toJson(data)
        assertTrue(runner(root, 'tasks').buildAndFail().output.contains('Select the plugin supplied by sdk.json'))
    }
    @Test void packageDependsOnTestsAndNativeVerification() {
        def output = runner(project(), 'packageMod', '--dry-run').build().output
        assertTrue(output.contains(':windowsTest SKIPPED'))
        assertTrue(output.contains(':verifyNativeMod SKIPPED'))
        assertTrue(output.contains(':hubManifest SKIPPED'))
    }
    @Test void linuxManifestUsesSharedObjectWithoutNativeBuild() {
        File root = project('linux_x64')
        def result = runner(root, 'generateModManifest').build()
        assertEquals('[NRFMod]\nlibrary=IndependentMod.so\n', new File(root, 'build/gradle-linux/generated/mod/nrf-mod.ini').text)
        assertFalse(result.tasks.any { it.path.contains('compileKotlin') || it.path.contains('install') })
    }
    @Test void linuxPackageRequiresNativeTestsAndVerification() {
        def output = runner(project('linux_x64'), 'packageMod', '--dry-run').build().output
        assertTrue(output.contains(':linuxTest SKIPPED'))
        assertTrue(output.contains(':verifyNativeMod SKIPPED'))
        assertTrue(output.contains(':hubManifest SKIPPED'))
    }
    @Test void rejectsWindowsAdapterInLinuxKit() {
        File root = project('linux_x64')
        assertTrue(new File(root, 'sdk with spaces/bin/NimbyKotlinMod.so').delete())
        new File(root, 'sdk with spaces/bin/NimbyKotlinMod.dll').text = 'wrong platform'
        assertTrue(runner(root, 'tasks').buildAndFail().output.contains('missing bin/NimbyKotlinMod.so'))
    }
    @Test void releaseManifestKeepsChannelPlatformAndExactArchiveIdentity() {
        File root = project('linux_x64')
        File mod = new File(root, 'mod.json')
        mod.text = mod.text.replace('"version":"1.0.0"', '"version":"1.0.0-beta.2"')
        File archive = new File(root, 'build/gradle-linux/distributions/Independent-1.0.0-beta.2-linux-x64.zip')
        archive.parentFile.mkdirs()
        archive.bytes = [0, 1, 2, 127, -1] as byte[]
        String base = 'https://github.com/NimbyRails-France/independent/releases/download/v1.0.0-beta.2'
        runner(root, 'hubManifest', '-x', 'modArchive', "-PreleaseBaseUrl=${base}").build()
        File canonical = new File(archive.parentFile, 'project-linux-x64.json')
        assertEquals(canonical.text, new File(archive.parentFile, 'project.json').text)
        def manifest = new JsonSlurper().parse(canonical)
        assertEquals('beta', manifest.channel)
        assertEquals('1.0.0-beta.2', manifest.version)
        assertEquals('linux-x64', manifest.platform)
        assertEquals('IndependentMod.so', manifest.module)
        assertEquals(base + '/' + archive.name, manifest.url)
        assertEquals(archive.length(), manifest.size as long)
        assertEquals(MessageDigest.getInstance('SHA-256').digest(archive.bytes).encodeHex().toString(), manifest.sha256)
        assertTrue(runner(root, 'hubManifest', '-x', 'modArchive',
            '-PreleaseBaseUrl=https://github.com/NimbyRails-France/independent/releases/download/v1.0.0').buildAndFail()
            .output.contains('releaseBaseUrl must be ' + base))
    }
}
