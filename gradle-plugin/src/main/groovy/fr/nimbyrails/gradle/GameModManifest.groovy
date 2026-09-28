package fr.nimbyrails.gradle

import groovy.json.JsonSlurper
import groovy.json.JsonOutput
import java.security.MessageDigest
import org.gradle.api.GradleException

/** Render only the compiled declaration. Never parse Kotlin with regular
 * expressions or execute a rule to guess its possible textures. */
final class GameModManifest {
    static String render(Map mod, Object declaration, File root) {
        if (!(declaration instanceof Map) || declaration.format != 1)
            fail('Invalid compiled mod declaration')
        if (declaration.id != mod.id || declaration.name != mod.name)
            fail('Kotlin identity differs from mod.json. Use signalMod(modInfo) or toolMod(modInfo).')
        if (new File(root, 'assets/mod.txt').exists())
            fail('assets/mod.txt is generated now: move its metadata/resources to Kotlin, then remove the manual file.')
        if (new File(root, 'assets/nrf-metadata.json').exists())
            fail('assets/nrf-metadata.json is generated: declare translated metadata in Kotlin instead.')
        def signals = declaration.signals
        if (!(signals instanceof List) || signals.size() > 16) fail('At most 16 signal catalogues are supported')
        def translationsFile = new File(root, 'assets/translations.json')
        Map translations = translationsFile.isFile() ? new JsonSlurper().parse(translationsFile, 'UTF-8') as Map : [:]
        def text = new StringBuilder('[ModMeta]\nschema=1\n')
        Map metadata = metadata(declaration, root)
        def fallback = metadata.languages[metadata.fallback]
        text << "name=${line(fallback.name, 'name')}\n"
        text << "author=${line(declaration.author, 'author')}\n"
        text << "desc=${line(fallback.description.replace('\n', '<br>'), 'description')}\n"
        // ModMeta.version is documented as a string, including alpha suffixes.
        text << "version=${line(mod.version, 'version')}\n"
        Set ids = [], catalogues = []
        signals.each { signal ->
            if (!(signal instanceof Map)) fail('Invalid signal declaration')
            if (!ids.add(signal.id) || !catalogues.add(signal.textures)) fail('Duplicate signal id or catalogue')
            def catalogue = line(signal.textures, 'textures')
            def kind = line(signal.kind, 'kind')
            if (!(kind ==~ '[a-z_]{1,32}')) fail('Invalid native signal kind')
            if (!(signal.states instanceof List) || signal.states.isEmpty() || signal.states.size() > 256)
                fail('Each signal needs 1 to 256 texture states')
            Set paths = []
            text << "\n[SignalTextures]\nid=${catalogue}\n"
            text << "name_en=${translated(signal.catalogueName, translations)}\n"
            String catalogueKey = nativeKey(declaration.id, signal, 'textures')
            if (catalogueKey != null) text << "name_loc=${catalogueKey}\n"
            signal.states.each { state ->
                String path = image(root, state)
                if (!paths.add(path.toLowerCase(Locale.ROOT))) fail("Duplicate texture: ${path}")
                text << "state=${path}\n"
            }
            text << "\n[SignalTemplate]\nname_en=${translated(signal.name, translations)}\n"
            String templateKey = nativeKey(declaration.id, signal, 'template')
            if (templateKey != null) text << "name_loc=${templateKey}\n"
            text << "kind=${kind}\ntextures=${catalogue}\n"
        }
        text.toString()
    }

    /** Fully resolved metadata, isolated by package. The native adapter never
     * executes Kotlin, changes mod.txt, or accepts a path from this document. */
    static Map metadata(Object declaration, File root) {
        File catalog = new File(root, 'assets/translations.json')
        Map translations = catalog.isFile() ? new JsonSlurper().parse(catalog, 'UTF-8') as Map : [:]
        String fallback = translations.fallback ?: 'en'
        Map languages = [:]
        def codes = translations.languages ? translations.languages.keySet() : [fallback]
        if (!codes.contains(fallback) || codes.size() > 64) fail('Missing fallback language or too many languages')
        codes.each { code ->
            if (!(code ==~ '[a-zA-Z0-9_-]{2,32}')) fail('Invalid metadata language')
            languages[code] = [name: translated(declaration.displayName != null ? declaration.displayName : declaration.name, translations, code),
                description: translated(declaration.description, translations, code, true)]
        }
        List resources = []
        declaration.signals.each { signal ->
            ['textures', 'template'].each { kind ->
                def value = kind == 'textures' ? signal.catalogueName : signal.name
                if (isReference(value)) resources.add([
                    kind: kind, id: kind == 'textures' ? signal.textures : signal.id,
                    key: nativeKey(declaration.id, signal, kind),
                    default: translated(value, translations),
                    languages: codes.collectEntries { code -> [(code): translated(value, translations, code)] }
                ])
            }
        }
        def result = [format: resources ? 2 : 1, fallback: fallback, languages: languages]
        if (resources) result.resources = resources
        if (JsonOutput.toJson(result).getBytes('UTF-8').length > 1048576) fail('Metadata translations exceed 1 MiB')
        result
    }

    private static boolean isReference(Object value) { value instanceof String && value.startsWith('\u001eNRF:') }

    /** Native keys use package + model + role, never a translated display name.
     * The digest keeps keys bounded even when the model uses a long identifier.
     * Existing raw native keys remain available for non-NRF catalogues. */
    private static String nativeKey(String mod, Map signal, String kind) {
        def value = kind == 'textures' ? signal.catalogueName : signal.name
        def explicit = kind == 'textures' ? signal.catalogueNameKey : signal.nameKey
        if (!isReference(value)) return explicit == null ? null : line(explicit, 'native localisation key')
        if (explicit != null) fail('Use tr(...) without nameKey/catalogueNameKey: the SDK generates its localisation key.')
        def identity = [mod, signal.id, kind].join('\u0000').getBytes('UTF-8')
        'nrf.sdk.' + MessageDigest.getInstance('SHA-256').digest(identity).encodeHex().toString()
    }

    private static String line(Object value, String field) {
        if (!(value instanceof String) || !value.trim() || value.length() > 4096 || value.any { it < ' ' || it == '\u007f' })
            fail("${field} must be a nonempty single line without control characters")
        value
    }

    private static String translated(Object value, Map translations, String language = 'en', boolean description = false) {
        if (!(value instanceof String) || !value.startsWith('\u001eNRF:')) return textValue(value, description)
        def reference = new JsonSlurper().parseText(value.substring(5))
        if (!(reference instanceof List) || reference.size() != 2 || !(reference[1] instanceof Map)) fail('Invalid translation reference')
        String key = reference[0]
        def languages = translations.languages
        def translated = languages?.get(language)?.get(key) ?: languages?.get(language.split('[-_]')[0])?.get(key) ?: languages?.get(translations.fallback ?: 'en')?.get(key)
        if (!(translated instanceof String)) fail("Missing build-time translation: ${key}")
        // Match named placeholders, preserving escaped braces. Unlike native
        // panel text, this serialises a resolved string. name_en remains the
        // readable fallback; generated name_loc keys select the runtime locale.
        String result = translated.replaceAll(/\{\{|\}\}|\{([a-zA-Z0-9_.-]+)\}/) { all, name ->
            if (all == '{{') return '{'
            if (all == '}}') return '}'
            if (!reference[1].containsKey(name)) fail("Missing translation argument: ${name}")
            reference[1][name].toString()
        }
        textValue(result, description)
    }

    private static String textValue(Object value, boolean description) {
        if (!description) return line(value, 'translated name')
        if (!(value instanceof String)) fail('Description must be text')
        String normalized = value.replace('\r\n', '\n')
        line(normalized.replace('\n', '<br>'), 'description')
        // The game itself treats <br> in ModMeta.desc as a line break.
        normalized.replace('<br>', '\n')
    }

    private static String image(File root, Object value) {
        String path = line(value, 'texture path')
        if (path.getBytes('UTF-8').length >= 96 || path.contains('\\') || path.contains(':') ||
            path.startsWith('/') || path.split('/', -1).any { !it || it in ['.', '..'] || it.endsWith(' ') || it.endsWith('.') })
            fail("Texture must use a safe package-relative path (under 96 UTF-8 bytes): ${path}")
        List<File> candidates = [new File(root, "assets/${path}")]
        if (path.startsWith('imgs/') || path.startsWith('config/')) candidates.add(new File(root, path))
        def matches = candidates.findAll { it.isFile() }
        if (matches.size() != 1) fail("Missing or ambiguous texture: ${path}")
        File selected = matches.first()
        File sourceRoot = selected == candidates.first() ? new File(root, 'assets') : new File(root, path.split('/')[0])
        if (!selected.toPath().toRealPath().startsWith(sourceRoot.toPath().toRealPath())) fail("Texture escapes resource directory: ${path}")
        path
    }

    private static void fail(String message) { throw new GradleException(message) }
}
