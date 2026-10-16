# Tester la branche master

`master` est une branche de travail à faire essayer par les développeurs. Elle
ne correspond pas à une release et ne déclenche ni publication GitHub, ni
distribution par le Hub, ni annonce de mise à jour. Les canaux `alpha`, `beta`
et `main` conservent leur procédure de publication habituelle.

Après chaque push sur `master`, le workflow **Master SDK validation** compile le
SDK Windows, exécute les tests disponibles et construit le kit Kotlin avec un
mod consommateur. Les tests Wine ne remplacent pas un essai dans NIMBY Rails.
Un échec empêche la création de l'artefact final.

Dans la page Actions du commit, télécharger l'artefact
`nrf-sdk-master-<commit>`, conservé sept jours, puis l'extraire dans un nouveau
dossier. Définir `NRF_KOTLIN_SDK` ou la propriété Gradle `nrfSdkDir` vers ce
dossier. Recompiler les mods à tester avec ce kit.

Utiliser aussi les DLL d'exécution de son dossier `bin` : le kit et le runtime
doivent provenir du **même commit**. Ne pas déduire leur compatibilité du seul
numéro `0.9.0-alpha.4`, qui peut rester identique entre deux commits de travail.
L'artefact n'est pas un installateur Hub ; conserver une copie de l'installation
de test avant de remplacer son runtime, jeu fermé.

Pour AB `111001000`, tester T32 et T33 sur le parcours effectivement analysé,
avec et sans signal de protection suivant. Les deux blancs imposent 30 km/h.
L'absence de signal suivant ne prouve pas une fin de parcours : une limite
native positivement reconnue est nécessaire. Un butoir physique n'est pas
encore une fin prouvée par cette API. Les réservations du jeu restent en place.

Laisser `NRF_INTERLOCKING_SHADOW` et `NRF_DRIVING_TRANSACTIONAL_COMMIT` absents
de l'environnement. Ce sont des expériences désactivées par défaut, inutiles
pour cet essai de manœuvre et non qualifiées pour l'exploitation normale.

Pour un signal qui refuse de s'ouvrir, noter le commit SDK, le commit du mod,
la version du jeu et les étapes de reproduction, puis exporter les diagnostics
du Hub. Une validation automatique réussie ne constitue pas une validation en jeu.
