package fr.nimbyrails.gradle

import groovy.json.JsonSlurper
import org.gradle.api.GradleException

/** Source identity. Artifact hashes are calculated only after a successful build. */
final class ModManifest {
    static final String VERSION = '(0|[1-9][0-9]{0,3})\\.(0|[1-9][0-9]{0,3})\\.(0|[1-9][0-9]{0,3})(?:-(alpha|beta)\\.([1-9][0-9]{0,8}))?'

    static Map read(File file) {
        if (!file.isFile()) throw new GradleException("Missing mod.json: ${file}")
        def value = new JsonSlurper().parseText(file.getText('UTF-8').replaceFirst('^\uFEFF', ''))
        if (!(value instanceof Map)) throw new GradleException('mod.json must contain a JSON object')
        Map m = value
        check(m.id, '[a-z][a-z0-9-]{0,63}', 'id')
        check(m.version, VERSION, 'version')
        check(m.module, '[A-Za-z][A-Za-z0-9_-]{0,99}', 'module (without .dll)')
        check(m.modId, '[A-Za-z0-9_-]{1,70}', 'modId')
        check(m.sdkMin, VERSION, 'sdkMin')
        check(m.sdkMaxExclusive, VERSION, 'sdkMaxExclusive')
        if (compareVersions(m.sdkMin, m.sdkMaxExclusive) >= 0) throw new GradleException('Invalid SDK version range')
        if (m.language != 'kotlin-native') throw new GradleException('language must be kotlin-native')
        if (m.containsKey('developmentStatus') && !(m.developmentStatus in ['in-development', 'stable']))
            throw new GradleException('developmentStatus must be in-development or stable when present')
        if (!(m.name instanceof String) || !m.name.trim()) throw new GradleException('name is required')
        if (!(m.gameSha256 instanceof List) || m.gameSha256.isEmpty()) throw new GradleException('gameSha256 must list supported game binaries')
        m.gameSha256.each { check(it, '[a-fA-F0-9]{64}', 'gameSha256') }
        m
    }

    static void check(Object value, String pattern, String field) {
        if (!(value instanceof String) || !(value ==~ pattern)) throw new GradleException("Invalid ${field} in mod.json")
    }

    static int compareVersions(String a, String b) {
        def left = (a =~ VERSION)[0]
        def right = (b =~ VERSION)[0]
        for (int i = 1; i <= 3; i++) {
            int comparison = left[i].toInteger() <=> right[i].toInteger()
            if (comparison) return comparison
        }
        def rank = { stage -> stage == 'alpha' ? 0 : stage == 'beta' ? 1 : 2 }
        int comparison = rank(left[4]) <=> rank(right[4])
        comparison ?: ((left[5] ?: '0').toInteger() <=> (right[5] ?: '0').toInteger())
    }
}
