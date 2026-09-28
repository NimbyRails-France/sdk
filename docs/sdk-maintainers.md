# Maintenir et distribuer le SDK

Ce guide s'adresse aux contributeurs du SDK. Les auteurs de mods suivent
le [démarrage Kotlin](kotlin-mods.md) et n'ont pas à construire le pont natif.

La [description de l'architecture](architecture.md) précise les frontières du noyau.
Voir aussi [l'audit Windows](windows-audit.md), la [gestion mémoire](memory-and-performance.md)
et la [validation Linux dans VMware](linux-validation.md).

## Organisation du dépôt

| Dossier | Responsabilité |
| --- | --- |
| `include/nimby/` | Contrats natifs internes et adaptateurs |
| `src/engine/`, `src/runtime/`, `src/loader/` | Algorithmes, captures et orchestration communs |
| `src/platform/windows/`, `src/platform/linux/` | Implémentations système et adaptations au binaire cible |
| `kotlin/src/nimby/` | API Kotlin publique |
| `kotlin/native/` | Transport Kotlin/natif, adaptateur et vérifications |
| `gradle-plugin/` | Plugin Gradle commun aux mods |
| `verification/kotlin-consumer/` | Fixture de compilation du client installé, non distribuée |
| `tests/` | Tests natifs du SDK |
| `tools/` | Construction des kits et diagnostics du SDK |
| `docs/` | Guides et références ; recherches historiques identifiées séparément |

Les scripts PowerShell mainteneurs sont propres à la fabrication du SDK.
Ils ne sont pas copiés dans les mods. Le Hub reste responsable des installations
dans le jeu et de la sélection des profils.

L'outil facultatif `tools/texture_catalog.py` produit un catalogue HTML et JSON
depuis un dossier de mod assemblé : `python tools/texture_catalog.py MOD SORTIE`.
Il est commun au SDK et n'est pas une étape de compilation des mods.

## Prérequis

Pour le SDK natif : Windows x64, CMake 3.24+, Ninja et MinGW compatible.
Les scripts actuels utilisent les outils fournis par CLion ; les projets CMake
peuvent aussi être configurés avec des chemins de toolchain explicites.

Pour le plugin Gradle : JDK 21 et le Wrapper fourni dans `gradle-plugin/`.
Pour construire l'API Kotlin : distribution Kotlin/Native 2.2.20 Windows x64.
Ces dépendances sont celles des mainteneurs, pas celles d'un joueur.

## Vérifier le plugin Gradle

Depuis `gradle-plugin/` :

```text
gradlew.bat build
```

Les tests couvrent le manifeste, les plages de versions, les noms de fichiers,
les erreurs de kit et le graphe de tâches du consommateur. TestKit utilise un
projet indépendant, y compris avec des espaces dans le chemin du SDK.

## Construire les kits

Depuis la racine du SDK, avec les outils CMake disponibles :

```text
cmake --preset Release
cmake --build --preset Release
ctest --preset Release
cmake --install build/clion-Release --prefix install/development
```

Les presets du dépôt utilisent la toolchain CLion. Pour une autre installation,
adapter les options CMake ou employer des presets utilisateur non versionnés.

Pour produire le kit Kotlin :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/windows/package-kotlin-sdk.ps1 -KotlinHome C:/Toolchains/kotlin-native-prebuilt-windows-x86_64-2.2.20
```

Le script consomme par défaut `install/development`, compile l'API et l'adaptateur,
vérifie et publie le plugin dans le dépôt Maven embarqué, puis copie l'exemple,
la documentation et les licences. Il prépare `install/kotlin` et le ZIP sous
`dist/`. `-SdkInstall` et `-Destination` permettent de choisir les emplacements.

Les autres distributions restent distinctes :

- `tools/windows/package.ps1` : composants natifs internes et vérification de l'exemple client Kotlin installé.
- `tools/windows/package-drop-in.ps1` : runtime et loader pour le jeu.
- `tools/windows/package-hub-sdk.ps1` : paquet SDK et métadonnées du Hub.

Ces commandes préparent des fichiers locaux. La publication d'une release
est une opération distincte.

## Vérifier une distribution

Avant de distribuer un kit :

1. Exécuter les tests natifs et les tests du plugin Gradle.
2. Extraire le ZIP dans un dossier neuf, hors de l'arborescence source.
3. Copier son exemple Kotlin dans un autre dossier.
4. Lancer `build` en pointant uniquement vers le kit extrait.
5. Vérifier les tests Kotlin, le chargement des DLL et le contenu du ZIP.
6. Ajouter ce projet neuf au Hub sans ancien manifeste ni catalogue préalable.
7. Effectuer les essais en jeu correspondant aux fonctionnalités modifiées.

Un test hors jeu ou une compilation ne valide pas la conduite dans une partie.
Les rapports de campagne doivent préciser la version, le binaire du jeu et le
périmètre observé ; ils ne doivent pas être présentés comme un guide utilisateur.

## Modifier un contrat

Lors d'une évolution, maintenir ensemble le plugin, `sdk.json`, le manifeste
source, l'exemple autonome et le lecteur de projets du Hub. Une modification des
noms de tâches ou des chemins de sortie est une modification du contrat d'outillage.

Pour la version 0.8.0, le Hub appelle `packageMod` et attend
`build/gradle/distributions/<modId>-<version>-windows-x64.zip`.
Les contraintes du jeu et du SDK doivent rester explicites ; ne pas les déduire
du poste du mainteneur.
# Construction locale depuis le Hub

Le profil Développer du Hub sait construire ce dépôt Windows via `hub-local.json`.
Choisir la racine du dépôt dans la page SDK, puis **Construire et préparer le SDK**.
`VERSION` est l'unique version du kit et de son plugin Gradle. Le script
`tools/windows/build-for-hub.ps1` enchaîne CMake Release, CTest, installation dans
un dossier neuf, kit Kotlin et archive Hub. Il n'installe rien dans le jeu et ne
publie rien. Le Hub vérifie les sorties puis sélectionne le kit correspondant ;
les mods doivent ensuite être recompilés avant l'activation du profil.

Le cache natif est `build/hub-native`, protégé par un verrou pendant la construction.
Chaque kit validé reste sous `install/hub/sdk-…` ; ne pas supprimer le kit indiqué
dans les paramètres du Hub. Un reçu `hub-result.json` décrit le manifeste, l'archive
et le kit, avec des chemins relatifs au dossier de sortie. Il est écrit en dernier.
Un build interrompu ne produit pas de résultat importable et ne remplace pas le kit
précédent. Les variables `NRF_CLION_HOME`, `NRF_KOTLIN_HOME` et `NRF_JAVA_HOME`
permettent de choisir les outils Windows ; par défaut CLion dans LocalAppData,
Kotlin/Native 2.2.20 dans `.konan` et le JDK configuré ou Adoptium 21 sont utilisés.
