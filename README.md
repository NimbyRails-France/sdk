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