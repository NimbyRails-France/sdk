package fr.nimbyrails.gradle

import groovy.json.JsonOutput
import groovy.json.JsonSlurper
import org.gradle.api.*
import org.gradle.api.file.DuplicatesStrategy
import org.gradle.api.tasks.*
import org.gradle.api.tasks.bundling.Zip
import org.jetbrains.kotlin.gradle.dsl.KotlinMultiplatformExtension
import org.jetbrains.kotlin.gradle.plugin.mpp.NativeBuildType
import java.security.MessageDigest

/** Shared build contract. It never installs into the game or publishes a release. */
class NrfModPlugin implements Plugin<Project> {
    static String pluginVersion() {
        Properties metadata = new Properties()
        NrfModPlugin.getResourceAsStream('/nrf-plugin.properties').withCloseable { metadata.load(it) }
        metadata.getProperty('version')
    }
    void apply(Project p) {
        Map mod = ModManifest.read(p.file('mod.json'))
        String sdkPath = p.providers.gradleProperty('nrfSdkDir').orElse(p.providers.environmentVariable('NRF_KOTLIN_SDK')).orNull
        if (!sdkPath) throw new GradleException('Set nrfSdkDir or NRF_KOTLIN_SDK to the extracted Kotlin SDK.')
        File sdk = p.file(sdkPath)
        if (!new File(sdk, 'sdk.json').isFile()) throw new GradleException("Incomplete Kotlin SDK: missing sdk.json in ${sdk}")
        Map metadata = new JsonSlurper().parseText(new File(sdk, 'sdk.json').getText('UTF-8').replaceFirst('^\uFEFF', '')) as Map
        NativePlatform platform = NativePlatform.forTarget(metadata.target)
        // The VPS cross-compiles Windows binaries. Wine is an explicit CI
        // runner, never an implicit replacement for tests on a Windows host.
        String wineRunner = p.providers.gradleProperty('nrfWineRunner').orNull
        if (wineRunner && (platform.id != 'windows-x64' || !System.getProperty('os.name').startsWith('Linux')))
            throw new GradleException('nrfWineRunner is only for Windows tests on a Linux CI worker.')
        File runner = wineRunner ? p.file(wineRunner) : null
        if (runner && !runner.isFile()) throw new GradleException('nrfWineRunner does not exist.')
        platform.requiredFiles().each {
            if (!new File(sdk, it).isFile()) throw new GradleException("Incomplete Kotlin SDK: missing ${it} in ${sdk}")
        }
        if (metadata.format != 1 || metadata.kotlinVersion != '2.2.20' || metadata.gradlePluginVersion != pluginVersion())
            throw new GradleException("Incompatible Kotlin SDK: expected format 1, Kotlin 2.2.20, Gradle plugin ${pluginVersion()}. Select the plugin supplied by sdk.json in pluginManagement.")
        ModManifest.check(metadata.sdkVersion, ModManifest.VERSION, 'SDK sdkVersion')
        if (ModManifest.compareVersions(metadata.sdkVersion, mod.sdkMin) < 0 || ModManifest.compareVersions(metadata.sdkVersion, mod.sdkMaxExclusive) >= 0)
            throw new GradleException("SDK ${metadata.sdkVersion} is outside [${mod.sdkMin}, ${mod.sdkMaxExclusive}).")
        if (!(metadata.gameSha256 instanceof List) || !mod.gameSha256.every { hash -> metadata.gameSha256.any { it.equalsIgnoreCase(hash) } })
            throw new GradleException('The mod declares game binaries unsupported by this SDK.')

        p.version = mod.version
        p.layout.buildDirectory.set(p.layout.projectDirectory.dir(platform.buildDirectory))
        p.pluginManager.apply('org.jetbrains.kotlin.multiplatform')
        def kotlin = p.extensions.getByType(KotlinMultiplatformExtension)
        def target = platform.createTarget(kotlin)
        target.binaries.sharedLib { baseName = "${mod.module}Kotlin" }
        def mainSources = kotlin.sourceSets.getByName("${platform.sourceSet}Main")
        mainSources.kotlin.srcDirs('src/main/kotlin', new File(sdk, 'bridge'))
        mainSources.with {
            dependencies { implementation(p.files(new File(sdk, 'klib/nimby-mod-api.klib'))) }
        }
        def testSources = kotlin.sourceSets.getByName("${platform.sourceSet}Test")
        testSources.kotlin.srcDir('src/test/kotlin')
        testSources.with {
            dependencies { implementation('org.jetbrains.kotlin:kotlin-test:2.2.20') }
        }

        def testAssets = p.tasks.register('prepareTestAssets', Sync) {
            from('assets'); from('imgs') { into('imgs') }; from('config') { into('config') }
            into(p.layout.buildDirectory.dir('test-assets'))
        }
        p.tasks.withType(getClass().classLoader.loadClass('org.jetbrains.kotlin.gradle.targets.native.tasks.KotlinNativeTest')).configureEach {
            dependsOn(testAssets)
            workingDir = p.layout.buildDirectory.dir('test-assets').get().asFile.absolutePath
            testLogging { events('passed', 'skipped', 'failed') }
        }

        def wineTests = null
        if (runner) {
            def testBinary = target.binaries.getTest(NativeBuildType.DEBUG)
            wineTests = p.tasks.register('windowsTestsWithWine', Exec) {
                group = 'verification'
                description = 'Run cross-compiled Windows tests under Wine on CI.'
                dependsOn(testAssets, testBinary.linkTaskProvider)
                workingDir(p.layout.buildDirectory.dir('test-assets'))
                commandLine('python3', runner, testBinary.outputFile)
            }
        }

        def generated = p.layout.buildDirectory.dir('generated/mod')
        def manifest = p.tasks.register('generateModManifest') {
            inputs.file(p.file('mod.json'))
            outputs.file(generated.map { it.file('nrf-mod.ini') })
            doLast {
                File ini = generated.get().file('nrf-mod.ini').asFile
                ini.parentFile.mkdirs()
                ini.setText("[NRFMod]\nlibrary=${mod.module}${platform.extension}\n", 'UTF-8')
            }
        }
        def stages = [:]
        [Debug: NativeBuildType.DEBUG, Release: NativeBuildType.RELEASE].each { name, type ->
            def binary = target.binaries.getSharedLib(type)
            stages[name] = p.tasks.register("assemble${name}Mod", Sync) {
                group = 'build'
                description = "Assemble the ${name} mod using the SDK's precompiled adapter."
                dependsOn(binary.linkTaskProvider, manifest)
                from(binary.outputFile) { rename { "${mod.module}Kotlin${platform.extension}" } }
                from(new File(sdk, "bin/${platform.adapter}")) { rename { "${mod.module}${platform.extension}" } }
                from('assets') { exclude('nrf-mod.ini') }
                from(generated)
                from('imgs') { into('imgs') }
                from('config') { into('config') }
                from('docs') { into('docs') }
                from('README.md', 'LICENSE', 'LICENSE.txt')
                from(new File(sdk, 'licenses')) { into('licenses/sdk') }
                from('licenses') { into('licenses/mod') }
                duplicatesStrategy = DuplicatesStrategy.FAIL
                into(p.layout.buildDirectory.dir("mod/${name.toLowerCase()}"))
            }
        }
        def verifyStage = p.tasks.register('prepareModVerification', Sync) {
            dependsOn(stages.Release)
            from(stages.Release)
            from(new File(sdk, 'bin')) { include(platform.libraries + [platform.loaderTest]) }
            into(p.layout.buildDirectory.dir('verification'))
        }
        def verify = p.tasks.register('verifyNativeMod', Exec) {
            group = 'verification'
            description = 'Verify native exports and mod lifecycle without running the game.'
            dependsOn(verifyStage)
            File directory = p.layout.buildDirectory.dir('verification').get().asFile
            workingDir(directory)
            def arguments = [new File(directory, platform.loaderTest), new File(directory, "${mod.module}${platform.extension}"), '--smoke']
            commandLine(runner ? ['python3', runner] + arguments : arguments)
            platform.configureVerification(delegate, directory)
            doFirst {
                if (!runner && !platform.executableOnHost()) throw new GradleException("Native verification requires a ${platform.id} host")
                platform.validateVerification(directory)
            }
        }
        String root = "${mod.modId}-${mod.version}"
        def archive = p.tasks.register('modArchive', Zip) {
            group = 'distribution'
            description = 'Build and verify a mod archive; no installation or publication.'
            dependsOn(stages.Release, p.tasks.named('allTests'), verify)
            if (wineTests) dependsOn(wineTests)
            archiveFileName.set("${root}-${platform.id}.zip")
            destinationDirectory.set(p.layout.buildDirectory.dir('distributions'))
            preserveFileTimestamps = false
            reproducibleFileOrder = true
            into(root) { from(stages.Release) }
        }
        def descriptor = p.tasks.register('hubManifest') {
            group = 'distribution'
            description = 'Generate local Hub metadata from the verified archive.'
            dependsOn(archive)
            inputs.file(p.file('mod.json')); inputs.file(archive.flatMap { it.archiveFile })
            outputs.file(p.layout.buildDirectory.file('distributions/project.json'))
            outputs.file(p.layout.buildDirectory.file("distributions/project-${platform.id}.json"))
            inputs.property('releaseBaseUrl', p.providers.gradleProperty('releaseBaseUrl').orElse(''))
            doLast {
                File zip = archive.get().archiveFile.get().asFile
                def digest = MessageDigest.getInstance('SHA-256')
                zip.withInputStream { stream -> byte[] buffer = new byte[65536]; int n; while ((n = stream.read(buffer)) != -1) digest.update(buffer, 0, n) }
                Map project = [id: mod.id, name: mod.name, kind: 'native-mod', version: mod.version,
                    modId: mod.modId, loaderApi: 1, platform: platform.id, module: "${mod.module}${platform.extension}", sdkMin: mod.sdkMin,
                    sdkMaxExclusive: mod.sdkMaxExclusive, rootFolder: root, gameSha256: mod.gameSha256,
                    channel: mod.version.contains('-') ? mod.version.split('-')[1].split('\\.')[0] : 'stable',
                    size: zip.length(), sha256: digest.digest().encodeHex().toString(), url: zip.name]
                String baseUrl = p.providers.gradleProperty('releaseBaseUrl').orElse('').get()
                if (baseUrl) {
                    String expected = "https://github.com/NimbyRails-France/${mod.id}/releases/download/v${mod.version}"
                    if (baseUrl != expected) throw new GradleException("releaseBaseUrl must be ${expected}")
                    project.url = baseUrl + '/' + zip.name
                }
                String json = JsonOutput.prettyPrint(JsonOutput.toJson(project)) + '\n'
                ['project.json', "project-${platform.id}.json"].each { name ->
                    p.layout.buildDirectory.file("distributions/${name}").get().asFile.setText(json, 'UTF-8')
                }
            }
        }
        p.tasks.register('packageMod') {
            group = 'distribution'
            description = 'Create the verified mod ZIP and local Hub manifest.'
            dependsOn(descriptor)
        }
        p.tasks.named('assemble').configure { dependsOn(stages.Debug, stages.Release) }
        p.tasks.named('check').configure {
            dependsOn(verify)
            if (wineTests) dependsOn(wineTests)
        }
        p.tasks.named('build').configure { dependsOn(descriptor) }
    }
}
