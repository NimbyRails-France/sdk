# NimbyRailsSDK 0.6.0

SDK natif C++20 / Windows x64 pour observer NIMBY Rails depuis un autre programme.
API C versionnée, snapshots de trains, vitesses, voies, gares, signaux, Paths,
**portions réservées et occupation native**.
MinHook est compilé dans la DLL ; aucun fichier MinHook.dll à installer.

La version 0.5 ajoute `NimbySdk_CopyTrackReservations` et
`NimbySdk_CopyTrackOccupations` : ID train, ID voie et bornes normalisées sur la voie.
Les listes sont actualisées à chaque capture ; `DATA_UNAVAILABLE` signifie inconnu,
alors que `OK` avec zéro entrée signifie une collection observée vide.
Ces portions ne forment pas un itinéraire ordonné et n'incluent pas les réservations
virtuelles des scripts. [Preuves et limites](docs/research/reservations.md).

## Installer les DLL dans le jeu

Pour le chargement automatique à chaque démarrage, télécharger
**[NimbyRailsSDK-0.6.0-drop-in-windows-x64.zip](https://github.com/NimbyRails-France/sdk/releases/tag/v0.6.0)**.
Il contient **SDL3.dll + NimbyRailsSDK.dll**, ainsi que **libwinpthread-1.dll** obligatoire.
Jeu fermé, extraire le ZIP puis lancer son `install-proxy.ps1` : la SDL originale
est conservée sous `NimbyRailsSDL3Original.dll`. Ne pas écraser directement la SDL du jeu.

**[Installation, copie manuelle et désinstallation](docs/install-drop-in.md).**
Le paquet `windows-x64-mingw.zip` décrit ci-dessous est le kit de développement ;
le paquet `drop-in-windows-x64.zip` est celui à utiliser pour le dossier du jeu.

## Installer depuis une release

1. Ouvrir les [releases du SDK](https://github.com/NimbyRails-France/sdk/releases).
2. Télécharger **NimbyRailsSDK-0.6.0-windows-x64-mingw.zip** dans **Assets** (pas « Source code »).
3. Extraire tout le ZIP, par exemple sous `C:/SDK/`. Le dossier obtenu est
   `C:/SDK/NimbyRailsSDK-0.6.0/`, contenant `include`, `lib`, `bin` et `share`.
4. Dans CLion, sélectionner une toolchain **MinGW x64**, puis ajouter aux options CMake :
   `-DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.6.0`.
5. Lier votre cible à `NimbyRailsSDK::SDK` et copier les DLL de `bin` près de votre `.exe`.

Le paquet est compilé avec MinGW GCC 15.2. Pour MSVC, reconstruire depuis les sources
avec MSVC x64 ; le ZIP MinGW ne fournit pas de bibliothèque d'import `.lib` MSVC.
Pour utiliser simplement le TCO, télécharger sa [release prête à lancer](https://github.com/NimbyRails-France/tco/releases) : elle inclut déjà le runtime SDK.

### Exemple de projet CMake

```cmake
cmake_minimum_required(VERSION 3.24)
project(MyNimbyTool LANGUAGES CXX)
find_package(NimbyRailsSDK 0.5 CONFIG REQUIRED)
add_executable(MyNimbyTool main.cpp)
target_compile_features(MyNimbyTool PRIVATE cxx_std_20)
target_link_libraries(MyNimbyTool PRIVATE NimbyRailsSDK::SDK)
add_custom_command(TARGET MyNimbyTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${NimbyRailsSDK_RUNTIME_FILES} "$<TARGET_FILE_DIR:MyNimbyTool>"
    COMMAND_EXPAND_LISTS)
```

Un programme complet est fourni dans `share/NimbyRailsSDK/examples/observer` du ZIP.
Copier ce dossier dans votre espace de travail, puis :

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:/SDK/NimbyRailsSDK-0.6.0
cmake --build build
./build/MyNimbyObserver.exe <PID-du-jeu>
```

CMake, Ninja et le compilateur MinGW doivent être dans `PATH`.
Inclure `<nimby/observation.h>` ; suivre le [guide développeur](docs/developing.md)
pour ouvrir une session, capturer un snapshot et libérer les ressources.
Dans le ZIP, ce guide est sous `share/doc/NimbyRailsSDK/developing.md`.

### Compatibilité et limites

Cette release est expérimentale et son API publique est en lecture seule.
Elle reconnaît uniquement le binaire SHA-256
`FFF49AC21720ABFC824C2B4F68B862727630EB0DB71CFE1F9EA8F685D0DB10AE`.
Un autre binaire est refusé. Une position absente reste indisponible.
Les Paths ne prouvent pas une réservation ; aiguilles complètes et aspects des
signaux restent à valider. L'observation externe ne nécessite ni proxy, ni injection,
ni copie dans le dossier du jeu. Les utilitaires historiques de chargement sont
séparés de cette API ; les hooks internes du jeu restent désactivés.

Pour contrôler le téléchargement : `Get-FileHash chemin/du/ZIP -Algorithm SHA256`
et comparer avec `SHA256SUMS.txt` joint à la release.

## Où travailler ?

```text
NimbyRailsSDK/
├── src/                    Code interne du SDK
│   ├── main.cpp            Entrée Windows de la DLL : DllMain minimal
│   ├── train_monitor.cpp   Moniteur externe : trains, vitesses et réseau
│   ├── runtime/            Initialisation, arrêt et journalisation
│   ├── hooks/              Backend MinHook et règles d'activation
│   ├── loader/             Chargeur automatique (exécutable séparé)
│   ├── proxy/              SDL3.dll : chargement du SDK au démarrage du jeu
│   └── engine/             Identification, résolution mémoire, lecture du réseau
├── include/               Tous nos headers (.h)
│   ├── nimby/              API publique : sdk.h et observation.h
│   ├── hooks/             Headers internes des hooks
│   ├── loader/            Déclarations internes du chargeur
│   ├── runtime/           Protocole d'initialisation à distance
│   └── engine/            Headers internes d'adaptation au jeu
├── tests/                  Programmes autonomes de vérification
├── tools/                  Script de compilation et inspecteur PE
├── cmake/                  Configuration du paquet pour find_package
├── examples/observer/      Projet consommateur autonome
├── docs/                   Guide d'utilisation et recherche conservée
├── third_party/            Dépendances externes : MinHook et sa licence
├── build/                  Résultats générés : Debug/ et Release/
└── CMakeLists.txt          Définition de la compilation
```

Pour commencer : [src/main.cpp](src/main.cpp) est l'entrée de la DLL ;
[src/runtime/runtime.cpp](src/runtime/runtime.cpp) contient son véritable cycle de vie.
L'API de lecture est dans [include/nimby/observation.h](include/nimby/observation.h),
les diagnostics et le cycle de vie historique dans [include/nimby/sdk.h](include/nimby/sdk.h).
Tous nos `.h` sont dans `include/`, et nos `.cpp` dans `src/` (ou `tests/`).
Seul `include/nimby/` fait partie de l'API publique distribuée aux mods.
MinHook reste isolé dans `third_party/minhook/` : uniquement les sources `.c` et
`.h` nécessaires à Windows x64 et la licence. CMake les intègre statiquement au SDK :
aucune DLL MinHook séparée à distribuer.

## Compiler et tester

Depuis la racine du projet, avec les outils fournis par CLion :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/build.ps1
```

Ajouter `-Configuration Release` pour Release, ou `-ClionHome 'chemin/CLion'`
si CLion est installé ailleurs. La DLL est produite dans `build/Debug/NimbyRailsSDK.dll`,
ainsi que `NimbyRailsLoader.exe` et le proxy sous `drop-in/`.
Le script lance neuf tests, plus le test du proxy lorsqu'une SDL originale est
fournie à CMake via `NIMBY_SDL_ORIGINAL` (dix tests sur ce poste).
Avec MinGW, le SDK utilise aussi `libwinpthread-1.dll`, fournie avec le paquet ;
le chargeur et les tests conservent leurs runtimes MinGW près de leurs exécutables.

## Charger le SDK dans le jeu

Pour le chargement dans le jeu, le proxy SDL est construit dans
`build/Release/drop-in/` : `SDL3.dll`, `NimbyRailsSDK.dll` et, avec MinGW,
`libwinpthread-1.dll`. Le proxy n'est pas nécessaire à l'API de lecture externe.
**Ne pas écraser directement la SDL d'origine** : l'installateur la conserve intacte
sous le nom `NimbyRailsSDL3Original.dll` puis dépose le SDK et ses dépendances.

Jeu fermé, depuis la racine du projet :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/install-proxy.ps1
```

Ensuite, lancer le jeu normalement depuis Steam. Aucun superviseur à laisser ouvert.
Le SDK démarre après SDL_Init, uniquement pour les versions reconnues du jeu et de
SDL ; ses hooks du jeu restent désactivés. Journal : `%LOCALAPPDATA%/NimbyRailsSDK/proxy.log`.

Pour désinstaller et restaurer la SDL originale, jeu fermé :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/install-proxy.ps1 -Action Remove
```

L'ancien chargeur dans `src/loader/` est conservé pour les tests et le diagnostic,
mais il ne doit pas fonctionner en même temps que le proxy. Voir [le guide](docs/usage.md).

Dans CLion : recharger le projet CMake et sélectionner la cible `NimbyRailsSDK`.
Pour observer les trains : ouvrir une partie, puis lancer
`build/Release/NimbyTrainMonitor.exe`. La fenêtre affiche à 4 Hz les noms, vitesses,
voies, positions, sens, limites de voie, gares associées et signaux du même segment.
Ces nouvelles données réseau sont expérimentales et en lecture seule :
[preuves, redémarrage réel et limites](docs/research/network.md).
Les colonnes **Copie / Simulation** sont retrouvées par une chaîne depuis
le module, vérifiée après un redémarrage réel. Voir [les preuves et limites](docs/research/stable-resolution.md).
Le bouton **Appliquer une fois** teste une vitesse sur le train choisi dans **Simulation**.
Il effectue une écriture ponctuelle, avec relecture ; le moteur peut recalculer la valeur.
Tester sur une partie jetable ; la synchronisation avec le moteur reste à valider.
Le dossier `cmake-build-debug/`, si présent, est le cache généré par le profil CLion.
`.idea/` contient uniquement les réglages de l'IDE.
Les anciens builds sont conservés dans `build/legacy/` comme archives ; leurs
caches CMake contiennent les anciens chemins et ne doivent pas être réutilisés.

Le [guide d'utilisation](docs/usage.md) détaille l'API et le cycle de vie.
Les preuves d'analyse sont dans [docs/research/](docs/research/first-milestone.md),
et la validation de MinHook dans [ce rapport](docs/research/minhook-integration.md).
