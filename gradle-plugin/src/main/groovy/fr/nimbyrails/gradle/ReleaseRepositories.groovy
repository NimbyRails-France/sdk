package fr.nimbyrails.gradle

/** Repository renames never change a mod's installation or saved identity.
 * Only these exact official aliases are accepted; names and versions still
 * come from the validated source manifest, never from an arbitrary URL. */
final class ReleaseRepositories {
    private static final Map<String, String> RENAMED = [
        signalisationfrancaiserealiste: 'ab-signalisation-lumineuse',
        'signal-placement': 'ba-signal-placement',
        'time-change': 'bb-timechange',
    ].asImmutable()

    static List<String> baseUrls(String projectId, String version) {
        [projectId, RENAMED[projectId]].findAll { it != null }.unique().collect { repository ->
            "https://github.com/NimbyRails-France/${repository}/releases/download/v${version}".toString()
        }
    }
}
