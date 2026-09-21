# NimbyRails France SDK

SDK pour développer des mods Kotlin/Native et des clients C++20 pour NIMBY Rails
sur Windows x64. Le SDK fournit les observations du jeu, l'intégration native et
le contrat de chargement. Les règles de signalisation et les ressources restent
dans les mods.

Cette branche développe la version **0.7.3**. Un paquet construit localement
n'est pas une release publiée.

## Créer un mod Kotlin

1. Installer un JDK 21 et extraire le kit Kotlin du SDK.
2. Copier `examples/kotlin-mod` depuis ce kit dans son propre espace de travail.
3. Configurer `NRF_KOTLIN_SDK` ou la propriété Gradle `nrfSdkDir`.
4. Ouvrir le projet dans IntelliJ IDEA et exécuter `build`.
5. Ajouter le projet dans le profil développeur du Hub pour l'essayer en jeu.

Le fichier de compilation du mod contient uniquement :

```kotlin
plugins {
    id("fr.nimbyrails.mod") version "0.7.3"
}
```

Le plugin Gradle livré avec le SDK gère Kotlin/Native, le pont précompilé,
les tests, la vérification du chargement et le paquet. Le projet du mod ne
requiert aucun script PowerShell, CMake, compilateur C++ ou dossier `tools`.

**[Démarrage Kotlin](docs/kotlin-mods.md)** ·
[Référence Gradle](docs/gradle-plugin.md) ·
[Mode développeur du Hub](docs/hub-development.md)

## Choisir le bon kit

| Besoin | Kit et documentation |
| --- | --- |
| Écrire un mod Kotlin | Kit Kotlin : API `.klib`, plugin Gradle, pont précompilé, exemple autonome |
| Écrire un client ou un mod C++ | Kit C++ : headers, bibliothèques, cibles CMake ; [guide C++](docs/developing.md) |
| Jouer avec des mods | SDK d'exécution et NRF Loader, installés par le Hub ; aucun JDK requis |
| Contribuer au SDK | [Guide de contribution et de construction](docs/sdk-maintainers.md) |

Le kit de compilation et le SDK installé pour jouer ont des rôles distincts.
Le Hub gère les installations et les profils ; il invoque les mêmes tâches
Gradle que l'IDE pour compiler un projet.

## Contrats et compatibilité

Les versions du SDK, de Kotlin, de Gradle et du jeu prises en charge sont
déclarées explicitement. Voir la [politique de compatibilité](docs/sdk-version.md).
Les captures sont immuables, mais leurs lectures ne sont pas atomiques.
Une donnée absente ne vaut ni zéro ni voie libre. Les réservations, occupations,
itinéraires et indications restent distincts.

Les interfaces sous `nimby/detail` et les transports natifs sont privés.
Les adaptations au binaire du jeu restent expérimentales ; le SDK ne prétend
pas prendre en charge une mise à jour du jeu sans vérification.

Consulter [l'index de documentation](docs/README.md) pour les guides,
les références d'API et les limites des fonctionnalités.

## Versions, changelog et notifications

- La version de référence est dans `VERSION`. Elle doit correspondre à `CMakeLists.txt` ou à `package.json` et son lockfile, selon le projet.
- Documenter les changements dans `CHANGELOG.md`, sous `[Unreleased]` pendant le développement, puis dans une section `## [X.Y.Z] - AAAA-MM-JJ` au moment de publier.
- Après une CI réussie, créer le tag `vX.Y.Z` sur le commit vérifié et publier sa release GitHub avec les notes de cette section (`python .woodpecker/check-release.py --notes`). Joindre les artefacts construits avec l'outillage habituel lorsqu'ils sont nécessaires.
- Les builds Woodpecker sont annoncés dans le salon Discord `1550478726557470791`. Seules les releases GitHub publiées, versionnées et avec des notes sont annoncées dans `1549088597594873907`. Un push ou un tag seul ne publie aucune annonce de mise à jour.
- La CI refuse les incohérences de versions et les tags sans changelog daté. Les releases en brouillon ne sont pas annoncées. Une correction des notes modifie l'annonce existante.

Woodpecker compile Windows x64 avec MinGW et exécute les tests CTest autonomes sous Wine. Cela ne remplace pas les essais dans le jeu ni la validation native Windows des installateurs et scripts PowerShell.

## Canaux de publication

**Stable** : `vX.Y.Z` (release normale). **Bêta** : `vX.Y.Z-beta.N`. **Alpha** : `vX.Y.Z-alpha.N` (ces deux dernières sont des prereleases GitHub). `N` commence à 1. Le Hub mémorise un canal par projet, stable par défaut, sans basculer vers un autre canal si aucune release n’existe. Un retour vers une version plus ancienne nécessite une installation manuelle.

`VERSION` et le manifeste portent la version complète ; la version CMake garde seulement `X.Y.Z`. Publier le ZIP et son `project.json` dans la **même release**, avec son changelog. Pour le Hub lui-même, publier l’installateur et `hub-latest.json`. Le manifeste donne la taille, le SHA-256, le dossier racine et les règles de compatibilité. Aucun catalogue central ne doit être modifié.

La politique est dans `release-channels.json`. Le contrôle `.woodpecker/check-release.py` refuse les autres canaux. Une release de test n’est jamais marquée comme dernière version stable.

## Publier une mise à jour

- **main** : canal stable.
- **alpha** : canal alpha.
- **beta** : canal beta.

Un commit ordinaire lance les vérifications sans publier. Pour publier, préparez la même version dans `VERSION` et les fichiers de version du projet, puis ajoutez une entrée datée dans `CHANGELOG.md`. Décrivez les nouveautés, améliorations et corrections du point de vue des utilisateurs.

Le titre exact du commit de publication est `release X.Y.Z` (exemple : alpha : `release 0.4.0-alpha.1` ; beta : `release 0.4.0-beta.1`). Poussez ce commit sur la branche du canal choisi. La compilation, les tests et la préparation des téléchargements doivent réussir avant la publication GitHub et son annonce Discord. Une version déjà publiée ne peut pas être remplacée : choisissez un nouveau numéro.

Ne créez pas le tag à la main. Les préversions restent dans leur canal et ne remplacent pas la version stable.
