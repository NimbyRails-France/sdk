# Développer avec NimbyRailsSDK 0.5

## Réservations et occupation (nouveau en 0.5)

Les [états des signaux](signal-states.md) disposent également d'une API dédiée,
`NimbySdk_CopySignalStates` : aspect général, identifiant spécifique et index
de texture avec validité indépendante. Les sélecteurs natifs sont lus ; leur
correspondance avec un aspect ferroviaire général reste inconnue.

`NimbySdk_CopyTrackReservations` et `NimbySdk_CopyTrackOccupations` utilisent le
même protocole buffer/count que `CopyTrains`, avec des éléments `NimbyTrackUsage` :
`train_id`, `track_id`, `fraction_begin`, `fraction_end`. Filtrer par `train_id`
pour montrer les portions réservées d'un train sélectionné ; relier `track_id`
aux voies du même snapshot. Les bornes sont normalisées, finies, dans `[0,1]`.
Les listes ne sont pas ordonnées et peuvent contenir des intervalles qui se recouvrent.

Chaque composant est disponible indépendamment : `NIMBY_DATA_UNAVAILABLE` met
`required` à zéro et signifie **inconnu**, même si le snapshot contient des trains.
`NIMBY_OK` avec zéro entrée signifie **collection observée vide**. Remplacer toute
la liste à chaque nouveau snapshot, la vider sur indisponibilité/changement de
sélection, et expirer l'affichage si les captures cessent. Ne jamais conserver une
ancienne réservation comme état courant. Un snapshot existant reste immuable.

L'ajout conserve les structures et fonctions ABI v1 existantes. Un client lié aux
nouveaux exports nécessite la DLL 0.5 ou ultérieure : ne pas associer les nouveaux
headers à la DLL 0.4. Les réservations virtuelles de scripts et l'ordre d'itinéraire
ne sont pas exposés. [Recherche, preuves, limites](research/reservations.md).

Ajouts expérimentaux 0.3 (ABI v1 existante conservée) :
`NimbySdk_CopyTrackNodes` copie un graphe principal partiel avec coordonnées natives ;
`NimbySdk_CopyTrainPathTracks` copie les IDs de voies du Path d'un train dans un
snapshot. Même protocole buffer/count que les autres fonctions Copy ; Path absent
ou instable : `NIMBY_DATA_UNAVAILABLE`. Aucun pointeur interne exposé. Le Path ne
prouve ni réservation ni occupation ; les connexions spéciales restent incomplètes.
Voir [preuves et contrat détaillé](research/train-paths.md).

NimbyRailsSDK est une DLL Windows x64 avec une API C versionnée, utilisable depuis
un programme C ou C++. C++20 est utilisé pour construire le SDK et l'exemple.
La version 0.2 expose une **API d'observation en lecture seule** : trains, vitesses,
voies, gares et signaux. Les adaptateurs de mémoire restent expérimentaux et
acceptent uniquement les empreintes de jeu documentées dans la recherche.

## Créer le paquet

Depuis le dépôt, fermer le moniteur s'il utilise le build Release puis exécuter :

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/package.ps1
```

Le script compile Release, exécute les tests, installe dans
`dist/NimbyRailsSDK-0.5.0/`, puis copie et compile l'exemple **depuis le paquet
installé** dans `build/sdk-consumer/`. Aucune injection ni installation dans le jeu.
`-SkipBuild` permet de réinstaller un build Release déjà vérifié.

Le paquet contient :

| Dossier | Rôle |
|---|---|
| `include/nimby/` | Headers publics uniquement : `sdk.h`, `observation.h` |
| `lib/` | Bibliothèque d'import correspondant au compilateur choisi |
| `lib/cmake/NimbyRailsSDK/` | Configuration pour `find_package` |
| `bin/` | DLL du SDK, dépendances du runtime et ancien chargeur de diagnostic |
| `share/NimbyRailsSDK/examples/observer/` | Projet CMake autonome |
| `share/doc/NimbyRailsSDK/` | Ce guide |
| `share/licenses/MinHook/` | Licence de MinHook |
| `share/licenses/MinGW/` | Notices du compilateur et des runtimes fournis sur ce poste |

Le paquet peut être déplacé : les cibles CMake utilisent son emplacement courant.
Construire un paquet avec le même type de chaîne que le consommateur : MinGW x64
sur ce poste ; pour MSVC, reconstruire le SDK avec MSVC x64 pour obtenir sa `.lib`.
La compatibilité binaire entre ces chaînes n'a pas été testée.

## Dans un autre projet CMake / CLion

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

Dans les options CMake de CLion, ajouter :

```text
-DCMAKE_PREFIX_PATH=C:/chemin/vers/NimbyRailsSDK-0.5.0
```

Ou en terminal avec le compilateur configuré dans PATH :

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:/chemin/vers/NimbyRailsSDK-0.5.0
cmake --build build
```

Inclure `<nimby/observation.h>` et utiliser les fonctions `NimbySdk_*`.
Ne pas copier `src/`, `include/engine/` ou MinHook dans le projet consommateur.
Le DLL du SDK construit ici intègre GCC/libstdc++ statiquement, mais dépend encore
de `libwinpthread-1.dll`. Celle-ci est fournie dans `bin/` ; la variable CMake
`NimbyRailsSDK_RUNTIME_FILES` permet de copier les DLL du paquet près du programme.
Le consommateur peut également nécessiter les runtimes de son propre compilateur.

## Exemple minimal

```cpp
#include <nimby/observation.h>
#include <cstdio>
#include <vector>

int observe(uint32_t gamePid) {
    NimbySession session = 0;
    auto status = NimbySdk_OpenProcess(NIMBY_OBSERVATION_ABI_VERSION,
                                      gamePid, &session);
    if (status != NIMBY_OK) return static_cast<int>(status);

    NimbySnapshot snapshot = 0;
    status = NimbySdk_CaptureSnapshot(session, &snapshot);
    NimbySdk_CloseSession(session);
    if (status != NIMBY_OK) return static_cast<int>(status);

    uint32_t count = 0;
    status = NimbySdk_CopyTrains(snapshot, nullptr, 0, &count);
    if (status == NIMBY_OK) {
        std::vector<NimbyTrain> trains(count);
        status = NimbySdk_CopyTrains(snapshot, trains.data(), count, &count);
        if (status == NIMBY_OK) for (const auto& train : trains) {
            if (train.flags & NIMBY_TRAIN_PRESENT)
                std::printf("%s : %.1f km/h\n", train.name_utf8,
                            train.speed_mps * 3.6);
        }
    }
    NimbySdk_ReleaseSnapshot(snapshot);
    return static_cast<int>(status);
}
```

Pour un projet réel, utiliser des gardes RAII pour garantir la fermeture même
si une allocation du consommateur échoue : voir l'exemple `observer/main.cpp`.
Lancer cet exemple avec le PID du jeu et une partie chargée. Il observe cinq fois
à 250 ms d'intervalle puis termine, sans modifier le jeu.

## Contrat de l'API

- `OpenProcess` vérifie le SHA-256 de l'exécutable et ouvre seulement un accès de
  lecture. PID 0 désigne le processus appelant, pour un futur module chargé dans
  le jeu. Le programme externe n'a besoin d'aucun proxy ni injecteur.
- `CaptureSnapshot` retrouve les racines et les IDs complets à chaque appel.
  Un snapshot est une copie immuable détenue par le SDK. Il reste consultable
  après fermeture de sa session ou arrêt du jeu jusqu'à `ReleaseSnapshot`.
- Les handles sont des entiers opaques, jamais des adresses. Ne pas les sérialiser,
  les deviner ou les réutiliser après libération/rechargement de la DLL.
- `Copy*` avec buffer nul et capacité 0 donne le nombre d'éléments. Un buffer trop
  petit reçoit `NIMBY_BUFFER_TOO_SMALL`, la taille requise et **aucune copie partielle**.
  Les buffers fournis doivent être valides et assez grands. Les champs réservés
  sont à ignorer. Les tableaux et chaînes copiés appartiennent ensuite au client.
- Chaînes UTF-8 terminées par zéro ; ID zéro signifie absence de référence.
  Une chaîne de gare vide indique un nom automatique encore non résolu.
- Vitesses en m/s, conversion km/h par multiplication par 3,6. Lire les flags du
  train avant sa vitesse ou sa position : une valeur indisponible n'est pas un arrêt.
  Position expérimentale dans [0,1], sens +1/-1 selon l'orientation du segment,
  pas une direction géographique. La limite de voie n'est pas la consigne du train.
- Relier `train.track_id` à `track.id`, `track.station_id` à `station.id` et
  `signal.track_id` à `track.id`. Une balise est un signal de type
  `NIMBY_SIGNAL_BALISE`. Les quais, couleurs des signaux et prochain signal sur
  l'itinéraire ne sont pas encore exposés.
- `GetSnapshotInfo` exige `struct_size = sizeof(NimbySnapshotInfo)` ; il fournit
  empreinte, PID, heure UTC de fin de capture et compteurs. Les flags
  `EXPERIMENTAL | NON_ATOMIC` sont toujours présents dans cette version.
- Les appels sont sérialisés et peuvent provenir de plusieurs threads. Les buffers
  du consommateur doivent être protégés par lui. Une capture peut être coûteuse :
  utiliser un thread de travail, typiquement 4 Hz, jamais DllMain ni un callback TLS.
- Limites actuelles : 8 sessions, 16 snapshots simultanés ; libérer régulièrement.
  `CloseSession` n'annule pas les snapshots déjà produits. Les fonctions de
  diagnostic `Initialize/Shutdown` sont indépendantes et ne gèrent pas ces handles.
- Avant `FreeLibrary`, arrêter et joindre les appelants, libérer tous les snapshots
  et fermer toutes les sessions ; appeler aussi `Shutdown` si les diagnostics ont
  été initialisés. Aucun pointeur C++ ni exception ne traverse l'ABI.

### Erreurs et redémarrage

| Statut | Réaction attendue |
|---|---|
| `NIMBY_UNSUPPORTED_GAME` | Binaire inconnu : refuser l'observation, attendre un profil validé |
| `NIMBY_DATA_UNAVAILABLE` | Menu, chargement ou données modifiées : réessayer plus tard |
| `NIMBY_PROCESS_EXITED` | Fermer la session et en ouvrir une avec le nouveau PID |
| `NIMBY_INVALID_HANDLE` | Handle nul, fermé ou du mauvais type : corriger le cycle de vie |
| `NIMBY_RESOURCE_LIMIT` | Fermer/libérer les ressources conservées |
| `NIMBY_IO_ERROR` | Processus/fichier inaccessible : vérifier PID et droits |
| `NIMBY_INVALID_ARGUMENT` | Mauvaise version ABI, taille de structure ou argument |
| `NIMBY_INTERNAL_ERROR` | Échec interne/allocation ; aucun résultat n'est publié |

`NimbySdk_StatusString` renvoie un texte statique valable jusqu'au déchargement
de la DLL, à ne pas libérer. Le SDK ne relance ni ne reconnecte automatiquement
le jeu. Les IDs d'objets sont à interpréter dans leur session/partie, jamais comme
une identité globale entre sauvegardes.

## Évolution et limites

API publique v1 et version du paquet 0.5.0 sont deux notions distinctes. Les
structures de cette ABI sont figées ; une modification incompatible nécessitera
une nouvelle version d'API. La compatibilité CMake est limitée à la même version
mineure pendant la phase 0.x. Les offsets du moteur restent dans `engine/` et
peuvent évoluer par profil sans imposer leur connaissance aux clients.

Le SDK vérifie l'empreinte sur disque à l'ouverture ; il ne vérifie pas chaque
page de l'image déjà chargée contre ce fichier. Les instantanés externes sont
optimistes : la simulation peut évoluer entre deux lectures malgré les gardes.
Ils conviennent à l'observation expérimentale, pas à des décisions exigeant un
tick atomique. Les tests synthétiques ne constituent pas une validation de thread
ou de cible de hook. Aucun setter de vitesse ni d'élément réseau n'est publié.

Le moniteur historique garde son bouton expérimental séparé. La DLL installée
dans le jeu n'est pas remplacée par la création du paquet ; pour utiliser le
nouveau SDK dans un programme externe, les fichiers du paquet suffisent.

Tests : API C, rejets ABI/handles, buffers, résolution de mémoire simulée,
cycle de vie DLL, MinHook autonome et proxy. Le test optionnel
`nimby_observation_tests.exe <PID>` valide également un snapshot réel en lecture
seule et sa durée de vie après fermeture de session.
