# Notes techniques du SDK

Les guides destinés aux créateurs de mods et d'outils sont maintenant développés
dans le [dépôt wiki](https://github.com/NimbyRails-France/wiki-dev), pour le site
**https://wiki-dev.nimbyrails-france.fr**. Ils couvrent la nouvelle API `signalMod`,
le client `Nimby.connect`, les exemples et la référence Kotlin synchronisée.
Ce dossier conserve les contrats internes, rapports et notes de maintenance.

Documentation de la branche **0.9.0-alpha.3**, pour Windows x64. Les guides décrivent
les contrats du code de cette branche ; ils ne constituent pas une annonce
de publication.

## Développer un mod Kotlin

| Étape | Guide |
| --- | --- |
| Installer le kit, créer son projet et compiler | [Démarrage Kotlin](kotlin-mods.md) |
| Réunir plusieurs modèles dans un même mod | [Types de signaux](signal-types.md) |
| Comprendre les tâches, fichiers et sorties | [Plugin Gradle](gradle-plugin.md) |
| Déclarer les réglages, décisions et consignes | [Référence de l'API Kotlin](kotlin-api.md) |
| Comprendre la séparation mod/moteur et sa validation | [Conduite fournie par les mods](automatic-driving-validation.md) |
| Tester un projet local et revenir à la version publiée | [Mode développeur du Hub](hub-development.md) |
| Choisir les versions et migrer un projet | [Compatibilité et versions](sdk-version.md) |
| Résoudre un problème | [Dépannage](troubleshooting.md) |

## Développer un outil Kotlin

Suivre le [parcours Kotlin/JVM du wiki](https://wiki-dev.nimbyrails-france.fr/lire/connexion). Le client public C++ et ses exemples ont été retirés.
Les références C++ restantes documentent les composants internes du SDK.

La [construction expérimentale](construction.md) fait partie des builds alpha
Windows concernés et des kits locaux du Hub développeur. Les API Kotlin
permettent de préparer, poser, consulter le résultat et annuler une série en
solo. Les limites de qualification restent décrites dans son contrat ; cela
ne vaut pas annonce de publication ni validation du multijoueur.

## Références par domaine

| Domaine | Références |
| --- | --- |
| Réseau et cantons | [Topologie des signaux](signal-topology.md), [longueurs natives](track-metrics.md), [cantons](blocks.md), [quais et occupations](platform-occupations.md) |
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

La [recherche sur la construction des signaux](research/signal-construction.md)
suit les commandes natives, leur résultat et l'annulation pour le projet de
pose Kotlin. Ce rapport historique complète le contrat public actuel ; les
adresses et protocoles internes ne sont pas nécessaires à un mod outil.

- [Commandes de recette SDK + mod](recipe-commands.md) : controle temporaire, contraintes par train, horloge et contrats de validation.

- [Journaux et diagnostic de production Windows](production-diagnostics.md)
