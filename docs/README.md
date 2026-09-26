# Documentation du SDK

Documentation de la branche **0.8.0**, pour Windows x64. Les guides décrivent
les contrats du code de cette branche ; ils ne constituent pas une annonce
de publication.

## Développer un mod Kotlin

| Étape | Guide |
| --- | --- |
| Installer le kit, ouvrir l'exemple et compiler | [Démarrage Kotlin](kotlin-mods.md) |
| Comprendre les tâches, fichiers et sorties | [Plugin Gradle](gradle-plugin.md) |
| Déclarer les réglages, décisions et consignes | [Référence de l'API Kotlin](kotlin-api.md) |
| Comprendre la séparation mod/moteur et sa validation | [Conduite fournie par les mods](automatic-driving-validation.md) |
| Tester un projet local et revenir à la version publiée | [Mode développeur du Hub](hub-development.md) |
| Choisir les versions et migrer un projet | [Compatibilité et versions](sdk-version.md) |
| Résoudre un problème | [Dépannage](troubleshooting.md) |

## Développer un outil Kotlin

Utiliser le [client Kotlin/JVM](kotlin-client.md) et l'exemple
`examples/kotlin-observer`. Le client public C++ et ses exemples ont été retirés.
Les références C++ restantes documentent les composants internes du SDK.

## Références par domaine

| Domaine | Références |
| --- | --- |
| Réseau et cantons | [Topologie des signaux](signal-topology.md), [cantons](blocks.md), [quais et occupations](platform-occupations.md) |
| Indications et rendu | [États](signal-states.md), [commandes de textures](texture-commands.md) |
| Signalisation et conduite | [Runtime générique C++](signalling-runtime.md), [observations de conduite](driving-observation.md), [commandes de mod](mod-commands.md) |
| Horloge et installation | [Horloge de simulation](simulation-clock.md), [installation native avancée](install-drop-in.md) |

Les références du noyau décrivent aussi ses mécanismes internes. Seules les
API Kotlin documentées définissent ce qu'un mod ou un outil peut utiliser.

## Maintenir le SDK

La [description de l'architecture](architecture.md) précise les frontières du noyau.
Voir aussi [l'audit Windows](windows-audit.md), la [gestion mémoire](memory-and-performance.md)
et les notes historiques de [validation Linux dans VMware](linux-validation.md).
Le développement et la publication Linux sont suspendus.


Le [guide des mainteneurs](sdk-maintainers.md) couvre la construction des kits,
les tests et la préparation des distributions. Le [guide CLion](clion.md)
concerne le développement du SDK natif.

Les dossiers `research/` et les fichiers `*validation.md` sont des archives
techniques : observations, investigations et résultats datés. Ils ne définissent
pas le contrat public, et leurs résultats ne valident pas une nouvelle version.

- [Commandes de recette SDK + mod](recipe-commands.md) : controle temporaire, contraintes par train, horloge et contrats de validation.

- [Journaux et diagnostic de production Windows](production-diagnostics.md)
