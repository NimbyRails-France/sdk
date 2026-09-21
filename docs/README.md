# Documentation du SDK

Documentation de la branche **0.7.3**, pour Windows x64. Les guides décrivent
les contrats du code de cette branche ; ils ne constituent pas une annonce
de publication.

## Développer un mod Kotlin

| Étape | Guide |
| --- | --- |
| Installer le kit, ouvrir l'exemple et compiler | [Démarrage Kotlin](kotlin-mods.md) |
| Comprendre les tâches, fichiers et sorties | [Plugin Gradle](gradle-plugin.md) |
| Déclarer les réglages, décisions et consignes | [Référence de l'API Kotlin](kotlin-api.md) |
| Tester un projet local et revenir à la version publiée | [Mode développeur du Hub](hub-development.md) |
| Choisir les versions et migrer un projet | [Compatibilité et versions](sdk-version.md) |
| Résoudre un problème | [Dépannage](troubleshooting.md) |

## Développer un outil Kotlin

Utiliser le [client Kotlin/JVM](kotlin-client.md) et l'exemple
`examples/kotlin-observer`. Les anciens exemples clients C++ ont été retirés.
Les références C++ restantes documentent les composants internes du SDK.

## Références par domaine

| Domaine | Références |
| --- | --- |
| Réseau et cantons | [Topologie des signaux](signal-topology.md), [cantons](blocks.md), [quais et occupations](platform-occupations.md) |
| Indications et rendu | [États](signal-states.md), [commandes de textures](texture-commands.md) |
| Signalisation et conduite | [Runtime générique C++](signalling-runtime.md), [observations de conduite](driving-observation.md), [commandes de mod](mod-commands.md) |
| Horloge et installation | [Horloge de simulation](simulation-clock.md), [installation native avancée](install-drop-in.md) |

Les capacités C++ ne sont pas toutes exposées à l'API Kotlin. La référence
Kotlin définit précisément ce qu'un mod Kotlin peut utiliser.

## Maintenir le SDK

Le [guide des mainteneurs](sdk-maintainers.md) couvre la construction des kits,
les tests et la préparation des distributions. Le [guide CLion](clion.md)
concerne le développement du SDK natif.

Les dossiers `research/` et les fichiers `*validation.md` sont des archives
techniques : observations, investigations et résultats datés. Ils ne définissent
pas le contrat public, et leurs résultats ne valident pas une nouvelle version.
