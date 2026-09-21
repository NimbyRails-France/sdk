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

## Utiliser les API C++

Le SDK conserve son API C++20 pour les clients externes et les intégrations natives.

- [Captures automatiques avec le client C++](tutorial-cpp-client.md).
- [Premier outil à captures manuelles](tutorial-first-tool.md).
- [Intégration et distribution du kit C++](developing.md).
- [Référence C++](cpp-api-reference.md).
- [Création d'un mod C++](tutorial-create-mod.md) et [contrat du loader](mod-loader.md).
- [Migration de l'ancienne API 0.6](migration-0.7.md).

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