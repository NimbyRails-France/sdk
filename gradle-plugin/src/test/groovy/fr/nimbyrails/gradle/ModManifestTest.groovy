package fr.nimbyrails.gradle

import groovy.json.JsonOutput
import org.gradle.api.GradleException
import org.junit.*
import org.junit.rules.TemporaryFolder
import static org.junit.Assert.*

class ModManifestTest {
    @Rule public TemporaryFolder temp = new TemporaryFolder()
    private Map valid() {
        [id: 'independent-mod', name: 'Independent', modId: 'Independent', module: 'IndependentMod',
         version: '1.2.3', language: 'kotlin-native', sdkMin: '0.7.3', sdkMaxExclusive: '0.8.0', gameSha256: ['a' * 64]]
    }
    private Map read(Map m) {
        File file = temp.newFile()
        file.setText(JsonOutput.toJson(m), 'UTF-8')
        ModManifest.read(file)
    }
    @Test void acceptsIndependentIdentityAndPrereleaseVersions() {
        assertEquals('IndependentMod', read(valid()).module)
        assertEquals('1.2.3-beta.2', read(valid() + [version: '1.2.3-beta.2']).version)
        assertTrue(ModManifest.compareVersions('0.7.3-beta.2', '0.7.3') < 0)
        assertTrue(ModManifest.compareVersions('0.7.3-beta.10', '0.7.3-beta.2') > 0)
    }
    @Test void rejectsUnsafeArtifactNamesAndUnspecifiedCompatibility() {
        [[module: '../escape'], [modId: '../escape'], [version: 'bad'], [gameSha256: []],
         [sdkMin: '0.8.0', sdkMaxExclusive: '0.7.3'], [sdkMaxExclusive: null]].each { change ->
            assertThrows(GradleException) { read(valid() + change) }
        }
    }
}
