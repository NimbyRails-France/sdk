# Plugin Gradle des mods Kotlin

Identifiant : `fr.nimbyrails.mod`, version **0.9.0-alpha.3**.
Le plugin est distribué dans le dépôt Maven `gradle-repository/` du kit SDK.
Il n'est pas annoncé comme publié sur le Gradle Plugin Portal.

## Configuration

`build.gradle.kts` :

```kotlin
plugins {
    id("fr.nimbyrails.mod")
}
```

`settings.gradle.kts` :

```kotlin
pluginManagement {
    val sdk = providers.gradleProperty("nrfSdkDir")
        .orElse(providers.environmentVariable("NRF_KOTLIN_SDK"))
        .orNull ?: error("Configurer nrfSdkDir ou NRF_KOTLIN_SDK vers le kit SDK Kotlin.")
    repositories {
        maven { url = uri(file(sdk).resolve("gradle-repository")) }
        gradlePluginPortal()
        mavenCentral()
    }
    val metadata = groovy.json.JsonSlurper()
        .parseText(file(sdk).resolve("sdk.json").readText().removePrefix("\uFEFF")) as Map<*, *>
    plugins { id("fr.nimbyrails.mod") version (metadata["gradlePluginVersion"] as String) }
}
dependencyResolutionManagement { repositories { mavenCentral() } }
rootProject.name = "mon-mod"
```

La propriété Gradle `nrfSdkDir` a priorité sur `NRF_KOTLIN_SDK`.
Utiliser un chemin absolu dans le Hub ou les réglages personnels. Les chemins
relatifs passés à Gradle sont résolus depuis le projet.

Le plugin applique Kotlin Multiplatform, ajoute l'API `.klib` et les exports du
SDK. La cible vient de `sdk.json` : `mingw_x64` crée `windows`/`mingwX64` et des
DLL ; `linux_x64` crée `linux`/`linuxX64` et des fichiers `.so`. Il vérifie le
format, les fichiers, la version et la compatibilité du kit avant compilation.
La vérification native s'exécute sur le système correspondant au kit.

Le contrat Linux est préparé dans le plugin, mais aucun kit SDK Linux complet
n'est encore validé ou publié. Les tests de configuration emploient des fichiers
factices et ne constituent pas un essai des hooks. La vérification Kotlin/Native
contre les sources reste disponible dans `verification/kotlin-mod`.

## Manifeste source

`mod.json` est la source des métadonnées de construction et de compatibilité :

```json
{
  "id": "mon-mod",
  "name": "Mon mod",
  "modId": "MonMod",
  "module": "MonModCore",
  "version": "0.1.0",
  "language": "kotlin-native",
  "sdkMin": "0.9.0-alpha.3",
  "sdkMaxExclusive": "0.10.0",
  "gameSha256": ["fff49ac21720abfc824c2b4f68b862727630eb0db71cfe1f9ea8f685d0db10ae"]
}
```

| Champ | Contrat |
| --- | --- |
| `id` | Identifiant Hub : minuscules, chiffres et tirets, commence par une lettre, 64 caractères maximum |
| `name` | Nom lisible non vide |
| `modId` | Nom du dossier de mod et préfixe du paquet : lettres, chiffres, tirets, underscores, 70 caractères maximum |
| `module` | Nom de la DLL sans extension, commence par une lettre, 100 caractères maximum |
| `version` | Trois nombres, éventuellement suivis de `-alpha.N` ou `-beta.N` |
| `language` | `kotlin-native` |
| `sdkMin`, `sdkMaxExclusive` | Intervalle de compatibilité, borne basse incluse et borne haute exclue |
| `gameSha256` | Liste non vide des binaires du jeu pris en charge ; doit être couverte par le kit choisi |

Les identifiants du panneau et du catalogue de textures sont définis par le
code et les ressources du mod. Ils restent stables pour conserver les réglages
des sauvegardes. `assets/mod.txt` décrit les ressources destinées au jeu ;
le plugin ne réécrit pas ce catalogue.

## Sources et ressources

- `src/main/kotlin` : code du mod, dont `nimby.mod.createMod()`.
- `src/test/kotlin` : tests Kotlin.
- `assets/` : fichiers copiés à la racine du paquet.
- `imgs/`, `config/`, `docs/` : dossiers facultatifs conservés dans le paquet.
- `licenses/` : notices du mod, copiées dans `licenses/mod/`.
- `README.md`, `LICENSE`, `LICENSE.txt` : fichiers facultatifs distribués.

Le plugin génère `nrf-mod.ini`. Un ancien exemplaire dans `assets` est ignoré.
Les licences du SDK sont copiées dans `licenses/sdk/`. Les scripts locaux,
les captures, les sources et les tests ne sont pas incorporés au paquet.

## Tâches et sorties

| Tâche | Résultat |
| --- | --- |
| `windowsTest` | Tests Kotlin ; rapports sous `build/gradle/reports/tests/` |
| `generateModManifest` | Manifeste du loader sous `build/gradle/generated/mod/` |
| `assembleDebugMod` | Mod Debug sous `build/gradle/mod/debug/` |
| `assembleReleaseMod` | Mod Release sous `build/gradle/mod/release/` |
| `verifyNativeMod` | Vérification du chargement, arrêt et redémarrage, sans jeu |
| `packageMod` | ZIP vérifié et manifeste Hub sous `build/gradle/distributions/` |
| `hubManifest` | Métadonnées et empreinte du ZIP ; construit d'abord le paquet |
| `build` | Assemblages, tests, vérification native et distribution |
| `clean` | Retire uniquement `build/gradle/` |

Nom du ZIP : `<modId>-<version>-windows-x64.zip`.
Sa racine est `<modId>-<version>/`.
Le paquet comprend `<module>.dll` et `<module>Kotlin.dll`.
Le runtime SDK et le testeur natif sont utilisés dans le dossier de vérification,
et ne sont pas distribués avec le mod.

`project.json` contient la taille et le SHA-256 réels du ZIP produit.
Son champ `url` désigne un nom de fichier local : ce document sert au Hub
en développement. Une publication exige une URL de release et le processus
de catalogue approprié ; cette étape n'est pas effectuée par le plugin.

## Limites

Le plugin ne publie rien, ne copie rien dans le jeu et ne modifie aucun profil
du Hub. Il ne fournit pas encore de mécanisme d'acquisition automatique du kit
SDK, de publication distante ou de garantie de fonctionnement hors ligne.
La compatibilité de cette chaîne est vérifiée avec JDK 21 et Gradle 8.14.3.
