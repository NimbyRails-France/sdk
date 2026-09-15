# Tutoriel : afficher les trains avec le SDK

[Documentation](README.md) · [Dépannage](troubleshooting.md)

**Objectif :** obtenir un programme console qui affiche les noms des trains,
leurs vitesses, leurs positions sur les voies et la gare associée à chaque voie.
Il tente cinq captures, avec une pause de 250 ms entre deux tentatives, puis se ferme.

Le programme tourne à côté du jeu et lit ses données par l'API du SDK.
Le kit de développement suffit : le proxy SDL n'est pas nécessaire pour ce tutoriel.
Si vous l'avez déjà installé avec le Hub, vous pouvez le laisser en place.

## 1. Préparer les outils

Il vous faut :

- Windows x64 et une version du jeu [reconnue par le SDK](README.md#ce-qui-est-expérimental).
- CMake 3.24 ou ultérieur, Ninja et un compilateur MinGW x64 compatible C++20.
- Le kit **NimbyRailsSDK-0.6.0-windows-x64-mingw.zip**, disponible dans les
  [assets de la release 0.6.0](https://github.com/NimbyRails-France/sdk/releases/tag/v0.6.0).

Le kit 0.6.0 est construit avec MinGW GCC 15.2. Le plus simple est d'utiliser la
même chaîne pour l'exemple. Pour MSVC, reconstruire le SDK avec MSVC x64 ; la
bibliothèque d'import du ZIP MinGW ne fournit pas le fichier `.lib` requis.

Extraire **tout le kit**, par exemple dans :

```text
C:/SDK/NimbyRailsSDK-0.6.0/
├── bin/       DLL à côté de votre futur programme
├── include/   Headers publics nimby/*.h
├── lib/       Bibliothèque d'import et configuration CMake
└── share/     Exemples, documentation et licences
```

Si le SDK est installé par le Hub, son dossier SDK contient aussi le kit.
Utiliser le dossier qui contient directement `include`, `lib` et `bin`,
et non son sous-dossier `loader`.

## 2. Créer le projet

Créer `C:/Dev/MyFirstNimbyTool` et y copier les deux fichiers de
`examples/first-observer` :

- [`main.cpp`](https://github.com/NimbyRails-France/sdk/blob/main/examples/first-observer/main.cpp)
- [`CMakeLists.txt`](https://github.com/NimbyRails-France/sdk/blob/main/examples/first-observer/CMakeLists.txt)

**Dans une copie du dépôt**, ces fichiers sont directement sous `examples/first-observer`.
**Dans un nouveau paquet**, ils sont sous `share/NimbyRailsSDK/examples/first-observer`.
Si votre ZIP 0.6.0 ne contient que `observer`, récupérer ces deux fichiers dans les
sources ; il n'est pas nécessaire de remplacer sa DLL 0.6.0.

Vous devez obtenir :

```text
C:/Dev/MyFirstNimbyTool/
├── CMakeLists.txt
└── main.cpp
```

Le fichier CMake trouve le kit avec `find_package(NimbyRailsSDK 0.6 CONFIG REQUIRED)`,
lie `NimbyRailsSDK::SDK` et copie les DLL du kit à côté de l'exécutable après compilation.
Il ne compile pas le SDK depuis ses sources.

## 3. Compiler

### Avec CLion

1. Ouvrir `C:/Dev/MyFirstNimbyTool` comme un projet.
2. Sélectionner une toolchain **MinGW x64** et le générateur **Ninja**.
3. Dans les options CMake du profil, ajouter :

   ```text
   -DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.6.0
   ```

4. Recharger CMake et compiler la cible **MyFirstNimbyTool**.
5. Dans sa configuration d'exécution, mettre `--check-sdk` dans les arguments.

Le chemin de sortie dépend du profil CLion ; la suite utilise `build/` pour
les commandes de terminal. Dans CLion, lancer la cible du profil choisi.

### Avec PowerShell

Depuis `C:/Dev/MyFirstNimbyTool`, avec CMake, Ninja et MinGW dans `PATH` :

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.6.0
cmake --build build
./build/MyFirstNimbyTool.exe --check-sdk
```

Pour les outils CLion à l'emplacement par défaut, vous pouvez préparer le `PATH`
de **ce terminal** avant ces commandes :

```powershell
$clionTools = "$env:LOCALAPPDATA/Programs/CLion/bin"
$env:PATH = "$clionTools/cmake/win/x64/bin;$clionTools/ninja/win/x64;$clionTools/mingw/bin;$env:PATH"
```

Adapter ce chemin si CLion se trouve ailleurs. Si vous changez de compilateur,
utiliser un nouveau dossier de compilation, par exemple `build-mingw`.

Avec le kit 0.6.0, résultat attendu du contrôle sans jeu :

```text
SDK 0.6.0 | ABI 1
```

Ce contrôle vérifie que l'exécutable charge une DLL compatible et peut appeler
`GetVersion`. Il ne vérifie pas encore l'accès à une partie.

## 4. Trouver le processus du jeu

Lancer NIMBY Rails, ouvrir une partie contenant au moins un train, puis exécuter :

```powershell
Get-Process -Name NIMBYRails | Select-Object Id, ProcessName, Path
```

`Id` est le **PID**, le numéro Windows du processus. Il peut changer à chaque
redémarrage. Copier le numéro de l'instance à observer.

Par exemple, si la colonne `Id` indique `12345` :

```powershell
./build/MyFirstNimbyTool.exe 12345
```

Dans CLion, remplacer `--check-sdk` par ce numéro dans les arguments d'exécution.
Ne pas utiliser `$PID` pour stocker le numéro du jeu : PowerShell réserve cette
variable à son propre processus.

## 5. Lire le résultat

Exemple **illustratif** de sortie ; les noms, IDs, nombres et vitesses dépendent
de votre partie :

```text
SDK 0.6.0 | ABI 1

Snapshot 1 | pid=12345 | captured_unix_ms=1800000000000 | 2 trains
Train TER 01 | id=1001 | speed=72.0 km/h | track=2001 | position=25.0% | direction=1 | track station=Lyon
Train TER 02 | id=1002 | speed=unavailable | position=unavailable
```

- `speed=unavailable` signifie que la vitesse n'est pas connue. Ce n'est pas `0 km/h`.
- `position=25.0%` est une fraction de la voie, pas une latitude/longitude.
- `direction=1` ou `-1` suit l'orientation interne de la voie, pas le nord ou le sud.
- `track station` est la gare associée à la voie. Ce n'est ni la destination ni
  la preuve que le train dessert cette gare ou y est arrêté.
- Une partie sans trains peut donner une capture valide avec `0 trains`.

Chaque capture affiche de nouveau ses trains. Le programme ne conserve pas une
ancienne position pour remplacer une donnée devenue indisponible.

## 6. Comprendre le code

### Vérifier la DLL

`checkVersion()` remplit `NimbySdkVersion.struct_size`, appelle
`NimbySdk_GetVersion`, puis vérifie SDK 0.6.x et ABI 1. La version du paquet
et celle de l'ABI, c'est-à-dire le contrat binaire des fonctions, sont distinctes.

### Ouvrir une session

```cpp
Session session;
check(NimbySdk_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,
                          gamePid, &session.value), "OpenProcess");
```

Une **session** représente l'accès en lecture à ce processus. L'ouverture vérifie
son exécutable. Aucun appel à `Initialize` n'est nécessaire pour l'observation externe.

### Capturer un instantané

`NimbySdk_CaptureSnapshot` crée une copie des données, appelée **snapshot**.
Cette copie ne change plus. En revanche, le jeu peut évoluer pendant sa collecte :
ce n'est pas la garantie d'un unique instant de simulation.

Si la capture renvoie `NIMBY_DATA_UNAVAILABLE`, l'exemple attend la tentative
suivante. Toute autre erreur est affichée avec le nom de l'opération concernée.

### Lire les tableaux en deux appels

`readRecords` demande d'abord le nombre d'éléments avec un buffer nul, alloue un
`std::vector`, puis y copie les éléments. La capacité est un nombre de structures,
pas un nombre d'octets. Le snapshot reste identique entre les deux appels.

### Vérifier les indicateurs de validité

Le champ `flags` contient des bits : chaque bit indique une propriété disponible.

```cpp
if (train.flags & NIMBY_TRAIN_PRESENT) {
    std::printf("%.1f km/h\n", train.speed_mps * 3.6);
}
```

La vitesse est fournie en mètres par seconde. Pour la position, tester séparément
`NIMBY_TRAIN_POSITION_VALID` avant de lire `track_id`, `track_fraction` et `direction`.

### Relier les objets

L'exemple cherche `train.track_id` parmi les `NimbyTrack.id`, puis
`track.station_id` parmi les `NimbyStation.id`. Les trois listes viennent du
**même snapshot**. Il gère aussi une référence absente ou un nom automatique
de gare non résolu.

### Libérer les ressources

Les petits objets `Session` et `Snapshot` ferment leurs handles dans leur
destructeur, même en cas d'erreur : c'est le principe **RAII** de C++.
Ils ne sont pas copiables, pour éviter de libérer deux fois la même ressource.
Le snapshot est libéré à chaque tour et la session à la fin du programme.

## 7. Aller plus loin

- Pour une interface graphique : lire le [guide d'intégration](developing.md).
- Pour les signaux, réservations et occupations : ouvrir `examples/observer`
  et consulter la [référence](api-reference.md).
- Pour distribuer votre programme : suivre les [règles de distribution](developing.md#distribuer-votre-programme).
- Si le résultat diffère : suivre le [dépannage](troubleshooting.md).

Codes de sortie du premier exemple : `0` = contrôle SDK réussi ou au moins une
capture réussie ; `1` = erreur SDK ou C++ ; `2` = arguments invalides ;
`3` = cinq tentatives sans capture disponible.
