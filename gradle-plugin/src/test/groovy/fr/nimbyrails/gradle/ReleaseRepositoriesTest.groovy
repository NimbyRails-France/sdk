package fr.nimbyrails.gradle

import org.junit.Test
import static org.junit.Assert.*

class ReleaseRepositoriesTest {
    @Test void onlyExactProjectSpecificOfficialAliasesAreListed() {
        def renamed = [signalisationfrancaiserealiste: 'ab-signalisation-lumineuse',
            'signal-placement': 'ba-signal-placement', 'time-change': 'bb-timechange']
        renamed.each { project, repository ->
            String original = "https://github.com/NimbyRails-France/${project}/releases/download/v0.1.0"
            String current = "https://github.com/NimbyRails-France/${repository}/releases/download/v0.1.0"
            def allowed = ReleaseRepositories.baseUrls(project, '0.1.0')
            assertEquals([original, current], allowed)
            [current.replace('NimbyRails-France', 'OtherOwner'),
             current.replace('/v0.1.0', '/v0.1.1'), current + '/', current + '?download=1',
             current.replace('https://github.com/', 'https://github.com.evil.example/')].each {
                assertFalse(allowed.contains(it))
            }
            renamed.findAll { id, ignored -> id != project }.values().each {
                assertFalse(allowed.contains(current.replace(repository, it)))
            }
        }
        assertEquals(['https://github.com/NimbyRails-France/independent/releases/download/v1.0.0-beta.2'],
            ReleaseRepositories.baseUrls('independent', '1.0.0-beta.2'))
    }
}
