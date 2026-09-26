# Créer un mod Kotlin

Ce guide utilise le kit Kotlin **0.8.0**, Windows x64, Kotlin/Native **2.2.20**
et Gradle **8.14.3**. Prévoir un **JDK 21** pour Gradle ; le joueur n'a pas
besoin de Java. L'accès aux dépôts Gradle et Maven est nécessaire au premier
build : le kit n'est pas une distribution entièrement hors ligne.

## Installer le kit

Extraire `NimbyRailsFranceSDK-kotlin-0.8.0-windows-x64.zip` dans un dossier
durable, par exemple `C:/SDK/NimbyKotlin-0.8.0`. Ce dossier doit contenir
`sdk.json`, `gradle-repository/`, `klib/`, `bridge/` et `bin/`.

Le kit contient le plugin Gradle précompilé, l'API Kotlin et ses sources
consultables, le pont natif précompilé, les dépendances nécessaires aux
vérifications, les licences et un exemple autonome. Les sources C++ du SDK,
CMake et CLion ne sont pas nécessaires pour créer un mod.

## Créer le projet

Copier `examples/kotlin-mod/` du kit vers un nouveau dossier. Ce projet
ne dépend pas de son emplacement d'origine.

```text
mon-mod/
  build.gradle.kts
  settings.gradle.kts
  gradle.properties
  mod.json
  gradlew
  gradlew.bat
  gradle/wrapper/
  src/main/kotlin/Entry.kt
  src/test/kotlin/ExampleTest.kt
  assets/mod.txt
  assets/signal.svg
```

Choisir ses identifiants dans `mod.json` et adapter les ressources ainsi que
`nimby.mod.createMod()`. Les tests utilisent `kotlin.test.Test`.

Le Wrapper Gradle est standard et se conserve dans Git. Aucun script
PowerShell de compilation ou d'installation n'est à écrire ou recopier.

## Configurer IntelliJ IDEA

1. Ouvrir `build.gradle.kts` comme projet.
2. Choisir le JDK 21 comme **Gradle JVM**.
3. Définir `NRF_KOTLIN_SDK` dans l'environnement, ou ajouter
   `nrfSdkDir=C:/SDK/NimbyKotlin-0.8.0` à son fichier utilisateur
   `%USERPROFILE%/.gradle/gradle.properties`.
4. Synchroniser Gradle, puis lancer la tâche `build`.

Le chemin du SDK est une préférence de la machine, pas un chemin personnel
à versionner dans le mod. Le Hub peut le transmettre pour chaque compilation.

## Compiler et tester

Dans un terminal Windows avec le JDK 21 sélectionné :

```text
gradlew.bat build -PnrfSdkDir=C:/SDK/NimbyKotlin-0.8.0
gradlew.bat windowsTest -PnrfSdkDir=C:/SDK/NimbyKotlin-0.8.0
gradlew.bat assembleDebugMod -PnrfSdkDir=C:/SDK/NimbyKotlin-0.8.0
```

`build` produit les assemblages, lance les tests Kotlin, vérifie le chargement
natif et crée le paquet. Les sorties sont sous `build/gradle/`.
Les détails se trouvent dans la [référence Gradle](gradle-plugin.md).

Le testeur natif vérifie les exports et les cycles de vie sans lancer le jeu.
Il ne valide pas les règles métier ni le comportement dans une partie.

## Essayer en jeu

Ajouter le dossier du projet dans le profil **Développer** du Hub, choisir le
kit Kotlin et le SDK d'exécution compatibles, puis utiliser **Compiler**.
Le Hub prépare le paquet local sans remplacer la version publiée.

Le code Kotlin chargé demeure en mémoire jusqu'à la fin du processus :
redémarrer le jeu pour essayer un nouveau binaire. Voir le
[parcours Hub](hub-development.md), notamment les limites d'isolation des données.

## Migrer un ancien projet

Remplacer le build Gradle spécifique au mod par la déclaration du plugin
`fr.nimbyrails.mod`, et reprendre `settings.gradle.kts` depuis l'exemple.
Compléter `mod.json` selon la [référence](gradle-plugin.md#manifeste-source).
Retirer les anciens scripts de compilation et d'installation ainsi que
`assets/nrf-mod.ini` : le SDK génère ce dernier.

L'ancien `build-kotlin-mod.ps1` n'est plus le parcours consommateur.
Les outils ponctuels de capture peuvent rester dans un dossier `tools/`
ignoré par Git. Ni le build ni le guide du mod ne doivent en dépendre.