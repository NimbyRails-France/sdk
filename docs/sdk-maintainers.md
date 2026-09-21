# Maintenir et distribuer le SDK

Ce guide s'adresse aux contributeurs du SDK. Les auteurs de mods suivent
le [démarrage Kotlin](kotlin-mods.md) et n'ont pas à construire le pont natif.

## Organisation du dépôt

| Dossier | Responsabilité |
| --- | --- |
| `include/nimby/` | Contrats natifs internes et adaptateurs |
| `src/` | Observations, ponts natifs et runtime |
| `kotlin/src/nimby/` | API Kotlin publique |
| `kotlin/native/` | Transport Kotlin/natif, adaptateur et vérifications |
| `gradle-plugin/` | Plugin Gradle commun aux mods |
| `examples/kotlin-mod/` | Projet Kotlin autonome distribué dans le kit |
| `examples/kotlin-observer/` | Exemple client Kotlin/JVM |
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
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package-kotlin-sdk.ps1 -KotlinHome C:/Toolchains/kotlin-native-prebuilt-windows-x86_64-2.2.20
```

Le script consomme par défaut `install/development`, compile l'API et l'adaptateur,
vérifie et publie le plugin dans le dépôt Maven embarqué, puis copie l'exemple,
la documentation et les licences. Il prépare `install/kotlin` et le ZIP sous
`dist/`. `-SdkInstall` et `-Destination` permettent de choisir les emplacements.

Les autres distributions restent distinctes :

- `tools/package.ps1` : composants natifs internes et vérification de l'exemple client Kotlin installé.
- `tools/package-drop-in.ps1` : runtime et loader pour le jeu.
- `tools/package-hub-sdk.ps1` : paquet SDK et métadonnées du Hub.

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

Pour la version 0.7.3, le Hub appelle `packageMod` et attend
`build/gradle/distributions/<modId>-<version>-windows-x64.zip`.
Les contraintes du jeu et du SDK doivent rester explicites ; ne pas les déduire
du poste du mainteneur.
