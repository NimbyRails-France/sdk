package fr.nimbyrails.gradle

import groovy.json.JsonOutput
import org.gradle.api.GradleException
import org.junit.*
import org.junit.rules.TemporaryFolder
import static org.junit.Assert.*

class GameModManifestTest {
    @Rule public TemporaryFolder temp = new TemporaryFolder()
    private Map mod = [id: 'demo', name: 'Demo', version: '1.2.0-alpha.3']
    private Map declaration() { [format: 1, id: 'demo', name: 'Demo', author: 'Author', description: 'Description', signals: []] }
    private Map signal() { [id: 'signal', textures: 'catalogue', kind: 'path', name: 'Signal', catalogueName: 'Textures', states: ['closed.svg', 'open.svg']] }
    private void asset(String name, String contents = '<svg/>') {
        File file = new File(temp.root, 'assets/' + name); file.parentFile.mkdirs(); file.setText(contents, 'UTF-8')
    }
    @Test void toolNeedsNoFictitiousSignalAndKeepsAlphaVersion() {
        String text = GameModManifest.render(mod, declaration(), temp.root)
        assertEquals('[ModMeta]\nschema=1\nname=Demo\nauthor=Author\ndesc=Description\nversion=1.2.0-alpha.3\n', text)
    }
    @Test void multipleModelsKeepCatalogueIdentityOrderAndNativeNames() {
        asset('closed.svg'); asset('open.svg')
        def first = signal() + [nameKey: 'old.template', catalogueNameKey: 'old.catalogue']
        def second = signal() + [id: 'other', textures: 'other_catalogue', states: ['open.svg', 'closed.svg']]
        String text = GameModManifest.render(mod, declaration() + [signals: [first, second]], temp.root)
        assertTrue(text.contains('name_loc=old.catalogue\nstate=closed.svg\nstate=open.svg'))
        assertTrue(text.contains('name_loc=old.template\nkind=path\ntextures=catalogue'))
        assertTrue(text.contains('state=open.svg\nstate=closed.svg'))
        assertEquals(text, GameModManifest.render(mod, declaration() + [signals: [first, second]], temp.root))
    }
    @Test void rejectsMissingUnsafeAmbiguousAndDuplicateResources() {
        asset('closed.svg'); asset('open.svg')
        ['missing.svg', '../escape.svg', '/outside.svg', 'C:/outside.svg', 'imgs\\bad.svg', 'a\nstate=other.svg'].each { path ->
            assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [signals: [signal() + [states: [path]]]], temp.root) }
        }
        assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [signals: [signal() + [states: ['closed.svg', 'closed.svg']]]], temp.root) }
        asset('imgs/a.svg')
        File duplicate = new File(temp.root, 'imgs/a.svg'); duplicate.parentFile.mkdirs(); duplicate.text = '<svg/>'
        assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [signals: [signal() + [states: ['imgs/a.svg']]]], temp.root) }
    }
    @Test void usesEnglishThenFallbackForConstructionNamesWithoutLeakingTokens() {
        asset('closed.svg'); asset('open.svg')
        asset('translations.json', JsonOutput.toJson([fallback: 'fr', languages: [en: [title: 'Signal {number}'], fr: [title: 'Feu', textures: 'Images']]]))
        def entry = signal() + [name: '\u001eNRF:["title",{"number":"2"}]', catalogueName: '\u001eNRF:["textures",{}]']
        String text = GameModManifest.render(mod, declaration() + [signals: [entry]], temp.root)
        assertTrue(text.contains('name_en=Signal 2')); assertTrue(text.contains('name_en=Images'))
        assertFalse(text.contains('NRF:'))
        def catalog = GameModManifest.metadata(declaration() + [signals: [entry]], temp.root)
        assertEquals(2, catalog.format)
        assertEquals(2, catalog.resources.size())
        assertTrue(catalog.resources.every { it.key ==~ 'nrf.sdk.[a-f0-9]{64}' })
        assertEquals(2, catalog.resources.collect { it.key }.toSet().size())
        catalog.resources.each { assertTrue(text.contains('name_loc=' + it.key)) }
        def template = catalog.resources.find { it.kind == 'template' }
        assertEquals('Signal 2', template.default)
        assertEquals('Feu', template.languages.fr)
        def other = GameModManifest.metadata(declaration() + [id: 'another', signals: [entry]], temp.root)
        assertFalse(template.key == other.resources.find { it.kind == 'template' }.key)
        assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [signals: [entry + [nameKey: 'native.key']]], temp.root) }
        assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [signals: [entry + [name: '\u001eNRF:["missing",{}]']]], temp.root) }
    }
    @Test void rejectsManualFilesIdentityMismatchAndIniInjection() {
        assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [id: 'other'], temp.root) }
        assertThrows(GradleException) { GameModManifest.render(mod, declaration() + [author: 'a\n[SignalTemplate]'], temp.root) }
        asset('mod.txt', 'manual')
        assertThrows(GradleException) { GameModManifest.render(mod, declaration(), temp.root) }
    }
    @Test void metadataUsesDeclaredFallbackAndSeparateDisplayName() {
        asset('translations.json', JsonOutput.toJson([fallback: 'fr', languages: [
            fr: [title: 'Horloge', about: 'Changer la date\net l’heure'],
            en: [title: 'Clock', about: 'Change date\nand time'], de: [title: 'Uhr']]]))
        def entry = declaration() + [displayName: '\u001eNRF:["title",{}]', description: '\u001eNRF:["about",{}]']
        def metadata = GameModManifest.metadata(entry, temp.root)
        assertEquals('Horloge', metadata.languages.fr.name)
        assertEquals('Clock', metadata.languages.en.name)
        assertEquals('Changer la date\net l’heure', metadata.languages.de.description)
        def text = GameModManifest.render(mod, entry, temp.root)
        assertTrue(text.contains('name=Horloge\n'))
        assertTrue(text.contains('desc=Changer la date<br>et l’heure\n'))
        assertFalse(text.contains('NRF:'))
        assertEquals('Demo', entry.name) // Package identity never becomes a translation.
        assertThrows(GradleException) { GameModManifest.render(mod, entry + [displayName: ''], temp.root) }
        asset('nrf-metadata.json', '{}')
        assertThrows(GradleException) { GameModManifest.render(mod, entry, temp.root) }
    }
}
